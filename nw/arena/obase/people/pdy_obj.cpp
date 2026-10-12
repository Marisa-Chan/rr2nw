#include "People.h"
#include "message/peopmsg.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "kernel/h/session.h"
#include "zav.h"

void      *People::queryInterface( int interNum )
{
    switch( interNum )
    {
    case IUnknownIID:       return (KR_Object*)this;
    case IDynamicObjectIID: if(  m_isNotCreate  )
                                 return 0;
                            return (IDynamicObject*)this;

    case IUnitIID:          
                            return (IUnit*)this;
    }

    return 0;
}

// ----- interface IDynamicObject
CFVector3  People::getPos()
{
    CFMatrix3x4 m;
    getMatrix(m);
    return getPosition()+m*getCenter();
}

double     People::getHAngle   ()
{
     return m_rotateOy;
}

CFVector3  People::getUpVector ()
{
     return CFVector3(0,1,0); // FIXME
}

CFVector3  People::getCenter   () // Относительно 0 объекта
{
    return m_skin.Model()->Center();
}

double     People::getRadius   () // Относительно центра
{
    return m_skin.Model()->Radius();
}

double     People::getRadius0  () // Относительно 0 объекта
{
    return m_skin.Model()->Radius0();
}

CFVector3  People::getMoveDir  () // Направление движения
{
    return Normal(m_dir);
}

double     People::getMoveSpeed() // Скорость
{
    return Abs(m_dir);
}

void       People::getMatrix   ( CFMatrix3x4 &m )
{
    m.LoadIdentity()
     .RotateOyL(-m_hAngle-M_PI/2); // FIXME
}


double People::getPower   () // Сила юнита 0..10
{
   return 0.1;
}

int    People::isFriend   ( const KR_ObjectID &commanderID )
{
   if(  m_commanderID.isNUL()  )
        return 1;
   return m_commanderID==commanderID;
}

double People::getDamage  () // Целостность от 0..1
{
   return m_damage;
}

void   People::setDamage  ( double d, const CFVector3 &pos, double ts, KR_ObjectID fromID)
{
   if(  isShooter() && m_killed==KILL_NONE )
   {
        pushState(pe_STATE_ATTACK,fromID);

        if(  ts-m_lastDamageTime>2.0 )
        {
             context->removeEvent(pe_EVC_NEXTNODE,getObjectID());
             KR_Event event;
             event.label  = pe_EVC_NEXTNODE;
             event.source = getObjectID();
             event.destination = getObjectID();
             event.timeStamp   = ts;
             issueEvent( event );
        }
        m_lastDamageTime = ts;
   }

   m_damage -= d;
   if(  m_damage <= 0  )
   {
        onKilled(pos,ts);
   }
}

KR_ObjectID People::getCommander()
{
   return m_commanderID;
}

double People::desireShoot()
{
   return isShooter();
}

void   People::setCommander(KR_ObjectID oID)
{
    m_commanderID = oID;
    KR_Event    event;

    event.label = pe_EVC_FIND_ENEMY;
    event.source = getObjectID();
    event.destination = getObjectID();
    event.timeStamp = Session::m_moment+5.0;
    issueEvent( event );
}
