#ifndef PARSER_DATA_H
#define PARSER_DATA_H

//Enumeration of possible line meaning
typedef enum {LABEL, DIRECTIVE, INSTRUCTION, COMMENT} roles;

//Enumeration of CPU registers
typedef enum {ra, sp, gp, tp, t0, t1, t2, t3, t4, t5, t6, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, a0, a1, a2, a3, a4, a5, a6, a7, zero, reg_err} reg;

//Reg enumeration -> reg name mapping
char *regname[] = {"ra", "sp", "gp", "tp", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "s0" "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "zero"};

//Enumeration of instruction families
typedef enum {u, i, s, r, j, jr, b, al, as, sext, _2arg, bz, nop, err} family;

//Enumeration of GNU Assembler directives
typedef enum {_2BYTE, _4BYTE, _8BYTE, ALIGN, ALTMACRO, ASCII, ASCIZ, ATTRIBUTE, BALIGN, BSS, BUNDLE_ALIGN_MODE, BUNDLE_LOCK, BUNDLE_UNLOCK, BYTE, COMM, DATA, 
                DC_A, DC_B, DC_D, DC_L, DC_S, DC_W, DC_X, DCB_B, DCB_D, DCB_L, DCB_S, DCB_W, DCB_X, DEF, DESC, DIM, DOUBLE, DS_B, DS_D, DS_L, DS_P, DS_S, DS_W, DS_X, 
                DTPRELDWORD, DTPRELWORD, DWORD, ELSE, ELSEIF, END, ENDEF, ENDFUNC, ENDIF, ENDM, ENDR, EQU, EQUIV, EQV, ERR, ERROR, EXITM, FAIL, FILL, FIND_ERR, FLOAT, 
                FUNC, GLOBAL, GLOBL, GNU_ATTRIBUTE, HALF, HIDDEN, HWORD, IDENT, IF, IFB, IFC, IFDEF, IFEQ, IFEQS, IFGE, IFGT, IFLE, IFLT, IFNB, IFNC, IFNDEF, IFNE, IFNES, 
                IFNOTDEF, INSN, INT, INTERNAL, IRP, IRPC, LCOMM, LOC, LOCAL, LOC_MARK_LABELS, LONG, MACRO, NOALTMACRO, NOPS, OCTA, OFFSET, OPTION, ORG, P2ALIGN, P2ALIGNL, 
                P2ALIGNW, POPSECTION, PREVIOUS, PROTECTED, PURGEM, PUSHSECTION, QUAD, RELOC, REPT, SBTTL, SCL, SECTION, SET, SHORT, SINGLE, SIZE, SKIP, SLEB128, SPACE, 
                STABD, STABN, STABS, STRING, STRING16, STRING32, STRING64, STRING8, STRUCT, SUBSECTION, SYMVER, TAG, TEXT, TYPE, ULEB128, VAL, VERSION, VTABLE_ENTRY, 
                VTABLE_INHERIT, WARNING, WORD, ZERO} as_directive;

//Dir enumeration -> dir name mapping
char *dirname[] = {".2byte", ".4byte", ".8byte", ".align", ".altmacro", ".ascii", ".asciz", ".attribute", ".balign", ".bss", ".bundle_align_mode", ".bundle_lock", ".bundle_unlock", ".byte", ".comm", ".data", ".dca", ".dcb", ".dcd", ".dcl", ".dcs", ".dcw", ".dcx", ".dcbb", ".dcbd", ".dcbl", ".dcbs", ".dcbw", ".dcbx", ".def", ".desc", ".dim", ".double", ".dsb", ".dsd", ".dsl", ".dsp", ".dss", ".dsw", ".dsx", ".dtpreldword", ".dtprelword", ".dword", ".else", ".elseif", ".end", ".endef", ".endfunc", ".endif", ".endm", ".endr", ".equ", ".equiv", ".eqv", ".err", ".error", ".exitm", ".fail", ".fill", ".find_err", ".float", ".func", ".global", ".globl", ".gnu_attribute", ".half", ".hidden", ".hword", ".ident", ".if", ".ifb", ".ifc", "ifdef", ".ifeq", ".ifeqs", ".ifge", ".ifgt", ".ifle", ".iflt", ".ifnb", ".ifnc", ".ifndef", ".ifne", ".ifnes", ".ifnotdef", ".insn", ".int", ".internal", ".irp", ".irpc", ".lcomm", ".loc", ".local", ".loc_mark_labels", ".long", ".macro", ".noaltmacro", ".nops", ".octa", ".offset", ".option", ".org", ".p2align", ".p2alignl", ".p2alignw", ".popsection", ".previous", ".protected", ".purgem", ".pushsection", ".quad", ".reloc", ".rept", ".sbttl", ".scl", ".section", ".set", ".short", ".single", ".size", ".skip", ".sleb128", ".space", ".stabd", ".stabn", ".stabs", ".string", ".string16", ".string32", ".string64", ".string8", ".struct", ".subsection", ".symver", ".tag", ".text", ".type", ".uleb128", ".val", ".version", ".vtable_entry", ".vtable_inherit", ".warning", ".word", ".zero"}

#endif /* PARSER_DATA_H */
