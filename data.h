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
typedef enum {u, i, s, r, j, jr, b, al, as, sext, _2arg, bz, nop, err} family;
typedef enum {_2BYTE, _4BYTE, _8BYTE, ALIGN, ALTMACRO, ASCII, ASCIZ, ATTRIBUTE, BALIGN, BSS, BUNDLE_ALIGN_MODE, BUNDLE_LOCK, BUNDLE_UNLOCK, BYTE, COMM, DATA, 
                DC_A, DC_B, DC_D, DC_L, DC_S, DC_W, DC_X, DCB_B, DCB_D, DCB_L, DCB_S, DCB_W, DCB_X, DEF, DESC, DIM, DOUBLE, DS_B, DS_D, DS_L, DS_P, DS_S, DS_W, DS_X, 
                DTPRELDWORD, DTPRELWORD, DWORD, ELSE, ELSEIF, END, ENDEF, ENDFUNC, ENDIF, ENDM, ENDR, EQU, EQUIV, EQV, ERR, ERROR, EXITM, FAIL, FILL, FIND_ERR, FLOAT, 
                FUNC, GLOBAL, GLOBL, GNU_ATTRIBUTE, HALF, HIDDEN, HWORD, IDENT, IF, IFB, IFC, IFDEF, IFEQ, IFEQS, IFGE, IFGT, IFLE, IFLT, IFNB, IFNC, IFNDEF, IFNE, IFNES, 
                IFNOTDEF, INSN, INT, INTERNAL, IRP, IRPC, LCOMM, LOC, LOCAL, LOC_MARK_LABELS, LONG, MACRO, NOALTMACRO, NOPS, OCTA, OFFSET, OPTION, ORG, P2ALIGN, P2ALIGNL, 
                P2ALIGNW, POPSECTION, PREVIOUS, PROTECTED, PURGEM, PUSHSECTION, QUAD, RELOC, REPT, SBTTL, SCL, SECTION, SET, SHORT, SINGLE, SIZE, SKIP, SLEB128, SPACE, 
                STABD, STABN, STABS, STRING, STRING16, STRING32, STRING64, STRING8, STRUCT, SUBSECTION, SYMVER, TAG, TEXT, TYPE, ULEB128, VAL, VERSION, VTABLE_ENTRY, 
                VTABLE_INHERIT, WARNING, WORD, ZERO} as_directive;