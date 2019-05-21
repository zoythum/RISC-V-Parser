#ifndef PARSER_MAIN_H
#define PARSER_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include "rvp-data.h"
#include "rvp-utility-functions.h"


/* OUTPUT DATA FORMAT */

/*
 * What follows are the data structures the assembler source is converted into after being parsed.
 *
 * The line_encaps structure is merely a utility construct which holds two struct aggregates: a chain of parsed lines and a chain of labels.
 */

typedef struct line_encaps {
    struct line *line_head;
    struct symb_tab *symbol_head;
} line_encaps;

/*
 * Each line structure in the chain is a unit composed of a single "syntactic functor", that is: a symbol, a directive or an instruction.
 * In order to tell apart these very different elements, a so called "role" is attributed to the line; there is one role for each recognized functor.
 *
 * Since a line can only assume a single role, but an assembly statement can indeed have one or more labels per line referring to a directive or
 * instruction, labels are artificially split on multiple lines, maintaining their ordering to ensure semantic equivalence.
 */

typedef struct line {
   roles role;
   union Ptr{
      struct instruction *instr;
      struct symbol *sym;
      struct directive *dir;
   } ptr;
   struct line *next_line;
   struct line *prev_line;
}line;

/*
 * For every syntactic functor an appropriate structure exists.
 *
 * Because of the limited scope of our code manipulation attempts, we deemed unnecessary to interpret some more refined forms of arguments that
 * may accompany a statement, so we either ignore them by making a byte-by-byte copy to the output structures or we simply let the program state
 * degrade. Unfortunately, we are not the best C programmers out there.
 *
 * On our defence, we can say that complex arithmetic expressions and the like are rarely (maybe never) present in automatically generated assembler
 * sources.
 */

//Parsed symbol, with its integer value and a flag to specify whether it is a mundane symbol or a label.
typedef struct symbol {
    char *name;
    int value;
    bool islab;
} symbol;

//Parsed directive, identified by way of an (incomplete) enumeration consisting of what we considered worthwhile GNU Assembler directives.
typedef struct directive {
    as_directive name;
    int args_num;
    char **args;
} directive;

//Parsed instruction, complete with enumerated registers, instruction type (aka "family") and, eventually, a reference to a symbolic operand.
typedef struct instruction {
    char *opcode;
    reg r1;
    reg r2;
    reg r3;
    bool is_literal;
    union immediate{
        int literal;
        char *symb;
    } imm_field;
    family type;
} instruction;

/*
 * Finally, the chain of symbols constitutes a very simple form of symbol table which can be useful for code interpretation by (semi)automated tools.
 * Each node of the chain contains a direct reference to a symbol which already happens to be contained in the chain of lines.
 *
 * It's a completely optional structure, but we thought it would be a nice addition.
 */

typedef struct symb_tab {
    struct symbol *sym;
    struct symb_tab *next;
    struct symb_tab *prev;
} symb_tab;


/* MAIN PUBLIC FUNCTIONS */

/*
 * This section hosts the main function of this little library of ours: parse().
 * It's interface is simple: feed it a valid assembler source file's descriptor and it spits out what we discussed above.
 */

line_encaps *parse(FILE *work);

#endif /* PARSER_MAIN_H */
