#ifndef __CGEN_H__
#define __CGEN_H__

#include "parser.h"
#include "tr_code.h"
#include "outstrm.h"


typedef struct {
    int          m_name;      /* const char * Имя ячейки  */
    DATA_TYPE    m_type;      /* Тип ячейки               */
    NAMEDEF_TYPE m_ndef;      /* Стековое представление   */
    int          m_stackPos;  /* Позиция в стеке          */
    int          m_next;      /* TStackCellDef            */
} TStackCellDef;

typedef struct {
    TInt           m_magic;    /* FUNCMAGIC                 */
    TInt           m_nextFunc; /* позиция следующей функции */
    TInt           m_defs;     /* TStackCellDef *;          */
    TInt           m_code;     /* TByte *                   */
    TInt           m_funcName; /* const char *              */
    DATA_TYPE      m_funcType;
} TDebugInfo;

typedef struct S_LoopItem {
    TInt               m_repPtr;
    TInt               m_contList;
    TInt               m_breakList;
    TName             *m_descr;
    struct S_LoopItem *m_next;
} TLoopItem;

typedef struct {
	os_TOutStream  *m_cs;
	os_TOutStream  *m_linkInfo;
    TError      *m_error;
    int          m_localSize;
    int          m_paramSize;
    int          m_retStackPos;
    TLoopItem   *m_loopList;
    TInt         m_linkList;
} TCGContext;

int  cg_GetParamSize( TName *descr );
void cg_InitTCGContext( TCGContext    *cgc,
						os_TOutStream *cs,
						os_TOutStream *linkInfo,
						TError      *error );
void cg_ClearTCGContext( TCGContext *cgc );
void cg_DropTCGContext ( TCGContext *cgc );
void cg_PrintLinkInfo  ( TCGContext *cgc );
void cg_CodeGen        ( TCGContext *cgc, TTree *p );
void cg_CodeGenProg    ( TCGContext *cgc, TTree *p );
void cg_DisAsm         ( os_TOutStream *cs );

#endif
/* End of CGEN.H */


