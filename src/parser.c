#include "parser.h"

#include "chunk_alc.h"
#include "lexer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int toks_idx;
	token_list toks;
	item_list items;
	chunk_alc alc;
} context;

#define SYNTAX_ERROR(...) do { \
	fprintf(stderr, "parse error: " __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	exit(EXIT_FAILURE); \
} while(0)

#define CUR (ctx->toks.items[ctx->toks_idx >= ctx->toks.len ? ctx->toks.len - 1 : ctx->toks_idx])

#define EAT(expect) do { \
	if (CUR.tag != expect) { \
		SYNTAX_ERROR( \
			"expected %s, got %s (%d)", \
			token_string(expect), \
			token_string(CUR.tag), \
			ctx->toks_idx \
		); \
	} \
	ctx->toks_idx += 1; \
} while (0)

#define EAT_ANY (ctx->toks_idx += 1)

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

	if (CUR.tag == TOK_ARROW) {
		ast_type *fn_type = mem_alloc(&ctx->alc, ast_type);
		fn_type->tag = TYPE_FUNCTION;
		fn_type->fn.in = type;

		EAT(TOK_ARROW);

		fn_type->fn.out = parse_type(ctx);

		return fn_type;
	}

	return type;
}

static ast_expr *parse_expression(context *ctx);

static ast_expr *parse_block(context *ctx) {
	ast_expr *expr = mem_alloc(&ctx->alc, ast_expr);
	expr->tag = EXPR_BLOCK;

	array_init(&expr->blk.stmts, 16);

	EAT(TOK_BRACE_OPEN);

	while (CUR.tag != TOK_BRACE_CLOSE) {
		if (CUR.tag == TOK_EOF) {
			SYNTAX_ERROR("unterminated block");
		}

		EAT_ANY;
	}
	
	EAT(TOK_BRACE_CLOSE);

	return expr;
}

static ast_expr *parse_function_block(context *ctx) {
	ast_expr *expr = mem_alloc(&ctx->alc, ast_expr);
	expr->tag = EXPR_FUNCTION_BLOCK;

	EAT(TOK_FN);

	expr->fn_blk.type = NULL;
	if (CUR.tag != TOK_BRACE_OPEN) {
		expr->fn_blk.type = parse_type(ctx);
	}

	expr->fn_blk.body = parse_block(ctx);

	return expr;
}

static ast_expr *parse_foreign(context *ctx) {
	ast_expr *expr = mem_alloc(&ctx->alc, ast_expr);
	expr->tag = EXPR_FOREIGN;

	EAT(TOK_FOREIGN);
	EAT(TOK_STRING);
	parse_type(ctx);

	return expr;
}

static ast_expr *parse_primary_expression(context *ctx) {
	switch (CUR.tag) {
	case TOK_FN: return parse_function_block(ctx);
	case TOK_FOREIGN: return parse_foreign(ctx);
	default:
		SYNTAX_ERROR("expected expression");
	}
}

static ast_expr *parse_expression_prec(context *ctx, uint32 prec) {
	(void)prec;
	ast_expr *expr = parse_primary_expression(ctx);

	return expr;
}

static ast_expr *parse_expression(context *ctx) {
	return parse_expression_prec(ctx, 0);
}

static void parse_variable(context *ctx, ast_item *item) {
	item->tag = ITEM_VARIABLE;

	EAT(TOK_LET);
	EAT(TOK_SYMBOL);
	
	// type
	item->var.type = NULL;
	if (CUR.tag == TOK_COLON) {
		EAT(TOK_COLON);
		item->var.type = parse_type(ctx);
	}

	// value
	if (CUR.tag == TOK_EQUAL) {
		EAT(TOK_EQUAL);
		item->var.val = parse_expression(ctx);
	}
}

static ast_item *parse_item(context *ctx) {
	ast_item *item = mem_alloc(&ctx->alc, ast_item);

	switch (CUR.tag) {
	case TOK_LET: parse_variable(ctx, item); break;
	default: SYNTAX_ERROR("expected top level item, got %s", token_string(CUR.tag));
	}

	return item;
}

static void parse_module(context *ctx) {
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
