#include "parser.h"
#include "array.h"
#include "lexer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    Module    *mod;
    size_t     toks_idx;
    TokenArray toks;
} Parser;

#define CUR (psr->toks.items[psr->toks_idx])

#define EAT(expect) do {                               \
    if (CUR.type != expect) {                          \
        fprintf(stderr, "expected %s, got %s (%ld)\n", \
                token_string_from_type(expect),        \
                token_string(CUR),                     \
                psr->toks_idx);                        \
        exit(EXIT_FAILURE);                            \
    }                                                  \
    psr->toks_idx += 1;                                \
} while (0)

#define EAT_ANY (psr->toks_idx += 1)

_Bool is_type(Parser *psr) {
    switch (CUR.type) {
    case TOK_I8:
    case TOK_I32:
        return 1;

    default:
        return 0;
    }
}

Type *parse_type(Parser *psr) {
    Type *ty = malloc(sizeof(Type));

    switch (CUR.type) {
    case TOK_UNIT:
        ty->kind = TYPE_UNIT;
        EAT(TOK_UNIT);
        break;

    case TOK_I8:
        ty->kind = TYPE_I8;
        EAT(TOK_I8);
        break;

    case TOK_I32:
        ty->kind = TYPE_I32;
        EAT(TOK_I32);
        break;

    case TOK_WORD:
        ty->kind = TYPE_WORD;
        EAT(TOK_WORD);
        break;

    case TOK_STAR:
        ty->kind = TYPE_PTR;
        EAT(TOK_STAR);
        ty->ptr = parse_type(psr);
        break;

    case TOK_FN:
        ty->kind = TYPE_FUNC;
        EAT(TOK_FN);
        ty->func.in = parse_type(psr);
        EAT(TOK_ARROW);
        ty->func.out = parse_type(psr);
        break;

    case TOK_BRACKET_OPEN:
        ty->kind = TYPE_ARRAY;
        EAT(TOK_BRACKET_OPEN);

        Token num_tok = CUR;
        EAT(TOK_NUMBER);
        char *end;
        ty->arr.len = strtol(num_tok.src, &end, 10);

        EAT(TOK_X);

        ty->arr.ty = parse_type(psr);
        EAT(TOK_BRACKET_CLOSE);
        break;

    case TOK_PAREN_OPEN:
        ty->kind = TYPE_TUP;
        array_init(&ty->fields, 4);
        EAT(TOK_PAREN_OPEN);
        for (;;) {
            Type *field = parse_type(psr);
            array_append(&ty->fields, &field);

            if (CUR.type != TOK_COMMA) {
                break;
            }
            EAT(TOK_COMMA);
        }
        EAT(TOK_PAREN_CLOSE);
        assert(ty->fields.len > 1);
        break;

    default:
        fprintf(stderr, "expected type, got %s\n", token_string_from_type(CUR.type));
        exit(EXIT_FAILURE);
    }

    return ty;
}

Value *parse_value(Parser *psr) {
    Value *val = malloc(sizeof(Value));

    switch (CUR.type) {
    case TOK_NUMBER:
        val->type = VAL_NUMBER;
        val->tok = CUR;
        EAT(TOK_NUMBER);
        break;

    case TOK_STRING:
        val->type = VAL_STRING;
        val->tok = CUR;
        EAT(TOK_STRING);
        break;

    case TOK_SYMBOL:
        val->type = VAL_SYMBOL;
        val->tok = CUR;
        EAT(TOK_SYMBOL);
        break;

    case TOK_VARIABLE:
        val->type = VAL_VARIABLE;
        val->tok = CUR;
        EAT(TOK_VARIABLE);
        break;

    case TOK_REF:
        val->type = VAL_REF;
        EAT(TOK_REF);
        val->val = parse_value(psr);
        break;

    case TOK_PAREN_OPEN:
        EAT(TOK_PAREN_OPEN);
        val->type = VAL_TUPLE;
        array_init(&val->tup, 4);
        for (;;) {
            Value *v = parse_value(psr);
            array_append(&val->tup, &v);
            if (CUR.type != TOK_COMMA) {
                break;
            }
            EAT(TOK_COMMA);
        }
        EAT(TOK_PAREN_CLOSE);
        assert(val->tup.len > 1);
        break;

    default:
        fprintf(stderr, "unhandled value\n");
        break;
    }

    return val;
}

Instr *parse_instruction(Parser *psr);
void parse_add(Parser *psr, Instr *instr) {
    EAT(TOK_ADD);

    instr->type = INSTR_ADD;
    instr->add.lhs = parse_value(psr);
    instr->add.rhs = parse_value(psr);
}

void parse_imm(Parser *psr, Instr *instr) {
    EAT(TOK_IMM);

    instr->type = INSTR_IMM;
    instr->ty = parse_type(psr);
    instr->val = parse_value(psr);
}

void parse_call(Parser *psr, Instr *instr) {
    EAT(TOK_CALL);

    instr->type = INSTR_CALL;
    instr->call.target = CUR;
    EAT(TOK_SYMBOL);
    instr->call.args = parse_value(psr);
}

void parse_ret(Parser *psr, Instr *instr) {
    EAT(TOK_RET);

    instr->type = INSTR_RET;
    instr->val = NULL;
    if (CUR.type == TOK_LABEL || CUR.type == TOK_END) {
        return;
    }

    instr->val = parse_value(psr);
}

Instr *parse_instruction(Parser *psr) {
    Instr *instr = malloc(sizeof(Instr));

    if (CUR.type == TOK_SYMBOL || CUR.type == TOK_VARIABLE) {
        instr->var = parse_value(psr);
        EAT(TOK_EQUAL);
    }

    switch (CUR.type) {
    case TOK_ADD: parse_add(psr, instr); break;
    case TOK_IMM: parse_imm(psr, instr); break;
    case TOK_CALL: parse_call(psr, instr); break;
    case TOK_RET: parse_ret(psr, instr); break;
    default: return NULL;
    }

    return instr;
}

Block *parse_block(Parser *psr, _Bool entry) {
    Block *blk = malloc(sizeof(Block));
    array_init(&blk->instrs, 8);

    if (!entry) {
        if (CUR.type != TOK_LABEL) {
            fprintf(stderr, "block has missing label (got %s)\n", token_string(CUR));
            exit(EXIT_FAILURE);
        }

        EAT(TOK_LABEL);
    }

    for (;;) {
        Instr *instr = parse_instruction(psr);
        if (instr == NULL) break;

        array_append(&blk->instrs, &instr);
    }

    return blk;
}

Item *parse_item(Parser *psr) {
    Item *item = malloc(sizeof(Item));
    item->sym = CUR.src;
    item->sym_len = CUR.len;

    EAT(TOK_SYMBOL);

    switch (CUR.type) {
    case TOK_GLOBAL:
        EAT(TOK_GLOBAL);
        item->vis = ITEM_VIS_GLOBAL;
        break;

    case TOK_FOREIGN:
        EAT(TOK_FOREIGN);
        item->vis = ITEM_VIS_FOREIGN;
        break;

    default:
        item->vis = ITEM_VIS_LOCAL;
        break;
    }

    item->ty = parse_type(psr);

    if (item->vis == ITEM_VIS_FOREIGN) {
        return item;
    }

    if (item->ty->kind == TYPE_FUNC) {
        array_init(&item->func.blks, 8);

        Block *entry = parse_block(psr, 1);
        array_append(&item->func.blks, &entry);
        
        while (CUR.type != TOK_END) {
            Block *blk = parse_block(psr, 0);
            array_append(&item->func.blks, &blk);
        }

        EAT(TOK_END);
    } else {
        item->obj = parse_value(psr);
    }

    return item;
}

void parse_toplevel(Parser *psr) {
    switch (CUR.type) {
    case TOK_SYMBOL: {
        Item *item = parse_item(psr);
        Item **found = map_get(&psr->mod->item_map, item->sym, item->sym_len);

        if (found != NULL) {
            fprintf(stderr, "error: duplicate item name %.*s\n", (int)item->sym_len, item->sym);
            exit(EXIT_FAILURE);
        }
        array_append(&psr->mod->items, &item);
        map_insert(&psr->mod->item_map, item->sym, item->sym_len, &item);
        break;
    }

    default:
        fprintf(stderr, "unexpected item: %s\n", token_string(CUR));
        exit(EXIT_FAILURE);
    }
}

void parse_module(Parser *psr) {
    psr->mod = malloc(sizeof(Module));
    array_init(&psr->mod->types, 8);
    array_init(&psr->mod->items, 8);
    map_init(&psr->mod->type_map, 16);
    map_init(&psr->mod->item_map, 16);

    while (CUR.type != TOK_EOF) {
        parse_toplevel(psr);
    }
}

Module *parse(TokenArray toks) {
    Parser psr;
    psr.toks     = toks;
    psr.toks_idx = 0;

    parse_module(&psr);

    return psr.mod;
}
