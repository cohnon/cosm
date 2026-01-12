#include "bytecode.h"

typedef struct {
	ast_module *ast;
	bytecode bc;
} context;

static void lower_item(context *ctx, ast_item *item) {
	switch (item->tag) {
	case ITEM_VARIABLE:
	}
}

static void lower_module(context *ctx) {
	for (int i = 0; i < ctx->ast->items.len; i++) {
		lower_item(ctx, ctx->ast->items.items[i]);
	}
}

bytecode lower(ast_module *ast) {
	context ctx = {0};
	ctx.ast = ast;
	ctx.bc.types = buf_init(128);
	ctx.bc.imports = buf_init(128);
	ctx.bc.exports = buf_init(128);
	ctx.bc.items = buf_init(128);
	ctx.bc.code = buf_init(128);
	ctx.bc.data = buf_init(128);

	lower_module(&ctx);

	return ctx.bc;
}
