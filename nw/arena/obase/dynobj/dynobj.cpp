#define LAST_H__VIEW
#include "game.h"
#include "DynObj.h"

void s_ViewDynamicObject::Draw()
 {
    m_ref.LoadLights(m_dwLights);
    m_ref.Draw();
 }

void  s_ViewDynamicObject::prepareToRender()
 {
    m_dynBase = m_dynBase1 = m_bump.start = m_ref.Center();
    m_bump.vel = CFVector3(0,0,0);
    m_bump.fTime = 0;
    m_bump.fRadius = m_ref.Model()->Radius();
 }
