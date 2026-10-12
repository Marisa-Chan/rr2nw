#ifndef	__MAIN_CPP__
#include"_main.h"
#endif

#include<new.h>
#include<string.h>

#ifdef	MSDOS
#include<i86.h>
#endif

#ifdef	__sgi
#include<X11/keysym.h>
#endif

#include"zav.h"
#include "briefing.h" // Green
#include "hardware.h" // Green
#include "../arena/h/vehicle.h"

#include "registry.h"

#include "direct.h"

// --------------------------------------------------------
#ifdef	__DOS__
#define	_PROFILE	1
#else
#define _PROFILE	0
#endif

#if _PROFILE

extern "C" {
	dword	_prof_dwLowAddr;
	dword	_prof_dwAddrSpan;
	byte	_prof_nShiftStep;
	dword	_prof_dwExtraCnt;
	dword	_prof_dwExtraSegCnt;
	dword	*_prof_pdwTable;
	dword	_prof_dwLowAddrUnrel;
	dword	_prof_stack[20];
	void ProfileBase();
}

//extern "C" void _GR_FillTexturedScan(void *);

static dword BeginProfile(CConfigFile &config)
{
	for( int i = 0 ; i < sizeof(_prof_stack)/4 ;i++ )	_prof_stack[i] = 0;
	//FILE *f = fopen("profile.cfg","rt");
	if(!config("Debug","Profile")) {
		_prof_dwAddrSpan = 0;
		return 1;
	}
	printf("Reading profile settings\n");
	dword dwEnd, dwStep, dwTimer;
	RTCHECK(config("Debug","Profile","%X %X %X %X",&_prof_dwLowAddrUnrel,&dwEnd,&dwStep,
											&dwTimer)==4,"profile.cfg bad");
	//fclose(f);
	RTCHECK( dwEnd > _prof_dwLowAddrUnrel, "end < start");
	_prof_dwLowAddr = _prof_dwLowAddrUnrel + (dword)ProfileBase - 0x10;
	_prof_nShiftStep = -1;
	for( dword dw = dwStep ; dw ; dw >>= 1 ) _prof_nShiftStep++;
	RTCHECK((1<<_prof_nShiftStep) == dwStep, "Bad step");
	_prof_dwAddrSpan = ((dwEnd-_prof_dwLowAddrUnrel+dwStep-1)/dwStep)*dwStep;
	_prof_dwExtraCnt = _prof_dwExtraSegCnt = 0;
	dword dwSize = _prof_dwAddrSpan/dwStep;
	_prof_pdwTable = new dword[dwSize];
	for( dw = 0 ; dw < dwSize ; dw++ )	_prof_pdwTable[dw] = 0;
	return dwTimer;
}

static	void EndProfile()
{
	if( _prof_dwAddrSpan == 0 ) return;
	printf("Writing profile\n");
	FILE *f = fopen("profile.tbl","wb");
	RTCHECK(f,"profile.tbl not created");
	fwrite(&_prof_dwLowAddrUnrel,4,1,f);
	dword d;
	d = _prof_dwLowAddrUnrel+_prof_dwAddrSpan;
	fwrite(&d,4,1,f);
	d = 1<<_prof_nShiftStep;
	fwrite(&d,4,1,f);
	fwrite(&_prof_dwExtraCnt,4,1,f);
	fwrite(&_prof_dwExtraSegCnt,4,1,f);
	dword dwCnt = _prof_dwAddrSpan/(1<<_prof_nShiftStep);
	for( d = 0; d < dwCnt ; d++ )
		fwrite(&_prof_pdwTable[d],4,1,f);
	fclose(f);
}
#else
#define	BeginProfile(cn)	1
#define	EndProfile()	((void)0)
#endif	// PROFILE

dword	MemMax()
{
	#ifdef MSDOS
		REGS	rg;
		SREGS	sr;
		dword	buf[0x30],size;

		rg.w.ax = 0x500;
		sr.ds = 0;
		sr.es = FP_SEG((void far*)buf);
		rg.x.edi = FP_OFF((void far*)buf);
		int386x(0x31,&rg,&rg,&sr);
		size = buf[0];

		return size;
	#else
		return 0;
	#endif
}
// --------------------------------------------------------

static CUserCtlDefs	m_ctl;
double	m_fDeltaT, fTerrain0;
static	dword	m_dwPrevTime, dwTime0, dwFrames, nWhiteColor, dwMem0, dwMem1;
//static	ibool	bLimitMotion, bKeysDebug;



void	NoMemory()
{
	RTCHECK(0,"No Memory");
}

struct	SModeDef {
	bool bVESA2;
	int	 nMode;
	int  bWaitRetrace;
	int  nWidth, nHeight, nPages, nPagesTotal;
	int  nRefresh;
};

CViewScene	*pScene = NULL;
static SGRViewport **ppViewports = NULL;
static SModeDef	modeDef;
static int	nActivePage = 0;
CFixedColorFont	font5("figs5x3c.fnt");
static	byte	*pVirtualScreen = NULL;
static double	fFocus = 200;
static int	nHazeMin = 400, nHazeDist = 128;

SGRViewport *ZAV_Viewport() {
	return ppViewports[0];
}

extern "C" void DbgSaveViewPoint(bool bNumeric)
{
	char s[100];
	strcpy(s,"vessel.!d");
	if( bNumeric ) {
		static int n = 0;
		sprintf(s,"vess%04d.!",n++);
	}
}

static void	DbgCleanUp()
{
	#ifdef _DEBUG
		DbgSaveViewPoint(FALSE);
	#endif
	#ifdef __sgi
		if( _gr_dpy ) XAutoRepeatOn(_gr_dpy);
	#endif
	GRTerminateDevice();
	//GRCleanUp();
	//GREnableTextMode();
	//GRUninitGraphGlide();
	CTimer::Uninstall();
}

static void CleanUp()
{
	delete pScene;
	CViewLibTexturedFigure::ClearLib();
	//GRUninitGraphNT();
	if( ppViewports ) {
		for( int i = 0 ; i < modeDef.nPages ; i++ )
	 		GRReleaseViewport(ppViewports[i]);
		delete [] ppViewports;
	}

	EndProfile();
	DbgCleanUp();
	delete [] pVirtualScreen;
//	GRUninitGraphSGI();
}

#include<conio.h>

#if defined(MSDOS) && _USE_OWNGR
bool VESA2_SetVESA2Mode(int mode,bool bClearMem=TRUE);
#endif

#if _USE_OWNGR
void dbgs()
{
	#if defined(MSDOS) && _USE_OWNGR
		if( _gr_bIsVESA2Mode ) {
			VESA2_SetVESA2Mode(modeDef.nMode,FALSE);
			GRSetVisualPage(_gr_nVisualPage,FALSE);
		}
		else
			GREnableMCGAMode();
		GRSetPalette((byte*)paletteTranslator.Palette());
	#endif
	GRDumpScreen();
	#if defined(MSDOS)
		getch();
	#endif
}
#endif

#if _USE_GLIDE
void dbgs()
{
	grSstPassthruMode(GR_PASSTHRU_SHOW_SST1);
	GRDumpScreen();
	#ifdef MSDOS
		getch();
		grSstPassthruMode(GR_PASSTHRU_SHOW_VGA);
	#endif
}

void dbgf()
{
	grSstPassthruMode(GR_PASSTHRU_SHOW_VGA);
}
#endif

void MessageLoop() {} //FIXME


void QCallback(CViewObjectBaseSet *pBaseSet,CViewObjectBase *pBase,void *pInfo)
{
	ASSERT(pInfo == (void*)1234);
	(void)pBase;
	//pBase->KFSet().Mod("all").RotateOy(M_PI*10/180,CFVector3(5,0,0)).Update();
	(void)pBaseSet;
}


#define	_MILL 0


extern CViewObjectRef *pVesselObj;
extern bool   __bDrawObjects, __bDrawLand;

//#include"config.cpp"

#if (defined(MSDOS) && _USE_OWNGR) || _USE_GLIDE
static TCchar *SafeNullStr(TCchar *s)
{
	return s ? s : "";
}
#endif

#if defined(MSDOS) && _USE_OWNGR
static int VESA2ModeEnumerator(SGRVESA2EnumModes &s)
{
	if( ((SModeDef*)s.pUserInfo)->nWidth == s.nWidth &&
				((SModeDef*)s.pUserInfo)->nHeight == s.nHeight ) {
		((SModeDef*)s.pUserInfo)->nPagesTotal = s.nPages;
		return FALSE;
	}
	return TRUE;
}
#endif

#if (defined(MSDOS) && _USE_OWNGR) || _USE_GLIDE
static void VideoMode(CConfigFile &config,SModeDef &s)
{
	#if _USE_OWNGR
		#define	SYS "DOS"
		char sys[100];
		if( config(SYS,"WaitRetrace") )
			RTCHECK1(config(SYS,"WaitRetrace","%d",&s.bWaitRetrace)==1,"Bad wait retrace flag: %s", config(SYS,"WaitRetrace"));
		if( config(SYS,"Mode","%s",sys) == 1 ) {
			if( strcmpi(sys,"MCGA") == 0 ) {
				s.nWidth = 320; s.nHeight = 200; s.nPages = 1; s.bVESA2 = FALSE;
				s.nMode = 0x13;
				goto EndMode;
			}
			if( strcmpi(sys,"VESA2") == 0 ) {
				if( config(SYS,"Mode","%s %d x %d [%d]",sys,&s.nWidth,&s.nHeight,&s.nPages) == 4 ) {
					s.nMode = GREnumVESA2Modes(VESA2ModeEnumerator,&s);
					s.bVESA2 = TRUE;
					RTCHECK1(s.nMode >= 0, "Graphics mode '%s' not available",config(SYS,"Mode") );
					RTCHECK(s.nPagesTotal>=s.nPages, "Not enough video memory");
					goto EndMode;
				}
			} // if vesa2
		} // if found sys
	#endif

	#if _USE_GLIDE
		#define	SYS "3DFX"
		static struct	{
			int n, x, y;
		} modes[] = {
			#define	DEFMODE(X,Y) { GR_RESOLUTION_##X##x##Y, X, Y }
			DEFMODE(320,200),
			DEFMODE(320,240),
			DEFMODE(400,256),
			DEFMODE(512,384),
			DEFMODE(640,200),
			DEFMODE(640,480),
			DEFMODE(800,600),
			DEFMODE(960,720),
			DEFMODE(856,480),
			DEFMODE(512,256)
			#undef	DEFMODE
		};
		char sys[100];
		if( config(SYS,"Mode","%s %d x %d",sys,&s.nWidth,&s.nHeight)==3 ) {
			for( int i = 0 ; i < sizeof(modes)/sizeof(modes[0]) ; i++ ) {
				if( modes[i].x == s.nWidth && modes[i].y == s.nHeight ) {
					s.bVESA2 = FALSE;
					s.nMode = modes[i].n;
					goto EndMode;
				}
			}
		}
	#endif

	RTCHECK1( 0, "Bad mode: %s", SafeNullStr(config(SYS,"Mode")));
	#undef SYS
EndMode:;
	#if _USE_GLIDE
		static struct {
			int n, rate;
		} refs[] = {
			#define	DEFREFRESH(X) { GR_REFRESH_##X##Hz, X }
			DEFREFRESH(60),
			DEFREFRESH(70),
			DEFREFRESH(72),
			DEFREFRESH(75),
			DEFREFRESH(80),
			DEFREFRESH(90),
			DEFREFRESH(100),
			DEFREFRESH(85),
			DEFREFRESH(120)
			#undef DEFREFRESH
		};
		int rate;
		if( config("3DFX","Refresh","%d",&rate) == 1 ) {
			for( int i = 0 ; i < sizeof(refs)/sizeof(refs[0]) ; i++ )
				if( rate == refs[i].rate )	{
					modeDef.nRefresh = refs[i].n;
					goto EndRefresh;
				}
		}
		RTCHECK1(0, "Bad or unsupported refresh: %s", SafeNullStr(config("3DFX","Refresh")) );
EndRefresh:;
	#endif
}
#else
#define	VideoMode(CONFIG,RECT) ((void)0)
#endif

//#if !_USE_GLIDE
extern	int __terrainLin;
extern	double	__terrainCell, __terrainWaterline;
extern	int __terrainVerts, __terrainCoords;
extern	int __terrainIntC;
extern	int __terrainObjs, __loadTextures;
//extern	double fTerrainDistRed0;
extern	int __polygonReductionCall, __reductionCall, __cellCount, __reductionCallLink;
extern	int __countRc;
extern	int	__randomPos;
//#endif

//extern	int __glidePassthru, __glideWindow;

#if	defined(__NT__) && !defined(_DEBUG)
#define	DBGF_RR2_DEFAULT	(DBGF_DEFAULT&~DBGF_ERR_INT3)
#else
#define	DBGF_RR2_DEFAULT	DBGF_DEFAULT
#endif

double	waterline = 200;
extern SDeviceList _dL;
extern IDirectDraw            *_dd;
extern char * GRGetLastErrorStr();


//Window Procedure: since we're doing everything
//in RenderScene, this just handles window close message
extern LRESULT CALLBACK WndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int GRAppInit(HINSTANCE hInstance)
{
    WNDCLASS wc;

    wc.style         = CS_HREDRAW | CS_VREDRAW;      // enable redraw
    wc.lpfnWndProc   = (WNDPROC)WndProc;            // window function
    wc.cbClsExtra    = 0;                            // no extra
    wc.cbWndExtra    = 0;                            // no extra
    wc.hInstance     = hInstance;                     // copy handle
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);  // load icon
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);  // standart cursor
    wc.hbrBackground = NULL;        // window color
    wc.lpszMenuName  = NULL;                   // window menu
    wc.lpszClassName = "RT3DWndClass";                   // class name

    if (!RegisterClass(&wc))
        return FALSE;
    /*
     * Create a window with some default settings that may change
     */
    _gr_hWnd = CreateWindowEx(
            WS_EX_APPWINDOW,
           wc.lpszClassName,
           "The Next Worlds",
           WS_BORDER | WS_CAPTION | WS_OVERLAPPED | WS_CLIPSIBLINGS |
           WS_CLIPCHILDREN | WS_MINIMIZEBOX /*|WS_SYSMENU*/,
           0, 0,
           320, 320,
           NULL,                  /* parent window */
           NULL,                  /* menu handle */
           hInstance,             /* program handle */
           NULL);                 /* create parms */

    RTCHECK(_gr_hWnd,"Cannot create window");
    if (!_gr_hWnd) return FALSE;

    //_hAccelApp = LoadAccelerators(hSavInst, "APP_MENU");

    /*
     * Display the window
     */

    _gr_hDC = GetDC(_gr_hWnd);

    ShowWindow(_gr_hWnd, SW_SHOWNORMAL);

    UpdateWindow(_gr_hWnd);

    ShowWindow(_gr_hWnd, SW_HIDE);


    return TRUE;
}

// ======================================================================
static CConfigFile	*pConfig = NULL;
CViewObjectModel	basevessel;
CViewObjectRef	refvessel;

extern int __revCnt, __fullCnt;

void ILY_StartupImage()
{  CGRImage image(_gr_nScreenWidth, _gr_nScreenHeight);
   unsigned char pal[768];
   FILE *f = fopen("..\\start.pal","rb");

   fread(pal,768,1,f);
   fclose(f);

   for(int i = 0;i < 768;i++)
      pal[i] = (unsigned char)(((float)pal[i]*255.0) / 63.0);

   GRStartScene();
   GREndScene();

   GRSetPalette(pal);


   if (_gr_nScreenWidth >= 640)
      image.LoadFromSPRFile("..\\start_hi.spr",pal,
                            _gr_nScreenWidth/2-320, _gr_nScreenHeight/2 - 240);
   else
      image.LoadFromSPRFile("..\\start_lo.spr",pal,
                            _gr_nScreenWidth/2-160, _gr_nScreenHeight/2 - 120);


 
   GRClearScreen(TRUE,GRFillColor(0,0,0));
   GRStartScene();
   GREndScene();
   image.Draw(0,0);

   GRDumpScreen();

   GRClearScreen(TRUE,GRFillColor(0,0,0));
   GRStartScene();
   GREndScene();

   image.Draw(0,0);

   GRDumpScreen();

}


extern int __printInfo;

int   ZAV_InitGraph(HINSTANCE hInst)
{
   __revCnt = __fullCnt = 0;

   if (!CheckExistingRegistry()) {
      MessageBox (NULL, "You must run Setup first", "Error", MB_OK | MB_ICONERROR);
      return 0;
   }

   #if _USE_GLIDE
		CViewObjectBaseSet::SetDistanceModify(0);
	#endif

   set_new_handler(NoMemory);
	SetDebugCleanupProc(DbgCleanUp);
	SetDebugFlags(DBGF_RR2_DEFAULT);

   modeDef.nWidth = 320; modeDef.nHeight = 200; modeDef.nMode = 13;
	modeDef.nPages = modeDef.nPagesTotal = 1; modeDef.nRefresh = 0; modeDef.bVESA2 = FALSE;
	modeDef.bWaitRetrace = FALSE;
	#if defined(__NT__)
		modeDef.nWidth = 640; modeDef.nHeight = 480;
      //config("Graphics","Resolution","%dx%d",&modeDef.nWidth,&modeDef.nHeight);
		//fTerrainDistRed0 = 8;
	#endif
	#if defined(__sgi)
		modeDef.nWidth = 640; modeDef.nHeight = 480;
	#else
		//VideoMode(config,modeDef);
	#endif
	//config("Visual","Terrain0","%lg",&fTerrainDistRed0);

   GRAppInit(hInst);

	{
	    SDeviceList  *dl;
	    SDeviceDescr *dd;

	    dl = GRBuildDevicesList();
	    dd = dl->dDescr;
	    int err;
	    int k;

       int j;// = config.GetInt("Graphics","3DCard");
       int fullS;
       int mode;

       GetChooseRegistry(&j, &fullS, &mode);

	    k = j;
	    while (dd->link && j-->0)  dd = dd->link;
	    //dd->agp=0;
       modeDef.nWidth  = GET_MODE_WIDTH (dd->modes[mode]);
       modeDef.nHeight = GET_MODE_HEIGHT(dd->modes[mode]);

       if ((err = GRInitDevice(dd, MAKE_MODE_DATA(modeDef.nWidth, modeDef.nHeight, GET_MODE_BPP(dd->modes[mode])), fullS)) != TRUE) {
          GRTerminateDevice();
          MessageBox (NULL, GRGetLastErrorStr(), "Error Init Device", MB_OK | MB_ICONERROR);
          MessageBox (NULL, "Can't set required video mode.\nRestore last work video settings.", "Error Init Device", MB_OK | MB_ICONERROR);

          GetLastWorkRegistry(&j, &fullS, &mode);

          k = j;
          dd = dl->dDescr;
          while (dd && j-->0)  dd = dd->link;
          //dd->agp=0;
          modeDef.nWidth  = GET_MODE_WIDTH (dd->modes[mode]);
          modeDef.nHeight = GET_MODE_HEIGHT(dd->modes[mode]);

          if ((err = GRInitDevice(dd, MAKE_MODE_DATA(modeDef.nWidth, modeDef.nHeight, GET_MODE_BPP(dd->modes[mode])), fullS)) != TRUE) {
             GRTerminateDevice();
             MessageBox (NULL, "Can't set required video mode.", "Error Init Device", MB_OK | MB_ICONERROR);
             return 0;
          }
      }

      SetLastWorkRegistry(k,fullS,mode);

      //ShowWindow(_gr_hWnd, SW_SHOWNORMAL);


      if(_dL.currDevice->fullScreen || g_hardware.UseMouse()) g_hardware.HideMouseCursor();

   }

   CRect2      viewRect(0,0,modeDef.nWidth,modeDef.nHeight);
   CVector2    origin(viewRect.CenterX(),viewRect.CenterY());

   {
		ppViewports = new SGRViewport*[modeDef.nPages];
		for( int i = 0 ; i < modeDef.nPages ; i++ ) {
			//if( modeDef.nPages > 1 )	GRSetPageScreen(i);
			ppViewports[i] = GRCreateViewport(origin.x,origin.y,viewRect);
		}
	}

	CViewObject::SetClipRect  (ppViewports[0]->clipRect);
   GRSetViewport(ppViewports[0]);
   return 1;
}


//========================================================================
char _currPath[PATH_MAX + 1];


int   ZAV_InitLevel(const char *dir)
{
   if (getcwd(_currPath, PATH_MAX + 1) == NULL)
	return 0;

   if (chdir(dir) != 0) return 0;

   pConfig = new CConfigFile("level.cfg");
	CConfigFile	&config = *pConfig;
	config("Visual","HazeMin","%d",&nHazeMin);
	config("Visual","HazeDist","%d",&nHazeDist);
	bool bBSPCheck = config.GetInt("Debug","BSPCheck",0);
	if( config.GetInt("Debug","Log",0) || bBSPCheck ) SetDebugFlags( DBGF_RR2_DEFAULT | DBGF_LOG_MSG );

   /*
   {
		int nFlattenColor = -1;
		double fFlattenRange = 0;
		config("Render","Flatten3D","%d,%lg",&nFlattenColor,&fFlattenRange);
		CViewTexturedFigure::SetFlatColor(nFlattenColor,fFlattenRange);
	}
   */

   __printInfo = config.GetInt("Visual","FrameInfo",0);

   double fNearClip = config.GetDouble("Visual","NearClip",1);
	__terrainWaterline = waterline = config.GetDouble("Visual","Waterline",200.);
	__loadTextures = config.GetInt("Visual","LoadTextures",1);
	__randomPos = config.GetInt("Debug","RandomPos",0);

   fFocus = config.GetDouble("Visual","Focus",1)*modeDef.nWidth*5./8;

   _dL.currDevice->fogCurrent = config.GetInt("Visual","FogMode");

   //if (!(_dL.currDevice->fogCurrent & _dL.currDevice->fog) && _dL.currDevice->fogCurrent != 0)
   //      _dL.currDevice->fogCurrent = _dL.currDevice->fog;



   CPaletteTranslator::LoadPalettePack("default.ptp");
	paletteTranslator.EnableMatch(FALSE);

   paletteTranslator.SetGRTables();
   GRSetPalette((byte*)paletteTranslator.Palette(),FALSE);

   font5.RecreateFont();

	nWhiteColor = GRFillColor(255,255,255);
	CPaletteTranslator::ComputeHazeColors();

	//GRInitParticle();

   ILY_StartupImage();

   GRSetPalette((byte*)paletteTranslator.Palette(), FALSE);


	CViewLibTexturedFigure::LoadLib(NULL);

   GRReInitTextureDB();

	CTaggedFile	f(TRUE);
   f.Open(SubstNULL(config("Scene","Load"),"1.sce"));
	dwMem0 = MemMax();
	pScene = new CViewScene(f);
	dwMem1 = MemMax();
	f.Close();

	RTCHECK(!bBSPCheck,"Program terminated due to BSPCheck request in config");

	__AdjustPlane(pScene->Order());

	pScene->GetTerrain()->SetReductionStart(fTerrain0=config.GetDouble("Visual","Terrain0",4));

    // BY GREEN
    _CViewTerrain::CWaterParams w;

    w.m_transWater   = config.GetDouble("Water", "TransWater");
    w.m_transBump    = config.GetDouble("Water", "TransBump");
    w.m_xwTimeP      = config.GetDouble("Water", "xwTimeP");
    w.m_ywTimeP      = config.GetDouble("Water", "ywTimeP");
    w.m_xwTimeLandPx = config.GetDouble("Water", "xwTimeLandPx");
    w.m_xwTimeLandPy = config.GetDouble("Water", "xwTimeLandPy");
    w.m_ywTimeLandPx = config.GetDouble("Water", "ywTimeLandPx");
    w.m_ywTimeLandPy = config.GetDouble("Water", "ywTimeLandPy");
    w.m_xwStaticA    = config.GetDouble("Water", "xwStaticA");
    w.m_ywStaticA    = config.GetDouble("Water", "ywStaticA");
    w.m_xwDynamicA   = config.GetDouble("Water", "xwDynamicA");
    w.m_ywDynamicA   = config.GetDouble("Water", "ywDynamicA");
    w.m_xbTimeP      = config.GetDouble("Water", "xbTimeP");
    w.m_ybTimeP      = config.GetDouble("Water", "ybTimeP");
    w.m_xbTimeLandPx = config.GetDouble("Water", "xbTimeLandPx");
    w.m_xbTimeLandPy = config.GetDouble("Water", "xbTimeLandPy");
    w.m_ybTimeLandPx = config.GetDouble("Water", "ybTimeLandPx");
    w.m_ybTimeLandPy = config.GetDouble("Water", "ybTimeLandPy");
    w.m_xbStaticA    = config.GetDouble("Water", "xbStaticA");
    w.m_ybStaticA    = config.GetDouble("Water", "ybStaticA");
    w.m_xbDynamicA   = config.GetDouble("Water", "xbDynamicA");
    w.m_ybDynamicA   = config.GetDouble("Water", "ybDynamicA");
    w.m_maxPhaseVal  = config.GetDouble("Water", "MaxPhaseVal");
    pScene->GetTerrain()->InitWaterParams(w);
    pScene->GetTerrain()->InitWaterTabulation();

    // BY GREEN END



	//((CViewObjectRef*)(pScene->ObjRefNames()["refs"][0]))->SetAnimationCallback(QCallback,(void*)1234);
	//double fCellSize = 10;
   /*
	CTimer::Install( dwProfileTimer!=1 ? dwProfileTimer : 4096);
	CKeyBoard::Install();
	CKeyBoard::EnableDefault(config.GetInt("Debug","KdbDefProc",1));
	CKeyBoard::SetEventKey(SC_UP);
	CKeyBoard::SetEventKey(SC_DOWN);
 	CKeyBoard::SetEventKey(SC_LEFT);
 	CKeyBoard::SetEventKey(SC_RIGHT);
 	CKeyBoard::SetEventKey(SC_A);
  	CKeyBoard::SetEventKey(SC_Z);
  	CKeyBoard::SetEventKey(SC_PGUP);
  	CKeyBoard::SetEventKey(SC_PGDN);
  	CKeyBoard::SetEventKey(SC_ALT);
  	CKeyBoard::SetEventKey(SC_Q);
	#if 0
	{
		if( ! modeDef.bVESA2 )
			GREnableMCGAMode();
		else
			GREnableVESA2Mode(modeDef.nMode);
	}
	GRInitGraphGlide(modeDef.nMode,modeDef.nRefresh,2);
	#endif
   */
	//#if _USE_GLIDE
	//	if( config.GetInt("3DFX","ShowVGA",0))  dbgf();
	//#endif
	//GRInitTables(0);
	//GRClearTextureCache();
	//GRSetPalette(paletteTranslator.Palette64());
   nActivePage = 0;

   CViewObject::SetClipPlanes(fNearClip,nHazeMin+nHazeDist);

    //byte nHaze = paletteTranslator.HazeColorIdx();

	paletteTranslator.Haze(0).nHazeMin = nHazeMin;
	paletteTranslator.Haze(0).nHazeDist = nHazeDist;

	paletteTranslator.Haze(1).nHazeMin = config.GetInt("Visual","HazeMinW",10);
	paletteTranslator.Haze(1).nHazeDist = config.GetInt("Visual","HazeDistW",nHazeDist);

   CViewFigure::SetHaze(paletteTranslator.Haze(0));
	CViewObject::SetHaze(TRUE);


   double fAspRatio = modeDef.nHeight*4./3./modeDef.nWidth;
	//#ifndef	MSDOS
	//	fAspRatio = 1;
	//#endif
	pScene->CheckDynamicMap();
	pScene->SetScale(fFocus,fFocus*fAspRatio);

   return 1;
}
// ======================================================================

void ZAV_DeInitLevel()
{
   delete pScene;
	CViewLibTexturedFigure::ClearLib();

   chdir(_currPath);
}

// ======================================================================

void	ZAV_BeginLoop()
{
	dwFrames = 0;
	FlushDebugLogFile();
	pScene->CheckDynamicMap();
	GRSetViewport(ppViewports[nActivePage]);
}

// ======================================================================

/*void ZAV_BeginRenderFrame(CViewDynamicList &list)
{
} */

void ZAV_RenderFrame(TCSFMatrix3x4 *pDir,CViewDynamicList &list)
{
	ASSERT(pDir ) ;
	TCSFMatrix3x4 &dir = *pDir;

	pScene->GetTerrain()->SetCurTime(CTimer::Diff(CTimer::Time(), dwTime0)/CTimer::TicksD());
	pScene->Draw(dir,list);
}

void ZAV_EndRenderFrame()
{
	CViewDynamicList::WasteBox().Clear(FALSE);
	pScene->CheckDynamicMap();
}

// ======================================================================

void	ZAV_NextFrame()
{
	dwFrames++;
}

// ======================================================================

void	ZAV_SaveScreenshot()
{
	#if 0
	static int n = 0;
	if(!n) {
		FILE *f = fopen("shot.pal","wb");
		fwrite(paletteTranslator.Palette64(),768,1,f);
		fclose(f);
	}
	char s[100];
	sprintf(s,"shot%04d.spr",n++);
	FILE *f = fopen(s,"wb");
	fwrite(&_gr_nScreenWidth,2,1,f);
	fwrite(&_gr_nScreenHeight,2,1,f);
	fwrite(&_gr_nScreenWidth,1,1,f);
	fwrite(pVirtualScreen,_gr_nScreenWidth*_gr_nScreenHeight,1,f);
	fclose(f);
	#endif
}

CConfigFile	&ZAV_Config()
{
	ASSERT(pConfig);
	return *pConfig;
}

extern int __bViewHide;

/*void ZAV_ProcessKey(byte key)
{
	// By Green
    if( bKeysDebug && key == SC_R) pScene->GetTerrain()->ChangeWaterTextureState();
    if( bKeysDebug && key == SC_T) pScene->GetTerrain()->ChangeWaterBumpState();
    // End by Green
}*/

/*CVesselPack &ZAV_Vessels()
{
	ASSERT(pVessels);
	return *pVessels;
}*/

CViewScene *ZAV_Scene()
{
	return pScene;
}

void ZAV_Deinit()
{
	CleanUp();
}

void GETCYCLE(unsigned *p);
#pragma aux GETCYCLE = \
    "mov  ebx,eax " \
    ".586p " "rdtsc "        \
    "mov [ebx],eax "\
    "mov [ebx+4],edx "  parm [eax] modify [eax ebx edx]

double _prevTime = 0;

double GetCycle_()
 {
    unsigned u[2];
    double   m = (unsigned)(-1);
    GETCYCLE(u);
    return (((double)u[0]) +((double)u[1])*(m+1))/300000000.;
 }


void ZAV_PrintFrameInfo()
{
	char s[100];
   //sprintf(s,"%lg FPS",1/m_fDeltaT);
   CFVector3	v(g_vehicle->getPosition());
   double r = __fullCnt > 0 ? __revCnt*100./__fullCnt : 0;
   //sprintf(s,"[%lg %lg %lg]   ",v.x,v.y,v.z);
    double t = GetCycle_();//Session::m_realTimer->GetTime();
    //sprintf(s,"%lg %lg %lg   %-7lg FPS",v.x,v.y,v.z,1/(t - _prevTime));
    sprintf(s,"%-7lg FPS",1/(t - _prevTime));

	GREndScene();
   _prevTime = t;
   //font5.PrintClipColorAt(_gr_clipRect.left,_gr_clipRect.top+1,s,nWhiteColor);
	__revCnt = __fullCnt = 0;
}

void ZAV_PrintOverallInfo()
{
	dword dwTime1 = m_dwPrevTime;
	double fTicks = CTimer::TicksD();
	#if 1 // defined(MSDOS) || defined(__NT__)
	char s[40];
	sprintf(s,"%lg Mem=%d",
		dwFrames/((dwTime1-dwTime0)/fTicks),
		dwMem0-dwMem1);
	#endif
	//MessageBox(NULL,s,"RR2",MB_OK);
	(void)dwTime0;
}

// ======================================================================