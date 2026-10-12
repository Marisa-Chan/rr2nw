/*
 * File   : C:\NW\ARENA\OBASE\DynObj\DynPartLn.cpp
 * Author : Suavik
 * Ver   1.0 
 */
#define LAST_H__VIEW
#include "game.h"
#include "DynPartLn.h"
#include "kernel/h/s_debug.h"

void s_ViewDynParticleLine::Draw()
{
    int i;

//    if( !CViewObject::IsVisible((m_p0+m_p1)*0.5,Abs(m_p1-m_p0)/2) )
//        return;

#if 1
    CFVector3 dir( Normal(m_p1-m_p0) );
    CFVector3 pos(m_p0);
    double offset = m_offset;
    double step  = m_step0,
           maxOffset = Abs(m_p1-m_p0),
           widthAspect = (m_radius1-m_radius0)/maxOffset;
    

    for( i = 0 ;    i < 1000; ++i )
    {
         CFVector3 v = CViewObject::m_viewPointDirSMx*(pos+dir*offset);
         if(  v.z < CViewObject::m_fFrontClip  ) 
         {
              offset += step;
              step += m_step;
              if(  offset >= maxOffset  )
                   break;
              continue;
         }
         double d_v = 1./v.z;
         double width = m_radius0+widthAspect*offset;
         int screen_width  = Round(width*CViewObject::m_viewPointScale.x*d_v);
         int screen_x = Round(v.x*d_v),
		     screen_y = Round(v.y*d_v);

         int index = offset*16/(maxOffset+0.0001);
         if(  index < 0  ) index = 0; else if(  index > 15  ) index = 15;

         GRDrawParticle(screen_x,screen_y,screen_width,65536*d_v,m_color[index]);

         offset += step;
         step += m_step;
         if(  offset >= maxOffset  )
              break;
    }

#else

    double firstCol  = 0;
    double colCnt    = 15.5;

    CFVector3 p0, p1;
    p0 = CViewObject::m_viewPointDirSMx*m_p0;
    p1 = CViewObject::m_viewPointDirSMx*m_p1;

//    if( !CViewObject::IsVisible((p0+p1)*0.5,Abs(p1-p0)/2) )
//        return;

    bool bel0 = p0.z <= CViewObject::m_fFrontClip-0.1;
    bool bel1 = p1.z <= CViewObject::m_fFrontClip-0.1;

    if(  bel0 || bel1  )
         return;

    if(  fabs(p1.z-p0.z)>0.01  )
    {
         if(  bel0  )
         {
              double a = (CViewObject::m_fFrontClip-p1.z)/(p0.z-p1.z);
              p0       = p1+(p0-p1)*a;
              firstCol = colCnt*(1-fabs(a));
              colCnt  *= fabs(a);
         }
         else 
         if(  bel1  )
              p1 = p0+(p1-p0)*(CViewObject::m_fFrontClip-p0.z)/(p1.z-p0.z);
    }

    double d_v0 = 1./p0.z,
           d_v1 = 1./p1.z;
    double w0 = m_radius0*CViewObject::m_viewPointScale.x*d_v0;
    double w1 = m_radius1*CViewObject::m_viewPointScale.x*d_v1;

    int x0 = Round(p0.x*d_v0);
    int y0 = Round(p0.y*d_v0);

    int x1 = Round(p1.x*d_v1);
    int y1 = Round(p1.y*d_v1);

    int sx = x1-x0;
    int sy = y1-y0;
    int dx = abs(sx);
    int dy = abs(sy);
    int sigx = 0, sigy = 0;
    if(  sx>0  ) sigx = 1; else if(  sx<0  ) sigx = -1;
    if(  sy>0  ) sigy = 1; else if(  sy<0  ) sigy = -1;

    int screen_width = 2;


    if(  dx >= dy  )
    {
         double di = 65536.0*(d_v1-d_v0)/(dx+1);
         d_v0 *= 65536;
         double colIndex = firstCol;
         double colDelta = colCnt/(dx+1);
         double dw = (w1-w0)/(dx+1);

         y0 <<= 16;
         sy = (sy<<16)/(dx+1);
         int cnt = dx;
         if(  cnt > 300  ) cnt = 300;

         for( i = 0; i < cnt; ++i  )
         {
              int n = (int)colIndex;
              if( n < 0  ) n = 0; else if( n > 15 ) n = 15;
              colIndex += colDelta;

              w0 += dw;
              screen_width = (int)w0;
              if(  screen_width < 1  ) screen_width = 1;

              GRDrawParticle((x0)+0,(y0>>16)+0,screen_width,d_v0,m_color[n]);
              d_v0 += di;
              x0 += sigx;
              y0 += sy;
         }
    }
    else
    {
         double di = 65536.0*(d_v1-d_v0)/(dy+1);
         d_v0 *= 65536;
         double colIndex = firstCol;
         double colDelta = colCnt/(dy+1);
         double dw = (w1-w0)/(dy+1);

         x0 <<= 16;
         sx = (sx<<16)/(dy+1);

         int cnt = dx;
         if(  cnt > 300  ) cnt = 300;

         for( i = 0; i < cnt; ++i  )
         {
              int n = (int)colIndex;
              if( n < 0  ) n = 0; else if( n > 15 ) n = 15;
              colIndex += colDelta;

              w0 += dw;
              screen_width = (int)w0;
              if(  screen_width < 1  ) screen_width = 1;


              GRDrawParticle((x0>>16)+0,y0+0,screen_width,d_v0,m_color[n]);
              d_v0 += di;
              y0 += sigy;
              x0 += sx;
         }
    }
#endif
}


/* End of file C:\NW\ARENA\OBASE\DynObj\DynPartLn.cpp */
