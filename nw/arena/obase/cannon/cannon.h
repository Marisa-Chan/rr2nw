/*
 * File  : D:\GAME\OBASE\Cannon\Cannon.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __CANNON_H__INCLUDED
#define __CANNON_H__INCLUDED

#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"
#include "storage/h/savefile.h"
#include "i/cannon.i"


class AttributeCannon;
class CDC;


typedef struct {	
	int				   m_attrIndex;
    ct_ClassTableID    m_bulletTable;
    KR_ObjectID        m_cannonMaster;
    CFVector3          m_vector;
    double             m_hAngle;
    double             m_vAngle;
    double             m_localHAngle,
                       m_localVAngle;

} CannonData;

class Cannon : 
               public ct_Subject, 
               public KR_ActiveObject,
               public ICannon,
			   public CannonData
{
 public:
    
    AttributeCannon   *m_attr;    

    void startInitialize()
    {
        m_bulletTable = -1;
        m_cannonMaster      = KR_ObjectID::NUL();
        m_vector      = CFVector3(0,0,-1);
        m_hAngle      = 0.0;
        m_vAngle      = 0.3;
        m_localHAngle = 0;
        m_localVAngle = 0;
    }
             Cannon();
    virtual ~Cannon();
    virtual void*queryInterface( int interf );
    virtual int  receiveEvent( KR_Event &event );
    virtual void draw        ( CDC &gc );
    virtual void addNotify   ();
    virtual void removeNotify();

    //------------ ICannon interface --------------------
    virtual void       setLocalHAngle(double hAngle);
	virtual double     getLocalHAngle();
	virtual void       setLocalVAngle(double vAngle);
    virtual double     getLocalVAngle();
	virtual double     getHAngle     ();
	virtual double     getVAngle     ();
  
    virtual void       rotate        ( 
	                                   double prevTimeStamp, 
									   double hAngle,
									   double vAngle
									 );
    virtual void       rotateAndShoot(
	                                   double prevTimeStamp, 
									   double hAngle,
									   double vAngle,
									   int count,
									   int bulletAttrIndex
	                                 );
    //---------------------------------------------------
    
    //{{ACTIONF_DECLARE
    void from_STAY__to__SHOOTING__F  ( KR_Event &event );
    void from_SHOOTING__to__IDLE__F  ( KR_Event &event );
    void repeatShootF                ( KR_Event &event );
    void from_STAY__to__SINGLEIDLE__F( KR_Event &event );
    void rebuldF                     ( KR_Event &event );
    void from_AUTOIDLE__to__STAY__F  ( KR_Event &event );
    //}}END_OF_ACTIONF_DECLARE

    void         loadStateTransitionTable();
    CFVector3    queryMasterPos();
    void         shoot( int bulletAttrIndex, double timeStamp );
    void         setDirection(); // inline 
    virtual CFVector3     realPosition();
	virtual int        isFixed       ();

	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
	virtual void	loadNotify();
	virtual bool	shouldDump () { return true; } 
};

inline void Cannon::setDirection()
 {
    double cos_V = cos(m_vAngle);

    m_vector = CFVector3(   cos(m_hAngle)*cos_V,
                            sin(m_vAngle),
                            sin(m_hAngle)*cos_V );
 }

#endif // ifndef __CANNON_H__INCLUDED
/* End of file D:\GAME\OBASE\Cannon\Cannon.h */
