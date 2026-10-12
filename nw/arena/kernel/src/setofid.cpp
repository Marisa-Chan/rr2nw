#include "kernel/h/setofid.h"

#define HANDLE int
#include "storage\h\savefile.h"

bool KR_SetOfID::dump(PIN_SaveFile & sf)
{
	if (!sf.WriteData( (char *) m_list,  sizeof(KR_ObjectID ) * MAX_ID_CNT) ||
		!sf.WriteData( (char *) & m_count, sizeof(int)))
		return false;

	return true;
}

bool KR_SetOfID::load(PIN_SaveFile & sf)
{
	if (!sf.GetData( (char *) m_list,  sizeof(KR_ObjectID ) * MAX_ID_CNT) ||
		!sf.GetData( (char *) & m_count, sizeof(int)))
		return false;

	return true;
}


bool  KR_SetOfID::add( const KR_ObjectID &ID )
 {
    if(  m_count < MAX_ID_CNT  )
    {
         m_list[m_count] = ID;
         m_count++;
         return true;
    }
    return false;
 }

bool  KR_SetOfID::del( const KR_ObjectID &ID )
 {
    int          i;
    KR_ObjectID *curID = m_list;

    for( i = m_count; i > 0; i--, curID++ )
         if(  ID == *curID  )
         {
              m_count--;
              for( i--; i > 0; i--, curID++ )
                   curID[0] = curID[1];
         }

    return false;
 }

bool  KR_SetOfID::del( int index )
 {
    if(  index < 0 || index >= m_count  )
         return false;

    int          i;
    KR_ObjectID *curID = &(m_list[index]);
    m_count--;

    for( i = index; i < m_count; i++, curID++ )
         curID[0] = curID[1];

    return true;
 }

int KR_SetOfID::find( const KR_ObjectID &ID ) const
 {
    int i;
    const KR_ObjectID *curID = m_list;

    for( i = 0; i < m_count; i++, curID++ )
         if(  ID == *curID  )
              return i;
    return -1;
 }