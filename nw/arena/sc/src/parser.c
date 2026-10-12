/*
    File:  PARSER.C
    Autor: Suavik
    Ver    1.2

    Синтаксический разбор программы и построение дерева разбора.
    Префикс p_

    ------------------

    void        p_TxtError()
    void        p_InitTParserContext()
    void        ReadLex()
    LEX_TYPE    LexType()
    const char *LexSTR()
    TError     *p_ErrorOf()
    void        p_Error()

    void        ExpectedSymbol()
    void        ExpectedLParent()
    void        ExpectedRParent()
    void        ExpectedComma()
    void        ExpectedSemicolon()
    int         ReadLexIfEqu()

    TInt        RangeOfArray()
    TTree      *CreateNEG()
    TTree      *ReadField()
    TTree      *MakeArrayAdr()
    TTree      *ReadArrayIndex()
    TTree      *ReadCallParVarExp()
    TTree      *ReadCallParam()
    TTree      *ReadCallExp()
    TTree      *ReadVarExpGlobal()
    TTree      *ReadVarExp()

    TTree      *ReadUnarExp()
    TTree      *ReadPowExp()
    TTree      *ReadProdExp()
    TTree      *ReadSumExp()
    TTree      *ReadCompExp()
    TTree      *ReadAndExp()
    TTree      *ReadOrExp()

    TInt        ReadConstI()
    DATA_TYPE   FuncType()
    TTree      *OperatorRETURN()
    void        ReadArrayDef()
    TName      *ReadFuncParamDef()
    TTree      *p_Const()
    int         ChainOfNameNIL()
    TTree      *p_Function()
    void        ReadVarDef()
    TTree      *Assign()
    TTree      *Assignment()
    TTree      *OperatorIF()
    TTree      *OperatorLOOP()
    TTree      *Operators()
    TTree      *ReadLoopName()
    TTree      *OperatorFOR()
    TTree      *OperatorBREAK_CONTINUE()
    TTree      *OperatorBREAK()
    TTree      *OperatorCONTINUE()
    int         OperatorNOP()
    TTree      *OperatorPRINT()
    TTree      *OperatorCALL()
    TTree      *Operator()
    TTree      *p_GlobalDefs()
    TTree      *p_Programm()

    ------------------
    BUGS :
 *  нет костант;
 *  нет рекурсивных функций;
 *  нет внешних функций EXTERN;
 *  нет RNDI, RNDF;
 *  нет ATAN2;
 *  нет строк ( тип STR )
 *  нет FORWARD определений функций

 *  не выводится позиция и строка текста при выводе ошибки;
 *  не обрабатываются ошибки выдаваемые сканером;
 *  при неправильном объявлении локальных переменных не выдается ошибки и
      не компилируется : FUNC VOID Ok() INT x; {}   .
                                        ^
 *  нет обработки вызовов фунции VOID
 *  нет обработчика внутренних ошибок компилятора.
 *  не правильно обрабатываются параметры по ссылке.
 *  не передаются массивы как параметры по значению. FIX:Не поддерживается.
    слишком много одинаковых мест чтения индексов массива
 *  при чтении выражения в p_Const стоял вывод ошибки если считана именно
    константа.

    в ReadUnarExp некоторые выражения могут быть NIL (в результате ошибки)
      при этом вылетает ASSERT в CreateNEG

    Нет оператора SELECT
    
    Не обрабатываются пустые скобки !!!!!!!!!!
    ------------------
    23.07.97 Выкинул проверку с ошибкой при считывании OperatorNOP
             Потому, что при закрывании } Operator должен вернуть NIL,
             Но неизвестная конструкция возможно не отработается.

    31.07.97 Добавил передачу снаружи TNameDefs *ndefs в функции
             p_Programm, p_Function. Теперь можно объединять новые переменные
             в список за пределами PARSER.C

    04.08.97 Добавлена функция p_TxtError для вывода строки и линии.
             без отладки.

    07.08.97 Сделана обработка костант.
             Обработка рекурсивного вызова функции. Раньше ее имя
             регистрировалось как локальное и искалась переменная.
             Сделан оператор PRINT.

    11.08.97 Добавлен оператор вызова функции.
    12.08.97 Введен lng_ASSERT
    13.08.97 Добавлена и отлажена передача пареметра по ссылке для элемента
             массива.
    14.08.97 Отлажена передача массивов по ссылке.
             Вставил break при считывании точки в ReadPowExp
    23.08.97 Поправил в ReadUnarExp считывание переменной или функции, в
             случае если вернулся NIL в SY_WORD.

    24.08.97 В p_Function добавлено без тестирования FORWARD декларации.
             сомнения вызывает сравнение с прототипом
    27.08.97 В p_Function исправлено сравнение с прототипом.
             Добавлет тип STR.
             Не правильно обрабатывались константы типа FLOAT; исправлено.
    28.08.97 Вставлена обработка на NIL в ReadOrExp
    01.09.97 Начато дабавление EXTERN.
    10.09.97 Добавлена костанта EXTERN.
    10.01.99 Fix "Module: fileName"
 */
#include <math.h>
#include <string.h>

#include "langer.h"
#include "parser.h"
#include "tr_code.h"
#include "strequ.h"

 /*  Структура, содержащая все необходимое для синтаксического разбора */
typedef struct{
    TTreeHeap  *m_heap;       /* Данные о куче             */
    TScanner   *m_scanner;    /* данные сканера            */
    TNameArray *m_nameDescr;  /* Куча   имен               */
    TNameDefs  *m_ndefs;      /* Списки имен               */
    int         m_beenRETURN; /* Флаг использования RETURN */
} TParserContext;

const char *EXPECTED_PAR = "Expected parameter";

TTree *ReadOrExp  ( TParserContext *pcont, const char *nilError );
TTree *ReadCompExp( TParserContext *pcont, const char *nilError );
TTree *Operator   ( TParserContext *pcont, int level );
void   ReadVarDef ( TParserContext *pcont );
TTree *ReadVarExp ( TParserContext *pcont, int mayCall );


 /*========================================================================*/
TError *p_ErrorOf( TParserContext *pcont )
 {
     return h_ErrorOf( pcont->m_heap );
 }

 /*========================================================================*/
 /*
  * Выдача ошибки с позицией в тексте
  */
void p_TxtError( TParserContext *pcont, const char *format, ... )
 {
    va_list  ap;
    int      line = lex_Line( pcont->m_scanner ),
             pos  = lex_Pos ( pcont->m_scanner );

    strcpy( p_ErrorOf( pcont )->m_fileName, pcont->m_scanner->fileName );

    va_start( ap, format );
    lng_TextError( h_ErrorOf( pcont->m_heap ),
                   line, pos,
                   format, ap );
    va_end( ap );
 }

 /*========================================================================*/
void p_TxtError2( TScanner *scanner, TError *err, const char *format, ... )
 {
    va_list  ap;
    int      line = lex_Line( scanner ),
             pos  = lex_Pos ( scanner );

    strcpy( err->m_fileName, scanner->fileName );

    va_start( ap, format );
    lng_TextError( err,
                   line, pos,
                   format, ap );
    va_end( ap );
 }



 /*========================================================================*/
void p_InitTParserContext( TParserContext *pcont,
                           TTreeHeap  *heap,
                           TScanner   *scanner,
                           TNameArray *nameDescr,
                           TNameDefs  *ndefs )
 {
     pcont->m_heap       = heap;
     pcont->m_scanner    = scanner;
     pcont->m_nameDescr  = nameDescr;
     pcont->m_ndefs      = ndefs;
     pcont->m_beenRETURN = 0;
 }
 /*========================================================================*/
void ReadLex( TParserContext *pcont )
 {
     if( !lex_Get( pcont->m_scanner ) )
          p_TxtError( pcont,
                      lex_ErrorToStr( lex_GetErrorCode( pcont->m_scanner )));
 }

 /*========================================================================*/
LEX_TYPE LexType( TParserContext *pcont )
 {
     return lex_GetType( pcont->m_scanner );
 }

 /*========================================================================*/
const char *LexSTR( TParserContext *pcont )
 {
    return lex_GetSTR( pcont->m_scanner );
 }

 /*========================================================================*/
void p_Error(  TParserContext *pcont, const char *msg  )
 {
    strcpy( p_ErrorOf( pcont )->m_fileName, pcont->m_scanner->fileName );
    lng_Error( p_ErrorOf( pcont ), msg );
 }

 /*========================================================================*/
void ExpectedSymbol( TParserContext *pcont,
                     const char     *sym,
                     LEX_TYPE        lex )
 {
    if( LexType( pcont ) != lex )
         p_TxtError( pcont, "Expected %s", sym );

    ReadLex( pcont );
 }

 /*========================================================================*/
void ExpectedLParent( TParserContext *pcont )
 {
    ExpectedSymbol( pcont, "(", SY_LPARENT );
 }

 /*========================================================================*/
void ExpectedRParent( TParserContext *pcont )
 {
    ExpectedSymbol( pcont, ")", SY_RPARENT );
 }

 /*========================================================================*/
void ExpectedComma( TParserContext *pcont )
 {
    ExpectedSymbol( pcont, ",", SY_COMMA );
 }

 /*========================================================================*/
void ExpectedSemicolon( TParserContext *pcont )
 {
    ExpectedSymbol( pcont, ";",SY_SEMICOLON );
 }

 /*========================================================================*/
int ReadLexIfEqu( TParserContext *pcont, LEX_TYPE expLex  )
 {
    if( LexType( pcont ) == expLex )
    {
         ReadLex( pcont );
         return 1;
    }

    return 0;
 }

 /*========================================================================*/
 /*
  * Определяет границы всего массива в целом
  */
TInt RangeOfArray( TName *descr )
 {
    int  i;
    TInt size = 1;

    for( i = 0; i < ld_GetArrayCnt( descr ) ; ++i )
         size *= ld_GetArrayRange( descr, i );

    return size;
 }

 /*========================================================================*/
TTree *CreateNEG( int sign, TTreeHeap *heap, TTree *p )
 {
    lng_ASSERT( p != NIL, "CreateNEG" );

    if( sign )
         return h_NewR0( heap, SY_NEG ,p );

    return p;
 }

 /*========================================================================*/
 /************
  *          *
  *  Parser  *
  *          *
  ************/

 /*
  *
  * Чтение поля вектора.
  * Последняя буква - имя поля.
  *
  *  field ::= SY_FIELDX
  *            |
  *            exp
  *
  *  field ::= SY_FIELDY
  *            |
  *            exp
  *
  *  field ::= SY_FIELDZ
  *            |
  *            exp
  *
  */
TTree *ReadField( TParserContext *pcont, TTree *p )
 {
    if( ReadLexIfEqu( pcont, SY_PERIOD ) )
         if( LexType( pcont ) == SY_WORD )
         {
              const char *name = LexSTR( pcont );
              TTreeHeap  *heap = pcont->m_heap;

              ReadLex( pcont );
              lng_ASSERT( p != NIL, "ReadField" );

              if( str_StrEQU( name, "x" ) )
                   p = h_NewR0( heap, SY_FIELDX, p );
              else
              if( str_StrEQU( name, "y" ) )
                   p = h_NewR0( heap, SY_FIELDY, p );
              else
              if( str_StrEQU( name, "z" ) )
                   p = h_NewR0( heap, SY_FIELDZ, p );
              else p_TxtError( pcont, "Expected field x|y|z" );
         }
         else p_TxtError( pcont, "Expected field x|y|z" );

    return p;
 }

 /*========================================================================*/
 /*
  * Расчитывается адрес в массиве до endInd включительно.
  * если endInd равен максимальной размерности массива, то
  * расчитывается полный адрес ячейки, если endInd меньше,
  * то расчитывается адрес строчки
  */
TTree *MakeArrayAdr( TTreeHeap *heap,
                     TTree     *e[MAX_ARRAY_CNT],
                     TName     *descr,
                     int        endInd  )
 {
    int    i, size;
    TTree *r;

    switch( ld_GetType( descr ) )
    {
    case T_INT:
    case T_FLOAT:
    case T_STR:
                              size = 1; break;
    case T_VECTOR:            size = 3; break;
    default: lng_ASSERTNQ( "MakeArrayAdr: unknown type" );
    }

    for( i = ld_GetArrayCnt( descr ) - 1 ; ( i >= 0 ) ; --i )
    {
         if( size != 1 && i <= endInd )
              e[i] = h_New( heap, SY_SMUL,
                            e[i], h_NewI( heap, _TINT(size) ));

         size *= (int)ld_GetArrayRange( descr, i );
    }

    r = e[0];

    for( i = 1; ( i <= endInd )  ; ++i )
         r = h_New( heap, SY_PLUS, r, e[i] );

    return r;
 }

 /*========================================================================*/
TTree *ReadArrayIndex( TParserContext *pcont, int cnt, TName *descr )
 {
    int        i;
    TTreeHeap *heap = pcont->m_heap;
    TTree     *p = NIL, *e[ MAX_ARRAY_CNT ];

    lng_ASSERT( cnt > 0 && cnt <= MAX_ARRAY_CNT, "ReadArrayIndex" );
    ExpectedSymbol( pcont, "[", SY_LBRACK );

    for( i = 0; ( i < cnt ) ; ++i )
    {
         e[i] = h_New( heap, SY_BOUND,
                       ReadOrExp( pcont,
                                  "Expected expression in array index" ),
                       h_NewI( heap,
                               ld_GetArrayRange( descr, i )));

         if( i != cnt - 1 )
              ExpectedComma( pcont );
    }

    ExpectedSymbol( pcont, "]", SY_RBRACK );
    p = MakeArrayAdr( heap, e, descr, cnt - 1 );
    p = h_NewA( heap, descr, p );

    return p;
 }

 /*========================================================================*/
 /*
  * Считывание параметра функции, передаваемого с VAR
  */
TTree *ReadCallParVarExp( TParserContext *pcont, TName *descr )
 {
    NAMEDEF_TYPE  nameDef;
    TTreeHeap    *heap = pcont->m_heap;
    TName        *parDescr;
    int           arrayCnt = ld_GetArrayCnt( descr );
    int           parArrayCnt, i;

    if( LexType( pcont ) == SY_WORD )
    {
         int    isLocal;

         /*
          * Ищем переменную в списке имен
          */
         parDescr = h_SearchVariable( LexSTR( pcont ),
                                      pcont->m_ndefs, &isLocal, NIL );

         if( parDescr == NIL )
              p_TxtError( pcont,
                         "Variable %s not definition", LexSTR( pcont ));

         nameDef = ld_GetNameDef( parDescr );

         if( ( !isLocal ) || nameDef == DEF_FUNC )
              p_TxtError( pcont, "Expected variable" );

         lng_ASSERT( nameDef == DEF_VAR ||
                     nameDef == DEF_PAR ||
                     nameDef == DEF_PARVAR, "ReadCallParVarExp") ;

         if( ld_GetType( descr ) != ld_GetType( parDescr ) )
              p_TxtError( pcont, "Parameter type does not match" );

         ReadLex( pcont );

         /*
          * В данном месте мы уверены, что переменная определена,
          * это именно переменная, а не константа или функция,
          * ее тип совпадает с типом параметра
          */
         parArrayCnt = ld_GetArrayCnt( parDescr );

         if( arrayCnt == 0 && parArrayCnt == 0 )
              return h_NewV( heap, parDescr );

         if( arrayCnt != 0 && parArrayCnt == arrayCnt )
         {
              for( i = 0; ( i < arrayCnt ) ; ++i )
                   if(    ld_GetArrayRange( descr,    i )
                       != ld_GetArrayRange( parDescr, i ) )
                          p_TxtError( pcont,
                                      "Parameter dimension does not match" );
              return h_NewV( heap, parDescr );
         }


         if( arrayCnt > parArrayCnt )
              p_TxtError( pcont, "Dimension does not match" );

         return ReadArrayIndex( pcont, parArrayCnt - arrayCnt, parDescr );
    }
    else p_TxtError( pcont, "Expected variable name" );

    return NIL;
 }

 /*========================================================================*/
 /*
  *
  * Чтение параметра при вызове функции.
  * Последняя буква - тип параметра.
  *
  *  param ::= SY_CALLPARAMI
  *            |
  *            exp
  *
  *  param ::= SY_CALLPARAMF
  *            |
  *            exp
  *
  *  param ::= SY_CALLPARAMV
  *            |
  *            exp
  *
  *  param ::= SY_CALLPARVAR
  *            |
  *            exp(SY_VARIABLE)
  */
TTree *ReadCallParam( TParserContext *pcont, TName *descr )
 {
    TTree     *p;
    TTree     *param;
    TTreeHeap *heap = pcont->m_heap;

    if( ld_GetNameDef( descr ) == DEF_PARVAR )
    {
         param = ReadCallParVarExp( pcont, descr );

         if( param == NIL )
              p_TxtError( pcont, "Expected variable" );

         lng_ASSERT( h_TypeOf( param ) == SY_VARIABLE ||
                     h_TypeOf( param ) == SY_ARRAY, "ReadCallParam" );

         p = h_NewR0( heap, SY_CALLPARVAR, param );
    }
    else
    {
         param = ReadOrExp( pcont, EXPECTED_PAR );

         switch( ld_GetType( descr ) )
         {
         case T_INT:    p = h_NewR0( heap, SY_CALLPARAMI, param );
                        break;

         case T_FLOAT:  p = h_NewR0( heap, SY_CALLPARAMF, param );
                        break;

         case T_VECTOR: p = h_NewR0( heap, SY_CALLPARAMV, param );
                        break;

         case T_STR:    p = h_NewR0( heap, SY_CALLPARAMS, param );
                        break;

         default:
                 lng_ASSERTNQ("ReadCallParam: Unknown param type");
         }
    }

    return p;
 }

 /*========================================================================*/
 /*
  *   Чтение вызова функции
  *
  *   callParam ::= param
  *   callParam ::= param - callParam
  *
  *   call := SY_CALL - callParam
  *           |
  *           SY_VARIABLE(Описание функции)
  */
TTree *ReadCallExp( TParserContext *pcont, TName *descr )
 {
    NAMEDEF_TYPE nameDef = ld_GetNameDef( descr );
    TName       *parDescr;
    TTree       *p, *par;
    TTreeHeap   *heap;
    LEX_TYPE     callType;

    if( nameDef != DEF_FUNC &&
        nameDef != DEF_FUNCEXTERN )
         return NIL;

    heap = pcont->m_heap;

    switch( nameDef )
    {
    case DEF_FUNC:       callType = SY_CALL;       break;
    case DEF_FUNCEXTERN: callType = SY_CALLEXTERN; break;
    }

    p = h_NewR0( heap, callType, h_NewV( heap, descr ));
    par = p;
    ReadLex( pcont );

    /*
     * Читаем список параметров
     */
    ExpectedLParent( pcont );

    while( (parDescr = ld_GetNext( descr )) != NIL )
    {
            if( ld_GetNameDef( parDescr ) == DEF_VAR )
                 break;

            cdr(par) = ReadCallParam( pcont, parDescr );
            par = cdr(par);

            if( par == NIL )
                 p_TxtError( pcont,
                             "Call %s():Expected expression",
                             ld_GetName( descr ));

            descr = parDescr;

            if( ld_GetNext( descr ) != NIL &&
                ld_GetNameDef( ld_GetNext( descr ) ) != DEF_VAR )
                 ExpectedComma( pcont );
    }

    ExpectedRParent( pcont );

    return p;
 }

 /*========================================================================*/
 /*
  * Функция разбора глобального имени.
  * Вызывается только из ReadVarExp().
  */
TTree *ReadVarExpGlobal( TParserContext *pcont,
                         TName          *descr,
                         TTree          *globalDefPtr )
 {
    TTreeHeap *heap = pcont->m_heap;

    switch( ld_GetNameDef( descr ) )
    {
    case DEF_CONST:
            {
            TTree *constPtr = car(globalDefPtr);

            ReadLex( pcont );

            switch(  h_TypeOf( constPtr )  )
            {
            case SY_FLOATCONS:
                     return h_NewF( heap, h_GetConstFLOAT( constPtr ));

            case SY_INTCONS:
                     return h_NewI( heap, h_GetConstINT  ( constPtr ));

            case SY_STRINGCONS:
                     return h_NewS( heap, h_GetConstSTR  ( constPtr ));
            default:
                   lng_ASSERTNQ("ReadVarExpGlobal: unknown const type");
            }
            }
            break;

    case DEF_CONSTEXTERN:
            ReadLex( pcont );
            return h_NewV( heap, descr );

    case DEF_FUNC:
	case DEF_FUNCEXTERN: /* CHECKME */
                   return ReadCallExp( pcont, descr );
    default:
            lng_ASSERTNQ("ReadVarExpGlobal:Unknown global name def");
    }

    return NIL;
 }

 /*========================================================================*/
 /*
  * Чтение переменной.
  * Если имя встретилось в списке глобальных имен
  * вызываем разборщик чтения вызова функции ReadCallExp.
  *
  * index  ::= SY_BOUND - SY_INTCONS(rangeOfArray)
  *            |
  *            exp
  *
  * mindex ::= index
  * mindex ::= SY_PLUS - mindex
  *            |
  *            index
  *
  *------------------------------------------------------
  *
  * varexp ::= SY_VARIABLE
  * varexp ::= SY_ARRAY
  *            |
  *            index
  *
  * varexp ::= SY_ARRAY
  *            |
  *            mindex
  */
TTree *ReadVarExp( TParserContext *pcont, int mayCall )
 {
    TName       *descr;
    TTree       *p = NIL, *l, *r, *e[ MAX_ARRAY_CNT ];
    int          i;
    NAMEDEF_TYPE nameDef;
    TTreeHeap   *heap = pcont->m_heap;

    if( LexType( pcont ) == SY_WORD )
    {
         int    isLocal;
         TTree *globalDefPtr;

         /*
          * Ищем переменную в списке имен
          */
         descr = h_SearchVariable( LexSTR( pcont ),
                                   pcont->m_ndefs, &isLocal, &globalDefPtr );

         if( descr == NIL )
              p_TxtError( pcont,
                         "Variable %s not definition", LexSTR( pcont ));

         if( !isLocal )
         {
              /*
               * Это вызов функции VOID
               */
              if( !mayCall  )
                   return NIL;

              return ReadVarExpGlobal( pcont, descr, globalDefPtr );
         }

         nameDef = ld_GetNameDef( descr );

         /*
          * Если это рекурсивный вызов самого себя, читаем параметры
          */
         if( nameDef == DEF_FUNC )
         {
              if( ld_GetType( descr ) != T_NONE )
                   return ReadCallExp( pcont, descr );
              return NIL;
         }

         if( nameDef != DEF_VAR &&
             nameDef != DEF_PAR &&
             nameDef != DEF_PARVAR )
              return NIL;

         ReadLex( pcont );

         /*
          * Если переменная объявлена как массив, то требуем
          * индексы.
          */
         if( ld_GetArrayCnt( descr ) != 0 )
         {
              ExpectedSymbol( pcont, "[", SY_LBRACK );

              if( ReadLexIfEqu( pcont, SY_PERIOD ) )
              {
                   /*
                    * Линейное обращение к массиву как [. i]
                    */
                   l = ReadOrExp( pcont, "Expected index" );
                   l = h_New( heap, SY_BOUND , l,
                              h_NewI( heap, RangeOfArray( descr )));
                   p = h_NewA( heap, descr, l );

              }
              else
              {
                   /*
                    * Обращение к многомерному массиву
                    */
                   int arrayCnt = ld_GetArrayCnt( descr );

                   for( i = 0;  ( i < arrayCnt )  ; ++i )
                   {
                        e[i] = h_New( heap, SY_BOUND,
                                      ReadOrExp( pcont, "Expected index" ),
                                      h_NewI( heap,
                                              ld_GetArrayRange( descr, i )));

                        if( i != arrayCnt - 1 )
                             ExpectedComma( pcont );
                   }

                   r = MakeArrayAdr( heap, e, descr, arrayCnt - 1 );
                   p = h_NewA( heap, descr, r );
              }

              ExpectedSymbol( pcont, "]", SY_RBRACK );
         }
         else p = h_NewV( heap, descr );

         /*
          * В конце проверяем на обращение к полю вектора
          */
         if( ld_GetType( descr ) == T_VECTOR )
              p = ReadField( pcont, p );
    }

    return p;
 }

 /*========================================================================*/
  /*
   * Чтение унарного выражения
   *
   * func ::= SY_SIN,  SY_COS,  SY_TAN,
   *          SY_ASIN, SY_ACOS, SY_ATAN,
   *          SY_EXP,  SY_LOG,  SY_ABS,
   *          SY_CREATEINT, SY_CREATEFLOAT
   *
   * unaExp ::= SY_CRG
   *            |
   *            unaExp
   *
   * unaExp ::= SY_INTCONS(val)
   * unaExp ::= SY_FLOATCONS(val)
   * unaExp ::= exp
   * unaExp ::= func
   *            |
   *            exp
   * unaExp ::= index
   * unaExp ::= SY_LVECTORITEM - SY_RVECTORITEM - exp(z)
   *            |                |
   *            exp(x)           exp(y)
   *
   * unaExp ::= varExp
   * unaExp ::= SY_NOT
   *            |
   *            compExp
   * unaExp ::= SY_NEG
   *            |
   *            unaExp
   * unaExp ::= SY_GR
   *            |
   *            unaExp
   */
TTree *ReadUnarExp( TParserContext *pcont )
 {
    int        sign = 0;
    LEX_TYPE   com;
    TTree     *p    = NIL, *r, *l;
    TTreeHeap *heap = pcont->m_heap;

    for(;;)
         if( ReadLexIfEqu( pcont, SY_MINUS ) )
              sign = !sign;
         else
         if( !ReadLexIfEqu( pcont, SY_PLUS ) )
              break;

    switch( LexType( pcont ) )
    {
    case SY_TILDA:
            ReadLex( pcont );
            p = h_NewR0( heap, SY_CRG , ReadUnarExp( pcont ));
            break;

    case SY_INTCONS:
            p = h_NewI( heap, lex_GetINT( pcont->m_scanner ) );
            ReadLex( pcont );
            break;

    case SY_FLOATCONS:
            p = h_NewF( heap, lex_GetFLOAT( pcont->m_scanner ) );
            ReadLex( pcont );
            break;

    case SY_STRINGCONS:
            p = h_NewS( heap, lex_GetSTR( pcont->m_scanner ) );
            ReadLex( pcont );
            return p;

    case SY_PI:
            p = h_NewF( heap, CONST_PI );
            ReadLex( pcont );
            break;

    case SY_LPARENT:
            ReadLex( pcont );
            p = ReadOrExp( pcont, "Statment expected after (" );
            ExpectedRParent( pcont );
            break;

    case SY_SIN:  case SY_COS:   case SY_TAN:
    case SY_ASIN: case SY_ACOS:  case SY_ATAN:
    case SY_EXP:  case SY_LOG:   case SY_ABS:
    case SY_INT:  case SY_FLOAT: case SY_RNDI:
            com = LexType( pcont );
            ReadLex( pcont );
            ExpectedLParent( pcont );
            p = h_NewR0( heap, com ,ReadOrExp( pcont, EXPECTED_PAR ));
            ExpectedRParent( pcont );
            break;

    case SY_ATAN2:
            ReadLex( pcont );
            ExpectedLParent( pcont );
            l = ReadOrExp( pcont, EXPECTED_PAR );
            ExpectedComma( pcont );
            r = ReadOrExp( pcont, EXPECTED_PAR );
            ExpectedRParent( pcont );
            p = h_New( heap, SY_ATAN2, l, r );
            break;

    case SY_RNDF:
            ReadLex( pcont );
            p = h_New0( heap, SY_RNDF );
            ExpectedLParent( pcont );
            ExpectedRParent( pcont );
            break;

    case SY_BOUND:
            ReadLex( pcont );
            ExpectedLParent( pcont );
            l = ReadOrExp( pcont, "Expected index" );
            ExpectedComma( pcont );
            r = ReadOrExp( pcont, "Expected range" );
            ExpectedRParent( pcont );

            p = h_New( heap, SY_BOUND, l, r );
            break;

    case SY_LBRACK: /* vector */
            ReadLex( pcont );
            /*
             *    [SY_LVECTORITEM]
             *   / \
             *  v   [SY_RVECTORITEM](val)
             *     / \
             *    v   ?(val)
             *
             */
            l = ReadOrExp( pcont, "VECTOR[Expected Expression" );
            p = h_NewR0( heap, SY_LVECTORITEM, l );
            ExpectedComma( pcont );

            l = ReadOrExp( pcont, "VECTOR[expr,Expected Expression" );
            cdr(p)   = h_NewR0( heap, SY_RVECTORITEM, l );
            ExpectedComma( pcont );

            l = ReadOrExp( pcont, "VECTOR[expr,expr,Expected Expression" );
            cdr(cdr(p)) = l;
            ExpectedSymbol( pcont, "]", SY_RBRACK );

            return CreateNEG( sign, heap, p );

    case SY_WORD:
            p = ReadVarExp( pcont, 1 );
            if( p == NIL )
                 p_TxtError( pcont, "Expected variable" );
            break;

    case SY_NOT:
            ReadLex( pcont );
            p = h_NewR0( heap, SY_NOT,
                         ReadCompExp( pcont,
                                      "After NOT expected expression" ));
            break;

    default:
            return NIL;
    } /* switch */

    if( ReadLexIfEqu( pcont, SY_TILDA ) )
         p = h_NewR0( heap, SY_CGR , p );


    p = CreateNEG( sign, heap, p );

    return p;
 }

 /*========================================================================*/
 /*
  * Возведение в степень
  *
  * powExp ::= unaExp
  * powExp ::= SY_POWER - powExp
  *            |
  *            unaExp
  * powExp ::= field
  */
TTree *ReadPowExp( TParserContext *pcont )
 {
    TTree *l, *r;

    if( (l = ReadUnarExp( pcont )) == NIL )
         return NIL;

    for(;;)
    {
         if( ReadLexIfEqu( pcont, SY_POWER ) )
         {
              if( (r = ReadPowExp( pcont )) == NIL )
                   p_TxtError( pcont, "After ** an expressin is expected" );

              l = h_New( pcont->m_heap, SY_POWER, l, r );
         }
         else
         if( LexType( pcont ) == SY_PERIOD )
         {
              l = ReadField( pcont, l );
              break;
         }
         else break;
    }

    return l;
 }

 /*========================================================================*/
 /*
  * Чтение операций с приоритетом умножения.
  *
  *  prodSig ::= SY_SMUL, SY_SSLASH, SY_PERSENT, SY_MOD
  *
  *  prodExp ::= prodSig - powExp
  *              |
  *              powExp
  */
TTree *ReadProdExp( TParserContext *pcont )
 {
    LEX_TYPE com;
    TTree   *l, *r;

    if( (l = ReadPowExp( pcont )) == NIL )
         return NIL;

    for(;;)
    {
         com = LexType( pcont );
         if( com != SY_SMUL    && com != SY_SLASH &&
             com != SY_PERSENT && com != SY_MOD )
              break;

         ReadLex( pcont );

         if( (r = ReadPowExp( pcont )) == NIL )
              p_TxtError( pcont, "After */MOD% an expressin is expected" );

         l = h_New( pcont->m_heap, com , l, r );
    }


    return l;
 }

 /*========================================================================*/
 /*
  * Чтение операций с приоритетом сложения
  *
  *  sumSig ::= SY_PLUS, SY_MINUS
  *
  *  sumExp ::= sumSig - prodExp
  *             |
  *             prodExp
  */
TTree *ReadSumExp( TParserContext *pcont )
 {
    LEX_TYPE   com;
    TTree     *l, *r;


    if( (l = ReadProdExp( pcont )) == NIL )
         return NIL;

    for(;;)
    {
         com = LexType( pcont );
         if( com != SY_PLUS && com != SY_MINUS )
              break;

         ReadLex( pcont );

         if( (r = ReadProdExp( pcont )) == NIL )
              p_TxtError( pcont, "After +- an expressin is expected" );

         l = h_New( pcont->m_heap, com, l, r );
    }

    return l;
 }

 /*========================================================================*/
 /*
  * Чтение операции с приоритетом сравнения
  *
  * compSig ::= SY_EQL, SY_LEQ, SY_NEQ, SY_LSS, SY_GEQ, SY_GRT
  *
  * compExp ::= compSig - sumExp
  *             |
  *             sumExp
  */
TTree *ReadCompExp( TParserContext *pcont, const char *nilError )
 {
    LEX_TYPE   com;
    TTree     *l, *r;

    if( (l = ReadSumExp( pcont )) == NIL )
    {
         if( nilError != NIL )
              p_TxtError( pcont, nilError );
         return NIL;
    }

    for(;;)
    {
         com = LexType( pcont );
         if( com != SY_EQL  &&  com != SY_LEQ &&
             com != SY_NEQ  &&  com != SY_LSS &&
             com != SY_GEQ  &&  com != SY_GRT )
              break;

         ReadLex( pcont );

         if( (r = ReadSumExp( pcont )) == NIL )
              p_TxtError( pcont, "After compare an expressin is expected" );

         l = h_New( pcont->m_heap, com, l, r );
    }

    return l;
 }

 /*========================================================================*/
 /*
  * Чтение операции с приоритетом AND
  *
  * compExp ::= SY_AND - compExp
  *             |
  *             compExp
  */
TTree *ReadAndExp( TParserContext *pcont )
 {
    TTree *l,*r;

    if( (l = ReadCompExp( pcont, NIL )) == NIL )
         return NIL;

    for(;;)
    {
         if( !ReadLexIfEqu( pcont, SY_AND ) )
              break;

         if( (r = ReadCompExp( pcont, NIL )) == NIL )
              p_TxtError( pcont, "After AND an expressin is expected" );

         l = h_New( pcont->m_heap, SY_AND , l, r );
    }

    return l;
 }

 /*========================================================================*/
 /*
  * Чтение операции с приоритетом OR
  *
  * compExp ::= SY_OR - andExp
  *             |
  *             andExp
  */
TTree *ReadOrExp( TParserContext *pcont, const char *nilError )
 {
    TTree *l, *r;

    if( (l = ReadAndExp( pcont )) == NIL )
    {
         if( nilError != NIL )
              p_TxtError( pcont, nilError );
         return NIL;
    }

    for(;;)
    {
         if( !ReadLexIfEqu( pcont, SY_OR ) )
              break;

         if( (r = ReadAndExp( pcont ) ) == NIL )
              p_TxtError( pcont, "After OR an expressin is expected" );

         l = h_New( pcont->m_heap, SY_OR , l, r );
    }

    if( l == NIL && nilError != NIL )
         p_TxtError( pcont, nilError );

    return l;
 }


 /*========================================================================*/
 /*
  * Чтение константного выражения.
  * Создается дерево разбора, все константные
  * вычисления обрабатываются. если полученное значение
  * констнта - она возвращается, иначе выдается ошибка.
  * Куча возвращается в исходное выражение
  */
TInt ReadConstI( TParserContext *pcont )
 {
    DATA_TYPE  type;
    int        mark;
    TInt       val;
    TTreeHeap *heap = pcont->m_heap;

    mark = h_MarkHeap( heap );

    {
         TTree *p = tc_OptimizeExprLev1(
                        heap,
                        tc_ConvertToCode( heap,
                                          ReadOrExp( pcont,
                                                     "Expected expression" ),
                                          &type ),
                        pcont->m_scanner,
                        pcont->m_nameDescr,
                        pcont->m_ndefs );

         if( type != T_INT  ||  !h_IsConstI( p ) )
              p_TxtError( pcont, "Need const integer expression" );

         val = h_GetConstINT( p );
    }

    h_ReleaseHeap( heap, mark );

    return val;
 }

 /*========================================================================*/
DATA_TYPE FuncType( TParserContext *pcont )
 {
    return ld_GetType( h_GetLocal( pcont->m_ndefs ) );
 }

 /*========================================================================*/
 /*
  * Чтение оператора RETURN
  *
  * retSym ::= SY_RETURN, SY_RETURNI, SY_RETURNF, SY_RETURNV
  *
  * return ::= retSym
  *            |
  *            exp
  */
TTree *OperatorRETURN( TParserContext *pcont )
 {
    if( ReadLexIfEqu( pcont, SY_RETURN ) )
    {
         TTree     *p;
         TTreeHeap *heap = pcont->m_heap;

         pcont->m_beenRETURN =1;

         if( FuncType( pcont ) == T_NONE )
         {
              ExpectedSemicolon( pcont );
              return h_New0( heap, SY_RETURN );
         }

         p = ReadOrExp( pcont, "RETURN: expected expression" );

         ExpectedSemicolon( pcont );

         switch( FuncType( pcont ) )
         {
         case T_INT:    return h_NewR0( heap, SY_RETURNI, p );
         case T_FLOAT:  return h_NewR0( heap, SY_RETURNF, p );
         case T_VECTOR: return h_NewR0( heap, SY_RETURNV, p );
         default:
              p_Error( pcont, "?? Function unknown type" );
         }
    }

    return NIL;
 }

 /*========================================================================*/
 /*
  * Чтение определения массива.
  * Если идут квадрантые скобки читается определения и вся
  * информация заносится в descr.
  */
void ReadArrayDef( TParserContext *pcont, TName *descr )
 {
    int i, j;

    if( ReadLexIfEqu( pcont, SY_LBRACK ) )
    {
         TInt arrayR[ MAX_ARRAY_CNT ];

         for( i = 0; i < MAX_ARRAY_CNT; ++i )
         {
              TInt rg = ReadConstI( pcont );

              if( rg <= 0 )
                   p_TxtError( pcont,
                              "Range of array [%d]<=0",((int)(rg)));

              arrayR[i] = rg;

              if( ReadLexIfEqu( pcont, SY_RBRACK ) )
                   break;

              if( !ReadLexIfEqu( pcont, SY_COMMA ) )
                   p_TxtError( pcont, "Expected , or ]" );
         } /* for */

         ++i;
         ld_SetArrayCnt( descr, i );

         for( j = 0; j < i; ++j )
              ld_SetArrayRange( descr, j, arrayR[j] );
    }
 }

 /*========================================================================*/
 /*
  * Чтение объявления одного параметра функции.
  */
TName *ReadFuncParamDef( TParserContext *pcont )
 {
    int      isVar = 0;
    LEX_TYPE lexParType;

    if( ReadLexIfEqu( pcont, SY_VAR ) )
         isVar = 1;

    lexParType = LexType( pcont );

    if( lexParType == SY_INT    ||
        lexParType == SY_FLOAT  ||
        lexParType == SY_VECTOR ||
        lexParType == SY_STR )
    {
         TName *descr;

         ReadLex( pcont );

         if( LexType( pcont ) != SY_WORD )
              p_TxtError( pcont, "Expected variable name" );

         descr = h_AddName( p_ErrorOf( pcont ), pcont->m_nameDescr );
         ld_InitTName( descr,
                       LexSTR( pcont ),
                       isVar ? DEF_PARVAR : DEF_PAR,
                       T_NONE );

         switch( lexParType )
         {
         case SY_INT:    ld_SetType( descr, T_INT );     break;
         case SY_FLOAT:  ld_SetType( descr, T_FLOAT );   break;
         case SY_VECTOR: ld_SetType( descr, T_VECTOR );  break;
         case SY_STR:    ld_SetType( descr, T_STR );  break;
         }

         ReadLex( pcont );
         if( isVar )
              ReadArrayDef( pcont, descr );

         return descr;
    }

    return NIL;
 }

 /*========================================================================*/
 /*
  * Чтение объявления константы
  *
  * const ::= SY_VARIABLE(DEF_CONST)
  *           |
  *           CPU_PUSHC I|F
  */
TTree *p_Const( TParserContext *pcont,
                TTree         **treeHead )
 {
    TScanner      *scanner   = pcont->m_scanner;
    TNameDefs     *ndefs     = pcont->m_ndefs;
    TTreeHeap     *heap      = pcont->m_heap;
    TNameArray    *nameDescr = pcont->m_nameDescr;

    TTree         *p = NIL, *cval;
    LEX_TYPE       lexConstType;
    TName         *descr;
    int            isLocal;
    int            mark;
    TInt           vali;
    TFloat         valf;
    DATA_TYPE      type;
    TTree         *cp;

    if( !ReadLexIfEqu( pcont, SY_CONST ) )
         return NIL;

    lexConstType = lex_GetType( scanner );

    if(    lexConstType != SY_INT
        && lexConstType != SY_FLOAT
        && lexConstType != SY_STR )
         p_TxtError( pcont, "CONST: Expected type" );

    lex_Get( scanner );

    if( lex_GetType( scanner ) != SY_WORD )
         p_TxtError( pcont, "Expected CONST name" );

    /*
     * Проверяем на дублирование имени
     */
    h_InitTNameDefs( ndefs, *treeHead, NIL );
    descr = h_SearchVariable( lex_GetSTR( scanner ), ndefs, &isLocal, NIL );

    if( descr != NIL )
         p_TxtError( pcont, "Dublicate identefier %s", lex_GetSTR( scanner ));

    /*
     * Заводим новое имя
     */
    descr = h_AddName( h_ErrorOf( heap ), nameDescr );
    ld_InitTName( descr, lex_GetSTR( scanner ), DEF_CONST, T_NONE );
    h_InitTNameDefs( ndefs, *treeHead, descr );

    switch( lexConstType )
    {
    case SY_INT:    ld_SetType( descr, T_INT   ); break;
    case SY_FLOAT:  ld_SetType( descr, T_FLOAT ); break;
    case SY_STR:    ld_SetType( descr, T_STR   ); break;
    }

    lex_Get( scanner );

    /*
     * Если эта константа extern, то переопределяем
     * ее тип объявления и выходим.
     */
    if( ReadLexIfEqu( pcont, SY_EXTERN ) )
    {
         DATA_TYPE ct = ld_GetType( descr );

         if(  ct != T_INT   &&
              ct != T_FLOAT )
                 p_TxtError( pcont, "External constant may be int or float" );

         ld_SetNameDef( descr, DEF_CONSTEXTERN );
         p = h_NewV( heap, descr );
         ExpectedSemicolon( pcont );

         return p;
    }

    ExpectedSymbol( pcont, "=", SY_EQL );

    /*
     * Считываем значение
     */
    if(    lexConstType == SY_INT
        || lexConstType == SY_FLOAT )
    {
         mark = h_MarkHeap( heap );
         cp = tc_OptimizeExprLev1(
                      heap,
                      tc_ConvertToCode(
                            heap,
                            ReadOrExp( pcont,
                                       "Expected constant expression" ),
                            &type ),
                      scanner,
                      nameDescr,
                      ndefs );



         switch( type )
         {
         case T_INT:   if( !h_IsConstI( cp ) )
                            p_TxtError( pcont, "Need INT const expression" );
                       vali = h_GetConstINT( cp );
                       break;

         case T_FLOAT: if( !h_IsConstF( cp ) )
                            p_TxtError( pcont, "Need FLOAT const expression");
                       valf = h_GetConstFLOAT( cp );
                       break;
         }

         h_ReleaseHeap( heap, mark );

         switch( type )
         {
         case T_INT:
                 switch( lexConstType )
                 {
                 case SY_INT:   cval = h_NewI( heap, vali ); break;
                 case SY_FLOAT: cval = h_NewF( heap, vali ); break;
                 }
                 break;

         case T_FLOAT:
                 switch( lexConstType )
                 {
                 case SY_INT:   cval = h_NewI( heap, (TInt)valf ); break;
                 case SY_FLOAT: cval = h_NewF( heap, valf );       break;
                 }
                 break;
         }

    }
    else
    if( lexConstType == SY_STR )
    {
         cval = ReadOrExp( pcont, "Expected string constant" );

         if( h_TypeOf( cval ) != SY_STRINGCONS )
               p_TxtError( pcont, "Expected string const" );
    }
    else p_TxtError( pcont, "Unknown CONST type" );

    p = h_NewV( heap, descr );
    car(p) = cval;

    ExpectedSemicolon( pcont );

    return p;
 }

 /*========================================================================*/
int ChainOfNameNIL( TName *name )
 {
    return    name == NIL
           || (    ld_GetNameDef( name ) != DEF_PAR
                && ld_GetNameDef( name ) != DEF_PARVAR );
 }

 /*========================================================================*/
 /*
  * Чтение описания функции
  *
  * funcDef ::= SY_FUNCTION
  *             |
  *             SY_VARIABLE(func)
  *             |
  *             operator
  *
  * Списком глобальных переменных служит дерево разбора, на вершину
  * которого указывает treeHead.
  */
TTree *p_Function( TParserContext *pcont,
                   TTree         **treeHead )
 {
    TScanner      *scanner   = pcont->m_scanner;
    TNameDefs     *ndefs     = pcont->m_ndefs;
    TTreeHeap     *heap      = pcont->m_heap;
    TNameArray    *nameDescr = pcont->m_nameDescr;

    LEX_TYPE       lexFuncType;
    TName         *descr, *paramDescr;
    TTree         *p, *res, *globalDef;
    int            isLocal, beenFORWARD = 0;

    if( !ReadLexIfEqu( pcont, SY_FUNC ) )
         return NIL;

    lexFuncType = lex_GetType( scanner );

    if( lexFuncType != SY_INT    &&
        lexFuncType != SY_FLOAT  &&
        lexFuncType != SY_VECTOR &&
        lexFuncType != SY_VOID )
         p_TxtError( pcont, "FUNC: Expected type" );

    lex_Get( scanner );

    if( lex_GetType( scanner ) != SY_WORD )
         p_TxtError( pcont, "Expected FUNC name" );

    /*
     * Проверяем на дублирование имени
     */
    h_InitTNameDefs( ndefs, *treeHead, NIL );
    descr = h_SearchVariable( lex_GetSTR( scanner ), ndefs, &isLocal,
                              &globalDef );

    if( descr != NIL && !isLocal )
    if( h_TypeOf( globalDef ) == SY_FORWARD )
         beenFORWARD = 1;
    else p_TxtError( pcont, "Dublicate identefier %s", lex_GetSTR( scanner ));

    /*
     * Заводим новое имя
     */
    descr = h_AddName( h_ErrorOf( heap ), nameDescr );
    ld_InitTName( descr, lex_GetSTR( scanner ), DEF_FUNC, T_NONE );
    h_InitTNameDefs( ndefs, *treeHead, descr );

    switch( lexFuncType )
    {
    case SY_VOID:   ld_SetType( descr, T_NONE   ); break;
    case SY_INT:    ld_SetType( descr, T_INT    ); break;
    case SY_FLOAT:  ld_SetType( descr, T_FLOAT  ); break;
    case SY_VECTOR: ld_SetType( descr, T_VECTOR ); break;
    }

    lex_Get( scanner );

    /*
     * Считываем аргументы
     */
    ExpectedLParent( pcont );
    h_SetLastDef( ndefs, descr );

    while( (paramDescr = ReadFuncParamDef( pcont )) != NIL )
    {
         h_AddDef( ndefs, paramDescr );

         if( !ReadLexIfEqu( pcont, SY_COMMA ) )
              break;
    }

    ExpectedRParent( pcont );

    switch( lex_GetType( scanner ) )
    {
    case SY_FORWARD:
            lex_Get( scanner );
            ExpectedSemicolon( pcont );
            return h_NewR0( heap, SY_FORWARD, h_NewV( heap, descr ));

    case SY_EXTERN:
            lex_Get( scanner );
            ExpectedSemicolon( pcont );
            ld_SetNameDef( descr, DEF_FUNCEXTERN );
            p = h_NewV( heap, descr );
            return h_NewR0( heap, SY_EXTERN, p );
    }

    /*
     * Считываем определения локальных переменных
     */
    while( lex_GetType( scanner ) == SY_VAR )
         ReadVarDef( pcont );

    p = h_NewV( heap, descr );

    /*
     * Считываем описание тела
     */
    if( (car(p) = Operator( pcont, 0 )) == NIL )
         p_TxtError( pcont, "Expected function body" );

    res = h_NewR0( heap, SY_FUNCTION, p);

    /*
     * Если было FORWARD определение сравниваем с прототипом
     */
    if( beenFORWARD )
    {
         TName *fdescr = h_GetDescr(car(globalDef));
         TName *cdescr = descr;

         for(;;)
         {
              int i;

              if(   fdescr == NIL && cdescr != NIL
                 || cdescr == NIL && fdescr != NIL
                 || ld_GetNameDef( fdescr )  != ld_GetNameDef( cdescr )
                 || ld_GetArrayCnt( fdescr ) != ld_GetArrayCnt( cdescr )
                 || ld_GetType( fdescr )     != ld_GetType( cdescr )
                )
                   p_TxtError( pcont,
                               "Not matched with FORWARD declaration %s",
                               ld_GetName( fdescr ) );

              for( i = 0; i < ld_GetArrayCnt( fdescr ) ; ++i )
                   if(    ld_GetArrayRange( fdescr, i )
                       != ld_GetArrayRange( cdescr, i ) )
                        p_TxtError( pcont, "Not matched array range"
                                           " with FORWARD declaration" );

              fdescr = ld_GetNext( fdescr );
              cdescr = ld_GetNext( cdescr );

              if( ChainOfNameNIL( fdescr ) && ChainOfNameNIL( cdescr ) )
                   break;
         }

         /*
          * Вместо FORWARD определения ставим заглушку
          */
         h_TypeSet( globalDef, SY_EMPTY );
    }

    if( lexFuncType != SY_VOID && !pcont->m_beenRETURN )
         p_TxtError( pcont,
                     "Function %s should return a value",
                     ld_GetName( descr ));

    return res;
 }



 /*========================================================================*/
 /*
  * Чтение объявления переменной
  */
void ReadVarDef( TParserContext *pcont )
 {
    LEX_TYPE   lexVarType;
    TName     *descr;
    DATA_TYPE  type;

    if( !ReadLexIfEqu( pcont, SY_VAR ) )
         return;

    lexVarType =  LexType( pcont );

    if( lexVarType != SY_INT    &&
        lexVarType != SY_FLOAT  &&
        lexVarType != SY_VECTOR &&
        lexVarType != SY_STR )
         p_TxtError( pcont, "VAR: Expected type" );

    ReadLex( pcont );

    switch( lexVarType )
    {
    case SY_INT:    type = T_INT;    break;
    case SY_FLOAT:  type = T_FLOAT;  break;
    case SY_VECTOR: type = T_VECTOR; break;
    case SY_STR:    type = T_STR;    break;
    }

    if( LexType( pcont ) != SY_WORD )
         p_TxtError( pcont, "VAR: Expected variable name" );

    for(;;)
    {
         int isLocal;
         descr =h_SearchVariable( LexSTR( pcont ),
                                  pcont->m_ndefs, &isLocal, NIL );
         if( descr != NIL )
              p_TxtError( pcont, "Duplicate identefier %s", LexSTR( pcont ));

         descr = h_AddName( p_ErrorOf( pcont ), pcont->m_nameDescr );
         ld_InitTName( descr, LexSTR( pcont ), DEF_VAR, type );
         ReadLex( pcont );
         ReadArrayDef( pcont, descr );
         h_AddDef( pcont->m_ndefs, descr );

         if( !ReadLexIfEqu( pcont, SY_COMMA ) )
              break;
    }

    ExpectedSemicolon( pcont );
 }


 /*========================================================================*/
 /*
  * Разбор оператора присвоения.
  * Используется в отдельно и при разборе FOR
  */
TTree *Assign( TParserContext *pcont, LEX_TYPE lex )
 {
    TTree *l, *r;

    if( (r = ReadVarExp( pcont, 0 ) ) == NIL )
         return NIL;

    ExpectedSymbol( pcont, ":=", SY_BECOMES );
    l = ReadOrExp( pcont, "Expected expression" );

    return h_New( pcont->m_heap, lex, l, r );
 }

 /*========================================================================*/
 /*
  * Разбор оператора присвоения.
  * assign ::= SY_BECOMES - varExp(destination)
  *            |
  *            expr(source)
  */
TTree *Assignment( TParserContext *pcont )
 {
    TTree *p;

    p = Assign( pcont, SY_BECOMES );
    if( p != NIL )
         ExpectedSemicolon( pcont );

    return p;
 }


 /*========================================================================*/
 /*
  * Разбор оператора IF.
  *
  *  opIF ::= SY_IF - operator
  *           |
  *           expr
  *  opIF ::= SY_IFELSE - SY_OPERATOR - operator(ELSE)
  *           |           |
  *           expr        operator(THEN)
  */
TTree *OperatorIF( TParserContext *pcont,
                   int             loopDeep )
 {
    TTree *l, *r, *e = NIL;

    if( ReadLexIfEqu( pcont, SY_IF ) )
    {
         TTreeHeap *heap = pcont->m_heap;

         l = ReadOrExp ( pcont, "IF expected expression" );
         ExpectedSymbol( pcont, "THEN", SY_THEN );
         r = Operator( pcont, loopDeep );

         if( ReadLexIfEqu( pcont, SY_ELSE ) )
         {
              e = Operator( pcont, loopDeep );

              return h_New( heap, SY_IFELSE, l,
                          h_New( heap, SY_OPERATOR, r, e ));
         }


         return h_New( heap, SY_IF, l, r );
    }

    return NIL;
 }

 /*========================================================================*/
 /*
  * opLOOP ::= SY_LOOP
  *            |
  *            operator
  */
TTree *OperatorLOOP( TParserContext *pcont,
                     int             loopDeep )
 {
    if( ReadLexIfEqu( pcont, SY_LOOP ) )
         return h_NewR0( pcont->m_heap, SY_LOOP,
                       Operator( pcont, loopDeep + 1 ));

    return NIL;
 }


 /*========================================================================*/
 /*
  * Разбор составного оператора.
  *
  * mop ::= SY_OPERATOR
  *         |
  *         operator
  * mop ::= SY_OPERAOR - mop
  *         |
  *         operator
  * ----------------------------
  * opList ::= SY_OPERATOR
  *            |
  *            mop
  */
TTree *Operators( TParserContext *pcont,
                  int             loopDeep )
 {
    if( ReadLexIfEqu( pcont, SY_LBRACKE ) )
    {
         TTree *list = NIL, *p, *first = NIL;

         while( (p = Operator( pcont, loopDeep )) != NIL )
         {
             p = h_NewR0( pcont->m_heap, SY_OPERATOR, p );

             if( list == NIL )
                  first = list = p;
             else
             {
                  cdr(list) = p;
                  list = p;
             }
         }

         ExpectedSymbol( pcont, "}", SY_RBRACKE );

         return h_NewR0( pcont->m_heap, SY_OPERATOR, first );
    }

    return NIL;
 }

 /*========================================================================*/
 /*
  * Разбор объявления имени цикла.
  * loopName ::= SY_VARIABLE(name)
  */
TTree *ReadLoopName( TParserContext *pcont,
                     int             loopDeep )
 {
    if( ReadLexIfEqu( pcont, SY_LSS ) )
    {
         TName *descr;
         int    isLocal;

         if( LexType( pcont ) != SY_WORD )
              p_TxtError( pcont, "Expected LOOP name" );

         descr = h_SearchVariable( LexSTR( pcont ),
                                   pcont->m_ndefs, &isLocal, NIL );
         if( descr != NIL  &&  isLocal )
              p_TxtError( pcont,
                         "LOOP:Dublicate identifier %s", LexSTR( pcont ));

         descr = h_AddName( p_ErrorOf( pcont ), pcont->m_nameDescr );
         ld_InitTName( descr, LexSTR( pcont ), DEF_LOOPNAME, T_LABEL );
         ld_SetDataPtr( descr, loopDeep );
         ReadLex( pcont );

         h_AddDef( pcont->m_ndefs, descr );

         ExpectedSymbol( pcont, ">",SY_GRT );

         return h_NewV( pcont->m_heap, descr );
    }

    return NIL;
 }

 /*========================================================================*/
 /*
  * Разбор оператора FOR.
  *
  * forType ::= SY_FOR, SY_FORD
  *
  *  opFOR ::= forType - operator
  *            |
  *            SY_FORTO - expr(destination)
  *            |
  *            SY_FORASSIGN - varExp(loop's name)
  *            |
  *            expr(start)
  *
  */
TTree *OperatorFOR( TParserContext *pcont,
                    int             loopDeep )
 {
    if( ReadLexIfEqu( pcont, SY_FOR ) )
    {
         TTree        *p, *l, *r;
         LEX_TYPE      lex;
         NAMEDEF_TYPE  ndef;
         TTreeHeap    *heap = pcont->m_heap;


         if( (l = Assign( pcont, SY_FORASSIGN )) == NIL )
              p_TxtError( pcont, "FOR: Expected assignment" );

         ndef = ld_GetNameDef( h_GetDescr( cdr(l) ) );

         if( ndef != DEF_VAR && ndef != DEF_PAR )
              p_TxtError( pcont, "FOR: Expected local variable" );

         lex = LexType( pcont );

         if( lex !=  SY_TO  &&  lex != SY_DOWNTO )
              p_TxtError( pcont, "Expected TO or DOWNTO" );

         ReadLex( pcont );

         if( lex == SY_TO )
              p = h_New0( heap, SY_FOR  );
         else p = h_New0( heap, SY_FORD );

         r = ReadOrExp( pcont, "FOR: Expected destination value" );
         ExpectedSymbol( pcont, "LOOP", SY_LOOP );

         car(p) = h_New( heap, SY_FORTO, l, r );
         cdr(p) = Operator( pcont, loopDeep + 1 );

         return p;
    }

    return NIL;
 }

 /*========================================================================*/
 /*
  * Разбор операторов BREAK или CONTINUE
  *
  */
TTree *OperatorBREAK_CONTINUE( TParserContext *pcont,
                               int             loopDeep,
                               LEX_TYPE        lex,
                               const char     *opName )
 {
    if( ReadLexIfEqu( pcont, lex ) )
    {
         TTreeHeap *heap = pcont->m_heap;

         if( loopDeep <= 0 )
              p_TxtError( pcont, "%s without LOOP", opName );

         if( ReadLexIfEqu( pcont, SY_SEMICOLON ) )
              return h_New0( heap, lex );

         /*
          * Распознаем имя метки
          */
         if( LexType( pcont ) == SY_WORD )
         {
              int    isLocal;
              TTree *p;
              TName *descr = h_SearchVariable( LexSTR( pcont ),
                                               pcont->m_ndefs,
                                               &isLocal,
                                               NIL );
              if( descr == NIL )
                   p_TxtError( pcont,
                              "Loop label %s not defined", LexSTR( pcont ));

              if( ld_GetNameDef( descr ) != DEF_LOOPNAME || !isLocal )
                   p_TxtError( pcont,
                              "Label %s is't loop's name", LexSTR( pcont ));

              p = h_NewV( heap, descr );
              ReadLex( pcont );

              ExpectedSemicolon( pcont );

              return h_NewR0( heap, lex, p );
         }
    }

    return NIL;
 }

 /*========================================================================*/
 /*
  * Разбор оператора BREAK.
  *  opBREAK ::= SY_BREAK
  *  opBREAK ::= SY_BREAK
  *              |
  *              SY_VARIABLE(label)
  */
TTree *OperatorBREAK( TParserContext *pcont, int loopDeep )
 {
    return OperatorBREAK_CONTINUE( pcont, loopDeep, SY_BREAK, "BREAK" );
 }

 /*========================================================================*/
 /*
  * Разбор оператора CONTINUE.
  *  opCONTINUE ::= SY_CONTINUE
  *  opCONTINUE ::= SY_CONTINUE
  *                 |
  *                 SY_VARIABLE(label)
  */
TTree *OperatorCONTINUE( TParserContext *pcont, int loopDeep )
 {
    return OperatorBREAK_CONTINUE( pcont, loopDeep, SY_CONTINUE, "CONTINUE" );
 }

 /*========================================================================*/
int OperatorNOP( TParserContext *pcont )
 {
    return ReadLexIfEqu( pcont, SY_SEMICOLON );
 }

 /*========================================================================*/
 /*
  * print ::= SY_PRINT
  *           |
  *           SY_CALLPARAM - SY_CALLPARAM ...
  *
  */
TTree *OperatorPRINT( TParserContext *pcont )
 {
    TTreeHeap *heap = pcont->m_heap;
    TTree     *p, *par, *firstPar, *pval;

    if( !ReadLexIfEqu( pcont, SY_PRINT ) )
         return NIL;

    p = h_New0( heap, SY_PRINT );
    ExpectedLParent( pcont );

    for( firstPar = NIL ;;)
    {
         if( ReadLexIfEqu( pcont, SY_RPARENT ) )
              break;

         if( ReadLexIfEqu( pcont, SY_COMMA ) )
         {
              if( firstPar == NIL )
                   firstPar = par = h_New0( heap, SY_CALLPARAMEOL );
              else
              {
                   cdr(par) = h_New0( heap, SY_CALLPARAMEOL );
                   par = cdr(par);
              }
              continue;
         }

         pval = ReadOrExp( pcont, NIL );
         if( pval != NIL )
         {
              if( firstPar == NIL )
                   firstPar = par = h_NewR0( heap, SY_CALLPARAM, pval );
              else
              {
                   cdr(par) = h_NewR0( heap, SY_CALLPARAM, pval );
                   par = cdr(par);
              }
              continue;
         }

         break;
    }

    car(p) = firstPar;
    ExpectedSemicolon( pcont );

    return p;
 }

 /*========================================================================*/
TTree *OperatorCALL( TParserContext *pcont )
 {
    int           isLocal, dropSize;
    TTree        *globalDefPtr;
    TName        *descr;
    NAMEDEF_TYPE  nameDef;
    const char   *fname;
    TTree        *p;

    if( LexType( pcont ) != SY_WORD )
         return NIL;

    fname = LexSTR( pcont );

    /*
     * Ищем переменную в списке имен
     */
    descr = h_SearchVariable( fname,
                              pcont->m_ndefs, &isLocal, &globalDefPtr );

    if( descr == NIL )
         p_TxtError( pcont, "Unknown function %s", fname );

    nameDef = ld_GetNameDef( descr );

    if( nameDef != DEF_FUNC  &&
        nameDef != DEF_FUNCEXTERN )
         p_TxtError( pcont, "Call non function %s", fname );

    p = ReadCallExp( pcont, descr );

    dropSize = 0;
    switch( ld_GetType( descr ) )
    {
    case T_NONE: break;
    case T_INT: case T_FLOAT: case T_STR: dropSize = 1; break;
    case T_VECTOR:                        dropSize = 3; break;
    default: lng_ASSERTNQ("OperatorCALL: Unknown func type");
    }

    if( dropSize != 0 )
    {
         TTreeHeap *heap = pcont->m_heap;

         p = h_New( heap, SY_DROPSTACK, p, h_NewI( heap, _TINT(dropSize) ) );
    }


    ExpectedSemicolon( pcont );

    return p;
 }

 /*========================================================================*/
 /*
  * Разбор оператора.
  *
  * operator ::= assign
  * operator ::= opList
  * operator ::= opIF
  * operator ::= SY_CYCLE - SY_VARIBLE(label)
  *              |
  *              opLOOP, opFOR
  * operator ::= opBREAK
  * operator ::= opCONTINUE
  * operator ::= opRETURN
  * operator ::= opCALL
  */
TTree *Operator( TParserContext *pcont,
                 int             loopDeep )
 {
    TTree     *p;
    TTreeHeap *heap = pcont->m_heap;

    if( (p = Assignment( pcont )) != NIL )
         return p;

    if( (p = OperatorIF( pcont, loopDeep )) != NIL )
         return p;

    if( (p = OperatorPRINT( pcont )) != NIL )
         return p;

    if( (p = OperatorCALL( pcont )) != NIL )
         return p;

    if( (p = Operators ( pcont, loopDeep )) != NIL )
         return p;

    {
         TTree *loopName = ReadLoopName( pcont, loopDeep );

         p = OperatorLOOP ( pcont, loopDeep );

         if( p == NIL )
              p = OperatorFOR( pcont, loopDeep );

         if( p != NIL )
             return h_New( heap, SY_CYCLE, p, loopName );

         if( loopName != NIL )
              p_TxtError( pcont, "Define loop name without LOOP" );
    }

    if( (p = OperatorBREAK( pcont, loopDeep )) != NIL )
         return p;

    if( (p = OperatorCONTINUE( pcont, loopDeep )) != NIL )
         return p;

    if( (p = OperatorRETURN( pcont )) != NIL )
         return p;

    OperatorNOP( pcont );

    return NIL;
 }

 /*========================================================================*/
TTree *p_GlobalDefs( TTreeHeap  *heap,
                     TScanner   *scanner,
                     TNameArray *nameDescr,
                     TTree     **treeHead,
                     TNameDefs  *ndefs  )
 {
    TTree          *p;
    TParserContext  pcont;

    p_InitTParserContext( &pcont, heap, scanner, nameDescr, ndefs );

    p = p_Function( &pcont, treeHead );

    if( p != NIL )
    {
         /*
          * Инициализируем вершину дерева - она же  список функций и констант
          */
         if( *treeHead == NIL )
              *treeHead = p;

         return p;
    }

    p = p_Const( &pcont, treeHead );

    if( p != NIL )
    {
         if( *treeHead == NIL )
              *treeHead = p;

         return p;
    }

    if( lex_GetType( scanner ) == SY_WORD )
         p_TxtError( &pcont, "Unknown word %s", lex_GetSTR( scanner ) );

    if( lex_GetType( scanner ) != SY_EOF )
         p_TxtError( &pcont, "Sintaxis error" );

    return p;
 }


 /*========================================================================*/
TScanner *p_Include(TScanner *scanner, TError *err)
 {
    if(  lex_GetType( scanner ) == SY_INCLUDE  )
    {    
         if( !lex_Get( scanner )  )
              p_TxtError2( scanner, err,
                          lex_ErrorToStr( lex_GetErrorCode( scanner )));
         if(  lex_GetType( scanner ) != SY_STRINGCONS  )
              p_TxtError2( scanner, err, "Include: Expected string" );
         else return lex_Include( scanner, lex_GetSTR( scanner) );
    }

    return NULL;
 }


 /*========================================================================*/
 /*
  * Разбор всей программы.
  * Результат возвращается в виде списка функций:
  *
  * programm ::= SY_FUNCTION - SY_FUNCTION ...
  */
TTree *p_Programm( TTreeHeap  *heap,
                   TScanner   *scanner,
                   TNameArray *nameDescr,
                   TTree     **treeHead,
                   TNameDefs  *ndefs )
 {
    TTree    *p;
    TScanner *incScan = p_Include( scanner, h_ErrorOf( heap ) );

    if(  incScan != NULL  )
         scanner = incScan;

    p = p_GlobalDefs( heap, scanner, nameDescr, treeHead, ndefs );

    while( p != NIL )
    {
         incScan = p_Include( scanner, h_ErrorOf( heap ) );

         if(  incScan != NULL  )
              scanner = incScan;

         cdr(p) = p_GlobalDefs( heap, scanner, nameDescr, treeHead, ndefs );

         if( cdr(p) == NIL )
              break;

         p = cdr(p);
    }

    return *treeHead;
 }

/* End of PARSER.C */

