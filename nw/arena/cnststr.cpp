/*
   File:  CNSTSTR.CPP
   Autor: Suavik
   Ver    1.06
 */
//#define str_USE_THROW str_ERROR_EXIT
#include <string.h>
#include <ctype.h>
#include "h/cnststr.h"

//#define str_MAX_DOUBLE     1.79769313486231570E+308
//#define str_MIN_DOUBLE     4.94065645841246544e-324

static const long str_minDouble[2] = {1,0};
#define str_MAX_DOUBLE       1.797693134862315E+308
#define str_MIN_DOUBLE       (*((double*)(str_minDouble)))
#define str_MAX_FLOAT        3.40282346638528860e+38
#define str_MAX_DOUBLE_EXP   308
#define str_MIN_DOUBLE_EXP   (-324)
#define str_MAX_DOUBLE_DIGIT 20

#define str_MAX_ULONG  ((unsigned long)(-1L))
#define str_MAX_LONG   (str_MAX_ULONG>>1)
#define str_MIN_LONG   (~str_MAX_LONG)

#define str_MAX_UINT   ((unsigned int)(-1))
#define str_MAX_INT    (str_MAX_UINT>>1)
#define str_MIN_INT    (~str_MAX_INT)

#define str_MAX_USHORT ((unsigned short)(-1))
#define str_MAX_SHORT  (str_MAX_USHORT>>1)
#define str_MIN_SHORT  (~str_MAX_SHORT)

#define str_MAX_UCHAR ((unsigned char)(-1))
#define str_MAX_CHAR  (str_MAX_UCHAR>>1)
#define str_MIN_CHAR  (~str_MAX_CHAR)

const unsigned long str_maxULong = str_MAX_ULONG;
const unsigned long str_maxLong  = str_MAX_LONG;
const unsigned long str_minLong  = str_MIN_LONG;

const unsigned long str_maxUInt  = str_MAX_UINT;
const unsigned long str_maxInt   = str_MAX_INT;
const unsigned long str_minInt   = str_MIN_INT;

const unsigned long str_maxUShort= str_MAX_USHORT;
const unsigned long str_maxShort = str_MAX_SHORT;
const unsigned long str_minShort = str_MIN_SHORT;

const unsigned long str_maxUChar = str_MAX_UCHAR;
const unsigned long str_maxChar  = str_MAX_CHAR;
const unsigned long str_minChar  = str_MIN_CHAR;


 //========================================================================
str_ConstStr &str_ConstStr::assign(  const char * str )
 {
    if(  str == NULL  )
    {
         str_THROW( str_ExceptionValueIsNULL(
                        "assign(cchar*): Value is NULL" ) );
         m_value = "";
         m_count = 0;
         return *this;
    }
    m_value    = str;
    m_count    = strlen(str);
    return *this;
 }



 //========================================================================
int str_ConstStr::indexOf( char c ) const
 {
    return indexOf( c, 0 );
 }

 //========================================================================
int str_ConstStr::indexOf( char c, int from ) const
 {
    int i;
    int len = length();

    if(  from == len  )
         return -1;

    if(  from < 0  ||  from > len  )
    {
         str_THROW( str_Exception( "indexOf(char,int): Index out of range" ) );
         return -1;
    }

    for( i = from; i < len; ++i )
         if(  m_value[i] == c  )
              return i;

    return -1;
 }

 //========================================================================
int str_ConstStr::indexOf( const char *str ) const
 {
    if(  str == NULL  )
    {
         str_THROW( str_ExceptionValueIsNULL(
                               "indexOf(cchar*): Parameter==NULL" ) );
         return -1;
    }

    return indexOf( str, 0 );
 }

 //========================================================================
int str_ConstStr::indexOf( const char *str, int from ) const
 {
    if(  str == NULL  )
    {
         str_THROW( str_ExceptionValueIsNULL(
                               "indexOf(cchar*,int): Parameter==NULL" ) );
         return -1;
    }
    str_ConstStr s(str);
    return indexOf( s, from );
 }

 //========================================================================
int str_ConstStr::indexOf( const str_ConstStr &str ) const
 {
    return indexOf( str, 0 );
 }

 //========================================================================
int str_ConstStr::indexOf( const str_ConstStr &str, int from ) const
 {
    const char *v1 = m_value;
    const char *v2 = str.m_value;
    int     count1 = length();
    int     count2 = str.length();
    int        max = count1 - count2;

    if(  from < 0  ||  from > count1  )
    {
         str_THROW( str_Exception( "indexOf(str,from): Index out of range" ));
         return -1;
    }

    for( int i = from; i <= max; ++i )
    {
		 int j = 0;
         for(j = 0; j < count2; ++j )
              if(  v1[i+j] != v2[j]  )
                   break;

         if(  j == count2  )
              return i;

    }

    return -1;
 }

 //========================================================================
int Rmatch( const char *s, int i, const char *p, int j )
 {
	if(  p[0] == 0  )
         return 1;

	for(;; ++i, ++j )
    {
		 if(  s[i] == 0  &&  p[j] == 0  )
              return 1;

         if(  p[j] == 0  )
              return 0;

         // End of string matched with '?' and '*' if pattern char is last.
         if(  s[i] == 0  )
              if(
                     ( p[j] == '?' || p[j] == '*' )
                  && ( p[j+1] == 0 )
                )
                   return 1;
              else return 0;

         if(  p[j] == '*'  )
         {
              // is last char ?
			  if(  p[j+1] == 0  )
                   return 1;

              for( int k = i ; s[k] != 0 ; ++k )
				   if(  Rmatch( s, k, p, j+1 )  )
                        return 1;
			  return 0;
		 }
         else
         if(  p[j] != '?'  &&  p[j] != s[i]  )
              return 0;
	}

 }

 //========================================================================
int str_ConstStr::match( const char *pattern, int from ) const
 {
    if(  from < 0  ||  from > length()  )
    {
         str_THROW( str_Exception( "match(cchar*,int): Index out of range" ) );
         return 0;
    }

    if(  pattern == NULL  )
    {
         str_THROW( str_ExceptionValueIsNULL( "match: Parameter is NULL" ) );
         return 0;
    }

    return Rmatch( m_value, from , pattern, 0 );
 }

 //========================================================================
int str_ConstStr::trim( int from ) const
 {
    int i;

    if(  from < 0  ||  from > length()  )
    {
         str_THROW( str_Exception( "trim(): Index out of range" ) );
         return from;
    }

    if(  from == length()  )
         return from;

    for( i = from;; ++i )
    {
         switch( m_value[i] )
         {
         case 0: return i;
         case ' ':
         case '\t':
         case '\v':
         case '\b':
         case '\f':
         case '\r':
         case '\n': continue;
         default:
                return i;
         }
    }
 }

 //========================================================================
int str_ConstStr::to(
                     unsigned long &dest,
                     unsigned long radix,
                     int           from
                    ) const
 {
    unsigned long v = 0,
             maxDigit;
    dest = 0;

    switch( radix )
    {
    case 2:  maxDigit = str_maxULong / 2UL;  break;
    case 8:  maxDigit = str_maxULong / 8UL;  break;
    case 10: maxDigit = str_maxULong / 10UL; break;
    case 16: maxDigit = str_maxULong / 16UL; break;
    default:
           str_THROW( str_Exception( "to(ulong): Not supported radix" ) );
           return 0;
    }

    int  i = from;

    for(;; ++i )
    {
         char c = m_value[i];
         if(
               (c < '0' || c > '9')
            && (c < 'a' || c > 'f')
            && (c < 'A' || c > 'F')
           )
             break;

         if(  v > maxDigit  )
         {
              str_THROW( str_ExceptionValueTooBig("to(ulong): Value too big") );
              return from;
         }

         v *= radix;

         unsigned long add;

         if(  radix == 16  )
         {
              if(  c >= '0'  &&  c <= '9'  )
                   add = (unsigned long)  ((unsigned char)(c-'0'));
              else
              if(  c >= 'a'  &&  c <= 'f'  )
                   add = (unsigned long)  ((unsigned char)(c-'a'+10));
              else add = (unsigned long)  ((unsigned char)(c-'A'+10));
         }
         else
         {
              if(  c > '9'  )
                   break;
              else add = (unsigned long)  ((unsigned char)(c-'0'));

              if(  add >= radix  )
              {
                   str_THROW( str_Exception( "to(ulong): Digit too big with this radix" ) );
                   return from;
              }
         }

         if(  v > str_maxULong - add  )
         {
              str_THROW( str_ExceptionValueTooBig("to(ulong): Value too big") );
              return from;
         }

         v += add;
    }

    dest = v;

    return i;
 }

 //========================================================================
int str_ConstStr::to( unsigned long &dest, int from ) const
 {
    char c;
    int  newIndex;

    dest = 0;

    if(  from < 0  ||  from >= length()  )
    {
         str_THROW( str_Exception( "to(ulong,int): Index out of range" ) );
         return from;
    }

    int i = trim( from );

    if(  m_value[i] == '0'  )
    {

         switch( m_value[i+1] )
         {
         case 'x': case 'X':
              c = m_value[i+2];

              if(   (c < '0' || c > '9' )
                 && (c < 'a' || c > 'f' )
                 && (c < 'A' || c > 'F' ) )
              {
                   str_THROW( str_ExceptionIsNotNumber("to(ulong,int): Is not number") );
                   return from;
              }

              newIndex = to( dest, 16, i+2 );

              if(  newIndex == i+2  ) // Is Error detected?
                   return from;

              return newIndex;

         case 'b': case 'B':
              c = m_value[i+2];

              if(  c < '0' ||  c > '1'   )
              {
                   str_THROW( str_ExceptionIsNotNumber("to(ulong,int): Is not number") );
                   return from;
              }

              newIndex = to( dest,  2, i+2 );

              if(  newIndex == i+2  ) // Is Error detected?
                   return from;

              return newIndex;

         case 'o': case 'O':
              c = m_value[i+2];

              if(  c < '0'  ||  c > '7'  )
              {
                   str_THROW( str_ExceptionIsNotNumber("to(ulong,int): Is not number") );
                   return from;
              }

              newIndex = to( dest,  8, i+2 );

              if(  newIndex == i+2  ) // Is Error detected?
                   return from;

              return newIndex;


         case '0': case '1': case '2': case '3':
         case '4': case '5': case '6': case '7':
              newIndex = to( dest,  8, i+1 );

              if(  newIndex == i+1  ) // Is Error detected?
                   return from;

              return newIndex;


         default:
              dest = 0;
              return from+1;
         }
    }

    c = m_value[ i ];
    if(  c < '0'  ||  c > '9'  )
    {
         str_THROW( str_ExceptionIsNotNumber("to(ulong,int): Is not number") );
         return from;
    }

    newIndex = to( dest, 10, i );

    if(  newIndex==i  )
         return from;

    return newIndex;
 }

 //========================================================================
int str_ConstStr::useSignum( int &from ) const
 {
    int haveSig = 0;

    from = trim(from);

    switch( m_value[from] )
    {
    case '+': ++from;              break;
    case '-': ++from; haveSig = 1; break;
    }

    return haveSig;
 }

 //========================================================================
int str_ConstStr::to( long &dest, int from ) const
 {
    unsigned long v;

    dest = 0;

    if(  from < 0  ||  from >= length()  )
    {
         str_THROW( str_Exception( "to(long,int) Index out of range" ) );
         return from;
    }

    int i        = from;
    int haveSig  = useSignum( i );
    int newIndex = to( v, i );

    if(  newIndex == i  )
         return from;

    if(  haveSig  )
    {
         if(  v > str_minLong  )
         {
              str_THROW( str_ExceptionValueTooBig("to(long,int): Value too low") );
              return from;
         }
         dest = -((long)v);
    }
    else
    {
         if(  v > str_maxLong  )
         {
              str_THROW( str_ExceptionValueTooBig("to(long,int): Value too big") );
              return from;
         }
         dest = ((long)v);
    }


    return newIndex;
 }

 //========================================================================
int str_ConstStr::to( unsigned short &dest, int from ) const
 {
    unsigned long v;

    dest = 0;

    if(  from < 0  ||  from >= length()  )
    {
         str_THROW( str_Exception( "to(ushort,int) Index out of range" ) );
         return from;
    }

    int i        = from;
    int newIndex = to(v,i);

    if(  newIndex == i  )
         return from;

    if(  v >  str_maxUShort  )
    {
         str_THROW( str_ExceptionValueTooBig("to(ushort): Value too big") );
         return from;
    }

    dest = (unsigned short)v;

    return newIndex;
 }

 //========================================================================
int str_ConstStr::to( short &dest, int from ) const
 {
    unsigned long  v;

    dest = 0;

    if(  from < 0  ||  from >= length()  )
    {
         str_THROW( str_Exception( "to(short,int) Index out of range" ) );
         return from;
    }

    int i        = from;
    int haveSig  = useSignum( i );
    int newIndex = to( v, i );

    if(  newIndex == i  )
         return from;

    if(  haveSig  )
    {
         if(  v > str_minShort  )
         {
              str_THROW( str_ExceptionValueTooBig("to(short): Value too low") );
              return from;
         }
         dest = ((short)-v);
    }
    else
    {
         if(  v > str_maxShort  )
         {
              str_THROW( str_ExceptionValueTooBig("to(short): Value too big") );
              return from;
         }
         dest = ((short)v);
    }

    return newIndex;
 }

 //========================================================================
int str_ConstStr::to( unsigned int  &dest, int from ) const
 {
    unsigned long v;

    dest = 0;

    if(  from < 0  ||  from >= length()  )
    {
         str_THROW( str_Exception( "to(uint,int) Index out of range" ) );
         return from;
    }

    int i        = from;
    int newIndex = to( v, i );

    if(  newIndex == i  )
         return from;

    if(  v > str_maxUInt  )
    {
         str_THROW( str_ExceptionValueTooBig("to(uint): Value too big") );
         return from;
    }

    dest = (unsigned int)v;

    return newIndex;
 }

 //========================================================================
int str_ConstStr::to( int  &dest, int from ) const
 {
    unsigned long v;

    dest = 0;

    if(  from < 0  ||  from >= length()  )
    {
         str_THROW( str_Exception( "to(int,int) Index out of range" ) );
         return from;
    }

    int i        = from;
    int haveSig  = useSignum( i );
    int newIndex = to( v, i );

    if(  newIndex == i  )
         return from;

    if(  haveSig  )
    {
         if(  v > str_minInt  )
         {
              str_THROW( str_ExceptionValueTooBig("to(int): Value too low"));
              return from;
         }
         dest = -((int)v);
    }
    else
    {
         if(  v > str_maxInt  )
         {
              str_THROW( str_ExceptionValueTooBig("to(int): Value too big"));
              return from;
         }
         dest = ((int)v);
    }

    return newIndex;
 }

 //========================================================================
int str_ConstStr::to( unsigned char &dest, int from ) const
 {
    unsigned long v;

    dest = 0;

    if(  from < 0  ||  from >= length()  )
    {
         str_THROW( str_Exception( "to(uchar,int) Index out of range" ) );
         return from;
    }

    int i        = from;
    int newIndex = to(v,i);

    if(  newIndex == i  )
         return from;

    if(  v > str_maxUChar  )
    {
         str_THROW( str_ExceptionValueTooBig("to(uchar): Value too big") );
         return from;
    }

    dest = (unsigned char)v;

    return newIndex;
 }

 //========================================================================
int str_ConstStr::to( char &dest, int from ) const
 {
    unsigned long v;

    dest = 0;

    if(  from < 0  ||  from >= length()  )
    {
         str_THROW( str_Exception( "to(char,int) Index out of range" ) );
         return from;
    }

    int i        = from;
    int haveSig  = useSignum( i );
    int newIndex = to( v, i );

    if(  newIndex == i  )
         return from;

    if(  haveSig  )
    {
         if(  v > str_minChar  )
         {
              str_THROW( str_ExceptionValueTooBig("to(char): Value too low"));
              return from;
         }
         dest = ((char)-v);
    }
    else
    {
         if(  v > str_maxChar  )
         {
              str_THROW( str_ExceptionValueTooBig("to(char): Value too big"));
              return from;
         }
         dest = ((char)v);
    }


    return newIndex;
 }

 //========================================================================
double str_GetDigit( double result, char c, int &error )
 {
    error = 0;
    static
    double digits[]     = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    if(  result > str_MAX_DOUBLE / 10.0  )
    {
         str_THROW( str_ExceptionValueTooBig( "to(double): Value too big" ) );
         error = 1;
         return str_MAX_DOUBLE;
    }
    result *= 10;
    double digit = digits[c - '0'];

    if(  result > str_MAX_DOUBLE - digit  )
    {
         str_THROW( str_ExceptionValueTooBig( "to(double): Value too big" ) );
         error = 1;
         return str_MAX_DOUBLE;
    }

    return result + digit;
 }

 //========================================================================
int str_ConstStr::to( double &dest, int from ) const
 {
    int index = from,
        newIndex;

    dest = 0;

    if(  from < 0  ||  from >= length()  )
    {
         str_THROW( str_Exception( "to(double,int) Index out of range" ) );
         return from;
    }


    static
    double powerof10[]  = { 1.e1, 1.e2, 1.e4, 1.e8, 1.e16, 1.e32,
                            1.e64,1.e128,1.e256 };
    static
    double powerofm10[] = { 1.e-1, 1.e-2,  1.e-4, 1.e-8, 1.e-16, 1.e-32,
                            1.e-64,1.e-128,1.e-256 };

    int exp     = 0;               /* exponent              */
    int i;
    int haveSig = useSignum( index );
    int parse   = 0;
    /*
     *  Load first str_MAX_DOUBLE_DIGIT digits
     */
    index = trim(index);
    newIndex = index;

    double      result = 0;

    for( i = 0;    i < str_MAX_DOUBLE_DIGIT
                && isdigit( m_value[index] ) ; ++index, ++i )
    {
         int isError;
         result = str_GetDigit( result, m_value[index], isError );

         if(  isError  )
              return from;

         parse  = 1;
    }


    /*
     * Load last chars
     */
    for(; isdigit( m_value[index] ); ++index )
    {
         ++exp;
         parse = 1;
    }


    if(  m_value[index] == '.' )
         for( ++index; isdigit( m_value[index] ); ++index, --exp )
         {
              int isError;
              result = str_GetDigit( result, m_value[index], isError );

              if(  isError  )
                   return from;

              parse  = 1;
         }

    if(  !parse  )
    {
         str_THROW( str_Exception("do(double,int): Is not double value") );
         return from;
    }

    /*
     *	Read in explicit exponent and calculate real exponent.
     */
    if(  m_value[index] == 'E' ||  m_value[index] == 'e'  )
    {
         int expSign;
         int count  = 0;
         ++index;

         if(    (expSign = (m_value[index] == '-')) != 0
             ||             m_value[index] == '+' )
	          ++index;

         for( i = count = 0 ;
                isdigit( m_value[index] ) && i < 3;
                  ++i, ++index )
              count = count*10 + m_value[index] - '0';

         if(  isdigit(m_value[index])  )
         {
              str_THROW( str_ExceptionValueTooBig( "to(double): Exponent too big" ) );
              return from;
         }

         if(  expSign != 0  )
              exp -= count;
         else exp += count;
    }

    /*
     * ADJUST NUMBER BY POWERS OF TEN SPECIFIED BY FORMAT AND EXPONENT.
     */
    if(  result != 0.0  )
    {
         if(  exp < 0  )
         {
	          for( i = 0, exp = -exp;   exp  ; ++i, exp >>= 1 )
		           if(  exp & 1  )
                   {
                        if(  result < str_MIN_DOUBLE * powerof10[i]  )
                        {
                             str_THROW( str_ExceptionValueTooBig( "to(double): Value too low" ) );
                             return from;
                        }
                        result *= powerofm10[i];
                   }
         }
         else
         {
              for( i = 0 ;   exp ; ++i, exp >>= 1 )
                   if(  exp & 1  )
                   {
                        if(  result > str_MAX_DOUBLE * powerofm10[i]  )
                        {
                             str_THROW( str_ExceptionValueTooBig( "to(double): Value too big" ) );
                             return from;
                        }
                        result *= powerof10[i];
                   }
         }
    }

    if(  haveSig  )
         result = -result;

    if(  newIndex == index  )
         return from;

    dest = result;

    return index;
 }

 //========================================================================
int str_ConstStr::to( float &dest, int from ) const
 {
    double v;
    int newIndex = to(v,from);

    if(  newIndex == from  )
         return from;

    if(  v > str_MAX_FLOAT  )
    {
         str_THROW( str_ExceptionValueTooBig( "to(float): Value too big" ) );
         return from;
    }
    else
    if(  v < -str_MAX_FLOAT  )
    {
         str_THROW( str_ExceptionValueTooBig( "to(float): Value too low" ) );
         return from;
    }

    dest = (float)v;

    return newIndex;
 }

 //========================================================================
int str_ConstStr::to( unsigned long  &dest ) const { return to(dest,0); }
int str_ConstStr::to( long           &dest ) const { return to(dest,0); }

int str_ConstStr::to( unsigned short &dest ) const { return to(dest,0); }
int str_ConstStr::to( short          &dest ) const { return to(dest,0); }

int str_ConstStr::to( unsigned int   &dest ) const { return to(dest,0); }
int str_ConstStr::to( int            &dest ) const { return to(dest,0); }

int str_ConstStr::to( unsigned char  &dest ) const { return to(dest,0); }
int str_ConstStr::to( char           &dest ) const { return to(dest,0); }

int str_ConstStr::to( double         &dest ) const { return to(dest,0); }
int str_ConstStr::to( float          &dest ) const { return to(dest,0); }


 //========================================================================
int str_ConstStr::get( const char *pattern, int from ) const
 {
    int i;

    if(  pattern == NULL  )
    {
         str_THROW( str_ExceptionValueIsNULL( "get(): String is NULL" ) );
         return from;
    }

    if(  from < 0  ||  from > length()  )
    {
         str_THROW( str_Exception( "get(cchar*,int): Index out of range" ) );
         return from;
    }

    for( i = 0;  m_value[from+i] == pattern[i]  ; ++i )
         if(  pattern[i] == 0  )
              return from+i;

    if(  pattern[i] == 0  )
         return from+i;

    return from;
 }

 //========================================================================
int str_ConstStr::get( const char   *pattern )       const
 {
    if(  pattern == NULL  )
    {
         str_THROW( str_ExceptionValueIsNULL( "get(cchar*): pattern==NULL" ) );
         return 0;
    }
    return get(pattern,0);
 }
int str_ConstStr::get( const str_ConstStr &pattern ) const{ return get(pattern,0); }


 //========================================================================
int str_ConstStr::cmp( const char *str ) const
 {
    if(  str == NULL  )
    {
         str_THROW( str_ExceptionValueIsNULL( "cmp(cchar*): pattern==NULL" ) );
         return 2;
    }

    return strcmp( m_value, str );
 }

 //========================================================================
int str_ConstStr::cmp( const str_ConstStr &str ) const
 {
    return cmp( str.m_value );
 }

 //========================================================================
unsigned long str_ConstStr::hashCode() const
 {
    int  i;
    unsigned long val = 0;

    for( i = 0; m_value[i] !=0 ; ++i )
    {
         unsigned long c = (unsigned char)(m_value[i]);
         val += i;
         val += c;
         val ^= ~(c << 4);
         val -= c >> 4;
    }
    val &= 0xFFFFul;

    val |= ((val^0xFACEul) << 16);

    return val;
 }

 //========================================================================
int   isCorrectHashCode( unsigned long hashCode )
 {
    return ((hashCode>>16)^0xFACEul) == (hashCode&0xFFFFul);
 }

 //========================================================================
int str_ConstStr::operator == ( const char *str ) const
 {
    if(  str == NULL  )
    {
         str_THROW( str_ExceptionValueIsNULL("operator==(NULL)") );
         return 0;
    }

    return strcmp( m_value, str ) == 0;
 }

 //========================================================================
int str_ConstStr::operator == ( const str_ConstStr &str ) const
 {
    return strcmp( m_value, str.m_value ) == 0;
 }

 //========================================================================
int operator ==( const char *str1, const str_ConstStr &str2 )
 {
    if(  str1 == NULL  )
    {
         str_THROW( str_ExceptionValueIsNULL("operator==(NULL)") );
         return 0;
    }
    return strcmp( str1, str2.m_value ) == 0;
 }

 //========================================================================
int str_ConstStr::operator != ( const char *str )         const
 {
    if(  str == NULL  )
    {
         str_THROW( str_ExceptionValueIsNULL("operator!=(NULL)") );
         return 0;
    }

    return strcmp( m_value, str ) != 0;
 }

 //========================================================================
int str_ConstStr::operator != ( const str_ConstStr &str ) const
 {
    return strcmp( m_value, str.m_value ) != 0;
 }

 //========================================================================
int operator !=( const char *str1, const str_ConstStr &str2 )
 {
    if(  str1 == NULL  )
    {
         str_THROW( str_ExceptionValueIsNULL("operator!=(NULL)") );
         return 0;
    }
    return strcmp( str1, str2.m_value ) != 0;
 }

 //========================================================================
int str_ConstStr::operator <  ( const str_ConstStr &str ) const
 {
    return strcmp(m_value,str.m_value) < 0;
 }

 //========================================================================
int str_ConstStr::operator >  ( const str_ConstStr &str ) const
 {
    return strcmp(m_value,str.m_value) > 0;
 }

 //========================================================================
int str_ConstStr::operator <= ( const str_ConstStr &str ) const
 {
    return strcmp(m_value,str.m_value) <= 0;
 }

 //========================================================================
int str_ConstStr::operator >= ( const str_ConstStr &str ) const
 {
    return strcmp(m_value,str.m_value) >= 0;
 }

 //========================================================================
int IsInt( const str_ConstStr &str, int from )
 {
    char c = str[from];

    if(  c == '+'  ||  c == '-'  )
         from = str.trim(from+1);

    if(  str[from] == '0'  )
    {
         switch( str[from+1] )
         {
         case 'x': case 'X':
         case 'b': case 'B':
         case 'o': case 'O': return 1;
         case '0': return 1;
         case '.': return 0;
         }
    }

    for(; isdigit( str[from] ) ; ++from );

    switch( str[from] )
    {
    case '.': case 'e': case 'E':
            return 0;
    }

    return 1;
 }

 //========================================================================
int IsFloat( const str_ConstStr &str, int from )
 {
    char c = str[from];

    if(  c == '+' ||  c == '-'  )
         from = str.trim(from+1);

    if(  str[from] == '0'  )
    {
         switch( str[from+1] )
         {
         case 'x': case 'X':
         case 'b': case 'B':
         case 'o': case 'O': return 0;
         case '0': return 0;
         case '.': return 1;
         }
    }

    for(; isdigit( str[from] ) ; ++from );

    switch( str[from] )
    {
    case '.': case 'e': case 'E':
            return 1;
    }

    return 0;
 }

 //========================================================================
str_ConstStr::STR_TYPE str_ConstStr::query( int from ) const
 {
    if(  from < 0  ||  from >= length()  )
    {
         //str_THROW( str_Exception( "query(): Index out of range" ) );
         return STR_OUTOFRANGE;
    }

    switch( charAt(from) )
    {
    case 0:     return STR_EOL;
    case '\t':
    case '\v':
    case '\b':
    case '\f':
    case '\r':
    case ' ':
    case '\n': return STR_SEPARATOR;

    case '+': case '-':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
               if(  IsInt(*this,from)  )
                    return STR_INT;

               if(  IsFloat(*this,from)  )
                    return STR_FLOAT;

               break;

    case '.':  if(  IsFloat(*this,from)  )
                    return STR_FLOAT;
               break;
    }
    return STR_OTHER;
 }

 /**************************
  *
  *         EVAL
  *
  **************************/


 //========================================================================
#define str_SYNTAXERROR(lexLen,errStr,func) newIndex = func;       \
     if( newIndex == index+lexLen )                                 \
     {                                                              \
          str_THROW( str_ExceptionEval("eval(int)" errStr) );       \
          return from;                                              \
     }                                                              \
     index = newIndex;
 //========================================================================
int str_Expr( const str_ConstStr &str, long &val, int from )
 {
    int index = str.trim( from );

    switch( str.query( index ) )
    {
    case str_ConstStr::STR_EOL:
            return from;

    case str_ConstStr::STR_INT:
            return str.to( val, index );

    case str_ConstStr::STR_FLOAT:
            str_THROW( str_ExceptionEval("eval(int): Expected int value" ));
            return from;

    case str_ConstStr::STR_OTHER:
            if(  str.charAt(index) == '('  )
            {
                 int newIndex;
                 str_SYNTAXERROR(1,"Syntax error",str.eval( val, index+1 ))
                 index = str.trim(index);

                 if(  str.charAt(index) != ')'  )
                 {
                      str_THROW( str_ExceptionEval( "eval(int): "
                                                    "Expected ')'" ));
                      return from;
                 }

                 return index + 1;
            }
    }

    return from;
 }


 //========================================================================
int str_UnaExpr( const str_ConstStr &str, long &val, int from )
 {
    int  index = from;
    int  newIndex;
    char op;

    index = str.trim(index);

    switch( op = str.charAt(index) )
    {
    case '+':
    case '-':
    case '!':
    case '~':
            str_SYNTAXERROR( 1,"Expected expression",
                             str_UnaExpr( str, val, index+1 ))

            switch( op )
            {
            case '+':             break;
            case '-': val = -val; break;
            case '!': val = !val; break;
            case '~': val = ~val; break;
            }
            break;

    default:
            str_SYNTAXERROR( 0,"Expected expression",
                             str_Expr( str, val, index ))
    }

    return index;
 }

 //========================================================================
int str_MulExpr( const str_ConstStr &str, long &val, int from )
 {
    long val2  = 0;
    int  index = str_UnaExpr( str, val, from );
    int  newIndex;
    char op;

    for(;;)
    {
         index = str.trim( index );

         switch( op = str.charAt(index) )
         {
         case '*':
         case '/':
         case '%':
                  str_SYNTAXERROR(1,"Expected expression",
                                 str_UnaExpr( str, val2, index+1 ))

                  switch( op )
                  {
                  case '*': val *= val2; break;
                  case '/':
                            if(  val2==0  )
                            {
                                 str_THROW( str_ExceptionEval( "eval(int)"
                                            "Division by 0"  ));
                                 return from;
                            }
                            val /= val2;
                            break;

                  case '%':
                            if(  val2==0  )
                            {
                                 str_THROW( str_ExceptionEval( "eval(int)"
                                            "Division by 0"  ));
                                 return from;
                            }
                            val %= val2;
                            break;
                  }
                  break;

         default: return index;
         }
    }
 }

 //========================================================================
int str_SumExpr( const str_ConstStr &str, long &val, int from )
 {
    long val2  = 0;
    int  index = from;
    int  newIndex;
    char op;

    index = str_MulExpr( str, val, index );

    for(;;)
    {
         index = str.trim( index );

         switch( op = str.charAt(index) )
         {
         case '+':
         case '-':
                  str_SYNTAXERROR( 1,"Expected expression",
                                   str_MulExpr( str, val2, index+1 ))

                  switch( op )
                  {
                  case '+': val += val2; break;
                  case '-': val -= val2; break;
                  }
                  break;

         default: return index;
         }
    }
 }


 //========================================================================
int str_ShiftExpr( const str_ConstStr &str, long &val, int from )
 {
    long val2  = 0;
    int  index = from;
    int  newIndex;


    index = str_SumExpr( str, val, index );

    for(;;)
    {
         index = str.trim( index );

         if(  str.get("<<",index) != index  )
         {
              str_SYNTAXERROR( 2,"Expected expression",
                               str_SumExpr( str, val2, index+2 ))

              val <<= val2;
         }
         else
         if(  str.get(">>",index) != index  )
         {
              str_SYNTAXERROR( 2,"Expected expression",
                               str_SumExpr( str, val2, index+2 ))

              val >>= val2;
         }
         else return index;
    }
 }

 //========================================================================
int str_CompExpr( const str_ConstStr &str, long &val, int from )
 {
    long val2  = 0;
    int  index = from;
    int  newIndex;
    char op;

    index = str_ShiftExpr( str, val, index );

    for(;;)
    {
         index = str.trim( index );

         if(  str.get("<=",index) != index  )
         {
              str_SYNTAXERROR( 2,"Expected expression",
                               str_ShiftExpr( str, val2, index+2 ))

              val = val <= val2;
         }
         else
         if(  str.get(">=",index) != index  )
         {
              str_SYNTAXERROR( 2,"Expected expression",
                               str_ShiftExpr( str, val2, index+2 ))

              val = val >= val2;
         }
         else
              switch( op = str.charAt(index) )
              {
              case '>':
              case '<':
                      str_SYNTAXERROR( 1,"Expected expression",
                                       str_ShiftExpr( str, val2, index+1 ))

                      switch( op )
                      {
                      case '>': val = val > val2; break;
                      case '<': val = val < val2; break;
                      }
                      break;

              default: return index;
              }
    }
 }

 //========================================================================
int str_EquExpr( const str_ConstStr &str, long &val, int from )
 {
    long val2  = 0;
    int  index = from;
    int  newIndex;

    index = str_CompExpr( str, val, index );

    for(;;)
    {
         index = str.trim( index );

         if(  str.get("==",index) != index  )
         {
              str_SYNTAXERROR( 2,"Expected expression",
                               str_CompExpr( str, val2, index+2 ))
              val = val == val2;
         }
         else
         if(  str.get("!=",index) != index  )
         {
              str_SYNTAXERROR( 2,"Expected expression",
                               str_CompExpr( str, val2, index+2 ))
              val = val != val2;
         }
         else return index;
    }
 }

 //========================================================================
int str_BitAndExpr( const str_ConstStr &str, long &val, int from )
 {
    long val2  = 0;
    int  index = from;
    int  newIndex;

    index = str_EquExpr( str, val, index );

    for(;;)
    {
         index = str.trim( index );

         if(     str.get("&&",index) == index
             &&  str.charAt(index)   == '&' )
         {
              str_SYNTAXERROR( 1,"Expected expression",
                               str_EquExpr( str, val2, index+1 ))
              val &= val2;
         }
         else return index;
    }
 }

 //========================================================================
int str_BitXorExpr( const str_ConstStr &str, long &val, int from )
 {
    long val2  = 0;
    int  index = from;
    int  newIndex;

    index = str_BitAndExpr( str, val, index );

    for(;;)
    {
         index = str.trim( index );

         if(  str.charAt(index) == '^'  )
         {
              str_SYNTAXERROR( 1, "Expected expression",
                               str_BitAndExpr( str, val2, index+1 ))
              val ^= val2;
         }
         else return index;
    }
 }

 //========================================================================
int str_BitOrExpr( const str_ConstStr &str, long &val, int from )
 {
    long val2  = 0;
    int  index = from;
    int  newIndex;


    index = str_BitXorExpr( str, val, index );

    for(;;)
    {
         index = str.trim( index );

         if(     str.get("||",index) == index
             &&  str.charAt(index)   == '|' )
         {
              str_SYNTAXERROR( 1, "Expected expression",
                               str_BitXorExpr( str, val2, index+1 ));
              val |= val2;
         }
         else return index;
    }
 }

 //========================================================================
int str_AndExpr( const str_ConstStr &str, long &val, int from )
 {
    long val2  = 0;
    int  index = from;
    int  newIndex;

    index = str_BitOrExpr( str, val, index );

    for(;;)
    {
         index = str.trim( index );

         if(  str.get("&&",index) != index  )
         {
              str_SYNTAXERROR( 2, "Expected expression",
                               str_BitOrExpr( str, val2, index+2 ))
              val = val && val2;
         }
         else return index;
    }
 }

 //========================================================================
int str_OrExpr( const str_ConstStr &str, long &val, int from )
 {
    long val2 = 0;
    int  index = from;
    int  newIndex;

    index = str_AndExpr( str, val, index );

    for(;;)
    {
         index = str.trim( index );

         if(  str.get("||",index) != index  )
         {
              str_SYNTAXERROR( 2, "Expected expression",
                               str_AndExpr( str, val2, index+2 ));
              val = val || val2;
         }
         else return index;
    }
 }

 //========================================================================
int str_IfExpr( const str_ConstStr &str, long &val, int from )
 {
    long val2 = 0,
         val3 = 0;
    int  index = from;
    int  newIndex;

    str_SYNTAXERROR( 0,"Expected expression",
                     str_OrExpr( str, val, index ) )
    index = str.trim( index );

    if(  str.charAt( index ) == '?'  )
    {
         str_SYNTAXERROR( 1,"Expected expression",
                          str_OrExpr( str, val2, index+1 ) )

         index = str.trim( index );

         if(  str.charAt(index) != ':'  )
         {
              str_THROW( str_ExceptionEval("eval(): "
                                           "Past operator '?' expected ':'"));
              return from;
         }
         str_SYNTAXERROR( 1,"Expected expression",
                          str_OrExpr( str, val3, index+1 ) )

         if(  val!=0  )
              val = val2;
         else val = val3;
    }

    return index;
 }

 //========================================================================
int str_ConstStr::eval( long &val, int from )  const
 {
    val = 0;
    return str_IfExpr( *this, val, from );
 }

#if str_USE_THROW==str_ERROR_TEST

int ErrDetect()
 {
    if(  g_curExcept!=NULL  )
    {
         printf("Exception: %s\n",g_curExcept->m_errorStr );
         delete g_curExcept;
         g_curExcept = NULL;
         return 1;
    }
    return 0;
 }

static void str_TestConstStr()
 {
    printf("\n===============StartTest\n\n");
    str_ConstStr str;
    int i = 0;
    //int useSignum( int &from ) const;

    printf("useSignum+[1]==%i\n",str.assign("  + ").useSignum(i));
    i = 0;
    printf("useSignum-[1]==%i\n",str.assign("  - ").useSignum(i));


    //const char    *getChars()            const;            // inline
    printf("getChars()   ==%s\n",str.assign("getChars()").getChars());

    //char           charAt  ( int index ) const;            // inline
    printf("charAt()  [A]==%c\n",str.assign("mAma").charAt(1));

    //int            length  ()            const;            // inline
    printf("length()  [5]==%i\n",str.assign("hello").length());

    //unsigned long  hashCode() const;
    printf("hashCode(): Numbers must been different:\n");
    printf("   %lX %lX %lX %lX\n",
              str.assign("Hello").hashCode(),
              str.assign("Helloo").hashCode(),
              str.assign("Helol").hashCode(),
              str.assign("Hell").hashCode(),
              str.assign("world").hashCode()
          );
    //friend   int   isCorrectHashCode( unsigned long hashCode );
    printf("isCorrectHashCode() %i==%i\n",
               isCorrectHashCode(str.assign("pussy").hashCode()),
               !isCorrectHashCode(0)
             );
    //operator const char *() const;                         // inline
    printf("operator char*==%s\n",(const char*)str.assign("operator char*"));

    //str_ConstStr &assign( const str_ConstStr &str );       // inline
    str_ConstStr str2;

    str.assign("assign");
    str2.assign(str);
    printf("assign() %s==%s\n", (const char *)str,(const char *)str2 );

    //str_ConstStr();                                        // inline
    //str_ConstStr( const char * str );                      // inline
    //str_ConstStr( const str_ConstStr &str );               // inline
    str_ConstStr str3,
                 str4("Hello"),
                 str5(str);

    //int indexOf( char c )                           const;
    printf("indexOf(char) 2==%i\n", str.assign("Hello").indexOf('l'));

    //int indexOf( char c, int from )                 const;
    printf("indexOf(char,int) 4==%i\n", str.indexOf('o',2));
    printf("indexOf(char,int) 4==%i\n", str.indexOf('o',4));
    printf("indexOf(char,int)-1==%i\n", str.indexOf('o',5));

    //int indexOf( const char *str )                  const;
    printf("indexOf(cchar*) 3==%i\n",   str.indexOf("lo"));
    printf("indexOf(cchar*) 2==%i\n",   str.indexOf("llo"));
    printf("indexOf(cchar*)-1==%i\n",   str.indexOf("ol"));

    //int indexOf( const char *str, int from )        const;
    printf("indexOf(cchar*,int)  2==%i\n",     str.indexOf("llo",1));
    printf("indexOf(cchar*,int)  2==%i\n",     str.indexOf("llo",2));
    printf("indexOf(cchar*,int) -1==%i\n",     str.indexOf("llo",3));
    printf("indexOf(cchar*,int) ??==%i\n",     str.indexOf("",3));
    printf("indexOf(cchar*,int) -1==%i\n",     str.indexOf("Hello!",0));

    //int indexOf( const str_ConstStr &str )          const;
    printf("indexOf(str) 3==%i\n",   str.indexOf(str_ConstStr("lo")));
    printf("indexOf(str) 2==%i\n",   str.indexOf(str_ConstStr("llo")));
    printf("indexOf(str)-1==%i\n",   str.indexOf(str_ConstStr("ol")));
    //int indexOf( const str_ConstStr &str, int from )const;
    printf("indexOf(str,int)  2==%i\n",     str.indexOf(str_ConstStr("llo"),1));
    printf("indexOf(str,int)  2==%i\n",     str.indexOf(str_ConstStr("llo"),2));
    printf("indexOf(str,int) -1==%i\n",     str.indexOf(str_ConstStr("llo"),3));
    printf("indexOf(str,int) ??==%i\n",     str.indexOf(str_ConstStr(""),3));
    printf("indexOf(str,int) -1==%i\n",     str.indexOf(str_ConstStr("Hello!"),0));

    //int match( const char *patter     , int from ) const;
    printf("match(cchar*,int) 1=%i\n", str.match("?e*o",0));
    printf("match(cchar*,int) 0=%i\n", str.match("?e*o",1));
    printf("match(cchar*,int) 1=%i\n", str.match("e*o",1));
    printf("match(cchar*,int) 1=%i\n", str.match("Hello",0));
    printf("match(cchar*,int) 0=%i\n", str.match("Helloo",0));
    printf("match(cchar*,int) 0=%i\n", str.match("Hell",0));

    str.assign("HJGjkhg hjkg jkhgyJK ghhjkg jkhgjhsdf;ergoreo9uerodosivjkvh");
    static char buf0[100];
    int j;

    for( i = 0; i < 1000; ++i )
    {
          int cnt = ((i^0xFACE)*37);

          strcpy(buf0,str.getChars());

          if( cnt < 0 ) cnt = -cnt;

          cnt %= 40;

          for( j = 0 ; j < cnt; ++j )
          {
               int pos = ((i ^ j)*37+i);
               if( pos < 0 ) pos = -pos;

               buf0[ pos % str.length() ] = '?';
          }

          cnt += 13;
          cnt *= 37;

          if( cnt < 0 ) cnt = -cnt;

          cnt %= 40;

          for( j = 0 ; j < cnt; ++j )
          {
               int pos  = (((i*j)^0xBEDA)*37);
               int qnty = (j*7^i);

               if( pos < 0 ) pos = -pos;
               if( qnty< 0 ) qnty= -qnty;

               pos %= strlen(buf0);
               qnty%= strlen(buf0)/8;

               pos -= 1;
               if( pos < 0 ) continue;


               if( pos + qnty >= strlen(buf0) )
                    qnty = strlen(buf0) - pos - 1;

               if( qnty > 0 )
               {
                   int ii;

                   for( ii = pos+1;   ; ++ii )
                   {
                        buf0[ii] = buf0[ii+qnty-1];
                        if( buf0[ii+qnty-1]==0 )
                             break;
                   }
                   buf0[pos] = '*';
               }
          }

          if( !str.match(buf0,0) )
          {
               printf("Error in match()!\n");
               return;
          }
    }
    printf("match() ok\n");
    //int match( const char *patter     )            const;  // inline
    str.assign("Hello");
    printf("match(cchar*) 1=%i\n", str.match("*el*") );

    //int match( const str_ConstStr &str, int from ) const;  // inline
    printf("match(str,0)  1=%i\n", str.match(str_ConstStr("*el*"),1));
    printf("match(str,0)  1=%i\n", str.match(str_ConstStr("*el*")));

    //int trim ( int from ) const;
    printf("trim() 2=%i\n", str.assign("  Hello").trim(0));
    printf("trim() 2=%i\n", str.assign("  ").trim(0));
    printf("trim() 2=%i\n", str.assign(" \t").trim(1));
    printf("trim() 2=%i\n", str.assign("\n\r").trim(0));

    //int to( unsigned long  &dest, unsigned long radix, int from ) const;
    unsigned long ul;
    str.assign("1234").to(ul,10,0);
    printf("to(ulong&,int,int) 1234=%lu\n", ul);

    str.assign("FaceBeda").to(ul,16,0);
    printf("to(ulong&,int,int) FacEbeda=%lX\n", ul);

    str.assign("377").to(ul,8,0);
    printf("to(ulong&,int,int) FF=%lX\n", ul);

    i = str.assign("01010101").to(ul,2,0);
    printf("to(ulong&,int,int) 55=%lX\n", ul);

    //int to( unsigned long  &dest, int from ) const;
    str.assign("0xFACE1234").to(ul,0);
    printf("to(ulong&,int)   FACE1234=%lX\n", ul);

    str.assign("0b01010101").to(ul,0);
    printf("to(ulong&,int)   55=%lX\n", ul);

    str.assign("0o377").to(ul,0);
    printf("to(ulong&,int)   255=%lu\n", ul);

    str.assign("0377").to(ul,0);
    printf("to(ulong&,int)   255=%lu\n", ul);

    //int to( long           &dest, int from ) const;
    long l;
    str.assign("12345678").to(l,0);
    printf("to(long,int)   12345678= %li\n", l);
    str.assign("-12345678").to(l,0);
    printf("to(long,int)  -12345678=%li\n", l);

    //int to( unsigned short &dest, int from ) const;
    unsigned short us;
    str.assign("1234").to(us,0);
    printf("to(ushort) 1234=%i\n",(int)us);
    //int to( short          &dest, int from ) const;
    short s;
    str.assign("-1234").to(s,0);
    printf("to(short)  -1234=%i\n",(int)s);

    //int to( unsigned int   &dest, int from ) const;
    unsigned int ui;
    str.assign("1234").to(ui,0);
    printf("to(uint) 1234=%i\n",ui);

    //int to( int            &dest, int from ) const;
    str.assign("-1234").to(i,0);
    printf("to(int)  -1234=%i\n",i);

    //int to( unsigned char  &dest, int from ) const;
    unsigned char uc;
    str.assign(" 12").to(uc,0);
    if( uc!=12 )
    {
         printf("Error: to(uchar,int)\n");
         return;
    }

    //int to( char           &dest, int from ) const;
    char c;
    str.to(c,0);
    if( c != 12 )
    {
         printf("Error: to(char,int)\n");
         return;
    }

    //int to( double         &dest, int from ) const;
    double d;
    str.assign(" + 1234.2173461287e+100").to(d,0);
    d -= 1.2342173461287e+103;
    if( d < 0 ) d = -d;
    if( d > 1e-10 )
    {
         printf("Error: to(double,int) %lf\n",d);
         return;
    }
    str.assign("12347362576452376542736452367.237").to(d,0);
    //int to( float          &dest, int from ) const;
    float f;
    str.assign("12.34e12").to(f,0);
    printf("to(float,int) 1.234e13=%g\n",f);

    //int to( unsigned long  &dest ) const;
    str.assign("1234").to(ul);
    if( ul!=1234 ) {printf("Error!(to(ulong))\n"); return;}

    //int to( long           &dest ) const;
    str.to(l);
    if( l!=1234 ) {printf("Error!(to(long))\n"); return;}

    //int to( unsigned short &dest ) const;
    str.to(us);
    if( us!=1234 ) {printf("Error!(to(ushort))\n"); return;}

    //int to( short          &dest ) const;
    str.to(s);
    if( s!=1234 ) {printf("Error!(to(short))\n"); return;}

    //int to( unsigned int   &dest ) const;
    str.to(ui);
    if( ui!=1234 ) {printf("Error!(to(uint))\n"); return;}
    //int to( int            &dest ) const;
    str.to(i);
    if( i!=1234 ) {printf("Error!(to(int))\n"); return;}

    //int to( unsigned char  &dest ) const;
    str.assign( "12" );
    str.to(uc);
    if( uc!=12 ) {printf("Error!(to(uchar))\n"); return;}

    //int to( char           &dest ) const;
    str.to(c);
    if( c!=12 ) {printf("Error!(to(char))\n"); return;}

    //int to( double         &dest ) const;
    str.assign("1.2e-3");
    str.to(d);

    //int to( float          &dest ) const;
    str.to(f);
    if( ((float)d)-f > 1e-4 )  {printf("Error!(to(float|double))\n"); return;}
    if( ((float)d)-f <-1e-4 )  {printf("Error!(to(float|double))\n"); return;}

    //int get( const char         *pattern, int from ) const;
    str = "Hello";
    if( str.get("Hel",0) != 3 ) {printf("Error!get(cchar*)");return;}

    //int get( const str_ConstStr &pattern, int from ) const;// inline
    if( str.get(str_ConstStr("Hel"),0) != 3 ) {printf("Error!get(str)");return;}

    //int get( const char         *pattern )           const;
    if( str.get("Hel") != 3 ) {printf("Error!get(cchar*)");return;}

    //int get( const str_ConstStr &pattern )           const;
    if( str.get(str_ConstStr("Hello"),0) != 5 ) {printf("Error!get(str)");return;}

    //int cmp( const char         *str ) const;
    if( str.cmp("Hello")!=0 ) {printf("Error! cmp(char*)");return;}
    //int cmp( const str_ConstStr &str ) const;
    if( str.cmp(str_ConstStr("Hello"))!=0 ) {printf("Error! cmp(str)");return;}

    str2 = "mmm";
    str3 = str;

    //int operator == ( const str_ConstStr &str ) const;
    if( str == str2 ) { printf("Error: ==str\n"); return; }
    if( !(str == str3) ) { printf("Error: ==str\n"); return; }

    //int operator == ( const char *str )         const;
    if( str == "Hel" ) { printf("Error: ==char*\n"); return; }
    if( !(str == "Hello") ) { printf("Error: ==char*\n"); return; }


    //friend int operator ==( const char *str1, const str_ConstStr &str2 );
    if( "Hel" == str ) { printf("Error: ==char*\n"); return; }
    if( !("Hello" == str) ) { printf("Error: ==char*\n"); return; }

    //int operator != ( const char *str )         const;
    if( str!="Hello" ) { printf("Error:!=char*\n"); return; }
    if(!(str!="Hel") ) { printf("Error:!=char*\n"); return; }

    //int operator != ( const str_ConstStr &str ) const;
    if( str!=(str2="Hello") ) { printf("Error:!=char*\n"); return; }
    if(!(str!=(str2="Hel")) ) { printf("Error:!=char*\n"); return; }

    //friend int operator !=( const char *str1, const str_ConstStr &str2 );
    if( "Hello"!=str ) { printf("Error: char*!=\n"); return; }
    if(!("Hel"!=str) ) { printf("Error: char*!=\n"); return; }

    //int operator <  ( const str_ConstStr &str ) const;
    str = "AAA";
    if( !(str < (str2="BBB")) ) {printf("Error: < str\n");return;}
    if( str < (str2="!!!") )    {printf("Error: < str\n");return;}

    //int operator >  ( const str_ConstStr &str ) const;
    if( !(str > (str2="!!!")) ) {printf("Error: < str\n");return;}
    if( str > (str2="BBB") )    {printf("Error: < str\n");return;}

    //int operator <= ( const str_ConstStr &str ) const;
    if( !(str <= (str2="BBB")) ) {printf("Error: <= str\n");return;}
    if( str <= (str2="!!!") )    {printf("Error: <= str\n");return;}
    if( !(str <= (str2="AAA")) ) {printf("Error: <= str\n");return;}

    //int operator >= ( const str_ConstStr &str ) const;
    if( !(str > (str2="!!!")) ) {printf("Error: >= str\n");return;}
    if( str > (str2="BBB") )    {printf("Error: >= str\n");return;}
    if( !(str >= (str2="AAA")) ) {printf("Error: >= str\n");return;}

    /*************
     *
     * Error test
     *
     *************/
     /*

    int operator == ( const char *str )         const;
    int operator == ( const str_ConstStr &str ) const;
    friend int operator ==( const char *str1, const str_ConstStr &str2 );

    int operator != ( const char *str )         const;
    int operator != ( const str_ConstStr &str ) const;
    friend int operator !=( const char *str1, const str_ConstStr &str2 );

    int operator <  ( const str_ConstStr &str ) const;
    int operator >  ( const str_ConstStr &str ) const;
    int operator <= ( const str_ConstStr &str ) const;
    int operator >= ( const str_ConstStr &str ) const;
    */
    //const char    *getChars()            const;            // inline

    printf("\n\n======= Error tests =========\n\n");
    str = "Hello";
    //char           charAt  ( int index ) const;            // inline
    str.charAt(-1);
    if(!ErrDetect()) return;

    str.charAt(str.length());
    if( ErrDetect() ) return;

    str.charAt(str.length()+1);
    if(!ErrDetect()) return;

    str = NULL;
    if(!ErrDetect()) return;

    c = str[-1];
    if(!ErrDetect()) return;

    c = str[str.length()];
    if(ErrDetect()) return;

    c = str[str.length()+1];
    if(!ErrDetect()) return;

    (void)c;

    //str_ConstStr &assign( const char *str );
    str.assign(NULL);
    if(!ErrDetect()) return;

    //str_ConstStr( const char * str );                      // inline
    str_ConstStr tmpStr(NULL);
    if(!ErrDetect()) return;

    //int indexOf( char c )                           const;
    str = "Hello";

    str.indexOf('o');
    if(ErrDetect()) return;

    str.indexOf('a');
    if(ErrDetect()) return;

    //int indexOf( char c, int from )                 const;
    if( str.indexOf('o',-1) != -1) return;
    if(!ErrDetect()) return;

    if(str.indexOf('o',4)!=4) return;
    if( ErrDetect()) return;

    if(str.indexOf('o',str.length()) != -1) return;
    if( ErrDetect()) return;

    if(str.indexOf('o',str.length()+1) != -1) return;
    if( !ErrDetect()) return;

    //int indexOf( const char *str )                  const;
    if( str.indexOf((const char *)NULL)!=-1 ) return;
    if( !ErrDetect()) return;

    str.indexOf("");
    if( ErrDetect()) return;

    //int indexOf( const char *str, int from )        const;
    if( str.indexOf((const char*)NULL,0) != -1 ) return;
    if( !ErrDetect()) return;

    if( str.indexOf("lo",-1)!=-1 ) return;
    if( !ErrDetect()) return;

    if( str.indexOf("lo",str.length())!=-1 ) return;
    if( ErrDetect()) return;

    if( str.indexOf("lo",str.length()+1)!=-1 ) return;
    if( !ErrDetect()) return;


    //int indexOf( const str_ConstStr &str )          const;
    str.indexOf("");
    if( ErrDetect()) return;
    if( str.indexOf(str2="23423534534") != -1) return;
    if( ErrDetect()) return;

    //int indexOf( const str_ConstStr &str, int from )const;
    if( str.indexOf(str2="Hello")!=0 ) return;
    if( ErrDetect()) return;

    if( str.indexOf(str2="Hell",-1)!=-1 ) return;
    if( !ErrDetect()) return;

    if( str.indexOf(str2="Hell",str.length()) != -1 ) return;
    if( ErrDetect()) return;

    if( str.indexOf(str2="Hell",str.length()+1) != -1 ) return;
    if( !ErrDetect()) return;

    //int match( const char *patter     , int from ) const;
    if( str.match((const char *)NULL,0) ) return;
    if( !ErrDetect()) return;

    if( str.match("*",-1) ) return;
    if( !ErrDetect()) return;

    if( str.match("A",str.length()) ) return;
    if( ErrDetect()) return;

    if( str.match("*",str.length()+1) ) return;
    if( !ErrDetect()) return;

    //int match( const char *patter     )            const;  // inline
    if( str.match((const char *)NULL) ) return;
    if( !ErrDetect()) return;

    if( str.match("Hello* world") ) return;
    if( ErrDetect()) return;

    if( str.match("Hello**?* world") ) return;
    if( ErrDetect()) return;

    //int match( const str_ConstStr &str, int from ) const;  // inline
    if( str.match(str2="*",-1) ) return;
    if( !ErrDetect()) return;

    if( str.match(str2="A",str.length()) ) return;
    if( ErrDetect()) return;

    if( str.match(str2="*",str.length()+1) ) return;
    if( !ErrDetect()) return;

    //int match( const str_ConstStr &str )           const;  // inline
    if( str.match(str2="Hello* world") ) return;
    if( ErrDetect()) return;

    if( str.match(str2="Hello**?* world") ) return;
    if( ErrDetect()) return;

    str.match("");
    if( ErrDetect()) return;

    //int trim ( int from ) const;
    if( str.trim(0)!=0 ) return;
    if( ErrDetect()) return;

    if( str.trim(2)!=2 ) return;
    if( ErrDetect()) return;

    if( str.trim(str.length())!=str.length() ) return;
    if( ErrDetect()) return;

    if( str.trim(str.length()+1) != str.length()+1 ) return;
    if( !ErrDetect()) return;

    if( str.trim(-2) != -2 ) return;
    if( !ErrDetect()) return;

    //int to( unsigned long  &dest, int from ) const;
    str = "1234";
    if( str.to(ul,-1)!=-1 ) return;
    if( !ErrDetect()) return;

    if( str.to(ul,str.length())!=str.length() ) return;
    if( !ErrDetect()) return;

    if( str.to(ul,str.length()+1)!=str.length()+1) return;
    if( !ErrDetect()) return;

    str = "123 :AA";
    if( str.to(ul,3)!=3 ) return;
    if( !ErrDetect()) return;

    str = "123 /AA";
    if( str.to(ul,3)!=3 ) return;
    if( !ErrDetect()) return;

    //-------- radix 2
    str = "  0b0102";
    if( str.to(ul,1) != 1) return;
    if( !ErrDetect()) return;

    str = "  0b2101";
    if( str.to(ul,1) != 1) return;
    if( !ErrDetect()) return;

    str = "  0b/010";
    if( str.to(ul,1) != 1) return;
    if( !ErrDetect()) return;

    //------- radix 8
    str = "  0o12348";
    if( str.to(ul,1) != 1 ) return;
    if( !ErrDetect()) return;

    str = "  0o/1234";
    if( str.to(ul,1) != 1 ) return;
    if( !ErrDetect()) return;

    str = "  0o81234";
    if( str.to(ul,1) != 1 ) return;
    if( !ErrDetect()) return;

    //--------
    str = "  012348";
    if( str.to(ul,1) != 1 ) return;
    if( !ErrDetect()) return;

    str = "  0/1234";
    if( str.to(ul,1) != 1 ) return;
    if( !ErrDetect()) return;

    str = "  081234";
    if( str.to(ul,1) != 1 ) return;
    if( !ErrDetect()) return;

    str = "  0A1234";
    if( str.to(ul,1) != 1 ) return;
    if( !ErrDetect()) return;

    //---------------------
    str = "  1312423453546";
    if( str.to(ul,1)!= 1) return;
    if( !ErrDetect()) return;

    str = "  4294967296";
    if( str.to(ul,1) != 1) return;
    if( !ErrDetect()) return;

    //---------- radix 16
    str = "  0xG2348";
    if( str.to(ul,1) != 1 ) return;
    if( !ErrDetect()) return;

    str = "  0x/1234";
    if( str.to(ul,1) != 1 ) return;
    if( !ErrDetect()) return;

    str = "  0xFFFFFFFF1";
    if( str.to(ul,1) != 1 ) return;
    if( !ErrDetect()) return;

    //int to( long           &dest, int from ) const;
    str = "  0xF0000000";
    if( str.to(l,1)!=1 ) return;
    if( !ErrDetect()) return;

    str = "  12234";
    if( str.to(l,-1)!=-1 ) return;
    if( !ErrDetect()) return;

    str = "  12234";
    if( str.to(l,str.length())!=str.length() ) return;
    if( !ErrDetect()) return;

    str = "  12234";
    if( str.to(l,str.length()+1)!=str.length()+1 ) return;
    if( !ErrDetect()) return;


    //int to( unsigned short &dest, int from ) const;
    str = "  0xFFFFF";
    if( str.to(us,1)!=1 ) return;
    if( !ErrDetect()) return;

    str = "  1234";
    if( str.to(us,-1)!=-1 ) return;
    if( !ErrDetect()) return;

    if( str.to(us,str.length())!=str.length() ) return;
    if( !ErrDetect()) return;

    if( str.to(us,str.length()+1)!=str.length()+1 ) return;
    if( !ErrDetect()) return;

    str = " FACE";
    if( str.to(us,1) != 1 ) return;
    if( !ErrDetect()) return;

    //int to( short          &dest, int from ) const;
    str = "  0xFFFFF";
    if( str.to(s,1)!=1 ) return;
    if( !ErrDetect()) return;

    str = "  1234";
    if( str.to(s,-1)!=-1 ) return;
    if( !ErrDetect()) return;

    if( str.to(s,str.length())!=str.length() ) return;
    if( !ErrDetect()) return;

    if( str.to(s,str.length()+1)!=str.length()+1 ) return;
    if( !ErrDetect()) return;

    str = " FACE";
    if( str.to(s,1) != 1 ) return;
    if( !ErrDetect()) return;

    str = "  0xF000 ";
    if( str.to(s,1) != 1 ) return;
    if( !ErrDetect()) return;

    //int to( unsigned int   &dest, int from ) const;
    str = "  0xFFFFFFFFFFF";
    if( str.to(ui,1)!=1 ) return;
    if( !ErrDetect()) return;

    str = "  1234";
    if( str.to(ui,-1)!=-1 ) return;
    if( !ErrDetect()) return;

    if( str.to(ui,str.length())!=str.length() ) return;
    if( !ErrDetect()) return;

    if( str.to(ui,str.length()+1)!=str.length()+1 ) return;
    if( !ErrDetect()) return;

    str = " FACE";
    if( str.to(ui,1) != 1 ) return;
    if( !ErrDetect()) return;

    //int to( int            &dest, int from ) const;

    str = "  0xFFFFFFFFFF";
    if( str.to(i,1)!=1 ) return;
    if( !ErrDetect()) return;

    str = "  1234";
    if( str.to(i,-1)!=-1 ) return;
    if( !ErrDetect()) return;

    if( str.to(i,str.length())!=str.length() ) return;
    if( !ErrDetect()) return;

    if( str.to(i,str.length()+1)!=str.length()+1 ) return;
    if( !ErrDetect()) return;

    str = " FACE";
    if( str.to(i,1) != 1 ) return;
    if( !ErrDetect()) return;

    {
    char buf[20];
    sprintf( buf, "0x%X", -324 );
    str = buf;
    if( str.to(i,1) != 1 ) return;
    if( !ErrDetect()) return;
    }

    //int to( unsigned char  &dest, int from ) const;
    str = "  0xFFFFF";
    if( str.to(uc,1)!=1 ) return;
    if( !ErrDetect()) return;

    str = "  12";
    if( str.to(uc,-1)!=-1 ) return;
    if( !ErrDetect()) return;

    if( str.to(uc,str.length())!=str.length() ) return;
    if( !ErrDetect()) return;

    if( str.to(uc,str.length()+1)!=str.length()+1 ) return;
    if( !ErrDetect()) return;

    str = " FA";
    if( str.to(uc,1) != 1 ) return;
    if( !ErrDetect()) return;


    //int to( char           &dest, int from ) const;
    str = "  0xFFFFF";
    if( str.to(c,1)!=1 ) return;
    if( !ErrDetect()) return;

    str = "  12";
    if( str.to(c,-1)!=-1 ) return;
    if( !ErrDetect()) return;

    if( str.to(c,str.length())!=str.length() ) return;
    if( !ErrDetect()) return;

    if( str.to(c,str.length()+1)!=str.length()+1 ) return;
    if( !ErrDetect()) return;

    str = " AA";
    if( str.to(c,1) != 1 ) return;
    if( !ErrDetect()) return;

    str = "  0xF0 ";
    if( str.to(c,1) != 1 ) return;
    if( !ErrDetect()) return;


    //int to( double         &dest, int from ) const;
    str = "  0.123e456";
    if( str.to(d,1)!=1 ) return;
    if( !ErrDetect()) return;

    str = "  12";
    if( str.to(d,-1)!=-1 ) return;
    if( !ErrDetect()) return;

    if( str.to(d,str.length())!=str.length() ) return;
    if( !ErrDetect()) return;

    if( str.to(d,str.length()+1)!=str.length()+1 ) return;
    if( !ErrDetect()) return;

    str = " AA";
    if( str.to(d,1) != 1 ) return;
    if( !ErrDetect()) return;

    str = "";
    if( str.to(d,1) != 1 ) return;
    if( !ErrDetect()) return;


    str = "  0.1e+11111 ";
    if( str.to(d,1)!=1 ) return;
    if( !ErrDetect()) return;

    str = "  1.797693134862315E+308";
    i = str.to(d,1);
    if( i!=1 ) return;
    if( !ErrDetect()) return;


    //       4.94065645841246544e-324
    str = "  4.940656458412465e-324";
    i = str.to(d,1);
    if( i!=1 ) return;
    if( !ErrDetect()) return;

    //int to( float          &dest, int from ) const;

    str = "  3.40282346638529860e+38";
    if( str.to(f,1)!=1 ) return;
    if( !ErrDetect()) return;

    str = "  -3.40282346638529860e+38";
    if( str.to(f,1)!=1 ) return;
    if( !ErrDetect()) return;

    //int get( const char         *pattern, int from ) const;

    str = "Hello";
    if( str.get("erew")!=0 ) return;
    if( ErrDetect()) return;

    if( str.get("el",-1)!=-1 ) return;
    if( !ErrDetect()) return;

    if( str.get("el",str.length())!=str.length() ) return;
    if( ErrDetect()) return;

    if( str.get("el",str.length()+1)!=str.length()+1 ) return;
    if( !ErrDetect()) return;

    if( str.get("",0)!=0 ) return;
    if( ErrDetect()) return;

    if( str.get((const char *)NULL,0)!=0 ) return;
    if( !ErrDetect()) return;

    //int get( const str_ConstStr &pattern, int from ) const;// inline
    if( str.get(str2="erew")!=0 ) return;
    if( ErrDetect()) return;

    if( str.get(str2="el",-1)!=-1 ) return;
    if( !ErrDetect()) return;

    if( str.get(str2="el",str.length())!=str.length() ) return;
    if( ErrDetect()) return;

    if( str.get(str2="el",str.length()+1)!=str.length()+1 ) return;
    if( !ErrDetect()) return;


    //int get( const char         *pattern )           const;
    if( str.get((const char *)NULL)!=0 ) return;
    if( !ErrDetect()) return;

    //int cmp( const char         *str ) const;
    if( str.cmp((const char *)NULL)==0 ) return;
    if( !ErrDetect()) return;

    if( str.cmp( "" )==0 )return;
    if( ErrDetect()) return;

    str = "";
    if( str.cmp( "" )!=0 )return;
    if( ErrDetect()) return;

    //int operator == ( const char *str )         const;
    if( str == (const char *)NULL ) return;
    if( !ErrDetect()) return;
    //friend int operator ==( const char *str1, const str_ConstStr &str2 );
    if( (const char *)NULL == str ) return;
    if( !ErrDetect()) return;

    //int operator != ( const char *str )         const;
    if( str != (const char *)NULL ) return;
    if( !ErrDetect()) return;

    //friend int operator !=( const char *str1, const str_ConstStr &str2 );
    if( (const char *)NULL != str ) return;
    if( !ErrDetect()) return;



    printf("\nOk Error test\n");

    /*********
     *
     *  query
     *
     *********/

    str = "  0x56";
    if( str.query( 2 ) != str_ConstStr::STR_INT )
    {
         printf("Error: query\n");
         return;
    }

    str = "  + 0x56";
    if( str.query( 2 ) != str_ConstStr::STR_INT )
    {
         printf("Error: query\n");
         return;
    }

    str = "  + 0x1e";
    if( str.query( 2 ) != str_ConstStr::STR_INT )
    {
         printf("Error: query\n");
         return;
    }

    str = "  + 0b10";
    if( str.query( 2 ) != str_ConstStr::STR_INT )
    {
         printf("Error: query\n");
         return;
    }

    str = "  + 0o10";
    if( str.query( 2 ) != str_ConstStr::STR_INT )
    {
         printf("Error: query\n");
         return;
    }

    str = "  + 00";
    if( str.query( 2 ) != str_ConstStr::STR_INT )
    {
         printf("Error: query\n");
         return;
    }

    str = "  - 0";
    if( str.query( 2 ) != str_ConstStr::STR_INT )
    {
         printf("Error: query\n");
         return;
    }

    str = "  0.";
    if( str.query( 2 ) != str_ConstStr::STR_FLOAT )
    {
         printf("Error: query\n");
         return;
    }

    str = "  .1";
    if( str.query( 2 ) != str_ConstStr::STR_FLOAT )
    {
         printf("Error: query\n");
         return;
    }

    str = "  1e2";
    if( str.query( 2 ) != str_ConstStr::STR_FLOAT )
    {
         printf("Error: query\n");
         return;
    }

    str = " 1>3?-(1+3*4-5*(2+8)):--25";
    //str = "2*3*3";
    str.eval(l,0);
    printf("%li\n",l);
 }

extern unsigned _stklen = 32000;

void main()
 {
    str_TestConstStr();
 }
#endif
/* End of file CNSTSTR.CPP */


