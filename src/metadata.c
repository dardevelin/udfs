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
    int ok = sqlite3_open_v2("metadata.db", &g_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);
    if (ok != SQLITE_OK) {
        fprintf(stderr, "%s", sqlite3_errmsg(g_db));
        exit(EXIT_FAILURE);
    }
}
