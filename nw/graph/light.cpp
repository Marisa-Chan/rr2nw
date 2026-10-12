#include <windows.h>
#include <math.h>

#define _LIGHT_CPP_
#include "graph.h"
#include "d3d.h"
//#include "light.h"

#define LIGHT_SIZE 32


extern SDeviceList _dL;
extern IDirect3DDevice2       *_d3dDevice;

extern "C" SGRPolygon _gr_polygon;
extern void D3D_UnLockTexture(void *handle);
extern unsigned char * D3D_LockTexture(void *handle, long *pitch);
/*
extern "C" long _gr_nScreenOriginX;
extern "C" long _gr_nScreenOriginY;
 */

#define LIGHT_LIGHT  0
#define LIGHT_SHADOW 1

extern "C" {
   void * _LightTextH[2] = {NULL,NULL};
   SGRLight _gr_pLights[LIGHT_SOURCE_COUNT];
}

unsigned char _Light[13];
unsigned char *_LightData[2] = {NULL,};
//unsigned char *_LightDataSW = NULL;
//unsigned short *_LightTmp = NULL;

extern float _fogStart, _fogEnd;

UGRVertex _vert[128];


float _LightSize = 0;
//float _LightSizeSW = 0;

int _LightNum = 0;

//SGRLight _LightMem[LIGHT_SOURCE_COUNT];
//int    _LightFreeMem = 0;

unsigned long _LightScaleTable[LIGHT_COLOR_COUNT][256];
unsigned char *_LightMixTable = NULL;
//STextureFormat *_LightFmt;

void SetMixLightTable(unsigned char *ptr)
{
   if (_LightMixTable == NULL) return;

   memcpy(_LightMixTable, ptr, LIGHT_COLOR_COUNT*32*256);

}

int GRInitLight(GR_HTEXTURE hT)
{ int i;
  int ch0 = 0, ch1 = 0;

  if (_dL.currDevice == NULL) return 0;

  if (hT == NULL) {
     ch0 = 1;
     ch1 = 1;
  }
  else {
     if (_LightTextH[0] == hT) ch0 = 1;
     if (_LightTextH[1] == hT) ch1 = 1;
  }

  if (ch0) {
     _LightSize = LIGHT_SIZE;

     //color > 0
     _LightData[0] = _Light + 12;
     *TEXT_HEIGHT_PTR(_LightData[0]) = LIGHT_SIZE;
     *TEXT_WIDTH_PTR(_LightData[0]) = LIGHT_SIZE;
     *TEXT_FLAG_PTR(_LightData[0]) = TEXTURE_UNLOAD | TEXTURE_RGB |
                                  TEXTURE_MEM_FORMAT | TEXTURE_NOT_PRESENT;

     _LightTextH[0] = GRLoadTextureToDB(hT, NULL, 0, _LightData[0]);

     if (_LightTextH[0] == NULL) return FALSE;

     GRSetTextureLoadIntFunc( _LightTextH[0], GRInitLight);
  }

  if (ch1) {
     _LightSize = LIGHT_SIZE;
     //color == 0
     _LightData[1] = _Light + 12;
     *TEXT_HEIGHT_PTR(_LightData[1]) = LIGHT_SIZE*2;
     *TEXT_WIDTH_PTR(_LightData[1]) = LIGHT_SIZE*2;
     *TEXT_FLAG_PTR(_LightData[1]) = TEXTURE_UNLOAD | TEXTURE_ALPHA |
                                     TEXTURE_NOT_PRESENT;

     _LightTextH[1] = GRLoadTextureToDB(hT, NULL, 0, _LightData[1]);

     if (_LightTextH[1] == NULL) return FALSE;

     GRSetTextureLoadIntFunc( _LightTextH[1], GRInitLight);

  }

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     STextureFormat *tF = & _dL.currDevice->textureFormat[RGB_TEXTURE_INDEX];
     STextureFormat *tF1 = & _dL.currDevice->textureFormat[ALPHA_TEXTURE_INDEX];

     //Fill color table
     for(int j = 0;j < LIGHT_COLOR_COUNT;j++) {
        unsigned long mask = 0;

        if (j&1) //red
           mask = 0xff0000;

        if (j&2) //green
           mask |= 0xff00;

        if (j&4) //blue
           mask |= 0xff;


        for(unsigned long i = 0;i < 256;i++) {
           unsigned long color;

           if (j == 0) {
              color = ((255-i)<<16) |
                      ((255-i)<<8) |
                      (255-i);
              //color = ((255-i)<<16) |
              //   ((255-i)<<8) |
              //   ((255-i));
              _LightScaleTable[j][i] = 0xffffff;//color;
           }
           else {
              color = (i<<16) |
                      (i<<8) |
                      (i);
              //color = (i<<16) |
              //   (i<<8) |
              //   (i);
              _LightScaleTable[j][i] = color & mask;
           }
        }
     }
     //_LightFmt = tF;
     //if (_LightMixTable != NULL) {
     //   delete [] _LightMixTable;
     //   _LightMixTable = NULL;
     //}
     long pitch;
     unsigned char *ptr;

     if (ch0) {
        //color > 0
        ptr = D3D_LockTexture(_LightTextH[0], &pitch);

        for(int j = 0;j < LIGHT_SIZE;j++,ptr += pitch) {
           unsigned short *ptr1 = (unsigned short*)ptr;
           for(i = 0;i < LIGHT_SIZE;i++,ptr1++) {
              float r = sqrt((j-16)*(j-16) + (i-16)*(i-16));
              int b;

              b = 255 - (int)((r*255.)/16.);
              if (b>255)
                 b = 255;
              else
                 if (b < 0) b = 0;

              *ptr1 = (unsigned short)(((b>>tF->redScale)<<tF->redShift) |
                      ((b>>tF->greenScale)<<tF->greenShift) |
                      ((b>>tF->blueScale)<<tF->blueShift));
           }
        }

        // Unlock the surface
        D3D_UnLockTexture(_LightTextH[0]);
     }

     if (ch1) {
        //color == 0
        ptr = D3D_LockTexture(_LightTextH[1], &pitch);


        for(int j = 0;j < LIGHT_SIZE*2;j++,ptr += pitch) {
           unsigned short *ptr1 = (unsigned short*)ptr;
           for(i = 0;i< LIGHT_SIZE*2;i++,ptr1++) {
              float r = sqrt((j-16*2)*(j-16*2) + (i-16*2)*(i-16*2));
              int b;

              if (r >= 15.0*2)
                 b = 0;
              else {
                 b = 280 - (int)((r*255.)/(16.*2));
                 if (b > 255) b = 255;
                 else if (b < 0) b = 0;
              }

              *ptr1 = (unsigned short)(((b>>tF1->alphaScale)<<tF1->alphaShift));
           }
        }

        // Unlock the surface
        D3D_UnLockTexture(_LightTextH[1]);
     }
  }
  else {
     if (_LightMixTable == NULL)
        _LightMixTable = (unsigned char*) new char [LIGHT_COLOR_COUNT*32*256];

     //for(int j = 0;j < 32*256;j++) _LightMixTable[j] = (j&255 + j/256)&255;

  }


  for(i = 0;i  < LIGHT_SOURCE_COUNT;i++)
     _gr_pLights[i].type = GR_LIGHT;

  return TRUE;
}

/*
void * GRCreateLightSource(float x, float y, float z, float rad, int power, int color)
{ SGRLight *l = &_LightMem[_LightFreeMem++];

  if (_LightTextH[0] == NULL) return NULL;

  if (_LightFreeMem >= LIGHT_SOURCE_COUNT) _LightFreeMem = 0;

  while (_LightMem[_LightFreeMem].use != 0)
     if (++_LightFreeMem >= LIGHT_SOURCE_COUNT) _LightFreeMem = 0;

  l->x = x;
  l->y = y;
  l->z = z;
  l->r = rad;
  l->r2 = rad*rad;
  l->power0 = power;

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     l->colorTable = _LightScaleTable[color];
     l->power = power;
     l->handler = (color == 0?_LightTextH[LIGHT_SHADOW]:_LightTextH[LIGHT_LIGHT]);
  }
  else {
     l->colorTable = (unsigned long*)&_LightMixTable[color*32*256];
     l->power = power>>3;
  }

  l->color = color;

  l->use = 1;

  return l;
}

void GRDeleteLightSource(void *handle)
{
  ((SGRLight*)handle)->use = 0;
}
*/

typedef struct {
   float uX, uY, uZ;
   float vX, vY, vZ;
}SMatr2x3;

#define COPY_FLOAT(a,b) *((int*)&(a)) = *((int*)&(b))
#define SET_FLOAT_ZERO(a) *((int*)&(a)) = 0
#define FLOAT_NEG(a,b) *((int*)&(a)) = (*((int*)&(b)) ^ 0x80000000)
#define FLOAT_CHANGE_SIGN(f) (*((int*)&(f)) ^= 0x80000000)


void CalculateTransform(SMatr2x3 * matr)
{ int c;
  float len;

  //--------Find Max Normal Element------------------
  if (fabs(_gr_polygon.a) >= fabs(_gr_polygon.b))
     if (fabs(_gr_polygon.a) >= fabs(_gr_polygon.c)) c = 0;
     else c = 2;
  else
     if (fabs(_gr_polygon.b) >= fabs(_gr_polygon.c)) c = 1;
     else c = 2;

  //------------Calc Matrix--------------------
  switch (c) {
     case 0: //X max
     case 2: //Z max
        //1 col (0 1 0)x(a b c)
        COPY_FLOAT(matr->uX, _gr_polygon.c);
        SET_FLOAT_ZERO(matr->uY);
        FLOAT_NEG(matr->uZ,_gr_polygon.a);
        //----normalize
        len = 1.0 / sqrt(matr->uX*matr->uX + matr->uZ*matr->uZ);
        matr->uX *= len;
        matr->uZ *= len;
        //2 col
        matr->vX = -matr->uZ * _gr_polygon.b;
        matr->vY = matr->uZ * _gr_polygon.a - matr->uX * _gr_polygon.c;
        matr->vZ = matr->uX * _gr_polygon.b;
        break;
     case 1: //Y max
        //1 col (0 0 1)x(a b c)
        FLOAT_NEG(matr->uX, _gr_polygon.b);
        COPY_FLOAT(matr->uY, _gr_polygon.a);
        SET_FLOAT_ZERO(matr->uZ);
        //----normalize
        len = 1.0 / sqrt(matr->uX*matr->uX + matr->uY*matr->uY);
        matr->uX *= len;
        matr->uY *= len;
        //2 col
        matr->vX = matr->uY * _gr_polygon.c;
        matr->vY = -matr->uX * _gr_polygon.c;
        matr->vZ = matr->uX * _gr_polygon.b - matr->uY * _gr_polygon.a;
        break;
  }

}

inline float CalcDistance(SGRLight *light)
{
  return (light->x*_gr_polygon.a + light->y*_gr_polygon.b +
          light->z*_gr_polygon.c - _gr_polygon.d);
}

//extern float _kX, _kY;
float _kX = 0.01, _kY = 0.01;
float _kXX = 0.0001, _kYY = 0.0001, _kXY = 0.0001;
float _ikX = 100., _ikY = 100.;
extern float _hazeK;

void GRSetScale(float kx, float ky)
{
  _kX   = kx;
  _kY   = ky;
  _kXY  = kx * ky;
  _ikX  = 1.0 / kx;
  _ikY  = 1.0 / ky;
  _kXX = kx * kx;
  _kYY = ky * ky;

}

float _sunX, _sunY, _sunZ;

void GRSetSunDirection(float x, float y, float z)
{
  _sunX = x;
  _sunY = y;
  _sunZ = z;

}


//-----------------------NEW HARDWARE LIGHT------------------------------------

float _pol1[64*5], _pol2[64*5];

void ClipHorizontal(float *out, float *inp, float *in, float y)
{ float i;

  i = (y - inp[1]) / (in[1] - inp[1]);
  COPY_FLOAT(out[1], y);
  out[0] = (in[0] - inp[0]) * i + inp[0];
  out[2] = (in[2] - inp[2]) * i + inp[2];
  out[3] = (in[3] - inp[3]) * i + inp[3];
}

void ClipVertical(float *out, float *inp, float *in, float x)
{ float i;

  i = (x - inp[0]) / (in[0] - inp[0]);
  COPY_FLOAT(out[0], x);
  out[1] = (in[1] - inp[1]) * i + inp[1];
  out[2] = (in[2] - inp[2]) * i + inp[2];
  out[3] = (in[3] - inp[3]) * i + inp[3];
}


float *ClipLightPolygon(float *pol, int vertQ, int mask, float left, float right, float top, float bottom, int *vertC)
{ float *pin = pol, *pout = _pol1, *in, *out;
  int num = 0, curin, i, nextin;

  //x,y,u,v
  //--------------TOP----------------------
  if (mask & 4) {
     in = pin;
     out = pout;
     curin = (in[1] >= top);

     for(i = 0 ;i < vertQ - 1 ;i++,in += 5,curin = nextin) {

        // Keep the current vertex if it's inside the plane
        if (curin) {
           COPY_FLOAT(*out, *in);
           COPY_FLOAT(*(out+1) , *(in+1));
           COPY_FLOAT(*(out+2) , *(in+2));
           COPY_FLOAT(*(out+3) , *(in+3));
           COPY_FLOAT(*(out+4) , *(in+4));
           num++;
           out += 5;
        }

        nextin = (in[5+1] >= top);

        // Add a clipped vertex if one end of the current edge is
        // inside the plane and the other is outside
        if (curin != nextin) {
           ClipHorizontal(out,in,in + 5,top);
           *((long*)(out+4)) = -1;
           num++;
           out += 5;
        }
     }

     if (curin) {
        COPY_FLOAT(*out, *in);
        COPY_FLOAT(*(out+1) , *(in+1));
        COPY_FLOAT(*(out+2) , *(in+2));
        COPY_FLOAT(*(out+3) , *(in+3));
        COPY_FLOAT(*(out+4) , *(in+4));
        num++;
        out += 5;
     }

     if (curin != (pin[1] >= top)) {
        ClipHorizontal(out,in,pin,top);
        *((long*)(out+4)) = -1;
        num++;
        //out += 5;
     }

     if (num < 3) return NULL;

     vertQ = num;

     //in    = pin;
     pin   = pout;
     pout  = _pol2;
  }

  //--------------LEFT----------------------
  if (mask & 1) {
     in = pin;
     out = pout;
     curin = (in[0] >= left);
     num = 0;

     for(i = 0 ;i < vertQ - 1 ;i++,in += 5,curin = nextin) {

        // Keep the current vertex if it's inside the plane
        if (curin) {
           COPY_FLOAT(*out, *in);
           COPY_FLOAT(*(out+1) , *(in+1));
           COPY_FLOAT(*(out+2) , *(in+2));
           COPY_FLOAT(*(out+3) , *(in+3));
           COPY_FLOAT(*(out+4) , *(in+4));
           num++;
           out += 5;
        }

        nextin = (in[5+0] >= left);

        // Add a clipped vertex if one end of the current edge is
        // inside the plane and the other is outside
        if (curin != nextin) {
           ClipVertical(out,in,in + 5,left);
           *((long*)(out+4)) = -1;
           num++;
           out += 5;
        }
     }

     if (curin) {
        COPY_FLOAT(*out, *in);
        COPY_FLOAT(*(out+1) , *(in+1));
        COPY_FLOAT(*(out+2) , *(in+2));
        COPY_FLOAT(*(out+3) , *(in+3));
        COPY_FLOAT(*(out+4) , *(in+4));
        num++;
        out += 5;
     }

     if (curin != (pin[0] >= left)) {
        ClipVertical(out,in,pin,left);
        *((long*)(out+4)) = -1;
        num++;
        //out += 5;
     }

     if (num < 3) return NULL;

     vertQ = num;
     in    = pin;
     pin   = pout;
     if (in == pol)
        pout = _pol2;
     else
        pout = in;
  }

  //--------------BOTTOM----------------------
  if (mask & 8) {
     in = pin;
     out = pout;
     curin = (in[1] <= bottom);
     num = 0;

     for(i = 0 ;i < vertQ - 1 ;i++,in+=5,curin = nextin) {

        // Keep the current vertex if it's inside the plane
        if (curin) {
           COPY_FLOAT(*out, *in);
           COPY_FLOAT(*(out+1) , *(in+1));
           COPY_FLOAT(*(out+2) , *(in+2));
           COPY_FLOAT(*(out+3) , *(in+3));
           COPY_FLOAT(*(out+4) , *(in+4));
           num++;
           out += 5;
        }

        nextin = (in[5+1] <= bottom);

        // Add a clipped vertex if one end of the current edge is
        // inside the plane and the other is outside
        if (curin != nextin) {
           ClipHorizontal(out,in,in + 5,bottom);
           *((long*)(out+4)) = -1;
           num++;
           out += 5;
        }
     }

     if (curin) {
        COPY_FLOAT(*out, *in);
        COPY_FLOAT(*(out+1) , *(in+1));
        COPY_FLOAT(*(out+2) , *(in+2));
        COPY_FLOAT(*(out+3) , *(in+3));
        COPY_FLOAT(*(out+4) , *(in+4));
        num++;
        out += 5;
     }

     if (curin != (pin[1] <= bottom)) {
        ClipHorizontal(out,in,pin,bottom);
        *((long*)(out+4)) = -1;
        num++;
        //out += 5;
     }

     if (num < 3) return NULL;

     vertQ = num;
     in    = pin;
     pin   = pout;
     if (in == pol)
        pout = _pol2;
     else
        pout = in;
  }

  //--------------RIGHT----------------------
  if (mask & 2) {
     in = pin;
     out = pout;
     curin = (in[0] <= right);
     num = 0;

     for(i = 0 ;i < vertQ - 1 ;i++,in+=5,curin = nextin) {

        // Keep the current vertex if it's inside the plane
        if (curin) {
           COPY_FLOAT(*out, *in);
           COPY_FLOAT(*(out+1) , *(in+1));
           COPY_FLOAT(*(out+2) , *(in+2));
           COPY_FLOAT(*(out+3) , *(in+3));
           COPY_FLOAT(*(out+4) , *(in+4));
           num++;
           out += 5;
        }

        nextin = (in[5+0] <= right);

        // Add a clipped vertex if one end of the current edge is
        // inside the plane and the other is outside
        if (curin != nextin) {
           ClipVertical(out,in,in + 5,right);
           *((long*)(out+4)) = -1;
           num++;
           out += 5;
        }
     }

     if (curin) {
        COPY_FLOAT(*out, *in);
        COPY_FLOAT(*(out+1) , *(in+1));
        COPY_FLOAT(*(out+2) , *(in+2));
        COPY_FLOAT(*(out+3) , *(in+3));
        COPY_FLOAT(*(out+4) , *(in+4));
        num++;
        out += 5;
     }

     if (curin != (pin[0] <= right)) {
        ClipVertical(out,in,pin,right);
        *((long*)(out+4)) = -1;
        num++;
        //out += 4;
     }

     if (num < 3) return NULL;
     pin = pout;
  }

  *vertC = num;

  return pin;

}


void D3D_NEW_PreparePolygonForLight(UGRVertex * vertS, int vertQnty, int lightMask)
{ int i, j;
  unsigned long mask;
  float minU, minV, maxU, maxV;
  float x, y, z;
  SMatr2x3 matr;
  UGRVertex *v = vertS;
  SGRLight *l = _gr_pLights;
  float polygonV[64][5]; //x,y,u,v

  CalculateTransform(&matr);

  //-----Calc x,y,z and transform to u,v
  z = -1.0 / v->d3d.oow;//v->d3d.z / _hazeK;
  x = -(v->d3d.x - _gr_nScreenOriginX) * z * _kX;
  y = (v->d3d.y - _gr_nScreenOriginY) * z * _kY;

  minU = maxU = polygonV[0][0] = x*matr.uX + y*matr.uY + z*matr.uZ;
  minV = maxV = polygonV[0][1] = x*matr.vX + y*matr.vY + z*matr.vZ;

  for(i = 1, v++;i < vertQnty; i++,v++) {
     z = -1.0 / v->d3d.oow;//v->d3d.z / _hazeK;
     x = -(v->d3d.x - _gr_nScreenOriginX) * z * _kX;
     y = (v->d3d.y - _gr_nScreenOriginY) * z * _kY;

     polygonV[i][0] = x*matr.uX + y*matr.uY + z*matr.uZ;
     polygonV[i][1] = x*matr.vX + y*matr.vY + z*matr.vZ;

     if (polygonV[i][0] > maxU) maxU = polygonV[i][0];
     else if (polygonV[i][0] < minU) minU = polygonV[i][0];

     if (polygonV[i][1] > maxV) maxV = polygonV[i][1];
     else if (polygonV[i][1] < minV) minV = polygonV[i][1];
  }


  for(mask = 1;mask;i++,l++,mask<<=1) {
     if ((mask & lightMask) == 0) {
        l->draw = 0;
        continue;
     }

     switch (l->type) {
        case GR_LIGHT: {
           float h = CalcDistance(l);
           if (_gr_polygon.dwAddType & GR_POLY_ADD_LIGHTTHROUGH) h = fabs(h);

           if (h < 0 || h > l->r) {
              l->draw = 0;
              //COPY_FLOAT(l->r, l->saveR);
              break;
           }

           //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11
           float lr = fabs(l->z);//l->x*l->x + l->y*l->y + l->z*l->z;
           COPY_FLOAT(l->saveR, l->r); // save original r

           if (lr >= _fogStart) {
              l->r = (l->r*(_fogEnd - lr))/(_fogEnd - _fogStart);
              if (l->r < 0) SET_FLOAT_ZERO(l->r);
           }


           //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11

           float uc, vc, delta, left, right,top,bottom,r2;


           ///////////////////////////////
           r2 = l->r*l->r;
           //---calc center coord and transform
           x = l->x - h*_gr_polygon.a;
           y = l->y - h*_gr_polygon.b;
           z = l->z - h*_gr_polygon.c;

           uc = x*matr.uX + y*matr.uY + z*matr.uZ;
           vc = x*matr.vX + y*matr.vY + z*matr.vZ;

           //-----calc light square coord
           delta = sqrt((r2 - h*h));
           //delta = h*sqrt(l->power0*l->r2/(10*l->r2+l->power0*h*h) - 1);

           left = uc - delta;
           right = uc + delta;
           top = vc - delta;
           bottom = vc + delta;

           if (minU > right || maxU < left || minV > bottom || maxV < top)
              l->draw = 0;
           else {
              //---calc Ul,Vl,.... for polygon corners
              float idel2, *clipP;
              int vertC;

              idel2 = 1.0 / (2.0 * delta);

              for(j = 0;j < vertQnty;j++) {
                 polygonV[j][2] = (polygonV[j][0] - uc) * idel2 + 0.5;
                 polygonV[j][3] = (polygonV[j][1] - vc) * idel2 + 0.5;
                 *((long *)&(polygonV[j][4])) = j;
              }

              int notclipped = 1;
              //simple clipping
              if (minU >= left && maxU <= right && minV >= top && maxV <= bottom) {
                 clipP = (float*)polygonV;
                 vertC = vertQnty;
                 l->draw = 1;
              }
              else {
                 int mask = 0;
                 if (minU < left)   mask  = 1;
                 if (maxU > right)  mask |= 2;
                 if (minV < top)    mask |= 4;
                 if (maxV > bottom) mask |= 8;

                 clipP = ClipLightPolygon((float*)polygonV, vertQnty, mask, left, right, top, bottom, &vertC);
                 if (clipP == NULL) {
                    l->draw = 0;
                    COPY_FLOAT(l->r, l->saveR);
                    break;
                 }

                 notclipped = 0;
                 l->draw = 1;
              }

              //////////////////////////////////////
              l->colorTable = _LightScaleTable[l->color];
              l->handler = (l->color == 0?_LightTextH[LIGHT_SHADOW]:_LightTextH[LIGHT_LIGHT]);
              ////////////////////////////////////
              /*
              int br;
              //br = 300-(h*255)/l->r;
              //if (br >255) br = 255;
              //else if(br<0) br = 0;
              br = l->power0;
              */

              //int color = (br<<16) | (br<<8) | br;

              int color = ((unsigned long*)l->colorTable)[l->power0] | 0xFF000000;

              //convert polygon to x,y,z and screen coord
              for(i = 0, j = 0;j < vertC;j++,i += 5) {
                 if (notclipped) {
                    COPY_FLOAT(l->points[j].d3d.x, vertS[j].d3d.x);
                    COPY_FLOAT(l->points[j].d3d.y, vertS[j].d3d.y);
                    COPY_FLOAT(l->points[j].d3d.oow, vertS[j].d3d.oow);
                    COPY_FLOAT(l->points[j].d3d.z, vertS[j].d3d.z);
                 }
                 else {
                    if ((*((long*)&(clipP[j*5+4]))) >= 0) {
                       int n = *((long*)&(clipP[j*5+4]));

                       COPY_FLOAT(l->points[j].d3d.x, vertS[n].d3d.x);
                       COPY_FLOAT(l->points[j].d3d.y, vertS[n].d3d.y);
                       COPY_FLOAT(l->points[j].d3d.oow, vertS[n].d3d.oow);
                       COPY_FLOAT(l->points[j].d3d.z, vertS[n].d3d.z);
                    }
                    else {
                       x = clipP[i+0]*matr.uX + clipP[i+1]*matr.vX + _gr_polygon.a*_gr_polygon.d;
                       y = clipP[i+0]*matr.uY + clipP[i+1]*matr.vY + _gr_polygon.b*_gr_polygon.d;
                       z = -1.0 / (clipP[i+0]*matr.uZ + clipP[i+1]*matr.vZ + _gr_polygon.c*_gr_polygon.d);

                       l->points[j].d3d.x = (x * z * _ikX) + _gr_nScreenOriginX;
                       l->points[j].d3d.y = -(y * z * _ikY) + _gr_nScreenOriginY;
                       COPY_FLOAT(l->points[j].d3d.oow, z);
                       l->points[j].d3d.z = 1.0 - z * _gr_fFrontClip;//_hazeK / z;
                    }
                 }

                 l->points[j].d3d.d3dcolor.color = color;
                 l->points[j].d3d.specular = 0xFF000000; //for haze

                 if (clipP[i+2] < 0)
                    l->points[j].d3d.u = 0;
                 else {
                    if (clipP[i+2] > 1.0) l->points[j].d3d.u = 1.0;
                       else l->points[j].d3d.u = clipP[i+2];
                 }

                 if (clipP[i+3] < 0)
                    l->points[j].d3d.v = 0;
                 else {
                    if (clipP[i+3] > 1.0) l->points[j].d3d.v = 1.0;
                       else l->points[j].d3d.v = clipP[i+3];
                 }

              }

              l->qnty = vertC;
           }
           COPY_FLOAT(l->r, l->saveR); //restore original r
        }
        break;

        case GR_SHADOW: {
            /*
           SGRShadow *sh = l->shadow;
           float shPoints[64][2];
           float shPolygon[64][2];

           //simple clip
           x = CalcDistance(l);
           if (_gr_polygon.dwAddType & GR_POLY_ADD_LIGHTTHROUGH) x = fabs(x);

           if (x < 0 || x > l->r) {
              l->draw = 0;
              break;
           }


           //calc shadow points projection
           float inv = 1.0 /
                       (_gr_polygon.a * _sunX +
                        _gr_polygon.b * _sunY +
                        _gr_polygon.c * _sunZ
                       );

           for(i = 0;i < sh->numPoints;i++) {
              float t = inv * (_gr_polygon.d -
                               _gr_polygon.a * sh->points[i][0] -
                               _gr_polygon.b * sh->points[i][1] -
                               _gr_polygon.c * sh->points[i][2]
                              );
              x = sh->points[i][0] + _sunX*t;
              y = sh->points[i][1] + _sunY*t;
              z = sh->points[i][2] + _sunZ*t;

              shPoints[i][0] = x*matr.uX + y*matr.uY + z*matr.uZ;
              shPoints[i][1] = x*matr.vX + y*matr.vY + z*matr.vZ;
           }

           //clip all shadow polygons throught POLYGON
           for(i = 0;i < sh->numPolygons;i++) {
              float minX, minY, maxX, maxY;
              COPY_FLOAT(minX, *((float*)shPoints + polygons[i][0]));
              COPY_FLOAT(minY, *((float*)shPoints + polygons[i][0] + 1));
              COPY_FLOAT(maxX, minX);
              COPY_FLOAT(maxY, minY);
              COPY_FLOAT(shPolygon[0][0], minX);
              COPY_FLOAT(shPolygon[0][1], minY);

              for(j = 1;j < sh->polyPntCnt[i];j++) {
                 COPY_FLOAT(x , *((float*)shPoints + polygons[i][j]));
                 COPY_FLOAT(y , *((float*)shPoints + polygons[i][j] + 1));

                 COPY_FLOAT(shPolygon[j][0], x);
                 COPY_FLOAT(shPolygon[j][1], y);

                 if (x > maxX) maxX = x;
                 else if (x < minX) minX = x;

                 if (y > maxY) maxY = y;
                 else if (y < minY) minY = y;
              }

              //simple clipping
              if () {
                 l->draw = 0;
                 //nodraw
              }
              else {
                 // clip shadow polygon

                 // create UGRVertex for shadow polygon
                 l->draw = 1;
              }

           }
           */
        }
        break;
     }
  }


}

//-------------------------------------------------

extern "C" long _minY;
extern "C" float PDC, PDC0, PDC1, PDB, PDB0, PDA;
extern "C" float PUC, PUC0, PUC1, PUB, PUB0, PUA;
extern "C" float L0;

#define N(i) _gr_polygon.i


extern "C" void ASM_PrepareLightSource(SGRLight *l)
{ float h = CalcDistance(l);
  float s;
  float b,d,e,f;

  if (_gr_polygon.dwAddType & GR_POLY_ADD_LIGHTTHROUGH)
     h = fabs(h);
  if (h < 0 || h > l->r) {
     l->colorTable = NULL;
     return;
  }

  s = l->x*l->x+l->y*l->y+l->z*l->z;
  //////////////
  l->colorTable = (unsigned long*)&_LightMixTable[l->color*32*256];
  //////////////

  L0 = -(float)(l->power0>>3)*65536.0;//(h/(l->r*l->r))*65536.0;

  h = L0 / (l->r*l->r) ;//h*h*(l->power0>>3);



  //PDA = (N(a)*N(a)*s + N(d)*(N(d)+2.0*N(a)*l->x))*_kX*_kX;
  //b = (N(b)*N(b)*s + N(d)*(N(d)+2.0*N(b)*l->y))*_kY*_kY;
  //PDB0 = -(2.0*(N(a)*N(b)*s + N(d)*(N(b)*l->x+N(a)*l->y)))*_kX*_kY;
  //d = -(2.0*(N(a)*N(c)*s + N(d)*(N(c)*l->x+N(a)*l->z)))*_kX;
  //e = (2.0*(N(c)*N(b)*s + N(d)*(N(c)*l->y+N(b)*l->z)))*_kY;
  //f = N(c)*N(c)*s + N(d)*(N(d)+2.0*N(c)*l->z);

  PUA = (N(a)*N(a)*s + N(d)*(N(d)-2.0*N(a)*l->x))*_kXX*h;
  b = (N(b)*N(b)*s + N(d)*(N(d)-2.0*N(b)*l->y))*_kYY*h;
  PUB0 = (-2.0*(N(a)*N(b)*s - N(d)*(N(b)*l->x+N(a)*l->y)))*_kXY*h;
  d = (-2.0*(N(a)*N(c)*s - N(d)*(N(c)*l->x+N(a)*l->z)))*_kX*h;
  e = (2.0*(N(c)*N(b)*s - N(d)*(N(c)*l->y+N(b)*l->z)))*_kY*h;
  f = (N(c)*N(c)*s + N(d)*(N(d)-2.0*N(c)*l->z))*h;


  PUC  = b*_minY*_minY + e*_minY + f;
  PUC0 = b*(2.0*_minY+1) + e;
  PUC1 = 2.0*b;
  PUB  = PUB0*_minY+d;
  //pdb0 = c;
  //pda  = a;


  //PUA = (N(a)*N(a)*h*_kX*_kX)*65536.0;
  //b = (N(b)*N(b)*h*_kY*_kY)*65536.0;
  //PUB0 = -(2.0*N(a)*N(b)*h*_kX*_kY)*65536.0;
  //d = -(2.0*N(a)*N(c)*h*_kX)*65536.0;
  //e = (2.0*N(b)*N(c)*h*_kY)*65536.0;
  //f = (N(c)*N(c)*h)*65536.0;


  PDA = (N(a)*N(a)*_kXX);
  b = (N(b)*N(b)*_kYY);
  PDB0 = (-2.0*N(a)*N(b)*_kXY);
  d = (-2.0*N(a)*N(c)*_kX);
  e = (2.0*N(b)*N(c)*_kY);
  f = (N(c)*N(c));


  PDC  = b*_minY*_minY + e*_minY + f;
  PDC0 = b*(2.0*_minY+1) + e;
  PDC1 = 2.0*b;
  PDB  = PDB0*_minY+d;
  //pub0 = c1;
  //pua  = a1;
}



/*
extern "C" void ASM_PrepareLightSource(SGRLight *l)
{ float h = CalcDistance(l);
  float s;
  float b,d,e,f;

  if (_gr_polygon.dwAddType & GR_POLY_ADD_LIGHTTHROUGH)
     h = fabs(h);
  if (h < 0 || h > l->r) {
     l->colorTable = NULL;
     return;
  }

  s = l->x*l->x+l->y*l->y+l->z*l->z;
  //////////////
  l->colorTable = (unsigned long*)&_LightMixTable[l->color*32*256];
  //////////////

  h = h*h*(l->power0>>3)*65536.0;

  L0 = (h/(l->r*l->r));


  //PDA = (N(a)*N(a)*s + N(d)*(N(d)+2.0*N(a)*l->x))*_kX*_kX;
  //b = (N(b)*N(b)*s + N(d)*(N(d)+2.0*N(b)*l->y))*_kY*_kY;
  //PDB0 = -(2.0*(N(a)*N(b)*s + N(d)*(N(b)*l->x+N(a)*l->y)))*_kX*_kY;
  //d = -(2.0*(N(a)*N(c)*s + N(d)*(N(c)*l->x+N(a)*l->z)))*_kX;
  //e = (2.0*(N(c)*N(b)*s + N(d)*(N(c)*l->y+N(b)*l->z)))*_kY;
  //f = N(c)*N(c)*s + N(d)*(N(d)+2.0*N(c)*l->z);

  PDA = (N(a)*N(a)*s + N(d)*(N(d)-2.0*N(a)*l->x))*_kX*_kX;
  b = (N(b)*N(b)*s + N(d)*(N(d)-2.0*N(b)*l->y))*_kY*_kY;
  PDB0 = (-2.0*(N(a)*N(b)*s - N(d)*(N(b)*l->x+N(a)*l->y)))*_kX*_kY;
  d = (-2.0*(N(a)*N(c)*s - N(d)*(N(c)*l->x+N(a)*l->z)))*_kX;
  e = (2.0*(N(c)*N(b)*s - N(d)*(N(c)*l->y+N(b)*l->z)))*_kY;
  f = N(c)*N(c)*s + N(d)*(N(d)-2.0*N(c)*l->z);


  PDC  = b*_minY*_minY + e*_minY + f;
  PDC0 = b*(2.0*_minY+1) + e;
  PDC1 = 2.0*b;
  PDB  = PDB0*_minY+d;
  //pdb0 = c;
  //pda  = a;


  //PUA = (N(a)*N(a)*h*_kX*_kX)*65536.0;
  //b = (N(b)*N(b)*h*_kY*_kY)*65536.0;
  //PUB0 = -(2.0*N(a)*N(b)*h*_kX*_kY)*65536.0;
  //d = -(2.0*N(a)*N(c)*h*_kX)*65536.0;
  //e = (2.0*N(b)*N(c)*h*_kY)*65536.0;
  //f = (N(c)*N(c)*h)*65536.0;


  PUA = (N(a)*N(a)*h*_kX*_kX);
  b = (N(b)*N(b)*h*_kY*_kY);
  PUB0 = (-2.0*N(a)*N(b)*h*_kX*_kY);
  d = (-2.0*N(a)*N(c)*h*_kX);
  e = (2.0*N(b)*N(c)*h*_kY);
  f = (N(c)*N(c)*h);


  PUC  = b*_minY*_minY + e*_minY + f;
  PUC0 = b*(2.0*_minY+1) + e;
  PUC1 = 2.0*b;
  PUB  = PUB0*_minY+d;
  //pub0 = c1;
  //pua  = a1;
}
*/


/*
//-----------------------HARDWARE LIGHT------------------------------------
void D3D_PreparePolygonForLight(UGRVertex * vertS, UGRVertex *vertE, void **lightH, int lightC)
{ int i;
  float minU, minV, maxU, maxV;
  float x, y, z, scale;
  SMatr2x3 matr;
  UGRVertex *v = vertS;
  SGRLight **light = (SGRLight**) lightH;

  //--------------Stretch Polygon--------------------
  CalculateTransform(&matr);

  z = -v->d3d.z / _hazeK;
  x = -(v->d3d.x - _gr_nScreenOriginX) * z * _kX;
  y = (v->d3d.y - _gr_nScreenOriginY) * z * _kY;

  minU = maxU = v->d3d.u = x*matr.uX + y*matr.uY + z*matr.uZ;
  minV = maxV = v->d3d.v = x*matr.vX + y*matr.vY + z*matr.vZ;

  for(v++;v <= vertE; v++) {
     z = -v->d3d.z / _hazeK;
     x = -(v->d3d.x - _gr_nScreenOriginX) * z * _kX;
     y = (v->d3d.y - _gr_nScreenOriginY) * z * _kY;

     v->d3d.u = x*matr.uX + y*matr.uY + z*matr.uZ;
     if (v->d3d.u < minU) minU = v->d3d.u;
     else if (v->d3d.u > maxU) maxU = v->d3d.u;

     v->d3d.v = x*matr.vX + y*matr.vY + z*matr.vZ;
     if (v->d3d.v < minV) minV = v->d3d.v;
     else if (v->d3d.v > maxV) maxV = v->d3d.v;
  }

  if (maxU - minU > maxV - minV)
     scale = 1. / (maxU - minU);
  else
     scale = 1. / (maxV - minV);

  //----recalcilate texture coordinates-------------
  for(v = vertS;v <= vertE; v++) {
     v->d3d.u = (v->d3d.u - minU) * scale;
     v->d3d.v = (v->d3d.v - minV) * scale;
  }

  //------Calculate Center of Light Source------------------------
  float lightScale = scale * _LightSize;
  for(i = 0;i < lightC;i++, light++) {
     float h = CalcDistance(*light);
     float uc, vc, uc0, vc0, r0, r1, r2;
     int clip = 0;

     x = (*light)->x - h*_gr_polygon.a;
     y = (*light)->y - h*_gr_polygon.b;
     z = (*light)->z - h*_gr_polygon.c;

     uc0 = (x*matr.uX + y*matr.uY + z*matr.uZ - minU);
     vc0 = (x*matr.vX + y*matr.vY + z*matr.vZ - minV);

     uc = uc0 * scale;
     vc = vc0 * scale;

     if (uc < 0) clip |= 1;
     else if (uc > 1.) clip |= 4;

     if (vc < 0) clip |= 8;
     else if (vc > 1.) clip |= 2;

     float ttt = (*light)->power*h*h;
     #define KKK 3
     r2 = ttt*(*light)->r2/(KKK*(*light)->r2 + ttt)*scale*scale;

     //clip = 0;

     switch (clip) {
        case 1:
           r0 = uc*uc + vc*vc;
           r1 = uc*uc + (vc - 1.0)*(vc - 1.0);
           if (r0 < r2 || r1 < r2) clip = 0;
           break;
        case 2:
           r0 = uc*uc + (vc - 1.0)*(vc - 1.0);
           r1 = (uc - 1.0)*(uc - 1.0) + (vc - 1.0)*(vc - 1.0);
           if (r0 < r2 || r1 < r2) clip = 0;
           break;
        case 3:
           r1 = uc*uc + (vc - 1.0)*(vc - 1.0);
           if (r1 < r2) clip = 0;
           break;
        case 4:
           r0 = (uc - 1.0)*(uc - 1.0) + vc*vc;
           r1 = (uc - 1.0)*(uc - 1.0) + (vc - 1.0)*(vc - 1.0);
           if (r0 < r2 || r1 < r2) clip = 0;
           break;
        case 6:
           r1 = (uc - 1.0)*(uc - 1.0) + (vc - 1.0)*(vc - 1.0);
           if (r1 < r2) clip = 0;
           break;
        case 8:
           r0 = uc*uc + vc*vc;
           r1 = (uc - 1.0)*(uc - 1.0) + vc*vc;
           if (r0 < r2 || r1 < r2) clip = 0;
           break;
        case 9:
           r0 = uc*uc + vc*vc;
           if (r0 < r2 || r1 < r2) clip = 0;
           break;
        case 12:
           r1 = (uc - 1.0)*(uc - 1.0) + vc*vc;
           if (r1 < r2) clip = 0;
           break;
     }

     if (clip == 0) {
        (*light)->uC = uc0 * lightScale;
        (*light)->vC = vc0 * lightScale;
        (*light)->h2 = h * h * lightScale * lightScale;
        (*light)->irad2 = 1. / (r2 * _LightSize * _LightSize);
     }
     else (*light)->uC = 0x80000000;
  }
}


void CreateLightT(UGRVertex *vertS, UGRVertex *vertE, void *lightH)
{ float x, y, z, h, L0, L1, r, x1,y1,z1;
  UGRVertex *v = vertS;
  SGRLight *light = (SGRLight*) lightH;
  int L, i, u0, v0, fl = 1;

  h = CalcDistance(light);

  //L1 = light->power * h * h;
  //L0 = L1/(light->r2);
  L1 = light->power / 2;
  L0 = light->r2/4;

  x1 = (light)->x - h*_gr_polygon.a;
  y1 = (light)->y - h*_gr_polygon.b;
  z1 = (light)->z - h*_gr_polygon.c;

  u0 = -x1 / (z1 * _kX) + _gr_nScreenOriginX;
  v0 = y1 / (z1 *_kY) + _gr_nScreenOriginY;

  for(i = 0;v <= vertE;v++,i++) {

     z = -v->d3d.z / _hazeK;
     x = -(v->d3d.x - _gr_nScreenOriginX) * z * _kX;
     y = (v->d3d.y - _gr_nScreenOriginY) * z * _kY;

     r = (x-light->x)*(x-light->x) + (y-light->y)*(y-light->y) +
         (z-light->z)*(z-light->z);


     if (r > light->r2) L = 0;
     else {
        if (r > L0) L = L1*(4 - r/L0);
        else L = L1*r/L0;
     }

     //L = L1/r  - L0;
     //if (L < 0) L = 0;
     //else if (L > 255) L = 255;

     v->d3d.d3dcolor.color = light->colorTable[L];

     if (fl) {
        int dx,dy;

        if (v == vertE) {
           dx = (vertS)->d3d.x - v->d3d.x;
           dy = (vertS)->d3d.y - v->d3d.y;
        }
        else {
           dx = (v+1)->d3d.x - v->d3d.x;
           dy = (v+1)->d3d.y - v->d3d.y;
        }

        if (dx == 0) {
           if (u0 > v->d3d.x) fl = 0;
        }
        else {
           if (dy == 0) {
              if (v0 > v->d3d.y) fl = 0;
           }
           else {
              if (v0 > (dy*u0)/dx + (v->d3d.y - (dy*v->d3d.x)/dx))  fl = 0;
           }
        }
     }
  }

  if (fl) {
     v->d3d.x = u0;
     v->d3d.y = v0;
     v->d3d.oow = -1. / z1;
     v->d3d.z = -_hazeK * z1;

     r = (x1-light->x)*(x1-light->x) + (y1-light->y)*(y1-light->y) +
         (z1-light->z)*(z1-light->z);

     //L = L1/r  - L0;
     //if (L < 0) L = 0;
     //else if (L > 255) L = 255;
     if (r > light->r2) L = 0;
     else {
        if (r > L0) L = L1*(4 - r/L0);
        else L = L1*r/L0;
     }


     v->d3d.d3dcolor.color = light->colorTable[L];

     unsigned short ind[3], j;

     ind[0] = (unsigned short)(i+1);
     for(j = 0;j < i-1;j++) {
        ind[1] = j;
        ind[2] = (unsigned short)(j + 1);

        _d3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DVT_TLVERTEX,(LPVOID) vertS,
         i+1, ind, 3, D3DDP_DONOTCLIP);
     }
     ind[1] = j;
     ind[2] = 0;
     _d3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DVT_TLVERTEX,(LPVOID) vertS,
         i+1, ind, 3, D3DDP_DONOTCLIP);

  }
  else {
     _d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                    (LPVOID) vertS, i, D3DDP_DONOTCLIP);
  }

}

void * D3D_CreateLightTexture(void *lightH)
{ float p1, p2, r0, dx, dy;
  float r, dxTmp;
  long  l0, l1, dL, pitch, i, j, k;
  unsigned char *ptr;
  unsigned short *ptrTmp;
  SGRLight *light = (SGRLight *) lightH;
  unsigned long *color = light->colorTable;
  int num = _LightNum;

  if (light->uC == 0x80000000) return NULL;

  _LightNum = (_LightNum+1)&(MAX_LIGHT_PER_POLYGON - 1);

  ptr = (unsigned char *)_LightTmp;
  pitch = 32*2;
  //if (ptr == NULL) return NULL;
  //ptr = _LightData;
  //pitch = _LightSize*2;

  p1 = light->h2 * light->power;
  r0 = light->uC * light->uC + light->h2 + light->vC*light->vC;
  p2 = p1 * light->irad2;
  dx = -2*LIGHT_STEP*light->uC + LIGHT_STEP*LIGHT_STEP;
  dy = -2*light->vC + 1;

  for(i = 0; i < _LightSize;i++, ptr += pitch) {
     r = r0;
     dxTmp = dx;
     ptrTmp = (unsigned short*) ptr;
     l0 = (long)((p1/r - p2)*65536.0);

     for(j = 0;j < _LightSize;j += LIGHT_STEP) {
        r += dxTmp;
        dxTmp += 2*LIGHT_STEP*LIGHT_STEP;
        l1 = (long)((p1/r - p2)*65536.0);

        if (l0 <= 0) {
           if (l1 <= 0) {
              for(k = 0;k < LIGHT_STEP;k++) {
                 *ptrTmp = (unsigned short)*color;
                 ptrTmp++;
              }
           }
           else {
              dL = (l1 - l0)>>LIGHT_SHIFT;

              for(k = 0;k < LIGHT_STEP;k++) {
                 if (l0 <= 0) *ptrTmp = (unsigned short)*color;
                 else *ptrTmp = (unsigned short)color[l0>>16];
                 ptrTmp++;
                 l0 += dL;
              }
           }
        }
        else {
           dL = (l1 - l0)>>LIGHT_SHIFT;

           if (l1 <= 0) {
              for(k = 0;k < LIGHT_STEP;k++) {
                 if (l0 <= 0) *ptrTmp = (unsigned short)*color;
                 else *ptrTmp = (unsigned short)color[l0>>16];
                 ptrTmp++;
                 l0 += dL;
              }
           }
           else {
              for(k = 0;k < LIGHT_STEP;k++) {
                 *ptrTmp = (unsigned short)color[l0>>16];
                 ptrTmp++;
                 l0 += dL;
              }
           }
        }
        l0 = l1;
     }
     r0 += dy;
     dy += 2;
  }

  ptr = D3D_LockTexture(_LightTextH[num], &pitch);
  ptrTmp = _LightTmp;

  for(j = 0;j < 32;j++,ptr += pitch,ptrTmp += 32)
     memcpy(ptr, ptrTmp, 32*2);

  // Unlock the surface
  D3D_UnLockTexture(_LightTextH[num]);
  //if (!GRLoadTextureToDB(_LightTextH, 0, NULL, _LightData)) return NULL;

  return _LightTextH[num];
}
*/

/*
extern "C" long _ZPres;
//-----------------------SOFTWARE LIGHT------------------------------------
extern "C" void ASM_PreparePolygonForLight(UGRVertex * vertS, UGRVertex *vertE, void **lightH, int lightC)
{ int i;
  float minU, minV, maxU, maxV;
  float x, y, z, scale;
  SMatr2x3 matr;
  UGRVertex *v = vertS;
  SGRLight **light = (SGRLight**) &lightH;

  //--------------Stretch Polygon--------------------
  CalculateTransform(&matr);

  z = -(float)_ZPres / (float) v->any.iz;
  x = -v->any.x * z * _kX;
  y =  v->any.y * z * _kY;

  minU = maxU = v->d3d.u = x*matr.uX + y*matr.uY + z*matr.uZ;
  minV = maxV = v->d3d.v = x*matr.vX + y*matr.vY + z*matr.vZ;

  for(v++; v <= vertE; v++) {
     z = -(float)_ZPres / (float) v->any.iz;
     x = -v->any.x * z * _kX;
     y =  v->any.y * z * _kY;

     v->d3d.u = x*matr.uX + y*matr.uY + z*matr.uZ;
     if (v->d3d.u < minU) minU = v->d3d.u;
     else if (v->d3d.u > maxU) maxU = v->d3d.u;

     v->d3d.v = x*matr.vX + y*matr.vY + z*matr.vZ;
     if (v->d3d.v < minV) minV = v->d3d.v;
     else if (v->d3d.v > maxV) maxV = v->d3d.v;
  }

  if (maxU - minU > maxV - minV)
     scale = _LightSizeSW / (maxU - minU);
  else
     scale = _LightSizeSW / (maxV - minV);

  //----recalcilate texture coordinates-------------
  for(v = vertS;v <= vertE; v++) {
     v->texture.u = ((v->d3d.u - minU) * scale + 1) * v->any.iz;
     v->texture.v = ((v->d3d.v - minV) * scale + 1) * v->any.iz;
  }

  //------Calculate Center of Light Source------------------------
  float lightScale = scale;// / 65536.0;
  for(i = 0;i < lightC;i++, light++) {
     float h = CalcDistance(*light);

     x = (*light)->x - h*_gr_polygon.a;
     y = (*light)->y - h*_gr_polygon.b;
     z = (*light)->z - h*_gr_polygon.c;

     (*light)->uC = (x*matr.uX + y*matr.uY + z*matr.uZ - minU) * lightScale + 1;
     (*light)->vC = (x*matr.vX + y*matr.vY + z*matr.vZ - minV) * lightScale + 1;
     (*light)->h2 = h * h * lightScale * lightScale;
     (*light)->irad2 = 1. / ((*light)->r2 * lightScale * lightScale);
  }
}


extern "C" void ASM_CreateLightTexture(void *lightH)
{ float p1, p2, r0, dx, dy;
  float r, dxTmp;
  long  l0, l1, dL, pitch, i, j, k;
  unsigned char *ptr;
  unsigned char *ptrTmp;
  SGRLight *light = (SGRLight *) lightH;

  //ptr = D3D_LockTexture(_LightTextH, &pitch);
  ptr = _LightDataSW;
  pitch = _LightSize;

  p1 = light->h2 * light->power;
  r0 = light->uC * light->uC + light->h2 + light->vC*light->vC;
  p2 = p1 * light->irad2;
  dx = -2*LIGHT_STEP*light->uC + LIGHT_STEP*LIGHT_STEP;
  dy = -2*light->vC + 1;

  for(i = 0; i < _LightSize;i++, ptr += pitch) {
     r = r0;
     dxTmp = dx;
     ptrTmp = ptr;
     l0 = (long)((p1/r - p2)*65536.0);

     for(j = 0;j < _LightSize;j += LIGHT_STEP) {
        r += dxTmp;
        dxTmp += 2*LIGHT_STEP*LIGHT_STEP;
        l1 = (long)((p1/r - p2)*65536.0);

        if (l0 <= 0) {
           if (l1 <= 0) {
              for(k = 0;k < LIGHT_STEP;k++) {
                 *ptrTmp = 0;
                 ptrTmp++;
              }
           }
           else {
              dL = (l1 - l0)>>LIGHT_SHIFT;

              for(k = 0;k < LIGHT_STEP;k++) {
                 if (l0 <= 0) *ptrTmp = 0;
                 else *ptrTmp = (unsigned char)(l0>>16);
                 ptrTmp++;
                 l0 += dL;
              }
           }
        }
        else {
           dL = (l1 - l0)>>LIGHT_SHIFT;

           if (l1 <= 0) {
              for(k = 0;k < LIGHT_STEP;k++) {
                 if (l0 <= 0) *ptrTmp = 0;
                 else *ptrTmp = (unsigned char)(l0>>16);
                 ptrTmp++;
                 l0 += dL;
              }
           }
           else {
              for(k = 0;k < LIGHT_STEP;k++) {
                 *ptrTmp = (unsigned char)(l0>>16);
                 ptrTmp++;
                 l0 += dL;
              }
           }
        }
        l0 = l1;
     }
     r0 += dy;
     dy += 2;
  }
  // Unlock the surface
  //D3D_UnLockTexture(_LightTextH);
  //GRLoadTextureToDB(_LightTextH, 0, NULL, _LightData);

}
*/