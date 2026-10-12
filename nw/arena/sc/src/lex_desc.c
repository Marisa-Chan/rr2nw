/*
 * File:  LEX_DESCR.C    Добор до полей структуры TName
 * Autor: Suavik
 * Ver    1.0
 * Префикс ld_
 *
 * 24.09.97 в ld_CheckNum не допускался 3-х мерный массив.
 */
#include <process.h>
#include "lex_desc.h"


static
void ld_CheckNIL( TName *descr, const char *funcname )
 {
    if( descr == NIL )
    {
         printf( "Error:Use NIL pointer in %s\n", funcname );
         exit( 0 );
    }
 }

static
void ld_CheckNum( int num, const char *funcname )
 {
    if( num < 0 ||  num > MAX_ARRAY_CNT )
    {
         printf( "%s: number of array(%i)\n", funcname, num );
         exit( 0 );
    }
 }


 /*-----------------------------------------------------------------------*/
void ld_InitTNameStart( TName *descr )
 {
    int i;

    ld_CheckNIL( descr, "ld_InitTName" );

    descr->m_name    = NIL;
    descr->m_dataPtr = 0;
    descr->m_nameDef = DEF_NONE;
    descr->m_type    = T_NONE;
    descr->m_arrayCnt= 0;

    for( i = 0; i < MAX_ARRAY_CNT ; ++i )
         descr->m_arrayRange[ i ] = 0;

    descr->m_next = NIL;
 }


 /*-----------------------------------------------------------------------*/
void ld_SetName( TName *descr, const char *name )
 {
    ld_CheckNIL( descr, "ld_SetName" );
    if( name == NIL )
    {
         printf("ld_SetName: name==NIL\n");
         exit(0);
    }
    descr->m_name = name;
 }

 /*-----------------------------------------------------------------------*/
void ld_SetDataPtr( TName *descr, int dataPtr )
 {
    ld_CheckNIL( descr, "ld_SetDataPtr" );
    descr->m_dataPtr = dataPtr;
 }

 /*-----------------------------------------------------------------------*/
void ld_SetNameDef( TName *descr, NAMEDEF_TYPE nameDef )
 {
    ld_CheckNIL( descr, "ld_SetNameDef" );
    descr->m_nameDef = nameDef;
 }


 /*-----------------------------------------------------------------------*/
void ld_SetType( TName *descr, DATA_TYPE type )
 {
    ld_CheckNIL( descr, "ld_SetType" );
    descr->m_type = type;
 }

 /*-----------------------------------------------------------------------*/
void ld_SetArrayCnt( TName *descr, int arrayCnt )
 {
    ld_CheckNIL( descr, "ld_SetArrayCnt" );
    ld_CheckNum( arrayCnt, "ld_SetArrayCnt" );
    descr->m_arrayCnt = arrayCnt;
 }

 /*-----------------------------------------------------------------------*/
void ld_SetArrayRange( TName *descr, int arrayNum, long range )
 {
    ld_CheckNIL( descr, "ld_SetArrayRange" );
    ld_CheckNum( arrayNum, "ld_SetArrayRange" );
    descr->m_arrayRange[ arrayNum ] = range;
 }

 /*-----------------------------------------------------------------------*/
void ld_SetNext( TName *descr, TName *next )
 {
    ld_CheckNIL( descr, "ld_SetNext" );
    descr->m_next = next;
 }

 /*-----------------------------------------------------------------------*/
const char *ld_GetName( TName *descr )
 {
    ld_CheckNIL( descr, "ld_GetName" );

    if( descr->m_name==NIL )
    {
         printf( "Error: descr->m_name==NIL");
         exit( 0 );
    }
    
    return descr->m_name;
 }

 /*-----------------------------------------------------------------------*/
int  ld_GetDataPtr( TName *descr )
 {
    ld_CheckNIL( descr, "ld_GetDataPtr" );
    return descr->m_dataPtr;
 }

 /*-----------------------------------------------------------------------*/
NAMEDEF_TYPE ld_GetNameDef( TName *descr )
 {
    NAMEDEF_TYPE nameDef;

    ld_CheckNIL( descr, "ld_GetNameDef" );
    nameDef = descr->m_nameDef;

    if( nameDef < DEF_NONE || nameDef >= DEF_LASTNUMBER )
    {
         printf( "ld_GetNameDef: Error type\n" );
         exit( 0 );
    }

    return descr->m_nameDef;
 }

 /*-----------------------------------------------------------------------*/
DATA_TYPE ld_GetType( TName *descr )
 {
    DATA_TYPE type;
    ld_CheckNIL( descr, "ld_GetType" );

    type = descr->m_type;

    if( type < T_NONE || type >= T_LASTNUMBER )
    {
         printf( "ld_GetType: Error type" );
         exit( 0 );
    }

    return descr->m_type;
 }

 /*-----------------------------------------------------------------------*/
int ld_GetArrayCnt( TName *descr )
 {
    ld_CheckNIL( descr, "ld_GetArrayCnt" );
    ld_CheckNum( descr->m_arrayCnt, "ld_GetArrayCnt" );
    return descr->m_arrayCnt;
 }

 /*-----------------------------------------------------------------------*/
long ld_GetArrayRange( TName *descr, int arrayNum )
 {
    ld_CheckNIL( descr, "ld_GetArrayRange" );
    ld_CheckNum( arrayNum, "ld_GetArrayRange" );
    return descr->m_arrayRange[ arrayNum ];
 }

 /*-----------------------------------------------------------------------*/
TName *ld_GetNext( TName *descr )
 {
    ld_CheckNIL( descr, "ld_GetNext" );
    return descr->m_next;
 }

 /*-----------------------------------------------------------------------*/
void ld_InitTName( TName       *descr,
                   const char  *name,
                   NAMEDEF_TYPE nameDef,
                   DATA_TYPE    type )
 {
    ld_CheckNIL( descr, "ld_TName" );

    if( name == NIL )
    {
         printf( "ld_InitTName: name==NULL\n" );
         exit(0);
    }

    ld_SetName   ( descr, name );
    ld_SetNameDef( descr, nameDef );
    ld_SetType   ( descr, type );
 }

