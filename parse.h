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
typedef enum {ra, sp, gp, tp, t0, t1, t2, t3, t4, t5, t6, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, a0, a1, a2, a3, a4, a5, a6, a7, zero} reg;
// TODO: complete enum
typedef enum {...} opcode;
typedef enum {...} family;

//Only the following assembler directives are supported by this parser (that is: it does not throw an error when one of them is found)
typedef enum { ALIGN, ALTMACRO, ASCII, ASCIZ, BALIGN, BUNDLE_ALIGN_MODE, BUNDLE_LOCK, BUNDLE_UNLOCK, BYTE, COMM, DATA, DC.A, DC.B, DC.D, DC.L, DC.S, DC.W, DC.X, DCB.B, DCB.D, DCB.L, DCB.S, DCB.W, DCB.X, DS.B, DS.D, DS.L, DS.P, DS.S, DS.W, DS.X, DEF, DESC, DIM, DOUBLE, ELSE, ELSEIF, END, ENDEF, ENDFUNC, ENDIF, EQU, EQUIV, EQV, ERR, ERROR, EXITM, FAIL, FILL, FLOAT, FUNC, GLOBAL, GLOBL, GNU_ATTRIBUTE, HIDDEN, HWORD, IDENT, IF, IFDEF, IFB, IFC, IFEQ, IFEQS, IFGE, IFGT, IFLE, IFLT, IFNB, IFNC, IFNDEF, IFNOTDEF, IFNE, IFNES, INT, INTERNAL, IRP, IRPC, ENDR, LCOMM, LOC, LOC_MARK_LABELS, LOCAL, LONG, MACRO, ENDM, NOALTMACRO, NOPS, OCTA, OFFSET, ORG, P2ALIGN, P2ALIGNW, P2ALIGNL, POPSECTION, PREVIOUS, PROTECTED, PURGEM, PUSHSECTION, QUAD, RELOC, REPT, SBTTL, SCL, SECTION, SET, SHORT, SINGLE, SIZE, SKIP, SLEB128, SPACE, STABD, STABN, STABS, STRING, STRING8, STRING16, STRING32, STRING64, STRUCT, SUBSECTION, SYMVER, TAG, TEXT, TYPE, ULEB128, VAL, VERSION, VTABLE_ENTRY, VTABLE_INHERIT, WARNING, WORD, ZERO, 2BYTE, 4BYTE, 8BYTE, ALIGN, HALF, WORD, DWORD, DTPRELWORD, DTPRELDWORD, BSS, OPTION, INSN, ATTRIBUTE } as_directive;

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
