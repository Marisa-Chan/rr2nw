/*
    * File:  SC.C
    * Autor: Suavik
    * Ver  1.0
    *
    * Префикс sc_
    ------------------------------------------
    * Потоки слов и кода можно унифицировать.
    * Нет инициализации TOutStream ??
    *

    * При создании нового процесса должен сообщаться идентификатор
    программы, которую необходимо запускать.
    * entry - должен сообщаться программе.
    * Reset вставляется в Compile.

    */
#include <malloc.h>
#include "sc.h"
#include "instrm.h"
#include "strequ.h"

    /*========================================================================*/
    /*
    * Освобождение всей дредварительно выделенной памяти в случае,
    * если в произошла ошибка в процессе инициализации.
    */
void FreeAllocMem( void **allocMem, int cnt )
    {
    int i;

    for( i = 0; i < cnt ; ++i )
            SC_DELETE( allocMem[i] );
    }

    /*========================================================================*/
    /*
    * Инициализация множества процессов
    * Под все процессы выделяется куча стеков, в которой каждому
    * процессу выделяется своя часть.
    */
int sc_InitTSetOfProcess( TSetOfProcess *sop,
                            int            stackSize,
                            int            processCnt )
    {
    int   i = 0;
    void *allocMem[20];

    sop->m_maxProcess = 0;
    sop->m_stackSize  = 0;
    sop->m_stacks     = NULL;
    sop->m_process    = NULL;

    sop->m_processCnt = 0;
    sop->m_curProcess = -1;
    {
        /*
        * Выделение кучи под стеки
        */
        TStackCell *stackBuf;

        SC_NEW( stackBuf, stackSize, TStackCell );

        if( stackBuf == NULL )
        {
            FreeAllocMem( allocMem, i );
            return 0;
        }

        allocMem[i] = (void *)stackBuf; ++i;

            sop->m_stacks     = stackBuf;
            sop->m_stackAlloc = 0;
            sop->m_stackSize  = stackSize;
    }

    {
            /*
            * Выделение массива под процессы
            */
            TProcessContext *pc;

            SC_NEW( pc, processCnt, TProcessContext );

            if( pc == NULL )
            {
                FreeAllocMem( allocMem, i );
                return 0;
        }
            allocMem[i] = (void *)pc; ++i;

            sop->m_process    = pc;
            sop->m_processCnt = 0;
            sop->m_maxProcess = processCnt;
    }

    sop->m_curProcess = 0;
    return 1;
    }

    /*========================================================================*/
    /*
    * Сброс буферов языка для компиляции очередной программы.
    * Остаются только уже скомпилированные программы.
    */
void sc_ResetTSuacript( TSuaCript *suacript )
    {
    h_ClearTNameArray ( &(suacript->m_nameDescr) );
    h_ClearTTreeHeap  ( &(suacript->m_heap) );
    os_ResetTOutStream( &(suacript->m_codeStream) );
    os_ClearTOutStream( &(suacript->m_linkInfo) );
    lex_ClearTScanner ( &(suacript->m_scanner) );

    cg_ClearTCGContext( &(suacript->m_cgc) );

    suacript->m_codeTree = NIL;
    suacript->m_type     = T_NONE;
    }

    /*========================================================================*/
    /*
    * Очистка всех буферов для запуска компиляций с нуля.
    */
void sc_ClearBuffers( TSuaCript *suacript )
    {
    h_ClearTNameArray ( &(suacript->m_nameDescr) );
    h_ClearTTreeHeap  ( &(suacript->m_heap) );
    os_ClearTOutStream( &(suacript->m_codeStream) );
    os_ClearTOutStream( &(suacript->m_linkInfo) );
    lex_ClearTScanner ( &(suacript->m_scanner) );
    cg_ClearTCGContext( &(suacript->m_cgc) );

    suacript->m_codeTree = NIL;
    suacript->m_type     = T_NONE;
    }

    /*========================================================================*/
    /*
    * Определение идентификатора программы по имени.
    */
int sc_ProgrammId( TSuaCript *suacript, const char *progName )
    {
    int i;

    for( i = 0; i < suacript->m_programmCnt; ++i )
            if( str_StrEQU( suacript->m_programm[i].m_name, progName ) )
                return i;

    return -1;
    }

    /*========================================================================*/
    /*
    * Создание нового процесса.
    */
int sc_CreateProcess( TSetOfProcess   *sop,
                        TSuaCript       *suacript,
                        int              progId,
                        int              stackSize,
                        int              quants,
                        TLinkConstExtern constExternLinkTable[] )
    {
    TProcessContext *pc;
    TProgramm       *prog;

    lng_ASSERT( progId>=0 &&
                progId<suacript->m_programmCnt , "Bad programm Id" );

    if( sop->m_processCnt >= sop->m_maxProcess )
            return 0;

    if( sop->m_stackAlloc + stackSize > sop->m_stackSize )
            return 0;

    pc   = &(sop->m_process[ sop->m_processCnt ]);
    prog = &(suacript->m_programm[ progId ]);

    if( stackSize-4 < prog->m_useStackEntry )
    {
            /* FIXME - нужна нормальная ошибка (lng_Error)*/
            lng_ASSERTNQ("Stack for execute too low");
            return 0;
    }

    ci_InitTProcessContext(
                pc,
                &(sop->m_stacks[ sop->m_stackAlloc ]),
                stackSize,
                quants,
                &(prog->m_code),
                prog->m_entry,
                constExternLinkTable );

    sop->m_processCnt += 1;
    sop->m_stackAlloc += stackSize;
    return 1;
    }

    /*========================================================================*/
    /*
    * Инициализация сканера для чтения из ASCIIZ строки в памяти
    */
void sc_InitScannerFromMem( TSuaCript *sc, char *text )
    {
    lex_InitScannerFromMem( &(sc->m_scanner), text );
    }

    /*========================================================================*/
    /*
    * Инициализация сканера для чтения из файла
    */
void sc_InitScannerFromFile( TSuaCript *sc, FILE *f )
    {
    lex_InitScannerFromFile( &(sc->m_scanner), f );
    }


double initialize_math = 4.6;
    /*========================================================================*/
    /* FIXME - проставить 0 в размерах при неудачном выделении */
int sc_InitTSuaCript( TSuaCript    *sc,
                        int           wordBufSize,     /*  ~1024 */
                        int           stringBufSize,   /*  ~1024 */
                        int           nameCnt,         /*  ~ 90  */
                        int           treeBufSize,     /*  ~2000 */
                        int           codeStreamSize,  /*  ~4096 */
                        int           linkInfoSize,    /*  ~1024 */
                        int           progNameBufSize, /* 256    */
                        int           maxProgrammCnt )
    {
    int   i = 0;
    void *allocMem[20];

    initialize_math *= 2;
    {
            /*
            * Инициализация буфера программ
            */
            TProgramm *prog;
            TByte     *progNames;
            SC_NEW( prog, maxProgrammCnt, TProgramm );

            if( prog == NULL )
            {
                FreeAllocMem( allocMem, i );
                return 0;
            }
            allocMem[i] = (void*)prog; ++i;
            sc->m_maxProgramm = maxProgrammCnt;
            sc->m_programmCnt = 0;
            sc->m_programm    = prog;

            SC_NEW( progNames, progNameBufSize, TByte );

            if( progNames == NULL )
            {
                FreeAllocMem( allocMem, i );
                return 0;
            }
            os_InitTOutStream( &(sc->m_programmNames),
                            progNames,
                            progNameBufSize,
                            "progNameBuff"  );
    }

    {
            /*
            * Инициализация результирующего потока кода
            */
            TByte *codeStreamBuf;
            SC_NEW( codeStreamBuf, codeStreamSize, TByte );

            if( codeStreamBuf == NULL )
            {
                FreeAllocMem( allocMem, i );
                return 0;
            }
            allocMem[i] = (void *)codeStreamBuf; ++i;
            os_InitTOutStream( &(sc->m_codeStream),
                            codeStreamBuf,
                            codeStreamSize,
                            "codeStream" );
    }

    {
            /*
            * Инициализация буфера имен.
            * В буфере хранятся только описания без строк.
            */
            TName  *namesBuf;
            SC_NEW( namesBuf, nameCnt, TName );

            if( namesBuf == NULL )
            {
                FreeAllocMem( allocMem, i );
                return 0;
            }
            allocMem[i] = (void *)namesBuf; ++i;
            h_InitTNameArray( &(sc->m_nameDescr), namesBuf, nameCnt);
    }

    {
            /*
            * Инициализация кучи для дерева разбора
            */
            TTree  *treeBuf;
            SC_NEW( treeBuf, treeBufSize, TTree );

            if( treeBuf == NULL )
            {
                FreeAllocMem( allocMem, i );
                return 0;
            }
            allocMem[i] = (void *)treeBuf; ++i;
            h_InitTTreeHeap ( &(sc->m_heap), treeBuf, treeBufSize );
    }


    {
            /*
            * Инициализация потока линкерной информации
            */
            TByte *linkInfoBuf;
            SC_NEW( linkInfoBuf, linkInfoSize, TByte );

            if( linkInfoBuf == NULL )
            {
                FreeAllocMem( allocMem, i );
                return 0;
            }
            allocMem[i] = (void *)linkInfoBuf;; ++i;
            os_InitTOutStream( &(sc->m_linkInfo),
                            linkInfoBuf,
                            linkInfoSize,
                            "linkInfo" );
    }

    {
            /*
            * Инициализация сканера
            */
            char *wordBuf, *stringBuf;

            SC_NEW( wordBuf, wordBufSize, char );

            if( wordBuf == NULL )
            {
                FreeAllocMem( allocMem, i );
                return 0;
            }
            allocMem[i] = (void *)wordBuf; ++i;

            SC_NEW( stringBuf, stringBufSize, char );
            if( stringBuf == NULL )
            {
                FreeAllocMem( allocMem, i );
                return 0;
            }
            allocMem[i] = (void *)stringBuf; ++i;

            lex_InitTScanner( &(sc->m_scanner),
                            wordBuf,   wordBufSize,
                            stringBuf, stringBufSize );

    }

    lng_InitTError( &(sc->m_heap.m_error), &(sc->m_scanner) );

    cg_InitTCGContext( &(sc->m_cgc),
                        &(sc->m_codeStream),
                        &(sc->m_linkInfo),
                        h_ErrorOf( &(sc->m_heap) ) );

    sc->m_codeTree = NULL;

    return 1;
    }

    /*========================================================================*/
void sc_DeleteTSuaCript( TSuaCript *sc )
    {
    SC_DELETE( *h_ProvideForDeletingTNameArray( &(sc->m_nameDescr  ) ) );
    SC_DELETE( *h_ProvideForDeletingTTreeHeap ( &(sc->m_heap       ) ) );
    SC_DELETE(*os_ProvideForDeletingTOutStream( &(sc->m_codeStream ) ) );
    SC_DELETE(*os_ProvideForDeletingTOutStream( &(sc->m_linkInfo   ) ) );
    SC_DELETE( *lex_ProvideForDeletingNames   ( &(sc->m_scanner    ) ) );
    SC_DELETE( *lex_ProvideForDeletingStrings ( &(sc->m_scanner    ) ) );

    SC_DELETE(*os_ProvideForDeletingTOutStream( &(sc->m_programmNames) ) );
    SC_DELETE( sc->m_programm );

    sc->m_codeTree = NIL;
    sc->m_type     = T_NONE;

    h_DropTNameArray ( &(sc->m_nameDescr) );
    lex_DropTScanner ( &(sc->m_scanner)   );
    h_DropTTreeHeap  ( &(sc->m_heap) );
    os_DropTOutStream( &(sc->m_codeStream) );
    os_DropTOutStream( &(sc->m_linkInfo) );
    }

    /*========================================================================*/
void sc_Compile( TSuaCript        *sc,
                    const char       *programmName,
                    TLinkExtern       externLinkTable[],
                    TLinkConstExtern  constExternLinkTable[] )
    {
    TTree      *treeHead = NIL;
    TNameDefs   ndefs;
    TTreeHeap  *heap     = &(sc->m_heap);
    TProgramm  *prog;

    if( sc->m_programmCnt >= sc->m_maxProgramm )
            lng_Error( h_ErrorOf( heap ),
                    "Too many programm" );

    sc->m_codeTree =
                tc_OptimizeExprLev1( heap,
                tc_ConvertToCode(
                    heap,
                    p_Programm( heap, &(sc->m_scanner),
                                &(sc->m_nameDescr),&treeHead, &ndefs),
                    &(sc->m_type)),
                &(sc->m_scanner),&(sc->m_nameDescr),
                &ndefs);

    cg_CodeGenProg( &(sc->m_cgc), sc->m_codeTree );

    ci_LinkProgramm( sc->m_cgc.m_linkInfo,
                        sc->m_cgc.m_cs,
                        sc->m_cgc.m_linkList,
                        sc->m_cgc.m_error,
                        externLinkTable, constExternLinkTable );
    //cg_DisAsm( &(sc->m_codeStream) );

    prog = &(sc->m_programm[ sc->m_programmCnt ] );

    if( sc_ProgrammId( sc, programmName ) >= 0 )
    {
            lng_Error( h_ErrorOf( &(sc->m_heap) ),
                    "Dublicate programm name %s", programmName );
    }

    prog->m_name = os_CurStr( &(sc->m_programmNames) );
    os_PutStr( &(sc->m_programmNames), programmName );
    prog->m_entry = ci_GetFuncPtr( &(sc->m_codeStream),
                                    "main",
                                    &(prog->m_useStackEntry)  );

    if( prog->m_entry < 0 )
            lng_Error( h_ErrorOf( heap ),
                    "Can't found 'main()'" );
    os_InitTOutStream( &(prog->m_code),
                        (TByte*)os_GetStr( &(sc->m_codeStream), 0 ),
                        (int)os_CurPos( &(sc->m_codeStream) ),
                        "codeStream" );
    /* FIXME - нельзя добираться напрямую до полей */
    prog->m_code.m_pos = prog->m_code.m_size;

    sc->m_programmCnt += 1;

    sc_ResetTSuacript( sc );
    }

    /*========================================================================*/
int sc_RunProcess( TSetOfProcess *sop, int runProgCnt, void *arena )
    {
    int breakProc = 0,
        beenRun   = runProgCnt ;

    for( ; runProgCnt ; --runProgCnt )
    {
            TProcessContext *pc = &(sop->m_process[ sop->m_curProcess ]);
            int      q = pc->m_quants;

            if( q > 0 )
            {
                if( ci_RunProcess( pc, q, arena ) )
                {
                    pc->m_quants = 0;
                    ++breakProc;
                }
            }
            else ++breakProc;
            /* FIXME */

            sop->m_curProcess += 1;
            if( sop->m_curProcess >= sop->m_processCnt )
                sop->m_curProcess = 0;
    }

    return breakProc == beenRun;
    }

    /*========================================================================*/
    /*

    DEBUG
char *LoadProgramm( const char *fileName )
    {
    FILE *f = fopen(fileName,"rb");
    char *buf;
    long  size;

    if( f == NIL )
            return NIL;

    fseek( f, 0, SEEK_END );
    size = ftell( f );
    fseek( f, 0, SEEK_SET );
    buf = (char*)malloc( (int)size + 1 );

    if( buf == NIL )
    {
            fclose( f );
            return NIL;
    }

    size = fread( buf, 1, (int)size, f );
    buf[ (int)size ] = 0;

    return buf;
    }

    */

#define TEST_SC 0
#if TEST_SC
unsigned _stklen = 34u*1024u;


#define BIGMEM 0
    /*========================================================================*/
int main(int parCnt, char **par)
    {
    TSuaCript      suacript;
    TSetOfProcess  sop;
    FILE *f;
    int i;

    printf("\n-------\n");

    if( parCnt == 1 )
    {
            printf( "lang filename.sc\n" );
            return 0;
    }

#if BIGMEM
    if( !sc_InitTSetOfProcess( &sop,
                                    320*200*2+100,
                                    4 ) )
#else
    if( !sc_InitTSetOfProcess( &sop,
                                    100,
                                    4 ) )
#endif
    {
            printf("Can't initialize process");
            return 0;
    }

#if BIGMEM
    if( !sc_InitTSuaCript( &suacript,
                            1024,
                            1024,
                            1000,
                            9000,
                            14000,
                            2024,
                            256,
                            2 ) )
#else
    if( !sc_InitTSuaCript( &suacript,
                            1024, /* word buf size   */
                            1024, /* string buf size */
                            100,  /* name cnt        */
                            500,  /* tree size       */
                            1024,
                            1024,
                            256,
                            2 ) )
#endif
    {
            printf("Can't initialize suacript\n");
            return 0;
    }

    if( SUACRIPT_REGISTER_ERROR_HANDLE(suacript) )
    {
            printf("%s\n",suacript.m_heap.m_error.m_msg);
            return 0;
    }

    for( i = 1; i< parCnt; ++i )
    {
         f = fopen(par[i],"rb");
         if( f == NULL )
         {
              printf("Can't read file %s\n",par[i]);
              return 0;
         }
         sc_InitScannerFromFile( &suacript, f );


         sc_Compile(&suacript, par[i], externLinkTable, constExternLinkTable);
         fclose(f);

         if( !sc_CreateProcess( &sop,
                                &suacript,
                                sc_ProgrammId( &suacript,par[i] ),
#if BIGMEM
                                320*200+100,
#else
                                100,
#endif
                                100,
                                constExternLinkTable ) )
         {
               printf("Can't create process");
               return 0;
         }
    }


    while( !sc_RunProcess( &sop, parCnt, NULL ) );

    sc_DeleteTSuaCript( &suacript );
    return 0;
    }
#endif
/* End of file SC.C */


