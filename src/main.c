#include "main.h"

static const char *hello_path = "/hello";
sqlite3      *g_db;
sqlite3_stmt *g_stmt;

static int udfs_getattr(const char *path, struct stat *stbuf)
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

static int udfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    int ok, file_id;
    (void) offset, fi;
    
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
    ok = sqlite3_prepare(g_db, "SELECT name FROM files WHERE path=?", 1000, &g_stmt, NULL); //FIXME: every 1000 magic numbers
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

static int udfs_open(const char *path, struct fuse_file_info *fi)
{
    return 0;

    if (strcmp(path, hello_path) != 0)
        return -ENOENT;

    if ((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}

static int udfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
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

static struct fuse_operations g_operations = {
    .getattr  = udfs_getattr,    
    .readdir  = udfs_readdir,
    .open     = udfs_open,
    .read     = udfs_read,
};

int main(int argc, char **argv)
{
    printf("Loading metadata database...\n");
    int ok = sqlite3_open_v2("metadata.db", &g_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);
    if (ok != SQLITE_OK) {
        fprintf(stderr, "%s", sqlite3_errmsg(g_db));
    }

    printf("Mounting filesystem...\n"); 
    fuse_main(argc, argv, &g_operations, NULL);
    sqlite3_close(g_db);

    return EXIT_SUCCESS;
}
