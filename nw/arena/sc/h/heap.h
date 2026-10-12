#ifndef __HEAP_H__
#define __HEAP_H__

#include <stdio.h>

#ifndef __LEX_H__
#include "lex.h"
#endif

#ifndef __LANGER_H__
#include "langer.h"
#endif

#ifndef __SCTYPES_H__
#include "sctypes.h"
#endif


#ifndef NIL
#define NIL NULL
#endif

#define DEBUG 1

//#define CONST_PI 3.14159265358979323846
#define CONST_PI 3.1415926535897932384

typedef struct S_Tree {
	TLexem         m_sy;
	struct S_Tree *m_left, *m_right;
} TTree;

typedef struct {
    TTree     *m_heap;
    TError     m_error;
    int        m_heapSize;
    int        m_heapPos;
} TTreeHeap;


typedef struct {
    TTree     *m_global;
    TName     *m_local;
    TName     *m_lastDef;
} TNameDefs;

void    h_InitTTreeHeap( TTreeHeap *heap, TTree *buf, int bufSize );
TTree **h_ProvideForDeletingTTreeHeap( TTreeHeap *heap );
void    h_ClearTTreeHeap( TTreeHeap *heap );
void    h_DropTTreeHeap ( TTreeHeap *heap );

int  h_MarkHeap( TTreeHeap *heap );
void h_ReleaseHeap( TTreeHeap *heap, int mark );

TTree *h_New  ( TTreeHeap *heap, LEX_TYPE lex, TTree *left, TTree *right );
TTree *h_NewR0( TTreeHeap *heap, LEX_TYPE lex, TTree *left );
TTree *h_NewI ( TTreeHeap *heap, long val );
TTree *h_NewF ( TTreeHeap *heap, double val );
TTree *h_New0 ( TTreeHeap *heap, LEX_TYPE lex);
TTree *h_NewV ( TTreeHeap *heap, TName *ndescr );
TTree *h_NewS ( TTreeHeap *heap, const char *str );
TTree *h_NewC_V( TTreeHeap *heap, LEX_TYPE lex, TName *descr );
TTree *h_NewA ( TTreeHeap *heap, TName *ndescr, TTree *left );
TTree **_car( TTree *p );
TTree **_cdr( TTree *p );
TTree **_cur( TTree *p );

LEX_TYPE _TypeOf   ( TTree *p );
void     _TypeSet  ( TTree *op, LEX_TYPE t );
int      h_IsConstI ( TTree *p );
int      h_IsConstF ( TTree *p );
long     h_GetConstINT  ( TTree *p );
double   h_GetConstFLOAT( TTree *p );
const char*
         h_GetConstSTR( TTree *p );
int      h_IsAdd( TTree *p );
int      h_IsSub( TTree *p );
int      h_IsSum( TTree *p );
int      h_IsLog( TTree *p );
int      h_IsMul( TTree *p );
int      h_IsDiv( TTree *p );
int      h_IsMulDiv( TTree *p );
int      h_IsFunc  ( TTree *p );
int      h_IsConst ( TTree *p );
int      h_ConstEqu( TTree *p, int val );
int      h_IsAtom( TTree *p );
void     h_MkAtom( TTree *p );
TName   *h_AddName( TError *er, TNameArray *nameDescr );

void     h_InitTNameArray    ( TNameArray *na, TName *nameAr, int max );
void     h_ClearTNameArray   ( TNameArray *na );
void     h_DropTNameArray    ( TNameArray *na );
TName  **h_ProvideForDeletingTNameArray( TNameArray *na );

TName   *h_SearchVariable( const char *name,
                           TNameDefs  *ndefs,
                           int        *isLocal,
                           TTree     **globalPtr );
const char *h_UnicalName( TError *er, TScanner *scanner, TNameArray *na );


TNameDefs  *h_InitTNameDefs( TNameDefs *ndefs,TTree *global, TName *local );
TName *h_GetLocal  ( TNameDefs *ndefs );
TTree *h_GetGlobal ( TNameDefs *ndefs );
TName *h_GetLastDef( TNameDefs *ndefs );
void   h_SetLastDef( TNameDefs *ndefs, TName *lastDef );
void   h_AddDef    ( TNameDefs *ndefs, TName *descr );

TName   *h_GetDescr( TTree *p );


#if DEBUG

int     _NamesCnt    ( TNameArray *na );
int     _NamesMaxCnt ( TNameArray *na );
TName * _NamesGetName( TNameArray *na, int n );
void    _NamesCntSet ( TNameArray *na, int nc );



#define car(x)  (*_car(x))
#define cdr(x)  (*_cdr(x))
#define cur(x)  (*_cur(x))
#define h_TypeOf(x)    _TypeOf(x)
#define h_TypeSet(x,y) _TypeSet((x),(y))


#define h_NamesCnt(x)       _NamesCnt(x)
#define h_NamesMaxCnt(x)    _NamesMaxCnt(x)
#define h_NamesGetName(x,i) _NamesGetName((x),(i))

#define h_NamesCntSet(x,i)  _NamesCntSet((x),(i))

#else

#define car(x)   ((x)->m_left)
#define cdr(x)   ((x)->m_right)
#define cur(x)   (x)
#define h_TypeOf(x)    ((x)->m_sy.m_type)
#define h_TypeSet(x,y) ((x)->m_sy.m_type = (y))

#define h_NamesCnt(x)       ((x)->m_nameCnt)
#define h_NamesMaxCnt(x)    ((x)->m_maxNameCnt)
#define h_NamesGetName(x,i) (&((x)->m_names[i]))

#define h_NamesCntSet(x,i)  (((x)->m_nameCnt) = (i))


#endif

#define h_ErrorOf(x)  (&((x)->m_error))

#endif

/* End of HEAP.H */

