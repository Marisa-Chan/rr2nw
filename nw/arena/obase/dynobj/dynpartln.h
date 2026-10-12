/*
 * File   : C:\NW\ARENA\OBASE\DynObj\DynPartLn.h
 * Author : Suavik
 * Ver   1.0 
 */
#ifndef __DYNPARTLN_H__INCLUDED
#define __DYNPARTLN_H__INCLUDED


class s_ViewDynParticleLine : public  CViewSphericDynamic
 {
 public:
        
        CFVector3 m_p0, m_p1;
        double    m_step0, m_step;
        double    m_offset;             // Смещение первой точки
        double    m_radius0, m_radius1;
        unsigned long *m_color;

        s_ViewDynParticleLine()
         : m_p0(0,0,0), m_p1(0,0,0)
        {
             m_step0 = 1;
             m_step  = 1;
             m_offset = 0;
             m_radius0 = 1;
             m_radius1 = 1;
             m_color   = 0;
             m_bump.vel = CFVector3(0,0,0);
        }

	virtual void Draw();

    void         prepareToRender(
                                   const CFVector3 &p0,
                                   const CFVector3 &p1,
                                   double radius0, double radius1,
                                   double step0, double step,
                                   unsigned long *color,
                                   double offset = 0
                                )
    {
           m_p0 = p0;
           m_p1 = p1;
           m_radius0 = radius0;
           m_radius1 = radius1;
           m_step0   = step0;
           m_step    = step;
           m_color   = color;
           m_offset  = offset;

           m_dynBase = m_dynBase1 = m_bump.start = (p0+p1)*0.5;
           m_bump.fRadius = Abs(p0-m_dynBase);
           m_bump.vel = CFVector3(0,0,0);
           m_bump.fTime = 0;
    }
 };


#endif // ifndef __DYNPARTLN_H__INCLUDED
/* End of file C:\NW\ARENA\OBASE\DynObj\DynPartLn.h */
