#ifndef AST_H
#define AST_H

#include "array.h"

typedef enum TypeTag {
	TYPE_UNIT,

	TYPE_I8,
	TYPE_I16,
	TYPE_I32,
	TYPE_I64,

	TYPE_PTR,
	TYPE_SLC,

	TYPE_FN,
	TYPE_TYPE,
} TypeTag;

typedef struct Type Type;

typedef struct TypePtr {
	Type *to;
} TypePtr;

typedef struct TypeSlc {
	Type *of;
} TypeSlc;

typedef struct TypeFn {
	Type *in, *out;
} TypeFn;

struct Type {
	TypeTag tag;

	union {
		TypePtr ptr;
		TypeSlc slc;
		TypeFn fn;
	};
};

typedef enum InsnTag {
	INSN_ADD_I32,
	INSN_CONST_I32,

	INSN_CALL,
	INSN_RETURN,
} InsnTag;

typedef struct Insn {
	InsnTag tag;
} Insn;

ARRAY_DECL(InsnList, Insn);

typedef struct Value {
	union {
		double flt;
		long intg;
	};
} Value;

typedef struct Function {
	InsnList insns;
} Function;

typedef enum Visibility {
	ITEM_LOCAL,
	ITEM_GLOBAL,
	ITEM_FOREIGN,
} Visibility;

typedef enum ItemTag {
	ITEM_TYPE_DECL,
	ITEM_VALUE,
} ItemTag;

typedef struct Item {
	ItemTag tag;
	Visibility vis;

	union {
		Value *val;
		Type *type_decl;
	};
} Item;

ARRAY_DECL(ItemList, Item);

#endif
