#include "hook_rename.h"

int hook_rename(const char *old, const char *new)
{
    return udfs_metadata_file_rename(old, new);
}
