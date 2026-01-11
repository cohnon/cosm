#include "ast.h"

#include <stdio.h>

static void print_expression(ast_expr *expr) {
	switch (expr->tag) {
	case EXPR_BLOCK:
		printf("<block>\n");
		break;

	case EXPR_FUNCTION:
		printf("<function>\n");
		break;

	case EXPR_FOREIGN:
		printf("<foreign function>\n");
		break;

	default:
		printf("<unhandled expression>\n");
	}
}

static void print_item(ast_item *item) {
	switch (item->tag) {
	case ITEM_VARIABLE:
		printf("<var>\n");
		print_expression(item->var.val);
		break;
	}
}

void print_ast(ast_root *ast) {
	for (int i = 0; i < ast->items.len; i++) {
		print_item(ast->items.items[i]);
	}
}
