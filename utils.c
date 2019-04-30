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