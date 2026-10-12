#include "bytecod.h"
#include "cgen.h"
#include "usestack.h"

 /*========================================================================*/
void us_SetMaxSt( int deep, int *maxDeep )
 {
    if( deep > *maxDeep  )
         *maxDeep = deep;
 }

 /*========================================================================*/
int us_CalcUseStackFunc( TTree *p, int deep, int *maxDeep )
 {
    if( p == NIL )
         return deep;

    switch( h_TypeOf( p ) )
    {
    case SY_VARIABLE: /* Skip const define */ return deep;

    case CPU_PUSH_VI:
    case CPU_PUSH_VS:
    case CPU_PUSH_VF:

    case CPU_PUSH_VX:
    case CPU_PUSH_VY:
    case CPU_PUSH_VZ:

    case CPU_PUSH_CI:
    case CPU_PUSH_CF:
    case CPU_PUSH_CS:
    case CPU_RNDI:
    case CPU_RNDF:
            deep += 1;
            break;

    case CPU_PUSH_VV:
            deep += 3;
            break;

    case CPU_FIELDX:
    case CPU_FIELDY:
    case CPU_FIELDZ:
            us_CalcUseStackFunc( car(p), deep, maxDeep );
            deep -= 2;
            break;

    case CPU_OPERATOR:
            if( car(p) != NIL && h_TypeOf( car(p) ) == CPU_OPERATOR )
            {
                 TTree *list = car(p);

                 while( list != NIL )
                 {
                     us_CalcUseStackFunc( car(list), deep, maxDeep );
                     list = cdr(list);

                 }
            }
            else
            {
                 us_CalcUseStackFunc( car(p), deep, maxDeep );
                 us_CalcUseStackFunc( cdr(p), deep, maxDeep );
            }
            break;


    case CPU_CONTINUE:
    case CPU_BREAK:
            break;


    case CPU_ADDF:
    case CPU_ADDI:
    case CPU_SUBI:
    case CPU_ATAN2:
    case CPU_ADDV:
    case CPU_SUBF:
    case CPU_SUBV:
    case CPU_DIVF:
    case CPU_DIVI:
    case CPU_DIVVF:
    case CPU_MOD:
    case CPU_MULF:
    case CPU_MULI:
    case CPU_MULFV:
    case CPU_POWFI:
    case CPU_POWI:
    case CPU_POWF:
    case CPU_AND:
    case CPU_OR:
            deep -= 1;
            break;

    case CPU_MULVVV:
            deep -= 3;
            break;

    case CPU_SIN:
    case CPU_COS:
    case CPU_TAN:
    case CPU_ASIN:
    case CPU_ACOS:
    case CPU_ATAN:
    case CPU_LOG:
    case CPU_EXP:
    case CPU_ABSI:
    case CPU_ABSF:
    case CPU_ABSV:
    case CPU_NEGI:
    case CPU_NEGF:
    case CPU_NEGV:
    case CPU_CIF:
    case CPU_CFI:
    case CPU_NOT:
    case CPU_LSSI0:
    case CPU_LEQI0:
    case CPU_GRTI0:
    case CPU_GEQI0:
    case CPU_EQUI0:
    case CPU_NEQI0:

    case CPU_LSSF0:
    case CPU_LEQF0:
    case CPU_GRTF0:
    case CPU_GEQF0:
    case CPU_EQUF0:
    case CPU_NEQF0:
    case CPU_ADD_VCONSTI:
            break;

    case SY_LVECTORITEM:
            us_CalcUseStackFunc( car(p),      deep, maxDeep );
            us_CalcUseStackFunc( car(cdr(p)), deep, maxDeep );
            us_CalcUseStackFunc( cdr(cdr(p)), deep, maxDeep );
            break;

    case CPU_PUSH_AI:
    case CPU_PUSH_AS:
    case CPU_PUSH_AF:
            us_CalcUseStackFunc( car(p), deep, maxDeep );
            deep += 1;
            break;

    case CPU_PUSH_AV:
            us_CalcUseStackFunc( car(p), deep, maxDeep );
            deep += 3;
            break;

    /***************** POP Area *********************/
    case CPU_POP_VS:
    case CPU_POP_VI:
    case CPU_POP_VF:
    case CPU_POP_VX:
    case CPU_POP_VY:
    case CPU_POP_VZ:
            deep -= 1;
            break;

    case CPU_POP_VV:
            deep -= 3;
            break;

    case CPU_POP_AS:
    case CPU_POP_AI:
    case CPU_POP_AF:
    case CPU_POP_AX:
    case CPU_POP_AY:
    case CPU_POP_AZ:
            us_CalcUseStackFunc( car(p), deep, maxDeep );
            deep -= 2;
            break;

    case CPU_POP_AV:
            us_CalcUseStackFunc( car(p), deep, maxDeep );
            deep -= 3+1;
            break;

            /* end of POP Area */

    case CPU_BOUND:
            us_CalcUseStackFunc( car(p), deep, maxDeep );
            us_CalcUseStackFunc( cdr(p), deep, maxDeep );
            deep -= 1;
            break;



    case CPU_MULVV:
            deep -= 6-1; /* CHECKME Scalar mul ? */
            break;


    case CPU_MOVI:
    case CPU_MOVF:
    case CPU_MOVV:
    case CPU_MOVS:
    case CPU_FORTO:
    case CPU_FORASSIGN:
    case CPU_IF:
    case CPU_IFELSE:
    case CPU_LOOP:
    case CPU_FOR: case CPU_FORD:
            us_CalcUseStackFunc( car(p), deep, maxDeep );
            us_CalcUseStackFunc( cdr(p), deep, maxDeep );
            break;

    case CPU_CYCLE:
            us_CalcUseStackFunc( car(p), deep, maxDeep );
            break;

    case SY_EXTERN:
            break;

    case CPU_FUNCTION:
            us_CalcUseStackFunc( car(car(p)), deep, maxDeep );
            /* FIXME */
            break;

    case CPU_RETURN:
            deep -= CALL_STACK_RESERVED;
            break;

    case CPU_RETURNINT:
    case CPU_RETURNFLOAT:
            us_CalcUseStackFunc( car(p), deep, maxDeep );
            deep -= 1;
            break;


    case CPU_RETURNVECTOR:
            us_CalcUseStackFunc( car(p), deep, maxDeep );
            deep -= 3;
            break;

    case CPU_CALL:
    case CPU_CALLEXTERN:
            {
            TTree *param = cdr(p);

            deep += CALL_STACK_RESERVED;

            switch( tr_GetVarType( car(p) ) )
            {
            case T_STR:
            case T_INT:
            case T_FLOAT:  deep += 1; break;

            case T_VECTOR: deep += 3; break;
            case T_NONE: break;
            default: lng_ASSERTNQ("us_CalcUseStackFunc: CPU_CALL");
            }

            while( param != NIL )
            {
                 us_CalcUseStackFunc( car(param), deep, maxDeep );

                 switch( h_TypeOf( param ) )
                 {
                 case CPU_CALLPARVAR:
                 case CPU_CALLPARAMI:
                 case CPU_CALLPARAMF:
                 case CPU_CALLPARAMS:
                      deep += 1;
                      break;
                 case CPU_CALLPARAMV:
                      deep += 3;
                      break;
                 }

                 param = cdr(param);
            }

            }
            break;


    case CPU_PRINT:
            p = car(p);

            while( p != NIL )
            {
                 us_CalcUseStackFunc( car(p), deep, maxDeep );

                 switch( h_TypeOf(p) )
                 {
                 case CPU_CALLPARAMI:
                 case CPU_CALLPARAMF:
                 case CPU_CALLPARAMS:
                                deep -= 1;
                                break;

                 case CPU_CALLPARAMV:
                                deep -= 3;
                                break;

                 case CPU_CALLPARAMEOL:
                                break;
                 default:
                         lng_ASSERTNQ( "Unknown type in PRINT" );
                 }
                 p = cdr(p);
            }
            break;

    case CPU_CALLPARVAR:
            switch( h_TypeOf( car(p) ) )
            {
            case CPU_PUSH_VI:
            case CPU_PUSH_VF:
            case CPU_PUSH_VV:
            case CPU_PUSH_VS:
                    deep += 1;
                    break;

            case CPU_PUSH_AI:
            case CPU_PUSH_AF:
            case CPU_PUSH_AV:
            case CPU_PUSH_AS:
                    us_CalcUseStackFunc( car(car(p)), deep, maxDeep );
                    deep += 1;
                    break;
            }
            break;

    case CPU_PUSH_AX:
    case CPU_PUSH_AY:
    case CPU_PUSH_AZ:
            us_CalcUseStackFunc( car(p), deep, maxDeep );
            deep += 1;
            break;

    case CPU_NEQV:
    case CPU_EQUV:
            deep -= 6-1;
            break;

    case CPU_ADD_VARI:
    case CPU_ADD_VARF:
    case CPU_SUB_VARI:
    case CPU_SUB_VARF:
    case CPU_MUL_VARI:
    case CPU_MUL_VARF:
    case CPU_DIV_VARI:
    case CPU_DIV_VARF:
    case CPU_MOD_VAR:
            us_CalcUseStackFunc( car(p), deep, maxDeep );
            break;

    case CPU_DROPSTACK:
            us_CalcUseStackFunc( car(p), deep, maxDeep );
            deep -= (int)h_GetConstINT( cdr(p) );
            break;
    case SY_EMPTY:  break;


    default:
            lng_ASSERTNQ( "us_CalcUseStackFunc: Unknown command" );
    }

    us_SetMaxSt( deep, maxDeep );

    return deep;
 }

 /*========================================================================*/
int us_CalcLocalVarSize( TTree *p )
 {
    TName *descr = h_GetDescr( p );
    int    size  = 0;

    while( descr != NIL )
    {
         if( ld_GetNameDef( descr ) == DEF_VAR )
              size += cg_GetParamSize( descr );

         descr = ld_GetNext( descr );
    }

    return size;
 }

 /*========================================================================*/
void us_CalcUseStack( TTree *p )
 {
    int deep, maxDeep;

    while( p != NIL )
    {
         deep    = 0;
         maxDeep = deep;

         switch( h_TypeOf( p ) )
         {
         case CPU_FUNCTION:
                 us_CalcUseStackFunc( car(car(p)), deep, &maxDeep );
                 maxDeep += us_CalcLocalVarSize( car(p) );
                 ld_SetDataPtr( h_GetDescr( car(p) ), maxDeep  );
                 break;

         case CPU_PUSH_VI:
         case CPU_PUSH_VF:
         case CPU_PUSH_VS:
                 break;
         }

         p = cdr(p);
    }
 }
