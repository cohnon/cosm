#ifndef COSI_ARRAY_H
#define COSI_ARRAY_H

#include <stddef.h>

#define ARRAY_DECL(name, T) typedef struct { \
    T  *items; \
    int len; \
    int cap; \
} name

void array_init_untyped(void *arr, size_t item_sz, size_t init_cap);
#define array_init(arr, init_cap) array_init_untyped((arr), sizeof(*(arr)->items), init_cap)

void array_deinit(void *any_arr);

void array_append_untyped(void *any_arr, size_t item_sz, void *item);
#define array_append(arr, item) array_append_untyped((arr), sizeof(*(arr)->items), item);

#endif
