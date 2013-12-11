#include "metadata.h"

void udfs_metadata_file_filler(const char *dirname, void *buf, fuse_fill_dir_t filler)
{
    int ok;

    /* Get file listing for dirname */
    ok = sqlite3_prepare(g_db, "SELECT basename FROM files WHERE dirname=?", UDFS_SIZE_ZSQL, &g_stmt, NULL);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    ok = sqlite3_bind_text(g_stmt, 1, dirname, -1, SQLITE_STATIC);
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
    
    /* Query sqlite database for file metadata */
    ok = sqlite3_prepare(g_db, "DELETE FROM files WHERE dirname=? AND basename=?", UDFS_SIZE_ZSQL, &g_stmt, NULL);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    ok = sqlite3_bind_text(g_stmt, 1, dirname, -1, SQLITE_STATIC);
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
    
    /* Query sqlite database for file metadata */
    ok = sqlite3_prepare(g_db, "INSERT INTO files (dirname, basename, type, mode, size) VALUES (?, ?, ?, ?, ?)", UDFS_SIZE_ZSQL, &g_stmt, NULL);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    ok = sqlite3_bind_text(g_stmt, 1, dirname, -1, SQLITE_STATIC);
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

struct udfs_file udfs_metadata_file_query(const char *path)
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

    /* Prepare metadata object */
    struct udfs_file file = {0};
    
    /* Query sqlite database for file metadata */
    ok = sqlite3_prepare(g_db, "SELECT id, type, mode, size FROM files WHERE dirname=? AND basename=?", UDFS_SIZE_ZSQL, &g_stmt, NULL);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    ok = sqlite3_bind_text(g_stmt, 1, dirname, -1, SQLITE_STATIC);
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
                "'dirname' TEXT NOT NULL,"
                "'basename' TEXT NOT NULL,"
                "'type' INTEGER NOT NULL,"
                "'mode' INTEGER NOT NULL,"
                "'size' INTEGER NOT NULL DEFAULT (0)"
                ")";
    ok = sqlite3_exec(g_db, sql, NULL, NULL, NULL);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    /* Insert the root folder to `files` if missing */
    sql = "INSERT INTO files (dirname, basename, type, mode, size) "
          "SELECT '/', '', 1, 493, 0 "
          "WHERE NOT EXISTS(SELECT 1 FROM files WHERE dirname = '/' AND basename = '')";
    ok = sqlite3_exec(g_db, sql, NULL, NULL, NULL);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));
}
