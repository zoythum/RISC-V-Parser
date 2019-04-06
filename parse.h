#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


extern int errno;

//enumeration of possible line meaning
typedef enum {LABEL, DIRECTIVE, INSTRUCTION, COMMENT} roles;

//output data structure
typedef struct line{
    char **tokens;
    int line;
    int token_num;
    roles role;
} line;


int isTokenDelim(char value);

line *parse(FILE *work);
