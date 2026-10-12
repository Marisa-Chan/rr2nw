/*
 * File   : C:\NW\ARENA\OBASE\DynObj\DynPart.h
 * Author : Suavik
 * Ver   1.0 
 */
#ifndef __DYNPART_H__INCLUDED
#define __DYNPART_H__INCLUDED
#include "kernel/h/s_debug.h"


class s_ViewDynamicParticle : public  CViewSphericDynamic
 {
 public:
        CFVector3 m_pos;
        double    m_radius;
        unsigned long m_color;

        s_ViewDynamicParticle( )
                : m_pos(0,0,0),
                  m_radius(0),
                  m_color(0)
        {
           m_bump.vel = CFVector3(0,0,0);
        }

	virtual void Draw();

    void         prepareToRender(
                                const CFVector3 &pos,
                                double           radius
                                );
 };


#endif // ifndef __DYNPART_H__INCLUDED
/* End of file C:\NW\ARENA\OBASE\DynObj\DynPart.h */
