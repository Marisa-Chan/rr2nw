#ifndef __BYTECOD_H__
#define __BYTECOD_H__

enum FUNC_TYPE {
  FT_SCRIPT,
  FT_EXTERN,
  FT_CNSTEX
};


#define FUNCMAGIC           0xFACEBEDAL
#define CALL_STACK_RESERVED 2
#define COPSIZE             4
typedef enum {
    AM_NOP,
    AM_RESERVED,
    AM_RETURN,
    AM_RETURN_CLR,
    AM_JMP,
    AM_JMPIFFALSE,
    AM_JMPIFTRUE,

    AM_PUSHCI,
    AM_PUSHCF,
    AM_PUSHCS,
    AM_PUSH_ADR,

    /*-------------------------*/
   	AM_POP_VI,    AM_POP_AI,     AM_POP_VX,      AM_POP_AX,
   	AM_POP_VF,    AM_POP_AF,     AM_POP_VY,      AM_POP_AY,
   	AM_POP_V3,    AM_POP_A3,     AM_POP_VZ,      AM_POP_AZ,

    AM_POP_VI_P,  AM_POP_AI_P,   AM_POP_VX_P,    AM_POP_AX_P,
    AM_POP_VF_P,  AM_POP_AF_P,   AM_POP_VY_P,    AM_POP_AY_P,
    AM_POP_V3_P,  AM_POP_A3_P,   AM_POP_VZ_P,    AM_POP_AZ_P,

    /*-------------------------*/
    AM_PUSH_VI,   AM_PUSH_VX,    AM_PUSH_VI_P,   AM_PUSH_VX_P,
    AM_PUSH_VF,   AM_PUSH_VY,    AM_PUSH_VF_P,   AM_PUSH_VY_P,
    AM_PUSH_V3,   AM_PUSH_VZ,    AM_PUSH_V3_P,   AM_PUSH_VZ_P,

    AM_PUSH_AI,   AM_PUSH_AI_P,
    AM_PUSH_AF,   AM_PUSH_AF_P,
    AM_PUSH_A3,   AM_PUSH_A3_P,

    AM_PUSH_AX,   AM_PUSH_AX_P,
    AM_PUSH_AY,   AM_PUSH_AY_P,
    AM_PUSH_AZ,   AM_PUSH_AZ_P,

    AM_ADDF,      AM_SUBF,       AM_POWFI,
    AM_ADDI,      AM_SUBI,       AM_POWI,
    AM_ADD3,      AM_SUB3,       AM_POWF,


    AM_DIVF,      AM_MULF,       AM_MUL3,
    AM_DIVI,      AM_MULI,       AM_MUL33,
    AM_DIV3F,     AM_MULF3,

    AM_MOD,       AM_SQRT,

    AM_LSSI,      AM_LSSF,
    AM_LEQI,      AM_LEQF,
    AM_GRTI,      AM_GRTF,
    AM_GEQI,      AM_GEQF,
    AM_EQUI,      AM_EQUF,
    AM_NEQI,      AM_NEQF,

    AM_AND,
    AM_OR,

    AM_SIN,       AM_ASIN,
    AM_COS,       AM_ACOS,
    AM_TAN,       AM_ATAN,

    AM_LOG,       AM_EXP,
    AM_ABSI,      AM_ABSF,       AM_ABS3,
    AM_NEGI,      AM_NEGF,       AM_NEG3,
    AM_CIF,
    AM_CFI,
    AM_NOT,
    AM_ATAN2,
    AM_RNDI,      AM_RNDF,

    AM_GETFIELDX,
    AM_GETFIELDY,
    AM_GETFIELDZ,

    AM_LOOP,
    AM_LOOPD,
    AM_LEAVE,
    AM_LEAVED,

    AM_CALL,
    AM_CALLEXTERN,
    AM_PRINTI,
    AM_PRINTF,
    AM_PRINT3,
    AM_PRINTS,
    AM_PRINTEOL,

    AM_ADDCONSTI,
    AM_MOV_VCONSTI,
    AM_ADD_VCONSTI,

    AM_EQU3,
    AM_NEQ3,

    AM_ADD_VARI,
    AM_ADD_VARF,
    AM_SUB_VARI,
    AM_SUB_VARF,

    AM_MUL_VARI,
    AM_MUL_VARF,
    AM_DIV_VARI,
    AM_DIV_VARF,
    AM_MOD_VAR,

    AM_PUSH_CEXTERNI,
    AM_PUSH_CEXTERNF,

    AM_BOUND,
}  AM_TYPE;

#endif
/* End of BYTECOD.H */

