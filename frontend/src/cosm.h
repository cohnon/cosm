#ifndef COSI_H
#define COSI_H

#include <stdint.h>

#define COSM_MAGIC ("cosm")
#define COSM_VERSION (0)

#define COSM_MAX_SYMBOL_LENGTH ((1<<6))

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef enum CosmTypeKind {
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
} CosmTypeKind;

typedef enum CosmSectionType {
  COSM_SEC_CUSTOM,

  COSM_SEC_IMPORTS,
  COSM_SEC_EXPORTS,
  COSM_SEC_STRING_TABLE,

  COSM_SEC_TYPES,
  COSM_SEC_FUNCTIONS,
  COSM_SEC_OBJECTS,

  COSM_SEC_CODE,
  COSM_SEC_DATA,
} CosmSectionType;

enum CosmItemType {
  COSM_ITEM_FUNCTION,
  COSM_ITEM_OBJECT,
};
typedef u8 CosmItemType;

enum CosmCallConv {
  COSM_CALLCONV_COSM,
  COSM_CALLCONV_C,
};
typedef u8 CosmCallConv;

typedef struct CosmImport {
  CosmItemType type;
  CosmCallConv callconv;

  u16 pad;

  u32 idx;

  u32 sym_pos;
  u32 sym_len;
} CosmImport;

typedef struct CosmHeader {
  char magic[sizeof(COSM_MAGIC)];
  u32  version;
  u32  size;
  u32  n_imports;
  u32  n_exports;
} CosmHeader;

#endif
