#include "main.h"

static const char *hello_path = "/hello";
sqlite3      *g_db;
sqlite3_stmt *g_stmt;

static int udfs_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    } else if (strcmp(path, hello_path) == 0) {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = 10747; /* file size */
    } else {
        /* Everything else */
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = 10; /* file size */
    }
        //res = -ENOENT;

    return res;
}

static int udfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
        off_t offset, struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, hello_path + 1, NULL, 0);
    filler(buf, "testa", NULL, 0);
    filler(buf, "testb", NULL, 0);
    filler(buf, "testc", NULL, 0);

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

    return size;
}

static struct fuse_operations g_operations = {
    .getattr = udfs_getattr,    
    .readdir = udfs_readdir,
    .open    = udfs_open,
    .read    = udfs_read,
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
