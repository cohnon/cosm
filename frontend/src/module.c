#include "module.h"
#include <stdlib.h>

void print_type(FILE *out, Type *type) {
    switch (type->kind) {
    case TYPE_UNIT:
        fprintf(out, "unit");
        break;

    case TYPE_I8:
        fprintf(out, "i8");
        break;

    case TYPE_I32:
        fprintf(out, "i32");
        break;

    case TYPE_WORD:
        fprintf(out, "word");
        break;

    case TYPE_ARRAY:
        fprintf(out, "[%ld x ", type->arr.len);
        print_type(out, type->arr.ty);
        fprintf(out, "]");
        break;

    case TYPE_PTR:
        fprintf(out, "*");
        print_type(out, type->ptr);
        break;

    case TYPE_FUNC:
        fprintf(out, "fn ");
        print_type(out, type->func.in);
        fprintf(out, " -> ");
        print_type(out, type->func.out);
        break;

    case TYPE_TUP:
        fprintf(out, "(");
        for (int i = 0; i < type->fields.len; i++) {
            print_type(out, type->fields.items[i]);

            if (i + 1 < type->fields.len) {
                fprintf(out, ", ");
            }
        }
        fprintf(out, ")");
        break;

    default:
        fprintf(out, "<unhandled type>");
        break;
    }
}

long parse_number(Token tok)
{
    long x = 0;
    for (size_t i = 0; i < tok.len; i++) {
        x = x * 10 + (tok.src[i] - '0');
    }

    return x;
}

void print_value(FILE *out, Value *val) {
    switch (val->type) {
    case VAL_SYMBOL:
    case VAL_VARIABLE:
        fprintf(out, "%.*s", TOK_FMT(val->tok));
        break;

    case VAL_NUMBER:
        fprintf(out, "%ld", parse_number(val->tok));
        break;

    case VAL_TUPLE:
        fprintf(out, "(");
        for (int i = 0; i < val->tup.len; i++) {
            print_value(out, val->tup.items[i]);
            if (i + 1 < val->tup.len) {
                fprintf(out, ", ");
            }
        }
        fprintf(out, ")");
        break;

    case VAL_REF:
        fprintf(out, "ref ");
        print_value(out, val->val);
        break;

    default:
        fprintf(out, "<unhandled value>");
        break;
    }
}

void print_block(FILE *out, Block *blk) {
    for (int i = 0; i < blk->instrs.len; i++) {
        fprintf(out, "    ");

        Instr *instr = blk->instrs.items[i];
        if (instr->var != NULL) {
            print_value(out, instr->var);
            fprintf(out, " = ");
        }

        switch (instr->type) {
        case INSTR_ADD:
            fprintf(out, "add ");
            print_value(out, instr->val);
            break;

        case INSTR_IMM:
            fprintf(out, "imm ");
            print_value(out, instr->val);
            break;

        case INSTR_CALL:
            fprintf(out, "call %.*s ", TOK_FMT(instr->call.target));
            print_value(out, instr->call.args);
            break;

        case INSTR_RET:
            fprintf(out, "ret");
            if (instr->val != NULL) {
                fprintf(out, " ");
                print_value(out, instr->val);
            }
            break;

        default:
            fprintf(out, "<unhandled instr>");
            break;
        }

        fprintf(out, "\n");
    }
}

void print_function(FILE *out, Item *item) {
    for (int i = 0; i < item->func.blks.len; i++) {
        Block *blk = item->func.blks.items[i];
        if (i != 0) {
            fprintf(out, "blk.%d\n", i);
        }
        print_block(out, blk);
    }
}

void print_item(FILE *out, Item *item) {
    fprintf(out, "%.*s ", (int)item->sym_len, item->sym);

    switch (item->vis) {
    case ITEM_VIS_GLOBAL:
        fprintf(out, "global ");
        break;

    case ITEM_VIS_FOREIGN:
        fprintf(out, "foreign ");
        break;

    case ITEM_VIS_LOCAL:
        break;
    }

    print_type(out, item->ty);

    if (item->vis == ITEM_VIS_FOREIGN) {
        fprintf(out, "\n");
        return;
    }

    if (item->ty->kind == TYPE_FUNC) {
        fprintf(out, "\n");
        print_function(out, item);
        fprintf(out, "end\n");
    } else {
        fprintf(out, " ");
        print_value(out, item->obj);
        fprintf(out, "\n");
    }
}

void print_module(FILE *out, Module *mod) {
    for (int i = 0; i < mod->items.len; i++) {
        Item *item = mod->items.items[i];
        print_item(out, item);
        if (i + 1 < mod->items.len) {
            fprintf(out, "\n");
        }
    }
}
