#ifndef AST_H
#define AST_H

#include "array.h"
#include "token.h"

typedef enum {
	TYPE_INFER,

	TYPE_UNIT,
	TYPE_SYMBOL,

	TYPE_POINTER,
	TYPE_SLICE,

	TYPE_TUPLE,
	TYPE_FUNCTION,
} ast_type_tag;

typedef struct ast_type ast_type;
ARRAY_DECL(type_list, ast_type);

struct ast_type {
	ast_type_tag tag;
	union {
		struct {
			ast_type *to;
		} ptr;

		struct {
			ast_type *of;
		} slc;

		struct {
			ast_type *in;
			ast_type *out;
		} fn;

		type_list tup;
	};
};

typedef enum {
	OPERATOR_INVALID,

	// order defines the precedence
	OPERATOR_ADDITION,
	OPERATOR_MULTIPLICATION,
	OPERATOR_APPLICATION,
} ast_operator;

char *operator_string(ast_operator op);

typedef enum {
	EXPR_NUMBER,
	EXPR_CHARACTER,
	EXPR_STRING,
	EXPR_SYMBOL,
	EXPR_BINARY_OP,
	EXPR_BLOCK,
	EXPR_TUPLE,
	EXPR_FUNCTION,
	EXPR_FOREIGN,
} ast_expr_tag;

typedef struct ast_expr ast_expr;
ARRAY_DECL(expr_list, ast_expr *);

struct ast_expr {
	ast_expr_tag tag;
	union {
		token num;

		token sym;

		struct {
			ast_operator op;
			ast_expr *lhs;
			ast_expr *rhs;
		} bin_op;

		struct {
			expr_list stmts;
		} blk;

		struct {
			expr_list vals;
		} tup;

		struct {
			ast_type *type;
			ast_expr *body;
		} fn;
	};
};

typedef enum {
	ITEM_VARIABLE,
} ast_item_tag;

typedef struct {
	ast_item_tag tag;
	union {
		struct {
			token name;
			ast_type *type;
			ast_expr *val;
		} var;
	};
} ast_item;

ARRAY_DECL(item_list, ast_item *);

typedef struct {
	item_list items;
} ast_root;

void print_ast(ast_root *ast);

#endif
