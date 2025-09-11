# Cosi Binary Format (v0)

A cosi binary file represents a single module. It starts with the following
header:

## Header

| magic number | 4 bytes ("cosi") |
| version      | 4 bytes          |
| no. of items | 4 bytes          |

## Items

A module is made up of a list of items. Each items is prefixed with an
item type (1 byte), and its size in bytes (4 bytes). Items are referenced by
4 bytes in the order they are declared.

| Item Type | Description |
| 0x00      | Custom      |
| 0x01      | Function    |
| 0x02      | Global      |

### Functions

Functions take N input and M output parameters. Each parameter is one of the
primitive types.

| name    | string          |
| inputs  | \[type]         |
| outputs | \[type]         |
| body    | \[instructions] |

## Types

### Integers

`i32 | i64`

Both signed and unsigned integers are represented the same at the type level
and their representation is based on the operations

### Floats

`f32 | f64`

### References

`ref | wref`

### Tuples

`tupN`

## Instructions

| Name                              | Code | Description                                                              |
| ( Control )                       |  --  |                                                                          |
| NOP                               | 0x00 | Does Nothing                                                             |
| CALL <func ID> \[<arg>]           | 0x01 | Calls a function with args. returns a tuple if multiple returns          |
| RET                               | 0x02 | Return from the current function                                         |
| JMP <blk>                         | 0x03 | Unconditionally jump to a basic block                                    |
| JMP <cond> <blk> <blk>            | 0x04 | Jump to the first block if condition is 1, else jump to the second block |
| SWITCH <cond> \[<val> <blk>]      | 0x05 | Jump to the block based the condition's value                            |
| ( Values )                        |  --  |                                                                          |
| LIT                               | 0x0A | Creates a literal
| ( Arithmetic )                    |  --  |                                                                          |
| ADD <type> <reg> <reg>            | 0x10 | Addition                                                                 |
| SUB <type> <reg> <reg>            | 0x11 | Subtraction                                                              |
| MUL <type> <reg> <reg>            | 0x12 | Multiplication                                                           |
| DIV <type> <reg> <reg>            | 0x13 | Division                                                                 |
| ( Memory )                        |  --  |                                                                          |
| PEEK <type> <from>                | 0x50 | Read a value from memory                                                 |
| PEEK <type> <from> <offset>       | 0x51 | Read a value from memory at an offset                                    |
| POKE <type> <from> <val>          | 0x52 | Write a value from memory                                                |
| POKE <type> <from> <offset> <val> | 0x53 | Write a value from memory at an offset                                   |
| MOV <from> <to>
| STACK <size>                      | 0x54 | Allocate space on the stack                                              |
