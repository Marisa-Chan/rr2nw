/*
   разобраться с mark и release
 */

#ifndef __LEX_H__
#define __LEX_H__

#include <stdio.h>

#ifndef __OUTSTRM_H__
#include "outstrm.h"
#endif


/****************** Опции для сканера *********************************/
enum {
	MAX_WORD_LEN       = 256,    /* Максимальная длина слова или строки */
	RESERVED_WORD_CNT  = 41,     /* Число зарезервированных слов        */
	TAB_SIZE           = 8,      /* Размер табуляции                    */
	MAX_NUM_LEN        = 50,     /* Максимальное число значимых цифр    */
	MAX_LONG_LEN       = 10,     /* Максимальное число цифр в целом     */
        MAXINCLUDEFILELEN  = 64
};

/***************** Ошибки *********************************************/
enum {
	ER_WORD_TOO_LONG,
	ER_BUFFER_OVERFLOW,
	ER_UNEXPECTED_END_OF_FILE,
	ER_STRING_TOO_LONG,
	ER_UNKNOWN_CHAR,
    ER_INCLUDE_FILE_NOT_FOUND
};

typedef enum {
	SY_EOL  = 13,                /* Код перевода строки                 */
	EOL_LEN = 2,                 /* Число символов в переводе строки    */
	SY_TAB  = 9,                 /* Код табуляции                       */
	SY_EOF  = 0,                 /* Код конца текста                    */
	SY_BECOMES=256,
	SY_COLON,   SY_LEQ,     SY_NEQ,     SY_LSS,   SY_GEQ,   SY_GRT,
	SY_PERIOD,  SY_LPARENT, SY_RPARENT, SY_TIMES, SY_SLASH, SY_POWER,
	SY_PLUS,    SY_MINUS,   SY_EQL,     SY_COMMA, SY_SEMICOLON,
	SY_LBRACK,  SY_RBRACK,  SY_TILDA,   SY_PERSENT,         SY_THETA,
	SY_SMUL,    SY_LBRACKE, SY_RBRACKE,
	SY_FLOATCONS,           SY_INTCONS, SY_WORD,
        SY_VARIABLE,
        SY_FORASSIGN,
        SY_FORTO,
	SY_STRINGCONS,          SY_BOUND,   SY_ERROR,
    SY_CONTINUE,
    SY_STR,
    SY_RNDI,    SY_RNDF,
    SY_INCLUDE,


	/* ------------ */
    SY_EMPTY,
	SY_CGR,     SY_CRG,
	SY_NEG,     SY_LVECTORITEM, SY_RVECTORITEM,
        SY_ARRAY,   /*  Если необходимо, то привести к INT */
        SY_FIELDX,  SY_FIELDY,  SY_FIELDZ,
        SY_OPERATOR,
        SY_IFELSE,
        SY_CONST,
        SY_FUNCTION,
        SY_RETURNI,
        SY_RETURNF,
        SY_RETURNV,
        SY_CALLPARAM, SY_CALLPARAMEOL,
        SY_CALLPARAMI,
        SY_CALLPARAMF,
        SY_CALLPARAMV,
        SY_CALLPARAMS,

        SY_CALLPARVAR,

        SY_CALL,
        SY_CALLEXTERN,
        SY_DROPSTACK,

	/* -----------  */
	SY_AND,     SY_OR,      SY_NOT,
	SY_SIN,     SY_COS,     SY_TAN,
	SY_ASIN,    SY_ACOS,    SY_ATAN,
	SY_ATAN2,   SY_LOG,     SY_EXP,     SY_ABS,
	SY_VAR,     SY_INT,     SY_FLOAT,   SY_VECTOR,  SY_VOID,
	SY_FUNC,    SY_LOOP,    SY_RETURN,  SY_BREAK,   SY_GOTO,
	SY_IF,      SY_THEN,    SY_ELSE,    SY_PRINT,   SY_MOD,
	SY_FOR,     SY_TO,      SY_DOWNTO,  SY_FORD,    SY_CYCLE,
    SY_PI,      SY_FORWARD, SY_EXTERN,

	CPU_HALT,

    /* PUSH Area */
    CPU_PUSH_VS,
    CPU_PUSH_AS,

	CPU_PUSH_CI,
	CPU_PUSH_CF,
	CPU_PUSH_CS,

	CPU_PUSH_VI,
	CPU_PUSH_VF,
	CPU_PUSH_VV,

	CPU_PUSH_VX,
	CPU_PUSH_VY,
	CPU_PUSH_VZ,

	CPU_PUSH_AI,
	CPU_PUSH_AF,
	CPU_PUSH_AV,

	CPU_PUSH_AX,
	CPU_PUSH_AY,
	CPU_PUSH_AZ,
    /* End of PUSH Area */

	CPU_CIF,
	CPU_CFI,

	CPU_ADDI,
	CPU_SUBI,
	CPU_MULI,
	CPU_DIVI,
        CPU_DIVVF,

	CPU_ADDF,
	CPU_SUBF,
	CPU_MULF,
	CPU_DIVF,


        CPU_FIELDX,
        CPU_FIELDY,
        CPU_FIELDZ,

        /* POP Area */
        CPU_POP_VS,
        CPU_POP_AS,

	    CPU_POP_VI,
	    CPU_POP_VF,
	    CPU_POP_VV,

	    CPU_POP_AI,
	    CPU_POP_AF,
	    CPU_POP_AV,

        CPU_POP_VX,
        CPU_POP_VY,
        CPU_POP_VZ,

        CPU_POP_AX,
        CPU_POP_AY,
        CPU_POP_AZ,
        /* End of POP Area */

	CPU_ADDV,
	CPU_SUBV,
	CPU_MULVF,
	CPU_MULFV,
	CPU_MULVV,
	CPU_MULVVV,

	CPU_MOD,
	CPU_POWI,
	CPU_POWF,
        CPU_POWFI,

	CPU_NEGI,
	CPU_NEGF,
	CPU_NEGV,

        CPU_EQUI, CPU_EQUF, CPU_EQUV,
        CPU_LEQI, CPU_LEQF,
        CPU_NEQI, CPU_NEQF, CPU_NEQV,
        CPU_LSSI, CPU_LSSF,
        CPU_GEQI, CPU_GEQF,
        CPU_GRTI, CPU_GRTF,

        CPU_LSSI0,
        CPU_LEQI0,
        CPU_GRTI0,
        CPU_GEQI0,
        CPU_EQUI0,
        CPU_NEQI0,

        CPU_LSSF0,
        CPU_LEQF0,
        CPU_GRTF0,
        CPU_GEQF0,
        CPU_EQUF0,
        CPU_NEQF0,

	CPU_SIN,  CPU_COS,  CPU_TAN,
	CPU_ASIN, CPU_ACOS, CPU_ATAN,
	CPU_LOG,  CPU_EXP,  CPU_ABSI, CPU_ABSF, CPU_ABSV,

        CPU_NOT,  CPU_AND, CPU_OR,

        CPU_BOUND,
        CPU_MOVI, CPU_MOVF, CPU_MOVV, CPU_MOVS,

        CPU_OPERATOR,
        CPU_IF,
        CPU_IFELSE,
        CPU_LOOP,
        CPU_FOR,
        CPU_FORD,
        CPU_BREAK,
        CPU_CYCLE,
        CPU_FORASSIGN,
        CPU_FORTO,
        CPU_FUNCTION,
        CPU_PRINT,
        CPU_CONTINUE,
        CPU_RETURN,
        CPU_RETURNINT,
        CPU_RETURNFLOAT,
        CPU_RETURNVECTOR,

        CPU_CALLPARAMEOL,
        CPU_CALLPARAMI,  CPU_CALLPARAMF,  CPU_CALLPARAMV,
        CPU_CALLPARVAR,
        CPU_CALLPARAMS,
        CPU_CALL,
        CPU_CALLEXTERN,
        CPU_ADD_VARI,
        CPU_ADD_VARF,
        CPU_SUB_VARI,
        CPU_SUB_VARF,

        CPU_MUL_VARI,
        CPU_MUL_VARF,
        CPU_DIV_VARI,
        CPU_DIV_VARF,
        CPU_MOD_VAR,

    CPU_RNDI,  CPU_RNDF,
    CPU_ADD_VCONSTI,
    CPU_ATAN2,
    CPU_DROPSTACK,

	CPU_ERROR_CONVERT_TYPE,

        LEX_ERROR,
        LEX_END = LEX_ERROR
} LEX_TYPE ;

typedef enum {
    T_NONE   = -1,
    T_INT    = 0,
    T_FLOAT  = 1,
    T_VECTOR = 2,
    T_LABEL  = 3,
    T_STR    = 4,

    T_LASTNUMBER
} DATA_TYPE;


typedef enum {
    DEF_NONE,
    DEF_VAR,
    DEF_FUNC,
    DEF_PARVAR,
    DEF_PAR,
    DEF_LOOPNAME,
    DEF_CONST,
    DEF_FUNCEXTERN,
    DEF_CONSTEXTERN,

    DEF_LASTNUMBER
} NAMEDEF_TYPE;



#define MAX_ARRAY_CNT 3

typedef struct S_Name {
   const char    *m_name;
   int            m_dataPtr;
   NAMEDEF_TYPE   m_nameDef;
   DATA_TYPE      m_type;
   int            m_arrayCnt;
   long           m_arrayRange[ MAX_ARRAY_CNT ];
   struct S_Name *m_next;  /* Для создания списка параметров */
} TName;


typedef struct {
   int        m_nameCnt;
   int        m_maxNameCnt;
   TName     *m_names;
} TNameArray;

typedef union {
   long          i;
   double        f;
   const char   *s;
   int           er;
   TName        *m_descr;
} TLexVal;

typedef	struct {
   LEX_TYPE     m_type;
   TLexVal      m_val;
} TLexem;


typedef struct t_TScanner {
   const char        *textPath;
   const char        *text;
   FILE              *inputFile;
   char               fileName[MAXINCLUDEFILELEN];
   struct t_TScanner *prevScanner;

   unsigned int line, pos,chptr; /* CHECKME - проверить на использование chptr */
   char         ch, och;
   os_TOutStream   m_names;
   os_TOutStream   m_strings;

   void (*m_NextChar)(struct t_TScanner *scanner);
   TLexem sy;
} TScanner;

typedef void(*TNextCharFunc)( TScanner *scanner );


void         lex_InitTScanner( TScanner *scanner,
                               char *wordBuf,   int wordBufSize,
                               char *stringBuf, int stringBufSize );
void         lex_ClearTScanner( TScanner *scanner );
void         lex_DropTScanner ( TScanner *scanner );
TByte      **lex_ProvideForDeletingNames( TScanner *scanner );
TByte      **lex_ProvideForDeletingStrings( TScanner *scanner );

int          lex_Get ( TScanner *sc );

LEX_TYPE     lex_GetType     ( const TScanner *sc );
long         lex_GetINT      ( const TScanner *sc );
double       lex_GetFLOAT    ( const TScanner *sc );
const char  *lex_GetSTR      ( const TScanner *sc );
int          lex_GetErrorCode( const TScanner *sc );
TName       *lex_GetDescr    ( const TScanner *sc );

int          lex_SizeOf  ( DATA_TYPE type );

unsigned     lex_Pos ( const TScanner *sc );
unsigned     lex_Line( const TScanner *sc );
void         lex_DetectNewStroke(os_TOutStream *os, TScanner *sc, const char st[]);
const char  *lex_ErrorToStr( int erCode );

void lex_InitScannerFromMem ( TScanner *scanner, char       *text );
void lex_InitScannerFromFile( TScanner *scanner, FILE       *f );
TScanner *lex_Include       ( TScanner *scanner, const char *fileName );
void      lex_SetIncludePath( TScanner *scanner, const char *path );
#endif


/*  End of LEX.H */



