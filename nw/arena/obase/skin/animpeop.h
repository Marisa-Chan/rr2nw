#ifndef __ANIMPEOP_H__
#define __ANIMPEOP_H__


#include "i/skin.i"
#include "kernel/h/context.h"
#include "kernel/h/s_debug.h"

 //===========================================================================
class AnimateCell
{
public:
     int       m_type;
     CFVector3 m_axis;
     CFVector3 m_dir;
     double    A;        // Amplitude
     double    w;        // cycle speed
     double    F;        // phase
     int       m_animNum;

     void startInitialize()
     {
         m_type = -1;
         m_axis = CFVector3(0,0,0);
         m_dir  = CFVector3(0,0,0);
         A      = 1;        // Amplitude
         w      = 1;        // 
         F      = 0;        // phase
     }
};

class AnimateInfo
{
public:
     enum { ANIMATE_CELL_CNT = 4 };
     AnimateCell m_acell[ANIMATE_CELL_CNT];
     int         m_acellCnt;

     ISkin    *m_askin;

     AnimateCell &cur() { s_ASSERT(m_acellCnt!=0,"AnimateInfo:cur range check error"); return m_acell[m_acellCnt-1]; }
     AnimateCell &cur(int i) { return m_acell[i]; }

     int       setAnim( KR_Event &event, int animNum );
     void startInitialize( ISkin *skin )
     {
         m_askin    = skin;
         m_acellCnt = 0;
         for( int i = 0; i <ANIMATE_CELL_CNT; ++i )
              m_acell[i].startInitialize();
     }

     void animateProg( CViewObjectBaseSet *,
                       CViewObjectBase *pBase
                     );
};

#endif
