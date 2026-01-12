#ifndef BYTECODE_H
#define BYTECODE_H

#include "array.h"
#include "ast.h"
#include "buffer.h"

typedef enum {
	COSM_UNIT,

	COSM_INT8,
	COSM_INT16,
	COSM_INT32,
	COSM_INT64,

	COSM_UINT8,
	COSM_UINT16,
	COSM_UINT32,
	COSM_UINT64,

	COSM_FLOAT32,
	COSM_FLOAT64,

	COSM_FN,
	COSM_TUPLE,
	COSM_UNION,
} bc_type;

ARRAY_DECL(type_list, bc_type);

typedef struct {
	byte_buffer types;
	byte_buffer imports;
	byte_buffer exports;
	byte_buffer items;
	byte_buffer code;
	byte_buffer data;
} bytecode;

bytecode lower(ast_module *ast);

#endif
