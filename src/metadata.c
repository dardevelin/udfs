#include "metadata.h"

struct udfs_metadata udfs_metadata_query(const char *path)
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
    struct udfs_metadata metadata = {0};
    
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
        metadata.id    = sqlite3_column_int(g_stmt, 0);
        metadata.type  = sqlite3_column_int(g_stmt, 1);
        metadata.mode  = sqlite3_column_int(g_stmt, 2);
        metadata.size  = sqlite3_column_int(g_stmt, 3);
        metadata.found = true;
    } else {
        metadata.found = false;
    }

    sqlite3_reset(g_stmt);
    return metadata;
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
