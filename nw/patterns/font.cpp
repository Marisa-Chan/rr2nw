#ifndef	__PATTERNS_CPP__
#include"_pattern.h"
#endif

#if 0 //_USE_OWNGR || _USE_GLIDE
#define FONT_NAME "FONT"

CFixedFont::CFixedFont(const char * filename)
{
  FILE *f;
  char fName[4];
  long size;

  pFntSpr = NULL;
  if ((f = CFileResource::FOpenCurrent(filename))==NULL) {
	RTCHECK(f,filename); return;
  }
  fread(fName,4,1,f);
  if (strnicmp(fName,FONT_NAME,4) != 0) {
		RTCHECK(0,filename);
	  fclose(f);
	  return;
  }
  fread(&nFntWidth,4,1,f);
  fread(&nFntHeight,4,1,f);
  fread(&size,4,1,f);
  fread(pPal,48,1,f);
  fread(pSmFntTable,512,4,f);
  pFntSpr = new unsigned char [size];
  fread(pFntSpr,size,1,f);
  fclose(f);
  xs = ys = 0;
}

#if _USE_GLIDE

#define ABGR(a,b,g,r) ((((long)(a))<<24)+(((long)(b))<<16)+ \
                      (((long)(g))<<8)+((long)(r)))

static void DrawFont(TCchar * str, long x, long y, long nWidth,long nHeight,
			long * SmTable, unsigned char *FntSpr,long color)
{   unsigned long *ptr,col;
	long i,j,sx,off;

	grLfbBegin();
	grLfbBypassMode(GR_LFBBYPASS_ENABLE);
	grLfbWriteMode(GR_LFBWRITEMODE_888);
	ptr = (unsigned long *) grLfbGetWritePtr(GR_BUFFER_BACKBUFFER);

	if (color < 0) col = 0x0;
	else col = ABGR(0,_gr_glidePal[color].b,
	          _gr_glidePal[color].g,_gr_glidePal[color].r);
	x += _gr_nScreenOriginX;
	y += _gr_nScreenOriginY;
	while(*str != 0) {
	   if ((sx = SmTable[((long)*str)*2]) != 0) {
	      off = SmTable[((long)*str)*2+1];
		  for(j=0;j<nHeight;j++,off+=nWidth)
			 for(i=0;i<sx;i++)
				if(FntSpr[off+i] < 16) ptr[x+i+(y+j)*1024] = col;
		  x += sx;
	   }
	   str++;
	}

	grLfbBypassMode(GR_LFBBYPASS_DISABLE);
	grLfbEnd();
}

//------------------------------------------------
extern "C" void GRPrintString(TCchar * str, long x, long y, long nWidth,long nHeight,
			long * SmTable, unsigned char *FntSpr)
{
  DrawFont(str,x,y,nWidth,nHeight,SmTable,FntSpr,-1);
}

extern "C" void GRCPrintString(TCchar * str, long x, long y, long nWidth,long nHeight,
			long * SmTable, unsigned char *FntSpr)
{
  DrawFont(str,x,y,nWidth,nHeight,SmTable,FntSpr,-1);
}

extern "C" void GRCPrintStringColor(TCchar * str, long x, long y, long nWidth,long nHeight,
			long * SmTable, unsigned char *FntSpr,long color)
{
  DrawFont(str,x,y,nWidth,nHeight,SmTable,FntSpr,color);
}

#endif

#endif
