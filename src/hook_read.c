#include "hook_read.h"

int hook_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    UNUSED(fi);

    /* Get file id */
    struct udfs_file file = udfs_metadata_file_query(path);
    if (file.found) {
        //printf("File id: %u\n", metadata.id);
    } else {
        fprintf(stderr, "Error: no existing metadata for this file\n");
        return -ENOENT;
    }

    /*
     * TODO: find chunks for this file id
     * TODO: write data to buf - calculated from size and offset
     */

    return file.size;
}
