#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <stdio.h>


int OAssert(const char *xMsg,
			const char *fileName,int line,
			const char *msg);

int OAssert_P(const char *xMsg,
			  const char *fileName,int line,
			  const char *msg,...);

void OStartAssert(const char *runProgram);

void OStartAssert(const char *runProgram);

#define IF(X)       ((X)
#define THEN(X)    ?(X)
#define ELSE(X)    :(X))

#define __FINFO__   __FILE__##" Date :"##__DATE__
#define __ASSERTMSG__      __FINFO__,__LINE__

#define  OASSERT(X,MSG)    IF(X) THEN(0) ELSE(OAssert(#X,__ASSERTMSG__,MSG))
#define  OASSERT_NQ(MSG)         OASSERT(0,MSG)
#define _OASSERT(X,MSG)    (void)OASSERT(X,MSG)
#define _OASSERT_NQ(MSG)   (void)OASSERT_NQ(MSG)

#define  OASSERT_P(X,MSG,P) IF(X)THEN(0)\
								 ELSE(OAssert_P(#X,__ASSERTMSG__,MSG,P))
#define  OASSERT_NQP(MSG,P)          OASSERT_P(0,MSG,P)
#define _OASSERT_P(X,MSG,P)    (void)OASSERT_P(X,MSG,P)
#define _OASSERT_NQP(MSG,P)    (void)OASSERT_NQP(MSG,P)

#define  OASSERT_PP(X,MSG,P1,P2)   IF(X)THEN(0)\
										ELSE(OAssert_P(#X,__ASSERTMSG__,MSG,P1,P2))
#define  OASSERT_NQPP(MSG,P1,P2)        OASSERT_PP(0,MSG,P1,P2)
#define _OASSERT_PP(X,MSG,P1,P2)  (void)OASSERT_PP(X,MSG,P1,P2)
#define _OASSERT_NQPP(MSG,P1,P2)  (void)OASSERT_NQPP(MSG,P1,P2)

#define  OASSERT_PPP(X,MSG,P1,P2,P3)    IF(X)THEN(0)\
											 ELSE(OAssert_P(#X,__ASSERTMSG__,MSG,P1,P2,P3))
#define  OASSERT_NQPPP(MSG,P1,P2,P3)        OASSERT_PPP(0,MSG,P1,P2,P3)
#define _OASSERT_PPP(X,MSG,P1,P2,P3)  (void)OASSERT_PPP(X,MSG,P1,P2,P3)
#define _OASSERT_NQPPP(MSG,P1,P2,P3)  (void)OASSERT_NQPPP(MSG,P1,P2,P3)

#define  OASSERT_PPPP(X,MSG,P1,P2,P3,P4) IF(X)THEN(0)\
											  ELSE(OAssert_P(#X,__ASSERTMSG__,MSG,P1,P2,P3,P4))
#define  OASSERT_NQPPPP(MSG,P1,P2,P3,P4)        OASSERT_PPPP(0,MSG,P1,P2,P3,P4)
#define _OASSERT_PPPP(X,MSG,P1,P2,P3,P4)  (void)OASSERT_PPPP(X,MSG,P1,P2,P3,P4)
#define _OASSERT_NQPPPP(MSG,P1,P2,P3,P4)  (void)OASSERT_NQPPPP(MSG,P1,P2,P3,P4)

#define OSTART_ASSERT(RUN_PROGRAMM)     OStartAssert(RUN_PROGRAMM)

#ifndef ENTRY
#define ENTRY(PROC_NAME) CCallStack __entryPoint__(#PROC_NAME); (void)__entryPoint__
#endif

class CCallStack {
 public:
	static const char *m_procName[128];
	static int         m_procNameCnt;
	static int         m_procNamePos;
	 CCallStack(const char *procName);
	~CCallStack();
};


#endif
