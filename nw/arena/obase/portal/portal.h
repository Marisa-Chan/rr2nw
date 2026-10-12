/*
 * File  : C:\WinGame\OBASE\portal\portal.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __BIRD_H__INCLUDED
#define __BIRD_H__INCLUDED

#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "zav.h"
#include "..\DynObj\DynObj.h"
#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"
#include "storage/h/strgdefs.h"

#include "i/portal.i"
#include "i/dynobj.i"

class AttributePortal;

typedef struct {
    KR_ObjectID m_portalAttrID;
    CFVector3   m_pos;
    int         m_slotCnt;
    int         m_occupiedSlotCnt;
} PortalData;

class Portal :	public ct_Subject,
		public PortalData,
                public IPortal,
                public IDynamicObject
{
 void setPortalAttr();
 public:
    const AttributePortal   *m_attr;


	
             Portal();
    virtual ~Portal();
    virtual int  receiveEvent( KR_Event &event );
    virtual void addNotify   ();
    virtual void removeNotify();
    virtual void draw        ();
    virtual CFVector3 realPosition();

    void *queryInterface(int IID);

    virtual bool shouldDump () { return true; } 
    virtual bool	dump(PIN_SaveFile & sf);
    virtual bool	load(PIN_SaveFile & );
    virtual void	loadNotify();

    virtual CFVector3 portalGetCoord();
    virtual int       portalGetSlotCnt();
    virtual int       portalGetOccupiedSlot();
    virtual void      portalAddArtefact( KR_ObjectID artID );
    virtual void      portalInit(CFVector3 pos, int sc);
    virtual void      portalSetPortalPoint(CFVector3 pos);

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
    virtual double     getMass	   () {return 1;}	// Масса

    static CFMatrix3x4 dummy;

    virtual TCCFMatrix3x4 &GetDir  () { ASSERT(0); return dummy; }
    virtual void SetDir(TCSFMatrix3x4 &dir) {setPosition(dir.Offset() ); }

};

#endif // ifndef __BIRD_H__INCLUDED
/* End of file C:\WinGame\OBASE\portal\portal.h */