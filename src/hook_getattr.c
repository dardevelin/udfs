#include "hook_getattr.h"

int hook_getattr(const char *path, struct stat *stbuf)
{
    /* I don't know all the fields, let's clear everything */
    memset(stbuf, 0, sizeof(struct stat));
    
    /* Query metadata */
    struct udfs_metadata metadata = udfs_metadata_query(path);    

    /* Return an error if not found */
    if (!metadata.found) {
        return -ENOENT;
    }

    /* Fill the struct stat object */
    switch (metadata.type) {
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
    stbuf->st_mode |= metadata.mode;
    stbuf->st_size  = metadata.size;

    return 0;
}
