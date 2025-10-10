#include "analyse.h"

typedef struct Context {
    Module *mod;
} Context;

void analyse_item(Context *ctx, Item *item) {
    (void)ctx;
    (void)item;
}

void analyse(Module *mod) {
    Context ctx;
    ctx.mod = mod;

    for (int i = 0; i < mod->items.len; i++) {
        analyse_item(&ctx, mod->items.items[i]);
    }
}
