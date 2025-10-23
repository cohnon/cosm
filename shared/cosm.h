#ifndef COSM_H
#define COSM_H

#include <stdint.h>

#define COSM_MAGIC ("cosm")
#define COSM_VERSION (0)

typedef uint64_t COSM_Ref;

typedef enum COSM_SecType {
    COSM_SEC_CUSTOM = 0x00,
    COSM_SEC_IMPORTS,
    COSM_SEC_EXPORTS,
    COSM_SEC_STRINGS,
    COSM_SEC_TYPES,
    COSM_SEC_ITEMS,
    COSM_SEC_CODE,
    COSM_SEC_DATA,
} COSM_SecType;

typedef enum COSM_Type {
    COSM_TYPE_UNIT = 0x00,

    COSM_TYPE_I8,
    COSM_TYPE_I16,
    COSM_TYPE_I32,
    COSM_TYPE_I64,

    COSM_TYPE_WORD,
    COSM_TYPE_HALF,

    COSM_TYPE_F32,
    COSM_TYPE_F64,

    COSM_TYPE_PTR,
    COSM_TYPE_ARRAY,
    COSM_TYPE_SLICE,

    COSM_TYPE_TUP,
    COSM_TYPE_UNION,

    COSM_TYPE_FUNC,

    COSM_TYPE_MUT,

    COSM_TYPE_TYPE,
} COSM_Type;

typedef enum COSM_InstrType {
    COSM_INSTR_ADD,
    COSM_INSTR_IMM,
    COSM_INSTR_CALL,
    COSM_INSTR_RET,
} COSM_InstrType;

#endif
