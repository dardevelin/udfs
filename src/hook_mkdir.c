#include "hook_mkdir.h"

int hook_mkdir(const char *path, mode_t mode)
{
    udfs_metadata_file_add(path, UDFS_TYPE_DIR, mode, 0);

    return 0;
}
