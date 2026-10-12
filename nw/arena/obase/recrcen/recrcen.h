/*
 * File  : C:\NW\ARENA\OBASE\recrcen\Recrcen.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __RECRCEN_H__INCLUDED
#define __RECRCEN_H__INCLUDED

#include "storage/h/subject.h"
#include "kernel/h/active.h"
#include "i/dynobj.i"
#include "i/player.i"
#include "dmap.h"


typedef struct {
    
	CFVector3	m_eject;
    int			m_working;  
    char		m_myCommander[50];
    KR_ObjectID m_comID;
    double      m_prevVisitTime;
	char		m_defaultBriefing[80];	

} RecruitCenterData;

class RecruitCenter : 
               public ct_Subject,
               public IDynamicObject,
			   public RecruitCenterData

{
 public:	
	IPlayer  *m_player;
             RecruitCenter();
    virtual ~RecruitCenter();
    virtual int  receiveEvent( KR_Event &event );
    virtual void draw        ();
    virtual void addNotify   ();
    virtual void removeNotify();
    virtual CFVector3 realPosition();

    KR_ObjectID chooseProject(double timeStamp);
    int         runProject   (KR_ObjectID oID,double ts);

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

//  IDynamicObject
    virtual double 	getMass	   () {return 1;}	// Масса

    static CFMatrix3x4 dummy;

    virtual TCCFMatrix3x4 &GetDir  () { ASSERT(0); return dummy; }
    virtual void SetDir(TCSFMatrix3x4 &dir) {setPosition(dir.Offset() ); }

	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
	virtual void	loadNotify();
	virtual bool	shouldDump () { return true; }    
};

extern TLinkConstExtern externConst[];
extern TLinkExtern externFunc[];


#endif // ifndef __RECRCEN_H__INCLUDED
/* End of file C:\NW\ARENA\OBASE\recrcen\Recrcen.h */