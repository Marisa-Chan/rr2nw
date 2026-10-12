#include <process.h>
#include "tr_data.h"


 /*========================================================================*/
static
void tr_CheckNIL( TTree *p, const char *funcname )
 {
    if( p == NIL )
    {
         printf( "NIL pointer in %s\n", funcname );
         exit( 0 );
    }
 }

 /*========================================================================*/
 /*
  * Взятие имени переменной в данном элементе дерева
  */
const char *tr_GetVarName( TTree *p )
 {
    tr_CheckNIL( p, "tr_GetVarName" );
    return ld_GetName( h_GetDescr( p ) );
 }

 /*========================================================================*/
 /*
  * Взятие типа переменной в данном узле дерева
  */
DATA_TYPE tr_GetVarType( TTree *p )
 {
    tr_CheckNIL( p, "tr_GetVarType" );
    return ld_GetType( h_GetDescr( p ) );
 }

 /*========================================================================*/
TTree *tr_NewPUSHCF( TTreeHeap *heap, TFloat f )
 {
    TTree *p = h_NewR0( heap, CPU_PUSH_CF, NIL );
    p->m_sy.m_val.f = f;

    return p;
 }

 /*========================================================================*/
TTree *tr_NewPUSHCI( TTreeHeap *heap, TInt i )
 {
    TTree *p = h_NewR0( heap, CPU_PUSH_CI, NIL );
    p->m_sy.m_val.i = i;

    return p;
 }

 /*========================================================================*/
void tr_CreateCI( TTree *p, TInt i )
 {
    h_TypeSet( p, CPU_PUSH_CI );
    p->m_sy.m_val.i = i;
    h_MkAtom( p );
 }

 /*========================================================================*/
void tr_CreateCF( TTree *p, TFloat f )
 {
    h_TypeSet( p, CPU_PUSH_CF );
    p->m_sy.m_val.f = f;
    h_MkAtom( p );
 }
