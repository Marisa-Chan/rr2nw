#include <stdio.h>
#include "heap.h"
#include "lex_desc.h"
#include "strequ.h"

 /*========================================================================*/
void h_InitTTreeHeap( TTreeHeap *heap,
                      TTree     *buf,
                      int        bufSize )
 {
     heap->m_heap     = buf;
     heap->m_heapSize = bufSize;
     heap->m_heapPos  = 0;
 }

 /*========================================================================*/
TTree **h_ProvideForDeletingTTreeHeap( TTreeHeap *heap )
 {
     return &(heap->m_heap);
 }

 /*========================================================================*/
void h_ClearTTreeHeap( TTreeHeap *heap )
 {
     heap->m_heapPos  = 0;
 }

 /*========================================================================*/
void h_DropTTreeHeap( TTreeHeap *heap )
 {
     heap->m_heap     = NULL;
     heap->m_heapSize = 0;
     heap->m_heapPos  = 0;
 }

 /*========================================================================*/
int h_MarkHeap( TTreeHeap *heap )
 {
    return heap->m_heapPos;
 }

 /*========================================================================*/
void h_ReleaseHeap( TTreeHeap *heap, int mrk )
 {
    heap->m_heapPos = mrk;
 }

 /*========================================================================*/
TTree *AllocTreeItem( TTreeHeap *heap )
 {
    TTree *cur;

    if( heap->m_heapPos >= heap->m_heapSize )
         lng_Error( h_ErrorOf( heap ), "Tree heap overflow" );

    cur = &(heap->m_heap[ heap->m_heapPos ]);
    ++(heap->m_heapPos);
    return cur;
 }


 /*========================================================================*/
TTree *h_New( TTreeHeap *heap, LEX_TYPE typ, TTree *left, TTree *right )
 {
    TTree *p = AllocTreeItem( heap );

    p->m_sy.m_type = typ;
    p->m_left  = left;
    p->m_right = right;
    return p;
 }

 /*========================================================================*/
TTree *h_New0( TTreeHeap *heap, LEX_TYPE lex)
 {
    return h_New( heap, lex, NIL, NIL );
 }


 /*========================================================================*/
TTree *h_NewR0( TTreeHeap *heap, LEX_TYPE lex, TTree *left )
 {
    return h_New( heap, lex, left, NIL );
 }


 /*========================================================================*/
void h_MkAtom( TTree *p )
 {
    lng_ASSERT(p!=NIL,"h_MkAtom");
    car(p) = NIL;
    cdr(p) = NIL;
 }


 /*========================================================================*/
TTree *h_NewI( TTreeHeap *heap, long val )
 {
    TTree *p = AllocTreeItem( heap );

    p->m_sy.m_type  = SY_INTCONS;
    p->m_sy.m_val.i = val;
    h_MkAtom( p );

    return p;
 }

 /*========================================================================*/
TTree *h_NewF( TTreeHeap *heap, double val )
 {
    TTree *p = AllocTreeItem( heap );

    p->m_sy.m_type  = SY_FLOATCONS;
    p->m_sy.m_val.f = val;
    h_MkAtom(p);

    return p;
 }

 /*========================================================================*/
TTree *h_NewC_V( TTreeHeap *heap, LEX_TYPE lex, TName *ndescr )
 {
    TTree *p = AllocTreeItem( heap );

    p->m_sy.m_type = lex;
    p->m_sy.m_val.m_descr = ndescr;
    h_MkAtom( p );

    return p;
 }

 /*========================================================================*/
TTree *h_NewV( TTreeHeap *heap, TName *descr )
 {
    return h_NewC_V( heap, SY_VARIABLE, descr );
 }


 /*========================================================================*/
TTree *h_NewS( TTreeHeap *heap, const char *str )
 {
    TTree *p = AllocTreeItem( heap );

    p->m_sy.m_type  = SY_STRINGCONS;
    p->m_sy.m_val.s = str;
    h_MkAtom(p);

    return p;
 }

 /*========================================================================*/
TTree *h_NewA( TTreeHeap *heap, TName *ndescr, TTree *left )
 {
    TTree *p = AllocTreeItem( heap );

    p->m_sy.m_type = SY_ARRAY;
    p->m_sy.m_val.m_descr = ndescr;
    p->m_left  = left;
    p->m_right = NIL;

    return p;
 }


 /*========================================================================*/
TTree **_car( TTree *p )
 {
    if( p == NIL )
    {
         lng_ASSERTNQ("car(NIL)");
    }

    return &(p->m_left);
 }

 /*========================================================================*/
TTree **_cdr( TTree *p )
 {
    if( p == NIL )
    {
         lng_ASSERTNQ("cdr(NIL)");
    }

    return &(p->m_right);
 }

  /*********************
   *                   *
   * Service functions *
   *                   *
   *********************/

 /*========================================================================*/
TName   *h_GetDescr( TTree *p )
 {
    if( p == NIL )
    {
         lng_ASSERTNQ("h_GetDescr(NIL)");
    }

    switch( h_TypeOf(p) )
    {
    case SY_VARIABLE:
    case CPU_POP_VI:
    case CPU_POP_VF:
    case CPU_POP_VV:

    case CPU_PUSH_VI:
    case CPU_PUSH_VF:
    case CPU_PUSH_VV:

    case CPU_POP_AI:
    case CPU_POP_AF:
    case CPU_POP_AV:

    case CPU_PUSH_AI:
    case CPU_PUSH_AF:
    case CPU_PUSH_AV:

    case CPU_PUSH_VX:
    case CPU_PUSH_VY:
    case CPU_PUSH_VZ:

    case CPU_PUSH_AX:
    case CPU_PUSH_AY:
    case CPU_PUSH_AZ:

    case CPU_POP_VX:
    case CPU_POP_VY:
    case CPU_POP_VZ:

    case CPU_POP_AX:
    case CPU_POP_AY:
    case CPU_POP_AZ:

    case CPU_PUSH_VS:
    case CPU_POP_VS:

    case CPU_PUSH_AS:
    case CPU_POP_AS:

    case SY_ARRAY:
            break;

    default:
         lng_ASSERTNQ("h_GetDescr: this not var");
    }

    return p->m_sy.m_val.m_descr;
 }

 /*========================================================================*/
LEX_TYPE _TypeOf( TTree *p )
 {
    if( p == NIL )
    {
         lng_ASSERTNQ("Error TypeOf");
    }

    return p->m_sy.m_type;
 }

 /*========================================================================*/
void _TypeSet( TTree *p, LEX_TYPE t )
 {
    if( p == NIL )
    {
        lng_ASSERTNQ("Error TypeSet");
    }

    p->m_sy.m_type = t;
 }


 /*========================================================================*/
int h_IsConstI( TTree *p )
 {
    if( p == NIL )
         return 0;
    return h_TypeOf(p) == CPU_PUSH_CI;
 }


 /*========================================================================*/
int h_IsConstF(TTree *p)
 {
   return h_TypeOf(p) == CPU_PUSH_CF;
 }


 /*========================================================================*/
long h_GetConstINT( TTree *p )
 {
    if( p == NIL ||
        ( p->m_sy.m_type != CPU_PUSH_CI && p->m_sy.m_type != SY_INTCONS ) )
    {
         lng_ASSERTNQ("Error CPUGetInt");
    }

    return p->m_sy.m_val.i;
 }


 /*========================================================================*/
double h_GetConstFLOAT( TTree *p )
 {
    if( p == NIL ||
        ( p->m_sy.m_type != CPU_PUSH_CF && p->m_sy.m_type != SY_FLOATCONS ))
    {
         lng_ASSERTNQ("Error h_GetConstFLOAT");
    }

    return p->m_sy.m_val.f;
 }

 /*========================================================================*/
const char *h_GetConstSTR( TTree *p )
 {
    if( p == NIL ||
        ( p->m_sy.m_type != CPU_PUSH_CS && p->m_sy.m_type != SY_STRINGCONS ))
    {
         lng_ASSERTNQ("Error h_GetConstSTR");
    }

    return p->m_sy.m_val.s;
 }

 /*========================================================================*/
int  h_IsAdd( TTree *p )
 {
    LEX_TYPE type = h_TypeOf( p );
    return type == CPU_ADDI || type == CPU_ADDF;
 }

 /*========================================================================*/
int  h_IsSub( TTree *p )
 {
    LEX_TYPE type = h_TypeOf( p );
    return type == CPU_SUBI || type == CPU_SUBF;
 }


 /*========================================================================*/
int  h_IsSum( TTree *p )
 {
    return h_IsAdd( p ) || h_IsSub( p );
 }

 /*========================================================================*/
int  h_IsLog( TTree *p )
 {
    LEX_TYPE type = h_TypeOf( p );
    return type == CPU_AND || type == CPU_OR;
 }

 /*========================================================================*/
int  h_IsMul( TTree *p )
 {
     LEX_TYPE type = h_TypeOf( p );
     return type == CPU_MULF || type == CPU_MULI;
 }

 /*========================================================================*/
int  h_IsDiv( TTree *p )
 {
     LEX_TYPE type = h_TypeOf( p );
     return type == CPU_DIVF || type == CPU_DIVI;
 }

 /*========================================================================*/
int  h_IsMulDiv( TTree *p )
 {
     return h_IsMul( p ) || h_IsDiv( p );
 }

 /*========================================================================*/
int  h_IsConst( TTree *p )
 {
     LEX_TYPE type = h_TypeOf( p );
     return type == CPU_PUSH_CI || type == CPU_PUSH_CF;
 }

 /*========================================================================*/
int  h_ConstEqu(TTree *p,int val)
 {
    switch( h_TypeOf( p ) )
    {
    case CPU_PUSH_CI:     return p->m_sy.m_val.i == val;
    case CPU_PUSH_CF:     return p->m_sy.m_val.f == val;
    }
    return 0;
 }

 /*========================================================================*/
int h_IsAtom(TTree *p)
 {
    if( p == NIL ) return 1;
    return car(p) == NIL && cdr(p) == NIL;
 }

 /*========================================================================*/
int h_IsFunc( TTree *p )
 {
    LEX_TYPE type = h_TypeOf(p);

    return type == CPU_CFI   || type == CPU_CIF  ||
           type == CPU_BOUND ||
           type == CPU_SIN   || type == CPU_COS  ||
           type == CPU_ASIN  || type == CPU_ACOS ||
           type == CPU_TAN   || type == CPU_ATAN ||
           type == CPU_LOG   || type == CPU_EXP  ||
           type == CPU_ABSI  || type == CPU_ABSF;
 }


 /****************************
  *                          *
  *  Work with variable data *
  *                          *
  ****************************/

#ifdef DEBUG


 /***********************
  *                     *
  * Variables list util *
  *                     *
  ***********************/


 /*========================================================================*/
int _NamesCnt( TNameArray *na )
 {
    return na->m_nameCnt;
 }

 /*========================================================================*/
int _NamesMaxCnt( TNameArray *na )
 {
    return na->m_maxNameCnt;
 }

 /*========================================================================*/
TName * _NamesGetName( TNameArray *na, int n )
 {
    if( n < 0 || n >= h_NamesCnt( na ) )
    {
         lng_ASSERTNQ("Error:_NamesGetName name index");
    }

    return &(na->m_names[ n ]);
 }

 /*========================================================================*/
void _NamesCntSet( TNameArray *na, int nc )
 {
    if( nc < h_NamesMaxCnt( na ) )
    {
         na->m_nameCnt = nc;
         return;
    }
    lng_ASSERTNQ("_NamesCntSet: Names buffer overflow");
 }


#endif


 /*========================================================================*/
TName *h_SearchVariable( const char *name,
                         TNameDefs  *ndefs,
                         int        *isLocal,
                         TTree     **globalPtr )
 {
    TName *funcDescr = h_GetLocal( ndefs );
    TTree *p         = h_GetGlobal( ndefs );

    *isLocal = 0;

    /*
     * Поиск локольного имени
     */
    while( funcDescr != NIL )
    {
         if( str_StrEQU( name, ld_GetName( funcDescr ) ) )
         {
             *isLocal = 1;
             return funcDescr;
         }

         funcDescr = ld_GetNext( funcDescr );
    }

    /*
     * Поск глобального имени
     */
    while( p != NIL )
    {
         switch( h_TypeOf( p ) )
         {
         case SY_VARIABLE: /*  constant */
                 if( str_StrEQU( name, ld_GetName( h_GetDescr( p ) ) ) )
                 {
                      if( globalPtr != NIL )
                           *globalPtr = p;
                      return h_GetDescr(p);
                 }
                 break;

         case CPU_FUNCTION:
         case SY_FUNCTION:
         case SY_FORWARD:
         case SY_EXTERN:
                 if( str_StrEQU( name, ld_GetName( h_GetDescr( car(p) ) ) ) )
                 {
                      if( globalPtr != NIL )
                           *globalPtr = p;
                      return h_GetDescr( car(p) );
                 }
                 break;
         }

         p = cdr(p);
    }

    return NIL;
 }

 /*========================================================================*/
TName *h_SearchAllName( const char *name, TNameArray *nameDescr )
 {
    int   i;
    TName *names = nameDescr->m_names;

    for( i = 0; i < nameDescr->m_nameCnt ; ++i, names += 1 )
         if( str_StrEQU( name, ld_GetName( names ) ) )
             return names;


    return NIL;
 }


 /*========================================================================*/
const char *h_UnicalName( TError *er, TScanner *scanner, TNameArray *na )
 {
    int  i;
    char buf[10];

    for( i = 0 ; i < 32000 ; ++i )
    {
         sprintf( buf, "l_%04x", i );

         if( h_SearchAllName( buf, na ) == NIL )
         {
              lex_DetectNewStroke(&(scanner->m_names), scanner, buf );
              if( lex_GetType( scanner ) == ER_BUFFER_OVERFLOW )
                   lng_Error( er, "Names buffer overflow" );

              if( lex_GetSTR( scanner )==NIL )
                   lng_Error( er, "h_UnicaName: lUnknown error" );

              return lex_GetSTR( scanner );
         }
    }

    lng_Error( er, "Can't make unical name" );

    return NIL;
 }


 /*========================================================================*/
void h_InitTNameArray( TNameArray *na, TName *nameAr, int max )
 {
    na->m_maxNameCnt = max; /* FIXME - поправить имя */
    h_NamesCntSet( na, 0 );
    na->m_names = nameAr;
 }

 /*========================================================================*/
TName **h_ProvideForDeletingTNameArray( TNameArray *na )
 {
    return &(na->m_names);
 }

 /*========================================================================*/
void h_ClearTNameArray( TNameArray *na )
 {
    na->m_nameCnt  = 0;
 }

 /*========================================================================*/
void h_DropTNameArray( TNameArray *na )
 {
    h_ClearTNameArray( na );
    na->m_maxNameCnt = 0;
    na->m_names      = NULL;
 }

 /*========================================================================*/
void CheckNdefsNIL( TNameDefs *ndefs, const char *fname )
 {
    if( ndefs == NIL )
    {
         lng_ASSERTNQ_P( "Error: ndefs == NIL in func %s\n", fname );
    }
 }

 /*========================================================================*/
TNameDefs  *h_InitTNameDefs( TNameDefs *ndefs,
                             TTree     *global,
                             TName     *local )
 {
    CheckNdefsNIL( ndefs, "h_InitTNameDefs" );
    ndefs->m_local   = local;
    ndefs->m_global  = global;
    ndefs->m_lastDef = NIL;
    return ndefs;
 }

 /*========================================================================*/
TName *h_GetLocal( TNameDefs *ndefs )
 {
    CheckNdefsNIL( ndefs, "h_GetLocal" );
    return ndefs->m_local;
 }

 /*========================================================================*/
TTree *h_GetGlobal( TNameDefs *ndefs )
 {
    CheckNdefsNIL( ndefs, "h_GetGlobal" );
    return ndefs->m_global;
 }

 /*========================================================================*/
TName *h_GetLastDef( TNameDefs *ndefs )
 {
    CheckNdefsNIL( ndefs, "h_GetLastDef" );
    return ndefs->m_lastDef;
 }

 /*========================================================================*/
void h_SetLastDef( TNameDefs *ndefs, TName *lastDef )
 {
    CheckNdefsNIL( ndefs, "h_SetLastDef" );
    ndefs->m_lastDef = lastDef;
 }

 /*========================================================================*/
void h_AddDef( TNameDefs *ndefs, TName *descr )
 {
    CheckNdefsNIL( ndefs, "h_AddDef" );

    if( ndefs->m_lastDef ==  NIL )
         ndefs->m_lastDef = descr;
    else
    {
         ndefs->m_lastDef->m_next = descr;
         ndefs->m_lastDef = descr;
    }
 }

 /*========================================================================*/
TName *h_AddName( TError *er, TNameArray *nameDescr )
 {
    if( h_NamesCnt( nameDescr ) < h_NamesMaxCnt( nameDescr ) )
    {
         TName *descr;

         h_NamesCntSet( nameDescr, h_NamesCnt( nameDescr ) + 1 );
         descr = h_NamesGetName( nameDescr, h_NamesCnt( nameDescr ) - 1);
         ld_InitTNameStart( descr );

         return descr;
    }

    lng_Error( er, "Buffer of names overflow" );

    return NIL;
 }


#ifdef TEST_HEAP
#include <stdio.h>

void Out( TTree *c )
 {
    if( c != NIL )
    {
         Out(c->left);

         switch( c->sy.type )
         {
         case SY_INTCONS:   printf( "%li\n", c->sy.val.i ); break;
         case SY_FLOATCONS: printf( "%lf\n", c->sy.val.f ); break;
         case SY_PLUS:      printf( "+\n" ); break;
         case SY_MINUS:     printf( "-\n" ); break;
         }

         Out( c->right );
    }
 }

int main(void)
 {
    char buf[1000];

    h_HeapInit(buf,1000);

    Out(
	    h_New(SY_PLUS,h_New(SY_MINUS,h_NewI(123,NIL,NIL),h_NewF(567,NIL,NIL)),
				    h_NewI(234,NIL,NIL))
    );
    return 0;
 }
#endif
