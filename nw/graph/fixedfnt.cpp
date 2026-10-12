#include "windows.h"
#include "graph.h"
#include "sd1_epal.h"
#include "debugext.h"

extern IDirectDrawSurface     *_BackBuffer;

extern int _rScale, _rShift;
extern int _gScale, _gShift;
extern int _bScale, _bShift;

extern TExtendedPalette  _EPal;

extern SDeviceList _dL;

void D3DSetError(char * title, int error);
extern int BitCount(unsigned int dw);

extern "C" extern void ASM_PrintAt(const char *, long, long, long, long, long*, unsigned char*);
extern "C" extern void ASM_PrintColorAt(const char *, long, long, long, long, long*, unsigned char*,long);
extern "C" extern void ASM_PrintClipColorAt(const char *, long, long, long, long, long*, unsigned char*,long);
extern "C" extern void ASM_PrintClipAt(const char *, long, long, long, long, long*, unsigned char*);

/***********************************************************************
 ********************           CFixedFont          ********************
 ***********************************************************************/


int CFixedColorFont::Read(const char * filename)
{
  FILE *f;

  pOriginalFntSpr = NULL;
  pSwFntSpr = NULL;
  pHwFntSpr = NULL;

  f = fopen(filename, "rb");
  RTCHECK1(f,"File not found %s",filename);
  //if (f == NULL) return 0;
  //if ((f = CFileResource::FOpenCurrent(filename))==NULL) {
  //  RTCHECK(f,filename); return;
  //}

  fread(&fontHeader,sizeof(fontHeader),1,f);

  if (strnicmp(fontHeader.id,FONT_ID,sizeof(FONT_ID) - 1) != 0) {
      //RTCHECK(0,filename);
	  fclose(f);
      RTCHECK1(0,"Font file %s bad",filename);
      return 0;
  }

  //if (_dL.currDevice == NULL) {
 ///    fclose(f);
  //   return;
  //}

  pOriginalFntSpr = new unsigned char [fontHeader.nFntSprSize];
  fread(pOriginalFntSpr,fontHeader.nFntSprSize,1,f);
  fclose(f);

  RecreateFont();

  //if (! RecreateFont()) {
 //    delete [] pOriginalFntSpr;
 //    pOriginalFntSpr = NULL;
 // }
  return 1;

}


int CFixedColorFont::ReadFromMemory(void *buf)
{
  pOriginalFntSpr = NULL;
  pSwFntSpr = NULL;
  pHwFntSpr = NULL;

  RTCHECK(buf,"NULL pointer");

  //fread(&fontHeader,sizeof(fontHeader),1,f);
  memcpy(&fontHeader, buf, sizeof(fontHeader));

  if (strnicmp(fontHeader.id,FONT_ID,sizeof(FONT_ID) - 1) != 0) {
      //RTCHECK(0,filename);
     //fclose(f);
      RTCHECK(0,"Memory Font bad");
      return 0;
  }


  pOriginalFntSpr = new unsigned char [fontHeader.nFntSprSize];
  //fread(pOriginalFntSpr,fontHeader.nFntSprSize,1,f);
  memcpy(pOriginalFntSpr,((char*)buf) + sizeof(fontHeader),
         fontHeader.nFntSprSize);
  //fclose(f);

  RecreateFont();

  //if (! RecreateFont()) {
 //    delete [] pOriginalFntSpr;
 //    pOriginalFntSpr = NULL;
 // }
  return 1;

}


int CFixedColorFont::RecreateFont()
{ int i;

  if (_dL.currDevice == NULL || pOriginalFntSpr == NULL) return 0;

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     if (pSwFntSpr) {
        delete [] pSwFntSpr;
        pSwFntSpr = NULL;
     }

     if (pHwFntSpr == NULL)
        pHwFntSpr = new unsigned short [fontHeader.nFntSprSize];

     /*
     DDSURFACEDESC ddsd;
     //memset(&ddsd, 0, sizeof(DDSURFACEDESC));
     ddsd.dwSize = sizeof(DDSURFACEDESC);

     int err = _BackBuffer->Lock(NULL,&ddsd,DDLOCK_WRITEONLY | DDLOCK_WAIT,NULL);
     if (err != DD_OK) {
        D3DSetError("Can't Lock BackBuffer", err);
        return 0;
     }

     _BackBuffer->Unlock(NULL);

     int s,m;

     for (s = 0, m = ddsd.ddpfPixelFormat.dwRBitMask; !(m & 1); s++, m >>= 1);
     _rFShift = s;
     _rFScale = 8 - BitCount(ddsd.ddpfPixelFormat.dwRBitMask);

     for (s = 0, m = ddsd.ddpfPixelFormat.dwGBitMask; !(m & 1); s++, m >>= 1);
     _gFShift = s;
     _gFScale = 8 - BitCount(ddsd.ddpfPixelFormat.dwGBitMask);

     for (s = 0, m = ddsd.ddpfPixelFormat.dwBBitMask; !(m & 1); s++, m >>= 1);
     _bFShift = s;
     _bFScale = 8 - BitCount(ddsd.ddpfPixelFormat.dwBBitMask);
     */

     unsigned short hPal[256];

     //FILE *f=fopen("a.a","w");

     for(i = 0;i < 256;i++) {
        hPal[i] = (unsigned short)(((((unsigned long)fontHeader.pPal[i*3+0])>>_rScale)<<_rShift) |
                  ((((unsigned long)fontHeader.pPal[i*3+1])>>_gScale)<<_gShift) |
                  ((((unsigned long)fontHeader.pPal[i*3+2])>>_bScale)<<_bShift));

        //fprintf(f,"%x\n",hPal[i]);
        //hPal[i] = (hPal[i]>>8) | (hPal[i]<<8);
     }

     //fclose(f);

     for(i = 0;i < fontHeader.nFntSprSize;i++)
        pHwFntSpr[i] = hPal[pOriginalFntSpr[i]];

  }
  else {
     if (pHwFntSpr) {
        delete [] pHwFntSpr;
        pHwFntSpr = NULL;
     }

     if (pSwFntSpr == NULL)
        pSwFntSpr = new unsigned char [fontHeader.nFntSprSize];

     unsigned char sPal[256];

     for(i = 0;i < 256;i++) {
        RGB_i rgb(fontHeader.pPal[i*3+0],
                  fontHeader.pPal[i*3+1],
                  fontHeader.pPal[i*3+2]);
        sPal[i] = (unsigned char) epal_Match( _EPal, rgb);
     }

     for(i = 0;i < fontHeader.nFntSprSize;i++)
        pSwFntSpr[i] = sPal[pOriginalFntSpr[i]];

  }

  return 1;
}


int CFixedColorFont::PrintAt(long x, long y, const char *str)
{ int sx, off, j, i;
  unsigned char *ptr,*ptr1;
  unsigned short *ptrX;

  if (_dL.currDevice == NULL || pOriginalFntSpr == NULL)
     return 0;

  if (*str == 0) return 1;

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     if (pHwFntSpr == NULL) return 0;

     DDSURFACEDESC ddsd;
     //memset(&ddsd, 0, sizeof(DDSURFACEDESC));
     ddsd.dwSize = sizeof(DDSURFACEDESC);

     int err = _BackBuffer->Lock(NULL,&ddsd,DDLOCK_WRITEONLY | DDLOCK_WAIT,NULL);
     if (err != DD_OK) {
        D3DSetError("Can't Lock BackBuffer", err);
        return 0;
     }

     //ASM_HwDrawFont();
     x += _gr_nScreenOriginX;
     y += _gr_nScreenOriginY;

     ptr = (unsigned char*)ddsd.lpSurface + y*ddsd.lPitch + x*2;

     while(*str != 0) {
        if ((sx = fontHeader.pSmFntTable[((long)*str)*2]) != 0) {
           off = fontHeader.pSmFntTable[((long)*str)*2+1];
           ptr1 = ptr;
           for(j = 0;j < fontHeader.nFntHeight;j++,off += fontHeader.nFntWidth,ptr1 += ddsd.lPitch)
              for(i = 0,ptrX = (unsigned short*)ptr1;i < sx;i++,ptrX++)
                 if(pHwFntSpr[off+i] != 0) *ptrX = pHwFntSpr[off+i];
           ptr += sx*2;
        }
        str++;
     }

     err = _BackBuffer->Unlock(NULL);
     if (err != DD_OK) {
        D3DSetError("Can't UnLock BackBuffer", err);
        return 0;
     }
  }
  else {
     if (pSwFntSpr == NULL) return 0;

     ASM_PrintAt(str,x,y,fontHeader.nFntWidth,fontHeader.nFntHeight,fontHeader.pSmFntTable,pSwFntSpr);
  }

  return 1;
}


int CFixedColorFont::PrintColorAt(long x, long y, const char *str, unsigned long color)
{ int sx, off, j, i;
  unsigned char *ptr,*ptr1;
  unsigned short *ptrX;

  if (_dL.currDevice == NULL || pOriginalFntSpr == NULL)
     return 0;

  if (*str == 0) return 1;

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     if (pHwFntSpr == NULL) return 0;

     DDSURFACEDESC ddsd;
     //memset(&ddsd, 0, sizeof(DDSURFACEDESC));
     ddsd.dwSize = sizeof(DDSURFACEDESC);

     int err = _BackBuffer->Lock(NULL,&ddsd,DDLOCK_WRITEONLY | DDLOCK_WAIT,NULL);
     if (err != DD_OK) {
        D3DSetError("Can't Lock BackBuffer", err);
        return 0;
     }

     //ASM_HwDrawFont();
     x += _gr_nScreenOriginX;
     y += _gr_nScreenOriginY;

     ptr = (unsigned char*)ddsd.lpSurface + y*ddsd.lPitch + x*2;

     while(*str != 0) {
        if ((sx = fontHeader.pSmFntTable[((long)*str)*2]) != 0) {
           off = fontHeader.pSmFntTable[((long)*str)*2+1];
           ptr1 = ptr;
           for(j = 0;j < fontHeader.nFntHeight;j++,off += fontHeader.nFntWidth,ptr1 += ddsd.lPitch)
              for(i = 0,ptrX = (unsigned short*)ptr1;i < sx;i++,ptrX++)
                 if(pHwFntSpr[off+i] != 0) *ptrX = (unsigned short)color;
           ptr += sx*2;
        }
        str++;
     }

     err = _BackBuffer->Unlock(NULL);
     if (err != DD_OK) {
        D3DSetError("Can't UnLock BackBuffer", err);
        return 0;
     }
  }
  else {
     if (pSwFntSpr == NULL) return 0;

     ASM_PrintColorAt(str,x,y,fontHeader.nFntWidth,fontHeader.nFntHeight,fontHeader.pSmFntTable,pSwFntSpr,color);
  }

  return 1;
}

int CFixedColorFont::PrintClipAt(long x, long y, const char *str)
{ int sx, off, j, i;
  unsigned char *ptr,*ptr1;
  unsigned short *ptrX;
  int right, bottom, mask;

  if (_dL.currDevice == NULL || pOriginalFntSpr == NULL)
     return 0;

  if (*str == 0) return 1;

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     if (pHwFntSpr == NULL) return 0;

     right  = x + StringWidth(str);
     bottom = y + fontHeader.nFntHeight;

     //simple clip
     if (x >= _gr_clipRect.right || right < _gr_clipRect.left ||
         y >= _gr_clipRect.bottom || bottom < _gr_clipRect.top
        )
        return 1;


     DDSURFACEDESC ddsd;
     //memset(&ddsd, 0, sizeof(DDSURFACEDESC));
     ddsd.dwSize = sizeof(DDSURFACEDESC);

     int err = _BackBuffer->Lock(NULL,&ddsd,DDLOCK_WRITEONLY | DDLOCK_WAIT,NULL);
     if (err != DD_OK) {
        D3DSetError("Can't Lock BackBuffer", err);
        return 0;
     }

     mask = 0;

     if (x < _gr_clipRect.left) mask = 1;
     if (y < _gr_clipRect.top) mask |= 2;
     if (right > _gr_clipRect.right) mask |= 4;
     if (bottom > _gr_clipRect.bottom) mask |= 8;

     if (mask == 0) {
        //no clip needed
        x += _gr_nScreenOriginX;
        y += _gr_nScreenOriginY;

        ptr = (unsigned char*)ddsd.lpSurface + y*ddsd.lPitch + x*2;

        while(*str != 0) {
           if ((sx = fontHeader.pSmFntTable[((long)*str)*2]) != 0) {
              off = fontHeader.pSmFntTable[((long)*str)*2+1];
              ptr1 = ptr;
              for(j = 0;j < fontHeader.nFntHeight;j++,off += fontHeader.nFntWidth,ptr1 += ddsd.lPitch)
                 for(i = 0,ptrX = (unsigned short*)ptr1;i < sx;i++,ptrX++)
                    if(pHwFntSpr[off+i] != 0) *ptrX = pHwFntSpr[off+i];
              ptr += sx*2;
           }
           str++;
        }
     }
     else {
        //clip needed
        int startY = 0, height = fontHeader.nFntHeight;
        int startOff = 0;

        if (mask & 2) {
           //top
           startY = _gr_clipRect.top - y;
           height -= startY;
           startOff = fontHeader.nFntWidth * startY;
        }

        if (mask & 8) {
           //bottom
           height -= bottom - _gr_clipRect.bottom;
        }

        if (mask & 1) {
           //left
           while(1) {
              sx = fontHeader.pSmFntTable[((long)*str)*2];
              if (x + sx >= _gr_clipRect.left) break;
              x += sx;
              str++;
           }

           int offX = _gr_clipRect.left - x;
           x = _gr_clipRect.left + _gr_nScreenOriginX;
           y += _gr_nScreenOriginY + startY;

           ptr = (unsigned char*)ddsd.lpSurface + y*ddsd.lPitch + x*2;
           off = fontHeader.pSmFntTable[((long)*str)*2+1] + startOff + offX;
           ptr1 = ptr;
           for(j = 0;j < height;j++,off += fontHeader.nFntWidth,ptr1 += ddsd.lPitch)
              for(i = 0,ptrX = (unsigned short*)ptr1;i < sx - offX;i++,ptrX++)
                 if(pHwFntSpr[off+i] != 0) *ptrX = pHwFntSpr[off+i];
           ptr += (sx-offX)*2;
           str++;

        }
        else {
           x += _gr_nScreenOriginX;
           y += _gr_nScreenOriginY + startY;

           ptr = (unsigned char*)ddsd.lpSurface + y*ddsd.lPitch + x*2;
        }

        if (mask & 4) {
           //right
           while (*str != 0) {
              sx = fontHeader.pSmFntTable[((long)*str)*2];

              if (x + sx > _gr_clipRect.right + _gr_nScreenOriginX) {
                 int width = _gr_clipRect.right + _gr_nScreenOriginX - x;

                 off = fontHeader.pSmFntTable[((long)*str)*2+1] + startOff;
                 ptr1 = ptr;
                 for(j = 0;j < height;j++,off += fontHeader.nFntWidth,ptr1 += ddsd.lPitch)
                    for(i = 0,ptrX = (unsigned short*)ptr1;i < width;i++,ptrX++)
                       if(pHwFntSpr[off+i] != 0) *ptrX = pHwFntSpr[off+i];
                 break;
              }
              else {
                 if (sx != 0) {
                    off = fontHeader.pSmFntTable[((long)*str)*2+1] + startOff;
                    ptr1 = ptr;
                    for(j = 0;j < height;j++,off += fontHeader.nFntWidth,ptr1 += ddsd.lPitch)
                       for(i = 0,ptrX = (unsigned short*)ptr1;i < sx;i++,ptrX++)
                          if(pHwFntSpr[off+i] != 0) *ptrX = pHwFntSpr[off+i];
                    ptr += sx*2;
                    x += sx;
                 }
              }
              str++;
           }
        }
        else {
           while(*str != 0) {
              if ((sx = fontHeader.pSmFntTable[((long)*str)*2]) != 0) {
                 off = fontHeader.pSmFntTable[((long)*str)*2+1] + startOff;
                 ptr1 = ptr;
                 for(j = 0;j < height;j++,off += fontHeader.nFntWidth,ptr1 += ddsd.lPitch)
                    for(i = 0,ptrX = (unsigned short*)ptr1;i < sx;i++,ptrX++)
                       if(pHwFntSpr[off+i] != 0) *ptrX = pHwFntSpr[off+i];
                 ptr += sx*2;
              }
              str++;
           }
        }

     }

     err = _BackBuffer->Unlock(NULL);
     if (err != DD_OK) {
        D3DSetError("Can't UnLock BackBuffer", err);
        return 0;
     }
  }
  else {
     if (pSwFntSpr == NULL) return 0;

     ASM_PrintClipAt(str,x,y,fontHeader.nFntWidth,fontHeader.nFntHeight,fontHeader.pSmFntTable,pSwFntSpr);
  }

  return 1;
}


int CFixedColorFont::PrintClipColorAt(long x, long y, const char *str, unsigned long color)
{ int sx, off, j, i;
  unsigned char *ptr,*ptr1;
  unsigned short *ptrX;
  int right, bottom, mask;

  if (_dL.currDevice == NULL || pOriginalFntSpr == NULL)
     return 0;

  if (*str == 0) return 1;

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     if (pHwFntSpr == NULL) return 0;

     right  = x + StringWidth(str);
     bottom = y + fontHeader.nFntHeight;

     //simple clip
     if (x >= _gr_clipRect.right || right < _gr_clipRect.left ||
         y >= _gr_clipRect.bottom || bottom < _gr_clipRect.top
        )
        return 1;


     DDSURFACEDESC ddsd;
     //memset(&ddsd, 0, sizeof(DDSURFACEDESC));
     ddsd.dwSize = sizeof(DDSURFACEDESC);

     int err = _BackBuffer->Lock(NULL,&ddsd,DDLOCK_WRITEONLY | DDLOCK_WAIT,NULL);
     if (err != DD_OK) {
        D3DSetError("Can't Lock BackBuffer", err);
        return 0;
     }

     mask = 0;

     if (x < _gr_clipRect.left) mask = 1;
     if (y < _gr_clipRect.top) mask |= 2;
     if (right > _gr_clipRect.right) mask |= 4;
     if (bottom > _gr_clipRect.bottom) mask |= 8;

     if (mask == 0) {
        //no clip needed
        x += _gr_nScreenOriginX;
        y += _gr_nScreenOriginY;

        ptr = (unsigned char*)ddsd.lpSurface + y*ddsd.lPitch + x*2;

        while(*str != 0) {
           if ((sx = fontHeader.pSmFntTable[((long)*str)*2]) != 0) {
              off = fontHeader.pSmFntTable[((long)*str)*2+1];
              ptr1 = ptr;
              for(j = 0;j < fontHeader.nFntHeight;j++,off += fontHeader.nFntWidth,ptr1 += ddsd.lPitch)
                 for(i = 0,ptrX = (unsigned short*)ptr1;i < sx;i++,ptrX++)
                    if(pHwFntSpr[off+i] != 0) *ptrX = (unsigned short)color;
              ptr += sx*2;
           }
           str++;
        }
     }
     else {
        //clip needed
        int startY = 0, height = fontHeader.nFntHeight;
        int startOff = 0;

        if (mask & 2) {
           //top
           startY = _gr_clipRect.top - y;
           height -= startY;
           startOff = fontHeader.nFntWidth * startY;
        }

        if (mask & 8) {
           //bottom
           height -= bottom - _gr_clipRect.bottom;
        }

        if (mask & 1) {
           //left
           while(1) {
              sx = fontHeader.pSmFntTable[((long)*str)*2];
              if (x + sx >= _gr_clipRect.left) break;
              x += sx;
              str++;
           }

           int offX = _gr_clipRect.left - x;
           x = _gr_clipRect.left + _gr_nScreenOriginX;
           y += _gr_nScreenOriginY + startY;

           ptr = (unsigned char*)ddsd.lpSurface + y*ddsd.lPitch + x*2;
           off = fontHeader.pSmFntTable[((long)*str)*2+1] + startOff + offX;
           ptr1 = ptr;
           for(j = 0;j < height;j++,off += fontHeader.nFntWidth,ptr1 += ddsd.lPitch)
              for(i = 0,ptrX = (unsigned short*)ptr1;i < sx - offX;i++,ptrX++)
                 if(pHwFntSpr[off+i] != 0) *ptrX = (unsigned short)color;
           ptr += (sx-offX)*2;
           str++;

        }
        else {
           x += _gr_nScreenOriginX;
           y += _gr_nScreenOriginY + startY;

           ptr = (unsigned char*)ddsd.lpSurface + y*ddsd.lPitch + x*2;
        }

        if (mask & 4) {
           //right
           while (*str != 0) {
              sx = fontHeader.pSmFntTable[((long)*str)*2];

              if (x + sx > _gr_clipRect.right + _gr_nScreenOriginX) {
                 int width = _gr_clipRect.right + _gr_nScreenOriginX - x;

                 off = fontHeader.pSmFntTable[((long)*str)*2+1] + startOff;
                 ptr1 = ptr;
                 for(j = 0;j < height;j++,off += fontHeader.nFntWidth,ptr1 += ddsd.lPitch)
                    for(i = 0,ptrX = (unsigned short*)ptr1;i < width;i++,ptrX++)
                       if(pHwFntSpr[off+i] != 0) *ptrX = (unsigned short)color;
                 break;
              }
              else {
                 if (sx != 0) {
                    off = fontHeader.pSmFntTable[((long)*str)*2+1] + startOff;
                    ptr1 = ptr;
                    for(j = 0;j < height;j++,off += fontHeader.nFntWidth,ptr1 += ddsd.lPitch)
                       for(i = 0,ptrX = (unsigned short*)ptr1;i < sx;i++,ptrX++)
                          if(pHwFntSpr[off+i] != 0) *ptrX = (unsigned short)color;
                    ptr += sx*2;
                    x += sx;
                 }
              }
              str++;
           }
        }
        else {
           while(*str != 0) {
              if ((sx = fontHeader.pSmFntTable[((long)*str)*2]) != 0) {
                 off = fontHeader.pSmFntTable[((long)*str)*2+1] + startOff;
                 ptr1 = ptr;
                 for(j = 0;j < height;j++,off += fontHeader.nFntWidth,ptr1 += ddsd.lPitch)
                    for(i = 0,ptrX = (unsigned short*)ptr1;i < sx;i++,ptrX++)
                       if(pHwFntSpr[off+i] != 0) *ptrX = (unsigned short)color;
                 ptr += sx*2;
              }
              str++;
           }
        }

     }

     err = _BackBuffer->Unlock(NULL);
     if (err != DD_OK) {
        D3DSetError("Can't UnLock BackBuffer", err);
        return 0;
     }
  }
  else {
     if (pSwFntSpr == NULL) return 0;

     ASM_PrintClipColorAt(str,x,y,fontHeader.nFntWidth,fontHeader.nFntHeight,fontHeader.pSmFntTable,pSwFntSpr,color);
  }

  return 1;
}

 