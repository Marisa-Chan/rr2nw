#ifndef __PLAYER_I__INCLUDED
#define __PLAYER_I__INCLUDED

#include "kernel/h/setofid.h"


enum
{
  IPlayerIID = 14
};

typedef enum
{
    MISSION_NONE,
    MISSION_INPROCESS,
    MISSION_SUCCESS,
    MISSION_FAILED,
    MISSION_SURRENDER,
} MISSION_STATUS;


// This stuff previously lived in dmap.h
typedef int TMissionId;

#define MAX_ROUTES       8
#define MAX_ROUTE_POINTS 128
#define MAX_TEXT_LEN     2048
#define MAX_MISSIONS     8


class PlayerMission
{
public:
    int         success_filed; // order
    KR_ObjectID mID, comID;
	
	TMissionId	m_TMissionId;

	int			m_missionInfoExist;
	char		m_missionText[MAX_TEXT_LEN];
    char		m_missionName[80];
    double		m_missionsw,m_missionew;
    int			m_missionrgb;
    KR_ObjectID m_missionRouteID;

    KR_SetOfID  success_needKill;
    KR_SetOfID  success_needLive;
    KR_SetOfID  success_needReached;
    CFVector2   success_reachedPos[10];
    double      success_reachedRadius[10];

    KR_SetOfID  filed_needKill;
    KR_SetOfID  filed_needLive;
    KR_SetOfID  filed_needReached;
    CFVector2   filed_reachedPos[10];
    double      filed_reachedRadius[10];
	
	
    
	MISSION_STATUS m_status;

    PlayerMission() {}
    void startInitialize(KR_ObjectID oID, KR_ObjectID _comID)
    {
        success_filed = 1;
        m_status      = MISSION_NONE;

        mID   = oID;
        comID = _comID;
        success_needKill.clr();
        success_needLive.clr();
        success_needReached.clr();

        filed_needKill.clr();
        filed_needLive.clr();
        filed_needReached.clr();
    }
};


class IPlayer
{
public:
   virtual double &damageDiff      (KR_ObjectID commander) = 0;
   virtual int    &isRenegat       (KR_ObjectID commander) = 0;
   virtual int    &isMissionSuccess(KR_ObjectID commander) = 0;
   virtual double &getDamage       ()                     = 0;
   virtual void    resetPlayer     ()                     = 0;
   virtual void    addCommander     (KR_ObjectID commander) = 0;
   virtual void    findCommander    ()                     = 0;
   virtual void    attackUnit      (KR_ObjectID commander,double damage) = 0;

   virtual int     getMissionCnt() const                  = 0;
   virtual KR_ObjectID getMission(int i)                  = 0;
   virtual int     addMission( KR_ObjectID mID, KR_ObjectID comID ) = 0;
   //virtual void    delMission( KR_ObjectID mID )          = 0;
   virtual void    CleanupMissionPool() = 0;
   virtual bool    doYouWant ( KR_ObjectID mID )          = 0;
   virtual PlayerMission &getMiss(int i)                  = 0;
   virtual void	   BetrayFor    (KR_ObjectID commander)   = 0;
   virtual void	   SetHostility (KR_ObjectID commander)   = 0;
};

#endif
