#include "main.h"

static const char *hello_path = "/hello";

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
    } else
        res = -ENOENT;

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

    return 0;
}

static int udfs_open(const char *path, struct fuse_file_info *fi)
{
    if (strcmp(path, hello_path) != 0)
        return -ENOENT;

    if ((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}

static int udfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    (void) fi;
    /*
    size_t len = 0;
    int ch;

    //if(strcmp(path, hello_path) != 0) {
    //  return -ENOENT;
    //}
    
    char *p      = buf;
    FILE *fp     = fopen("/home/nitrix/projects/udfs/build/test.jpg", "r"); // read mode
    if (!fp) {
        fprintf(stderr, "Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    fseek(fp, offset, SEEK_SET);
    while ((ch=fgetc(fp)) != EOF && len < size) {
        *p = ch;
        p++;
        len++;
    }
    fclose(fp);
    */

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
    printf("Hello World!\n"); 
    return fuse_main(argc, argv, &g_operations, NULL);
}
