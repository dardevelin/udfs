#ifndef HOOK_WRITE_H
#define HOOK_WRITE_H

#include "general.h"
#include "metadata.h"
#include "chunk.h"

int hook_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);

#endif /* HOOK_WRITE_H */
