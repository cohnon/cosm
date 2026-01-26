#ifndef BUFFER_H
#define BUFFER_H

#include "cosm.h"
#include <stddef.h>

typedef struct {
    uint8_t *ptr;
    size_t   len;
    size_t   cap;
} byte_buffer;

byte_buffer buf_init(size_t init_cap);
void buf_deinit(byte_buffer **buf);

uint64 buf_ref(byte_buffer *buf);
uint64 buf_push(byte_buffer *buf, uint8_t n);
uint64 buf_push_int32(byte_buffer *buf, uint32_t n);
uint64 buf_push_varint(byte_buffer *buf, uint64_t n);

uint64 buf_push_bytes(byte_buffer *buf, void *ptr, size_t size);
uint64 buf_push_array(byte_buffer *buf, void *ptr, size_t size);

#endif
