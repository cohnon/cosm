#ifndef MODULE_H
#define MODULE_H

#include "buffer.h"
#include "cosm.h"
#include "map.h"
#include <stdint.h>
#include <stdio.h>

MAP_DECL(Map, COSM_Ref);

typedef struct Module {
    Buffer foreigns;
    Buffer globals;
    Buffer items;
    Buffer code;
    Buffer data;
    Buffer types;
    Buffer strings;

    Map item_ns;
} Module;

void write_module(Module *mod, FILE *out);

#endif
