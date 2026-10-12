/*
   File : Suavik\E:\COM\S_evdata.h
   Autor: Suavik
   Ver    2.0
 */

#include <string.h>

#include "Kernel/h/s_debug.h"
#include "Kernel/h/s_evdata.h"


s_ELNTable *g_ELNTable = NULL;

 // ===========================================================================
const s_ELN *s_FindLabel( int label, const s_ELN *ignore )
 {
     for(
          const s_ELNTable *table = g_ELNTable;
          table != NULL;
          table = table->m_next
        )
     {
          for(
               const s_ELN *eln = table->m_table;
               eln->m_labelText != NULL;
               eln++
             )
               if(  eln != ignore && eln->m_label == label  )
                    return eln;
     }

     return NULL;
 }

 // ===========================================================================
const char  *s_FindLabelName(int label)
 {
     const s_ELN *el = s_FindLabel(label);
     const char  *eln = "?";
     if(  el != NULL  )
          eln = el->m_labelText;
     return eln;
 }

 // ===========================================================================
void s_VerifyLabel()
 {
     bool er = false;

     for(
          const s_ELNTable *table = g_ELNTable;
          table != NULL;
          table = table->m_next
        )
     {
          for(
               const s_ELN *eln = table->m_table;
               eln->m_labelText != NULL;
               eln++
             )
          {
               const s_ELN *dub = s_FindLabel( eln->m_label, eln );

               if(  dub != NULL  )
               {
                    g_console->echo( "s_VerifyLabel: Dublicate label %i\n "
                                    "               %s == %s ",
                                    eln->m_label,
                                    eln->m_labelText,
                                    dub->m_labelText );
                    er = true;
               }
          }
     }

     if(  er  )
          g_console->terminate();
 }



const char *ed_Tag2Msg( s_EventDataItemStyle ts )
 {
    switch( ts )
    {
    case EDI_DESCEND:  return "descend";
    case EDI_ASCEND:   return "ascend";
    case EDI_BYTE:     return "byte";
    case EDI_CHAR:     return "char";
    case EDI_INT:      return "int";
    case EDI_UINT:     return "unsigned int";
    case EDI_LONG:     return "long";
    case EDI_ULONG:    return "unsigned long";
    case EDI_SHORT:    return "short";
    case EDI_USHORT:   return "unsigned short";
    case EDI_DOUBLE:   return "double";
    case EDI_FLOAT:    return "float";
    case EDI_DATA:     return "data";
    case EDI_STR:      return "string";
    case EDI_OBJECTID: return "KR_ObjectID";
    case EDI_FUNC:     return "function()";
    default:           return "Unknown tag type";
    }
 }


 // ===========================================================================
void s_EventData::getCopy( const s_EventData &ev )
 {
#if  s_DEBUG_EVENT
    m_pos    = ev.m_pos;
    m_size   = ev.m_size;
    m_open   = ev.m_open;
    m_level  = ev.m_level;
    memcpy( m_buff, ev.m_buff, ev.m_size );
#else
    m_pos    = ev.m_pos;
    m_size   = ev.m_size;
    memcpy( m_buff, ev.m_buff, ev.m_size );
#endif
 }

//
//
#if   s_DEBUG_EVENT
//
//

 // ===========================================================================
bool ed_PutByte( s_EventData &ed, unsigned char b )
 {
    if(  ed.m_size < ed.BUFF_SIZE  )
    {
         ed.m_buff[ ed.m_size ] = b;
         ++ed.m_size;

         return true;
    }

    s_ASSERTNQ( "ed_PutByte: Buffer overflow" );

    return false;
 }

 //---------------------------------------------------------------------------
bool ed_GetByte( s_EventData &ed, unsigned char &b )
 {
    if(  ed.m_pos < ed.m_size  )
    {
         b = ed.m_buff[ ed.m_pos ];
         ++ed.m_pos;

         return true;
    }

    s_ASSERTNQ( "ed_GetByte: Read data too big" );

    return false;
 }

 //---------------------------------------------------------------------------
template <class T>
bool ed_PutT( s_EventData &ed, const T &b  )
 {
    int i;
    unsigned char *adr = (unsigned char*)(&b);

    for( i = 0; i < sizeof(T) ; ++i, ++adr )
         if(  !ed_PutByte( ed, *adr )  )
              return false;

    return true;
 };

 //---------------------------------------------------------------------------
template <class T>
bool ed_GetT( s_EventData &ed, const T &b  )
 {
    int i;
    unsigned char *adr = (unsigned char*)(&b);

    for( i = 0; i<sizeof(T); ++i, ++adr )
         if(  !ed_GetByte( ed, *adr )  )
              return false;

    return true;
 };

 //---------------------------------------------------------------------------
bool ed_PutShort( s_EventData &ed, int i )
 {
    short x = (short)i;
    return ed_PutT( ed, x );
 }

 //---------------------------------------------------------------------------
bool ed_PutInt( s_EventData &ed, int i )
 {
    return ed_PutT( ed, i );
 }

 //---------------------------------------------------------------------------
bool ed_PutType( s_EventData &ed, s_EventDataItemStyle edi )
 {
    return ed_PutByte( ed, (unsigned char)edi );
 }

 //---------------------------------------------------------------------------
bool ed_GetShort( s_EventData &ed, int &i )
 {
    short x;
    bool  f = ed_GetT( ed, x );
    i = x;

    return f;
 }

 //---------------------------------------------------------------------------
bool ed_GetInt( s_EventData &ed, int &i )
 {
    return ed_GetT( ed, i );
 }

 //---------------------------------------------------------------------------
bool ed_GetType( s_EventData &ed, s_EventDataItemStyle &edi )
 {
    unsigned char b;
    bool          f = ed_GetByte( ed, b );

    edi = (s_EventDataItemStyle)( b );

    return f;
 }


// ============================================================================
s_EventData &s_EventData::open( s_EventDataOpen open )
 {
     s_ENTRY(s_EventData::open)

     switch( m_open )
     {
     case EDO_NONE:                                              break;
     case EDO_READ:  s_ASSERTNQ( "Data is opened for read"  );
     case EDO_WRITE: s_ASSERTNQ( "Data is opened for write" );
     }

     s_ASSERT( open != EDO_NONE, "Need EDO_READ or EDO_WRITE" );

     if(  open == EDO_WRITE  )
          m_size = 0;

     m_pos    = 0;
     m_open   = open;
     m_level  = -1;

     return *this;
 }

// ============================================================================
void s_EventData::close()
 {
     s_ENTRY(s_EventData::close)

     s_ASSERT( m_level == -1, "Error balance descend-ascend\n" );
     s_ASSERT( m_open != EDO_READ || m_pos == m_size,
               "Data size > read size" );

     m_pos  = 0;
     m_open = EDO_NONE;
 }

// ============================================================================
s_EventData &s_EventData::descend(
                                    s_EventTagID  et,
                                    int           ver
                                   )
 {
     s_ENTRY(s_EventData::descend)

     s_EventTagID         r_et;
     int                  r_ver;
     s_EventDataItemStyle r_tag;

     switch( m_open )
     {
     case EDO_NONE:
             s_ASSERTNQ( "Data not open\n" );
             break;

     case EDO_WRITE:
             if(  !ed_PutType ( *this, EDI_DESCEND ) ) break;
             if(  !ed_PutT    ( *this, et          ) ) break;
             if(  !ed_PutShort( *this, ver         ) ) break;

             ++m_level;
             break;

     case EDO_READ:
             if(  !ed_GetType ( *this, r_tag)  )
                  break;

             if(  is_ASSERT1( r_tag == EDI_DESCEND, "Read: (expected %s)",
                                                    ed_Tag2Msg( r_tag ) ) )
                  break;

             if(  !ed_GetT    ( *this, r_et )  ) break;
             if(  !ed_GetShort( *this, r_ver)  ) break;

             if(  is_ASSERT( et  == r_et , "Error read tag type" ) )
                  break;

             if(  is_ASSERT( ver == r_ver, "Error read tag version" ) )
                  break;

             ++m_level;
             break;
     }

     return *this;
 }

// ============================================================================
s_EventData &s_EventData::ascend()
 {
     s_ENTRY(s_EventData::ascend)

     switch( m_open )
     {
     case EDO_NONE:
             s_ASSERTNQ("Event data not open");
             break;

     case EDO_WRITE:
             if(  !is_ASSERT( m_level >= 0, "ascend without descend" )  )
                  m_level--;
             ed_PutType( *this, EDI_ASCEND );
             break;

     case EDO_READ:
             {
             if(  !is_ASSERT( m_level >= 0, "ascend without descend" )  )
                  m_level--;
             s_EventDataItemStyle r_tag;
             if(  !ed_GetType( *this, r_tag )  )
                  break;
             s_ASSERT1( r_tag == EDI_ASCEND, "Error read ascend(expected %s)",
                                             ed_Tag2Msg( r_tag ) );
             }
             break;
     }

     return *this;
 }


 /**************************
  *
  *         put
  *
  **************************/

// ============================================================================
s_EventData &s_EventData::put( const void *data, int size )
 {
     s_ENTRY(s_EventData::put)

     int i;

     switch( m_open )
     {
     case EDO_NONE:  s_ASSERTNQ( "Tag not open"      ); break;
     case EDO_READ:  s_ASSERTNQ( "Tag open for read" ); break;

     case EDO_WRITE:
             ed_PutType ( *this, EDI_DATA );
             ed_PutShort( *this, size     );

             for( i = 0; i<size; ++i )
                  if(  !ed_PutByte( *this, ((unsigned char*)data)[i] )  )
                       break;

             break;
     }

     return *this;
 }


// ============================================================================
s_EventData &s_EventData::putStr( const char *str )
 {
     s_ENTRY(s_EventData::putStr)

     const char *s;
     char c;

     switch( m_open )
     {
     case EDO_NONE: s_ASSERTNQ( "Tag not open"      ); break;
     case EDO_READ: s_ASSERTNQ( "Tag open for read" ); break;

     case EDO_WRITE:
             ed_PutType ( *this, EDI_STR );
             s = str;
             do
             {
                  c = *s;
                  ++s;
                  ed_PutByte( *this, (unsigned char)c );
             } while( c!=0 );

             break;
     }

     return *this;
 }


// ============================================================================
s_EventData &s_EventData::putByte( unsigned char x )
 {
     s_ENTRY(s_EventData::putByte)

     switch( m_open )
     {
     case EDO_NONE: s_ASSERTNQ( "Tag not open"      ); break;
     case EDO_READ: s_ASSERTNQ( "Tag open for read" ); break;

     case EDO_WRITE:
             ed_PutType ( *this, EDI_BYTE );
             ed_PutByte ( *this, x );
             break;
     }

     return *this;
 }

// ============================================================================
s_EventData &s_EventData::putChar( char x )
 {
     s_ENTRY(s_EventData::putChar)

     switch( m_open )
     {
     case EDO_NONE: s_ASSERTNQ( "Tag not open"      ); break;
     case EDO_READ: s_ASSERTNQ( "Tag open for read" ); break;

     case EDO_WRITE:
             ed_PutType ( *this, EDI_CHAR );
             ed_PutByte ( *this, (unsigned char)x );
             break;
     }

     return *this;
 }

// ============================================================================
#define ed_PUT_TAG(name,type,x,en) s_EventData &s_EventData::name     \
                                                         ( type x )   \
 {                                                                    \
     s_ENTRY(s_EventData::name)                                       \
     switch( m_open )                                                 \
     {                                                                \
     case EDO_NONE: s_ASSERTNQ("Tag not open");  break;               \
     case EDO_READ: s_ASSERTNQ("Tag open for read\n"); break;         \
     case EDO_WRITE:                                                  \
             ed_PutType ( *this, en );                                \
             ed_PutT( *this, x);                                      \
             break;                                                   \
     }                                                                \
     return *this;                                                    \
 }

ed_PUT_TAG( putInt , int,            x, EDI_INT    )
ed_PUT_TAG( putUInt, unsigned,       x, EDI_UINT   )
ed_PUT_TAG( putShort,short,          x, EDI_SHORT  )
ed_PUT_TAG( putUShort,unsigned short,x, EDI_USHORT )
ed_PUT_TAG( putLong, long,           x, EDI_LONG   )
ed_PUT_TAG( putULong,unsigned long,  x, EDI_ULONG  )
ed_PUT_TAG( putFloat,float,          x, EDI_FLOAT  )
ed_PUT_TAG( putDouble,double,        x, EDI_DOUBLE )
ed_PUT_TAG( putObjectID,KR_ObjectID,  x, EDI_OBJECTID )

 /********************************
  *
  *            get
  *
  ********************************/

// ============================================================================
s_EventData &s_EventData::get( void *data, int size )
 {
     s_ENTRY(s_EventData::get)

     s_EventDataItemStyle t;
     int                   s, i;

     switch( m_open )
     {
     case EDO_NONE:  s_ASSERTNQ( "Data not open" );       break;
     case EDO_WRITE: s_ASSERTNQ( "Data open for write" ); break;

     case EDO_READ:
             if(  ed_GetType( *this, t )  )
             {
                  if(  is_ASSERT1(t == EDI_DATA,"Tag not DATA(expected %s)",
                                                ed_Tag2Msg(t))  )
                       break;

                  if(  ed_GetShort( *this, s )  )
                  {
                       if(  is_ASSERT1( s == size, "Bad size(need %i)", s )  )
                            break;

                       for( i = 0; i<size; ++i )
                            if(  !ed_GetByte( *this, ((unsigned char*)data)[i] )  )
                                 break;
                  }
             }
             break;
     }

     return *this;
 }

// ============================================================================
s_EventData &s_EventData::getStr( char *str, int maxStrLen )
 {
     s_ENTRY(s_EventData::getStr)

     s_EventDataItemStyle t;
     unsigned char        c;

     switch( m_open )
     {
     case EDO_NONE:  s_ASSERTNQ( "Data not open" );       break;
     case EDO_WRITE: s_ASSERTNQ( "Data open for write" ); break;

     case EDO_READ:
             if(  ed_GetType( *this, t )  )
             {
                  if(  is_ASSERT1(t == EDI_STR,"Tag not Str(expected %s)",
                                               ed_Tag2Msg(t) )  )
                       break;

                  do
                  {
                      if(  !ed_GetByte( *this, c )  )
                           break;

                      if( is_ASSERT( maxStrLen > 0," Destinaion buffer overflow" ) )
                          break;

                      --maxStrLen;
                      *str = c;
                      ++str;
                  } while( c!=0 );
             }
             break;
     }

     return *this;
 }

// ============================================================================
s_EventData &s_EventData::getByte( unsigned char &b )
 {
     s_ENTRY(s_EventData::getByte)

     s_EventDataItemStyle t;

     switch( m_open )
     {
     case EDO_NONE:  s_ASSERTNQ( "Data not open" );       break;
     case EDO_WRITE: s_ASSERTNQ( "Data open for write" ); break;

     case EDO_READ:
             if(  ed_GetType( *this, t )  )
             {
                  if(  is_ASSERT1(t == EDI_BYTE,"Tag not byte(expected %s)",
                                                ed_Tag2Msg(t))  )
                       break;

                  ed_GetT( *this, b);
             }
             break;
     }

     return *this;
 }

// ============================================================================
s_EventData &s_EventData::getChar( char &b )
 {
     s_ENTRY(s_EventData::getChar)

     s_EventDataItemStyle t;

     switch( m_open )
     {
     case EDO_NONE:  s_ASSERTNQ( "Data not open"       ); break;
     case EDO_WRITE: s_ASSERTNQ( "Data open for write" ); break;

     case EDO_READ:
             if(  ed_GetType( *this, t )  )
             {
                  if(  is_ASSERT1( t == EDI_CHAR,"Tag not byte(expected %s)",
                                                 ed_Tag2Msg(t) )  )
                       break;

                  ed_GetT( *this, b);
             }
             break;
     }

     return *this;
 }


// ============================================================================
#define ed_GET_TAG(name,type,x,en)  s_EventData &s_EventData::name        \
                                    ( type &x )                           \
 {                                                                        \
     s_ENTRY(s_EventData::name)                                           \
     s_EventDataItemStyle t;                                              \
     switch( m_open )                                                     \
     {                                                                    \
     case EDO_NONE:  s_ASSERTNQ( "Data not open"      ); break;           \
     case EDO_WRITE: s_ASSERTNQ( "Data open for write"); break;           \
     case EDO_READ:                                                       \
             if(  ed_GetType( *this, t )  )                               \
             {                                                            \
                  if(  is_ASSERT1(t == en,"Type mismatch(expected %s)",   \
                                          ed_Tag2Msg(t))  )               \
                       break;                                             \
                  ed_GetT(*this,x);                                       \
             }                                                            \
             break;                                                       \
     }                                                                    \
     return *this;                                                        \
 }

ed_GET_TAG( getInt ,     int,            x, EDI_INT      )
ed_GET_TAG( getUInt,     unsigned,       x, EDI_UINT     )
ed_GET_TAG( getShort,    short,          x, EDI_SHORT    )
ed_GET_TAG( getUShort,   unsigned short, x, EDI_USHORT   )
ed_GET_TAG( getLong,     long,           x, EDI_LONG     )
ed_GET_TAG( getULong,    unsigned long,  x, EDI_ULONG    )
ed_GET_TAG( getFloat,    float,          x, EDI_FLOAT    )
ed_GET_TAG( getDouble,   double,         x, EDI_DOUBLE   )
ed_GET_TAG( getObjectID, KR_ObjectID,     x, EDI_OBJECTID )
//
//
#else
// ed_DEBUG_EVENT
//
// ============================================================================
s_EventData &s_EventData::putStr( const char *str )
 {
     register const char *s = str;
     char c;

     do
     {
         c = *s;
         ++s;
         m_buff[m_size] = c;
         ++m_size;
     }
     while( c!=0 );

     return *this;
 }

// ============================================================================
s_EventData &s_EventData::getStr( char *str, int maxStrLen )
 {
     register char *s = str;
     char c;
     do
     {
          c = m_buff[m_pos];
          ++m_pos;
          if(  maxStrLen > 0  )
          {
               *s=c;
               ++s;
          }
          --maxStrLen;
     } while( c!=0 );
     return *this;
 }

//
//
#endif
// end ed_DEBUG_EVENT
//

#if 0
#include <stdio.h>
void main()
 {
    s_EventData ev, e;

    ev.open(EDO_WRITE);
      ev.putByte(1);
      ev.putChar(2);
      ev.putShort(3);
      ev.putUShort(4);
      ev.putInt(5);
      ev.putUInt(6);
      ev.putLong(7);
      ev.putULong(8);
      ev.putFloat(9);
      ev.putDouble(10);
      ev.put(0,57);
    ev.close();

    e.open (EDO_WRITE);
      e.descend(1,0);
        e.putStr("Hello");

        e.descend(2,0);
          e.putFloat(3.14);
        e.ascend();
      e.ascend();
    e.close();

    char buf[256];
    float pi;

    printf("\n\n\n------read\n\n\n");
    e.open(EDO_READ);
       e.descend(1,0);
        e.getStr(buf,256);

        e.descend(2,0);
          e.getFloat(pi);
        e.ascend();
       e.ascend();
    e.close();
 }

#endif