
#define BIN_COMMAND_CNT 16
#define UNA_COMMAND_CNT 3


#define I(com,conv) {CPU_##com,conv}
#define CPU_ER       CPU_ERROR_CONVERT_TYPE
#define E           {CPU_ERROR_CONVERT_TYPE,0}


/*
 * integer to float   1
 * float   to integer 2
 * none               0
 * --------------------
 * result:
 * integer 1
 * float   2
 * vector  3
 * ====================
 *
 *  210
 *  ||\_ convert to... cdr()
 *  |\__ convert to... car()
 *  \___ result type + 1
 */

#define DEFBINCOM(name) const TConvert name [9]
/*                      0           1          2            3           4           5            6            7            8           */
/*                      ii         if          iv           fi          ff          fv           vi           vf           vv          */
DEFBINCOM(convADD) = { I(ADDI,100),I(ADDF,210),E,           I(ADDF,201),I(ADDF,200),E,           E,           E,           I(ADDV,300) };
DEFBINCOM(convSUB) = { I(SUBI,100),I(SUBF,210),E,           I(SUBF,201),I(SUBF,200),E,           E,           E,           I(SUBV,300) };
DEFBINCOM(convMUL) = { I(MULI,100),I(MULF,210),I(MULFV,310),I(MULF,201),I(MULF,200),I(MULFV,300),I(MULVF,301),I(MULVF,300),I(MULVV,200)};
DEFBINCOM(convDIV) = { I(DIVI,100),I(DIVF,210),I(DIVVF,310),I(DIVF,201),I(DIVF,200),I(DIVVF,300),E,           E,           E           };
DEFBINCOM(convMOD) = { I(MOD, 100),I(MOD, 102),E,           I(MOD, 120),I(MOD, 122),E,           E,           E,           E           };
DEFBINCOM(convMULV)= { E,          E,          E,           E,          E,          E,           E,           E,           I(MULVVV,300)};
DEFBINCOM(convPOW) = { I(POWI,100),I(POWFI,200),E,          I(POWF,201),I(POWF,200),E,           E,           E,           E           };
DEFBINCOM(convBND) = { I(BOUND,100),I(BOUND,102),E,         I(BOUND,120),I(BOUND,122),E,         E,           E,           E           };
DEFBINCOM(convAND) = { I(AND, 100),I(AND, 102),E,           I(AND, 120),I(AND, 122),E,           E,           E,           E           };
DEFBINCOM(convOR ) = { I(OR , 100),I(OR , 102),E,           I(OR , 120),I(OR , 122),E,           E,           E,           E           };
DEFBINCOM(convLEQ) = { I(LEQI,100),I(LEQF,110),E,           I(LEQF,101),I(LEQF,100),E,           E,           E,           E           };
DEFBINCOM(convLSS) = { I(LSSI,100),I(LSSF,110),E,           I(LSSF,101),I(LSSF,100),E,           E,           E,           E           };
DEFBINCOM(convGEQ) = { I(GEQI,100),I(GEQF,110),E,           I(GEQF,101),I(GEQF,100),E,           E,           E,           E           };
DEFBINCOM(convGRT) = { I(GRTI,100),I(GRTF,110),E,           I(GRTF,101),I(GRTF,100),E,           E,           E,           E           };
DEFBINCOM(convEQU) = { I(EQUI,100),I(EQUF,110),E,           I(EQUF,101),I(EQUF,100),E,           E,           E,           I(EQUV,100) };
DEFBINCOM(convNEQ) = { I(NEQI,100),I(NEQF,110),E,           I(NEQF,101),I(NEQF,100),E,           E,           E,           I(NEQV,100) };
DEFBINCOM(convMOV) = { I(MOVI,100),I(MOVF,202),E,           I(MOVF,101),I(MOVF,200),E,           E,           E,           I(MOVV,300) };

/*                                  0           1           2        */
/*                                  i           f           v        */
const TConvert unaNEG[3] = { I(NEGI,10) ,I(NEGF,20), I(NEGV,30) };
const TConvert unaNOT[3] = { I(NOT ,10) ,I(NOT, 12), E          };
const TConvert unaABS[3] = { I(ABSI,10) ,I(ABSF,20), I(ABSV,20) };

const TReplLex replLex[] = {
  { SY_INTCONS,   CPU_PUSH_CI, T_INT    },
  { SY_FLOATCONS, CPU_PUSH_CF, T_FLOAT  },
  { SY_STRINGCONS,CPU_PUSH_CS, T_STR    },
  { SY_OPERATOR,  CPU_OPERATOR,T_NONE   },
  { SY_LOOP,      CPU_LOOP,    T_NONE   },
  { SY_FOR,       CPU_FOR,     T_NONE   },
  { SY_FORD,      CPU_FORD,    T_NONE   },
  { SY_BREAK,     CPU_BREAK,   T_NONE   },
  { SY_CYCLE,     CPU_CYCLE,   T_NONE   },
  { SY_FUNCTION,  CPU_FUNCTION,T_NONE   },
  { SY_CONTINUE,  CPU_CONTINUE,T_NONE   },
  { SY_RETURN,    CPU_RETURN,  T_NONE   },
  { SY_PRINT,     CPU_PRINT,   T_NONE   },
  { LEX_ERROR,    LEX_ERROR,   T_NONE }
};


/* End of OPTABL.H */

