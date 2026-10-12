/*
   File : Suavik\E:\COM\S_evdata.h
   Autor: Suavik
   Ver    2.0

   enum
   --------------------------------------------------------------------------
    s_EventDataOpen
    s_EventDataItemStyle

   class
   --------------------------------------------------------------------------
    s_ELN
    s_ELNTable
    s_EventData
 */
#ifndef __S_EVDATA_H__
#define __S_EVDATA_H__

#ifndef __KRTYPES_H__
#include "Krtypes.h"
#endif

#ifndef __S_SET_H__
#include "s_set.h"
#endif

#include <string.h>

typedef int s_EventTagID;

typedef enum {
    EDO_NONE  = 0,
    EDO_WRITE = 1,
    EDO_READ  = 2
}
    s_EventDataOpen;

typedef enum
 {
    EDI_DESCEND,
    EDI_ASCEND,
    EDI_BYTE,
    EDI_CHAR,
    EDI_INT,
    EDI_UINT,
    EDI_LONG,
    EDI_ULONG,
    EDI_SHORT,
    EDI_USHORT,
    EDI_DOUBLE,
    EDI_FLOAT,
    EDI_DATA,
    EDI_STR,
    EDI_OBJECTID,
    // Extended for attributes
    EDI_FUNC,
    EDI_NONE
 }
    s_EventDataItemStyle;

class  s_ELNTable;
extern s_ELNTable *g_ELNTable;

 //***************************************************************************
class s_ELN // Event Label Name
 {
 public:
    int         m_label;
    const char *m_labelText;

 protected:
    void startInitialize()
    {
        m_label     = -1;
        m_labelText = NULL;
    }

 public:

    s_ELN( int label, const char *labelText )
    {
        m_label     = label;
        m_labelText = labelText;
    }

    s_ELN() // last item in table
    {
        startInitialize();
    }
 };

 //***************************************************************************
class s_ELNTable
 {
 public:
    const char       *m_tableText;
    const s_ELN      *m_table;
    s_ELNTable       *m_next;

 protected:
    void startInitialize()
    {
        m_tableText = NULL;
        m_table     = NULL;
        m_next      = NULL;
    }

 public:
    s_ELNTable( const char *tableText, const s_ELN *table )
    {
        m_tableText = tableText;
        m_table     = table;
        m_next      = g_ELNTable;
        g_ELNTable  = this;
    }
 };

 //***************************************************************************
class s_EventData
 {
 friend bool ed_PutByte( s_EventData &ed, unsigned char  b );
 friend bool ed_GetByte( s_EventData &ed, unsigned char &b );

 public:
    enum {
         BUFF_SIZE=140
    };

 protected:
    unsigned char           m_buff[ BUFF_SIZE ];

    int                     m_pos;
    int                     m_size;

// Provide compatibility for save
// between debug and release versions
// by PIN
//#if s_DEBUG_EVENT
    s_EventDataOpen         m_open;
    int                     m_level;
//#endif

 public:

    s_EventData()
    {
       m_pos    = 0;
       m_size   = 0;
#if s_DEBUG_EVENT
       m_open   = EDO_NONE;
       m_level  = -1;
#endif
    }

    void         getCopy     ( const s_EventData &ev );

    s_EventData &open        ( s_EventDataOpen open  );
    void         close       ();

    s_EventData &descend     ( s_EventTagID et, int ver );
    s_EventData &ascend      ();

    s_EventData &put         ( const void *data, int size );
    s_EventData &putStr      ( const char    *str   );
    s_EventData &putChar     ( char           c     );
    s_EventData &putByte     ( unsigned char  b     );

    s_EventData &putInt      ( int            i     );
    s_EventData &putUInt     ( unsigned int   u     );

    s_EventData &putShort    ( short          i     );
    s_EventData &putUShort   ( unsigned short u     );

    s_EventData &putLong     ( long           i     );
    s_EventData &putULong    ( unsigned long  u     );

    s_EventData &putDouble   ( double         d     );
    s_EventData &putFloat    ( float          f     );

    s_EventData &putObjectID ( KR_ObjectID id       );
    
    //..................

    s_EventData &get         ( void *data, int size );
    s_EventData &getStr      ( char           *str, int maxStrLen );
    s_EventData &getChar     ( char           &c    );
    s_EventData &getByte     ( unsigned char  &b    );

    s_EventData &getInt      ( int            &i    );
    s_EventData &getUInt     ( unsigned int   &u    );

    s_EventData &getShort    ( short          &i    );
    s_EventData &getUShort   ( unsigned short &u    );

    s_EventData &getLong     ( long           &i    );
    s_EventData &getULong    ( unsigned long  &u    );

    s_EventData &getDouble   ( double         &d    );
    s_EventData &getFloat    ( float          &f    );

    s_EventData &getObjectID ( KR_ObjectID     &id  );
 };


#if s_DEBUG_EVENT==0

#define s_DECLARE_EFUNC   inline s_EventData &s_EventData::

#define s_WRBODY(type,s)  memcpy( &(m_buff[m_size]), &s, sizeof(type) ); \
                          m_size += sizeof(type);                        \
                          return *this;

#define s_RBODY(type,d)   memcpy( &d, &(m_buff[m_pos]), sizeof(type) ); \
                          m_pos += sizeof(type);                        \
                          return *this;


s_DECLARE_EFUNC open( s_EventDataOpen as  )
 {
    switch( as )
    {
    case EDO_WRITE: m_pos = 0; m_size = 0; return *this;
    case EDO_READ:  m_pos = 0; return *this;
    }
    return *this;
 }

s_DECLARE_EFUNC descend( s_EventTagID , int )
 {
    return *this; 
 }


s_DECLARE_EFUNC ascend()                        { return *this;             }

s_DECLARE_EFUNC put( const void *data, int size )
 {
    memcpy( &(m_buff[m_size]), data, size );
    m_size += size;
    return *this;
 }

s_DECLARE_EFUNC putChar    ( char           c ) { s_WRBODY(char,c)          }
s_DECLARE_EFUNC putByte    ( unsigned char  b ) { s_WRBODY(unsigned char,b) }

s_DECLARE_EFUNC putInt     ( int            i ) { s_WRBODY(int,i)           }
s_DECLARE_EFUNC putUInt    ( unsigned int   u ) { s_WRBODY(unsigned,u)      }

s_DECLARE_EFUNC putShort   ( short          i ) { s_WRBODY(short,i)         }
s_DECLARE_EFUNC putUShort  ( unsigned short u ) { s_WRBODY(unsigned short,u)}

s_DECLARE_EFUNC putLong    ( long           i ) { s_WRBODY(long,i)          }
s_DECLARE_EFUNC putULong   ( unsigned long  u ) { s_WRBODY(unsigned long,u) }

s_DECLARE_EFUNC putDouble  ( double         d ) { s_WRBODY(double,d)        }
s_DECLARE_EFUNC putFloat   ( float          f ) { s_WRBODY(float,f)         }

s_DECLARE_EFUNC putObjectID( KR_ObjectID    id) { s_WRBODY(KR_ObjectID,id)   }


//..................

s_DECLARE_EFUNC get        ( void *data, int size )
 {
    memcpy( data, &(m_buff[m_pos]), size );
    m_pos += size;
    return *this;
 }

s_DECLARE_EFUNC getChar    ( char           &c ) { s_RBODY(char,c)           }
s_DECLARE_EFUNC getByte    ( unsigned char  &b ) { s_RBODY(unsigned char,b)  }

s_DECLARE_EFUNC getInt     ( int            &i ) { s_RBODY(int,i)            }
s_DECLARE_EFUNC getUInt    ( unsigned int   &u ) { s_RBODY(unsigned,u)       }

s_DECLARE_EFUNC getShort   ( short          &i ) { s_RBODY(short,i)          }
s_DECLARE_EFUNC getUShort  ( unsigned short &u ) { s_RBODY(unsigned short,u) }

s_DECLARE_EFUNC getLong    ( long           &i ) { s_RBODY(long,i)           }
s_DECLARE_EFUNC getULong   ( unsigned long  &u ) { s_RBODY(unsigned long,u)  }

s_DECLARE_EFUNC getDouble  ( double         &d ) { s_RBODY(double,d)         }
s_DECLARE_EFUNC getFloat   ( float          &f ) { s_RBODY(float,f)          }

s_DECLARE_EFUNC getObjectID( KR_ObjectID    &id) { s_RBODY(KR_ObjectID,id)    }

inline void  s_EventData::close()                { }

#undef s_RBODY
#undef s_WRBODY
#undef s_DECLARE_EFUNC

#endif

const char  *ed_Tag2Msg( s_EventDataItemStyle ts );
const s_ELN *s_FindLabel( int label, const s_ELN *ignore = NULL );
const char  *s_FindLabelName(int label);
void         s_VerifyLabel();

#endif


/* End of file S_EVENT.H */

