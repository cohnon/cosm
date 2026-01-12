#include "chunk_allocator.h"
#include <stdlib.h>

struct chunk {
	uint64 used;
	chunk *prev;
	uint8 buf[];
};

static chunk *create_chunk(chunk_allocator *alc) {
	chunk *c = malloc(sizeof(chunk) + alc->chunk_size);
	c->used = 0;
	c->prev = alc->cur;

	return c;
}

chunk_allocator chunk_allocator_init(uint64 chunk_size) {
	chunk_allocator alc = { 0 };
	alc.chunk_size = chunk_size;
	alc.cur = create_chunk(&alc);

	return alc;
}

void chunk_allocator_deinit(chunk_allocator *alc) {
	chunk *cur = alc->cur;
	while (cur) {
		chunk *to_free = cur;
		cur = cur->prev;
		free(to_free);
	}
}

void *mem_alloc_untyped(chunk_allocator *alc, size_t size) {
	if (alc->cur->used + size > alc->chunk_size) {
		create_chunk(alc);
	}

	void *ptr = alc->cur->buf + alc->cur->used;
	alc->cur->used += size;

	return ptr;
}
