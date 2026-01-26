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

	case EXPR_BINARY_OP:
		printf("%s ", operator_string(expr->bin_op.op));
		print_expression(expr->bin_op.lhs);
		printf(" ");
		print_expression(expr->bin_op.rhs);
		break;

	case EXPR_CALL:
		printf("call ");
		print_expression(expr->call.callee);
		printf(" ");
		expr_list *args = &expr->call.args;
		for (int i = 0; i < args->len; i++) {
			print_expression(args->items[i]);
			if (i < args->len - 1) { printf(", "); }
		}
		break;

	default:
		printf("unhandled expression %d\n", expr->tag);
	}

	printf(")");
}

static void print_item(ast_item *item) {
	switch (item->tag) {
	case ITEM_FUNCTION:
		printf("<func>\n");
		if (item->func.body != NULL) {
			expr_list *stmts = &item->func.body->blk.stmts;
			for (int i = 0; i < stmts->len; i++) {
				print_expression(stmts->items[i]);
			}
		} else {
			printf("foreign");
		}
		printf("\n");
		break;

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
	default: return "<invalid operator>";
	}
}

