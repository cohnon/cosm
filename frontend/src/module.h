#ifndef MODULE_H
#define MODULE_H

#include "cosm.h"
#include "array.h"
#include "map.h"
#include "lexer.h"
#include <stdio.h>

// === TYPES ===
typedef enum Type {
    TYPE_UNIT,

    TYPE_I8,
    TYPE_I16,
    TYPE_I32,
    TYPE_I64,

    TYPE_WORD,
    TYPE_HALF,

    TYPE_F32,
    TYPE_F64,

    TYPE_PTR,
    TYPE_ARRAY,
    TYPE_SLICE,
    TYPE_FUNC,

    TYPE_TUP,
    TYPE_UNION,
} Type;
ARRAY_DECL(TypeArray, Type);

TUP (I8, I32, (I8, I32))

CosmIdx type_new_tuple(
Type type_get(CosmIdx idx);

/// === VALUES ===
typedef enum ValueType {
    VAL_NUMBER,
    VAL_STRING,
    VAL_SYMBOL,
    VAL_VARIABLE,
    VAL_TUPLE,
    VAL_REF,
} ValueType;

typedef struct Value Value;
ARRAY_DECL(ValueArray, Value*);

struct Value {
    ValueType type;
    union {
        Token tok;
        ValueArray tup;
        Value *val;
    };
};

typedef enum InstrType {
    INSTR_ADD,
    INSTR_IMM,
    INSTR_CALL,
    INSTR_RET,
} InstrType;

typedef struct Add {
    Value *lhs;
    Value *rhs;
} Add;

typedef struct Call {
    Token target;
    Value *args;
} Call;

typedef struct Instr Instr;
struct Instr {
    InstrType type;
    Value *var;
    Type *ty;

    union {
        Add add;
        Call call;
        Value *val;
    };
};
ARRAY_DECL(InstrArray, Instr*);

typedef struct Block {
    InstrArray instrs;
} Block;
ARRAY_DECL(BlockArray, Block*);

typedef enum ItemType {
    ITEM_TYPE,
    ITEM_FUNC,
    ITEM_OBJ,
} ItemType;

typedef struct Func {
    BlockArray blks;
} Func;

typedef enum ItemVisibility {
    ITEM_VIS_LOCAL,
    ITEM_VIS_GLOBAL,
    ITEM_VIS_FOREIGN,
} ItemVisibility;

typedef struct Item {
    Type *ty;

    char  *sym;
    size_t sym_len;

    int idx;

    ItemVisibility vis;

    union {
        Func   func;
        Value *obj;
    };
} Item;

typedef struct Import {
    CosmWord mod_name;
    CosmWord item_name;
    CosmIdx item_type;
} Import;
ARRAY_DECL(ImportArray, Import);

typedef struct Export {
    CosmWord mod_name;
    CosmIdx item_type;
} Export;
ARRAY_DECL(ExportArray, Export);

typedef struct Function {
    CosmWord type;
} Function;
ARRAY_DECL(FunctionArray, Function);

ARRAY_DECL(ByteArray, unsigned char);
typedef struct Module {
    TypeArray types;
} Module;

void print_module(FILE *out, Module *mod);

#endif
