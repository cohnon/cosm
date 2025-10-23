#include "parser.h"
#include "lexer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    Module    *mod;
    size_t     toks_idx;
    TokenArray toks;
    _Bool      completed_foreigns;
} Context;

#define SYNTAX_ERROR(...) do {                     \
    fprintf(stderr, "syntax error: " __VA_ARGS__); \
    fprintf(stderr, "\n");                         \
    exit(EXIT_FAILURE);                            \
} while(0)

#define CUR (ctx->toks.items[ctx->toks_idx])

#define EAT(expect) do {                    \
    if (CUR.type != expect) {               \
        SYNTAX_ERROR(                       \
            "expected %s, got %s (%ld)",    \
            token_string_from_type(expect), \
            token_string(CUR),              \
            ctx->toks_idx                   \
        );                                  \
    }                                       \
    ctx->toks_idx += 1;                     \
} while (0)

#define EAT_ANY (ctx->toks_idx += 1)
#define EAT_RET(expect) CUR; EAT(expect)

void add_string(Context *ctx, Buffer *ref_buf, char *ptr, size_t len) {
    COSM_Ref ref = buf_push_bytes(&ctx->mod->strings, ptr, len);
    buf_push_varint(ref_buf, ref);
    buf_push_varint(ref_buf, len);
}

static COSM_Ref push_type(Context *ctx, COSM_Type type) {
    return buf_push_varint(&ctx->mod->types, type) + 128;
}

static void inc_type(Context *ctx, COSM_Ref ref) {
    buf_inc_varint(&ctx->mod->types, ref);
}

COSM_Ref parse_type(Context *ctx) {
    switch (CUR.type) {
    case TOK_I8: return COSM_TYPE_I8;
    case TOK_I32: return COSM_TYPE_I32;
    case TOK_WORD: return COSM_TYPE_WORD;

    case TOK_STAR:
        EAT(TOK_STAR);
        COSM_Ref ptr_ref = push_type(ctx, COSM_TYPE_PTR);
        parse_type(ctx);
        return ptr_ref;

    case TOK_PAREN_OPEN:
        EAT(TOK_PAREN_OPEN);
        COSM_Ref tup_ref = push_type(ctx, COSM_TYPE_FUNC);
        COSM_Ref len_ref = push_type(ctx, 1);
        parse_type(ctx);
        while (CUR.type == TOK_COMMA) {
            inc_type(ctx, len_ref);
            parse_type(ctx);
        }
        EAT(TOK_PAREN_CLOSE);
        return tup_ref;

    case TOK_FN:
        EAT(TOK_FN);
        COSM_Ref fn_ref = push_type(ctx, COSM_TYPE_FUNC);
        parse_type(ctx);
        EAT(TOK_ARROW);
        parse_type(ctx);
        return fn_ref;

    default:
        SYNTAX_ERROR("error");
    }
}

void parse_item(Context *ctx) {
    Token sym = EAT_RET(TOK_SYMBOL);

    COSM_Ref *exists = map_get(&ctx->mod->item_ns, sym.src, sym.len);
    if (exists != NULL) {
        SYNTAX_ERROR("duplicate item name %.*s", TOK_FMT(sym));
    }

    if (CUR.type == TOK_FOREIGN) {
        if (ctx->completed_foreigns) {
            SYNTAX_ERROR("all foreign items must be at top of file");
        }

        EAT(TOK_FOREIGN);
        add_string(ctx, &ctx->mod->foreigns, sym.src, sym.len);
        COSM_Ref ty_ref = parse_type(ctx);
        buf_push_varint(&ctx->mod->foreigns, ty_ref);
        return;
    }

    ctx->completed_foreigns = 1;

    if (CUR.type == TOK_GLOBAL) {
        EAT(TOK_GLOBAL);
        add_string(ctx, &ctx->mod->globals, sym.src, sym.len);
        buf_push_varint(&ctx->mod->globals, ctx->mod->items.len);
    }

    COSM_Ref ty_ref = parse_type(ctx);
    buf_push_varint(&ctx->mod->items, ty_ref);

    /*
    COSM_Ref type =  ctx->mod->types.items[ty_ref];
    switch (type) {
    case COSM_TYPE_FUNC:
        parse_function(ctx, ty_ref);
        break;

    default:
        SYNTAX_ERROR("unhandled item type");
    }
    */
    SYNTAX_ERROR("only foreign items for now :/");
}

void parse_toplevel(Context *ctx) {
    switch (CUR.type) {
    case TOK_SYMBOL:
        parse_item(ctx);
        break;

    default:
        SYNTAX_ERROR("unexpected top level statement: %s", token_string(CUR));
    }
}

void parse_module(Context *ctx) {
    ctx->mod = malloc(sizeof(Module));
    ctx->mod->foreigns = buf_init(64);
    ctx->mod->globals = buf_init(64);
    ctx->mod->items = buf_init(64);
    ctx->mod->code= buf_init(64);
    ctx->mod->data = buf_init(64);
    ctx->mod->types = buf_init(64);
    ctx->mod->strings = buf_init(64);

    map_init(&ctx->mod->item_ns, 32);

    while (CUR.type != TOK_EOF) {
        parse_toplevel(ctx);
    }
}

Module *parse(TokenArray toks) {
    Context ctx;
    ctx.toks = toks;
    ctx.toks_idx = 0;
    ctx.completed_foreigns = 0;

    parse_module(&ctx);

    return ctx.mod;
}
