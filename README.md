# RISC-V Assembler Source Parser
---
## Introduction
This project offers its functionalities either as a standalone program converting an assembler source into a well-structured JSON file, or as a static C library for
generating an in-memory intermediate representation of it.
<br>
When used as a library, two functions expose the main functionalities:
* `parse()` can elaborate a RISC-V  assembly source code and create a structure that can easily be manipolated and analyzed;
* `rebuild()` can recreate a source file starting from the same structure obtained from `parse()`.
<br>
Additionally, a function for exporting the data strucuture as a JSON list of parsed statements (`export_to_json()`) is made available as a separate module, in case more
flexibility is required than that offered by the standalone version.

## Usage
### As a library
The `parse()` function takes as input a `FILE*` containing a reference to an assembler source, reads its content and returns a structure of `line_encaps` type containing
the heads of two distinct bilinked lists, one of `line` tipe and the other one of `lab_tab` tipe.
The `rebuild()` function takes a `line_encaps` IR container argument with the parsed source, and a `FILE*` argument as the destination stream for the rebuilt source.
<br>
In order to make use of the parsing/rebuilding functionalities, a user must include the `riscv-parse.h` header and link `riscv-parse.c` and `rvp-utility-functions.c` to
his code.
<br>
The `export_to_json()` procedure requires two parameters: a valid `line_encaps` structure, from which the parsed source is taken, and a `FILE*` pointer, representing the
destination stream.
<br>
If the user needs to integrate the JSON export functionalities into its application, then `rvp-utility-json.h` has to be included and, consequently, `rvp-utility-json.c`
has to be linked to the client code.

### As a standalone program
When used as a standalone program, the parser needs to be invoked with two command-line arguments, the first indicating the assembler source, and the latter pointing at
the desired destination for the produced JSON representation.

## Proposed IR
What follows is an high level description of the data structures used for storing the parsed source.

### Enums
First we define all the enumerations that will be used inside our structures:

```
typedef enum {...} reg;
typedef enum {...} family;
typedef enum {INSTRUCTION, DIRECTIVE, LABEL} role;
```
* `reg` introduces an enumeration of all possible registers used;
* `family` introduces an enumeration of all possible instruction types;
* `role` introduces an enumeration of all possible roles that a line can assume.

### Structures
The primary data structure is composed of two linked lists attached to a `line_encaps` structure via pointers to their heads.
The two lists constitute a sequence of statements and a primitive form of global labels table, and are respectively made of `line` and `symb_tab` nodes.
<br>
`line` is composed of two data fields:

```
typedef struct line{
   roles role;
   union Ptr{
      instruction *instr;
      char *label;
      directive *dir;
   } ptr;
   struct line *next_line;
   struct line *prev_line;
}line;
```

* a union of three different pointers to further structures, identified by the line's role;
* a field of `role` type identifying the line's meaning.
It can be noticed that labels get directly embedded inside the top level structure, since the only thing we care about is their identifier and position in the sequence.
<br>
`lab_tab` instead only contains a pointer to a `char` object, representing a label identifier:

```
typedef struct lab_tab {
    char *label;
    struct lab_tab *next;
    struct lab_tab *prev;
} lab_tab;
```

A second layer of structures defines each line accordingly to it's meaning:

```
    typedef struct instruction{
        char *opcode;
        reg r1;
        reg r2;
        reg r3;
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

Even if those second layer structures are self explanatory, few comments are due:
* since assembly instructions may present an immediate operand either in the form of a literal value or as a symbol, the operand's representation inside the `instruction` struct can come in two variants. `is_literal` indicates if the immediate value must be interpreted as an integer or a symbol;
* the `directive` struct simply has an array of strings containing all the different arguments of the represented directive.

## Internal architecture
This parser is not a monolithic piece of software, but a collection of functions (or "modules") called by the main parse() function.
These support modules concur to the realization of 5 different high level functionalities: assembler code ingestion, normalization and validation; string tokenization and dispatch; label and symbol comprehension; directives encapsulation; instruction decoding.
### Code ingestor/verifier (`line_feeder()`)
This first module reads contents directly from the supplied file.
Firstly it stream-normalizes its input, removing comments and unnecessary blank characters.
Secondly, it validates the read code following the [GNU Assembler](https://sourceware.org/binutils/docs-2.32/as/) input files specification.
Lastly, the encapsulation of the normalized assembler file's lines takes place, using an internal data exchange structure.
### String tokenizer/dispatcher (`string_tokenizer()`)
Following the generation of the clean code lines, the string tokenizer divides these into space separated tokens, recognizes the characterizing (first) token as one of the predefined roles (instruction, directive or label) and encapsulates the related tokens on the same line in a suiting data structure, tailored to the specific role. This structure is then passed on to the decoding battery that follows.
### Directive encapsulator (`directive_decoder()`)
The directive encapsulator has relatively little work to do, since we are interested only on a subset of assembler directives, namely the ones that alter the data and instruction flows.
Its job is to encapsulate most of the directives in a `DIRECTIVE <--> ARGUMENT(S)` separating structure, so that data/instruction mangling programs using this parser can easily act on the argument's values.
### Instruction decoder (`instruction_decoder()`)
The instruction decoder is tasked with recognizing the families of instructions and normalizing their dishomogeneous arguments syntax into a well-defined data structure representing the opcode, the registers and the immediate values of the passed instruction line.

## Json compatibility
As previously stated, this piece of software is able to export the internal structure to a JSON file, thus making it interoperable with other software.
The resulting JSON data presents an object list as the top structure. Each object rerpesents a statement:
```
{
    "role": <statement type/role>
    [statement-specific fields]
}
```
Depending on the statement's type, a number of key-value pairs may appear.
For labels it's simply:
`"name": <label name>`
Directives have a name and a list of arguments:
```
"name": <directive identifier>
"args" : [
    <arg1>,
    <arg2>,
    ...
]
```
Finally, instructions represent the most complex case:
```
"opcode": <opcode>
"r1": <reg number>
"r2": <reg number>
"r3": <reg number>
"immediate": <immediate field, either in literal or symbolic form>
"family": <instruction family>
```
