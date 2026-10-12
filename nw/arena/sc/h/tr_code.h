#ifndef __TR_CODE_H__
#define __TR_CODE_H__

#ifndef __HEAP_H__
#include "heap.h"
#endif

TTree *tc_ConvertToCode( TTreeHeap *heap, TTree *p, DATA_TYPE *the_type );
TTree *tc_OptimizeExprLev1( TTreeHeap *heap,
                         TTree      *p,
                         TScanner   *scanner,
                         TNameArray *nameDescr,
                         TNameDefs  *ndef );

#endif

