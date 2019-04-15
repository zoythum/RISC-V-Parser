#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define TOKEN_SHARP '#'
#define TOKEN_UNDERSCORE '_'
#define TOKEN_DOT '.'
#define TOKEN_BACKSLASH '\\'
#define TOKEN_DOLLAR '$'
#define TOKEN_A_MAIUSC 'A'
#define TOKEN_Z_MAIUSC 'Z'
#define TOKEN_A_MINUSC 'a'
#define TOKEN_Z_MINUSC 'z'
#define TOKEN_ZERO '0'
#define TOKEN_NINE '9'
#define TOKEN_DOUBLE_QUOTE '"'
#define TOKEN_SLASH '/'
#define TOKEN_STAR '*'

#define TOKEN_SIZE_DEF 5
#define ARRAY_SIZE 250
#define BUFFER_SIZE 250


//enumeration of possible line meaning
typedef enum {LABEL, DIRECTIVE, INSTRUCTION, COMMENT} roles;
typedef enum {...} opcode;
typedef enum {...} reg;
typedef enum {...} family;
typedef enum {...} as_directive;

//output data structure
typedef struct{
    char *name;
    int value;
    line *ptr;
    bool islab;
} symbol;

typedef struct instruction{
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

typedef struct directive{
    as_directive name;
    char **args;
} directive;

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

int isTokenDelim(char value);

line *parse(FILE *work);
