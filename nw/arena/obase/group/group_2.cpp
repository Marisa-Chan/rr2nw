/*
 * File  : D:\GAME\OBASE\GROUP\group_0.cpp
 * Autor :
 * Ver   1.0
 */
#include "draw.h"
#include "group.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/groupmsg.h"
#include "message/unitmsg.h"
#include "message/dcrossmsg.h"
#include "i/dynobj.i"
#include "i/unit.i"

#ifdef  __TRACE_NW__
#include "afxwin.h"
#else
class CDC{};
#endif


//{{EVENT_LABEL_NAMES
static s_ELN elnTable[]=
{
   s_ELN(tg_EVCMD_GO_TO,"tg_EVCMD_GO_TO"),
   s_ELN(tg_EVC_FIND_ENEMY,"tg_EVC_FIND_ENEMY"),
   s_ELN(tg_EVC_MOVING,"tg_EVC_MOVING"),
   s_ELN(tg_EV_TARGET_LOCKED,"tg_EV_TARGET_LOCKED"),
   s_ELN(tg_EV_TARGET_LOST,"tg_EV_TARGET_LOST"),
   s_ELN(tg_EV_REACHED,"tg_EV_REACHED"),
   s_ELN(tg_EV_YOU_ATTACKED,"tg_EV_YOU_ATTACKED"),
   s_ELN(tg_EV_TARGET_DESTROED,"tg_EV_TARGET_DESTROED"),
   s_ELN(tg_EV_REBUILD,"tg_EV_REBUILD"),
   s_ELN(),
};

static s_ELNTable selnTable("Default",elnTable);

//}}END_OF_EVENT_LABEL_NAMES

CFVector3 TankGroup::realPosition()
{
    return m_position;
}

 //===========================================================================
double TankGroup::queryDamage() // FIXME Передавать время ?
 {
    double             d = 0;

    for( int i = 0; i < m_members.getCount(); ++i )
    {
         IUnit *unit = (IUnit*)(context->queryInterface(m_members[i],IUnitIID));
         if(  unit==0  )
         {
              m_members.del(i);
              i--;
              continue;
         }
         d += unit->getDamage();
    }

    if(  m_members.getCount()==0  )
         context->removeObject(getObjectID());
    return d;
 }

 /****************************
  *
  *
  *        MOVING
  *
  *
  ****************************/

 //===========================================================================
void TankGroup:: gotoMOVING( KR_Event &event )
 {
    context->removeEvent(tg_EVC_MOVING      ,getObjectID());

    double prevTimeStamp = event.timeStamp;
    
    event.source      = getObjectID();
    event.destination = getObjectID();
    event.label       = tg_EVC_MOVING;
    //{{PUT_EVENT(tg_EVC_MOVING)
    event.data.open(EDO_WRITE)
                   .putDouble(prevTimeStamp)
              .close();
    //}}END_OF_PUT_EVENT(tg_EVC_MOVING)
    issueEvent( event );
 }

 //===========================================================================
void     gotoSTAY( KR_Event & )
 {
 }

 //===========================================================================
double TankGroup::calcDamageK( double deltaT )
 {
    double damage = queryDamage();
    
    if(  m_members.getCount() > 0  )
         damage /= m_members.getCount() * deltaT;

    return damage;
 }

 //===========================================================================
bool TankGroup::moveToPosition(
                               double           prevTimeStamp,
                               double           curTimeStamp,
                               const CFVector2 &destPos
                              )
 {
    s_ENTRY(TankGroup::moveToPosition)

    CFVector2 groupCenter(m_position.x,m_position.z);
    double    dist = Abs(destPos - groupCenter);
    KR_Event  event; 

    if(  dist <= m_attr->m_reachedDelta  )
    {
         event.label       = tg_EV_REACHED;
         event.source      = getObjectID();
         event.destination = getObjectID();
         event.timeStamp   = curTimeStamp;
         //{{PUT_EVENT(tg_EV_REACHED)
         //}}END_OF_PUT_EVENT(tg_EV_REACHED)
         issueEvent( event );

         return false;
    }

    /*
     * Wait letting members
     */
    {
    SimulationContext *context = getContext();
    KR_Event           ev;
    CFVector2          memCoord;
    bool               wait = false;

    ev.timeStamp = curTimeStamp;
    for( int i = 0; i < m_members.getCount(); ++i )
    {
         const KR_ObjectID curMember = m_members[i];
         IDynamicObject *dobj = (IDynamicObject *)(context->queryInterface(curMember,IDynamicObjectIID));

         if(  dobj==0  )
         {
              m_members.del(i);
              i--;
              continue;
         }


         CFVector3 p(dobj->getPos());
         memCoord.x = p.x;
         memCoord.y = p.z;

         
         double dist = Abs(memCoord-groupCenter);
         if(  dist > m_attr->m_waitDist  )
              wait = true;
         
         if(  dist > m_attr->m_nearDist  )
         {
              /*
               * Если член группы далеко от нас, то подгоняем его
               */
              //dc_CreateCross(0,p,10,0,0,1,1,0,".....GO");
              double moveSpeed   = 1,//(dist - m_attr->m_nearDist)
                                  ///(m_attr->m_waitDist - m_attr->m_nearDist),
                     xVec        = m_position.x-memCoord.x,
                     yVec        = m_position.z-memCoord.y,
                     maxMoveDist = dist;

              event.source      = getObjectID();
              event.destination = curMember;
              event.label       = t_EV_MOVE;
              event.timeStamp   = curTimeStamp;
              //{{PUT_EVENT(t_EV_MOVE)
              event.data.open(EDO_WRITE)
                             .putDouble(moveSpeed)
                             .descend( VECTOR_LAND, 0 )
                               .putDouble(xVec)
                               .putDouble(yVec)
                             .ascend()
                             .putDouble(maxMoveDist)
                        .close();
              //}}END_OF_PUT_EVENT(t_EV_MOVE)
              issueEvent( event );
         }

    }

    if(  m_members.getCount()==0  )
    {
         context->removeObject(getObjectID());
         return false;
    }


    if(  !wait  )
    {
         CFVector3   dir(d_mov.m_crd - m_position);
         CFVector3   addVec(Normal(dir)
                      *m_attr->m_speed
                      *(curTimeStamp - prevTimeStamp));
         double len0 = Abs(addVec),
                len1 = Abs(dir);

         if(  len0 > len1  )
              addVec *= len1/(len0+0.1);

         m_position += addVec;
         //dc_CreateCross(0,m_position,10,0,0,1,1);
    }
    else
    {
         /*
          * Просим подождать
          */
         for( int i = 0; i < m_members.getCount(); ++i )
         {
              const KR_ObjectID curMember = m_members[i];
              IDynamicObject *dobj = (IDynamicObject *)(context->queryInterface(curMember,IDynamicObjectIID));

              if(  dobj==0  )
              {
                   m_members.del(i);
                   i--;
                   continue;
              }


              CFVector3 p(dobj->getPos());
              memCoord.x = p.x;
              memCoord.y = p.z;

         
              double dist = Abs(memCoord-groupCenter);
              if(  dist > m_attr->m_waitDist  )
                   wait = true;
         
              if(  dist <= m_attr->m_nearDist  )
              {
                   /*
                    * Если член группы Близко просим подождать
                    */
                   //dc_CreateCross(0,p,10,0,0,1,1,0,"WAIT");
                   double moveSpeed   = 0.0,
                          xVec        = m_position.x-memCoord.x,
                          yVec        = m_position.z-memCoord.y,
                          maxMoveDist = dist;

                   event.source      = getObjectID();
                   event.destination = curMember;
                   event.label       = t_EV_MOVE;
                   event.timeStamp   = curTimeStamp;
                   //{{PUT_EVENT(t_EV_MOVE)
                   event.data.open(EDO_WRITE)
                                  .putDouble(moveSpeed)
                                  .descend( VECTOR_LAND, 0 )
                                    .putDouble(xVec)
                                    .putDouble(yVec)
                                  .ascend()
                                  .putDouble(maxMoveDist)
                             .close();
                   //}}END_OF_PUT_EVENT(t_EV_MOVE)
                   context->sendEventNow( event );
              }

         }

         if(  m_members.getCount()==0  )
         {
              context->removeObject(getObjectID());
              return false;
         }

    }

    }

    return true;
 }

 //===========================================================================
void TankGroup::forgetLostTargets( KR_SetOfID &soid )
 {
    SimulationContext *context = getContext();
    KR_Event           ev;
    CFVector2          centerGroup(m_position.x,m_position.z);
    CFVector2          crd;

    for( int i = soid.getCount()-1; i>=0; --i )
    {
         IDynamicObject *dobj =(IDynamicObject *)(context->queryInterface(soid[i],IDynamicObjectIID));
         if(  dobj == 0   )
              soid.del(i);
         else
         {
         CFVector3 pos(dobj->getPos());

         if(  Abs(CFVector2(pos.x,pos.z)-centerGroup) > m_attr->m_maxViewDist  )
                soid.del(i);
         }
    }
 }


 //===========================================================================
KR_ObjectID TankGroup::findEnemy( double timeStamp )
 {
    KR_Event event;
    event.timeStamp = timeStamp;
 //dc_CreateCross(0,m_position,40,0,0,1,1,0,"FI");
    int i;
    SimulationContext *context = getContext();
    double xMin=1e10, 
           zMin=1e10, 
           xMax=-1e10, 
           zMax=-1e10;
    /*
     *  calc members area
     */
    event.source      = getObjectID();

    for( i = 0; i < m_members.getCount(); ++i )
    {
         IDynamicObject *dobj =  (IDynamicObject *)
               (context->queryInterface(m_members[i],IDynamicObjectIID));
         if(  dobj==0  )
         {
              m_members.del(i);
              --i;
              continue;
         }
         CFVector3 v(dobj->getPos());
         if(  v.x < xMin  ) xMin = v.x;
         if(  v.x > xMax  ) xMax = v.x;
         if(  v.z < zMin  ) zMin = v.z;
         if(  v.z > zMax  ) zMax = v.z;
    }

    if(  m_members.getCount() <=0  )
    {
         context->removeObject(getObjectID());
         return KR_ObjectID::NUL();
    }

    xMin -= m_attr->m_findEnemyDist;
    zMin -= m_attr->m_findEnemyDist;
    xMax += m_attr->m_findEnemyDist;
    zMax += m_attr->m_findEnemyDist;

    /*
     *  find enemy
     */

    ct_SubjectFindData fsd;
    bool               okFind = false;
    double             bestAlienK = 0;
    int                bestAlien  = -1;
    CFVector2          groupCenter( m_position.x,m_position.z );


    ct_Arena::findFirstSubject( fsd, xMin, zMin, xMax, zMax );


    for( i = 0; i < fsd.getCount(); ++i )
    {
         const KR_ObjectID  &enemyID = fsd[i];
         IUnit *u =(IUnit *)
                   (context->queryInterface( enemyID, IUnitIID ));
         if(  u==0  )
              continue;

         if(  !u->isFriend(m_commander)  )
              if(  m_lockedTarget.find( enemyID ) < 0  )
              {
                   double     power = u->getPower();
                   IDynamicObject *dobj = (IDynamicObject *)
                      (context->queryInterface( enemyID, IDynamicObjectIID ));

                   if(  dobj != 0  )
                   {
                        CFVector3 pos(dobj->getPos());
                        double curK = power/(1+Abs(groupCenter-CFVector2(pos.x,pos.z)));
                        if(  curK > bestAlienK  )
                        {
                             bestAlien  = i;
                             bestAlienK = curK;
                             okFind     = true;
                        }
                   }
              }
    }

 
    if(  okFind  )
    {
         const KR_ObjectID &enemyID = fsd[bestAlien]; 
         //dc_CreateCross(0,m_position,40,0,0,1,1,0,"FIND ENEMY");

         return enemyID;
    }
    return KR_ObjectID::NUL();
 }


