#ifndef HOOK_MKNOD_H
#define HOOK_MKNOD_H

#include "general.h"
#include "metadata.h"

int hook_mknod(const char *path, mode_t mode, dev_t dev);

#endif /* HOOK_MKNOD_H */
