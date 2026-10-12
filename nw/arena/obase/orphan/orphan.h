/*
 * File   : C:\NW\ARENA\OBASE\Orphan\Orphan.h
 * Author : Suavik
 * Ver   1.0 
 */
#ifndef __Orphan_H__INCLUDED
#define __Orphan_H__INCLUDED

#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "..\DynObj\DynObj.h"
#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"
#include "i/dynobj.i"
#include "obase/sound/wavobj.h"



class AttributeTaxi;
class AttributeOrphan;


typedef struct {
	KR_ObjectID          m_orphanAttrID;
	KR_ObjectID          m_snd;
	ct_ClassTableID      m_ctsndID;
	CFVector3            m_speed;
	double               m_damage;
	double               m_lastEventTime;
	CFMatrix3x4          m_dir;
        CFVector3            m_lastMovePos;
        double               m_lastMoveDeltaT;
} OrphanData;

class Orphan : 
         public ct_Subject,
         public IDynamicObject,
         public OrphanData
{

	void setOrphanAttr();

 public:

	
    WAVObj                * m_wav;	   
    AttributeTaxi         * m_taxiAttr;
    AttributeOrphan       * m_attr;
    CViewObjectRef          m_skin;
    s_ViewDynamicObject     m_viewDynObj;

	
	void KillMe(CFVector3 & newPos, double ts);

             Orphan();
    virtual ~Orphan();
    virtual int  receiveEvent( KR_Event &event );
    virtual void draw        ();
    virtual void addNotify   ();
    virtual void removeNotify();

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
    virtual double 	getMass	   () {return 1;}	// Масса
    virtual TCCFMatrix3x4 &GetDir  () {return m_skin.GetDir(); }
    virtual void SetDir(TCSFMatrix3x4 &dir) { m_skin.GetDirModify() = dir; }
    virtual void onHide(double ts);
    virtual void 	onEnterAudibleZone(double ts);
    virtual void        onExitAudibleZone (double ts);


	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
	virtual void	loadNotify();
	virtual bool	shouldDump () { return true; } 
};

#endif // ifndef __Orphan_H__INCLUDED
/* End of file C:\NW\ARENA\OBASE\Orphan\Orphan.h */