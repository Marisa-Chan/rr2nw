/*
   File : Suavik\E:\COM\S_debug.h
   Autor: Suavik
   Ver    3.0
 */
#include <process.h>
#include <stdio.h>
#include <conio.h>
#include "Kernel/h/s_debug.h"


static s_Console console;
s_Console *g_console=&console;

extern void Int3(void);
#pragma aux Int3 = "int 3"
 //===========================================================================
void s_Console::terminate()
 {
    getch();
    
    //exit(0);
    Int3();
 }

 //===========================================================================
void s_Console::echo( const char *format, ... )
 {
    va_list msg;

    va_start(msg,format);
    vprintf (format,msg);
    va_end  (msg);
 }

 //===========================================================================
void s_Console::echo( const char *format, va_list msg )
 {
    vprintf (format,msg);
 }



 //===========================================================================
bool s_Assert(
               s_Console  *con,
               const char *exp,
               int         line,
               const char *amsg,
               const char *format,
               ...
             )
 {
    va_list msg;


    con->echo( "ASSERT\n"
              "Expr: %s\n"
              "Line: %i\n"
              "%s\n", exp, line, amsg);

    con->echo( "CALL:\n" );

    for( int i = 0; i < con->m_depth; ++ i )
         con->echo( "%3i:  %s\n", i, con->m_stack[i] );

    va_start(msg,format);
    con->echo(format,msg);
    va_end(msg);

    con->echo("\n");
    con->terminate();

    return true;
 }

#if 0
void test0()
 {
    s_ENTRY(test2)
    s_ASSERTNQ1("sdfs %i",5);
 }

void test1()
 {
    s_ENTRY(test1)
    test0();
 }

   int i = 3, j = 4;
void main()
 {
   s_ENTRY(main::hello)
   printf("\n-----------\n");


   s_ASSERT2( i < j, "Whot? (%i < %i)", i, j );

   if( is_ASSERT2( i < j, "Whot??( %i < %i)", i, j ) )
       return;


   test1();
 }

#endif
