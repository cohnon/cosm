#include "parser.h"

#include "chunk_alc.h"
#include "lexer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	size_t toks_idx;
	token_list toks;
	item_list items;
	chunk_alc alc;
} context;

#define SYNTAX_ERROR(...) do { \
	fprintf(stderr, "parse error: " __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	exit(EXIT_FAILURE); \
} while(0)

#define CUR (ctx->toks.items[ctx->toks_idx])

#define EAT1(expect) do { \
	if (CUR.tag != expect) { \
		SYNTAX_ERROR( \
			"expected %s, got %s (%ld)", \
			token_string(expect), \
			token_string(CUR.tag), \
			ctx->toks_idx \
		); \
	} \
	ctx->toks_idx += 1; \
} while (0)

#define EAT_ANY (ctx->toks_idx += 1)
#define EAT(expect) CUR; EAT1(expect)

ast_type *infer_type(context *ctx) {
	ast_type *type = mem_alloc(&ctx->alc, ast_type);
	type->tag = TYPE_INFER;
	return type;
}

ast_type *parse_type(context *ctx) {
	ast_type *type = mem_alloc(&ctx->alc, ast_type);

	switch (CUR.tag) {
	case TOK_SYMBOL:
		type->tag = TYPE_SYMBOL;
		EAT(TOK_SYMBOL);
		break;

	case TOK_STAR:
		type->tag = TYPE_POINTER;
		EAT(TOK_STAR);
		type->ptr.to = parse_type(ctx);
		break;

	case TOK_BRACE_OPEN:
		type->tag = TYPE_SLICE;
		EAT(TOK_BRACKET_OPEN);
		EAT(TOK_BRACKET_CLOSE);
		type->slc.of = parse_type(ctx);
		break;

	default:
		SYNTAX_ERROR("expected type token");
	}

	return type;
}

ast_expr *parse_expression(context *ctx);

void parse_primary_expression(context *ctx, ast_expr *expr) {
	expr->tag = EXPR_NUMBER;
	EAT(TOK_NUMBER);
}

ast_expr *parse_expression_prec(context *ctx, uint32 prec) {
	(void)prec;
	ast_expr *expr = mem_alloc(&ctx->alc, ast_expr);

	parse_primary_expression(ctx, expr);

	return expr;
}

ast_expr *parse_expression(context *ctx) {
	return parse_expression_prec(ctx, 0);
}

ast_pattern *parse_pattern(context *ctx) {
	ast_pattern *pat = mem_alloc(&ctx->alc, ast_pattern);

	pat->tag = PATTERN_SYMBOL;

	return pat;
}

void parse_variable(context *ctx, ast_item *item) {
	item->tag = ITEM_VARIABLE;

	EAT(TOK_LET);
	EAT(TOK_SYMBOL);
	
	// type
	if (CUR.tag == TOK_COLON) {
		EAT(TOK_COLON);
		item->var.type = parse_type(ctx);
	} else {
		item->var.type = infer_type(ctx);
	}

	// value
	if (CUR.tag == TOK_EQUAL) {
		EAT(TOK_EQUAL);
		item->var.val = parse_expression(ctx);
	}
}

ast_item *parse_item(context *ctx) {
	ast_item *item = mem_alloc(&ctx->alc, ast_item);

	switch (CUR.tag) {
	case TOK_LET: parse_variable(ctx, item);
	default: SYNTAX_ERROR("expected top level item");
	}

	return item;
}

void parse_module(context *ctx) {
	while (CUR.tag != TOK_EOF) {
		parse_item(ctx);
	}
}

item_list parse(token_list toks) {
	context ctx;
	ctx.toks = toks;
	ctx.toks_idx = 0;
	ctx.alc = chunk_alc_init(CHUNK_SIZE_SMALL);

	parse_module(&ctx);

	return ctx.items;
}
