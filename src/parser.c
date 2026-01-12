#include "module.h"

#include "chunk_allocator.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SYNTAX_ERROR(...) do { \
	fprintf(stderr, "parse error: " __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	exit(EXIT_FAILURE); \
} while(0)

#define CUR (mod->toks.items[mod->toks_idx >= mod->toks.len ? mod->toks.len - 1 : mod->toks_idx])

#define EAT(expect) do { \
	if (CUR.tag != expect) { \
		SYNTAX_ERROR( \
			"expected %s, got %s (%d)", \
			token_string(expect), \
			token_string(CUR.tag), \
			mod->toks_idx \
		); \
	} \
	mod->toks_idx += 1; \
} while (0)

#define EAT_RET(expect) CUR; EAT(expect)
#define EAT_ANY (mod->toks_idx += 1)

ast_type *parse_type(module *mod) {
	ast_type *type = mem_alloc(&mod->alc, ast_type);

	switch (CUR.tag) {
	case TOK_SYMBOL:
		type->tag = TYPE_SYMBOL;
		EAT(TOK_SYMBOL);
		break;

	case TOK_STAR:
		type->tag = TYPE_POINTER;
		EAT(TOK_STAR);
		type->ptr.to = parse_type(mod);
		break;

	case TOK_BRACE_OPEN:
		type->tag = TYPE_SLICE;
		EAT(TOK_BRACKET_OPEN);
		EAT(TOK_BRACKET_CLOSE);
		type->slc.of = parse_type(mod);
		break;

	default:
		SYNTAX_ERROR("expected type token");
	}

	if (CUR.tag == TOK_ARROW) {
		ast_type *fn_type = mem_alloc(&mod->alc, ast_type);
		fn_type->tag = TYPE_FUNCTION;
		fn_type->fn.in = type;

		EAT(TOK_ARROW);

		fn_type->fn.out = parse_type(mod);

		return fn_type;
	}

	return type;
}

static ast_expr *parse_expression(module *mod);

static ast_expr *parse_block(module *mod) {
	ast_expr *expr = mem_alloc(&mod->alc, ast_expr);
	expr->tag = EXPR_BLOCK;

	array_init(&expr->blk.stmts, 16);

	EAT(TOK_BRACE_OPEN);

	while (CUR.tag != TOK_BRACE_CLOSE) {
		ast_expr *stmt_expr = parse_expression(mod);
		array_append(&expr->blk.stmts, &stmt_expr);

		if (CUR.tag != TOK_SEMICOLON) break;

		EAT(TOK_SEMICOLON);
	}

	EAT(TOK_BRACE_CLOSE);

	return expr;
}

static ast_expr *parse_tuple(module *mod) {
	ast_expr *expr = mem_alloc(&mod->alc, ast_expr);
	expr->tag = EXPR_TUPLE;

	array_init(&expr->tup.vals, 4);
	
	EAT(TOK_PAREN_OPEN);

	while (CUR.tag != TOK_PAREN_CLOSE) {
		ast_expr *val_expr = parse_expression(mod);
		array_append(&expr->blk.stmts, &val_expr);

		if (CUR.tag != TOK_COMMA) break;

		EAT(TOK_COMMA);
	}

	EAT(TOK_PAREN_CLOSE);

	return expr;
}

static ast_expr *parse_function_block(module *mod) {
	ast_expr *expr = mem_alloc(&mod->alc, ast_expr);
	expr->tag = EXPR_FUNCTION;

	EAT(TOK_FN);

	expr->fn.type = NULL;
	if (CUR.tag != TOK_BRACE_OPEN) {
		expr->fn.type = parse_type(mod);
	}

	expr->fn.body = parse_block(mod);

	return expr;
}

static ast_expr *parse_foreign(module *mod) {
	ast_expr *expr = mem_alloc(&mod->alc, ast_expr);
	expr->tag = EXPR_FOREIGN;

	EAT(TOK_FOREIGN);
	EAT(TOK_STRING);
	parse_type(mod);

	return expr;
}

static ast_expr *parse_symbol(module *mod) {
	ast_expr *expr = mem_alloc(&mod->alc, ast_expr);
	expr->tag = EXPR_SYMBOL;
	expr->sym = EAT_RET(TOK_SYMBOL);

	return expr;
}

static ast_expr *parse_number(module *mod) {
	ast_expr *expr = mem_alloc(&mod->alc, ast_expr);
	expr->tag = EXPR_NUMBER;
	expr->num = EAT_RET(TOK_NUMBER);

	return expr;
}

static ast_expr *parse_character(module *mod) {
	ast_expr *expr = mem_alloc(&mod->alc, ast_expr);
	expr->tag = EXPR_CHARACTER;
	expr->num = EAT_RET(TOK_CHARACTER);

	return expr;
}

static ast_expr *parse_string(module *mod) {
	ast_expr *expr = mem_alloc(&mod->alc, ast_expr);
	expr->tag = EXPR_STRING;
	expr->num = EAT_RET(TOK_STRING);

	return expr;
}

static bool cur_starts_expression(module *mod) {
	switch (CUR.tag) {
	case TOK_NUMBER:
	case TOK_CHARACTER:
	case TOK_STRING:
	case TOK_BRACE_OPEN:
	case TOK_PAREN_OPEN:
	case TOK_FN:
	case TOK_FOREIGN:
	case TOK_SYMBOL: return true;
	
	default: return false;
	}
}

static ast_expr *parse_primary_expression(module *mod) {
	switch (CUR.tag) {
	case TOK_FN: return parse_function_block(mod);
	case TOK_BRACE_OPEN: return parse_block(mod);
	case TOK_PAREN_OPEN: return parse_tuple(mod);
	case TOK_FOREIGN: return parse_foreign(mod);
	case TOK_SYMBOL: return parse_symbol(mod);
	case TOK_NUMBER: return parse_number(mod);
	case TOK_CHARACTER: return parse_character(mod);
	case TOK_STRING: return parse_string(mod);
	default:
		SYNTAX_ERROR("expected expression, got %s", token_string(CUR.tag));
	}
}

static ast_operator parse_operator(module *mod) {
	switch (CUR.tag) {
	case TOK_PLUS:
	case TOK_DASH: return OPERATOR_ADDITION;

	case TOK_STAR:
	case TOK_SLASH: return OPERATOR_MULTIPLICATION;

	default: return OPERATOR_INVALID;
	}
}

static ast_expr *parse_expression_prec(module *mod, uint32 prec) {
	ast_expr *lhs = parse_primary_expression(mod);

    for (;;) {
		// check application
		if (cur_starts_expression(mod)) {
			// a + f x + c
			//      ^ f(x)
			
			if (OPERATOR_APPLICATION <= prec) break;

			ast_expr *op_expr = mem_alloc(&mod->alc, ast_expr);
			op_expr->tag = EXPR_BINARY_OP;
			op_expr->bin_op.op = OPERATOR_APPLICATION;
			op_expr->bin_op.lhs = lhs;
			op_expr->bin_op.rhs = parse_expression_prec(mod, OPERATOR_APPLICATION);

			lhs = op_expr;

			continue;
		}

        ast_operator op = parse_operator(mod);

        // no operator
        if (op == OPERATOR_INVALID) break;

        // if next operator doesn't exceed current precedence
        // break out and wrap expression so far
        // 1 + 2 + 3 * 4
        //       ^ has same precedence so wrap previous expression (1 + 2)
        if (op <= prec) break;

        EAT_ANY; // eat operator

        ast_expr *rhs = parse_expression_prec(mod, op);

        ast_expr *op_expr = mem_alloc(&mod->alc, ast_expr);
        op_expr->tag = EXPR_BINARY_OP;
        op_expr->bin_op.op = op;
        op_expr->bin_op.lhs = lhs;
        op_expr->bin_op.rhs = rhs;

        lhs = op_expr;
    }

	return lhs;
}

static ast_expr *parse_expression(module *mod) {
	return parse_expression_prec(mod, 0);
}

static void parse_variable(module *mod, ast_item *item) {
	item->tag = ITEM_VARIABLE;

	EAT(TOK_LET);
	item->var.name = EAT_RET(TOK_SYMBOL);
	
	// type
	item->var.type = NULL;
	if (CUR.tag == TOK_COLON) {
		EAT(TOK_COLON);
		item->var.type = parse_type(mod);
	}

	// value
	if (CUR.tag == TOK_EQUAL) {
		EAT(TOK_EQUAL);
		item->var.val = parse_expression(mod);
	}
}

static ast_item *parse_item(module *mod) {
	ast_item *item = mem_alloc(&mod->alc, ast_item);

	switch (CUR.tag) {
	case TOK_LET: parse_variable(mod, item); break;
	default: SYNTAX_ERROR("expected top level item, got %s", token_string(CUR.tag));
	}

	return item;
}

void parse(module *mod) {
	mod->toks_idx = 0;

	array_init(&mod->ast.items, 32);

	while (CUR.tag != TOK_EOF) {
		ast_item *item = parse_item(mod);
		array_append(&mod->ast.items, &item);
	}
}
