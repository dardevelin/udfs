#include "main.h"

static struct fuse_operations g_operations = {
    .getattr  = hook_getattr,    
    .readdir  = hook_readdir,
    .open     = hook_open,
    .read     = hook_read,
};

int main(int argc, char **argv)
{
    printf("Loading metadata database...\n");
    udfs_metadata_init();

    printf("Mounting filesystem...\n"); 
    fuse_main(argc, argv, &g_operations, NULL);

    return EXIT_SUCCESS;
}
