#include <windows.h>
#include <math.h>

#define _LIGHT_CPP_
#include "graph.h"
#include "d3d.h"
//#include "light.h"

extern SDeviceList _dL;
extern IDirect3DDevice2       *_d3dDevice;

extern "C" SGRPolygon _gr_polygon;
extern void D3D_UnLockTexture(void *handle);
extern unsigned char * D3D_LockTexture(void *handle, long *pitch);
/*
extern "C" long _gr_nScreenOriginX;
extern "C" long _gr_nScreenOriginY;
 */


extern float _fogStart, _fogEnd;

UGRVertex _vert[128];


int _LightNum = 0;

typedef struct {
   float uX, uY, uZ;
   float vX, vY, vZ;
}SMatr2x3;

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
        matr->uX = _gr_polygon.c;
        matr->uY = 0;
        matr->uZ = -_gr_polygon.a;
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
        matr->uX = -_gr_polygon.b;
        matr->uY = _gr_polygon.a;
        matr->uZ = 0;
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
extern float _kX = 0.01, _kY = 0.01;
extern float _hazeK;

//-----------------------NEW HARDWARE LIGHT------------------------------------

float _pol1[64*5], _pol2[64*5];

void ClipHorizontal(float *out, float *inp, float *in, float y)
{ float i;

  i = (y - inp[1]) / (in[1] - inp[1]);
  out[1] = y;
  out[0] = (in[0] - inp[0]) * i + inp[0];
  out[2] = (in[2] - inp[2]) * i + inp[2];
  out[3] = (in[3] - inp[3]) * i + inp[3];
}

void ClipVertical(float *out, float *inp, float *in, float x)
{ float i;

  i = (x - inp[0]) / (in[0] - inp[0]);
  out[0] = x;
  out[1] = (in[1] - inp[1]) * i + inp[1];
  out[2] = (in[2] - inp[2]) * i + inp[2];
  out[3] = (in[3] - inp[3]) * i + inp[3];
}


float *ClipLightPolygon(float *pol, int vertQ, float left, float right, float top, float bottom, int *vertC)
{ float *pin = pol, *pout = _pol1, *in, *out;
  int num = 0, curin, i, nextin;

  //x,y,u,v
  //--------------TOP----------------------
  in = pin;
  out = pout;
  curin = (in[1] >= top);

  for(i = 0 ;i < vertQ - 1 ;i++,in += 5,curin = nextin) {

     // Keep the current vertex if it's inside the plane
     if (curin) {
        *out = *in;
        *(out+1) = *(in+1);
        *(out+2) = *(in+2);
        *(out+3) = *(in+3);
        *((long*)(out+4)) = *((long*)(in+4));
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
     *out++ = *in;
     *out++ = *(in+1);
     *out++ = *(in+2);
     *out++ = *(in+3);
     *((long*)out++) = *((long*)(in+4));
     num++;
  }

  if (curin != (pin[1] >= top)) {
     ClipHorizontal(out,in,pin,top);
     *((long*)(out+4)) = -1;
     num++;
     //out += 5;
  }

  if (num < 3) return NULL;

  vertQ = num;
  pin   = pout;
  pout  = _pol2;


  //--------------LEFT----------------------
  in = pin;
  out = pout;
  curin = (in[0] >= left);
  num = 0;

  for(i = 0 ;i < vertQ - 1 ;i++,in += 5,curin = nextin) {

     // Keep the current vertex if it's inside the plane
     if (curin) {
        *out++ = *in;
        *out++ = *(in+1);
        *out++ = *(in+2);
        *out++ = *(in+3);
        *((long*)out++) = *((long*)(in+4));
        num++;
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
     *out++ = *in;
     *out++ = *(in+1);
     *out++ = *(in+2);
     *out++ = *(in+3);
     *((long*)out++) = *((long*)(in+4));
     num++;
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
  pout  = in;

  //--------------BOTTOM----------------------
  in = pin;
  out = pout;
  curin = (in[1] <= bottom);
  num = 0;

  for(i = 0 ;i < vertQ - 1 ;i++,in+=5,curin = nextin) {

     // Keep the current vertex if it's inside the plane
     if (curin) {
        *out++ = *in;
        *out++ = *(in+1);
        *out++ = *(in+2);
        *out++ = *(in+3);
        *((long*)out++) = *((long*)(in+4));
        num++;
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
     *out++ = *in;
     *out++ = *(in+1);
     *out++ = *(in+2);
     *out++ = *(in+3);
     *((long*)out++) = *((long*)(in+4));
     num++;
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
  pout  = in;

  //--------------RIGHT----------------------
  in = pin;
  out = pout;
  curin = (in[0] <= right);
  num = 0;

  for(i = 0 ;i < vertQ - 1 ;i++,in+=5,curin = nextin) {

     // Keep the current vertex if it's inside the plane
     if (curin) {
        *out++ = *in;
        *out++ = *(in+1);
        *out++ = *(in+2);
        *out++ = *(in+3);
        *((long*)out++) = *((long*)(in+4));
        num++;
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
     *out++ = *in;
     *out++ = *(in+1);
     *out++ = *(in+2);
     *out++ = *(in+3);
     *((long*)out++) = *((long*)(in+4));
     num++;
  }

  if (curin != (pin[0] <= right)) {
     ClipVertical(out,in,pin,right);
     *((long*)(out+4)) = -1;
     num++;
     //out += 4;
  }

  if (num < 3) return NULL;

  *vertC = num;

  return pout;


}


void D3D_NEW_PreparePolygonForShadow(UGRVertex * vertS, int vertQnty, int lightMask)
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
  z = -v->d3d.z / _hazeK;
  x = -(v->d3d.x - _gr_nScreenOriginX) * z * _kX;
  y = (v->d3d.y - _gr_nScreenOriginY) * z * _kY;

  minU = maxU = polygonV[0][0] = x*matr.uX + y*matr.uY + z*matr.uZ;
  minV = maxV = polygonV[0][1] = x*matr.vX + y*matr.vY + z*matr.vZ;

  for(i = 1, v++;i < vertQnty; i++,v++) {
     z = -v->d3d.z / _hazeK;
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

     //SGRLight *l = light;
     float h = CalcDistance(l);
	 if (_gr_polygon.dwAddType & GR_POLY_ADD_LIGHTTHROUGH) h = fabs(h);

     //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11
     float lr = fabs(l->z);//l->x*l->x + l->y*l->y + l->z*l->z;
     l->h2 = l->r; // save original r
     if (lr >= _fogStart) {
        l->r = (l->r*(_fogEnd - lr))/(_fogEnd - _fogStart);
        if (l->r < 0) l->r = 0;
     }

     if (h < 0 || h > l->r) {
        l->draw = 0;
		l->r = l->h2;
        continue;
     }
	 //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11

     float uc, vc, delta, left, right,top,bottom;


     ///////////////////////////////
     l->r2 = l->r*l->r;
     //l->power = l->power0;
     ///////////////////////////////
     //---calc center coord and transform
     x = l->x - h*_gr_polygon.a;
     y = l->y - h*_gr_polygon.b;
     z = l->z - h*_gr_polygon.c;

     uc = x*matr.uX + y*matr.uY + z*matr.uZ;
     vc = x*matr.vX + y*matr.vY + z*matr.vZ;

     //-----calc light square coord
     delta = sqrt((l->r2 - h*h));
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
           clipP = ClipLightPolygon((float*)polygonV, vertQnty, left, right, top, bottom, &vertC);
           if (clipP == NULL) {
              l->draw = 0;
              l->r = l->h2;
              continue;
           }
           notclipped = 0;
           l->draw = 1;
        }

        //////////////////////////////////////
        l->colorTable = _LightScaleTable[l->color];
        l->handler = (l->color == 0?_LightTextH[LIGHT_SHADOW]:_LightTextH[LIGHT_LIGHT]);
        ////////////////////////////////////

        int br;
        //br = 300-(h*255)/l->r;
        //if (br >255) br = 255;
        //else if(br<0) br = 0;
        br = l->power0;

        //int color = (br<<16) | (br<<8) | br;
        int color = ((unsigned long*)l->colorTable)[br] | 0xFF000000;

        //convert polygon to x,y,z and screen coord
        for(j = 0;j < vertC;j++) {
           if (notclipped) {
              l->points[j].d3d.x = vertS[j].d3d.x;
              l->points[j].d3d.y = vertS[j].d3d.y;
              l->points[j].d3d.oow = vertS[j].d3d.oow;
              l->points[j].d3d.z = vertS[j].d3d.z;
           }
           else {
              if ((*((long*)&(clipP[j*5+4]))) >= 0) {
                 int n = *((long*)&(clipP[j*5+4]));

                 l->points[j].d3d.x = vertS[n].d3d.x;
                 l->points[j].d3d.y = vertS[n].d3d.y;
                 l->points[j].d3d.oow = vertS[n].d3d.oow;
                 l->points[j].d3d.z = vertS[n].d3d.z;
              }
              else {
                 x = clipP[j*5+0]*matr.uX + clipP[j*5+1]*matr.vX + _gr_polygon.a*_gr_polygon.d;
                 y = clipP[j*5+0]*matr.uY + clipP[j*5+1]*matr.vY + _gr_polygon.b*_gr_polygon.d;
                 z = -1.0 / (clipP[j*5+0]*matr.uZ + clipP[j*5+1]*matr.vZ + _gr_polygon.c*_gr_polygon.d);

                 l->points[j].d3d.x = (x * z / _kX) + _gr_nScreenOriginX;
                 l->points[j].d3d.y = -(y * z / _kY) + _gr_nScreenOriginY;
                 l->points[j].d3d.oow = z;
                 l->points[j].d3d.z = _hazeK / z;
              }
           }
           l->points[j].d3d.d3dcolor.color = color;
           l->points[j].d3d.specular = 0xFF000000; //for haze

           if (clipP[j*5+2] < 0) l->points[j].d3d.u = 0;
           else {
              if (clipP[j*5+2] > 1.0) l->points[j].d3d.u = 1.0;
              else l->points[j].d3d.u = clipP[j*5+2];
           }

           if (clipP[j*5+3] < 0) l->points[j].d3d.v = 0;
           else {
              if (clipP[j*5+3] > 1.0) l->points[j].d3d.v = 1.0;
              else l->points[j].d3d.v = clipP[j*5+3];
           }

        }

        //correct screen coord
        //if (!notclipped) {
       //
       // }

        l->qnty = vertC;
        //l->h2 = h*h;
     }
     l->r = l->h2; //restore original r

        //(*light)->uC = uc0 * lightScale;
        //(*light)->vC = vc0 * lightScale;
     //(*light)->
     //(*light)->h2 = h * h;
     //(*light)->irad2 = 1.0 / r2;
  }

}

