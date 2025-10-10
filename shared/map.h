#ifndef COSM_MAP_H
#define COSM_MAP_H

#include <stddef.h>

typedef struct MapKey {
    char *ptr;
    size_t len;
} MapKey;

#define MAP_DECL(name, T) typedef struct name { \
    T      *items;    \
    MapKey *keys;     \
    int     used; \
    int     cap;      \
} name

void map_init_untyped(void *any_map, size_t item_sz, size_t init_cap);
#define map_init(map, init_cap) \
    map_init_untyped((map), sizeof(__typeof__(*(map)->items)), init_cap)

void map_deinit(void *any_map);

void map_insert_untyped(
    void *any_map, size_t item_sz,
    char *key, size_t key_len,
    void *item
);
#define map_insert(map, key, key_len, item) do {                            \
    __typeof__((map)->items) tmp = item;                                    \
    map_insert_untyped((map), sizeof(__typeof__(*tmp)), key, key_len, tmp); \
} while (0)

void *map_get_untyped(
    void *any_map, size_t item_sz,
    char *key, size_t key_len
);
#define map_get(map, key, key_len) \
    (__typeof__((map)->items))map_get_untyped( \
        (map), sizeof(__typeof__(*(map)->items)), key, key_len \
    )

#endif
