#ifndef PARSER_H
#define PARSER_H

#include "cosm.h"
#include "lexer.h"
#include <stddef.h>
#include <stdio.h>

typedef struct CosmModule {
  // module visibility
  size_t        imports_len;
  CosmImport     *imports;

  size_t        exports_len;
  CosmExport  *exports;

  // items
  size_t        funcs_len;
  size_t        objs_len;
  size_t        types_len;
  CosmFunction *funcs;
  CosmObject   *objs;
  CosmType      types;

  // sources
  size_t        code_size;
  size_t        data_size;
  size_t        typedata_size;
  u8           *code;
  u8           *data;
  u8           *typedata;
} CosmModule;

CosmModule parse(TokenArray toks);

#endif
