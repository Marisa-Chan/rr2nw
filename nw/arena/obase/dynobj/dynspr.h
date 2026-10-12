#ifndef __DYNSPR_H__INCLUDED
#define __DYNSPR_H__INCLUDED

#include "kernel/h/s_debug.h"

class CViewTexture;

class s_ViewDynamicSprite : public  CViewSphericDynamic
 {
        CViewTexture *m_ref;

 public:
        CFVector3 m_pos;
        double    m_radius;
        int u0,v0,u1,v1;

        s_ViewDynamicSprite( )
                : m_ref(0),
                  m_pos(0,0,0),
                  m_radius(0)
        {
        }

	virtual void Draw();
    void         initRef(CViewTexture *ref)
    {
        s_ASSERT(ref!=0,"s_ViewDynamicSprite::initRef");
        m_ref = ref;
    }

    void         prepareToRender(
                                const CFVector3 &pos,
                                double           radius,
                                int u0l, int v0l,
                                int u1l, int v1l,
                                CViewTexture    *ref
                                );
 };

#endif

