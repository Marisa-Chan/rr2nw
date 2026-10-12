/*
 * File  : D:\GAME\OBASE\GROUP\group.h
 * Autor :
 * Ver   1.0
 */
#ifndef __GROUP_H__INCLUDED
#define __GROUP_H__INCLUDED

#include "kernel/h/active.h"
#include "kernel/h/setofid.h"
#include "kernel/h/context.h"
#include "message/funitmsg.h"
#include "enum/spaceEnum.h"
#include "storage/h/subject.h"
#include "storage/h/attr.h"


#define      GROUP_CHECK_DAMAGE_TIME_DELTA   (10.0)
#define      GROUP_FIND_TARGET_TIME_DELTA    (3.2)
#define      GROUP_MOVING_TIME_DELTA         (0.4)
#define      GROUP_REACHED_DELTA             (3.0)


 //===========================================================================
class AttributeTankGroup : public ct_Attribute
{
 public:
//{{ATTRIBUTE
    ct_AttrItem  m_array[8];
    double          m_bravery                ;  // смелость
    double          m_nearDist               ;  // –ассто€ние от центра группы, которое считаетс€ нулевым
    double          m_waitDist               ;  // –ассто€ние, при превышении которого ждем отставших
    double          m_speed                  ;  // m/s —корость движени€
    double          m_reachedDelta           ;  // –адиус, в пределах которого считаетс€, что точка достигнута
    double          m_maxViewDist            ;  // ћаксимальна€ дальность, при которой враг обнаруживаетс€
    double          m_findEnemyDist          ;  // радиус, в пределах которого ищем врагов
    double          m_movingAttackingTimeIncrement;  // 

    AttributeTankGroup()
    {
        m_bravery            = (4./15)*0.3;
        m_nearDist           = 10.0;
        m_waitDist           = 30.0;
        m_speed              = 10.0;
        m_reachedDelta       = 1.0;
        m_maxViewDist        = 300.0;
        m_findEnemyDist      = 200.0;
        m_movingAttackingTimeIncrement = 2.0;

        m_array[0].set("m_bravery",m_bravery);
        m_array[1].set("m_nearDist",m_nearDist);
        m_array[2].set("m_waitDist",m_waitDist);
        m_array[3].set("m_speed",m_speed);
        m_array[4].set("m_reachedDelta",m_reachedDelta);
        m_array[5].set("m_maxViewDist",m_maxViewDist);
        m_array[6].set("m_findEnemyDist",m_findEnemyDist);
        m_array[7].set("m_movingAttackingTimeIncrement",m_movingAttackingTimeIncrement);

        linkTable(m_array,8);
    }
//}}END_OF_ATTRIBUTE
};


 //===========================================================================
class TankGroup :	public ct_Subject, 
					public KR_ActiveObject
{
 public:

    AttributeTankGroup  *m_attr;
    KR_SetOfID           m_lockedTarget;
    KR_SetOfID           m_members;
    CFVector3            m_position;
    KR_ObjectID          m_commander;
	KR_ObjectID          m_attrID;

    struct MovingData 
    {
      CFVector3    m_crd;
      KR_ObjectID  m_targetID;
      CFVector3    m_pointDeparted;

      MovingData() 
           : m_targetID(-1,-1),
             m_crd(0,0,0),
             m_pointDeparted(0,0,0)
      {
      }
    } d_mov; //===========================================================


    void startInitialize()
    {
       m_position    = CFVector3(0,0,0);
       m_commander   = KR_ObjectID::NUL();       
    }

    int      findVarior(const CFVector2 &enemyCoord,double &minDist);
    void     attackEnemy(KR_ObjectID enemyID, double ts);

    double   queryDamage();

    double   calcDamageK( double deltaT );
    bool     moveToPosition(   // return true if repeat
                               double           prevTimeStamp,
                               double           curTimeStamp,
                               const CFVector2 &destPos
                           );
    void     forgetLostTargets( KR_SetOfID &soid );
    KR_ObjectID findEnemy( double timeStamp );

    void     gotoMOVING              ( KR_Event &event );
    void     gotoSTAY                ( KR_Event &event );

             TankGroup();
    virtual ~TankGroup();
    virtual int  receiveEvent( KR_Event &event );
    virtual void addNotify   ();
    virtual void removeNotify();
	virtual void forcedRemoveNotify();
    virtual CFVector3     realPosition();
    void    unitTargetLost( KR_Event &event );

    void findEnemyFC   ( KR_Event &event );
    //{{ACTIONF_DECLARE
    void gotoF        ( KR_Event &event );
    void move_ReachedF( KR_Event &event );
    void movingFC     ( KR_Event &event );
    //}}END_OF_ACTIONF_DECLARE

    void         loadStateTransitionTable();

	virtual bool	shouldDump () { return true; } 

	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
	virtual void	loadNotify();
};

#endif // ifndef __GROUP_H__INCLUDED
/* End of file D:\GAME\OBASE\GROUP\group.h */