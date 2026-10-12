#include "storage/h/subject.h"
#include "player.h"
#include "h/olevel.h"
#include "kernel/h/echo.h"
#include "mproj/h/mproj.h"
#include "../output/defs.h"
#include "dmap.h"
#include "storage\h\savefile.h"
#include "i/commander.i"
#include "debugext.h"



bool Player::dump(PIN_SaveFile & sf)
{
	if (!sf.WriteData( (char *) m_playerStatus, sizeof(PlayerData)  ))
		return false;

	return true;
}

bool Player::load(PIN_SaveFile & sf)
{
	if (!sf.GetData( (char *) m_playerStatus, sizeof(PlayerData)  ))
		return false;

	return true;
}


void Player::loadNotify()
{
	// Walk through all missions and
	// set map mission infos

	g_debugMap.ClearMissions();	
	
   for( int i = 0; i < m_missCnt; ++i )
   {
        if(  m_mission[i].m_missionInfoExist)
		{			
			m_mission[i].m_TMissionId = g_debugMap.CreateMission(m_mission[i].m_missionName);	
			g_debugMap.AddMissionText(m_mission[i].m_TMissionId, m_mission[i].m_missionText, "Font.fnt16x16.fnt");
			IRouteObject * iroute = (IRouteObject * )
				g_arena.context->queryInterface(m_mission[i].m_missionRouteID,IRouteObjectIID);
			VERIFY(iroute);
			g_debugMap.AddMissionRoute(m_mission[i].m_TMissionId, iroute, m_mission[i].m_missionrgb, 
				m_mission[i].m_missionsw, m_mission[i].m_missionew);						
		}            
   }
}


PlayerStatus *Player::searchStatus(KR_ObjectID commander)
{
   for(int i = 0; i < m_sideQnty; ++i)
   {
       if(  commander==m_playerStatus[i].m_masterID  )
       {
            return &(m_playerStatus[i]);
       }
   }
   return 0;
}

double &Player::damageDiff      (KR_ObjectID commander)
{
   PlayerStatus *status = searchStatus(commander);
   if(  status != 0  )
   {
        return status->m_damageDiff;
   }
   s_ASSERTNQ("Player::damageDiff: Unknown commander");
   static double bed = 0;
   return bed;
}

int    &Player::isRenegat       (KR_ObjectID commander)
{
   PlayerStatus *status = searchStatus(commander);
   if(  status != 0  )
   {
        return status->m_isRenegat;
   }
   s_ASSERTNQ("Player::isRenegat: Unknown commander");
   static int bed = 0;
   return bed;
}

int    &Player::isMissionSuccess(KR_ObjectID commander)
{
   PlayerStatus *status = searchStatus(commander);
   if(  status != 0  )
   {
        return status->m_isMissionSuccess;
   }
   s_ASSERTNQ("Player::isMissionSuccess: Unknown commander");
   static int bed = 0;
   return bed;
}

double &Player::getDamage       ()                    
{
   return m_damage;
}

void    Player::resetPlayer     ()                    
{
   m_damage   = 1;
   m_sideQnty = 0;
}

void    Player::addCommander     (KR_ObjectID commander)
{
   if(  m_sideQnty < 5  )
   {
        PlayerStatus &status = m_playerStatus[m_sideQnty];
        m_sideQnty++;
        status.startInitialize();
        status.m_masterID = commander;
   }
}

bool g_findCommander(KR_ObjectID oID, void *pl)
{
    ((Player*)pl)->addCommander(oID);
    return 1;
}

void    Player::findCommander    () 
{
    m_sideQnty = 0;
    g_arena.userFind("Commander",g_findCommander,(void *)this);
}

void  Player::attackUnit(KR_ObjectID commander,double damage)
{
   for( int i = 0; i < m_sideQnty; ++i )
   {
        PlayerStatus &status = m_playerStatus[i];
        if(  commander==status.m_masterID  )
        {
             status.m_damageDiff -= damage*g_levelAttr.m_levelSubRatingScale;
             if(  status.m_damageDiff < g_levelAttr.m_minRating  )
                  status.m_damageDiff = g_levelAttr.m_minRating;
        }
        else 
        {
             status.m_damageDiff += damage*g_levelAttr.m_levelSubRatingScale;
             if(  status.m_damageDiff > g_levelAttr.m_maxRating )
                  status.m_damageDiff = g_levelAttr.m_maxRating;
        }
        status.m_isRenegat = status.m_damageDiff < g_levelAttr.m_renegatThreshold;
   }
}


void Player::constructor()
{
    startInitialize();
}

void Player::destructor()
{
}

int     Player::getMissionCnt() const
{
    return m_missCnt;
}

KR_ObjectID Player::getMission(int i)
{
   return m_mission[i].mID;
}

int Player::addMission( KR_ObjectID mID, KR_ObjectID comID )
{
    if(  m_missCnt<6  )
    {
         m_mission[m_missCnt].startInitialize(mID,comID);
         m_mission[m_missCnt].m_status = MISSION_INPROCESS;
		 m_mission[m_missCnt].m_missionInfoExist = 0;
         m_total_misCount++;
         return (m_missCnt+=1)-1;
    }
    return -1;
}

/*void Player::delMission( KR_ObjectID mID )
{
    int i;

    for( i = 0; i < 6; ++i )
    {
         if(  mID==m_mission[i].mID  )
         {

			  // Remove stuff from debug map
			  
			  if (m_mission[i].m_missionInfoExist)
			  {
				  g_debugMap.DeleteMission(m_mission[i].m_TMissionId);
			  }

              for(;i<6-1;++i)
                   m_mission[i] = m_mission[i+1];
              m_missCnt--;
              break;
         }
    }
}*/

bool Player::doYouWant ( KR_ObjectID mID )
{
   bool ok = true;
   mp_NodeNum pNULL = mp_NodeNULL();

   for( int i = 0; i < m_missCnt; ++i )
   {
        if(  m_mission[i].mID==mID  )
             return false;
   }

   for(
       mp_NodeNum  pn = projectTable.getProjectRoot( mID );
       pn != pNULL;
       pn = projectTable.getRight( pn )
   )
   {
       switch( projectTable.getCommand( pn ) )
       {
       case COM_MISSIONINFO:
        {
             int num = 0;

             mp_OpenData   ( projectTable, pn, EDO_READ );
             mp_ReadInt    ( projectTable, pn, num );
             mp_CloseData  ( projectTable, pn);
             if(  num > m_total_misCount  )
                  ok = false;
        }
        break;

       }
   } 

   return ok;
}

PlayerMission &Player::getMiss(int i)
{
   s_ASSERT(i>=0,"Player::getMiss()");
   return m_mission[i];
}


void Player::SetHostility (KR_ObjectID commander)
{	
	for(int i = 0; i < m_sideQnty; ++i)	
	{
		ICommander * icommander = (ICommander*)(g_arena.getContext()->queryInterface(commander,ICommanderIID));
		
		if (icommander->isHostile(m_playerStatus[i].m_masterID))
		{
			m_playerStatus[i].m_isRenegat = 1;
		}
	}

	// Fail All Missions for sides, where we are renegats

	for (int i = 0; i < m_missCnt; i++)
	{
		if ( isRenegat(m_mission[i].comID) )
			m_mission[i].m_status = MISSION_SURRENDER;
	}

}

void Player::CleanupMissionPool()
{

	echo("Player Cleanup. Number of missions = %d",m_missCnt);

	for (int i = 0; i < m_missCnt; i++)
	{
		if (   m_mission[i].m_status == MISSION_SURRENDER
			|| m_mission[i].m_status == MISSION_FAILED
			|| m_mission[i].m_status == MISSION_SUCCESS)
		{
			
			mp_Project * pj = projectTable.searchProject( m_mission[i].mID );
			VERIFY(pj);

			if (!pj->m_permanent)			
				g_arena.getContext()->removeObject(m_mission[i].mID);

			// Remove stuff from debug map			  
			if (m_mission[i].m_missionInfoExist)
				  g_debugMap.DeleteMission(m_mission[i].m_TMissionId);
			
			for (int j = i; j < m_missCnt - 1; j++)			
				m_mission[j] = m_mission[j+1];
			m_missCnt--;
		}
	}
}

void Player::BetrayFor (KR_ObjectID commander) 
{

   // First pass, players former friends 
   // become most terrible enemies

   for(int i = 0; i < m_sideQnty; ++i)
   {	 
	 if (m_playerStatus[i].m_isRenegat == 0) 
	 {
		 m_playerStatus[i].m_isRenegat = 1;

		 if (m_playerStatus[i].m_damageDiff > 0)
		 {
			 m_playerStatus[i].m_damageDiff = - m_playerStatus[i].m_damageDiff;			 
		 }

		 if (m_playerStatus[i].m_damageDiff > g_levelAttr.m_renegatThreshold)
			 m_playerStatus[i].m_damageDiff = g_levelAttr.m_renegatThreshold - 0.1;
	 }
   }

   // Second pass, become minimally friendly for
   // all of our new master's friends

   ICommander * icommander = (ICommander*)(g_arena.getContext()->queryInterface(commander,ICommanderIID));
//   VERIFYMSG(icommander,"Cannot find commander");

   for(int i = 0; i < m_sideQnty; ++i)
   {
	   if (icommander->isFriendly(m_playerStatus[i].m_masterID))
	   {
            m_playerStatus[i].m_damageDiff = g_levelAttr.m_minRating;
			m_playerStatus[i].m_isRenegat = 0;
	   }
   }  
}

