#include "parser.h"

#include "lexer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	Module *mod;
	size_t toks_idx;
	TokenArray toks;
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

Type *parse_type(Context *ctx) {
	Type *type = malloc(sizeof(Type));

	switch (CUR.type) {
	case TOK_UNIT:
		EAT(TOK_UNIT);
		type->tag = TYPE_UNIT;
		break;

	case TOK_I8:
		EAT(TOK_I8);
		type->tag = TYPE_I8;
		break;

	case TOK_I32:
		EAT(TOK_I32);
		type->tag = TYPE_I32;
		break;

	case TOK_STAR:
		EAT(TOK_STAR);
		type->tag = TYPE_PTR;
		type->ptr.to = parse_type(ctx);
		break;

	case TOK_BRACKET_OPEN:
		EAT(TOK_BRACKET_OPEN);
		EAT(TOK_BRACKET_CLOSE);
		type->tag = TYPE_SLC;
		type->slc.of = parse_type(ctx);
		break;

	case TOK_PAREN_OPEN:
		EAT(TOK_PAREN_OPEN);
		for (;;) {
			parse_type(ctx);
			if (CUR.type != TOK_COMMA) break;
			EAT(TOK_COMMA);
		}
		EAT(TOK_PAREN_CLOSE);
		break;

	case TOK_FN:
		EAT(TOK_FN);
		type->tag = TYPE_FN;
		type->fn.in = parse_type(ctx);
		EAT(TOK_ARROW);
		type->fn.out = parse_type(ctx);
		break;

	case TOK_TYPE:
		EAT(TOK_TYPE);
		type->tag = TYPE_TYPE;
		break;

	case TOK_SYMBOL: {
		EAT(TOK_SYMBOL);
		if (CUR.type == TOK_COLON) {
			EAT(TOK_COLON);
			parse_type(ctx);
		}
		break;
	}
	default:
		SYNTAX_ERROR("expected type, got %.*s", TOK_FMT(CUR));
	}

	return NULL;
}

Value *parse_value(Context *ctx);

Insn *parse_instruction(Context *ctx) {
	Insn *insn = malloc(sizeof(Insn));

	if (CUR.type == TOK_SYMBOL) {
		EAT(TOK_SYMBOL);
		EAT(TOK_EQUAL);
	}

	switch (CUR.type) {
	case TOK_ADD_I32:
		insn->tag = INSN_ADD_I32;
		EAT(TOK_ADD_I32);
		parse_value(ctx);
		parse_value(ctx);
		break;

	case TOK_CONST_I32:
		insn->tag = INSN_CONST_I32;
		EAT(TOK_CONST_I32);
		parse_value(ctx);
		break;

	case TOK_CALL:
		insn->tag = INSN_CALL;
		EAT(TOK_CALL);
		parse_value(ctx);
		parse_value(ctx);
		break;

	case TOK_RETURN:
		insn->tag = INSN_RETURN;
		EAT(TOK_RETURN);
		parse_value(ctx);
		break;

	default:
		SYNTAX_ERROR("expected instruction. got %.*s", TOK_FMT(CUR));
	}

	return insn;
}

Value *parse_value(Context *ctx) {
	Value *val = malloc(sizeof(Value));

	switch (CUR.type) {
	case TOK_BRACE_OPEN:
		EAT(TOK_BRACE_OPEN);
		
		while (CUR.type != TOK_BRACE_CLOSE) {
			parse_instruction(ctx);
		}

		EAT(TOK_BRACE_CLOSE);
		break;

	case TOK_NUMBER:
		EAT(TOK_NUMBER);
		break;

	case TOK_STRING:
		EAT(TOK_STRING);
		break;
	
	case TOK_SYMBOL:
		EAT(TOK_SYMBOL);
		break;

	case TOK_PAREN_OPEN:
		EAT(TOK_PAREN_OPEN);
		for (;;) {
			parse_value(ctx);
			if (CUR.type != TOK_COMMA) break;
			EAT(TOK_COMMA);
		}
		EAT(TOK_PAREN_CLOSE);
		break;

	default:
		SYNTAX_ERROR("expected value. got %.*s", TOK_FMT(CUR));
	}

	return val;
}

void parse_type_decl(Context *ctx) {
	EAT_ANY;
	parse_type(ctx);
}

Item *parse_item(Context *ctx) {
	Item *item = malloc(sizeof(Item));
	item->vis = ITEM_LOCAL;

	Token sym = EAT(TOK_SYMBOL);

	// TODO: check if symbol exists

	if (CUR.type == TOK_PLUS) {
		EAT(TOK_PLUS);
		item->vis = ITEM_GLOBAL;
	}

	item->type = parse_type(ctx);

	if (CUR.type != TOK_EQUAL) {
		if (item->vis == ITEM_GLOBAL)
			SYNTAX_ERROR("global '%.*s' has missing value", TOK_FMT(sym));

		item->vis = ITEM_FOREIGN;
		return item;
	}

	EAT(TOK_EQUAL);

	if (item->type.tag = TYPE_TYPE) {
		item->type = parse_type_decl(ctx);
	} else {
		item->val = parse_value(ctx);
	}


	return item;
}
	
void parse_toplevel(Context *ctx) {
	switch (CUR.type) {
	case TOK_SYMBOL:
		parse_item(ctx);
		break;

	case TOK_TYPE:
		EAT(TOK_TYPE);
		EAT(TOK_SYMBOL);
		parse_type_decl(ctx);
		break;

	default:
		SYNTAX_ERROR("unexpected top level statement: %s", token_string(CUR));
	}
}

void parse_module(Context *ctx) {
	while (CUR.type != TOK_EOF) {
		parse_toplevel(ctx);
	}
}

Module *parse(TokenArray toks) {
	Context ctx;
	ctx.toks = toks;
	ctx.toks_idx = 0;

	parse_module(&ctx);

	return ctx.mod;
}
