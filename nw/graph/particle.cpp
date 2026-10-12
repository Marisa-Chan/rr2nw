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

/*
extern "C" long _gr_nScreenOriginX;
extern "C" long _gr_nScreenOriginY;
 */

extern float _hazeK, _ifogStart, _ifogEnd;
//extern SRectangle _gr_ilyaClip;

//extern "C" {
   void * _ParticleTextH = NULL;
//}

unsigned char _Particle[13];
unsigned char *_ParticleData = NULL;


#define I65536 (0.00001525878906)


#define PARTICLE_TEXTURE_SIZE 16

#define COPY_FLOAT(a,b) *((int*)&(a)) = *((int*)&(b))
#define SET_FLOAT_ZERO(a) *((int*)&(a)) = 0
#define FLOAT_NEG(a,b) *((int*)&(a)) = (*((int*)&(b)) ^ 0x80000000)
#define FLOAT_CHANGE_SIGN(f) (*((int*)&(f)) ^= 0x80000000)


int GRInitParticle(GR_HTEXTURE hT)
{
  if (_dL.currDevice == NULL) return 0;


  if (_dL.currDevice->swHw == GR_HARDWARE) {
     _ParticleData = _Particle + 12;
     *TEXT_HEIGHT_PTR(_ParticleData) = PARTICLE_TEXTURE_SIZE;
     *TEXT_WIDTH_PTR(_ParticleData) = PARTICLE_TEXTURE_SIZE;
     *TEXT_FLAG_PTR(_ParticleData) = TEXTURE_UNLOAD | TEXTURE_ALPHA |
                                  TEXTURE_MEM_FORMAT | TEXTURE_NOT_PRESENT;

     _ParticleTextH = GRLoadTextureToDB(hT, NULL, 0, _ParticleData);
     if (_ParticleTextH == NULL) return 0;

     GRSetTextureLoadIntFunc( _ParticleTextH, GRInitParticle);

     STextureFormat *tF = & _dL.currDevice->textureFormat[ALPHA_TEXTURE_INDEX];

     long pitch;
     unsigned char *ptr;
     unsigned short *ptr1;

     ptr = D3D_LockTexture(_ParticleTextH, &pitch);

     for(int j = 0;j < PARTICLE_TEXTURE_SIZE;j++,ptr+=pitch) {
        ptr1 = (unsigned short *) ptr;
        for(int i = 0;i < PARTICLE_TEXTURE_SIZE;i++,ptr1++) {
           unsigned long rgb,a;
           long b;
           float r;

           r = sqrt((i+0.5-PARTICLE_TEXTURE_SIZE/2)*(i+0.5-PARTICLE_TEXTURE_SIZE/2) +
                    (j+0.5-PARTICLE_TEXTURE_SIZE/2)*(j+0.5-PARTICLE_TEXTURE_SIZE/2));

           b = 480 - (int)(r*530.0/(PARTICLE_TEXTURE_SIZE/2));

           if (b > 255) b = 255;
           else if (b < 0) b = 0;

           rgb = 255;//b;
           a = b;


           *ptr1 = (unsigned short)(((rgb>>tF->redScale)<<tF->redShift) |
                              ((rgb>>tF->greenScale)<<tF->greenShift) |
                              ((rgb>>tF->blueScale)<<tF->blueShift) |
                              ((a>>tF->alphaScale)<<tF->alphaShift)
                             );
        }
     }

     D3D_UnLockTexture(_ParticleTextH);
  }

  return 1;
}


extern "C" void D3D_GRDrawParticle(int x, int y, int size, int iz, unsigned long color)
{ UGRVertex *v;
  D3D_ZList *zl;
  int x0,y0,x1,y1;
  float oow = (float)iz * I65536;
  float z = 1. - oow * _gr_fFrontClip;//_hazeK / oow;
  unsigned long specular = 0xFF000000;
  unsigned long colHW = (color>>8) | 0xFF000000;

  x0 = x - size/2;
  y0 = y - size/2;
  x1 = x + size/2 + 1;
  y1 = y + size/2 + 1;

  if (x0 >= _gr_clipRect.right || x1 < _gr_clipRect.left ||
      y0 >= _gr_clipRect.bottom || y1 < _gr_clipRect.top)
     return;

  if (x0 < _gr_clipRect.left || x1 > _gr_clipRect.right ||
      y0 < _gr_clipRect.top || y1 > _gr_clipRect.bottom)
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
  v = zl->v;
  zl->type = 0;
  zl->texture = _ParticleTextH;

  v[0].d3d.x = x0 + _gr_nScreenOriginX;
  v[0].d3d.y = y0 + _gr_nScreenOriginY;
  COPY_FLOAT(v[0].d3d.z, z);
  COPY_FLOAT(v[0].d3d.oow, oow);
  SET_FLOAT_ZERO(v[0].d3d.u);
  SET_FLOAT_ZERO(v[0].d3d.v);
  v[0].d3d.specular = specular;
  v[0].d3d.d3dcolor.color = colHW;

  COPY_FLOAT(v[1].d3d.x, v[0].d3d.x);
  v[1].d3d.y = y1 + _gr_nScreenOriginY;
  COPY_FLOAT(v[1].d3d.z, z);
  COPY_FLOAT(v[1].d3d.oow, oow);
  SET_FLOAT_ZERO(v[1].d3d.u);
  v[1].d3d.v = 0.99;
  v[1].d3d.specular = specular;
  v[1].d3d.d3dcolor.color = colHW;

  v[2].d3d.x = x1 + _gr_nScreenOriginX;
  COPY_FLOAT(v[2].d3d.y, v[1].d3d.y);
  COPY_FLOAT(v[2].d3d.z, z);
  COPY_FLOAT(v[2].d3d.oow, oow);
  v[2].d3d.u = 0.99;
  v[2].d3d.v = 0.99;
  v[2].d3d.specular = specular;
  v[2].d3d.d3dcolor.color = colHW;

  COPY_FLOAT(v[3].d3d.x, v[2].d3d.x);
  COPY_FLOAT(v[3].d3d.y, v[0].d3d.y);
  COPY_FLOAT(v[3].d3d.z, z);
  COPY_FLOAT(v[3].d3d.oow, oow);
  v[3].d3d.u = 0.99;
  SET_FLOAT_ZERO(v[3].d3d.v);
  v[3].d3d.specular = specular;
  v[3].d3d.d3dcolor.color = colHW;

  /*
  if (x0 < _gr_clipRect.left) {
     v[0].d3d.u = ((_gr_ilyaClip.left - v[0].d3d.x) * (v[2].d3d.u - v[0].d3d.u)) /
                     (v[2].d3d.x - v[0].d3d.x) + v[0].d3d.u;
     v[0].d3d.x = _gr_ilyaClip.left;
     v[1].d3d.x = _gr_ilyaClip.left;
     v[1].d3d.u = v[0].d3d.u;
  }

  if (x1 > _gr_clipRect.right) {
     v[2].d3d.u = ((_gr_ilyaClip.right - v[0].d3d.x) * (v[2].d3d.u - v[0].d3d.u)) /
                     (v[2].d3d.x - v[0].d3d.x) + v[0].d3d.u;
     v[2].d3d.x = _gr_ilyaClip.right;
     v[3].d3d.x = _gr_ilyaClip.right;
     v[3].d3d.u = v[2].d3d.u;
  }

  if (y0 < _gr_clipRect.top) {
     v[0].d3d.v = ((_gr_ilyaClip.top - v[0].d3d.y) * (v[1].d3d.v - v[0].d3d.v)) /
                     (v[1].d3d.y - v[0].d3d.y) + v[0].d3d.v;
     v[0].d3d.y = _gr_ilyaClip.top;
     v[3].d3d.y = _gr_ilyaClip.top;
     v[3].d3d.v = v[0].d3d.v;
  }

  if (y1 > _gr_clipRect.bottom) {
     v[1].d3d.v = ((_gr_ilyaClip.bottom - v[0].d3d.y) * (v[1].d3d.v - v[0].d3d.v)) /
                     (v[1].d3d.y - v[0].d3d.y) + v[0].d3d.v;
     v[1].d3d.y = _gr_ilyaClip.bottom;
     v[2].d3d.y = _gr_ilyaClip.bottom;
     v[2].d3d.v = v[1].d3d.v;
  }
  */


  /*
  if ( LoadTextureTo3DCard(_ParticleTextH) != TRUE) return;



  _d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                    (LPVOID) v, 4, D3DDP_DONOTCLIP);

  */
}