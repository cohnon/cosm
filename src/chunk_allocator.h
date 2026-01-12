#ifndef CHUNK_ALLOCATOR_H
#define CHUNK_ALLOCATOR_H

#include "cosm.h"
#include <stddef.h>

#define CHUNK_SIZE_SMALL  (4096 << 0)
#define CHUNK_SIZE_MEDIUM (4096 << 4)
#define CHUNK_SIZE_LARGE  (4096 << 8)

typedef struct chunk chunk;

typedef struct {
	uint64 chunk_size;
	chunk *cur;
} chunk_allocator;

chunk_allocator chunk_allocator_init(uint64 chunk_size);
void chunk_allocator_deinit(chunk_allocator *alc);

void *mem_alloc_untyped(chunk_allocator *alc, size_t size);
#define mem_alloc(alc, type) (type *)mem_alloc_untyped(alc, sizeof(type))

#endif
