/*
 * File   : C:\NW\ARENA\OBASE\DynObj\DynPart.cpp
 * Author : Suavik
 * Ver   1.0 
 */
#define LAST_H__VIEW
#include "game.h"
#include "DynCorona.h"

void s_ViewDynamicCorona::Draw()
{
    double		width = m_radius, height  = m_radius;

    CFVector3	v = CViewObject::m_viewPointDirSMx*m_pos;
    if( v.z < CViewObject::m_fFrontClip ) return;
    double d_v = 1./v.z;
    int		screen_width  = Round(width*CViewObject::m_viewPointScale.x*d_v);
    int		screen_x = Round(v.x*d_v),
		    screen_y = Round(v.y*d_v);

    SGRAlphaSprite par;

    par.x0 = screen_x-screen_width/2;
    par.y0 = screen_y-screen_width/2;
    par.x1 = par.x0+screen_width;
    par.y1 = par.y0+screen_width;

    par.u0 = u0;
    par.v0 = v0;
    par.u1 = u1;
    par.v1 = v1;

    par.color   = m_color;
    par.opacity = m_alpha;
    par.iz      = (int)(d_v*65536);

    par.hTexture = m_htext;

    GRDrawAlphaSprite(&par);
}

void s_ViewDynamicCorona::prepareToRender(
                                const CFVector3 &pos,
                                double           radius,
                                int u0l, int v0l,
                                int u1l, int v1l,
                                int   alpha,
                                unsigned long color,
                                GR_HTEXTURE htext
                                )
{
    m_pos = pos;
    m_radius = radius;
    m_bump.fTime = 0;

    m_dynBase = m_dynBase1 = m_bump.start = m_pos;
    m_bump.fRadius = m_radius;
    u0 = u0l;
    v0 = v0l;
    u1 = u1l;
    v1 = v1l;
    if(  alpha < 0 ) alpha = 0; else if( alpha > 255 ) alpha = 255;
    m_alpha = alpha;
    m_color = color;
    m_htext = htext;
}

/* End of file C:\NW\ARENA\OBASE\DynObj\DynPart.cpp */
