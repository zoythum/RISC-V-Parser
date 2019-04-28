#include "data.h"

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
* Utility function, takes an opcode as input and return it's family
*/
family family_finder(char *work) {
    
    if (strcmp(work, "add") == 0){
        return(r);
    } else if (strcmp(work, "addw") == 0) {
        return(r);
    } else if (strcmp(work, "addi") == 0) {
        return(i);
    } else if (strcmp(work, "addiw") == 0) {
        return(i);
    } else if (strcmp(work, "and") == 0) {
        return(r);
    } else if (strcmp(work, "andi") == 0) {
        return(i);
    } else if (strcmp(work, "auipc") == 0) {
        return(u);
    } else if (strcmp(work, "beq") == 0) {
        return(b);
    } else if (strcmp(work, "bge") == 0) {
        return(b);
    } else if (strcmp(work, "bgeu") == 0) {
        return(b);
    } else if (strcmp(work, "blt") == 0) {
        return(b);
    } else if (strcmp(work, "bltu") == 0) {
        return(b);
    } else if (strcmp(work, "bne") == 0) {
        return(b);
    } else if (strcmp(work, "jal") == 0) {
        return(j);
    } else if (strcmp(work, "jalr") == 0) {
        return(j);
    } else if (strcmp(work, "or") == 0) {
        return(r);
    } else if (strcmp(work, "ori") == 0) {
        return(i);
    } else if (strcmp(work, "sext.w") == 0) {
        return(sext);
    } else if (strcmp(work, "sll") == 0) {
        return(r);
    } else if (strcmp(work, "sllw") == 0) {
        return(r);
    } else if (strcmp(work, "slli") == 0) {
        return(i);
    } else if (strcmp(work, "slliw") == 0) {
        return(i);
    } else if (strcmp(work, "slt") == 0) {
        return(r);
    } else if (strcmp(work, "slti") == 0) {
        //not entirely sure this is correct, must be checked
        return(i);
    } else if (strcmp(work, "sltiu") == 0) {
        return(i);
    } else if (strcmp(work, "sltu") == 0) {
        return(r);
    } else if (strcmp(work, "sra") == 0) {
        return(r);
    } else if (strcmp(work, "sraw") == 0) {
        return(r);
    } else if (strcmp(work, "srai") == 0) {
        return(i);
    } else if (strcmp(work, "sraiw") == 0) {
        return(i);
    } else if (strcmp(work, "srliw") == 0) {
        return(i);
    } else if (strcmp(work, "srl") == 0) {
        return(r);
    } else if (strcmp(work, "srlw") == 0) {
        return(r);
    } else if (strcmp(work, "srli") == 0) {
        return(i);
    } else if (strcmp(work, "sub") == 0) {
        return(r);
    } else if (strcmp(work, "subw") == 0) {
        return(r);
    } else if (strcmp(work, "xor") == 0) {
        return(r);
    } else if (strcmp(work, "xori") == 0) {
        return(i);
    } else if (strcmp(work, "mul") == 0) {
        return(r);
    } else if (strcmp(work, "mulh") == 0) {
        return(r);
    } else if (strcmp(work, "mulhsu") == 0) {
        return(r);
    } else if (strcmp(work, "mulhu") == 0) {
        return(r);
    } else if (strcmp(work, "div") == 0) {
        return(r);
    } else if (strcmp(work, "divu") == 0) {
        return(r);
    } else if (strcmp(work, "rem") == 0) {
        return(r);
    } else if (strcmp(work, "remu") == 0) {
        return(r);
    } else if (strcmp(work, "mulw") == 0) {
        return(r);
    } else if (strcmp(work, "divw") == 0) {
        return(r);
    } else if (strcmp(work, "divuw") == 0) {
        return(r);
    } else if (strcmp(work, "remw") == 0) {
        return(r);
    } else if (strcmp(work, "remuw") == 0) {
        return(r);
    } else if (strcmp(work, "lr.w") == 0) {
        return(al);
    } else if (strcmp(work, "sc.w") == 0) {
        return(as);
    } else if (strcmp(work, "amoswap.w") == 0) {
        return(as);
    } else if (strcmp(work, "amoadd.w") == 0) {
        return(as);
    } else if (strcmp(work, "amoxor.w") == 0) {
        return(as);
    } else if (strcmp(work, "amoor.w") == 0) {
        return(as);
    } else if (strcmp(work, "amoand.w") == 0) {
        return(as);
    } else if (strcmp(work, "amomin.w") == 0) {
        return(as);
    } else if (strcmp(work, "amomax.w") == 0) {
        return(as);
    } else if (strcmp(work, "amominu.w") == 0) {
        return(as);
    } else if (strcmp(work, "amomaxu.w") == 0) {
        return(as);
    } else if (strcmp(work, "lr.d") == 0) {
        return(al);
    } else if (strcmp(work, "sc.d") == 0) {
        return(as);
    } else if (strcmp(work, "amoswap.d") == 0) {
        return(as);
    } else if (strcmp(work, "amoadd.d") == 0) {
        return(as);
    } else if (strcmp(work, "amoxor.d") == 0) {
        return(as);
    } else if (strcmp(work, "amoand.d") == 0) {
        return(as);
    } else if (strcmp(work, "amomin.d") == 0) {
        return(as);
    } else if (strcmp(work, "amomax.d") == 0) {
        return(as);
    } else if (strcmp(work, "amominu.d") == 0) {
        return(as);
    } else if (strcmp(work, "amomaxu.d") == 0) {
        return(as);
    } else if (strcmp(work, "lb") == 0) {
        return(i);
    } else if (strcmp(work, "lh") == 0) {
        return(i);
    } else if (strcmp(work, "lui") == 0) {
        return(u);
    } else if (strcmp(work, "lw") == 0) {
        return(i);
    } else if (strcmp(work, "sb") == 0) {
        return(s);
    } else if (strcmp(work, "sh") == 0) {
        return(s);
    } else if (strcmp(work, "sw") == 0) {
        return(s);
    } else {
        return (err);
    }
}

reg register_finder(char *in) {
    if (strcmp(in, "ra") == 0) {
        return (ra);
    } else if (strcmp(in, "sp") == 0) {
        return(sp);
    } else if (strcmp(in, "gp") == 0) {
        return(gp);
    } else if (strcmp(in, "tp") == 0) {
        return(tp);
    } else if (strcmp(in, "t0") == 0) {
        return(t0);
    } else if (strcmp(in, "t1") == 0) {
        return(t1);
    } else if (strcmp(in, "t2") == 0) {
        return(t2);
    } else if (strcmp(in, "t3") == 0) {
        return(t3);
    } else if (strcmp(in, "t4") == 0) {
        return(t4);
    } else if (strcmp(in, "t5") == 0) {
        return(t5);
    } else if (strcmp(in, "t6") == 0) {
        return(t6);
    } else if (strcmp(in, "s0") == 0) {
        return(s0);
    } else if (strcmp(in, "s1") == 0) {
        return(s1);
    } else if (strcmp(in, "s2") == 0) {
        return(s2);
    } else if (strcmp(in, "s3") == 0) {
        return(s3);
    } else if (strcmp(in, "s4") == 0) {
        return(s4);
    } else if (strcmp(in, "s5") == 0) {
        return(s5);
    } else if (strcmp(in, "s6") == 0) {
        return(s6);
    } else if (strcmp(in, "s7") == 0) {
        return(s7);
    } else if (strcmp(in, "s8") == 0) {
        return(s8);
    } else if (strcmp(in, "s9") == 0) {
        return(s9);
    } else if (strcmp(in, "s10") == 0) {
        return(s10);
    } else if (strcmp(in, "s11") == 0) {
        return(s11);
    } else if (strcmp(in, "a0") == 0) {
        return(a0);
    } else if (strcmp(in, "a1") == 0) {
        return(a1);
    } else if (strcmp(in, "a2") == 0) {
        return(a2);
    } else if (strcmp(in, "a3") == 0) {
        return(a3);
    } else if (strcmp(in, "a4") == 0) {
        return(a4);
    } else if (strcmp(in, "a5") == 0) {
        return(a5);
    } else if (strcmp(in, "a6") == 0) {
        return(a6);
    } else if (strcmp(in, "a7") == 0) {
        return(a7);
    } else if (strcmp(in, "zero") == 0) {
        return(zero);
    } else {
        printf("invalid input");
        return(ra);
    }
}