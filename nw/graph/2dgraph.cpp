#include "windows.h"
#include "graph.h"

extern IDirectDrawSurface     *_BackBuffer;

extern SDeviceList _dL;
extern int _StartSceneHW;


void D3DSetError(char * title, int error);

DDSURFACEDESC _ddsd;
int           _2dEnable = 0;
int           _startScene;




int   GREnable2D()
{
   if (_dL.currDevice == NULL) return 0;
   if (_2dEnable) return 1;

   if (_StartSceneHW) {
      _startScene = 1;
      GREndScene();         //save
   }
   else _startScene = 0;

   if (_dL.currDevice->swHw == GR_HARDWARE) {
      _ddsd.dwSize = sizeof(DDSURFACEDESC);

     int err = _BackBuffer->Lock(NULL,&_ddsd,DDLOCK_WRITEONLY | DDLOCK_WAIT,NULL);
     if (err != DD_OK) {
        D3DSetError("Can't Lock BackBuffer", err);
        return 0;
     }
   }

   _2dEnable = 1;
   return 1;
}

int   GRDisable2D()
{
   if (_dL.currDevice == NULL) return 0;
   if (!_2dEnable) return 1;

   if (_dL.currDevice->swHw == GR_HARDWARE) {
      int err = _BackBuffer->Unlock(NULL);
      if (err != DD_OK) {
         D3DSetError("Can't UnLock BackBuffer", err);
         return 0;
      }
   }

   _2dEnable = 0;

   if (_startScene) GRStartScene(); // restore

   return 1;
}

void   Point(int x0, int y0, unsigned long color)
{
   if (x0 < 0 || y0 < 0 || x0 >= _gr_nScreenWidth || y0 >= _gr_nScreenHeight)
      return;

   if (_dL.currDevice->swHw == GR_HARDWARE) {
      *((unsigned short*)((unsigned char*)_ddsd.lpSurface + y0*_ddsd.lPitch +
                         x0*2)) = (unsigned short) color;
   }
   else {
      *(_gr_pScreen + y0*_gr_nScreenWidth + x0) = (unsigned char) color;
   }
}

void   HLine(int x0, int y0, int x1, unsigned long color)
{
   if (x0 >= _gr_nScreenWidth || x1 < 0 || y0 < 0 || y0 >= _gr_nScreenHeight)
      return;

   if (x0 < 0) x0 = 0;
   if (x1 >= _gr_nScreenWidth) x1 = _gr_nScreenWidth - 1;

   if (_dL.currDevice->swHw == GR_HARDWARE) {
      unsigned short *ptr = (unsigned short*)(
                    (unsigned char*)_ddsd.lpSurface + y0*_ddsd.lPitch + x0*2);

      for (;x0 <= x1;x0++,ptr++)
          *ptr = (unsigned short) color;
   }
   else {
      unsigned char *ptr = _gr_pScreen + y0*_gr_nScreenWidth + x0;

      for (;x0 <= x1;x0++,ptr++)
          *ptr = (unsigned char) color;
   }

}

void   VLine(int x0, int y0, int y1, unsigned long color)
{
   if (x0 >= _gr_nScreenWidth || x0 < 0 || y1 < 0 || y0 >= _gr_nScreenHeight)
      return;

   if (y0 < 0) y0 = 0;
   if (y1 >= _gr_nScreenHeight) y1 = _gr_nScreenHeight - 1;

   if (_dL.currDevice->swHw == GR_HARDWARE) {
      unsigned char *ptr =
                    (unsigned char*)_ddsd.lpSurface + y0*_ddsd.lPitch + x0*2;

      for (;y0 <= y1;y0++,ptr += _ddsd.lPitch)
          *((unsigned short*)ptr) = (unsigned short) color;
   }
   else {
      unsigned char *ptr = _gr_pScreen + y0*_gr_nScreenWidth + x0;

      for (;y0 <= y1;y0++,ptr += _gr_nScreenWidth)
          *ptr = (unsigned char) color;
   }

}

void   GRPset(int x0, int y0, unsigned long color)
{
   if (!_2dEnable) return;
   Point(x0, y0, color);
}

void   GRLine(int x0, int y0, int x1, int y1, unsigned long color)
{  int dx = 1, dy = 1, di, si, dxi, dyi, sc, dc, bx;

   if (!_2dEnable) return;

   di = y1 - y0;
   if (di < 0) {
      di = -di;
      dy = -1;
   }
   dyi = dy;

   si = x1 - x0;
   if (si < 0) {
      si = -si;
      dx = -1;
   }
   dxi=dx;

   if (si < di) {
      sc = si;
      dx = 0;
      si = di;
      di = sc;
   }
   else
      dy = 0;

   sc = di<<1;
   bx = sc - si;
   dc = bx - si;

   for(;si >= 0;si--) {
      Point(x0,y0,color);
      if (bx < 0) {
         x0 += dx;
         y0 += dy;
         bx += sc;
      }
      else {
         x0 += dxi;
         y0 += dyi;
         bx += dc;
      }
   }
}

void   GRRect(int x0, int y0, int x1, int y1, unsigned long color)
{
   if (!_2dEnable) return;

   HLine(x0,y0,x1,color);
   HLine(x0,y1,x1,color);
   VLine(x0,y0,y1,color);
   VLine(x1,y0,y1,color);
}

void   GRBar(int x0, int y0, int x1, int y1, unsigned long color)
{
   if (!_2dEnable) return;

   for(;y0 <= y1;y0++)
      HLine(x0,y0,x1,color);
}

void   GRCircle(int x0, int y0, int r, unsigned long color, int fill)
{  long xx = 0, yy = r, d = ((1 - r)<<1);

   if (!_2dEnable) return;

   while (yy >= 0) {
      if (fill) {
         HLine(x0 - xx, y0 - yy, x0 + xx, color);
         HLine(x0 - xx, y0 + yy, x0 + xx, color);
      }
      else {
         Point(x0 + xx, y0 - yy, color);
         Point(x0 + xx, y0 + yy, color);
         Point(x0 - xx, y0 + yy, color);
         Point(x0 - xx, y0 - yy, color);
      }

      if (d < 0) {
         xx++;
         if ((d<<1) + (yy<<1) - 1 <= 0)
            d += (xx<<1) + 1;
         else {
            yy--;
            d += (xx<<1) - (yy<<1) + 2;
         }
      }
      else {
         if (d > 0) {
            yy--;
            if ((d<<1) - (xx<<1) - 1 > 0)
               d += 1 - (yy<<1);
            else {
               xx++;
               d += (xx<<1) - (yy<<1) + 2;
            }
         }
         else {
            xx++;
            yy--;
            d += (xx<<1) - (yy<<1) + 2;
         }
      }
   }
}