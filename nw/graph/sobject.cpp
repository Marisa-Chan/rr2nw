#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define __OBJECT_C__
#include "sobject.h"

#include "ctrlbrk.h"

#define ASSERT_TERMINATE 1
#define RESERVED_MEMORY (1024*10)


#ifdef ASSERT_TERMINATE
#include <assert.h>
#endif

//***************************************************************************

static char revisionFile[129],progName[129];
static int  firstErr = 1;
char *reservedMem;

static void FirstMessage()
 {
	FILE *f = fopen(revisionFile,"a+t");
	if(f==NULL) {
		delete reservedMem;
		f = fopen(revisionFile,"a+t");
	}

	if(f!=NULL) {
		fprintf(f,"\n----------------------------------------\nProgramm: %s\n",progName);
		fclose(f);
	}
	firstErr = 0;
 }

void OStartAssert(const char *runProgram)
 {
	int len = strlen(runProgram);

	reservedMem = new char[RESERVED_MEMORY];
	SetControlBreak();


	strcpy(progName,runProgram);

	while(len>0 && runProgram[len]!='\\' && runProgram[len]!='/')
		--len;

	strcpy(revisionFile,runProgram);
	if( revisionFile[len]=='\\' || revisionFile[len]=='/'  )
		   revisionFile[len+1] = 0;
	else	   revisionFile[len] = 0;

	strcat(revisionFile,"REVISION.ERR");

	firstErr = 1;
 }

static void PrintStack(FILE *f)
 {
	if(CCallStack::m_procNameCnt>0) {
		if(f==NULL)
			 printf(  "Call of stack:\n");
		else
			fprintf(f,"Call of stack:\n");
		for(int i=0; i<CCallStack::m_procNameCnt;++i) {
			if(f==NULL)
			 printf(  "  >%s\n",
				CCallStack::m_procName[(CCallStack::m_procNamePos-i+128)&127]
			);
			else
			fprintf(f,"  >%s\n",
				CCallStack::m_procName[(CCallStack::m_procNamePos-i+128)&127]
			);
		}
	}
 }


int OAssert_P(const char *xMsg,
			  const char *fileName,int line,
			  const char *msg,...)
 {
	char buf[256];

	va_list ap;

	if( firstErr ) FirstMessage();

	va_start(ap,msg);
	vsprintf(buf,msg, ap);//va_arg(ap,void *))
	va_end(ap);

	FILE *f = fopen(revisionFile,"a+t");
	if(f==NULL) {
		delete reservedMem;
		f = fopen(revisionFile,"a+t");
	}

	if(f!=NULL) {
		fprintf(f,"File: %s, line: %i\n%s\nMessage:%s\n",
				fileName,line,
				xMsg,buf);
		PrintStack(f);
		fprintf(f,"\n\n");
		fclose(f);
	} else {
		printf("File: %s, line: %i\n%s\nMessage:%s\n",
				fileName,line,
				xMsg,buf);
		PrintStack(NULL);
	}
#if ASSERT_TERMINATE
#ifdef __BORLANDC__
       asm int 3;
#endif
    assert(0);
#endif
	return 1;
 }

int OAssert(const char *xMsg,
			const char *fileName,int line,
			const char *msg)
 {
	return OAssert_P(xMsg,fileName,line,"%s",msg);
 }

//***************************************************************************

const char *CCallStack::m_procName[128];
int         CCallStack::m_procNameCnt = 0;
int         CCallStack::m_procNamePos = 0;

CCallStack::CCallStack(const char *procName)
 {
	m_procNamePos = (m_procNamePos+1)&127;
	m_procName[m_procNamePos] = procName;
	if(m_procNameCnt<127)
		++m_procNameCnt;

 }

CCallStack::~CCallStack()
 {
	m_procNamePos = (m_procNamePos+127)&127;
	if(m_procNameCnt>0)
		--m_procNameCnt;

 }

