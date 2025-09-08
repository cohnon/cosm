#include "array.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

ARRAY_DECL(Array, unsigned char);

static void ensure_cap(void *any_arr, int item_sz, size_t cap) {
    Array *arr = any_arr;

    if (cap > (size_t)arr->cap) {
        arr->cap = cap;

        arr->items = realloc(arr->items, arr->cap * item_sz);
        assert(arr->items != NULL);
    }
}

static void grow(void *any_arr, size_t item_sz) {
    Array *arr = any_arr;

    size_t new_cap = (size_t)((double)arr->cap * 1.5);
    ensure_cap(arr, item_sz, new_cap);
}

static void *items_from_idx(void *any_arr, size_t item_sz, size_t idx) {
    Array *arr = any_arr;
    return arr->items + (idx * item_sz);
}

void array_init_untyped(void *any_arr, size_t item_sz, size_t init_cap) {
    Array *arr = any_arr;

    assert(item_sz > 0);
    assert(init_cap >= 8);

    arr->items = NULL;
    arr->len = 0;
    arr->cap = 0;

    ensure_cap(arr, item_sz, init_cap);
}

void array_deinit(void *any_arr) {
    Array *arr = any_arr;

    assert(arr != NULL);

    free(arr->items);
    arr->items = NULL;
    arr->len = 0;
    arr->cap = 0;
}

void array_append_untyped(void *any_arr, size_t item_sz, void *item) {
    Array *arr = any_arr;

    assert(arr != NULL);
    assert(item_sz > 0);
    assert(item != NULL);

    if (arr->len + 1 > arr->cap) {
        grow(arr, item_sz);
    }

    memcpy(items_from_idx(arr, item_sz, arr->len), item, item_sz);

    arr->len += 1;
}

