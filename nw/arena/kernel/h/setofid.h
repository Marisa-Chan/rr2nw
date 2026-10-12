#ifndef __SETOFID_H__INCLUDED
#define __SETOFID_H__INCLUDED

#include "kernel/h/krtypes.h"
#include "kernel/h/s_debug.h"


class PIN_SaveFile;

class KR_SetOfID
{
public:
      enum 
      {
         MAX_ID_CNT = 10
      };

private:
      KR_ObjectID m_list[MAX_ID_CNT];
      int         m_count;
      

public:

      KR_SetOfID()
      {
          m_count = 0;
      }

      int                getCount() const { return m_count; }
      const KR_ObjectID &operator [] ( int index ) const
      {
         s_ASSERT2( index < m_count && index >=0,
                    "KR_SetOfID[%i] of %i",index,m_count);
         return m_list[index];
      }

      bool  add ( const KR_ObjectID &ID );
      bool  del ( const KR_ObjectID &ID );
      bool  del ( int index );
      int   find( const KR_ObjectID &ID ) const;
      void  clr()                        { m_count = 0; }
      
	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & sf);

};

#endif
