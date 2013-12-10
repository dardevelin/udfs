#ifndef HOOK_GETATTR_H
#define HOOK_GETATTR_H

#include "general.h"
#include "metadata.h"

int hook_getattr(const char *path, struct stat *stbuf);

#endif /* HOOK_GETATTR_H */
