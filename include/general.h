#ifndef GENERAL_H
#define GENERAL_H

#define FUSE_USE_VERSION 28

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fuse.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>

#define UDFS_SIZE_ZSQL 1000
#define UDFS_SIZE_CHUNK 1024
#define UNUSED(x) ((void)x)
#define MIN(x,y) (x < y ? x : y)

#endif /* GENERAL_H */
