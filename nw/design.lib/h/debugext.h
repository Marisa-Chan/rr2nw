
#ifndef	__DEBUGEXT_H__
#define	__DEBUGEXT_H__

#ifndef	_NOAUTOINCLUDE
	#define	_NOAUTOINCLUDE
	#if !defined(__STDIO_H) && !defined(_STDIO_H_INCLUDED)
		#include<stdio.h>
	#endif
	#if !defined(__STRING_H) && !defined(_STRING_H_INCLUDED)
		#include<string.h>
	#endif
	#ifndef	__STDTYPES_H__
		#include"stdtypes.h"
	#endif
	#undef	_NOAUTOINCLUDE
#endif


#if	defined(_DEBUG) && defined(__cplusplus)
	class CDebugCallStack {
	protected:
		static TCchar	*m_procName[128];
		static int		m_procNameCnt;
		static int		m_procNamePos;
	public:
		CDebugCallStack(TCchar *szProcName) {
			m_procName[m_procNamePos = (m_procNamePos+1)&127] = szProcName;
			if(m_procNameCnt<127) ++m_procNameCnt; }
		~CDebugCallStack() {
			m_procNamePos = (m_procNamePos+127)&127;
			if(m_procNameCnt>0) --m_procNameCnt; }
		static void Dump(FILE *f,bool bNewLines);
	};
#endif

#ifdef	__cplusplus
	extern "C" {
#endif

#ifdef	_DEBUG
	#ifndef	__DEBUGEXT_FILEMSG
		#define	__DEBUGEXT_FILEMSG	__FILE__
	#endif
	int DebugAssertionFailedD(TCchar *pszAssertion,TCchar *pszFile,int nLine,TCchar *pszMsg);
	int DebugAssertionFailedDP(TCchar *pszAssertion,TCchar *pszFile,int nLine,TCchar *pszMsg,...);
	#define	ASSERT(X)	((X)?(void)0:(void)DebugAssertionFailedD(#X,__DEBUGEXT_FILEMSG,__LINE__,0))
	#define	ASSERTMSG(X,MSG)	((X)?(void)0:(void)DebugAssertionFailedD(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG))
	#define	ASSERTMSG1(X,MSG,P1)	((X)?(void)0:(void)DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1))
	#define	ASSERTMSG2(X,MSG,P1,P2)	((X)?(void)0:(void)DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2))
	#define	ASSERTMSG3(X,MSG,P1,P2,P3)	((X)?(void)0:(void)DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3))
	#define	ASSERTMSG4(X,MSG,P1,P2,P3,P4)	((X)?(void)0:(void)DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3,P4))
	#define	ASSERTMSG5(X,MSG,P1,P2,P3,P4,P5)	((X)?(void)0:(void)DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3,P4,P5))
	#define	_ASSERT(X)	((X)?1:DebugAssertionFailedD(#X,__DEBUGEXT_FILEMSG,__LINE__,0))
	#define	_ASSERTMSG(X,MSG)	((X)?1:DebugAssertionFailedD(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG))
	#define	_ASSERTMSG1(X,MSG,P1)	((X)?1:DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1))
	#define	_ASSERTMSG2(X,MSG,P1,P2)	((X)?1:DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2))
	#define	_ASSERTMSG3(X,MSG,P1,P2,P3)	((X)?1:DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3))
	#define	_ASSERTMSG4(X,MSG,P1,P2,P3,P4)	((X)?1:DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3,P4))
	#define	_ASSERTMSG5(X,MSG,P1,P2,P3,P4,P5)	((X)?1:DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3,P4,P5))
	#define	VERIFY(X)	((X)?(void)0:(void)DebugAssertionFailedD(#X,__DEBUGEXT_FILEMSG,__LINE__,0))
	#define	VERIFYMSG(X,MSG)	((X)?(void)0:(void)DebugAssertionFailedD(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG))
	#define	VERIFYMSG1(X,MSG,P1)	((X)?(void)0:(void)DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1))
	#define	VERIFYMSG2(X,MSG,P1,P2)	((X)?(void)0:(void)DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2))
	#define	VERIFYMSG3(X,MSG,P1,P2,P3)	((X)?(void)0:(void)DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3))
	#define	VERIFYMSG4(X,MSG,P1,P2,P3,P4)	((X)?(void)0:(void)DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3,P4))
	#define	VERIFYMSG5(X,MSG,P1,P2,P3,P4,P5)	((X)?(void)0:(void)DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3,P4,P5))
	#define	_VERIFY(X)	((X)?1:DebugAssertionFailedD(#X,__DEBUGEXT_FILEMSG,__LINE__,0))
	#define	_VERIFYMSG(X,MSG)	((X)?1:DebugAssertionFailedD(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG))
	#define	_VERIFYMSG1(X,MSG,P1)	((X)?1:DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1))
	#define	_VERIFYMSG2(X,MSG,P1,P2)	((X)?1:DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2))
	#define	_VERIFYMSG3(X,MSG,P1,P2,P3)	((X)?1:DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3))
	#define	_VERIFYMSG4(X,MSG,P1,P2,P3,P4)	((X)?1:DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3,P4))
	#define	_VERIFYMSG5(X,MSG,P1,P2,P3,P4,P5)	((X)?1:DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3,P4,P5))
	#define	RTCHECK(X,MSG)	((X)?(void)0:(void)DebugAssertionFailedD(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG))
	#define	RTCHECK1(X,MSG,P1)	((X)?(void)0:(void)DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1))
	#define	RTCHECK2(X,MSG,P1,P2)	((X)?(void)0:(void)DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2))
	#define	RTCHECK3(X,MSG,P1,P2,P3)	((X)?(void)0:(void)DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3))
	#define	RTCHECK4(X,MSG,P1,P2,P3,P4)	((X)?(void)0:(void)DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3,P4))
	#define	RTCHECK5(X,MSG,P1,P2,P3,P4,P5)	((X)?(void)0:(void)DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3,P4,P5))
	#define	_RTCHECK(X,MSG)	((X)?1:DebugAssertionFailedD(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG))
	#define	_RTCHECK1(X,MSG,P1)	((X)?1:DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1))
	#define	_RTCHECK2(X,MSG,P1,P2)	((X)?1:DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2))
	#define	_RTCHECK3(X,MSG,P1,P2,P3)	((X)?1:DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3))
	#define	_RTCHECK4(X,MSG,P1,P2,P3,P4)	((X)?1:DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3,P4))
	#define	_RTCHECK5(X,MSG,P1,P2,P3,P4,P5)	((X)?1:DebugAssertionFailedDP(#X,__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3,P4,P5))
	// MSG may equal NULL
	#ifdef __cplusplus
		#define DBG_PROC_ENTRY(PROC_NAME) CDebugCallStack __entryPoint__(#PROC_NAME)
	#else
		#define	DBG_PROC_ENTRY(PROC_NAME) ((void)0)
	#endif
	void DebugWriteLog(TCchar *pszFile,int nLine,TCchar *pszMsg);
	void DebugWriteLogP(TCchar *pszFile,int nLine,TCchar *pszMsg,...);
	#define	WRITELOG(MSG)	DebugWriteLog(__DEBUGEXT_FILEMSG,__LINE__,MSG)
	#define	WRITELOG1(MSG,P1)	DebugWriteLogP(__DEBUGEXT_FILEMSG,__LINE__,MSG,P1)
	#define	WRITELOG2(MSG,P1,P2)	DebugWriteLogP(__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2)
	#define	WRITELOG3(MSG,P1,P2,P3)	DebugWriteLogP(__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3)
	#define	WRITELOG4(MSG,P1,P2,P3,P4)	DebugWriteLogP(__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3,P4)
	#define	WRITELOG5(MSG,P1,P2,P3,P4,P5)	DebugWriteLogP(__DEBUGEXT_FILEMSG,__LINE__,MSG,P1,P2,P3,P4,P5)
	#define	FLUSHLOG() FlushDebugLogFile()
	#define	IF_DEBUG(X) X
#else	// ifndef _DEBUG
	int DebugAssertionFailedR(TCchar *pszMsg);
	int DebugAssertionFailedRP(TCchar *pszMsg,...);
	#define	ASSERT(X)	((void)0)
	#define	ASSERTMSG(X,MSG)	((void)0)
	#define	ASSERTMSG1(X,MSG,P1)	((void)0)
	#define	ASSERTMSG2(X,MSG,P1,P2)	((void)0)
	#define	ASSERTMSG3(X,MSG,P1,P2,P3)	((void)0)
	#define	ASSERTMSG4(X,MSG,P1,P2,P3,P4)	((void)0)
	#define	ASSERTMSG5(X,MSG,P1,P2,P3,P4,P5)	((void)0)
	#define	_ASSERT(X)	(1)
	#define	_ASSERTMSG(X,MSG)	(1)
	#define	_ASSERTMSG1(X,MSG,P1)	(1)
	#define	_ASSERTMSG2(X,MSG,P1,P2)	(1)
	#define	_ASSERTMSG3(X,MSG,P1,P2,P3)	(1)
	#define	_ASSERTMSG4(X,MSG,P1,P2,P3,P4)	(1)
	#define	_ASSERTMSG5(X,MSG,P1,P2,P3,P4,P5)	(1)
	#define	VERIFY(X)	((void)(X))
	#define	VERIFYMSG(X,MSG)	((void)(X))
	#define	VERIFYMSG1(X,MSG,P1)	((void)(X))
	#define	VERIFYMSG2(X,MSG,P1,P2)	((void)(X))
	#define	VERIFYMSG3(X,MSG,P1,P2,P3)	((void)(X))
	#define	VERIFYMSG4(X,MSG,P1,P2,P3,P4)	((void)(X))
	#define	VERIFYMSG5(X,MSG,P1,P2,P3,P4,P5)	((void)(X))
	#define	_VERIFY(X)	MAKEBOOL(X)
	#define	_VERIFYMSG(X,MSG)	MAKEBOOL(X)
	#define	_VERIFYMSG1(X,MSG,P1)	MAKEBOOL(X)
	#define	_VERIFYMSG2(X,MSG,P1,P2)	MAKEBOOL(X)
	#define	_VERIFYMSG3(X,MSG,P1,P2,P3)	MAKEBOOL(X)
	#define	_VERIFYMSG4(X,MSG,P1,P2,P3,P4)	MAKEBOOL(X)
	#define	_VERIFYMSG5(X,MSG,P1,P2,P3,P4,P5)	MAKEBOOL(X)
	#define	RTCHECK(X,MSG)	((X)?(void)0:(void)DebugAssertionFailedR(MSG))
	#define	RTCHECK1(X,MSG,P1)	((X)?(void)0:(void)DebugAssertionFailedRP(MSG,P1))
	#define	RTCHECK2(X,MSG,P1,P2)	((X)?(void)0:(void)DebugAssertionFailedRP(MSG,P1,P2))
	#define	RTCHECK3(X,MSG,P1,P2,P3)	((X)?(void)0:(void)DebugAssertionFailedRP(MSG,P1,P2,P3))
	#define	RTCHECK4(X,MSG,P1,P2,P3,P4)	((X)?(void)0:(void)DebugAssertionFailedRP(MSG,P1,P2,P3,P4))
	#define	RTCHECK5(X,MSG,P1,P2,P3,P4,P5)	((X)?(void)0:(void)DebugAssertionFailedRP(MSG,P1,P2,P3,P4,P5))
	#define	_RTCHECK(X,MSG)	((X)?1:DebugAssertionFailedR(MSG))
	#define	_RTCHECK1(X,MSG,P1)	((X)?1:DebugAssertionFailedRP(MSG,P1))
	#define	_RTCHECK2(X,MSG,P1,P2)	((X)?1:DebugAssertionFailedRP(MSG,P1,P2))
	#define	_RTCHECK3(X,MSG,P1,P2,P3)	((X)?1:DebugAssertionFailedRP(MSG,P1,P2,P3))
	#define	_RTCHECK4(X,MSG,P1,P2,P3,P4)	((X)?1:DebugAssertionFailedRP(MSG,P1,P2,P3,P4))
	#define	_RTCHECK5(X,MSG,P1,P2,P3,P4,P5)	((X)?1:DebugAssertionFailedRP(MSG,P1,P2,P3,P4,P5))
	// MSG may equal NULL
	#define	DBG_PROC_ENTRY(PROC_NAME) ((void)0)
	#define	WRITELOG(MSG) ((void)0)
	#define	WRITELOG1(MSG,P1) ((void)0)
	#define	WRITELOG2(MSG,P1,P2) ((void)0)
	#define	WRITELOG3(MSG,P1,P2,P3) ((void)0)
	#define	WRITELOG4(MSG,P1,P2,P3,P4) ((void)0)
	#define	WRITELOG5(MSG,P1,P2,P3,P4,P5) ((void)0)
	#define	FLUSHLOG()	((void)0)
	#define	IF_DEBUG(X)
#endif

void (*SetDebugCleanupProc(void (*pCleanupProc)(void)))(void);

enum {
	DBGF_ERR_ABORT = 1,	// Abort on error
	DBGF_ERR_LOG = 2,	// Write to logfile on error
	DBGF_ERR_SOURCE = 4, // Dump source file name and line on error
	DBGF_ERR_CALLS = 8,	// Dump call stack on error
	DBGF_ERR_EXENAME = 16,	// Dump executable name on error
	DBGF_ERR_INT3 = 32, // Enable hard-coded breakpoint
	DBGF_LOG_MSG = 64,	// Enable WRITELOG
	DBGF_FLUSHLOG = 128,	// Flush logfile after each write
	DBGF_ERR_DEFAULT = DBGF_ERR_SOURCE|DBGF_ERR_INT3,
	DBGF_ERR_ALL = DBGF_ERR_SOURCE|DBGF_ERR_CALLS|DBGF_ERR_EXENAME,
	DBGF_LOG_DEFAULT = 0,	// For future compatibility
	DBGF_LOG_ALL = 0,	// For future compatibility
	DBGF_DEFAULT = DBGF_ERR_ABORT|DBGF_ERR_DEFAULT
};
dword SetDebugFlags(dword dwFlags);
dword GetDebugFlags();

#ifdef	__cplusplus
TCchar *SetDebugLogFileName(TCchar *szName,bool bAppend=FALSE);
TCchar *GetDebugLogFileName();	// Only pointer is kept for logfile names
								// Do not overwrite char buffer
#endif
void	FlushDebugLogFile();

#ifdef	__cplusplus
	}
#endif

#ifdef __cplusplus
class	CTypedObject {
	#ifdef _DEBUG
	private:
		dword	m___dwTypedObjectTag;
	protected:
		CTypedObject() { m___dwTypedObjectTag = 0xDECADACE; }
		virtual void __AssertTypeName(TCchar *szType) const {
								ASSERT(strcmp(szType,"CTypedObject")==0); }
		virtual TCchar *__TypeName() const { return "CTypedObject"; }
	public:
		void __AssertType(TCchar *szType) const {
						ASSERT(this!=NULL&&m___dwTypedObjectTag==0xDECADACE);
			 			__AssertTypeName(szType); }
	#endif	// _DEBUG
};

#ifdef _DEBUG
#define	DECLARETYPEDOBJECT(CLASS,BASECLASS)	\
	public:	\
		void CLASS::__AssertType__##CLASS(TCchar *szType) const {	\
				((CTypedObject const *)this)->__AssertType(szType); } \
	protected:	\
		virtual void __AssertTypeName(TCchar *szType) const {	\
			if(strcmp(szType,#CLASS)!=0) BASECLASS::__AssertTypeName(szType); } \
		virtual TCchar *__TypeName() const { return #CLASS; }
#define	ASSERTTYPE(X,CLASS)	((CLASS const *)&X)->__AssertType__##CLASS(#CLASS)
#define	ASSERTPTRTYPE(P,CLASS)	((CLASS const *)(P))->__AssertType__##CLASS(#CLASS)
#define	ASSERTPTRTYPE0(P,CLASS)	((P)?((CLASS const *)(P))->__AssertType__##CLASS(#CLASS):(void)0)
#else
#define	DECLARETYPEDOBJECT(CLASS,BASECLASS)
#define	ASSERTTYPE(X,CLASS)	((void)0)
#define	ASSERTPTRTYPE(P,CLASS)	((void)0)
#define	ASSERTPTRTYPE0(P,CLASS)	((void)0)
#endif

#endif	// __cplusplus

#endif	// __DEBUGEXT_H__
