#ifndef AST_H
#define AST_H

#include "array.h"
#include "token.h"

typedef enum {
	TYPE_UNIT,
	TYPE_POINTER,
	TYPE_SLICE,
	TYPE_SYMBOL,
	TYPE_INFER,
} ast_type_tag;

typedef struct ast_type ast_type;
struct ast_type {
	ast_type_tag tag;
	union {
		struct {
			ast_type *to;
		} ptr;

		struct {
			ast_type *of;
		} slc;
	};
};

typedef enum {
	PATTERN_SYMBOL,
} ast_pattern_tag;

typedef struct {
	ast_pattern_tag tag;
	union {
		struct {
			token tok;
		} symbol;
	};
} ast_pattern;

ARRAY_DECL(pattern_list, ast_pattern);

typedef enum {
	EXPR_NUMBER,
	EXPR_SYMBOL,
	EXPR_BINARY_OP,
	EXPR_BLOCK,
} ast_expr_tag;

typedef struct ast_expr ast_expr;
ARRAY_DECL(expr_list, ast_expr *);

struct ast_expr {
	ast_expr_tag tag;
	union {
		union {
			double float_;
			uint64 int_;
		} number;

		struct {
			ast_expr *lhs;
			ast_expr *rhs;
		} bin_op;

		struct {
			ast_expr *in;
			ast_expr *out;
			expr_list *body;
		} blk;
	};
};

typedef enum {
	ITEM_VARIABLE,
} ast_item_tag;

typedef struct {
	ast_item_tag tag;
	union {
		struct {
			ast_expr *type;
			ast_expr *val;
		} var;
	};
} ast_item;

#endif
