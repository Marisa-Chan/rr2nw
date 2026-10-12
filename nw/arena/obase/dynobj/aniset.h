#ifndef __ANISET_H__INCLUDED
#define __ANISET_H__INCLUDED

#include "kernel/h/s_debug.h"

//===================================================================================
class AniCell
{
 public:
    CViewBaseModifier *m_fs;
    bool               m_use;
};

//===================================================================================
class AniCell0
{
 public:
    CViewBaseModifier0 *m_fs;
    bool                m_use;
};

//===================================================================================
class AniSet
{
public:
     enum
     {
        MAX_ANI = 50,
        MAX_ANI0 = 100
     };

protected:

     int                m_reducCnt, m_aniCnt;
     int                m_reducCnt0, m_aniCnt0;

     static AniCell     m_array [MAX_ANI];
     static AniCell0    m_array0[MAX_ANI0];
     CViewObjectModel  *m_model;

public:
     AniSet(
             int  size,  AniCell *array, 
             int  size0, AniCell *array0 
           )
     {
        m_model  = NULL;
        m_size   = size;
        m_array  = array;
        m_size0  = size0;
        m_array0 = array0;
     }

     void init(
                CViewObjectModel *model,
                int  aniCnt, int  reducCnt,
                int  aniCnt0,int  reducCnt0
              )
     {
        for( int i = 0; i < m_size; ++i )
             m_array[i].m_use = 0;
        m_model = model;
     }

     AniSet &set( int num ,const char *aniBlock)
     {
         s_ASSERT( num>=0 && num < m_size && !m_array[num].m_use, "AniSet::set" );
         AniCell &cell = m_array[num];
         cell.m_fs  = &(m_model->BaseSet(0).Base(reduc).KFSet().Mod(aniBlock));
         cell.m_use = 1;
         return *this;
     }
     AniSet &set0( int num ,const char *aniBlock)
     {
         s_ASSERT( num>=0 && num < m_size0 && !m_array0[num].m_use, "AniSet::set" );
         AniCell0 &cell = m_array0[num];
         cell.m_fs  = &(m_model->BaseSet(0).Base(reduc).KFSet().Mod0(aniBlock));
         cell.m_use = 1;
         return *this;
     }

     CViewKeyFrame &operator [] (int num)
     {
         s_ASSERT(mun >=0 && num < m_size && m_array[num].m_use,"AniSet::[]");
         return *(m_array[num].m_fs);
     }
};

#endif
