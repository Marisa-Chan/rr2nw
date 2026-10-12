/*
	File:  CGEN.C
	Autor: Suavik
	Ver:   1.0

	Создание по дереву кода байт-кода, линкерной и дебагерной
	информации.

	Префикс: cg_

	BUGS:
 *  PutByte, PutInt ... сделать от TCodeStream
 *  Не правильно сохраняется link - информация:
	  Одна функция многократно дублируется.
	Дублируются строки - ввести кэш для строк.

 *  Ввести команду  добавления    (add ..., константы)
 *	Поместить в линкеpную инфоpмацию инфоpмацию о внешних константах

    Добор до двухмерного массива занимает слишком много операций. Надо
    попробовать сократить их.

    24.09.97 Не правильно работал Break из вложенного цикла - была попытка
             при поиске имени цикла брать имя, когда оно отсутствовало.
    06.09.97 Не правильно обрабатывался список операторов CPU_OPERATOR.

    29.10.97 Из процедуры cg_CodeGen вынесены отдельные участки, чтобы
             меньше резервировалось памяти под стек
    02.11.97 Не было инициализациии константных переменных. Не вызывалась
             функция подсчета смещений.
 */

#include "cgen.h"
#include "bytecod.h"
#include "strequ.h"
#include "usestack.h"

 /*************************
  *                       *
  * Code stream functions *
  *                       *
  *************************/

 /*========================================================================*/
TInt  cg_GetCurCodePtr( TCGContext  *cgc )
 {
	return os_CurPos( cgc->m_cs );
 }

 /*========================================================================*/
void cg_InitTCGContext( TCGContext    *cgc,
						os_TOutStream *cs,
						os_TOutStream *linkInfo,
						TError        *error )
 {
	cgc->m_cs        = cs;
	cgc->m_error     = error;
	cgc->m_localSize = 0;
	cgc->m_paramSize = 0;
	cgc->m_loopList  = NIL;
    cgc->m_linkInfo  = linkInfo;
    cgc->m_linkList  = -1;
    cgc->m_retStackPos = 0;
 }


 /*========================================================================*/
void cg_ClearTCGContext( TCGContext *cgc )
 {
    cgc->m_localSize = 0;
    cgc->m_paramSize = 0;
    cgc->m_loopList  = NIL;
    cgc->m_linkList  = -1;
    cgc->m_retStackPos = 0;
 }

 /*========================================================================*/
void cg_DropTCGContext( TCGContext *cgc )
 {
    cg_ClearTCGContext( cgc );
    cgc->m_error    = NULL;
    cgc->m_cs       = NULL;
    cgc->m_linkInfo = NULL;
 }

 /*========================================================================*/
void cg_PutByte( TCGContext *cgc, TInt b )
 {
    os_PutByte( cgc->m_cs, b );
 }

 /*========================================================================*/
void cg_PutInt( TCGContext *cgc, TInt val )
 {
    os_PutInt( cgc->m_cs, val );
 }

 /*========================================================================*/
TInt cg_PutSysInt( TCGContext *cgc, int val )
 {
    TInt oldPos = cg_GetCurCodePtr( cgc );
    os_PutInt( cgc->m_cs, _TINT(val) );
    return oldPos;
 }

 /*========================================================================*/
void cg_PutEnum( TCGContext *cgc, int val )
 {
    os_PutInt( cgc->m_cs, _TINT(val) );
 }

 /*========================================================================*/
void cg_PutPtr( TCGContext *cgc, TBytePtr val )
 {
    os_PutPtr( cgc->m_cs, val );
 }

 /*========================================================================*/
void cg_PutFloat( TCGContext *cgc, TFloat val )
 {
    os_PutFloat( cgc->m_cs, val );
 }

 /*========================================================================*/
void cg_PutStr( TCGContext *cgc, const char *str )
 {
     os_PutStr( cgc->m_cs, str );
 }
 /*========================================================================*/
void cg_PutCOP( TCGContext *cgc, int val )
 {
#    if COPSIZE != 1
     cg_PutSysInt( cgc, val );
#    else
     cg_PutByte  ( cgc, _TINT(val) );
#    endif
 }

 /*========================================================================*/
void cg_SetByte( TCGContext *cgc, TInt pos, int b)
 {
    os_SetByte( cgc->m_cs, (int)pos, _TINT( b ) );
 }

 /*========================================================================*/
void cg_SetInt( TCGContext *cgc, TInt pos, TInt val )
 {
    os_SetInt( cgc->m_cs, (int)pos, val );
 }

 /*========================================================================*/
void cg_SetPtr( TCGContext *cgc, TInt pos, TBytePtr val )
 {
    os_SetPtr( cgc->m_cs, (int)pos, val );
 }


 /*******************
  *                 *
  * Link info utils *
  *                 *
  *******************/
 /*========================================================================*/
void cg_LinkOutByte( TCGContext *cgc, int b )
 {
    os_PutByte( cgc->m_linkInfo, _TINT( b ) );
 }

 /*========================================================================*/
void cg_LinkOutInt( TCGContext *cgc, TInt val )
 {
    os_PutInt( cgc->m_linkInfo, val );
 }

 /*========================================================================*/
void cg_LinkOutEnum(  TCGContext *cgc, int val )
 {
    cg_LinkOutInt( cgc, _TINT(val) );
 }

 /*========================================================================*/
void cg_LinkOutSysInt( TCGContext *cgc, int val )
 {
    cg_LinkOutInt( cgc, _TINT(val) );
 }

 /*========================================================================*/
void cg_LinkOutPtr( TCGContext *cgc, TBytePtr val )
 {
    os_PutPtr( cgc->m_linkInfo, val );
 }

 /*========================================================================*/
void cg_LinkSetByte( TCGContext *cgc, TInt pos, int b)
 {
    os_SetByte( cgc->m_linkInfo, (int)pos, _TINT( b ) );
 }

 /*========================================================================*/
void cg_LinkSetInt( TCGContext *cgc, TInt pos, TInt val )
 {
    os_SetInt( cgc->m_linkInfo, (int)pos, val );
 }

 /*========================================================================*/
void cg_LinkOutStr( TCGContext *cgc, const char *str )
 {
     os_PutStr( cgc->m_linkInfo, str );
 }

 /*========================================================================*/
 /*
  *     Формирование информации для линкера
  *
  *     [ ] указатель на имя (строку)
  *     [ ] тип вызова (FT_SCRIPT/FT_EXTERN)
  *     [ ] указатель на следующий элемент
  *     [ ] указатель на элемент списка адресов
  *     [][][][] строка
  */
void cg_OutLink( TCGContext *cgc, TInt adrPos, TName *descr )
 {
    os_TOutStream  *linkInfo = cgc->m_linkInfo;
    TInt         namePtr  = cgc->m_linkList;
    const char  *searchName = ld_GetName( descr ),
                *curName;
    /*
     * Ищем имя функции
     */
    while( namePtr != -1 && namePtr < os_CurPos( linkInfo ) )
    {
           TInt name, next;

           name     = os_GetIntStep( linkInfo, &namePtr );
                      os_GetIntStep( linkInfo, &namePtr );
           next     = os_GetIntStep( linkInfo, &namePtr );
           curName  = os_GetStr    ( linkInfo, name );

           if( str_StrEQU( curName, searchName) )
           {
                TInt posOldAdr, oldAdr, posOldNext, oldNext, lastDefPos;
                /*
                 * Добавляем наш адрес в список данного имени.
                 * Для этого читаем первые в списке adr-next,
                 * Помещаем их в конце последовательности и
                 * на их месте записываем новую последовательность
                 * со ссылкой на то место, куда перенесли старую
                 */
                namePtr = os_GetIntStep( linkInfo, &namePtr );

                posOldAdr  = namePtr;
                oldAdr     = os_GetIntStep( linkInfo, &namePtr );
                posOldNext = namePtr;
                oldNext    = os_GetIntStep( linkInfo, &namePtr );
                /*
                 * считываем позицию, куда будем писать
                 */
                lastDefPos    = os_CurPos( linkInfo );

                /*
                 * Переносим в хвост списка.
                 */
                cg_LinkOutInt( cgc, oldAdr  );
                cg_LinkOutInt( cgc, oldNext );

                cg_LinkSetInt( cgc, posOldAdr,  adrPos  );
                cg_LinkSetInt( cgc, posOldNext, lastDefPos );
                return;
           }

           namePtr = next;
    }

    {
    /*
     * Поиск имени завершился неудачно
     */
    TInt nameCell = os_CurPos( linkInfo );
    TInt adrListPos, prevNameCell;

    prevNameCell    = cgc->m_linkList;
    cgc->m_linkList = nameCell;

    cg_LinkOutInt( cgc, _TINT(0) );                /* name Ptr  */
    switch( ld_GetNameDef( descr ) )
    {
    case DEF_FUNC:        cg_LinkOutEnum( cgc, FT_SCRIPT ); break;
    case DEF_FUNCEXTERN:  cg_LinkOutEnum( cgc, FT_EXTERN ); break;
    case DEF_CONSTEXTERN: cg_LinkOutEnum( cgc, FT_CNSTEX ); break;
    default:
            lng_ASSERTNQ( "cg_OutLink: unknown func def" );
    }
    cg_LinkOutInt( cgc, prevNameCell );            /* next name */
    adrListPos = os_CurPos( linkInfo );
    cg_LinkOutInt( cgc, _TINT(-1) );               /* ptr list */

    if( ld_GetNameDef( descr ) != DEF_CONSTEXTERN )
    {
         cg_LinkSetInt( cgc, nameCell, os_CurPos( linkInfo ) );
         cg_LinkOutStr( cgc, searchName );

         cg_LinkSetInt( cgc, adrListPos, os_CurPos( linkInfo ) );
         cg_LinkOutInt( cgc, adrPos );
         cg_LinkOutInt( cgc, -1L );
    }
    else
    {
         cg_LinkOutSysInt( cgc, ld_GetDataPtr( descr ) );
         cg_LinkSetInt   ( cgc, nameCell, os_CurPos( linkInfo ) );
         cg_LinkOutStr   ( cgc, searchName );
    }

    }
 }

 /*********
  *       *
  * Utils *
  *       *
  *********/

 /*========================================================================*/
 /*
  * Запись дебагерной информации о переменной.
  */
void cg_OutFuncParam( TCGContext *cgc,
                      TName      *descr,
                      TInt       *fieldNext )
 {
    TInt strPtr = cg_GetCurCodePtr( cgc );
    cg_PutSysInt( cgc, 0 );                      /*  name     */
    cg_PutEnum  ( cgc, ld_GetType( descr ));     /*  type     */
    cg_PutEnum  ( cgc, ld_GetNameDef( descr ));  /*  name def */
    cg_PutSysInt( cgc, ld_GetDataPtr( descr ));  /*  stackPos */
    *fieldNext = cg_GetCurCodePtr( cgc );
    cg_PutSysInt( cgc, 0 );                      /*  next     */

    cg_SetInt( cgc, strPtr, cg_GetCurCodePtr( cgc ) );
    cg_PutStr( cgc, ld_GetName( descr ) );
 }

 /*========================================================================*/
void cg_VarToList( TCGContext    *cgc,
                   TName         *descr,
                   TInt           listPtr,
                   int           *firstParam,
                   TInt          *prevVarPtr )
 {
    TInt curPtr = cg_GetCurCodePtr( cgc );
    TInt newPtr = *prevVarPtr;

    cg_OutFuncParam( cgc, descr, &newPtr );

    if( *firstParam  )
    {
         cg_SetInt( cgc, listPtr, curPtr );
         *firstParam = 0;
    }
    else cg_SetInt( cgc, *prevVarPtr, curPtr );

    *prevVarPtr = newPtr;
 }

 /*========================================================================*/
int cg_GetParamSize( TName *descr )
 {
    int typeSize;

    if( ld_GetNameDef( descr ) == DEF_PARVAR )
         return 1;


    switch( ld_GetType( descr ) )
    {
    case T_INT:    typeSize = 1;  break;
    case T_FLOAT:  typeSize = 1;  break;
    case T_VECTOR: typeSize = 3;  break;
    case T_STR:    typeSize = 1;  break;
    default:
            lng_ASSERTNQ( "cg_CalcParamSize: param type unknown" );
    }

    if( ld_GetArrayCnt( descr ) != 0 )
    {
         int i;

         for( i = 0; i < ld_GetArrayCnt( descr ) ; ++i )
              typeSize *= (int)ld_GetArrayRange( descr, i );
    }

    return typeSize;
 }

 /*========================================================================*/
int cg_CalcAllParamSize( TName *descr )
 {
    int stackSize = 0;

    while( descr != NIL  && ld_GetNameDef( descr ) != DEF_VAR )
    {
         stackSize += cg_GetParamSize( descr );
         descr = ld_GetNext( descr );
    }

    return stackSize;
 }

 /*========================================================================*/
void cg_OutDebugInfo( TCGContext *cgc,
                      TTree      *p,
                      int *ret, int *par, int *loc,
                      TInt *codeNextPtr )
 {
    int        stackPos;
    TName     *descr;
    int        firstParam = 1;
    TInt       prevVarPtr = 0;
    struct {
      TInt  defs;
      TInt  code;
      TInt  funcName;
    } head;

    *ret = 0;
    *par = 0;
    *loc = 0;

    if( h_TypeOf( p ) != CPU_FUNCTION &&
        h_TypeOf( p ) != SY_EXTERN )
    {
         lng_ASSERTNQ( "cg_OutDebugInfo: Unknown code" );
    }

    /*
     * Выводим заголовок
     */
    cg_PutInt( cgc, FUNCMAGIC );              /* magic */
    *codeNextPtr  = cg_PutSysInt( cgc, 0 );   /* next  */
    head.defs     = cg_PutSysInt( cgc, 0 );   /* defs  */
    head.code     = cg_PutSysInt( cgc, 0 );   /* code  */
    head.funcName = cg_PutSysInt( cgc, 0 );   /* funcName */
    cg_PutEnum( cgc, tr_GetVarType( car(p) ));/* funcType */

    descr = h_GetDescr( car(p) );
    if( ld_GetNameDef( descr ) == DEF_FUNC )
    {
         /*
          * Сохраняем потребность в стеке для CheckStack
          */
         cg_PutSysInt( cgc, ld_GetDataPtr( descr ) );
    }
    /*
     * Устанавливаем указатель на имя
     */
    cg_SetInt( cgc, head.funcName, cg_GetCurCodePtr( cgc ) );
    cg_PutStr( cgc, ld_GetName( descr ) );

    /*
     * Определяем число возвращаемых параметров
     */
    switch( tr_GetVarType( car(p) ) )
    {
    case T_NONE:    *ret = 0; break;
    case T_INT:     *ret = 1; break;
    case T_FLOAT:   *ret = 1; break;
    case T_VECTOR:  *ret = 3; break;
    default: lng_ASSERTNQ("cg_OutDebugInfo: return unknown type");
    }

    /*
     * Определяем объем параметров в стеке
     */
    descr = h_GetDescr( car(p) );
    descr = ld_GetNext( descr );
    *par  = cg_CalcAllParamSize( descr );
    stackPos = -CALL_STACK_RESERVED - *par;
    cgc->m_retStackPos = stackPos - *ret;

    while( descr != NIL  && ld_GetNameDef( descr ) != DEF_VAR )
    {
         ld_SetDataPtr( descr, (int)stackPos );
         stackPos += cg_GetParamSize( descr );

         /*
          * Соединяем переменные в список
          */
         cg_VarToList( cgc, descr, head.defs, &firstParam, &prevVarPtr );

         descr = ld_GetNext( descr );
    }

    if( stackPos != -CALL_STACK_RESERVED )
    {
         lng_ASSERTNQ("Error stackPos");
    }
    /*
     * Учитываем код возврата
     */
    stackPos = 0;

    /*
     * Просматриваем локальные переменные
     */
    while( descr != NIL )
    {
         if( ld_GetNameDef( descr ) == DEF_VAR )
         {
              int typeSize = 0;

              ld_SetDataPtr( descr, stackPos );
              typeSize = cg_GetParamSize( descr );
              cg_VarToList( cgc, descr, head.defs, &firstParam, &prevVarPtr );

              stackPos += typeSize;
              (*loc)   += typeSize;
         }

         descr = ld_GetNext( descr );
    }
    stackPos = stackPos;

#   if COPSIZE != 1
    cg_SetInt( cgc, head.code,
                (cg_GetCurCodePtr( cgc )+COPSIZE-1)
               &(~((TInt)(COPSIZE-1))) );
#   else
    cg_SetInt( cgc, head.code, cg_GetCurCodePtr( cgc ) );
#   endif

 }

 /*========================================================================*/
 /*
  *     [ ] указатель на имя (строку)
  *     [ ] тип вызова (FT_SCRIPT/FT_EXTERN)
  *     [ ] указатель на следующий элемент
  *     [ ] указатель на элемент списка адресов
  *     [][][][] строка
  */
int cg_CalcOffsetConstExtern( TTree *p )
 {
    int offset = 0;

    while( p != NIL )
    {
         LEX_TYPE lex = h_TypeOf( p );

         if( lex == CPU_PUSH_VI ||
             lex == CPU_PUSH_VF ||
             lex == CPU_PUSH_VS )
         {
              ld_SetDataPtr( h_GetDescr( p ), offset );
              ++offset;
         }
         p = cdr(p);
    }

    return offset;
 }

 /*****************************
  *                           *
  * Code generation functions *
  *                           *
  *****************************/

 /*========================================================================*/
void cg_Reserved( TCGContext *cgc, TInt cnt )
 {
    if( cnt > 0 )
    {
         cg_PutCOP( cgc, AM_RESERVED );
         cg_PutInt( cgc, cnt );
    }
 }


 /*========================================================================*/
void cg_CodeGenBinOp( TCGContext *cgc, TTree *p, int binOp )
 {
    cg_CodeGen( cgc, car(p) );
    cg_CodeGen( cgc, cdr(p) );
    cg_PutCOP ( cgc, binOp );
 }

 /*========================================================================*/
void cg_CodeGenUnaOp( TCGContext *cgc, TTree *p, int unaOp )
 {
    if( cdr(p) != NIL )
    {
         lng_ASSERTNQ("cg_CodeGenUnaOp: Error unar op\n");
    }
    cg_CodeGen( cgc, car(p) );
    cg_PutCOP ( cgc, unaOp );
 }

 /*========================================================================*/
void cg_GenRETURN( TCGContext *cgc )
 {
    if( cgc->m_localSize == 0 && cgc->m_paramSize == 0 )
         cg_PutCOP( cgc, AM_RETURN );
    else
    {
         cg_PutCOP   ( cgc, AM_RETURN_CLR );
         cg_PutSysInt( cgc, cgc->m_localSize );
         cg_PutSysInt( cgc, cgc->m_paramSize );
    }
 }

 /*========================================================================*/
void cg_GenPOP_V( TCGContext *cgc, TTree *p, AM_TYPE par, AM_TYPE parVar )
 {
    TName *descr = h_GetDescr( p );

    switch( ld_GetNameDef( descr ) )
    {
    case DEF_PARVAR: cg_PutCOP   ( cgc, parVar );
                     cg_PutSysInt( cgc, ld_GetDataPtr( descr ) );
                     break;
    case DEF_VAR:    /* FALLSTHROUGH */
    case DEF_PAR:    cg_PutCOP   ( cgc, par );
                     cg_PutSysInt( cgc, ld_GetDataPtr( descr ) );
                     break;
    default:
            lng_ASSERTNQ( "cg_GenPOP_V" );
    }
 }

 /*========================================================================*/
void cg_GenPOP_A( TCGContext *cgc, TTree *p, AM_TYPE par, AM_TYPE parVar )
 {
    cg_CodeGen( cgc, car(p) );
    cg_GenPOP_V( cgc, p, par, parVar );
 }

 /*========================================================================*/
void cg_CodeGenVarToStack( TCGContext *cgc, TTree *p, AM_TYPE op )
 {
    cg_CodeGen   ( cgc, car(p) );
    cg_PutCOP    ( cgc, op );
    cg_PutSysInt ( cgc, ld_GetDataPtr( h_GetDescr( cdr(p) )));
 }

 /*========================================================================*/
void cg_CodeGenPushVar( TCGContext *cgc,
                        TTree      *p,
                        AM_TYPE    am_push_p,
                        AM_TYPE    am_push_v,
                        AM_TYPE    am_push_cextern )
 {
    TName *descr = h_GetDescr( p );

    switch( ld_GetNameDef( descr ) )
    {
    case DEF_PARVAR: cg_PutCOP ( cgc, am_push_p );       break;
    case DEF_VAR:    /* FALLSTHROUGH */
    case DEF_PAR:    cg_PutCOP ( cgc, am_push_v );       break;
    case DEF_CONSTEXTERN:
                     cg_PutCOP ( cgc, am_push_cextern ); break;
    default:
            lng_ASSERTNQ("cg_CodeGenPushVar: Unknown variable def");
    }

    cg_PutSysInt ( cgc, ld_GetDataPtr( descr ) );
 }

 /*========================================================================*/
void cg_CodeGenPush3(  TCGContext *cgc,
                       TTree      *p,
                       AM_TYPE     am_push_p,
                       AM_TYPE     am_push )
 {
    TName *descr = h_GetDescr( p );

    if( ld_GetNameDef( descr ) == DEF_PARVAR )
         cg_PutCOP ( cgc, am_push_p );
    else cg_PutCOP ( cgc, am_push   );

    cg_PutSysInt ( cgc, ld_GetDataPtr( descr ) );
 }

 /*========================================================================*/
 /*
  * Вывод операции перехода для CONTINUE и BREAK.
  * Для каждого цикла создается список адресов, указывающих на
  * метку выхода и список адресов продолжения цикла.
  * на вершину списка указывают:
  *      cgc->m_loopList->m_breakList,
  *      cgc->m_loopList->m_contList.
  * Сам список содержится в коде операции в полях адреса
  */
void cg_CodeGenContBreak( TCGContext *cgc,
                          TTree      *p,
                          int         isBreak )
 {
    TInt destAdrPtr;

    if( car(p) == NIL )
    {
         TInt oldLabelPtr;

         cg_PutCOP ( cgc, AM_JMP );
         destAdrPtr = cg_GetCurCodePtr( cgc );

         if( isBreak )
         {
              oldLabelPtr                  = cgc->m_loopList->m_breakList;
              cgc->m_loopList->m_breakList = destAdrPtr;
         }
         else
         {
              oldLabelPtr                  = cgc->m_loopList->m_contList;
              cgc->m_loopList->m_contList  = destAdrPtr;
         }
         cg_PutInt ( cgc, oldLabelPtr );
    }
    else
    {
         TLoopItem *li = cgc->m_loopList;
         int found = 0;

        /*
         * ищем метку
         */
         while( li != NIL )
         {
              if( li->m_descr != NIL ) /* CHECKME */
              if( str_StrEQU( ld_GetName( li->m_descr ),
                              tr_GetVarName( car(p) )))
              {
                   TInt oldLabelPtr;

                   cg_PutCOP ( cgc, AM_JMP );
                   destAdrPtr = cg_GetCurCodePtr( cgc );

                   if( isBreak )
                   {
                        oldLabelPtr     = li->m_breakList;
                        li->m_breakList = destAdrPtr;
                   }
                   else
                   {
                        oldLabelPtr     = li->m_contList;
                        li->m_contList  = destAdrPtr;
                   }
                   cg_PutInt ( cgc, oldLabelPtr );
                   found = 1;
                   break;
              }

              li = li->m_next;
         }

         if( !found )
              lng_Error( cgc->m_error,
                         "Label %s does not belong to this loop",
                         tr_GetVarName( car(p) )  );
    }
 }

 /*========================================================================*/
void cg_CodeGenFOR( TCGContext *cgc, TTree *p )
 {
    TInt  pos, ptr, ptr1;
    TName *descr;
    TTree *p0   = car(car(p));
    TName *dest = h_GetDescr( cdr(cdr(car(p))) );

    descr = h_GetDescr( cdr(p0) );

    /*
     * Начальная инициализация цикловой переменной
     */
    cg_CodeGen( cgc, cdr(car(p)));

    h_TypeSet( p0, CPU_MOVI );
    cg_CodeGen( cgc, p0 );

    /*
     * Формирование выхода, если начальное условие ложно
     */
    switch( h_TypeOf( p ) )
    {
    case CPU_FOR:
            switch( ld_GetNameDef( descr ) )
            {
            case DEF_PAR: case DEF_VAR:
                    cg_PutCOP ( cgc, AM_LEAVE );   break;
            default:
                    lng_ASSERTNQ("cg_CodeGen: CPU_FOR:CPU_FOR");
            }
            break;
    case CPU_FORD:
            switch( ld_GetNameDef( descr ) )
            {
            case DEF_PAR: case DEF_VAR:
                    cg_PutCOP ( cgc, AM_LEAVED );   break;
            default:
                    lng_ASSERTNQ("cg_CodeGen: CPU_FOR:CPU_FORD");
            }
            break;
    }
    cg_PutSysInt ( cgc, ld_GetDataPtr( descr ) );
    cg_PutSysInt ( cgc, ld_GetDataPtr( dest  ) );
    pos = cg_PutSysInt ( cgc, 0 );


    /*
     * Тело цикла
     */
    ptr = cg_GetCurCodePtr( cgc );
    cg_CodeGen( cgc, cdr(p) );

    /*
     * Шаг цикла
     */
    cgc->m_loopList->m_repPtr = cg_GetCurCodePtr( cgc );

    switch( h_TypeOf( p ) )
    {
    case CPU_FOR:
            switch( ld_GetNameDef( descr ) )
            {
            case DEF_PAR: case DEF_VAR:
                    cg_PutCOP ( cgc, AM_LOOP );   break;
            default:
                    lng_ASSERTNQ("cg_CodeGen: error def\n");
            }
            break;
    case CPU_FORD:
            switch( ld_GetNameDef( descr ) )
            {
            case DEF_PAR: case DEF_VAR:
                    cg_PutCOP ( cgc, AM_LOOPD );   break;
            default:
                    lng_ASSERTNQ("cg_CodeGen: error def\n");
            }
            break;
    }
    cg_PutSysInt ( cgc, ld_GetDataPtr( descr ) );
    cg_PutSysInt ( cgc, ld_GetDataPtr( dest  ) );
    ptr1 = cg_GetCurCodePtr( cgc );
    cg_PutInt ( cgc, ptr - ptr1 - sizeof( TInt ) );

    /*
     * Выход
     */
    cg_SetInt( cgc, pos, cg_GetCurCodePtr( cgc ) - pos - sizeof( TInt ));

 }

 /*========================================================================*/
void cg_CodeGenCYCLE( TCGContext *cgc, TTree *p )
 {
    TLoopItem loopItem, *oldList;
    TInt      breakPtr, endOfLoop, repPtr;

    loopItem.m_repPtr    = 0;
    loopItem.m_breakList = 0;
    loopItem.m_contList  = 0;

    if( cdr(p) != NIL )
         loopItem.m_descr= h_GetDescr( cdr(p) );
    else loopItem.m_descr= NIL;

    loopItem.m_next      = cgc->m_loopList;

    oldList           = cgc->m_loopList;
    cgc->m_loopList   = &loopItem;

    cg_CodeGen( cgc, car(p) );
    endOfLoop = cg_GetCurCodePtr( cgc );

    /*
     * Идем по списку BREAK и проставляем адрес
     */
    for( breakPtr = cgc->m_loopList->m_breakList; breakPtr != 0 ;   )
    {
         TInt nextBreak = os_GetInt( cgc->m_cs, (int)breakPtr );

         cg_SetInt( cgc, breakPtr,
                    endOfLoop - breakPtr - sizeof(TInt) );
         breakPtr = nextBreak;
    }

    lng_ASSERT( loopItem.m_repPtr != 0, "cg_CodeGen: loopList" );

    /*
     * Идем по списку CONTINUE и проставляем адрес
     */
    for( repPtr = cgc->m_loopList->m_contList; repPtr != 0 ;    )
    {
         TInt nextRep =os_GetInt( cgc->m_cs, (int)repPtr );

         cg_SetInt( cgc, repPtr,
                    loopItem.m_repPtr - repPtr - sizeof(TInt) );
         repPtr = nextRep;
    }

    cgc->m_loopList = oldList;
 }

 /*========================================================================*/
void cg_CodeGenCallParVar( TCGContext *cgc, TTree *p )
 {
    LEX_TYPE com;
    TName    *descr;

    p = car(p);
    com = h_TypeOf( p );
    descr = h_GetDescr( p );

    switch( com )
    {
    case CPU_PUSH_VI:
    case CPU_PUSH_VF:
    case CPU_PUSH_VV:
    case CPU_PUSH_VS:
            switch( ld_GetNameDef( descr ) )
            {
            case DEF_PAR:
            case DEF_VAR:
                    cg_PutCOP ( cgc, AM_PUSH_ADR );      break;

            case DEF_PARVAR:
                    cg_PutCOP ( cgc, AM_PUSH_VI );       break;

            case DEF_CONSTEXTERN:
                    cg_PutCOP ( cgc, AM_PUSH_CEXTERNI );  break;
            default:
                     lng_ASSERTNQ("cg_CodeGen: Unknown variable def");
            }
            cg_PutSysInt ( cgc, ld_GetDataPtr( descr ) );
            break;

    case CPU_PUSH_AI:
    case CPU_PUSH_AF:
    case CPU_PUSH_AV:
    case CPU_PUSH_AS:
            switch( ld_GetNameDef( descr ) )
            {
            case DEF_PAR:
            case DEF_VAR:
                    cg_PutCOP ( cgc, AM_PUSH_ADR );
                    if( h_TypeOf( car(p) ) == CPU_PUSH_CI )
                         cg_PutInt ( cgc, ld_GetDataPtr( descr ) +
                                     h_GetConstINT( car(p) ) );
                    else
                    {
                         cg_PutSysInt( cgc, ld_GetDataPtr( descr ) );
                         cg_CodeGen  ( cgc, car(p) );
                         cg_PutCOP   ( cgc, AM_ADDI  );
                    }
                    break;
            case DEF_PARVAR:
                    cg_PutCOP    ( cgc, AM_PUSH_VI );
                    cg_PutSysInt ( cgc, ld_GetDataPtr( descr ) );

                    /*
                     * Если есть индекс, то добавляем его
                     */
					if( car(p) != NIL ) /* CHECKME */
                    {
                         cg_CodeGen( cgc, car(p) );
                         cg_PutCOP ( cgc, AM_ADDI );
                    }
                    break;
            default:
                     lng_ASSERTNQ("cg_CodeGen: Unknown variable def");
            }
            break;
    default:
            lng_ASSERTNQ("cg_CodeGenCallParVar:Unknown variable type");
    }
 }

 /*========================================================================*/
void cg_CodeGen_MOVI( TCGContext *cgc, TTree *p )
 {
    if( h_IsConstI( car(p) ) )
    {
         TName *descr = h_GetDescr( cdr(p) );

         switch( h_TypeOf( cdr(p) ) )
         {
         case CPU_POP_VI:
                 switch( ld_GetNameDef( descr ) )
                 {
                 case DEF_VAR: case DEF_PAR:
                         cg_PutCOP   ( cgc, AM_MOV_VCONSTI );
                         cg_PutSysInt( cgc, ld_GetDataPtr( descr ) );
                         cg_PutInt   ( cgc, h_GetConstINT( car(p)) );
                         break;
                 default:
                         cg_CodeGen( cgc, car(p) );
                         cg_CodeGen( cgc, cdr(p) );
                 }
                 break;
         default:
                 cg_CodeGen( cgc, car(p) );
                 cg_CodeGen( cgc, cdr(p) );
         }
    }
    else
    {
         cg_CodeGen( cgc, car(p) );
         cg_CodeGen( cgc, cdr(p) );
    }
 }

 /*========================================================================*/
void cg_CodeGen_IF( TCGContext *cgc, TTree *p )
 {
    TInt pos, ptr, pos1, ptr1;

    switch( h_TypeOf( p ) )
    {
    case CPU_IF:
            if( h_IsConstI( car(p) ) )
            {
                 if( h_GetConstINT( car(p) ) )
                      cg_CodeGen( cgc, cdr(p) );
            }
            else
            {
                 cg_CodeGen( cgc, car(p) );

                 if( h_TypeOf( cdr(p) ) == CPU_BREAK ||
                     h_TypeOf( cdr(p) ) == CPU_CONTINUE )
                 {
                      pos = cg_GetCurCodePtr( cgc );
                      cg_CodeGen( cgc, cdr(p) );
                      cg_SetByte( cgc, pos, AM_JMPIFTRUE );
                 }
                 else
                 {
                      cg_PutCOP    ( cgc, AM_JMPIFFALSE );
                      pos = cg_PutSysInt ( cgc, 0 );
                      ptr = cg_GetCurCodePtr( cgc );

                      cg_CodeGen( cgc, cdr(p) );

                      cg_SetInt( cgc, pos, cg_GetCurCodePtr( cgc ) - ptr );
                 }
            }
            break;

    case CPU_IFELSE:
            if( h_IsConstI( car(p) ) )
            {
                 if( h_GetConstINT( car(p) ) )
                      cg_CodeGen( cgc, car(cdr(p)) );
                 else cg_CodeGen( cgc, cdr(cdr(p)) );
            }
            else
            {
                 cg_CodeGen   ( cgc, car(p) );

                 cg_PutCOP    ( cgc, AM_JMPIFFALSE );
                 pos = cg_PutSysInt ( cgc, 0 );
                 ptr = cg_GetCurCodePtr( cgc );
                 /* THEN */
                 cg_CodeGen   ( cgc, car(cdr(p)) );
                 cg_PutCOP    ( cgc, AM_JMP );
                 pos1 = cg_PutSysInt ( cgc, 0 );
                 ptr1 = cg_GetCurCodePtr( cgc );

                 cg_SetInt( cgc, pos, cg_GetCurCodePtr( cgc ) - ptr );
                 /* ELSE */
                 cg_CodeGen( cgc, cdr(cdr(p)) );

                 cg_SetInt( cgc, pos1, cg_GetCurCodePtr( cgc ) - ptr1 );
            }
            break;
    }
 }

 /*========================================================================*/
void cg_CodeGen_PUSH_CS( TCGContext *cgc, TTree *p )
 {
    TInt pos;

    cg_PutCOP ( cgc, AM_PUSHCS );
	/* FIXME сделать пропуск дублирующихся строк */
    cg_PutInt ( cgc, cg_GetCurCodePtr( cgc ) + sizeof(TInt)*2 );
    pos  = cg_PutSysInt( cgc, 0 );
    cg_PutStr   ( cgc, h_GetConstSTR( p ) );

#           if COPSIZE != 1
    while( (cg_GetCurCodePtr( cgc )&(COPSIZE-1)) != 0 )
          cg_PutByte( cgc, 0L );
#           endif

    cg_SetInt ( cgc, pos, cg_GetCurCodePtr( cgc ) );
 }

 /*========================================================================*/
void cg_CodeGen_PUSH_ARRAY( TCGContext *cgc, TTree *p )
 {
    TName   *descr;

    switch( h_TypeOf( p ) )
    {
    case CPU_PUSH_AI:
    case CPU_PUSH_AS:
            cg_CodeGen( cgc, car(p) );
            descr = h_GetDescr( p );

            if( ld_GetNameDef( descr ) == DEF_PARVAR )
                 cg_PutCOP ( cgc, AM_PUSH_AI_P );
            else cg_PutCOP ( cgc, AM_PUSH_AI );

            cg_PutSysInt ( cgc, ld_GetDataPtr( descr ) );
            break;

    case CPU_PUSH_AF:
            cg_CodeGen( cgc, car(p) );
            descr = h_GetDescr( p );

            if( ld_GetNameDef( descr ) == DEF_PARVAR )
                 cg_PutCOP ( cgc, AM_PUSH_AF_P );
            else cg_PutCOP ( cgc, AM_PUSH_AF );

            cg_PutSysInt ( cgc, ld_GetDataPtr( descr ) );
            break;

    case CPU_PUSH_AV:
            cg_CodeGen( cgc, car(p) );
            descr = h_GetDescr( p );

            if( ld_GetNameDef( descr ) == DEF_PARVAR )
                 cg_PutCOP ( cgc, AM_PUSH_A3_P );
            else cg_PutCOP ( cgc, AM_PUSH_A3 );

            cg_PutSysInt ( cgc, ld_GetDataPtr( descr ) );
            break;
    }
 }

 /*========================================================================*/
void cg_CodeGen_CALL( TCGContext *cgc, TTree *p )
 {
    TName *descr;
    int    retSize = 0, paramSize = 0;
    TTree *param = cdr(p);

    switch( tr_GetVarType( car(p) ) )
    {
    case T_INT:    retSize = 1; break;
    case T_FLOAT:  retSize = 1; break;
    case T_VECTOR: retSize = 3; break;
    }
    cg_Reserved( cgc, _TINT(retSize) );

    while( param != NIL )
    {
         if( h_TypeOf( param ) == CPU_CALLPARVAR )
         {
              cg_CodeGen( cgc, param );
              paramSize += 1;
         }
         else
         {
              cg_CodeGen( cgc, car(param) );
              switch( h_TypeOf( param ) )
              {
              case CPU_CALLPARAMI: paramSize += 1; break;
              case CPU_CALLPARAMF: paramSize += 1; break;
              case CPU_CALLPARAMS: paramSize += 1; break;
              case CPU_CALLPARAMV: paramSize += 3; break;
              default: lng_ASSERTNQ("cg_CodeGen:CPU_CALL");
              }
         }

         param = cdr(param);
    }

    descr = h_GetDescr( car(p) );

    switch( h_TypeOf(p) )
    {
    case CPU_CALLEXTERN:
            cg_PutCOP   ( cgc, AM_CALLEXTERN );
            cg_OutLink  ( cgc, cg_GetCurCodePtr( cgc ), descr );
            cg_PutPtr   ( cgc, 0 );
            cg_PutSysInt( cgc, paramSize );
            break;

    case CPU_CALL:
            cg_PutCOP   ( cgc, AM_CALL );
            cg_OutLink  ( cgc, cg_GetCurCodePtr( cgc ), descr );
            cg_PutSysInt( cgc, 0 );
            cg_PutSysInt( cgc, ld_GetDataPtr( descr ) );
            break;
    }
 }

 /*========================================================================*/
void cg_CodeGen( TCGContext *cgc, TTree *p )
 {
    TInt     pos, pos1;
    TName   *descr;

    if( p == NIL )
         return;

    switch( h_TypeOf( p ) )
    {
    case SY_VARIABLE: /* Skip const define */ return;

    case CPU_PUSH_VI:
    case CPU_PUSH_VS:
            cg_CodeGenPushVar( cgc, p,
                               AM_PUSH_VI_P,
                               AM_PUSH_VI,
                               AM_PUSH_CEXTERNI );
            break;

    case CPU_PUSH_VF:
            cg_CodeGenPushVar( cgc, p,
                               AM_PUSH_VF_P,
                               AM_PUSH_VF,
                               AM_PUSH_CEXTERNF );
            break;

    case CPU_PUSH_VV:
            cg_CodeGenPush3( cgc, p, AM_PUSH_V3_P, AM_PUSH_V3 ); break;

    case CPU_PUSH_VX:
            cg_CodeGenPush3( cgc, p, AM_PUSH_VX_P, AM_PUSH_VX ); break;

    case CPU_PUSH_VY:
            cg_CodeGenPush3( cgc, p, AM_PUSH_VY_P, AM_PUSH_VY ); break;

    case CPU_PUSH_VZ:
            cg_CodeGenPush3( cgc, p, AM_PUSH_VZ_P, AM_PUSH_VZ ); break;

            /*...................................................*/
    case CPU_FIELDX:
            cg_CodeGen( cgc, car(p) );
            cg_PutCOP ( cgc, AM_GETFIELDX );
            break;

    case CPU_FIELDY:
            cg_CodeGen( cgc, car(p) );
            cg_PutCOP ( cgc, AM_GETFIELDY );
            break;

    case CPU_FIELDZ:
            cg_CodeGen( cgc, car(p) );
            cg_PutCOP ( cgc, AM_GETFIELDZ );
            break;

    case CPU_PUSH_CI:
            cg_PutCOP ( cgc, AM_PUSHCI );
            cg_PutInt ( cgc, h_GetConstINT( p ) );
            break;

    case CPU_PUSH_CF:
            cg_PutCOP  ( cgc, AM_PUSHCF );
            cg_PutFloat( cgc, h_GetConstFLOAT( p ) );
            break;

    case CPU_PUSH_CS:
            cg_CodeGen_PUSH_CS( cgc, p );
            break;

    case CPU_OPERATOR:
            {
                 TTree *list = p;

                 while( list != NIL )
                 {
                     cg_CodeGen( cgc, car(list) );
                     list = cdr(list);
                 }
            }
            break;

    case CPU_LOOP:
            pos = cg_GetCurCodePtr( cgc );
            cgc->m_loopList->m_repPtr = pos;
            cg_CodeGen( cgc, car(p) );
            cg_PutCOP ( cgc, AM_JMP );
            pos1 = cg_GetCurCodePtr( cgc ) + sizeof(TInt);
            cg_PutInt ( cgc, pos - pos1 );
            break;

    case CPU_FOR: case CPU_FORD:
            cg_CodeGenFOR( cgc, p );
            break;

    case CPU_CYCLE:
            cg_CodeGenCYCLE( cgc, p );
            break;

    case CPU_CONTINUE: cg_CodeGenContBreak( cgc, p, 0 ); break;
    case CPU_BREAK:    cg_CodeGenContBreak( cgc, p, 1 ); break;


    case CPU_ADDF:   cg_CodeGenBinOp( cgc, p, AM_ADDF ); break;
    case CPU_ADDI:
                     if( h_IsConstI( car(p) ) )
                     {
                          cg_CodeGen( cgc, cdr(p) );
                          cg_PutCOP ( cgc, AM_ADDCONSTI );
                          cg_PutInt ( cgc, h_GetConstINT( car(p) ) );
                     }
                     else
                     if( h_IsConstI( cdr(p) ) )
                     {
                          cg_CodeGen( cgc, car(p) );
                          cg_PutCOP ( cgc, AM_ADDCONSTI );
                          cg_PutInt ( cgc, h_GetConstINT( cdr(p) ) );
                     }
                     else cg_CodeGenBinOp( cgc, p, AM_ADDI );
                     break;
    case CPU_SUBI:
                     if( h_IsConstI( cdr(p) ) )
                     {
                          cg_CodeGen( cgc, car(p) );
                          cg_PutCOP ( cgc, AM_ADDCONSTI );
                          cg_PutInt ( cgc, -h_GetConstINT( cdr(p) ) );
                     }
                     else cg_CodeGenBinOp( cgc, p, AM_SUBI );
                     break;

    case CPU_ATAN2:  cg_CodeGenBinOp( cgc, p, AM_ATAN2 ); break;

    case CPU_ADDV:   cg_CodeGenBinOp( cgc, p, AM_ADD3 ); break;
    case CPU_SUBF:   cg_CodeGenBinOp( cgc, p, AM_SUBF ); break;
    case CPU_SUBV:   cg_CodeGenBinOp( cgc, p, AM_SUB3 ); break;
    case CPU_DIVF:   cg_CodeGenBinOp( cgc, p, AM_DIVF ); break;
    case CPU_DIVI:   cg_CodeGenBinOp( cgc, p, AM_DIVI ); break;
    case CPU_DIVVF:  cg_CodeGenBinOp( cgc, p, AM_DIV3F); break;
    case CPU_MOD:    cg_CodeGenBinOp( cgc, p, AM_MOD  ); break;
    case CPU_MULF:   cg_CodeGenBinOp( cgc, p, AM_MULF ); break;
    case CPU_MULI:   cg_CodeGenBinOp( cgc, p, AM_MULI ); break;
    case CPU_MULFV:  cg_CodeGenBinOp( cgc, p, AM_MULF3); break;
    case CPU_POWFI:  cg_CodeGenBinOp( cgc, p, AM_POWFI); break;
    case CPU_POWI:   cg_CodeGenBinOp( cgc, p, AM_POWI ); break;
    case CPU_POWF:   if(    h_IsConstF( cdr(p) )
                         && h_GetConstFLOAT( cdr(p) ) == 0.5 )
                     {
                         cg_CodeGen( cgc, car(p) );
                         cg_PutCOP ( cgc, AM_SQRT );
                     }
                     else cg_CodeGenBinOp( cgc, p, AM_POWF );
                     break;
    case CPU_MULVVV: cg_CodeGenBinOp( cgc, p, AM_MUL3 ); break;

    case CPU_SIN:    cg_CodeGenUnaOp( cgc, p, AM_SIN );  break;
    case CPU_COS:    cg_CodeGenUnaOp( cgc, p, AM_COS );  break;
    case CPU_TAN:    cg_CodeGenUnaOp( cgc, p, AM_TAN );  break;
    case CPU_ASIN:   cg_CodeGenUnaOp( cgc, p, AM_ASIN ); break;
    case CPU_ACOS:   cg_CodeGenUnaOp( cgc, p, AM_ACOS ); break;
    case CPU_ATAN:   cg_CodeGenUnaOp( cgc, p, AM_ATAN ); break;
    case CPU_LOG:    cg_CodeGenUnaOp( cgc, p, AM_LOG );  break;
    case CPU_EXP:    cg_CodeGenUnaOp( cgc, p, AM_EXP );  break;
    case CPU_ABSI:   cg_CodeGenUnaOp( cgc, p, AM_ABSI ); break;
    case CPU_ABSF:   cg_CodeGenUnaOp( cgc, p, AM_ABSF ); break;
    case CPU_ABSV:   cg_CodeGenUnaOp( cgc, p, AM_ABS3 ); break;
    case CPU_NEGI:   cg_CodeGenUnaOp( cgc, p, AM_NEGI ); break;
    case CPU_NEGF:   cg_CodeGenUnaOp( cgc, p, AM_NEGF ); break;
    case CPU_NEGV:   cg_CodeGenUnaOp( cgc, p, AM_NEG3 ); break;
    case CPU_CIF:    cg_CodeGenUnaOp( cgc, p, AM_CIF );  break;
    case CPU_CFI:    cg_CodeGenUnaOp( cgc, p, AM_CFI );  break;
    case CPU_NOT:    cg_CodeGenUnaOp( cgc, p, AM_NOT );  break;

    case SY_LVECTORITEM:
            cg_CodeGen( cgc, car(p) );
            cg_CodeGen( cgc, car(cdr(p)) );
            cg_CodeGen( cgc, cdr(cdr(p)) );
            break;

    case CPU_PUSH_AI:
    case CPU_PUSH_AS:
    case CPU_PUSH_AF:
    case CPU_PUSH_AV:
            cg_CodeGen_PUSH_ARRAY( cgc, p );
            break;

    /***************** POP Area *********************/
    case CPU_POP_VS:
    case CPU_POP_VI: cg_GenPOP_V( cgc, p, AM_POP_VI, AM_POP_VI_P ); break;
    case CPU_POP_VF: cg_GenPOP_V( cgc, p, AM_POP_VF, AM_POP_VF_P ); break;
    case CPU_POP_VV: cg_GenPOP_V( cgc, p, AM_POP_V3, AM_POP_V3_P ); break;

    case CPU_POP_AS:
    case CPU_POP_AI: cg_GenPOP_A( cgc, p, AM_POP_AI, AM_POP_AI_P ); break;
    case CPU_POP_AF: cg_GenPOP_A( cgc, p, AM_POP_AF, AM_POP_AF_P ); break;
    case CPU_POP_AV: cg_GenPOP_A( cgc, p, AM_POP_A3, AM_POP_A3_P ); break;

    case CPU_POP_VX: cg_GenPOP_V( cgc, p, AM_POP_VX, AM_POP_VX_P ); break;
    case CPU_POP_VY: cg_GenPOP_V( cgc, p, AM_POP_VY, AM_POP_VY_P ); break;
    case CPU_POP_VZ: cg_GenPOP_V( cgc, p, AM_POP_VZ, AM_POP_VZ_P ); break;

    case CPU_POP_AX: cg_GenPOP_A( cgc, p, AM_POP_AX, AM_POP_AX_P ); break;
    case CPU_POP_AY: cg_GenPOP_A( cgc, p, AM_POP_AY, AM_POP_AY_P ); break;
    case CPU_POP_AZ: cg_GenPOP_A( cgc, p, AM_POP_AZ, AM_POP_AZ_P ); break;
            /* end of POP Area */

    case CPU_BOUND: cg_CodeGenBinOp( cgc, p, AM_BOUND ); break;

    case CPU_LSSI0: cg_CodeGenBinOp( cgc, p, AM_LSSI ); break;
    case CPU_LEQI0: cg_CodeGenBinOp( cgc, p, AM_LEQI ); break;
    case CPU_GRTI0: cg_CodeGenBinOp( cgc, p, AM_GRTI ); break;
    case CPU_GEQI0: cg_CodeGenBinOp( cgc, p, AM_GEQI ); break;
    case CPU_EQUI0: cg_CodeGenBinOp( cgc, p, AM_EQUI ); break;
    case CPU_NEQI0: cg_CodeGenBinOp( cgc, p, AM_NEQI ); break;

    case CPU_LSSF0: cg_CodeGenBinOp( cgc, p, AM_LSSF ); break;
    case CPU_LEQF0: cg_CodeGenBinOp( cgc, p, AM_LEQF ); break;
    case CPU_GRTF0: cg_CodeGenBinOp( cgc, p, AM_GRTF ); break;
    case CPU_GEQF0: cg_CodeGenBinOp( cgc, p, AM_GEQF ); break;
    case CPU_EQUF0: cg_CodeGenBinOp( cgc, p, AM_EQUF ); break;
    case CPU_NEQF0: cg_CodeGenBinOp( cgc, p, AM_NEQF ); break;

    case CPU_AND:   cg_CodeGenBinOp( cgc, p, AM_AND ); break;
    case CPU_OR:    cg_CodeGenBinOp( cgc, p, AM_OR  ); break;

    case CPU_MULVV: cg_CodeGenBinOp( cgc, p, AM_MUL33 ); break;


    case CPU_MOVI:
            cg_CodeGen_MOVI( cgc, p );
            break;

    case CPU_MOVF:
    case CPU_MOVV:
    case CPU_MOVS:
            cg_CodeGen( cgc, car(p) );
            cg_CodeGen( cgc, cdr(p) );
            break;

    case CPU_IF:
    case CPU_IFELSE:
            cg_CodeGen_IF( cgc, p );
            break;

    case CPU_FUNCTION:
    case SY_EXTERN:
            {
                 int   ret, par, loc;
                 TInt  codeSizePtr;

                 cg_OutDebugInfo( cgc, p, &ret, &par, &loc, &codeSizePtr );
                 cgc->m_localSize = loc;
                 cgc->m_paramSize = par;

#                if COPSIZE != 1
                 while( (cg_GetCurCodePtr( cgc )&(COPSIZE-1)) != 0 )
                       cg_PutByte( cgc, 0L );
#                endif

                 cg_Reserved( cgc, _TINT(loc) );
                 cg_CodeGen( cgc, car(car(p)) );
                 if( h_TypeOf( p ) != SY_EXTERN )
                      cg_GenRETURN( cgc );
                 cg_SetInt( cgc, codeSizePtr, cg_GetCurCodePtr( cgc ) );
            }
            break;

    case CPU_RETURN:
            cg_GenRETURN( cgc );
            break;

    case CPU_RETURNINT:
            if( h_IsConstI( car(p) ) )
            {
                 cg_PutCOP   ( cgc, AM_MOV_VCONSTI );
                 cg_PutSysInt( cgc, cgc->m_retStackPos );
                 cg_PutInt   ( cgc, h_GetConstINT( car(p) ) );
            }
            else
            {
                 cg_CodeGen  ( cgc, car(p) );
                 cg_PutCOP   ( cgc, AM_POP_VI );
                 cg_PutSysInt( cgc, cgc->m_retStackPos );
            }

            cg_GenRETURN( cgc );
            break;

    case CPU_RETURNFLOAT:
            cg_CodeGen  ( cgc, car(p) );
            cg_PutCOP   ( cgc, AM_POP_VF );
            cg_PutSysInt( cgc, cgc->m_retStackPos );
            cg_GenRETURN( cgc );
            break;

    case CPU_RETURNVECTOR:
            cg_CodeGen  ( cgc, car(p) );
            cg_PutCOP   ( cgc, AM_POP_V3 );
            cg_PutSysInt( cgc, cgc->m_retStackPos );
            cg_GenRETURN( cgc );
            break;

    case CPU_CALL: case CPU_CALLEXTERN:
            cg_CodeGen_CALL( cgc, p );
            break;

    case CPU_PRINT:
            p = car(p);

            while( p != NIL )
            {
                 cg_CodeGen( cgc, car(p) );
                 switch( h_TypeOf(p) )
                 {
                 case CPU_CALLPARAMI: cg_PutCOP ( cgc, AM_PRINTI ); break;
                 case CPU_CALLPARAMF: cg_PutCOP ( cgc, AM_PRINTF ); break;
                 case CPU_CALLPARAMV: cg_PutCOP ( cgc, AM_PRINT3 ); break;
                 case CPU_CALLPARAMS: cg_PutCOP ( cgc, AM_PRINTS ); break;
                 case CPU_CALLPARAMEOL:cg_PutCOP (cgc, AM_PRINTEOL ); break;
                 default:
                         lng_ASSERTNQ( "Unknown type in PRINT" );
                 }
                 p = cdr(p);
            }
            break;

    case CPU_CALLPARVAR:
            cg_CodeGenCallParVar( cgc, p );
            break;

    case CPU_PUSH_AX:
    case CPU_PUSH_AY:
    case CPU_PUSH_AZ:
            descr = h_GetDescr( p );
            lng_ASSERT( car(p) != NIL, "cg_CodeGen.PUSH_AX");
            cg_CodeGen( cgc, car(p) );

            switch( ld_GetNameDef( descr ) )
            {
            case DEF_VAR:
            case DEF_PAR:
               switch( h_TypeOf( p ) )
               {
               case CPU_PUSH_AX: cg_PutCOP ( cgc, AM_PUSH_AX ); break;
               case CPU_PUSH_AY: cg_PutCOP ( cgc, AM_PUSH_AY ); break;
               case CPU_PUSH_AZ: cg_PutCOP ( cgc, AM_PUSH_AZ ); break;
               }
               break;
            case DEF_PARVAR:
               switch( h_TypeOf( p ) )
               {
               case CPU_PUSH_AX: cg_PutCOP ( cgc, AM_PUSH_AX_P ); break;
               case CPU_PUSH_AY: cg_PutCOP ( cgc, AM_PUSH_AY_P ); break;
               case CPU_PUSH_AZ: cg_PutCOP ( cgc, AM_PUSH_AZ_P ); break;
               }
               break;

            default:
                   lng_ASSERTNQ(
                      "cg_CodeGen.CPU_PUSH_AX: Unknown variable def");
            }
            cg_PutSysInt ( cgc, ld_GetDataPtr( descr ) );
            break;

    case CPU_RNDI:
            cg_CodeGen( cgc, car(p) );
            cg_PutCOP ( cgc, AM_RNDI );
            break;

    case CPU_RNDF:
            cg_PutCOP ( cgc, AM_RNDF );
            break;

    case CPU_ADD_VCONSTI:
            cg_PutCOP   ( cgc, AM_ADD_VCONSTI );
            cg_PutSysInt( cgc,
                        ld_GetDataPtr( h_GetDescr( cdr(p) ) ) );
            cg_PutInt ( cgc, h_GetConstINT( car(p) ) );
            break;

    case CPU_NEQV: cg_CodeGenBinOp( cgc, p, AM_NEQ3 ); break;
    case CPU_EQUV: cg_CodeGenBinOp( cgc, p, AM_EQU3 ); break;

    case CPU_ADD_VARI: cg_CodeGenVarToStack(cgc,p, AM_ADD_VARI ); break;
    case CPU_ADD_VARF: cg_CodeGenVarToStack(cgc,p, AM_ADD_VARF ); break;
    case CPU_SUB_VARI: cg_CodeGenVarToStack(cgc,p, AM_SUB_VARI ); break;
    case CPU_SUB_VARF: cg_CodeGenVarToStack(cgc,p, AM_SUB_VARF ); break;
    case CPU_MUL_VARI: cg_CodeGenVarToStack(cgc,p, AM_MUL_VARI ); break;
    case CPU_MUL_VARF: cg_CodeGenVarToStack(cgc,p, AM_MUL_VARF ); break;
    case CPU_DIV_VARI: cg_CodeGenVarToStack(cgc,p, AM_DIV_VARI ); break;
    case CPU_DIV_VARF: cg_CodeGenVarToStack(cgc,p, AM_DIV_VARF ); break;
    case CPU_MOD_VAR:  cg_CodeGenVarToStack(cgc,p, AM_MOD_VAR );  break;


    case CPU_DROPSTACK:
            cg_CodeGen( cgc, car(p) );
            cg_PutCOP ( cgc, AM_RESERVED );
            cg_PutInt ( cgc, -h_GetConstINT( cdr(p) ) );
            break;
    case SY_EMPTY:  break;


    default:
            lng_ASSERTNQ( "cg_CodeGen: Unknown command" );
    }
 }

 /*========================================================================*/
void cg_CodeGenProg( TCGContext *cgc, TTree *p )
 {
    TName *descr;

    us_CalcUseStack( p );
    cg_CalcOffsetConstExtern( p );

    /*
     * Первым параметром идет число резервируемых в стеке констант
     */
    while( p != NIL )
    {
         switch( h_TypeOf( p ) )
         {
         case CPU_FUNCTION:
                 cg_CodeGen( cgc, p );
                 break;

         case CPU_PUSH_VI:
         case CPU_PUSH_VF:
         case CPU_PUSH_VS:
                            descr = h_GetDescr( p );
                            cg_OutLink( cgc,
                                        _TINT(ld_GetDataPtr( descr )),
                                        descr );
                            break;
         }

         p = cdr(p);
    }
 }

 /*========================================================================*/
void cg_PrintLinkInfo( TCGContext *cgc )
 {
	os_TOutStream  *linkInfo = cgc->m_linkInfo;
    TInt         ptr      = cgc->m_linkList;

    if( ptr == -1 )
         return;

    while( ptr < linkInfo->m_pos )
    {
         TInt namePtr = os_GetIntStep( linkInfo, &ptr );
         TInt func_def= os_GetIntStep( linkInfo, &ptr );
         TInt next    = os_GetIntStep( linkInfo, &ptr );
         TInt adrList = os_GetIntStep( linkInfo, &ptr );
         printf( "%s ",  os_GetStr( linkInfo, namePtr ) );
         if( func_def == FT_EXTERN )
              printf("EXTERN ");

         while( adrList >= 0 )
         {
             ptr = adrList;
             printf( "0%lXh ", os_GetIntStep( linkInfo, &ptr ) );
             adrList = os_GetIntStep( linkInfo, &ptr );
         }

         ptr = next;
         if( ptr == -1 )
              break;
         printf("\n");
    }
    printf("\n");
 }

 /*========================================================================*/
void cg_DisAsm( os_TOutStream *cs )
 {
    TInt   pos = 0;
    const TByte *s = (TByte*)os_GetStr( cs, _TINT(0) );
    AM_TYPE command;
    TDebugInfo dinfo;


#define OUT_INT printf("%i", *((TInt*)(&(s[(int)pos]))) ); \
                pos += sizeof(TInt)

#define OUT_INTX printf("%08lx", *((TInt*)(&(s[(int)pos]))) ); \
                pos += sizeof(TInt)


#define OUT_RADR printf("%08lx", pos+sizeof(TInt)+*((TInt*)(&(s[(int)pos]))) ); \
                pos += sizeof(TInt)

#define OUT_FLOAT printf("%f", (float) *((TFloat*)(&(s[(int)pos]))) ); \
                pos += sizeof(TFloat)


    while( pos < cs->m_pos )
    {
         dinfo.m_magic= os_GetIntStep( cs, &pos );
         dinfo.m_nextFunc = os_GetIntStep( cs, &pos );
         dinfo.m_defs = os_GetIntStep( cs, &pos );
         dinfo.m_code = os_GetIntStep( cs, &pos );
         dinfo.m_funcName = os_GetIntStep( cs, &pos );
         dinfo.m_funcType = (DATA_TYPE)(os_GetIntStep( cs, &pos ));


         if( pos <= 0 )
         {
             lng_ASSERTNQ("Error! cg_DisAsm");
         }

         printf("; === FUNC ");
         switch( dinfo.m_funcType )
         {
         case T_NONE:    printf("VOID "); break;
         case T_INT:     printf("INT  "); break;
         case T_FLOAT:   printf("FLOAT "); break;
         case T_VECTOR:  printf("VECTOR "); break;
         }
         {
         const char *st = (const char *)(&s[ (int)dinfo.m_funcName ]);

         for(; *st != 0; ++st )
              printf("%c", *st );
         }
         printf("\n");

         if( dinfo.m_defs != 0 )
         {
              TStackCellDef loc;

              pos = dinfo.m_defs;

              for(;;)
              {
                   printf("; ");

                   loc.m_name     = (int)os_GetIntStep( cs, &pos );
                   loc.m_type     = (DATA_TYPE)   (os_GetIntStep( cs, &pos ));
                   loc.m_ndef     = (NAMEDEF_TYPE)(os_GetIntStep( cs, &pos ));
                   loc.m_stackPos = (int)os_GetIntStep( cs, &pos );
                   loc.m_next     = (int)os_GetIntStep( cs, &pos );

                   switch( loc.m_type )
                   {
                   case T_INT:    printf("INT    "); break;
                   case T_FLOAT:  printf("FLOAT  "); break;
                   case T_VECTOR: printf("VECTOR "); break;
                   }

                   switch( loc.m_ndef )
                   {
                   case DEF_PAR:    printf("par  "); break;
                   case DEF_PARVAR: printf("par &"); break;
                   case DEF_VAR:    printf("var  "); break;
                   }

                   printf("[%5i] ", loc.m_stackPos);

                   while( s[ loc.m_name ] != 0 )
                   {
                        printf( "%c", s[ loc.m_name ] );
                        ++loc.m_name;
                   }
                   printf("\n");
                   if( loc.m_next == 0 )
                        break;
                   pos = loc.m_next;
              }
         }

         pos = (int)dinfo.m_code;


         while( pos < cs->m_pos && pos < dinfo.m_nextFunc )
         {

              printf("%08x   ", pos);

              switch( command = (AM_TYPE)(s[ (int)((pos+=COPSIZE)-COPSIZE) ]) )
              {
              case AM_NOP:       printf("NOP");        break;

              case AM_RESERVED:  printf("RESERVED " );
                                 OUT_INT;
                                 break;

              case AM_RETURN:    printf("RETURN "); break;

              case AM_RETURN_CLR:printf("RETURN_CLR ");
                                 OUT_INT; printf(", ");
                                 OUT_INT;
                                 break;

              case AM_JMP:       printf("JMP ");
                                 OUT_RADR;
                                 break;

              case AM_JMPIFFALSE:printf("JMPIFFALSE ");
                                 OUT_RADR;
                                 break;

              case AM_JMPIFTRUE: printf("JMPIFTRUE ");
                                 OUT_RADR;
                                 break;

              case AM_PUSHCI:    printf("PUSHCI ");
                                 OUT_INT;
                                 break;

              case AM_PUSHCF:    printf("PUSHCF ");
                                 OUT_FLOAT;
                                 break;

              case AM_PUSHCS:    printf("PUSHCS ");
                                 {
                                 TInt strPos, lastPos;
                                 strPos = *((TInt*)&(s[(int)pos]));
                                 pos += sizeof(TInt);
                                 lastPos = *((TInt*)&(s[(int)pos]));
                                 pos += sizeof(TInt);

                                 printf("\"%s\"",&(s[(int)strPos]));
                                 pos = lastPos;
                                 }
                                 break;

              case AM_POP_VI:    printf("POP_VI  [");
                                 OUT_INT; printf("]");
                                 break;

              case AM_POP_VF:    printf("POP_VF  [");
                                 OUT_INT;printf("]");
                                 break;

              case AM_POP_V3:    printf("POP_V3  [");
                                 OUT_INT;printf("]");
                                 break;

              case AM_POP_VI_P:  printf("POP_VI *[");
                                 OUT_INT;printf("]");
                                 break;

              case AM_POP_VF_P:  printf("POP_VF *[");
                                 OUT_INT;printf("]");
                                 break;

              case AM_POP_V3_P:  printf("POP_V3 *[");
                                 OUT_INT;printf("]");
                                 break;

              case AM_PUSH_VI:   printf("PUSH_VI [");
                                 OUT_INT;printf("]");
                                 break;

              case AM_PUSH_VF:   printf("PUSH_VF [");
                                 OUT_INT;printf("]");
                                 break;

              case AM_PUSH_V3:   printf("PUSH_V3 [");
                                 OUT_INT;printf("]");
                                 break;

              case AM_PUSH_VI_P: printf("PUSH_VI *[");
                                 OUT_INT;printf("]");
                                 break;

              case AM_PUSH_VF_P: printf("PUSH_VF *[");
                                 OUT_INT;printf("]");
                                 break;

              case AM_PUSH_V3_P: printf("PUSH_V3 *[");
                                 OUT_INT;printf("]");
                                 break;

              case AM_ADDF:      printf("ADDF"); break;
              case AM_ADDI:      printf("ADDI"); break;
              case AM_ADD3:      printf("ADD3"); break;

              case AM_SUBF:      printf("SUBF"); break;
              case AM_SUBI:      printf("SUBI"); break;
              case AM_SUB3:      printf("SUB3"); break;

              case AM_DIVF:      printf("DIVF"); break;
              case AM_DIVI:      printf("DIVI"); break;
              case AM_MOD:       printf("MOD");  break;
              case AM_MULF:      printf("MULF"); break;
              case AM_MULI:      printf("MULI"); break;
              case AM_POWFI:     printf("POWFI");break;
              case AM_POWI:      printf("POWI"); break;
              case AM_POWF:      printf("POWF"); break;
              case AM_MUL3:      printf("MUL3"); break;

              case AM_MULF3:     printf("MULF3"); break;
              case AM_DIV3F:     printf("DIV3F"); break;

              case AM_LSSI:      printf("LSSI"); break;
              case AM_LEQI:      printf("LEQI"); break;
              case AM_GRTI:      printf("GRTI"); break;
              case AM_GEQI:      printf("GEQI"); break;
              case AM_EQUI:      printf("EQUI"); break;
              case AM_NEQI:      printf("NEQI"); break;

              case AM_LSSF:      printf("LSSF"); break;
              case AM_LEQF:      printf("LEQF"); break;
              case AM_GRTF:      printf("GRTF"); break;
              case AM_GEQF:      printf("GEQF"); break;
              case AM_EQUF:      printf("EQUF"); break;
              case AM_NEQF:      printf("NEQF"); break;

              case AM_AND:       printf("AND"); break;
              case AM_OR:        printf("OR");  break;

              case AM_SIN:       printf("SIN");  break;
              case AM_COS:       printf("COS");  break;
              case AM_TAN:       printf("TAN");  break;
              case AM_ASIN:      printf("ASIN"); break;
              case AM_ACOS:      printf("ACOS"); break;
              case AM_ATAN:      printf("ATAN"); break;
              case AM_LOG:       printf("LOG");  break;
              case AM_EXP:       printf("EXP");  break;
              case AM_ABSI:      printf("ABSI"); break;
              case AM_ABSF:      printf("ABSF"); break;
              case AM_ABS3:      printf("ABS3"); break;
              case AM_NEGI:      printf("NEGI"); break;
              case AM_NEGF:      printf("NEGF"); break;
              case AM_NEG3:      printf("NEG3"); break;
              case AM_CIF:       printf("CIF");  break;
              case AM_CFI:       printf("CFI");  break;
              case AM_NOT:       printf("NOT");  break;

              case AM_MUL33:     printf("MUL33"); break;

              case AM_PUSH_VX:   printf("PUSH_V [");
                                 OUT_INT;printf("].x");
                                 break;

              case AM_PUSH_VY:   printf("PUSH_V [");
                                 OUT_INT;printf("].y");
                                 break;

              case AM_PUSH_VZ:   printf("PUSH_V [");
                                 OUT_INT;printf("].z");
                                 break;

              case AM_PUSH_VX_P: printf("PUSH_V *[");
                                 OUT_INT;printf("].x");
                                 break;

              case AM_PUSH_VY_P: printf("PUSH_V *[");
                                 OUT_INT;printf("].y");
                                 break;

              case AM_PUSH_VZ_P: printf("PUSH_V *[");
                                 OUT_INT;printf("].z");
                                 break;

              case AM_POP_VX:    printf("POP_V [");
                                 OUT_INT;printf("].x");
                                 break;

              case AM_POP_VY:    printf("POP_V [");
                                 OUT_INT;printf("].y");
                                 break;

              case AM_POP_VZ:    printf("POP_V [");
                                 OUT_INT;printf("].z");
                                 break;

              case AM_POP_VX_P:  printf("POP_V *[");
                                 OUT_INT;printf("].x");
                                 break;

              case AM_POP_VY_P:  printf("POP_V *[");
                                 OUT_INT;printf("].y");
                                 break;

              case AM_POP_VZ_P:  printf("POP_V *[");
                                 OUT_INT;printf("].z");
                                 break;

              case AM_POP_AI:    printf("POP_AI [");
                                 OUT_INT;printf("]");
                                 break;

              case AM_POP_AF:    printf("POP_AF [");
                                 OUT_INT;printf("]");
                                 break;

              case AM_POP_A3:    printf("POP_A3 [");
                                 OUT_INT;printf("]");
                                 break;

              case AM_POP_AI_P:  printf("POP_AI *[");
                                 OUT_INT;printf("]");
                                 break;

              case AM_POP_AF_P:  printf("POP_AF *[");
                                 OUT_INT;printf("]");
                                 break;

              case AM_POP_A3_P:  printf("POP_A3 *[");
                                 OUT_INT;printf("]");
                                 break;

              case AM_BOUND:     printf("BOUND");
                                 break;

              /*----------------------------------------------------*/

              case AM_PUSH_AI:   printf("PUSH_AI [");
                                 OUT_INT;printf("]");
                                 break;

              case AM_PUSH_AF:   printf("PUSH_AF [");
                                 OUT_INT;printf("]");
                                 break;

              case AM_PUSH_A3:   printf("PUSH_A3 [");
                                 OUT_INT;printf("]");
                                 break;


              case AM_PUSH_AI_P: printf("PUSH_AI *[");
                                 OUT_INT;printf("]");
                                 break;

              case AM_PUSH_AF_P: printf("PUSH_AF *[");
                                 OUT_INT;printf("]");
                                 break;

              case AM_PUSH_A3_P: printf("PUSH_A3 *[");
                                 OUT_INT;printf("]");
                                 break;

              /*------------------------------------------------*/
              case AM_POP_AX:    printf("POP_A [");
                                 OUT_INT;printf("].x");
                                 break;

              case AM_POP_AY:    printf("POP_A [");
                                 OUT_INT;printf("].y");
                                 break;

              case AM_POP_AZ:    printf("POP_A [");
                                 OUT_INT;printf("].z");
                                 break;

              case AM_POP_AX_P:  printf("POP_A *[");
                                 OUT_INT;printf("].x");
                                 break;

              case AM_POP_AY_P:  printf("POP_A *[");
                                 OUT_INT;printf("].y");
                                 break;

              case AM_POP_AZ_P:  printf("POP_A *[");
                                 OUT_INT;printf("].z");
                                 break;
              /*------------------------------------------------*/

              case AM_GETFIELDX: printf("GETX");
                                 break;
              case AM_GETFIELDY: printf("GETY");
                                 break;
              case AM_GETFIELDZ: printf("GETZ");
                                 break;

              case AM_LOOP:      printf("LOOP [");
                                 OUT_INT;printf("] TO ["); OUT_INT; printf("] GO ");
                                 OUT_RADR;
                                 break;

              case AM_LOOPD:     printf("LOOPD [");
                                 OUT_INT;printf("] TO ["); OUT_INT; printf("] GO ");
                                 OUT_RADR;
                                 break;

              case AM_LEAVE:     printf("LEAVE [");
                                 OUT_INT;printf("] TO ["); OUT_INT; printf("] GO ");
                                 OUT_RADR;
                                 break;

              case AM_LEAVED:    printf("LEAVED [");
                                 OUT_INT;printf("] TO ["); OUT_INT; printf("] GO ");
                                 OUT_RADR;
                                 break;

              case AM_CALL:      printf("CALL ");
                                 OUT_INTX; printf(", STACK ");
                                 OUT_INT;
                                 break;

              case AM_PUSH_ADR:  printf("PUSH ADR [");OUT_INT;printf("]");
                                 break;

              case AM_PUSH_AX:   printf("PUSH_A [");OUT_INT;printf("].x");
                                 break;
              case AM_PUSH_AY:   printf("PUSH_A [");OUT_INT;printf("].y");
                                 break;
              case AM_PUSH_AZ:   printf("PUSH_A [");OUT_INT;printf("].z");
                                 break;
              case AM_PUSH_AX_P: printf("PUSH_A *[");OUT_INT;printf("].x");
                                 break;
              case AM_PUSH_AY_P: printf("PUSH_A *[");OUT_INT;printf("].y");
                                 break;
              case AM_PUSH_AZ_P: printf("PUSH_A *[");OUT_INT;printf("].z");
                                 break;
              case AM_SQRT:      printf("SQRT");  break;
              case AM_PRINTI:    printf("PRINTI"); break;
              case AM_PRINTF:    printf("PRINTF"); break;
              case AM_PRINT3:    printf("PRINT3"); break;
              case AM_PRINTS:    printf("PRINTS"); break;
              case AM_PRINTEOL:  printf("PRINTEOL"); break;

              case AM_RNDI:      printf("RNDI"); break;
              case AM_RNDF:      printf("RNDF"); break;
              case AM_ADDCONSTI: printf("ADDCONSTI "); OUT_INT; break;
              case AM_MOV_VCONSTI:
                                 printf("MOV_VCONST ["); OUT_INT;
                                 printf("], "); OUT_INT; break;

              case AM_ADD_VCONSTI:
                                 printf("ADD_VCONST ["); OUT_INT;
                                 printf("], "); OUT_INT; break;
              case AM_EQU3:      printf("EQU3");break;
              case AM_NEQ3:      printf("NEQ3");break;

              case AM_CALLEXTERN:printf("CALLEXTERN ");
                                 printf("%08lx", *((TInt*)(&(s[(int)pos]))) );
                                 pos += sizeof(TBytePtr);
                                 printf(", ");
                                 OUT_INT;
                                 break;
              case AM_ADD_VARI:  printf("ADD_VARI ["); OUT_INT; printf("]");
                                 break;
              case AM_ADD_VARF:  printf("ADD_VARF ["); OUT_INT; printf("]");
                                 break;
              case AM_SUB_VARI:  printf("SUB_VARI ["); OUT_INT; printf("]");
                                 break;
              case AM_SUB_VARF:  printf("SUB_VARF ["); OUT_INT; printf("]");
                                 break;

              case AM_MUL_VARI:  printf("MUL_VARI ["); OUT_INT; printf("]");
                                 break;
              case AM_MUL_VARF:  printf("MUL_VARF ["); OUT_INT; printf("]");
                                 break;
              case AM_DIV_VARI:  printf("DIV_VARI ["); OUT_INT; printf("]");
                                 break;
              case AM_DIV_VARF:  printf("DIV_VARF ["); OUT_INT; printf("]");
                                 break;
              case AM_MOD_VAR:   printf("MOD_VAR  ["); OUT_INT; printf("]");
                                 break;
              case AM_ATAN2:     printf("ATAN2"); break;
              case AM_PUSH_CEXTERNI:
                                 printf("PUSH_CEXI ["); OUT_INT; printf("]");
                                 break;
              case AM_PUSH_CEXTERNF:
                                 printf("PUSH_CEXF ["); OUT_INT; printf("]");
                                 break;

              default:;
                      lng_ASSERTNQ_P("DisAsm: unknown command",(int)command);

              }
              printf("\n");
         }
         pos = dinfo.m_nextFunc;
    }
 }

/* End of CGEN.C */
