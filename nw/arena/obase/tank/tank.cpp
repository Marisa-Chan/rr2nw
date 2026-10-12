/*
 * File  : Suavik\D:\GAME\OBASE\Tank\tank.cpp
 * Autor : Suavik
 * Ver   1.0
 */
#include "tank.h"

#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "kernel/h/session.h"

#include "message/unitmsg.h"
#include "message/funitmsg.h"
#include "message/groupmsg.h"
#include "message/cnmsg.h"
#include "message/comanmsg.h"
#include "message/skinmsg.h"
#include "message/fountmsg.h"
#include "message/dcrossmsg.h"
#include "message/sndmsg.h"

#include "phisics.h"

#include "enum/spaceEnum.h"

#include "Sound.h"
#include "obase/sound/wavobj.h"

#include "storage\h\savefile.h"

#ifdef  __TRACE_NW__
#include "afxwin.h"
#else
class CDC{};
#endif
 //===========================================================================
AttributeTank g_defaultTankAttr;
int  checkStaticCollision( 
                     const CFVector3 &from,    // начало движения
                     const CFVector3 &mdir,    // напрвление со скоростью
                     double           mradius, // радиус
                     double           deltaT,  // время для проверки
                     double          &clzTime  // время, через которое стукнемся
                   );


 /*********************************
  *
  *   Tank implementation
  *
  *********************************/

 //============================================================
Tank::Tank()
   : m_viewDynObj( m_skin )
 {
    m_attr = 0;//&g_defaultTankAttr;
    m_power       = 0;
    m_speed       = 0;
    m_angle       = 0;
    m_prevTime    = 0;
    m_have_UNIT_I_DRIVE = 0;
 }

 //============================================================
Tank::~Tank()
 {
 }

void Tank::onView(double time)
{
   if(  (!m_runSmoke)  && m_attr->m_type == 1  )
   {
        KR_Event ev;
        ev.label       = t_EV_CREATE_SMOKE;
        ev.timeStamp   = time+m_attr->m_blobCreateTime;
        ev.source      = getObjectID();
        ev.destination = ev.source;
        issueEvent(ev);
        m_runSmoke     = 1;
   }
}

void Tank::setTankAttr()
{
	ct_Attribute *attr = g_tankAttrTable.searchAttribute(m_tankAttrID);
	if( attr==NULL )
		echo( "Tank::receiveEvent: Unknown attribute %s",
		context->searchObject(m_tankAttrID));
	else 
		m_attr = (AttributeTank*)attr;
	
	updateSound( getObjectID(),
		context,
		m_attr->m_ctsndID,
		m_attr->m_wav,
		m_snd );
	
	

	m_skin.Attach(m_attr->m_cacheSkin);
	m_viewDynObj.BumpDef().fRadius = m_skin.Model()->Radius();
	
	switch( m_attr->m_type )
	{
	case 0: 
		m_skin.SetAnimationCallback(AnimateCallBack0); 
		break;
		
	case 1: 
		m_skin.SetAnimationCallback(AnimateCallBack1); 
		break;
	}

	m_skin.SetUserAttrib(this);
	m_askin = (ISkin*)(context->queryInterface(m_attr->m_skinID, ISkinIID));
        if( m_askin->isAutoAnim()  )
            m_askin->skinSetAnimAuto(&m_skin);
}

 //============================================================
int Tank::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case UNIT_I_DRIVE: 
            {
              m_lastDeltaT        = event.timeStamp-m_lastMoveTimeStamp;
              m_lastMoveTimeStamp = event.timeStamp;
              m_lastMovePos       = getPosition();

              m_maxDistScale++;
              m_have_UNIT_I_DRIVE = 0;

              if(  m_maxDistScale > m_attr->m_maxDistScale  )
              {
                   m_maxDistScale = 0;
                   if(  Abs(getPosition() - m_prevPos ) > m_maxMoveDist  )
                   {
                        event.label       = t_EV_STOP;
                        event.source      = getObjectID();
                        event.destination = getObjectID();
                        //{{PUT_EVENT(t_EV_STOP)
                        //}}END_OF_PUT_EVENT(t_EV_STOP)
                        issueEvent( event );
                        break;
                   }
              }
              //
              // Расчет фазы для анимации
              //
              double sp = 16*m_attr->rotSpeed;
              if(  sp < m_speed ) sp = m_speed;
              m_phase += sp*(event.timeStamp-m_prevTime);

              model_drive( event.timeStamp,
                           m_needPow,
                           m_needAng );
              carrierOnMove();
	      if (!m_snd.isNUL())       
              {
               CFVector3 pos(getPosition());
               event.label = snd_EV_MOVE_TO;
               event.destination = m_snd;
               event.data.open(EDO_WRITE)
                         .putDouble(pos.x)
                         .putDouble(pos.y)
                         .putDouble(pos.z)
                      .close();
               context->sendEventNow( event );
              }
            }
            break;


    case KR_SET_ATTR:
		{
            //KR_ObjectID oID;
            event.data.open(EDO_READ)
				.getObjectID(m_tankAttrID)
				.close();
			
			setTankAttr();
			onSetAttr(event.timeStamp);
            
            switch( m_attr->m_type )
            {
            case 0: 
				m_towerAxis = *(CFVector3*)(
                    m_skin.Model()->BaseSet(0).Base(0).Names().vertices["axisTower"][0]);
				m_towerAxis += *(CFVector3*)(
                    m_skin.Model()->BaseSet(0).Base(0).Names().vertices["axisTower"][1]);
				m_towerAxis *= 0.5;                    
				break;
            }
		}
            break;

    case fu_EV_QUERY_DAMAGE:   // double
            event.data.open(EDO_WRITE)
                       .putDouble(m_damage)
                      .close();
            event.label = fu_EV_QUERY_DAMAGE_OK;
            break;

    case fu_EV_QUERY_COORD:    // (VECTOR_LAND,0){ double x, double z }
            {
            CFVector3 pos(getPosition());

            event.data.open(EDO_WRITE)
                         .descend(VECTOR_LAND,0)
                             .putDouble(pos.x)
                             .putDouble(pos.z)
                         .ascend()
                      .close();
            event.label = fu_EV_QUERY_COORD_OK;
            }
            break;

    case fu_EV_QUERY_POSITION:
            {
            CFVector3 pos(getPosition());
            event.data.open(EDO_WRITE)
                         .descend(VECTOR3D_F,0)
                             .putDouble(pos.x)
                             .putDouble(pos.y)
                             .putDouble(pos.z)
                         .ascend()                      
                      .close();
            event.label = fu_EV_QUERY_POSITION_OK;
            }
            break;

    case fu_EV_QUERY_COMMANDER:// KR_ObjectID
            event.data.open(EDO_WRITE)
                          .putObjectID(m_commander)
                      .close();
            event.label = fu_EV_QUERY_COMMANDER_OK;
            break;

    case fu_EV_QUERY_POWER:    // double
            event.data.open(EDO_WRITE)
                          .putDouble(m_attr->m_power)
                      .close();
            event.label = fu_EV_QUERY_POWER_OK;
            break;

    case fu_EV_QUERY_ISFRIEND:
            {
               KR_ObjectID com;

               event.data.open(EDO_READ)
                            .getObjectID(com)
                         .close();
               event.data.open(EDO_WRITE)
                            .putInt(com==m_commander)
                         .close();
               event.label = fu_EV_QUERY_ISFRIEND_OK;
            }
            break;

    case t_EV_CREATE_SMOKE:
            {
              if(  m_isVisible  )
              {
                    event.label      = t_EV_CREATE_SMOKE;
                    event.destination= getObjectID();
                    event.source     = getObjectID();
                    event.timeStamp += m_attr->m_blobCreateTime + context->rnd_f(0.3);
                    issueEvent( event );
                    m_runSmoke = 1;
              }
              else m_runSmoke = 0;

              event.label       = fou_EVCMD_START;
              event.destination = g_arena.newObject(  m_attr->m_smokeTableID,"Smok." );

              if(  !event.destination.isNUL()  )
              {
                   CFVector3   sp( 0.02, 10.85, 3.35 );
                   CFMatrix3x4 m;
                   getMatrix(m);
                   sp = m*sp;

                   event.source      = getObjectID();
                   event.data.open(EDO_WRITE)
                                .putObjectID( m_attr->m_smokeAttrID )
                                .putDouble(sp.x)
                                .putDouble(sp.y)
                                .putDouble(sp.z)
                             .close();
                   context->sendEventNow( event );
              }
            }
            break;

    case KR_WAKE_UP:
            break;

    case t_EVCMD_SET_POSITION:
     {
           double x,y,z;
           event.data.open(EDO_READ)
                       .getDouble(x)
                       .getDouble(y)
                       .getDouble(z)
                     .close();
           setPosition( CFVector3(x,y,z) );

           double clzTime = 0;

           if(  m_attr->m_isCannon  )
           {
                 if(  checkStaticCollision( 
                     getPosition(),    // начало движения
                     CFVector3(0,-1,0),    // напрвление со скоростью
                     1, // радиус
                     500,  // время для проверки
                     clzTime // время, через которое стукнемся
                   ) ) 
                      setPosition( CFVector3(x,y-clzTime-1,z) );
           }
     }
     break;


    case com_EV_I_AM_DEAD:
			
			m_damage = -1;
			setDamage( 0, m_position, event.timeStamp, KR_ObjectID::NUL() );

			break;

    case com_EV_GROUP_YOUR_MY_MEMBER:
            //m_group = event.source;
            event.data.open(EDO_READ)
						.getObjectID(m_group)
                        .getObjectID(m_commander)
                      .close();
            break;

    case cn_EV_I_HAVE_SHOOT:
            m_attack = false;

            m_desireShoot -= g_levelAttr.m_subDesire;
            if(  m_desireShoot < g_levelAttr.m_minDesire  )
                 m_desireShoot = g_levelAttr.m_minDesire;

            if(  m_desireShoot < g_levelAttr.m_desireForgetThreshold  )
                 forgetEnemy(event.timeStamp);
            break;

    default:
            {
            int ok =  EVENTHANDLER( event );

            //s_ASSERT(ok,"Unknown message");
            if(!ok)
            {
                ok = carrierReceiveEvent(event);
                if(!ok)
                echo("Unknown message [%i] my state %i",event.label,m_currentState);
            }
            return ok;
            }
    }
    return 1;
 }


void addCannon( Tank &tank, int index, double ts )
{
    KR_Event    event;
    KR_ObjectID cannon = g_arena.newObject( tank.m_attr->m_cacheCannonTable,
                                            "Tank.Cannon");

    if(  event.destination.isNUL()  )
         return;

    event.label       = KR_SET_ATTR;
    event.source      = tank.getObjectID();
    event.destination = cannon;
    event.timeStamp   = ts;
    event.data.open(EDO_WRITE)
                .putInt(index)
              .close();
    tank.getContext()->sendEventNow( event );


    tank.m_cannons.add(  cannon );
}

void Tank::onSetAttr(double ts)
{
    int i;

    for( i = 0; i < m_attr->m_cannonCnt; ++i )
         addCannon( *this, m_attr->m_cannonIndex[i], ts );

    KR_Event event;
    event.timeStamp = ts;

    for( i = 0; i < m_cannons.getCount(); ++i )
    {
         event.label       = tg_EV_REBUILD;
         event.destination = m_cannons[i];
         event.source      = getObjectID();
         //{{PUT_EVENT(tg_EV_REBUILD)
         //}}END_OF_PUT_EVENT(tg_EV_REBUILD)
         context->sendEventNow( event );
    }

    massa_D       = 1.0 / m_attr->massa;
    maxPower_D    = 1.0 / m_attr->maxPower;

}

 //============================================================
void Tank::addNotify()
 {
    loadStateTransitionTable();
    ct_Subject::addNotify();
    // insert your code this
    startInitialize();
    resetState();
    setPosition(CFVector3(2060+rand()%10,0,-2390-rand()%10));
    m_runSmoke     = 0;
    m_stopMode     = 0;
    m_needPow      = 0;
    m_needAng      = 0;
    m_lastDeltaT   = 0;

    carrierAddNotify(context,Session::m_moment);
    // Green  BEGIN
    InitNearAI(*this);
    // Green END
 }

 //============================================================
void Tank::removeNotify()
 {
    carrierRemoveNotify(context,Session::m_moment);

    if(  !m_snd.isNUL()  )
         context->removeObject( m_snd );

    ct_Subject::removeNotify();    
    // insert your code this
    // context->removeObject(m_snd);

    ShutdownNearAI(*this); // Green
 }

 //============================================================
#ifdef  __TRACE_NW__
void Tank::draw( CDC &gc )
 {
    int x = (int)m_position.x,
        y = -(int)m_position.z;
    gc.Ellipse( x-3,y-3,x+3,y+3  );
    
 }
#else
void Tank::draw( CDC & ) {}
#endif

 /*******************************
  *
  * Приказ двигаться в заданном направлении.
  * Дается командирог группы
  *
  *******************************/
void Tank::from_STAY__to__MOVING__F( KR_Event &event )
{
    double xVec,yVec;
    double maxMoveDist;
    double moveSpeed;

    //{{GET_EVENT(t_EV_MOVE)
    event.data.open(EDO_READ)
                   .getDouble(moveSpeed)
                   .descend( VECTOR_LAND, 0 )
                     .getDouble(xVec)
                     .getDouble(yVec)
                   .ascend()
                   .getDouble(maxMoveDist)
              .close();
    //}}END_OF_GET_EVENT(t_EV_MOVE)

    m_stopMode    = moveSpeed<0.01;
    m_maxMoveDist = maxMoveDist;

    gotoMOVING( event );

    model_drive( event.timeStamp,
                 moveSpeed,
                 atan2(yVec,xVec)
               );
}

 //============================================================
void Tank::onExitAudibleZone(double ts) 
{
   if(  !m_snd.isNUL()  )
    {
     KR_Event event;
     event.destination = m_snd;
     event.source      = getObjectID();
     event.timeStamp   = ts;
     event.label       = snd_EV_END;
     context->sendEventNow( event );
    }
}

 //============================================================

void Tank::onEnterAudibleZone(double ts) 
{
   // Sound
    if(  !m_snd.isNUL() )
    {
     KR_Event event;


     CFVector3 pos(getPosition());
     event.label = snd_EV_MOVE_TO;
     event.destination = m_snd;
     event.source      = getObjectID();
     event.timeStamp   = ts;

     event.data.open(EDO_WRITE)
               .putDouble(pos.x)
               .putDouble(pos.y)
               .putDouble(pos.z)
              .close();
     context->sendEventNow( event );
   
     
     event.label       = snd_EV_START;
     event.data.open(EDO_WRITE)
            .putInt(0)
         .close();
     context->sendEventNow( event );
    } 
} 


 //============================================================


 /*******************************
  *
  * Приказ остановитьс
  *
  *******************************/
void Tank::from_MOVING__to__STAY__F( KR_Event &event )
{
    //{{GET_EVENT(t_EV_STOP)
    //}}END_OF_GET_EVENT(t_EV_STOP)
    m_stopMode = 1;
    m_needPow  = 0;
    model_drive(event.timeStamp,m_needPow,m_angle);
}

 /*******************************
  *
  * Продолжать двигаться и атаковать
  *
  *******************************/
void Tank::from_MOVING__to__MOVING_SHOOTING__F( KR_Event &event )
{
    KR_ObjectID enemyID;

    //{{GET_EVENT(t_EV_ATTACK)
    event.data.open(EDO_READ)
                   .getObjectID(enemyID)
              .close();
    //}}END_OF_GET_EVENT(t_EV_ATTACK)
    m_attackedEnemy = enemyID;

    gotoMOVING_SHOOTING( event );
}

void Tank::from_MOVING_SHOOTING__to__STAY_SHOOTING__F( KR_Event &event )
{
    //{{GET_EVENT(t_EV_STOP)
    //}}END_OF_GET_EVENT(t_EV_STOP)
    m_prevTime = event.timeStamp;
    context->removeEvent( t_EVC_MOVING_AND_SHOOTING, getObjectID() );
    m_stopMode = 1;
    m_needPow  = 0;
    model_drive(event.timeStamp,m_needPow,m_angle);
}

 /*******************************
  *
  * Потеря цели
  *
  *******************************/
void Tank::from_STAY_SHOOTING__to__STAY__F( KR_Event &event )
{
    KR_ObjectID targetID;

    //{{GET_EVENT(t_EV_TARGET_LOCKED)
    event.data.open(EDO_READ)
                   .getObjectID(targetID)
              .close();
    //}}END_OF_GET_EVENT(t_EV_TARGET_LOCKED)

    if(  m_attackedEnemy == targetID  )
         m_attackedEnemy = KR_ObjectID::NUL();
}

void Tank::from_STAY__to__STAY_SHOOTING__F( KR_Event &event )
{
    KR_ObjectID enemyID;

    //{{GET_EVENT(t_EV_YOU_ATTACKED)
    event.data.open(EDO_READ)
                   .getObjectID(enemyID)
              .close();
    //}}END_OF_GET_EVENT(t_EV_YOU_ATTACKED)

    m_attackedEnemy = enemyID;
}

void Tank::from_STAY__to__STAY_SHOOTING__F_attack( KR_Event &event )
{
    KR_ObjectID enemyID;
    //{{GET_EVENT(t_EV_ATTACK)
    event.data.open(EDO_READ)
                   .getObjectID(enemyID)
              .close();
    //}}END_OF_GET_EVENT(t_EV_ATTACK)
    m_attackedEnemy = enemyID;
}


void Tank::from_MOVING_SHOOTING__to__MOVING__F( KR_Event & )
{
    //{{GET_EVENT(t_EV_TARGET_LOST)
    //}}END_OF_GET_EVENT(t_EV_TARGET_LOST)
    context->removeEvent( t_EVC_MOVING_AND_SHOOTING, getObjectID() );
}

void Tank::from_STAY_SHOOTING__to__MOVING_SHOOTING__F( KR_Event &event )
{
    double xVec,yVec;
    double maxMoveDist;
    double moveSpeed;

    //{{GET_EVENT(t_EV_MOVE)
    event.data.open(EDO_READ)
                   .getDouble(moveSpeed)
                   .descend( VECTOR_LAND, 0 )
                     .getDouble(xVec)
                     .getDouble(yVec)
                   .ascend()
                   .getDouble(maxMoveDist)
              .close();
    //}}END_OF_GET_EVENT(t_EV_MOVE)

    m_maxMoveDist = maxMoveDist;
    m_stopMode    = moveSpeed<0.01;

    gotoMOVING_SHOOTING(event);
    model_drive( event.timeStamp,
                 moveSpeed,
                 atan2(yVec,xVec)
               );

}

void Tank::stayAndShootingFC( KR_Event &event )
{
    //{{GET_EVENT(t_EVC_STAY_AND_SHOOTING)
    //}}END_OF_GET_EVENT(t_EVC_STAY_AND_SHOOTING)

    if(  !m_attack  )
    if(  !m_attackedEnemy.isNUL() )
         attack(m_attackedEnemy,event);

    event.label       = t_EVC_STAY_AND_SHOOTING;
    event.source      = getObjectID();
    event.destination = getObjectID();
    event.timeStamp  += m_attr->m_movingAndShootingIncrement;

    //{{PUT_EVENT(t_EVC_STAY_AND_SHOOTING)
    //}}END_OF_PUT_EVENT(t_EVC_STAY_AND_SHOOTING)
    issueEvent( event );
}

void Tank::movingFC( KR_Event &event )
{
    double prevTimeStamp;

    //{{GET_EVENT(t_EVC_MOVING)
    event.data.open(EDO_READ)
                   .getDouble(prevTimeStamp)
              .close();
    //}}END_OF_GET_EVENT(t_EVC_MOVING)

    prevTimeStamp    = event.timeStamp;
    //event.timeStamp += m_attr->m_mvTimeInc;
    event.timeStamp += getContext()->rnd_f(m_attr->m_mvTimeInc*0.8, m_attr->m_mvTimeInc*1.2);

    if(  !m_stopMode  )
    {
          //dc_CreateCross(0,getPosition(),10,0,0,1,1,0,".....Move");
          Move(*this, prevTimeStamp);
    }
    else  m_prevTime = prevTimeStamp;

    //{{PUT_EVENT(t_EVC_MOVING)
    event.data.open(EDO_WRITE)
                   .putDouble(prevTimeStamp)
              .close();
    //}}END_OF_PUT_EVENT(t_EVC_MOVING)
    issueEvent( event );
}

void Tank::movingAndShootingFC( KR_Event &event )
{
    double prevTimeStamp;

    //{{GET_EVENT(t_EVC_MOVING_AND_SHOOTING)
    event.data.open(EDO_READ)
                   .getDouble(prevTimeStamp)
              .close();
    //}}END_OF_GET_EVENT(t_EVC_MOVING_AND_SHOOTING)

    if(  !m_attack  )
    if(  !m_attackedEnemy.isNUL() )
    {
         //char b[30];
         //sprintf(b,"DES...%lg",m_desireShoot);
         //dc_CreateCross(0,m_position+CFVector3(3,0,3),10,0,0,1,1,0,b);
         if(  m_desireShoot > g_levelAttr.m_desireAttackThreshold )
              attack(m_attackedEnemy,event);
    }

    event.label       = t_EVC_MOVING_AND_SHOOTING;
    event.source      = getObjectID();
    event.destination = getObjectID();
    event.timeStamp  += m_attr->m_movingAndShootingIncrement;
    //{{PUT_EVENT(t_EVC_MOVING_AND_SHOOTING)
    event.data.open(EDO_WRITE)
                   .putDouble(prevTimeStamp)
              .close();
    //}}END_OF_PUT_EVENT(t_EVC_MOVING_AND_SHOOTING)
    issueEvent( event );
}

 /*******************************
  *
  * Коррекция направления движения
  *
  *******************************/
void Tank::correctDirFC( KR_Event &event )
{
    double moveSpeed;
    double xVec,
           yVec,
           maxMoveDist;

    //{{GET_EVENT(t_EV_MOVE)
    event.data.open(EDO_READ)
                   .getDouble(moveSpeed)
                   .descend( VECTOR_LAND, 0 )
                     .getDouble(xVec)
                     .getDouble(yVec)
                   .ascend()
                   .getDouble(maxMoveDist)
              .close();
    //}}END_OF_GET_EVENT(t_EV_MOVE)

    m_stopMode = moveSpeed<0.01;
    m_needPow  = moveSpeed;

//    if(m_stopMode)dc_CreateCross(0,getPosition(),10,0,0,1,1,0,".....OK");

    // Green BEGIN
    double l = hypot(xVec, yVec);
    if(l <= 0) l = 1;
    m_destDir = CFVector2(xVec/l, yVec/l);
    // Green END

    if(  m_attackedEnemy.isNUL()  )
    {
         event.source = getObjectID();
         double hAngle = m_angle;
         double vAngle = 0;

         for( int i = 0; i < m_cannons.getCount(); ++i )
              rotateCannon(m_cannons[i],event,hAngle,vAngle);
    }
}


void Tank::rotateAndShooting( KR_Event &event )
{
    context->removeEvent(t_EVC_MOVING,getObjectID());

    double prevTimeStamp, hAngle, vAngle;
    int count, bulletAttrIndex;

    //{{GET_EVENT(cn_EVCMD_ROTATE_AND_SHOOT)
    event.data.open(EDO_READ)
                   .descend(cn_ROTATE , 0 )
                     .getDouble(prevTimeStamp)
                     .getDouble(hAngle)
                     .getDouble(vAngle)
                   .ascend()
                   .descend( cn_SHOOT, 0 )
                     .getInt(count)
                     .getInt(bulletAttrIndex)
                   .ascend()
              .close();
    //}}END_OF_GET_EVENT(cn_EVCMD_ROTATE_AND_SHOOT)

    save_cannonID = event.source;
    save_hAngle = hAngle;
    save_vAngle = vAngle;
    save_count  = count;
    save_attr   = bulletAttrIndex;

    m_stopMode = 1;
    model_drive( event.timeStamp,0,hAngle);

    event.source     = getObjectID();
    event.label      = t_EVC_CHECK_ROTATE;
    event.timeStamp += m_attr->m_checkRotateTime;
    issueEvent( event );

    context->removeEvent(t_EVC_MOVING_AND_SHOOTING,getObjectID());
}

void Tank::shooting( KR_Event &event )
{
    //{{GET_EVENT(t_EV_REACHED)
    //}}END_OF_GET_EVENT(t_EV_REACHED)
    gotoMOVING_SHOOTING( event );
}

void Tank::checkRotate( KR_Event &event )
{
    //{{GET_EVENT(t_EVC_CHECK_ROTATE)
    //}}END_OF_GET_EVENT(t_EVC_CHECK_ROTATE)
    double d = m_angle - save_hAngle;

    if(  d*d==d && d!=0  ) d = 0;
    while( d <= -M_PI  ) d += M_PI*2;
    while( d >=  M_PI  ) d -= M_PI*2;
    d = fabs(d);
    if(  d <= m_attr->m_checkRotateDelta )
    {
         int count      = save_count, 
         bulletAttr     = save_attr;
         double hAngle  = save_hAngle,
                vAngle  = save_vAngle;

//         double temphAngle;
//         CalcCannonParam(*this, enemyPos,myPos,temphAngle, vAngle);


         event.label       = cn_EVCMD_DIRECT_SHOOT;
         event.destination = save_cannonID;
         event.source      = getObjectID();
         //{{PUT_EVENT(cn_EVCMD_DIRECT_SHOOT)
         event.data.open(EDO_WRITE)
                        .putDouble(hAngle)
                        .putDouble(vAngle)
                        .putInt(bulletAttr)
                        .putInt(count)
                   .close();
         //}}END_OF_PUT_EVENT(cn_EVCMD_DIRECT_SHOOT)
         issueEvent( event );

         event.label       = t_EV_REACHED;
         event.destination = getObjectID();
         event.source      = getObjectID();
         issueEvent( event );
         m_stopMode = 0;
    }
    else
    {
         event.label      = t_EVC_CHECK_ROTATE;
         event.timeStamp += m_attr->m_checkRotateTime;
         issueEvent( event );

         m_checkRotateCount++;
         if(  (m_checkRotateCount&3)==0  )
         {
              IDynamicObject *dobj = (IDynamicObject*)
                         (context->queryInterface(m_attackedEnemy,IDynamicObjectIID));
              CFVector3 enemyPos;

              if(  dobj!= 0 )
              {
                   enemyPos = dobj->getPos();
                   CalcCannonParam(m_attr->m_bulletSpeed,enemyPos,getPosition(),
                       save_hAngle, save_vAngle,dobj->getMoveDir()*getMoveSpeed());
                   m_needAng = save_hAngle;
              }
         }
    }
}

void Tank::correctDirFC_hidden( KR_Event &event )
{
    double moveSpeed;
    double xVec,
           yVec,
           maxMoveDist;

    //{{GET_EVENT(t_EV_MOVE)
    event.data.open(EDO_READ)
                   .getDouble(moveSpeed)
                   .descend( VECTOR_LAND, 0 )
                     .getDouble(xVec)
                     .getDouble(yVec)
                   .ascend()
                   .getDouble(maxMoveDist)
              .close();
    //}}END_OF_GET_EVENT(t_EV_MOVE)

    m_stopMode = moveSpeed<0.01;
    // Green BEGIN
    double l = hypot(xVec, yVec);
    if(l <= 0) l = 1;
    m_destDir = CFVector2(xVec/l, yVec/l);
    // Green END
}

void Tank::iAmBusyF( KR_Event &event )
{
    KR_ObjectID enemyID;
    //{{GET_EVENT(t_EV_ATTACK)
    event.data.open(EDO_READ)
                   .getObjectID(enemyID)
              .close();
    //}}END_OF_GET_EVENT(t_EV_ATTACK)

    if (m_group.isNUL())
	return;

    event.label       = t_EV_TARGET_LOST;
    event.source      = getObjectID();
    event.destination = m_group;
    event.data.open(EDO_WRITE)
                 .putObjectID(enemyID)
              .close();
    issueEvent( event );

    //char s[100];
    //sprintf(s,"A AM BUSY. MY STATE %i  %lf",m_currentState,m_desireShoot);
    //dc_CreateCross(0,getPosition(),10,0,0,1,1,0,s);
}

//{{ACTIONF_IMPLEMENTATION

void Tank::loadStateTransitionTable()
 {
    //{{TRANSLATION_TABLE
    static KR_ActiveObject::StateTransitionTableElem row0[3] =
    {
        {t_EV_ATTACK              , ST_NEW_STATE,  3, (ACTION)from_STAY__to__STAY_SHOOTING__F_attack     },
        {t_EV_YOU_ATTACKED        , ST_NEW_STATE,  3, (ACTION)from_STAY__to__STAY_SHOOTING__F            },
        {t_EV_MOVE                , ST_NEW_STATE,  1, (ACTION)from_STAY__to__MOVING__F                   }
    };
    static KR_ActiveObject::StateTransitionTableElem row1[4] =
    {
        {t_EVC_MOVING             , ST_NEW_STATE,  1, (ACTION)movingFC                                   },
        {t_EV_MOVE                , ST_NEW_STATE,  1, (ACTION)correctDirFC                               },
        {t_EV_ATTACK              , ST_NEW_STATE,  2, (ACTION)from_MOVING__to__MOVING_SHOOTING__F        },
        {t_EV_STOP                , ST_NEW_STATE,  0, (ACTION)from_MOVING__to__STAY__F                   }
    };
    static KR_ActiveObject::StateTransitionTableElem row2[7] =
    {
        {t_EV_ATTACK              , ST_NEW_STATE,  2, (ACTION)iAmBusyF                                   },
        {cn_EVCMD_ROTATE_AND_SHOOT, ST_NEW_STATE,  4, (ACTION)rotateAndShooting                          },
        {t_EVC_MOVING             , ST_NEW_STATE,  2, (ACTION)movingFC                                   },
        {t_EV_MOVE                , ST_NEW_STATE,  2, (ACTION)correctDirFC                               },
        {t_EVC_MOVING_AND_SHOOTING, ST_NEW_STATE,  2, (ACTION)movingAndShootingFC                        },
        {t_EV_TARGET_LOST         , ST_NEW_STATE,  1, (ACTION)from_MOVING_SHOOTING__to__MOVING__F        },
        {t_EV_STOP                , ST_NEW_STATE,  3, (ACTION)from_MOVING_SHOOTING__to__STAY_SHOOTING__F }
    };
    static KR_ActiveObject::StateTransitionTableElem row3[4] =
    {
        {t_EV_ATTACK              , ST_NEW_STATE,  3, (ACTION)iAmBusyF                                   },
        {t_EVC_STAY_AND_SHOOTING  , ST_NEW_STATE,  3, (ACTION)stayAndShootingFC                          },
        {t_EV_MOVE                , ST_NEW_STATE,  2, (ACTION)from_STAY_SHOOTING__to__MOVING_SHOOTING__F },
        {t_EV_TARGET_LOST         , ST_NEW_STATE,  0, (ACTION)from_STAY_SHOOTING__to__STAY__F            }
    };
    static KR_ActiveObject::StateTransitionTableElem row4[5] =
    {
        {t_EV_ATTACK              , ST_NEW_STATE,  4, (ACTION)iAmBusyF                                   },
        {t_EV_TARGET_LOST         , ST_NEW_STATE,  4, (ACTION)NULL                                       },
        {t_EV_REACHED             , ST_NEW_STATE,  2, (ACTION)shooting                                   },
        {t_EV_MOVE                , ST_NEW_STATE,  4, (ACTION)correctDirFC_hidden                        },
        {t_EVC_CHECK_ROTATE       , ST_NEW_STATE,  4, (ACTION)checkRotate                                }
    };
    //}}END_OF_TRANSLATION_TABLE{{
    static StateElem STT[5]={
      StateElem(3,row0),
      StateElem(4,row1),
      StateElem(7,row2),
      StateElem(4,row3),
      StateElem(5,row4)
    };
    m_stateTable = STT;
    m_stateQnty  = 5;
    //}}END_OF_STATE_TABLE
 }


class TankTable : public ct_SubjectTable
{
 private:
    Tank *m_table;
 public:
    virtual  bool        isRendering();
    TankTable()
    {
        m_table = NULL;
        registerClass( "Tank" );
	m_defaultAddMode = CT_KILLINVISIBLE;
    }
    ~TankTable()
    {
        delete [] m_table;
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );

    virtual bool isAudible();
};


static TankTable  __classTable;
AttributeTable g_tankAttrTable;


 /*************************************
  *
  *   TankTable implementation
  *
  *************************************/


//============================================================

bool TankTable::isAudible()
{
  return true;
}


 //============================================================
void TankTable::allocObjects( int objectQnty )
 {
    m_table = new Tank[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void TankTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *TankTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"TankTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 //============================================================
bool TankTable::isRendering()
 {
    return true;
 }

 /*************************************
  *
  *   AttributeTable implementation
  *
  *************************************/

 //============================================================
void AttributeTable::allocObjects( int objectQnty )
 {
    m_table = new AttributeTank[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void AttributeTable::freeObjects()
 {
    delete [] m_table;
    m_table         = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *AttributeTable::getObjectPTR( int index )
 {
    s_ASSERT(index>=0 && index <m_maxObjectQnty,"AttributeTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 //============================================================
void AttributeTank::wakeUp( KR_Event &event )
 {
    m_cacheCannonTable     = g_arena.searchSeanceClassTable( m_cannonTable );
    m_cacheBulletAttrTable = g_arena.searchSeanceClassTable( m_bulletAttrTable );

    const KR_ObjectID &bulletAttrID = context->searchObject(m_bulletAttr);

    m_cacheBulletAttr      = g_arena.getAttributeIndex(
                                       m_cacheBulletAttrTable,
                                       bulletAttrID
                                      );
    event.label       = fu_EV_QUERY_SPEED;
    event.destination = bulletAttrID;
    context->sendEventNow( event );
    s_ASSERT(event.label == fu_EV_QUERY_SPEED_OK,"AttributeTank: Dont support" );
    event.data.open(EDO_READ)
                   .getDouble(m_bulletSpeed)
              .close();

    KR_ObjectID skinID =  context->searchObject( m_skin );
    m_skinID = skinID;

    s_ASSERT( !skinID.isNUL(), "AttributeTank::update" );
    event.label = sk_EV_QUERY_MODEL_PTR;
    event.destination = skinID;
    context->sendEventNow( event );
    s_ASSERT(event.label==sk_EV_QUERY_MODEL_PTR_OK,"AttributeTank::update");
    event.data.open(EDO_READ)
                .get(&m_cacheSkin,sizeof(void*))
              .close();

    m_smokeTableID = g_arena.searchSeanceClassTable( m_smokeTableName );
    m_smokeAttrID       = context->searchObject(m_smokeAttrName);


    int cannonAttr = g_arena.searchSeanceClassTable( "CannonAttr" );
    if(  m_cannonCnt > 0  )
    m_cannonIndex[0] = g_arena.getAttributeIndex( cannonAttr,
                                                  context->searchObject(m_cannon0));

    if(  m_cannonCnt > 1  )
    m_cannonIndex[1] = g_arena.getAttributeIndex( cannonAttr,
                                                  context->searchObject(m_cannon1));

    if(  m_cannonCnt > 2  )
    m_cannonIndex[2] = g_arena.getAttributeIndex( cannonAttr,
                                                  context->searchObject(m_cannon2));

    powerDelta = maxPower*MODEL_TIME_DELTA_FORWARD/powerDeltaTime;
 }

 //============================================================
void AttributeTank::update( double ts)
 {
    KR_Event event;
    event.timeStamp = ts;
    wakeUp(event);

    if (!SetSoundAttr(getObjectID(), context,m_soundName,m_ctsndID, (void * ) &m_wav))
        m_wav = NULL;

    m_cacheCorpseTable	= g_arena.searchSeanceClassTable( "Corpse" );
    m_cacheCorpseAttr	= g_arena.getAttributeIndex( 
                                    g_arena.searchSeanceClassTable( "CorpseAttr" ),
                                    context->searchObject(m_corpseAttrName) 
                                 );

    m_cacheExplosionTable = g_arena.searchSeanceClassTable( "Explosion" );

    m_cacheExplAttr = g_arena.getAttributeIndex( 
                                    g_arena.searchSeanceClassTable( "ExplosionAttr" ),
                                    context->searchObject(m_explAttrName) 
                                 );
 }


bool	Tank::dump(PIN_SaveFile & sf)
{
		if (	!ct_Subject::dump(sf) ||
			!KR_ActiveObject::dump(sf) ||
			!ICarrier::dump(sf) ||
			!m_cannons.dump(sf) ||
			!sf.WriteData( (char *) & m_tankAttrID, sizeof(TankData)  ))
			return false;
		
		return true;
}

bool	Tank::load(PIN_SaveFile & sf)
{
		if (	!ct_Subject::load(sf) ||
			!KR_ActiveObject::load(sf) ||
			!ICarrier::load(sf) ||
			!m_cannons.load(sf) ||
			!sf.GetData( (char *) & m_tankAttrID, sizeof(TankData)  ))
			return false;
		
		return true;
}

void	Tank::loadNotify()
{
	loadStateTransitionTable();
	ct_Subject::loadNotify();
	ICarrier::loadNotify();
	setTankAttr();
}

/* End of file D:\GAME\OBASE\Tank\tank.cpp */