#define INITGUID  //MUST precede everything
//#define D3D_OVERLOADS


#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <stdio.h>
#include <ddraw.h>
#include <d3d.h>
#include <math.h>

#include "rsx.h"

//#define _DRAWD3D_CPP_

#include "graph.h"
#include "drawd3d.h"
#include "sd1_epal.h"

//#define LAST_H__PATTERNS
//#include "game.h"

#define I65536 (0.00001525878906)

#define START_WIN_SIZE 320

#define TEXT_PRELOAD_COUNT 3

#define STAT 0



/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************  Extern Functions & Variables ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/

//Variables
/*
extern "C" SRectangle _gr_clipRect;
extern "C" long _gr_nScreenOriginX;
extern "C" long _gr_nScreenOriginY;
extern "C" long _gr_nScreenHeight;
extern "C" long _gr_nScreenWidth;
*/
extern TExtendedPalette  _EPal;
extern int _BlackColorIndex;

//Functions
extern "C" long MulDivL(long a, long b, long c);
extern "C" long CalcPerPar(long,float,long,float,long,long,float);
extern "C" float MulDivF(float a, long b, long c);
extern "C" float CalcPerParF(float,float,float,float,long,long,float);

/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************  DirectDraw Globals           ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/

extern IDirectDraw            *_dd;
extern IDirectDrawSurface     *_FrontBuffer;
extern IDirectDrawSurface     *_BackBuffer;
extern IDirectDrawSurface     *_ZBuffer;
extern IDirectDrawPalette     *_DDPalette;

DDCOLORKEY _ColorKey = { 0, 0};
//DDCOLORKEY _ColorKey1 = { 0xff00, 0xff00};



/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************    Direct3D Globals           ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/

extern IDirect3D2             *_d3d;
extern IDirect3DDevice2       *_d3dDevice;
//IDirect3DViewport2     *_d3dViewport;

/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************     Color Operation Globals   ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/

extern PALETTEENTRY _Palette[256];

extern unsigned short _palTo16[256];
extern unsigned long  _palTo24[256];
extern unsigned short _palTo16Alpha[256];
extern unsigned long  _palTo24Alpha[256];

extern int _rScale, _rShift;
extern int _gScale, _gShift;
extern int _bScale, _bShift;

unsigned char _GouraudTable[256*256]; //[brightness*256+color]

/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ******************** Clipping Func,Types & Globals ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/

//Types

typedef void (*ClipFunc)(UGRVertex *vNew, UGRVertex *vp, UGRVertex *v,long y);

//Variables
SRectangle _gr_ilyaClipI;

struct  {
  float left, top, right, bottom;
} _gr_ilyaClipF;

ClipFunc _HClipVertexFTable[TABLE_SIZE];
ClipFunc _VClipVertexFTable[TABLE_SIZE];

SBumpVertex *_bumpIn, * _bumpInNext, *_bumpOut;

//Functions
void HClip(UGRVertex *vNew, UGRVertex *vp, UGRVertex *v,long y);
void VClip(UGRVertex *vNew, UGRVertex *vp, UGRVertex *v,long x);
void HClipRGBZ(UGRVertex *vNew, UGRVertex *vp, UGRVertex *v,long y);
void VClipRGBZ(UGRVertex *vNew, UGRVertex *vp, UGRVertex *v,long x);
void HClipSTZ(UGRVertex *vNew, UGRVertex *vp, UGRVertex *v,long y);
void VClipSTZ(UGRVertex *vNew, UGRVertex *vp, UGRVertex *v,long x);
void HClipRGBSTZ(UGRVertex *vNew, UGRVertex *vp, UGRVertex *v,long y);
void VClipRGBSTZ(UGRVertex *vNew, UGRVertex *vp, UGRVertex *v,long x);

long Clip2DPolygon(UGRVertex **startBuf);

long Clip2DPolygonBump(UGRVertex **startBuf, SBumpVertex **startBumpBuf);


/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ******************** Texture Oper Func,Types & Glb ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/

//Types

typedef struct STextDBTAg {
   unsigned long       flags;//1-don't unload
   unsigned long       counter;  //0-not loaded, >0 - counter
   //for SW
   int                 *textureCache;
   unsigned char       *dataPtr;
   //
   char                filename[64];
   TTextureLoadFunc    loadFunc;
   void                *loadFuncUserPar;
   TTextureLoadIntFunc loadIntFunc;
   unsigned long       size;
   long                w, h;
   float               invW65536,
                       invH65536;
   STextureFormat      *textureFormat;
   D3DTEXTUREHANDLE    handleDev;
   LPDIRECTDRAWSURFACE lpTextureSurfMem;
   LPDIRECT3DTEXTURE2  lpTextureMem;
   LPDIRECTDRAWSURFACE lpTextureSurfDev;
   LPDIRECT3DTEXTURE2  lpTextureDev;
}STextDB;

typedef struct STextMemTag {
	STextDB *text; //NULL - free
	unsigned long size;
	struct STextMemTag *lLink, *rLink;
}STextMem;

//Variables
STextDB _TextDB[MAX_TEXTURE_COUNT];
unsigned long _TextCounter = 0;

unsigned long _DeviceMemory;
STextMem *_D3DMemUse = NULL;
D3DTEXTUREHANDLE _CurrentText = 0;
unsigned long _TextTime = 0;

STextMem  *_memory = NULL;
long _memoryFree = 0;

//Functions
int LoadTextureTo3DCard(void *tDB);

STextMem * MallocTextMem()
{ STextMem * tm = &_memory[_memoryFree++];

  if (_memoryFree >= MAX_TEXTURE_COUNT) _memoryFree = 0;

  while(_memory[_memoryFree].text != NULL)
  	 if (++_memoryFree >= MAX_TEXTURE_COUNT) _memoryFree = 0;

  return tm;
}

inline void FreeTextMem(STextMem * tm)
{
  tm->text = NULL;
}

/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************    DrawPolygon Globals        ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/

SBumpVertex _gr_bump_vertices[GR_MAX_VERTEX];

UGRVertex intBuf[GR_MAX_VERTEX];
SBumpVertex intBumpBuf[GR_MAX_VERTEX];

float _hazeK, _hazeS, _hazeE;
float _fogStart, _fogEnd, _fogOtn;
float _ifogStart, _ifogEnd;
float _ibumpStart, _ibumpEnd;

/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************    Application Globals & Func ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/

//HWND _hwnd;

float _ZPrecision = (float) 1.0 / (float)(2<<16);

char _LastError[256];

extern SDeviceList _dL;


#define COPY_FLOAT(a,b) *((int*)&(a)) = *((int*)&(b))
#define SET_FLOAT_ZERO(a) *((int*)&(a)) = 0
#define FLOAT_NEG(a,b) *((int*)&(a)) = (*((int*)&(b)) ^ 0x80000000)
#define FLOAT_CHANGE_SIGN(f) (*((int*)&(f)) ^= 0x80000000)


/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************      EXTERNAL FUNCTIONS       ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/


/***********************************************************************
 ********************    Get Last Error Message     ********************
 ***********************************************************************/

char * GRGetLastErrorStr()
{
   return _LastError;
}


extern "C" void D3D_GRSetZPrecision(int nShiftL65536)
{
  _ZPrecision = 1. / (float) (1 << (16 + nShiftL65536));
}

void * CreateLightTexture(UGRVertex *start, int verC);

//****************************Z List*********************************

#define ZLIST_LEN 1024

D3D_ZList _zList[ZLIST_LEN];
int _zListOff = 0;
D3D_ZList *_zListStart = NULL;

D3D_ZList * D3D_InsertToZList(float z)
{ D3D_ZList *zl,*zl1;

  if (_zListOff == 0) {
     _zList[0].z = z;
     _zListOff = 1;
     _zList[0].link = NULL;
     _zListStart = _zList;
     return _zList;
  }

  if (_zListOff == ZLIST_LEN) {
     D3D_DrawZList();
     _zList[0].z = z;
     _zListOff = 1;
     _zList[0].link = NULL;
     _zListStart = _zList;
     return _zList;
  }

  //find element
  if (z >= _zListStart->z) {
     _zList[_zListOff].link = _zListStart;
     _zListStart = &_zList[_zListOff++];
     _zListStart->z = z;
     return _zListStart;
  }

  zl = _zListStart;
  while (zl->link && z < zl->link->z) zl = zl->link;

  zl1 = &_zList[_zListOff++];
  zl1->link = zl->link;
  zl->link = zl1;
  zl1->z = z;

  return zl1;

}

void D3D_ClearZList()
{
  _zListOff = 0;
  _zListStart = NULL;
}

void D3D_DrawZList()
{ D3D_ZList *zl = _zListStart;

  if (_zListOff == 0) return;

  //GREndScene();
  //GRStartScene();

  while(zl) {
     switch(zl->type) {
        case 0:
            _d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
            LoadTextureTo3DCard(zl->texture);
            _d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                    (LPVOID) zl->v, 4, D3DDP_DONOTCLIP);
            _d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
        break;

        case D3DTBLEND_MODULATEALPHA:
            LoadTextureTo3DCard(zl->texture);

            _d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
            _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);

            _d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                    (LPVOID) zl->v, 4, D3DDP_DONOTCLIP);

            _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
            _d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
        break;

        case 1:
           D3D_GRSetZPrecision(0);
           memcpy(&_gr_polygon,&zl->polygon,sizeof(SGRPolygon));
           memcpy(_gr_vertices,zl->v,sizeof(UGRVertex)*zl->vertC);
           GRDrawPolygonPCCW();
        break;
     }
     zl = zl->link;
  }

  _zListOff = 0;
  _zListStart = NULL;

}


/***********************************************************************
 ********************    GRDrawPolygonPCCW          ********************
 **********************************************{
 *************************/

//#define   D3D_BUFFER_LEN  128

//UGRVertex _bufVert[D3D_BUFFER_LEN][64];
//int       _bufCount[D3D_BUFFER_LEN];
//int       _bufType[D3D_BUFFER_LEN];
//void     *_bufHandle[D3D_BUFFER_LEN];
//int       _bufLen = 0;


void D3D_DrawBuffer()
{
    /*
     for(int i = 0;i < _bufLen;i++) {
        if (_bufH[i] == NULL) {
           if (_CurrentText != 0) {
           _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, 0);
           _CurrentText = 0;
           }
        }
        else
           LoadTextureTo3DCard(_bufH[i]);

        _d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                    (LPVOID) _bufV[i], _bufC[i], D3DDP_DONOTCLIP);
     }

     _bufLen = 0;
     */
}

extern "C" int D3D_GRDrawPolygonPCCW()
{ UGRVertex *start;
  SBumpVertex * startB;
  int verC;
  unsigned long i;
  //void *lH[MAX_LIGHT_PER_POLYGON];


  //-------------------------------------------
  if (_gr_polygon.dwAddType & GR_POLY_ADD_BUMP) {

     switch (_gr_polygon.dwFullType) {
        case GR_POLY_GOURAUD: {
           unsigned char *rG = _GouraudTable + (long)_gr_polygon.rgbColor.r*256;
           unsigned char *gG = _GouraudTable + (long)_gr_polygon.rgbColor.g*256;
           unsigned char *bG = _GouraudTable + (long)_gr_polygon.rgbColor.b*256;

           for(i=0;i<_gr_polygon.nVertices;i++) {
              int br = _gr_vertices[i].gouraud.b>>12;

              _gr_vertices[i].any.x += _gr_nScreenOriginX;
              _gr_vertices[i].any.y += _gr_nScreenOriginY;
              _gr_bump_vertices[i].u = (float)_gr_vertices[i].bump.u *
                                 ((STextDB *)_gr_polygon.hBump)->invW65536;
              _gr_bump_vertices[i].v = (float)_gr_vertices[i].bump.v *
                                 ((STextDB *)_gr_polygon.hBump)->invH65536;

              _gr_vertices[i].d3d.rgbColor.r = rG[br];
              _gr_vertices[i].d3d.rgbColor.g = gG[br];
              _gr_vertices[i].d3d.rgbColor.b = bG[br];
              _gr_vertices[i].d3d.rgbColor.a = 0xFF;

              _gr_vertices[i].d3d.oow = (float) _gr_vertices[i].any.iz * _ZPrecision;
           }
           break;
        }
        case GR_POLY_GOURAUD_RGB:
           for(i=0;i<_gr_polygon.nVertices;i++) {
              _gr_vertices[i].any.x += _gr_nScreenOriginX;
              _gr_vertices[i].any.y += _gr_nScreenOriginY;
              _gr_bump_vertices[i].u = (float)_gr_vertices[i].bump.u *
                                 ((STextDB *)_gr_polygon.hBump)->invW65536;
              _gr_bump_vertices[i].v = (float)_gr_vertices[i].bump.v *
                                 ((STextDB *)_gr_polygon.hBump)->invH65536;
              _gr_vertices[i].d3d.oow = (float) _gr_vertices[i].any.iz * _ZPrecision;
              _gr_vertices[i].d3d.d3dcolor.color |= 0xFF000000;
           }
           break;

        case GR_POLY_TEXTURE_ALPHA: {
           int color = _gr_polygon.dwColor.color | (_gr_polygon.dwOpacity << 24);

           for(i=0;i<_gr_polygon.nVertices;i++) {
              _gr_vertices[i].any.x += _gr_nScreenOriginX;
              _gr_vertices[i].any.y += _gr_nScreenOriginY;
             _gr_bump_vertices[i].u = (float)_gr_vertices[i].bump.u *
                                 ((STextDB *)_gr_polygon.hBump)->invW65536;
             _gr_bump_vertices[i].v = (float)_gr_vertices[i].bump.v *
                                 ((STextDB *)_gr_polygon.hBump)->invH65536;
             _gr_vertices[i].d3d.u = (float)_gr_vertices[i].texture.u *
                                 ((STextDB *)_gr_polygon.hTexture)->invW65536;
             _gr_vertices[i].d3d.v = (float)_gr_vertices[i].texture.v *
                                 ((STextDB *)_gr_polygon.hTexture)->invH65536;
             _gr_vertices[i].d3d.oow = (float) _gr_vertices[i].any.iz * _ZPrecision;
             _gr_vertices[i].d3d.d3dcolor.color = color;
           }
           break;
        }

        case GR_POLY_TEXTURE_PERSP:
        case GR_POLY_TEXTURE_LIN:
        case GR_POLY_TEXTURE_SMP:
        case GR_POLY_SPRITE_PERSP:
        case GR_POLY_SPRITE_LIN:
        case GR_POLY_SPRITE_MIP:
           for(i=0;i<_gr_polygon.nVertices;i++) {
              _gr_vertices[i].any.x += _gr_nScreenOriginX;
              _gr_vertices[i].any.y += _gr_nScreenOriginY;
             _gr_bump_vertices[i].u = (float)_gr_vertices[i].bump.u *
                                 ((STextDB *)_gr_polygon.hBump)->invW65536;
             _gr_bump_vertices[i].v = (float)_gr_vertices[i].bump.v *
                                 ((STextDB *)_gr_polygon.hBump)->invH65536;
             _gr_vertices[i].d3d.u = (float)_gr_vertices[i].texture.u *
                                 ((STextDB *)_gr_polygon.hTexture)->invW65536;
             _gr_vertices[i].d3d.v = (float)_gr_vertices[i].texture.v *
                                 ((STextDB *)_gr_polygon.hTexture)->invH65536;
             _gr_vertices[i].d3d.oow = (float) _gr_vertices[i].any.iz * _ZPrecision;
             _gr_vertices[i].d3d.d3dcolor.color = 0xFFFFFFFF;
           }
           break;

        case GR_POLY_TEXTURE_GOURAUD:
           for(i=0;i<_gr_polygon.nVertices;i++) {
              _gr_vertices[i].any.x += _gr_nScreenOriginX;
              _gr_vertices[i].any.y += _gr_nScreenOriginY;
              _gr_bump_vertices[i].u = (float)_gr_vertices[i].bump.u *
                                  ((STextDB *)_gr_polygon.hBump)->invW65536;
              _gr_bump_vertices[i].v = (float)_gr_vertices[i].bump.v *
                                 ((STextDB *)_gr_polygon.hBump)->invH65536;
              _gr_vertices[i].d3d.u = (float)_gr_vertices[i].texture.u *
                                 ((STextDB *)_gr_polygon.hTexture)->invW65536;
              _gr_vertices[i].d3d.v = (float)_gr_vertices[i].texture.v *
                                 ((STextDB *)_gr_polygon.hTexture)->invH65536;
              _gr_vertices[i].d3d.d3dcolor.color = 0xFF000000 | (_gr_vertices[i].gouraud.b>>8);
              _gr_vertices[i].d3d.oow = (float) _gr_vertices[i].any.iz * _ZPrecision;
           }
           break;

        case GR_POLY_FLAT: {
           int color = 0xFF000000 | (_gr_polygon.dwColor.color >> 8);

           for(i=0;i<_gr_polygon.nVertices;i++) {
              _gr_vertices[i].any.x += _gr_nScreenOriginX;
              _gr_vertices[i].any.y += _gr_nScreenOriginY;
              _gr_vertices[i].d3d.oow = (float) _gr_vertices[i].any.iz * _ZPrecision;
              _gr_bump_vertices[i].u = (float)_gr_vertices[i].bump.u *
                                 ((STextDB *)_gr_polygon.hBump)->invW65536;
              _gr_bump_vertices[i].v = (float)_gr_vertices[i].bump.v *
                                 ((STextDB *)_gr_polygon.hBump)->invH65536;
              _gr_vertices[i].d3d.d3dcolor.color = color;
           }

           break;
        }

        case GR_POLY_TRANSPARENT: {
           int color = _gr_polygon.dwColor.color | (_gr_polygon.dwOpacity << 24);

           for(i=0;i<_gr_polygon.nVertices;i++) {
              _gr_vertices[i].any.x += _gr_nScreenOriginX;
              _gr_vertices[i].any.y += _gr_nScreenOriginY;
              _gr_vertices[i].d3d.oow = (float) _gr_vertices[i].any.iz * _ZPrecision;
              _gr_bump_vertices[i].u = (float)_gr_vertices[i].bump.u *
                                 ((STextDB *)_gr_polygon.hBump)->invW65536;
              _gr_bump_vertices[i].v = (float)_gr_vertices[i].bump.v *
                                 ((STextDB *)_gr_polygon.hBump)->invH65536;
              _gr_vertices[i].d3d.d3dcolor.color = color;
           }

           break;
        }

        default:
           for(i=0;i<_gr_polygon.nVertices;i++) {
              _gr_vertices[i].any.x += _gr_nScreenOriginX;
              _gr_vertices[i].any.y += _gr_nScreenOriginY;
             _gr_bump_vertices[i].u = (float)_gr_vertices[i].bump.u *
                                 ((STextDB *)_gr_polygon.hBump)->invW65536;
             _gr_bump_vertices[i].v = (float)_gr_vertices[i].bump.v *
                                 ((STextDB *)_gr_polygon.hBump)->invH65536;
             _gr_vertices[i].d3d.oow = (float) _gr_vertices[i].any.iz * _ZPrecision;
             _gr_vertices[i].d3d.d3dcolor.color = 0xFFFFFFFF;
           }
     }

     if ((verC = Clip2DPolygonBump(&start, &startB)) == 0) return TRUE;
  }
  else {
     switch (_gr_polygon.dwFullType) {
        case GR_POLY_GOURAUD: {
           //_d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, 0);
           unsigned char *rG = _GouraudTable + (long)_gr_polygon.rgbColor.r*256;
           unsigned char *gG = _GouraudTable + (long)_gr_polygon.rgbColor.g*256;
           unsigned char *bG = _GouraudTable + (long)_gr_polygon.rgbColor.b*256;

           for(i=0;i<_gr_polygon.nVertices;i++) {
              int br = _gr_vertices[i].gouraud.b>>12;

              _gr_vertices[i].any.x += _gr_nScreenOriginX;
              _gr_vertices[i].any.y += _gr_nScreenOriginY;

              _gr_vertices[i].d3d.rgbColor.r = rG[br];
              _gr_vertices[i].d3d.rgbColor.g = gG[br];
              _gr_vertices[i].d3d.rgbColor.b = bG[br];
              _gr_vertices[i].d3d.rgbColor.a = 0xFF;

              _gr_vertices[i].d3d.oow = (float) _gr_vertices[i].any.iz * _ZPrecision;
           }
           break;
        }
        case GR_POLY_GOURAUD_RGB: {
           //_d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, 0);
           for(i=0;i<_gr_polygon.nVertices;i++) {
              _gr_vertices[i].any.x += _gr_nScreenOriginX;
              _gr_vertices[i].any.y += _gr_nScreenOriginY;
              _gr_vertices[i].d3d.oow = (float) _gr_vertices[i].any.iz * _ZPrecision;
              _gr_vertices[i].d3d.d3dcolor.color |= 0xFF000000;
           }
           break;
        }

        case GR_POLY_TEXTURE_ALPHA: {
           int color = _gr_polygon.dwColor.color | (_gr_polygon.dwOpacity << 24);

           for(i=0;i<_gr_polygon.nVertices;i++) {
              _gr_vertices[i].any.x += _gr_nScreenOriginX;
              _gr_vertices[i].any.y += _gr_nScreenOriginY;
              _gr_vertices[i].d3d.u = (float)_gr_vertices[i].texture.u *
                                 ((STextDB *)_gr_polygon.hTexture)->invW65536;
              _gr_vertices[i].d3d.v = (float)_gr_vertices[i].texture.v *
                                 ((STextDB *)_gr_polygon.hTexture)->invH65536;
              _gr_vertices[i].d3d.oow = (float) _gr_vertices[i].any.iz * _ZPrecision;
              _gr_vertices[i].d3d.d3dcolor.color = color;
           }
           break;
        }

        case GR_POLY_TEXTURE_PERSP:
        case GR_POLY_TEXTURE_LIN:
        case GR_POLY_TEXTURE_SMP:
        case GR_POLY_SPRITE_PERSP:
        case GR_POLY_SPRITE_LIN:
        case GR_POLY_SPRITE_MIP:
           for(i=0;i<_gr_polygon.nVertices;i++) {
              _gr_vertices[i].any.x += _gr_nScreenOriginX;
              _gr_vertices[i].any.y += _gr_nScreenOriginY;
              _gr_vertices[i].d3d.u = (float)_gr_vertices[i].texture.u *
                                 ((STextDB *)_gr_polygon.hTexture)->invW65536;
              _gr_vertices[i].d3d.v = (float)_gr_vertices[i].texture.v *
                                 ((STextDB *)_gr_polygon.hTexture)->invH65536;
              _gr_vertices[i].d3d.oow = (float) _gr_vertices[i].any.iz * _ZPrecision;
              _gr_vertices[i].d3d.d3dcolor.color = 0xFFFFFFFF;
           }
           break;

        case GR_POLY_TEXTURE_GOURAUD:
           for(i=0;i<_gr_polygon.nVertices;i++) {
              _gr_vertices[i].any.x += _gr_nScreenOriginX;
              _gr_vertices[i].any.y += _gr_nScreenOriginY;
             _gr_vertices[i].d3d.u = (float)_gr_vertices[i].texture.u *
                                 ((STextDB *)_gr_polygon.hTexture)->invW65536;
             _gr_vertices[i].d3d.v = (float)_gr_vertices[i].texture.v *
                                 ((STextDB *)_gr_polygon.hTexture)->invH65536;
             _gr_vertices[i].d3d.d3dcolor.color = 0xFF000000 | (_gr_vertices[i].gouraud.b>>8);
             _gr_vertices[i].d3d.oow = (float) _gr_vertices[i].any.iz * _ZPrecision;
           }
           break;

        case GR_POLY_FLAT: {
           int color = 0xFF000000 | (_gr_polygon.dwColor.color >> 8);

           for(i=0;i<_gr_polygon.nVertices;i++) {
              _gr_vertices[i].any.x += _gr_nScreenOriginX;
              _gr_vertices[i].any.y += _gr_nScreenOriginY;
              _gr_vertices[i].d3d.oow = (float) _gr_vertices[i].any.iz * _ZPrecision;
              _gr_vertices[i].d3d.d3dcolor.color = color;
           }

           break;
        }

        case GR_POLY_TRANSPARENT: {
           int color = _gr_polygon.dwColor.color | (_gr_polygon.dwOpacity << 24);

           for(i=0;i<_gr_polygon.nVertices;i++) {
              _gr_vertices[i].any.x += _gr_nScreenOriginX;
              _gr_vertices[i].any.y += _gr_nScreenOriginY;
              _gr_vertices[i].d3d.oow = (float) _gr_vertices[i].any.iz * _ZPrecision;
              _gr_vertices[i].d3d.d3dcolor.color = color;
           }

           break;
        }

        default:
           //_d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, 0);
           for(i=0;i<_gr_polygon.nVertices;i++) {
              _gr_vertices[i].any.x += _gr_nScreenOriginX;
              _gr_vertices[i].any.y += _gr_nScreenOriginY;
              _gr_vertices[i].d3d.oow = (float) _gr_vertices[i].any.iz * _ZPrecision;
              _gr_vertices[i].d3d.d3dcolor.color = 0xFF000000 | _gr_polygon.dwColor.color;//0xFFFFFFFF;
           }
     }

          /*
           for(i=0;i<_gr_polygon.nVertices;i++) {
           if( _gr_vertices[i].d3d.oow < 1./600. ) {
             GRTerminateDevice();
             return FALSE;
           }
           }
           */

     if ((verC = Clip2DPolygon(&start)) == 0) return TRUE;
  }


  //if (_dL.currDevice->fogCurrent == GR_FOG_VERTEX) {
     if (_gr_polygon.dwAddType & GR_POLY_ADD_HAZE) {
        for(i=0;i<verC;i++) {
           start[i].d3d.x = (float)start[i].any.x;
           start[i].d3d.y = (float)start[i].any.y;
           float iz = start[i].d3d.oow;
           start[i].d3d.z = 1.0 - iz * _gr_fFrontClip;//_hazeK / iz;
           //vertex fog


           if (iz >= _ifogStart) start[i].d3d.specular = 0xFF000000;
           else {
              if (iz <= _ifogEnd)
                 start[i].d3d.specular = 0;
              else {
                 int br =  (255.0*(iz - _ifogEnd))/(_ifogStart-_ifogEnd);
                 if (br < 0) br = 0;
                 else if (br > 255) br = 255;
                 start[i].d3d.specular = (br<<24);
              }
           }

        }
     }
     else {
        for(i=0;i<verC;i++) {
           start[i].d3d.x = (float)start[i].any.x;
           start[i].d3d.y = (float)start[i].any.y;
           start[i].d3d.z = 1.0 - start[i].d3d.oow * _gr_fFrontClip;//_hazeK / start[i].d3d.oow;
           start[i].d3d.specular = 0xFF000000;
        }
     }
  //}
  //else {
  //   for(i=0;i<verC;i++) {
  //      start[i].d3d.x = (float)start[i].any.x;
  //      start[i].d3d.y = (float)start[i].any.y;
  //      start[i].d3d.z = _hazeK / start[i].d3d.oow;
  //   }
 // }

  //_d3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
  //precalc light textures

  //C = f*Cother;
  //Alpha = f*alpha
  /*
  if (_bufLen < 871) {
     memcpy(_bufV[_bufLen],  start, verC*32);
     _bufC[_bufLen] = verC;

     if (_gr_polygon.dwFullType == GR_POLY_FLAT ||
         _gr_polygon.dwFullType == GR_POLY_GOURAUD ||
         _gr_polygon.dwFullType == GR_POLY_TRANSPARENT)
        _bufH[_bufLen] = NULL;
      else
        _bufH[_bufLen] = _gr_polygon.hTexture;

     _bufLen++;
  }
  else {
     for(i = 0;i < _bufLen;i++) {
        if (_bufH[i] == NULL) {
        if (_CurrentText != 0) {
        _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, 0);
        _CurrentText = 0;
        }
        }
        else
           LoadTextureTo3DCard(_bufH[i]);

        _d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                    (LPVOID) _bufV[i], _bufC[i], D3DDP_DONOTCLIP);
     }

     _bufLen = 1;

     memcpy(_bufV[0],  start, verC*32);
     _bufC[0] = verC;

     if (_gr_polygon.dwFullType == GR_POLY_FLAT ||
         _gr_polygon.dwFullType == GR_POLY_GOURAUD ||
         _gr_polygon.dwFullType == GR_POLY_TRANSPARENT)
        _bufH[0] = NULL;
      else
        _bufH[0] = _gr_polygon.hTexture;

  }
  */
  //*
  switch (_gr_polygon.dwFullType) {
     case GR_POLY_FLAT:
     case GR_POLY_GOURAUD:
     case GR_POLY_GOURAUD_RGB:
        if (_CurrentText != 0) {
           _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, 0);
           _CurrentText = 0;
        }

        _d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                    (LPVOID) start, verC, D3DDP_DONOTCLIP);
     break;

     case GR_POLY_TRANSPARENT:
        if (_CurrentText != 0) {
           _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, 0);
           _CurrentText = 0;
        }

        _d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
        _d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                    (LPVOID) start, verC, D3DDP_DONOTCLIP);
        _d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
     break;

     case GR_POLY_SPRITE_MIP:
        _d3dDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, TRUE);
        if ( LoadTextureTo3DCard(_gr_polygon.hTexture) != TRUE) return FALSE;

        _d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                    (LPVOID) start, verC, D3DDP_DONOTCLIP);

        _d3dDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
     break;

     case GR_POLY_SPRITE_PERSP:
     case GR_POLY_SPRITE_LIN:
        _d3dDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, TRUE);
        if (!(((STextDB*)_gr_polygon.hTexture)->flags & TEXTURE_ALPHA)) {
           _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_NEAREST);
           _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_NEAREST);
        }

        if ( LoadTextureTo3DCard(_gr_polygon.hTexture) != TRUE) return FALSE;

        _d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                    (LPVOID) start, verC, D3DDP_DONOTCLIP);

        if (!(((STextDB*)_gr_polygon.hTexture)->flags & TEXTURE_ALPHA)) {
           _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_LINEAR);
           _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_LINEAR);
        }
        _d3dDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
     break;

     case GR_POLY_TEXTURE_PERSP:
     case GR_POLY_TEXTURE_LIN:
     case GR_POLY_TEXTURE_GOURAUD:
        if ( LoadTextureTo3DCard(_gr_polygon.hTexture) != TRUE) return FALSE;

        _d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                    (LPVOID) start, verC, D3DDP_DONOTCLIP);
     break;

     case GR_POLY_TEXTURE_SMP:
        _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_NEAREST);
        _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_NEAREST);

        if ( LoadTextureTo3DCard(_gr_polygon.hTexture) != TRUE) return FALSE;

        _d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                    (LPVOID) start, verC, D3DDP_DONOTCLIP);

        _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_LINEAR);
        _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_LINEAR);
     break;

     case GR_POLY_TEXTURE_ALPHA:
        if ( LoadTextureTo3DCard(_gr_polygon.hTexture) != TRUE) return FALSE;

        _d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
        _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);

        _d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                    (LPVOID) start, verC, D3DDP_DONOTCLIP);

        _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
        _d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
     break;
  }

  //_d3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT);


  //Draw Add Types
  //if (!_gr_polygon.dwAddType) return TRUE;

  if (_gr_polygon.dwAddType & GR_POLY_ADD_BUMP) {
     _d3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
     _d3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
     _d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);

     #if USE_Z_BUFFER
        //_d3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, FALSE);
        //_d3dDevice->SetRenderState(D3DRENDERSTATE_ZBIAS, 1);
       _d3dDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
     #endif

     if ( LoadTextureTo3DCard(_gr_polygon.hBump) != TRUE) return FALSE;

     for (i = 0;i < verC;i++) {
         COPY_FLOAT(start[i].d3d.u, startB[i].u);
         COPY_FLOAT(start[i].d3d.v, startB[i].v);
         start[i].d3d.specular = 0xFF000000;

         float izb = start[i].d3d.oow;

         if (izb >= _ibumpStart) start[i].d3d.d3dcolor.color = 0xFF646464;
         else {
            if (izb <= _ibumpEnd)
               start[i].d3d.d3dcolor.color = 0xFF000000;
            else {
               unsigned int br =
                 (unsigned char)(((100.0*(izb - _ibumpEnd))/(_ibumpStart-_ibumpEnd)));
               start[i].d3d.d3dcolor.color = 0xFF000000 | (br<<16)|(br<<8)|br;
            }
         }

	 }

     _d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                    (LPVOID) start, verC, D3DDP_DONOTCLIP);

     _d3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
     _d3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
     #if USE_Z_BUFFER
        //_d3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
        //_d3dDevice->SetRenderState(D3DRENDERSTATE_ZBIAS, 0);

        _d3dDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
     #endif
     _d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);

  }


  if (_gr_polygon.nLights != 0) {
     int prevBlack = 1;

     D3D_NEW_PreparePolygonForLight(start, verC, _gr_polygon.nLights);

     _d3dDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, FALSE);
     _d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
     #if USE_Z_BUFFER
        //_d3dDevice->SetRenderState(D3DRENDERSTATE_ZBIAS, 1);
        _d3dDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
        _d3dDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);

        //_d3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, FALSE);
     #endif

     for(i = 0;i < LIGHT_SOURCE_COUNT;i++) {
        if (_gr_pLights[i].draw == 0) continue;

        if ( LoadTextureTo3DCard(_gr_pLights[i].handler) != TRUE) return FALSE;

        if (_gr_pLights[i].color == LIGHT_COLOR_BLACK) {
           if (!prevBlack) {
              _d3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
              _d3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
              prevBlack = 1;
           }

        }
        else {
           if (prevBlack) {
              _d3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
              _d3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
              prevBlack = 0;
           }
        }

        //if (lH == NULL) return FALSE;

        _d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                    (LPVOID) _gr_pLights[i].points,
                    _gr_pLights[i].qnty, D3DDP_DONOTCLIP);
     }

     if (!prevBlack) {
        _d3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
        _d3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
     }
     _d3dDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);

     #if USE_Z_BUFFER
        //_d3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
        //_d3dDevice->SetRenderState(D3DRENDERSTATE_ZBIAS, 0);

        _d3dDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
        _d3dDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
     #endif
     _d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);

  }
  //*/
  return TRUE;
}

/***************************************************************************/

/***********************************************************************
 ********************      GR Application Init      ********************
 ***********************************************************************/

/***********************************************************************
 ********************        GRInitTables           ********************
 ***********************************************************************/


extern "C" int D3D_GRInitTables(unsigned long mmx)
{ int i,j;

  (void)mmx;
  //x, y, z, oow, [a,r,g,b], spec, u, v
  //------------------------------------------
  _HClipVertexFTable[GR_POLY_FLAT] = HClip;
  _VClipVertexFTable[GR_POLY_FLAT] = VClip;
  _HClipVertexFTable[GR_POLY_GOURAUD] = HClipRGBZ;
  _VClipVertexFTable[GR_POLY_GOURAUD] = VClipRGBZ;
  _HClipVertexFTable[GR_POLY_GOURAUD_RGB] = HClipRGBZ;
  _VClipVertexFTable[GR_POLY_GOURAUD_RGB] = VClipRGBZ;
  _HClipVertexFTable[GR_POLY_TEXTURE_PERSP] = HClipSTZ;
  _VClipVertexFTable[GR_POLY_TEXTURE_PERSP] = VClipSTZ;
  _HClipVertexFTable[GR_POLY_TEXTURE_LIN] = HClipSTZ;
  _VClipVertexFTable[GR_POLY_TEXTURE_LIN] = VClipSTZ;
  _HClipVertexFTable[GR_POLY_TEXTURE_SMP] = HClipSTZ;
  _VClipVertexFTable[GR_POLY_TEXTURE_SMP] = VClipSTZ;
  _HClipVertexFTable[GR_POLY_SPRITE_MIP] = HClipSTZ;
  _VClipVertexFTable[GR_POLY_SPRITE_MIP] = VClipSTZ;
  _HClipVertexFTable[GR_POLY_SPRITE_PERSP] = HClipSTZ;
  _VClipVertexFTable[GR_POLY_SPRITE_PERSP] = VClipSTZ;
  _HClipVertexFTable[GR_POLY_TRANSPARENT] = HClip;
  _VClipVertexFTable[GR_POLY_TRANSPARENT] = VClip;
  _HClipVertexFTable[GR_POLY_TEXTURE_ALPHA] = HClipSTZ;
  _VClipVertexFTable[GR_POLY_TEXTURE_ALPHA] = VClipSTZ;
  _HClipVertexFTable[GR_POLY_TEXTURE_GOURAUD] = HClipRGBSTZ;
  _VClipVertexFTable[GR_POLY_TEXTURE_GOURAUD] = VClipRGBSTZ;
  _HClipVertexFTable[GR_POLY_SPRITE_LIN] = HClipSTZ;
  _VClipVertexFTable[GR_POLY_SPRITE_LIN] = VClipSTZ;

  //------------------------------------------
  //
  //------------Set GouraudTable------------------------
  for(j = 0;j < 256;j++)
     for(i = 0;i < 256;i++) {
        int l = (j*i)/128;
        _GouraudTable[j*256+i] = (unsigned char) ((l > 255)?255:l);
     }

    //*************************************************************
    /*
     * Set default render state
     */

  SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, FALSE);
  #if USE_Z_BUFFER
      SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, TRUE);
      SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
      SET_RENDER_STATE(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
  #endif
  SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE);
  SET_RENDER_STATE(D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
  SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_LINEAR);
  SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_LINEAR);
  SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE); //!!!!
  SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
  SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
  SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
  SET_RENDER_STATE(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
  SET_RENDER_STATE(D3DRENDERSTATE_DITHERENABLE, TRUE);
  SET_RENDER_STATE(D3DRENDERSTATE_SPECULARENABLE, FALSE);
  SET_RENDER_STATE(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
  SET_RENDER_STATE(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
  // make sure ambient light is off
  SET_LIGHT_STATE(D3DLIGHTSTATE_AMBIENT, 0);

  return TRUE;
}

/***********************************************************************
 ********************        GRSetClipRect          ********************
 ***********************************************************************/

extern "C" void D3D_GR_SetClipRect()
{
    _gr_ilyaClipI.left   = _gr_clipRect.left   + _gr_nScreenOriginX;
    _gr_ilyaClipI.right  = _gr_clipRect.right  + _gr_nScreenOriginX;
    _gr_ilyaClipI.top    = _gr_clipRect.top    + _gr_nScreenOriginY;
    _gr_ilyaClipI.bottom = _gr_clipRect.bottom + _gr_nScreenOriginY;

    _gr_ilyaClipF.left   = _gr_ilyaClipI.left;
    _gr_ilyaClipF.right  = _gr_ilyaClipI.right;
    _gr_ilyaClipF.top    = _gr_ilyaClipI.top;
    _gr_ilyaClipF.bottom = _gr_ilyaClipI.bottom;
}


extern "C" void D3D_GRSetBump(int start, int end)
{
    _ibumpStart = 1.0 / start;
    _ibumpEnd = 1.0 / end;
}

/***********************************************************************
 ********************          GRSetHaze            ********************
 ***********************************************************************/

SGRColorDef *_hD = NULL;

extern "C" int D3D_GRSetHaze(int start, int len, SGRColorDef *hD)
{   //unsigned char nCol = CPaletteTranslator::HazeColorIdx();
    D3DCOLOR hazeCol = MY_RGB(hD->r,hD->g,hD->b);
                      //MY_RGB(_Palette[nCol].peRed, _Palette[nCol].peGreen,
                       //_Palette[nCol].peBlue);
    //D3DCOLOR hazeCol = (color>>8);

    _hD = hD;

    _fogEnd = start+len;
    _fogStart = start;//(start+len)*.8;
    _ifogEnd = 1.0 / _fogEnd;
    _ifogStart = 1.0 / _fogStart;
    //_fogOtn = _fogStart * _fogEnd;

    switch (_dL.currDevice->fogCurrent) {
       case GR_FOG_NONE:
          SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, FALSE);
          break;

       case GR_FOG_TABLE:
          SET_RENDER_STATE(D3DRENDERSTATE_FOGCOLOR, hazeCol);
          SET_RENDER_STATE(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_LINEAR);
          SET_RENDER_STATE(D3DRENDERSTATE_FOGTABLESTART, *((unsigned long *)(&_fogStart)));
          SET_RENDER_STATE(D3DRENDERSTATE_FOGTABLEEND, *((unsigned long *)(&_fogEnd)));
          SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, TRUE);
          break;

       case GR_FOG_VERTEX:
          SET_RENDER_STATE(D3DRENDERSTATE_FOGCOLOR, hazeCol);
          //SET_LIGHT_STATE(D3DLIGHTSTATE_FOGMODE, D3DFOG_LINEAR);
          SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, TRUE);
          //SET_LIGHT_STATE(D3DLIGHTSTATE_FOGDENSITY, *((unsigned long *)(&fg)));
          //SET_LIGHT_STATE(D3DLIGHTSTATE_FOGSTART, *((unsigned long *)(&fs)));//(float)start/(float)(start+len));
          //SET_LIGHT_STATE(D3DLIGHTSTATE_FOGEND, *((unsigned long *)(&fe)));//(float)start/(float)(start+len));
          //SET_LIGHT_STATE(D3DLIGHTSTATE_FOGMODE, D3DFOG_LINEAR);
          break;
    }

    _hazeK = 0.9 / _fogEnd;


    return TRUE;
}

/***********************************************************************
 ********************       GRSInitTextureDB        ********************
 ***********************************************************************/

extern "C" void GRInitTextureDB()
{ int i;

  //Release texture DB
  if (_memory != NULL) {
     for(i = 0;i < MAX_TEXTURE_COUNT;i++) {
        if (_memory[i].text != NULL) {
           RELEASE(_memory[i].text->lpTextureSurfDev);
           RELEASE(_memory[i].text->lpTextureDev);
           _memory[i].text = NULL;
        }
     }
  }

  for(i = 0;i < _TextCounter;i++) {
     if (_dL.currDevice->swHw == GR_SOFTWARE) {
        delete [] _TextDB[i].dataPtr;
        _TextDB[i].dataPtr = NULL;
        if (_TextDB[i].textureCache != NULL) {
           delete [] (_TextDB[i].textureCache - _TextDB[i].h*2);
           _TextDB[i].textureCache = NULL;
        }

     }
     else {
        RELEASE(_TextDB[i].lpTextureSurfMem);
        RELEASE(_TextDB[i].lpTextureMem);
        _TextDB[i].handleDev = 0;
     }
  }

  _memoryFree = 0;
  _D3DMemUse = NULL;
  _DeviceMemory = 0;
  _TextTime = 0;
  _CurrentText = 0;

  _TextCounter = 0;

  if (_memory == NULL) {
     _memory = new STextMem[MAX_TEXTURE_COUNT];
  }

  for(i = 0;i < MAX_TEXTURE_COUNT;i++)
     _memory[i].text = NULL;
  //_memory = new STextMem[MAX_TEXTURE_COUNT];

}

extern "C" void D3D_GRDrawSprite(int x0, int y0, int x1, int y1, int u0, int v0, int u1, int v1, int iz,void *hTex)
{ UGRVertex sprV[4];
  float oow = (float)iz * I65536;
  float z = 1.0 - oow * _gr_fFrontClip;//_hazeK / oow;
  unsigned long specular = 0xFF000000;

  if (x0 >= _gr_clipRect.right || x1 < _gr_clipRect.left ||
      y0 >= _gr_clipRect.bottom || y1 < _gr_clipRect.top)
     return;

  //if (_dL.currDevice->fogCurrent == GR_FOG_VERTEX) {
     if (oow < _ifogStart) {
        if (oow <= _ifogEnd)
           specular = 0;
        else {
           int br =  (255.0*(oow - _ifogEnd))/(_ifogStart-_ifogEnd);
           if (br < 0) br = 0;
           else if (br > 255) br = 255;
           specular = (br<<24);
        }
     }
  //}


  sprV[0].d3d.x = x0 + _gr_nScreenOriginX;
  sprV[0].d3d.y = y0 + _gr_nScreenOriginY;
  COPY_FLOAT(sprV[0].d3d.z, z);
  COPY_FLOAT(sprV[0].d3d.oow, oow);
  sprV[0].d3d.u = (float)u0 * ((STextDB *)hTex)->invW65536;
  sprV[0].d3d.v = (float)v0 * ((STextDB *)hTex)->invH65536;
  sprV[0].d3d.d3dcolor.color = 0xFFFFFFFF;
  sprV[0].d3d.specular = specular;

  COPY_FLOAT(sprV[1].d3d.x, sprV[0].d3d.x);
  sprV[1].d3d.y = y1 + _gr_nScreenOriginY;
  COPY_FLOAT(sprV[1].d3d.z, z);
  COPY_FLOAT(sprV[1].d3d.oow, oow);
  COPY_FLOAT(sprV[1].d3d.u, sprV[0].d3d.u);
  sprV[1].d3d.v = (float)v1 * ((STextDB *)hTex)->invH65536;
  sprV[1].d3d.d3dcolor.color = 0xFFFFFFFF;
  sprV[1].d3d.specular = specular;

  sprV[2].d3d.x = x1 + _gr_nScreenOriginX;
  COPY_FLOAT(sprV[2].d3d.y, sprV[1].d3d.y);
  COPY_FLOAT(sprV[2].d3d.z, z);
  COPY_FLOAT(sprV[2].d3d.oow, oow);
  sprV[2].d3d.u = (float)u1 * ((STextDB *)hTex)->invW65536;
  COPY_FLOAT(sprV[2].d3d.v, sprV[1].d3d.v);
  sprV[2].d3d.d3dcolor.color = 0xFFFFFFFF;
  sprV[2].d3d.specular = specular;

  COPY_FLOAT(sprV[3].d3d.x, sprV[2].d3d.x);
  COPY_FLOAT(sprV[3].d3d.y, sprV[0].d3d.y);
  COPY_FLOAT(sprV[3].d3d.z, z);
  COPY_FLOAT(sprV[3].d3d.oow, oow);
  COPY_FLOAT(sprV[3].d3d.u, sprV[2].d3d.u);
  COPY_FLOAT(sprV[3].d3d.v, sprV[0].d3d.v);
  sprV[3].d3d.d3dcolor.color = 0xFFFFFFFF;
  sprV[3].d3d.specular = specular;

  if (x0 < _gr_clipRect.left) {
     sprV[0].d3d.u = ((_gr_ilyaClipF.left - sprV[0].d3d.x) * (sprV[2].d3d.u - sprV[0].d3d.u)) /
                     (sprV[2].d3d.x - sprV[0].d3d.x) + sprV[0].d3d.u;
     COPY_FLOAT(sprV[0].d3d.x, _gr_ilyaClipF.left);
     COPY_FLOAT(sprV[1].d3d.x, _gr_ilyaClipF.left);
     COPY_FLOAT(sprV[1].d3d.u, sprV[0].d3d.u);
  }

  if (x1 > _gr_clipRect.right) {
     sprV[2].d3d.u = ((_gr_ilyaClipF.right - sprV[0].d3d.x) * (sprV[2].d3d.u - sprV[0].d3d.u)) /
                     (sprV[2].d3d.x - sprV[0].d3d.x) + sprV[0].d3d.u;
     COPY_FLOAT(sprV[2].d3d.x, _gr_ilyaClipF.right);
     COPY_FLOAT(sprV[3].d3d.x, _gr_ilyaClipF.right);
     COPY_FLOAT(sprV[3].d3d.u, sprV[2].d3d.u);
  }

  if (y0 < _gr_clipRect.top) {
     sprV[0].d3d.v = ((_gr_ilyaClipF.top - sprV[0].d3d.y) * (sprV[1].d3d.v - sprV[0].d3d.v)) /
                     (sprV[1].d3d.y - sprV[0].d3d.y) + sprV[0].d3d.v;
     COPY_FLOAT(sprV[0].d3d.y, _gr_ilyaClipF.top);
     COPY_FLOAT(sprV[3].d3d.y, _gr_ilyaClipF.top);
     COPY_FLOAT(sprV[3].d3d.v, sprV[0].d3d.v);
  }

  if (y1 > _gr_clipRect.bottom) {
     sprV[1].d3d.v = ((_gr_ilyaClipF.bottom - sprV[0].d3d.y) * (sprV[1].d3d.v - sprV[0].d3d.v)) /
                     (sprV[1].d3d.y - sprV[0].d3d.y) + sprV[0].d3d.v;
     COPY_FLOAT(sprV[1].d3d.y, _gr_ilyaClipF.bottom);
     COPY_FLOAT(sprV[2].d3d.y, _gr_ilyaClipF.bottom);
     COPY_FLOAT(sprV[2].d3d.v, sprV[1].d3d.v);
  }

  //*
  _d3dDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, TRUE);
  if ( LoadTextureTo3DCard(hTex) != TRUE) return;

  if (((STextDB*)hTex)->flags & TEXTURE_ALPHA) {
     _d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);

     //_d3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
     //_d3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
  }
  else {
     _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_NEAREST);
     _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_NEAREST);
  }

  _d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                    (LPVOID) sprV, 4, D3DDP_DONOTCLIP);

  if (((STextDB*)hTex)->flags & TEXTURE_ALPHA) {
     _d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
     //_d3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
     //_d3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
  }
  else {
     _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_LINEAR);
     _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_LINEAR);
  }
  _d3dDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);

  //*/
}

extern "C" void  D3D_GRDrawAlphaSprite(SGRAlphaSprite *sm)
{ UGRVertex *sprV;//[4];
  D3D_ZList *zl;
  float oow = (float)sm->iz * I65536;
  float z = 1.0 - oow * _gr_fFrontClip;//_hazeK / oow;
  unsigned long specular = 0xFF000000;
  unsigned long color = (sm->opacity << 24) | (sm->color);

  if (sm->x0 >= _gr_clipRect.right || sm->x1 < _gr_clipRect.left ||
      sm->y0 >= _gr_clipRect.bottom || sm->y1 < _gr_clipRect.top)
     return;

  //if (_dL.currDevice->fogCurrent == GR_FOG_VERTEX) {
     if (oow < _ifogStart) {
        if (oow <= _ifogEnd)
           specular = 0;
        else {
           int br =  (255.0*(oow - _ifogEnd))/(_ifogStart-_ifogEnd);
           if (br < 0) br = 0;
           else if (br > 255) br = 255;
           specular = (br<<24);
        }
     }
  //}

  zl = D3D_InsertToZList(z);
  sprV = zl->v;
  zl->type = D3DTBLEND_MODULATEALPHA;
  zl->texture = sm->hTexture;



  sprV[0].d3d.x = sm->x0 + _gr_nScreenOriginX;
  sprV[0].d3d.y = sm->y0 + _gr_nScreenOriginY;
  COPY_FLOAT(sprV[0].d3d.z, z);
  COPY_FLOAT(sprV[0].d3d.oow, oow);
  sprV[0].d3d.u = (float)sm->u0 * ((STextDB *)sm->hTexture)->invW65536;
  sprV[0].d3d.v = (float)sm->v0 * ((STextDB *)sm->hTexture)->invH65536;
  sprV[0].d3d.specular = specular;
  sprV[0].d3d.d3dcolor.color = color;

  COPY_FLOAT(sprV[1].d3d.x, sprV[0].d3d.x);
  sprV[1].d3d.y = sm->y1 + _gr_nScreenOriginY;
  COPY_FLOAT(sprV[1].d3d.z, z);
  COPY_FLOAT(sprV[1].d3d.oow, oow);
  COPY_FLOAT(sprV[1].d3d.u, sprV[0].d3d.u);
  sprV[1].d3d.v = (float)sm->v1 * ((STextDB *)sm->hTexture)->invH65536;
  sprV[1].d3d.specular = specular;
  sprV[1].d3d.d3dcolor.color = color;

  sprV[2].d3d.x = sm->x1 + _gr_nScreenOriginX;
  COPY_FLOAT(sprV[2].d3d.y, sprV[1].d3d.y);
  COPY_FLOAT(sprV[2].d3d.z, z);
  COPY_FLOAT(sprV[2].d3d.oow, oow);
  sprV[2].d3d.u = (float)sm->u1 * ((STextDB *)sm->hTexture)->invW65536;
  COPY_FLOAT(sprV[2].d3d.v, sprV[1].d3d.v);
  sprV[2].d3d.specular = specular;
  sprV[2].d3d.d3dcolor.color = color;

  COPY_FLOAT(sprV[3].d3d.x, sprV[2].d3d.x);
  COPY_FLOAT(sprV[3].d3d.y, sprV[0].d3d.y);
  COPY_FLOAT(sprV[3].d3d.z, z);
  COPY_FLOAT(sprV[3].d3d.oow, oow);
  COPY_FLOAT(sprV[3].d3d.u, sprV[2].d3d.u);
  COPY_FLOAT(sprV[3].d3d.v, sprV[0].d3d.v);
  sprV[3].d3d.specular = specular;
  sprV[3].d3d.d3dcolor.color = color;

  if (sm->x0 < _gr_clipRect.left) {
     sprV[0].d3d.u = ((_gr_ilyaClipF.left - sprV[0].d3d.x) * (sprV[2].d3d.u - sprV[0].d3d.u)) /
                     (sprV[2].d3d.x - sprV[0].d3d.x) + sprV[0].d3d.u;
     COPY_FLOAT(sprV[0].d3d.x, _gr_ilyaClipF.left);
     COPY_FLOAT(sprV[1].d3d.x, _gr_ilyaClipF.left);
     COPY_FLOAT(sprV[1].d3d.u, sprV[0].d3d.u);
  }

  if (sm->x1 > _gr_clipRect.right) {
     sprV[2].d3d.u = ((_gr_ilyaClipF.right - sprV[0].d3d.x) * (sprV[2].d3d.u - sprV[0].d3d.u)) /
                     (sprV[2].d3d.x - sprV[0].d3d.x) + sprV[0].d3d.u;
     COPY_FLOAT(sprV[2].d3d.x, _gr_ilyaClipF.right);
     COPY_FLOAT(sprV[3].d3d.x, _gr_ilyaClipF.right);
     COPY_FLOAT(sprV[3].d3d.u, sprV[2].d3d.u);
  }

  if (sm->y0 < _gr_clipRect.top) {
     sprV[0].d3d.v = ((_gr_ilyaClipF.top - sprV[0].d3d.y) * (sprV[1].d3d.v - sprV[0].d3d.v)) /
                     (sprV[1].d3d.y - sprV[0].d3d.y) + sprV[0].d3d.v;
     COPY_FLOAT(sprV[0].d3d.y, _gr_ilyaClipF.top);
     COPY_FLOAT(sprV[3].d3d.y, _gr_ilyaClipF.top);
     COPY_FLOAT(sprV[3].d3d.v, sprV[0].d3d.v);
  }

  if (sm->y1 > _gr_clipRect.bottom) {
     sprV[1].d3d.v = ((_gr_ilyaClipF.bottom - sprV[0].d3d.y) * (sprV[1].d3d.v - sprV[0].d3d.v)) /
                     (sprV[1].d3d.y - sprV[0].d3d.y) + sprV[0].d3d.v;
     COPY_FLOAT(sprV[1].d3d.y, _gr_ilyaClipF.bottom);
     COPY_FLOAT(sprV[2].d3d.y, _gr_ilyaClipF.bottom);
     COPY_FLOAT(sprV[2].d3d.v, sprV[1].d3d.v);
  }

  /*
  if ( LoadTextureTo3DCard(sm->hTexture) != TRUE) return;

  //_d3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
  //_d3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
  _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);

  _d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                    (LPVOID) sprV, 4, D3DDP_DONOTCLIP);

  //_d3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
  //_d3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
  _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
  */
}



extern "C" void * ASM_GRLoadTextureToDB(void * handle, unsigned char *pal, int palCnt, unsigned char *text)
{
  STextDB *tDB;
  int  i, j, h, w, flags;
  unsigned char *ptr;
  int *lookT;

  //(void)pal;

  if (handle == NULL) {
     if (_TextCounter >= MAX_TEXTURE_COUNT) {
		D3DSetError("Texture DB is Full", -1);
	 	return NULL;
	 }

     tDB = &_TextDB[_TextCounter++];

     tDB->flags = *TEXT_FLAG_PTR(text);


     tDB->counter = 0;

     h = *TEXT_HEIGHT_PTR(text);
	 w = *TEXT_WIDTH_PTR(text);

     tDB->w = w;
     tDB->h = h;
     tDB->size = w * h;

     #define FUCK_ME  h  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


     //create Texture Cache
     lookT = new int [2 * (h + FUCK_ME) + 1];

     for(i = -h-FUCK_ME; i <= h + FUCK_ME;i++)
        if (i < -h || i > h)
           lookT[i + h + FUCK_ME] = i<0?(2-h)*w:(h-2)*w;
        else
           lookT[i + h + FUCK_ME] = i*w;

     tDB->textureCache = lookT + h + FUCK_ME;

     tDB->dataPtr = (unsigned char *) new char [w * h];

  }
  else  {
     tDB = ((STextDB *) handle);
     h = tDB->h;
     w = tDB->w;
  }

  ptr = tDB->dataPtr;
  flags = tDB->flags;

  if (! (flags & TEXTURE_NOT_PRESENT)) {
     switch (flags & TEXTURE_FORMAT_MASK) {
        case TEXTURE_MEM_FORMAT:
           memcpy(ptr, text, w*h);
        break;

        case TEXTURE_PAL_FORMAT: {
           int cR[256];

           if (((flags & TEXTURE_ALPHA) && !(flags & TEXTURE_SPRITE)) || pal == NULL) {
              if (flags & TEXTURE_ALPHA) {
                 for(i = 0;i < w*h;i++)
                    ptr[i] = (unsigned char) (text[i]>>4);
              }
              else
                 memcpy(ptr, text, w*h);
           }
           else {
              for(i = 0, j = 0;i < 256*3;i += 3,j++)
                 cR[j] = epal_Match(_EPal, RGB_i(pal[i], pal[i+1], pal[i+2]));

              for(j = 0;j < h;j++)
                 for(i = 0;i < w;i++,ptr++) {
                    if ((flags & TEXTURE_SPRITE) && *text == 0)
                       *ptr = 0;
                    else
                       *ptr = (unsigned char) cR[*text];

                    text++;
                 }
           }
        }
        break;

        case TEXTURE_TXR_FORMAT: {
           int cR[4096];
           unsigned short *txr = (unsigned short*) text;

           for(i = 0,j = 0;i < palCnt*3;j++,i+=3)
              cR[j] = epal_Match( _EPal,RGB_i(pal[i], pal[i+1], pal[i+2]));

           if (flags & TEXTURE_SPRITE) {
              for(j = 0;j < h;j++)
                 for(i = 0;i < w;i++,ptr++) {
                    if (*txr == 0)
                       *ptr = 0;
                    else {
                       *ptr = (unsigned char) cR[*txr];
                    }
                    txr++;
                 }
           }
           else {
              if (flags & TEXTURE_ALPHA) {
                 for(j = 0;j < h;j++)
                    for(i = 0;i < w;i++,ptr++,txr++)
                       *ptr = (unsigned char) (*txr>>4);
              }
              else {
                 for(j = 0;j < h;j++)
                    for(i = 0;i < w;i++,ptr++) {
                       *ptr = (unsigned char) cR[*txr++];
                    }
              }
           }
        }
        break;
     }
  }

  tDB->filename[0] = 0;

  return tDB;
}

extern "C" void ASM_GRDeleteTextureFromDB(void* handle)
{ STextDB *tDB;

  if (handle == NULL) return;

  tDB = (STextDB*) handle;

  delete [] tDB->dataPtr;
  tDB->dataPtr = NULL;

  if (tDB->textureCache != NULL) {
     delete [] (tDB->textureCache - tDB->h*2);
     tDB->textureCache = NULL;
  }

}



void GRSetTextureLoadFunc( GR_HTEXTURE handle, const char *fileName, TTextureLoadFunc loadF, void *user)
{
  if (handle == NULL) return;

  strcpy(((STextDB *)handle)->filename, fileName);
  ((STextDB *)handle)->loadFunc = loadF;
  ((STextDB *)handle)->loadFuncUserPar = user;


}

void GRSetTextureLoadIntFunc( GR_HTEXTURE handle, TTextureLoadIntFunc loadF)
{
  if (handle == NULL) return;

  ((STextDB *)handle)->loadIntFunc = loadF;

}

unsigned char * D3D_LockTexture(void *handle, long *pitch)
{ DDSURFACEDESC ddsd; // DirectDraw surface description
  STextDB *tDB = (STextDB*) handle;
  int err;

  // Lock the surface so it can be filled
  //if (tDB->counter && _dL.currDevice->agp == 0) _d3dDevice->EndScene();
  memset(&ddsd, 0, sizeof(DDSURFACEDESC));
  ddsd.dwSize = sizeof(DDSURFACEDESC);
  while ((err = tDB->lpTextureSurfMem->Lock(NULL, &ddsd, 0, NULL)) != DD_OK);

  *pitch = ddsd.lPitch;

  return (unsigned char *) ddsd.lpSurface;
}

void D3D_UnLockTexture(void *handle)
{ STextDB * tDB = (STextDB*) handle;

  tDB->lpTextureSurfMem->Unlock(NULL);
  tDB->lpTextureSurfMem->QueryInterface(IID_IDirect3DTexture2,
                                (LPVOID*)&(tDB->lpTextureMem));
  if (_dL.currDevice->agp)
     tDB->lpTextureDev = tDB->lpTextureMem;
  else {
     if (tDB->counter)
        tDB->lpTextureDev->Load(tDB->lpTextureMem);
  }

  //if (tDB->counter && _dL.currDevice->agp == 0) _d3dDevice->BeginScene();

}

/***********************************************************************
 ********************       GRLoadTextureToDB       ********************
 ***********************************************************************/

int D3D_FillTexture(STextDB *tDB, STextureFormat *tF, int w, int h,
                    unsigned char *pal, int palCnt, unsigned char *text)
{
  int aSh, aSc;
  int i,j;
  unsigned char *lpCP, *lpCP1; // 8bpp
  unsigned short *lpSP; // 16/15bpp
  //unsigned long *lpLP; //32bpp
  unsigned char *saveText = text;
  DDSURFACEDESC ddsd; // DirectDraw surface description
  int err;
  int flags = tDB->flags;

  // Lock the surface so it can be filled
  memset(&ddsd, 0, sizeof(DDSURFACEDESC));
  ddsd.dwSize = sizeof(DDSURFACEDESC);
  err = tDB->lpTextureSurfMem->Lock(NULL, &ddsd, 0, NULL);
  if (err != DD_OK) {
     D3DSetError("Can't Lock Mem Surface",err);
     return 0;
  }

  // Fill the texture
  switch (flags & TEXTURE_FORMAT_MASK) {
//////////////////TEXTURE_MEM_FORMAT//////////////////////////////
     case TEXTURE_MEM_FORMAT:
        i = (tF->rgbBitCount>>3)*w;
        lpCP = (unsigned char *)ddsd.lpSurface;

        for(j = 0;j < h;j++, lpCP += ddsd.lPitch, text += i)
           memcpy(lpCP, text, i);

        tDB->size = i*h;
     break;

//////////////////TEXTURE_PAL_FORMAT//////////////////////////////
     case TEXTURE_PAL_FORMAT: {
        int cR[256];

        if (tF->usePalette) {
           if (pal != NULL) {
              if (flags & TEXTURE_SPRITE) {
                 for(i = 0, j = 0; i < 256*3;i += 3, j++)
                    cR[j] = epal_Match(_EPal, RGB_i(pal[i], pal[i+1], pal[i+2]));
                 cR[0] = 0;
              }
              else {
                 for(i = 0, j = 0; i < 256*3;i += 3, j++) {
                    int colIdx = epal_Match(_EPal, RGB_i(pal[i], pal[i+1], pal[i+2]));
                    //if (colIdx == 0)
                    //   cR[j] = _BlackColorIndex;
                    //else
                       cR[j] = colIdx;
                 }
              }
           }
           else
              for(i = 0; i < 256;i++)
                 cR[i] = i;

           lpCP = (unsigned char *)ddsd.lpSurface;

           for(j = 0;j < h;j++, lpCP += ddsd.lPitch)
              for(i = 0, lpCP1 = lpCP;i < w;i++)
                 *lpCP1++ = (unsigned char)cR[*text++];

           tDB->size = w*h;
           break;
        }

        if (flags & TEXTURE_ALPHA) {
           aSh = tF->alphaShift;
           aSc = tF->alphaScale;
           if (flags & TEXTURE_SPRITE) {
              unsigned long sprAlpha = ((255>>aSc)<<aSh);
              unsigned char *saveText = text;

              switch(tF->rgbBitCount) {
                 case 16:
                    lpCP = (unsigned char *)ddsd.lpSurface;
                    for(j = 0;j < h;j++, lpCP += ddsd.lPitch)
                       for(i = 0, lpSP = (unsigned short*) lpCP;i < w;i++)
                          if (*text == 0) {
                             unsigned short col = 0;

                             for(int m = -1;m <= 1;m++)
                                for(int n = -1;n <= 1;n++) {
                                   if (i+n >= 0 && i+n < w && j+m >= 0 && j+m < h) {
                                      if (saveText[i+n+(j+m)*w] != 0) {
                                         if (pal == NULL)
                                            col = (unsigned short)_palTo16Alpha[saveText[i+n+(j+m)*w]];
                                         else {
                                            unsigned char *palP = pal + (int)saveText[i+n+(j+m)*w]*3;
                                            col = (unsigned short)((((unsigned short)(*palP)>>tF->redScale)<<tF->redShift) |
                                                  (((unsigned short)(*(palP+1))>>tF->greenScale)<<tF->greenShift) |
                                                  (((unsigned short)(*(palP+2))>>tF->blueScale)<<tF->blueShift));
                                         }
                                         goto ok;
                                      }
                                   }
                                }
                             ok:
                             *lpSP++ = col;
                             text++;
                          }
                          else {
                             if (pal == NULL)
                                *lpSP++ = (unsigned short)(sprAlpha | _palTo16Alpha[*text++]);
                             else {
                                unsigned char *palP = pal + (int)(*text++)*3;
                                *lpSP++ = (unsigned short)((((unsigned short)(*palP)>>tF->redScale)<<tF->redShift) |
                                       (((unsigned short)(*(palP+1))>>tF->greenScale)<<tF->greenShift) |
                                       (((unsigned short)(*(palP+2))>>tF->blueScale)<<tF->blueShift) | sprAlpha);
                             }
                          }
                 break;
              }
           }
           else {
              unsigned long whiteCol = ((255>>tF->redScale)<<tF->redShift) |
                                       ((255>>tF->greenScale)<<tF->greenShift) |
                                       ((255>>tF->blueScale)<<tF->blueShift);
              switch(tF->rgbBitCount) {
                 case 16:
                    lpCP = (unsigned char *)ddsd.lpSurface;
                    for(j = 0;j < h;j++, lpCP += ddsd.lPitch)
                       for(i = 0, lpSP = (unsigned short*) lpCP;i < w;i++,text++)
                          *lpSP++ = (unsigned short)(((((unsigned long)(*text))>>aSc)<<aSh) | whiteCol);
                 break;
              }
           }

           tDB->size = 2*w*h;
           break;
        }

        if (flags & TEXTURE_SPRITE) {
           switch(tF->rgbBitCount) {
              case 16:
                 lpCP = (unsigned char *)ddsd.lpSurface;
                 for(j = 0;j < h;j++, lpCP += ddsd.lPitch)
                    for(i = 0, lpSP = (unsigned short*) lpCP;i < w;i++)
                       if (*text == 0) {
                          *lpSP++ = 0;
                          text++;
                       }
                       else {
                          if (pal == NULL)
                             *lpSP++ = (unsigned short)(_palTo16[*text++]);
                          else {
                             unsigned char *palP = pal + (int)(*text++)*3;
                             *lpSP++ = (unsigned short)((((unsigned short)(*palP)>>tF->redScale)<<tF->redShift) |
                                       (((unsigned short)(*(palP+1))>>tF->greenScale)<<tF->greenShift) |
                                       (((unsigned short)(*(palP+2))>>tF->blueScale)<<tF->blueShift));
                          }
                       }
              break;
           }
        }
        else {
           switch(tF->rgbBitCount) {
              case 16:
                 lpCP = (unsigned char *)ddsd.lpSurface;
                 for(j = 0;j < h;j++, lpCP += ddsd.lPitch)
                    for(i = 0, lpSP = (unsigned short*) lpCP;i < w;i++)
                       if (pal == NULL) {
                          unsigned short col = _palTo16[*text++];
                          //if (col == 0)
                          //   *lpSP++ = 2;
                          //else
                             *lpSP++ = col;
                       }
                       else {
                          unsigned char *palP = pal + (int)(*text++)*3;
                          //if (*palP == 0 && *(palP+1) == 0 && *(palP+2) == 0)
                          //   *lpSP++ = 2;
                          //else
                             *lpSP++ = (unsigned short)((((unsigned short)(*palP)>>tF->redScale)<<tF->redShift) |
                                       (((unsigned short)(*(palP+1))>>tF->greenScale)<<tF->greenShift) |
                                       (((unsigned short)(*(palP+2))>>tF->blueScale)<<tF->blueShift));
                       }
              break;
           }
        }

        tDB->size = 2*w*h;
     }
     break;

//////////////////TEXTURE_TXR_FORMAT//////////////////////////////
     case TEXTURE_TXR_FORMAT: {
        unsigned short *txr = (unsigned short *) text;
        unsigned int cR[4096];

        if (tF->usePalette) {

           if (flags & TEXTURE_SPRITE) {
              for(i = 0, j = 0; i < palCnt*3;i += 3, j++)
                 cR[j] = epal_Match(_EPal, RGB_i(pal[i], pal[i+1], pal[i+2]));
              cR[0] = 0;
           }
           else {
              for(i = 0, j = 0; i < palCnt*3;i += 3, j++) {
                 int colIdx = epal_Match(_EPal, RGB_i(pal[i], pal[i+1], pal[i+2]));
                 //if (colIdx == 0)
                 //   cR[j] = _BlackColorIndex;
                 //else
                    cR[j] = colIdx;
              }
           }

           lpCP = (unsigned char *)ddsd.lpSurface;

           for(j = 0;j < h;j++, lpCP += ddsd.lPitch)
              for(i = 0, lpCP1 = lpCP;i < w;i++)
                 *lpCP1++ = (unsigned char) cR[*txr++];

           tDB->size = w*h;
           break;
        }

        for(i = 0, j = 0; i < palCnt*3;i += 3, j++)
           cR[j] = (((unsigned int)(pal[i])>>tF->redScale)<<tF->redShift) |
                   (((unsigned int)(pal[i+1])>>tF->greenScale)<<tF->greenShift) |
                   (((unsigned int)(pal[i+2])>>tF->blueScale)<<tF->blueShift);

        if (flags & TEXTURE_ALPHA) {
           aSh = tF->alphaShift;
           aSc = tF->alphaScale;
           if (flags & TEXTURE_SPRITE) {
              unsigned long sprAlpha = ((255>>aSc)<<aSh);
              unsigned short *saveTxr = txr;

              switch(tF->rgbBitCount) {
                 case 16:
                    lpCP = (unsigned char *)ddsd.lpSurface;
                    for(j = 0;j < h;j++, lpCP += ddsd.lPitch)
                       for(i = 0, lpSP = (unsigned short*) lpCP;i < w;i++) //{
                          if (*txr == 0) {
                             unsigned short col = 0;

                             for(int m = -1;m <= 1;m++)
                                for(int n = -1;n <= 1;n++) {
                                   if (i+n >= 0 && i+n < w && j+m >= 0 && j+m < h) {
                                      if (saveTxr[i+n+(j+m)*w] != 0) {
                                         col = (unsigned short) cR[saveTxr[i+n+(j+m)*w]];

                                         goto ok1;
                                      }
                                   }
                                }
                             ok1:
                             *lpSP++ = col;
                             txr++;
                          }
                          else {
                             *lpSP++ = (unsigned short) (cR[*txr++] | sprAlpha);
                          }
                       //}
                 break;
              }
           }
           else {
              unsigned long whiteCol = ((255>>tF->redScale)<<tF->redShift) |
                                       ((255>>tF->greenScale)<<tF->greenShift) |
                                       ((255>>tF->blueScale)<<tF->blueShift);
              switch(tF->rgbBitCount) {
                 case 16:
                    lpCP = (unsigned char *)ddsd.lpSurface;
                    for(j = 0;j < h;j++, lpCP += ddsd.lPitch)
                       for(i = 0, lpSP = (unsigned short*) lpCP;i < w;i++)
                          *lpSP++ = (unsigned short)(((((unsigned short)(*txr++))>>aSc)<<aSh) | whiteCol);
                 break;
              }
           }

           tDB->size = 2*w*h;
           break;
        }

        if (flags & TEXTURE_SPRITE) {
           switch(tF->rgbBitCount) {
              case 16:
                 lpCP = (unsigned char *)ddsd.lpSurface;
                 for(j = 0;j < h;j++, lpCP += ddsd.lPitch)
                    for(i = 0, lpSP = (unsigned short*) lpCP;i < w;i++)
                       if (*txr == 0) {
                          *lpSP++ = 0;
                          txr++;
                       }
                       else {
                          *lpSP++ = (unsigned short) cR[*txr++];
                       }
              break;
           }
        }
        else {
           switch(tF->rgbBitCount) {
              case 16:
                 lpCP = (unsigned char *)ddsd.lpSurface;
                 for(j = 0;j < h;j++, lpCP += ddsd.lPitch)
                    for(i = 0, lpSP = (unsigned short*) lpCP;i < w;i++) {
                       unsigned short col = (unsigned short) cR[*txr++];
                       //if (col == 0)
                       //   *lpSP++ = 2;
                       //else
                          *lpSP++ = col;
                    }
              break;
           }
        }

        tDB->size = 2*w*h;
     }
     break;
  }

  // Unlock the surface
  tDB->lpTextureSurfMem->Unlock(NULL);

  return 1;
}


extern "C" void * D3D_GRLoadTextureToDB(void * handle, unsigned char *pal, int palCnt, unsigned char *text)
{
  STextDB *tDB;
  STextureFormat *tF;
  int h, w;

  //(void)pal;

  DDSURFACEDESC ddsd; // DirectDraw surface description
  int err;
  int flags;


  if (handle == NULL) {
     if (_TextCounter >= MAX_TEXTURE_COUNT) {
        D3DSetError("Texture DB is Full", -1);
        return NULL;
     }
     tDB = &_TextDB[_TextCounter++];

     tDB->filename[0] = 0;
     tDB->loadFunc = NULL;
     tDB->loadFuncUserPar = NULL;
     tDB->loadIntFunc = NULL;

     tDB->flags = *TEXT_FLAG_PTR(text);

     // Now create the source surface in SYSTEM MEMORY
     if (tDB->flags & TEXTURE_ALPHA)
         tF = & _dL.currDevice->textureFormat[ALPHA_TEXTURE_INDEX];
     else {
         if (tDB->flags & TEXTURE_RGB)
            tF = & _dL.currDevice->textureFormat[RGB_TEXTURE_INDEX];
         else
            tF = & _dL.currDevice->textureFormat[NORMAL_TEXTURE_INDEX];
     }

     if (_dL.currDevice->agp)
        flags = DDSCAPS_NONLOCALVIDMEM | DDSCAPS_VIDEOMEMORY;
     else
        flags = DDSCAPS_SYSTEMMEMORY;

     memcpy(&ddsd.ddpfPixelFormat, &tF->pixelFormat, sizeof(DDPIXELFORMAT));
     ddsd.dwSize = sizeof(DDSURFACEDESC);
     ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
     ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | flags;
     ddsd.dwHeight = (h = *TEXT_HEIGHT_PTR(text));
     ddsd.dwWidth = (w = *TEXT_WIDTH_PTR(text));

     err = _dd->CreateSurface(&ddsd, &tDB->lpTextureSurfMem, NULL);
     if (err != DD_OK) {
        D3DSetError("Can't create mem surface",err);
        return NULL;
     }

     tDB->counter = 0;
     tDB->textureFormat = tF;
     tDB->lpTextureDev = NULL;
     tDB->lpTextureSurfDev = NULL;

     tDB->w = w;
     tDB->h = h;
     tDB->invW65536 = 1.0 / (65536.0 * (float)w);
     tDB->invH65536 = 1.0 / (65536.0 * (float)h);
  }
  else  {
     tDB = (STextDB*) handle;
     if (tDB->counter) _d3dDevice->EndScene();
     tF = tDB->textureFormat;
     h = *TEXT_HEIGHT_PTR(text);
     w = *TEXT_WIDTH_PTR(text);
  }

  if (! (tDB->flags & TEXTURE_NOT_PRESENT)) {
     if (!D3D_FillTexture(tDB, tF, w, h, pal, palCnt, text))
        return NULL;
  }
  else
     tDB->size = (tF->rgbBitCount>>3)*w*h;


  if (handle == NULL && tF->usePalette) {
	 err = tDB->lpTextureSurfMem->SetPalette(_DDPalette);
	 if (err != DD_OK) {
		D3DSetError("Can't Set Palette to texture", err);
	 	return NULL;
	 }
  }

  err = tDB->lpTextureSurfMem->QueryInterface(IID_IDirect3DTexture2,
                                              (LPVOID*)&tDB->lpTextureMem);
  if (err != DD_OK) {
	 D3DSetError("Can't Get Mem Surface Interface", err);
	 return NULL;
  }

  if (_dL.currDevice->agp) {
	 tDB->lpTextureSurfDev = tDB->lpTextureSurfMem;
	 tDB->lpTextureDev = tDB->lpTextureMem;

     //if ((tDB->flags & TEXTURE_SPRITE))// && !(tDB->flags & TEXTURE_ALPHA))
     //if (!(tDB->flags & TEXTURE_UNLOAD) || (tDB->flags & TEXTURE_SPRITE))
     tDB->lpTextureSurfDev->SetColorKey(DDCKEY_SRCBLT, &_ColorKey);

     err = tDB->lpTextureDev->GetHandle(_d3dDevice, &tDB->handleDev);
     if (err != DD_OK) {
        D3DSetError("Can't Get Texture Handle", err);
        return FALSE;
     }

     if (handle != NULL && tDB->counter)
        _d3dDevice->BeginScene();
     else
        tDB->counter = 0;
  }
  else {
     if (handle != NULL && tDB->counter) {
        //_d3dDevice->EndScene();
        err = tDB->lpTextureDev->Load(tDB->lpTextureMem);
        _d3dDevice->BeginScene();
        if (err != DD_OK) {
           D3DSetError("Can't Overwrite Texture", err);
           return NULL;
        }
        // _CurrentText = -1;
     }
     else
        tDB->counter = 0;
  }

  return (void*) tDB;
}

extern "C" void D3D_GRDeleteTextureFromDB(void* handle)
{ STextDB *tDB;

  if (handle == NULL) return;

  tDB = (STextDB*) handle;

  RELEASE(tDB->lpTextureMem);
  RELEASE(tDB->lpTextureSurfMem);

}


/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************      INTERNAL FUNCTIONS       ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/


/***********************************************************************
 ********************    Create Device Surface      ********************
 ***********************************************************************/

int CreateDeviceTexture(STextDB *text)
{ int err;
  DDSURFACEDESC ddsd; // DirectDraw surface description
  LPDIRECTDRAWPALETTE lpPalette = NULL; // Destination DDraw palette
  PALETTEENTRY ppe[256]; // Palette table for destination surface

  RELEASE(text->lpTextureSurfDev);
  memset(&ddsd, 0, sizeof(DDSURFACEDESC));
  ddsd.dwSize = sizeof(DDSURFACEDESC);
  err = text->lpTextureSurfMem->GetSurfaceDesc(&ddsd);
  if (err != DD_OK) {
     D3DSetError("Can't Get Mem Surface Descr", err);
     return FALSE;
  }

  // Create destination texture surface
  ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
  ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_ALLOCONLOAD;

  ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
  err = _dd->CreateSurface(&ddsd, &text->lpTextureSurfDev, NULL);
  if (err != DD_OK) {
     D3DSetError("r:Can't Create Dev Surface", err);
     return FALSE;
  }

  //if ((text->flags & TEXTURE_SPRITE))// && !(text->flags & TEXTURE_ALPHA))
  //if (!(text->flags & TEXTURE_UNLOAD) || (text->flags & TEXTURE_SPRITE))
     text->lpTextureSurfDev->SetColorKey(DDCKEY_SRCBLT, &_ColorKey);

  if (text->textureFormat->usePalette) {
     // Attach a palette is required

     memset(ppe, 0, sizeof(PALETTEENTRY) * 256);
     err = _dd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256, ppe, &lpPalette, NULL);
     if (err != DD_OK) {
	    D3DSetError("r:Can't Create Palette", err);
	    return FALSE;
     }

     err = text->lpTextureSurfDev->SetPalette(lpPalette);
     if (err != DD_OK) {
	    D3DSetError("r:Can't Set Palette", err);
	    return FALSE;
     }

     RELEASE(lpPalette);
  }

  err = text->lpTextureSurfDev->QueryInterface(IID_IDirect3DTexture2,
			    (LPVOID*)&text->lpTextureDev);
  if (err != DD_OK) {
     D3DSetError("r:Can't Get Dev Surface Interface", err);
     return FALSE;
  }
  return TRUE;
}



#if STAT
int _NoFindTexture = 0;
int _ReloadTexture = 0;
int _UnloadTexture;
int _Memory;
#endif



struct {
    STextureFormat *tf;
    //int sprite;
    int size;
    int count;
    STextDB *td[TEXT_PRELOAD_COUNT];
} _Textures[256];

void GRPreLoadTextures()
{  STextDB *tDB;// = _TextDB;
   int i,j,size, unlSize = 0;
   int textC = 1;

   if (_dL.currDevice == NULL || _dL.currDevice->swHw == GR_SOFTWARE ||
       _dL.currDevice->agp
      )
      return;

   _Textures[0].tf    = _TextDB[0].textureFormat;
   _Textures[0].size  = _TextDB[0].size;
   _Textures[0].count = 1;
   _Textures[0].td[0] = _TextDB;

   size = _TextDB[0].size;
   if (_TextDB[0].flags & TEXTURE_UNLOAD)
      unlSize += size;

   for(i = 1;i < _TextCounter;i++) {
      tDB = &_TextDB[i];

      for(j = 0;j < textC;j++) {
         if (tDB->size == _Textures[j].size &&
             tDB->textureFormat == _Textures[j].tf
            ) {

            if (_Textures[j].count < TEXT_PRELOAD_COUNT)
               _Textures[j].td[_Textures[j].count] = tDB;

            _Textures[j].count++;

            size += _Textures[j].size;

            if (tDB->flags & TEXTURE_UNLOAD)
               unlSize += tDB->size;

            break;
         }
      }

      if (j == textC) {
         _Textures[textC].size  = tDB->size;
         _Textures[textC].tf    = tDB->textureFormat;
         _Textures[textC].td[0] = tDB;
         _Textures[textC].count = 1;

         size += tDB->size;

         if (tDB->flags & TEXTURE_UNLOAD)
            unlSize += tDB->size;

         textC++;
      }
   }

   if (size < _dL.currDevice->textureMemory)
      return; //all textures will be loaded in texture memory

#if STAT
   if (unlSize > _dL.currDevice->textureMemory)
      _ReloadTexture = 1000;
   _NoFindTexture = unlSize;
#endif


   for(i = 0;i < textC;i++) {
      j = (_Textures[i].count < TEXT_PRELOAD_COUNT)?_Textures[i].count:TEXT_PRELOAD_COUNT;
      for(int k = 0;k < j;k++)
         LoadTextureTo3DCard((void*)(_Textures[i].td[k]));
   }

}

extern unsigned char _currPalette[768];


void GRRestoreSurfaces()
{  int err;

   if (_dL.currDevice == NULL) return;

   //GREndScene();



   //ShowWindow(_gr_hWnd, SW_SHOWNORMAL);

   if (_FrontBuffer != NULL)
      if (_FrontBuffer->IsLost() == DDERR_SURFACELOST) {
         _FrontBuffer->Restore();
      }

   if (_dL.currDevice->swHw == GR_SOFTWARE)
      return;


      if (_BackBuffer->IsLost() == DDERR_SURFACELOST)
         _BackBuffer->Restore();


   //GRSetPalette(_currPalette);

#if USE_Z_BUFFER
   if (_ZBuffer->IsLost() == DDERR_SURFACELOST) {
      err = _ZBuffer->Restore();
      //if (err != DD_OK) {
      //   D3DSetError("Can't Restore Z", err);
      //   MessageBox (NULL, GRGetLastErrorStr(), "Error", MB_OK | MB_ICONERROR);
      //}
   }
#endif


   //restore textures
   if (_dL.currDevice->agp) {
      for(int i = 0;i < _TextCounter;i++)
         if (_TextDB[i].lpTextureSurfDev->IsLost()) {
            _TextDB[i].lpTextureSurfDev->Restore();

            if (_TextDB[i].loadFunc != NULL)
               (*_TextDB[i].loadFunc)(_TextDB[i].filename, (GR_HTEXTURE) &_TextDB[i], _TextDB[i].loadFuncUserPar);
            else {
               if (_TextDB[i].loadIntFunc != NULL)
                  (*_TextDB[i].loadIntFunc)((GR_HTEXTURE) &_TextDB[i]);
            }
         }
   }
   else {
      for(int i = 0;i < _TextCounter;i++) {
         if (_TextDB[i].lpTextureSurfMem->IsLost()) {
            _TextDB[i].lpTextureSurfMem->Restore();

            if (_TextDB[i].loadFunc != NULL)
               (*_TextDB[i].loadFunc)(_TextDB[i].filename, (GR_HTEXTURE) &_TextDB[i], _TextDB[i].loadFuncUserPar);
            else {
               if (_TextDB[i].loadIntFunc != NULL)
                  (*_TextDB[i].loadIntFunc)((GR_HTEXTURE) &_TextDB[i]);
            }
         }
         if (_TextDB[i].counter != 0) {
            if (_TextDB[i].lpTextureSurfDev->IsLost()) {
               _TextDB[i].lpTextureSurfDev->Restore();
               _TextDB[i].lpTextureDev->Load(_TextDB[i].lpTextureMem);
               _TextDB[i].lpTextureDev->GetHandle(_d3dDevice, &_TextDB[i].handleDev);
            }
         }
      }
   }

   //D3D_GRInitTables(0);
   //D3D_ClearZList();
   //D3D_GRSetHaze((int)_fogStart, (int)(_fogEnd - _fogStart), _hD);
   _CurrentText = -1;
}

/***********************************************************************
 ********************   Load Texture To 3D Card     ********************
 ***********************************************************************/

int LoadTextureTo3DCard(void *tH)
{ int err, count;
  STextMem *tm, *find;
  STextDB *tDB = (STextDB *) tH;

  if (tDB == NULL) {
     if (_CurrentText != 0) {
        SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREHANDLE, 0);
        _CurrentText = 0;
     }

     return TRUE;
  }

  _TextTime++;


  if (tDB->counter == 0) {
	 if (! _dL.currDevice->agp) {
		//if (_DeviceMemory < 1000000) {
		//RELEASE(tDB->lpTextureSurfDev);
		//RELEASE(tDB->lpTextureDev);

        if ((_DeviceMemory + tDB->size < _dL.currDevice->textureMemory) && (CreateDeviceTexture(tDB) == TRUE)) {
		   err = tDB->lpTextureDev->Load(tDB->lpTextureMem);


		   if (err == DD_OK)  {
			  DDSURFACEDESC ddsd;

              memset(&ddsd, 0, sizeof(DDSURFACEDESC));
              ddsd.dwSize = sizeof(DDSURFACEDESC);

              err = tDB->lpTextureSurfDev->GetSurfaceDesc(&ddsd);
              if (err != DD_OK) {
                 D3DSetError("Could not get the surface description of the loaded texture surface", err);
                 return FALSE;
			  }

             // Make sure that the ended up in video memory for a hardware device
             if (!(ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)) {
				err = !DD_OK;
				RELEASE(tDB->lpTextureSurfDev);
			 }

           }
		   //if (err == DD_OK) tDB->lpTextureDev->Release();
		}
		else err = !DD_OK;
		//}
		//else err = !DD_OK;

        if (err != DD_OK) {

		   tm = _D3DMemUse;
           find = NULL;
           count = _TextTime;

		   do {
			  if (!(tm->text->flags & TEXTURE_UNLOAD)) {
                 if (tm->text->size == tDB->size &&
				 	 tm->text->counter < count &&
                     tm->text->textureFormat == tDB->textureFormat
                    ) {
					find = tm;
					count = tm->text->counter;
				 }
			  }

			  tm = tm->rLink;

		   } while(tm != _D3DMemUse);

           if (find == NULL) {
              D3DSetError("r:Can't Find Texture", -1);
#if STAT
              _NoFindTexture++;
#endif

			  return FALSE; //OGO!!!!!!!!!!!!!!
           }


           GREndScene();

           find->text->counter = 0;

#if STAT
           _ReloadTexture++;
#endif
		   tDB->lpTextureSurfDev = find->text->lpTextureSurfDev;
		   tDB->lpTextureDev = find->text->lpTextureDev;
           //find->text->lpTextureSurfDev = NULL;


           //if (tDB->lpTextureSurfDev == NULL ||
           //    tDB->lpTextureDev == NULL) {
           //      _NoFindTexture += 1000;
           //      return FALSE;
           //    }


		   err = tDB->lpTextureDev->Load(tDB->lpTextureMem);

		   if (err != DD_OK) {
			  D3DSetError("r:Can't Load Texture", err);
			  return FALSE; //OGO!!!!!!!!!!!!!!
		   }

           //!!!!!!!!!!!!!1
           tDB->handleDev = find->text->handleDev;
           //!!!!!!!!!!!!!!
		   find->text = tDB;

           GRStartScene();
        }
		else {
		   //Texture Loaded in VIDEOMEMORY
		   tm = MallocTextMem();
		   if (_D3DMemUse == NULL) {
		      _D3DMemUse = tm;
		      tm->lLink = tm;
		      tm->rLink = tm;
		   }
		   else {
		      tm->rLink = _D3DMemUse;
		      tm->lLink = _D3DMemUse->lLink;
		      _D3DMemUse->lLink->rLink = tm;
		      _D3DMemUse->lLink = tm;
		   }

		   tm->size = tDB->size;
		   tm->text = tDB;
		   _DeviceMemory += tDB->size;
		}

        err = tDB->lpTextureDev->GetHandle(_d3dDevice, &tDB->handleDev);
        if (err != DD_OK) {
           D3DSetError("Can't Get Texture Handle", err);
           return FALSE;
        }
     }


     //err = tDB->lpTextureDev->GetHandle(_d3dDevice, &tDB->handleDev);
     //if (err != DD_OK) {
     //   D3DSetError("Can't Get Texture Handle", err);
     //   return FALSE;
     //}
	 //tDB->lpTextureDev->Release();


  }


  tDB->counter = _TextTime;

  /*
  if (tDB->lpTextureSurfDev->IsLost()) {
	 tDB->lpTextureSurfDev->Restore();
     //err = tDB->lpTextureSurfDev->QueryInterface(IID_IDirect3DTexture2,
	 //		    (LPVOID*)&tDB->lpTextureDev);
     //if (err != DD_OK) {
     //   SET_ERROR("r:Can't Get Dev Surface Interface");
     //   return FALSE;
	 //}
	 err = tDB->lpTextureDev->Load(tDB->lpTextureMem);
     err = tDB->lpTextureDev->GetHandle(_d3dDevice, &tDB->handleDev);
     if (err != DD_OK) {
		D3DSetError("Can't Get Texture Handle", err);
	 	return FALSE;
	 }
	 //tDB->lpTextureDev->Release();

  }
  */

  if (_CurrentText != tDB->handleDev) {
     //if (_CurrentText == 0) {
        SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREHANDLE, tDB->handleDev);
     //}
	 _CurrentText = tDB->handleDev;
  }

  return TRUE;
}

/***********************************************************************
 ********************   Clipping 2D Polygon         ********************
 ***********************************************************************/

#define COPY_CHAR4(dP,sP,off)  (*(((unsigned long*)(dP))+off) = *(((unsigned long*)(sP))+off))
#define COPY_CHAR8(dP,sP,off)  COPY_CHAR4(dP,sP,off);COPY_CHAR4(dP,sP,off+1)
#define COPY_CHAR16(dP,sP,off) COPY_CHAR8(dP,sP,off);COPY_CHAR8(dP,sP,off+2)

#define COPY_BUF4(dP,sP)  COPY_CHAR4(dP,sP,0)
#define COPY_BUF8(dP,sP)  COPY_CHAR8(dP,sP,0)
#define COPY_BUF16(dP,sP) COPY_CHAR16(dP,sP,0)
#define COPY_BUF32(dP,sP) COPY_CHAR16(dP,sP,0);COPY_CHAR16(dP,sP,4)



#define szVer 8*4

long Clip2DPolygon(UGRVertex **startBuf)
{ int num = 0, curin, nextin,vertC = _gr_polygon.nVertices,i;
  UGRVertex *pin = _gr_vertices, *pout = intBuf,*in,*out, *next;
  long leftX, rightX;
  long topY, bottomY;
  long mask = 0;//,szVer;
  ClipFunc Vert,Hor;

  leftX = rightX = pin->any.x;
  topY = bottomY = pin->any.y;

  for(i = 1,next = pin;i < vertC;i++) {
	 next++;

     if (topY > next->any.y) {
        topY = next->any.y;
	 }
     else if (bottomY < next->any.y) {
        bottomY = next->any.y;
	 }
     if (leftX > next->any.x) {
        leftX = next->any.x;
	 }
     else if (rightX < next->any.x) {
        rightX = next->any.x;
	 }
  }

  //----------simple clipping------------
  if (bottomY < _gr_ilyaClipI.top ||
      topY > _gr_ilyaClipI.bottom ||
      leftX > _gr_ilyaClipI.right ||
      rightX < _gr_ilyaClipI.left) return (0);

  if (topY < _gr_ilyaClipI.top)       mask++;
  if (bottomY > _gr_ilyaClipI.bottom) mask += 2;
  if (leftX < _gr_ilyaClipI.left)     mask += 4;
  if (rightX > _gr_ilyaClipI.right)   mask += 8;

  if (mask == 0) {
	//No Clipping needed
	//(*_CopyVertexFTable[_gr_polygon.inttyp])(_gr_vertices,vertC);
	*startBuf = pin;
	return(vertC);
  }

  //mask = 0xF;
  //---------------------------------------
  Hor = _HClipVertexFTable[_gr_polygon.dwFullType];
  Vert = _VClipVertexFTable[_gr_polygon.dwFullType];
  //_ClipCopyVertexSize[_gr_polygon.dwFullType];
  //--------------TOP----------------------
  if ((mask & 1)) {
	 in = pin;
	 out = pout;
     curin = (in->any.y >= _gr_ilyaClipI.top);

     for (i = 0 ;i < vertC - 1 ;i++,in++,curin = nextin) {
        next = in + 1;

        // Keep the current vertex if it's inside the plane
        if (curin) {
           COPY_BUF32(out,in);
           out++;
           //memcpy(out++,in,szVer);
		   num++;
		}

        nextin = (next->any.y >= _gr_ilyaClipI.top);

       // Add a clipped vertex if one end of the current edge is
       // inside the plane and the other is outside
       if (curin != nextin)
          (Hor)(out++,in,next,_gr_ilyaClipI.top), num++;
	 }

     if (curin) {
        COPY_BUF32(out,in);
        out++;
        //memcpy(out++,in,szVer);
		num++;
	 }

     if (curin != (pin->any.y >= _gr_ilyaClipI.top))
        (Hor)(out,in,pin,_gr_ilyaClipI.top), num++;

     if (num < 3) return 0;
     vertC = num;
	 next = pin;
     pin = pout;
	 pout = next;

  }

  //----------------------LEFT--------------------
  if ((mask & 4)) {
     num = 0;
     in = pin;
	 out = pout;
     curin = (in->any.x >= _gr_ilyaClipI.left);

     for (i = 0 ;i < vertC - 1 ;i++,in++,curin = nextin) {
        next = in + 1;

        // Keep the current vertex if it's inside the plane
        if (curin) {
           COPY_BUF32(out,in);
           out++;
           //memcpy(out++,in,szVer);
           num++;
        }

        nextin = (next->any.x >= _gr_ilyaClipI.left);

        // Add a clipped vertex if one end of the current edge is
        // inside the plane and the other is outside
        if (curin != nextin)
           (Vert)(out++,in,next,_gr_ilyaClipI.left), num++;
	 }

     if (curin) {
        COPY_BUF32(out,in);
        out++;
        //memcpy(out++,in,szVer);
        num++;
     }

     if (curin != (pin->any.x >= _gr_ilyaClipI.left))
        (Vert)(out,in,pin,_gr_ilyaClipI.left), num++;

     if (num < 3) return 0;
     vertC = num;
	 next = pin;
     pin = pout;
	 pout = next;

  }
  //------------------------BOTTOM---------------------
  if ((mask & 2)) {
     num = 0;
     in = pin;
	 out = pout;
     curin = (in->any.y <= _gr_ilyaClipI.bottom);

     for (i = 0 ;i < vertC - 1 ;i++,in++,curin = nextin) {
        next = in + 1;

        // Keep the current vertex if it's inside the plane
        if (curin) {
           COPY_BUF32(out,in);
           out++;
           //memcpy(out++,in,szVer);
           num++;
        }

        nextin = (next->any.y <= _gr_ilyaClipI.bottom);

        // Add a clipped vertex if one end of the current edge is
        // inside the plane and the other is outside
        if (curin != nextin)
           (Hor)(out++,in,next,_gr_ilyaClipI.bottom), num++;
	 }

     if (curin) {
        COPY_BUF32(out,in);
        out++;
        //memcpy(out++,in,szVer);
        num++;
     }

     if (curin != (pin->any.y <= _gr_ilyaClipI.bottom))
        (Hor)(out,in,pin,_gr_ilyaClipI.bottom), num++;

     if (num < 3) return 0;
     vertC = num;
	 next = pin;
     pin = pout;
	 pout = next;


  }
  //--------------------------RIGHT--------------------
  if ((mask & 8)) {
     num = 0;
     in = pin;
	 out = pout;
     curin = (in->any.x <= _gr_ilyaClipI.right);

     for (i = 0 ;i < vertC - 1 ;i++,in++,curin = nextin) {
        next = in + 1;

        // Keep the current vertex if it's inside the plane
        if (curin) {
           COPY_BUF32(out,in);
           out++;
           //memcpy(out++,in,szVer);
           num++;
        }

        nextin = (next->any.x <= _gr_ilyaClipI.right);

        // Add a clipped vertex if one end of the current edge is
        // inside the plane and the other is outside
        if (curin != nextin)
           (Vert)(out++,in,next,_gr_ilyaClipI.right), num++;
	 }

     if (curin) {
        COPY_BUF32(out,in);
        out++;
        //memcpy(out++,in,szVer);
        num++;
     }

     if (curin != (pin->any.x <= _gr_ilyaClipI.right))
        (Vert)(out,in,pin,_gr_ilyaClipI.right), num++;

     if (num < 3) return 0;
	 vertC = num;
	 pin = pout;

  }
  //-----------------Calc topY and bottomY-----------------
  //(*_CopyVertexFTable[_gr_polygon.inttyp])(pin,vertC);
  *startBuf = pin;
  return (vertC);
}

/***********************************************************************
 ********************   Clipping 2D Bump Polygon    ********************
 ***********************************************************************/

#define szBumpVer 2*4

long Clip2DPolygonBump(UGRVertex **startBuf, SBumpVertex **startBumpBuf)
{ int num = 0, curin, nextin,vertC = _gr_polygon.nVertices,i;
  UGRVertex *pin = _gr_vertices, *pout = intBuf,*in,*out, *next;
  long leftX, rightX;
  long topY, bottomY;
  long mask = 0;//,szVer;
  ClipFunc Vert,Hor;
  SBumpVertex *bumpPIn = _gr_bump_vertices, *bumpPOut = intBumpBuf, * tmpB;

  leftX = rightX = pin->any.x;
  topY = bottomY = pin->any.y;

  for(i = 1,next = pin;i < vertC;i++) {
	 next++;

     if (topY > next->any.y) {
        topY = next->any.y;
	 }
     else if (bottomY < next->any.y) {
        bottomY = next->any.y;
	 }
     if (leftX > next->any.x) {
        leftX = next->any.x;
	 }
     else if (rightX < next->any.x) {
        rightX = next->any.x;
	 }
  }

  //----------simple clipping------------
  if (bottomY < _gr_ilyaClipI.top ||
      topY > _gr_ilyaClipI.bottom ||
      leftX > _gr_ilyaClipI.right ||
      rightX < _gr_ilyaClipI.left) return (0);

  if (topY < _gr_ilyaClipI.top) mask++;
  if (bottomY > _gr_ilyaClipI.bottom) mask += 2;
  if (leftX < _gr_ilyaClipI.left) mask += 4;
  if (rightX > _gr_ilyaClipI.right) mask += 8;

  if (mask == 0) {
	//No Clipping needed
	//(*_CopyVertexFTable[_gr_polygon.inttyp])(_gr_vertices,vertC);
	*startBuf = pin;
	*startBumpBuf = bumpPIn;
	return(vertC);
  }

  //mask = 0xF;
  //---------------------------------------
  Hor = _HClipVertexFTable[_gr_polygon.dwFullType];
  Vert = _VClipVertexFTable[_gr_polygon.dwFullType];
  //_ClipCopyVertexSize[_gr_polygon.dwFullType];
  //--------------TOP----------------------
  if ((mask & 1)) {
	 in = pin;
	 out = pout;
	 _bumpIn = bumpPIn;
	 _bumpOut = bumpPOut;
     curin = (in->any.y >= _gr_ilyaClipI.top);

     for (i = 0 ;i < vertC - 1 ;i++,in++,_bumpIn++,curin = nextin) {
        next = in + 1;
		_bumpInNext = _bumpIn + 1;

        // Keep the current vertex if it's inside the plane
        if (curin) {
           COPY_BUF8(_bumpOut, _bumpIn);
           _bumpOut++;
           COPY_BUF32(out, in);
           out++;
           //memcpy(_bumpOut++, _bumpIn, szBumpVer);
           //memcpy(out++, in, szVer);
		   num++;
		}

        nextin = (next->any.y >= _gr_ilyaClipI.top);

       // Add a clipped vertex if one end of the current edge is
       // inside the plane and the other is outside
       if (curin != nextin) {
          (Hor)(out++,in,next,_gr_ilyaClipI.top);
		  _bumpOut++;
		  num++;
	   }
	 }

     if (curin) {
        COPY_BUF8(_bumpOut, _bumpIn);
        _bumpOut++;
        COPY_BUF32(out, in);
        out++;
        //memcpy(_bumpOut++, _bumpIn, szBumpVer);
        //memcpy(out++,in,szVer);
		num++;
	 }

     if (curin != (pin->any.y >= _gr_ilyaClipI.top)) {
		_bumpInNext = bumpPIn;
        (Hor)(out,in,pin,_gr_ilyaClipI.top);
		 num++;
	 }

     if (num < 3) return 0;
     vertC = num;

	 next = pin;
     pin = pout;
	 pout = next;

	 tmpB = bumpPIn;
	 bumpPIn = bumpPOut;
	 bumpPOut = tmpB;
  }

  //----------------------LEFT--------------------
  if ((mask & 4)) {
     num = 0;
     in = pin;
	 out = pout;
	 _bumpIn = bumpPIn;
	 _bumpOut = bumpPOut;
     curin = (in->any.x >= _gr_ilyaClipI.left);

     for (i = 0 ;i < vertC - 1 ;i++,in++,_bumpIn++,curin = nextin) {
        next = in + 1;
		_bumpInNext = _bumpIn + 1;

        // Keep the current vertex if it's inside the plane
        if (curin) {
           COPY_BUF8(_bumpOut, _bumpIn);
           _bumpOut++;
           COPY_BUF32(out, in);
           out++;
           //memcpy(out++,in,szVer);
           //memcpy(_bumpOut++, _bumpIn, szBumpVer);
		   num++;
		}

        nextin = (next->any.x >= _gr_ilyaClipI.left);

        // Add a clipped vertex if one end of the current edge is
        // inside the plane and the other is outside
        if (curin != nextin) {
           (Vert)(out++,in,next,_gr_ilyaClipI.left);
		   _bumpOut++;
		   num++;
		}
	 }

     if (curin) {
        COPY_BUF8(_bumpOut, _bumpIn);
        _bumpOut++;
        COPY_BUF32(out, in);
        out++;
        //memcpy(_bumpOut++, _bumpIn, szBumpVer);
        //memcpy(out++,in,szVer);
		num++;
	 }

     if (curin != (pin->any.x >= _gr_ilyaClipI.left)) {
		_bumpInNext = bumpPIn;
        (Vert)(out,in,pin,_gr_ilyaClipI.left);
		num++;
	 }

     if (num < 3) return 0;
     vertC = num;
	 next = pin;
     pin = pout;
	 pout = next;

	 tmpB = bumpPIn;
	 bumpPIn = bumpPOut;
	 bumpPOut = tmpB;
  }
  //------------------------BOTTOM---------------------
  if ((mask & 2)) {
     num = 0;
     in = pin;
	 out = pout;
	 _bumpIn = bumpPIn;
	 _bumpOut = bumpPOut;
     curin = (in->any.y <= _gr_ilyaClipI.bottom);

     for (i = 0 ;i < vertC - 1 ;i++,in++,_bumpIn++,curin = nextin) {
        next = in + 1;
		_bumpInNext = _bumpIn + 1;

        // Keep the current vertex if it's inside the plane
        if (curin) {
           COPY_BUF8(_bumpOut, _bumpIn);
           _bumpOut++;
           COPY_BUF32(out, in);
           out++;
           //memcpy(out++,in,szVer);
           //memcpy(_bumpOut++, _bumpIn, szBumpVer);
		   num++;
		}

        nextin = (next->any.y <= _gr_ilyaClipI.bottom);

        // Add a clipped vertex if one end of the current edge is
        // inside the plane and the other is outside
        if (curin != nextin) {
           (Hor)(out++,in,next,_gr_ilyaClipI.bottom);
		   _bumpOut++;
		   num++;
		}
	 }

     if (curin) {
        COPY_BUF8(_bumpOut, _bumpIn);
        _bumpOut++;
        COPY_BUF32(out, in);
        out++;
        //memcpy(_bumpOut++, _bumpIn, szBumpVer);
        //memcpy(out++,in,szVer);
		num++;
	 }

     if (curin != (pin->any.y <= _gr_ilyaClipI.bottom)) {
		_bumpInNext = bumpPIn;
        (Hor)(out,in,pin,_gr_ilyaClipI.bottom);
		num++;
	 }

     if (num < 3) return 0;
     vertC = num;
	 next = pin;
     pin = pout;
	 pout = next;

	 tmpB = bumpPIn;
	 bumpPIn = bumpPOut;
	 bumpPOut = tmpB;

  }
  //--------------------------RIGHT--------------------
  if ((mask & 8)) {
     num = 0;
     in = pin;
	 out = pout;
	 _bumpIn = bumpPIn;
	 _bumpOut = bumpPOut;
     curin = (in->any.x <= _gr_ilyaClipI.right);

     for (i = 0 ;i < vertC - 1 ;i++,in++,_bumpIn++,curin = nextin) {
        next = in + 1;
		_bumpInNext = _bumpIn + 1;

        // Keep the current vertex if it's inside the plane
        if (curin) {
           COPY_BUF8(_bumpOut, _bumpIn);
           _bumpOut++;
           COPY_BUF32(out, in);
           out++;
           //memcpy(out++,in,szVer);
           //memcpy(_bumpOut++, _bumpIn, szBumpVer);
		   num++;
		}

        nextin = (next->any.x <= _gr_ilyaClipI.right);

        // Add a clipped vertex if one end of the current edge is
        // inside the plane and the other is outside
        if (curin != nextin) {
           (Vert)(out++,in,next,_gr_ilyaClipI.right);
		   _bumpOut++;
		   num++;
		}
	 }

     if (curin) {
        COPY_BUF8(_bumpOut, _bumpIn);
        _bumpOut++;
        COPY_BUF32(out, in);
        out++;
        //memcpy(out++,in,szVer);
        //memcpy(_bumpOut++, _bumpIn, szBumpVer);
		num++;
	 }

     if (curin != (pin->any.x <= _gr_ilyaClipI.right)) {
		_bumpInNext = bumpPIn;
        (Vert)(out,in,pin,_gr_ilyaClipI.right);
		num++;
	 }

     if (num < 3) return 0;
	 vertC = num;
	 pin = pout;
     bumpPIn = bumpPOut;

  }
  //-----------------Calc topY and bottomY-----------------
  //(*_CopyVertexFTable[_gr_polygon.inttyp])(pin,vertC);
  *startBuf = pin;
  *startBumpBuf = bumpPIn;

  return (vertC);
}

/***********************************************************************
 ********************       Clipping Function       ********************
 ***********************************************************************/

void HClip(UGRVertex *vNew, UGRVertex *vp, UGRVertex *v,long y)
{

  vNew->any.y = y;
  vNew->any.x = (MulDivL((v->any.x-vp->any.x)<<10,y-vp->any.y,v->any.y-vp->any.y)>>10)+
                vp->any.x;
  vNew->d3d.oow = MulDivF(v->d3d.oow - vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y) + vp->d3d.oow;
  vNew->d3d.d3dcolor.color = vp->d3d.d3dcolor.color;
  //----------------------------------------------------------------------

  if (_gr_polygon.dwAddType & GR_POLY_ADD_BUMP) {
     //vNew->d3d.rgbColor.a = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.a,v->d3d.oow,(long)vp->d3d.rgbColor.a,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);

     _bumpOut->u = CalcPerParF(_bumpInNext->u,v->d3d.oow,_bumpIn->u,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);
     _bumpOut->v = CalcPerParF(_bumpInNext->v,v->d3d.oow,_bumpIn->v,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);
  }

  //-----------------------------------------------------------------------
}

void VClip(UGRVertex *vNew, UGRVertex *vp, UGRVertex *v,long x)
{

  vNew->any.x = x;
  vNew->any.y = (MulDivL((v->any.y-vp->any.y)<<10,x-vp->any.x,v->any.x-vp->any.x)>>10)+
                vp->any.y;
  vNew->d3d.oow = MulDivF(v->d3d.oow - vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x) + vp->d3d.oow;
  vNew->d3d.d3dcolor.color = vp->d3d.d3dcolor.color;
  //----------------------------------------------------------------------

  if (_gr_polygon.dwAddType & GR_POLY_ADD_BUMP) {
     //vNew->d3d.rgbColor.a = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.a,v->d3d.oow,(long)vp->d3d.rgbColor.a,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);

     _bumpOut->u = CalcPerParF(_bumpInNext->u,v->d3d.oow,_bumpIn->u,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);
     _bumpOut->v = CalcPerParF(_bumpInNext->v,v->d3d.oow,_bumpIn->v,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);
  }

  //-----------------------------------------------------------------------
}

void HClipRGBZ(UGRVertex *vNew, UGRVertex *vp, UGRVertex *v,long y)
{

  vNew->any.y = y;
  vNew->any.x = (MulDivL((v->any.x-vp->any.x)<<10,y-vp->any.y,v->any.y-vp->any.y)>>10)+
                vp->any.x;
  vNew->d3d.oow = MulDivF(v->d3d.oow - vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y) + vp->d3d.oow;

  vNew->d3d.rgbColor.r = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.r,v->d3d.oow,(long)vp->d3d.rgbColor.r,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);
  vNew->d3d.rgbColor.g = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.g,v->d3d.oow,(long)vp->d3d.rgbColor.g,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);
  vNew->d3d.rgbColor.b = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.b,v->d3d.oow,(long)vp->d3d.rgbColor.b,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);
  vNew->d3d.rgbColor.a = vp->d3d.rgbColor.a;
  //vNew->u = MulDiv(v->u - vp->u,y-vp->y,v->y-vp->y) + vp->u;
  //vNew->v = MulDiv(v->v - vp->v,y-vp->y,v->y-vp->y) + vp->v;
  //vNew->b = MulDiv(v->b - vp->b,y-vp->y,v->y-vp->y) + vp->b;
  //----------------------------------------------------------------------
  if (_gr_polygon.dwAddType & GR_POLY_ADD_BUMP) {
     //vNew->d3d.rgbColor.a = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.a,v->d3d.oow,(long)vp->d3d.rgbColor.a,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);

     _bumpOut->u = CalcPerParF(_bumpInNext->u,v->d3d.oow,_bumpIn->u,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);
     _bumpOut->v = CalcPerParF(_bumpInNext->v,v->d3d.oow,_bumpIn->v,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);
  }

  //-----------------------------------------------------------------------
}

void VClipRGBZ(UGRVertex *vNew, UGRVertex *vp, UGRVertex *v,long x)
{

  vNew->any.x = x;
  vNew->any.y = (MulDivL((v->any.y-vp->any.y)<<10,x-vp->any.x,v->any.x-vp->any.x)>>10)+
                vp->any.y;
  vNew->d3d.oow = MulDivF(v->d3d.oow - vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x) + vp->d3d.oow;

  vNew->d3d.rgbColor.r = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.r,v->d3d.oow,(long)vp->d3d.rgbColor.r,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);
  vNew->d3d.rgbColor.g = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.g,v->d3d.oow,(long)vp->d3d.rgbColor.g,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);
  vNew->d3d.rgbColor.b = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.b,v->d3d.oow,(long)vp->d3d.rgbColor.b,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);
  vNew->d3d.rgbColor.a = vp->d3d.rgbColor.a;
  //vNew->u = MulDiv(v->u - vp->u,x-vp->x,v->x-vp->x) + vp->u;
  //vNew->v = MulDiv(v->v - vp->v,x-vp->x,v->x-vp->x) + vp->v;
  //vNew->b = MulDiv(v->b - vp->b,x-vp->x,v->x-vp->x) + vp->b;
  //----------------------------------------------------------------------

  if (_gr_polygon.dwAddType & GR_POLY_ADD_BUMP) {
     //vNew->d3d.rgbColor.a = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.a,v->d3d.oow,(long)vp->d3d.rgbColor.a,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);

     _bumpOut->u = CalcPerParF(_bumpInNext->u,v->d3d.oow,_bumpIn->u,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);
     _bumpOut->v = CalcPerParF(_bumpInNext->v,v->d3d.oow,_bumpIn->v,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);
  }

  //-----------------------------------------------------------------------
}

void HClipSTZ(UGRVertex *vNew, UGRVertex *vp, UGRVertex *v,long y)
{

  vNew->any.y = y;
  vNew->any.x = (MulDivL((v->any.x-vp->any.x)<<10,y-vp->any.y,v->any.y-vp->any.y)>>10)+
                vp->any.x;
  vNew->d3d.oow = MulDivF(v->d3d.oow - vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y) + vp->d3d.oow;

  vNew->d3d.u = CalcPerParF(v->d3d.u,v->d3d.oow,vp->d3d.u,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);
  vNew->d3d.v = CalcPerParF(v->d3d.v,v->d3d.oow,vp->d3d.v,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);
  vNew->d3d.d3dcolor.color = vp->d3d.d3dcolor.color;
  //----------------------------------------------------------------------
  if (_gr_polygon.dwAddType & GR_POLY_ADD_BUMP) {
     //vNew->d3d.rgbColor.a = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.a,v->d3d.oow,(long)vp->d3d.rgbColor.a,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);

     _bumpOut->u = CalcPerParF(_bumpInNext->u,v->d3d.oow,_bumpIn->u,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);
     _bumpOut->v = CalcPerParF(_bumpInNext->v,v->d3d.oow,_bumpIn->v,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);
  }

  //-----------------------------------------------------------------------
}

void VClipSTZ(UGRVertex *vNew, UGRVertex *vp, UGRVertex *v,long x)
{

  vNew->any.x = x;
  vNew->any.y = (MulDivL((v->any.y-vp->any.y)<<10,x-vp->any.x,v->any.x-vp->any.x)>>10)+
                vp->any.y;
  vNew->d3d.oow = MulDivF(v->d3d.oow - vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x) + vp->d3d.oow;

  vNew->d3d.u = CalcPerParF(v->d3d.u,v->d3d.oow,vp->d3d.u,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);
  vNew->d3d.v = CalcPerParF(v->d3d.v,v->d3d.oow,vp->d3d.v,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);
  vNew->d3d.d3dcolor.color = vp->d3d.d3dcolor.color;
  //----------------------------------------------------------------------

  if (_gr_polygon.dwAddType & GR_POLY_ADD_BUMP) {
     //vNew->d3d.rgbColor.a = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.a,v->d3d.oow,(long)vp->d3d.rgbColor.a,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);

     _bumpOut->u = CalcPerParF(_bumpInNext->u,v->d3d.oow,_bumpIn->u,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);
     _bumpOut->v = CalcPerParF(_bumpInNext->v,v->d3d.oow,_bumpIn->v,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);
  }

  //-----------------------------------------------------------------------
}

void HClipRGBSTZ(UGRVertex *vNew, UGRVertex *vp, UGRVertex *v,long y)
{

  vNew->any.y = y;
  vNew->any.x = (MulDivL((v->any.x-vp->any.x)<<10,y-vp->any.y,v->any.y-vp->any.y)>>10)+
                vp->any.x;
  vNew->d3d.oow = MulDivF(v->d3d.oow - vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y) + vp->d3d.oow;

  vNew->d3d.u = CalcPerParF(v->d3d.u,v->d3d.oow,vp->d3d.u,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);
  vNew->d3d.v = CalcPerParF(v->d3d.v,v->d3d.oow,vp->d3d.v,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);

  vNew->d3d.rgbColor.r = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.r,v->d3d.oow,(long)vp->d3d.rgbColor.r,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);
  vNew->d3d.rgbColor.g = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.g,v->d3d.oow,(long)vp->d3d.rgbColor.g,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);
  vNew->d3d.rgbColor.b = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.b,v->d3d.oow,(long)vp->d3d.rgbColor.b,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);
  vNew->d3d.rgbColor.a = vp->d3d.rgbColor.a;
  //----------------------------------------------------------------------
  if (_gr_polygon.dwAddType & GR_POLY_ADD_BUMP) {
     //vNew->d3d.rgbColor.a = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.a,v->d3d.oow,(long)vp->d3d.rgbColor.a,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);

     _bumpOut->u = CalcPerParF(_bumpInNext->u,v->d3d.oow,_bumpIn->u,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);
     _bumpOut->v = CalcPerParF(_bumpInNext->v,v->d3d.oow,_bumpIn->v,vp->d3d.oow,y-vp->any.y,v->any.y-vp->any.y,vNew->d3d.oow);
  }

  //-----------------------------------------------------------------------
}

void VClipRGBSTZ(UGRVertex *vNew, UGRVertex *vp, UGRVertex *v,long x)
{

  vNew->any.x = x;
  vNew->any.y = (MulDivL((v->any.y-vp->any.y)<<10,x-vp->any.x,v->any.x-vp->any.x)>>10)+
                vp->any.y;
  vNew->d3d.oow = MulDivF(v->d3d.oow - vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x) + vp->d3d.oow;

  vNew->d3d.u = CalcPerParF(v->d3d.u,v->d3d.oow,vp->d3d.u,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);
  vNew->d3d.v = CalcPerParF(v->d3d.v,v->d3d.oow,vp->d3d.v,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);;

  vNew->d3d.rgbColor.r = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.r,v->d3d.oow,(long)vp->d3d.rgbColor.r,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);
  vNew->d3d.rgbColor.g = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.g,v->d3d.oow,(long)vp->d3d.rgbColor.g,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);
  vNew->d3d.rgbColor.b = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.b,v->d3d.oow,(long)vp->d3d.rgbColor.b,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);
  vNew->d3d.rgbColor.a = vp->d3d.rgbColor.a;
  //----------------------------------------------------------------------

  if (_gr_polygon.dwAddType & GR_POLY_ADD_BUMP) {
     //vNew->d3d.rgbColor.a = (unsigned char)CalcPerPar((long)v->d3d.rgbColor.a,v->d3d.oow,(long)vp->d3d.rgbColor.a,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);

     _bumpOut->u = CalcPerParF(_bumpInNext->u,v->d3d.oow,_bumpIn->u,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);
     _bumpOut->v = CalcPerParF(_bumpInNext->v,v->d3d.oow,_bumpIn->v,vp->d3d.oow,x-vp->any.x,v->any.x-vp->any.x,vNew->d3d.oow);
  }

  //-----------------------------------------------------------------------
}

/***********************************************************************
 ********************       ScreenShoot             ********************
 ***********************************************************************/

typedef struct {    //* rgbq
    BYTE    rgbBlue;
    BYTE    rgbGreen;
    BYTE    rgbRed;
    BYTE    rgbReserved;
} MRGBQUAD;

typedef struct {    // bmih
    DWORD   biSize;          // number of bytes required by the BITMAPINFOHEADER structure.
    LONG    biWidth;         // width of the bitmap, in pixels.
    LONG    biHeight;        // height of the bitmap, in pixels.
    WORD    biPlanes;        // number of planes for the target device. This member must be set to 1.
    WORD    biBitCount;      // number of bits per pixel. This value must be 1, 4, 8, or 24.
    DWORD   biCompression;   // type of compression for a compressed bitmap.
    DWORD   biSizeImage;     // size, in bytes, of the image
    LONG    biXPelsPerMeter; // horizontal resolution, in pixels per meter
    LONG    biYPelsPerMeter; // vertical resolution, in pixels per meter
    DWORD   biClrUsed;       // number of color indexes in the color table actually used by the bitmap
    DWORD   biClrImportant;  // the number of color indexes that are considered important
} MBITMAPINFOHEADER;


typedef struct {
   MBITMAPINFOHEADER    bmiHeader;
   MRGBQUAD             bmiColors[1];
} MBITMAPINFO;

typedef struct {
   char   bfType[2];
   DWORD  bfSize;
   WORD   bfReserved1,
          bfReserved2;
   DWORD  bfOffBits;
} MBITMAPFILEHEADER;


void GRSaveScrToBMP()
{  static char fname[13] ="rr2-000.bmp\0";
   FILE *f;
   unsigned long i,j,wid,need;
   unsigned char buf[3*2048];
   MBITMAPFILEHEADER bf;
   MBITMAPINFOHEADER bi;

   if (_dL.currDevice->swHw == GR_SOFTWARE) return;

   if ((f = fopen(fname,"wb")) == NULL) return;

   ++fname[6];
   if(fname[6] > '9') {
		fname[6] = '0';
		++fname[5];
      if(fname[5] > '9') {
			fname[5] = '0';
			++fname[4];
		}
	}

   wid = ((unsigned long)(_gr_nScreenWidth))*3ul;
	while ((wid&3) != 0 ) wid++;

   need = wid*_gr_nScreenHeight + sizeof(MBITMAPFILEHEADER) +
          sizeof(MBITMAPINFOHEADER);

   bf.bfType[0]   = 'B';
   bf.bfType[1]   = 'M';
   bf.bfSize      = sizeof(MBITMAPFILEHEADER)+sizeof(MBITMAPINFOHEADER)+need;
	bf.bfReserved1 = 0;
	bf.bfReserved2 = 0;
   bf.bfOffBits   = sizeof(MBITMAPFILEHEADER)+sizeof(MBITMAPINFOHEADER);

   bi.biSize          = sizeof(MBITMAPINFOHEADER);
   bi.biWidth         = (unsigned)(_gr_nScreenWidth);
   bi.biHeight        = (unsigned)(_gr_nScreenHeight);
   bi.biPlanes        = 1;
   bi.biBitCount      = 24;
   bi.biCompression   = 0;
   bi.biSizeImage     = need;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
   bi.biClrUsed       = 0;
   bi.biClrImportant  = 0;


	fwrite(&bf,1,sizeof(bf),f);
	fwrite(&bi,1,sizeof(bi),f);


   unsigned long *ptr;

   DDSURFACEDESC ddsd;
   ddsd.dwSize = sizeof(DDSURFACEDESC);

   int err = _BackBuffer->Lock(NULL,&ddsd,DDLOCK_READONLY | DDLOCK_WAIT,NULL);
   if (err != DD_OK) {
      D3DSetError("Can't Lock BackBuffer", err);
      return;
   }

   ptr = (unsigned long*)ddsd.lpSurface;

   for(j = (_gr_nScreenHeight-1)*ddsd.lPitch/4; ((long)j) >= 0; j -= ddsd.lPitch/4) {
      int pos = 0;

      for(i = j; i < j + _gr_nScreenWidth/2; ++i) {
			unsigned short p0,p1;

         p0 = (unsigned short)(ptr[i] & 0xFFFF);
         p1 = (unsigned short)(ptr[i] >> 16);

         buf[pos++] = (unsigned char)(((p0>>_bShift)<<_bScale)&0xFF);
         buf[pos++] = (unsigned char)(((p0>>_gShift)<<_gScale)&0xFF);
         buf[pos++] = (unsigned char)(((p0>>_rShift)<<_rScale)&0xFF);

         buf[pos++] = (unsigned char)(((p1>>_bShift)<<_bScale)&0xFF);
         buf[pos++] = (unsigned char)(((p1>>_gShift)<<_gScale)&0xFF);
         buf[pos++] = (unsigned char)(((p1>>_rShift)<<_rScale)&0xFF);
		}

		fwrite(buf,1,wid,f);
	}

	fclose(f);

   err = _BackBuffer->Unlock(NULL);
   if (err != DD_OK) {
      D3DSetError("Can't UnLock BackBuffer", err);
      return;
   }

 }



/***********************************************************************
 ********************    Convert error to string    ********************
 ***********************************************************************/

char _NoError[256];

char * D3DAppErrorToString(int error)
{
    switch(error) {
        case DD_OK:
            return "No error.\0";
        case DDERR_ALREADYINITIALIZED:
            return "This object is already initialized.\0";
        case DDERR_BLTFASTCANTCLIP:
            return "Return if a clipper object is attached to the source surface passed into a BltFast call.\0";
        case DDERR_CANNOTATTACHSURFACE:
            return "This surface can not be attached to the requested surface.\0";
        case DDERR_CANNOTDETACHSURFACE:
            return "This surface can not be detached from the requested surface.\0";
        case DDERR_CANTCREATEDC:
            return "Windows can not create any more DCs.\0";
        case DDERR_CANTDUPLICATE:
            return "Can't duplicate primary & 3D surfaces, or surfaces that are implicitly created.\0";
        case DDERR_CLIPPERISUSINGHWND:
            return "An attempt was made to set a cliplist for a clipper object that is already monitoring an hwnd.\0";
        case DDERR_COLORKEYNOTSET:
            return "No src color key specified for this operation.\0";
        case DDERR_CURRENTLYNOTAVAIL:
            return "Support is currently not available.\0";
        case DDERR_DIRECTDRAWALREADYCREATED:
            return "A DirectDraw object representing this driver has already been created for this process.\0";
        case DDERR_EXCEPTION:
            return "An exception was encountered while performing the requested operation.\0";
        case DDERR_EXCLUSIVEMODEALREADYSET:
            return "An attempt was made to set the cooperative level when it was already set to exclusive.\0";
        case DDERR_GENERIC:
            return "Generic failure.\0";
        case DDERR_HEIGHTALIGN:
            return "Height of rectangle provided is not a multiple of reqd alignment.\0";
        case DDERR_HWNDALREADYSET:
            return "The CooperativeLevel HWND has already been set. It can not be reset while the process has surfaces or palettes created.\0";
        case DDERR_HWNDSUBCLASSED:
            return "HWND used by DirectDraw CooperativeLevel has been subclassed, this prevents DirectDraw from restoring state.\0";
        case DDERR_IMPLICITLYCREATED:
            return "This surface can not be restored because it is an implicitly created surface.\0";
        case DDERR_INCOMPATIBLEPRIMARY:
            return "Unable to match primary surface creation request with existing primary surface.\0";
        case DDERR_INVALIDCAPS:
            return "One or more of the caps bits passed to the callback are incorrect.\0";
        case DDERR_INVALIDCLIPLIST:
            return "DirectDraw does not support the provided cliplist.\0";
        case DDERR_INVALIDDIRECTDRAWGUID:
            return "The GUID passed to DirectDrawCreate is not a valid DirectDraw driver identifier.\0";
        case DDERR_INVALIDMODE:
            return "DirectDraw does not support the requested mode.\0";
        case DDERR_INVALIDOBJECT:
            return "DirectDraw received a pointer that was an invalid DIRECTDRAW object.\0";
        case DDERR_INVALIDPARAMS:
            return "One or more of the parameters passed to the function are incorrect.\0";
        case DDERR_INVALIDPIXELFORMAT:
            return "The pixel format was invalid as specified.\0";
        case DDERR_INVALIDPOSITION:
            return "Returned when the position of the overlay on the destination is no longer legal for that destination.\0";
        case DDERR_INVALIDRECT:
            return "Rectangle provided was invalid.\0";
        case DDERR_LOCKEDSURFACES:
            return "Operation could not be carried out because one or more surfaces are locked.\0";
        case DDERR_NO3D:
            return "There is no 3D present.\0";
        case DDERR_NOALPHAHW:
            return "Operation could not be carried out because there is no alpha accleration hardware present or available.\0";
        case DDERR_NOBLTHW:
            return "No blitter hardware present.\0";
        case DDERR_NOCLIPLIST:
            return "No cliplist available.\0";
        case DDERR_NOCLIPPERATTACHED:
            return "No clipper object attached to surface object.\0";
        case DDERR_NOCOLORCONVHW:
            return "Operation could not be carried out because there is no color conversion hardware present or available.\0";
        case DDERR_NOCOLORKEY:
            return "Surface doesn't currently have a color key\0";
        case DDERR_NOCOLORKEYHW:
            return "Operation could not be carried out because there is no hardware support of the destination color key.\0";
        case DDERR_NOCOOPERATIVELEVELSET:
            return "Create function called without DirectDraw object method SetCooperativeLevel being called.\0";
        case DDERR_NODC:
            return "No DC was ever created for this surface.\0";
        case DDERR_NODDROPSHW:
            return "No DirectDraw ROP hardware.\0";
        case DDERR_NODIRECTDRAWHW:
            return "A hardware-only DirectDraw object creation was attempted but the driver did not support any hardware.\0";
        case DDERR_NOEMULATION:
            return "Software emulation not available.\0";
        case DDERR_NOEXCLUSIVEMODE:
            return "Operation requires the application to have exclusive mode but the application does not have exclusive mode.\0";
        case DDERR_NOFLIPHW:
            return "Flipping visible surfaces is not supported.\0";
        case DDERR_NOGDI:
            return "There is no GDI present.\0";
        case DDERR_NOHWND:
            return "Clipper notification requires an HWND or no HWND has previously been set as the CooperativeLevel HWND.\0";
        case DDERR_NOMIRRORHW:
            return "Operation could not be carried out because there is no hardware present or available.\0";
        case DDERR_NOOVERLAYDEST:
            return "Returned when GetOverlayPosition is called on an overlay that UpdateOverlay has never been called on to establish a destination.\0";
        case DDERR_NOOVERLAYHW:
            return "Operation could not be carried out because there is no overlay hardware present or available.\0";
        case DDERR_NOPALETTEATTACHED:
            return "No palette object attached to this surface.\0";
        case DDERR_NOPALETTEHW:
            return "No hardware support for 16 or 256 color palettes.\0";
        case DDERR_NORASTEROPHW:
            return "Operation could not be carried out because there is no appropriate raster op hardware present or available.\0";
        case DDERR_NOROTATIONHW:
            return "Operation could not be carried out because there is no rotation hardware present or available.\0";
        case DDERR_NOSTRETCHHW:
            return "Operation could not be carried out because there is no hardware support for stretching.\0";
        case DDERR_NOT4BITCOLOR:
            return "DirectDrawSurface is not in 4 bit color palette and the requested operation requires 4 bit color palette.\0";
        case DDERR_NOT4BITCOLORINDEX:
            return "DirectDrawSurface is not in 4 bit color index palette and the requested operation requires 4 bit color index palette.\0";
        case DDERR_NOT8BITCOLOR:
            return "DirectDrawSurface is not in 8 bit color mode and the requested operation requires 8 bit color.\0";
        case DDERR_NOTAOVERLAYSURFACE:
            return "Returned when an overlay member is called for a non-overlay surface.\0";
        case DDERR_NOTEXTUREHW:
            return "Operation could not be carried out because there is no texture mapping hardware present or available.\0";
        case DDERR_NOTFLIPPABLE:
            return "An attempt has been made to flip a surface that is not flippable.\0";
        case DDERR_NOTFOUND:
            return "Requested item was not found.\0";
        case DDERR_NOTLOCKED:
            return "Surface was not locked.  An attempt to unlock a surface that was not locked at all, or by this process, has been attempted.\0";
        case DDERR_NOTPALETTIZED:
            return "The surface being used is not a palette-based surface.\0";
        case DDERR_NOVSYNCHW:
            return "Operation could not be carried out because there is no hardware support for vertical blank synchronized operations.\0";
        case DDERR_NOZBUFFERHW:
            return "Operation could not be carried out because there is no hardware support for zbuffer blitting.\0";
        case DDERR_NOZOVERLAYHW:
            return "Overlay surfaces could not be z layered based on their BltOrder because the hardware does not support z layering of overlays.\0";
        case DDERR_OUTOFCAPS:
            return "The hardware needed for the requested operation has already been allocated.\0";
        case DDERR_OUTOFMEMORY:
            return "DirectDraw does not have enough memory to perform the operation.\0";
        case DDERR_OUTOFVIDEOMEMORY:
            return "DirectDraw does not have enough memory to perform the operation.\0";
        case DDERR_OVERLAYCANTCLIP:
            return "The hardware does not support clipped overlays.\0";
        case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
            return "Can only have ony color key active at one time for overlays.\0";
        case DDERR_OVERLAYNOTVISIBLE:
            return "Returned when GetOverlayPosition is called on a hidden overlay.\0";
        case DDERR_PALETTEBUSY:
            return "Access to this palette is being refused because the palette is already locked by another thread.\0";
        case DDERR_PRIMARYSURFACEALREADYEXISTS:
            return "This process already has created a primary surface.\0";
        case DDERR_REGIONTOOSMALL:
            return "Region passed to Clipper::GetClipList is too small.\0";
        case DDERR_SURFACEALREADYATTACHED:
            return "This surface is already attached to the surface it is being attached to.\0";
        case DDERR_SURFACEALREADYDEPENDENT:
            return "This surface is already a dependency of the surface it is being made a dependency of.\0";
        case DDERR_SURFACEBUSY:
            return "Access to this surface is being refused because the surface is already locked by another thread.\0";
        case DDERR_SURFACEISOBSCURED:
            return "Access to surface refused because the surface is obscured.\0";
        case DDERR_SURFACELOST:
            return "Access to this surface is being refused because the surface memory is gone. The DirectDrawSurface object representing this surface should have Restore called on it.\0";
        case DDERR_SURFACENOTATTACHED:
            return "The requested surface is not attached.\0";
        case DDERR_TOOBIGHEIGHT:
            return "Height requested by DirectDraw is too large.\0";
        case DDERR_TOOBIGSIZE:
            return "Size requested by DirectDraw is too large, but the individual height and width are OK.\0";
        case DDERR_TOOBIGWIDTH:
            return "Width requested by DirectDraw is too large.\0";
        case DDERR_UNSUPPORTED:
            return "Action not supported.\0";
        case DDERR_UNSUPPORTEDFORMAT:
            return "FOURCC format requested is unsupported by DirectDraw.\0";
        case DDERR_UNSUPPORTEDMASK:
            return "Bitmask in the pixel format requested is unsupported by DirectDraw.\0";
        case DDERR_VERTICALBLANKINPROGRESS:
            return "Vertical blank is in progress.\0";
        case DDERR_WASSTILLDRAWING:
            return "Informs DirectDraw that the previous Blt which is transfering information to or from this Surface is incomplete.\0";
        case DDERR_WRONGMODE:
            return "This surface can not be restored because it was created in a different mode.\0";
        case DDERR_XALIGN:
            return "Rectangle provided was not horizontally aligned on required boundary.\0";
        case D3DERR_BADMAJORVERSION:
            return "D3DERR_BADMAJORVERSION\0";
        case D3DERR_BADMINORVERSION:
            return "D3DERR_BADMINORVERSION\0";
        case D3DERR_EXECUTE_LOCKED:
            return "D3DERR_EXECUTE_LOCKED\0";
        case D3DERR_EXECUTE_NOT_LOCKED:
            return "D3DERR_EXECUTE_NOT_LOCKED\0";
        case D3DERR_EXECUTE_CREATE_FAILED:
            return "D3DERR_EXECUTE_CREATE_FAILED\0";
        case D3DERR_EXECUTE_DESTROY_FAILED:
            return "D3DERR_EXECUTE_DESTROY_FAILED\0";
        case D3DERR_EXECUTE_LOCK_FAILED:
            return "D3DERR_EXECUTE_LOCK_FAILED\0";
        case D3DERR_EXECUTE_UNLOCK_FAILED:
            return "D3DERR_EXECUTE_UNLOCK_FAILED\0";
        case D3DERR_EXECUTE_FAILED:
            return "D3DERR_EXECUTE_FAILED\0";
        case D3DERR_EXECUTE_CLIPPED_FAILED:
            return "D3DERR_EXECUTE_CLIPPED_FAILED\0";
        case D3DERR_TEXTURE_NO_SUPPORT:
            return "D3DERR_TEXTURE_NO_SUPPORT\0";
        case D3DERR_TEXTURE_NOT_LOCKED:
            return "D3DERR_TEXTURE_NOT_LOCKED\0";
        case D3DERR_TEXTURE_LOCKED:
            return "D3DERR_TEXTURELOCKED\0";
        case D3DERR_TEXTURE_CREATE_FAILED:
            return "D3DERR_TEXTURE_CREATE_FAILED\0";
        case D3DERR_TEXTURE_DESTROY_FAILED:
            return "D3DERR_TEXTURE_DESTROY_FAILED\0";
        case D3DERR_TEXTURE_LOCK_FAILED:
            return "D3DERR_TEXTURE_LOCK_FAILED\0";
        case D3DERR_TEXTURE_UNLOCK_FAILED:
            return "D3DERR_TEXTURE_UNLOCK_FAILED\0";
        case D3DERR_TEXTURE_LOAD_FAILED:
            return "D3DERR_TEXTURE_LOAD_FAILED\0";
        case D3DERR_MATRIX_CREATE_FAILED:
            return "D3DERR_MATRIX_CREATE_FAILED\0";
        case D3DERR_MATRIX_DESTROY_FAILED:
            return "D3DERR_MATRIX_DESTROY_FAILED\0";
        case D3DERR_MATRIX_SETDATA_FAILED:
            return "D3DERR_MATRIX_SETDATA_FAILED\0";
        case D3DERR_SETVIEWPORTDATA_FAILED:
            return "D3DERR_SETVIEWPORTDATA_FAILED\0";
        case D3DERR_MATERIAL_CREATE_FAILED:
            return "D3DERR_MATERIAL_CREATE_FAILED\0";
        case D3DERR_MATERIAL_DESTROY_FAILED:
            return "D3DERR_MATERIAL_DESTROY_FAILED\0";
        case D3DERR_MATERIAL_SETDATA_FAILED:
            return "D3DERR_MATERIAL_SETDATA_FAILED\0";
        case D3DERR_LIGHT_SET_FAILED:
            return "D3DERR_LIGHT_SET_FAILED\0";
        default: {
            sprintf(_NoError,"%X",error);
            return _NoError;
        }
    }
}

/***********************************************************************
 ********************    Convert error to string    ********************
 ***********************************************************************/

void D3DSetError(char * title, int error)
{
  strcpy(_LastError, title);
  strcat(_LastError, ":\n");
  strcat(_LastError, D3DAppErrorToString(error));

}