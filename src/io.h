#ifndef IO_H
#define IO_H

#include "bytecode.h"
#include <stdio.h>

char *io_file_read(char *path, int *src_len);
void io_file_write_bytecode(FILE *out, bytecode *bc);

#endif
