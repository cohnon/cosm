#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include <stddef.h>

typedef struct Buffer {
    uint8_t *ptr;
    size_t   len;
    size_t   cap;
} Buffer;

Buffer buf_init(size_t init_cap);
void buf_deinit(Buffer **buf);

size_t buf_push(Buffer *buf, uint8_t n);
size_t buf_push_bytes(Buffer *buf, void *ptr, size_t size);
size_t buf_push_i32(Buffer *buf, uint32_t n);
size_t buf_push_varint(Buffer *buf, uint64_t n);

void buf_inc_varint(Buffer *buf, size_t idx);

#endif
