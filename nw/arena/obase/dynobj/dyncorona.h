#ifndef __DYNCORONA_H__INCLUDED
#define __DYNCORONA_H__INCLUDED

#include "kernel/h/s_debug.h"


class s_ViewDynamicCorona : public  CViewSphericDynamic
 {
 public:
        CFVector3   m_pos;
        double      m_radius;
        int         u0,v0,u1,v1;
        int         m_alpha;
        unsigned long m_color;
        GR_HTEXTURE m_htext;

        s_ViewDynamicCorona( )
         :        m_pos(0,0,0),
                  m_radius(0)
        {
        }

	virtual void Draw();
    void         prepareToRender(
                                const CFVector3 &pos,
                                double           radius,
                                int u0l, int v0l,
                                int u1l, int v1l,
                                int   alpha,
                                unsigned long color,
                                GR_HTEXTURE htext
                                );
 };

#endif

