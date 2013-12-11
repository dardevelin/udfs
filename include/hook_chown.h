#ifndef HOOK_CHOWN_H
#define HOOK_CHOWN_H

#include "general.h"

int hook_chown(const char *path, uid_t uid, gid_t gid);

#endif /* HOOK_CHOWN_H */
