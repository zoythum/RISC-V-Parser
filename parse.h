#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "data.h"


struct symbol;
struct instruction;
struct directive;
struct line;

//output data structure
typedef struct symbol {
    char *name;
    int value;
    line *ptr;
    bool islab;
} symbol;

typedef struct instruction {
    reg r1;
    reg r2;
    reg r3;
    bool is_literal;
    union immediate{
        int literal;
        symbol *symb;
    } imm_field;
    family type;
} instruction;

typedef struct directive {
    as_directive name;
    char **args;
} directive;

typedef struct line {
   roles role;
   union Ptr{
      instruction *instr;
      symbol *sym;
      directive *dir;
   } ptr;
   struct line *next_line;
   struct line *prev_line;
}line;

typedef struct symb_tab {
    symbol *sym;
    struct symb_tab *next;
    struct symb_tab *prev;
} symb_tab;

int isTokenDelim(char value);

line *parse(FILE *work);
