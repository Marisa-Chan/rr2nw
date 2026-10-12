//#define INITGUID  //MUST precede everything

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <stdio.h>
#include <ddraw.h>
#include <d3d.h>
#include <math.h>

//#include "drawd3d.h"
#define _GRAPH_CPP_
#include "graph.h"

//#include "light.h"
#include "sd1_epal.h"
#include "mathlib.h"


#define ABS(a) ((a)<0?-(a):(a))


extern "C" {
   //OWN_GR specifics
   unsigned char  *_gr_pScreen = NULL;
   unsigned char  *_gr_pOrigin = NULL;
   unsigned char  **_gr_pYCache = NULL;
   unsigned char  *_gr_pHaze = NULL;
   SGRColorDef    *_gr_pTransparency = NULL;
   int            _gr_nTranspCount = 0;
   unsigned char  *_gr_pGouraud = NULL;
   long           _gr_pDiserTable[64*64];

   int   _gr_nScreenWidth = 320;
   int   _gr_nScreenHeight = 200;
   int   _gr_nScreenOriginX = 0;
   int   _gr_nScreenOriginY = 0;
   CRect2 _gr_clipRect;
   SGRViewport *_Viewport = NULL;

   float _gr_fFrontClip = 1.0;

   volatile int   _gr_bRestoreSurf = 0;

   HWND _gr_hWnd = NULL;
   HDC _gr_hDC = NULL;
   HPALETTE    _gr_hPal = NULL;
   GR_BITMAPINFO   _gr_DIBInfo;
   GR_LOGPALETTE _gr_logPal;
   POINTS  _gr_windowPos = { 0, 0 };

}


struct SGraphicsSettings {
   SDeviceDescr *device;
   int           mode;
   int           fullScreen;
   int           hazeMin, hazeDelta;
   SGRViewport   viewport;
}_gS;
/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************  DirectDraw Globals           ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/

IDirectDraw            *_dd = NULL;
IDirectDrawSurface     *_FrontBuffer;
IDirectDrawSurface     *_BackBuffer;
IDirectDrawSurface     *_ZBuffer;
IDirectDrawPalette     *_DDPalette;
IDirectDrawClipper     *_Clipper;

//DDCOLORKEY _ColorKey = { 0, 0};



/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************    Direct3D Globals           ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/

IDirect3D2             *_d3d;
IDirect3DDevice2       *_d3dDevice;
IDirect3DViewport2     *_d3dViewport;

/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************     Color Operation Globals   ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/

int _StartSceneHW;

TExtendedPalette  _EPal;
//int _BlackColorIndex;
TExtendedPalette  _ETransparencyPal;


PALETTEENTRY _Palette[256];
unsigned char _currPalette[768];


unsigned short _palTo16[256];
unsigned long  _palTo24[256];
unsigned short _palTo16Alpha[256];
unsigned long  _palTo24Alpha[256];

int _rScale = 0, _rShift = 0;
int _gScale = 0, _gShift = 0;
int _bScale = 0, _bShift = 0;

//pointers to graph functions
int  (*_pGRInitTables)(unsigned long);
//void (*_pGRSetClipRect)(void);
void (*_pGRCalcYDivCache)(void);

//#define GRSetClipRect()    (_pGRSetClipRect)()
#define GRCalcYDivCache()   (_pGRCalcYDivCache)()
#define GRInitTables(a)     (_pGRInitTables)(a)

extern "C" void   GRInitTextureDB();
extern "C" void   GRSetTextureFileName(void *handle, char *name);

//Functions for SOFTWARE Draw
extern "C" void*  ASM_GRLoadTextureToDB(void* handle, unsigned char *pal, int palCnt, unsigned char *text);
extern "C" void   ASM_GRDeleteTextureFromDB(void* handle);
extern "C" int    ASM_GRDrawPolygonPCCW();
extern "C" int    ASM_GRInitTables(unsigned long );
extern "C" void   ASM_GR_SetClipRect();
extern "C" int    ASM_GRSetHaze(int, int, SGRColorDef *hD);
extern "C" void   ASM_GRSetZPrecision(int );
extern "C" void   ASM_GRCalcYDivCache();
extern "C" void   ASM_GRSetBump(int,int);
extern "C" void   ASM_GRDrawSprite(int x0, int y0, int x1, int y1, int u0, int v0, int u1, int v1, int iz, void *hTex);
extern "C" void   ASM_GRDrawParticle(int x, int y, int size, int iz, unsigned long color);
extern "C" void   ASM_GRDrawAlphaSprite(SGRAlphaSprite *sm);

//Functions for HARDWARE Draw
extern "C" int    D3D_GRDrawPolygonPCCW();
extern "C" int    D3D_GRInitTables(unsigned long mmx);
extern "C" void   D3D_GR_SetClipRect();
extern "C" int    D3D_GRSetHaze(int start, int len, SGRColorDef *hD);
extern "C" void*  D3D_GRLoadTextureToDB(void* handle, unsigned char *pal, int palCnt, unsigned char *text);
extern "C" void   D3D_GRDeleteTextureFromDB(void* handle);
extern "C" void   D3D_GRSetZPrecision(int nShiftL65536);
extern "C" void   D3D_GRSetBump(int, int);
extern "C" void   D3D_GRDrawSprite(int x0, int y0, int x1, int y1, int u0, int v0, int u1, int v1, int iz, void *hTex);
extern "C" void   D3D_GRDrawParticle(int x, int y, int size, int iz, unsigned long color);
extern "C" void   D3D_GRDrawAlphaSprite(SGRAlphaSprite *sm);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
//Function
HRESULT CALLBACK BuildSWModeListCallback(LPDDSURFACEDESC pdds, LPVOID lParam);
void DDTerminate(BOOL fAll=TRUE);
BOOL CALLBACK BuildDevicesListCallback(GUID* lpGUID, LPSTR szName, LPSTR szDevice, LPVOID lParam);
BOOL CALLBACK InitDDCallback(GUID* lpGUID, LPSTR szName, LPSTR szDevice, LPVOID lParam);
void ChooseTextureFormat(SDeviceDescr *dd);
int BitCount(unsigned int dw);
void D3DSetError(char * title, int error);
void SetRenderFunctionTable(int swHw);

SDeviceList _dL = {NULL, -1,};

int GRIsHardware()
{
   return (_dL.currDevice->swHw == GR_HARDWARE);
}

int   GRGetScreenWidth()
{
	return	_gr_nScreenWidth;
}

void  GRSetViewport(SGRViewport *pViewport)
{
    _Viewport = pViewport;

    if (pViewport == NULL) return;

    _gr_nScreenOriginY = pViewport->y;
    _gr_nScreenOriginX = pViewport->x;
	_gr_clipRect = pViewport->clipRect;

    if (_dL.currDevice->swHw == GR_SOFTWARE) {
       _gr_pOrigin = pViewport->pOrigin;
       _gr_pYCache = pViewport->pCache;
    }

    GRSetClipRect();
}

SGRViewport * GRGetViewport()
{
   return _Viewport;
}

SGRViewport * GRCreateViewport(int originX, int originY, TCSRect2 &clipRect)
{
    SGRViewport *pViewport = new SGRViewport;
    int clipH = _gr_nScreenHeight;//clipRect.bottom - clipRect.top;

    pViewport->x = originX;
    pViewport->y = originY;
    pViewport->clipRect.left   = clipRect.left - originX;
    pViewport->clipRect.right  = clipRect.right - originX;
    pViewport->clipRect.top    = clipRect.top - originY;
    pViewport->clipRect.bottom = clipRect.bottom - originY;

    if (_dL.currDevice->swHw == GR_SOFTWARE) {
       pViewport->pCache0 = new byte*[clipH];
       pViewport->pCache = pViewport->pCache0 + originY;//- pViewport->clipRect.top;
       pViewport->pOrigin = _gr_pScreen +  originY * _gr_nScreenWidth + originX;
       //for(int i = pViewport->clipRect.top ; i < pViewport->clipRect.bottom ; i++ )
       for(int i = -originY;i < _gr_nScreenHeight - originY;i++)
          pViewport->pCache[i] = pViewport->pOrigin + i*_gr_nScreenWidth;
    }
    else
       pViewport->pCache0 = NULL;


    return  pViewport;
}

void GRReleaseViewport(SGRViewport *pViewport)
{
    if (!pViewport) return;
    if (_dL.currDevice->swHw == GR_SOFTWARE) {
       if ( _gr_pYCache == pViewport->pCache )  _gr_pYCache = NULL;
       delete  [] pViewport->pCache0;
    }

	delete	pViewport;
}

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1

void GRSetPaletteTables(TCbyte *pHaze,TCbyte *pTransparency,TCbyte *pGouraud)
//void    GRSetScreen(byte *pScreen,int nWidth,int nHeight)
void	GRSetViewport(SGRViewport *pViewport)
SGRViewport	* GRCreateViewport(TCSVector2 &origin,TCSRect2 &clipRect)
void GRReleaseViewport(SGRViewport *pViewport)
//void    GRClearScreen(byte nColor)
//void    GRClearClipRect(byte nColor)
//void GRCleanUp()
//void    GRDumpScreen(byte *pAddr,int nWidth,int nHeight)

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/


void D3D_ReadZBuffer(unsigned char *ptr)
{ DDSURFACEDESC ddsd; // DirectDraw surface description
  int err,i;
  unsigned char *ptr1;

  // Lock the surface so it can be filled
  //if (tDB->counter && _dL.currDevice->agp == 0) _d3dDevice->EndScene();
  memset(&ddsd, 0, sizeof(DDSURFACEDESC));
  ddsd.dwSize = sizeof(DDSURFACEDESC);
  while ((err = _ZBuffer->Lock(NULL, &ddsd, 0, NULL)) != DD_OK);

  ptr1 = (unsigned char *) ddsd.lpSurface;

  for(i = 0;i < _gr_nScreenHeight;i++,ptr1 += ddsd.lPitch) {
     memcpy(ptr,ptr1,_gr_nScreenWidth*2);
     ptr += _gr_nScreenWidth*2;
  }

  _ZBuffer->Unlock(NULL);

}


/***********************************************************************
 ********************      GRBuildDeviceList        ********************
 ***********************************************************************/

int CompareModes( const void *arg1, const void *arg2 )
{	int m1 = *((int*)arg1),
        m2 = *((int*)arg2);

	if (GET_MODE_WIDTH(m1) < GET_MODE_WIDTH(m2))
	   return -1;
	else {
		if (GET_MODE_WIDTH(m1) > GET_MODE_WIDTH(m2))
		   return 1;
		else {
		   if (GET_MODE_HEIGHT(m1) < GET_MODE_HEIGHT(m2))
			  return -1;
		   else
			  return 1;
		}
	}
}



SDeviceList * GRBuildDevicesList()
{   SDeviceDescr *dd, *dd0;
    HKEY hSettings;
    unsigned long dwKeySize;
    char version[256];
    int agp = 0;

    //AGP-?
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion", 0, KEY_ALL_ACCESS, &hSettings) == ERROR_SUCCESS) {
       dwKeySize = sizeof(version);
       if (RegQueryValueEx(hSettings, (LPCSTR) "VersionNumber", 0, 0,
           (unsigned char *) version, &dwKeySize) != ERROR_SUCCESS) {
          //Windows NT possible
          RegCloseKey(hSettings);

          if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_ALL_ACCESS, &hSettings) == ERROR_SUCCESS) {
             dwKeySize = sizeof(version);
             if (RegQueryValueEx(hSettings, (LPCSTR) "VersionNumber", 0, 0,
                 (unsigned char *) version, &dwKeySize) != ERROR_SUCCESS)
                strcpy(version,"0.0");
          }
          else
             strcpy(version,"0.0");
       }

       RegCloseKey(hSettings);

       if (strcmp(version, WINDOWS_AGP_BUILD) >= 0) agp = 1;
    }

    _dL.dDescr = NULL;
    _dL.currDevice = NULL;
    _dL.currMode = -1;
    _dL.chooseDevice = NULL;
    _dL.chooseMode = -1;
    _dL.chooseFullScreen = -1;

    GRTerminateDevice();

    DirectDrawEnumerate(BuildDevicesListCallback, (LPVOID)&_dL);

    //Add Software Device

    dd = new SDeviceDescr;

    dd0 = _dL.dDescr;
    while(dd0) {
       if (dd0->guid == NULL)  break;
       dd0 = dd0->link;
    }

    if (dd0) strcpy(dd->name, dd0->name);
    else strcpy(dd->name,"Primary Display Driver (display)");

    dd->guid = NULL;
    dd->modesQnty = 0;
    dd->caps3d = 1;
    dd->swHw = GR_SOFTWARE;
    dd->present = 1;
    dd->textureH = dd->textureW = 1024;
    dd->agp = 0;

    DirectDrawCreate(NULL, &_dd, NULL);
    if (_dd) {
       _dd->EnumDisplayModes(0, NULL, (LPVOID)dd, BuildSWModeListCallback);
       DDTerminate();
       //add device
       if (!_dL.dDescr) dd->link = NULL;
       else dd->link = _dL.dDescr;
       _dL.dDescr = dd;
    }
    else delete dd;

    dd = _dL.dDescr;
    while(dd) {
       qsort(dd->modes,dd->modesQnty,sizeof(int),CompareModes);
       //set AGP
       if (!agp) dd->agp = 0;
       dd = dd->link;
    }

    return &_dL;
}

int GRCreateSurfaces(SDeviceDescr *dD, int width, int height, int bpp, int fullScreen)
{   int err;

    (void)bpp;
    //
    // Create surfaces
    //
    // NOTE you need to recreate the surfaces for a new display mode
    // they wont work when/if the mode is changed.
    //
    DDSURFACEDESC ddsd;

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    if (fullScreen) {

       if (dD->swHw == GR_HARDWARE) {
          ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
          ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                             DDSCAPS_FLIP |
                             DDSCAPS_COMPLEX |
                             DDSCAPS_3DDEVICE |
                             DDSCAPS_VIDEOMEMORY;
          //if (dD->guid == NULL) //Primary Device
          //   ddsd.dwBackBufferCount = 2;
          //else
             ddsd.dwBackBufferCount = 1;
       }
       else {
          ddsd.dwFlags = DDSD_CAPS;
          ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                             DDSCAPS_VIDEOMEMORY;
       }

       // try to get a double buffered video memory surface.
       err = _dd->CreateSurface(&ddsd, &_FrontBuffer, NULL);

       if (err != DD_OK) {
          D3DSetError("Can't Create FrontBuffer", err);
          return FALSE;
       }

       if (dD->swHw == GR_HARDWARE) {
          // get a pointer to the back buffer
          DDSCAPS caps;
          caps.dwCaps = DDSCAPS_BACKBUFFER;
          err = _FrontBuffer->GetAttachedSurface(&caps, &_BackBuffer);

          if (err != DD_OK) {
             D3DSetError("Can't Get Backbuffer", err);
             return FALSE;
          }
       }
    }
    else {
       //window
       if (dD->swHw == GR_HARDWARE) {
          ddsd.dwFlags = DDSD_CAPS;
          ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY;

          err = _dd->CreateSurface(&ddsd, &_FrontBuffer, NULL);

          if (err != DD_OK) {
             D3DSetError("Can't Create FrontBuffer for Window", err);
             return FALSE;
          }

          ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
          ddsd.dwWidth = width;
          ddsd.dwHeight = height;
          ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE |
                                DDSCAPS_VIDEOMEMORY;

          err = _dd->CreateSurface(&ddsd, &_BackBuffer, NULL);

          if (err != DD_OK) {
             D3DSetError("Can't Create BackBuffer for Window", err);
             return FALSE;
          }
          /*
           * Create the DirectDraw Clipper object and attach it to the window
           * and front buffer.
           */

          err = _dd->CreateClipper(0, &_Clipper, NULL);

          if (err != DD_OK) {
             D3DSetError("Can't Create Clipper", err);
             return FALSE;
          }

          err = _Clipper->SetHWnd(0, _gr_hWnd);

          if (err != DD_OK) {
             D3DSetError("Attaching clipper to window failed", err);
             return FALSE;
          }

          err = _FrontBuffer->SetClipper(_Clipper);

          if (err != DD_OK) {
             D3DSetError("Attaching clipper to front buffer failed", err);
             return FALSE;
          }

       }
       else {
          //software, windowed
         _gr_DIBInfo.bmiHeader.biSize = sizeof(_gr_DIBInfo.bmiHeader);
         _gr_DIBInfo.bmiHeader.biWidth = width;
         _gr_DIBInfo.bmiHeader.biHeight = -height;
         _gr_DIBInfo.bmiHeader.biPlanes = 1;
         _gr_DIBInfo.bmiHeader.biBitCount = 8;
         _gr_DIBInfo.bmiHeader.biCompression = BI_RGB;
         _gr_DIBInfo.bmiHeader.biSizeImage = 0;
         _gr_DIBInfo.bmiHeader.biXPelsPerMeter = 0;
         _gr_DIBInfo.bmiHeader.biYPelsPerMeter = 0;
         _gr_DIBInfo.bmiHeader.biClrUsed = 256;
         _gr_DIBInfo.bmiHeader.biClrImportant = 0;
       }
    }


#if USE_Z_BUFFER
    if (dD->swHw == GR_HARDWARE) {
       memset(&ddsd, 0, sizeof(ddsd));
       ddsd.dwSize = sizeof(ddsd);
       ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS |
				  DDSD_ZBUFFERBITDEPTH ;
       ddsd.dwWidth = width;
       ddsd.dwHeight = height;
       ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
       ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
       ddsd.dwZBufferBitDepth = 16;

       err = _dd->CreateSurface(&ddsd, &_ZBuffer, NULL);

       if (err != DD_OK ) {
          D3DSetError("Can't Create ZBuffer", err);
          return FALSE;
       }

       err = _BackBuffer->AddAttachedSurface(_ZBuffer);

       if (err != DD_OK ) return FALSE;

       //RELEASE(_ZBuffer);
    }
#endif

    if (dD->swHw == GR_SOFTWARE) {
       if (_gr_pScreen != NULL) delete [] _gr_pScreen;
       //FIXME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
       _gr_pScreen = (unsigned char *) new char [width * (height+1)];
	}

    return TRUE;
}


int __HazeStartInt;
float __HazeLen;

int GRSetHaze(int start, int len, SGRColorDef *hD)
{
    //_gS.hazeMin = start;
    //_gS.hazeDelta = len;
    __HazeStartInt = 65536. / start;
    __HazeLen = start + len;
    return _GRSetHaze(start, len, hD);
}


int GRChangeResolution(int vMode, int fullScreen)
{   int err, i, modeNum, swHw;
    int height = GET_MODE_HEIGHT(vMode),
        width = GET_MODE_WIDTH(vMode),
        bpp = GET_MODE_BPP(vMode);

    if (_dL.currDevice == NULL) return FALSE;

    swHw = _dL.currDevice->swHw;

    //bpp, windowed or fullscreen
    if (swHw == GR_SOFTWARE) bpp = 8; //always can window or fullscreen
    else {
       if (fullScreen == FALSE) {
          if (_dL.currDevice->guid != NULL)
             fullScreen = TRUE; //Only PDD can render in window
       }
    }

    //find requested video mode
    if (swHw == GR_HARDWARE || fullScreen) {
       int mode = (MAKE_MODE_DATA(width, height, bpp)) | 0x80000000;
       int modeType = (swHw == GR_HARDWARE?0x0:0x80000000);

       for(i = 0; i < _dL.currDevice->modesQnty;i++)
          if ((_dL.currDevice->modes[i] | modeType) == mode) {
             //dL->currMode = i;
             modeNum = i;
             mode = 0;
             break;
       }

       if (mode) return FALSE;
    }
    else modeNum = 0;

    GREndScene();

    //release surfaces
    //GRShaize();


    RELEASE(_ZBuffer);
    RELEASE(_BackBuffer);
    RELEASE(_FrontBuffer);
    RELEASE(_Clipper);

    RELEASE(_d3dViewport);
    RELEASE(_d3dDevice);
    RELEASE(_d3d);

    if (fullScreen) {
       err = _dd->SetCooperativeLevel(_gr_hWnd,
             DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX);

       if (err != DD_OK) return FALSE;

       err = _dd->SetDisplayMode(width, height, bpp);

       if (err != DD_OK) {
          D3DSetError("Can't Set Video Mode", err);
          return FALSE;
       }

    }
    else {
       if (swHw == GR_HARDWARE) {
          err = _dd->SetCooperativeLevel(_gr_hWnd, DDSCL_NORMAL);

          if (err != DD_OK) return FALSE;
       }

       //if (swHw == GR_HARDWARE) {
          RECT rc;

          SetRect(&rc, 0, 0, width, height);
          AdjustWindowRectEx(&rc, GetWindowLong(_gr_hWnd, GWL_STYLE),
                        GetMenu(_gr_hWnd) != NULL,
                        GetWindowLong(_gr_hWnd, GWL_EXSTYLE));
          SetWindowPos(_gr_hWnd, NULL, 0, 0, rc.right-rc.left,
                        rc.bottom-rc.top,
                        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
          SetWindowPos(_gr_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                        SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

       //}
       //else GRTerminateDevice();
       //if (swHw == GR_SOFTWARE) GRTerminateDevice();
    }

    if (!GRCreateSurfaces(_dL.currDevice, width, height, bpp, fullScreen)) return FALSE;

    if (swHw == GR_HARDWARE) {
       //---------------------------------------------------------
       //Set Variables for FILL_COLOR
      /*
       DDSURFACEDESC ddsd;
       int s, m;

       ZeroMemory(&ddsd, sizeof(ddsd));
       ddsd.dwSize = sizeof(ddsd);

       _dd->GetDisplayMode(&ddsd);
       if (bpp != 8) {
          for (s = 0, m = ddsd.ddpfPixelFormat.dwRBitMask; !(m & 1); s++, m >>= 1);
          _rShift = s;
          _rScale = 8 - BitCount(ddsd.ddpfPixelFormat.dwRBitMask);

          for (s = 0, m = ddsd.ddpfPixelFormat.dwGBitMask; !(m & 1); s++, m >>= 1);
          _gShift = s;
          _gScale = 8 - BitCount(ddsd.ddpfPixelFormat.dwGBitMask);

          for (s = 0, m = ddsd.ddpfPixelFormat.dwBBitMask; !(m & 1); s++, m >>= 1);
          _bShift = s;
          _bScale = 8 - BitCount(ddsd.ddpfPixelFormat.dwBBitMask);

       }
       */


       //**************Init 3D*****************************************
       err = _dd->QueryInterface(IID_IDirect3D2, (void**)&_d3d);

       if (err != DD_OK) {
          D3DSetError("This application requires DirectX 5.0", err);
          return FALSE;
       }

       err = _d3d->CreateDevice(IID_IDirect3DHALDevice, _BackBuffer, &_d3dDevice);

       if (err != DD_OK) {
          D3DSetError("Can't Create D3DHal Device", err);
          return FALSE;
       }

       //ChooseTextureFormat(dD);

       //
       // now make a Viewport
       //
       err = _d3d->CreateViewport(&_d3dViewport, NULL);

       if (err != DD_OK) {
          D3DSetError("Can't Create Viewport", err);
          return FALSE;
       }

       err = _d3dDevice->AddViewport(_d3dViewport);

       /*
        * Setup the viewport for a reasonable viewing area
        */
       D3DVIEWPORT2 viewData;
       memset(&viewData, 0, sizeof(D3DVIEWPORT2));
       viewData.dwSize = sizeof(D3DVIEWPORT2);
       viewData.dwX = 0;
       viewData.dwY = 0;
       viewData.dwWidth  = width;
       viewData.dwHeight = height;
       viewData.dvClipX = 0;
       viewData.dvClipWidth = width;
       viewData.dvClipHeight = height;
       viewData.dvClipY = 0;
       viewData.dvMinZ = 0.0f;
       viewData.dvMaxZ = 1.0f;
       err = _d3dViewport->SetViewport2(&viewData);

       if (err != DD_OK) return FALSE;

       err = _d3dDevice->SetCurrentViewport(_d3dViewport);

       if (err != DD_OK) return FALSE;

       //*************************************************************
       //Get Free Texture Memory
       _StartSceneHW = 0;
    }

    _gr_nScreenWidth = width;
    _gr_nScreenHeight = height;

    _dL.currMode = modeNum;
    _dL.currDevice->fullScreen = fullScreen;

    GRCalcYDivCache();
    //GRInitTables(0);
    //GRInitTextureDB();
    //GRInitParticle();
    //GRInitRay();

    //dD->swHw = GR_HARDWARE;

    GRSetPalette(_currPalette);

    //GRShaize();

    return TRUE;

}


/***********************************************************************
 ********************          GRD3DInit            ********************
 ***********************************************************************/

int GRInitDevice(SDeviceDescr *dD, int vMode, int fullScreen)
{   int err, i, modeNum, swHw;
    int height = GET_MODE_HEIGHT(vMode),
        width = GET_MODE_WIDTH(vMode),
        bpp = GET_MODE_BPP(vMode);

    if (!dD) return FALSE;

    swHw = dD->swHw;

    //bpp, windowed or fullscreen
    if (swHw == GR_SOFTWARE) bpp = 8; //always can window or fullscreen
    else {
       if (fullScreen == FALSE) {
          if (dD->guid != NULL)
             fullScreen = TRUE; //Only PDD can render in window
       }
    }

    //find requested video mode
    if (swHw == GR_HARDWARE || fullScreen) {
       int mode = (MAKE_MODE_DATA(width, height, bpp)) | 0x80000000;
       int modeType = (swHw == GR_HARDWARE?0x0:0x80000000);

       for(i = 0; i < dD->modesQnty;i++)
          if ((dD->modes[i] | modeType) == mode) {
             //dL->currMode = i;
             modeNum = i;
             mode = 0;
             break;
       }

       if (mode) return FALSE;
    }
    else modeNum = 0;

    //destroy all DD and D3D device
    GRTerminateDevice();
	//*********************Init DirectDraw***************
    if (fullScreen || swHw == GR_HARDWARE) {
       DirectDrawEnumerate(InitDDCallback, (LPVOID)dD);
       if (!_dd) {
          D3DSetError("Can't create DD Object", -1);
          return FALSE;
       }
    }
    else _dd = NULL;

    ShowWindow(_gr_hWnd, SW_SHOWNORMAL);

    if (fullScreen) {
       err = _dd->SetCooperativeLevel(_gr_hWnd,
             DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT);//DDSCL_ALLOWMODEX);

       if (err != DD_OK) return FALSE;

       err = _dd->SetDisplayMode(width, height, bpp);

       if (err != DD_OK) {
          D3DSetError("Can't Set Video Mode", err);
          return FALSE;
       }

    }
    else {
       if (swHw == GR_HARDWARE) {
          err = _dd->SetCooperativeLevel(_gr_hWnd, DDSCL_NORMAL);

          if (err != DD_OK) return FALSE;
       }


       //if (swHw == GR_HARDWARE) {
          RECT rc;

          SetRect(&rc, 0, 0, width, height);
          AdjustWindowRectEx(&rc, GetWindowLong(_gr_hWnd, GWL_STYLE),
                        GetMenu(_gr_hWnd) != NULL,
                        GetWindowLong(_gr_hWnd, GWL_EXSTYLE));
          SetWindowPos(_gr_hWnd, NULL, 0, 0, rc.right-rc.left,
                        rc.bottom-rc.top,
                        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
          SetWindowPos(_gr_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                        SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

       //}
       //else GRTerminateDevice();
       //if (swHw == GR_SOFTWARE) GRTerminateDevice();
    }

    // get rid of any previous surfaces.
    //DDTerminate(FALSE);

  //  if (_dd != NULL) _dd->SetCooperativeLevel(_gr_hWnd, DDSCL_NORMAL);

    //
    // Create surfaces
    if (!GRCreateSurfaces(dD, width, height, bpp, fullScreen)) return FALSE;

    //====================Set Render Function Table==============
	SetRenderFunctionTable(swHw);

    if (swHw == GR_HARDWARE) {
       //---------------------------------------------------------
       //Set Variables for FILL_COLOR
       DDSURFACEDESC ddsd;
       int s, m;

       ZeroMemory(&ddsd, sizeof(ddsd));
       ddsd.dwSize = sizeof(ddsd);

       _dd->GetDisplayMode(&ddsd);
       if (bpp != 8) {
          for (s = 0, m = ddsd.ddpfPixelFormat.dwRBitMask; !(m & 1); s++, m >>= 1);
          _rShift = s;
          _rScale = 8 - BitCount(ddsd.ddpfPixelFormat.dwRBitMask);

          for (s = 0, m = ddsd.ddpfPixelFormat.dwGBitMask; !(m & 1); s++, m >>= 1);
          _gShift = s;
          _gScale = 8 - BitCount(ddsd.ddpfPixelFormat.dwGBitMask);

          for (s = 0, m = ddsd.ddpfPixelFormat.dwBBitMask; !(m & 1); s++, m >>= 1);
          _bShift = s;
          _bScale = 8 - BitCount(ddsd.ddpfPixelFormat.dwBBitMask);

       }


       //**************Init 3D*****************************************
       err = _dd->QueryInterface(IID_IDirect3D2, (void**)&_d3d);

       if (err != DD_OK) {
          D3DSetError("This application requires DirectX 5.0", err);
          return FALSE;
       }

       err = _d3d->CreateDevice(IID_IDirect3DHALDevice, _BackBuffer, &_d3dDevice);

       if (err != DD_OK) {
          D3DSetError("Can't Create D3DHal Device", err);
          return FALSE;
       }

       ChooseTextureFormat(dD);

       //
       // now make a Viewport
       //
       err = _d3d->CreateViewport(&_d3dViewport, NULL);

       if (err != DD_OK) {
          D3DSetError("Can't Create Viewport", err);
          return FALSE;
       }

       err = _d3dDevice->AddViewport(_d3dViewport);

       /*
        * Setup the viewport for a reasonable viewing area
        */
       D3DVIEWPORT2 viewData;
       memset(&viewData, 0, sizeof(D3DVIEWPORT2));
       viewData.dwSize = sizeof(D3DVIEWPORT2);
       viewData.dwX = 0;
       viewData.dwY = 0;
       viewData.dwWidth  = width;
       viewData.dwHeight = height;
       viewData.dvClipX = 0;
       viewData.dvClipWidth = width;
       viewData.dvClipHeight = height;
       viewData.dvClipY = 0;
       viewData.dvMinZ = 0.0f;
       viewData.dvMaxZ = 1.0f;
       err = _d3dViewport->SetViewport2(&viewData);

       if (err != DD_OK) return FALSE;

       err = _d3dDevice->SetCurrentViewport(_d3dViewport);

       if (err != DD_OK) return FALSE;

       //*************************************************************
       //Get Free Texture Memory
       LPDIRECTDRAW2  lpdd2;
       unsigned long  t;
       DDSCAPS caps;

       err = _dd->QueryInterface(IID_IDirectDraw2, (void**)&lpdd2);
       if (err != DD_OK) return FALSE;

       caps.dwCaps = DDSCAPS_TEXTURE;
       err = lpdd2->GetAvailableVidMem(&caps, &t, &dD->textureMemory);

       if (err != DD_OK || dD->textureMemory == 0) {
          caps.dwCaps = DDSCAPS_VIDEOMEMORY;
          err = lpdd2->GetAvailableVidMem(&caps, &t, &dD->textureMemory);
          if (err != DD_OK || dD->textureMemory == 0) {
             D3DSetError("Can't get Availably Texture Memory Count", err);
             return FALSE;
          }
       }

       //dD->textureMemory = 2097000;//4096000;

       _StartSceneHW = 0;
    }

    _gr_nScreenWidth = width;
    _gr_nScreenHeight = height;

    _dL.currDevice = dD;
    _dL.currMode = modeNum;
    dD->fullScreen = fullScreen;

    GRCalcYDivCache();
    GRInitTables(0);
    GRInitTextureDB();

    GRInitParticle(NULL);
    GRInitRay(NULL);
    GRInitLight(NULL);

    GRInitBump();

    //dD->swHw = GR_HARDWARE;

    return TRUE;
}


void GRReInitTextureDB()
{
    GRInitTables(0);
    GRInitTextureDB();

    GRInitParticle(NULL);
    GRInitRay(NULL);
    GRInitLight(NULL);
}

void * GRLoadTextureToDBFromFile(char *filename, int flags)
{   FILE *f = fopen(filename, "rb");
    long *texture;
    short int w,h;

    fread(&w, 2, 1, f);
    fread(&h, 2, 1, f);
    fseek(f, 1, SEEK_CUR);

    texture = (long *) new char [(long) w * (long) h + 12];

    fread((unsigned char *)(texture + 3), (long) w * (long) h, 1, f);
    fclose(f);

    texture[0] = flags;
    texture[1] = h;
    texture[2] = w;

    void *handle =  GRLoadTextureToDB(NULL, NULL, 0,
                                      (unsigned char *)(texture + 3));

    delete [] texture;

    //GRSetTextureFileName(handle, filename);

    return handle;
}


/***********************************************************************
 ********************        GRClearScreen          ********************
 ***********************************************************************/

int GRClearScreen(BOOL fClr, long fColor)
{  int err;
   D3DRECT fufel;

   if (!_dL.currDevice) return FALSE;

   fufel.x1 = _gr_clipRect.left   + _gr_nScreenOriginX;
   fufel.y1 = _gr_clipRect.top    + _gr_nScreenOriginY;
   fufel.x2 = _gr_clipRect.right  + _gr_nScreenOriginX;
   fufel.y2 = _gr_clipRect.bottom + _gr_nScreenOriginY;

   if (_dL.currDevice->swHw == GR_HARDWARE) {

#if USE_Z_BUFFER

      //if (_ZBuffer->IsLost() == DDERR_SURFACELOST) _ZBuffer->Restore();

      err = _d3dViewport->Clear(1, &fufel, D3DCLEAR_ZBUFFER);
      if (err != DD_OK) {
         D3DSetError("Can't Clear ZBuffer", err);
         return FALSE;
      }
#endif

      if (fClr) {
	     DDBLTFX ddbltfx;

	     ddbltfx.dwSize = sizeof(ddbltfx);
	     ddbltfx.dwFillColor = fColor;

         //if (_BackBuffer->IsLost() == DDERR_SURFACELOST)
         //   _BackBuffer->Restore();

        err = _BackBuffer->Blt((RECT*)&fufel,NULL,NULL,DDBLT_COLORFILL | DDBLT_WAIT,&ddbltfx);
		 if (err != DD_OK) {
			D3DSetError("Can't Clear Backbuffer", err);
		 	return FALSE;
		 }
      }

      if (_StartSceneHW == 0) {
         err = _d3dDevice->BeginScene();
         if (err != DD_OK) {
            D3DSetError("Can't Begin Scene", err);
            return FALSE;
         }
         _StartSceneHW = 1;
      }

   }
   else {
      if (fClr) {
         unsigned char *scr = _gr_pScreen + fufel.x1 + fufel.y1 * _gr_nScreenWidth;
         int i;

         for( i = fufel.y1;i < fufel.y2;i++,scr += _gr_nScreenWidth)
            memset(scr, fColor, fufel.x2 - fufel.x1);
      }
   }

   return TRUE;
}

/***********************************************************************
 ********************        GRDumpScreen           ********************
 ***********************************************************************/
//extern void D3D_DrawBuffer();

int _ZBufferEnable = 1;

void GRZBufferEnable(int enable)
{
#if USE_Z_BUFFER
   if (_dL.currDevice->swHw == GR_SOFTWARE) return;

   if (enable) {
      if (_ZBufferEnable) return;
      _d3dDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
      _d3dDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
      _ZBufferEnable = 1;
   }
   else {
      if (!_ZBufferEnable) return;
      _d3dDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
      _d3dDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);
      _ZBufferEnable = 0;
   }
#endif
}



int GREndScene()
{

  if (_dL.currDevice == NULL) return FALSE;

  if (_dL.currDevice->swHw == GR_HARDWARE && _StartSceneHW) {
     //D3D_DrawBuffer();
     int err = _d3dDevice->EndScene();
     if (err != DD_OK) {
        D3DSetError("Can't EndScene",err);
        return FALSE;
     }
     _StartSceneHW = 0;
  }
  return TRUE;

}

int GRStartScene()
{
  if (_dL.currDevice == NULL) return FALSE;

  if (_dL.currDevice->swHw == GR_HARDWARE && _StartSceneHW == 0) {
     int err = _d3dDevice->BeginScene();
     if (err != DD_OK) {
        D3DSetError("Can't BeginScene",err);
        return FALSE;
     }
     _StartSceneHW = 1;
  }
  return TRUE;

}

int GRDumpScreen()
{  int err;

   //(void)nWidth;
   //(void)nHeight;

   if (!_dL.currDevice) return FALSE;

   if (_dL.currDevice->swHw == GR_HARDWARE) {
      //D3D_DrawBuffer();
      if (_StartSceneHW) {
         err = _d3dDevice->EndScene();
         if (err != DD_OK) {
            D3DSetError("Can't EndScene",err);
            return FALSE;
         }
         _StartSceneHW = 0;
      }


      //if (_FrontBuffer->IsLost() == DDERR_SURFACELOST)
      //   _FrontBuffer->Restore();

      if (_dL.currDevice->fullScreen) {
         err = _FrontBuffer->Flip(NULL, DDFLIP_WAIT);//DDFLIP_WAIT);
         //if (err != DD_OK) {
         //   D3DSetError("Can't Flip Buffers",err);
        //    return FALSE;
        // }
      }
      else {
        RECT rc;

        GetClientRect(_gr_hWnd, &rc);
        ClientToScreen(_gr_hWnd, (LPPOINT) &rc);
        ClientToScreen(_gr_hWnd, ((LPPOINT) &rc) + 1);
        _FrontBuffer->Blt(&rc, _BackBuffer, NULL, DDBLT_WAIT, NULL);//DDBLT_WAIT, NULL);
      }
   }
   else {
      if (_dL.currDevice->fullScreen) {
         DDSURFACEDESC ddsd;

         if (_FrontBuffer != NULL) {
            //if (_FrontBuffer->IsLost() == DDERR_SURFACELOST)
            //   _FrontBuffer->Restore();

            memset(&ddsd, 0, sizeof(DDSURFACEDESC));
            ddsd.dwSize = sizeof(DDSURFACEDESC);

            err = _FrontBuffer->Lock(NULL,&ddsd,0,0);
            if (err != DD_OK) {
               D3DSetError("Can't Lock FrontBuffer",err);
               return FALSE;
            }

            if (ddsd.lPitch == _gr_nScreenWidth)
               memcpy(ddsd.lpSurface, _gr_pScreen,
                   _gr_nScreenWidth * _gr_nScreenHeight);
            else {
               unsigned char *pDst = (unsigned char*)ddsd.lpSurface,
                             *pSrc = _gr_pScreen;
               for(int i = 0 ; i < _gr_nScreenHeight ; i++, pDst+=ddsd.lPitch,
		                                     pSrc+=_gr_nScreenWidth)
                  memcpy(pDst,pSrc,_gr_nScreenWidth);
            }
            _FrontBuffer->Unlock(NULL);
         }
      }
      else {
         //windowed, software
         SetDIBitsToDevice(_gr_hDC,
				0,0, // start on device
                _gr_nScreenWidth,_gr_nScreenHeight, // DIB dims
				0,0, // start in DIB
                0,_gr_nScreenHeight, // start scan and number of scans
                _gr_pScreen,
				(BITMAPINFO*)&_gr_DIBInfo,
				DIB_RGB_COLORS);

      }
   }

   return TRUE;
}

#define K   2
#define Km1 (K - 1)

void GRInitBump()
{  FILE *f;
   int i,j,k;

   if (_dL.currDevice == NULL || _dL.currDevice->swHw == GR_HARDWARE) return;

   f = fopen("dith.dth","rb");

   fseek(f,6,SEEK_SET);

   for(j = 0;j < 64;j++) {
      for(i = 0;i < 64;i++) {
         fread(&k,4,1,f);
         /*
         fseek(f,3*4,SEEK_CUR);
         switch (k) {
            case 256:
               k = 512;
            break;
            case -256:
               k = -512;
            break;
            case 257:
               k = 513;
            break;
            case -257:
               k = -513;
            break;
            case 254:
               k = 511;
            break;
            case -254:
               k = -511;
            break;
            case -1:
               k = -1;
            break;
            case 1:
               k = 1;
            break;
            default:
               k = 0;
         }
         */
         _gr_pDiserTable[j+64*i] = k;
      }
      //fseek(f,256*4*3,SEEK_CUR);
   }

   fclose(f);
   /*
   //set MUL Table
   for(j = 0;j < 256;j++)
      for(i = 0;i < 256;i++)
         if (i < 128)
            _gr_BumpMulTable[j*256+i] = (unsigned char)((i*j)/255);
         else
            _gr_BumpMulTable[j*256+i] = (unsigned char)((j*(i-256))/255);

   //set MIXCOLOR Table
   for(i = 0;i < 256;i++) {
      int r = _Palette[i].peRed,
          g = _Palette[i].peGreen,
          b = _Palette[i].peBlue;
      int r1,g1,b1;

      for(j = 0;j < 256;j++) {
         if (j < 128) {
            r1 = (r * (Km1*j + 127))/127;
            g1 = (g * (Km1*j + 127))/127;
            b1 = (b * (Km1*j + 127))/127;
            if (r1 > 255) r1 = 255;
            if (g1 > 255) g1 = 255;
            if (b1 > 255) b1 = 255;
         }
         else {
            r1 = (r * (j*Km1 - 128*K + 256))/(K*128);
            g1 = (g * (j*Km1 - 128*K + 256))/(K*128);
            b1 = (b * (j*Km1 - 128*K + 256))/(K*128);
         }

         _gr_BumpMixColorTable[i+j*256] =
                        (unsigned char)epal_Match( _EPal, RGB_i(r1, g1, b1 ));
      }
   }
   */
}

/***********************************************************************
 ********************        GRFillColor            ********************
 ***********************************************************************/

unsigned long GRFillColor(int r, int g, int b)
{
  //if (!_dL.currDevice) return 0;

  if (_dL.currDevice->swHw == GR_HARDWARE)
     return ((r>>_rScale)<<_rShift) |
            ((g>>_gScale)<<_gShift) |
            ((b>>_bScale)<<_bShift);
  else
     return epal_Match( _EPal, RGB_i(r, g,b ) );

}

unsigned long  GRCreateColor(int r, int g, int b)
{   STextureFormat *tf = &_dL.currDevice->textureFormat[NORMAL_TEXTURE_INDEX];

    if (tf->rgbBitCount == 8) {
       unsigned int  col = epal_Match( _EPal, RGB_i(r, g, b ));
       unsigned char *pal = &_currPalette[col*3];

       return ((((unsigned int)pal[0])<<24)|
               (((unsigned int)pal[1])<<16)|
               (((unsigned int)pal[2])<<8) | col);
    }

    return ((r<<24)|
            (g<<16)|
            (b<<8) | epal_Match( _EPal, RGB_i(r, g, b )));
    //return (((r>>_rScale)<<(24+_rScale))|
    //        ((g>>_gScale)<<(16+_gScale))|
    //        ((b>>_bScale)<<(8 +_bScale)) | epal_Match( _EPal, RGB_i(r, g, b )));
}

unsigned long GRTransparentColor(int r, int g, int b)
{
  if (_dL.currDevice->swHw == GR_HARDWARE)
     return ((r<<16)|(g<<8)|b);
  else
     return (unsigned long)(_gr_pTransparency[
                                 epal_Match(_ETransparencyPal, RGB_i(r, g,b ))
                                             ].pTable
                           );
}

void  GRSetPaletteTables(SGRColorDef *pTransparency, int nTranspCount, TCbyte *pGouraud)
{  unsigned char pal8[3*32];

   //_gr_pHaze = (unsigned char *)pHaze;
   _gr_pTransparency = pTransparency;
   _gr_nTranspCount = nTranspCount;

   _ETransparencyPal.Clear();
   for(int i = 0;i < nTranspCount;i++) {
      pal8[i*3+0] = (unsigned char) pTransparency[i].r;
      pal8[i*3+1] = (unsigned char) pTransparency[i].g;
      pal8[i*3+2] = (unsigned char) pTransparency[i].b;
   }
   epal_Load8BitPal( _ETransparencyPal, pal8, nTranspCount );

   _gr_pGouraud = (unsigned char *)pGouraud;
}



/***********************************************************************
 ********************        GRSetPalette           ********************
 ***********************************************************************/

int GRSetPalette(const unsigned char * pal8, int setScr)
{  STextureFormat *tF;
   int err, i;//, r = 256*256*4;// j;
	unsigned long col;

   if (!_dL.currDevice) return FALSE;

   memcpy(_currPalette,pal8,768);

   //"Clear" Palette
   _currPalette[0] = 0;
   _currPalette[1] = 0;
   _currPalette[2] = 0;
   for(i = 3;i < 256*3;i += 3) {
      if (_currPalette[i+0] == 0 &&
          _currPalette[i+1] == 0 &&
          _currPalette[i+2] == 0
         )
         _currPalette[i+2] = 2;

   }


   if ( _gr_hPal != NULL ) {
		SelectPalette(_gr_hDC, (HPALETTE)GetStockObject(DEFAULT_PALETTE),FALSE);
		DeleteObject(_gr_hPal);
		_gr_hPal = NULL;
	}

   _EPal.Clear();
   epal_Load8BitPal( _EPal, (unsigned char *)_currPalette, 256 );


   _gr_logPal.palVersion = 0x300;
	_gr_logPal.palNumEntries = 256;

   for (i = 0; i < 256; i++) {
      _gr_logPal.palPalEntry[i].peRed =
      _gr_DIBInfo.bmiColors[i].rgbRed =
      _Palette[i].peRed  = (unsigned char)(_currPalette[i * 3 + 0]);

      _gr_logPal.palPalEntry[i].peGreen =
      _gr_DIBInfo.bmiColors[i].rgbGreen =
      _Palette[i].peGreen = (unsigned char)(_currPalette[i * 3 + 1]);

      _gr_logPal.palPalEntry[i].peBlue =
      _gr_DIBInfo.bmiColors[i].rgbBlue =
      _Palette[i].peBlue = (unsigned char)(_currPalette[i * 3 + 2]);

      _gr_DIBInfo.bmiColors[i].rgbReserved = 0;
      _gr_logPal.palPalEntry[i].peFlags = PC_NOCOLLAPSE;
      _gr_DIBInfo.bmiIndex[i] = (WORD)i;

       /*
       int r1 = _Palette[i].peRed*_Palette[i].peRed +
                _Palette[i].peGreen*_Palette[i].peGreen +
                _Palette[i].peBlue*_Palette[i].peBlue;

       if (r1 < r && i > 0) {
          r = r1;
          _BlackColorIndex = i;
       }
        */
   }

   if (_dL.currDevice->swHw == GR_HARDWARE) {
       //==========================Normal Texture Palette======================
       tF = & _dL.currDevice->textureFormat[NORMAL_TEXTURE_INDEX];
       int rgbBit = tF->rgbBitCount;

       if (tF->usePalette) {
          tF = &_dL.currDevice->textureFormat[RGB_TEXTURE_INDEX];
          rgbBit = tF->rgbBitCount;
       }

       switch (rgbBit) {
          case 24:
          case 32:
             for(i = 0;i < 256;i++) {
                col =  ((((unsigned long)_Palette[i].peRed)>>tF->redScale)<<tF->redShift);
                col |= ((((unsigned long)_Palette[i].peGreen)>>tF->greenScale)<<tF->greenShift);
                col |= ((((unsigned long)_Palette[i].peBlue)>>tF->blueScale)<<tF->blueShift);
                _palTo24[i] = col;
             }
             break;

          case 16:
             for(i = 0;i < 256;i++) {
                col =  ((((unsigned long)_Palette[i].peRed)>>tF->redScale)<<tF->redShift);
                col |= ((((unsigned long)_Palette[i].peGreen)>>tF->greenScale)<<tF->greenShift);
                col |= ((((unsigned long)_Palette[i].peBlue)>>tF->blueScale)<<tF->blueShift);
                _palTo16[i] = (unsigned short)col;
             }
             break;
       }
       //==========================Alpha Texture Palette======================
       tF = & _dL.currDevice->textureFormat[ALPHA_TEXTURE_INDEX];
       switch (tF->rgbBitCount) {
          case 24:
          case 32:
             for(i = 0;i < 256;i++) {
                col =  ((((unsigned long)_Palette[i].peRed)>>tF->redScale)<<tF->redShift);
                col |= ((((unsigned long)_Palette[i].peGreen)>>tF->greenScale)<<tF->greenShift);
                col |= ((((unsigned long)_Palette[i].peBlue)>>tF->blueScale)<<tF->blueShift);
                _palTo24Alpha[i] = col;
             }
             break;

          case 16:
             for(i = 0;i < 256;i++) {
                col =  ((((unsigned long)_Palette[i].peRed)>>tF->redScale)<<tF->redShift);
                col |= ((((unsigned long)_Palette[i].peGreen)>>tF->greenScale)<<tF->greenShift);
                col |= ((((unsigned long)_Palette[i].peBlue)>>tF->blueScale)<<tF->blueShift);
                _palTo16Alpha[i] = (unsigned short)col;
             }
             break;
       }

       /*
       if (_gr_pTransparency != NULL) {
          delete [] _gr_pTransparency;
          _gr_pTransparency = NULL;
       }

       if (_gr_pGouraud != NULL) {
          delete [] _gr_pGouraud;
          _gr_pGouraud = NULL;
       }
        */
   }
   else {
       /*
       //------------Set _gr_pTransparency----------------
       #define TRANSP 128
       #define SOLID  (256 - TRANSP)

       if (_gr_pTransparency == NULL)
          _gr_pTransparency = (unsigned char *) new char [256 * 256];
       for(j = 0;j < 256;j++) {
          int rSrc = _Palette[j].peRed *   SOLID,
              gSrc = _Palette[j].peGreen * SOLID,
              bSrc = _Palette[j].peBlue *  SOLID;

          for(i = 0;i < 256;i++) {
             RGB_i rgb((rSrc + _Palette[i].peRed *   TRANSP)/256,
                       (gSrc + _Palette[i].peGreen * TRANSP)/256,
                       (bSrc + _Palette[i].peBlue *  TRANSP)/256);
             _gr_pTransparency[j*256 + i] = (unsigned char) epal_Match( _EPal, rgb);
          }
       }
       //-----------Set _gr_pGouraud-----------------------
       #define LAYER_COUNT 16

       if (_gr_pGouraud == NULL)
          _gr_pGouraud = (unsigned char *) new char [256 * LAYER_COUNT];
       for(j = 0;j < 256;j++) {
          int rSrc = (_Palette[j].peRed *   256)/(LAYER_COUNT/2),
              gSrc = (_Palette[j].peGreen * 256)/(LAYER_COUNT/2),
              bSrc = (_Palette[j].peBlue *  256)/(LAYER_COUNT/2);

          for(i = 0;i < LAYER_COUNT;i++) {
             RGB_i rgb((rSrc * i)/256,
                       (gSrc * i)/256,
                       (bSrc * i)/256);

             _gr_pGouraud[j*LAYER_COUNT + i] = (unsigned char) epal_Match( _EPal, rgb);
          }
       }
       */
   }

   // create the palette.
   if (_dL.currDevice->swHw == GR_HARDWARE || _dL.currDevice->fullScreen) {
       err = _dd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256,
                                _Palette, &_DDPalette, NULL);

       if (err == DD_OK) {
          if (GET_MODE_BPP(_dL.currDevice->modes[_dL.currMode]) == 8 && setScr) {
             err = _FrontBuffer->SetPalette(_DDPalette);
             if (err != DD_OK)
                D3DSetError("Can't Create Palette",err);
             if (_BackBuffer != NULL)
                _BackBuffer->SetPalette(_DDPalette);

          }
       }
       else {
          D3DSetError("Can't Create Palette",err);
          return FALSE;
       }
   }
   else {
       //software, windowed
       if (setScr) {
          SetSystemPaletteUse(_gr_hDC,SYSPAL_NOSTATIC);
          _gr_hPal = CreatePalette((LOGPALETTE*)&_gr_logPal);
          SelectPalette(_gr_hDC, _gr_hPal, FALSE);
          RealizePalette(_gr_hDC);
       }
   }

   return TRUE;
}

/***********************************************************************
 ********************    DirectDraw3D Terminate     ********************
 ***********************************************************************/

void GRTerminateDevice()
{
  if (_dd != NULL) {
     _dd->SetCooperativeLevel(_gr_hWnd, DDSCL_NORMAL);
     _dd->RestoreDisplayMode();
  }

  GRInitTextureDB(); //ReInitTextureDB

  DDTerminate(TRUE);


  if (_dL.currDevice != NULL && _dL.currDevice->swHw == GR_SOFTWARE) {
     if (_gr_pScreen) {
        delete [] _gr_pScreen;
        _gr_pScreen = NULL;
     }
  }
}

/***********************************************************************
 ********************    DirectDraw Terminate       ********************
 ***********************************************************************/

void DDTerminate(BOOL fAll)
{
    //RELEASE(_d3dLight);
    RELEASE(_d3dViewport);
    RELEASE(_d3dDevice);

    RELEASE(_ZBuffer);
    RELEASE(_BackBuffer);
    RELEASE(_FrontBuffer);
    RELEASE(_DDPalette);
    RELEASE(_Clipper);


    if( _gr_hPal != NULL ) {
		SelectPalette(_gr_hDC, (HPALETTE)GetStockObject(DEFAULT_PALETTE),FALSE);
		DeleteObject(_gr_hPal);
		_gr_hPal = NULL;
	}

    if (fAll)
    {
        RELEASE(_d3d);
        RELEASE(_dd);
    }
}

/*
 * Internal Functions
 *
 */

/***********************************************************************
 ********************  SW  BuilModeList Callback    ********************
 ***********************************************************************/

HRESULT CALLBACK BuildSWModeListCallback(LPDDSURFACEDESC pdds, LPVOID lParam)
{
	SDeviceDescr *dd = (SDeviceDescr *) lParam;
    int bits = BitCount(pdds->ddpfPixelFormat.dwRBitMask) +
               BitCount(pdds->ddpfPixelFormat.dwGBitMask) +
               BitCount(pdds->ddpfPixelFormat.dwBBitMask);

    if ( pdds->ddpfPixelFormat.dwRGBBitCount == 8 && bits == 0 && !(pdds->dwWidth == 320 && pdds->dwHeight == 200))
       dd->modes[dd->modesQnty++] = MAKE_MODE_DATA(pdds->dwWidth, pdds->dwHeight,
                                    pdds->ddpfPixelFormat.dwRGBBitCount);

    //return S_TRUE to stop enuming modes, S_FALSE to continue
    return S_FALSE;
}


/***********************************************************************
 ********************    BuilModeList Callback      ********************
 ***********************************************************************/

HRESULT CALLBACK BuildModeListCallback(LPDDSURFACEDESC pdds, LPVOID lParam)
{
	SDeviceDescr *dd = (SDeviceDescr *) lParam;

    if (pdds->ddpfPixelFormat.dwRGBBitCount == 16 && !(pdds->dwWidth == 320 && pdds->dwHeight == 200))
    dd->modes[dd->modesQnty++] = MAKE_MODE_DATA(pdds->dwWidth, pdds->dwHeight,
	                           pdds->ddpfPixelFormat.dwRGBBitCount);

    //return S_TRUE to stop enuming modes, S_FALSE to continue
    return S_FALSE;
}

/***********************************************************************
 ********************    EnumD3DDevice Callback     ********************
 ***********************************************************************/


void GetDevice3DCapabilites(SDeviceDescr *dd, LPD3DDEVICEDESC devHal)
{

   dd->caps3d = 1;

   if (devHal->dwMaxTextureWidth == 0)
      dd->textureW = dd->textureH = 256;
   else {
      dd->textureW = devHal->dwMaxTextureWidth;
      dd->textureH = devHal->dwMaxTextureHeight;
   }

   if (dd->textureW < 256 || dd->textureH < 256) dd->caps3d = 0;


   if (!(devHal->dwFlags & D3DDD_TRICAPS)) {
      dd->caps3d = 0;
      dd->fog = GR_FOG_NONE;
   }
   else {
      #if USE_Z_BUFFER
          if (!(devHal->dwFlags & D3DDD_DEVICEZBUFFERBITDEPTH) ||
              !(devHal->dpcTriCaps.dwZCmpCaps & D3DPCMPCAPS_LESSEQUAL)
             ) dd->caps3d = 0;
      #endif

      dd->fog = GR_FOG_NONE;

      //if (devHal->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGTABLE)
      //   dd->fog |= GR_FOG_TABLE;
      if (devHal->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGVERTEX)
         dd->fog |= GR_FOG_VERTEX;

      //if (dd->fog != GR_FOG_NONE) {
      //   if (!(devHal->dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_FOGFLAT) ||
      //       !(devHal->dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_FOGGOURAUD)
      //      ) dd->caps3d = 0;
      //}

      if (!(devHal->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_PERSPECTIVE))
         dd->caps3d = 0;

   }

}

HRESULT CALLBACK EnumD3DDeviceCallBack(GUID* lpGUID, LPSTR szDevice, LPSTR szName,
                                       LPD3DDEVICEDESC devHal, LPD3DDEVICEDESC devHel, LPVOID lParam)
{
	SDeviceDescr *dd = (SDeviceDescr *) lParam;
    //DDCAPS halC;

	(void)lpGUID;
	(void)szDevice;
	(void)szName;
	(void)devHel;

    if (devHal->dcmColorModel == D3DCOLOR_RGB) { //3d

	   //****************Check AGP*******************************
	   if ((devHal->dwFlags & D3DDD_DEVCAPS) && (devHal->dwDevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM))
		  dd->agp = 1;
	   else
		  dd->agp = 0;

	   //******************Capabilites***************************
       //_dd->GetCaps(&halC, NULL);

       GetDevice3DCapabilites(dd, devHal);

       if (devHal->dwFlags & D3DDD_TRICAPS) dd->alpha = devHal->dpcTriCaps.dwShadeCaps;
	   else dd->alpha = 0xffffffff;

	   if (devHal->dwFlags & D3DDD_DEVICERENDERBITDEPTH)
	   	  dd->renderBpp = devHal->dwDeviceRenderBitDepth;
	   else
		  dd->renderBpp = 0xffffffff;
	}

    return DDENUMRET_OK;

}

/***********************************************************************
 ********************    BuildDeviceList Callback   ********************
 ***********************************************************************/

BOOL CALLBACK BuildDevicesListCallback(GUID* lpGUID, LPSTR szName, LPSTR szDevice, LPVOID lParam)
{
	SDeviceList *dl = (SDeviceList *) lParam;
	SDeviceDescr *dd;
    int err, i;

    dd = new SDeviceDescr;

	wsprintf(dd->name,"%s (%s)",szName, szDevice);
    dd->guid = lpGUID;
	//dd->id = lpGUID;
	dd->modesQnty = 0;
	dd->caps3d = 0;

	DirectDrawCreate(lpGUID, &_dd, NULL);
	if (_dd) {
	   _dd->EnumDisplayModes(0, NULL, (LPVOID)dd, BuildModeListCallback);
	   if (dd->modesQnty) {
	   	  dd->present = 1;

	      err = _dd->QueryInterface(IID_IDirect3D2, (void**)&_d3d);
		  if (err == DD_OK)
		 	 _d3d->EnumDevices(&EnumD3DDeviceCallBack,(LPVOID)dd);
		  else dd->caps3d = 0;
	   }
	   else dd->present = 0;

	   DDTerminate();
	}
	//Set usable video modes
	if (dd->present && dd->caps3d) {
	   for(i = 0;i < dd->modesQnty;i++) {
		  int bpp = GET_MODE_BPP(dd->modes[i]);
		  switch(bpp) {
             case 8:
                bpp = DDBD_8;
                break;
			 case 16:
				bpp = DDBD_16;
				break;
             case 24:
                bpp = DDBD_24;
                break;
             case 32:
                bpp = DDBD_32;
                break;
			 default:
				bpp = 0;
				break;
		  }

		  if (bpp & dd->renderBpp) dd->modes[i] |= 0x80000000;
	   }

       dd->swHw = GR_HARDWARE;
       //add Device
       if (!dl->dDescr) dd->link = NULL;
       else dd->link = dl->dDescr;
       dl->dDescr = dd;
    }
    else delete dd;

    return DDENUMRET_OK;
}


/***********************************************************************
 ********************    InitDirectDraw  Callback   ********************
 ***********************************************************************/

BOOL CALLBACK InitDDCallback(GUID* lpGUID, LPSTR szName, LPSTR szDevice, LPVOID lParam)
{   char tmp[128];
	SDeviceDescr * dD = (SDeviceDescr *) lParam;

	wsprintf(tmp,"%s (%s)",szName, szDevice);

    //if (lstrcmpi(dD->name, tmp) == 0) {
    if (lstrcmpi(dD->name, tmp) == 0 || dD->guid == lpGUID) {
	   DirectDrawCreate(lpGUID, &_dd, NULL);
	   return DDENUMRET_CANCEL;
	}

    return DDENUMRET_OK;
}

/***********************************************************************
 ********************            Bit Count          ********************
 ***********************************************************************/

int BitCount(unsigned int dw)
{
    int i;

    for (i=0; dw; dw=dw>>1)
        i += (dw & 1);

    return i;
}

/***********************************************************************
 ********************  FindTextureFormat Callback   ********************
 ***********************************************************************/

HRESULT CALLBACK FindTextureFormatCallback(DDSURFACEDESC *DeviceFmt, LPVOID lParam)
{
    SDeviceDescr * dd = (SDeviceDescr *)lParam;
    DDPIXELFORMAT ddpf = DeviceFmt->ddpfPixelFormat;
    int texturebpp = TEXTURE_BPP;

    if (dd->agp) texturebpp = 16; /////////

    //
    // we use GetDC/BitBlt to init textures so we only
    // want to use formats that GetDC will support.
    //
    if (ddpf.dwFlags & DDPF_ALPHA)
        return DDENUMRET_OK;

    //
    // BUGBUG GetDC does not work for 1 or 4bpp YET!
    //
    if (ddpf.dwRGBBitCount < 8)
        return DDENUMRET_OK;

    if (ddpf.dwRGBBitCount > 8 && !(ddpf.dwFlags & DDPF_RGB))
        return DDENUMRET_OK;

    if (ddpf.dwRGBBitCount == 8 && !(ddpf.dwFlags & DDPF_PALETTEINDEXED8))
        return DDENUMRET_OK;



    //
    // keep the texture format that is nearest to the bitmap we have
    //
    if (BitCount(ddpf.dwRGBAlphaBitMask) != 0) {
       //if (ddpf.dwFlags & DDPF_ALPHA) {
       if (BitCount(dd->textureFormat[ALPHA_TEXTURE_INDEX].pixelFormat.dwRGBAlphaBitMask) == 0 ||
          //(ddpf.dwRGBBitCount >= TEXTURE_BPP &&
          //(ABS((int)ddpf.dwRGBBitCount - TEXTURE_BPP) <=
          //ABS((int)dd->textureFormat[ALPHA_TEXTURE_INDEX].pixelFormat.dwRGBBitCount - TEXTURE_BPP)) &&
          ((ABS((int)BitCount(ddpf.dwRGBAlphaBitMask) - TEXTURE_ALPHA_BPP) <
	      ABS((int)BitCount(dd->textureFormat[ALPHA_TEXTURE_INDEX].pixelFormat.dwRGBAlphaBitMask) - TEXTURE_ALPHA_BPP)))) {

	      dd->textureFormat[ALPHA_TEXTURE_INDEX].pixelFormat = ddpf;

       }
       //}
	}
	else {
	   int bppD = BitCount(dd->textureFormat[NORMAL_TEXTURE_INDEX].pixelFormat.dwRBitMask) +
		     BitCount(dd->textureFormat[NORMAL_TEXTURE_INDEX].pixelFormat.dwGBitMask) +
			 BitCount(dd->textureFormat[NORMAL_TEXTURE_INDEX].pixelFormat.dwBBitMask);
	   int bppS = BitCount(ddpf.dwRBitMask) +
		     BitCount(ddpf.dwGBitMask) +
			 BitCount(ddpf.dwBBitMask);

	   if (bppD == 0) bppD = dd->textureFormat[NORMAL_TEXTURE_INDEX].pixelFormat.dwRGBBitCount;
	   if (bppS == 0) bppS = ddpf.dwRGBBitCount;

       if (dd->textureFormat[NORMAL_TEXTURE_INDEX].pixelFormat.dwRGBBitCount == 0 ||
          (ABS(bppS - texturebpp) < ABS(bppD - texturebpp))) {

	        dd->textureFormat[NORMAL_TEXTURE_INDEX].pixelFormat = ddpf;

          }

       //RGB format
       bppD = BitCount(dd->textureFormat[RGB_TEXTURE_INDEX].pixelFormat.dwRBitMask) +
             BitCount(dd->textureFormat[RGB_TEXTURE_INDEX].pixelFormat.dwGBitMask) +
             BitCount(dd->textureFormat[RGB_TEXTURE_INDEX].pixelFormat.dwBBitMask);

       if (dd->textureFormat[RGB_TEXTURE_INDEX].pixelFormat.dwRGBBitCount == 0 ||
           bppS == 16) {

            dd->textureFormat[RGB_TEXTURE_INDEX].pixelFormat = ddpf;

          }
    }

    return DDENUMRET_OK;
}

/***********************************************************************
 ********************     Chose Texture Format      ********************
 ***********************************************************************/

void ChooseTextureFormat(SDeviceDescr *dd)
{   int i, s, m;
	STextureFormat * tF;

	dd->textureFormat[NORMAL_TEXTURE_INDEX].pixelFormat.dwRGBBitCount = 0;
	dd->textureFormat[ALPHA_TEXTURE_INDEX].pixelFormat.dwRGBAlphaBitMask = 0;

	_d3dDevice->EnumTextureFormats(FindTextureFormatCallback, (LPVOID)dd);

	for(i = 0; i < MAX_TEXTURE_INDEX;i++) {
	   tF = &dd->textureFormat[i];
       tF->usePalette = (tF->pixelFormat.dwFlags & DDPF_PALETTEINDEXED8);
       tF->rgbBitCount = tF->pixelFormat.dwRGBBitCount;

      if (tF->usePalette == 0) {
          // Determine the red, green and blue masks' shift and scale.
          for (s = 0, m = tF->pixelFormat.dwRBitMask; !(m & 1); s++, m >>= 1);
          tF->redShift = s;
          tF->redScale = 8 - BitCount(tF->pixelFormat.dwRBitMask);

	      for (s = 0, m = tF->pixelFormat.dwGBitMask; !(m & 1); s++, m >>= 1);
          tF->greenShift = s;
          tF->greenScale = 8 - BitCount(tF->pixelFormat.dwGBitMask);

          for (s = 0, m = tF->pixelFormat.dwBBitMask; !(m & 1); s++, m >>= 1);
          tF->blueShift = s;
          tF->blueScale = 8 - BitCount(tF->pixelFormat.dwBBitMask);
	   }

	   if (BitCount(tF->pixelFormat.dwRGBAlphaBitMask) != 0) {
          for (s = 0, m = tF->pixelFormat.dwRGBAlphaBitMask; !(m & 1); s++, m >>= 1);
          tF->alphaShift = s;
          tF->alphaScale = 8 - BitCount(tF->pixelFormat.dwRGBAlphaBitMask);
	   }

    }
}

/***********************************************************************
 ********************        NULL Functions         ********************
 ***********************************************************************/

extern "C" void D3DNULL_GRCalcYDivCache()
{
}

extern "C" int ASMNULL_GRSetPalette(unsigned char *pal6)
{
  (void) pal6;

  return TRUE;
}

//extern "C" void ASMNULL_GRInitTextureDB()
//{
//}

//extern "C" void * ASMNULL_GRLoadTextureToDB(void * handle, int palCnt, unsigned char *pal, unsigned char *text)
//{
//  (void) handle;
//  (void) palCnt;
//  (void) pal;
//  (void) text;
//
//  return text;
//}
/***********************************************************************
 ********************    Set Render Function Table  ********************
 ***********************************************************************/

void SetRenderFunctionTable(int swHw)
{
   switch(swHw) {
	  case GR_HARDWARE:
		 _pGRDrawPolygonPCCW = D3D_GRDrawPolygonPCCW;
         _pGRDrawSprite      = D3D_GRDrawSprite;
		 _pGRInitTables      = D3D_GRInitTables;
		 _pGRSetClipRect     = D3D_GR_SetClipRect;
         //_pGRSetPalette      = D3D_GRSetPalette;
		 _pGRSetHaze         = D3D_GRSetHaze;
         //_pGRInitTextureDB   = D3D_GRInitTextureDB;
		 _pGRLoadTextureToDB = D3D_GRLoadTextureToDB;
         _pGRDeleteTextureFromDB = D3D_GRDeleteTextureFromDB;
         _pGRSetZPrecision   = D3D_GRSetZPrecision;
		 _pGRCalcYDivCache   = D3DNULL_GRCalcYDivCache;
         _pGRSetBump         = D3D_GRSetBump;
         _pGRDrawParticle    = D3D_GRDrawParticle;
         _pGRDrawAlphaSprite = D3D_GRDrawAlphaSprite;
	     break;

      default: //GR_SOFTWARE
         _pGRDrawPolygonPCCW = ASM_GRDrawPolygonPCCW;
         _pGRDrawSprite      = ASM_GRDrawSprite;
         _pGRInitTables      = ASM_GRInitTables;
         _pGRSetClipRect     = ASM_GR_SetClipRect;
         //_pGRSetPalette      = ASMNULL_GRSetPalette;
         _pGRSetHaze         = ASM_GRSetHaze;
         //_pGRInitTextureDB   = ASMNULL_GRInitTextureDB;
         _pGRLoadTextureToDB = ASM_GRLoadTextureToDB;
         _pGRSetZPrecision   = ASM_GRSetZPrecision;
         _pGRCalcYDivCache   = ASM_GRCalcYDivCache;
         _pGRDeleteTextureFromDB = ASM_GRDeleteTextureFromDB;
         _pGRSetBump         = ASM_GRSetBump;
         _pGRDrawParticle    = ASM_GRDrawParticle;
         _pGRDrawAlphaSprite = ASM_GRDrawAlphaSprite;
         break;
   }
}

void ClearComboBox    (HWND hWndCombo);
void FillUpDriverCombo(HWND hWndCombo);
void FillUpModeCombo  (HWND hWndCombo, int nItem);
BOOL CenterWindow     (HWND hwndChild, HWND hwndParent);


void SwitchToGDISurface()
{
   if (_dL.currDevice->fullScreen) {
      _dd->FlipToGDISurface();
      DrawMenuBar(_gr_hWnd);
      RedrawWindow(_gr_hWnd, NULL, NULL, RDW_FRAME);
   }
}

//=========================================================================
BOOL CALLBACK PropertiesProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{

    (void) lParam;

    switch (msg)
    {
        case WM_INITDIALOG:
            // center window
            CenterWindow(hWnd, GetParent(hWnd));
            // init check box
            if (_dL.currDevice->fullScreen)
                SendDlgItemMessage(hWnd, 100, BM_SETCHECK, 1, 0);
            else
                SendDlgItemMessage(hWnd, 100, BM_SETCHECK, 0, 0);
            // init combo boxes
            FillUpDriverCombo(GetDlgItem(hWnd, 101));
            FillUpModeCombo(GetDlgItem(hWnd, 102), -1);
            return TRUE;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case 101:
                    // video driver combo box
                    if (HIWORD(wParam) == CBN_SELENDOK)
                    {
                        //
                        int nItem = SendDlgItemMessage(hWnd, 101, CB_GETCURSEL, 0, 0);
                        if (nItem != CB_ERR)
                        {
                           ClearComboBox(GetDlgItem(hWnd, 102));
                           FillUpModeCombo(GetDlgItem(hWnd, 102), nItem);

                           _dL.chooseDevice = _dL.dDescr;
                           while (nItem-- > 0)
                              _dL.chooseDevice = _dL.chooseDevice->link;

                        }
                    }
                    return TRUE;
                case 102:
                    // video mode combo box
                    if (HIWORD(wParam) == CBN_SELENDOK)
                    {
                        //
                        int nItem = SendDlgItemMessage(hWnd, 102, CB_GETCURSEL, 0, 0);
                        if (nItem != CB_ERR)
                           _dL.chooseMode = nItem;
                    }
                    return TRUE;
                case IDOK:
                    // Ok button
                    _dL.chooseFullScreen =
                        SendDlgItemMessage(hWnd, 100, BM_GETCHECK, 0, 0);
                    EndDialog(hWnd, TRUE);
                    return TRUE;
                case IDCANCEL:
                    // Cancel button
                    EndDialog(hWnd, FALSE);
                    return TRUE;
                default:
                    return FALSE;
            }
        default:
            return FALSE;
  }
}
//-------------------------------------------------------------------------
void ClearComboBox(HWND hWndCombo)
{
    int i, nComboItems;

    nComboItems = SendMessage(hWndCombo, CB_GETCOUNT, 0, 0);
    for (i = 0; i < nComboItems; i++)
        SendMessage(hWndCombo, CB_DELETESTRING, 0, 0);
}
//-------------------------------------------------------------------------
void FillUpDriverCombo(HWND hWndCombo)
{   SDeviceDescr *dd = _dL.dDescr;
    char str[128];

    while (dd) {
       strcpy(str, dd->name);
       if (dd->swHw == GR_HARDWARE)
          strcat(str, " HARDWARE");
       else
          strcat(str, " SOFTWARE");

       SendMessage(hWndCombo, CB_ADDSTRING, 0, (long)str);

       dd = dd->link;
    }

    if (_dL.currDevice != NULL) {
       strcpy(str, _dL.currDevice->name);
       if (_dL.currDevice->swHw == GR_HARDWARE)
          strcat(str, " HARDWARE");
       else
          strcat(str, " SOFTWARE");
       _dL.chooseDevice = _dL.currDevice;
    }
    else  strcpy(str, "No Video Driver Selected");

    SendMessage(hWndCombo, WM_SETTEXT,   0, (long)str);
}
//-------------------------------------------------------------------------
void FillUpModeCombo(HWND hWndCombo, int nItem)
{   SDeviceDescr *dd;
    char str[128];

    if (nItem < 0) {
       if (_dL.currDevice == NULL) {
          SendMessage(hWndCombo, WM_SETTEXT,   0, (long)"No Video Mode Available");
          return;
       }
       else
          dd = _dL.currDevice;
    }
    else {
       dd = _dL.dDescr;
       int i = nItem;
       while (i-- > 0) dd = dd->link;
    }

    for(int i = 0;i < dd->modesQnty;i++) {
       sprintf(str, "%d x %d x %d", GET_MODE_WIDTH(dd->modes[i]),
                     GET_MODE_HEIGHT(dd->modes[i]),
                     GET_MODE_BPP(dd->modes[i]));

       SendMessage(hWndCombo, CB_ADDSTRING, 0, (long)str);
    }

    if ((_dL.currMode < 0 || nItem >= 0) && dd != _dL.currDevice)
       strcpy(str, "No Video Mode Selected");
    else {
       sprintf(str, "%d x %d x %d",
                     GET_MODE_WIDTH(_dL.currDevice->modes[_dL.currMode]),
                     GET_MODE_HEIGHT(_dL.currDevice->modes[_dL.currMode]),
                     GET_MODE_BPP(_dL.currDevice->modes[_dL.currMode]));

       _dL.chooseMode = _dL.currMode;
    }

    SendMessage(hWndCombo, WM_SETTEXT,   0, (long) str);
}
//=========================================================================

//=========================================================================
// Dialog boxes take on the screen position that they were designed
// at, which is not always appropriate. centreing the dialog over a
// particular window usually results in a better position.
BOOL CenterWindow(HWND hwndChild, HWND hwndParent)
{
    int  cxChild, cyChild, cxParent, cyParent;
    int  cxScreen, cyScreen, xNew, yNew;
    RECT rcChild, rcParent;
    HDC  hdc;

    // get the Height and Width of the child window
    GetWindowRect(hwndChild, &rcChild);
    cxChild = rcChild.right-rcChild.left;
    cyChild = rcChild.bottom-rcChild.top;
    // get the Height and Width of the parent window
    GetWindowRect(hwndParent, &rcParent);
    cxParent = rcParent.right-rcParent.left;
    cyParent = rcParent.bottom-rcParent.top;
    // get the display limits
    hdc = GetDC(hwndChild);
    cxScreen = GetDeviceCaps(hdc, HORZRES);
    cyScreen = GetDeviceCaps(hdc, VERTRES);
    ReleaseDC(hwndChild, hdc);
    // calculate new X position, then adjust for screen
    xNew = rcParent.left+((cxParent-cxChild)/2);
    if (xNew < 0)
        xNew = 0;
    else if ((xNew+cxChild) > cxScreen)
        xNew = cxScreen-cxChild;
    // calculate new Y position, then adjust for screen
    yNew = rcParent.top+((cyParent-cyChild)/2);
    if (yNew < 0)
        yNew = 0;
    else if ((yNew+cyChild) > cyScreen)
        yNew = cyScreen-cyChild;
    // set it, and return
    return SetWindowPos(hwndChild,
                        NULL,
                        xNew, yNew,
                        0, 0,
                        SWP_NOSIZE|SWP_NOZORDER);
}
//=========================================================================