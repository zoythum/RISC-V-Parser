#include "data.h"
#include <string.h>
#include <stdlib.h>
#include "stdint.h" /* Replace with <stdint.h> if appropriate */

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

uint32_t Hash (const char * data, int len) {
    uint32_t hash = len, tmp;
    int rem;
        if (len <= 0 || data == NULL) return 0;
        rem = len & 3;
        len >>= 2;
        /* Main loop */
        for (;len > 0; len--) {
            hash  += get16bits (data);
            tmp    = (get16bits (data+2) << 11) ^ hash;
            hash   = (hash << 16) ^ tmp;
            data  += 2*sizeof (uint16_t);
            hash  += hash >> 11;
        }
        /* Handle end cases */
        switch (rem) {
            case 3: hash += get16bits (data);
                    hash ^= hash << 16;
                    hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
                    hash += hash >> 11;
                    break;
            case 2: hash += get16bits (data);
                    hash ^= hash << 11;
                    hash += hash >> 17;
                    break;
            case 1: hash += (signed char)*data;
                    hash ^= hash << 10;
                    hash += hash >> 1;
        }
        /* Force "avalanching" of final 127 bits */
        hash ^= hash << 3;
        hash += hash >> 5;
        hash ^= hash << 4;
        hash += hash >> 17;
        hash ^= hash << 25;
        hash += hash >> 6;
        return hash;
}

int isTokenDelim(char value){
    if ((value > TOKEN_A_MAIUSC && value < TOKEN_Z_MAIUSC) || 
        (value > TOKEN_A_MINUSC && value < TOKEN_Z_MINUSC) || 
        (value > TOKEN_ZERO && value < TOKEN_NINE)){
            return(0);
        } else if (value == TOKEN_DOT){
            return(1);
        } else if (value == TOKEN_DOLLAR){
            return(2);
        } else if (value == TOKEN_UNDERSCORE){
            return(3);
        } else if (value == TOKEN_BACKSLASH){
            return(4);
        } else if (value == TOKEN_SHARP){
            return(5);
        } else if (value == TOKEN_DOUBLE_QUOTE){
            return(6);
        } else if (value == TOKEN_SLASH){
            return(7);
        } else if (value == TOKEN_STAR){
            return(8);
        }
    return(-1);
}

/*
* Utility function, returns family tipe of a single opcode
*/
family family_finder(char *work) {
    int size = strlen(work);
    int value = Hash(work, size);

    switch(value){
        case 1214017035:  //opcode is add
            return(r);
        case 1978360227:  //opcode is addw
            return(r);
        case 1674232835:  //opcode is addi
            return(i);
        case 468532801:  //opcode is addiw
            return(i);
        case 1065556399:  //opcode is and
            return(r);
        case -1675356173:  //opcode is andi
            return(i);
        case -686161264:  //opcode is auipc
            return(u);
        case 150453219:  //opcode is beq
            return(b);
        case -736386852:  //opcode is bge
            return(b);
        case -1835824430:  //opcode is bgeu
            return(b);
        case -607412531:  //opcode is blt
            return(b);
        case 1660028292:  //opcode is bltu
            return(b);
        case -1793993816:  //opcode is bne
            return(b);
        case -2026240283:  //opcode is jal
            return(j);
        case 48719781:  //opcode is jalr
            return(jr);
        case -643937062:  //opcode is jr
            return(jr);
        case 2136545382:  //opcode is j
            return(j);
        case -132861449:  //opcode is or
            return(r);
        case -738803725:  //opcode is ori
            return(i);
        case 533513632:  //opcode is sext.w
            return(sext);
        case -1592371343:  //opcode is sll
            return(r);
        case -227500400:  //opcode is sllw
            return(r);
        case 396667874:  //opcode is slli
            return(i);
        case 1351661262:  //opcode is slliw
            return(i);
        case 328581899:  //opcode is slt
            return(r);
        case 2066622244:  //opcode is slti
            //not entirely sure this is correct, must be checked
            return(i);
        case 1401672236:  //opcode is sltiu
            return(i);
        case -1953876265:  //opcode is sltu
            return(r);
        case 1139295975:  //opcode is sra
            return(r);
        case -1100121253:  //opcode is sraw
            return(r);
        case -794282983:  //opcode is srai
            return(i);
        case 1451830690:  //opcode is sraiw
            return(i);
        case -730813274:  //opcode is srl
            return(i);
        case -1585354039:  //opcode is srlw
            return(r);
        case 302025000:  //opcode is srli
            return(r);
        case 979146525:  //opcode is srliw
            return(i);
        case 500433795:  //opcode is sub
            return(r);
        case 1400379623:  //opcode is subw
            return(r);
        case 1747456476:  //opcode is xor
            return(r);
        case 1347018357:  //opcode is xori
            return(i);
        case 811459139:  //opcode is mul
            return(r);
        case 1344747577:  //opcode is mulh
            return(r);
        case -671239506:  //opcode is mulhsu
            return(r);
        case -734518715:  //opcode is mulhu
            return(r);
        case -1163922948:  //opcode is div
            return(r);
        case -9286787:  //opcode is divu
            return(r);
        case 1904496798:  //opcode is rem
            return(r);
        case 290653223:  //opcode is remu
            return(r);
        case 343543809:  //opcode is mulw
            return(r);
        case -228337044:  //opcode is divw
            return(r);
        case -413873077:  //opcode is divuw
            return(r);
        case -1058605236:  //opcode is remw
            return(r);
        case -197203696:  //opcode is remuw
            return(r);
        case 814691197:  //opcode is lr.w
            return(al);
        case 1235548040:  //opcode is sc.w
            return(as);
        case -1600078277:  //opcode is amoswap.w
            return(as);
        case 400155076:  //opcode is amoadd.w
            return(as);
        case 164992337:  //opcode is amoxor.w
            return(as);
        case 632706036:  //opcode is amoor.w
            return(as);
        case -697712828:  //opcode is amoand.w
            return(as);
        case 1498677381:  //opcode is amomin.w
            return(as);
        case -10636114:  //opcode is amomax.w
            return(as);
        case 2046025111:  //opcode is amominu.w
            return(as);
        case -1981225575:  //opcode is amomaxu.w
            return(as);
        case 1903415582:  //opcode is lr.d
            return(al);
        case 1233687425:  //opcode is sc.d
            return(as);
        case -2075183246:  //opcode is amoswap.d
            return(as);
        case 752462242:  //opcode is amoadd.d
            return(as);
        case 1023101186:  //opcode is amoxor.d
            return(as);
        case -1465543219:  //opcode is amoand.d
            return(as);
        case -1828889817:  //opcode is amomin.d
            return(as);
        case 1541558695:  //opcode is amomax.d
            return(as);
        case 1646907168:  //opcode is amominu.d
            return(as);
        case 1710549081:  //opcode is amomaxu.d
            return(as);
        case -2112210961:  //opcode is lb
            return(i);
        case -1699892590:  //opcode is lh
            return(i);
        case 872642469:  //opcode is lui
            return(u);
        case -2076388154:  //opcode is lw
            return(i);
        case -1078091375:  //opcode is sb
            return(s);
        case 45073304:  //opcode is sh
            return(s);
        case -156959514:  //opcode is sw
            return(s);
        default:
            return(err);
    }
}

/**
 * Utility function, strips the first size char from input string
 */
char *strip_front(char *input, int size) {
  char *out;
  if (size > strlen(input)) {
    return NULL;
  }
  out = malloc((size)*sizeof(char));
  for (int i = size; i < strlen(input); i++) {
    out[i-size] = input[i];
  }
  return(out);
}

/**
 * Utility function, strips the last size char from input string
 */
char *strip_back(char *input, int size) {
  if (size >= strlen(input)) {
    return NULL;
  }
  char *out = malloc((strlen(input)-size)*sizeof(char));
  for (int i = 0; i < (strlen(input)-size); i++) {
    out[i] = input[i];
  }
  return(out);
}

/*
* Utility function, returns register tipe of a single register
*/
reg register_finder(char *work) {
    int size = strlen(work);
    int value = Hash(work, size);
    
    switch(value){
        case -150596914:  //register is ra
            return(ra);
        case -1517093426:  //register is sp
            return(sp);
        case -513559413:  //register is gp
            return(gp);
        case 1179373834:  //register is tp
            return(tp);
        case 1717065290:  //register is t0
            return(t0);
        case -1907073815:  //register is t1
            return(t1);
        case 1243464504:  //register is t2
            return(t2);
        case -1045755954:  //register is t3
            return(t3);
        case -248130971:  //register is t4
            return(t4);
        case 2021882418:  //register is t5
            return(t5);
        case 127989648:  //register is t6
            return(t6);
        case -158033702:  //register is s0
            return(s0);
        case 854553419:  //register is s1
            return(s1);
        case -1874521304:  //register is s2
            return(s2);
        case 1715286910:  //register is s3
            return(s3);
        case 1830845081:  //register is s4
            return(s4);
        case -1148160988:  //register is s5
            return(s5);
        case 163956233:  //register is s6
            return(s6);
        case -949592404:  //register is s7
            return(s7);
        case 28729601:  //register is s8
            return(s8);
        case 360503782:  //register is s9
            return(s9);
        case -1800796468:  //register is s10
            return(s10);
        case -1301573870:  //register is s11
            return(s11);
        case 314461287:  //register is a0
            return(a0);
        case -540820899:  //register is a1
            return(a1);
        case 6324610:  //register is a2
            return(a2);
        case -692553544:  //register is a3
            return(a3);
        case 1348367483:  //register is a4
            return(a4);
        case 217736977:  //register is a5
            return(a5);
        case 1366448405:  //register is a6
            return(a6);
        case -1081310294:  //register is a7
            return(a7);
        case -877533795:  //register is zero
            return(zero);
        default:
            return(reg_err);
    }
}

as_directive directive_finder(char *work) {
    int size = strlen(work);
    int value = Hash(work, size);

    switch(value) {
        case -1762104660: //case 2byte
	        return(_2BYTE);
        case 980539621: //case 4byte
            return(_4BYTE);
        case -1720845963: //case 8byte
            return(_8BYTE);
        case -285577657: //case align
            return(ALIGN);
        case 1394657002: //case altmacro
            return(ALTMACRO);
        case -1472051895: //case ascii
            return(ASCII);
        case 2089521851: //case asciz
            return(ASCIZ);
        case -739066146: //case attribute
            return(ATTRIBUTE);
        case 1642946269: //case balign
            return(BALIGN);
        case 415094169: //case bss
            return(BSS);
        case -980815365: //case bundle_align_mode
            return(BUNDLE_ALIGN_MODE);
        case 819896226: //case bundle_lock
            return(BUNDLE_LOCK);
        case -101895735: //case bundle_unlock
            return(BUNDLE_UNLOCK);
        case 1812869122: //case byte
            return(BYTE);
        case -836240586: //case comm
            return(COMM);
        case -1355558154: //case data
            return(DATA);
        case -307806596: //case dc.a
            return(DC_A);
        case -158122129: //case dc.b
            return(DC_B);
        case -1887398946: //case dc.d
            return(DC_D);
        case -376270082: //case dc.l
            return(DC_L);
        case -399624785: //case dc.s
            return(DC_S);
        case -116306311: //case dc.w
            return(DC_W);
        case 2113998193: //case dc.x
            return(DC_X);
        case 1522197416: //case dcb.b
            return(DCB_B);
        case 533718331: //case dcb.d
            return(DCB_D);
        case 1184564937: //case dcb.l
            return(DCB_L);
        case 1423354648: //case dcb.s
            return(DCB_S);
        case -1165574139: //case dcb.w
            return(DCB_W);
        case 437134441: //case dcb.x
            return(DCB_X);
        case -705062228: //case def
            return(DEF);
        case 1716071808: //case desc
            return(DESC);
        case -1647477941: //case dim
            return(DIM);
        case 197731685: //case double
            return(DOUBLE);
        case 1128710500: //case ds.b
            return(DS_B);
        case -1723567717: //case ds.d
            return(DS_D);
        case -1509214217: //case ds.l
            return(DS_L);
        case -1781540327: //case ds.p
            return(DS_P);
        case 329852383: //case ds.s
            return(DS_S);
        case -1734124244: //case ds.w
            return(DS_W);
        case -218434337: //case ds.x
            return(DS_X);
        case 875894841: //case dtpreldword
            return(DTPRELDWORD);
        case -1750956962: //case dtprelword
            return(DTPRELWORD);
        case 1830004995: //case dword
            return(DWORD);
        case 1240304287: //case else
            return(ELSE);
        case 1740767466: //case elseif
            return(ELSEIF);
        case -1315432669: //case end
            return(END);
        case -2108239485: //case endef
            return(ENDEF);
        case 1476313943: //case endfunc
            return(ENDFUNC);
        case 1845751444: //case endif
            return(ENDIF);
        case -1323858254: //case endm
            return(ENDM);
        case 1543195512: //case endr
            return(ENDR);
        case -112892396: //case equ
            return(EQU);
        case -1054386737: //case equiv
            return(EQUIV);
        case 1357046662: //case eqv
            return(EQV);
        case 1386256788: //case err
            return(ERR);
        case -1576616828: //case error
            return(ERROR);
        case -906261933: //case exitm
            return(EXITM);
        case 563848375: //case fail
            return(FAIL);
        case 1770712659: //case fill
            return(FILL);
        case -1095963025: //case float
            return(FLOAT);
        case 1524571510: //case func
            return(FUNC);
        case 1101884787: //case global
            return(GLOBAL);
        case -1667053591: //case globl
            return(GLOBL);
        case 796959662: //case gnu_attribute
            return(GNU_ATTRIBUTE);
        case 1176902931: //case half
            return(HALF);
        case 411400358: //case hidden
            return(HIDDEN);
        case -692008140: //case hword
            return(HWORD);
        case 1123671650: //case ident
            return(IDENT);
        case 759748795: //case if
            return(IF);
        case 1445898315: //case ifb
            return(IFB);
        case -377950551: //case ifc
            return(IFC);
        case 2125800317: //case ifdef
            return(IFDEF);
        case 1351750501: //case ifeq
            return(IFEQ);
        case 250784441: //case ifeqs
            return(IFEQS);
        case 2058588877: //case ifge
            return(IFGE);
        case -837790675: //case ifgt
            return(IFGT);
        case 1127232756: //case ifle
            return(IFLE);
        case 804552671: //case iflt
            return(IFLT);
        case 384880417: //case ifnb
            return(IFNB);
        case -1023866959: //case ifnc
            return(IFNC);
        case -728598209: //case ifndef
            return(IFNDEF);
        case 956185139: //case ifne
            return(IFNE);
        case -2044313054: //case ifnes
            return(IFNES);
        case -1252887493: //case ifnotdef
            return(IFNOTDEF);
        case -840082937: //case insn
            return(INSN);
        case -1120890216: //case int
            return(INT);
        case -1307835935: //case internal
            return(INTERNAL);
        case 33379406: //case irp
            return(IRP);
        case -307659074: //case irpc
            return(IRPC);
        case 2059211161: //case lcomm
            return(LCOMM);
        case 752432404: //case loc
            return(LOC);
        case -1401736832: //case local
            return(LOCAL);
        case -1095688433: //case loc_mark_labels
            return(LOC_MARK_LABELS);
        case -371686494: //case long
            return(LONG);
        case 638204840: //case macro
            return(MACRO);
        case -24192287: //case noaltmacro
            return(NOALTMACRO);
        case 507134247: //case nops
            return(NOPS);
        case 61973199: //case octa
            return(OCTA);
        case -1474998629: //case offset
            return(OFFSET);
        case -73750906: //case option
            return(OPTION);
        case 1511773979: //case org
            return(ORG);
        case 650319031: //case p2align
            return(P2ALIGN);
        case 602576583: //case p2alignl
            return(P2ALIGNL);
        case 456571957: //case p2alignw
            return(P2ALIGNW);
        case -1779960693: //case popsection
            return(POPSECTION);
        case -330188435: //case previous
            return(PREVIOUS);
        case -720304834: //case protected
            return(PROTECTED);
        case -785648197: //case purgem
            return(PURGEM);
        case -2124145029: //case pushsection
            return(PUSHSECTION);
        case 321744855: //case quad
            return(QUAD);
        case 1893093869: //case reloc
            return(RELOC);
        case -1623903121: //case rept
            return(REPT);
        case 455593456: //case sbttl
            return(SBTTL);
        case 1932907010: //case scl
            return(SCL);
        case -977229801: //case section
            return(SECTION);
        case 64663487: //case set
            return(SET);
        case 1851956301: //case short
            return(SHORT);
        case -1099899464: //case single
            return(SINGLE);
        case -1479456913: //case size
            return(SIZE);
        case -696924171: //case skip
            return(SKIP);
        case -648275712: //case sleb128
            return(SLEB128);
        case 110606196: //case space
            return(SPACE);
        case -1952923388: //case stabd
            return(STABD);
        case -1171838929: //case stabn
            return(STABN);
        case 1936206557: //case stabs
            return(STABS);
        case 1286563616: //case string
            return(STRING);
        case -1718442362: //case string16
            return(STRING16);
        case -991941735: //case string32
            return(STRING32);
        case 1405982157: //case string64
            return(STRING64);
        case 1909685154: //case string8
            return(STRING8);
        case 692932162: //case struct
            return(STRUCT);
        case 752862863: //case subsection
            return(SUBSECTION);
        case 818129942: //case symver
            return(SYMVER);
        case -2132537841: //case tag
            return(TAG);
        case 573440167: //case text
            return(TEXT);
        case -1660824767: //case type
            return(TYPE);
        case -618700695: //case uleb128
            return(ULEB128);
        case 964416672: //case val
            return(VAL);
        case 489336668: //case version
            return(VERSION);
        case -1884582530: //case vtable_entry
            return(VTABLE_ENTRY);
        case -1637790323: //case vtable_inherit
            return(VTABLE_INHERIT);
        case -601927470: //case warning
            return(WARNING);
        case -459338997: //case word
            return(WORD);
        case -877533795: //case zero
            return(ZERO);
        default:
            return(FIND_ERR);
    }
}