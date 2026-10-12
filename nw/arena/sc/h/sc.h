#ifndef __SC_H__
#define __SC_H__

#include "cinter.h"
#include "cgen.h"

#define SUACRIPT_REGISTER_ERROR_HANDLE(sc) (setjmp((sc).m_heap.m_error.m_jumper) != 0)
#define SC_NEW(var,cnt,type)  (var) = (type*)malloc((cnt)*sizeof(type))
#define SC_DELETE(var)        free(var); (var) = NULL

typedef struct {
	TStackCell      *m_stacks;
	int              m_stackAlloc;
	int              m_stackSize;

	int              m_processCnt;
	int              m_maxProcess;
	TProcessContext *m_process;
	int              m_curProcess;
} TSetOfProcess;

typedef struct {
	os_TOutStream  m_code;
	TInt           m_entry;
    int            m_useStackEntry;
	const char    *m_name;
} TProgramm;


typedef struct {
	TNameArray     m_nameDescr;
	TScanner       m_scanner;
	TTreeHeap      m_heap;
	os_TOutStream  m_codeStream;
	os_TOutStream  m_linkInfo;
	TCGContext     m_cgc;

	TTree         *m_codeTree;
	DATA_TYPE      m_type;

	os_TOutStream  m_programmNames;
	TProgramm     *m_programm;
	int            m_programmCnt;
	int            m_maxProgramm;
} TSuaCript;

int sc_InitTSetOfProcess( TSetOfProcess *sop,
						  int            stackSize,
						  int            processCnt );
void sc_ResetTSuacript( TSuaCript *suacript );
void sc_ClearBuffers  ( TSuaCript *suacript );
int  sc_ProgrammId    ( TSuaCript *suacript, const char *progName );
int  sc_CreateProcess ( TSetOfProcess   *sop,
					    TSuaCript       *suacript,
					    int              progId,
					    int              stackSize,
					    int              quants,
					    TLinkConstExtern constExternLinkTable[] );

void sc_InitScannerFromMem ( TSuaCript *sc, char *text );
void sc_InitScannerFromFile( TSuaCript *sc, FILE *f );
int  sc_InitTSuaCript( TSuaCript    *sc,
                       int           wordBufSize,     /*  ~1024 */
                       int           stringBufSize,   /*  ~1024 */
                       int           nameCnt,         /*  ~ 90  */
                       int           treeBufSize,     /*  ~2000 */
                       int           codeStreamSize,  /*  ~4096 */
		   	           int           linkInfoSize,    /*  ~1024 */
                       int           progNameBufSize, /* 256    */
                       int           maxProgrammCnt );
void sc_DeleteTSuaCript( TSuaCript *sc );
void sc_Compile( TSuaCript        *sc,
                 const char       *programmName,
                 TLinkExtern       externLinkTable[],
                 TLinkConstExtern  constExternLinkTable[] );
int sc_RunProcess( TSetOfProcess *sop, int runProgCnt, void *arena );

#endif
/* End of SC.H */

