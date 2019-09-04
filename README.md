# RISC-V Source code parser
---
## Introduction
This project offers two distinct functionality:
* `parse()` can elaborate a RISC-V  assembly source code and create a structure that can easily be manipolated and analized
* `rebuild()` that can recreate a source file starting from the same structure obtained from `parse()`
## Proposed IR
### Enums
First we define all the enumerations that will be used inside our structures:

```
typedef enum {...} reg;
typedef enum {...} family;
typedef enum {INSTRUCTION, DIRECTIVE, LABEL} role;
```
* `reg` introduces an enumeration of all possible registers used
* `family` introduces an enumeration of all possible opcode types 
* `role` introduces an enumeration of all possible roles that a line can assume

### Structures
The primary data structure is composed of two different structures, `line` and `symb_tab`.
<br>
`line` is composed of two different fields

```
typedef struct line{
   roles role;
   union Ptr{
      instruction *instr;
      char *sym;
      directive *dir;
   } ptr;
   struct line *next_line;
   struct line *prev_line;
}line;
```

* a union of three different pointers identified by the line's role
* a field of `role` type identifying the line's meaning

`lab_tab` instead only contains a pointer to a `char` object

```
typedef struct lab_tab {
    struct char *sym;
    struct lab_tab *next;
    struct lab_tab *prev;
} lab_tab;
```

A second layer of structures defines each line accordingly to it's meaning, those structures are:

```
    typedef struct instruction{
        char *opcode;
        reg rd;
        reg rs1;
        reg rs2;
        bool is_literal;
        bool immediate;
        union immediate{
            int literal;
            char *symb;
        } imm_field;
        family type;
    } instruction;

    typedef struct directive{
        as_directive name;
        int args_num;
        char **args;
    } directive;
```
<br>

Even if those second layer structures are self explanatory, a small description must be given:
* Inside the `instruction` struct an immediate value can be found, `is_literal` indicates if the immediate value must be interpreted as an integer or a symbol. Info about the presence of an immediate field can be collected from `type` value
* `directive` struct simply has an array of strings containing all the different arguments of the specifed directive

## Usage
The proposed parser takes as input a `FILE*` containing a reference to an assembler source, reads its content and returns a structure of `line_encaps` type containing the heads of two distinct bilinked lists, one of `line` tipe and the other one of `labb_tab` tipe. 

## Internal architecture
This parser is not a monolithic piece of software, but a collection of functions (or "modules") called by the main parse() function.
These support modules concur to the realization of 5 different high level functionalities: assembler code ingestion, normalization and validation; string tokenization and dispatch; label and symbol comprehension; directives encapsulation; instruction decoding.
### Code ingestor/verifier (`line_feeder()`)
This first module reads contents directly from the supplied file.
Firstly it stream-normalizes its input, removing comments and unnecessary blank characters.
Secondly, it validates the read code following the [GNU Assembler](https://sourceware.org/binutils/docs-2.32/as/) input files specification.
Lastly, the encapsulation of the normalized assembler file's lines takes place, using an internal data exchange structure.
### String tokenizer/dispatcher (`string_tokenizer()`)
Following the generation of the clean code lines, the string tokenizer divides these into space separated tokens, recognizes the characterizing (first) token as one of the predefined roles (instruction, directive or instruction) and encapsulates the related tokens on the same line in a suiting data structure, tailored to the specific role. This structure is then passed on to the decoder battery that follows.
### Label/symbol decoder (`symbol_decoder()`)
Upon receiving the token collection from the preceding stage, this decoder assigns a value to a symbol and adds the association to the parser's global symbol table.
### Directive encapsulator (`directive_decoder()`)
The directive encapsulator has relatively little work to do, since we are interested only on a subset of assembler directives, namely the ones that alter the data and instruction flows.
Its job is to encapsulate most of the directives in a `DIRECTIVE <--> ARGUMENT(S)` separating structure, so that data/instruction mangling programs using this parser can easily act on the argument's values.
### Instruction decoder (`instruction_decoder()`)
The instruction decoder is tasked with recognizing the families of instructions and normalizing their dishomogeneous arguments syntax into a well-defined data structure representing the opcode, the registers and the immediate values of the passed instruction line.

## Json compatibility
It's possible to export the internal structure to a json file, thus making this library interoperable with other software. In order to create such file, it's necessary to link `rvp-utility-json` component and use the `export_to_json` function. This requires two parameters:
* a valid `line_encaps` structure 
* a `FILE` pointer.