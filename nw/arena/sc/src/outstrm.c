#include <string.h>
#include "outstrm.h"

#ifndef __LANGER_H__
#include "langer.h"
#endif


#define TEST 0

#if TEST
#define lng_ASSERT(x,y) if(!(x)){printf(y);exit(0);}
#define lng_ASSERTNQ(x) printf(x);exit(1)
#include <process.h>
#include <stdio.h>

#else

#endif

typedef union {
    TInt      i;
    TFloat    f;
    TStr      str;
    TBytePtr  ptr;
    os_TFunc  func;
    TByte     packByte[8];
} TBytePack;


 /*========================================================================*/
void os_InitTOutStream( os_TOutStream *os, TByte *buf, int size, const char *name )
 {
    lng_ASSERT(buf!=NULL,"Buffer empty");
    os->m_buffer = buf;
    os->m_size   = size;
    os->m_base   = 0;
    os->m_pos    = 0;
    strcpy( os->m_streamName, name );
 }

 /*========================================================================*/
TByte **os_ProvideForDeletingTOutStream( os_TOutStream *os )
 {
    return &(os->m_buffer);
 }

 /*========================================================================*/
void os_ClearTOutStream( os_TOutStream *os )
 {
    os->m_base = 0;
    os->m_pos  = 0;
 }

 /*========================================================================*/
void os_DropTOutStream( os_TOutStream *os )
 {
    os_ClearTOutStream( os );
    os->m_size   = 0;
    os->m_buffer = NULL;
 }

 /*========================================================================*/
void os_ResetTOutStream( os_TOutStream *os )
 {
    os->m_base = os->m_pos;
    os->m_pos  = 0;
 }

 /*========================================================================*/
TInt os_CurPos( os_TOutStream *os )
 {
    return os->m_pos;
 }

 /*========================================================================*/
char *os_CurStr( os_TOutStream *os )
 {
    return (char *)&(os->m_buffer[ os->m_base+(int)os_CurPos(os) ]);
 }

 /*========================================================================*/
int os_MarkOutStream( os_TOutStream *os )
 {
    return (int)os_CurPos( os );
 }

 /*========================================================================*/
void os_ReleaseOutStream( os_TOutStream *os, int pos )
 {
    lng_ASSERT( pos>=0 && pos+os->m_base<os->m_size, "Out of range" );
    os->m_pos = pos;
 }

 /*****************
  *               *
  * Put to stream *
  *               *
  *****************/
 /*========================================================================*/
void os_PutByte( os_TOutStream *os, TInt val )
 {
    lng_ASSERT( val >= 0 && val < 256, "Byte too big" );

    if( os->m_base + os->m_pos >= os->m_size )
    {
         lng_ASSERTNQ_P( "Buffer[%s] overflow", os->m_streamName );
    }
    else os->m_buffer[ os->m_base + os->m_pos ] = (TByte)val;
    ++(os->m_pos);
 }

 /*========================================================================*/
void os_PutInt( os_TOutStream *os, TInt val )
 {
    int       i;
    TBytePack bp;

    bp.i = val;

    for( i = 0; i < sizeof(TInt); ++i )
         os_PutByte( os, _TBYTE_TO_TINT( bp.packByte[i] ));
 }

 /*========================================================================*/
void os_PutFloat( os_TOutStream *os, TFloat val )
 {
    int       i;
    TBytePack bp;

    bp.f = val;

    for( i = 0; i < sizeof(TFloat); ++i )
         os_PutByte( os, _TBYTE_TO_TINT( bp.packByte[i] ));
 }

 /*========================================================================*/
void os_PutStr( os_TOutStream *os, const char * str )
 {
     TByte c;
     int   i = 0;

     lng_ASSERT( str!=NULL, "os_PutStr" );

     do
     {
          c = (TByte)(str[i]);
          ++i;
          os_PutByte( os, _TBYTE_TO_TINT(c) );
     }while( c != 0 );
 }

 /*========================================================================*/
void os_PutPtr( os_TOutStream *os, TBytePtr val )
 {
    int       i;
    TBytePack bp;

    bp.ptr = val;

    for( i = 0; i < sizeof(TBytePtr); ++i )
         os_PutByte( os, _TBYTE_TO_TINT( bp.packByte[i] ));
 }

 /*******************
  *                 *
  * Get from stream *
  *                 *
  *******************/

 /*========================================================================*/
TByte os_GetByte( os_TOutStream *os, int pos )
 {
    if( (os->m_base + pos >= os->m_size) ||
        (pos < 0 ) )
    {
         lng_ASSERTNQ( "Buffer out of range" );
         return 0;
    }

    return os->m_buffer[ os->m_base + pos ];
 }

 /*========================================================================*/
TInt os_GetInt( os_TOutStream *os, int pos )
 {
    int       i;
    TBytePack bp;

    for( i = 0; i < sizeof(TInt); ++i )
         bp.packByte[i] = os_GetByte( os, pos+i );

    return bp.i;
 }

 /*========================================================================*/
TInt os_GetIntStep( os_TOutStream *os, TInt *pos )
 {
    TInt val = os_GetInt( os, (int)*pos );
    *pos += sizeof(TInt);
    return val;
 }

 /*========================================================================*/
TFloat os_GetFloat( os_TOutStream *os, int pos )
 {
    int       i;
    TBytePack bp;

    for( i = 0; i < sizeof(TFloat); ++i )
         bp.packByte[i] = os_GetByte( os, pos+i );

    return bp.f;
 }


 /*========================================================================*/
TBytePtr os_GetPtr( os_TOutStream *os, int pos )
 {
    int       i;
    TBytePack bp;

    for( i = 0; i < sizeof(TBytePtr); ++i )
         bp.packByte[i] = os_GetByte( os, pos+i );

    return bp.ptr;
 }

 /*========================================================================*/
TStr os_GetStr( os_TOutStream *os, TInt pos )
 {
    if( os->m_base + pos >= os->m_size || pos < 0 )
    {
         lng_ASSERTNQ( "Buffer out of range" );
         return 0;
    }

    return (TStr)&(os->m_buffer[ (int)(os->m_base + pos) ]);
 }

 /*========================================================================*/
void os_SetByte( os_TOutStream *os, int pos, TInt val )
 {
    lng_ASSERT( val >= 0 && val < 256, "Byte too big" );

    if( os->m_base + pos >= os->m_size || pos < 0 )
    {
         lng_ASSERTNQ( "Buffer out of range" );
    }
    else os->m_buffer[ os->m_base + pos ] = (TByte)val;
 }

 /*========================================================================*/
void os_SetInt( os_TOutStream *os, int pos, TInt val )
 {
    int       i;
    TBytePack bp;

    bp.i = val;

    for( i = 0; i < sizeof(TInt); ++i )
         os_SetByte( os, pos+i, _TBYTE_TO_TINT( bp.packByte[i] ));
 }

 /*========================================================================*/
void os_SetFloat( os_TOutStream *os, int pos, TFloat val )
 {
    int       i;
    TBytePack bp;

    bp.f = val;

    for( i = 0; i < sizeof(TFloat); ++i )
         os_SetByte( os, pos+i, _TBYTE_TO_TINT( bp.packByte[i] ));
 }


 /*========================================================================*/
void os_SetPtr( os_TOutStream *os, int pos, TBytePtr val )
 {
    int       i;
    TBytePack bp;

    bp.ptr = val;

    for( i = 0; i < sizeof(TBytePtr); ++i )
         os_SetByte( os, pos+i, _TBYTE_TO_TINT( bp.packByte[i] ));
 }

 /*========================================================================*/
void os_SetFunc   ( os_TOutStream *os, int pos, os_TFunc val )
 {
    int       i;
    TBytePack bp;

    bp.func = val;

    for( i = 0; i < sizeof(os_TFunc); ++i )
         os_SetByte( os, pos+i, _TBYTE_TO_TINT( bp.packByte[i] ));
 }

#if TEST
#include <string.h>

void main()
 {
     TByte buf[1024];
	 os_TOutStream os;
     const char *testStr = "Hello world";

     printf("\n--------\n\n\n");

     os_InitTOutStream( &os, buf, sizeof(buf) );

     os_PutByte( &os, 12 );
     os_PutInt ( &os, 1234 );
     os_PutFloat(&os, 3.14 );
     os_PutPtr ( &os, buf );
     os_PutStr ( &os, testStr );

     if( os_GetByte( &os, 0 ) != 12   ) printf("Error1");
     if( os_GetInt ( &os, 1 ) != 1234 ) printf("Error2");
     if( os_GetFloat( &os, 5 ) != 3.14 )printf("Error3");
     if( os_GetPtr ( &os, 13 ) != buf ) printf("Error4");
     if( strcmp( os_GetStr( &os, 17 ),testStr)!=0 ) printf("Error4");

     os_ResetTOutStream( &os );

     os_PutByte( &os, 12 );
     os_PutInt ( &os, 1234 );
     os_PutFloat(&os, 3.14 );
     os_PutPtr ( &os, buf );
     os_PutStr ( &os, testStr );

     if( os_GetByte( &os, 0 ) != 12   ) printf("Error1");
     if( os_GetInt ( &os, 1 ) != 1234 ) printf("Error2");
     if( os_GetFloat( &os, 5 ) != 3.14 )printf("Error3");
     if( os_GetPtr ( &os, 13 ) != buf ) printf("Error4");
     if( strcmp( os_GetStr( &os, 17 ),testStr)!=0 ) printf("Error4");
 }
#endif
