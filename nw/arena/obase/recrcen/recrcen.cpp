/*
 * File   : C:\NW\ARENA\OBASE\recrcen\Recrcen.cpp
 * Author : Suavik
 * Ver   1.0 
 */
#include "sc/h/sc.h"

#define LAST_H__SCENE
#include "game.h"
#include "Recrcen.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "kernel/h/session.h"
#include "message/recrcenmsg.h"
#include "message/unitmsg.h"
#include "../output/defs.h"
#include "mproj/h/mproj.h"
#include "briefing.h"
#include "../arena/h/vehicle.h"
#include "h/phisics.h"
#include "h/olevel.h"
#include "dmap.h"

#define RADIUS 4.0

static struct
{
    MISSION_STATUS m_status;
    int            m_isRenegat;
    double         m_lastVisitTime;
    double         m_damage;
    int            m_secBulletCnt;
    int            iAmBreak;
}
 s_curStatus;

 //===========================================================================
class RecruitCenterTable : public ct_SubjectTable
{
 private:
    RecruitCenter *m_table;
 public:
    RecruitCenterTable()
    {
        m_table = NULL;
        registerClass( "RecruitCenter" );
    }
    ~RecruitCenterTable()
    {
        delete [] m_table;
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static RecruitCenterTable  __classTable;

CFMatrix3x4 RecruitCenter::dummy;
 /*********************************
  *
  *   RecruitCenter implementation
  *
  *********************************/

 //============================================================
RecruitCenter::RecruitCenter()
 {
 }

 //============================================================
RecruitCenter::~RecruitCenter()
 {
 }

int success(PlayerMission *p,SimulationContext *context)
{
    int def = 0;
    int i;
    if(  p==0  )
         return 0;

    for( i = 0; i < p->success_needKill.getCount(); ++i )
    {
         def = 1;
         if(  context->isExist(p->success_needKill[i])  )
              return 0;
    }

    for( i = 0; i < p->success_needLive.getCount(); ++i )
    {
         def = 1;
         if(  !context->isExist(p->success_needLive[i])  )
              return 0;
    }

    for( i = 0; i < p->success_needReached.getCount(); ++i )
    {
         IDynamicObject *dobj = 
           (IDynamicObject *)(context->queryInterface(p->success_needReached[i],
                     IDynamicObjectIID));
         if(  dobj!=0  )
         {
              CFVector3 pos = dobj->getPos();
              if(  Abs2(CFVector2(pos.x,pos.z)-p->success_reachedPos[i]) <=
                     p->success_reachedRadius[i]*p->success_reachedRadius[i]  )
                   return 1;
         }
    }

    return def;
}

int filed(PlayerMission *p,SimulationContext *context)
{
    int def = 0;
    int i;

    if(  p==0  )
         return 0;

    for( i = 0; i < p->filed_needKill.getCount(); ++i )
    {
         def = 1;
         if(  context->isExist(p->filed_needKill[i])  )
              return 0;
    }

    for( i = 0; i < p->filed_needLive.getCount(); ++i )
    {
         def = 1;
         if(  !context->isExist(p->filed_needLive[i])  )
              return 0;
    }

    for( i = 0; i < p->filed_needReached.getCount(); ++i )
    {
         IDynamicObject *dobj = 
           (IDynamicObject *)(context->queryInterface(p->filed_needReached[i],
                     IDynamicObjectIID));

         if(  dobj!=0  )
         {
              CFVector3 pos = dobj->getPos();
              if(  Abs2(CFVector2(pos.x,pos.z)-p->filed_reachedPos[i]) <=
                     p->filed_reachedRadius[i]*p->filed_reachedRadius[i]  )
                   return 1;
         }
    }

    return def;
}

 //============================================================
int RecruitCenter::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case rc_CHECK_MISSION:
            {
            int index;

            event.data.open(EDO_READ)
                        .getInt(index)
                      .close();

            IPlayer *pl= (IPlayer *)(g_vehicle->queryInterface(IPlayerIID));

            if(  pl!=0  )
            if(  index >=0 && index < pl->getMissionCnt() )
            {
                 PlayerMission *pm = &(pl->getMiss(index));

                 if(  pm->success_filed   )
                 {
                      if(  success(pm,context)  )
                      {
                           echo("!!!!!!!!!!!\n     OK\n!!!!!!!!!!!!!!!");
                           pl->getMiss(index).m_status = MISSION_SUCCESS;
                           g_GameConsole.PrintUrgent("Mission complete", 20, GameConsole::CENTER);
                      }
                      else
                      if(  filed(pm,context) )
                      {
                           //pl->delMission(pm->mID);
                           echo("###########\n     PLOHO\n############");
                           pl->getMiss(index).m_status = MISSION_FAILED;
                           g_GameConsole.PrintUrgent("Mission failed", 20, GameConsole::CENTER);
                      }
                      else
                      {
                           event.timeStamp += 10;
                           issueEvent(event);
                      }
                 }
                 else
                 {
                      if(  filed(pm,context) )
                      {
                           echo("###########\n     PLOHO\n############");
                           pl->getMiss(index).m_status = MISSION_FAILED;
                           g_GameConsole.PrintUrgent("Mission failed", 20, GameConsole::CENTER);
                      }
                      else
                      if(  success(pm,context)  )
                      {
                           echo("!!!!!!!!!!!\n     OK\n!!!!!!!!!!!!!!!");
                           pl->getMiss(index).m_status = MISSION_SUCCESS;
                           g_GameConsole.PrintUrgent("Mission complete", 20, GameConsole::CENTER);
                      }
                      else
                      {
                           event.timeStamp += 10;
                           issueEvent(event);
                      }
                 }
            }
            }
            break;

    case KR_WAKE_UP:
            break;

    case t_EV_SET_ATTR_POS:
      {
            CFVector3 pos;
            m_myCommander[0] = 0;

            event.data.open( EDO_READ )
                    .getDouble(pos.x)
                    .getDouble(pos.y)
                    .getDouble(pos.z)
                    .getStr   (m_myCommander,sizeof(m_myCommander))
					.getStr   (m_defaultBriefing,sizeof(m_defaultBriefing))
                 .close();
            setPosition(pos);
            m_comID = context->searchObject(m_myCommander);
       }
            break;

    case t_EV_ONCOLLISION:
		{
			
			IPlayer *pl= (IPlayer *)(g_vehicle->queryInterface(IPlayerIID));
			
			if (pl->isRenegat(m_comID))
			{
				bool oldStat = g_vehicle->m_playedBrief;
				g_vehicle->m_playedBrief = true;
				g_briefing.PlayBriefing(m_defaultBriefing);
				g_vehicle->m_playedBrief = oldStat;
				pl->BetrayFor(m_comID);	
			}
			
			pl->SetHostility (m_comID);
			
		}// No break here

	case rc_NEW_MISSION:
            if(  !m_working  )
            {
				m_working = 1;
				IPlayer *pl= (IPlayer *)(g_vehicle->queryInterface(IPlayerIID));
				
				s_curStatus.m_status        = MISSION_INPROCESS;
				
				s_curStatus.m_isRenegat     = pl->isRenegat(m_comID);
				
				if(  m_prevVisitTime < 0  )
					s_curStatus.m_lastVisitTime = 100;
				s_curStatus.m_lastVisitTime = event.timeStamp - m_prevVisitTime;
				s_curStatus.m_damage        = pl->getDamage();
				s_curStatus.m_secBulletCnt  = g_vehicle->m_secBulletCnt;
				
				m_prevVisitTime = event.timeStamp;
				
				KR_ObjectID prj = chooseProject(event.timeStamp);
				
				if(  !prj.isNUL()  )
				{

					// Cleanup missions
					// remove successful or failed or surrendered
					// missions from there
					
					pl->CleanupMissionPool();

					int index = runProject(prj,event.timeStamp);
					event.label = rc_CHECK_MISSION;
					event.timeStamp += 20;
					event.source     = getObjectID();
					event.data.open(EDO_WRITE)
						.putInt(index)
						.close();
					issueEvent( event );
				}
				
				g_vehicle->Restart();                                  
				g_vehicle->SetPos(m_position+m_eject);
				g_vehicle->Stop();
                m_working = 0;
            }
            break;

    case rc_SET_EJECT:
            event.data.open( EDO_READ )
                    .getDouble(m_eject.x)
                    .getDouble(m_eject.y)
                    .getDouble(m_eject.z)
                 .close();

            break;

    default: return 0;
    }
    return 1;
 }

 //============================================================
PlayerMission *searchMission(IPlayer*p,KR_ObjectID comID)
{
    for( int i = 0; i < p->getMissionCnt(); ++i)
    {
         PlayerMission &m = p->getMiss(i);
         if(  m.comID == comID  )
              return &m;
    }
    return 0;
}

 //============================================================
void DoMessage(double )
{
	KR_ObjectID m = g_arena.getObjectID();
	int st = s_curStatus.m_status;
	
	if(  st == MISSION_SUCCESS )
	{
		if(  s_curStatus.m_lastVisitTime < 3*60  )
			g_GameConsole.PrintUrgent("Oh, You can work very quickly! ", 12, GameConsole::CENTER);
		else
			if(  s_curStatus.m_lastVisitTime > 15*60  )
			{
				if(  s_curStatus.m_damage < 0.3  )
					g_GameConsole.PrintUrgent("I see it was not easy for you ?", 12, GameConsole::CENTER);                   
				else 
					g_GameConsole.PrintUrgent("What the hell have you been ?", 12, GameConsole::CENTER);
			}
			
			if(  s_curStatus.m_isRenegat  )
				g_GameConsole.PrintUrgent("You use nasty methods but do your job well", 12, GameConsole::CENTER);               
			else 
				g_GameConsole.PrintUrgent("Well done, great job", 12, GameConsole::CENTER);
			
	}
	else
		if( st == MISSION_FAILED  || st == MISSION_SURRENDER)
		{
			if(   s_curStatus.m_isRenegat  )
				g_GameConsole.PrintUrgent("You did your best but failed! We're disappointed", 12, GameConsole::CENTER);              
			else 
				g_GameConsole.PrintUrgent("You're nuts! Go fight and proove your loyalty", 12, GameConsole::CENTER);
		}
		else
			if(  st == MISSION_INPROCESS  )
			{
				if(  s_curStatus.m_lastVisitTime < 13  )
				{
					g_GameConsole.PrintUrgent("WHAT?? You've just leaved! Come on fight!", 12, GameConsole::CENTER);           
					return;
				}
				else
				{
					if(  s_curStatus.m_isRenegat  )
						g_GameConsole.PrintUrgent("Can't you distinguish between allies and enemies?", 12, GameConsole::CENTER);                   
					else 
						g_GameConsole.PrintUrgent("War is money, we ain't philantropists", 12, GameConsole::CENTER);
				}
				
				g_GameConsole.PrintUrgent("You haven't yet completed the mission given", 12, GameConsole::CENTER);         
			}
}

 //============================================================
int handleMission( IPlayer*p,double ts, KR_ObjectID com )
{
    PlayerMission *m = searchMission(p,com);
    if(  m == 0  )
         return 0;

    s_curStatus.m_status = m->m_status;
    s_curStatus.iAmBreak = 0;
    DoMessage(ts);

    if(  m->m_status == MISSION_INPROCESS  )
    {

         if(  g_vehicle->m_secBulletCnt < g_levelAttr.m_minSecBulletCnt  )
         {
		g_GameConsole.PrintUrgent("You haven't enough ammo, I've given you more", 12, GameConsole::CENTER);              
                g_vehicle->m_secBulletCnt = g_levelAttr.m_minSecBulletCnt;
         }
         if(  g_vehicle->getDamage() < g_levelAttr.m_minDamage  )
         {
		g_GameConsole.PrintUrgent("Your vehicle was broken, we fixed it a little", 12, GameConsole::CENTER);              
                g_vehicle->setDamage( -g_levelAttr.m_minDamage,g_vehicle->getPosition(),ts, KR_ObjectID::NUL() );
         }
         return 1;
    }
    else
    if(  m->m_status == MISSION_SUCCESS  )
    {
         g_vehicle->setDamage( g_levelAttr.m_minDamage-1,g_vehicle->getPosition(),ts, KR_ObjectID::NUL() );
         if(  g_vehicle->m_secBulletCnt < g_levelAttr.m_maxSecBulletCnt  )
              g_vehicle->m_secBulletCnt = g_levelAttr.m_maxSecBulletCnt;

    }
    return 0;
}

 //============================================================
static KR_ObjectID retPrjID;
bool searchProjectCallBack(KR_ObjectID oID,void*rc)
{
  RecruitCenter &self = *((RecruitCenter*)(rc));
  retPrjID = KR_ObjectID::NUL();

  if( !mp_IsCommanderEqu( projectTable, 
                    projectTable.getProjectRoot( oID ), 
                    self.m_myCommander ) )
      return true;

  if( self.m_player->doYouWant(oID)  )
  {
      retPrjID = oID;
      return false;
  }
  return true;
}

 //============================================================
KR_ObjectID RecruitCenter::chooseProject(double ts)
{
    m_player = (IPlayer*)(g_vehicle->queryInterface(IPlayerIID));

    if(  !handleMission(m_player,ts,m_comID)  )
         projectTable.userFind(searchProjectCallBack,(void*)this);
    else return KR_ObjectID::NUL();

    return retPrjID;
}

 //============================================================
int    RecruitCenter::runProject   (KR_ObjectID  prjID,double ts)
{
	if(  prjID.isNUL()  )
        return -1;
		
	IPlayer *pl= (IPlayer *)(g_vehicle->queryInterface(IPlayerIID));
	int index = -1;
	PlayerMission *pm = 0;
	
	if(  pl!=0  )
	{
        index = pl->addMission(prjID,m_comID);
        if(  index>=0  )
            pm = &(pl->getMiss(index));
	}
	
	
	mp_NodeNum pNULL = mp_NodeNULL();
	for(
		mp_NodeNum  pn = projectTable.getProjectRoot( prjID );
	pn != pNULL;
	pn = projectTable.getRight( pn )
		)
	{
		
		char className[40],
			oName[80],
			incubName[40],
			attrName[40];
		double after=0;
		CFVector2 pos;
		double    radius;
		int       index = 0;
		
		switch( projectTable.getCommand( pn ) )
		{
		case COM_SUCCESS_KILL:
			if(  pm!=0  ) 
			{
				mp_OpenData   ( projectTable, pn, EDO_READ );
				mp_ReadStr    ( projectTable, pn, oName );
				mp_CloseData  ( projectTable, pn);
				pm->success_needKill.add( context->searchObject(oName) );
			}
			break;
			
		case COM_SUCCESS_LIVE:
			if(  pm!=0  ) 
			{
				mp_OpenData   ( projectTable, pn, EDO_READ );
				mp_ReadStr    ( projectTable, pn, oName );
				mp_CloseData  ( projectTable, pn);
				pm->success_needLive.add( context->searchObject(oName) );
			}
			break;
			
		case COM_SUCCESS_REACHED:
			if(  pm!=0  ) 
			{
				mp_OpenData   ( projectTable, pn, EDO_READ );
				mp_ReadStr    ( projectTable, pn, oName );
				mp_ReadFloat  ( projectTable, pn, pos.x );
				mp_ReadFloat  ( projectTable, pn, pos.y );
				mp_ReadFloat  ( projectTable, pn, radius );
				mp_CloseData  ( projectTable, pn);
				if( pm->success_needReached.add( context->searchObject(oName) ) )
				{
					index = pm->success_needReached.getCount()-1;
					if(  index >= 0 && index < 10  )
					{
						pm->success_reachedPos[index]    = pos;
						pm->success_reachedRadius[index] = radius;
					}
				}
			}
			break;
			
		case COM_FILED_KILL:
			if(  pm!=0  ) 
			{
				mp_OpenData   ( projectTable, pn, EDO_READ );
				mp_ReadStr    ( projectTable, pn, oName );
				mp_CloseData  ( projectTable, pn);
				pm->filed_needKill.add( context->searchObject(oName) );
			}
			break;
			
		case COM_FILED_LIVE:
			if(  pm!=0  ) 
			{
				mp_OpenData   ( projectTable, pn, EDO_READ );
				mp_ReadStr    ( projectTable, pn, oName );
				mp_CloseData  ( projectTable, pn);
				pm->filed_needLive.add( context->searchObject(oName) );
			}
			break;
			
		case COM_FILED_REACHED:
			if(  pm!=0  ) 
			{
				mp_OpenData   ( projectTable, pn, EDO_READ );
				mp_ReadStr    ( projectTable, pn, oName );
				mp_ReadFloat  ( projectTable, pn, pos.x );
				mp_ReadFloat  ( projectTable, pn, pos.y );
				mp_ReadFloat  ( projectTable, pn, radius );
				mp_CloseData  ( projectTable, pn);
				if( pm->filed_needReached.add( context->searchObject(oName) ) )
				{
					index = pm->filed_needReached.getCount()-1;
					if(  index >= 0 && index < 10  )
					{
						pm->filed_reachedPos[index]    = pos;
						pm->filed_reachedRadius[index] = radius;
					}
				}
			}
			break;
			
		case COM_SKIP_WAY:
			{
				double time;
				CFVector3 p;
				
				mp_OpenData   ( projectTable, pn, EDO_READ );
				mp_ReadFloat  ( projectTable, pn, p.x );
				mp_ReadFloat  ( projectTable, pn, p.y );
				mp_ReadFloat  ( projectTable, pn, p.z );
				mp_ReadFloat  ( projectTable, pn, time );
				mp_CloseData  ( projectTable, pn);
				Session();
				g_vehicle->SkipTime(p,time);
			}
			break;
			
		case COM_SUCCESS_FILED:
			if(  pm!=0  ) 
				pm->success_filed = 1;
			break;
			
		case COM_FILED_SUCCESS:
			if(  pm!=0  ) 
				pm->success_filed = 0;
			break;
			
			
		case COM_CREATE_UNITS:
			{
				mp_OpenData   ( projectTable, pn, EDO_READ );
				mp_ReadStr    ( projectTable, pn, className );
				mp_ReadStr    ( projectTable, pn, oName );
				mp_ReadFloat  ( projectTable, pn, after );
				mp_ReadStr    ( projectTable, pn, attrName );
				mp_ReadStr    ( projectTable, pn, incubName );
				mp_CloseData  ( projectTable, pn);
				
				KR_Event  event;
				event.label       = rc_CREATE;
				event.source      = getObjectID();
				event.destination = context->searchObject( incubName );
				event.timeStamp   =  ts + after;
				event.data.open(EDO_WRITE)
					.putObjectID( context->searchObject( attrName  ) )
					.putInt     ( g_arena.searchSeanceClassTable( className ) )
					.putStr     ( oName )
					.close();
				issueEvent( event );
			}
			break;
			
		case COM_PLAY_BRIEFING:
			mp_OpenData   ( projectTable, pn, EDO_READ );
			mp_ReadStr    ( projectTable, pn, oName );
			mp_CloseData  ( projectTable, pn);
			{
				bool oldStat = g_vehicle->m_playedBrief;
				g_vehicle->m_playedBrief = true;
				g_briefing.PlayBriefing(oName);
				g_vehicle->m_playedBrief = oldStat;
			}
			break;
			
		case COM_PLAY_BRIEFING_MSG:
			{
				KR_Event  event;
				
				mp_OpenData   ( projectTable, pn, EDO_READ );
				mp_ReadStr    ( projectTable, pn, oName );
				mp_ReadInt	 ( projectTable, pn, event.label );
				mp_CloseData  ( projectTable, pn);
				
				bool oldStat = g_vehicle->m_playedBrief;
				g_vehicle->m_playedBrief = true;
				g_briefing.PlayBriefing(oName);
				g_vehicle->m_playedBrief = oldStat;
				
				event.source      = getObjectID();
				event.destination = context->searchObject("Publisher");
				event.timeStamp   = Session::m_moment;
				issueEvent( event );		   
			}
			break;
			
		case COM_SET_MISSION_SUMMARY:
			{						
				mp_OpenData   ( projectTable, pn, EDO_READ );
				
				mp_ReadStr    ( projectTable, pn, pm->m_missionName);        

				pm->m_TMissionId = g_debugMap.CreateMission(pm->m_missionName);	
				pm->m_missionInfoExist = 1;
				
				mp_ReadStr    ( projectTable, pn, pm->m_missionText);
				g_debugMap.AddMissionText(pm->m_TMissionId, pm->m_missionText, "Font.fnt16x16.fnt");
				
				
				char missionRoute[80];
				
				mp_ReadStr    ( projectTable, pn, missionRoute);
				mp_ReadInt    ( projectTable, pn, pm->m_missionrgb);
				mp_ReadFloat  ( projectTable, pn, pm->m_missionsw );
				mp_ReadFloat  ( projectTable, pn, pm->m_missionew );
				
				pm->m_missionRouteID = g_arena.newObject("Route",missionRoute);
				if( pm->m_missionRouteID.isNUL()  )
				{
					echo("Cannot create route");
				}
				else
				{
					IRouteObject * iroute = (IRouteObject * )
					context->queryInterface(pm->m_missionRouteID,IRouteObjectIID);
					VERIFY(iroute);
					iroute->Load(missionRoute);
					g_debugMap.AddMissionRoute(pm->m_TMissionId, iroute, pm->m_missionrgb, pm->m_missionsw, pm->m_missionew);        
				}
				
				mp_CloseData  ( projectTable, pn);				
			}
			break;
			
		case COM_RUN_SCRIPT: //=================================================
			{
				TSuaCript suacript;
				TSetOfProcess sop;
				int        scStackSize     = 200,
					scWordBuffSize  = 1024*8,
					scStringBuffSize= 1024*8,
					scNameCnt       = 1024*2,
					scTreeBuffSize  = 1024*64,
					scCodeStreamSize= 1024*128,
					scLinkInfoSize  = 1024*30,
					scProgNameBuffSize=256,
					scMaxProgrammCnt  = 1;
				
				mp_OpenData   ( projectTable, pn, EDO_READ );
				mp_ReadStr    ( projectTable, pn, oName );
				mp_CloseData  ( projectTable, pn);
				
				if( SUACRIPT_REGISTER_ERROR_HANDLE(suacript) )
				{
					RTCHECK(0,suacript.m_heap.m_error.m_msg);
					return -1;
				}
				
				if( !sc_InitTSetOfProcess( &sop,
					scStackSize,   // stack
					1 ) )  // process size
				{
					echo("RecruitCenter::openLevel: Can't initialize process\n");
					break;
				}
				
				
				if( !sc_InitTSuaCript( &suacript,
					/* word buff size */    scWordBuffSize,
					/*string buff size */   scStringBuffSize,
					/* name cnt        */   scNameCnt,
					/* tree buff size  */   scTreeBuffSize,
					/* code stream size */  scCodeStreamSize,
					/* link info size   */  scLinkInfoSize,
					/* prog name buff size*/scProgNameBuffSize,
					/* max programm cnt */  scMaxProgrammCnt ) )
				{
					echo("RecruitCenter:: Can't initialize suacript\n");
					break;
				}
				
				
				/*****************************
				*
				*    Load and compile
				*
				*****************************/
				
				FILE *f = fopen( oName,"rb" );
				if( f==NULL )
				{
					echo("RecruitCenter::COM_RUN_SCRIPT: Can't open file %s\n",
						oName );
					break;
				}
				sc_InitScannerFromFile( &suacript, f );
				sc_Compile(
					&suacript,
					oName,
					externLinkTable,
					constExternLinkTable
					);
				
				fclose(f);
				
				if( !sc_CreateProcess( &sop,
					&suacript,
					sc_ProgrammId(
					&suacript,
					oName
					),
					200,
					32000,
					constExternLinkTable ) )
				{
					echo("RecruitCenter::openLevel: Can't create process\n");
					break;
				}
				
				while( !sc_RunProcess( &sop, 1, &g_arena ) );
				
				sc_DeleteTSuaCript( &suacript );
			}
			break;
   }
   
   }
   return index;
}

 //============================================================
void RecruitCenter::addNotify()
 {
    ct_Subject::addNotify();
    // insert your code this
    m_eject  = CFVector3(0,0,0);
    m_working = 0;
    m_prevVisitTime = -1;
    //m_missionExist = 0;
 }

 //============================================================
void RecruitCenter::removeNotify()
 {
    ct_Subject::removeNotify();
    // insert your code this
 }

 //============================================================
void RecruitCenter::draw()
 {
 }

 /*************************************
  *
  *   RecruitCenterTable implementation
  *
  *************************************/

 //============================================================
void RecruitCenterTable::allocObjects( int objectQnty )
 {
    m_table = new RecruitCenter[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void RecruitCenterTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *RecruitCenterTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"RecruitCenterTable::getObjectPTR");
    return &(m_table[ index ]);
 }


CFVector3 RecruitCenter::realPosition()
{
   return getPosition();
}


   /*********************************
    *
    *  Dynamic interface
    *
    *********************************/

void      *RecruitCenter::queryInterface( int interNum )
{
    switch( interNum )
    {
    case IUnknownIID:       return (KR_Object*)this;
    case IDynamicObjectIID: return (IDynamicObject*)this;
    }

    return 0;
}

// ----- interface IDynamicObject
CFVector3  RecruitCenter::getPos      ()
{
    return getPosition();
}

double     RecruitCenter::getHAngle   ()
{
     return 0;//FIXME
}

CFVector3  RecruitCenter::getUpVector ()
{
     return CFVector3(0,1,0); //
}

CFVector3  RecruitCenter::getCenter   () // +ªýþ¸øªõû¹ýþ 0 þñ·õúªð
{
    return CFVector3(0,0,0);
}

double     RecruitCenter::getRadius   () // +ªýþ¸øªõû¹ýþ ¡õýª¨ð
{
    return RADIUS;
}

double     RecruitCenter::getRadius0  () // +ªýþ¸øªõû¹ýþ 0 þñ·õúªð
{
    return RADIUS;
}

CFVector3  RecruitCenter::getMoveDir  () // =ðÿ¨ðòûõýøõ ôòøöõýø 
{
    return CFVector3(0,0,1);
}

double     RecruitCenter::getMoveSpeed() // Túþ¨þ¸ª¹
{
    return 0;
}

void       RecruitCenter::getMatrix   ( CFMatrix3x4 &m )
{
    m.LoadIdentity();
}


 //************************************************************************
 //************************************************************************
 //************************************************************************



void s_SuccessStatus( TProcessContext *pc, void * )
 {
    SC_PARI(0) = (int)s_curStatus.m_status;
 }
void s_IsRenegat( TProcessContext *pc, void * )
 {
    SC_PARI(0) = s_curStatus.m_isRenegat;
 }
void s_LastVisitTime( TProcessContext *pc, void * )
 {
    SC_PARF(0) = s_curStatus.m_lastVisitTime;
 }
void s_Damage( TProcessContext *pc, void * )
 {
    SC_PARF(0) = s_curStatus.m_damage;
 }
void s_SecBulletCnt( TProcessContext *pc, void * )
 {
    SC_PARI(0) = s_curStatus.m_secBulletCnt;
 }

void s_Return( TProcessContext *, void * )
 {
    s_curStatus.iAmBreak = 1;
 }

/*void s_CreateMessage( TProcessContext *pc, void * )
{
   const char *str = (char *)&(pc->m_codePtr[SC_PARI(1)]);
   double time     = SC_PARF(0);
   createMessage(g_arena.getObjectID(),
      (char*)str, time,
      Session::m_moment);
} */


TLinkConstExtern externConst[] = {
// {"START_FARTING",           s_Const_START_FARTING,   0},
 { NULL   , NULL, 0 }
};


TLinkExtern externFunc[] = {
  {"s_SuccessStatus"    , s_SuccessStatus  , 0},
  {"s_IsRenegat"        , s_IsRenegat      , 0},
  {"s_LastVisitTime"    , s_LastVisitTime  , 0},
  {"s_Damage"           , s_Damage         , 0},
  {"s_SecBulletCnt"     , s_SecBulletCnt   , 0},
  {"s_Return"           , s_Return         , 0},
  //{"s_CreateMessage"    , s_CreateMessage  , 0},
  {NULL,NULL}
};



bool	RecruitCenter::dump(PIN_SaveFile & sf)
{
		if (!ct_Subject::dump(sf))
			return false;


		if (!sf.WriteData( (char *) & m_eject, sizeof(RecruitCenterData)  ))
			return false;

		return true;
}

bool	RecruitCenter::load(PIN_SaveFile & sf)
{
		if (!ct_Subject::load(sf))
			return false;

		if (!sf.GetData( (char *) & m_eject, sizeof(RecruitCenterData)  ))
			return false;
		
		return true;
}

void	RecruitCenter::loadNotify()
{
	ct_Subject::loadNotify(); 
/*	m_player = (IPlayer*)(g_vehicle->queryInterface(IPlayerIID));

	g_debugMap.ClearMissions();	
	
        if (m_missionExist)
	{
	 m_TMissionId = g_debugMap.CreateMission(m_missionName);	
         g_debugMap.AddMissionText(m_TMissionId, m_missionText, "Font.fnt16x16.fnt");
	 IRouteObject * iroute = (IRouteObject * )
	 context->queryInterface(m_missionRouteID,IRouteObjectIID);
	 VERIFY(iroute);
 	 g_debugMap.AddMissionRoute(m_TMissionId, iroute, m_missionrgb, m_missionsw, m_missionew);
	}*/
}


/* End of file C:\NW\ARENA\OBASE\recrcen\Recrcen.cpp */

