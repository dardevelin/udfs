#ifndef HOOK_UTIMENS_H
#define HOOK_UTIMENS_H

#include "general.h"

int hook_utimens(const char *path, const struct timespec tv[2]);

#endif /* HOOK_UTIMENS_H */
