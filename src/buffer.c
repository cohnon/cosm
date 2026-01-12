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

size_t buf_push(byte_buffer *buf, uint8_t n) {
    ensure(buf, sizeof(n));
    return buf_push_bytes(buf, &n, sizeof(n));
}

size_t buf_push_bytes(byte_buffer *buf, void *ptr, size_t size) {
    ensure(buf, size);
    memcpy(buf->ptr + buf->len, ptr, size);
    size_t idx = buf->len;
    buf->len += size;

    return idx;
}

size_t buf_push_i32(byte_buffer *buf, uint32_t n) {
    ensure(buf, sizeof(n));
    return buf_push_bytes(buf, &n, sizeof(n));
}

size_t buf_push_varint(byte_buffer *buf, uint64_t n) {
    ensure(buf, sizeof(n));
    return buf_push_bytes(buf, &n, sizeof(n));
}

void buf_inc_varint(byte_buffer *buf, size_t idx) {
    *(size_t*)&buf->ptr[idx] += 1;
}
