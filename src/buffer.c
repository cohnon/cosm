#include "buffer.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

byte_buffer buf_init(size_t init_cap) {
    byte_buffer buf;
    buf.ptr = calloc(init_cap, 1);
    buf.cap = init_cap;
    buf.len = 0;

    return buf;
}

void buf_deinit(byte_buffer **buf) {
    free((*buf)->ptr);
    *buf = NULL;
}

static void ensure(byte_buffer *buf, size_t size) {
    if (buf->len + size >= buf->cap) {
        buf->ptr = realloc(buf->ptr, buf->cap * 1.5);
    }
}

uint64 buf_ref(byte_buffer *buf) {
	return buf->len;
}

uint64 buf_push(byte_buffer *buf, uint8 n) {
    ensure(buf, sizeof(n));
    return buf_push_bytes(buf, &n, sizeof(n));
}

uint64 buf_push_int32(byte_buffer *buf, uint64 n) {
    ensure(buf, sizeof(n));
    return buf_push_bytes(buf, &n, sizeof(n));
}

uint64 buf_push_varint(byte_buffer *buf, uint64_t n) {
	uint64_t encoded = n << 1;
	int n_bytes = 0;
	uint64_t max = 1 << 7;

	uint64 start = buf->len;

	if (n >= 1ull << 63) {
		buf->ptr[buf->len++] = 0xff;
		buf_push_bytes(buf, &encoded, 8);
		return start;
	}

	while (n > max) {
		encoded = (encoded << 1) | 1;
		max <<= 7;
		n_bytes += 1;
	}

	buf_push_bytes(buf, &encoded, 8);
	return start;
}

uint64 buf_push_bytes(byte_buffer *buf, void *ptr, size_t size) {
    ensure(buf, size);
    memcpy(buf->ptr + buf->len, ptr, size);
	uint64 start = buf->len;
    buf->len += size;

	return start;
}

uint64 buf_push_array(byte_buffer *buf, void *ptr, size_t size) {
	uint64 start = buf->len;
	buf_push_varint(buf, size);
	buf_push_bytes(buf, ptr, size);
	return start;
}
