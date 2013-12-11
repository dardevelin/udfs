#include "hook_unlink.h"

int hook_unlink(const char *path)
{
    return udfs_metadata_file_remove(path);
}
