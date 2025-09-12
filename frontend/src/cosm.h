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
  COSM_SEC_TYPEDATA,
} CosmSectionType;

typedef u8 CosmItemType;
enum CosmItemType {
  COSM_ITEM_FUNCTION,
  COSM_ITEM_OBJECT,
  COSM_ITEM_TYPE,
};

typedef struct CosmExport {
  CosmItemType type;
  u32 flags : 24;

  union {
    u32 func;
    u32 obj;
    u32 ty;
  };

  u32 sym_pos;
  u32 sym_len;
} CosmExport;

enum CosmTypeKind {
    TYPE_I8,
    TYPE_I16,
    TYPE_I32,
    TYPE_I64,

    TYPE_WORD,
    TYPE_HALF,

    TYPE_F32,
    TYPE_F64,

    TYPE_REF,

    TYPE_TUPLE,
    TYPE_UNION,
};
typedef u8 CosmTypeKind;

typedef struct CosmType {
  u64 typedata;
} CosmType;

typedef struct CosmFunction {
  u32 type;
  u64 code;
} CosmFunction;

typedef struct CosmObject {
  u32 type;
  u64 data;
} CosmObject;

typedef struct CosmHeader {
  char magic[sizeof(COSM_MAGIC)];
  u32  version;
  u32  size;
  u32  imports_len;
  u32  exports_len;
} CosmHeader;

#endif
