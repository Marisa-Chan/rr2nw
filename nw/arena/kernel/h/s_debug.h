/*
   Fild : Suavik\E:\COM\S_debug.h
   Autor: Suavik
   Ver    3.0

   class
   -------------------------------------------------------------------
    s_Console
    s_CallStack
 */
#ifndef __S_DEBUG_H__
#define __S_DEBUG_H__

#include <stdarg.h>

#ifndef __S_SET_H__
#include "kernel/h/s_set.h"
#endif

 //---------------------------------------------------------------------------
class s_Console
 {
 friend class s_CallStack;
 friend
        bool s_Assert(
               s_Console  *con,
               const char *exp,
               int         line,
               const char *amsg,
               const char *format,
               ...
             );

 protected:
    enum {
      STACK_SIZE = 2048
    };
    const char *m_stack[2048];
    int         m_depth;

 public:
    s_Console()
    {
       m_depth = 0;
    }
    virtual void terminate();
    virtual void echo( const char *format, ... );
    virtual void echo( const char *format, va_list msg );
 };

extern s_Console *g_console;

 //---------------------------------------------------------------------------
class s_CallStack
 {
 public:
   s_CallStack       ( const char *func );
  ~s_CallStack       ();
 };

inline s_CallStack::s_CallStack( const char *func )
 {
    g_console->m_stack[ g_console->m_depth ] = func;
    if(  g_console->m_depth < s_Console::STACK_SIZE-1  )
         g_console->m_depth++;
 }

inline s_CallStack::~s_CallStack()
 {
    if(  g_console->m_depth > 0  )
         g_console->m_depth--;
 }



bool s_Assert(
               s_Console  *con,
               const char *exp,
               int         line,
               const char *amsg,
               const char *format,
               ...
             );

 //***************************************************************************
#if s_ENABLE_ASSERT >= 1

#define __FINFO__          __FILE__##" Date :"##__DATE__
#define __ASSERTMSG__      __LINE__,__FINFO__


#define is_ASSERT(x,a)            ((x)?0:s_Assert(g_console,#x,__ASSERTMSG__,a))
#define is_ASSERT1(x,f,a)         ((x)?0:s_Assert(g_console,#x,__ASSERTMSG__,f,a))
#define is_ASSERT2(x,f,a,b)       ((x)?0:s_Assert(g_console,#x,__ASSERTMSG__,f,a,b))
#define is_ASSERT3(x,f,a,b,c)     ((x)?0:s_Assert(g_console,#x,__ASSERTMSG__,f,a,b,c))
#define is_ASSERT4(x,f,a,b,c,d)   ((x)?0:s_Assert(g_console,#x,__ASSERTMSG__,f,a,b,c,d))
#define is_ASSERT5(x,f,a,b,c,d,e) ((x)?0:s_Assert(g_console,#x,__ASSERTMSG__,f,a,b,c,d,e))

#define s_ASSERT(x,a)             if(!(x))s_Assert(g_console,#x,__ASSERTMSG__,a)
#define s_ASSERT1(x,f,a)          if(!(x))s_Assert(g_console,#x,__ASSERTMSG__,f,a)
#define s_ASSERT2(x,f,a,b)        if(!(x))s_Assert(g_console,#x,__ASSERTMSG__,f,a,b)
#define s_ASSERT3(x,f,a,b,c)      if(!(x))s_Assert(g_console,#x,__ASSERTMSG__,f,a,b,c)
#define s_ASSERT4(x,f,a,b,c,d)    if(!(x))s_Assert(g_console,#x,__ASSERTMSG__,f,a,b,c,d)
#define s_ASSERT5(x,f,a,b,c,d,e)  if(!(x))s_Assert(g_console,#x,__ASSERTMSG__,f,a,b,c,d,e)

#define s_ASSERTNQ(a)             s_Assert(g_console,"",__ASSERTMSG__,a)
#define s_ASSERTNQ1(f,a)          s_Assert(g_console,"",__ASSERTMSG__,f,a)
#define s_ASSERTNQ2(f,a,b)        s_Assert(g_console,"",__ASSERTMSG__,f,a,b)
#define s_ASSERTNQ3(f,a,b,c)      s_Assert(g_console,"",__ASSERTMSG__,f,a,b,c)
#define s_ASSERTNQ4(f,a,b,c,d)    s_Assert(g_console,"",__ASSERTMSG__,f,a,b,c,d)
#define s_ASSERTNQ5(f,a,b,c,d,e)  s_Assert(g_console,"",__ASSERTMSG__,f,a,b,c,d,e)


#define s_DECLARE_CONSOLE(type,con)    type con;

#define is_LASSERT(con,x,a)            ((x)?0:s_Assert(con,#x,__ASSERTMSG__,a))
#define is_LASSERT1(con,x,f,a)         ((x)?0:s_Assert(con,#x,__ASSERTMSG__,f,a))
#define is_LASSERT2(con,x,f,a,b)       ((x)?0:s_Assert(con,#x,__ASSERTMSG__,f,a,b))
#define is_LASSERT3(con,x,f,a,b,c)     ((x)?0:s_Assert(con,#x,__ASSERTMSG__,f,a,b,c))
#define is_LASSERT4(con,x,f,a,b,c,d)   ((x)?0:s_Assert(con,#x,__ASSERTMSG__,f,a,b,c,d))
#define is_LASSERT5(con,x,f,a,b,c,d,e) ((x)?0:s_Assert(con,#x,__ASSERTMSG__,f,a,b,c,d,e))

#define s_LASSERT(con,x,a)             if(!(x))s_Assert(con,#x,__ASSERTMSG__,a)
#define s_LASSERT1(con,x,f,a)          if(!(x))s_Assert(con,#x,__ASSERTMSG__,f,a)
#define s_LASSERT2(con,x,f,a,b)        if(!(x))s_Assert(con,#x,__ASSERTMSG__,f,a,b)
#define s_LASSERT3(con,x,f,a,b,c)      if(!(x))s_Assert(con,#x,__ASSERTMSG__,f,a,b,c)
#define s_LASSERT4(con,x,f,a,b,c,d)    if(!(x))s_Assert(con,#x,__ASSERTMSG__,f,a,b,c,d)
#define s_LASSERT5(con,x,f,a,b,c,d,e)  if(!(x))s_Assert(con,#x,__ASSERTMSG__,f,a,b,c,d,e)

#define s_LASSERTNQ(con,a)             s_Assert(con,"",__ASSERTMSG__,a)
#define s_LASSERTNQ1(con,f,a)          s_Assert(con,"",__ASSERTMSG__,f,a)
#define s_LASSERTNQ2(con,f,a,b)        s_Assert(con,"",__ASSERTMSG__,f,a,b)
#define s_LASSERTNQ3(con,f,a,b,c)      s_Assert(con,"",__ASSERTMSG__,f,a,b,c)
#define s_LASSERTNQ4(con,f,a,b,c,d)    s_Assert(con,"",__ASSERTMSG__,f,a,b,c,d)
#define s_LASSERTNQ5(con,f,a,b,c,d,e)  s_Assert(con,"",__ASSERTMSG__,f,a,b,c,d,e)

#define s_ENTRY(name) s_CallStack __add_called_function_name(#name);

#else
 //***************************************************************************

#define is_ASSERT(x,a)            (!(x))
#define is_ASSERT1(x,f,a)         (!(x))
#define is_ASSERT2(x,f,a,b)       (!(x))
#define is_ASSERT3(x,f,a,b,c)     (!(x))
#define is_ASSERT4(x,f,a,b,c,d)   (!(x))
#define is_ASSERT5(x,f,a,b,c,d,e) (!(x))

#define s_ASSERT(x,a)             (void)0
#define s_ASSERT1(x,f,a)          (void)0
#define s_ASSERT2(x,f,a,b)        (void)0
#define s_ASSERT3(x,f,a,b,c)      (void)0
#define s_ASSERT4(x,f,a,b,c,d)    (void)0
#define s_ASSERT5(x,f,a,b,c,d,e)  (void)0

#define s_ASSERTNQ(a)             (void)0
#define s_ASSERTNQ1(f,a)          (void)0
#define s_ASSERTNQ2(f,a,b)        (void)0
#define s_ASSERTNQ3(f,a,b,c)      (void)0
#define s_ASSERTNQ4(f,a,b,c,d)    (void)0
#define s_ASSERTNQ5(f,a,b,c,d,e)  (void)0


#define s_DECLARE_CONSOLE(type,con)

#define is_LASSERT(con,x,a)            (!(x))
#define is_LASSERT1(con,x,f,a)         (!(x))
#define is_LASSERT2(con,x,f,a,b)       (!(x))
#define is_LASSERT3(con,x,f,a,b,c)     (!(x))
#define is_LASSERT4(con,x,f,a,b,c,d)   (!(x))
#define is_LASSERT5(con,x,f,a,b,c,d,e) (!(x))

#define s_LASSERT(con,x,a)             (void)0
#define s_LASSERT1(con,x,f,a)          (void)0
#define s_LASSERT2(con,x,f,a,b)        (void)0
#define s_LASSERT3(con,x,f,a,b,c)      (void)0
#define s_LASSERT4(con,x,f,a,b,c,d)    (void)0
#define s_LASSERT5(con,x,f,a,b,c,d,e)  (void)0

#define s_LASSERTNQ(a)                 (void)0
#define s_LASSERTNQ1(f,a)              (void)0
#define s_LASSERTNQ2(f,a,b)            (void)0
#define s_LASSERTNQ3(f,a,b,c)          (void)0
#define s_LASSERTNQ4(f,a,b,c,d)        (void)0
#define s_LASSERTNQ5(f,a,b,c,d,e)      (void)0

#define s_ENTRY(name)

#endif

 //***************************************************************************


#endif

