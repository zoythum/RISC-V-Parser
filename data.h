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
typedef enum {ra, sp, gp, tp, t0, t1, t2, t3, t4, t5, t6, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, a0, a1, a2, a3, a4, a5, a6, a7, zero, reg_err} reg;
// TODO: complete enum
typedef enum {...} opcode;
typedef enum {u, i, s, r, j, b, al, as, sext, err} family;
typedef enum {...} as_directive;