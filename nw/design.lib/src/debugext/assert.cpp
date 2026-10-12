
#ifndef	__DEBUGEXT_CPP__
	#include<stdlib.h>
	#include<stdio.h>
	#include<stdarg.h>
	#include<string.h>
	#define _NOAUTOINCLUDE
	#include"stdtypes.h"
	#include"debugext.h"
#endif

#ifdef __NT__
	#include<windows.h>
	#include<conio.h>
#endif

// --------------------------------------------------------------------------
static	bool	bDebugAssertionFlag = FALSE;
static	void	(*pDebugCleanupProc)() = NULL;
static	dword	dwDebugFlags = DBGF_DEFAULT;
static	TCchar	*pszDebugLogFileName = "logfile";
static	FILE	*pDebugLogFile = NULL;
static	bool	bDebugLogFileAppend = FALSE;
static	char	szDebugDashes[] = "-------------------------------------------------------\n";
#ifndef __sgi
extern "C" {
	extern char **_argv;
}
#endif
// --------------------------------------------------------------------------
//#if	!defined(__NT__) || defined(_DEBUG)
static	FILE	*DebugOpenLogFile()
{
	if(!pDebugLogFile)
		pDebugLogFile = fopen(pszDebugLogFileName,
										bDebugLogFileAppend?"at":"wt");
	return	pDebugLogFile;
}
//#endif
// --------------------------------------------------------------------------
#ifdef	__WATCOMC__
	void Int3();
	#pragma aux Int3 = "int 3";
	void Int16_0();
	#pragma aux Int16_0 = \
		"mov ah,0" \
		"int 16h" \
		modify [eax]
#elif	defined(__BORLANDC__)
	#define	Int3()	__asm { int 3 }
	#define	Int16_0()	__asm { mov ah,0; int 0x16 }
#elif	defined(__sgi)
#include<assert.h>
inline void Int3() { IF_DEBUG(assert(0)); }
inline void Int16_0() {}
#endif
// --------------------------------------------------------------------------
extern "C" int
	#ifdef	_DEBUG
		DebugAssertionFailedD(TCchar *pszAssertion,TCchar *pszFile,int nLine,
															TCchar *pszMsg)
	#else
		DebugAssertionFailedR(TCchar *pszMsg)
	#endif
{
	if( dwDebugFlags & DBGF_ERR_ABORT ) {
		if(bDebugAssertionFlag) return 0;
		bDebugAssertionFlag = TRUE;
		if(pDebugCleanupProc)	pDebugCleanupProc();
	}
	FILE *f = dwDebugFlags & DBGF_ERR_LOG ? DebugOpenLogFile() : stdout;
	#ifdef	__NT__
		if( f == stdout ) {
			AllocConsole();
			freopen("CONOUT$","wt",stdout);
		}
	#endif
	if(dwDebugFlags & DBGF_ERR_LOG) fprintf(f,szDebugDashes);
	#ifndef __sgi
		if(dwDebugFlags & DBGF_ERR_EXENAME)
			fprintf(f,"Executable: %s\n",_argv[0]);
	#endif
	#ifdef _DEBUG
		if(dwDebugFlags & DBGF_ERR_SOURCE)
			fprintf(f,"Debug Assertion Failed\n"
					"File: %s\n"
					"Line: %d\n"
					"Assertion: %s\n", pszFile, nLine, pszAssertion );
		else
			fprintf(f,"Debug Assertion Failed: %s\n", pszAssertion );
		if(pszMsg)
			fprintf(f,"Message: %s\n", pszMsg );
		if(dwDebugFlags & DBGF_ERR_CALLS)
			CDebugCallStack::Dump(f,MAKEBOOL(dwDebugFlags & DBGF_ERR_LOG));
	#else
		if(!pszMsg)	pszMsg = "Unknown";
		fprintf(f,"Run-time error: %s\n", pszMsg );
	#endif
	if(dwDebugFlags & DBGF_ERR_LOG) {
		fprintf(f,szDebugDashes);
		if( dwDebugFlags & (DBGF_FLUSHLOG|DBGF_ERR_ABORT) ) fflush(f);
	}
	if( dwDebugFlags & DBGF_ERR_ABORT ) {
		if(dwDebugFlags & DBGF_ERR_LOG) {
			#ifdef _DEBUG
				printf("Debug Assertion Failed. See logfile for details\n");
			#else
				printf("Run-time error: %s\n", pszMsg );
			#endif
		}
		printf("Press any key\n");
		if( pDebugLogFile )	fclose(pDebugLogFile);
		#ifdef __NT__
			if( ! getch() ) getch();
			if( dwDebugFlags & DBGF_ERR_INT3 )	Int3();
			ExitProcess(1);
		#else
			Int16_0();
			if( dwDebugFlags & DBGF_ERR_INT3 )	Int3();
			exit(1);
		#endif
	}
	return 0;
}

extern "C" int
	#ifdef	_DEBUG
		DebugAssertionFailedDP(TCchar *pszAssertion,TCchar *pszFile,int nLine,
														TCchar *pszMsg,...)
	#else
		DebugAssertionFailedRP(TCchar *pszMsg,...)
	#endif
{
	va_list ap;
	va_start(ap,pszMsg);
	if( dwDebugFlags & DBGF_ERR_ABORT ) {
		if(bDebugAssertionFlag) return 0;
		bDebugAssertionFlag = TRUE;
		if(pDebugCleanupProc)	pDebugCleanupProc();
	}
	FILE *f = dwDebugFlags & DBGF_ERR_LOG ? DebugOpenLogFile() : stdout;
	#ifdef	__NT__
		if( f == stdout ) {
			AllocConsole();
			freopen("CONOUT$","wt",stdout);
		}
	#endif
	if(dwDebugFlags & DBGF_ERR_LOG) fprintf(f,szDebugDashes);
	#ifndef	__sgi
		if(dwDebugFlags & DBGF_ERR_EXENAME)
			fprintf(f,"Executable: %s\n",_argv[0]);
	#endif
	#ifdef _DEBUG
		if(dwDebugFlags & DBGF_ERR_SOURCE)
			fprintf(f,"Debug Assertion Failed\n"
					"File: %s\n"
					"Line: %d\n"
					"Assertion: %s\n", pszFile, nLine, pszAssertion );
		else
			fprintf(f,"Debug Assertion Failed: %s\n", pszAssertion );
		if(pszMsg) {
			fprintf(f,"Message: ");
			vfprintf(f, pszMsg, ap );
			fprintf(f,"\n");
		}
		if(dwDebugFlags & DBGF_ERR_CALLS)
			CDebugCallStack::Dump(f,MAKEBOOL(dwDebugFlags & DBGF_ERR_LOG));
	#else
		if(!pszMsg)	pszMsg = "Unknown";
		fprintf(f,"Run-time error: ");
		vfprintf(f, pszMsg, ap );
		fprintf(f,"\n");
	#endif
	if(dwDebugFlags & DBGF_ERR_LOG) {
		fprintf(f,szDebugDashes);
		if( dwDebugFlags & (DBGF_FLUSHLOG|DBGF_ERR_ABORT) ) fflush(f);
	}
	if( dwDebugFlags & DBGF_ERR_ABORT ) {
		if(dwDebugFlags & DBGF_ERR_LOG) {
			#ifdef _DEBUG
				printf("Debug Assertion Failed. See logfile for details\n");
			#else
				printf("Run-time error: ");
				vprintf( pszMsg, ap );
				printf("\n");
			#endif
		}
		printf("Press any key\n");
		if( pDebugLogFile )	fclose(pDebugLogFile);
		#ifdef __NT__
			if( ! getch() ) getch();
			if( dwDebugFlags & DBGF_ERR_INT3 )	Int3();
			ExitProcess(1);
		#else
			Int16_0();
			if( dwDebugFlags & DBGF_ERR_INT3 )	Int3();
			exit(1);
		#endif
	}
	va_end(ap);
	return 0;
}
// --------------------------------------------------------------------------
#ifdef	_DEBUG
extern "C"
void	DebugWriteLog(TCchar *pszFile,int nLine,TCchar *pszMsg)
{
	(void)pszFile; (void)nLine;
	if(!(dwDebugFlags & DBGF_LOG_MSG))	return;
	FILE *f = DebugOpenLogFile();
	ASSERT(pszMsg);
	fprintf(f,"%s",pszMsg);
	if( dwDebugFlags & DBGF_FLUSHLOG )	fflush(f);
}
extern "C"
void	DebugWriteLogP(TCchar *pszFile,int nLine,TCchar *pszMsg,...)
{
	(void)pszFile; (void)nLine;
	if(!(dwDebugFlags & DBGF_LOG_MSG))	return;
	FILE *f = DebugOpenLogFile();
	ASSERT(pszMsg);
	va_list ap;
	va_start(ap,pszMsg);
	vfprintf(f,pszMsg,ap);
	va_end(ap);
	if( dwDebugFlags & DBGF_FLUSHLOG )	fflush(f);
}
#endif
// --------------------------------------------------------------------------
extern "C"
void (*SetDebugCleanupProc(void (*pCleanupProc)(void)))(void)
{
	typedef	void (*TPCleanUpProc)(void);
	//void	(*pPreviousProc)(void) = pDebugCleanupProc;
	TPCleanUpProc	pPreviousProc = pDebugCleanupProc;
	pDebugCleanupProc = pCleanupProc;
	return	pPreviousProc;
}
// --------------------------------------------------------------------------
extern "C" dword SetDebugFlags(dword dwFlags)
{
	dword	dwOldFlags = dwDebugFlags;
	dwDebugFlags = dwFlags;
	return dwOldFlags;
}
extern "C" dword GetDebugFlags()
{
	return dwDebugFlags;
}
// --------------------------------------------------------------------------
extern "C" TCchar *SetDebugLogFileName(TCchar *pszName,bool bAppend)
{
	TCchar	*pszOldName = pszDebugLogFileName;
	pszDebugLogFileName = pszName;
	bDebugLogFileAppend = bAppend;
	if(pDebugLogFile) {
		fclose(pDebugLogFile);
		pDebugLogFile = NULL;
	}
	return pszOldName;
}
extern "C" TCchar *GetDebugLogFileName()
{
	return pszDebugLogFileName;
}
extern "C" void FlushDebugLogFile()
{
	if(pDebugLogFile)	fflush(pDebugLogFile);
}
// --------------------------------------------------------------------------
#ifdef _DEBUG
TCchar	*CDebugCallStack::m_procName[128];
int		CDebugCallStack::m_procNameCnt = 0;
int		CDebugCallStack::m_procNamePos = 0;

void CDebugCallStack::Dump(FILE *f,bool bNewLines)
{
	if(m_procNameCnt==0) {
		fprintf(f,"Call stack empty\n");
	}
	else {
		fprintf(f,bNewLines?"Call stack:\n":"Call stack: ");
		for(int i=0; i<m_procNameCnt;++i) {
			fprintf(f,bNewLines?"  >%s\n":"%s ",
				m_procName[(m_procNamePos-i+128)&127]
			);
		}
		if(!bNewLines) fprintf(f,"\n");
	}

}
#endif
