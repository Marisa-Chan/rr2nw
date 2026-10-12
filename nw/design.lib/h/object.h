#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <stdio.h>

#ifndef __MSG0_H__
#include "msg0.h"
#endif

#ifndef __SWIN_H__
#include "swin.h"
#endif

//#include "ctrlbrk.h"

/*************************************************************************
 * Class of transmision messages
 */

class CMessage_:public CMessageMessage
{
 public:
	enum {
		MSG_MAX_NUMBER_OF_MESSAGE = 128
	};
 private:
	static int m_msg[ MSG_MAX_NUMBER_OF_MESSAGE ];
	static int m_cnt;
	static int m_last;

 public:

	const char *SearchComFace(int comNum);
	CMessage_() { m_last = m_cnt = 0;}

	int &Cur() { return m_msg[m_last]; }

	void Set(int msg);
	int  Get();
	friend void Clr()
		{
			m_last = m_cnt = 0;
		}
	int  Empty() const { return m_cnt==0; }

	int  Ok_Expected(int msg);
	void ClearSysMessages();
};

#ifndef __OBJECT_C__
extern
#endif
CMessage_ Message;

/*************************************************************************
 * Class of transmission commands
 */
class  CCommand_:public CMessageMessage {
 public:
	enum {
		MAX_VALUE_LEN = 80
	};
 private:
	union {
		char          d_char;
		long          d_long;
		unsigned long d_unsigned;
		double        d_float;
		char         *d_string;
		const char   *d_constString;
		FILE         *d_FILE;
		void         *d_voidPtr;
	}m_data;
	CPoint            d_point;
	enum {
		TYPE_CHAR,
		TYPE_LONG,
		TYPE_UNSIGNED,
		TYPE_FLOAT,
		TYPE_STRING,
		TYPE_CONST_STRING,
		TYPE_FILE,
		TYPE_POINT,
		TYPE_VOIDPTR,
		TYPE_NONE
	};
	int m_parType;
	int m_beenRead,m_beenWrite;
	static char m_buf[MAX_VALUE_LEN];
	int  m_command;
	void ErrorLostParam();

 public:
	inline int    Command() const { return m_command; }
	inline CCommand_()      { m_command = MM_NONE;  m_parType = TYPE_NONE;m_beenRead = 0;m_beenWrite=0;}
		  ~CCommand_();
	CCommand_ &operator  = (int x);

	inline CCommand_ &operator += (char   x)	       {m_data.d_char    = x; m_parType = TYPE_CHAR; m_beenWrite=1; return *this;}
	inline CCommand_ &operator += (long   x)	       {m_data.d_long    = x; m_parType = TYPE_LONG; m_beenWrite=1;return *this;}
	inline CCommand_ &operator += (unsigned long   x)  {m_data.d_unsigned= x; m_parType = TYPE_UNSIGNED; m_beenWrite=1;return *this;}
	inline CCommand_ &operator += (double x)	       {m_data.d_float   = x; m_parType = TYPE_FLOAT; m_beenWrite=1;return *this;}
	inline CCommand_ &operator += (char * x)	       {m_data.d_string  = x; m_parType = TYPE_STRING; m_beenWrite=1;return *this;}
	inline CCommand_ &operator += (const char * x)	   {m_data.d_constString = x; m_parType = TYPE_CONST_STRING; m_beenWrite=1;return *this;}
	inline CCommand_ &operator += (FILE * x)	       {m_data.d_FILE    = x; m_parType = TYPE_FILE; m_beenWrite=1;return *this;}
	inline CCommand_ &operator += (CPoint &x)	       {d_point          = x; m_parType = TYPE_POINT; m_beenWrite=1;return *this;}
	inline CCommand_ &operator += (void * x)	       {m_data.d_voidPtr = x; m_parType = TYPE_VOIDPTR; m_beenWrite=1;return *this;}
	operator const char *   ();
	operator char *         ();
	operator char           ();
	operator short          ();
	operator unsigned short ();
	operator long           ();
	operator unsigned long  ();
	operator double         ();
	operator FILE *         ();
	operator CPoint         ();
	operator void *         ();

	void BeenCopy() { m_beenRead = 1; }
	void NoneCopy() { m_beenRead = 0; }
};

/*************************************************************************
 * Class of transmision information
 */

class CInformation_:public CMessageMessage
{
 public:
	enum {
		INF_MAX_NUMBER_OF_INFORMATION = 128
	};
 private:
	static CCommand_ m_info[ INF_MAX_NUMBER_OF_INFORMATION ];
	static int       m_cnt;
	static int       m_last;

 public:
	CInformation_() { m_last = m_cnt = 0;}

	CCommand_  &Cur() { return m_info[m_last]; }
	CCommand_  &Cur(int typ);

	void        Set(CCommand_ &info);

	CCommand_  &Get();
	CCommand_  &Get(int typ);
	void        Clr()
		{
			m_last = m_cnt = 0;
		}
	int  Empty() const { return m_cnt==0; }
};

#ifndef __OBJECT_C__
extern
#endif
CInformation_ Information;

#define CANCEL(x)      if(m_cancel) { Message.Set( MSG_BREAK_PROCESS ); x ; }
#define LOOP_CANCEL    CANCEL(break)
#define PROC_CANCEL    CANCEL(return)
#define FUNC_CANCEL(x) if(m_cancel) { Message.Set( MSG_BREAK_PROCESS );return x; }

/*************************************************************************
 * stansdart objects interface
 */
class CObject_ {
 private:
	 virtual int Do( CCommand_ &command ) { return 0;}
 public:
	static int       m_cancel;

 inline int operator ()(CCommand_ &command ) { return Do(command); }

 static char*LoadASCIIZ( FILE *f );
 static int  LoadASCIIZ( FILE *f,const char * str);
 static void SaveASCIIZ( FILE *f,const char * str);
};

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

#define ENTRY(PROC_NAME) CCallStack __entryPoint__(#PROC_NAME)

class CCallStack {
 public:
	static const char *m_procName[128];
	static int         m_procNameCnt;
	static int         m_procNamePos;
	 CCallStack(const char *procName);
	~CCallStack();
};


#endif
