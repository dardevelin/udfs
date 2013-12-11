#ifndef METADATA_H
#define METADATA_H

#include <sqlite3.h>
#include "general.h"

sqlite3      *g_db;
sqlite3_stmt *g_stmt;

enum utfs_type {
    UDFS_TYPE_FILE = 0,
    UDFS_TYPE_DIR  = 1,
};

struct udfs_file {
    int  id;
    int  type;
    int  mode;
    int  size;
    bool found;
};

struct udfs_file udfs_metadata_file_query(const char *path);
void             udfs_metadata_file_filler(const char *dirname, void *buf, fuse_fill_dir_t filler);
int              udfs_metadata_file_add(const char *path, enum utfs_type type, int mode, int size);
int              udfs_metadata_file_remove(const char *path);
void             udfs_metadata_init(void);
void             udfs_metadata_fini(void);

#endif /* METADATA_H */

