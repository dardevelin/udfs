#ifndef MAIN_H
#define MAIN_H

#define FUSE_USE_VERSION 28

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fuse.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "sqlite3.h"

int main(int argc, char **argv);

static int udfs_getattr(const char *path, struct stat *stbuf);
static int udfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
static int udfs_open(const char *path, struct fuse_file_info *fi);
static int udfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);

#endif /* MAIN_H */
