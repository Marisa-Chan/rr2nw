/*
     File:  TR_CODE.C
     Autor: Suavik
     Ver 1.0

     Префикс tc_

     Оптимизировать по обращению к переменным Считывание массивов
       (многократное)
     OptimizeExpr        i-i, i/i
     "VAR
      INT i; 5+6+i+3+(1+7.)"  ->  8.+FLOAT(14+i)
      i := 13.5+j; -> INT(1.35e1+FLOAT(j))

     Подумать как передавать безразмерные массивы

     Если есть деления можно использовать сокращения

     Удаление BOUND(BOUND+BOUND+BOUND())
     В константных вычислениях подобрать значение pow() для возведения 0**x
     Возможно надо расставлять скобки в логических выражениях

     Тест на проход всех линейных участков кода и
     на срабатычвание/несрабатывание всех if

     Не понял обработку ошибок в считывании переменной

     h_AddName - может вернуть NIL?

     В сканере не предусмотрена обработка ошибок в lex_GetINT,...
     Попробовать для параметров расставить const

     Сделать обработку внутренних ошибок компилятора.
     Если FOR кончился можно использовать его цикловую переменную конца цикла

     Нет операций сравнения векторов

     Текущая тестируемая функция CalcI( TTree *p )

     разгрузить tc_ConvertToCode на предмет cdr(p)

     Необходимо раскрывать {} на один оператор и скобки в пределах
     линейного участка кода.

     29.08.97 Добавлена оптимизация на использование переменной, которой
              было присвоена константа

     30.08.97 SortMulSum( TTree *p ) - Содержала серьезную ошибку при
              перекодировке знаков. Не было тщательной проверки.

     24.09.97 При оптимизации на удаление несколько раз перегруженной
              переменной небыло проверки на использование в индексе.

     22.10.97 SortMulSum - не правильно перекодировался список prl.
              Возможно не до конца исправлено.

              Plus0 - обрабатывал вычитание из 0 как прибавление 0.

     23.10.97 Удалено раздробление векторов при вызове функции,
              поскольку его надо запрещать при вызове с VAR, а определить
              как срабатывает UnionVectors при нескольких вызовах нет времени.

     11.12.97 Сначала создавалась новая пеpеменная, а потом пpи создании
              уникального имени искалось ее наличие, и получалось сpавнение
              со стpокой, pавной NULL.
 */


#include <math.h>
#include <process.h>

#include "parser.h"
#include "langer.h"
#include "tr_code.h"
#include "strequ.h"

//TTreeHeap *g_heap;
//void PrintTree( TTree *p);


/* FIXME */
#define CONST_MAXFLOAT 1e300

typedef struct{/* Тип элемента таблицы преобразования типов унарных операций*/
    LEX_TYPE m_com;  /* Код операции                                        */
    int      m_conv; /* Код типа привидения операнда и результата операции  */
} TConvert;

typedef struct{
    LEX_TYPE  m_lex;
    LEX_TYPE  m_op;
    DATA_TYPE m_type;
} TReplLex;

#include "optabl.h"

const char *FIELD_VECTOR_ERROR = "Vector component must be INT or FLOAT";

 /*========================================================================*/
DATA_TYPE Int2DATA_TYPE( int i )
 {
    switch( i )
    {
    case 0: return T_INT;
    case 1: return T_FLOAT;
    case 2: return T_VECTOR;
    default:
           lng_ASSERTNQ( "Error Int2DATA_TYPE\n" );
    }

    return T_NONE;
 }

 /*========================================================================*/
int DATA_TYPE2Int( DATA_TYPE t )
 {
    switch( t )
    {
    case T_NONE:   return -1;
    case T_INT:    return  0;
    case T_FLOAT:  return  1;
    case T_VECTOR: return  2;
    }

    lng_ASSERTNQ( "Error DATA_TYPE2Int\n" );

    return -1;
 }

 /*========================================================================*/
 /*
  * Проверка типа при взятии поля x, y или z
  */
void ExpectedVec( DATA_TYPE type, TTreeHeap *heap )
 {
    if( type != T_VECTOR )
        lng_Error( h_ErrorOf( heap ), ".x|y|z Only at VECTOR" );
 }

 /*========================================================================*/
 /*
  * Выдача сообщения об ошибке при приведеннии типа INT(), FLOAT(),
  * а также при автоматическом приведении типа к INT или FLOAT
  */
void ErrExpectedIntOrFloat( TTreeHeap *heap )
 {
    lng_Error( h_ErrorOf(heap), "Expected INT or FLOAT value" );
 }

 /*========================================================================*/
 /*
  * Перевод куска дерева после сканера в операцию.
  * Если необходимо, то производится преведение типов операндов.
  * Инициализирует переменную the_type типом после операции.
  */
void ToCodeBin( TTreeHeap      *heap,
                TTree          *p,
                DATA_TYPE      *the_type,
                const TConvert  convert[9],
                int             convCode )
 {
    if( convCode == -1 )
         lng_Error( h_ErrorOf( heap ), "Use type STR in expr" );

    lng_ASSERT( convCode >= 0 && convCode < 9, "ToCodeBin" );
    {
    const TConvert *cc    = &(convert[ convCode ]);
    int             types = cc->m_conv;

    if( cc->m_com == CPU_ERROR_CONVERT_TYPE )
         lng_Error( h_ErrorOf( heap ), "Error convert type" );

    h_TypeSet( p, cc->m_com );

    *the_type  = Int2DATA_TYPE( types / 100 - 1 );

    switch( types % 10 )
    {
    case 1: car(p) = h_NewR0( heap, CPU_CIF, car(p) ); break;
    case 2: car(p) = h_NewR0( heap, CPU_CFI, car(p) ); break;
    case 0: break;
    }

    switch( types / 10 % 10 )
    {
    case 1: cdr(p) = h_NewR0( heap, CPU_CIF, cdr(p) ); break;
    case 2: cdr(p) = h_NewR0( heap, CPU_CFI, cdr(p) ); break;
    case 0: break;
    }
    }
 }

 /*========================================================================*/
 /*
  * Используется только для операций NOT ABS NEG
  */
void UnaConvert( TTreeHeap      *heap,
                 TTree          *p,
                 DATA_TYPE      *the_type,
                 const TConvert  conv[3],
                 DATA_TYPE       parType )
 {
    TError *error = h_ErrorOf( heap );

    switch( parType )
    {
    case T_INT:
    case T_FLOAT:
    case T_VECTOR: break;
    case T_STR:  lng_Error( error, "Use type STR in expr" );
    case T_NONE: lng_Error( error, "Expected expression");
    }

    {
    const TConvert *cc    = &(conv[ DATA_TYPE2Int( parType ) ]);
    int             types = cc->m_conv;

    if( cc->m_com == CPU_ERROR_CONVERT_TYPE )
         lng_Error( error, "Error convert type" );

    h_TypeSet( p, cc->m_com );
    *the_type = Int2DATA_TYPE( types / 10 - 1 );

    switch( types % 10 )
    {
    /*
     * вариант 1 сделан для возможных расширений
     */
    case 1: car(p) = h_NewR0( heap, CPU_CIF, car(p) ); break;
    case 2: car(p) = h_NewR0( heap, CPU_CFI, car(p) ); break;
    }

    }
 }


 /*========================================================================*/
void GetFloatRight( TTreeHeap  *heap,
                    TTree      *p,
                    DATA_TYPE   type,
                    const char *msg )
 {
    switch( type )
    {
    case T_INT:   cdr(p) = h_NewR0( heap, CPU_CIF, cdr(p) ); break;
    case T_FLOAT: break;
    default:      lng_Error( h_ErrorOf( heap ), msg );
    }
 }

 /*========================================================================*/
void GetFloatLeft( TTreeHeap  *heap,
                   TTree      *p,
                   DATA_TYPE   type,
                   const char *msg )
 {
    switch( type )
    {
    case T_INT:   car(p) = h_NewR0( heap, CPU_CIF, car(p) ); break;
    case T_FLOAT: break;
    default:      lng_Error( h_ErrorOf( heap ), msg );
    }
 }

 /*========================================================================*/
void GetIntRight( TTreeHeap  *heap,
                  TTree      *p,
                  DATA_TYPE   type,
                  const char *msg )
 {
    switch( type )
    {
    case T_INT:    break;
    case T_FLOAT:  cdr(p) = h_NewR0( heap, CPU_CFI ,cdr(p) ); break;
    default:
            lng_Error( h_ErrorOf( heap ), msg );
    }
 }

 /*========================================================================*/
void GetIntLeft( TTreeHeap  *heap,
                 TTree      *p,
                 DATA_TYPE   type,
                 const char *msg )
 {
    switch( type )
    {
    case T_INT:    break;
    case T_FLOAT:  car(p) = h_NewR0( heap, CPU_CFI, car(p) ); break;
    default:
            lng_Error( h_ErrorOf( heap ), msg );
    }
 }

 /*========================================================================*/
 /*
  * Простая перекодировка SY_ в CPU_ по таблице
  */
TTree *ReplaceCom( TError        *error,
                   TTree         *p,
                   const TReplLex replLex[],
                   DATA_TYPE     *the_type )
 {
    int      i;
    LEX_TYPE lex = h_TypeOf( p );

    for( i = 0; ; ++i )
    {
         if( replLex[ i ].m_lex ==  LEX_ERROR )
              lng_Error( error, "Unknown Lexem" );

         if( lex == replLex[ i ].m_lex )
         {
              h_TypeSet( p, replLex[ i ].m_op );
              *the_type = replLex[ i ].m_type;
              break;
         }
    }

    return p;
 }

 /*========================================================================*/
 /*
  * Используется при обработке SY_GETFIELDX,...
  * Если требуется производится перекодировка команды к которой
  * применяется взятие поля и операция взятия удаляется.
  */
TTree *tc_ToCodePUSH( TTreeHeap *heap,
                      TTree     *p,
                      DATA_TYPE  type1,
                      LEX_TYPE   pushV,
                      LEX_TYPE   pushA,
                      LEX_TYPE   pushFIELD,
                      DATA_TYPE *the_type )
 {
    TTree * left = car(p);

    ExpectedVec( type1, heap );

    switch( h_TypeOf( left ) )
    {
    case CPU_PUSH_VV:
            p = left;
            h_TypeSet( p, pushV );
            break;

    case CPU_PUSH_AV:
            p = left;
            h_TypeSet( p, pushA );
            break;
    default:
            h_TypeSet( p, pushFIELD );
    }

    *the_type = T_FLOAT;
    return p;
 }

 /*========================================================================*/
TTree *tc_ConvertToCode( TTreeHeap *heap, TTree *p, DATA_TYPE *the_type )
 {
    DATA_TYPE type1, type2;
    int       cvCod;
    LEX_TYPE  lex;
    TError   *error;

    if( p == NIL )
    {
         *the_type = T_NONE;
         return NIL;
    }

    car(p) = tc_ConvertToCode( heap, car(p), &type1 );
    cdr(p) = tc_ConvertToCode( heap, cdr(p), &type2 );

    error = h_ErrorOf( heap );

    if( type1 == T_STR || type2 == T_STR )
         cvCod = -1;
    else cvCod = DATA_TYPE2Int( type2 ) * 3 + DATA_TYPE2Int( type1 );

    *the_type = T_NONE;

    switch( h_TypeOf( p ) )
    {
    case SY_INTCONS:     case SY_FLOATCONS:  case SY_STRINGCONS:
    case SY_OPERATOR:
    case SY_LOOP:        case SY_CYCLE:
    case SY_BREAK:
    case SY_FOR:         case SY_FORD:
    case SY_FUNCTION:
    case SY_CONTINUE:
    case SY_RETURN:
    case SY_PRINT:
            return ReplaceCom( error, p, replLex,the_type );

    case SY_IF:
            GetIntLeft( heap, p, type1, "Expected integer 'IF' expession" );
            h_TypeSet( p, CPU_IF );
            return p;

    case SY_IFELSE:
            GetIntLeft( heap, p, type1, "Expected integer 'IF' expession" );
            h_TypeSet( p, CPU_IFELSE );
            return p;

    case SY_BECOMES:
            if( type1 == T_STR && type2 == T_STR )
            {
                 switch( h_TypeOf( cdr(p) ) )
                 {
                 case CPU_PUSH_VS: h_TypeSet( cdr(p), CPU_POP_VS ); break;
                 case CPU_PUSH_AS: h_TypeSet( cdr(p), CPU_POP_AS ); break;
                 default:
                         lng_ASSERTNQ( "Unknown left expr" );
                 }

                 h_TypeSet( p, CPU_MOVS );
            }
            else
            {
                 ToCodeBin( heap, p, the_type, convMOV, cvCod );

                 switch( h_TypeOf( cdr(p) ) )
                 {
                 case CPU_PUSH_VI: h_TypeSet( cdr(p), CPU_POP_VI ); break;
                 case CPU_PUSH_VF: h_TypeSet( cdr(p), CPU_POP_VF ); break;
                 case CPU_PUSH_VV: h_TypeSet( cdr(p), CPU_POP_VV ); break;

                 case CPU_PUSH_AI: h_TypeSet( cdr(p), CPU_POP_AI ); break;
                 case CPU_PUSH_AF: h_TypeSet( cdr(p), CPU_POP_AF ); break;
                 case CPU_PUSH_AV: h_TypeSet( cdr(p), CPU_POP_AV ); break;

                 case CPU_PUSH_VX: h_TypeSet( cdr(p), CPU_POP_VX ); break;
                 case CPU_PUSH_VY: h_TypeSet( cdr(p), CPU_POP_VY ); break;
                 case CPU_PUSH_VZ: h_TypeSet( cdr(p), CPU_POP_VZ ); break;

                 case CPU_PUSH_AX: h_TypeSet( cdr(p), CPU_POP_AX ); break;
                 case CPU_PUSH_AY: h_TypeSet( cdr(p), CPU_POP_AY ); break;
                 case CPU_PUSH_AZ: h_TypeSet( cdr(p), CPU_POP_AZ ); break;

                 default:
                         lng_Error( error, "Unknown left expr" );
                 }
            }
            return p;


    case SY_VARIABLE:
            /*
             * Если это объявление, то ничего с ним не делаем
             */
            switch( ld_GetNameDef( h_GetDescr(p) ) )
            {
            case DEF_FUNC:
            case DEF_CONST:
                    return p;
            }

            switch( tr_GetVarType( p ) )
            {
            case T_INT:
                    h_TypeSet( p, CPU_PUSH_VI );
                    *the_type  = T_INT;
                    return p;

            case T_FLOAT:
                    h_TypeSet( p, CPU_PUSH_VF );
                    *the_type  = T_FLOAT;
                    return p;

            case T_VECTOR:
                    h_TypeSet( p, CPU_PUSH_VV );
                    *the_type  = T_VECTOR;
                    return p;

            case T_STR:
                    h_TypeSet( p, CPU_PUSH_VS );
                    *the_type = T_STR;
                    return p;

            case T_LABEL:
                    return p;

            case T_NONE:
                    return p;

            default:
                 lng_Error( error, "Unknown type" );
            }

            return NIL;

    case SY_FIELDX:
            return tc_ToCodePUSH( heap, p, type1, CPU_PUSH_VX, CPU_PUSH_AX,
                                  CPU_FIELDX, the_type );
    case SY_FIELDY:
            return tc_ToCodePUSH( heap, p, type1, CPU_PUSH_VY, CPU_PUSH_AY,
                                  CPU_FIELDY, the_type );
    case SY_FIELDZ:
            return tc_ToCodePUSH( heap, p, type1, CPU_PUSH_VZ, CPU_PUSH_AZ,
                                  CPU_FIELDZ, the_type );

    case SY_INT:
            switch( type1 )
            {
            case T_INT:    p = car(p);               break;
            case T_FLOAT:  h_TypeSet( p, CPU_CFI );  break;
            case T_VECTOR: ErrExpectedIntOrFloat( heap ); break;
            }

            *the_type  = T_INT;
            return p;

    case SY_FLOAT:
            switch( type1 )
            {
            case T_INT:    h_TypeSet( p, CPU_CIF );  break;
            case T_FLOAT:  p = car(p);               break;
            case T_VECTOR: ErrExpectedIntOrFloat( heap ); break;
            }
            *the_type  = T_FLOAT;
            return p;

    case  SY_CRG:
            h_TypeSet( p, CPU_MULF );
            cdr(p) = tr_NewPUSHCF( heap, 180.0 / CONST_PI );
            GetFloatLeft( heap, p, type1,
                          "INT or FLOAT is needed to perform"
                          " radian to degree conversion" );
            *the_type = T_FLOAT;
            return p;

    case  SY_CGR:
            h_TypeSet( p, CPU_MULF );
            cdr(p) = tr_NewPUSHCF( heap, CONST_PI / 180.0 );
            GetFloatLeft( heap, p, type1,
                          "INT or FLOAT is needed to perform"
                          " degree to radian conversion" );
            *the_type = T_FLOAT;
            return p;

    case SY_FORASSIGN:
            if( type2 != T_INT )
                 lng_Error( error, "FOR: Expected INT variable" );

            GetIntLeft( heap, p, type1, "Error type in FOR assign" );

            switch( h_TypeOf( cdr(p) ) )
            {
            case CPU_PUSH_VI: h_TypeSet( cdr(p), CPU_POP_VI ); break;
            case CPU_PUSH_AI: lng_Error( error, "Use array in 'FOR'" );
            default: lng_ASSERTNQ( "tc_ConvertToCode: SY_FORASSIGN\n" );
            }

            h_TypeSet( p, CPU_FORASSIGN );
            return p;

    case SY_FORTO:
            h_TypeSet( p, CPU_FORTO );
            GetIntRight( heap, p, type2, "Error type in FOR assign" );
            return p;

    case SY_RETURNI:
            h_TypeSet( p, CPU_RETURNINT );
            GetIntLeft( heap, p, type1, "Function must return INT value" );
            return p;

    case SY_RETURNF:
            h_TypeSet( p, CPU_RETURNFLOAT );
            GetFloatLeft( heap, p, type1,"Function must return FLOAT value" );
            return p;

    case SY_RETURNV:
            h_TypeSet( p, CPU_RETURNVECTOR );
            if( type1 != T_VECTOR )
                 lng_Error( error, "Function must return VECTOR value" );
            return p;

    case SY_LVECTORITEM:
            GetFloatLeft( heap, p, type1, FIELD_VECTOR_ERROR );
            *the_type = T_VECTOR;
            return p;

    case SY_RVECTORITEM:
            GetFloatLeft ( heap, p, type1, FIELD_VECTOR_ERROR );
            GetFloatRight( heap, p, type2, FIELD_VECTOR_ERROR );
            *the_type = T_VECTOR;
            return p;

    case  SY_CALL:
            h_TypeSet( p, CPU_CALL );
            *the_type = tr_GetVarType( car(p) );
            return p;

    case  SY_CALLEXTERN:
            h_TypeSet( p, CPU_CALLEXTERN );
            *the_type = tr_GetVarType( car(p) );
            return p;
    /*----------------------------------------------------------------*/
    case  SY_CALLPARAMI:
            h_TypeSet( p, CPU_CALLPARAMI );
            GetIntLeft( heap, p, type1, "Expected INT parameter" );
            return p;

    case SY_CALLPARAMF:
            h_TypeSet( p, CPU_CALLPARAMF );
            GetFloatLeft( heap, p, type1, "Expected FLOAT parameter" );
            return p;

    case SY_CALLPARAMV:
            h_TypeSet( p, CPU_CALLPARAMV );
            if( type1 != T_VECTOR )
                 lng_Error( error, "Expected VECTOR parameter" );
            return p;

    case SY_CALLPARAMS:
            h_TypeSet( p, CPU_CALLPARAMS );
            if( type1 != T_STR )
                 lng_Error( error, "Expected STR parameter" );
            return p;

    case SY_CALLPARVAR:
            h_TypeSet( p, CPU_CALLPARVAR );
            return p;

    /*----------------------------------------------------------------*/


    case  SY_ARRAY:
            /*
             * По умолчанию приходит уже готовый INT внутри BOUND
             */
			GetIntLeft( heap, p, type1, "Use vector as index" ); /* CHECKME */

            switch( tr_GetVarType( p ) )
            {
            case T_INT:
                    h_TypeSet( p, CPU_PUSH_AI );
                    *the_type = T_INT;
                    break;

            case T_FLOAT:
                    h_TypeSet( p, CPU_PUSH_AF );
                    *the_type = T_FLOAT;
                    break;

            case T_VECTOR:
                    h_TypeSet( p, CPU_PUSH_AV );
                    *the_type = T_VECTOR;
                    break;

            case T_STR:
                    h_TypeSet( p, CPU_PUSH_AS );
                    *the_type = T_STR;
                    break;

            default: lng_ASSERTNQ("tc_ConvertToCode: SY_ARRAY");
            }

            return p;

    case SY_PLUS:   ToCodeBin( heap, p, the_type, convADD, cvCod ); return p;
    case SY_MINUS:  ToCodeBin( heap, p, the_type, convSUB, cvCod ); return p;
    case SY_SMUL:   ToCodeBin( heap, p, the_type, convMUL, cvCod ); return p;
    case SY_SLASH:  ToCodeBin( heap, p, the_type, convDIV, cvCod ); return p;
    case SY_MOD:    ToCodeBin( heap, p, the_type, convMOD, cvCod ); return p;
    case SY_PERSENT:ToCodeBin( heap, p, the_type, convMULV,cvCod ); return p;
    case SY_POWER:  ToCodeBin( heap, p, the_type, convPOW, cvCod ); return p;
    case SY_BOUND:  ToCodeBin( heap, p, the_type, convBND, cvCod ); return p;
    case SY_AND:    ToCodeBin( heap, p, the_type, convAND, cvCod ); return p;
    case SY_OR:     ToCodeBin( heap, p, the_type, convOR,  cvCod ); return p;
    case SY_LEQ:    ToCodeBin( heap, p, the_type, convLEQ, cvCod ); return p;
    case SY_LSS:    ToCodeBin( heap, p, the_type, convLSS, cvCod ); return p;
    case SY_GEQ:    ToCodeBin( heap, p, the_type, convGEQ, cvCod ); return p;
    case SY_GRT:    ToCodeBin( heap, p, the_type, convGRT, cvCod ); return p;
    case SY_EQL:    ToCodeBin( heap, p, the_type, convEQU, cvCod ); return p;
    case SY_NEQ:    ToCodeBin( heap, p, the_type, convNEQ, cvCod ); return p;


    case SY_SIN:    case SY_COS:    case  SY_TAN:
    case SY_ASIN:   case SY_ACOS:   case  SY_ATAN:
    case SY_LOG:    case SY_EXP:
            GetFloatLeft( heap, p, type1,
                          "Expected FLOAT function parameter" );
            *the_type = T_FLOAT;
            lex = LEX_ERROR;

            switch( h_TypeOf( p ) )
            {
            case SY_SIN:  lex = CPU_SIN;  break;
            case SY_COS:  lex = CPU_COS;  break;
            case SY_TAN:  lex = CPU_TAN;  break;
            case SY_ASIN: lex = CPU_ASIN; break;
            case SY_ACOS: lex = CPU_ACOS; break;
            case SY_ATAN: lex = CPU_ATAN; break;
            case SY_LOG:  lex = CPU_LOG;  break;
            case SY_EXP:  lex = CPU_EXP;  break;
            }

            h_TypeSet( p, lex );

            return p;

    case SY_ATAN2:
            GetFloatLeft ( heap, p, type1, "Expected: atan2(float,float)" );
            GetFloatRight( heap, p, type2, "Expected: atan2(float,float)" );
            h_TypeSet( p, CPU_ATAN2 );
            *the_type = T_FLOAT;
            return p;

    case SY_NOT: UnaConvert( heap, p, the_type, unaNOT, type1 ); return p;
    case SY_ABS: UnaConvert( heap, p, the_type, unaABS, type1 ); return p;
    case SY_NEG: UnaConvert( heap, p, the_type, unaNEG, type1 ); return p;

    case SY_CALLPARAM:
            switch( type1 )
            {
            case T_INT:    h_TypeSet( p, CPU_CALLPARAMI ); break;
            case T_FLOAT:  h_TypeSet( p, CPU_CALLPARAMF ); break;
            case T_VECTOR: h_TypeSet( p, CPU_CALLPARAMV ); break;
            case T_STR:    h_TypeSet( p, CPU_CALLPARAMS ); break;
            }
            return p;

    case SY_CALLPARAMEOL:
            h_TypeSet( p, CPU_CALLPARAMEOL );
            return p;

    case SY_RNDI:
            h_TypeSet( p, CPU_RNDI );
            *the_type = T_INT;
            return p;

    case SY_RNDF:
            h_TypeSet( p, CPU_RNDF );
            *the_type = T_FLOAT;
            return p;

    case SY_EMPTY:  return p;
    case SY_EXTERN: return p;
    case SY_DROPSTACK: h_TypeSet( p, CPU_DROPSTACK ); return p;
	} /* switch */

  lng_ASSERTNQ( "Unknown lexem (tc_ConvertToCode)" );
  return NIL;
 }

 /*========================================================================*/

/****************************
 *                          *
 * Sort expression by Const *
 *                          *
 ****************************/


 /*========================================================================*/
TTree *SortExpr( TTree *p )
 {
    TTree    *l, *r;
    LEX_TYPE  lex;

    if( p == NIL )
         return NIL;

    l   = car(p);
    r   = cdr(p);
    lex = h_TypeOf(p);

    if( (l != NIL) &&
       (
          (h_IsSum( p ) && h_IsSum( l )) ||
          (h_IsMul( p ) && h_IsMul( l )) ||
          (lex == CPU_AND  && h_TypeOf( l ) == CPU_AND  ) ||
          (lex == CPU_OR   && h_TypeOf( l ) == CPU_OR   )
       )
      )
    {
         /*
          *     p
          *    / \up(r)
          *   l
          *  / \down
          * ..
          *
          */
         TTree    *down    = cdr(l),
                  *up      = r;

         if( (!h_IsConst( down )) && h_IsConst( up ) )
         {
              LEX_TYPE  lexUp   = h_TypeOf( p ),
                        lexDown = h_TypeOf( l );

              cdr(p)      = down;
              cdr(car(p)) = up;

              h_TypeSet( p, lexDown );
              h_TypeSet( l, lexUp   );

              car(p) = SortExpr( car(p) );
              cdr(p) = SortExpr( cdr(p) );
         }
    }
    else
    if( (h_IsSum( p ) || h_IsMul( p ) || h_IsLog( p )) &&
         h_IsConst( r ) && (!h_IsConst( l )) )
         switch( lex )
         {
         case CPU_ADDI: case CPU_ADDF:
         case CPU_MULI: case CPU_MULF:
         case CPU_AND:  case CPU_OR:
                 car(p) = r;
                 cdr(p) = l;
                 break;

         case CPU_SUBI:
                 r->m_sy.m_val.i = -h_GetConstINT( r );
                 h_TypeSet( p, CPU_ADDI );
                 car(p) = r;
                 cdr(p) = l;
                 break;

         case CPU_SUBF:
                 r->m_sy.m_val.f = -h_GetConstFLOAT( r );
                 h_TypeSet( p, CPU_ADDF );
                 car(p) = r;
                 cdr(p) = l;
                 break;
         }
    else
    {
         car(p) = SortExpr( l );
         cdr(p) = SortExpr( r );
    }

    return p;
 }

 /*========================================================================*/
 /*
  *      +                   +
  *     / \                 / \
  *    a   b               b   c
  *       / \       =>    / \
  *     prl  x0         prl  x0
  *     / \             / \
  *    c   x1          a   x1
  *
  */
void OpenParent( TTree *p, TTree *prl )
 {
    TTree *a, *b, *c;

    a = car(p);
    b = cdr(p);
    c = car(prl);

    cdr(p)   = c;
    car(prl) = a;
    car(p)   = b;
 }

 /*========================================================================*/
 /*
  * Перевод всех выражений одного ранга в левую ветку
  * Неправильная балансировка появляется
  * от большого количества скобок
  */
void  SortMulSum( TTree *p )
 {
    TTree *r;

    if( p == NIL )
         return;

    SortMulSum( car(p) );
    r = cdr(p);

    if( r == NIL )
         return;

    SortMulSum( r );

    if( h_IsSum( p ) && h_IsSum( r ) )
    {
         int    isSub = h_IsSub( p );
         TTree *prl   = r;
         TTree *l     = prl;

         for(;;)
         {
             if( !h_IsSum( l ) )
                  break;

             if( isSub )
                  switch( h_TypeOf( l ) )
                  {
                  case CPU_ADDI: h_TypeSet( l, CPU_SUBI ); break;
                  case CPU_SUBI: h_TypeSet( l, CPU_ADDI ); break;
                  case CPU_ADDF: h_TypeSet( l, CPU_SUBF ); break;
                  case CPU_SUBF: h_TypeSet( l, CPU_ADDF ); break;
                  default: lng_ASSERTNQ("SortMulSum");
                  }

             prl = l;
             l = car(prl);
         }

         OpenParent( p, prl );
    }
    else
    {
         LEX_TYPE lex = h_TypeOf( p );

         if( h_IsMul( p ) ||  lex == CPU_AND || lex == CPU_OR )
         if( h_TypeOf( r ) == lex )
         {
              TTree *prl = r;
              TTree *l;

              for(;;)
              {
                   l = car(prl);

                   if( h_TypeOf( l ) != lex )
                        break;

                   prl = l;
              }

              OpenParent( p, prl );
         }
    }
 }

 /*========================================================================*/

/************************
 *                      *
 * Optimize expression  *
 *                      *
 ************************/

void CalcI( TError   *er,
            TTree    *dest,
            TTree    *op0,
            TTree    *op1,
            LEX_TYPE  com )
 {
    TInt i0, i1, res;

    i0 = h_GetConstINT( op0 );
    i1 = h_GetConstINT( op1 );

    switch( com )
    {
    case CPU_ADDI: res = i0 + i1; break;
    case CPU_SUBI: res = i0 - i1; break;
    case CPU_MULI: res = i0 * i1; break;
    case CPU_DIVI:
            if( i1 == 0 )
                 lng_Error( er, "Division by 0" );

            res = i0 / i1;
            break;

    case CPU_MOD:
            if( i1 == 0 )
                 lng_Error( er, "Division by 0(MOD)" );
            res = i0 % i1;
            break;
    }
    dest->m_sy.m_val.i = res;
 }

 /*========================================================================*/
void CalcF( TError   *er,
            TTree    *dest,
            TTree    *op0,
            TTree    *op1,
            LEX_TYPE  com)
 {
    TFloat f0, f1, res;

    f0 = h_GetConstFLOAT( op0 );
    f1 = h_GetConstFLOAT( op1 );

    switch( com )
    {
    case CPU_ADDF: res = f0 + f1; break;
    case CPU_SUBF: res = f0 - f1; break;
    case CPU_MULF: res = f0 * f1; break;
    case CPU_ATAN2:if( f0*2 == f0 && f1*2 == f1 )
                        res = 0;
                   else res = atan2(f0,f1); break;
    case CPU_DIVF:
            if( f1 == 0.0 )
                 lng_Error( er, "Division by 0.0" );
            else
            if( f1 < 1.0 && CONST_MAXFLOAT * f1 <= f0 )
				 lng_Error( er, "Divide overflow" ); /* CHECKME */

            res = f0 / f1;
            break;
    }

    dest->m_sy.m_val.f = res;
 }


 /*========================================================================*/
void Div0( TError *er, TTree *op1, LEX_TYPE com )
 {
    if( com == CPU_DIVI || com == CPU_DIVF )
    if( h_IsConst( op1 ) )
    if( h_ConstEqu( op1, 0 ) )
         lng_Error( er, "Division by 0" );
 }

 /*========================================================================*/
 /****************************
  * Если выражение сложное, то
  * объявляется переменная и
  * выражение присваивается ей
  * Используется только для
  * значений типа FLOAT
  ****************************/
TTree *ExpToVar( TTreeHeap   *heap,
                 TTree       *p,
                 TTree      **head,
                 TScanner    *scanner,
                 TNameArray  *nameDescr,
                 TNameDefs   *ndefs )
 {
    LEX_TYPE lex = h_TypeOf( p );

    if( (!h_IsAtom( p ))    &&
        lex != CPU_FIELDX &&
        lex != CPU_FIELDY &&
        lex != CPU_FIELDZ )
    {
         const char *unicalName = h_UnicalName(
                                                h_ErrorOf( heap ),
                                                scanner,
                                                nameDescr
                                              );
         TName *descr =  h_AddName( h_ErrorOf( heap ), nameDescr );
         TTree *popVar, *list;

         ld_InitTName( descr,
                       unicalName,
                       DEF_VAR,
                       T_FLOAT );
         h_AddDef( ndefs, descr );
         popVar = h_NewV( heap, descr );
         h_TypeSet( popVar, CPU_POP_VF );


         if( *head == NIL )
              *head = h_New( heap, CPU_OPERATOR,
                             h_New( heap, CPU_MOVF, p, popVar ),
                             *head );
         else
         {
             list = *head;

             while( cdr(list) != NIL )
                 list = cdr(list);

             cdr(list) = h_NewR0( heap, CPU_OPERATOR,
                           h_New( heap, CPU_MOVF, p, popVar ));
         }

         p = h_NewV( heap, descr );
         h_TypeSet( p, CPU_PUSH_VF );
    }

    return p;
 }


 /*========================================================================*/
TTree *UnionVectors( TTreeHeap   *heap,
                     TTree       *p,
                     TTree      **head,
                     TScanner    *scanner,
                     TNameArray  *nameDescr,
                     TNameDefs   *ndefs )
 {
    LEX_TYPE comType, arCom;
    TTree    *l, *r;
    TName    *descr;

    if( p == NIL )
         return NIL;

    car(p) = l = UnionVectors( heap, car(p), head, scanner, nameDescr, ndefs);
    cdr(p) = r = UnionVectors( heap, cdr(p), head, scanner, nameDescr, ndefs);

    comType = h_TypeOf( p );

    switch( comType )
    {
    case CPU_PUSH_VV:
            descr = h_GetDescr(p);
            return
                h_New( heap, SY_LVECTORITEM,
                       h_NewC_V( heap, CPU_PUSH_VX, descr ),
                       h_New  ( heap, SY_RVECTORITEM,
                                h_NewC_V( heap, CPU_PUSH_VY, descr ),
                                h_NewC_V( heap, CPU_PUSH_VZ, descr )));
    case CPU_ABSV:
            if( h_TypeOf( l ) != SY_LVECTORITEM )
                 return p;

            h_TypeSet( p, CPU_POWF );
            cdr(p) = tr_NewPUSHCF( heap, 0.5 );
            car(p) =
                 h_New( heap, CPU_ADDF,
                      h_New( heap, CPU_POWFI,
                             car(l),
                             tr_NewPUSHCI( heap, _TINT(2) )),
                      h_New( heap, CPU_ADDF,
                           h_New( heap, CPU_POWFI,
                                  car(cdr(l)),
                                  tr_NewPUSHCI( heap, _TINT(2) )),
                           h_New( heap, CPU_POWFI,
                                  cdr(cdr(l)),
                                  tr_NewPUSHCI( heap, _TINT(2) ))));
            return p;

    case CPU_ADDV: case CPU_SUBV:
            if( h_TypeOf( r ) != SY_LVECTORITEM ||
                h_TypeOf( l ) != SY_LVECTORITEM )
                 return p;
            /*
             *    +
             *   / \
             *  1   1
             *   \   \
             *    2   2
             *     \   \
             *      3   3
             *
             *
             *
             *   [SY_LVECTORITEM]
             *  / \
             * v   [SY_RVECTORITEM](val)
             *    / \
             *   v   ?(val)
             *
             */
            {
            TTree *p1 = car( l ),
                  *p2 = car(cdr( l )),
                  *p3 = cdr(cdr( l ));

            p = r;

            switch( comType )
            {
            case CPU_ADDV: arCom = CPU_ADDF; break;
            case CPU_SUBV: arCom = CPU_SUBF; break;
            }

            car(p)      = h_New( heap, arCom, p1, car(p));
            car(cdr(p)) = h_New( heap, arCom, p2, car(cdr(p)));
            cdr(cdr(p)) = h_New( heap, arCom, p3, cdr(cdr(p)));
            }
            return p;

    case CPU_MULVV:
            if( h_TypeOf( r ) != SY_LVECTORITEM ||
                h_TypeOf( l ) != SY_LVECTORITEM )
                 return p;
            /*      *
             *     / \
             *    /   \
             *   1     1
             *  / \   / \
             * p   2 d   2
             *    / \   / \
             *   p   p d   d
             */
            {
            TTree *p1 = car( l ),
                  *p2 = car(cdr( l )),
                  *p3 = cdr(cdr( l ));

            TTree *d1 = car( r ),
                  *d2 = car(cdr( r )),
                  *d3 = cdr(cdr( r ));

            return
                h_New( heap, CPU_ADDF,
                     h_New( heap, CPU_MULF, p1, d1 ),
                     h_New( heap, CPU_ADDF,
                          h_New( heap, CPU_MULF, p2, d2 ),
                          h_New( heap, CPU_MULF, p3, d3 )));
            }

    case CPU_MULVVV:
            if( h_TypeOf( r ) != SY_LVECTORITEM ||
                h_TypeOf( l ) != SY_LVECTORITEM )
                 return p;
            {
            TTree *px = car( l ),
                  *py = car(cdr( l )),
                  *pz = cdr(cdr( l ));

            TTree *dx = car( r ),
                  *dy = car(cdr( r )),
                  *dz = cdr(cdr( r ));

            px = ExpToVar( heap, px, head, scanner, nameDescr, ndefs );
            py = ExpToVar( heap, py, head, scanner, nameDescr, ndefs );
            pz = ExpToVar( heap, pz, head, scanner, nameDescr, ndefs );

            dx = ExpToVar( heap, dx, head, scanner, nameDescr, ndefs );
            dy = ExpToVar( heap, dy, head, scanner, nameDescr, ndefs );
            dz = ExpToVar( heap, dz, head, scanner, nameDescr, ndefs );

            return
                h_New( heap, SY_LVECTORITEM,
                     h_New( heap, CPU_SUBF,
                          h_New( heap, CPU_MULF, py, dz ),
                          h_New( heap, CPU_MULF, pz, dy )),
                     h_New( heap, SY_RVECTORITEM,
                          h_New( heap, CPU_SUBF,
                               h_New( heap, CPU_MULF, pz, dx ),
                               h_New( heap, CPU_MULF, px, dz )),
                          h_New( heap, CPU_SUBF,
                               h_New( heap, CPU_MULF, px, dy ),
                               h_New( heap, CPU_MULF, py, dx ))));

            }

    case CPU_EQUV: case CPU_NEQV:
            if( h_TypeOf( r ) != SY_LVECTORITEM ||
                h_TypeOf( l ) != SY_LVECTORITEM )
                 return p;
            {
            TTree *p1 = car( l ),
                  *p2 = car(cdr( l )),
                  *p3 = cdr(cdr( l ));

            TTree *d1 = car( r ),
                  *d2 = car(cdr( r )),
                  *d3 = cdr(cdr( r ));

            switch( comType )
            {
            case CPU_EQUV: arCom = CPU_EQUF; comType = CPU_AND; break;
            case CPU_NEQV: arCom = CPU_NEQF; comType = CPU_OR;  break;
            }
            return
                h_New( heap, comType,
                       h_New( heap, arCom, p1, d1 ),
                       h_New( heap, comType,
                              h_New( heap, arCom, p2, d2 ),
                              h_New( heap, arCom, p3, d3 )));
            }

    case CPU_MULVF:
            car(p) = r;
            cdr(p) = l;
            l = car(p);
            r = cdr(p);
            /* FALLSTHROUGH */
    case CPU_MULFV:
            if( h_TypeOf( l ) != SY_LVECTORITEM )
                 return p;

            r = ExpToVar( heap, r, head, scanner, nameDescr, ndefs );
            car(l)      = h_New( heap, CPU_MULF, car(l), r);
            car(cdr(l)) = h_New( heap, CPU_MULF, car(cdr(l)), r);
            cdr(cdr(l)) = h_New( heap, CPU_MULF, cdr(cdr(l)), r);
            return l;

    case CPU_DIVVF:
            if( h_TypeOf( l ) != SY_LVECTORITEM )
                 return p;

            r = ExpToVar( heap, r, head, scanner, nameDescr, ndefs );
            car(l)      = h_New( heap, CPU_DIVF, car(l), r);
            car(cdr(l)) = h_New( heap, CPU_DIVF, car(cdr(l)), r);
            cdr(cdr(l)) = h_New( heap, CPU_DIVF, cdr(cdr(l)), r);
            return l;

    case CPU_NEGV:
            if( h_TypeOf( car(p) ) != SY_LVECTORITEM )
                 return p;
            car( l )      = h_NewR0( heap,CPU_NEGF,car( l ));
            car(cdr( l )) = h_NewR0( heap,CPU_NEGF,car(cdr( l )));
            cdr(cdr( l )) = h_NewR0( heap,CPU_NEGF,cdr(cdr( l )));
            return car(p);

    case CPU_FIELDX:
            if( h_TypeOf( l ) == SY_LVECTORITEM )
                 return car( l );
            else
            {
				 switch( h_TypeOf( l ) ) /* CHECKME */
				 {
				 case CPU_PUSH_AV:
					  h_TypeSet( l, CPU_POP_AX );
					  return l;
				 }
				 return p;
			}

	case CPU_FIELDY:
			if( h_TypeOf( l ) == SY_LVECTORITEM )
				 return car(cdr( l ));
			else
			{
                 switch( h_TypeOf( l ) ) /* CHECKME */
                 {
                 case CPU_PUSH_AV:
                      h_TypeSet( l, CPU_POP_AY );
                      return l;
                 }
                 return p;
            }

    case CPU_FIELDZ:
            if( h_TypeOf( l ) == SY_LVECTORITEM )
                 return cdr(cdr( l ));
            else
            {
				 switch( h_TypeOf( l ) ) /* CHECKME */
                 {
                 case CPU_PUSH_AV:
                      h_TypeSet( l, CPU_POP_AZ );
                      return l;
                 }
                 return p;
            }
    }
    return p;
 }

 /*========================================================================*/
TTree *UnionVecMOV( TTreeHeap  *heap,
                    TTree      *p,
                    TScanner   *scanner,
                    TNameArray *nameDescr,
                    TNameDefs  *ndefs )
 {
    TTree    *head = NIL;

    if( p == NIL )
         return NIL;

    car(p) = UnionVecMOV( heap, car(p), scanner, nameDescr, ndefs );
    cdr(p) = UnionVecMOV( heap, cdr(p), scanner, nameDescr, ndefs );

    if( h_TypeOf( p ) == CPU_OPERATOR &&
        car(p) != NIL )
    {
         LEX_TYPE comType = h_TypeOf( car(p) );

         if( comType == CPU_MOVI ||
             comType == CPU_MOVF ||
             comType == CPU_MOVV )
         {
              car(p) = UnionVectors( heap, car(p),
                                    &head, scanner, nameDescr, ndefs );

              if( head != NIL )
                   car(p) = h_New( heap, CPU_OPERATOR, head,
                                   h_NewR0( heap, CPU_OPERATOR, car(p) ));
         }
    }
    else
    if( car(p) != NIL )
    {
         LEX_TYPE comType = h_TypeOf( car(p) );

         if( comType == CPU_IF  || comType == CPU_IFELSE ||
             comType == CPU_FOR || comType == CPU_FORD )
         {
              car(car(p)) = UnionVectors( heap, car(car(p)),
                                          &head, scanner, nameDescr, ndefs );

              if( head != NIL )
                   car(p) = h_NewR0( heap, CPU_OPERATOR,
                                     h_New( heap, CPU_OPERATOR, head,
                                     h_NewR0( heap, CPU_OPERATOR, car(p) )));
         }
    }

    return p;
 }


 /*========================================================================*/
TTree *Plus0( TTree *p )
 {
    /*  0+m , m+0*/
    if( h_IsAdd( p ) )
    {
         if( h_IsConst( car(p) ) )
              if( h_ConstEqu( car(p), 0 ) )
                   return cdr(p);

         if( h_IsConst( cdr(p) ) )
              if( h_ConstEqu( cdr(p), 0 ) )
                   return car(p);
    }
    else
    if( h_IsSub( p ) )
    {
         if( h_IsConst( cdr(p) ) )
              if( h_ConstEqu( cdr(p), 0 ) )
                   return car(p);

         if( h_IsConst( car(p) ) )
              if( h_ConstEqu( car(p), 0 ) )
              {
                   switch( h_TypeOf(p) )
                   {
                   case CPU_SUBI: h_TypeSet( p, CPU_NEGI ); break;
                   case CPU_SUBF: h_TypeSet( p, CPU_NEGF ); break;
                   }
                   car(p) = cdr(p);
                   cdr(p) = NIL;
                   return p;
              }
    }

    return NIL;
 }

 /*========================================================================*/
TTree *Mul0( TTree *p )
 {
    if( h_IsMul(p) )
    {
         if( h_IsConst( car(p) ) )
              if( h_ConstEqu( car(p), 0 ) )
                   return car(p);

         if( h_IsConst( cdr(p) ) )
              if( h_ConstEqu( cdr(p), 0 ) )
                   return cdr(p);
    }
    return NIL;
 }

 /*========================================================================*/
TTree *Mul1( TTree *p )
 {
    if( h_IsMul(p) )
    {
         if( h_IsConst(car(p)) )
              if( h_ConstEqu(car(p),1) )
                   return cdr(p);

         if( h_IsConst(cdr(p)) )
              if( h_ConstEqu(cdr(p),1) )
                   return car(p);
    }
    return NIL;
 }

 /*========================================================================*/
TTree *Div1( TTree *p )
 {
    if( h_IsDiv( p ) )
         if( h_IsConst( cdr(p) ) )
              if( h_ConstEqu( cdr(p), 1 ) )
                   return car(p);

    return NIL;
 }

 /*========================================================================*/
 /*
  * Балансировка выражений заключается в преобразовании сравнения в
  * вычитание и сравнение с 0
  */
void CompareSubI( TTreeHeap *heap, TTree *p, LEX_TYPE lex )
 {
    h_TypeSet( p, lex );
    car(p) = h_New( heap, CPU_SUBI, car(p), cdr(p) );
    cdr(p) = NIL;
 }

 /*========================================================================*/
void CompareSubF( TTreeHeap *heap, TTree *p, LEX_TYPE lex )
 {
    h_TypeSet( p, lex );
    car(p) = h_New( heap, CPU_SUBF, car(p), cdr(p) );
    cdr(p) = NIL;
 }

 /*========================================================================*/
TTree *BalanceComp( TTreeHeap *heap, TTree *p )
 {
    if( p == NIL )
         return NIL;

    car(p) = BalanceComp( heap, car(p) );
    cdr(p) = BalanceComp( heap, cdr(p) );

    switch( h_TypeOf( p ) )
    {
    case CPU_LSSI: CompareSubI( heap, p, CPU_LSSI0 ); break;
    case CPU_LEQI: CompareSubI( heap, p, CPU_LEQI0 ); break;
    case CPU_GRTI: CompareSubI( heap, p, CPU_GRTI0 ); break;
    case CPU_GEQI: CompareSubI( heap, p, CPU_GEQI0 ); break;
    case CPU_EQUI: CompareSubI( heap, p, CPU_EQUI0 ); break;
    case CPU_NEQI: CompareSubI( heap, p, CPU_NEQI0 ); break;

    case CPU_LSSF: CompareSubF( heap, p, CPU_LSSF0 ); break;
    case CPU_LEQF: CompareSubF( heap, p, CPU_LEQF0 ); break;
    case CPU_GRTF: CompareSubF( heap, p, CPU_GRTF0 ); break;
    case CPU_GEQF: CompareSubF( heap, p, CPU_GEQF0 ); break;
    case CPU_EQUF: CompareSubF( heap, p, CPU_EQUF0 ); break;
    case CPU_NEQF: CompareSubF( heap, p, CPU_NEQF0 ); break;
    }

    return p;
 }


 /*========================================================================*/
 /*
  * Удаление лишних NEG в умножениях
  */
int SearchMulNeg( TTree *p )
 {
    int      s;
    LEX_TYPE lex;

    if( p == NIL )
         return 0;

    if( !h_IsMulDiv( p ) )
         return 0;

    s = 0;
    lex = h_TypeOf( car(p) );
    if( lex == CPU_NEGI || lex == CPU_NEGF )
         ++s;

    lex = h_TypeOf( cdr(p) );
    if( lex == CPU_NEGI || lex == CPU_NEGF )
         ++s;

    s+= SearchMulNeg( car(p) );
    s+= SearchMulNeg( cdr(p) );
    return s;
 }

 /*========================================================================*/
void DropNeg( TTree *p, LEX_TYPE *comNegType )
 {
    LEX_TYPE lex;

    if( p == NIL )
         return;

    if( !h_IsMulDiv( p ) )
         return;

    lex = h_TypeOf( car(p) );
    if( lex == CPU_NEGI || lex == CPU_NEGF )
    {
        *comNegType = lex;
        car(p) = car(car(p));
    }

    lex = h_TypeOf( cdr(p) );
    if( lex == CPU_NEGI || lex == CPU_NEGF )
    {
        *comNegType = lex;
        cdr(p) = car(cdr(p));
    }

    DropNeg( car(p), comNegType );
    DropNeg( cdr(p), comNegType );
 }

 /*========================================================================*/
TTree *DropMulNeg( TTreeHeap *heap, TTree *p )
 {
    if( p == NIL )
         return NIL;


    if( h_IsMulDiv( p ) )
    {
       int i = SearchMulNeg( p );

       if( i != 0 )
       {
           LEX_TYPE comNegType;

           DropNeg( p, &comNegType );
           if( (i & 1) != 0 )
                p = h_NewR0( heap, comNegType, p );
       }
    }

    car(p) = DropMulNeg( heap, car(p) );
    cdr(p) = DropMulNeg( heap, cdr(p) );

    return p;
 }

 /*========================================================================*/
int TreeEQU( TTree *l, TTree *r )
 {
    LEX_TYPE lex;

    if( l == NIL && r == NIL )
         return 1;

    if( l == NIL )
         return 0;

    if( r == NIL )
         return 0;

    if( !TreeEQU( car(l), car(r) ) )
         return 0;

    if( !TreeEQU( cdr(l), cdr(r) ) )
         return 0;

    lex = h_TypeOf( l );
    if( lex != h_TypeOf( r ) )
         return 0;

    if( lex == CPU_PUSH_CI )
         return h_GetConstINT(l) == h_GetConstINT( l );

    if( lex == CPU_PUSH_CF )
         return h_GetConstFLOAT(l) == h_GetConstFLOAT( l );

    if( lex == CPU_PUSH_VI || 
        lex == CPU_PUSH_VF || 
        lex == CPU_PUSH_VV )
         return str_StrEQU( tr_GetVarName( l ), tr_GetVarName( r ) );

    return 1;
 }

 /*========================================================================*/
TTree *Calc01( TError *error, TTree *p )
 {
    TTree *d;

    if( (d = Plus0( p )) != NIL )
         return d;

    if( (d = Mul0( p )) != NIL )
         return d;

    if( (d = Mul1( p )) != NIL )
         return d;

    if( (d = Div1( p )) != NIL )
         return d;

    Div0( error, cdr(p), h_TypeOf( p ) );

    return NIL;
 }

 /*========================================================================*/
 /*
  * Расчет константных выражений,
  * удаление избыточных опеpаций типа: (--i)
  */
TTree *OptimizeExprLev0(TError *er,TTree *p)
 {
    TFloat v1, v2, sf;
    TInt   i1, i2, s;
    TTree  *d, *l, *r;
    int    i;

    if( p == NIL )
         return NIL;

    car(p) = l = OptimizeExprLev0( er, car(p) );
    cdr(p) = r = OptimizeExprLev0( er, cdr(p) );

    switch( h_TypeOf( p ) )
    {
    /*
     * Unar operations
     */
    case CPU_CIF:
            if( h_TypeOf( l ) == CPU_CIF )
                 car(p) = car(l);

            if( h_IsConstI( car(p) ) )
                 tr_CreateCF( p, h_GetConstINT( car(p) ));

            break;

    case CPU_CFI:
            if( h_TypeOf( l ) == CPU_CFI )
                  car(p) = car(l);

            if( h_TypeOf( car(p) ) == CPU_CIF )
            {
                 p = car(car(p));
                 break;
            }

            if( h_IsConstF( car(p) ) )
                 tr_CreateCI( p, (TInt)h_GetConstFLOAT( car(p) ));
            break;

    case CPU_NEGI:
            if( h_IsConstI( l ) )
                 tr_CreateCI( p, -h_GetConstINT( l ) );

            if( h_TypeOf( l ) == CPU_NEGI )
                 p = car(l);
            break;

    case CPU_NEGF:
            if( h_IsConstF( l ) )
                 tr_CreateCF(p, -h_GetConstFLOAT( l ) );
            else
            if( h_TypeOf( l ) == CPU_NEGF )
                 p = car(l);
            break;

    /*
     * Binar operations
     */
    case CPU_ADDF:
    case CPU_SUBF:
    case CPU_MULF:
    case CPU_DIVF:
    case CPU_ATAN2:
            if( (d = Calc01( er, p )) != NIL )
            {
                 p = d;
                 break;
            }

            if( h_IsConst( l ) && h_IsConst( r ) )
            {
                 CalcF( er, p, l, r, h_TypeOf( p ) );
                 h_TypeSet( p, CPU_PUSH_CF );
                 h_MkAtom( p );
            }
            break;

    case CPU_ADDI:
    case CPU_SUBI:
    case CPU_MULI:
    case CPU_DIVI:
    case CPU_MOD:
            if( (d = Calc01( er, p )) != NIL )
            {
                 p = d;
                 break;
            }

            if( h_IsConstI( l ) && h_IsConstI( r ) )
            {
                 CalcI( er, p, l, r, h_TypeOf( p ) );
                 h_TypeSet( p, CPU_PUSH_CI );
                 h_MkAtom( p );
            }
            break;

    case CPU_POWI:
            if( h_IsConstI( l ) &&
                h_IsConstI( r ) )
            {
                 i1 = h_GetConstINT( l );
                 i2 = h_GetConstINT( r );
                 s  = i1;

                 for( i = 1 ; (i < i2) ; i++ )
                      s *= i1;

                 tr_CreateCI( p, s );
            }
            break;

    case CPU_POWFI:
            if( h_IsConstF( l ) &&
                h_IsConstI( r ) )
            {
                 v1 = h_GetConstFLOAT( l );
                 i2 = h_GetConstINT( r );
                 sf = v1;

                 for( i = 1 ; (i < i2) ; i++ )
                      sf *= v1;

                 tr_CreateCF( p, sf );
            }
            break;

    case CPU_POWF:
            if( h_IsConstF( l ) &&
                h_IsConstF( r ) )
            {
                 TFloat vf;
                 v1 = h_GetConstFLOAT( l );
                 v2 = h_GetConstFLOAT( r );

                 if( v1 == 0 )
                      vf = 0;
                 else vf = exp( log(v1) * v2 );

                    tr_CreateCF( p, vf );
            }
            break;
    /*
     * functions
     */
    case CPU_SIN:    case CPU_COS:    case CPU_TAN:
    case CPU_ASIN:   case CPU_ACOS:   case CPU_ATAN:
    case CPU_LOG:    case CPU_EXP:    case CPU_ABSF:
            if( h_IsConstF( l ) )
            {
                 v1 = h_GetConstFLOAT( l );

                 switch( h_TypeOf( p ) )
                 {
                 case CPU_SIN:  v1 = sin  ( v1 ); break;
                 case CPU_COS:  v1 = cos  ( v1 ); break;
                 case CPU_TAN:  v1 = tan  ( v1 ); break;
                 case CPU_ASIN: v1 = asin ( v1 ); break;
                 case CPU_ACOS: v1 = acos ( v1 ); break;
                 case CPU_ATAN: v1 = atan ( v1 ); break;
                 case CPU_LOG:  v1 = log  ( v1 ); break;
                 case CPU_EXP:  v1 = exp  ( v1 ); break;
                 case CPU_ABSF: v1 = fabs ( v1 ); break;
                 }

                 tr_CreateCF( p, v1 );
            }
            break;

    case CPU_ABSI:
         if( h_IsConstI( l ) )
         {
             TInt v = h_GetConstINT( l );

             if( v < 0 )
                  v = -v;
             tr_CreateCI(p, v);
         }
         break;

    case CPU_BOUND:
         if( h_IsConst( l ) && h_IsConst( r ) )
         {
            TInt index = h_GetConstINT( l );

            if( index >= h_GetConstINT( r ) || index < 0 )
                 lng_Error( er, "Range check error [%d]", index );

            p = l;
            break;
         }

         if( h_TypeOf(l) == CPU_BOUND )
              if( TreeEQU( r, cdr(l) ) )
                   p = l;

         break;

    case CPU_LSSI0:
    case CPU_LEQI0:
    case CPU_GRTI0:
    case CPU_GEQI0:
    case CPU_EQUI0:
    case CPU_NEQI0:
         if( h_IsConst( l ) )
         {
              TInt   v  = h_GetConstINT( l );
              int    res;

              switch( h_TypeOf( p ) )
              {
              case CPU_LSSI0: res = v <  0; break;
              case CPU_LEQI0: res = v <= 0; break;
              case CPU_GRTI0: res = v >  0; break;
              case CPU_GEQI0: res = v >= 0; break;
              case CPU_EQUI0: res = v == 0; break;
              case CPU_NEQI0: res = v != 0; break;
              }

              tr_CreateCI( p, _TINT(res) );
         }
         break;

    case CPU_LSSF0:
    case CPU_LEQF0:
    case CPU_GRTF0:
    case CPU_GEQF0:
    case CPU_EQUF0:
    case CPU_NEQF0:
         if( h_IsConst( l ) )
         {
             TFloat  v  = h_GetConstFLOAT( l );
             int     res;

             switch( h_TypeOf( p ) )
             {
             case CPU_LSSF0: res = v <  0.; break;
             case CPU_LEQF0: res = v <= 0.; break;
             case CPU_GRTF0: res = v >  0.; break;
             case CPU_GEQF0: res = v >= 0.; break;
             case CPU_EQUF0: res = v == 0.; break;
             case CPU_NEQF0: res = v != 0.; break;
             }
             tr_CreateCI( p, _TINT(res) );
         }
         break;

    case CPU_AND:
         if( h_IsConst( l ) && h_IsConst( r ) )
              tr_CreateCI( p, _TINT( h_GetConstINT( l ) &&
                                     h_GetConstINT( r )) );
         else
         if( h_IsConst( l ) )
              if( h_GetConstINT( l ) != 0 )
                   p = r;
              else tr_CreateCI( p, _TINT(0) );

         break;

    case CPU_OR:
         if( h_IsConst( l ) && h_IsConst( r ) )
             tr_CreateCI( p, _TINT( h_GetConstINT( l ) ||
                                    h_GetConstINT( r ) ) );
         else
         if( h_IsConst( l ) )
             if( h_GetConstINT( l ) == 0 )
                  p = r;
             else tr_CreateCI( p, _TINT(1) );

         break;

    case CPU_NOT:
         if( h_IsConst( l ) )
             tr_CreateCI( p, _TINT(h_GetConstINT( l ) == 0 ) );
         break;
    }

    return p;
 }

 /*========================================================================*/
TTree *CreateFORAssignFunc( TTreeHeap  *heap,
                            TTree      *p,
                            TScanner   *scanner,
                            TNameArray *nameDescr,
                            TNameDefs  *ndefs )
 {
    if( p == NIL )
         return NIL;

    car(p) = CreateFORAssignFunc( heap, car(p), scanner, nameDescr, ndefs );
    cdr(p) = CreateFORAssignFunc( heap, cdr(p), scanner, nameDescr, ndefs );

    if( h_TypeOf( p ) == CPU_FORTO )
    {
         const char *unicalName = h_UnicalName(
                                                h_ErrorOf( heap ),
                                                scanner,
                                                nameDescr
                                              );
         TName *descr =  h_AddName( h_ErrorOf( heap ), nameDescr );
         TTree *popVar;

         ld_InitTName( descr,
                       unicalName,
                       DEF_VAR,
                       T_INT );
         h_AddDef( ndefs, descr );

         popVar = h_NewV( heap, descr );
         h_TypeSet( popVar, CPU_POP_VI );

         cdr(p) = h_New( heap, CPU_MOVI, cdr(p), popVar );
    }

    return p;
 }

 /*========================================================================*/
void CreateFORAssign( TTreeHeap  *heap,
                      TTree      *p,
                      TScanner   *scanner,
                      TNameArray *nameDescr,
                      TNameDefs  *ndefs )
 {
    while( p != NIL )
    {
        if( h_TypeOf( p ) == CPU_FUNCTION )
        {
             TName *descr;

             descr = h_GetDescr(car(p));
             ndefs->m_local = descr;

             while( ld_GetNext( descr ) != NIL )
                  descr = ld_GetNext( descr );

             ndefs->m_lastDef = descr;
             car(p) = CreateFORAssignFunc( heap, car(p),
                                           scanner, nameDescr, ndefs );
        }

        p = cdr(p);
    }
 }

 /*========================================================================*/
 /*
  * Сравниваем предложенную переменную p1 с переменной var
  * если они идентичны возвращаем 1.
  * Функция имеет два режима запуска - для эквивалентности
  * при чтении и записи в переменную.
  * Дело в том, что если переменная - это элемент массива и индекс
  * не константа, то ма не имеем права вместо переменной подставлять
  * константу и возвращаем 0, как не эквивалентность.
  * Если мы проверяем на факт изменения, то считаем их эквивалентыми,
  * т.е. переменная сменила свое значение.
  */
int EquVariable( TTree *var, TTree *p1, int forPush )
 {
    TName *descr0, *descr1;
    int    i;
    LEX_TYPE p1type = h_TypeOf( p1 );

    if(    p1type != CPU_PUSH_VI
        && p1type != CPU_POP_VI
        && p1type != CPU_PUSH_AI
        && p1type != CPU_POP_AI

        && p1type != CPU_PUSH_VF
        && p1type != CPU_POP_VF
        && p1type != CPU_PUSH_AF
        && p1type != CPU_POP_AF

        && p1type != CPU_PUSH_VV
        && p1type != CPU_POP_VV
        && p1type != CPU_PUSH_AV
        && p1type != CPU_POP_AV )

         return 0;

    descr0 = h_GetDescr( var );
    descr1 = h_GetDescr( p1  );

    if( !str_StrEQU( ld_GetName( descr0 ), ld_GetName( descr1 ) ) )
         return 0;

    if( ld_GetNameDef( descr0 )  != ld_GetNameDef( descr1 ) )
         return 0;

    if( ld_GetArrayCnt( descr0 ) != ld_GetArrayCnt( descr1 ) )
         return 0;

    for( i = 0; i < ld_GetArrayCnt( descr0 ); ++i )
         if( ld_GetArrayRange( descr0, i ) != ld_GetArrayRange( descr1, i ))
              return 0;

    if( ld_GetArrayCnt( descr0 ) != 0 )
    {
        if(   (!h_IsConstI( car(var) ))
           || (!h_IsConstI( car(p1) )) )
        {
             if( forPush )
                  return 0;
             else return 1;
        }
        else return h_GetConstINT( car(var) ) == h_GetConstINT( car(p1) );
    }

    return 1;
 }

 /*========================================================================*/
 /*
  * Проверяем была ли изменена переменная в пределах указанного блока.
  * Если это элемент массива, то считается, что переменная всегда
  * изменяется
  */
int BeenChange( TTree *p, TTree *var )
 {
    if( p == NIL )
         return 0;

    if(    h_TypeOf( p ) == CPU_MOVI
        && EquVariable( var, cdr(p), 0 ) )
         return 1;

    if(    h_TypeOf( p ) == CPU_MOVF
        && EquVariable( var, cdr(p), 0 ) )
         return 1;

    if( (h_TypeOf( p ) == CPU_FOR || h_TypeOf( p ) == CPU_FORD) )
    if( EquVariable( var, cdr(car(car(p))), 0 ) )
         return 1;

    if(    h_TypeOf( p ) == CPU_CALLPARVAR
        && EquVariable( var, car(p), 0 ) )
         return 1;

    if( BeenChange( car(p), var ) )
         return 1;

    return BeenChange( cdr(p), var );
 }

 /*========================================================================*/
void ChangeInt( TTree *p, TTree *var, TInt val, int *repeat )
 {
    TTree *l;

    if( p == NIL )
         return;

    l = car(p);

    if( l != NIL )
    {
         LEX_TYPE lex = h_TypeOf( l );

         if(   lex == CPU_IF
            || lex == CPU_IFELSE )
              ChangeInt( car(l), var, val, repeat );

         l = car(l);

         if( l != NIL )
         {
              lex = h_TypeOf( l );

              if(   lex == CPU_FOR
                 || lex == CPU_FORD )
              {
                   l = car(l);
                   ChangeInt( car(cdr(l)), var, val, repeat );
                   ChangeInt( car(car(l)), var, val, repeat );
              }
         }
    }

    if( BeenChange( car(p), var ) )
    {
         if( h_TypeOf( car(p) ) == CPU_MOVI )
              ChangeInt( car(car(p)), var, val, repeat );
         return;
    }

    if( (   h_TypeOf( p ) == CPU_PUSH_VI
         || h_TypeOf( p ) == CPU_PUSH_AI)
        && EquVariable( var, p, 1 ) )
    {
         h_TypeSet( p, CPU_PUSH_CI );
         p->m_sy.m_val.i = val;
         *repeat = 1;
    }

    ChangeInt( car(p), var, val, repeat );
    ChangeInt( cdr(p), var, val, repeat );
 }

 /*========================================================================*/
void ChangeFloat( TTree *p, TTree *var, TFloat val, int *repeat )
 {
    if( p == NIL )
         return;

    if(  car(p) != NIL &&
        (   h_TypeOf( car(p) ) == CPU_IF
         || h_TypeOf( car(p) ) == CPU_IFELSE ))
         ChangeFloat( car(car(p)), var, val, repeat );

    if( BeenChange( car(p), var ) )
    {
         if( h_TypeOf( car(p) ) == CPU_MOVF )
              ChangeFloat( car(car(p)), var, val, repeat );

         return;
    }

    if( (   h_TypeOf( p ) == CPU_PUSH_VF
         || h_TypeOf( p ) == CPU_PUSH_AF )
        && EquVariable( var, p, 1 ) )
    {
         h_TypeSet( p, CPU_PUSH_CF );
         p->m_sy.m_val.f = val;
         *repeat = 1;
    }

    ChangeFloat( car(p), var, val, repeat );
    ChangeFloat( cdr(p), var, val, repeat );
 }

 /*========================================================================*/
 /*
  * Рассматриваются значения только в пределах блока, т.е.
  * если после присвоения идет список, то мы анализируем только
  * до конца списка, опускаясь вниз по рекурсии
  * CPU_OPERATOR - CPU_OPERATOR
  * |
  */
void OptimizeConstVarFunc( TTree *p, int *repeat )
 {
    TTree *l;

    if( p == NIL )
         return;

    l = car(p);

    if(    l != NIL
        && h_TypeOf( l ) == CPU_MOVI
        && h_IsConstI( car(l) ) )
         ChangeInt( cdr(p), cdr(l), h_GetConstINT( car(l) ), repeat );

    if(    l != NIL
        && h_TypeOf( l ) == CPU_MOVF
        && h_IsConstF( car(l) ) )
         ChangeFloat( cdr(p), cdr(l), h_GetConstFLOAT( car(l) ), repeat );

    OptimizeConstVarFunc( car(p), repeat );
    OptimizeConstVarFunc( cdr(p), repeat );
 }

 /*========================================================================*/
 /*
  * Если было присвоение переменной константы,
  * то, пока переменная не сменила своего значения,
  * вместо нее подставляется константа.
  *
  * Операция повторяется до тех пор, пока фиксируются
  * изменения
  */
void OptimizeConstVar( TTree *p )
 {
    TTree *prog;
    int    repeat;

    do
    {
         repeat = 0;
         prog   = p;

         while( prog != NIL )
         {
             if( h_TypeOf( prog ) == CPU_FUNCTION )
                  OptimizeConstVarFunc( car(prog), &repeat );

             prog = cdr(prog);
         }
    }
    while( repeat );
 }

 /*========================================================================*/
void DropConstIFFunc( TTree *p )
 {
    TTree *l;

    if( p == NIL )
        return;

    l = car(p);

    if( l != NIL )
    switch( h_TypeOf( l ) )
    {
    case CPU_IF:
            if( h_IsConstI( car(l) ) )
                 if( h_GetConstINT( car(l) ) )
                      car(p) = cdr(l);
                 else car(p) = NIL;
            break;

    case CPU_IFELSE:
            if( h_IsConstI( car(l) ) )
                 if( h_GetConstINT( car(l) ) )
                      car(p) = car(cdr(l));
                 else car(p) = cdr(cdr(l));
            break;
    }

    DropConstIFFunc( car(p) );
    DropConstIFFunc( cdr(p) );
 }

 /*========================================================================*/
void DropConstIF( TTree *p )
 {
    TTree *prog;

    prog   = p;

    while( prog != NIL )
    {
        if( h_TypeOf( prog ) == CPU_FUNCTION )
             DropConstIFFunc( car(prog) );

        prog = cdr(prog);
    }
 }

 /*========================================================================*/
void OptimizeADDCONSTFunc( TTree *p )
 {
     if( p == NIL )
          return;

     if( h_TypeOf( p ) == CPU_MOVI )
     {
          if( h_TypeOf( car(p) ) == CPU_ADDI &&
              h_IsConstI( car(car(p)) ) &&
              EquVariable( cdr(p), cdr(car(p)), 1 ))
          {
              TName *descr = h_GetDescr(cdr(p));

              if( ld_GetArrayCnt( descr ) == 0 &&
                  ( ld_GetNameDef ( descr ) == DEF_VAR ||
                    ld_GetNameDef ( descr ) == DEF_PAR ) )
              {
                   h_TypeSet( p, CPU_ADD_VCONSTI );
                   car(p) = car(car(p));
              }
          }
     }

     OptimizeADDCONSTFunc( car(p) );
     OptimizeADDCONSTFunc( cdr(p) );
 }

 /*========================================================================*/
 /* Оптимизация :
  *  var := const+var;
  * Реализована только для простых переменных типа INT
  */
void OptimizeADDCONST( TTree *p )
 {
    TTree *prog;

    prog   = p;

    while( prog != NIL )
    {
        if( h_TypeOf( prog ) == CPU_FUNCTION )
             OptimizeADDCONSTFunc( car(prog) );

        prog = cdr(prog);
    }
 }

 /*========================================================================*/
int BeenUse( TTree *p, TTree *var )
 {
    if( p == NIL )
         return 0;

    if( EquVariable( var, p, 1 ) )
         return 1;

    if( BeenUse( car(p), var ) )
         return 1;

    return BeenUse( cdr(p), var );
 }

 /*========================================================================*/
void DropReloadVarFunc( TTree *p )
 {
    LEX_TYPE  lex;

    if( p == NIL )
         return;

    if( car(p) == NIL )
         return;

    lex = h_TypeOf( car(p) );

    if( lex == CPU_MOVI ||
        lex == CPU_MOVF )
    {
         TTree    *prog, *var;
         int       changeFlag;

         var = cdr(car(p));
         /*
          * Рассматриваем список операторов после присвоения
          */
         prog   = cdr(p);

         /*
          * Массивы пока (FIXME) оптимизировать не будем
          */
         if( ld_GetArrayCnt( h_GetDescr( var ) ) == 0 )
         {
              changeFlag = 0;

              /*
               * Идем по списку операторов и ищем перезагрузку переменной
               */
              while( prog != NIL )
              {
                  TTree *l = car(prog);

                  if( l != NIL )
                  {
                       lex = h_TypeOf( l );

                       if( lex == CPU_MOVI ||
                           lex == CPU_MOVF )
                       {
                            if( BeenUse( car(l), var ) )
                                 break;

                            if( car(cdr(l)) != NIL )
                            if( BeenUse( car(cdr(l)), var ) )
                                 break;

                            changeFlag = BeenUse( cdr(l), var );

                            if( changeFlag )
                                 break;
                       }
                       else if( BeenUse( l, var ) )
                                 break;
                  }

                  prog = cdr(prog);
              }

              if( changeFlag )
                   car(p) = NIL;
         }
    }

    DropReloadVarFunc( car(p) );
    DropReloadVarFunc( cdr(p) );
 }

 /*========================================================================*/
 /*
  * Удаление перезагрузки переменной в пределах
  * линейного участка кода
  */
void DropReloadVar( TTree *p )
 {
    TTree *prog;

    prog   = p;

    while( prog != NIL )
    {
        if( h_TypeOf( prog ) == CPU_FUNCTION )
             DropReloadVarFunc( car(prog) );

        prog = cdr(prog);
    }
 }

 /*========================================================================*/

int BeenPush( TTree *var, TTree *p )
 {
    LEX_TYPE lex;

    if( p == NIL )
         return 0;

    lex = h_TypeOf( p );

    /*
     * Отличаем факты изменения переменной от
     * использования на чтение.
     * При встрече в for, как ссылка, считаем,
     * что переменная используется на чтение
     */
    if( lex == CPU_MOVI ||
        lex == CPU_MOVF ||
        lex == CPU_MOVV )
         return BeenPush( var, car(p) );

    if( var != p && EquVariable( var, p, 0 ) )
         return 1;

    return BeenPush( var, car(p) ) ||
           BeenPush( var, cdr(p) );
 }

 /*========================================================================*/

void DropAssignmentButNeverUsedFunc( TTree *p, TTree *func )
 {
    TTree    *l;
    LEX_TYPE  lex;

    if( p == NIL )
         return;

    l = car(p);

    if( l != NIL )
    {
         lex = h_TypeOf( l );

         if( lex == CPU_MOVI ||
             lex == CPU_MOVF ||
             lex == CPU_MOVV )
         {
              TTree *var = cdr(l);
              TName *descr = h_GetDescr( var );

              if( ld_GetNameDef( descr ) != DEF_PARVAR )
              if( !BeenPush( var, func ) )
                  car(p) = NIL;
         }
    }

    DropAssignmentButNeverUsedFunc( car(p), func );
    DropAssignmentButNeverUsedFunc( cdr(p), func );
 }

 /*========================================================================*/

void DropAssignmentButNeverUsed( TTree *p )
 {
    TTree *prog;

    prog   = p;

    while( prog != NIL )
    {
        if( h_TypeOf( prog ) == CPU_FUNCTION )
             DropAssignmentButNeverUsedFunc( car(p), car(p) );

        prog = cdr(prog);
    }
 }

 /*========================================================================*/
void CreateADD_VARFunc( TTree *p )
 {
    TName *descr;

    if( p == NIL )
         return;


    switch( h_TypeOf( p ) )
    {
    case CPU_ADDI:
    case CPU_ADDF:
    case CPU_SUBI:
    case CPU_SUBF:

    case CPU_MULI:
    case CPU_MULF:
    case CPU_DIVI:
    case CPU_DIVF:
    case CPU_MOD:
            switch( h_TypeOf( cdr(p) ) )
            {
            case CPU_PUSH_VI:
            case CPU_PUSH_VF:
                    descr = h_GetDescr( cdr(p) );
                    if( ld_GetArrayCnt( descr ) == 0 &&
                        ( ld_GetNameDef( descr ) == DEF_VAR ||
                          ld_GetNameDef( descr ) == DEF_PAR)  )
                    {
                         switch( h_TypeOf( p ) )
                         {
                         case CPU_ADDI: h_TypeSet( p, CPU_ADD_VARI ); break;
                         case CPU_ADDF: h_TypeSet( p, CPU_ADD_VARF ); break;
                         case CPU_SUBI: h_TypeSet( p, CPU_SUB_VARI ); break;
                         case CPU_SUBF: h_TypeSet( p, CPU_SUB_VARF ); break;

                         case CPU_MULI: h_TypeSet( p, CPU_MUL_VARI ); break;
                         case CPU_MULF: h_TypeSet( p, CPU_MUL_VARF ); break;
                         case CPU_DIVI: h_TypeSet( p, CPU_DIV_VARI ); break;
                         case CPU_DIVF: h_TypeSet( p, CPU_DIV_VARF ); break;
                         case CPU_MOD:  h_TypeSet( p, CPU_MOD_VAR  ); break;
                         }
                    }
            }
            break;
    }

    CreateADD_VARFunc( car(p) );
    CreateADD_VARFunc( cdr(p) );
 }

 /*========================================================================*/
 /*
  * ADD_VARTOSTACK
  */
void CreateADD_VAR( TTree *p )
 {
    TTree *prog;

    prog   = p;

    while( prog != NIL )
    {
        if( h_TypeOf( prog ) == CPU_FUNCTION )
             CreateADD_VARFunc( car(prog) );

        prog = cdr(prog);
    }
 }



 /*========================================================================*/
TTree *tc_OptimizeExprLev1( TTreeHeap  *heap,
                            TTree      *p,
                            TScanner   *scanner,
                            TNameArray *nameDescr,
                            TNameDefs  *ndefs )
 {
    int i;

    //g_heap = heap;

    /*
     * Предварительная обработка потока
     */
    CreateFORAssign( heap, p, scanner, nameDescr, ndefs );


    SortMulSum( p );
    p = SortExpr( p );
    p = OptimizeExprLev0( h_ErrorOf( heap ), p );
    OptimizeConstVar( p );

    p = UnionVecMOV( heap, p, scanner, nameDescr, ndefs );
    p = BalanceComp( heap, p );
    p = DropMulNeg( heap, p );

    /*
     * Прогон всех оптимизаций два раза для того,
     * чтобы учавствовали все комбинации оптимизаций
     */
    for( i = 0 ; i < 2 ; ++i )
    {
         OptimizeConstVar( p );
         SortMulSum( p );
         p = SortExpr( p );
         p = OptimizeExprLev0( h_ErrorOf( heap ), p );
         DropConstIF( p );
         DropReloadVar( p );
    }

    /*
     * Порядок важен
     */
    OptimizeADDCONST( p );
    CreateADD_VAR( p );
	DropAssignmentButNeverUsed( p );

    return p;

 }

/* End of TR_CODE.C */

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
#if 0

typedef struct{        /* описание структуры текста для выводя листинга */
    char   *m_outStr;  /* Указатель на буфер вывода                     */
    int     m_pos;     /* Текущая позиция для записи                    */
    int     m_maxPos;  /* Длина буфера                                  */
    TError *m_error;   /* Указатель на стуктуру для обработчика ошибки  */
} TOutStr;

void ts_Def2Str( TTree *p, TOutStr *str );
void ts_Tree2Str( TTree *p, TOutStr *str );


/**************************
 *                        *
 *    Convert to str      *
 *                        *
 **************************/

void ts_InitTOutStr( TOutStr *outStr, TError *er, char *buf, int bufSize )
 {
    buf[0] = 0;
    outStr->m_outStr = buf;
    outStr->m_maxPos = bufSize-1;
    outStr->m_error  = er;
    outStr->m_pos    = 0;
 }

 /*========================================================================*/
static
void Append( TOutStr *str, const char *sour )
 {
    for(; *sour!=0 ; ++sour )
    {
         if( str->m_pos >= str->m_maxPos )
              lng_Error( str->m_error, "Output string bugffer overflow" );

         str->m_outStr[ str->m_pos ] = *sour;
         ++(str->m_pos);
    }

    str->m_outStr[ str->m_pos ] = 0;
 }

 /*========================================================================*/
static
void AppendRParent( TOutStr *str )
 {
    Append( str, ")" );
 }

 /*========================================================================*/
static
void AppendLParent( TOutStr *str )
 {
    Append( str, "(" );
 }

 /*========================================================================*/
#define RPRES 10000
  /*
   * Вывод числа с оптимизацией на количество значимых цифр
   */
static
void PrintfLF( char *buf, double d )
 {
    double m, n;
    int    e, i, sig = 0;
    long   hp;

    if( d == 0.0 )
    {
         sprintf( buf, "0." );
         return;
    }

    if( d<0 )
    {
         sig = 1;
         d   = -d;
    }

    e = (int)(log10(d));
    if( e>0 )
         m = d/pow(10.,e);
    else m = d;

    n = m*RPRES;
    hp = (long)(n+0.5);

    if( fabs(hp-n) < 1e-10 )
    {
        long mm = (long)(m*RPRES+0.5);
        if( sig )
             sprintf( buf, "-%ld.%ld", mm/RPRES, mm%RPRES );
        else sprintf( buf, "%ld.%ld",  mm/RPRES, mm%RPRES );
    }
    else
        if( sig )
             sprintf( buf, "-%1.18lf", m );
        else sprintf( buf, "%1.18lf",  m );

    for( i=0; buf[i+1] != 0 ; ++i );
    for(    ; i >= 0 && buf[i] == '0' ; --i );
    buf[i+1] = 0;
    if( e != 0 )
         sprintf( &(buf[i+1]), "e%i", e );
 }

 /*========================================================================*/
void Binar2Str( TTree *p, TOutStr *str, const char *op )
 {
    ts_Def2Str( car(p), str );
    Append( str, op );
    ts_Def2Str( cdr(p), str );
 }

 /*========================================================================*/
void Tree2StrWithP( TTree *p, TOutStr *str )
 {
    if( h_IsAtom(p) || h_IsFunc(p) )
         ts_Def2Str( p, str );
    else
    {
         AppendLParent( str );
         ts_Def2Str( p, str );
         AppendRParent( str );
    }
 }

 /*========================================================================*/
void Binar2StrP( TTree *p, TOutStr *str, const char *op )
 {
    Tree2StrWithP( car(p), str );
    Append( str, op );
    Tree2StrWithP( cdr(p), str );
 }

 /*========================================================================*/
void ts_Def2Str( TTree *p, TOutStr *str )
 {
    char   buf[256];
    TName *descr;

    if( p == NIL )
         return;

    switch( h_TypeOf( p ) )
    {
    case SY_VARIABLE:
            Append( str, "CONST " );

            switch( h_TypeOf( car(p) ) )
            {
            case CPU_PUSH_CI: Append( str, "INT " ); break;
            case CPU_PUSH_CF: Append( str, "FLOAT " ); break;
            }

            Append( str, tr_GetVarName( p ) );
            Append( str, " = " );
            ts_Def2Str( car(p), str );
            Append( str, " ;\n" );
            break;

    case CPU_PUSH_VI:
    case CPU_PUSH_VF:
    case CPU_PUSH_VS:
    case CPU_PUSH_VV:

    case CPU_POP_VI:
    case CPU_POP_VF:
    case CPU_POP_VS:
    case CPU_POP_VV:
    case CPU_CALLPARVAR:
            Append( str, tr_GetVarName( p ) );
            break;

    case CPU_PUSH_CI:
            sprintf( buf, "%i", h_GetConstINT( p ) );
            Append( str, buf );
            break;

    case CPU_PUSH_CF:
            PrintfLF( buf, h_GetConstFLOAT( p ) );
            Append( str, buf );
            break;

    case CPU_PUSH_CS:
            Append( str, "\"" );
            Append( str, h_GetConstSTR( p ) );
            Append( str, "\"" );
            break;

    case CPU_OPERATOR:
            if( car(p) != NIL && h_TypeOf( car(p) ) == CPU_OPERATOR )
            {
                 TTree *list = car(p);
                 Append( str, "{\n" );
                 while( list != NIL )
                 {
                     ts_Def2Str( car(list), str );
                     list = cdr(list);
                 }

                 Append( str, "\n}\n" );
            }
            else
            {
                 ts_Def2Str( car(p), str );
                 ts_Def2Str( cdr(p), str );
                 Append( str, "\n" );
            }

            break;

    case CPU_LOOP:
            Append( str, "\nLOOP\n" );
            ts_Def2Str( car(p), str );
            break;

    case CPU_FOR: case CPU_FORD:
            Append( str, "\nFOR " );
            ts_Def2Str( cdr(car(car(p))), str );
            Append( str, " := " );
            ts_Def2Str( car(car(car(p))), str );

            switch( h_TypeOf( p ) )
            {
            case CPU_FOR:  Append( str, " TO " ); break;
            case CPU_FORD: Append( str, " DOWNTO " ); break;
            }

            ts_Def2Str( cdr(car(p)), str );
            Append( str, " LOOP\n" );
            ts_Def2Str( cdr(p), str );
            break;

    case CPU_CYCLE:
            if( cdr(p) != NIL )
            {
                 Append( str, "<" );
                 Append( str, tr_GetVarName( cdr(p) ) );
                 Append( str, ">" );
            }
            ts_Def2Str( car(p), str );
            break;

    case CPU_BREAK:
    case CPU_CONTINUE:
            switch( h_TypeOf( p ) )
            {
            case CPU_BREAK:    Append( str, " BREAK " ); break;
            case CPU_CONTINUE: Append( str, " CONTINUE " ); break;
            }
            if( car(p) != NIL )
                 Append( str, tr_GetVarName( car(p) ) );
            Append( str, ";\n" );
            break;

    case CPU_SIN:  case CPU_COS:    case CPU_TAN:
    case CPU_ASIN: case CPU_ACOS:   case CPU_ATAN:
    case CPU_LOG:  case CPU_EXP:    case CPU_ABSI:
    case CPU_ABSF: case CPU_ABSV:
            switch( h_TypeOf( p ) )
            {
            case CPU_SIN:  Append( str, "SIN("  ); break;
            case CPU_COS:  Append( str, "COS("  ); break;
            case CPU_TAN:  Append( str, "TAN("  ); break;
            case CPU_ASIN: Append( str, "ASIN(" ); break;
            case CPU_ACOS: Append( str, "ACOS(" ); break;
            case CPU_ATAN: Append( str, "ATAN(" ); break;
            case CPU_LOG:  Append( str, "LOG("  ); break;
            case CPU_EXP:  Append( str, "EXP("  ); break;
            case CPU_ABSI:
            case CPU_ABSF:
            case CPU_ABSV: Append( str, "ABS("  ); break;
            }

            ts_Def2Str( car(p), str );
            AppendRParent( str );
            break;

    case CPU_NEGI: case CPU_NEGF: case CPU_NEGV:
            Append( str, "-" );
            Tree2StrWithP( car(p), str );
            break;

    case CPU_ADDF: case CPU_ADDI: case CPU_ADDV:
                                  Binar2StrP( p, str, "+" );    break;
    case CPU_SUBF: case CPU_SUBI: case CPU_SUBV:
                                  Binar2StrP( p, str, "-" );    break;
    case CPU_DIVF: case CPU_DIVI: case CPU_DIVVF:
                                  Binar2StrP( p, str, "/" );    break;
    case CPU_MOD:                 Binar2StrP( p, str, " MOD "); break;
    case CPU_MULF: case CPU_MULI: case CPU_MULVV: case CPU_MULFV:
                                  Binar2StrP( p, str, "*" );    break;
    case CPU_POWFI:
    case CPU_POWI: case CPU_POWF: Binar2StrP( p, str, "**" );   break;
    case CPU_MULVVV:              Binar2StrP( p, str, "%"  );   break;

    case CPU_CIF:
            Append( str, "FLOAT(" );
            ts_Def2Str( car(p), str );
            AppendRParent( str );
            break;

    case CPU_CFI:
            Append( str, "INT(" );
            ts_Def2Str( car(p), str );
            AppendRParent( str );
            break;

    case SY_LVECTORITEM:
            Append( str, "[" );
            ts_Def2Str( car(p), str );
            Append( str, "," );
            ts_Def2Str( car(cdr(p)), str );
            Append( str, "," );
            ts_Def2Str( cdr(cdr(p)), str );
            Append( str, "]" );
            break;

    case CPU_PUSH_VX:
            Append( str, tr_GetVarName( p ) );
            Append( str, ".x" );
            break;
    case CPU_PUSH_VY:
            Append( str, tr_GetVarName( p ) );
            Append( str, ".y" );
            break;
    case CPU_PUSH_VZ:
            Append( str, tr_GetVarName( p ) );
            Append( str, ".z" );
            break;

    case CPU_FIELDX:
            Tree2StrWithP( car(p), str );
            Append( str, ".x" );
            break;


    case CPU_FIELDY:
            Tree2StrWithP( car(p), str );
            Append( str, ".y" );
            break;

    case CPU_FIELDZ:
            Tree2StrWithP( car(p), str );
            Append( str, ".z" );
            break;

    case CPU_POP_VX:
            Append( str, tr_GetVarName( p ) );
            Append( str, ".x" );
            break;

    case CPU_POP_VY:
            Append( str, tr_GetVarName( p ) );
            Append( str, ".y" );
            break;

    case CPU_POP_VZ:
            Append( str, tr_GetVarName( p ) );
            Append( str, ".z" );
            break;

    case CPU_EQUI: case CPU_EQUF: case CPU_EQUV:
            Binar2Str( p, str, "=" );
            break;

    case CPU_NEQI: case CPU_NEQF: case CPU_NEQV:
            Binar2Str( p, str, "<>" );
            break;

    case CPU_LSSI: case CPU_LSSF: Binar2Str( p, str, "<"  );  break;
    case CPU_LEQI: case CPU_LEQF: Binar2Str( p, str, "<=" );  break;
    case CPU_GEQI: case CPU_GEQF: Binar2Str( p, str, ">=" );  break;
    case CPU_GRTI: case CPU_GRTF: Binar2Str( p, str, ">"  );  break;

    case CPU_PUSH_AI: case CPU_PUSH_AF: case CPU_PUSH_AV:
    case CPU_POP_AI:  case CPU_POP_AF:  case CPU_POP_AV:
    case CPU_PUSH_AS: case CPU_POP_AS:
            Append( str, tr_GetVarName(p) );
            Append( str, "[." );
            ts_Def2Str( car(p), str );
            Append( str, "]" );
            break;

    case CPU_PUSH_AX: case CPU_POP_AX:
            Append( str, tr_GetVarName(p) );
            Append( str, "[." );
            ts_Def2Str( car(p), str );
            Append( str, "].x" );
            break;

    case CPU_PUSH_AY: case CPU_POP_AY:
            Append( str, tr_GetVarName(p) );
            Append( str, "[." );
            ts_Def2Str( car(p), str );
            Append( str, "].y" );
            break;

    case CPU_PUSH_AZ: case CPU_POP_AZ:
            Append( str, tr_GetVarName(p) );
            Append( str, "[." );
            ts_Def2Str( car(p), str );
            Append( str, "].z" );
            break;

    case CPU_BOUND:
            Append( str, "BOUND(" );
            ts_Def2Str( car(p), str );
            Append( str, "," );
            ts_Def2Str( cdr(p), str );
            Append( str, ")" );
            break;

    case CPU_LSSI0: Binar2Str( p, str, "<0"   ); break;
    case CPU_LEQI0: Binar2Str( p, str, "<=0"  ); break;
    case CPU_GRTI0: Binar2Str( p, str, ">0"   ); break;
    case CPU_GEQI0: Binar2Str( p, str, ">=0"  ); break;
    case CPU_EQUI0: Binar2Str( p, str, "=0"   ); break;
    case CPU_NEQI0: Binar2Str( p, str, "<>0"  ); break;

    case CPU_LSSF0: Binar2Str( p, str, "<0."  ); break;
    case CPU_LEQF0: Binar2Str( p, str, "<=0." ); break;
    case CPU_GRTF0: Binar2Str( p, str, ">0."  ); break;
    case CPU_GEQF0: Binar2Str( p, str, ">=0." ); break;
    case CPU_EQUF0: Binar2Str( p, str, "=0."  ); break;
    case CPU_NEQF0: Binar2Str( p, str, "<>0." ); break;

    case CPU_AND:
            Tree2StrWithP( car(p), str );
            Append( str, " AND " );
            Tree2StrWithP( cdr(p), str );
            break;

    case CPU_OR:
            ts_Def2Str( car(p), str );
            Append  ( str, " OR " );
            ts_Def2Str( cdr(p), str );
            break;

    case CPU_NOT:
            Append( str, "NOT " );
            Tree2StrWithP( car(p), str );
            break;

    case CPU_MOVI: case CPU_MOVF: case CPU_MOVV: case CPU_MOVS:
            ts_Def2Str( cdr(p), str );
            Append  ( str, " := " );
            ts_Def2Str( car(p), str );
            Append  ( str, ";" );
            break;

    case CPU_IF:
            Append  ( str, "IF " );
            ts_Def2Str( car(p), str );
            Append  ( str, " THEN " );
            ts_Def2Str( cdr(p), str );
            break;

    case CPU_IFELSE:
            Append  ( str, "IF " );
            ts_Def2Str( car(p), str );
            Append  ( str, " THEN " );
            ts_Def2Str( car(cdr(p)), str );
            Append  ( str, " ELSE " );
            ts_Def2Str( cdr(cdr(p)), str );
            break;

    case CPU_FUNCTION:
            Append( str, "FUNC ");

            switch( tr_GetVarType( car(p) ) )
            {
            case T_NONE:  Append( str, "VOID " ); break;
            case T_INT:   Append( str, "INT " );  break;
            case T_FLOAT: Append( str, "FLOAT " ); break;
            case T_VECTOR:Append( str, "VECTOR " );break;
            }


            descr = h_GetDescr( car(p) );
            Append( str, ld_GetName( descr ) );
            descr = ld_GetNext( descr );
            AppendLParent( str );

            while( descr != NIL  && ld_GetNameDef( descr ) != DEF_VAR )
            {
                 if( ld_GetNameDef( descr ) == DEF_PARVAR )
                      Append( str, "VAR " );

                 switch( ld_GetType( descr ) )
                 {
                 case T_INT:    Append( str, "INT " );    break;
                 case T_FLOAT:  Append( str, "FLOAT " );  break;
                 case T_VECTOR: Append( str, "VECTOR " ); break;
                 }

                 Append( str, ld_GetName( descr ) );

                 if( ld_GetArrayCnt( descr )!=0 )
                 {
                      int i;

                      Append( str, "[" );

                      for( i = 0; i < ld_GetArrayCnt( descr ) ; ++i )
                      {
                           sprintf( buf, "%i", ld_GetArrayRange( descr, i ));
                           Append( str, buf );

                           if( i != ld_GetArrayCnt( descr )-1 )
                                Append( str, "," );
                      }

                      Append( str, "]" );
                 }

                 descr = ld_GetNext( descr );

                 if( descr!= NIL &&
                     ld_GetNext( descr ) != NIL &&
                     ld_GetNameDef( descr ) != DEF_VAR )
                      Append( str, ", " );
            }

            Append( str, ")\n" );

            while( descr != NIL &&  ld_GetNameDef( descr ) == DEF_VAR )
            {
                 Append( str, "VAR " );

                 switch( ld_GetType( descr ) )
                 {
                 case T_INT:    Append( str, "INT " );    break;
                 case T_FLOAT:  Append( str, "FLOAT " );  break;
                 case T_VECTOR: Append( str, "VECTOR " ); break;
                 }

                 Append( str, ld_GetName( descr ) );

                 if( ld_GetArrayCnt( descr )!=0 )
                 {
                      int i;

                      Append( str, "[" );

                      for( i = 0; i < ld_GetArrayCnt( descr ) ; ++i )
                      {
                           sprintf( buf, "%i", ld_GetArrayRange( descr, i ));
                           Append( str, buf );

                           if( i != ld_GetArrayCnt( descr )-1 )
                                Append( str, "," );
                      }

                      Append( str, "]" );
                 }

                 Append( str, "; " );
                 descr = ld_GetNext( descr );

            }

            ts_Def2Str( car(car(p)), str );
            break;

    case CPU_RETURN:
            Append( str, "RETURN;" );
            break;

    case CPU_RETURNINT:
    case CPU_RETURNFLOAT:
    case CPU_RETURNVECTOR:
            Append( str, "RETURN " );
            ts_Def2Str( car(p), str );
            Append( str, ";\n" );
            break;

    case CPU_CALL:
            descr = h_GetDescr( car(p) );
            Append( str, ld_GetName( descr ) );
            AppendLParent( str );

            p = cdr(p);

            while( p != NIL )
            {
                 ts_Def2Str( car(p), str );

                 if( cdr(p) != NIL )
                      Append( str, ", " );

                 p = cdr(p);
            }

            AppendRParent( str );
            break;

    case CPU_PRINT:
            Append( str, "PRINT(" );

            p = car(p);

            while( p != NIL )
            {
                 ts_Def2Str( car(p), str );
                 if( cdr(p) != NIL )
                      Append( str, ", " );
                 p = cdr(p);
            }

            Append( str, " );\n" );
            break;

    case SY_EMPTY: break;
    case CPU_ADD_VCONSTI:
            Append( str, ld_GetName( h_GetDescr(cdr(p)) ) );
            Append( str, " := " );
            Append( str, ld_GetName( h_GetDescr(cdr(p)) ) );
            Append( str, " + " );
            ts_Tree2Str( car(p), str );
            Append( str, ";\n" );
            break;

    default:
            Append( str, "?" );
    }
 }

 /*========================================================================*/
void ts_Tree2Str( TTree *p, TOutStr *str )
 {
    for( ; p != NIL ; p = cdr(p) )
         ts_Def2Str( p, str );
 }


void PrintTree( TTree *p )
 {
    TOutStr  outStr;
    static char     buf[2048];
    ts_InitTOutStr( &outStr, h_ErrorOf(g_heap), buf, sizeof(buf) );
    ts_Tree2Str( p, &outStr );
    printf("\n----------2------------\n%s\n",buf);
 }

#endif


