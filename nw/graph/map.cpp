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


#define I65536 (0.00001525878906)


#define COPY_FLOAT(a,b) *((int*)&(a)) = *((int*)&(b))
#define SET_FLOAT_ZERO(a) *((int*)&(a)) = 0
#define FLOAT_NEG(a,b) *((int*)&(a)) = (*((int*)&(b)) ^ 0x80000000)
#define FLOAT_CHANGE_SIGN(f) (*((int*)&(f)) ^= 0x80000000)


#define ARROW_MAX_SECTORS 128


#define POINTER_R0 0.15
#define POINTER_R1 0.6


extern "C" void GRLUDrawArrow(int *points, int numPoint, float r0, float r1,
                              unsigned long color)
{ int i,j;
  float sumLen = 0, len = 0;
  float arrowT[ARROW_MAX_SECTORS][4];  //dx,dy,len,1/len
  float w,dx,dy;
  float xl0,yl0,xr0,yr0;
  float xl1,yl1,xr1,yr1;
  float xl2,yl2,xr2,yr2;
  float xl3,yl3,xr3,yr3;

  GRSetZPrecision(0);
  //GRZBufferEnable(0);

  _gr_polygon.dwFullType = GR_POLY_FLAT;
  _gr_polygon.dwAddType = 0;
  _gr_polygon.nVertices = 4;
  _gr_polygon.dwColor.color = color;
  _gr_polygon.nLights = 0;

  for(i = 0,j = 0;i < (numPoint - 1)*2;i += 2,j++) {
     arrowT[j][0] = points[i+2]-points[i+0];
     arrowT[j][1] = points[i+3]-points[i+1];
     arrowT[j][2] = sqrt(arrowT[j][0]*arrowT[j][0] + arrowT[j][1]*arrowT[j][1]);

     points[i+0] -= _gr_nScreenOriginX;
     points[i+1] -= _gr_nScreenOriginY;

     arrowT[j][3] = 1.0 / arrowT[j][2];

     sumLen += arrowT[j][2];
  }

  points[i+0] -= _gr_nScreenOriginX;
  points[i+1] -= _gr_nScreenOriginY;

  sumLen = (r1-r0) / sumLen;

  //start
  w   = (sumLen*len + r0) * arrowT[0][3];
  len += arrowT[0][2];
  dx  = arrowT[0][1] * w;
  dy  = arrowT[0][0] * w;
  xl0 = points[0] + dx;
  yl0 = points[1] - dy;
  xr0 = points[0] - dx;
  yr0 = points[1] + dy;
  //end
  w   = (sumLen*len + r0) * arrowT[0][3];
  dx  = arrowT[0][1] * w;
  dy  = arrowT[0][0] * w;
  xl1 = points[2] + dx;
  yl1 = points[3] - dy;
  xr1 = points[2] - dx;
  yr1 = points[3] + dy;

  //draw Arrow
  for(i = 1,j = 2;i < numPoint - 1;i++,j += 2) {
     w = (sumLen*len + r0) * arrowT[i][3];
     len += arrowT[i][2];
     dx  = arrowT[i][1] * w;
     dy  = arrowT[i][0] * w;
     xl2 = points[j+0] + dx;
     yl2 = points[j+1] - dy;
     xr2 = points[j+0] - dx;
     yr2 = points[j+1] + dy;

     w = (sumLen*len + r0) * arrowT[i][3];
     dx  = arrowT[i][1] * w;
     dy  = arrowT[i][0] * w;
     xl3 = points[j+2] + dx;
     yl3 = points[j+3] - dy;
     xr3 = points[j+2] - dx;
     yr3 = points[j+3] + dy;

     _gr_vertices[0].any.x = (int)xl0;
     _gr_vertices[0].any.y = (int)yl0;
     _gr_vertices[0].any.iz = 6550;

     _gr_vertices[1].any.x = (int)xr0;
     _gr_vertices[1].any.y = (int)yr0;
     _gr_vertices[1].any.iz = 6550;

     //solve left
     //x(yl1-yl0)+y(xl0-xl1)=xl0yl1-xl1yl0
     //x(yl3-yl2)+y(xl2-xl3)=xl2yl3-xl3yl2
     w = (yl1-yl0)*(xl2-xl3) - (yl3-yl2)*(xl0-xl1);
     if (w == 0.) {
        xl0 = xl1;
        yl0 = yl1;
     }
     else {
        w = 1. / w;
        dx = (xl0*yl1-xl1*yl0)*(xl2-xl3) - (xl2*yl3-xl3*yl2)*(xl0-xl1);
        dy = (yl1-yl0)*(xl2*yl3-xl3*yl2) - (yl3-yl2)*(xl0*yl1-xl1*yl0);

        xl0 = dx*w;
        yl0 = dy*w;
     }

     //solve right
     //x(yr1-yr0)+y(xr0-xr1)=xr0yr1-xr1yr0
     //x(yr3-yr2)+y(xr2-xr3)=xr2yr3-xr3yr2
     w = (yr1-yr0)*(xr2-xr3) - (yr3-yr2)*(xr0-xr1);
     if (w == 0.) {
        xr0 = xr1;
        yr0 = yr1;
     }
     else {
        w = 1. / w;
        dx = (xr0*yr1-xr1*yr0)*(xr2-xr3) - (xr2*yr3-xr3*yr2)*(xr0-xr1);
        dy = (yr1-yr0)*(xr2*yr3-xr3*yr2) - (yr3-yr2)*(xr0*yr1-xr1*yr0);

        xr0 = dx*w;
        yr0 = dy*w;
     }

     _gr_vertices[2].any.x = (int)xr0;
     _gr_vertices[2].any.y = (int)yr0;
     _gr_vertices[2].any.iz = 6550;

     _gr_vertices[3].any.x = (int)xl0;
     _gr_vertices[3].any.y = (int)yl0;
     _gr_vertices[3].any.iz = 6550;

     GRDrawPolygonPCCW();

     xl1 = xl3;
     yl1 = yl3;
     xr1 = xr3;
     yr1 = yr3;
  }

  _gr_vertices[0].any.x = (int)xl0;
  _gr_vertices[0].any.y = (int)yl0;
  _gr_vertices[0].any.iz = 6550;

  _gr_vertices[1].any.x = (int)xr0;
  _gr_vertices[1].any.y = (int)yr0;
  _gr_vertices[1].any.iz = 6550;

  _gr_vertices[2].any.x = (int)xr1;
  _gr_vertices[2].any.y = (int)yr1;
  _gr_vertices[2].any.iz = 6550;

  _gr_vertices[3].any.x = (int)xl1;
  _gr_vertices[3].any.y = (int)yl1;
  _gr_vertices[3].any.iz = 6550;

  GRDrawPolygonPCCW();

  //draw pointer
  i--;
  xl0 = points[j+0] - arrowT[i][0]*POINTER_R0;
  yl0 = points[j+1] - arrowT[i][1]*POINTER_R0;

  _gr_polygon.nVertices = 3;

  _gr_vertices[0].any.x = (int)points[j+0];
  _gr_vertices[0].any.y = (int)points[j+1];
  _gr_vertices[0].any.iz = 6550;

  dx = arrowT[i][0]*POINTER_R0*POINTER_R1;
  dy = arrowT[i][1]*POINTER_R0*POINTER_R1;

  _gr_vertices[1].any.x = (int)(xl0 + dy);
  _gr_vertices[1].any.y = (int)(yl0 - dx);
  _gr_vertices[1].any.iz = 6550;

  _gr_vertices[2].any.x = (int)(xl0 - dy);
  _gr_vertices[2].any.y = (int)(yl0 + dx);
  _gr_vertices[2].any.iz = 6550;

  GRDrawPolygonPCCW();

  //GRZBufferEnable(1);
}