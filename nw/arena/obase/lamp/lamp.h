/*
 * File  : C:\NW\ARENA\OBASE\Lamp\Lamp.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __Lamp_H__INCLUDED
#define __Lamp_H__INCLUDED

#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"
#include "obase/dynobj/dyncorona.h"
#include "obase/dynobj/dynpart.h"
#include "storage/h/savefile.h"

class AttributeLamp;


typedef struct {
	KR_ObjectID		   m_lampAttrID;

    CFVector3          m_lampPosition;
	
	CFVector3          m_startPosition;
	CFVector3          m_endPosition;


	CFVector3		   m_movementDelta;

	int				   m_movementCurrentStep;
	int				   m_movementCurrentDirection;
	int				   m_movementNumberOfSteps;

    int                m_cnt;
    double             m_startTime;
    double	       m_lastEventTime;
    bool               m_sentMove;
    double             m_brightness;

	double		m_currentTransparency;
	
	int				   m_blinkState;
	int				   m_renderLastFrame;

} LampData;


class Lamp : public ct_Subject,
			 public LampData
{
 void setLampAttr();
 public:
    AttributeLamp      *m_attr;

    s_ViewDynamicCorona   m_corona;
	s_ViewDynamicParticle m_particle;

	
    virtual void       onView( double ts );
             Lamp();
    virtual ~Lamp();
    virtual int  receiveEvent( KR_Event &event );
    virtual void addNotify   ();
    virtual void removeNotify();
    virtual CFVector3 realPosition();
    virtual void render( CViewDynamicList &,double);
    virtual void endRender( CViewScene *scene );


	void	RandomChange (KR_Event &event);
	void	Blink		 (KR_Event &event);
	void	Fade 		 ();

	virtual bool shouldDump () { return true; } 
	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
	virtual void	loadNotify();


};

#endif // ifndef __Lamp_H__INCLUDED
/* End of file C:\NW\ARENA\OBASE\Lamp\Lamp.h */
