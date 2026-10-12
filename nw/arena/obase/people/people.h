/*
 * File  : C:\WinGame\OBASE\People\People.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __People_H__INCLUDED
#define __People_H__INCLUDED

#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "..\DynObj\DynObj.h"
#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"
#include "i/dynobj.i"
#include "i/skin.i"
#include "i/unit.i"

enum
{
  KILL_NONE,
  KILL_FALLOW_1,
  KILL_FALLOW_2,
  KILL_WALK_1,
  KILL_WALK_2,
  KILL_SINK
};

enum
{
  pe_STATE_DEFAULT,
  pe_STATE_ATTACK,
  pe_STATE_BACK
};

class AttributePeople;


#pragma warning 004 10

typedef struct {
	
    KR_ObjectID		m_peopleAttrID;
    KR_ObjectID     m_routeID;
    int             m_curNode;
    double          m_damage;
    KR_ObjectID            m_commanderID;

    CFVector3              m_dir;
    CFVector3              m_nextNode;
    double                 m_prevTime;
    double                 m_rotateOx, 
                           m_rotateOy,
                           m_rotateOz;
    double                 m_calcPosInc;
    double                 m_hAngle;
    double                 m_prevShootTime;
    double                 m_correctScale;

    KR_ObjectID            m_snd;
    int                    m_killed;

    double                 m_kill_startPhase,
                           m_kill_deltaVAngle,
                           m_kill_time;
    int                    m_deleted;
    double                 m_lastDamageTime;
    CFVector3              m_returnPoint;
    int                    m_isNotCreate;
    CFVector3              m_minPos, m_maxPos;

    enum { MAX_STATE = 4 };

    int                    m_stateSP;
    int                    m_state[MAX_STATE];
    KR_ObjectID            m_enemyID[MAX_STATE];

    CFVector3              m_lastMovePos;
    double                 m_lastMoveDeltaT;
    int                    m_prevStartShoot;

    int                    m_stoped;
    int                    m_isClz;
} PeopleData;

class People : public ct_Subject,
               public IDynamicObject,
               public IUnit,
               public PeopleData
{
	void setPeopleAttr();
 public:
    AttributePeople        *m_attr;    
    CViewObjectRef         m_skin;
    s_ViewDynamicObject    m_viewDynObj;
    ISkin                 *m_askin;
        
    
    int                    getState()   const { return m_state[m_stateSP-1]; }
    KR_ObjectID            getEnemyID() const { return m_enemyID[m_stateSP-1]; }
    void                   pushState( int state, KR_ObjectID enemyID );
    void                   initState() { m_stateSP = 0; pushState(pe_STATE_DEFAULT,KR_ObjectID::NUL()); }
    void                   popState () { m_stateSP -= m_stateSP>1; }


    void     onKilled( const CFVector3 &explPos, double ts );
    void     onShoot(double ts);
    bool     isShooter();
	
             People();
    virtual ~People();
    virtual int  receiveEvent( KR_Event &event );
    virtual void draw        ();
    virtual void addNotify   ();
    virtual void removeNotify();

    virtual void render   ( CViewDynamicList &list, double ts );
    virtual void endRender( CViewScene *scene );
    virtual CFVector3     realPosition();

    virtual void        onEnterAudibleZone(double ts);
    virtual void        onExitAudibleZone (double ts);

    void nexNodeDefault(KR_Event &event);
    void manoeuvre(KR_Event &event, 
                    const CFVector3 &pos,
                    const CFVector3 &mpos
                   );
    void attackAllways(KR_Event &event, const CFVector3 &pos);
    void moveKilled( KR_Event &event, double deltaT );
    void nexNodeAttackFar(KR_Event &event, 
                              const CFVector3 &pos,
                              const CFVector3 &mpos,
                              IDynamicObject *dobj
                             );
    void nexNodeAttackNear(KR_Event &event, 
                              const CFVector3 &pos,
                              const CFVector3 &mpos,
                              IDynamicObject *dobj
                             );

    virtual void      *queryInterface( int interNum );
    // ----- interface IDynamicObject
    virtual CFVector3  getPos      ();
    virtual double     getHAngle   ();
    virtual CFVector3  getUpVector ();
    virtual CFVector3  getCenter   (); // Относительно 0 объекта
    virtual double     getRadius   (); // Относительно центра
    virtual double     getRadius0  (); // Относительно 0 объекта
    virtual CFVector3  getMoveDir  (); // Направление движения
    virtual double     getMoveSpeed(); // Скорость
    virtual void       getMatrix   ( CFMatrix3x4 &m );

    // ----- interface IUnit
    virtual double getPower   (); // Сила юнита 0..10
    virtual int    isFriend   ( const KR_ObjectID &commanderID );
    virtual double getDamage  (); // Целостность от 0..1
    virtual void   setDamage  ( double d, const CFVector3 &pos, double ts, KR_ObjectID from );
    virtual double desireShoot();
    virtual KR_ObjectID getCommander();
    virtual void   setCommander(KR_ObjectID oID);

    virtual double 	getMass	   () {return 1;}	// Масса
    virtual TCCFMatrix3x4 &GetDir  () {return m_skin.GetDir(); }
    virtual void SetDir(TCSFMatrix3x4 &dir) { m_skin.GetDirModify() = dir; }
    
    virtual bool  shouldDump () { return true; } 	
    virtual bool  dump(PIN_SaveFile & sf);
    virtual bool  load(PIN_SaveFile & );
    virtual void  loadNotify();

    void crashExpl( CFVector3 pos, double time );
    void crashCreateCorpse( CFVector3 pos, double time );
    void moveKilled_FALLOW_checkClz( KR_Event &event );
    void moveKilled_FALLOW_1( KR_Event &event, double deltaT );
    void moveKilled_FALLOW_2( KR_Event &event, double deltaT );
    void moveKilled_FALLOW_SINK( KR_Event &event, double deltaT );

    void setPosition(  const CFVector3 &vec  );
    void setHAngle( double hAngle );
    void setHAngleFixDir( double hAngle );
};


#pragma warning 004 1

#endif // ifndef __People_H__INCLUDED
/* End of file C:\WinGame\OBASE\People\People.h */