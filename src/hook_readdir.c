#include "hook_readdir.h"

int hook_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    UNUSED(offset);
    UNUSED(fi);

    /* Standard files */
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    /* Call the metadata file filler */
    udfs_metadata_file_filler(path, buf, filler);
}
