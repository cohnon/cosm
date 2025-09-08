#include "parser.h"
#include "lexer.h"

#include <assert.h>
#include <stdint.h>

#define ITEM_NAME_MAX_LEN ((1<<7) - 1)

typedef enum {
    TYPE_I8,
    TYPE_I16,
    TYPE_I32,
    TYPE_I64,

    TYPE_WORD,
    TYPE_HALF,

    TYPE_F32,
    TYPE_F64,

    TYPE_REF,

    TYPE_USER,
} TypeKind;

typedef struct {
    TypeKind kind;
    int elems;
} Type;

typedef struct {
    char name[ITEM_NAME_MAX_LEN];

    Type *ins;
    Type *outs;

    int ins_len;
    int outs_len;
} FuncSig;

typedef struct {
    char name[ITEM_NAME_MAX_LEN];
    Type type;
} Variable;

typedef struct {
    Type *types;
    int types_len;

    FuncSig *funcs;
    int funcs_len;

    Variable *vars;
    int vars_len;
} Context;

void parse(TokenArray toks) {
    (void)toks;
}
