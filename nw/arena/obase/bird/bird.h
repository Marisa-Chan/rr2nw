/*
 * File  : C:\WinGame\OBASE\bird\bird.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __BIRD_H__INCLUDED
#define __BIRD_H__INCLUDED

#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "..\DynObj\DynObj.h"
#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"
#include "storage/h/strgdefs.h"

class AttributeBird;

typedef struct {
    KR_ObjectID            m_birdAttrID;
    bool                   m_sendedMove;
    bool                   m_sendedNewPos;

    CFVector3              m_pos;
    CFVector3              m_dir;
    double                 m_prevTime;
    double                 m_rotateOx, 
                           m_rotateOy;

    CFVector3              m_minPos , m_maxPos ;
    CFVector3              m_lastMovePos;

} BirdData;

class Bird :	public ct_Subject,
				public BirdData
{
 void setBirdAttr();
 public:
    const AttributeBird   *m_attr;


    void check( double &a , double &b );
	
             Bird();
    virtual ~Bird();
    virtual int  receiveEvent( KR_Event &event );
    virtual void addNotify   ();
    virtual void removeNotify();

    void                  onView(double time);
    strg_SUBJECT_DYNVIEW_DECLARE

	virtual bool shouldDump () { return true; } 
	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
	virtual void	loadNotify();
};

#endif // ifndef __BIRD_H__INCLUDED
/* End of file C:\WinGame\OBASE\bird\bird.h */