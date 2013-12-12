#include "hook_write.h"

int hook_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    printf("Writing to %s\n", path);
    printf("Size %d\n", size);
    printf("Offset %d\n", offset);

    int chunk = offset / UDFS_SIZE_CHUNK; // [0 to infinity]
    int pos = offset % UDFS_SIZE_CHUNK;   // [0 through UDFS_SIZE_CHUNK]

    printf("Chunk: %d\n", chunk);
    printf("Pos: %d\n", pos);

    const char *end = buf + size;
    while (buf != end) {
        int len = MIN(UDFS_SIZE_CHUNK-pos,size);
        ufds_chunk_write(path, chunk, buf, len, pos);
        buf  += len;
        size -= len;
        pos   = 0;
        chunk++;
    }

    printf("done\n");

    return size;
}
