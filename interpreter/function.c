#include "fir.h"

fir_Function *fir_func_create(fir_Module *module, const char *name) {
    fir_Function *func = fir_arena_alloc(&module->arena, sizeof(fir_Module));
    func->module = module;
    func->name = fir_string_add(module, name);

    func->blocks = fir_array_init(fir_Block *, 8);
    fir_block_create(func, "entry");

    func->input = fir_array_init(fir_DataType, 8);
    func->output = fir_array_init(fir_DataType, 1);


    fir_array_append(fir_Function *, &module->funcs, func);

    return func;
}

void fir_func_destroy(fir_Function *func) {
    for (size_t i = 0; i < func->blocks.len; i++) {
        fir_block_destroy(*fir_array_get(fir_Block *, &func->blocks, i));
    }

    fir_array_deinit(&func->blocks);

    fir_array_deinit(&func->input);
    fir_array_deinit(&func->output);
}

fir_Block *fir_func_get_entry(fir_Function *func) {
    return *fir_array_get(fir_Block*, &func->blocks, 0);
}

void fir_func_add_input(fir_Function *func, fir_DataType type) {
    fir_array_append(fir_DataType, &func->input, type);

    fir_block_add_input(fir_func_get_entry(func), type);
}

void fir_func_add_output(fir_Function *func, fir_DataType type) {
    fir_array_append(fir_DataType, &func->output, type);
}

static void set_instr_idx(fir_Function *func) {
    size_t idx = 0;

    for (size_t blk_i = 0; blk_i < func->blocks.len; blk_i++) {
        fir_Block *blk = *fir_array_get(fir_Block *, &func->blocks, blk_i);

        for (size_t instr_i = 0; instr_i < blk->instrs.len; instr_i++) {
            fir_Instr *instr = *fir_array_get(fir_Instr *, &blk->instrs, instr_i);
            instr->idx = idx++;
        }
    }
}

void fir_func_dump(fir_Function *func, FILE *fp) {
    char *name_str = fir_string_get(func->module, func->name);
    fprintf(fp, "@%s (", name_str);

    // input
    for (size_t i = 0; i < func->input.len; i++) {
        fir_DataType in = *fir_array_get(fir_DataType, &func->input, i);
        fir_type_dump(in, fp);
        fprintf(fp, " %%%zu", i);

        if (i < func->input.len - 1) {
            fprintf(fp, ", ");
        }
    }

    fprintf(fp, ") -> (");

    // output
    for (size_t i = 0; i < func->output.len; i++) {
        fir_DataType out = *fir_array_get(fir_DataType, &func->output, i);
        fir_type_dump(out, fp);

        if (i < func->output.len - 1) {
            fprintf(fp, ", ");
        }
    }

    fprintf(fp, ")\n");

    set_instr_idx(func);

    for (size_t i = 0; i < func->blocks.len; i++) {
        fir_Block *blk = *fir_array_get(fir_Block *, &func->blocks, i);
        fir_block_dump(func, blk, fp);
    }
}
