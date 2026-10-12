#define LAST_H__VIEW
#include "game.h"
#include "DynSpr.h"

void s_ViewDynamicSprite::Draw()
 {
    double		width = m_radius, height  = m_radius;

    CFVector3	v = CViewObject::m_viewPointDirSMx*m_pos;
    if( v.z < CViewObject::m_fFrontClip ) return;
    double d_v = 1./v.z;
    int		screen_width  = Round(width*CViewObject::m_viewPointScale.x*d_v);
    int		screen_height = Round(-height*CViewObject::m_viewPointScale.y*d_v);
    int		screen_x = Round(v.x*d_v),
		    screen_y = Round(v.y*d_v);

    int x0 = screen_x-screen_width/2,
        y0 = screen_y-screen_height/2,
        x1 = x0+screen_width+1,
        y1 = y0+screen_height+1;

    GRDrawSprite( x0,y0, x1,y1, u0,v0, u1,v1, 65536*d_v, 
                  m_ref->HImage() );
 }

void  s_ViewDynamicSprite::prepareToRender(
                                const CFVector3 &pos,
                                double           radius,
                                int u0l, int v0l,
                                int u1l, int v1l,
                                CViewTexture    *ref
                                          )
 {
   m_pos = pos;
   m_radius = radius;
   u0 = u0l<<16;
   v0 = v0l<<16;
   u1 = u1l<<16;
   v1 = v1l<<16;
   m_ref = ref;

   m_dynBase = m_dynBase1 = m_bump.start = m_pos;
   m_bump.fRadius = m_radius; // 1/sqrt(2)
   m_bump.vel = CFVector3(0,0,0);
   m_bump.fTime = 0;
 }
