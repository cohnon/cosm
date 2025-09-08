#ifndef COSI_H
#define COSI_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct cosi_Module cosi_Module;
typedef struct cosi_Block cosi_BasicBlock;
typedef struct cosi_Instruction cosi_Instruction;


// Array
typedef struct {
    void  *ptr;
    size_t len : 32;
    size_t cap : 32;
} cosi_Array;

cosi_Array cosi_array_init_untyped(size_t elem_size, size_t init_cap);
#define cosi_array_init(T, init_cap) cosi_array_init_untyped(sizeof(T), init_cap)

void cosi_array_deinit(cosi_Array *array);

void cosi_array_append_untyped(cosi_Array *array, size_t elem_size, void *elem);
#define cosi_array_append(T, array, elem) cosi_array_append_untyped(array, sizeof(T), (T*)&elem)

void *cosi_array_get_untyped(cosi_Array *array, size_t elem_size, size_t idx);
#define cosi_array_get(T, array, idx) ((T*)cosi_array_get_untyped(array, sizeof(T), idx))

// Type
enum cosi_DataType {
    COSI_TYPE_I8,
    COSI_TYPE_I16,
    COSI_TYPE_I32,
    COSI_TYPE_I64,
    COSI_TYPE_I128,

    COSI_TYPE_F16,
    COSI_TYPE_F32,
    COSI_TYPE_F64,

    COSI_TYPE_REF,
    COSI_TYPE_WREF,

    COSI_TYPE_TUP,
};

// Module
#define COSI_MODULE_NAME_MAX_LEN ((1<<7) - 1)

struct cosi_Module {
    char   name[COSI_MODULE_NAME_MAX_LEN + 1];
    size_t name_len;

    cosi_Array funcs; // cosi_Function *
};

cosi_Module cosi_module_init(const char *name);
void cosi_module_deinit(cosi_Module *module);

void cosi_module_write_binary(cosi_Module *module, FILE *fp);
void cosi_module_write_text(cosi_Module *module, FILE *fp);

// Function
#define FUNCTION_NAME_MAX_LEN ((1<<7) - 1)
typedef struct cosi_Function {
    cosi_Module *module;

    char   name[FUNCTION_NAME_MAX_LEN];
    size_t name_len;

    cosi_Array    basic_blocks; // cosi_Block *

    cosi_Array input;  // cosi_DataType
    cosi_Array output; // cosi_DataType
} cosi_Function;

cosi_Function *cosi_func_create(cosi_Module *module, const char *name);
void cosi_func_destroy(cosi_Function *func);

cosi_Block *cosi_func_get_entry(cosi_Function *func);

void cosi_func_add_input(cosi_Function *func, cosi_DataType type);
void cosi_func_add_output(cosi_Function *func, cosi_DataType type);

void cosi_func_dump(cosi_Function *func, FILE *fp);

// Block
struct cosi_Block {
    cosi_Function *func;

    cosi_Array input; // cosi_DataType
    cosi_Array instrs; // cosi_Instruction *
};

cosi_Block *cosi_block_create(cosi_Function *func, const char *name);
void cosi_block_destroy(cosi_Block *blk);

void cosi_block_add_input(cosi_Block *blk, cosi_DataType type);
cosi_Instruction *cosi_block_get_terminator(cosi_Block *blk);

void cosi_block_dump(cosi_Function *func, cosi_Block *blk, FILE *fp);

// Instruction
enum cosi_OpCode {
    COSI_INSTR_NOP,

    // values
    COSI_INSTR_LIT, // LIT <type> <value>
    COSI_INSTR_ARG,

    // arithmetic
    COSI_INSTR_ADD, // ADD <val> <val>
    COSI_INSTR_SUB, // SUB <val> <val>
    COSI_INSTR_MUL, // MUL <val> <val>
    COSI_INSTR_DIV, // DIV <val> <val>

    COSI_INSTR_SHL, // SHL <val> <val>
    COSI_INSTR_SHR, // SHR <val> <val>

    COSI_INSTR_CMP, // CMP <val> <val>

    // memory
    COSI_INSTR_STACK,
    COSI_INSTR_OFFSET,
    COSI_INSTR_PEEK,
    COSI_INSTR_POKE,

    // terminators
    COSI_INSTR_CALL,
    COSI_INSTR_JMP,
    COSI_INSTR_IF,
    COSI_INSTR_RET,
};

#define uint8_t cosi_OpCode;

typedef union cosi_InstructionLit {
    double f;
    uint64_t i;
} cosi_InstructionLit;

typedef struct cosi_InstructionRet {
    size_t args_n;
    cosi_Instruction *args[];
} cosi_InstructionRet;

typedef struct cosi_InstructionCall {
    cosi_Function *func;
    cosi_Instruction *args[];
} cosi_InstructionCall;

typedef struct cosi_InstructionProj {
    cosi_Instruction *instr;
    size_t idx;
} cosi_InstructionProj;

bool cosi_instr_is_terminator(cosi_Instruction *instr);

cosi_Instruction *cosi_instr_lit_int(
    cosi_Block *blk,
    cosi_DataType type,
    uint64_t val
);

cosi_Instruction *cosi_instr_lit_float(
    cosi_Block *blk,
    cosi_DataType type,
    double val
);

cosi_Instruction *cosi_instr_arg(cosi_Block *blk, size_t idx);

cosi_Instruction *cosi_instr_add(cosi_Block *blk, cosi_Instruction *lhs, cosi_Instruction *rhs);
cosi_Instruction *cosi_instr_sub(cosi_Block *blk, cosi_Instruction *lhs, cosi_Instruction *rhs);
cosi_Instruction *cosi_instr_mul(cosi_Block *blk, cosi_Instruction *lhs, cosi_Instruction *rhs);
cosi_Instruction *cosi_instr_div(cosi_Block *blk, cosi_Instruction *lhs, cosi_Instruction *rhs);

cosi_Instruction *cosi_instr_stack(cosi_Block *blk, cosi_Instruction *size);
cosi_Instruction *cosi_instr_offset(cosi_Block *blk, cosi_Instruction *ptr, cosi_Instruction *by);
void cosi_instr_write(cosi_Block *blk, cosi_Instruction *to, cosi_Instruction *from);
cosi_Instruction *cosi_instr_read(cosi_Block *blk, cosi_DataType type, cosi_Instruction *from);

cosi_Instruction *cosi_instr_ret(cosi_Block *blk);
void cosi_instr_ret_arg(cosi_Instruction *ret, cosi_Instruction *val);

cosi_Instruction *cosi_instr_call(cosi_Block *blk, cosi_Function *func);
void cosi_instr_call_arg(cosi_Instruction *call, cosi_Instruction *arg);

cosi_Instruction *cosi_instr_proj(cosi_Block *blk, cosi_Instruction *tuple, size_t idx);

void cosi_instr_dump(cosi_Instruction *instr, FILE *fp);

#endif

