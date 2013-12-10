#ifndef METADATA_H
#define METADATA_H

#include <sqlite3.h>
#include "general.h"

sqlite3      *g_db;
sqlite3_stmt *g_stmt;

enum {
    UDFS_TYPE_FILE = 0,
    UDFS_TYPE_DIR  = 1,
};

struct udfs_metadata {
    int  id;
    int  type;
    int  mode;
    int  size;
    bool found;
};

struct udfs_metadata udfs_metadata_query(const char *path);
void                 udfs_metadata_init(void);
void                 udfs_metadata_fini(void);

#endif /* METADATA_H */

