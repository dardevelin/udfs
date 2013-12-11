#ifndef MAIN_H
#define MAIN_H

#include "general.h"
#include "hook_chmod.h"
#include "hook_chown.h"
#include "hook_getattr.h"
#include "hook_mkdir.h"
#include "hook_mknod.h"
#include "hook_open.h"
#include "hook_read.h"
#include "hook_readdir.h"
#include "hook_rename.h"
#include "hook_rmdir.h"
#include "hook_unlink.h"
#include "hook_utimens.h"
#include "metadata.h"

int main(int argc, char **argv);

#endif /* MAIN_H */
