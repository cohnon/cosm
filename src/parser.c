#include "parser.h"

#include "lexer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	module *mod;
	size_t toks_idx;
	token_list toks;
} Context;

#define SYNTAX_ERROR(...) do { \
	fprintf(stderr, "syntax error: " __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	exit(EXIT_FAILURE); \
} while(0)

#define CUR (ctx->toks.items[ctx->toks_idx])

#define EAT1(expect) do { \
	if (CUR.type != expect) { \
		SYNTAX_ERROR( \
			"expected %s, got %s (%ld)", \
			token_string_from_type(expect), \
			token_string(CUR), \
			ctx->toks_idx \
		); \
	} \
	ctx->toks_idx += 1; \
} while (0)

#define EAT_ANY (ctx->toks_idx += 1)
#define EAT(expect) CUR; EAT1(expect)

void parse_module(Context *ctx) {
	while (CUR.tag != TOK_EOF) {
		EAT_ANY;
	}
}

module *parse(token_list toks) {
	Context ctx;
	ctx.toks = toks;
	ctx.toks_idx = 0;

	parse_module(&ctx);

	return ctx.mod;
}
