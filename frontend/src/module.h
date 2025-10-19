#ifndef MODULE_H
#define MODULE_H

#include "array.h"
#include "cosm.h"
#include "map.h"
#include <stdio.h>

ARRAY_DECL(ByteArray, unsigned char);
MAP_DECL(Map, COSM_Word);

typedef struct Module {
    ByteArray imports;
    ByteArray exports;
    ByteArray strings;
    ByteArray types;
    ByteArray items;
    ByteArray code;
    ByteArray data;

    Map item_map;
    Map type_map;
} Module;

void write_module(Module *mod, FILE *out);

#endif
