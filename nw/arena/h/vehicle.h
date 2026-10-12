#ifndef __VEHICLE_H__INCLUDED
#define __VEHICLE_H__INCLUDED

#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"

#include "i/dynobj.i"
#include "i/unit.i"
#include "i/vessel.i"
#include "i/carrier.i"

#include "sound.h"
#include "obase/sound/wavobj.h"
#include "obase/player/player.h"

#include "storage/h/savefile.h"

 //===========================================================================
class AttributeVehicle : public ct_Attribute
{
 public:
    CGRPanel        *m_panel;
    KR_ObjectID      m_taxiID;
    int              m_bulletTable;
    int              m_bulletAttrIndex;
    int              m_bulletSecAttrIndex;

    virtual void    update(double ts);
    virtual void    removeNotify();
//{{ATTRIBUTE
    ct_AttrItem  m_array[27];
    int             m_type                   ;  // 
    double          m_timeInc                ;  // 
    double          m_radius                 ;  // 
    double          m_centerOffsetY          ;  // 
    ct_AttrStr      m_panelName              ;  // 
    ct_AttrStr      m_dynamic                ;  // 
    double          m_radius0                ;  // 
    double          m_outOfsX                ;  // 
    double          m_outOfsZ                ;  // 
    double          m_bornY                  ;  // 
    ct_AttrStr      m_taxiName               ;  // 
    ct_AttrStr      m_soundName              ;  // 
    CFVector3       m_cannonOffset           ;  // 
    CFVector3       m_hAxis                  ;  // 
    ct_AttrStr      m_bulletAttrName         ;  // 
    double          m_bulletSlipTime         ;  // 
    ct_AttrStr      m_bulletSecAttrName      ;  // 
    ct_AttrStr      m_shootSecAttrName       ;  // 
    double          m_bulletSecSlipTime      ;  // 
    double          m_taxiRotateSpeed        ;  // 
    double          m_taxiMoveSpeed          ;  // 
    double          m_engineStartMaxPitch    ;  // 
    double          m_engineStartRecalcTime  ;  // 
    double          m_power                  ;  // 
    double          m_soundMinPitch          ;  // 
    double          m_soundMaxPitch          ;  // 
    ct_AttrStr      m_outFlicName            ;  // 

    AttributeVehicle()
    {
        m_type               = 0;
        m_timeInc            = 0.02;
        m_radius             = 3;
        m_centerOffsetY      = 2;
        strncpy(m_panelName,"", sizeof( ct_AttrStr )-1 );
        strncpy(m_dynamic,"Emveshka", sizeof( ct_AttrStr )-1 );
        m_radius0            = 4;
        m_outOfsX            = 14.5;
        m_outOfsZ            = 0;
        m_bornY              = 4;
        strncpy(m_taxiName,"", sizeof( ct_AttrStr )-1 );
        strncpy(m_soundName,"", sizeof( ct_AttrStr )-1 );
        m_cannonOffset       = CFVector3(0,2,0);
        m_hAxis              = CFVector3(0,0,0);
        strncpy(m_bulletAttrName,"", sizeof( ct_AttrStr )-1 );
        m_bulletSlipTime     = 0.2;
        strncpy(m_bulletSecAttrName,"", sizeof( ct_AttrStr )-1 );
        strncpy(m_shootSecAttrName,"", sizeof( ct_AttrStr )-1 );
        m_bulletSecSlipTime  = 0.8;
        m_taxiRotateSpeed    = 6.28;
        m_taxiMoveSpeed      = 15.0;
        m_engineStartMaxPitch = 3.0;
        m_engineStartRecalcTime = 0.1;
        m_power              = 10.0;
        m_soundMinPitch      = 0.15;
        m_soundMaxPitch      = 4.0;
        strncpy(m_outFlicName,"", sizeof( ct_AttrStr )-1 );

        m_array[0].set("m_type",m_type);
        m_array[1].set("m_timeInc",m_timeInc);
        m_array[2].set("m_radius",m_radius);
        m_array[3].set("m_centerOffsetY",m_centerOffsetY);
        m_array[4].set("m_panelName",m_panelName);
        m_array[5].set("m_dynamic",m_dynamic);
        m_array[6].set("m_radius0",m_radius0);
        m_array[7].set("m_outOfsX",m_outOfsX);
        m_array[8].set("m_outOfsZ",m_outOfsZ);
        m_array[9].set("m_bornY",m_bornY);
        m_array[10].set("m_taxiName",m_taxiName);
        m_array[11].set("m_soundName",m_soundName);
        m_array[12].set("m_cannonOffset",EDI_NONE,&m_cannonOffset);
        m_array[13].set("m_hAxis",EDI_NONE,&m_hAxis);
        m_array[14].set("m_bulletAttrName",m_bulletAttrName);
        m_array[15].set("m_bulletSlipTime",m_bulletSlipTime);
        m_array[16].set("m_bulletSecAttrName",m_bulletSecAttrName);
        m_array[17].set("m_shootSecAttrName",m_shootSecAttrName);
        m_array[18].set("m_bulletSecSlipTime",m_bulletSecSlipTime);
        m_array[19].set("m_taxiRotateSpeed",m_taxiRotateSpeed);
        m_array[20].set("m_taxiMoveSpeed",m_taxiMoveSpeed);
        m_array[21].set("m_engineStartMaxPitch",m_engineStartMaxPitch);
        m_array[22].set("m_engineStartRecalcTime",m_engineStartRecalcTime);
        m_array[23].set("m_power",m_power);
        m_array[24].set("m_soundMinPitch",m_soundMinPitch);
        m_array[25].set("m_soundMaxPitch",m_soundMaxPitch);
        m_array[26].set("m_outFlicName",m_outFlicName);

        linkTable(m_array,27);
    }
//}}END_OF_ATTRIBUTE
};

 

class CGRPanel;

enum
{
    IVehicleIID = 9
};

struct TViewPoint
{
  CFVector3 vp;
};
extern TViewPoint g_vp[40];



typedef struct {
    KR_ObjectID m_vehicleAttrID;
    KR_ObjectID m_vehicleAttrDefaultID;
    KR_ObjectID m_vehicleAttrDeadID;

// Unit properties. Derived from the appropriate taxi
    bool        m_firePrim, m_primEnable;
    bool        m_fireSec, m_secEnable;
    bool        m_firePrimPress;
    bool        m_fireSecPress;
    bool        m_playingBriefingSound;
    double      m_damage;
    double      m_power;
    double      m_currentPitch;
    KR_ObjectID m_commander;
    CFVector3   m_skipPos;
    double      m_skipTime;
    int	 	m_secBulletCnt;
    bool	m_playedBrief;
    double	m_lastLeaveTime;
    double      m_lastTime;

} VehicleData;

class Vehicle : 
                public ct_Subject,
                public IDynamicObject,
                public IUnit,
                public ICarrier,
				public VehicleData
				
{
    AttributeVehicle		 *m_attr;
    IVessel                  *m_vessel;
    CGRPanel                 *m_panel;
    void 		     *m_zavSavePoint;
    
    
    static bool MasterBumpCallBack( ct_Subject *, SBumpDef & );
        	
    IRSXDirectListener * m_lpDL;
    IRSXCachedEmitter  * m_lpCE;
    IRSXCachedEmitter  * m_lpCannonCE;
    WAVObj    *m_wav;	
    

    void LeaveVehicle(double ts, BOOL makeOrphan);	
	void setVehicleAttr();
	void InitSound();
private:
    Player      m_player;
    
public:
    
    Vehicle() {  m_playingBriefingSound = false; m_dead = false; m_power = 9.0; m_damage = 1; m_attr = 0; m_panel = 0; m_vessel = 0; m_lpDL = 0; m_lpCE = 0; m_lpCannonCE = 0; }

    virtual int  receiveEvent( KR_Event &event );
    virtual void addNotify();
    virtual void removeNotify();

    void updateSound(double ts);
    void setBriefingSound(char * name, int cycle, double ts);

	static bool m_dead;
	static double s_curTime;
	static int  m_isTakingTaxiNow;
	static double m_takingTaxiCurrentAngle; 
    static double m_takingTaxiFinalAngle;
    static double m_lastEventTime;
    static double m_taxiRotateSpeed;
    static double m_spX;
    static double m_spZ;
    static double m_spY;
    static CFVector3 m_currentTaxiPos;
    static CFVector3 m_currentTaxiOurPos;

    
	static void transformMatrix(CFMatrix3x4 & tdir);     

    virtual CFVector3  realPosition();
    IPlayer &player() { return m_player; }

    virtual void      *queryInterface( int interNum );
    // IDynamicObject interface

    virtual CFVector3  getPos      ();
    virtual double     getHAngle   ();
    virtual CFVector3  getUpVector ();
    virtual CFVector3  getCenter   (); // Относительно 0 объекта
    virtual double     getRadius   (); // Относительно центра
    virtual double     getRadius0  (); // Относительно 0 объекта
    virtual CFVector3  getMoveDir  (); // Направление движения
    virtual double     getMoveSpeed(); // Скорость
    virtual void       getMatrix   ( CFMatrix3x4 &m );

    // IUnit interface
    virtual double getPower(); // Сила юнита 0..10
    virtual int    isFriend( const KR_ObjectID &commanderID );
    virtual double getDamage(); // Целостность от 0..1
    virtual double desireShoot();
    virtual void   setDamage  ( double d, const CFVector3 &pos, double ts,KR_ObjectID from );
    virtual KR_ObjectID getCommander();
    virtual void setCommander(KR_ObjectID);


    void         NextFrame(double fDeltaT);
    void         UpdatePos();
    void         Restart();//                   { m_vessel->Restart(); UpdateSound(Session::m_moment); }

    CFMatrix3x4 &GetDir ()                   { return m_vessel->GetDir(); }
    virtual double 	getMass	   () {return 1;}	    
    virtual void SetDir(TCSFMatrix3x4 &dir) { m_vessel->SetDir(dir); }

    void         SetPos (const CFVector3 &v) { m_vessel->SetPos(v); }
    CFVector3    Pos()                       { return m_vessel->Pos(); }
    void         BeginPreStep()              { m_vessel->BeginPreStep(); }
    void         Stop()                      { m_vessel->Stop(); }

    void         setAttr        (KR_Event &event);
    void         openPanel      (double ts);
    void         drawPanel      ();
    void         closePanel     (double ts);
    void         onChangeVehicle(double ts);
    void         onFire         ( double timeStamp );
    void         onFireSec      ( double timeStamp );

    void         onSetTaxi              (KR_Event & event);     
    CFVector3	 Speed() { return m_vessel->Speed(); }

    virtual void carrierLoadMatrix  (CFMatrix3x4 &m);

    virtual void         SkipTime(CFVector3 pos,double t);
    void                 SkipTime();

	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
	virtual void	loadNotify();
	virtual bool	shouldDump () { return true; } 	

	static bool	SaveStaticData(PIN_SaveFile & sf);
	static bool	LoadStaticData(PIN_SaveFile & );

};


//===========================================================================
class VehicleTable : public ct_SubjectTable
{
 private:
    Vehicle *m_table;
 public:
    VehicleTable()
    {
        m_table = NULL;
        registerClass( "Vehicle" );
    }
    ~VehicleTable()
    {
        delete [] m_table;
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
    void    ReadConfig ();
};

 //===========================================================================
class AttributeTableVehicle : public ct_AttributeTable
{
 protected:
    AttributeVehicle *m_table;

 public:
    AttributeTableVehicle()
    {
       m_table = NULL;
       registerClass( "VehicleAttr" );
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

extern AttributeTableVehicle __attrVehicleTable;



extern Vehicle *g_vehicle;

#endif // ifndef __VEHICLE_H__INCLUDED
