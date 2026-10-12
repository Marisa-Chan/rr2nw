/*
 * File   : C:\NW\ARENA\OBASE\DynObj\DynPart.cpp
 * Author : Suavik
 * Ver   1.0 
 */
#define LAST_H__VIEW
#include "game.h"
#include "DynPart.h"

void s_ViewDynamicParticle::Draw()
{
    double		width = m_radius, height  = m_radius;

    CFVector3	v = CViewObject::m_viewPointDirSMx*m_pos;
    if( v.z < CViewObject::m_fFrontClip ) return;
    double d_v = 1./v.z;
    int		screen_width  = Round(width*CViewObject::m_viewPointScale.x*d_v);
    int		screen_x = Round(v.x*d_v),
		    screen_y = Round(v.y*d_v);

    GRDrawParticle(screen_x,screen_y,screen_width,65536*d_v,m_color);
}

void s_ViewDynamicParticle::prepareToRender(
                                const CFVector3 &pos,
                                double           radius
                                )
{
    m_pos = pos;
    m_radius = radius;
    m_bump.fTime = 0;

    m_dynBase = m_dynBase1 = m_bump.start = m_pos;
    m_bump.fRadius = m_radius;
}

/* End of file C:\NW\ARENA\OBASE\DynObj\DynPart.cpp */
