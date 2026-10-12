#include "sc.h"


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

    if( !sc_InitTSetOfProcess( &sop,
                                    320*200*2+100,
                                    4 ) )
    {
            printf("Can't initialize process");
            return 0;
    }

    if( !sc_InitTSuaCript( &suacript,
                            10240,
                            10240,
                            10000,
                            90000,
                            140000,
                            20240,
                            256,
                            2 ) )
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
                                320*200+100,
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



