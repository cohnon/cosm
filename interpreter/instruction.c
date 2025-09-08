#include "fir.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

bool fir_instr_is_terminator(fir_Instr *instr) {
    return FIR_INSTR_JMP <= instr->kind && instr->kind <= FIR_INSTR_RET;
}

static fir_Instr *create_instr(
    fir_Block *block, fir_InstrKind kind, fir_DataType type, size_t size
) {
    fir_Module *module = block->func->module;

    fir_Instr *instr = fir_arena_alloc(&module->arena, size);
    instr->kind = kind;
    instr->type = type;

    fir_array_append(fir_Instr *, &block->instrs, instr);

    return instr;
}

static fir_Instr *create_instr1(
    fir_Block *block,
    fir_InstrKind kind,
    fir_DataType type,
    fir_Instr *arg
) {
    assert(block != NULL);
    assert(arg != NULL);

    size_t instr_size = sizeof(fir_Instr) + sizeof(fir_Instr *);

    fir_Instr *instr = create_instr(block, kind, type, instr_size);

    fir_Instr **args = (fir_Instr **)instr->data;
    args[0] = arg;

    return instr;
}


static fir_Instr *create_instr2(
    fir_Block *block,
    fir_InstrKind kind,
    fir_DataType type,
    fir_Instr *arg0,
    fir_Instr *arg1
) {
    assert(block != NULL);
    assert(arg0 != NULL);
    assert(arg1 != NULL);

    size_t instr_size = sizeof(fir_Instr) + (2 * sizeof(fir_Instr *));

    fir_Instr *instr = create_instr(block, kind, type, instr_size);

    fir_Instr **args = (fir_Instr **)instr->data;
    args[0] = arg0;
    args[1] = arg1;

    return instr;
}

static fir_Instr *create_op_instr(
    fir_Block *block, fir_InstrKind kind, fir_Instr *arg0, fir_Instr *arg1
) {
    assert(fir_type_eq(arg0->type, arg1->type));

    return create_instr2(block, kind, arg0->type, arg0, arg1);
}

static fir_Instr *create_lit_instr(fir_Block *block, fir_DataType type) {
    assert(block != NULL);

    size_t instr_size = sizeof(fir_Instr) + sizeof(fir_InstrLit);

    fir_Instr *instr = create_instr(block, FIR_INSTR_LIT, type, instr_size);

    return instr;
}

fir_Instr *fir_instr_lit_int(
    fir_Block *block,
    fir_DataType type,
    uint64_t val
) {
    assert(type.kind == FIR_TYPE_INT);

    fir_Instr *instr = create_lit_instr(block, type);

    fir_InstrLit *lit = (fir_InstrLit *)instr->data;
    lit->i = val;

    return instr;
}

fir_Instr *fir_instr_lit_float(
    fir_Block *block,
    fir_DataType type,
    double val
) {
    assert(type.kind == FIR_TYPE_FLOAT);

    fir_Instr *instr = create_lit_instr(block, type);

    fir_InstrLit *lit = (fir_InstrLit *)instr->data;
    lit->f = val;

    return instr;
}

fir_Instr *fir_instr_arg(fir_Block *block, size_t idx) {
    assert(block != NULL);
    assert(idx < block->input.len);

    fir_DataType type = *fir_array_get(fir_DataType, &block->input, idx);
    size_t instr_size = sizeof(fir_Instr) + sizeof(size_t);

    fir_Instr *instr = create_instr(block, FIR_INSTR_ARG, type, instr_size);
    
    *(size_t *)instr->data = idx;

    return instr;
}

fir_Instr *fir_instr_add(fir_Block *block, fir_Instr *lhs, fir_Instr *rhs) {
    return create_op_instr(block, FIR_INSTR_ADD, lhs, rhs);
}

fir_Instr *fir_instr_sub(fir_Block *block, fir_Instr *lhs, fir_Instr *rhs) {
    return create_op_instr(block, FIR_INSTR_SUB, lhs, rhs);
}

fir_Instr *fir_instr_mul(fir_Block *block, fir_Instr *lhs, fir_Instr *rhs) {
    return create_op_instr(block, FIR_INSTR_MUL, lhs, rhs);
}

fir_Instr *fir_instr_stack(fir_Block *block, fir_Instr *size) {
    assert(size != NULL);
    assert(size->type.kind == FIR_TYPE_INT);

    return create_instr1(block, FIR_INSTR_STACK, fir_type_ptr(), size);
}

fir_Instr *fir_instr_offset(fir_Block *block, fir_Instr *ptr, fir_Instr *by) {
    assert(ptr != NULL);
    assert(ptr->type.kind == FIR_TYPE_PTR);

    return create_instr2(block, FIR_INSTR_OFFSET, fir_type_ptr(), ptr, by);
}

void fir_instr_write(fir_Block *block, fir_Instr *to, fir_Instr *from) {
    assert(to != NULL);
    assert(to->type.kind == FIR_TYPE_PTR);

    create_instr2(block, FIR_INSTR_WRITE, fir_type_void(), to, from);
}

fir_Instr *fir_instr_read(fir_Block *block, fir_DataType type, fir_Instr *from) {
    assert(from != NULL);
    assert(from->type.kind == FIR_TYPE_PTR);

    return create_instr1(block, FIR_INSTR_READ, type, from);
}

fir_Instr *fir_instr_ret(fir_Block *block) {
    assert(block != NULL);

    size_t args_n = block->func->output.len;
    size_t instr_size =
        sizeof(fir_Instr) +
        sizeof(fir_InstrRet) +
        (args_n * sizeof(fir_Instr *));

    fir_Instr *instr =
        create_instr(block, FIR_INSTR_RET, fir_type_void(), instr_size);

    fir_InstrRet *ret = (fir_InstrRet *)instr->data;
    ret->args_n = args_n;

    fir_Instr **args = (fir_Instr **)ret->args;
    for (size_t i = 0; i < ret->args_n; i++) {
        args[i] = NULL;
    }

    return instr;
}

void fir_instr_ret_arg(fir_Instr *instr, fir_Instr *arg) {
    assert(instr != NULL);
    assert(arg != NULL);
    assert(instr->kind == FIR_INSTR_RET);

    fir_InstrRet *ret = (fir_InstrRet *)instr->data;
    fir_Instr **args = (fir_Instr **)ret->args;
    
    size_t i = 0;
    for (; i < ret->args_n; i++) {
        if (args[i] == NULL) {
            args[i] = arg;
            break;
        }
    }

    assert(i < ret->args_n && "passing in too many args to ret");
}

fir_Instr *fir_instr_call(fir_Block *block, fir_Function *func) {
    assert(block != NULL);
    assert(func != NULL);

    fir_DataType type;
    if (func->output.len == 0) {
        type = fir_type_void();
    } else if (func->output.len == 1) {
        type = *fir_array_get(fir_DataType, &func->output, 0);
    } else {
        type = fir_type_tuple(func->output.len);
    }

    size_t instr_size =
        sizeof(fir_Instr) +
        sizeof(fir_InstrCall) +
        (func->input.len * sizeof(fir_Instr *));

    fir_Instr *instr = create_instr(block, FIR_INSTR_CALL, type, instr_size);

    fir_InstrCall *call = (fir_InstrCall *)instr->data;
    call->func = func;

    fir_Instr **args = (fir_Instr **)call->args;

    for (size_t i = 0; i < func->input.len; i++) {
        args[i] = NULL;
    }

    return instr;
}

void fir_instr_call_arg(fir_Instr *instr, fir_Instr *arg) {
    assert(instr != NULL);
    assert(arg != NULL);
    assert(instr->kind == FIR_INSTR_CALL);

    fir_InstrCall *call = (fir_InstrCall *)instr->data;
    fir_Instr **args = (fir_Instr **)call->args;

    size_t i = 0;
    for (; i < call->func->input.len; i++) {
        if (args[i] == NULL) {
            fir_DataType type = *fir_array_get(fir_DataType, &call->func->input, i);
            assert(fir_type_eq(arg->type, type));

            args[i] = arg;
            break;
        }
    }

    assert(i < call->func->input.len && "passing in too many args to call");
}

fir_Instr *fir_instr_proj(fir_Block *block, fir_Instr *tuple, size_t idx) {
    assert(block != NULL);
    assert(tuple != NULL);
    assert(idx < tuple->type.data);
    assert(tuple->kind == FIR_INSTR_CALL);

    fir_InstrCall *call = (fir_InstrCall *)tuple->data;
    fir_DataType type = *fir_array_get(fir_DataType, &call->func->output, idx);
    size_t instr_size = sizeof(fir_Instr) + sizeof(fir_InstrProj);

    fir_Instr *instr = create_instr(block, FIR_INSTR_PROJ, type, instr_size);

    fir_InstrProj *proj = (fir_InstrProj *)instr->data;
    proj->instr = tuple;
    proj->idx = idx;

    return instr;
}

void fir_instr_dump(fir_Instr *instr, FILE *fp) {
    assert(instr != NULL);
    assert(fp != NULL);

    switch (instr->kind) {
    case FIR_INSTR_LIT: {
        fprintf(fp, "lit");

        fir_InstrLit *lit = (fir_InstrLit *)instr->data;
        switch (instr->type.kind) {
        case FIR_TYPE_INT:
            fprintf(fp, " %" PRId64, lit->i);
            break;
        case FIR_TYPE_FLOAT:
            fprintf(fp, " %g", lit->f);
            break;
        case FIR_TYPE_PTR:
            __builtin_unreachable();
        case FIR_TYPE_TUPLE:
            __builtin_unreachable();
        }
        break;
    }
    case FIR_INSTR_ARG: {
        // arg instructions are implicit
        break;
    }
    case FIR_INSTR_ADD: {
        fir_Instr **args = (fir_Instr **)instr->data;

        fprintf(fp, "add");
        fprintf(fp, " %%%d, %%%d", args[0]->idx, args[1]->idx);
        break;
    }
    case FIR_INSTR_STACK: {
        fir_Instr **args = (fir_Instr **)instr->data;
        
        fprintf(fp, "stack %%%d", args[0]->idx);
        break;
    }
    case FIR_INSTR_OFFSET: {
        fir_Instr **args = (fir_Instr **)instr->data;

        fprintf(fp, "offset %%%d, %%%d", args[0]->idx, args[1]->idx);
        break;
    }
    case FIR_INSTR_WRITE: {
        fir_Instr **args = (fir_Instr **)instr->data;

        fprintf(fp, "write %%%d, %%%d", args[0]->idx, args[1]->idx);
        break;
    }
    case FIR_INSTR_READ: {
        fir_Instr **args = (fir_Instr **)instr->data;

        fprintf(fp, "read %%%d", args[0]->idx);
        break;
    }
    case FIR_INSTR_SUB: {
        fir_Instr **args = (fir_Instr **)instr->data;

        fprintf(fp, "sub");
        fprintf(fp, " %%%d, %%%d", args[0]->idx, args[1]->idx);
        break;
    }
    case FIR_INSTR_MUL: {
        fir_Instr **args = (fir_Instr **)instr->data;

        fprintf(fp, "mul");
        fprintf(fp, " %%%d, %%%d", args[0]->idx, args[1]->idx);
        break;
    }
    case FIR_INSTR_RET: {
        fprintf(fp, "ret");

        fir_InstrRet *ret = (fir_InstrRet *)instr->data;
        if (ret->args_n > 0) {
            fir_Instr **args = (fir_Instr **)ret->args;

            fprintf(fp, " ");
            for (size_t i = 0; i < ret->args_n; i++) {
                fprintf(fp, "%%%d", args[i]->idx);
                if (i < ret->args_n - 1) {
                    fprintf(fp, ", ");
                }
            }
        }
        break;
    }
    case FIR_INSTR_CALL: {
        fir_InstrCall *call = (fir_InstrCall *)instr->data;
        char *name = fir_string_get(call->func->module, call->func->name);

        fprintf(fp, "call @%s", name);

        if (call->func->input.len > 0) {
            fprintf(fp, " ");
            fir_Instr **args = (fir_Instr **)call->args;
            for (size_t i = 0; i < call->func->input.len; i++) {
                fprintf(fp, "%%%d", args[i]->idx);

                if (i < call->func->input.len - 1) {
                    fprintf(fp, ", ");
                }
            }
        }
        break;
    }
    case FIR_INSTR_PROJ: {
        fir_InstrProj *proj = (fir_InstrProj *)instr->data;
        fprintf(fp, "proj %%%d %zu", proj->instr->idx, proj->idx);
        break;
    }
    default:
        fprintf(fp, "<unhandled instruction>");
        break;
    }
}
