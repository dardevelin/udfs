#include "metadata.h"

int udfs_metadata_file_walk(const char *dirname)
{
    int ok, parent_id = 1, segment_length = 0;
    const char *end = dirname + strlen(dirname);
    char *pos;

    /* Do not even query the database for "/", it's always 1 */
    if (strcmp(dirname, "/") == 0)
        return 1;

    dirname++;
    while (1) {
        pos = strchr(dirname, '/');
        if (pos) {
            segment_length = pos - dirname;
        } else {
            segment_length = end - dirname;
        }

        ok = sqlite3_prepare(g_db, "SELECT id FROM files WHERE name=? AND parent_id=?", UDFS_SIZE_ZSQL, &g_stmt, NULL);
        if (ok != SQLITE_OK) 
            fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

        ok = sqlite3_bind_text(g_stmt, 1, dirname, segment_length, SQLITE_STATIC);
        if (ok != SQLITE_OK) 
            fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));
        
        ok = sqlite3_bind_int(g_stmt, 2, parent_id);
        if (ok != SQLITE_OK) 
            fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

        if (sqlite3_step(g_stmt) == SQLITE_ROW) {
            parent_id = sqlite3_column_int(g_stmt, 0);
            dirname += segment_length + 1;
        } else {
            return 0;
        }
    
        sqlite3_reset(g_stmt);

        /* We were doing our last loop, there's no more '/' after that */
        if (!pos) {
            break;
        }
    }

    return parent_id;
}

void udfs_metadata_file_filler(const char *dirname, void *buf, fuse_fill_dir_t filler)
{
    int ok, id;

    /* Find the current folder's id */
    id = udfs_metadata_file_walk(dirname);
    if (!id) {
        /* Let's not query the database for nothing */
        return;
    } 
    
    /* Get all files inside that folder */
    ok = sqlite3_prepare(g_db, "SELECT name FROM files WHERE parent_id=?", UDFS_SIZE_ZSQL, &g_stmt, NULL);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    ok = sqlite3_bind_int(g_stmt, 1, id);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    while (sqlite3_step(g_stmt) == SQLITE_ROW) {
        const unsigned char *name = sqlite3_column_text(g_stmt, 0);
        if (name[0]) {
            filler(buf, (char*)name, NULL, 0);
        }
    }
    
    sqlite3_reset(g_stmt);
}

int udfs_metadata_file_remove(const char *path)
{
    int ok;

    /* Generate dirname and basename */
    char *basename = strrchr(path, '/') + 1;
    char  dirname[basename-path+1];
    strncpy(dirname, path, basename-path);
    dirname[basename-path] = 0;
    if (basename-path-1 > 0) {
        dirname[basename-path-1] = 0;
    }

    /* Parent folder id */
    int parent_id = udfs_metadata_file_walk(dirname);

    //FIXME: if folder and not empty, please refuse
    
    /* Query sqlite database for file metadata */
    ok = sqlite3_prepare(g_db, "DELETE FROM files WHERE parent_id=? AND name=?", UDFS_SIZE_ZSQL, &g_stmt, NULL);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    ok = sqlite3_bind_int(g_stmt, 1, parent_id);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));
    
    ok = sqlite3_bind_text(g_stmt, 2, basename, -1, SQLITE_STATIC);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    sqlite3_step(g_stmt);
    
    sqlite3_reset(g_stmt);
    return 0;
}

int udfs_metadata_file_add(const char *path, enum utfs_type type, int mode, int size)
{
    int ok;

    /* Just make sure the file doesn't already exists */
    //FIXME: we could use the trick I had in _init to put everything in one sql query and get rid of this code
    struct udfs_file file = udfs_metadata_file_query(path);
    if (file.found) {
        fprintf(stderr, "Error: file already exists\n");
        return -EEXIST;
    }

    /* Generate dirname and basename */
    char *basename = strrchr(path, '/') + 1;
    char  dirname[basename-path+1];
    strncpy(dirname, path, basename-path);
    dirname[basename-path] = 0;
    if (basename-path-1 > 0) {
        dirname[basename-path-1] = 0;
    }

    /* Find parent_id */
    int parent_id = udfs_metadata_file_walk(dirname);

    /* Query sqlite database for file metadata */
    ok = sqlite3_prepare(g_db, "INSERT INTO files (parent_id, name, type, mode, size) VALUES (?, ?, ?, ?, ?)", UDFS_SIZE_ZSQL, &g_stmt, NULL);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    ok = sqlite3_bind_int(g_stmt, 1, parent_id);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));
    
    ok = sqlite3_bind_text(g_stmt, 2, basename, -1, SQLITE_STATIC);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));
    
    ok = sqlite3_bind_int(g_stmt, 3, type);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));
    
    ok = sqlite3_bind_int(g_stmt, 4, mode);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));
    
    ok = sqlite3_bind_int(g_stmt, 5, size);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    if (sqlite3_step(g_stmt) != SQLITE_DONE) {
        return -EEXIST;
    }

    sqlite3_reset(g_stmt);
    return 0;
}

int udfs_metadata_file_rename(const char *old, const char *new)
{
    int ok;

    /* Generate old dirname and basename */
    char *old_basename = strrchr(old, '/') + 1;
    char  old_dirname[old_basename-old+1];
    strncpy(old_dirname, old, old_basename-old);
    old_dirname[old_basename-old] = 0;
    if (old_basename-old-1 > 0) {
        old_dirname[old_basename-old-1] = 0;
    }

    /* Generate new dirname and basename */
    char *new_basename = strrchr(new, '/') + 1;
    char  new_dirname[new_basename-new+1];
    strncpy(new_dirname, new, new_basename-new);
    new_dirname[new_basename-new] = 0;
    if (new_basename-new-1 > 0) {
        new_dirname[new_basename-new-1] = 0;
    }

    /* Find folder parent id */
    int old_parent_id = udfs_metadata_file_walk(old_dirname);
    int new_parent_id = udfs_metadata_file_walk(new_dirname);

    /* Query sqlite database to rename the folder entry */
    ok = sqlite3_prepare(g_db, "UPDATE files SET name=?,parent_id=? WHERE parent_id=? AND name=?", UDFS_SIZE_ZSQL, &g_stmt, NULL);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    ok = sqlite3_bind_text(g_stmt, 1, new_basename, -1, SQLITE_STATIC);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));
    
    ok = sqlite3_bind_int(g_stmt, 2, new_parent_id);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));
    
    ok = sqlite3_bind_int(g_stmt, 3, old_parent_id);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));
    
    ok = sqlite3_bind_text(g_stmt, 4, old_basename, -1, SQLITE_STATIC);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));
    

    if (sqlite3_step(g_stmt) != SQLITE_DONE) {
        return -1;
    }
    
    sqlite3_reset(g_stmt);
    return 0;
}

struct udfs_file udfs_metadata_file_query(const char *path)
{
    int ok, id;

    /* Generate dirname and basename */
    char *basename = strrchr(path, '/') + 1;
    char  dirname[basename-path+1];
    strncpy(dirname, path, basename-path);
    dirname[basename-path] = 0;
    if (basename-path-1 > 0) {
        dirname[basename-path-1] = 0;
    }

    /* Prepare metadata object */
    struct udfs_file file = {0};

    /* Walk to get our folder's id */
    if (strcmp(path, "/") == 0) {
        /* little optimisation for the root */
        id = 0;
    } else {
        id = udfs_metadata_file_walk(dirname);
    }

    /* Find everything inside that folder */
    ok = sqlite3_prepare(g_db, "SELECT id, type, mode, size FROM files WHERE parent_id=? AND name=?", UDFS_SIZE_ZSQL, &g_stmt, NULL);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    ok = sqlite3_bind_int(g_stmt, 1, id);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));
    
    ok = sqlite3_bind_text(g_stmt, 2, basename, -1, SQLITE_STATIC);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));
    
    if (sqlite3_step(g_stmt) == SQLITE_ROW) {
        file.id    = sqlite3_column_int(g_stmt, 0);
        file.type  = sqlite3_column_int(g_stmt, 1);
        file.mode  = sqlite3_column_int(g_stmt, 2);
        file.size  = sqlite3_column_int(g_stmt, 3);
        file.found = true;
    } else {
        file.found = false;
    }

    sqlite3_reset(g_stmt);
    return file;
}

void udfs_metadata_fini(void)
{
    sqlite3_close(g_db);
}

void udfs_metadata_init(void)
{
    int ok;
    
    /* Open and create a database if it doesn't exists */
    ok = sqlite3_open_v2("metadata.db", &g_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);
    if (ok != SQLITE_OK) {
        fprintf(stderr, "%s", sqlite3_errmsg(g_db));
        exit(EXIT_FAILURE);
    }

    /* Create the `files` table if missing */
    char *sql = "CREATE TABLE IF NOT EXISTS files ("
                "'id' INTEGER PRIMARY KEY AUTOINCREMENT,"
                "'parent_id' INTEGER NOT NULL DEFAULT (0),"
                "'name' TEXT,"
                "'type' INTEGER NOT NULL,"
                "'mode' INTEGER NOT NULL,"
                "'size' INTEGER NOT NULL DEFAULT (0)"
                ")";
    ok = sqlite3_exec(g_db, sql, NULL, NULL, NULL);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    /* Insert the root folder to `files` if missing */
    /* Id has to be 1, parent_id 0, and name empty */
    //FIXME primary key on parent_id and maybe name would help
    sql = "INSERT INTO files (parent_id, name, type, mode, size) "
          "SELECT 0, '', 1, 493, 0 "
          "WHERE NOT EXISTS(SELECT 1 FROM files WHERE parent_id = 0 AND name = '')";
    ok = sqlite3_exec(g_db, sql, NULL, NULL, NULL);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));
}
