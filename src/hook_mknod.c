#include "hook_mknod.h"

int hook_mknod(const char *path, mode_t mode, dev_t dev)
{
    return udfs_metadata_file_add(path, UDFS_TYPE_FILE, mode, 0);
}
