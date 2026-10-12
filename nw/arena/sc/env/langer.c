#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <process.h>
#include "langer.h"
#include "filesys.h" // for RTCHECK

void lng_Error( TError *er, const char *format, ... )
 {
    va_list  ap;

    va_start( ap, format );
    printf( "Module: %s\n", er->m_fileName );
    vsprintf( er->m_msg, format, ap );
    va_end( ap );
    longjmp( er->m_jumper, 1 );
 }


void lng_TextError( TError     *er,
                    int         line, int pos,
                    const char *format,
                    va_list     ap )
 {
    char buf[1024];

    printf( "Module: %s\n", er->m_fileName );
    sprintf( er->m_msg, "Line %i pos %i:\n", line, pos );
    vsprintf( buf, format, ap );
    strcat(  er->m_msg, buf );
    longjmp( er->m_jumper, 1 );
 }


void lng_InitTError( TError *er, TScanner *scanner )
 {
    er->m_scanner = scanner;
    er->m_msg[0]  = 0;
    strcpy( er->m_fileName, scanner->fileName );
 }

void lng_Assert( const char *fileName,
                 const char *acase,
                 const char *msg, ...)
 {
    va_list  ap;

    va_start( ap, msg );
    printf( "ASSERT: %s\n", acase );
    printf( "File  : %s\n", fileName );
    vprintf( msg, ap );
    printf( "\n------\n");
    va_end( ap );

    RTCHECK(0,"Suacript error");
 }
