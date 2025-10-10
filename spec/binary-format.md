# Cosm Binary Format (v0)

A cosm binary file represents a single module. It starts with the following
header:

## Module

| header   | Header     |
| sections | \[Section] |

## Header

| magic number | "cosm"       |
| version      | u32 (0)      |
| name         | ref .strings |

## Section

| type     | u32        |
| size     | word       |
| contents | bytes      |

## Import

| module name | ref .names |
| item name   | ref .names |
| item type   | idx .types |

## Export

| name | ref .names |
| idx  | idx .items |

## Item

| type     | idx .types |
| contents | ref .??    |

## Types

### Integers

`i32 | i64`

### Floats

`f32 | f64`

### Pointers

`*<ty>`

### Tuples

`(<ty>, ..)`

### Functions

`fn <ty> -> <ty>`

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

