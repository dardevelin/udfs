#ifndef CHUNK_H
#define CHUNK_H

#include "general.h"
#include "metadata.h"

int ufds_chunk_write(const char *path, int chunk, const char *buf, size_t size, off_t offset);

#endif /* CHUNK_H */
