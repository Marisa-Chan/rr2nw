/*
 * File: CINTER.C
 * Autor: Suavik
 * Ver    1.0
 *
 * Префикс ci_
 * Проверена работа всех операций
 * * Не правильно определяется момент выхода из интерпретатора по isQuit
 *
 * 02.09.97 Начат перевод операций на использование CODEI, CODESTEPI,
 *          последняя переведенная - AM_POP_VX_P.
 *
 */
#include <math.h>
#include <stdlib.h>
#include <process.h>

#include <conio.h>
#include "cinter.h"
#include "bytecod.h"
#include "strequ.h"

 /*========================================================================*/
void ci_InitTProcessContext( TProcessContext *pc,
                             TStackCell      *stack,
                             int              stackSize,
                             int              quants,
                             os_TOutStream   *cs,
                             TInt             entry,
                             TLinkConstExtern constExternLinkTable[] )
 {
    TLinkConstExtern *constExtern = constExternLinkTable;
    int               stackUp, sp;

    pc->m_stack     = stack;
    pc->m_stackSize = stackSize;
    pc->m_codePtr   = (TByte*)os_GetStr( cs, _TINT(0) );
    pc->SP          = 0;
    pc->BP          = 0;
    pc->IP          = entry;
    pc->m_quants    = quants;


    stackUp = (int)(pc->SP);
    for(;; ++constExtern)
    {
         if( constExtern->m_name == NULL )
              break;

         sp = constExtern->m_offset;
         constExtern->m_func( &(pc->m_stack[ sp ]) );

         if( sp+1 > stackUp )
              stackUp = sp+1;
    }
    pc->SP = stackUp;

    /*
     * Пишем фиктивный адрес возврата, чтобы корректно выходить
     * из вызываемой функции (main)
     */
    pc->m_stack[ (int)(pc->SP) ].i  = -1; ++(pc->SP);
    pc->BP = pc->SP;
 }


 /*========================================================================*/
TInt ci_GetFuncPtr( os_TOutStream *cs, const char *fname, int *useStack )
 {
    TInt pos = 0, next, codePtr, fnamePtr;

    do
    {
         if( os_GetIntStep( cs, &pos ) != FUNCMAGIC )
         {
              lng_ASSERTNQ("Error: ci_GetFuncPtr bad magic");
         }
         next     = os_GetIntStep( cs, &pos );
                    os_GetIntStep( cs, &pos );
         codePtr  = os_GetIntStep( cs, &pos );
         fnamePtr = os_GetIntStep( cs, &pos );

         if( useStack != NULL )
         {
              os_GetIntStep( cs, &pos );
              *useStack = (int)os_GetIntStep( cs, &pos );
         }

         if( str_StrEQU( os_GetStr( cs, fnamePtr ), fname) )
              return codePtr;
         pos = next;
    } while( pos != 0 );

    return -1;
 }

 /*========================================================================*/
TFuncExtern ci_GetFuncExternPtr( TLinkExtern externLinkTable[],
                                 const char *fname )
 {
    TFuncExtern fex;
    const char *name;
    int i;

    for( i = 0 ;; ++i )
    {
         name = externLinkTable[i].m_name;
         fex  = externLinkTable[i].m_func;

         if( fex == NULL )
              return NULL;

         if( str_StrEQU( fname, name ) )
              return fex;
    }
 }

 /*========================================================================*/
TLinkConstExtern *ci_SearchConstExtern(
                    TLinkConstExtern  constExternLinkTable[],
                    const char       *fname )
 {
     int i;

     for( i = 0 ;; ++i )
     {
          if( constExternLinkTable[i].m_name == NULL )
               return NULL;

          if( str_StrEQU( fname, constExternLinkTable[i].m_name ) )
               return &(constExternLinkTable[i]);
     }
 }

 /*========================================================================*/
void ci_LinkProgramm( os_TOutStream   *linkInfo,
					  os_TOutStream   *cs,
                      TInt             linkList,
                      TError          *error,
                      TLinkExtern      externLinkTable[],
                      TLinkConstExtern constExternLinkTable[] )
 {
    TInt         ptr      = linkList;
    TInt         funcCodePtr;
    TFuncExtern  funcExternPtr;
    TInt         constExternPtr;

    if( ptr < 0 )
         return;

    while( ptr < linkInfo->m_pos )
    {
           /*
              [ ] указатель на имя (строку)
              [ ] тип вызова (FT_SCRIPT/FT_EXTERN)
              [ ] указатель на следующий элемент
              [ ]  указатель на элемент списка адресов
              [][][][] строка
            */
           TInt namePtr   = os_GetIntStep( linkInfo, &ptr );
           TInt func_type = os_GetIntStep( linkInfo, &ptr );
           TInt next      = os_GetIntStep( linkInfo, &ptr );
           TInt listAdr   = os_GetIntStep( linkInfo, &ptr );
           const char  *fname = os_GetStr( linkInfo, namePtr );

           if( func_type == FT_CNSTEX )
           {
                TLinkConstExtern *constExtern;

                constExternPtr = os_GetIntStep( linkInfo, &ptr );
                constExtern    = ci_SearchConstExtern( constExternLinkTable,
                                                       fname );
                if( constExtern == NULL )
                     lng_Error( error, "Unknown link const extern %s",
                                              fname );
                constExtern->m_offset = (int)constExternPtr;
           }

           if( listAdr != -1 )
           {
                switch( (int)func_type )
                {
                case FT_SCRIPT:
                        funcCodePtr = ci_GetFuncPtr( cs, fname, NULL );
                        if( funcCodePtr == -1 )
                             lng_Error( error,
                                        "Unknown link %s", fname );
                        break;

                case FT_EXTERN:
                        funcExternPtr = ci_GetFuncExternPtr( externLinkTable,
                                                             fname );
                        if( funcExternPtr == NULL )
                             lng_Error( error,
                                        "Unknown link %s", fname );
                        break;

                default: lng_ASSERTNQ("ci_LinkPrigramm: Bed func");
                }
           }

           while( listAdr >= 0 ) /* CHECKME */
           {
               TInt pos, fadr;
               pos = listAdr;
               fadr    = os_GetIntStep( linkInfo, &pos );
               listAdr = os_GetIntStep( linkInfo, &pos );


                switch( (int)func_type )
                {
                case FT_SCRIPT: os_SetInt( cs, (int)fadr, funcCodePtr );  break;
                case FT_EXTERN: os_SetFunc( cs, (int)fadr,
                                    (os_TFunc)funcExternPtr );
                                break;
                default: lng_ASSERTNQ("ci_LinkProgramm: Unknown funcDef");
                }


           }

           ptr = next;

           if( ptr == -1 )
                break;
    }
 }

 /*========================================================================*/
int ci_RunProcess( TProcessContext *pc, int clockCnt, void *arena )
 {
#define S_PTR2INT(x) ((TInt)((x) - pc->m_stack))
#define S_INT2PTR(x) (&(pc->m_stack[ (int)(x) ]))

    TByte    *code = pc->m_codePtr;
#define C_INT2PTR(x)   (&(code[(int)(x)]))
#define C_PTR2INT(x)   ((TInt)((x)-code))

    register TStackCell  *SP = S_INT2PTR(pc->SP);
    register TStackCell  *BP = S_INT2PTR(pc->BP);
    register TByte       *IP = C_INT2PTR(pc->IP);
    TStackCell           *SPTR;
    TInt                  svali;
    TFloat                valf, svalf;
    volatile int          isQuit = 0;
    volatile TInt         svali1;
    register int          clock;

#define IP_ADD(x)  IP+=((int)(x))

#define CURI ((SP-1)->i)
#define CURF ((SP-1)->f)
#define GETSI(p) (pc->m_stack[(int)(p)].i)
#define GETSF(p) (pc->m_stack[(int)(p)].f)
#define GETITO(vali) vali=*((TInt*)IP);   IP+=sizeof(TInt)
#define GETF         valf=*((TFloat*)IP); IP+=sizeof(TFloat)
#define POPI(v) --SP; v=SP->i
#define POPF(v) --SP; v=SP->f
#define PUSHI(v) SP->i=v; ++SP
#define PUSHF(v) SP->f=v; ++SP

#define CODEI        *((TInt*)IP)
#define CODESTEPI    IP+=sizeof(TInt)

    for( clock = clockCnt; clock; --clock )
    {
		   /* opCode = (AM_TYPE)*IP; */
		   /* ++IP; */
#if 0
{
int i;
gotoxy(1,1);
clreol();
printf("Stack===\n");
for(i = 0; i<SP-pc->m_stack; ++i)
{
    clreol();
    printf("%5i:    %9li   %lf\n", i,pc->m_stack[i].i, pc->m_stack[i].f);
}
for(i=0; i<10; ++i)
{
      clreol();
      printf("\n");
}

}
#endif

           switch( *((IP+=COPSIZE)-COPSIZE) )
           {
           case AM_NOP:                                     break;
 /* */     case AM_RESERVED:   SP += (int)CODEI; CODESTEPI; break;

 /* */     case AM_JMP:        IP_ADD(CODEI); CODESTEPI; break;
 /* */     case AM_JMPIFFALSE:
                   {
                   TInt relAdr;

                   GETITO(relAdr);
                   POPI(svali);
                   if( !svali ) IP_ADD(relAdr);
                   break;
                   }
 /* */     case AM_JMPIFTRUE:
                   {
                   TInt relAdr;

                   GETITO(relAdr);
                   POPI(svali);
                   if( svali ) IP_ADD(relAdr);
                   break;
                   }
 /*.*/     case AM_BOUND:
                   POPI(svali);
                   if( CURI >= svali )
                   {
                        lng_ASSERTNQ_PP( "RunTime: Range check error[%li] IP=%05lX",
                                        CURI, C_PTR2INT(IP) );
                        return 1;
                   }
                   break;

 /* */     case AM_LEAVE:
                   {
                   TStackCell *cx, *dest;
                   cx   = BP+(int)CODEI; CODESTEPI;
                   dest = BP+(int)CODEI; CODESTEPI;
                   GETITO(svali);
                   if( cx->i > dest->i )
                        IP_ADD(svali);
                   }
                   break;

 /* */     case AM_LEAVED:
                   {
                   TStackCell *cx, *dest;
                   cx   = BP+(int)CODEI; CODESTEPI;
                   dest = BP+(int)CODEI; CODESTEPI;
                   GETITO(svali);
                   if( cx->i < dest->i )
                        IP_ADD(svali);
                   }
                   break;

 /* */     case AM_LOOP:
                   {
                   TStackCell *dest;
                   SPTR = BP+(int)CODEI; CODESTEPI;
                   dest = BP+(int)CODEI; CODESTEPI;
                   GETITO(svali);
                   if( SPTR->i < dest->i )
                   {
                        IP_ADD(svali);
                        ++(SPTR->i);
                   }
                   }
                   break;

 /* */     case AM_LOOPD:
                   {
                   TStackCell *dest;
                   SPTR = BP+(int)CODEI; CODESTEPI;
                   dest = BP+(int)CODEI; CODESTEPI;
                   GETITO(svali);
                   if( SPTR->i > dest->i )
                   {
                        IP_ADD(svali);
                        --(SPTR->i);
                   }
                   }
                   break;

 /* */     case AM_PUSHCI:           PUSHI(CODEI); CODESTEPI; break;
 /* */     case AM_PUSHCF:     GETF; PUSHF(valf); break;
 /* */     case AM_PUSHCS:
                   {
                   TInt strPos, lastPos;
                   strPos  = CODEI; CODESTEPI;
                   lastPos = CODEI; CODESTEPI;
                   PUSHI( strPos );

                   IP = C_INT2PTR(lastPos);
                   }
                   break;
 /* */     case AM_PUSH_ADR:
                   PUSHI( S_PTR2INT(BP+(int)CODEI) );
                   CODESTEPI;
                   break;
           /*------------------------ POP -*/
 /* */     case AM_POP_VI:
                    POPI(svali);
                    (BP+(int)CODEI)->i = svali;
                    CODESTEPI;
                    break;

 /* */     case AM_POP_VF:
                    POPF(svalf);
                    (BP+(int)CODEI)->f = svalf;
                    CODESTEPI;
                    break;

 /* */     case AM_POP_V3:
                    SPTR = BP+(int)CODEI; CODESTEPI;
                    POPF(svalf);

                    (SPTR+2)->f = svalf;

                    POPF(svalf);
                    (SPTR+1)->f = svalf;

                    POPF(svalf);
                    (SPTR+0)->f = svalf;
                    break;

 /* */     case AM_POP_VI_P:
                    POPI(svali);
                    pc->m_stack[ (int)(BP+(int)CODEI)->i ].i = svali;
                    CODESTEPI;
                    break;

 /* */     case AM_POP_VF_P:
                    POPF(svalf);
                    pc->m_stack[ (int)(BP+(int)CODEI)->i ].f = svalf;
                    CODESTEPI;
                    break;

 /* */     case AM_POP_V3_P:
                    SPTR = &(pc->m_stack[ (int)((BP+(int)CODEI)->i) ]);
                    CODESTEPI;
                    POPF(svalf);
                    (SPTR+2)->f = svalf;
                    POPF(svalf);
                    (SPTR+1)->f = svalf;
                    POPF(svalf);
                    (SPTR+0)->f = svalf;
                    break;

           case AM_POP_VX:
                    POPF(svalf);
                    (BP+(int)CODEI)->f = svalf; CODESTEPI;
                    break;

           case AM_POP_VY:
                    POPF(svalf);
                    (BP+(int)CODEI+1)->f = svalf; CODESTEPI;
                    break;

           case AM_POP_VZ:
                    POPF(svalf);
                    (BP+(int)CODEI+2)->f = svalf; CODESTEPI;
                    break;

 /* */     case AM_POP_AI:
                    POPI(svali);
                    POPI(svali1);
                    (BP+(int)(CODEI+svali))->i = svali1; CODESTEPI;
                    break;

           case AM_POP_AF:
                    POPI(svali);
                    POPF(svalf);
                    (BP+(int)(CODEI+svali))->f = svalf; CODESTEPI;
                    break;

           case AM_POP_AX:
                    POPI(svali);
                    POPF(svalf);
                    (BP+(int)(CODEI+svali))->f = svalf; CODESTEPI;
                    break;

           case AM_POP_AY:
                    POPI(svali);
                    POPF(svalf);
                    (BP+(int)(CODEI+svali)+1)->f = svalf; CODESTEPI;
                    break;

           case AM_POP_AZ:
                    POPI(svali);
                    POPF(svalf);
                    (BP+(int)(CODEI+svali)+2)->f = svalf; CODESTEPI;
                    break;

           case AM_POP_A3:
                    {
                    TFloat x,y,z;

                    SPTR = (BP+(int)CODEI); CODESTEPI;
                    POPI(svali);
                    SPTR += (int)svali;
                    POPF(z);
                    POPF(y);
                    POPF(x);
                    (SPTR+0)->f = x;
                    (SPTR+1)->f = y;
                    (SPTR+2)->f = z;
                    }
                    break;


           case AM_POP_AI_P:
                    POPI(svali);
                    POPI(svali1);
                    GETSI(
                         (int)  ((BP+(int)CODEI)->i + svali)
                         ) = svali1;
                    CODESTEPI;
                    break;

           case AM_POP_AF_P:
                    POPI(svali);
                    POPF(svalf);
                    GETSF(
                         (int)  ((BP+(int)CODEI)->i + svali)
                         ) = svalf;
                    CODESTEPI;
                    break;

           case AM_POP_A3_P:
                    POPI(svali);
                    svali = (BP+(int)CODEI)->i+svali; CODESTEPI;
                    SPTR = &(pc->m_stack[ (int)svali ]);
                    POPF(svalf); (SPTR+2)->f = svalf;
                    POPF(svalf); (SPTR+1)->f = svalf;
                    POPF(svalf); (SPTR+0)->f = svalf;
                    break;

           case AM_POP_AX_P:
                    POPI(svali);
                    svali = (BP+(int)CODEI)->i+svali; CODESTEPI;
                    POPF(svalf); GETSF(svali) = svalf;
                    break;

           case AM_POP_AY_P:
                    POPI(svali);
                    svali = (BP+(int)CODEI)->i+svali; CODESTEPI;
                    POPF(svalf); GETSF(svali+1) = svalf;
                    break;

           case AM_POP_AZ_P:
                    POPI(svali);
                    svali = (BP+(int)CODEI)->i+svali; CODESTEPI;
                    POPF(svalf); GETSF(svali+2) = svalf;
                    break;
           /*------------------------ PUSH -*/
 /* */     case AM_PUSH_VI: PUSHI((BP+(int)CODEI)->i); CODESTEPI; break;
 /* */     case AM_PUSH_VF: PUSHF((BP+(int)CODEI)->f); CODESTEPI; break;
 /* */     case AM_PUSH_V3:
                           SPTR = (BP+(int)CODEI); CODESTEPI;
                           PUSHF((SPTR+0)->f);
                           PUSHF((SPTR+1)->f);
                           PUSHF((SPTR+2)->f);
                           break;
           case AM_PUSH_VX:PUSHF((BP+(int)CODEI+0)->f); CODESTEPI;break;
           case AM_PUSH_VY:PUSHF((BP+(int)CODEI+1)->f); CODESTEPI;break;
           case AM_PUSH_VZ:PUSHF((BP+(int)CODEI+2)->f); CODESTEPI;break;

 /* */     case AM_PUSH_VI_P:
                      PUSHI(GETSI((BP+(int)CODEI)->i));
                      CODESTEPI;
                      break;
 /* */     case AM_PUSH_VF_P:
                      PUSHF(GETSF((BP+(int)CODEI)->i));
                      CODESTEPI;
                      break;
 /* */     case AM_PUSH_V3_P:
                      SPTR = &(pc->m_stack[(int)((BP+(int)CODEI)->i)]);
                      CODESTEPI;
                      PUSHF((SPTR+0)->f);
                      PUSHF((SPTR+1)->f);
                      PUSHF((SPTR+2)->f);
                      break;

           case AM_POP_VX_P:
                      POPF(svalf);
                      GETSF((BP+(int)CODEI)->i) = svalf;
                      CODESTEPI;
                      break;
           case AM_POP_VY_P:
                      POPF(svalf);
                      GETSF((BP+(int)CODEI)->i+1) = svalf;
                      CODESTEPI;
                      break;
           case AM_POP_VZ_P:
                      POPF(svalf);
                      GETSF((BP+(int)CODEI)->i+2) = svalf;
                      CODESTEPI;
                      break;

           case AM_PUSH_AI:
                      POPI(svali);
                      PUSHI((BP+(int)(CODEI+svali))->i);
                      CODESTEPI;
                      break;
           case AM_PUSH_AF:
                      POPI(svali);
                      PUSHF((BP+(int)(CODEI+svali))->f);
                      CODESTEPI;
                      break;

           case AM_PUSH_A3:
                      POPI(svali);
                      SPTR = (BP+(int)(CODEI+svali));
                      CODESTEPI;
                      PUSHF((SPTR+0)->f);
                      PUSHF((SPTR+1)->f);
                      PUSHF((SPTR+2)->f);
                      break;

           case AM_PUSH_AI_P:
                      POPI(svali);
                      svali += (BP+(int)CODEI)->i;
                      PUSHI( GETSI(svali) );
                      CODESTEPI;
                      break;
           case AM_PUSH_AF_P:
                      POPI(svali);
                      svali += (BP+(int)CODEI)->i;
                      PUSHF( GETSF(svali) );
                      CODESTEPI;
                      break;

           case AM_PUSH_A3_P:
                      POPI(svali);
                      svali += (BP+(int)CODEI)->i;
                      CODESTEPI;
                      SPTR = &(pc->m_stack[(int)svali]);
                      PUSHF((SPTR+0)->f);
                      PUSHF((SPTR+1)->f);
                      PUSHF((SPTR+2)->f);
                      break;

           case AM_PUSH_AX_P:
                      POPI(svali);
                      svali += (BP+(int)CODEI)->i;
                      CODESTEPI;
                      PUSHF(GETSF(svali));
                      break;

           case AM_PUSH_AY_P:
                      POPI(svali);
                      svali += (BP+(int)CODEI)->i;
                      CODESTEPI;
                      PUSHF(GETSF(svali+1));
                      break;

           case AM_PUSH_AZ_P:
                      POPI(svali);
                      svali += (BP+(int)CODEI)->i;
                      CODESTEPI;
                      PUSHF(GETSF(svali+2));
                      break;

           case AM_PUSH_AX:
                      POPI(svali);
                      PUSHF((BP+(int)(CODEI+svali))->f);
                      CODESTEPI;
                      break;

           case AM_PUSH_AY:
                      POPI(svali);
                      PUSHF((BP+(int)(CODEI+svali+1))->f);
                      CODESTEPI;
                      break;

           case AM_PUSH_AZ:
                      POPI(svali);
                      PUSHF((BP+(int)(CODEI+svali+2))->f);
                      CODESTEPI;
                      break;

           case AM_PUSH_VX_P:
                      PUSHF(GETSF( (BP+(int)CODEI)->i ));
                      CODESTEPI;
                      break;
           case AM_PUSH_VY_P:
                      PUSHF(GETSF( (BP+(int)CODEI)->i+1 ));
                      CODESTEPI;
                      break;
           case AM_PUSH_VZ_P:
                      PUSHF(GETSF( (BP+(int)CODEI)->i+2 ));
                      CODESTEPI;
                      break;
           /*-------------------------------*/
           case AM_AND:  POPI(svali); CURI = CURI && svali; break;
           case AM_OR:   POPI(svali); CURI = CURI || svali; break;

           case AM_ADDF: POPF(svalf); CURF += svalf; break;
           case AM_ADDI: POPI(svali); CURI += svali; break;
           case AM_ADD3: {
                         double x,y,z;
                         POPF(z);
                         POPF(y);
                         POPF(x);
                         (SP-1)->f += z;
                         (SP-2)->f += y;
                         (SP-3)->f += x;
                         }
                         break;

           case AM_SUBF: POPF(svalf); CURF -= svalf; break;
 /* */     case AM_SUBI: POPI(svali); CURI -= svali; break;
           case AM_SUB3:
                        {
                         TFloat x,y,z;
                         POPF(z);
                         POPF(y);
                         POPF(x);
                         (SP-1)->f -= z;
                         (SP-2)->f -= y;
                         (SP-3)->f -= x;
                        }
                        break;


           case AM_POWFI:
                        {
                        TFloat val;
                        POPI(svali);
                        POPF(svalf);
                        val = svalf;

                        if( svali == 0 )
                        {
                             PUSHF(1);
                        }
                        else
                        if( svali > 0 )
                        {
                             for(; svali > 1 ; --svali )
                                  svalf *= val;
                             PUSHF(svalf);
                        }
                        else
                        {
                             for(; svali < -1 ; ++svali )
                                  svalf *= val;

                             svalf = 1.0 / svalf;
                             PUSHF(svalf);
                        }
                        }
                        break;
           case AM_POWI:
                        {
                        TInt val,power;
                        POPI(power);
                        POPI(svali);
                        val = svali;

                        if( power == 0 )
                        {
                             PUSHI(1);
                        }
                        else
                        if( power > 0 )
                        {
                             for(; power > 1 ; --power )
                                  svali *= val;
                             PUSHI(svali);
                        }
                        else lng_ASSERTNQ("RunTime: INT power (-)");
                        }
                        break;
           case AM_POWF:
                        {
                        TFloat val;
                        POPF(val);
                        POPF(svalf);
                        lng_ASSERT(svalf>0,"RunTime: Negativ base");
                        PUSHF(exp(log(svalf)*val));
                        }
                        break;
           case AM_SQRT:
                        lng_ASSERT(CURF>=0,"Runtime: SQRT(-)");
                        CURF = sqrt(CURF);
                        break;
           case AM_DIVF: POPF(svalf); CURF /= svalf; break;
           case AM_DIVI: POPI(svali); CURI /= svali; break;
           case AM_DIV3F:
                         POPF(svalf);
                         svalf = 1.0 / svalf;
                         (SP-1)->f *= svalf;
                         (SP-2)->f *= svalf;
                         (SP-3)->f *= svalf;
                         break;

           case AM_MULF: POPF(svalf); CURF *= svalf; break;
 /* */     case AM_MULI:
                          POPI(svali);
                          CURI *= svali;
                          break;
           case AM_MUL33:
                         {
                         TFloat x0,y0,z0, x1,y1,z1;
                         POPF(z0); POPF(y0); POPF(x0);
                         POPF(z1); POPF(y1); POPF(x1);
                         PUSHF(x0*x1 + y0*y1 + z0*z1);
                         }
                         break;
           case AM_MUL3:
                         {
                         TFloat x0,y0,z0, x1,y1,z1;
                         POPF(z1); POPF(y1); POPF(x1);
                         POPF(z0); POPF(y0); POPF(x0);
                         PUSHF(y0*z1 - z0*y1);
                         PUSHF(z0*x1 - x0*z1);
                         PUSHF(x0*y1 - y0*x1);
                         }
                         break;
           case AM_MULF3:
                         {
                         POPF(svalf);
                         (SP-1)->f   *= svalf;
                         (SP-2)->f   *= svalf;
                         (SP-3)->f   *= svalf;
                         }
                         break;
           /*-------------------------------*/
           case AM_LSSI: CURI=CURI <  0; break;
           case AM_LEQI: CURI=CURI <= 0; break;
 /* */     case AM_GRTI: CURI=CURI >  0; break;
           case AM_GEQI: CURI=CURI >= 0; break;
           case AM_EQUI: CURI=CURI == 0; break;
 /* */     case AM_NEQI: CURI=CURI != 0; break;

           case AM_LSSF: CURI=CURF <  0.0; break;
           case AM_LEQF: CURI=CURF <= 0.0; break;
           case AM_GRTF: CURI=CURF >  0.0; break;
           case AM_GEQF: CURI=CURF >= 0.0; break;
           case AM_EQUF: CURI=CURF == 0.0; break;
           case AM_NEQF: CURI=CURF != 0.0; break;
           /*-------------------------------*/

           case AM_PRINTI: POPI(svali); printf("%li", svali); break;
           case AM_PRINTF: POPF(svalf); printf("%lf", svalf); break;
 /* */     case AM_PRINTS: POPI(svali); printf("%s",&(code[(int)svali])); break;
           case AM_PRINT3:
                          {
                           TFloat x,y,z;
                           POPF(z);
                           POPF(y);
                           POPF(x);
                           printf("[ %lf, %lf, %lf ]",x,y,z);
                          }
                          break;
 /* */     case AM_PRINTEOL: printf("\n"); break;


 /* */     case AM_CALL:
                  GETITO(svali);

                  if( CODEI+S_PTR2INT(SP) > pc->m_stackSize - 4 )
                  {
                       printf("Stack overflow");
                       return 1;
                  }
                  CODESTEPI;
                  PUSHI( S_PTR2INT(BP) );
                  PUSHI( C_PTR2INT(IP) );
                  IP = C_INT2PTR(svali);
                  BP = SP;
                  break;

           case AM_CALLEXTERN:
                  {
                  TFuncExtern fex;

                  fex=*((TFuncExtern*)IP);
                  IP+=sizeof(TFuncExtern);
                  GETITO(svali);

                  pc->SP = S_PTR2INT(SP);
                  pc->BP = S_PTR2INT(BP);
                  pc->IP = C_PTR2INT(IP);

                  fex( pc,arena );
                  SP -= (int)svali;
                  }
                  break;

           case AM_RETURN:
                  POPI( svali );
                  if( svali < 0 )
                  {
                       isQuit = 1;
                       goto endOfInter;
                  }
                  IP = C_INT2PTR(svali);
                  POPI( svali );
                  BP = S_INT2PTR(svali);
                  break;

 /* */     case AM_RETURN_CLR:
                  {
                  TInt paramSize;

                  GETITO(svali);
                  GETITO(paramSize);

                  SP -= (int)svali;
                  POPI( svali );
                  if( svali < 0 )
                  {
                       isQuit = 1;
                       goto endOfInter;
                  }
                  IP = C_INT2PTR(svali);
                  POPI( svali );
                  BP = S_INT2PTR(svali);
                  SP -= (int)paramSize;
                  }
                  break;

           case AM_MOD:  POPI(svali); CURI = CURI % svali; break;
           case AM_SIN:  CURF = sin (CURF); break;
           case AM_COS:  CURF = cos (CURF); break;
           case AM_TAN:  CURF = tan (CURF); break;
           case AM_ASIN: CURF = asin(CURF); break;
           case AM_ACOS: CURF = acos(CURF); break;
           case AM_ATAN: CURF = atan(CURF); break;
           case AM_LOG:  CURF = log (CURF); break;
           case AM_EXP:  CURF = exp (CURF); break;
           case AM_ABSI: if(CURI<0) CURI = -CURI; break;
           case AM_ABSF: if(CURF<0) CURF = -CURF; break;
           case AM_ABS3:
                  {
                  TFloat x,y,z;
                  POPF(z); POPF(y); POPF(x);
                  PUSHF(sqrt(x*x+y*y+z*z));
                  }
                  break;
 /* */     case AM_NEGI: CURI = -CURI; break;
 /* */     case AM_NEGF: CURF = -CURF; break;
 /* */     case AM_NEG3:
                  (SP-1)->f = -(SP-1)->f;
                  (SP-2)->f = -(SP-2)->f;
                  (SP-3)->f = -(SP-3)->f;
                  break;
 /* */     case AM_NOT: CURI = !CURI; break;
           case AM_ATAN2: POPF(svalf); CURF = atan2(CURF,svalf); break;

           case AM_CIF: CURF = CURI; break;
           case AM_CFI: CURI = (TInt)CURF; break;

           case AM_GETFIELDX: SP -= 2; break;
           case AM_GETFIELDY: SP -= 1;
                              POPF(svalf);
                              SP -= 1;
                              PUSHF(svalf);
                              break;
           case AM_GETFIELDZ: POPF(svalf);
                              SP -= 2;
                              PUSHF(svalf);
                              break;
           case AM_RNDI:
                  CURI = ((TInt)rand())*CURI/ (RAND_MAX+1);
                  break;

           case AM_RNDF:
                  PUSHF( ((TFloat)rand())/( RAND_MAX ) );
                  break;

 /* */     case AM_ADDCONSTI:
                  CURI += CODEI;
                  CODESTEPI;
                  break;

 /* */     case AM_MOV_VCONSTI:
                  GETITO(svali);
                  (BP+(int)svali)->i = CODEI;
                  CODESTEPI;
                  break;

 /* */     case AM_ADD_VCONSTI:
                  GETITO(svali);
                  (BP+(int)svali)->i += CODEI;
                  CODESTEPI;
                  break;

 /* */     case AM_EQU3:
                  {
                  TFloat x0,y0,z0, x1,y1,z1;
                  POPF(z0);
                  POPF(y0);
                  POPF(x0);

                  POPF(z1);
                  POPF(y1);
                  POPF(x1);
                  PUSHI( x0==x1 && y0==y1 && z0==z1 );
                  break;
                  }

 /* */     case AM_NEQ3:
                  {
                  TFloat x0,y0,z0, x1,y1,z1;
                  POPF(z0);
                  POPF(y0);
                  POPF(x0);

                  POPF(z1);
                  POPF(y1);
                  POPF(x1);
                  PUSHI( x0!=x1 || y0!=y1 || z0!=z1 );
                  break;
                  }

           case AM_ADD_VARI: CURI += (BP+(int)CODEI)->i; CODESTEPI; break;
           case AM_ADD_VARF: CURF += (BP+(int)CODEI)->f; CODESTEPI; break;

           case AM_SUB_VARI: CURI -= (BP+(int)CODEI)->i; CODESTEPI; break;
           case AM_SUB_VARF: CURF -= (BP+(int)CODEI)->f; CODESTEPI; break;

           case AM_MUL_VARI: CURI *= (BP+(int)CODEI)->i; CODESTEPI; break;
           case AM_MUL_VARF: CURF *= (BP+(int)CODEI)->f; CODESTEPI; break;

           case AM_DIV_VARI: CURI /= (BP+(int)CODEI)->i; CODESTEPI; break;
           case AM_DIV_VARF: CURF /= (BP+(int)CODEI)->f; CODESTEPI; break;

           case AM_MOD_VAR : CURI %= (BP+(int)CODEI)->i; CODESTEPI; break;

           case AM_PUSH_CEXTERNI:
                             PUSHI( pc->m_stack[ (int)(CODEI) ].i );
                             CODESTEPI;
                             break;

           case AM_PUSH_CEXTERNF:
                             PUSHF( pc->m_stack[ (int)(CODEI) ].f );
                             CODESTEPI;
                             break;

           default:
                  {
                  AM_TYPE opcode = (AM_TYPE)(*(IP-1));
                  printf( "Invalid opcode %i\n", (int)opcode );
                  exit( 0 );
                  }
           }
    }
  endOfInter:
    pc->SP = S_PTR2INT(SP);
    pc->BP = S_PTR2INT(BP);
    pc->IP = C_PTR2INT(IP);

    return isQuit;
 }


