#ifndef	__MAIN_CPP__
#include"_main.h"
#endif

#define	_SUAVIK	1
#define VIEW_ROUTE 0

#if _SUAVIK
#include"suavik.h"
#else
#define	SUA_InitEverything()	((void)0)
#define	SUA_DeinitEverything()	((void)0)
#define PIN_InitEverything()    ((void)0)
#define PIN_DeInitEverything()  ((void)0)
#define	SUA_BeginRender(SCENE,LIST)	((void)0)
#define	SUA_EndRender(SCENE)	((void)0)
#define	SUA_ProcessEvents()	((void)0)
#endif

#include "zav.h"
#include "hardware.h"
#include "briefing.h" // Green
#include "dmap.h"
#include "menu.h"
#include "../arena/h/vehicle.h"
#include "kernel/h/session.h"
#include "console.h"

extern double	m_fDeltaT;
int mx = 10, my = 10;
extern void VRectangle(int x, int y, int len, int h, int c, bool trans, int opasity, int transCoords);
extern void D3D_DrawZList();


#if VIEW_ROUTE
#include "viewRoute.h"
#endif


int __printInfo = 0;

void	VesselMonitor(int checkupdate)
{
	static	TCchar *szCfgName = "vessels.cfg";
	static	FILETIME	ft0;

	WIN32_FIND_DATA	finddata;
	HANDLE	h = FindFirstFile(szCfgName,&finddata);
	RTCHECK1(h!=INVALID_HANDLE_VALUE,"%s not found",szCfgName);
	FindClose(h);
	FILETIME &ft = finddata.ftLastWriteTime;
	if( checkupdate ) {
		LONG res = CompareFileTime(&ft,&ft0);
		RTCHECK1(res>=0,"%s time error",szCfgName);
		if( res == 0 ) return;
	}


	ft0 = ft;
	g_GameConsole.ReadVessels(szCfgName);
}

VOID CALLBACK CBVesselMonitor( HWND  hwnd, UINT  uMsg, UINT  idEvent, DWORD  dwTime )
{
	(void)hwnd;
	(void)uMsg;
	(void)idEvent;
	(void)dwTime;
	VesselMonitor(1);
}

void InstallVesselMonitor(int install=1)
{
	static UINT id;
	if( install ) {
		VesselMonitor(0);
		id = SetTimer(NULL,1,500,(TIMERPROC)CBVesselMonitor);
	}
	else
		KillTimer(NULL,id);
}

extern void GRSaveScrToBMP();


static CConfigFile	*pGlobalConfig = NULL;

void PIN_InitGlobalConfig()
{
   pGlobalConfig = new CConfigFile("game.cfg");
}

CConfigFile	&GLOBAL_Config()
{
	ASSERT(pGlobalConfig);
	return *pGlobalConfig;
}


char g_HDDir[260], g_CDDir[260];



//===========================================================================
//  Reads install paths from the registry key SOFTWARE\LOGOS\RR2
//  (restored from nw.exe).
//===========================================================================
bool InitHddCdPaths()
{
   HKEY  hSettings;
   DWORD type, size;
   
   GetCurrentDirectory(sizeof(g_HDDir), g_HDDir);   
   strcat(g_HDDir, "\\");
   strcpy(g_CDDir, g_HDDir);

   if (RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\LOGOS\\RR2", 0, KEY_ALL_ACCESS, &hSettings) != ERROR_SUCCESS)
      return 0;

   size = sizeof(g_HDDir);
   if (RegQueryValueEx(hSettings, "HDDir", NULL, &type, (LPBYTE)g_HDDir, &size) != ERROR_SUCCESS
       || type != REG_SZ)
   {
      RegCloseKey(hSettings);
      return 0;
   }
   g_HDDir[sizeof(g_HDDir) - 1] = '\0';

   size = sizeof(g_CDDir);
   if (RegQueryValueEx(hSettings, "CDDir", NULL, &type, (LPBYTE)g_CDDir, &size) != ERROR_SUCCESS
       || type != REG_SZ)
   {
      RegCloseKey(hSettings);
      return 0;
   }
   g_CDDir[sizeof(g_CDDir) - 1] = '\0';

   RegCloseKey(hSettings);
   return 1;
}



bool InitLevel()
{
	static char configLevelName[128];
	sprintf(configLevelName,"%d",ol_Level::m_levelNumber);
   	if (!ZAV_InitLevel(GLOBAL_Config()("Levels",configLevelName)))
	 return 0;

	SUA_InitEverything();
	GRPreLoadTextures();

	GRClearScreen(TRUE, GRFillColor(0,0,0));
	GRDumpScreen();
	GRSetPalette((byte*)paletteTranslator.Palette());

   ZAV_BeginLoop();
   g_briefing.ChangeResEvent();

	// Green BEGIN
   	if(ZAV_Config().GetInt("Briefing", "Play")){
     	 g_briefing.PlayBriefing(ZAV_Config()("Briefing", "Name"));
    	}
   	// Green END

	g_vehicle->Restart();
	g_vehicle->GetDir().LoadIdentity();


	CFVector3 v;
	int n = ZAV_Config()("Vessel","Init","%lg %lg %lg",&v.x,&v.y,&v.z);
	g_vehicle->SetPos(v);
	g_vehicle->openPanel(Session::m_moment);
	return 1;
}


#ifdef  __NT__
int WINAPI WinMain( HINSTANCE  hInst, HINSTANCE, LPSTR, int )
#else
int main()
#endif
{
	
   InitHddCdPaths();

   if (!ZAV_InitGraph(hInst))
	return 0;
   PIN_InitGlobalConfig();

   ol_Level::m_levelNumber = GLOBAL_Config().GetInt("Init", "StartLevel");

   if (GLOBAL_Config().GetInt("Debug", "Console"))
   {
    AllocConsole();
    freopen("CONOUT$","wt",stdout);
   }

   PIN_InitEverything();


   if (!InitLevel())
	return 0;

   if (GLOBAL_Config().GetInt("Debug", "VesselMonitor"))
    InstallVesselMonitor();
   else
    VesselMonitor(0);



   //sprintf(configLevelName,"%d",ol_Level::m_levelNumber);
   //if (!ZAV_InitLevel(GLOBAL_Config()("Levels",configLevelName)))
   // return 0;
   //PIN_InitEverything();
   //SUA_InitEverything();
   //GRPreLoadTextures();
   //GRClearScreen(TRUE, GRFillColor(0,0,0));
   //GRDumpScreen();
   //GRSetPalette((byte*)paletteTranslator.Palette());
   //g_briefing.ChangeResEvent(); // … ’Ž†!!!!!!!!!!!
   //g_vehicle->Restart();
   //g_vehicle->GetDir().LoadIdentity();
   /*{
		CFVector3 v;
		int n = ZAV_Config()("Vessel","Init","%lg %lg %lg",&v.x,&v.y,&v.z);
                g_vehicle->SetPos(v);//2060 70 -2390
	}

     g_vehicle->openPanel(Session::m_moment);*/


	for(;;) {
		WRITELOG("-------------------------------\n");
		CViewDynamicList list;
        if (_gr_bRestoreSurf) {
           GRRestoreSurfaces();
           _gr_bRestoreSurf = 0;
        }

	SUA_BeginRender(ZAV_Scene(),list);
	CFMatrix3x4	tdir = g_vehicle->GetDir();


        if (Vehicle::m_isTakingTaxiNow)
         Vehicle::transformMatrix(tdir);
        else
         tdir.TranslateR(-g_vehicle->Pos());

        if(!g_debugMap.IsActive())
	{
            ZAV_RenderFrame( &tdir,list);
            if (GRIsHardware()) D3D_DrawZList();
             g_vehicle->drawPanel();
        }
        else
	{
           g_debugMap.Draw();

           ///////////////////////////
           SGRViewport *oldV = GRGetViewport();

           float focus = g_debugMap.m_winDx*5./8;
           float fAspRatio = _gr_nScreenHeight*4./3./_gr_nScreenWidth;

           ZAV_Scene()->SetScale(focus,focus*fAspRatio);
           CViewObject::SetClipRect(g_debugMap.m_vPort->clipRect);
           GRSetViewport(g_debugMap.m_vPort);

           ZAV_RenderFrame( &tdir,list);
           if (GRIsHardware()) D3D_DrawZList();

           focus = _gr_nScreenWidth*5./8;
           //fAspRatio = _gr_nScreenHeight*4./3./_gr_nScreenWidth;
           ZAV_Scene()->SetScale(focus,focus*fAspRatio);
           CViewObject::SetClipRect(oldV->clipRect);
           GRSetViewport(oldV);
        }

		//ZAV_RenderFrame( NULL,list);
#if VIEW_ROUTE
                viewRoute();
#endif
        if (GRIsHardware()) D3D_DrawZList();
        if(!g_debugMap.IsActive()) g_vehicle->drawPanel();

		ZAV_PrintFrameInfo();

		//SUA_ProcessEvents();

		SUA_EndRender(ZAV_Scene());

		ZAV_EndRenderFrame();

		g_hardware.MessageLoop();
		g_vehicle->BeginPreStep();
		SUA_ProcessEvents();
		g_vehicle->UpdatePos();

		ZAV_NextFrame();

        if(g_menu.IsActive()) g_menu.Draw();
        g_GameConsole.Draw();
if(ol_Level::m_saveGameStatus != ol_Level::MST_NONE)
		{
		  switch(ol_Level::m_saveGameStatus)
		  {
			case ol_Level::MST_SAVE:
					SaveGame(ol_Level::m_saveGameName,g_menu.getContext());
					break;
			case ol_Level::MST_LOAD:
					if (ol_Level::m_levelToLoad != ol_Level::m_levelNumber)
					{
						ol_Level::m_levelNumber = ol_Level::m_levelToLoad;
						SUA_DeinitEverything();
						ZAV_DeInitLevel();
				        	if (!InitLevel())
					  		return 0;
					}
					LoadGame(ol_Level::m_saveGameName,g_menu.getContext());
					break;
			case ol_Level::MST_RESTART:

					SUA_DeinitEverything();
					ZAV_DeInitLevel();
					if (!InitLevel())
					  return 0;
					break;

		  }

		  ol_Level::m_saveGameStatus = ol_Level::MST_NONE;
		}
	}
}
