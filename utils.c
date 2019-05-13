#include "utils.h"

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

/**
 * Utility function, strips the first size char from input string
 */
char *strip_front(char *input, int size) {
  char *out;
        case 1179373834:  //register is tp
            return(tp);
        case 1717065290:  //register is t0
            return(t0);
        case -1907073815:  //register is t1
            return(t1);
        case 1243464504:  //register is t2
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

/**
 * Utility function, returns as_type tipe of a single directive
 */
as_directive directive_finder(char *work) {
    int size = strlen(work);
    int value = Hash(work, size);

    switch(value) {
        case 573440167: //case text
            return(TEXT);
        case -285577657: //case align
            return(ALIGN);
        case -73750906: //case option
            return(OPTION);
        case -977229801: //case section
            return(SECTION);
        case -1762104660: //case 2byte
	        return(_2BYTE);
        case 980539621: //case 4byte
            return(_4BYTE);
        case -1720845963: //case 8byte
            return(_8BYTE);
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
