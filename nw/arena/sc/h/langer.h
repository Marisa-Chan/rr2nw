#ifndef __LANGER_H__
#define __LANGER_H__

#include <setjmp.h>
#include <stdarg.h>
#ifndef __LEX_H__
#include "lex.h"
#endif

typedef struct {
  jmp_buf      m_jumper;
  char         m_msg[1024];
  char         m_fileName[MAXINCLUDEFILELEN];
  TScanner    *m_scanner;
} TError;

void lng_InitTError( TError *er, TScanner *scanner );
void lng_Error(TError *er,const char *format,...);
void lng_TextError( TError     *er,
                    int         line, int pos,
                    const char *format,
                    va_list     ap );

void lng_Assert( const char *fileName,
                 const char *acase,
                 const char *msg, ...);

#define lng_ASSERTNQ(msg)          lng_Assert(__FILE__,"",msg)
#define lng_ASSERTNQ_P(msg,p)      lng_Assert(__FILE__,"",msg,p)
#define lng_ASSERTNQ_PP(msg,p,p1)  lng_Assert(__FILE__,"",msg,p,p1)

#define lng_ASSERT(q,msg)         if(!(q)) lng_Assert(__FILE__,#q,msg)
#define lng_ASSERT_P(q,msg,p)     if(!(q)) lng_Assert(__FILE__,#q,msg,p)
#define lng_ASSERT_PP(q,msg,p,p1) if(!(q)) lng_Assert(__FILE__,#q,msg,p,p1)
#endif

/* End of LANGER.H */

