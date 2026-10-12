/*
 * File   : C:\NW\ARENA\OBASE\Taxi\Taxi.h
 * Author : Suavik
 * Ver   1.0 
 */
#ifndef __TAXI_H__INCLUDED
#define __TAXI_H__INCLUDED




#include "..\DynObj\DynObj.h"
#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"
#include "i/dynobj.i"
#include "i/unit.i"
#include "i/taxi.i"
#include "i/skin.i"
#include "obase/sound/wavobj.h"


 //===========================================================================
class AttributeTaxi : public ct_Attribute
{
 public:
    CViewObjectModel*   m_cacheSkin              ;  // 

	ct_ClassTableID     m_cacheCorpseTable;
    int					m_cacheCorpseAttr;

    KR_ObjectID			m_skinID;
    KR_ObjectID         m_attrForVehicle;
    virtual void    update(double ts);  

//{{ATTRIBUTE
    ct_AttrItem  m_array[7];
    ct_AttrStr      m_name                   ;  // 
    ct_AttrStr      m_skinName               ;  // 
    ct_AttrStr      m_attrForVehicleName     ;  // 
    ct_AttrStr      m_corpseAttrName         ;  // 
    double          m_initialDamage          ;  // 
    double          m_yOffset                ;  // 
    int             m_buzzing                ;  // Звучит или нет

    AttributeTaxi()
    {
        strncpy(m_name,"otank", sizeof( ct_AttrStr )-1 );
        strncpy(m_skinName,"sk.Tank", sizeof( ct_AttrStr )-1 );
        strncpy(m_attrForVehicleName,"", sizeof( ct_AttrStr )-1 );
        strncpy(m_corpseAttrName,"Corpse.Attr.Default", sizeof( ct_AttrStr )-1 );
        m_initialDamage      = 1.0;
        m_yOffset            = 0;
        m_buzzing            = 0;

        m_array[0].set("m_name",m_name);
        m_array[1].set("m_skinName",m_skinName);
        m_array[2].set("m_attrForVehicleName",m_attrForVehicleName);
        m_array[3].set("m_corpseAttrName",m_corpseAttrName);
        m_array[4].set("m_initialDamage",m_initialDamage);
        m_array[5].set("m_yOffset",m_yOffset);
        m_array[6].set("m_buzzing",m_buzzing);

        linkTable(m_array,7);
    }
//}}END_OF_ATTRIBUTE
};

 //===========================================================================
class AttributeTableTaxi : public ct_AttributeTable
{
 protected:
    AttributeTaxi *m_table;

 public:
    AttributeTableTaxi()
    {
       m_table = NULL;
       registerClass( "TaxiAttr" );
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};


extern AttributeTableTaxi __attrTaxiTable;



typedef struct {
	KR_ObjectID      m_taxiAttrID;
	KR_ObjectID      m_snd;
	ct_ClassTableID  m_ctsndID;
	double			 m_damage;
    int              m_bulletCnt;
	CFMatrix3x4 m_taxiDir;
} TaxiData;

class Taxi : 
         public ct_Subject,
         public IDynamicObject,
         public IUnit,
         public ITaxi,
         public TaxiData

{
 void setTaxiAttr();

 public:
    AttributeTaxi         *m_attr;
    CViewObjectRef         m_skin;
    s_ViewDynamicObject    m_viewDynObj;
    ISkin                 *m_askin;
	WAVObj				  * m_wav;	   

	

             Taxi();
    virtual ~Taxi();
    virtual int  receiveEvent( KR_Event &event );
    virtual void draw        ();
    virtual void addNotify   ();
    virtual void removeNotify();
	void    setPosition ( const CFVector3 &vec );

    virtual void render   ( CViewDynamicList &list, double ts );
    virtual void endRender( CViewScene *scene );
    virtual CFVector3     realPosition();
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
    virtual void   setDamage  ( double d, const CFVector3 &pos, double ts, KR_ObjectID );
    virtual KR_ObjectID getCommander();
    virtual void setCommander(KR_ObjectID);

    // ITaxi interface
    virtual KR_ObjectID getAttributeForVehicle();
    virtual CFVector3   taxiPos();
    virtual void        taxiSetBulletCnt(int cnt);
    virtual int         taxiGetBulletCnt();

//  IDynamicObject
    virtual double 	getMass	   () {return 1;}	// Масса
    virtual TCCFMatrix3x4 &GetDir  () {return m_skin.GetDir(); }
    virtual void SetDir(TCSFMatrix3x4 &dir) { m_skin.GetDirModify() = dir; }


	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
	virtual void	loadNotify();
	virtual bool	shouldDump () { return true; }    

    virtual void 	onEnterAudibleZone(double ts);
    virtual void        onExitAudibleZone (double ts);


};

#endif // ifndef __TAXI_H__INCLUDED
/* End of file C:\NW\ARENA\OBASE\Taxi\Taxi.h */