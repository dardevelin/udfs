#include "hook_open.h"

int hook_open(const char *path, struct fuse_file_info *fi)
{
    return 0;
    return -ENOENT;

    if ((fi->flags & 3) != O_RDONLY)
        return -EACCES;
}
