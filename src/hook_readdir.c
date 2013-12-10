#include "hook_readdir.h"

int hook_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    UNUSED(offset);
    UNUSED(fi);

    int ok, file_id;

    char *basename = strrchr(path, '/') + 1;
    char  dirname[basename-path+1];
    strncpy(dirname, path, basename-path);
    dirname[basename-path] = 0;

    printf("Listing files in path: %s\n", path);

    /* Check if the directory exists */
    /* FIXME: may not be necssary, maybe this is called only when it's assurely a directory
    if (length > 0) {
        ok = sqlite3_prepare(g_db, "SELECT id FROM files WHERE path=? AND name=? AND type=?", 1000, &g_stmt, NULL);
        if (ok != SQLITE_OK) 
            fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

        ok = sqlite3_bind_text(g_stmt, 1, path, length, SQLITE_STATIC);
        if (ok != SQLITE_OK) 
            fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));
        
        ok = sqlite3_bind_text(g_stmt, 2, filename, -1, SQLITE_STATIC);
        if (ok != SQLITE_OK) 
            fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));
        
        ok = sqlite3_bind_int(g_stmt, 3, UDFS_TYPE_DIR);
        if (ok != SQLITE_OK) 
            fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

        if (sqlite3_step(g_stmt) != SQLITE_ROW) {
            return -ENOENT;
        }
    }
    */
    
    /* Standard files */
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    /* Get files listing from path */
    ok = sqlite3_prepare(g_db, "SELECT basename FROM files WHERE dirname=?", UDFS_SIZE_ZSQL, &g_stmt, NULL);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    ok = sqlite3_bind_text(g_stmt, 1, path, -1, SQLITE_STATIC);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    while (sqlite3_step(g_stmt) == SQLITE_ROW) {
        const unsigned char *name = sqlite3_column_text(g_stmt, 0);
        if (name[0]) {
            filler(buf, (char*)name, NULL, 0);
        }
    }
    
    sqlite3_reset(g_stmt);
    return 0;
}
