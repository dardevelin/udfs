#include "main.h"

static struct fuse_operations g_operations = {
    .chmod    = hook_chmod,
    .chown    = hook_chown,
    .getattr  = hook_getattr,    
    .mkdir    = hook_mkdir,
    .mknod    = hook_mknod,
    .open     = hook_open,
    .read     = hook_read,
    .readdir  = hook_readdir,
    .rename   = hook_rename,
    .rmdir    = hook_rmdir,
    .unlink   = hook_unlink,
    .utimens  = hook_utimens
};

int main(int argc, char **argv)
{
    printf("Loading metadata database...\n");
    udfs_metadata_init();

    printf("Mounting filesystem...\n"); 
    fuse_main(argc, argv, &g_operations, NULL);

    return EXIT_SUCCESS;
}
