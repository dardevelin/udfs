#include "chunk.h"

int ufds_chunk_write(const char *path, int chunk, const char *buf, size_t size, off_t offset)
{
    printf("-------------\n");
    printf("\tWrite path: %s\n", path);
    printf("\tWrite chunk: %d\n", chunk);
    printf("\tWrite size: %d\n", size);
    printf("\tWrite offset: %d\n", offset);
    printf("\tWrite data: %.*s\n", size, buf);
    printf("-------------\n");

    if (offset > 0) {
        char buffer[size];

        // TODO: Fetching old chunk
        // something like udfs_chunk_read(path, buffer, size, offset);

        // Patch with our new data
        memcpy(buffer+offset, buf, size);
        
        // TODO: write to some endpoint and obtain a resource identifier
        // (not sure how the endpoint is determined yet)
        // resource = ideone_put(buffer, size);    
    } else {
        // TODO: write to some endpoint and obtain a resource identifier
        // (not sure how the endpoint is determined yet)
        // resource = ideone_put(buf, size);
    }
    
    // TODO: save the new resource identifier

    /*
     * TODO: metadata part, insert or update our chunk info
     */    
}
