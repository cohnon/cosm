#include "parser.h"
#include "array.h"
#include "lexer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    Module    *mod;
    size_t     toks_idx;
    TokenArray toks;
} Parser;

#define CUR (psr->toks.items[psr->toks_idx])

#define EAT(expect) do {                               \
    if (CUR.type != expect) {                          \
        fprintf(stderr, "expected %s, got %s (%ld)\n", \
                token_string_from_type(expect),        \
                token_string(CUR),                     \
                psr->toks_idx);                        \
        exit(EXIT_FAILURE);                            \
    }                                                  \
    psr->toks_idx += 1;                                \
} while (0)

#define EAT_ANY (psr->toks_idx += 1)

_Bool is_type(Parser *psr) {
    switch (CUR.type) {
    case TOK_I8:
    case TOK_I32:
        return 1;

    default:
        return 0;
    }
}

void parse_item(Parser *psr) {
    EAT(TOK_SYMBOL);

    switch (CUR.type) {
    case TOK_GLOBAL:
        EAT(TOK_GLOBAL);
        break;

    case TOK_FOREIGN:
        EAT(TOK_FOREIGN);
        break;

    default:
        break;
    }

    if (1) {
        while (CUR.type != TOK_END) {
        }

        EAT(TOK_END);
    } else {
    }
}

void parse_toplevel(Parser *psr) {
    switch (CUR.type) {
    case TOK_SYMBOL: {
        map_get(&psr->mod->item_map, item->sym, item->sym_len);

        if (found != NULL) {
            fprintf(stderr, "error: duplicate item name %.*s\n", (int)item->sym_len, item->sym);
            exit(EXIT_FAILURE);
        }
        array_append(&psr->mod->items, &item);
        map_insert(&psr->mod->item_map, item->sym, item->sym_len, &item);
        break;
    }

    default:
        fprintf(stderr, "unexpected item: %s\n", token_string(CUR));
        exit(EXIT_FAILURE);
    }
}

void parse_module(Parser *psr) {
    psr->mod = malloc(sizeof(Module));
    array_init(&psr->mod->imports, 64);
    array_init(&psr->mod->exports, 64);
    array_init(&psr->mod->strings, 64);
    array_init(&psr->mod->types, 64);
    array_init(&psr->mod->items, 64);
    array_init(&psr->mod->code, 64);
    array_init(&psr->mod->data, 64);

    map_init(&psr->mod->type_map, 16);
    map_init(&psr->mod->item_map, 16);

    while (CUR.type != TOK_EOF) {
        parse_toplevel(psr);
    }
}

Module *parse(TokenArray toks) {
    Parser psr;
    psr.toks = toks;
    psr.toks_idx = 0;

    parse_module(&psr);

    return psr.mod;
}
