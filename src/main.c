#include "main.h"

static const char *hello_path = "/hello";
sqlite3      *g_db;
sqlite3_stmt *g_stmt;

static struct fuse_operations g_operations = {
    .getattr  = hook_getattr,    
    .readdir  = hook_readdir,
    .open     = hook_open,
    .read     = hook_read,
};

int main(int argc, char **argv)
{
    printf("Loading metadata database...\n");
    int ok = sqlite3_open_v2("metadata.db", &g_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);
    if (ok != SQLITE_OK) {
        fprintf(stderr, "%s", sqlite3_errmsg(g_db));
    }

    printf("Mounting filesystem...\n"); 
    fuse_main(argc, argv, &g_operations, NULL);
    sqlite3_close(g_db);

    return EXIT_SUCCESS;
}
