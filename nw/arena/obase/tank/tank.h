/*
 * File   : D:\GAME\OBASE\Tank\tank.h
 * Author :
 * Ver   1.0 

 Проверить на очистку всех множеств
 */
#ifndef __TANK_H__INCLUDED
#define __TANK_H__INCLUDED

#include "kernel/h/setofid.h"
#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"
#include "kernel/h/context.h"
#include "message/funitmsg.h"
#include "enum/spaceenum.h"
#include "obase/sound/wavobj.h"
#include "h/olevel.h"

#include "i/dynobj.i"
#include "i/unit.i"
#include "i/skin.i"
#include "i/cannon.i"
#include "i/carrier.i"

#define LAST_H__VIEW
#include "game.h"
#include "..\DynObj\DynObj.h"

#include "nearAI.h" // Green

#define MODEL_TIME_DELTA_FORWARD (0.05)
#define MAX_CANNON 3
#define SUB_DESIRE 0.5
#define ADD_DESIRE 0.03

#pragma warning 004 10

class AttributeTank : public ct_Attribute
{
 public:
    virtual void wakeUp( KR_Event &event );
    virtual void update(double ts);
    KR_ObjectID  m_skinID;
    int          m_cannonIndex[MAX_CANNON];

    WAVObj            * m_wav;  
    ct_ClassTableID     m_ctsndID;

    ct_ClassTableID m_smokeTableID;
    KR_ObjectID     m_smokeAttrID;
    int             m_cacheExplAttr;

    ct_ClassTableID  m_cacheCorpseTable;
    int	             m_cacheCorpseAttr;
    ct_ClassTableID  m_cacheExplosionTable;


    double          powerDelta;
//{{ATTRIBUTE
    ct_AttrItem  m_array[51];
    int             m_type                   ;  // Тип танка
    double          m_power                  ;  // Мощность по 10-бальной шкале
    double          m_armor                  ;  // 0..1 - бронированность
    double          m_radius                 ;  // Приблизительный радиус объекта с запасом
    double          m_viewDist               ;  // Расстояние, на которое види объект
    double          massa                    ;  // Масса объекта
    double          maxPower                 ;  // Максимальная развиваемая тяга
    double          refriction               ;  // сила трения  %
    double          maxSpeed                 ;  // максимальная скорость м/с
    double          minSpeed                 ;  // Скорость, добавляемая при расчете сопротивления
    double          rotSpeed                 ;  // 
    double          stopAngle                ;  // Угол останова поворота
    double          maxRotSpeed              ;  // максимальная скорость вращения
    double          stopRadius2              ;  // квадрат радиуса начала торможения
    double          stopRadius               ;  // квадрат радиуса от точки останова
    double          deaxelerate              ;  // коэффициент сброса скорости при торможении
    double          rotRefrict               ;  // коэффициент трения для поворота
    double          stopSpeed                ;  // скорость, при достижении которой остановиться
    double          stopPower                ;  // 
    int             m_maxDistScale           ;  // Кратность, с которой проверяется, что мы не ушли слишком далеко
    ct_AttrStr      m_cannonTable            ;  // 
    int             m_cacheCannonTable       ;  // 
    ct_AttrStr      m_bulletAttrTable        ;  // Имя таблицы атрибутов для пули
    int             m_cacheBulletAttrTable   ;  // 
    ct_AttrStr      m_bulletAttr             ;  // Имя объекта - аттрибута для пули
    int             m_cacheBulletAttr        ;  // 
    double          m_movingAndShootingIncrement;  // 
    double          m_bulletSpeed            ;  // 
    ct_AttrStr      m_skin                   ;  // 
    CViewObjectModel* m_cacheSkin              ;  // 
    double          m_blobCreateTime         ;  // 
    ct_AttrStr      m_smokeTableName         ;  // 
    ct_AttrStr      m_smokeAttrName          ;  // 
    double          m_mvTimeInc              ;  // Time between checking moving direction
    double          m_maxViewDist            ;  // Tank view distanse
    double          m_turnAngleTreshold      ;  // Constraint on turning
    double          m_obstH                  ;  // Max ovecomable obstacle height (not used yet)
    double          m_staticTresh            ;  // Threshold of static objects
    double          m_dynamicTresh           ;  // Threshold of dynamic objects
    double          m_checkRotateTime        ;  // 
    double          m_checkRotateDelta       ;  // 
    ct_AttrStr      m_soundName              ;  // 
    double          m_attackDelay            ;  // 
    int             m_cannonCnt              ;  // 
    ct_AttrStr      m_cannon0                ;  // 
    ct_AttrStr      m_cannon1                ;  // 
    ct_AttrStr      m_cannon2                ;  // 
    double          powerDeltaTime           ;  // 
    ct_AttrStr      m_corpseAttrName         ;  // 
    ct_AttrStr      m_explAttrName           ;  // 
    int             m_isCannon               ;  // 

    AttributeTank()
    {
        m_type               = 0;
        m_power              = 5.0;
        m_armor              = 0.5;
        m_radius             = 5.0;
        m_viewDist           = 100;
        massa                = 1000.0;
        maxPower             = 8000000.0;
        refriction           = 0.5;
        maxSpeed             = 10.0;
        minSpeed             = 0.2;
        rotSpeed             = 0.5;
        stopAngle            = 0.01;
        maxRotSpeed          = 3.0;
        stopRadius2          = 25.0;
        stopRadius           = 4.0;
        deaxelerate          = 0.2;
        rotRefrict           = 0.45;
        stopSpeed            = maxSpeed*0.1;
        stopPower            = maxPower*0.01;
        m_maxDistScale       = 4;
        strncpy(m_cannonTable,"Cannon", sizeof( ct_AttrStr )-1 );
        m_cacheCannonTable   = -1;
        strncpy(m_bulletAttrTable,"BulletAttr", sizeof( ct_AttrStr )-1 );
        m_cacheBulletAttrTable = -1;
        strncpy(m_bulletAttr,"Bullet.Led", sizeof( ct_AttrStr )-1 );
        m_cacheBulletAttr    = -1;
        m_movingAndShootingIncrement = 0.8;
        m_bulletSpeed        = 60.0;
        strncpy(m_skin,"sk.Tank.0", sizeof( ct_AttrStr )-1 );
        m_cacheSkin          = NULL;
        m_blobCreateTime     = 1.0;
        strncpy(m_smokeTableName,"Smoke", sizeof( ct_AttrStr )-1 );
        strncpy(m_smokeAttrName,"Smoke.Attr.Small", sizeof( ct_AttrStr )-1 );
        m_mvTimeInc          = 2.0;
        m_maxViewDist        = 30.0;
        m_turnAngleTreshold  = 0.3;
        m_obstH              = 0.2;
        m_staticTresh        = 0.5;
        m_dynamicTresh       = 0.5;
        m_checkRotateTime    = 0.02;
        m_checkRotateDelta   = 3.14/90;
        strncpy(m_soundName,"wav.Tank2", sizeof( ct_AttrStr )-1 );
        m_attackDelay        = 5;
        m_cannonCnt          = 1;
        strncpy(m_cannon0,"Cannon.Ch", sizeof( ct_AttrStr )-1 );
        strncpy(m_cannon1,"Cannon.Ht", sizeof( ct_AttrStr )-1 );
        strncpy(m_cannon2,"Cannon.Ht", sizeof( ct_AttrStr )-1 );
        powerDeltaTime       = 2;
        strncpy(m_corpseAttrName,"Corpse.Attr.Default", sizeof( ct_AttrStr )-1 );
        strncpy(m_explAttrName,"Expl.Attr.Big", sizeof( ct_AttrStr )-1 );
        m_isCannon           = 0;

        m_array[0].set("m_type",m_type);
        m_array[1].set("m_power",m_power);
        m_array[2].set("m_armor",m_armor);
        m_array[3].set("m_radius",m_radius);
        m_array[4].set("m_viewDist",m_viewDist);
        m_array[5].set("massa",massa);
        m_array[6].set("maxPower",maxPower);
        m_array[7].set("refriction",refriction);
        m_array[8].set("maxSpeed",maxSpeed);
        m_array[9].set("minSpeed",minSpeed);
        m_array[10].set("rotSpeed",rotSpeed);
        m_array[11].set("stopAngle",stopAngle);
        m_array[12].set("maxRotSpeed",maxRotSpeed);
        m_array[13].set("stopRadius2",stopRadius2);
        m_array[14].set("stopRadius",stopRadius);
        m_array[15].set("deaxelerate",deaxelerate);
        m_array[16].set("rotRefrict",rotRefrict);
        m_array[17].set("stopSpeed",stopSpeed);
        m_array[18].set("stopPower",stopPower);
        m_array[19].set("m_maxDistScale",m_maxDistScale);
        m_array[20].set("m_cannonTable",m_cannonTable);
        m_array[21].set("m_cacheCannonTable",m_cacheCannonTable);
        m_array[22].set("m_bulletAttrTable",m_bulletAttrTable);
        m_array[23].set("m_cacheBulletAttrTable",m_cacheBulletAttrTable);
        m_array[24].set("m_bulletAttr",m_bulletAttr);
        m_array[25].set("m_cacheBulletAttr",m_cacheBulletAttr);
        m_array[26].set("m_movingAndShootingIncrement",m_movingAndShootingIncrement);
        m_array[27].set("m_bulletSpeed",m_bulletSpeed);
        m_array[28].set("m_skin",m_skin);
        m_array[29].set("m_cacheSkin",EDI_NONE,&m_cacheSkin);
        m_array[30].set("m_blobCreateTime",m_blobCreateTime);
        m_array[31].set("m_smokeTableName",m_smokeTableName);
        m_array[32].set("m_smokeAttrName",m_smokeAttrName);
        m_array[33].set("m_mvTimeInc",m_mvTimeInc);
        m_array[34].set("m_maxViewDist",m_maxViewDist);
        m_array[35].set("m_turnAngleTreshold",m_turnAngleTreshold);
        m_array[36].set("m_obstH",m_obstH);
        m_array[37].set("m_staticTresh",m_staticTresh);
        m_array[38].set("m_dynamicTresh",m_dynamicTresh);
        m_array[39].set("m_checkRotateTime",m_checkRotateTime);
        m_array[40].set("m_checkRotateDelta",m_checkRotateDelta);
        m_array[41].set("m_soundName",m_soundName);
        m_array[42].set("m_attackDelay",m_attackDelay);
        m_array[43].set("m_cannonCnt",m_cannonCnt);
        m_array[44].set("m_cannon0",m_cannon0);
        m_array[45].set("m_cannon1",m_cannon1);
        m_array[46].set("m_cannon2",m_cannon2);
        m_array[47].set("powerDeltaTime",powerDeltaTime);
        m_array[48].set("m_corpseAttrName",m_corpseAttrName);
        m_array[49].set("m_explAttrName",m_explAttrName);
        m_array[50].set("m_isCannon",m_isCannon);

        linkTable(m_array,51);
    }
//}}END_OF_ATTRIBUTE
};



typedef struct {
	KR_ObjectID	m_tankAttrID;
// Green BEGIN
	int			m_dirCnt;
	CFVector2	m_dir[MAX_DIRECTIONS+1];
	double		m_dirAngle[MAX_DIRECTIONS+1];
	double		m_maxViewDist;
	CFVector2	m_destDir;
	CFVector2	m_prevDir;
// Green END

    // model data
    int       m_stopMode;
    double    m_needPow,m_needAng;

    double		m_phase;
    int			m_runSmoke;
    double		m_lastAttackTime;
    KR_ObjectID m_group;

	int                    m_maxDistScale;
    KR_ObjectID            m_snd;

    CFVector3              m_towerAxis;

    CFVector3    m_destPos;
    KR_TimeDelta m_prevTime;
    int          m_have_UNIT_I_DRIVE;

    CFVector3    m_normal;
    double
                 m_speed,
                 m_power,
                 m_angle,

                 m_damage;
    double       m_desireShoot;

	/*
     * Move to
     */
    double       m_maxMoveDist;
    CFVector3    m_prevPos;

    KR_ObjectID  m_attackedEnemy;
    KR_ObjectID  m_commander;

    double       massa_D,
                 maxPower_D;
    bool         m_attack;
    int          m_checkRotateCount;


    double       save_hAngle, save_vAngle;
    int          save_count,  save_attr;
    KR_ObjectID  save_cannonID;
    CFVector3    m_lastMovePos;
    double       m_lastDeltaT;

} TankData;


class Tank : public ct_Subject, 
             public KR_ActiveObject, 
             public IDynamicObject,
             public IUnit,
             public ICarrier,
			 public TankData
{

 void setTankAttr();
 public:

	KR_SetOfID			   m_cannons;
    ISkin			      *m_askin;
    AttributeTank		  *m_attr;
    CViewObjectRef         m_skin;
    s_ViewDynamicObject    m_viewDynObj;

    
    
    void     startInitialize()
    {
         m_lastAttackTime = -1e10;
         m_have_UNIT_I_DRIVE = 0;
         m_prevTime = 0;
         m_destPos  = CFVector3(0,0,0);
         m_damage   = 1;
         m_speed    = 0;
         m_power    = 0;
         m_angle    = 0;
         m_normal   = CFVector3(0,1,0);

         m_towerAxis = CFVector3(0,0,0);

         m_maxMoveDist = 100;
         m_prevPos     = CFVector3(0,0,0);
         m_attackedEnemy = KR_ObjectID::NUL();

         m_maxDistScale = 0;
         m_commander = KR_ObjectID::NUL();
         m_cannons.clr();
         m_phase = 0;

         m_attack = false;
         m_askin = 0;
         m_desireShoot = 1;
    }

//  IDynamicObject
    virtual double 	getMass	   () {return 1;}	// Масса
    virtual TCCFMatrix3x4 &GetDir  () {return m_skin.GetDir(); }
    virtual void SetDir(TCSFMatrix3x4 &dir) { m_skin.GetDirModify() = dir; }


    void     onSetAttr(double ts);

    void     model_drive(
                          KR_TimeDelta     timeStamp,
                          double           power,
                          double           angle
                        );
    bool     queryPosition(  
                             SimulationContext &context,
                             const KR_ObjectID &ID,
                             KR_Event          &ev,
                             CFVector3         &pos
                          ); // inline
    void     rotateCannon(
                             const KR_ObjectID &cannon, 
                             KR_Event          &event,
                             double             hAngle,
                             double             vAngle
                         );
    void     shoot( const KR_ObjectID &cannon, KR_Event &event, int count);

    void     rotateAndShoot(
                             const KR_ObjectID &cannon, 
                             KR_Event          &event,
                             double             hAngle,
                             double             vAngle,
                             int                count
                           );
    void     attack( const KR_ObjectID &enemy, KR_Event &event);
    virtual  CFVector3 realPosition();



             Tank();
    virtual ~Tank();
    virtual void render( CViewDynamicList &list, double ts );
    virtual void endRender( CViewScene *scene );
    virtual int  receiveEvent( KR_Event &event );
    virtual void draw        ( CDC &gc );
    virtual void addNotify   ();
    virtual void removeNotify();

    //{{ACTIONF_DECLARE
    void from_STAY__to__MOVING__F                  ( KR_Event &event );
    void from_MOVING__to__STAY__F                  ( KR_Event &event );
    void from_MOVING__to__MOVING_SHOOTING__F       ( KR_Event &event );
    void from_MOVING_SHOOTING__to__STAY_SHOOTING__F( KR_Event &event );
    void from_STAY_SHOOTING__to__STAY__F           ( KR_Event &event );
    void from_STAY__to__STAY_SHOOTING__F           ( KR_Event &event );
    void from_MOVING_SHOOTING__to__MOVING__F       ( KR_Event &event );
    void from_STAY_SHOOTING__to__MOVING_SHOOTING__F( KR_Event &event );
    void stayAndShootingFC                         ( KR_Event &event );
    void movingAndShootingFC                       ( KR_Event &event );
    void correctDirFC                              ( KR_Event &event );
    void from_STAY__to__STAY_SHOOTING__F_attack    ( KR_Event &event );
    void movingFC                                  ( KR_Event &event );
    void checkRotate                               ( KR_Event &event );
    void correctDirFC_hidden                       ( KR_Event &event );
    void rotateAndShooting                         ( KR_Event &event );
    void shooting                                  ( KR_Event &event );
    void iAmBusyF                                  ( KR_Event &event );
    //}}END_OF_ACTIONF_DECLARE

    //{{GOTO_STATE_DECLARE
    void   gotoMOVING( KR_Event &event );
    void   gotoMOVING_SHOOTING( KR_Event &event );
    void   gotoSTAY_SHOOTING( KR_Event &event );
    void   gotoRETREATING( KR_Event &event );
    void   gotoRETURN( KR_Event &event );
    //void   gotoMOVING( KR_Event &event );
    //void   gotoMOVING_SHOOTING( KR_Event &event );
    //void   gotoSTAY_SHOOTING( KR_Event &event );
    void   gotoROTATE_AND_SHOOTING( KR_Event &event );
    //}}END_OF_GOTO_STATE_DECLARE
    void         loadStateTransitionTable();
    static void AnimateCallBack0(CViewObjectBaseSet *,CViewObjectBase *pBase,CViewObjectRef *ref);
    static void AnimateCallBack1(CViewObjectBaseSet *,CViewObjectBase *pBase,CViewObjectRef *ref);
    static void AnimateCallBack03(CViewObjectBaseSet *,CViewObjectBase *pBase,CViewObjectRef *ref);
    void queryCannonAngle(int can, double &vAngle, double &hAngle);

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
    virtual double getPower (); // Сила юнита 0..10
    virtual int    isFriend ( const KR_ObjectID &commanderID );
    virtual double getDamage(); // Целостность от 0..1
    virtual void   setDamage  ( double d, const CFVector3 &pos, double ts, KR_ObjectID fromID );
    virtual double desireShoot();
    virtual KR_ObjectID getCommander();
    virtual void setCommander(KR_ObjectID);

    // ------ interface ICarrier
    virtual void carrierLoadMatrix  (CFMatrix3x4 &m);


    virtual void onEnterAudibleZone(double ts);
    virtual void onExitAudibleZone (double ts);

    void  forgetEnemy( double timeStamp );
    void  onView(double time);

	virtual bool	shouldDump () { return true; } 
	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
	virtual void	loadNotify();


};

inline bool Tank::queryPosition(
                             SimulationContext &context,
                             const KR_ObjectID &ID,
                             KR_Event          &,
                             CFVector3         &pos
                           ) // FIXME передавать время ?
 {
    IDynamicObject *dobj = (IDynamicObject*)
              (context.queryInterface(ID,IDynamicObjectIID));
    if(  dobj != 0  )
    {
         pos = dobj->getPos();
         return true;
    }
    return false;
 }

 //===========================================================================
class AttributeTable : public ct_AttributeTable
{
 protected:
    AttributeTank *m_table;

 public:
    AttributeTable()
    {
       m_table = NULL;
       registerClass( "TankAttr" );
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};


extern AttributeTank  g_defaultTankAttr;
extern AttributeTable g_tankAttrTable;
//double CalcCannonParam(Tank &tank,const CFVector3 &enemyPos,const CFVector3 &myPos,
                       //double &hAngle, double &vAngle,const CFVector3 &eDir);


#pragma warning 004 1

#endif // ifndef __TANK_H__INCLUDED

/* End of file D:\GAME\OBASE\Tank\tank.h */