#include <windows.h>
#include "filesys.h"
#include "message/hardmsg.h"
#include "hardware.h"

#define LAST_H__VIEW
#include "game.h"
#include "scene.h"

#include "kernel/h/session.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"

#include "h/vehicle.h"
#include "message/vehiclemsg.h"
#include "zav.h"
#include "h/phisics.h"
#include "suavik.h"
#include "console.h"

static int    Vehicle::m_isTakingTaxiNow = 0;
static double Vehicle::m_takingTaxiCurrentAngle;
static double Vehicle::m_takingTaxiFinalAngle;

static double Vehicle::m_lastEventTime;
static double Vehicle::m_taxiRotateSpeed;

static double Vehicle::m_spZ;
static double Vehicle::m_spX;
static double Vehicle::m_spY;
static CFVector3 Vehicle::m_currentTaxiPos;
static CFVector3 Vehicle::m_currentTaxiOurPos;
static bool	  Vehicle::m_dead = 0;


void Vehicle::transformMatrix(CFMatrix3x4	& tdir)
{
	double dt = Session::m_moment - m_lastEventTime;
	m_lastEventTime = Session::m_moment;
 

	if (!m_dead)
	{
		m_takingTaxiCurrentAngle = interpolateAngle( 
			m_takingTaxiCurrentAngle, 
			m_takingTaxiFinalAngle,
			m_taxiRotateSpeed,
			dt );
		
		tdir.RotateOyR(m_takingTaxiCurrentAngle);
		
		m_currentTaxiOurPos.x -= m_spX * dt;
		m_currentTaxiOurPos.z -= m_spZ * dt;			
		m_currentTaxiOurPos.y += m_spY * dt;					
	}
	else
	{
		CFVector3 v = - tdir.Row(2);
		double angle = atan2(v.y, hypot(v.x, v.z));
		tdir.RotateOxL(M_PI / 2 + angle);

		if ( -m_currentTaxiOurPos.y < CViewFigure::HazeMax() + CViewFigure::HazeMin() )
		  m_currentTaxiOurPos.y -= 8.0 * dt;
		else
		  exit(0);
	}

	tdir.TranslateR(m_currentTaxiOurPos);
	

}


//-------------------------------------------------------------
void Vehicle::UpdatePos(){
    double curTime = Session::m_viewTime;
    s_curTime = curTime;

	
    double fDeltaT = curTime-m_lastTime;
    m_vessel->AccumPreStep(fDeltaT);
    m_vessel->PreStep(fDeltaT);
    m_vessel->NextFrame(); 
    m_vessel->ApplyStep();
    m_lastTime = curTime;


    if (m_lpDL)
    {
        CFVector3 pos = CViewObject::m_viewPointInvMx.Offset();
        RSXVECTOR3D v3d, dir, up;

        v3d.x = pos.x;
        v3d.y = pos.y;
        v3d.z = pos.z;

        if (m_lpCE)
          m_lpCE->SetPosition(&v3d);
        m_lpDL->SetPosition(&v3d);

        CFVector3 front = CViewObject::m_viewPointInvMx.Column(2);
        CFVector3 upper = CViewObject::m_viewPointInvMx.Column(1);

        dir.x = float(front.x); 
        dir.y = float(front.y); 
        dir.z = float(front.z); 

        up.x  = float(upper.x);
        up.y  = float(upper.y);
        up.z  = float(upper.z);

        m_lpDL->SetOrientation(&dir, &up);
        if (m_vessel && m_lpCE && !m_playingBriefingSound)
        {
		    double pitch = 1 + Abs(m_vessel->Speed()) * 0.05;

			if (pitch > m_attr->m_soundMaxPitch)
				pitch = m_attr->m_soundMaxPitch;

			if (pitch < m_attr->m_soundMinPitch)
				pitch = m_attr->m_soundMinPitch;

		    if (pitch != m_currentPitch)
		    {
		      m_currentPitch = pitch;
		      m_lpCE->SetPitch(m_currentPitch);
		    }
        }

     }
     carrierOnMove();
}
//-------------------------------------------------------------
int Vehicle::receiveEvent(KR_Event &event){
	int repeat;
        double down;
	//m_curTime = event.timeStamp;
	int code;
	int ctrlEvent;
	double timeStamp;
        int um, uj;
        double mx, my, jx, jy;
	
	switch( event.label )
	{
	case EV_VEHICLE_FIRE:
		{
			int isPrim;
			event.data.open(EDO_READ)
				.getInt(isPrim)
				.close();
			if(  isPrim  )
			{
				m_primEnable = true;
				if(  m_firePrimPress  )
				{
					m_firePrim = true;
					onFire( event.timeStamp );
					event.timeStamp += m_attr->m_bulletSlipTime;
					issueEvent( event );
				}
				else m_firePrim = false;
			}
			else 
			{
				m_secEnable = true;
				if(  m_fireSecPress )
				{
					m_fireSec = true;
					if(  m_secBulletCnt>0  )
						onFireSec( event.timeStamp  );
					event.timeStamp += m_attr->m_bulletSecSlipTime;
					issueEvent( event );
				}
				else m_fireSec = false;
			}
		}
		break;
		

		
	case KR_SET_ATTR:
		setAttr(event);
		break;
	case VEHICLE_UPDATE_POS: 
		//UpdatePos();
		break;
		
	case EV_VEHICLE_POINT0:  g_vehicle->Restart(); g_vehicle->SetPos(g_vp[0].vp);  break;
	case EV_VEHICLE_POINT1:  g_vehicle->Restart(); g_vehicle->SetPos(g_vp[1].vp);  break;
	case EV_VEHICLE_POINT2:  g_vehicle->Restart(); g_vehicle->SetPos(g_vp[2].vp);  break;
	case EV_VEHICLE_POINT3:  g_vehicle->Restart(); g_vehicle->SetPos(g_vp[3].vp);  break;
	case EV_VEHICLE_POINT4:  g_vehicle->Restart(); g_vehicle->SetPos(g_vp[4].vp);  break;
	case EV_VEHICLE_POINT5:  g_vehicle->Restart(); g_vehicle->SetPos(g_vp[5].vp);  break;
	case EV_VEHICLE_POINT6:  g_vehicle->Restart(); g_vehicle->SetPos(g_vp[6].vp);  break;
	case EV_VEHICLE_POINT7:  g_vehicle->Restart(); g_vehicle->SetPos(g_vp[7].vp);  break;
	case EV_VEHICLE_POINT8:  g_vehicle->Restart(); g_vehicle->SetPos(g_vp[8].vp);  break;
		
	case EV_VEHICLE_SETTAXI: onSetTaxi(event); break;
		
	case CTRL_CHAR: break;	
		
	case CTRL_BUTTONS_MSG:
	case CTRL_MOUSE_MOVE_MSG:
	case CTRL_JOYSTICK_MOVE_MSG:{
			timeStamp = event.timeStamp;		
                        ctrlEvent = -1;
                        if(timeStamp < m_lastTime) timeStamp = m_lastTime;
                        m_vessel->AccumPreStep(timeStamp-m_lastTime);
                        m_lastTime = timeStamp;

                        switch(event.label){
                           case CTRL_BUTTONS_MSG:
                              event.data.open(EDO_READ)
                                           .getInt(ctrlEvent)                    
                                           .getDouble(down)
                                           .getInt(code)
                                           .getInt(repeat)
                                        .close();
                              break;  
                           case CTRL_MOUSE_MOVE_MSG:
                              event.data.open(EDO_READ)
                                           .getInt(um)
                                           .getDouble(mx)
                                           .getDouble(my)
                                        .close();
                              if(um) m_vessel->MoveMouse(mx, my);
                              break;
                           case CTRL_JOYSTICK_MOVE_MSG:
                              event.data.open(EDO_READ)
                                         .getInt(uj)
                                         .getDouble(jx)
                                         .getDouble(jy)
                                        .close();
                              if(uj) m_vessel->Joystick(jx, jy);
                              break;
                           default: s_ASSERTNQ("");
                        }
                        if(ctrlEvent == -1) break;

		switch(ctrlEvent){
		case MOVE_FORWARD:		if (!m_dead) m_vessel->Throttle(down);  
			break;
		case MOVE_BACKWARD:		if (!m_dead) m_vessel->Throttle(-down); 
			break;
		case STRAFE_LEFT:		if (!m_dead) m_vessel->StrafeHorz(-down);
            break;
		case STRAFE_RIGHT:		if (!m_dead) m_vessel->StrafeHorz(down);
            break;
		case STRAFE_UP:			if (!m_dead) m_vessel->StrafeVert(down);
            break;
		case STRAFE_DOWN:		if (!m_dead) m_vessel->StrafeVert(-down);
            break;
		case LOOK_UP:			m_vessel->Raise(-down);  break;
		case LOOK_DOWN:			m_vessel->Raise(down);  break;
		case TURN_LEFT:			m_vessel->Incline(-down); break;
		case TURN_RIGHT:		m_vessel->Incline(down); break;
		case ROLL_LEFT:			m_vessel->Rotate(-down); break;
		case ROLL_RIGHT:		m_vessel->Rotate(down);  break;
		case TURRET_LEFT:		break;
		case TURRET_RIGHT:		break;
		case STRAFE:			if (!m_dead) 
									m_vessel->SetStrafe(down);	
			break;
			
		case FIRE_PRIMARY:		if (!m_dead) 
								{
                                    m_firePrimPress = (down!=0.0);
									if(  m_firePrimPress && m_primEnable  )
										if(  !m_firePrim  )
										{
											m_firePrim = true;
											event.label = EV_VEHICLE_FIRE;
											event.data.open(EDO_WRITE).putInt(1).close();
											issueEvent( event );
										}
								}
			break;
			
		case FIRE_SECONDARY:	if (!m_dead) 
								{
									m_fireSecPress = (down!=0.0);
									if(  m_fireSecPress && m_secEnable  )
										if(  !m_fireSec  )
											if(  m_secBulletCnt > 0  )
											{
												m_fireSec = true;
												event.label = EV_VEHICLE_FIRE;
												event.data.open(EDO_WRITE).putInt(0).close();
												issueEvent( event );
											}
								}
			break;
			
		case CHANGE_WEAPON:		break;
		case WEAPON0:			break;
		case WEAPON1:			break;
		case WEAPON2:
		case WEAPON3:
		case WEAPON4:
		case WEAPON5:
		case WEAPON6:
		case WEAPON7:
		case WEAPON8:
		case WEAPON9:
		case LOCK_TAGERT: break;
		case JUMP:        m_vessel->Jump(down); break;
		case CROUCH:      break;
		case FORCEAGE:		if (!m_dead) m_vessel->SetForceage(down); break;
		case VIEW_CABINE:
		case VIEW_EXTERN_FIXED:
		case VIEW_EXTERN_CLEAVER:
		case VIEW_LAND_FIXED:
		case VIEW_LAND_CLEAVER:
		case VIEW_NEXT_WINGMAN:
		case VIEW_PREV_WINGMAN:
		case VIEW_WINGMAN:
		case VIEW_NEXT_OBJECT:
		case VIEW_PREV_OBJECT:
		case VIEW_OBJECT:
		case VIEW_OBSERVER:
		case CENTER_VIEW:
		case TOGGLE_FOLLOW_SCOUT:
		case LEAVE_VEHICLE:
		case GET_VEHICLE:
		case ZOOM_IN:
		case ZOOM_OUT:
		case BRF_BEGIN_RECORD:
		case BRF_END_RECORD:
		case BRF_ADD_CTRL_POINT:
		case BRF_ADD_POS:
		case BRF_SAVE_FLIGHT:
		case BRF_SAVE_POS:
		case EXEC:
		case CONSOLE:
		case LIGHT:
                  break;

		case DROP_ARTEFACT:
                  {
                      if(  m_artefact  )
                      {
                           IArtefact *a = m_artefact;

                           carrierDropArtefact(event.timeStamp);

                           CFMatrix3x4 m;
                           getMatrix( m );
                           CFVector3   dir = -m.Column(2);
                           m.TranslateL( getPosition()+dir*(getRadius0()+1.5) );
                           a->moveTo( m );
                           a->artefactMove( dir*7 );
                      }
                  }
                  break;

		case TOGGLE_LIGHT:		  break;
		case STOP_VEHICLE:        if (!m_dead) 
									  if(down) m_vessel->Stop(); break;
		case CHANGE_VEHICLE:      if (!m_dead) 
									  if(down) onChangeVehicle(event.timeStamp); break;
									  
		default: return(0);
			}
			break;
		}
	case KR_WAKE_UP:break;

	case EV_VEHICLE_SURRENDER:
		{
		 IPlayer *pl= (IPlayer *)(g_vehicle->queryInterface(IPlayerIID));	
		 ASSERT(pl);

		 int nom = pl->getMissionCnt();

		 if (!nom)
		 {
		  g_GameConsole.PrintUrgent("Are you nuts?\\nYou are not given a mission", 20, GameConsole::CENTER);	
		  break;
		 } 

		 for (int i = 0; i < nom; i++)
		  pl->getMiss(i).m_status = MISSION_SURRENDER;
                 g_GameConsole.PrintUrgent("Surrender Granted\\nYou may now return to\\nthe recruit center or nevertheless\\ntry to complete the mission", 20, GameConsole::CENTER);	
		}
		break;
    	case EV_VEHICLE_MISSIONOK:
		{
		 IPlayer *pl= (IPlayer *)(g_vehicle->queryInterface(IPlayerIID));	
		 ASSERT(pl);

		 int nom = pl->getMissionCnt();

		 if (!nom)
		 {
		  g_GameConsole.PrintUrgent("Are you nuts?\\nYou are not given a mission", 20, GameConsole::CENTER);	
		  break;
		 }
		 
		for (int i = 0; i < nom; i++)
		  pl->getMiss(i).m_status = MISSION_SUCCESS;
                 g_GameConsole.PrintUrgent("Mission complete", 20, GameConsole::CENTER);	
		}
		break;

        case EV_VEHICLE_SKIPTIME:
             SkipTime();
             break;

	default: 
                 return  carrierReceiveEvent(event);
    }
    return(1);
}

void Vehicle::SkipTime(CFVector3 p,double t)
{
  m_skipPos  = p;
  m_skipTime = t;
}

void Vehicle::SkipTime()
{
  if(  m_skipTime>0  )
  {
       Stop();
       SUA_SkipTime(m_skipTime);
       SetPos(m_skipPos);
       Stop();
       m_skipTime = -1;
  }
}

//-------------------------------------------------------------
