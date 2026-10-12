/*
   File:  SCANER.C
   Autor: Suavik
   Ver    1.1

   Лексический разбор.
   Префикс lex_


   комбинация "" выбивает сканер

   05.08.97 В чтении костант были удалены фигурные скобки и else
   при преведении степени срабатывал от внутрициклового if. Исправлено;

   11.08.97 Введены lng_ASSERT
   01.10.97 При чтении числа стояла проверка на exp < 0 внутри левого if().
            Сделано корректное чтение пустых строк.
   31.07.97 ═рўры фюсрты Є№ шэъы■ф√
   25.10.97 set include path
 */

/*
#define TEST_SCANNER
*/

#include <process.h>
#include <malloc.h>
#include <string.h>
#include "lex.h"
#include "instrm.h"
#include "langer.h"
#include "strequ.h"

#define NEXTCH     scanner->m_NextChar(scanner)
#define ERROR(x)   Error(scanner,x)
/* #define MAX_DOUBLE 1.7976931348623158E+308 
#define MAX_DOUBLE 1.797693134862315E+308
 */
#define MAX_DOUBLE 1.79769313486231E+308
#define MAX_DOUBLE_EXP 308
#define MIN_DOUBLE_EXP (-307)
#define NIL ((void *)0L)

static const struct {
    const char   *w;
    LEX_TYPE     lex;
} ResWord[RESERVED_WORD_CNT] = {
    {"var",   SY_VAR   },{"int",   SY_INT   },{"float", SY_FLOAT },
    {"vector",SY_VECTOR},{"void",  SY_VOID  },{"func",  SY_FUNC  },
    {"loop",  SY_LOOP  },{"return",SY_RETURN},{"break", SY_BREAK },
    {"goto",  SY_GOTO  },{"if",    SY_IF    },{"then",  SY_THEN  },
    {"else",  SY_ELSE  },{"print", SY_PRINT },{"mod",   SY_MOD   },
    {"for",   SY_FOR   },{"to",    SY_TO    },{"downto",SY_DOWNTO},
    {"and",   SY_AND   },{"or",    SY_OR    },{"not",   SY_NOT   },
    {"sin",   SY_SIN   },{"cos",   SY_COS   },{"tan",   SY_TAN   },
    {"asin",  SY_ASIN  },{"acos",  SY_ACOS  },{"atan",  SY_ATAN  },
    {"atan2", SY_ATAN2 },{"log",   SY_LOG   },{"exp",   SY_EXP   },
    {"bound", SY_BOUND },{"const", SY_CONST },
    {"abs",   SY_ABS   },{"forward",SY_FORWARD},
    {"PI",    SY_PI    },{"str",    SY_STR  },
    {"rndi",  SY_RNDI  },{"rndf",  SY_RNDF},
    {"extern",SY_EXTERN},
    {"include",SY_INCLUDE},
    {"continue",SY_CONTINUE }
};

 /*========================================================================*/
void InitializeScanner( TScanner *scanner )
 {
    scanner->line     = 1;
    scanner->pos      = 0;
    scanner->chptr    = 0;
    scanner->ch       = ' ';
    scanner->och      = ' ';
    scanner->textPath = "";
 }

 /*========================================================================*/
 /*
  * Инициализация сканера для чтения из памяти.
  */
void lex_InitScannerFromMem( TScanner     *scanner,
                             char         *text )
 {
    scanner->inputFile  = NULL;
    scanner->text       = text;
    scanner->m_NextChar = is_NextCh;
    scanner->fileName[0]= 0;
    InitializeScanner( scanner );
    NEXTCH;
    lex_Get( scanner );
 }

 /*========================================================================*/
 /*
  * Инициализация сканера для чтения из откратого файла
  */
void lex_InitScannerFromFile( TScanner   *scanner,
                              FILE       *inputFile )
 {
    scanner->text       = NULL;
    scanner->inputFile  = inputFile;
    scanner->prevScanner= NULL;
    scanner->m_NextChar = is_NextChFile;
    InitializeScanner( scanner );
    NEXTCH;
    lex_Get( scanner );
 }

/*--------------------------------------------------------------------*/
LEX_TYPE lex_GetType( const TScanner *scanner )
 {
    return scanner->sy.m_type;
 }

long lex_GetINT( const TScanner *scanner )
 {
    return scanner->sy.m_val.i;
 }

double lex_GetFLOAT( const TScanner *scanner )
 {
    return scanner->sy.m_val.f;
 }

const char * lex_GetSTR( const TScanner *scanner )
 {
    return scanner->sy.m_val.s;
 }

TName *lex_GetDescr( const TScanner *scanner )
 {
    return scanner->sy.m_val.m_descr;
 }


int lex_SizeOf( DATA_TYPE type )
 {
    switch( type )
    {
    case T_INT:    return 4;
    case T_FLOAT:  return 8;
    case T_VECTOR: return 8*4;
    }

    return 0;
 }

unsigned lex_Pos( const TScanner *scanner )
 {
    return scanner->pos;
 }

unsigned lex_Line( const TScanner *scanner )
 {
    return scanner->line;
 }


void Error( TScanner *scanner, int er )
 {
    scanner->sy.m_type   = SY_ERROR;
    scanner->sy.m_val.er = er;
 }


 /*========================================================================*/
void lex_ClearTScanner( TScanner *scanner )
 {
    os_ClearTOutStream( &(scanner->m_names)   );
    os_ClearTOutStream( &(scanner->m_strings) );
    scanner->text      = NULL;
    scanner->inputFile = NULL;
    scanner->line      = 0;
    scanner->pos       = 0;
    scanner->ch        = 0;
    scanner->och       = 0;
	scanner->m_NextChar = NULL; /* FIXME - поставить на умолчальную ошибку */
 }

 /*========================================================================*/
void lex_DropTScanner( TScanner *scanner )
 {
    lex_ClearTScanner( scanner );
    os_DropTOutStream( &(scanner->m_names)   );
    os_DropTOutStream( &(scanner->m_strings) );
 }

 /*========================================================================*/
TByte **lex_ProvideForDeletingNames( TScanner *scanner )
 {
    return os_ProvideForDeletingTOutStream( &(scanner->m_names) );
 }

 /*========================================================================*/
TByte **lex_ProvideForDeletingStrings( TScanner *scanner )
 {
    return os_ProvideForDeletingTOutStream( &(scanner->m_strings) );
 }

 /*========================================================================*/
void lex_InitTScanner( TScanner *scanner,
                       char *wordBuf,   int wordBufSize,
                       char *stringBuf, int stringBufSize )
 {
    InitializeScanner( scanner );
    os_InitTOutStream( &(scanner->m_names),
                       (TByte*)wordBuf,
                       wordBufSize,
                       "wordBuff" );
    os_InitTOutStream( &(scanner->m_strings),
                       (TByte*)stringBuf,
                       stringBufSize,
                       "stringBuff" );
 }


/*
 *  Return 1 if no errors
 *  else return 0
 */
 /*========================================================================*/
static int  Comment( TScanner *scanner )
 {
    int  cLevel = 1;
    char och;

    while( cLevel > 0 && scanner->ch != SY_EOF )
    {
         och = scanner->ch;
         NEXTCH;

         if( och == '*' && scanner->ch == '/' )
         {
              cLevel--;
              NEXTCH;
         }
         else
         if( och == '/' && scanner->ch == '*' )
         {
              cLevel++;
              NEXTCH;
         }
    }

    if( cLevel > 0 )
    {
         ERROR( ER_UNEXPECTED_END_OF_FILE );
         return 0;
    }

    return 1;
 }


int IsDigit( char c )
 {
    return c >= '0' && c <= '9';
 }

void LoadConstant( TScanner *scanner )
 {
    static
    double powerof10[]  = { 1.e1, 1.e2, 1.e4, 1.e8, 1.e16, 1.e32,
                            1.e64,1.e128,1.e256 };
    static
    double digits[]     = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    double      result = 0;
    long        intResult = 0;
    long        newVal;
    int         exp    = 0;               /* exponent              */
    int         count  = 1;               /* exponent calculation  */
    int         value  = 0;               /* successful parse      */
    int         sign;
    int	        isInt  = 1;
    int         i;
    char        buf[MAX_NUM_LEN];

    /*
     *  Load first number
     */
    buf [ 0 ] = scanner->och;
    while( IsDigit ( scanner->ch ) && count < MAX_NUM_LEN )
    {
        buf[ count++ ] = scanner->ch;

        if( count > MAX_LONG_LEN )   /* number too long */
             isInt = 0;              /* This real value */

        NEXTCH;
    }

    /*
     * If buffer overflow  then load last chars
     */
    while( IsDigit( scanner->ch ) )
    {
        exp++;
        NEXTCH;
        isInt = 0;
    }

    if( isInt != 0 && scanner->ch != '.' &&
	    scanner->ch != 'e' && scanner->ch != 'E' )
    {
         for( i = 0 ; i < count; i++ )
         {
              newVal = intResult * 10 + buf[i] - '0';

              if( newVal < intResult )  /* Overflow ? */
              {
                   isInt = 0;
                   break;
              }

              intResult = newVal;
         }

         if( isInt != 0 )
         {
              scanner->sy.m_type  = SY_INTCONS;
              scanner->sy.m_val.i = intResult;
              return;
         }
    }

    /*
     * Read in fractional part of number, until an 'E' is reached.
     * count digits after decimal point.
     */
    for( i = 0 ; i < count; i++ )
    {
         result = result * 10 + digits[buf[i] - '0'];
         value  = 1;
    }

    if( scanner->ch == '.')
    {
         NEXTCH;
         while( IsDigit( scanner->ch ) )
         {
             result = result * 10 + digits[scanner->ch - '0'];
             NEXTCH;
             value  = 1;
             --exp;
         }
    }

    /*
     *	Read in explicit exponent and calculate real exponent.
     */
    if( value && (scanner->ch == 'E' || scanner->ch == 'e') )
    {
         int i; /* Don't let exponent overflow */
         NEXTCH;

         if( (sign = (scanner->ch == '-')) != 0 || scanner->ch == '+' )
	      NEXTCH;

         for( i = count = 0 ;   IsDigit( scanner->ch ) && i < 3;   i++ )
         {
              count *= 10;
              count += scanner->ch - '0';
              NEXTCH;
         }

         while( IsDigit(scanner->ch) )
             NEXTCH;

         if( sign != 0 )
              exp -= count;
         else exp += count;
    }

    /*
     * ADJUST NUMBER BY POWERS OF TEN SPECIFIED BY FORMAT AND EXPONENT.
     */
    if( result != 0.0 )
         if( exp > MAX_DOUBLE_EXP )
              result = (result < 0) ? -MAX_DOUBLE: MAX_DOUBLE;
         else
         if( exp < MIN_DOUBLE_EXP )
              result = 0.0;
         else
         if( exp < 0 )
         {
	          for( count = 0, exp = -exp;   exp  ; count++, exp >>= 1 )
		           if( exp & 1)
                        result /= powerof10[count];
         }
         else
         {
              for( count = 0 ;   exp ; count++, exp >>= 1 )
                   if( exp & 1 )
                        result *= powerof10[count];
         }

    scanner->sy.m_type  = SY_FLOATCONS;
    scanner->sy.m_val.f = result;
 }

/*
 *  Search word in buffer scanner.wordBuf.
 *  input: search word
 *  if complit return pointer to word
 *        else return 0
 */

char *WordPtr( os_TOutStream *wb, const char *w )
 {
    int i;
    int maxPos = (int)os_CurPos( wb );

    if( w == (const char *)NIL )
    {
         printf("Error: LEX.C  word==NIL\n");
         exit(0);
    }

    for( i = 0; i < maxPos; ++i )
    {
        if( str_StrEQU( w, os_GetStr( wb, _TINT(i) ) ) )
             return os_GetStr( wb, _TINT(i) );

        /* search end of word */
        while( os_GetByte( wb, i ) != 0  &&  i < wb->m_pos  )
             ++i;
    }

    return (char*)NIL;
 }

/*
 * Search string "st" in buffer
 * if this word not found - load to buffer.
 * scanner.sy.val.s = pointer to word in buffer
 */
void lex_DetectNewStroke( os_TOutStream *wb,
                          TScanner *scanner,
                          const char st[] )
 {
    char *wordPTR = WordPtr( wb, st );

    if( wordPTR != NIL )
         scanner->sy.m_val.s = wordPTR;
    else
    {
         scanner->sy.m_val.s = os_CurStr( wb );
         os_PutStr( wb, st );
    }
 }

/*
 *  Load word from input stream.
 */
static void LoadWord( TScanner *scanner )
 {
    register int  i = 1;
    register int  k;
    int           j, eq;
    char buf [ MAX_WORD_LEN ];
    const char *word;

    /*
     * Loan word in buffer 'buf'
     */
    buf[ 0 ] = scanner->och;

    while( (scanner->ch >= 'A' && scanner->ch <= 'Z') ||
           (scanner->ch >= 'a' && scanner->ch <= 'z') ||
           (scanner->ch == '_') ||
           IsDigit( scanner->ch ) )
    {
         if( i >= MAX_WORD_LEN )
         {
	          ERROR( ER_WORD_TOO_LONG );
	          return;
         }

         buf[ i++ ] = scanner->ch;
         NEXTCH;
    }

    buf [ i++ ] = 0;
    /* i == length of word */

    /*
     *  If this is reserverd word return lexem
     */
    for( j = 0 ; j < RESERVED_WORD_CNT; j++ )
    {
	     eq = 1;
	     /*  Compare 2 word */
	     word = ResWord[ j ].w;

	     for( k = 0 ; k < i; k++ )
	          if( buf[ k ]  !=  word[ k ] )
                  {
                       eq = 0;
                       break;
	          }

	     if( eq != 0 )
                  break;
    }

    if( eq != 0 )
    {
         scanner->sy.m_type  = ResWord[ j ].lex;
         return;
    }

    /*
     *  Registers new world
     */
    scanner->sy.m_type  = SY_WORD;
    lex_DetectNewStroke( &(scanner->m_names),scanner,buf );
 }


/*
 * Load string constant and put to buffer
 */
void LoadStringConstant( TScanner *scanner )
 {
    int  i = 0;
    int  first = 1;
    char stringChar = scanner->och;
    char buf[MAX_WORD_LEN], curChar;

    scanner->sy.m_type = SY_STRINGCONS;

    for(;;)
    {
         curChar = scanner->ch;

         if( (!first) &&
             curChar == stringChar &&
             curChar == scanner->och  )
         {
              NEXTCH;
              first = 0;
         }
         else
         if( curChar == stringChar )
         {
              NEXTCH;
              first = 0;
              break;
         }

         if( i >=  MAX_WORD_LEN )
         {
              ERROR( ER_STRING_TOO_LONG );
              return;
         }

         buf[ i++ ] = curChar;
         NEXTCH;
    }

    buf[ i ] = 0;
    lex_DetectNewStroke( &(scanner->m_strings), scanner, buf );
 }

/*--------------------------------------------------------------------*/
void CopyScanner(TScanner *dest, TScanner *sour )
 {
    dest->ch  = sour->ch;
    dest->och = sour->och;
    memcpy( &(dest->m_names), 
            &(sour->m_names), sizeof(os_TOutStream) );
    memcpy( &(dest->m_strings), 
            &(sour->m_strings), sizeof(os_TOutStream) );
    memcpy( &(dest->sy), &(sour->sy), sizeof(TLexem) );
    strcpy( dest->fileName, sour->fileName );
 }

/*--------------------------------------------------------------------*/
void RestoreScanner(TScanner *dest, TScanner *prev )
 {
    dest->text        = prev->text;
    dest->inputFile   = prev->inputFile;
    dest->prevScanner = prev->prevScanner;

    dest->line        = prev->line;
    dest->pos         = prev->pos;
    dest->chptr       = prev->chptr;

    dest->ch          = prev->ch;
    dest->och         = prev->och;

    dest->m_NextChar  = prev->m_NextChar;
    memcpy( &(dest->sy), &(prev->sy), sizeof(TLexem) );

 }

/*--------------------------------------------------------------------*/
TScanner *lex_Include( TScanner *scanner, const char *fileName )
 {
    TScanner *newScan;
    char      nameBuf[1024];
    FILE     *f;

    strcpy( nameBuf, scanner->textPath );

    if(  nameBuf[0]!=0 && nameBuf[strlen(nameBuf)]!='\\' )
         strcat( nameBuf, "\\" );
    strcat( nameBuf, fileName );
    f  = fopen( nameBuf, "rb" );

    lex_Get( scanner );

    if(  f == NULL  )
    {
         ERROR( ER_INCLUDE_FILE_NOT_FOUND );
         newScan = NULL;
    }
    else
    {
         newScan = (TScanner *)(malloc(sizeof(TScanner)));

         InitializeScanner      ( newScan );
         CopyScanner            ( newScan, scanner );
         lex_InitScannerFromFile( newScan, f );
         strncpy( newScan->fileName, fileName, MAXINCLUDEFILELEN-1 );
         newScan->prevScanner = scanner;
    }
    return newScan;
 }

/*--------------------------------------------------------------------*/
int lex_Get( TScanner *scanner )
 {
    for(;;)
    {
	     NEXTCH;

         switch( scanner->och )
         {
         case SY_EOF: /*---------------------------- End of text  */
                 if(  scanner->prevScanner != NULL  )
                 {
                      TScanner *prev = scanner->prevScanner;

                      fclose     ( scanner->inputFile );
                      RestoreScanner( scanner, prev );

                      prev->prevScanner = NULL;
                      free       ( prev );
                 }
                 else scanner->sy.m_type = SY_EOF;
                 break;

         case SY_TAB:
         case SY_EOL:
         case ' ': /*------------------------------- ' ' Tab  eol */
                 while( scanner->ch == ' '    ||
                        scanner->ch == SY_TAB ||
                        scanner->ch == SY_EOL )
                      NEXTCH;
                 continue;

         case '"':
                 LoadStringConstant( scanner );
                 break;

         case '%':
                 scanner->sy.m_type = SY_PERSENT;
                 break;

         case '\'': /*-------------------- "     '         */
                 LoadStringConstant( scanner );
                 break;

         case '(':
                 scanner->sy.m_type = SY_LPARENT;
                 break;

         case ')':
	             scanner->sy.m_type  = SY_RPARENT;
                 break;

         case '*':
                 if( scanner->ch == '*' )
                 {
                      NEXTCH;
                      scanner->sy.m_type = SY_POWER;
                 }
                 else scanner->sy.m_type = SY_SMUL;
                 break;

         case '+':
                 scanner->sy.m_type = SY_PLUS;
                 break;

         case ',':
                 scanner->sy.m_type = SY_COMMA;
                 break;

         case '-':
                 scanner->sy.m_type = SY_MINUS;
                 break;

         case '.': /*------------------------------ .                */
                 scanner->sy.m_type = SY_PERIOD;
                 break;

         case '/': /*------------------------------- / *  //   /     */
                 if( scanner->ch == '/' )
                 {
                      NEXTCH;
                      while( scanner->ch != SY_EOL &&
                             scanner->ch != SY_EOF )
                          NEXTCH;

	                  continue;
                 }
                 else
                 if( scanner->ch == '*' )
                 {
                      NEXTCH;

                      if( Comment(scanner) )
	                       continue;

                      break;
                 }

                 scanner->sy.m_type = SY_SLASH;
                 break;

         case '0': case '1': case '2': case '3': case '4':
         case '5': case '6': case '7': case '8': case '9':
                 LoadConstant( scanner );
                 break;

         case ':': /*------------------------------- :=    =      */
                 if( scanner->ch == '=' )
                 {
                      NEXTCH;
                      scanner->sy.m_type = SY_BECOMES;
                 }
                 else scanner->sy.m_type = SY_COLON;
                 break;

         case ';':
                 scanner->sy.m_type = SY_SEMICOLON;
                 break;

         case '<': /*------------------------------- <=    <>      < */
                 if( scanner->ch == '=' )
                 {
                      NEXTCH;
                      scanner->sy.m_type = SY_LEQ;
                 }
                 else
                 if( scanner->ch == '>' )
                 {
                      NEXTCH;
                      scanner->sy.m_type = SY_NEQ;
                 }
                 else scanner->sy.m_type = SY_LSS;
                 break;

         case '=':
                 scanner->sy.m_type = SY_EQL;
                 break;

         case '>': /*------------------------------- >=    >         */
                 if( scanner->ch == '=' )
                 {
                      NEXTCH;
                      scanner->sy.m_type = SY_GEQ;
                 }
                 else scanner->sy.m_type = SY_GRT;
                 break;

         case '?':
                 scanner->sy.m_type = SY_PRINT;
                 break;

         case '@':
                 scanner->sy.m_type = SY_THETA;
                 break;

         case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
         case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
         case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
         case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
         case 'Y': case 'Z':
                 LoadWord( scanner );
                 break;

         case '[':
                 scanner->sy.m_type = SY_LBRACK;
                 break;

         case ']':
                 scanner->sy.m_type = SY_RBRACK;
                 break;

         case '_':
         case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
         case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
         case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
         case 's': case 't': case 'u': case 'v': case 'w': case 'x':
         case 'y': case 'z':
                 LoadWord( scanner );
                 break;

         case '{':
                 scanner->sy.m_type = SY_LBRACKE;
                 break;

         case '}':
                 scanner->sy.m_type = SY_RBRACKE;
                 break;

         case '~':
                 scanner->sy.m_type = SY_TILDA;
                 break;
         default:
                 ERROR( ER_UNKNOWN_CHAR );
         } /* switch (och)  */

         break;
    }

    if( scanner->sy.m_type == SY_ERROR )
	     return 0;

    return 1;
 } /*--- GetLex ---*/

int lex_GetErrorCode( const TScanner *sc )
 {
    return sc->sy.m_val.er;
 }

const char *lex_ErrorToStr( int erCode )
 {
    switch( erCode )
    {
	case ER_WORD_TOO_LONG:          return "SCANNER: Word too long";
	case ER_BUFFER_OVERFLOW:        return "SCANNER: Buffer overflow";
	case ER_UNEXPECTED_END_OF_FILE: return "SCANNER: Unexpected end of file";
	case ER_STRING_TOO_LONG:        return "SCANNER: String too long";
	case ER_UNKNOWN_CHAR:           return "SCANNER: Unknown char";
    case ER_INCLUDE_FILE_NOT_FOUND: return "SCANNER: Include file not found";
    }
    return "SCANER: Unknown error";
 }

void lex_SetIncludePath( TScanner *scanner, const char *path )
{
    scanner->textPath = path;
}

#ifdef TEST_SCANNER
#include <stdio.h>
void ScannerTest(void)
 {
    for(;;)
    {
	 GetLex();

	 switch( scanner.sy.type )
         {
	 case SY_EOF:
		 printf("EOF\n");
		 return;
	 case SY_ERROR:
		 printf("Error in pos %i : ",scanner.pos);

		 switch( scanner.sy.val.er )
                 {
		 case ER_WORD_TOO_LONG:
			 printf("Word too long\n");
			 return;
		 case ER_BUFFER_OVERFLOW:
			 printf("Buffer overflow\n");
			 return;
		 case ER_UNEXPECTED_END_OF_FILE:
			 printf("Unexpected end of file\n");
			 return;
		 case ER_STRING_TOO_LONG:
			 printf("Stering too long\n");
			 return;
		 case ER_UNKNOWN_CHAR:
			 printf("Unknown char\n");
			 return;
		 default:
			 printf("Scanner Error 1\n");
			 return;
		 } /* switch Error */


	 case SY_BECOMES:
		 printf(":=\n");
		 break;
	 case SY_COLON:
		 printf(":\n");
		 break;
	 case SY_LEQ:
		 printf("<=\n");
		 break;
	 case SY_NEQ:
		 printf("<>\n");
		 break;
	 case SY_LSS:
		 printf("<\n");
		 break;
	 case SY_GEQ:
		 printf(">=\n");
		 break;
	 case SY_GRT:
		 printf(">\n");
		 break;
	 case SY_PERIOD:
		 printf(".\n");
		 break;
	 case SY_LPARENT:
		 printf("(\n");
		 break;
	 case SY_POWER:
		 printf("**\n");
		 break;
	 case SY_SMUL:
		 printf("*\n");
		 break;
	 case SY_SLASH:
		 printf("/\n");
		 break;
	 case SY_PLUS:
		 printf("+\n");
		 break;
	 case SY_MINUS:
		 printf("-\n");
		 break;
	 case SY_EQL:
		 printf("=\n");
		 break;
	 case SY_RPARENT:
		 printf(")\n");
		 break;
	 case SY_COMMA:
		 printf(",\n");
		 break;
	 case SY_SEMICOLON:
		 printf(";\n");
		 break;
	 case SY_LBRACK:
		 printf("[\n");
		 break;
	 case SY_TILDA:
		 printf("~\n");
		 break;
	 case SY_RBRACK:
		 printf("]\n");
		 break;
	 case SY_LBRACKE:
		 printf("{\n");
		 break;
	 case SY_RBRACKE:
		 printf("}\n");
		 break;
	 case SY_PERSENT:
		 printf("%\n");
		 break;
	 case SY_THETA:
		 printf("@\n");
		 break;
	 case SY_PRINT:
		 printf("PRINT\n");
		 break;
	 case SY_FLOATCONS:
		 printf("Float    : %lf\n",scanner.sy.val.f);
		 break;
	 case SY_INTCONS:
		 printf("Int      : %li\n",scanner.sy.val.i);
		 break;
	 case SY_WORD:
		 printf("Word     : %s\n",scanner.sy.val.s);
		 break;
	 case SY_STRINGCONS:
		 printf("String   : \"%s\"\n",scanner.sy.val.s);
		 break;
	 case SY_VAR:
		 printf("VAR\n");
		 break;
	 case SY_INT:
		 printf("INT\n");
		 break;
	 case SY_FLOAT:
		 printf("FLOAT\n");
		 break;
	 case SY_VECTOR:
		 printf("VECTOR\n");
		 break;
	 case SY_VOID:
		 printf("VOID\n");
		 break;
	 case SY_FUNC:
		 printf("FUNC\n");
		 break;
	 case SY_LOOP:
		 printf("LOOP\n");
		 break;
	 case SY_RETURN:
		 printf("RETURN\n");
		 break;
	 case SY_BREAK:
		 printf("BREAK\n");
		 break;
	 case SY_GOTO:
		 printf("GOTO\n");
		 break;
	 case SY_IF:
		 printf("IF\n");
		 break;
	 case SY_THEN:
		 printf("THEN\n");
		 break;
	 case SY_ELSE:
		 printf("ELSE\n");
		 break;
	 case SY_MOD:
		 printf("MOD\n");
		 break;
	 case SY_FOR:
		 printf("FOR\n");
		 break;
	 case SY_TO:
		 printf("TO\n");
		 break;
	 case SY_DOWNTO:
		 printf("DOWNTO\n");
		 break;
	 default:
		 printf("Scanner error 3\n");
		 return;
	 }
    }
 }

 /*
int main(void)
 {
	char *s = " VAR INT FLOAT  VECTOR VOID\n"
			  "hello fucking world {}= := < <= <>";


	printf("\n\n\n");
	lex_Init(s);
	ScannerTest();
	return 0;
 }
 */
#endif

