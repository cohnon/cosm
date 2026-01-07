#ifndef CHUNK_ALC_H
#define CHUNK_ALC_H

#include "cosm.h"
#include <stddef.h>

#define CHUNK_SIZE_SMALL  (4096 << 0)
#define CHUNK_SIZE_MEDIUM (4096 << 4)
#define CHUNK_SIZE_LARGE  (4096 << 8)

typedef struct chunk chunk;

typedef struct {
	uint64 chunk_size;
	chunk *cur;
} chunk_alc;

chunk_alc chunk_alc_init(uint64 chunk_size);
void chunk_alc_deinit(chunk_alc *alc);

void *mem_alloc_untyped(chunk_alc *alc, size_t size);
#define mem_alloc(alc, type) (type *)mem_alloc_untyped(alc, sizeof(type))

#endif
