#include <windows.h>
#include <math.h>

//#define _LIGHT_CPP_
#include "graph.h"
#include "d3d.h"
#include "drawd3d.h"
//#include "light.h"

extern SDeviceList _dL;
extern IDirect3DDevice2       *_d3dDevice;

//extern "C" SGRPolygon _gr_polygon;
extern void D3D_UnLockTexture(void *handle);
extern unsigned char * D3D_LockTexture(void *handle, long *pitch);
extern int LoadTextureTo3DCard(void *tDB);


extern float _hazeK, _ifogStart, _ifogEnd;
//extern SRectangle _gr_ilyaClip;

//extern "C" {
void * _RayTextH = NULL;
//}



#define I65536 (0.00001525878906)


#define RAY_TEXTURE_W 32
#define RAY_TEXTURE_H 32

#define COPY_FLOAT(a,b) *((int*)&(a)) = *((int*)&(b))
#define SET_FLOAT_ZERO(a) *((int*)&(a)) = 0
#define FLOAT_NEG(a,b) *((int*)&(a)) = (*((int*)&(b)) ^ 0x80000000)
#define FLOAT_CHANGE_SIGN(f) (*((int*)&(f)) ^= 0x80000000)


int GRInitRay(GR_HTEXTURE hT)
{ unsigned char ray[RAY_TEXTURE_W*RAY_TEXTURE_H+12];
  unsigned char *rayData;
  int b;

  if (_dL.currDevice == NULL) return 0;


  rayData = ray + 12;
  *TEXT_HEIGHT_PTR(rayData) = RAY_TEXTURE_H;
  *TEXT_WIDTH_PTR(rayData) = RAY_TEXTURE_W;
  *TEXT_FLAG_PTR(rayData) = TEXTURE_UNLOAD | TEXTURE_ALPHA |
                             TEXTURE_PAL_FORMAT;

  #define END_0 3

  //fill texture data
  for(int i = 0;i < RAY_TEXTURE_W;i++) {
     if (i < END_0) b = 0;
     else b = (255 * (i-(END_0-1))) / (RAY_TEXTURE_W - END_0);
     for(int j = 0;j < RAY_TEXTURE_H;j++) {
        rayData[j*RAY_TEXTURE_W+i] = (unsigned char)b;
     }
  }

  _RayTextH = GRLoadTextureToDB(hT, NULL, 0, rayData);

  if (_RayTextH == NULL) return 0;

  GRSetTextureLoadIntFunc( _RayTextH, GRInitRay);

  return 1;
}

extern int __HazeStartInt;


extern "C" void GRDrawRay(int x0, int y0, int x1, int y1, unsigned long color,int opacity, int iz, float width2, float center)
{
  float xc = (float)x0 + (float)(x1 - x0)*center;
  float yc = (float)y0 + (float)(y1 - y0)*center;
  float wc = width2*center;
  float invLen = 1.0 / sqrt((float)(x1-x0)*(float)(x1-x0) +
                            (float)(y1-y0)*(float)(y1-y0));

  int addType = iz < __HazeStartInt?GR_POLY_ADD_HAZE:0;

  if (_dL.currDevice->swHw == GR_SOFTWARE) {

     int uCoord = (((RAY_TEXTURE_W-4)*opacity) / 255 + 2)<<16;

     GRSetZPrecision(0);

     _gr_polygon.dwFullType = GR_POLY_TEXTURE_ALPHA;
     _gr_polygon.nVertices = 3;
     _gr_polygon.hTexture = _RayTextH;
     _gr_polygon.dwColor.color = color;
     _gr_polygon.dwOpacity = opacity;
     _gr_polygon.dwAddType = addType;

     float iSin = (y1 - y0) * invLen;
     float iCos = (x1 - x0) * invLen;
     float wX, wY;

     _gr_vertices[0].any.x = x0;
     _gr_vertices[0].any.y = y0;
     _gr_vertices[0].any.iz = iz;
     _gr_vertices[0].texture.u = 1<<16;
     _gr_vertices[0].texture.v = (RAY_TEXTURE_H/2)<<16;

     wX = wc*iSin;
     wY = wc*iCos;

     int xc0,yc0,xc1,yc1;

     _gr_vertices[1].any.x = xc1 = (int)(xc - wX);
     _gr_vertices[1].any.y = yc1 = (int)(yc + wY);
     _gr_vertices[1].any.iz = iz;
     _gr_vertices[1].texture.u = uCoord;
     _gr_vertices[1].texture.v = (RAY_TEXTURE_H-2)<<16;

     _gr_vertices[2].any.x = xc0 = (int)(xc + wX);
     _gr_vertices[2].any.y = yc0 = (int)(yc - wY);
     _gr_vertices[2].any.iz = iz;
     _gr_vertices[2].texture.u = uCoord;
     _gr_vertices[2].texture.v = 1<<16;

     GRDrawPolygonPCCW();


     _gr_polygon.dwFullType = GR_POLY_TEXTURE_ALPHA;
     _gr_polygon.nVertices = 4;
     //_gr_polygon.hTexture = _RayTextH;
     _gr_polygon.dwColor.color = color;
     //_gr_polygon.dwOpacity = opacity;
     _gr_polygon.dwAddType = addType;

     wX = width2*iSin;
     wY = width2*iCos;

     _gr_vertices[0].any.x = xc0;
     _gr_vertices[0].any.y = yc0;
     _gr_vertices[0].any.iz = iz;
     _gr_vertices[0].texture.u = uCoord - (1<<16);
     _gr_vertices[0].texture.v = 1<<16;

     _gr_vertices[1].any.x = xc1;
     _gr_vertices[1].any.y = yc1;
     _gr_vertices[1].any.iz = iz;
     _gr_vertices[1].texture.u = uCoord - (1<<16);
     _gr_vertices[1].texture.v = (RAY_TEXTURE_H-2)<<16;

     _gr_vertices[2].any.x = (int)(x1 - wX);
     _gr_vertices[2].any.y = (int)(y1 + wY);
     _gr_vertices[2].any.iz = iz;
     _gr_vertices[2].texture.u = 1<<16;
     _gr_vertices[2].texture.v = (RAY_TEXTURE_H-2)<<16;

     _gr_vertices[3].any.x = xc0 = (int)(x1 + wX);
     _gr_vertices[3].any.y = yc0 = (int)(y1 - wY);
     _gr_vertices[3].any.iz = iz;
     _gr_vertices[3].texture.u = 1<<16;
     _gr_vertices[3].texture.v = 1<<16;

     GRDrawPolygonPCCW();
  }
  else {
     UGRVertex *v;
     D3D_ZList *zl;
     float z = 1. - (float)iz * I65536 * _gr_fFrontClip;

     zl = D3D_InsertToZList(z);
     v = zl->v;
     zl->type = 1;
     zl->vertC = 3;

     zl->polygon.dwFullType = GR_POLY_TEXTURE_ALPHA;
     zl->polygon.nVertices = 3;
     zl->polygon.hTexture = _RayTextH;
     zl->polygon.dwColor.color = color;
     zl->polygon.dwOpacity = opacity;
     zl->polygon.dwAddType = addType;

     float iSin = (y1 - y0) * invLen;
     float iCos = (x1 - x0) * invLen;
     float wX, wY;

     v[0].any.x = x0;
     v[0].any.y = y0;
     v[0].any.iz = iz;
     v[0].texture.u = 1<<16;
     v[0].texture.v = (RAY_TEXTURE_H/2)<<16;

     wX = wc*iSin;
     wY = wc*iCos;

     int xc0,yc0,xc1,yc1;

     v[1].any.x = xc1 = (int)(xc - wX);
     v[1].any.y = yc1 = (int)(yc + wY);
     v[1].any.iz = iz;
     v[1].texture.u = (RAY_TEXTURE_W-2)<<16;
     v[1].texture.v = (RAY_TEXTURE_H-2)<<16;

     v[2].any.x = xc0 = (int)(xc + wX);
     v[2].any.y = yc0 = (int)(yc - wY);
     v[2].any.iz = iz;
     v[2].texture.u = (RAY_TEXTURE_W-2)<<16;
     v[2].texture.v = 1<<16;

     //GRDrawPolygonPCCW();
     zl = D3D_InsertToZList(z);
     v = zl->v;
     zl->type = 1;
     zl->vertC = 4;


     zl->polygon.dwFullType = GR_POLY_TEXTURE_ALPHA;
     zl->polygon.nVertices = 4;
     zl->polygon.hTexture = _RayTextH;
     zl->polygon.dwColor.color = color;
     zl->polygon.dwOpacity = opacity;
     zl->polygon.dwAddType = addType;

     wX = width2*iSin;
     wY = width2*iCos;

     v[0].any.x = xc0;
     v[0].any.y = yc0;
     v[0].any.iz = iz;
     v[0].texture.u = (RAY_TEXTURE_W-3)<<16;
     v[0].texture.v = 1<<16;

     v[1].any.x = xc1;
     v[1].any.y = yc1;
     v[1].any.iz = iz;
     v[1].texture.u = (RAY_TEXTURE_W-3)<<16;
     v[1].texture.v = (RAY_TEXTURE_H-2)<<16;

     v[2].any.x = (int)(x1 - wX);
     v[2].any.y = (int)(y1 + wY);
     v[2].any.iz = iz;
     v[2].texture.u = 1<<16;
     v[2].texture.v = (RAY_TEXTURE_H-2)<<16;

     v[3].any.x = xc0 = (int)(x1 + wX);
     v[3].any.y = yc0 = (int)(y1 - wY);
     v[3].any.iz = iz;
     v[3].texture.u = 1<<16;
     v[3].texture.v = 1<<16;
  }
}