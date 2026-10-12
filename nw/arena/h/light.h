#ifndef __LIGHT_H__INCLUDED
#define __LIGHT_H__INCLUDED

#include "mathlib.h"

class LightPoint
{
public:
    LightPoint *m_next;
    double      m_dist;
    double      m_radius;
    CFVector3   m_pos;
    int         m_brightness;
    int         m_color;
};

class LightChain
{
public:
   enum
   {
      MAX_LIGHT = 64
   };
   LightPoint m_dim[MAX_LIGHT];   

   LightPoint *m_list;
   int         m_count;

   LightChain()
   {
      m_list  = 0;
      m_count = 0;
   }
   double dist( const CFVector3 &pos ); // inline
   void add( const CFVector3 &pos, int color, int brightness, double radius );
   void render();
};

 //==========================================================================
inline double LightChain::dist( const CFVector3 &pos )
{
    return Abs2( CViewObject::m_viewPointInvMx.Offset() - pos );
}

extern LightChain g_lightChain;

#endif


