#include "buffer.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

Buffer buf_init(size_t init_cap) {
    Buffer buf;
    buf.ptr = calloc(init_cap, 1);
    buf.cap = init_cap;
    buf.len = 0;

    return buf;
}

void buf_deinit(Buffer **buf) {
    free((*buf)->ptr);
    *buf = NULL;
}

static void ensure(Buffer *buf, size_t size) {
    if (buf->len + size >= buf->cap) {
        buf->ptr = realloc(buf->ptr, buf->cap * 1.5);
    }
}

size_t buf_push(Buffer *buf, uint8_t n) {
    ensure(buf, sizeof(n));
    return buf_push_bytes(buf, &n, sizeof(n));
}

size_t buf_push_bytes(Buffer *buf, void *ptr, size_t size) {
    ensure(buf, size);
    memcpy(buf->ptr + buf->len, ptr, size);
    size_t idx = buf->len;
    buf->len += size;

    return idx;
}

size_t buf_push_i32(Buffer *buf, uint32_t n) {
    ensure(buf, sizeof(n));
    return buf_push_bytes(buf, &n, sizeof(n));
}

size_t buf_push_varint(Buffer *buf, uint64_t n) {
    ensure(buf, sizeof(n));
    return buf_push_bytes(buf, &n, sizeof(n));
}

void buf_inc_varint(Buffer *buf, size_t idx) {
    *(size_t*)&buf->ptr[idx] += 1;
}
