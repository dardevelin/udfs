#ifndef GENERAL_H
#define GENERAL_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fuse.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

enum {
    UDFS_TYPE_FILE = 0,
    UDFS_TYPE_DIR  = 1,
};

#endif /* GENERAL_H */
