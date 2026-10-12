/*
 * File  : \OBASE\Artefact\Artefact.h
 * Autor : Suavik
 * Ver   1.0 
 */
#ifndef __ARTEFACT_H__INCLUDED
#define __ARTEFACT_H__INCLUDED

#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "..\DynObj\DynObj.h"
#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "storage/h/strgdefs.h"

#include "kernel/h/active.h"

#include "i/carrier.i"
#include "i/unit.i"
#include "i/dynobj.i"

#include "obase/dynobj/dyncorona.h"

class AttributeArtefact;
class ArtefactObj : public s_ViewDynamicObject
{
public:
        const AttributeArtefact *m_attr;
        CFVector3          m_position;
        double             m_startTime;
        unsigned long      m_rayColor;
        int                m_useRay;
        s_ViewDynamicCorona  m_corona;

        ArtefactObj(CViewObjectRef &par) : s_ViewDynamicObject(par) {}
	virtual void Draw();
};

typedef struct
{
   CFVector3   pos, myPos;
   KR_ObjectID id;
   double       dist2;
} TPortalFind;


typedef struct {
	KR_ObjectID              m_artefactAttrID;
	CFMatrix3x4              m_orient;
    CFVector3                m_dir;
    KR_ObjectID              m_commander;
} ArtefactData;


class Artefact : public ct_Subject,
                 public IArtefact,
                 public IUnit,
                 public IDynamicObject,
				 public ArtefactData
{
	void setArtefactAttr();
 public:
    const AttributeArtefact *m_attr;
    CViewObjectRef  m_skin;
    ArtefactObj     m_viewDynObj;
    
    virtual  void  moveTo    ( CFMatrix3x4 &m );
    virtual  int   attachTo  ( KR_ObjectID masterID, ICarrier *master );
    virtual  void  drop      ( CFMatrix3x4 &m, double ts );

	
             Artefact();
    virtual ~Artefact();
    virtual int  receiveEvent( KR_Event &event );
    virtual void addNotify   ();
    virtual void removeNotify();

    void         onView(double time);
   
	virtual void render   ( CViewDynamicList &list, double ts );
    virtual void endRender( CViewScene *scene );
    virtual CFVector3     realPosition();
    void                  onRender(double ts);

    virtual double getPower   ();
    virtual int    isFriend   ( const KR_ObjectID &commanderID );
    virtual double getDamage  ();
    virtual void   setDamage  ( double d, const CFVector3 &pos, double ts,
                                    KR_ObjectID fromID );
    virtual double desireShoot();
    virtual KR_ObjectID getCommander();
    virtual void        setCommander(KR_ObjectID oID);

    virtual void       *queryInterface( int IID );
    // dynamic object
    virtual CFVector3  getPos      ();
    virtual double     getHAngle   ();
    virtual CFVector3  getUpVector ();
    virtual CFVector3  getCenter   (); // Относительно 0 объекта
    virtual double     getRadius   (); // Относительно центра
    virtual double     getRadius0  (); // Относительно 0 объекта
    virtual CFVector3  getMoveDir  (); // Направление движения
    virtual double     getMoveSpeed(); // Скорость
    virtual void       getMatrix   ( CFMatrix3x4 &m);

    virtual double     getMass	   ();	// Масса
    virtual TCCFMatrix3x4 &GetDir	   ();
    virtual void SetDir(TCSFMatrix3x4 &dir);

	virtual bool	shouldDump () { return true; }
	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
	virtual void	loadNotify();  
    virtual  void  artefactMove( const CFVector3 &dir );


    bool findPortal( TPortalFind &pf );
};

#endif // ifndef __ARTEFACT_H__INCLUDED
/* End of file \OBASE\Artefact\Artefact.h */
