#include "tank.h"
#include "phisics.h"
#include "i/player.i"
#include "kernel/h/echo.h"
#include "kernel/h/session.h"

void *Tank::queryInterface( int interNum )
 {
    switch( interNum )
    {
    case IUnknownIID:       return (KR_Object*)this;
    case IDynamicObjectIID: return (IDynamicObject*)this;
    case IUnitIID:          return (IUnit*)this;
    case ICarrierIID:       return (ICarrier*)this;
    }

    return 0;
 }

CFVector3 Tank::getPos()
{
    return getPosition();
}

double Tank::getHAngle()
{
    return m_angle;
}

CFVector3  Tank::getUpVector()
{
    return m_normal;
}

CFVector3  Tank::getCenter() // Относительно 0 объекта
{
    return m_skin.Model()->Center();
}

double Tank::getRadius() // Относительно центра
{
    return m_skin.Model()->Radius();
}

double Tank::getRadius0() // Относительно 0 объекта
{
    return m_skin.Model()->Radius0();
}

CFVector3  Tank::getMoveDir() // Направление движения
{
    CFVector3 d0(cos(m_angle),0,sin(m_angle));
    return d0-m_normal*(m_normal*d0);
}

double Tank::getMoveSpeed() // Скорость
{
    return m_speed;
}

 //=====================================================================
double Tank::getPower()
{
    return m_attr->m_power;
}

int Tank::isFriend( const KR_ObjectID &commanderID )
{
    return m_commander==commanderID;
}

double Tank::getDamage() // Целостность от 0..1
{
    return m_damage;
}

double Tank::desireShoot()
{
     return m_desireShoot*(m_attack+0.001);
}


void  Tank::getMatrix( CFMatrix3x4 &m )
{
    
    GetMatrixByAngles(m, m_normal, m_angle);
    m.TranslateL( getPosition() );
}

void   Tank::setDamage( double d, const CFVector3 &, double ts, KR_ObjectID fromID)
{
    if(  fromID.isNUL()  )
         m_damage -= d*g_levelAttr.m_unitDamageScale;
    else
    {
         IPlayer *p = (IPlayer*)(context->queryInterface(fromID,IPlayerIID));
         if(  p==0  )
              m_damage -= d*g_levelAttr.m_unitDamageScale;
         else m_damage -= d*g_levelAttr.m_unitDamageScaleFromPlayer;
    }

    if(  m_damage <= 0  )
    {
		if (m_isVisible)
		{
			createExplosion( 
				getPosition(), 
				ts,
				getObjectID(),
				m_attr->m_cacheExplosionTable,
				m_attr->m_cacheExplAttr
				);
			createCorpse(	getPosition() , 
				ts,
				getObjectID(),
				m_attr->m_cacheCorpseTable, 
				m_attr->m_cacheCorpseAttr);
		}
		
		context->removeObject(getObjectID() );
    }
}

KR_ObjectID Tank::getCommander()
{
    return m_commander;
}

void Tank::setCommander(KR_ObjectID)
{
    echo("Tank don't support setCommander");
}

void Tank::carrierLoadMatrix  (CFMatrix3x4 &m)
{
    getMatrix( m );
    m.TranslateR(CFVector3(0,10,0));
}

