#include "hook_read.h"

int hook_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    (void) fi;

    printf("Reading file: %s\n", path);

    /* Get file id from name */
    int ok, file_id;
    char *filename = strrchr(path, '/') + 1;
    
    ok = sqlite3_prepare(g_db, "SELECT id FROM files WHERE path=? AND name=?", 200, &g_stmt, NULL);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    ok = sqlite3_bind_text(g_stmt, 1, path, filename-path, SQLITE_STATIC);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    ok = sqlite3_bind_text(g_stmt, 2, filename, -1, SQLITE_STATIC);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    if (sqlite3_step(g_stmt) == SQLITE_ROW) {
        file_id = sqlite3_column_int(g_stmt, 0);
        printf("File id: %u\n", file_id);
    } else {
        fprintf(stderr, "Error: no existing metadata for this file\n");
        return -ENOENT;
    }

    sqlite3_reset(g_stmt);
    return size;
}
