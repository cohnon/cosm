#include "fir_util.h"

#include <assert.h>
#include <stdlib.h>
#include <stdalign.h>

struct fir_Page {
    fir_Page *prev;

    size_t len;
    uint8_t data[];
};

static void new_page(fir_Arena *arena) {
    fir_Page *new_page = malloc(sizeof(fir_Page) + arena->page_size);

    assert(new_page != NULL);

    new_page->len = 0;
    new_page->prev = arena->cur_page;

    arena->cur_page = new_page;
}

static _Bool fits(fir_Arena *arena, size_t size) {
    return (arena->cur_page->len + size) <= arena->page_size;
}

static void ensure_size(fir_Arena *arena, size_t size) {
    if (!fits(arena, size)) {
        new_page(arena);
    }
}

fir_Arena fir_arena_init(size_t page_size) {
    assert(page_size > 0);

    fir_Arena arena;
    arena.cur_page = NULL;
    arena.page_size = page_size;

    new_page(&arena);

    return arena;
}

void fir_arena_deinit(fir_Arena *arena) {
    assert(arena != NULL);

    fir_Page *idx = arena->cur_page;
    while (idx) {
        fir_Page *to_free = idx;
        idx = idx->prev;

        free(to_free);
    }

    arena->cur_page = NULL;
    arena->page_size = 0;
}

void *fir_arena_alloc(fir_Arena *arena, size_t size) {
    assert(arena != NULL);
    assert(size > 0);

    ensure_size(arena, size);

    fir_Page *page = arena->cur_page;

    void *ptr = page->data + page->len;
    page->len += size;

    return ptr;
}
