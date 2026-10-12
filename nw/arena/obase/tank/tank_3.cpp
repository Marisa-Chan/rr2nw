#include "tank.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "enum/spaceEnum.h"

#include "scene.h"
#include "phisics.h"
#include "vehicle.h"

#include "message/unitmsg.h"
#include "message/funitmsg.h"
#include "message/cnmsg.h"
#include "message/groupmsg.h"
#include "message/dcrossmsg.h"

#ifdef  __TRACE_NW__
#include "afxwin.h"
#else
class CDC{};
#endif


// FRIEND UNIT MSG
//{{EVENT_LABEL_NAMES
static s_ELN elnTable[]=
{
   s_ELN(t_EV_MOVE,"t_EV_MOVE"),
   s_ELN(t_EV_STOP,"t_EV_STOP"),
   s_ELN(t_EV_ATTACK,"t_EV_ATTACK"),
   s_ELN(t_EV_TARGET_LOCKED,"t_EV_TARGET_LOCKED"),
   s_ELN(t_EV_YOU_ATTACKED,"t_EV_YOU_ATTACKED"),
   s_ELN(t_EV_TARGET_LOST,"t_EV_TARGET_LOST"),
   s_ELN(t_EV_RETREATING,"t_EV_RETREATING"),
   s_ELN(t_EV_RETURN,"t_EV_RETURN"),
   s_ELN(t_EV_REACHED,"t_EV_REACHED"),
   s_ELN(t_EVC_STAY_AND_SHOOTING,"t_EVC_STAY_AND_SHOOTING"),
   s_ELN(t_EVC_MOVING_AND_SHOOTING,"t_EVC_MOVING_AND_SHOOTING"),
   s_ELN(t_EVC_RETREATING,"t_EVC_RETREATING"),
   s_ELN(t_EVC_RETURN,"t_EVC_RETURN"),
   s_ELN(UNIT_I_DRIVE,"UNIT_I_DRIVE"),
   s_ELN(t_EVC_CHECK_ROTATE,"t_EVC_CHECK_ROTATE"),
   s_ELN(),
};

static s_ELNTable selnTable("Default",elnTable);

//}}END_OF_EVENT_LABEL_NAMES


int  checkStaticCollision( 
                     const CFVector3 &from,    // начало движения
                     const CFVector3 &mdir,    // напрвление со скоростью
                     double           mradius, // радиус
                     double           deltaT,  // время для проверки
                     double          &clzTime  // время, через которое стукнемся
                   );

//============================================================================

CFVector3 calcNormal(CFVector3 &p, double hAngle, int isVisible,KR_ObjectID /*from*/)
{
    CFVector3 normal(0,1,0);

    if(  !isVisible  )
    {
         double    height;
         CViewScene::Current()->GetTerrain()->GetPlane(p,normal,height);
         p.y = height;
    }
    else
    {
         double      tm, tm2, tm3;
         KR_ObjectID oID,oID2;
         double cos_oy = cos(hAngle);
         double sin_oy = sin(hAngle);
         CFVector3 to1( 1*cos_oy-  1*sin_oy,  3,  1*sin_oy+  1*cos_oy),
                   to2( 1*cos_oy- -1*sin_oy,  3,  1*sin_oy+ -1*cos_oy),
                   to3(-1*cos_oy-  0*sin_oy,  3, -1*sin_oy+  0*cos_oy);  

         if(  checkStaticCollision( 
                     p+to1,    // начало движения
                     CFVector3(0,-1,0),    // напрвление со скоростью
                     1, // радиус
                     50,  // время для проверки
                     tm // время, через которое стукнемся
                   ) ) 
          {
              double height = 3-tm-1 + p.y;

              if(  checkStaticCollision( 
                         p+to2,               // начало движения
                         CFVector3(0,-1,0),    // напрвление со скоростью
                         1, // радиус
                         50,  // время для проверки
                         tm2            // время, через которое стукнемся
                       ) ) 
              {
                   double height2 = 3-tm2-1+p.y;

                   if(  checkStaticCollision( 
                         p+to3,                // начало движения
                         CFVector3(0,-1,0),    // напрвление со скоростью
                         1, // радиус
                         50,  // время для проверки
                         tm3            // время, через которое стукнемся
                       ) ) 
                   {
                   double height3 = 3-tm3-1+p.y;
                   to1.y = height;
                   to2.y = height2;
                   to3.y = height3;

                   normal = (to2-to1)%(to3-to1);
                   p.y = (height+height2+height3)*0.33333333;
                   }

              }
         }
    }

    return normal;
}

void Tank::model_drive(
                        KR_TimeDelta  timeStamp,
                        double        power,
                        double        angle
                      )
 {
    KR_TimeDelta timeDelta = timeStamp - m_prevTime;
    m_prevTime = timeStamp;

    if( timeDelta > 0.5 ) timeDelta = 0;
    //s_ASSERT1( timeDelta <= 0.5, "Tank::model_drive: Too long timeStamp %lf", timeDelta );
    s_ASSERT(power<1.3,"Whit?");

    m_needPow = power;
    m_needAng = angle;


    if(  m_desireShoot < g_levelAttr.m_maxDesire  )
         m_desireShoot += g_levelAttr.m_addDesire*timeDelta;

    const AttributeTank &model = *m_attr;
    double pow = power*model.maxPower;

    /*
     * Проверяем, что цикл не запущен
     */
    if(  m_have_UNIT_I_DRIVE > 0  )
    {
         context->removeEvent(UNIT_I_DRIVE, getObjectID());
         m_have_UNIT_I_DRIVE = 0;
    }

    double refric   =  model.refriction // Сила сопротивления в процентах
                      *model.maxPower;  

    /*
     * Изменяем мощность до указанной
     */
    if(  m_power < pow  )
    {
         m_power += model.powerDelta * timeDelta;
         if(  m_power > pow  )
              m_power = pow;
    }
    else
    if(  m_power > pow  )
    {
         m_power -= model.powerDelta * timeDelta;
         if(  m_power < pow  )
              m_power = pow;
    }

    m_speed += (m_power-refric)*massa_D*timeDelta;


    if(  m_speed > model.maxSpeed  )
         m_speed = model.maxSpeed;
    else
    if(  m_speed < 0  )
         m_speed = 0;



    if( timeDelta>=1.0 )
         m_angle = angle;
    else m_angle = interpolateAngle( 
                         m_angle, 
                         angle,
                         model.rotSpeed,
                         timeDelta
                       );

    CFVector3 np(getPosition() + CFVector3(cos(m_angle),0,sin(m_angle))
                                *(m_speed*timeDelta*0.5));
    
    CFVector3 normal;
    if(  !m_attr->m_isCannon  )
         normal = calcNormal(np, m_angle,m_isVisible,getObjectID());
    else normal = CFVector3(0,1,0);
    setPosition( np );

    {
    normal      = Normal(normal);
    double a    = acos(normal*m_normal);
    double movA = timeDelta*model.rotSpeed;
    if(   a <  movA )
         m_normal = normal;
    else
    {
         if(  movA < 1e10*a  )
         {
              double k = movA/a;
              m_normal = Normal(normal*k + m_normal*(1-k));
         }
         else m_normal = normal;
    }
    }

    if(    fabs(m_speed)       <= model.stopSpeed
        && pow                 <= model.stopPower
        && fabs(m_angle-angle) <= model.stopAngle
      )
    {
         m_speed    = 0;
         m_angle    = angle;
         m_power    = 0;
         m_have_UNIT_I_DRIVE = 0;
         return;
    }


    KR_Event event(KR_Event( UNIT_I_DRIVE,
                 timeStamp,
                 getObjectID(),
                 getObjectID() ));

    double haze = CViewFigure::HazeMax();
    event.timeStamp += MODEL_TIME_DELTA_FORWARD*(Abs(g_vehicle->getPos()-getPosition())+haze*0.2)/haze;

    //{{PUT_EVENT(UNIT_I_DRIVE)
    //}}END_OF_PUT_EVENT(UNIT_I_DRIVE)
    issueEvent( event );

    m_have_UNIT_I_DRIVE = 1;
    m_prevPos     = getPosition();
 }

void Tank::rotateCannon(
                        const KR_ObjectID &cannon, 
                        KR_Event          &event,
                        double            hAngle,
                        double            vAngle
                          )
 {
    double prevTimeStamp = event.timeStamp;
    context->removeEvent(cn_EVC_ROTATE,cannon);

    event.label       = cn_EVC_ROTATE;
    event.destination = cannon;
    //{{PUT_EVENT(cn_EVC_ROTATE)
    event.data.open(EDO_WRITE)
                   .putDouble(prevTimeStamp)
                   .putDouble(hAngle)
                   .putDouble(vAngle)
              .close();
    //}}END_OF_PUT_EVENT(cn_EVC_ROTATE)
    context->sendEventNow( event );
 }

void Tank::shoot( const KR_ObjectID &cannon, KR_Event &event, int count)
 {
    int bulletAttrIndex = m_attr->m_cacheBulletAttr;

    event.label       = cn_EV_SHOOT;
    event.destination = cannon;
    //{{PUT_EVENT(cn_EV_SHOOT)
    event.data.open(EDO_WRITE)
                   .putInt(count)
                   .putInt(bulletAttrIndex)
              .close();
    //}}END_OF_PUT_EVENT(cn_EV_SHOOT)
    issueEvent( event );
 }

void  Tank::forgetEnemy( double timeStamp )
{
    KR_Event event;

    event.timeStamp = timeStamp;
    //dc_CreateCross(0,getPosition(),60,0,0,1,1, 0,"FORGET");
    
    m_attack = false;

    if (m_group.isNUL())
	return;

    event.label       = t_EV_TARGET_LOST;
    event.source      = getObjectID();
    event.destination = m_group;
    event.data.open(EDO_WRITE)
                 .putObjectID(m_attackedEnemy)
              .close();
    issueEvent( event );
   
    m_attackedEnemy = KR_ObjectID::NUL();
    event.label       = t_EV_TARGET_LOST;
    event.source      = getObjectID();
    event.destination = getObjectID();
    issueEvent( event );
}

/*
double CalcCannonParam(Tank &tank,const CFVector3 &enemyPos,const CFVector3 &myPos,
                       double &hAngle, double &vAngle,
                       const CFVector3 &eDir)
{
    double v = tank.m_attr->m_bulletSpeed;
    double l = Abs(enemyPos-myPos);
    
    hAngle = atan2(enemyPos.z-myPos.z,enemyPos.x-myPos.x);
    vAngle = 0;

    if(  v >0.01  )
    {
         double x = hypot(enemyPos.x-myPos.x,enemyPos.z-myPos.z);
         double y = enemyPos.y - myPos.y-3.5; // ЌҐ¬­®Ј® ­Ё¦Ґ
         double g = 9.8;
         double a = g*x*x/(2*v*v);
         double b = -x;
         double c = y+g*x*x/(2*v*v);
         double D = b*b-4*a*c;
         if(  D>=0 && a>0.001  )
         {
              vAngle = atan2((-b-sqrt(D)), (2*a));
         }
         double flyT = x/cos(vAngle)*v;
         
         CFVector3 ePos = enemyPos+flyT*eDir;
         hAngle = atan2(ePos.z-myPos.z,ePos.x-myPos.x);
         
    }

//    vAngle = atan2(enemyPos.y-myPos.y,l)
//                    +asin(l*9.8/(v*v))/2;
    return l;
}

  */


void Tank::attack( const KR_ObjectID &enemyID, KR_Event &event)
 {
    CFVector3 enemyPos;
    IDynamicObject *dobj = (IDynamicObject*)
              (context->queryInterface(enemyID,IDynamicObjectIID));
    if(  dobj == 0  )
    {
         forgetEnemy(event.timeStamp);
         return;
    }

    enemyPos = dobj->getPos();

         CFVector3 myPos(getPosition());
         double prevTimeStamp = event.timeStamp;
         m_attack = true;

         for( int i = 0; i < m_cannons.getCount(); ++i )
         {

              ICannon *cn = (ICannon *)(context->queryInterface( m_cannons[i],ICannonIID ));
              if(  cn != 0  )
              {
                   if(  cn->isFixed()  )
                   {
                         if(  m_lastAttackTime + m_attr->m_attackDelay > event.timeStamp )
                         {
                              m_attack = false;
                              continue;
                         }
                         m_lastAttackTime = event.timeStamp;
                   }
                  

                   double vAngle, hAngle;
                   double l = CalcCannonParam(m_attr->m_bulletSpeed, enemyPos,myPos,hAngle, vAngle,
                                   dobj->getMoveDir()*getMoveSpeed() );

                   if(  l > 200 )//FIXME
                   {
                        forgetEnemy(event.timeStamp);
                        return;
                        
                   }

                   cn->rotateAndShoot(event.timeStamp,hAngle,vAngle,1,m_attr->m_cacheBulletAttr);
              }
         }
 }

void Tank::rotateAndShoot(
                         const KR_ObjectID &cannon, 
                         KR_Event          &event,
                         double             hAngle,
                         double             vAngle,
                         int                count
                       )
 {
    ICannon *c = (ICannon*)(context->queryInterface(cannon,ICannonIID));
    c->rotateAndShoot(event.timeStamp,hAngle,vAngle,count,m_attr->m_cacheBulletAttr);
 }

CFVector3 Tank::realPosition()
 {
    return getPosition();
 }


void Tank::render(  CViewDynamicList &list, double ts  )
 {
    CFMatrix3x4 &m = m_skin.GetDirModify();
    getMatrix(m);
    if(  m_lastDeltaT> 0.001 && m_lastDeltaT < 0.2 &&  ts>m_lastMoveTimeStamp )
    m.TranslateL(
              (ts-m_lastMoveTimeStamp)*((getPosition()-m_lastMovePos)/m_lastDeltaT)
               );
    m_viewDynObj.prepareToRender();
    list.Load( &m_viewDynObj );
 }

void Tank::endRender( CViewScene *scene )
 {
    scene->RemoveLandDynamic( &m_viewDynObj );
/*
    double up = 12;
    drawLine( getPosition()+CFVector3(0,up,0),
              getPosition()+CFVector3(0,up,0)+CFVector3(0,4,0)*m_desireShoot,
              4,  0,255,0);

    drawPoint(getPosition()+CFVector3(0,up,0),                  6,  255, 0, 0);
    drawPoint(getPosition()+CFVector3(0,up,0)+CFVector3(0,2,0), 5,  255, 0, 0);
    drawPoint(getPosition()+CFVector3(0,up+4,0),                6,  255, 0, 0);
 */   
 }

void Tank::queryCannonAngle(int can, double &vAngle, double &hAngle)
 {
    ICannon*c = (ICannon*)(context->queryInterface(m_cannons[can],ICannonIID));
    vAngle = c->getVAngle();
    hAngle = c->getHAngle();
 }

//extern CFixedColorFont	font5;

 //============================================================
void Tank::AnimateCallBack0(CViewObjectBaseSet *,CViewObjectBase *pBase,CViewObjectRef *ref)
 {
    Tank &self = *((Tank*)(ref->GetUserAttrib()));
    const CFMatrix3x4 &m = self.m_skin.GetDir();

    double vA,hA;
    self.queryCannonAngle(0,vA,hA);
    double lhang = calcLocalAngle(m,hA);

/*
    for( int i = 0; i < self.m_attr->m_cannonCnt; ++i )
    {
         KR_ObjectID cID = self.m_cannons[i];
         ICannon *c = (ICannon)(g_arena.context->queryInterface(cID,ICannonIID));
         CFVector3 ax;

         switch( i )
         {
         case 0: ax = self.m_attr->m_cannonAx0; break;
         case 1: ax = self.m_attr->m_cannonAx1; break;
         case 2: ax = self.m_attr->m_cannonAx2; break;
         }

         self.m_askin->get0(i,reduc)->
          LoadIdentity()
         .RotateOy( c->getLocalHAngle(), self.m_towerAxis)
         .Update();

    }

 */
    pBase->KFSet().Mod0("tower")
         .LoadIdentity()
         .RotateOy( lhang ,self.m_towerAxis)
         .Update();
 }

 //============================================================
void Tank::AnimateCallBack03(CViewObjectBaseSet *,CViewObjectBase *pBase,CViewObjectRef *ref)
 {
    Tank &self = *((Tank*)(ref->GetUserAttrib()));
    const CFMatrix3x4 &m = self.m_skin.GetDir();

    double vA,hA;
    self.queryCannonAngle(0,vA,hA);
    double lhang = calcLocalAngle(m,hA);
/*
    CFVector3 hvec(cos(hA),0,sin(hA));
    hvec = hvec - m.Column(1)*(m.Column(1)*hvec);
    double lhang = atan2(-(hvec*m.Column(0)), -(hvec*m.Column(2)));
*/
    pBase->KFSet().Mod0("tower")
         .LoadIdentity()
         .RotateOy( lhang ,self.m_towerAxis)
         .Update();
 }

 //============================================================
void Tank::AnimateCallBack1(CViewObjectBaseSet *,CViewObjectBase *pBase,CViewObjectRef *ref)
 {
    //(void)pBase;
    Tank &self = *((Tank*)(ref->GetUserAttrib()));

    double a = self.m_phase;
    double z0 = cos(a),
           y0 = sin(a),
           z1 = cos(a-M_PI*3/4),
           y1 = sin(a-M_PI*3/4);

    if(  y0 < 0  )
         y0 = 0;

    if(  y1 < 0  )
         y1 = 0;

    int reduc = pBase->ReductionNum();

    //pBase->KFSet().Mod0("N_FR")
    self.m_askin->get0(0,reduc)->
          LoadIdentity()
         .Translate(CFVector3(0,y0,z0))
         .Update();

    //pBase->KFSet().Mod0("N_BL")
    self.m_askin->get0(1,reduc)->
          LoadIdentity()
         .Translate(CFVector3(0,y0,z0))
         .Update();

    double 
    newA =  -atan2(6.77-3.94+y0,-2.41-0.45+z0)
            +atan2(6.77-3.94   ,-2.41-0.45);
  
    //pBase->KFSet().Mod0("P_FRU")
    self.m_askin->get0(2,reduc)->
          LoadIdentity()
         .RotateOx( newA, CFVector3(0,3.94, 0.45))
         .Update();

    //pBase->KFSet().Mod0("P_FRD")
    self.m_askin->get0(3,reduc)->
          LoadIdentity()
         .RotateOx( newA, CFVector3(0,2.34,0.45))
         .Update();

    newA =  -atan2(6.77-3.94+y0,5.8 -2.95+z0)
            +atan2(6.77-3.94   ,5.8 -2.95);

    //pBase->KFSet().Mod0("P_BLU")
    self.m_askin->get0(4,reduc)->
          LoadIdentity()
         .RotateOx( newA, CFVector3(0,3.99,2.95))
         .Update();

    //pBase->KFSet().Mod0("P_BLD")
    self.m_askin->get0(5,reduc)->
          LoadIdentity()
         .RotateOx( newA, CFVector3(0,2.36,2.95))
         .Update();



 //----------------------------------------------------------------------
    //pBase->KFSet().Mod0("N_FL")
    self.m_askin->get0(6,reduc)->
          LoadIdentity()
         .Translate(CFVector3(0,y1,z1))
         .Update();

    //pBase->KFSet().Mod0("N_BR")
    self.m_askin->get0(7,reduc)->
          LoadIdentity()
         .Translate(CFVector3(0,y1,z1))
         .Update();

    newA =  -atan2(6.77-3.94+y1,-2.41- -0.41+z1)
            +atan2(6.77-3.94   ,-2.41- -0.41);

    //pBase->KFSet().Mod0("P_FLU")
    self.m_askin->get0(8,reduc)->
          LoadIdentity()
         .RotateOx( newA, CFVector3(0,3.94,-0.41))
         .Update();

    //pBase->KFSet().Mod0("P_FLD")
    self.m_askin->get0(9,reduc)->
          LoadIdentity()
         .RotateOx( newA, CFVector3(0,2.36,-0.41))
         .Update();

    newA =  -atan2(6.77-3.94+y1,5.80-2.95+z1)
            +atan2(6.77-3.94   ,5.80-2.95);

    //pBase->KFSet().Mod0("P_BRU")
    self.m_askin->get0(10,reduc)->
          LoadIdentity()
         .RotateOx( newA, CFVector3(0,3.94,2.95))
         .Update();

    //pBase->KFSet().Mod0("P_BRD")
    self.m_askin->get0(11,reduc)->
          LoadIdentity()
         .RotateOx( newA, CFVector3(0,2.41,2.95))
         .Update();
 }

