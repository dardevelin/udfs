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

#include "hook_getattr.h"
#include "hook_open.h"
#include "hook_read.h"
#include "hook_readdir.h"

int main(int argc, char **argv);

#endif /* MAIN_H */
