/*
   File:  CNSTSTR.H
   Autor: Suavik
   Ver    1.06

   Функции get пытаются опpеделить соответствие символов в шаблоне
   символам основной стpоки. Если стpоку не удалось полностью подобpать,
   то возвpащается индекс,с котоpого стаpтовали, иначе возвpащается номеp
   символа, идущего вслед за последним подобpанным. Допустимым диапозоном н
   ачала сpавнения является: начиная с 0, до длины исходной стpоки
   включительно. В случае ошибки возвpащается стаpтовый идекс (Если ошибка
   настpоена на непpеpывание pаботы пpогpаммы). Пустая возвpащает идекс,
   сообщаемый функции.
 */
#ifndef __CNSTSTR_H__
#define __CNSTSTR_H__

#include <stdio.h>

#define str_ERROR_THROW 1
#define str_ERROR_EXIT  2
#define str_ERROR_TEST  3



#ifndef str_USE_THROW
#define str_USE_THROW str_ERROR_EXIT
#endif



#if str_USE_THROW==str_ERROR_EXIT

#   include <process.h>
#   define str_THROW(x) exit(1)

#else

#   if str_USE_THROW==str_ERROR_THROW
#      define str_THROW(x) throw new x
#   else

#      if  str_USE_THROW==str_ERROR_TEST
           class str_Exception;
           str_Exception *g_curExcept = NULL;
#          define str_THROW(x) g_curExcept = new x
#      else
#          error Unknown value  str_USE_THROWW
#      endif


#   endif

#endif



class str_Exception
 {
 public:
    const char *m_errorStr;

    str_Exception( const char *errorStr )
    {
       m_errorStr = errorStr;
#if str_USE_THROWW==str_ERROR_EXIT
       printf("%s\n", errorStr);
       exit(0);
#endif
    }
 };

class str_ExceptionValueIsNULL: public str_Exception
 {
 public:
    str_ExceptionValueIsNULL( const char *errorStr )
         :str_Exception(errorStr)
    {
    }
 };

class str_ExceptionValueTooBig: public str_Exception
 {
 public:
    str_ExceptionValueTooBig( const char *errorStr )
         :str_Exception(errorStr)
    {
    }
 };

class str_ExceptionIsNotNumber: public str_Exception
 {
 public:
    str_ExceptionIsNotNumber( const char *errorStr )
         :str_Exception(errorStr)
    {
    }
 };

class str_ExceptionEval: public str_Exception
 {
 public:
    str_ExceptionEval( const char *errorStr )
         :str_Exception(errorStr)
    {
    }
 };

class str_ConstStr
 {
 friend void str_TestConstStr();
 protected:
    int           m_count;
    const char   *m_value;

    int useSignum( int &from ) const;
    int to( unsigned long  &dest, unsigned long radix, int from ) const;
 public:
    typedef enum
    {
        STR_EOL,
        STR_SEPARATOR,
        STR_INT,
        STR_FLOAT,
        STR_OUTOFRANGE,
        STR_OTHER
    } STR_TYPE;

    const char    *getChars()            const;            // inline
    char           charAt  ( int index ) const;            // inline
    int            length  ()            const;            // inline

    unsigned long  hashCode() const;
    friend   int   isCorrectHashCode( unsigned long hashCode );

    operator const char *() const;                         // inline
    str_ConstStr &operator =( const char *str );           // inline
    str_ConstStr &operator =( const str_ConstStr &str );   // inline
    char operator [] (int index)                    const; // inline


    str_ConstStr &assign( const char *str );
    str_ConstStr &assign( const str_ConstStr &str );       // inline
    str_ConstStr();                                        // inline
    str_ConstStr( const char * str );                      // inline
    str_ConstStr( const str_ConstStr &str );               // inline

    int indexOf( char c )                           const;
    int indexOf( char c, int from )                 const;
    int indexOf( const char *str )                  const;
    int indexOf( const char *str, int from )        const;
    int indexOf( const str_ConstStr &str )          const;
    int indexOf( const str_ConstStr &str, int from )const;

    /*
     *               BOOLEAN functions
     *
     * returns 1 if the string in Source matches the string in Pattern
     * The pattern may contain any number of the wild characters '*' and '?'
     * '?' matches any single character
     * '*' matches any sequence of charcters (including a zero length
     * sequence).
     * EG '*m?t*i*' will match 'Automatic'
     */
    int match( const char *patter     , int from ) const;
    int match( const char *patter     )            const;  // inline
    int match( const str_ConstStr &str, int from ) const;  // inline
    int match( const str_ConstStr &str )           const;  // inline

    /*
     *           INDEX functions
     */
    int trim ( int from ) const;

    /*
     * Numeric sequence
     */
    int to( unsigned long  &dest, int from ) const;
    int to( long           &dest, int from ) const;

    int to( unsigned short &dest, int from ) const;
    int to( short          &dest, int from ) const;

    int to( unsigned int   &dest, int from ) const;
    int to( int            &dest, int from ) const;

    int to( unsigned char  &dest, int from ) const;
    int to( char           &dest, int from ) const;

    int to( double         &dest, int from ) const;
    int to( float          &dest, int from ) const;

    int to( unsigned long  &dest ) const;
    int to( long           &dest ) const;

    int to( unsigned short &dest ) const;
    int to( short          &dest ) const;

    int to( unsigned int   &dest ) const;
    int to( int            &dest ) const;

    int to( unsigned char  &dest ) const;
    int to( char           &dest ) const;

    int to( double         &dest ) const;
    int to( float          &dest ) const;

    /*
     * Symbolyc sequence
     */
    int get( const char         *pattern, int from ) const;
    int get( const str_ConstStr &pattern, int from ) const;// inline
    int get( const char         *pattern )           const;
    int get( const str_ConstStr &pattern )           const;

    /*
     * Compare.
     */

    /*
     *  return -1, 0, 1
     */
    int cmp( const char         *str ) const;
    int cmp( const str_ConstStr &str ) const;

    /*
     * BOOLEAN
     */
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

    STR_TYPE query( int from )                  const;
    int eval( long &val, int from )             const;
 };


 //========================================================================
inline int  str_ConstStr::length() const
 {
    return m_count;
 }

 //========================================================================
inline const char *str_ConstStr::getChars() const
 {
    return m_value;
 }

 //========================================================================
inline str_ConstStr::operator const char *() const
 {
    return getChars();
 }

 //========================================================================
inline str_ConstStr &str_ConstStr::operator =( const char *str )
 {
    assign(str);
    return *this;
 }

 //========================================================================
inline str_ConstStr &str_ConstStr::operator =( const str_ConstStr &str )
 {
    assign(str);
    return *this;
 }

 //========================================================================
inline str_ConstStr::str_ConstStr()
 {
    m_count    = 0;
    m_value    = "";
 }


 //========================================================================
inline str_ConstStr::str_ConstStr( const char * str )
 {
    assign( str );
 }

 //========================================================================
inline str_ConstStr &str_ConstStr::assign( const str_ConstStr &str )
 {
    m_count    = str.m_count;
    m_value    = str.m_value;
    return *this;
 }

 //========================================================================
inline str_ConstStr::str_ConstStr( const str_ConstStr &str )
 {
    assign( str );
 }

 //========================================================================
inline char str_ConstStr::charAt( int index ) const
 {
    if( index < 0 || index > m_count )
    {
         str_THROW( str_Exception( "charAt(int): String index out of range" ) );
         return '\00';
    }

    return m_value[ index ];
 }

 //========================================================================
inline char str_ConstStr::operator [] (int index) const
 {
    return charAt(index);
 }

 //========================================================================
inline int str_ConstStr::match( const str_ConstStr &str, int from ) const
 {
    return match( str.m_value, from );
 }

 //========================================================================
inline int str_ConstStr::match( const str_ConstStr &str ) const
 {
    return match( str, 0 );
 }

 //========================================================================
inline int str_ConstStr::match( const char *str ) const
 {
    return match( str, 0 );
 }

 //========================================================================
inline int str_ConstStr::get( const str_ConstStr &pattern, int from ) const
 {
    return get( pattern.m_value, from );
 }

#endif


/* End of file cnststr.h */
