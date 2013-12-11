#include "hook_rmdir.h"

int hook_rmdir(const char *path)
{
    udfs_metadata_file_remove(path);
    return 0;
}
