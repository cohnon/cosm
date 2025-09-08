#include "fir.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

fir_Block *fir_block_create(fir_Function *func, const char *name) {
    fir_Block *blk = fir_arena_alloc(&func->module->arena, sizeof(fir_Block));

    blk->func = func;
    blk->name = fir_string_add(func->module, name);

    blk->input = fir_array_init(fir_DataType, 4);
    blk->instrs = fir_array_init(fir_Instr *, 8);

    fir_array_append(fir_Block *, &func->blocks, blk);

    return blk;
}

void fir_block_destroy(fir_Block *block) {
    fir_array_deinit(&block->input);
    fir_array_deinit(&block->instrs);

    block->func = NULL;
}

void fir_block_add_input(fir_Block *block, fir_DataType type) {
    fir_array_append(fir_DataType, &block->input, type);
}


fir_Instr *fir_block_get_terminator(fir_Block *block) {
    fir_Instr *last_instr =
        *fir_array_get(fir_Instr *, &block->instrs, block->instrs.len - 1);

    if (fir_instr_is_terminator(last_instr)) {
        return last_instr;
    }

    return NULL;
}

void fir_block_dump(fir_Function *func, fir_Block *block, FILE *fp) {
    const char *name = fir_string_get(func->module, block->name);

    if (strcmp(name, "entry") != 0) {
        fprintf(fp, "%s:", name);

        if (block->input.len > 0) {
            fprintf(fp, " (");
            for (size_t i = 0; i < block->input.len; i++) {
                fir_DataType in = *fir_array_get(fir_DataType, &block->input, i);
                fir_type_dump(in, fp);

                if (i < block->input.len - 1) {
                    fprintf(fp, ", ");
                }
            }
            fprintf(fp, ")");
        }

        fprintf(fp, "\n");
    }

    if (block->instrs.len > 0) {
        for (size_t i = 0; i < block->instrs.len; i++) {
            fir_Instr *instr = *fir_array_get(fir_Instr *, &block->instrs, i);

            // arg instructions are implicit
            if (instr->kind == FIR_INSTR_ARG) { continue; }

            fprintf(fp, "  ");

            if (instr->type.kind == FIR_TYPE_TUPLE) {
                fprintf(fp, "(");
                assert(instr->kind == FIR_INSTR_CALL);
                fir_InstrCall *call = (fir_InstrCall *)instr->data;
                for (size_t i = 0; i < call->func->output.len; i++) {
                    fir_type_dump(*fir_array_get(fir_DataType, &call->func->output, i), fp);

                    if (i < call->func->output.len - 1) {
                        fprintf(fp, ", ");
                    }
                }

                fprintf(fp, ") %%%d = ", instr->idx);
            } else if (!fir_instr_is_terminator(instr) && !fir_type_is_void(instr->type)) {
                fir_type_dump(instr->type, fp);
                fprintf(fp, " %%%d = ", instr->idx);
            }

            fir_instr_dump(instr, fp);
            fprintf(fp, "\n");
        }
    }
}
