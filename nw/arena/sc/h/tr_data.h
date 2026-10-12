#ifndef __TR_DATA_H__
#define __TR_DATA_H__


#ifndef __LEX_DESCR_H__
#include "lex_desc.h"
#endif

#ifndef __HEAP_H__
#include "heap.h"
#endif


const char *tr_GetVarName( TTree *p );
DATA_TYPE   tr_GetVarType( TTree *p );
TTree      *tr_NewPUSHCF( TTreeHeap *heap, TFloat f );
TTree      *tr_NewPUSHCI( TTreeHeap *heap, TInt i );
void        tr_CreateCI( TTree *p, TInt i );
void        tr_CreateCF( TTree *p, TFloat f );

#endif

/* End of TR_DATA.H  */
