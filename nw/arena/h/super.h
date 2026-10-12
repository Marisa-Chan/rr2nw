#ifndef __SUPER_H__INCLUDED
#define __SUPER_H__INCLUDED

#include "h/olevel.h"
#include "kernel/h/session.h"
#include "kernel/h/observer.h"
#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "obase/publish/publish.h"


class a_TObserver: public KR_Observer, public KR_Object
 {
 public:
     ct_Arena *m_arena;

     static KR_ActiveObject::StateTransitionTableElem m_table[1];
     static KR_ActiveObject::StateElem                m_state[1];

     int          eventHandler( KR_Event & )
     { return 0; }

     int          receiveEvent( KR_Event &event )
     {
        switch( event.label )
        {
        case KR_WAKE_UP: break;
        default: return eventHandler(event);
        }
        return 1;
     }
     void         init( ct_Arena &arena )
     {
        m_arena = &arena;
     }
     virtual void loadStateTransitionTable();
     virtual void draw( CDC &gc );

     virtual bool shouldDump () { return false; } // we don't dump attributes

 };


class Supervisor
{
 public:
    ol_Level            m_level;
    TSuaCript           m_suacript;
    a_TObserver         m_observer;
    Session             m_session;
    SimulationContext  *m_context;
    Publisher          *m_publisher;


                   Supervisor() 
                   {
                      m_context   = NULL;
                      m_publisher = NULL;
                   }

                  ~Supervisor() 
                   {
                      delete m_context;
                      delete m_publisher;
                   }

    void           startSeance();
    void           endSeance();

};

extern Supervisor g_super;


typedef struct {
	double  m_aspect;
    double  m_curTime;
    double  m_pauseTime;
	long	m_deltaTime;
} TimerData;

class a_TTimer: public KR_RealTimer,
				public TimerData
 {
 public:
   
   long    m_prevTime;
   long    m_startTick;

       void addTime(double t);
       virtual double GetTime();
       virtual double GetTimeDiff(double timeStamp);
	   virtual double ConvertSysTime(dword tick); 
	   virtual void   Wait(double interval);
	   virtual void   SetCurTime(double interval);
       void    Start();
       a_TTimer() { m_aspect = 1.0; }

	bool	dump(PIN_SaveFile & sf);
	bool	load(PIN_SaveFile & sf);
};


extern a_TTimer g_timer;


#endif

