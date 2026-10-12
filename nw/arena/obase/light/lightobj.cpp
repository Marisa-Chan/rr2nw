#define LAST_H__VIEW
#include "game.h"
#include "h\light.h"
#include "kernel/h/echo.h"

LightChain g_lightChain;


 //==========================================================================
void LightChain::add( const CFVector3 &pos, 
                      int              color, 
                      int              brightness, 
                      double           radius 
                    )
{
    if(  m_count == 0  )
    {
         m_list = m_dim;
         m_list->m_next       = 0;
         m_list->m_dist       = dist(pos);
         m_list->m_radius     = radius;
         m_list->m_pos        = pos;
         m_list->m_brightness = brightness;
         m_list->m_color      = color;
         m_count = 1;
    }
    else
    if(  m_count <  MAX_LIGHT  )
    {
         LightPoint *cur = &(m_dim[m_count]);

         cur->m_dist       = dist(pos);
         cur->m_radius     = radius;
         cur->m_pos        = pos;
         cur->m_brightness = brightness;
         cur->m_color      = color;
         
         if(  cur->m_dist <= m_list->m_dist )
         {
              cur->m_next = m_list;
              m_list = cur;
         }
         else
         for( 
              LightPoint *lp = m_list;;
              lp = lp->m_next
            )
              if(  lp->m_next == 0  )
              {
                   cur->m_next  = 0;
                   lp->m_next = cur;
                   break;
              }
              else
              if(  cur->m_dist <= lp->m_next->m_dist  )
              {
                   cur->m_next = lp->m_next;
                   lp->m_next  = cur;
                   break;
              }

         m_count++;
    }
    else echo("Lights overflow");
}

 //==========================================================================
void LightChain::render()
{

    int i    = 0;
    int mask = 0;

    for(
         LightPoint *lp = m_list;
         i < 32 && lp != 0;
         lp = lp->m_next, mask |= (1<<i), i++
       )
         CViewObject::SetLight( i,
                                lp->m_color ,
                                lp->m_pos,
                                lp->m_radius,
                                lp->m_brightness );

    CViewObject::EnableLights(mask);
    m_list  = 0;
    m_count = 0;
}

