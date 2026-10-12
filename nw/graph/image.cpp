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


/***********************************************************************
 ********************             CGRImage            ********************
 ***********************************************************************/

CGRImage::CGRImage(int w, int h)
{

  image = NULL;
  usePalette = 0;

  SetWidthHeight(w, h);
}

//===================================================================

void CGRImage::SetWidthHeight(int w, int h)
{
  if (image != NULL || _dL.currDevice == NULL) return;

  imageW = w;
  imageH = h;

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     image = new unsigned short [w*h];
     memset(image, 0, w*h*2);

  }
  else {
     image = new unsigned char [w*h];
     memset(image, 0, w*h);
  }

}
//===================================================================

void CGRImage::SetPalette(unsigned char *pal8)
{

  if (_dL.currDevice == NULL) return;

  if (pal8 == NULL) {
     usePalette = 0;
     return;
  }

  usePalette = 1;

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     for(int i = 0;i < 256;i++) {
        int r = (pal8[i*3 + 0]),
            g = (pal8[i*3 + 1]),
            b = (pal8[i*3 + 2]);

        palToScr16[i] = (unsigned short)(((r>>_rScale)<<_rShift) |
                                         ((g>>_gScale)<<_gShift) |
                                         ((b>>_bScale)<<_bShift)
                                        );
     }
     palToScr16[0] = 0;
  }
  else {
     for(int i = 0;i < 256;i++) {
        RGB_i rgb((pal8[i*3 + 0] ),
                  (pal8[i*3 + 1] ),
                  (pal8[i*3 + 2] ));

        palToScr8[i] = (unsigned char) epal_Match( _EPal, rgb);
     }
     palToScr8[0] = 0;
  }


}

//===================================================================
int CGRImage::LoadFromBMPFile(const char *bmpName, int x, int y)
{  FILE *fb;
   int w,h,wost;
   unsigned char *bmp;

   if (_dL.currDevice == NULL) return 0;

   fb = fopen(bmpName,"rb");
   RTCHECK1(fb,"BMP File not found %s",bmpName);

   BITMAPFILEHEADER    bf;
   BITMAPINFOHEADER    bi;

   RTCHECK1(
        fread(&bf,sizeof(bf),1,fb)==1        &&
		bf.bfType == 0x4D42					&&
        fread(&bi,sizeof(bi),1,fb)==1  ,"Error reading '%s'",bmpName);

   RTCHECK1( bi.biCompression == BI_RGB, "'%s' is compressed", bmpName );
   RTCHECK1( bi.biBitCount == 8, "8-bit image expected in '%s'", bmpName );
   RTCHECK1( bi.biClrUsed <= 256 /* && (m_bi.biClrImportant & 255) == 0*/, "Bad color count in '%s'",bmpName);

   RGBQUAD   pal[256];
   unsigned char iPal[768];

   memset(pal,0,sizeof(RGBQUAD)*256);
   if( bi.biClrUsed == 0 ) bi.biClrUsed = 256;
   RTCHECK1(fread(pal,sizeof(RGBQUAD),bi.biClrUsed,fb)==bi.biClrUsed, "Error reading palette from '%s'", bmpName);

   for(int i = 0;i < 256;i++) {
      iPal[i*3+0] = pal[i].rgbRed;
      iPal[i*3+1] = pal[i].rgbGreen;
      iPal[i*3+2] = pal[i].rgbBlue;
   }

   w = bi.biWidth;
   h = bi.biHeight >= 0?bi.biHeight:-bi.biHeight;

   wost = ((4-(w&3))&3);

   if (image == NULL) {
      SetWidthHeight(w, h);
   }

   bmp = new unsigned char [w*h];

   RTCHECK1(ftell(fb)==bf.bfOffBits, "Unexpected image offset in '%s'", bmpName );

   for(int i = 0;i < h;i++) {
      if (bi.biHeight > 0) //invert
         fread(bmp+(h-i-1)*w, 1, w, fb);
      else
         fread(bmp+i*w, 1, w, fb);

      fseek(fb, wost, SEEK_CUR);
   }

   fclose(fb);

   SetPalette(iPal);

   LoadPalImage(bmp, x, y,
                0, 0, w-1, h-1, w);

   delete [] bmp;

   return 1;
}

//===================================================================
int CGRImage::LoadFromSPRFile(const char *sprName, const unsigned char *pal8,
                              int x, int y
                             )
{  FILE *fs;
   unsigned short sH[3];
   unsigned char *spr;

   if (_dL.currDevice == NULL) return 0;

   fs = fopen(sprName,"rb");
   RTCHECK1(fs,"SPR File not found %s",sprName);

   fread(sH,5,1,fs);

   if (image == NULL) {
      SetWidthHeight(sH[0], sH[1]);
   }
   //else {
   //   if (sH[0] > imageW || sH[1] > imageH) {
  //       fclose(fs);
  //       return 0;
  //    }
  // }

   spr = new unsigned char [(long)sH[0]*(long)sH[1]];

   fread(spr,(long)sH[0]*(long)sH[1],1,fs);
   fclose(fs);

   SetPalette((unsigned char*)pal8);

   LoadPalImage(spr, x, y,
                0, 0, sH[0]-1, sH[1]-1, sH[0]);

   delete [] spr;

   return 1;
}

//===================================================================

void CGRImage::LoadPalImage(unsigned char *mem,
                          int x, int y,
                          int x0, int y0, int x1, int y1, int iW
                         )
{
  if (image == NULL || mem == NULL || x >= imageW || y >= imageH ||
      x + (x1-x0) < 0 || y + (y1-y0) < 0 || usePalette == 0)
     return;

  if (x < 0) {
     x0 -= x;
     x = 0;
  }

  if (y < 0) {
     y0 -= y;
     y = 0;
  }

  if (x + (x1-x0) >= imageW) {
     x1 -= x + (x1 - x0) - imageW + 1;
  }

  if (y + (y1-y0) >= imageH) {
     y1 -= y + (y1-y0) - imageH + 1;
  }

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     unsigned short *ptrI, *ptrI1;
     unsigned char *ptr, *ptr1;

     ptrI = ((unsigned short*)image) + x + y*imageW;
     ptr = mem + x0 + y0*iW;

     for(int j = y0;j <= y1;j++,ptrI += imageW,ptr += iW) {
        ptrI1 = ptrI;
        ptr1 = ptr;
        for(int i = x0;i <= x1;i++)
           *ptrI1++ = palToScr16[*ptr1++];
     }

  }
  else {
     unsigned char *ptrI, *ptrI1;
     unsigned char *ptr, *ptr1;

     ptrI = ((unsigned char*)image) + x + y*imageW;
     ptr = mem + x0 + y0*iW;

     for(int j = y0;j <= y1;j++,ptrI += imageW,ptr += iW) {
        ptrI1 = ptrI;
        ptr1 = ptr;
        for(int i = x0;i <= x1;i++)
           *ptrI1++ = palToScr8[*ptr1++];
     }
  }

}

//===================================================================

void CGRImage::LoadRGBImage(unsigned long *mem,
                          int x, int y,
                          int x0, int y0, int x1, int y1, int iW
                         )
{
  if (image == NULL || mem == NULL || x >= imageW || y >= imageH ||
      x + (x1-x0) < 0 || y + (y1-y0) < 0)
     return;

  if (x < 0) {
     x0 -= x;
     x = 0;
  }

  if (y < 0) {
     y0 -= y;
     y = 0;
  }

  if (x + (x1-x0) >= imageW) {
     x1 -= x + (x1 - x0) - imageW + 1;
  }

  if (y + (y1-y0) >= imageH) {
     y1 -= y + (y1-y0) - imageH + 1;
  }

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     unsigned short *ptrI, *ptrI1;
     unsigned long *ptr, *ptr1;

     ptrI = ((unsigned short*)image) + x + y*imageW;
     ptr = mem + x0 + y0*iW;

     for(int j = y0;j <= y1;j++,ptrI += imageW,ptr += iW) {
        ptrI1 = ptrI;
        ptr1 = ptr;
        for(int i = x0;i <= x1;i++,ptr1++)
           *ptrI1++ = (unsigned short)(((((unsigned long)((unsigned char*)ptr1)[0])>>_rScale)<<_rShift) |
                      ((((unsigned long)((unsigned char*)ptr1)[1])>>_gScale)<<_gShift) |
                      ((((unsigned long)((unsigned char*)ptr1)[2])>>_bScale)<<_bShift));
     }

  }
  else {
     unsigned char *ptrI, *ptrI1;
     unsigned long *ptr, *ptr1;

     ptrI = ((unsigned char*)image) + x + y*imageW;
     ptr = mem + x0 + y0*iW;

     for(int j = y0;j <= y1;j++,ptrI += imageW,ptr += iW) {
        ptrI1 = ptrI;
        ptr1 = ptr;
        for(int i = x0;i <= x1;i++,ptr1++)
           if (((unsigned char*)ptr1)[0] == 0 &&
               ((unsigned char*)ptr1)[1] == 0 &&
               ((unsigned char*)ptr1)[2] == 0
              )
              *ptrI1++ = 0;
           else
              *ptrI1++ = (unsigned char) epal_Match( _EPal,
                          RGB_i(((unsigned char*)ptr1)[0],
                                ((unsigned char*)ptr1)[1],
                                ((unsigned char*)ptr1)[2]
                              ));
     }
  }


}

//===================================================================

void CGRImage::LoadFromImage(CGRImage &im,
                           int x, int y,
                           int x0, int y0, int x1, int y1)
{
  if (image == NULL || im.image == NULL || x >= imageW || y >= imageH ||
      x + (x1-x0) < 0 || y + (y1-y0) < 0)
     return;

  if (x < 0) {
     x0 -= x;
     x = 0;
  }

  if (y < 0) {
     y0 -= y;
     y = 0;
  }

  if (x + (x1-x0) >= imageW) {
     x1 -= x + (x1 - x0) - imageW + 1;
  }

  if (y + (y1-y0) >= imageH) {
     y1 -= y + (y1-y0) - imageH + 1;
  }

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     unsigned short *ptrI;
     unsigned short *ptr;

     ptrI = ((unsigned short*)image) + x + y*imageW;
     ptr = ((unsigned short*)im.image) + x0 + y0*im.imageW;

     for(int j = y0;j <= y1;j++,ptrI += imageW,ptr += im.imageW) {
        memcpy(ptrI,ptr,(x1-x0+1)*2);
     }

  }
  else {
     unsigned char *ptrI;
     unsigned char *ptr;

     ptrI = ((unsigned char*)image) + x + y*imageW;
     ptr = ((unsigned char*)im.image) + x0 + y0*im.imageW;

     for(int j = y0;j <= y1;j++,ptrI += imageW,ptr += im.imageW) {
        memcpy(ptrI,ptr,(x1-x0+1));
     }
  }
}

//===================================================================

void CGRImage::LoadFromScreen(int x, int y,
                              int x0, int y0, int x1, int y1)
{
  if (image == NULL || x >= imageW || y >= imageH ||
      x + (x1-x0) < 0 || y + (y1-y0) < 0)
     return;

  if (x < 0) {
     x0 -= x;
     x = 0;
  }

  if (y < 0) {
     y0 -= y;
     y = 0;
  }

  if (x + (x1-x0) >= imageW) {
     x1 -= x + (x1 - x0) - imageW + 1;
  }

  if (y + (y1-y0) >= imageH) {
     y1 -= y + (y1-y0) - imageH + 1;
  }

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     unsigned char *ptr;
     unsigned short *ptrI;

     DDSURFACEDESC ddsd;
     //memset(&ddsd, 0, sizeof(DDSURFACEDESC));
     ddsd.dwSize = sizeof(DDSURFACEDESC);

     int err = _BackBuffer->Lock(NULL,&ddsd,DDLOCK_READONLY | DDLOCK_WAIT,NULL);
     if (err != DD_OK) {
        D3DSetError("Can't Lock BackBuffer", err);
        return;
     }

     ptr = (unsigned char*)ddsd.lpSurface + y0*ddsd.lPitch + x0*2;
     ptrI = ((unsigned short*)image) + x + y*imageW;

     for(int j = y0;j <= y1;j++,ptr += ddsd.lPitch,ptrI += imageW) {
        memcpy(ptrI, ptr, (x1-x0+1)*2);
     }

     err = _BackBuffer->Unlock(NULL);
     if (err != DD_OK) {
        D3DSetError("Can't UnLock BackBuffer", err);
        return;
     }

  }
  else {
     unsigned char *ptrI;
     unsigned char *ptr;

     ptrI = ((unsigned char*)image) + x + y*imageW;
     ptr = ((unsigned char*)_gr_pScreen) + x0 + y0*_gr_nScreenWidth;

     for(int j = y0;j <= y1;j++,ptrI += imageW,ptr += _gr_nScreenWidth) {
        memcpy(ptrI,ptr,(x1-x0+1));
     }
  }
}

//===================================================================

int CGRImage::Pset(int x, int y, int r, int g, int b)
{
  if (image == NULL || x < 0 || y < 0 || x >= imageW || y >= imageH)
     return 0;

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     ((unsigned short*)image)[x + y*imageW] = (unsigned short)(((r>>_rScale)<<_rShift) |
                                              ((g>>_gScale)<<_gShift) |
                                              ((b>>_bScale)<<_bShift));
  }
  else {
     if (r == 0 && g == 0 && b == 0)
        ((unsigned char*)image)[x + y*imageW] = 0;
     else
        ((unsigned char*)image)[x + y*imageW] =
                            (unsigned char) epal_Match( _EPal, RGB_i(r,g,b));
  }

  return 1;
}

//===================================================================

int CGRImage::Pset(int x, int y, unsigned long color)
{
  if (image == NULL || x < 0 || y < 0 || x >= imageW || y >= imageH)
     return 0;

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     ((unsigned short*)image)[x + y*imageW] = (unsigned short) color;
  }
  else {
     ((unsigned char*)image)[x + y*imageW] = (unsigned char) color;
  }

  return 1;
}

//===================================================================

int CGRImage::Pset(int x, int y, unsigned char color)
{
  if (image == NULL || x < 0 || y < 0 || x >= imageW || y >= imageH || usePalette == 0)
     return 0;

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     ((unsigned short*)image)[x + y*imageW] = palToScr16[color];
  }
  else {
     ((unsigned char*)image)[x + y*imageW] = palToScr8[color];
  }

  return 1;
}

//===================================================================

int  CGRImage::Draw(int xs, int ys)  const
{ int x, y, x0, y0, x1, y1;

  if (image == NULL || xs >= _gr_nScreenWidth || ys >= _gr_nScreenHeight ||
                       xs + imageW <= 0 || ys + imageH <= 0)
     return 0;

  if (xs < 0) {
     x0 = -xs;
     x  = 0;
  }
  else {
     x0 = 0;
     x = xs;
  }

  if (ys < 0) {
     y0 = -ys;
     y = 0;
  }
  else {
     y0 = 0;
     y = ys;
  }

  if (xs + imageW >= _gr_nScreenWidth) {
     x1 = _gr_nScreenWidth - xs - 1;
  }
  else {
     x1 = imageW - 1;
  }

  if (ys + imageH >= _gr_nScreenHeight) {
     y1 = _gr_nScreenHeight - ys - 1;
  }
  else {
     y1 = imageH - 1;
  }

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     unsigned char *ptr;
     unsigned short *ptrI;

     DDSURFACEDESC ddsd;
     //memset(&ddsd, 0, sizeof(DDSURFACEDESC));
     ddsd.dwSize = sizeof(DDSURFACEDESC);

     int err = _BackBuffer->Lock(NULL,&ddsd,DDLOCK_WRITEONLY | DDLOCK_WAIT,NULL);
     if (err != DD_OK) {
        D3DSetError("Can't Lock BackBuffer", err);
        return 0;
     }

     ptr = (unsigned char*)ddsd.lpSurface + y*ddsd.lPitch + x*2;
     ptrI = ((unsigned short*)image) + x0 + y0*imageW;

     for(int j = y0;j <= y1;j++,ptr += ddsd.lPitch,ptrI += imageW) {
        memcpy(ptr, ptrI, (x1-x0+1)*2);
     }

     err = _BackBuffer->Unlock(NULL);
     if (err != DD_OK) {
        D3DSetError("Can't UnLock BackBuffer", err);
        return 0;
     }
  }
  else {
     unsigned char *ptr, *ptrI;

     ptr = _gr_pScreen + y*_gr_nScreenWidth + x;
     ptrI = ((unsigned char*)image) + x0 + y0*imageW;

     for(int j = y0;j <= y1;j++,ptr += _gr_nScreenWidth,ptrI += imageW) {
        memcpy(ptr, ptrI, (x1-x0+1));
     }
  }

  return 1;

}


//===================================================================

int  CGRImage::DrawX2(int xs, int ys)  const
{ int x, y, x0, y0, x1, y1;

  if (image == NULL || xs >= _gr_nScreenWidth || ys >= _gr_nScreenHeight ||
                       xs + imageW*2 <= 0 || ys + imageH*2 <= 0)
     return 0;

  if (xs < 0) {
     x0 = -xs;
     x  = 0;
  }
  else {
     x0 = 0;
     x = xs;
  }

  if (ys < 0) {
     y0 = -ys;
     y = 0;
  }
  else {
     y0 = 0;
     y = ys;
  }

  if (xs + imageW*2 >= _gr_nScreenWidth) {
     x1 = _gr_nScreenWidth - xs - 1;
  }
  else {
     x1 = imageW*2 - 1;
  }

  if (ys + imageH*2 >= _gr_nScreenHeight) {
     y1 = _gr_nScreenHeight - ys - 1;
  }
  else {
     y1 = imageH*2 - 1;
  }

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     unsigned char *ptr;
     unsigned short *ptrI;

     DDSURFACEDESC ddsd;
     //memset(&ddsd, 0, sizeof(DDSURFACEDESC));
     ddsd.dwSize = sizeof(DDSURFACEDESC);

     int err = _BackBuffer->Lock(NULL,&ddsd,DDLOCK_WRITEONLY | DDLOCK_WAIT,NULL);
     if (err != DD_OK) {
        D3DSetError("Can't Lock BackBuffer", err);
        return 0;
     }

     ptr = (unsigned char*)ddsd.lpSurface + y*ddsd.lPitch + x*2;
     ptrI = ((unsigned short*)image) + x0 + y0*imageW;

     for(int j = y0;j <= y1;j+=2,ptr += ddsd.lPitch*2,ptrI += imageW) {
        //memcpy(ptr, ptrI, (x1-x0+1)*2);
        for(int i = 0;i < (x1-x0+1)/2;i++) {
           x = ptrI[i];
           x = x | (x<<16);
           ((unsigned long*)ptr)[i] = x;
           ((unsigned long*)(ptr+ddsd.lPitch))[i] = x;
        }

     }

     err = _BackBuffer->Unlock(NULL);
     if (err != DD_OK) {
        D3DSetError("Can't UnLock BackBuffer", err);
        return 0;
     }
  }
  else {
     unsigned char *ptr, *ptrI;

     ptr = _gr_pScreen + y*_gr_nScreenWidth + x;
     ptrI = ((unsigned char*)image) + x0 + y0*imageW;

     for(int j = y0;j <= y1;j+=2,ptr += _gr_nScreenWidth*2,ptrI += imageW) {
        //memcpy(ptr, ptrI, (x1-x0+1));
        for(int i = 0;i < (x1-x0+1)/2;i++) {
           x = ptrI[i];
           x = x | (x<<8);
           ((unsigned short*)ptr)[i] = (unsigned short)x;
           ((unsigned short*)(ptr+_gr_nScreenWidth))[i] = (unsigned short)x;
        }
     }
  }

  return 1;

}

//===================================================================

int  CGRImage::DrawSprite(int xs, int ys)  const
{ int x, y, x0, y0, x1, y1;

  if (image == NULL || xs >= _gr_nScreenWidth || ys >= _gr_nScreenHeight ||
                       xs + imageW <= 0 || ys + imageH <= 0)
     return 0;

  if (xs < 0) {
     x0 = -xs;
     x  = 0;
  }
  else {
     x0 = 0;
     x = xs;
  }

  if (ys < 0) {
     y0 = -ys;
     y = 0;
  }
  else {
     y0 = 0;
     y = ys;
  }

  if (xs + imageW >= _gr_nScreenWidth) {
     x1 = _gr_nScreenWidth - xs - 1;
  }
  else {
     x1 = imageW - 1;
  }

  if (ys + imageH >= _gr_nScreenHeight) {
     y1 = _gr_nScreenHeight - ys - 1;
  }
  else {
     y1 = imageH - 1;
  }

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     unsigned char *ptr;
     unsigned short *ptr1,*ptrI,*ptrI1;

     DDSURFACEDESC ddsd;
     //memset(&ddsd, 0, sizeof(DDSURFACEDESC));
     ddsd.dwSize = sizeof(DDSURFACEDESC);

     int err = _BackBuffer->Lock(NULL,&ddsd,DDLOCK_WRITEONLY | DDLOCK_WAIT,NULL);
     if (err != DD_OK) {
        D3DSetError("Can't Lock BackBuffer", err);
        return 0;
     }

     ptr = (unsigned char*)ddsd.lpSurface + y*ddsd.lPitch + x*2;
     ptrI = ((unsigned short*)image) + x0 + y0*imageW;

     for(int j = y0;j <= y1;j++,ptr += ddsd.lPitch,ptrI += imageW) {
        ptr1 = (unsigned short*) ptr;
        ptrI1 = ptrI;
        for(int i = x0;i <= x1;i++,ptr1++,ptrI1++)
           if (*ptrI1 != 0) *ptr1 = *ptrI1;
     }

     err = _BackBuffer->Unlock(NULL);
     if (err != DD_OK) {
        D3DSetError("Can't UnLock BackBuffer", err);
        return 0;
     }
  }
  else {
     unsigned char *ptr, *ptrI, *ptr1, *ptrI1;

     ptr = _gr_pScreen + y*_gr_nScreenWidth + x;
     ptrI = ((unsigned char*)image) + x0 + y0*imageW;

     for(int j = y0;j <= y1;j++,ptr += _gr_nScreenWidth,ptrI += imageW) {
        ptr1 = ptr;
        ptrI1 = ptrI;
        for(int i = x0;i <= x1;i++,ptr1++,ptrI1++)
           if (*ptrI1 != 0) *ptr1 = *ptrI1;
     }
  }

  return 1;

}

//===================================================================

int  CGRImage::Draw(int xs, int ys, int x0, int y0, int x1, int y1)  const
{
  if (image == NULL || xs >= _gr_nScreenWidth || ys >= _gr_nScreenHeight ||
                       xs + (x1-x0) < 0 || ys + (y1-y0) < 0)
     return 0;

  if (xs < 0) {
     x0 -= xs;
     xs  = 0;
  }

  if (ys < 0) {
     y0 -= ys;
     ys = 0;
  }

  if (xs + (x1-x0) >= _gr_nScreenWidth) {
     x1 -= xs + (x1-x0) - _gr_nScreenWidth + 1;
  }

  if (ys + (y1-y0) >= _gr_nScreenHeight) {
     y1 -= ys + (y1-y0) - _gr_nScreenHeight + 1;
  }

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     unsigned char *ptr;
     unsigned short *ptrI;

     DDSURFACEDESC ddsd;
     //memset(&ddsd, 0, sizeof(DDSURFACEDESC));
     ddsd.dwSize = sizeof(DDSURFACEDESC);

     int err = _BackBuffer->Lock(NULL,&ddsd,DDLOCK_WRITEONLY | DDLOCK_WAIT,NULL);
     if (err != DD_OK) {
        D3DSetError("Can't Lock BackBuffer", err);
        return 0;
     }

     ptr = (unsigned char*)ddsd.lpSurface + ys*ddsd.lPitch + xs*2;
     ptrI = ((unsigned short*)image) + x0 + y0*imageW;

     for(int j = y0;j <= y1;j++,ptr += ddsd.lPitch,ptrI += imageW) {
        memcpy(ptr, ptrI, (x1-x0+1)*2);
     }

     err = _BackBuffer->Unlock(NULL);
     if (err != DD_OK) {
        D3DSetError("Can't UnLock BackBuffer", err);
        return 0;
     }
  }
  else {
     unsigned char *ptr, *ptrI;

     ptr = _gr_pScreen + ys*_gr_nScreenWidth + xs;
     ptrI = ((unsigned char*)image) + x0 + y0*imageW;

     for(int j = y0;j <= y1;j++,ptr += _gr_nScreenWidth,ptrI += imageW) {
        memcpy(ptr, ptrI, (x1-x0+1));
     }
  }

  return 1;

}

//===================================================================

int  CGRImage::DrawSprite(int xs, int ys, int x0, int y0, int x1, int y1) const
{
  if (image == NULL || xs >= _gr_nScreenWidth || ys >= _gr_nScreenHeight ||
                       xs + (x1-x0) < 0 || ys + (y1-y0) < 0)
     return 0;

  if (xs < 0) {
     x0 -= xs;
     xs  = 0;
  }

  if (ys < 0) {
     y0 -= ys;
     ys = 0;
  }

  if (xs + (x1-x0) >= _gr_nScreenWidth) {
     x1 -= xs + (x1-x0) - _gr_nScreenWidth + 1;
  }

  if (ys + (y1-y0) >= _gr_nScreenHeight) {
     y1 -= ys + (y1-y0) - _gr_nScreenHeight + 1;
  }

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     unsigned char *ptr;
     unsigned short *ptr1,*ptrI,*ptrI1;

     DDSURFACEDESC ddsd;
     //memset(&ddsd, 0, sizeof(DDSURFACEDESC));
     ddsd.dwSize = sizeof(DDSURFACEDESC);

     int err = _BackBuffer->Lock(NULL,&ddsd,DDLOCK_WRITEONLY | DDLOCK_WAIT,NULL);
     if (err != DD_OK) {
        D3DSetError("Can't Lock BackBuffer", err);
        return 0;
     }

     ptr = (unsigned char*)ddsd.lpSurface + ys*ddsd.lPitch + xs*2;
     ptrI = ((unsigned short*)image) + x0 + y0*imageW;

     for(int j = y0;j <= y1;j++,ptr += ddsd.lPitch,ptrI += imageW) {
        ptr1 = (unsigned short*) ptr;
        ptrI1 = ptrI;
        for(int i = x0;i <= x1;i++,ptr1++,ptrI1++)
           if (*ptrI1 != 0) *ptr1 = *ptrI1;
     }

     err = _BackBuffer->Unlock(NULL);
     if (err != DD_OK) {
        D3DSetError("Can't UnLock BackBuffer", err);
        return 0;
     }
  }
  else {
     unsigned char *ptr, *ptrI, *ptr1, *ptrI1;

     ptr = _gr_pScreen + ys*_gr_nScreenWidth + xs;
     ptrI = ((unsigned char*)image) + x0 + y0*imageW;

     for(int j = y0;j <= y1;j++,ptr += _gr_nScreenWidth,ptrI += imageW) {
        ptr1 = ptr;
        ptrI1 = ptrI;
        for(int i = x0;i <= x1;i++,ptr1++,ptrI1++)
           if (*ptrI1 != 0) *ptr1 = *ptrI1;
     }
  }

  return 1;

}