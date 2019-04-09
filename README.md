# Parser progetto ing. informatica

----
## Proposed IR
### Enums
Next we define all the enumerations that will be used inside our structures:

```
typedef enum {...} opcode;
typedef enum {...} register;
typedef enum {...} family;
typedef enum {...} as_directive;
typedef enum {INSTRUCTION, DIRECTIVE, LABEL} role;
```
* `opcode` introduces an enumeration of all possible opcodes in the RISC-V I-M-A-C instruction set
* `register` introduces an enumeration of all possible registers used
* `family` introduces an enumeration of all possible opcode types 
* `as_directive` introduces an enumeration of all possible directives
* `role` introduces an enumeration of all possible roles that a line can assume

### Structures
The primary data structure is composed of two different fields:

```
typedef struct line{
   roles role;
   union Ptr{
      instruction *instr;
      symbol *sym;
      directive *dir;
   } ptr;
   struct line *next_line;
   struct line *prev_line;
}line;
```

* a union of three different pointers identified by the line's role
* a field of `role` type identifying the line's meaning

A second layer of structures defines each line accordingly to it's meaning, those structures are:

```
    typedef struct{
        char *name;
        int value;
        line *ptr;
        islab boolean;
    } symbol;

    typedef struct instruction{
        register r1;
        register r2;
        register r3;
        is_literal boolean;
        union immediate{
            int literal;
            symbol *symb;
        } imm_field;
        family type;
    } instruction;

    typede struct directive{
        as_directive name;
        char **args;
    } directive;
```

## Usage
The proposed parser takes as input a `FILE*` containing a reference to an assembler source, reads its content and returns a `line` bilinked list. 

