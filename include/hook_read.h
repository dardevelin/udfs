#ifndef HOOK_READ_H
#define HOOK_READ_H

#include "general.h"
#include "metadata.h"

int hook_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);

#endif /* HOOK_READ_H */
