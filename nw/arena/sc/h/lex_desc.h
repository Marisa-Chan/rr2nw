#ifndef __LEX_DESCR_H__
#define __LEX_DESCR_H__

#ifndef __HEAP_H__
#include "heap.h"
#endif


void         ld_InitTNameStart( TName *descr );

void         ld_SetName      ( TName *descr, const char *name );
void         ld_SetDataPtr   ( TName *descr, int dataPtr );
void         ld_SetNameDef   ( TName *descr, NAMEDEF_TYPE nameDef );
void         ld_SetType      ( TName *descr, DATA_TYPE type );
void         ld_SetArrayCnt  ( TName *descr, int arrayCnt );
void         ld_SetArrayRange( TName *descr, int arrayNum, long range );
void         ld_SetNext      ( TName *descr, TName *next );

const char  *ld_GetName      ( TName *descr );
int          ld_GetDataPtr   ( TName *descr );
NAMEDEF_TYPE ld_GetNameDef   ( TName *descr );
DATA_TYPE    ld_GetType      ( TName *descr );
int          ld_GetArrayCnt  ( TName *descr );
long         ld_GetArrayRange( TName *descr, int arrayNum );
TName       *ld_GetNext      ( TName *descr );

void         ld_InitTName    ( TName       *descr,
                               const char  *name,
                               NAMEDEF_TYPE nameDef,
                               DATA_TYPE    type );


#endif

/* End of LEX_DECR.H */

