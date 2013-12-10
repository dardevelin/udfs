#include "hook_getattr.h"

int hook_getattr(const char *path, struct stat *stbuf)
{
    int ok, res = 0, type, mode, size;

    char *basename = strrchr(path, '/') + 1;
    char  dirname[basename-path+1];
    strncpy(dirname, path, basename-path);
    dirname[basename-path] = 0;

    // Strip the trailing slash if there's one; but don't if the string is "/"
    if (basename-path-1 > 0) {
        dirname[basename-path-1] = 0;
    }

    printf("Getting attributes for path %s\n", path);
    memset(stbuf, 0, sizeof(struct stat));

    /* Query type and size of a file */
    ok = sqlite3_prepare(g_db, "SELECT type, mode, size FROM files WHERE path=? AND name=?", 1000, &g_stmt, NULL);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    ok = sqlite3_bind_text(g_stmt, 1, dirname, -1, SQLITE_STATIC);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));
    
    ok = sqlite3_bind_text(g_stmt, 2, basename, -1, SQLITE_STATIC);
    if (ok != SQLITE_OK) 
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(g_db));

    if (sqlite3_step(g_stmt) == SQLITE_ROW) {
        type = sqlite3_column_int(g_stmt, 0);
        mode = sqlite3_column_int(g_stmt, 1);
        size = sqlite3_column_int(g_stmt, 2);

        switch (type) {
            case UDFS_TYPE_DIR:
                stbuf->st_mode = S_IFDIR;
                stbuf->st_nlink = 2;
                break;
            default:
            case UDFS_TYPE_FILE:
                stbuf->st_mode = S_IFREG;
                stbuf->st_nlink = 1;
                break;
        }
        
        stbuf->st_mode |= mode;
        stbuf->st_size = size;
    } else {
        printf("> problem\n");
        sqlite3_reset(g_stmt);
        return -ENOENT;
    }

    sqlite3_reset(g_stmt);
    return res;
}
