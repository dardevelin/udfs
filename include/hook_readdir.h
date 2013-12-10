#ifndef HOOK_READDIR_H
#define HOOK_READDIR_H

#include "general.h"
#include "metadata.h"

int hook_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);

#endif /* HOOK_READDIR_H */
