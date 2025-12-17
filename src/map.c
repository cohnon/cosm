#include "map.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

MAP_DECL(Map, void);

static size_t hash(char *key, size_t key_len) {
    uint64_t hash = 0xcbf29ce484222325;
    
    for (size_t i = 0; i < key_len; i++) {
        hash *= 0x100000001b3;
        hash ^= (unsigned char)key[i];
    }

    return hash;
}

static _Bool key_eq(char *key1, size_t key1_len, char *key2, size_t key2_len) {
    size_t longest_len = key1_len > key2_len ? key1_len : key2_len;
    return strncmp(key1, key2, longest_len) == 0;
}

static void resize(Map *map, size_t item_sz, size_t new_cap) {
    void *old_items = map->items;
    MapKey *old_keys = map->keys;
    size_t old_cap = map->cap;

    map->items = calloc(new_cap, item_sz);
    map->items = calloc(new_cap, sizeof(MapKey));

    map->used = 0;
    map->cap = new_cap;

    for (size_t i = 0; i < old_cap; i++) {
        MapKey key = old_keys[i];
        if (key.ptr != NULL) {
            void *item = (unsigned char *)old_items + (item_sz * i);
            map_insert_untyped(map, item_sz, key.ptr, key.len, item);
        }
    }

    free(old_items);
    free(old_keys);
}

void map_init_untyped(void *any_map, size_t item_sz, size_t init_cap) {
    assert(init_cap >= 4);

    Map *map = any_map;
    map->items = calloc(init_cap, item_sz);
    map->keys = calloc(init_cap, sizeof(MapKey));
    map->used = 0;
    map->cap = init_cap;
}

void map_deinit(void *any_map) {
    Map *map = any_map;
    free(map->items);
    free(map->keys);
}

void map_insert_untyped(
    void *any_map, size_t item_sz,
    char *key, size_t key_len,
    void *item
) {
    Map *map = any_map;

    float used_ratio = (float)map->used /  (float)map->cap;
    if (used_ratio > 0.7f) {
        resize(map, item_sz, map->cap * 2);
    }

    size_t base_idx = hash(key, key_len);
    for (int i = 0; i < map->cap; i++) {
        size_t idx = (base_idx + i) % map->cap;
        MapKey *it = &map->keys[idx];

        _Bool empty_slot = it->ptr == NULL;
        _Bool key_exists = it->ptr != NULL && key_eq(key, key_len, it->ptr, it->len);

        if (empty_slot || key_exists) {
            unsigned char *dst = (unsigned char *)map->items + (item_sz * idx);
            memcpy(dst, item, item_sz);
            map->keys[idx].ptr = key;
            map->keys[idx].len = key_len;
            return;
        }
    }
}

void *map_get_untyped(
    void *any_map, size_t item_sz,
    char *key, size_t key_len
) {
    Map *map = any_map;

    size_t base_idx = hash(key, key_len);
    for (int i = 0; i < map->cap; i++) {
        size_t idx = (base_idx + i) % map->cap;
        MapKey *it = &map->keys[idx];

        if (it->ptr == NULL) {
            return NULL;
        }

        if (key_eq(key, key_len, it->ptr, it->len)) {
            return (unsigned char *)map->items + (item_sz * idx);
        }
    }

    return NULL;
}
