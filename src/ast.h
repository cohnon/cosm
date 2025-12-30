#ifndef AST_H
#define AST_H

typedef enum {
	TYPE_UNIT,
} ast_type_tag;

typedef struct {
	ast_type_tag tag;
} ast_type;

typedef struct {
	ast_type type;
} ast_variable;

typedef enum {
	ITEM_VARIABLE,
	ITEM_TYPE,
} ast_item_tag;

typedef struct {
	ast_item_tag tag;
	union {
		ast_variable var;
	};
} ast_item;

#endif
