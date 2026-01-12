#include "ast.h"

#include <stdio.h>

static void print_expression(ast_expr *expr) {
	printf("(");
	switch (expr->tag) {
	case EXPR_NUMBER:
		printf("num");
		break;

	case EXPR_CHARACTER:
		printf("char");
		break;

	case EXPR_STRING:
		printf("str");
		break;

	case EXPR_SYMBOL:
		printf("symbol");
		break;

	case EXPR_BLOCK:
		printf("block");
		for (int i = 0; i < expr->blk.stmts.len; i++) {
			printf(" ");
			print_expression(expr->blk.stmts.items[i]);
		}
		break;

	case EXPR_TUPLE:
		printf("tuple");
		for (int i = 0; i < expr->tup.vals.len; i++) {
			printf(" ");
			print_expression(expr->tup.vals.items[i]);
		}
		break;

	case EXPR_FUNCTION:
		printf("fn ");
		print_expression(expr->fn.body);
		break;

	case EXPR_FOREIGN:
		printf("foreign fn");
		break;

	case EXPR_BINARY_OP:
		printf("%s ", operator_string(expr->bin_op.op));
		print_expression(expr->bin_op.lhs);
		printf(" ");
		print_expression(expr->bin_op.rhs);
		break;

	default:
		printf("unhandled expression");
	}

	printf(")");
}

static void print_item(ast_item *item) {
	switch (item->tag) {
	case ITEM_VARIABLE:
		printf("<var>\n");
		print_expression(item->var.val);
		printf("\n");
		break;
	}
}

void print_ast(ast_root *ast) {
	for (int i = 0; i < ast->items.len; i++) {
		print_item(ast->items.items[i]);
	}
}

char *operator_string(ast_operator op) {
	switch (op) {
	case OPERATOR_INVALID: return "invalid";
	case OPERATOR_ADDITION: return "+";
	case OPERATOR_MULTIPLICATION: return "*";
	case OPERATOR_APPLICATION: return "$";
	}
}

