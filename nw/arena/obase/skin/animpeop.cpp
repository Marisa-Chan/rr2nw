#define LAST_H__VIEW
#include "game.h"
#include "scene.h"

#include "kernel/h/context.h"
#include "kernel/h/session.h"
#include "kernel/h/echo.h"
#include "../output/animconst.h"
#include "AnimPeop.h"
#include "message/peopmsg.h"
#include "enum/spaceEnum.h"

int AnimateInfo::setAnim(KR_Event &event, int animNum )
{
    switch(event.label)
    {
    case pe_EV_SETANIM:
         if(  m_acellCnt<ANIMATE_CELL_CNT  )
              m_acellCnt++;
         else break;

         cur().m_animNum = animNum;
         event.data.getInt(cur().m_type);

         switch(cur().m_type)
         {
         case anim_UPDATE:
         case anim_LOADIDENTITY:
                     break;

         case anim_MOVE:
                     event.data
                     .descend(VECTOR3D_F,0)
                       .getDouble(cur().m_axis.x)
                       .getDouble(cur().m_axis.y)
                       .getDouble(cur().m_axis.z)
                     .ascend()

                     .descend(VECTOR3D_F,0)
                       .getDouble(cur().m_dir.x)
                       .getDouble(cur().m_dir.y)
                       .getDouble(cur().m_dir.z)
                     .ascend()

                     .getDouble(cur().A)
                     .getDouble(cur().w)
                     .getDouble(cur().F);
                     break;

         case anim_ROTATEOX:
         case anim_ROTATEOY:
         case anim_ROTATEOZ:
                     cur().A = 0;
                     event.data
                     .descend(VECTOR3D_F,0)
                       .getDouble(cur().m_axis.x)
                       .getDouble(cur().m_axis.y)
                       .getDouble(cur().m_axis.z)
                     .ascend()

                     .getDouble(cur().w)
                     .getDouble(cur().F);
                     break;

         case anim_ROTATEOXC:
         case anim_ROTATEOYC:
         case anim_ROTATEOZC:
                     event.data
                     .descend(VECTOR3D_F,0)
                       .getDouble(cur().m_axis.x)
                       .getDouble(cur().m_axis.y)
                       .getDouble(cur().m_axis.z)
                     .ascend()

                     .getDouble(cur().A)
                     .getDouble(cur().w)
                     .getDouble(cur().F);
                     break;

         default: echo("AnimateInfo::setAnim: Unknown animInfo ");
         }
         break;

    default: return 0;
    }
    return 1;
}

void AnimateInfo::animateProg( CViewObjectBaseSet *,
                               CViewObjectBase *pBase
                        )
 {
    int reduc = pBase->ReductionNum();
    double t  = Session::m_viewTime;

    for(int i=0; i < m_acellCnt; ++i)
    {
    AnimateCell &c = cur(i);
    switch( c.m_type )
    {
    case anim_UPDATE:
           m_askin->get0(c.m_animNum,reduc)->Update();
           break;

    case anim_MOVE:
           m_askin->get0(c.m_animNum,reduc)->Translate( c.m_axis 
                         + c.m_dir*c.A*sin( c.w*t + c.F ));
           break;

    case anim_ROTATEOX:
           m_askin->get0(c.m_animNum,reduc)->RotateOx(c.w*t + c.F, c.m_axis );
           break;

    case anim_ROTATEOY:
           m_askin->get0(c.m_animNum,reduc)->RotateOy(c.w*t + c.F , c.m_axis );
           break;

    case anim_ROTATEOZ:
           m_askin->get0(c.m_animNum,reduc)->RotateOz(c.w*t + c.F , c.m_axis );
           break;

    case anim_ROTATEOXC:
           m_askin->get0(c.m_animNum,reduc)->RotateOx(c.A*sin( c.w*t + c.F ), c.m_axis );
           break;

    case anim_ROTATEOYC:
           m_askin->get0(c.m_animNum,reduc)->RotateOy(c.A*sin( c.w*t + c.F ), c.m_axis );
           break;

    case anim_ROTATEOZC:
           m_askin->get0(c.m_animNum,reduc)->RotateOz(c.A*sin( c.w*t + c.F ), c.m_axis );
           break;

    case anim_LOADIDENTITY:
           m_askin->get0(c.m_animNum,reduc)->LoadIdentity();
           break;
    }
    }
 }

