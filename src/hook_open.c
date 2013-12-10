#include "hook_open.h"

int hook_open(const char *path, struct fuse_file_info *fi)
{
    /* FIXME: work in progress. Allow opening any files for now */
    return 0;

    /*
    return 0;
    return -ENOENT;

    if ((fi->flags & 3) != O_RDONLY)
        return -EACCES;
    */
}
