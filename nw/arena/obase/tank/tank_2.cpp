#include "tank.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/unitmsg.h"


//{{EVENT_LABEL_NAMES
static s_ELN elnTable[]=
{
   s_ELN(t_EV_MOVE,"t_EV_MOVE"),
   s_ELN(t_EV_STOP,"t_EV_STOP"),
   s_ELN(t_EV_ATTACK,"t_EV_ATTACK"),
   s_ELN(t_EV_TARGET_LOCKED,"t_EV_TARGET_LOCKED"),
   s_ELN(t_EV_YOU_ATTACKED,"t_EV_YOU_ATTACKED"),
   s_ELN(t_EV_TARGET_LOST,"t_EV_TARGET_LOST"),
   s_ELN(t_EV_RETREATING,"t_EV_RETREATING"),
   s_ELN(t_EV_RETURN,"t_EV_RETURN"),
   s_ELN(t_EV_REACHED,"t_EV_REACHED"),
   s_ELN(t_EVC_STAY_AND_SHOOTING,"t_EVC_STAY_AND_SHOOTING"),
   s_ELN(t_EVC_MOVING_AND_SHOOTING,"t_EVC_MOVING_AND_SHOOTING"),
   s_ELN(t_EVC_RETREATING,"t_EVC_RETREATING"),
   s_ELN(t_EVC_RETURN,"t_EVC_RETURN"),
   s_ELN(UNIT_I_DRIVE,"UNIT_I_DRIVE"),
   s_ELN(t_EVC_CHECK_ROTATE,"t_EVC_CHECK_ROTATE"),
   s_ELN(),
};

static s_ELNTable selnTable("Default",elnTable);

//}}END_OF_EVENT_LABEL_NAMES

void Tank::gotoMOVING( KR_Event &event )
{

    context->removeEvent(UNIT_I_DRIVE, getObjectID());
    m_have_UNIT_I_DRIVE = 0;

    double prevTimeStamp = event.timeStamp;
    m_prevTime        = event.timeStamp;
    m_prevPos         = getPosition();

	event.label       = t_EVC_MOVING;
    event.source      = getObjectID();
    event.destination = getObjectID();
    //{{PUT_EVENT(t_EVC_MOVING)
    event.data.open(EDO_WRITE)
                  .putDouble(prevTimeStamp)
              .close();
    //}}END_OF_PUT_EVENT(t_EVC_MOVING)
    issueEvent( event );

}

void Tank::gotoSTAY_SHOOTING( KR_Event &event )
{
    double prevTimeStamp = event.timeStamp;
    event.label       = t_EVC_STAY_AND_SHOOTING;
    event.source      = getObjectID();
    event.destination = getObjectID();
    //{{PUT_EVENT(t_EVC_STAY_AND_SHOOTING)
    //}}END_OF_PUT_EVENT(t_EVC_STAY_AND_SHOOTING)
    issueEvent( event );

}

void Tank::gotoRETREATING( KR_Event &event )
{
    double prevTimeStamp = event.timeStamp;
    m_prevTime        = event.timeStamp;


    event.label       = t_EVC_RETREATING;
    event.source      = getObjectID();
    event.destination = getObjectID();
    //{{PUT_EVENT(t_EVC_RETREATING)
    event.data.open(EDO_WRITE)
                   .putDouble(prevTimeStamp)
              .close();
    //}}END_OF_PUT_EVENT(t_EVC_RETREATING)
    issueEvent( event );

}

void Tank::gotoRETURN( KR_Event &event )
{
    double prevTimeStamp = event.timeStamp;
    event.label       = t_EVC_RETURN;
    event.source      = getObjectID();
    event.destination = getObjectID();
    //{{PUT_EVENT(t_EVC_RETURN)
    event.data.open(EDO_WRITE)
                   .putDouble(prevTimeStamp)
              .close();
    //}}END_OF_PUT_EVENT(t_EVC_RETURN)
    issueEvent( event );

}

void Tank::gotoMOVING_SHOOTING( KR_Event &event )
{
    double prevTimeStamp = event.timeStamp;
    m_prevTime        = event.timeStamp;
    m_prevPos         = getPosition();

    event.label       = t_EVC_MOVING_AND_SHOOTING;
    event.source      = getObjectID();
    event.destination = getObjectID();
    //{{PUT_EVENT(t_EVC_MOVING_AND_SHOOTING)
    event.data.open(EDO_WRITE)
                   .putDouble(prevTimeStamp)
              .close();
    //}}END_OF_PUT_EVENT(t_EVC_MOVING_AND_SHOOTING)
    issueEvent( event );

    event.timeStamp = prevTimeStamp;

    event.label       = t_EVC_MOVING;
    event.source      = getObjectID();
    event.destination = getObjectID();
    //{{PUT_EVENT(t_EVC_MOVING)
    event.data.open(EDO_WRITE)
                  .putDouble(prevTimeStamp)
              .close();
    //}}END_OF_PUT_EVENT(t_EVC_MOVING)
    issueEvent( event );

}

//{{GOTO_STATE_IMPLEMENTATION

