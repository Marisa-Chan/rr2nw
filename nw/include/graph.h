#ifndef __GRAPH_H__
#define __GRAPH_H__

#if defined(_GRAPH_CPP_)
#define GLOBAL
#undef _GRAPH_CPP_
#else
#define GLOBAL extern
#endif

#include <windows.h>
#include "ddraw.h"
#include "mathlib.h"
//#include "sd1_epal.h"

//#pragma warning 836 10

#define _USE_OWNGR  1
#define _USE_GLIDE  0
#define _USE_OPENGL 0

#define WINDOWS_AGP_BUILD "4.03.1212"

//Use ZBuffer?
#define USE_Z_BUFFER 1

//Hardware or Software Low-Level Graphics
#define GR_HARDWARE  0
#define GR_SOFTWARE  1

#define GR_DEVICE_MAX_VIDEO_MODES  128

//Recommended count of bits for Texture and AlphaTexture formats
#define TEXTURE_BPP       8
#define TEXTURE_ALPHA_BPP 4

//Indexes for all Texture Formats
#define NORMAL_TEXTURE_INDEX 0
#define ALPHA_TEXTURE_INDEX  1
#define RGB_TEXTURE_INDEX    2

#define MAX_TEXTURE_INDEX    3


#define GR_FOG_NONE   0
#define GR_FOG_TABLE  1
#define GR_FOG_VERTEX 2

//Description of Texture Format
typedef struct _STextureFormat {
   DDPIXELFORMAT pixelFormat;
	int rgbBitCount;
	int usePalette;
	int alphaShift, redShift, greenShift, blueShift;
	int alphaScale, redScale, greenScale, blueScale;
}STextureFormat;

//Device Description
typedef struct _SDeviceDescr {
	char name[128];
   GUID * guid;
   int  swHw; //Software or Hardware device
   int  useZBuffer; //Use or Not ZBuffer
   int  fullScreen;//Current use: FullScreen or Window
	int  present; //Device is present
	int  caps3d;  //Device support all needed for hardware low-level graphics
	int  agp;     //Device have a AGP Interface
	int  alpha;   //Alpha Formats for Device
   int  textureW, textureH;
   int  fog, fogCurrent;
	unsigned long textureMemory; //Texture Memory Count
	STextureFormat textureFormat[MAX_TEXTURE_INDEX]; //Texture Formats
	int  renderBpp; //Bpp supported for hardware low-level graphics
	int  modesQnty; //Video Modes Quantity
	unsigned long modes[GR_DEVICE_MAX_VIDEO_MODES]; //List of Video Modes
	struct _SDeviceDescr *link; //Link to a next Device
}SDeviceDescr;

//List of All Devices
typedef struct {
	SDeviceDescr *currDevice;
	int			 currMode;
   SDeviceDescr *chooseDevice;
   int          chooseMode;
   int          chooseFullScreen;
   SDeviceDescr *dDescr;
}SDeviceList;


#define MAKE_MODE_DATA(width, height, bpp) \
        (width) | ((height) << 12) | ((bpp) << 24)

#define GET_MODE_BPP(data)    (((data)&0x7f000000) >> 24)
#define GET_MODE_WIDTH(data)  (((data)&0x00000fff))
#define GET_MODE_HEIGHT(data) (((data)&0x00fff000) >> 12)

#define RELEASE(x) if (x) { (x)->Release(); (x) = NULL; }


//Maximal Vertex Count
#define GR_MAX_VERTEX 64

//Vertex Description

union UGRVertex {
   struct  {
      float x,y,z,oow;
      union {
         struct {
            unsigned long color;
         } d3dcolor;
         struct {
            unsigned char  b, g, r, a;
         } rgbColor;
      };

      unsigned long specular;
      float u;
      float v;
   } d3d;

   struct {
      int x, y, unused, iz;
   } any;
   struct {
      int x, y, unused1, iz;
   } flat;
   struct {
      int x, y, unused2, iz, b;
   } gouraud;
   struct {
      int x, y, unused3, iz, unused4[2], u, v;
   } texture;
   struct {
      int x, y, u, iz, unused5, v, unused6[2];
   } bump;

};

//Bump Vertex Description
typedef struct {
	float u, v;
} SBumpVertex;

typedef void* GR_HTEXTURE;

typedef GR_HTEXTURE (*TTextureLoadFunc)(const char *, GR_HTEXTURE, void *);
typedef int         (*TTextureLoadIntFunc)(GR_HTEXTURE);


//Polygon Description
typedef struct {
    long dwFullType,
         dwAddType;
    long nVertices;
    union {
       struct {
          long color;
       }dwColor;
       struct {
          unsigned char pal, b ,g, r;
       }rgbColor;
    };
    long  dwOpacity;
    void* hTexture;
    void* hBump;
    float a, b, c, d;
    unsigned long nLights;
}SGRPolygon;


//Texture Defines
#define TEXT_FLAG_PTR(ptr)   (((unsigned long *)(ptr))-3)
#define TEXT_WIDTH_PTR(ptr)  (((unsigned long *)(ptr))-1)
#define TEXT_HEIGHT_PTR(ptr) (((unsigned long *)(ptr))-2)
//[FLAGS][HEIGHT][WIDTH][TEXTURE]


//Texture Flags
#define TEXTURE_MEM_FORMAT  1
#define TEXTURE_PAL_FORMAT  2
#define TEXTURE_TXR_FORMAT  3

#define TEXTURE_FORMAT_MASK 3

#define TEXTURE_ALPHA       4
#define TEXTURE_RGB         8
#define TEXTURE_SPRITE      16

#define TEXTURE_UNLOAD      32

#define TEXTURE_NOT_PRESENT 64




//Define types and addTypes
#define    TYPE_COUNT       12
#define    ADD_TYPE_COUNT   3

#define    ADD_TYPE_SIZE   ((1 << ADD_TYPE_COUNT)*4)
#define    TABLE_SIZE      (TYPE_COUNT * ADD_TYPE_SIZE)

//all
#define    GR_POLY_FLAT             (0  *ADD_TYPE_SIZE)
//GRCreateColor
#define    GR_POLY_TRANSPARENT      (1  *ADD_TYPE_SIZE)
//GRTransparentColor, opacity
#define    GR_POLY_GOURAUD          (2  *ADD_TYPE_SIZE)
//GRCreateColor
#define    GR_POLY_TEXTURE_PERSP    (3  *ADD_TYPE_SIZE)
#define    GR_POLY_TEXTURE_LIN      (4  *ADD_TYPE_SIZE)
#define    GR_POLY_SPRITE_PERSP     (5  *ADD_TYPE_SIZE)
#define    GR_POLY_TEXTURE_ALPHA    (6  *ADD_TYPE_SIZE)
//GRTransparentColor, opacity
#define    GR_POLY_SPRITE_LIN       (7  *ADD_TYPE_SIZE)
//hardware ONLY
#define    GR_POLY_GOURAUD_RGB      (8  *ADD_TYPE_SIZE)
#define    GR_POLY_TEXTURE_SMP      (9  *ADD_TYPE_SIZE)
#define    GR_POLY_SPRITE_MIP       (10 *ADD_TYPE_SIZE)
#define    GR_POLY_TEXTURE_GOURAUD  (11 *ADD_TYPE_SIZE)

#define    GR_POLY_ADD_NONE         (0  *4)
#define    GR_POLY_ADD_HAZE         (1  *4)
#define    GR_POLY_ADD_BUMP         (2  *4)
#define    GR_POLY_ADD_LIGHTTHROUGH (4  *4)


typedef struct {
   unsigned char * pTable;
   int r, g, b;
}SGRColorDef;



#define LIGHT_SOURCE_COUNT  32

#define GR_LIGHT  0
#define GR_SHADOW 1

typedef struct {
   int type;
   unsigned long *colorTable;
   float x, y, z;
   int power0;
   float r;
   int color;

   UGRVertex points[64]; //x,y,z,u,v
   int qnty;
   int draw;
   void * handler;

   int power;
   //float irad2;
   float saveR; //h2 use for save r
   //long uC, vC;
   void Set(TCSFVector3 &v) { x = v.x; y = v.y; z = v.z; }
}SGRLight;

//sizeof (13 + 64*8)*4 = 2100

#define LIGHT_COLOR_BLACK  0
#define LIGHT_COLOR_RED    1
#define LIGHT_COLOR_GREEN  2
#define LIGHT_COLOR_BLUE   4
#define LIGHT_COLOR_YELLOW 3
#define LIGHT_COLOR_VIOLET 5
#define LIGHT_COLOR_CYAN   6
#define LIGHT_COLOR_WHITE  7

#define LIGHT_COLOR_COUNT  8

void SetMixLightTable(unsigned char *ptr);
int  GRInitLight(GR_HTEXTURE);
//void * GRCreateLightSource(float x, float y, float z, float rad, int power, int color);
//void GRDeleteLightSource(void *handle);
//void D3D_PreparePolygonForLight(UGRVertex * vertS, UGRVertex * vertE, void **lightH, int lightC);
//void * D3D_CreateLightTexture(void *lightH);
//void CreateLightT(UGRVertex *vertS, UGRVertex *vertE, void *lightH);
void D3D_NEW_PreparePolygonForLight(UGRVertex * vertS, int vertQnty, int lightMask);

void GRSetScale(float kx, float ky);


typedef struct {
   int x0, y0,
       x1, y1;
   int u0, v0,
       u1, v1;
   unsigned long color;
   int opacity;
   int iz;
   void *hTexture;
}SGRAlphaSprite;

/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************           ViewPort            ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/
typedef struct SRectangle_ {
	long left,top,right,bottom;
}SRectangle;


typedef struct SGRViewport_ {
    int  x, y;
    unsigned char **pCache,**pCache0;
    unsigned char *pOrigin;
    CRect2 clipRect;
}SGRViewport;

void  GRSetViewport(SGRViewport *pViewport);
SGRViewport * GRGetViewport();
SGRViewport * GRCreateViewport(int originX, int originY, TCSRect2 &clipRect);
void GRReleaseViewport(SGRViewport *pViewport);

/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************    Extern Structures & Vars   ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/

struct  GR_BITMAPINFO {
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD	bmiColors[256];
	WORD	bmiIndex[256];
};


extern "C" {
   //OWN_GR specifics
extern   unsigned char  *_gr_pScreen;
extern   unsigned char  *_gr_pOrigin;
extern   unsigned char  **_gr_pYCache;
extern   unsigned char *_gr_pHaze;
extern   SGRColorDef *_gr_pTransparency;
extern   int _gr_nTranspCount;
extern   unsigned char *_gr_pGouraud;
extern   long          _gr_pDiserTable[64*64];
//extern   unsigned char _gr_BumpMixColorTable[65536];
//extern   unsigned char _gr_BumpMulTable[65536];

extern   int   _gr_nScreenWidth;
extern   int   _gr_nScreenHeight;
extern   int   _gr_nScreenOriginX;
extern   int   _gr_nScreenOriginY;
extern   CRect2 _gr_clipRect;
extern   SGRViewport *_Viewport;

extern   float _gr_fFrontClip;

extern   volatile int   _gr_bRestoreSurf;

extern   HWND _gr_hWnd;
extern   HDC _gr_hDC;
extern   HPALETTE    _gr_hPal;
extern   GR_BITMAPINFO   _gr_DIBInfo;
extern   struct GR_LOGPALETTE {
      WORD palVersion;
      WORD palNumEntries;
      PALETTEENTRY palPalEntry[256];
   } _gr_logPal;
extern   POINTS  _gr_windowPos;
}



extern "C" extern UGRVertex _gr_vertices[GR_MAX_VERTEX];
extern "C" extern SGRPolygon _gr_polygon;

extern "C" extern SGRLight  _gr_pLights[LIGHT_SOURCE_COUNT];


/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************             Font              ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/


#define FONT_ID "FIXF"

class CFixedColorFont {
	private:
        struct  {
           char id[sizeof(FONT_ID) - 1];
           long nFntWidth;
           long nFntHeight;
           unsigned char  pPal[256*3];
           long pSmFntTable[256*2]; //sx,offset
           long nFntSprSize;
        }fontHeader;

        unsigned char  *pOriginalFntSpr;
        unsigned char  *pSwFntSpr;
        unsigned short *pHwFntSpr;

	public:
        CFixedColorFont() {pOriginalFntSpr = NULL;}
        CFixedColorFont(const char *filename) {Read(filename);}
        ~CFixedColorFont() {delete [] pOriginalFntSpr;
                       delete [] pSwFntSpr;
                       delete [] pHwFntSpr;}

        int Read(const char *filename);
        int ReadFromMemory(void *buf);
        int RecreateFont();


        int PrintAt(long x, long y, const char *str);
        int PrintColorAt(long x, long y,const char * str, unsigned long color);
        int PrintClipAt(long x, long y,const char *str);
        int PrintClipColorAt(long x, long y,const char * str, unsigned long color);


        int LUPrintAt(long x, long y, const char *str)
        {
           return PrintAt(x-_gr_nScreenOriginX,y-_gr_nScreenOriginY,str);
        }
        int LUPrintColorAt(long x, long y,const char * str, unsigned long color)
        {
           return PrintColorAt(x-_gr_nScreenOriginX,y-_gr_nScreenOriginY,str,color);
        }
        int LUPrintClipAt(long x, long y,const char *str)
        {
           return PrintClipAt(x-_gr_nScreenOriginX,y-_gr_nScreenOriginY,str);
        }
        int LUPrintClipColorAt(long x, long y,const char * str, unsigned long color)
        {
           return PrintClipColorAt(x-_gr_nScreenOriginX,y-_gr_nScreenOriginY,str,color);
        }

        long StringWidth(TCchar * string) const
        {  
		   long l = 0;
		   for(long i=0;string[i] != 0;l += fontHeader.pSmFntTable[(int)string[i++]<<1]) ;
		   return l;
		}
        int CharWidth(char ch) const { return fontHeader.pSmFntTable[((int)ch)<<1];}
        int Height() const {return fontHeader.nFntHeight;}
};


/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************           CImage              ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/

class CGRImage {
    private:
        int imageW;
        int imageH;
        void *image;

        int usePalette;
        unsigned char  palToScr8[256];
        unsigned short palToScr16[256];

    public:
        CGRImage() {image = NULL; usePalette = 0;}
        CGRImage(const char *bmpFile) {image = NULL; usePalette = 0; LoadFromBMPFile(bmpFile,0,0);}
        CGRImage(int w, int h);
        ~CGRImage() { delete [] image; image = NULL; usePalette = 0;}

        void Delete() {delete [] image; image = NULL; usePalette = 0;}


        int Height() const {return imageH;}
        int Width() const {return imageW;}

        void SetWidthHeight(int w, int h);

        void SetPalette(unsigned char *pal8);

        int LoadFromSPRFile(const char *sprName, const unsigned char *pal8,
                            int x, int y);

        int LoadFromBMPFile(const char *bmpName, int x, int y);

        void LoadPalImage(unsigned char *mem,
                          int x, int y,
                          int x0, int y0, int x1, int y1, int iW);
        void LoadRGBImage(unsigned long *mem,
                          int x, int y,
                          int x0, int y0, int x1, int y1, int iW);
        void LoadFromImage(CGRImage &image,
                           int x, int y,
                           int x0, int y0, int x1, int y1);
        void LoadFromScreen(int x, int y,
                            int x0, int y0, int x1, int y1);

        int Pset(int x, int y, int r, int g, int b);
        int Pset(int x, int y, unsigned long color); //GRFillColor
        int Pset(int x, int y, unsigned char color);

        int  Draw(int xs, int ys) const;
        int  DrawX2(int xs, int ys) const;
        int  Draw(int xs, int ys, int x0, int y0, int x1, int y1) const;
        int  DrawSprite(int xs, int ys) const;
        int  DrawSprite(int xs, int ys, int x0, int y0, int x1, int y1) const;
};

/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************           2D Graphics         ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/


int    GREnable2D();
void   GRPset(int x0, int y0, unsigned long color);
void   GRLine(int x0, int y0, int x1, int y1, unsigned long color); //GRFillColor
void   GRRect(int x0, int y0, int x1, int y1, unsigned long color);
void   GRBar(int x0, int y0, int x1, int y1, unsigned long color);
void   GRCircle(int x0, int y0, int r, unsigned long color, int fill);
int    GRDisable2D();

/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************           Panel               ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/

#define MAX_PANEL_RESOLUTIONS 4
#define MAX_PANEL_CONTROLS    4

#define CTRL_DIGITS_MAX_COUNT 3

class CGRPanel {
    private:
       typedef struct {
          int  numPoints;
          UGRVertex  point[1];
       } SHwPanelArea;

       typedef struct {
          void       *hTexture;
          int        numAreas;
          SHwPanelArea *area[1];
       } SHwPanelTexture;

       typedef struct {
          int             textureCount;
          SHwPanelTexture *texture[1];
       } SHwPanel;

       typedef struct {
          int           xStart,
                        yStart;
          unsigned char pPanel[1];
       } SSwPanel;

       enum {
          PANEL_CTRL_ARROW         = 0x100,
          PANEL_CTRL_INDICATOR     = 0x200,
          PANEL_CTRL_MOVE_SPRITE   = 0x300,
             CTRL_MOVE_SPRITE_ARC  = 0x301,
             CTRL_MOVE_SPRITE_LINE = 0x302,
             CTRL_MOVE_SPRITE_BOX  = 0x303,
          PANEL_CTRL_FILL_SPRITE   = 0x400,
             CTRL_FILL_SPRITE_LR   = 0x401,
             CTRL_FILL_SPRITE_RL   = 0x402,
             CTRL_FILL_SPRITE_TB   = 0x403,
             CTRL_FILL_SPRITE_BT   = 0x404,
          PANEL_CTRL_DIGITS        = 0x500,
             //CTRL_DIGITS_MAX_COUNT = 3,
       };

       typedef struct {
          char name[40];
          int  type;

          union {
             float fValue;
             int   iValue;
             char  sValue[CTRL_DIGITS_MAX_COUNT+1];
          } controlData;

          union {
             struct  {
                int x,y;
                float r, rTrsp;
                float alpha0, alpha1;
                unsigned long color;
             } arrow;

             struct  {
                int x,y;
                float r;
                float alpha0, alpha1;
                unsigned long color0, color1;
             } indicator;

             struct  {
               int type;
               int x,y;
               float r;
               float alpha0,alpha1;
               int x1,y1;  //dx,dy for box
               int u,v,
                   u1,v1;
               CGRImage *sprite;
             } moveSprite;

             struct  {
                int type;
                int x, y;
                CGRImage *sprite;
             } fillSprite;

             struct  {
                int digitsCount;
                int x[CTRL_DIGITS_MAX_COUNT];
                int y[CTRL_DIGITS_MAX_COUNT];
                CFixedColorFont *font;
             } digits;
          };
       } SControl;

       typedef struct {
          int         width,
                      height;
          SGRViewport *panelViewport;

          int         open;

          //-----------Software------------
          SSwPanel    *pSWPanel;
          //-----------Hardware------------
          SHwPanel    *pHWPanel;

          //-----------Controls------------
          int         controlCount;
          SControl    control[MAX_PANEL_CONTROLS];
       } SPanel;

       int      resolCount;
       SPanel   panel[MAX_PANEL_RESOLUTIONS];

       SPanel   *currentPanel;
       //int    currentFlag;
       int      drawCrosshair;
       int      drawPanel;
       CGRImage crosshair;

       void DrawSector(SControl *ct, float a0, float a1, unsigned long color);
       void DrawControlsFirst();
       void DrawControlsLast();


    public:
       CGRPanel(const char *filename);
       ~CGRPanel();

       void SetResolution(int width, int height);
       SGRViewport * Open();
       void Draw();
       void Close();

       void EnableDrawCrosshair(int draw) {drawCrosshair = draw;}
       void EnableDrawPanel(int draw) {drawPanel = draw;}
       int  IsDigitControl(const char *name);
       void SetControlValue(char *name, unsigned long value);

       void ReloadTextures(const char *filename, GR_HTEXTURE hT);

};

/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************        Graph Functions        ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/


SDeviceList * GRBuildDevicesList();
/* Function return pointer to a list of all devices found on computer.
 * Note: Function must call before using GRD3DInit function.
 */

int GRInitDevice(SDeviceDescr *dD, int mode, int fullScreen);
/* Function initialized DD, D3D, _ZBuffer and etc.
 * Parameters:
 *   dD - Pointer to device in device list.
 *   width, height, bpp - Video mode description
 *   swHw - Software or Hardware Renderer.
 * Return TRUE on success.
 */

void GRReInitTextureDB();
int GRChangeResolution(int vMode, int fullScreen);
int GRSetPalette(const unsigned char *pal8, int setScr = TRUE);
unsigned long GRFillColor(int r, int g, int b);
unsigned long GRCreateColor(int r, int g, int b);
unsigned long GRTransparentColor(int r, int g, int b);
int GRClearScreen(BOOL fClr = FALSE, long fColor = 0);
int GRDumpScreen();
void GRInitBump();
int GREndScene();
int GRStartScene();
void GRTerminateDevice();
void GRZBufferEnable(int enable);
int GRSetHaze(int start, int len, SGRColorDef *hd);
void * GRLoadTextureToDBFromFile(char *filename, int flags);

extern void GRPreLoadTextures();
extern void GRRestoreSurfaces();
extern void GRSetTextureLoadFunc( GR_HTEXTURE handle, const char *fileName, TTextureLoadFunc loadF, void *user = NULL);
extern void GRSetTextureLoadIntFunc( GR_HTEXTURE handle, TTextureLoadIntFunc loadF);

void  GRSetPaletteTables(SGRColorDef *pTransparency, int nTranspCount, TCbyte *pGouraud);

int GRIsHardware();

int GRInitParticle(GR_HTEXTURE);
int GRInitRay(GR_HTEXTURE);
extern "C" void GRDrawRay(int x0, int y0, int x1, int y1, unsigned long color,int opacity, int iz, float width2, float center);


extern "C" void GRLUDrawArrow(int *points, int numPoint, float r0, float r1,
                            unsigned long color);




/***********************************************************************
 ***********************************************************************
 ********************                               ********************
 ********************  Pointers to Graph Functions  ********************
 ********************                               ********************
 ***********************************************************************
 ***********************************************************************/


GLOBAL int  (*_pGRDrawPolygonPCCW)(void);
GLOBAL void (*_pGRDrawSprite)(int x0, int y0, int x1, int y1, int u0, int v0, int u1, int v1, int iz, void *hTex);
//GLOBAL int  (*_pGRInitTables)(unsigned long);
GLOBAL void (*_pGRSetClipRect)(void);
//GLOBAL int   (*_pGRSetPalette)(unsigned char*);
GLOBAL int  (*_pGRSetHaze)(int, int, SGRColorDef *pH);
//GLOBAL void (*_pGRInitTextureDB)(void);
GLOBAL void* (*_pGRLoadTextureToDB)(void* handle, unsigned char *pal, int palCnt, unsigned char *text);
GLOBAL void (*_pGRDeleteTextureFromDB)(void*);
GLOBAL void (*_pGRSetZPrecision)(int);
GLOBAL void (*_pGRSetBump)(int,int);
GLOBAL void (*_pGRDrawParticle)(int x, int y, int size, int iz, unsigned long color);
GLOBAL void (*_pGRDrawAlphaSprite)(SGRAlphaSprite *sm);
//GLOBAL void (*_pGRCalcYDivCache)(void);

#define GRDrawPolygonPCCW() (_pGRDrawPolygonPCCW)()
#define GRDrawSprite(x0, y0, x1, y1, u0, v0, u1, v1, iz, hTex) (_pGRDrawSprite)(x0, y0, x1, y1, u0, v0, u1, v1, iz, hTex)
//#define GRInitTables(a)     (_pGRInitTables)(a)
#define GRSetClipRect()    (_pGRSetClipRect)()
//#define GRSetPalette(pal)   (_pGRSetPalette)(pal)
#define _GRSetHaze(s, e, h)     (_pGRSetHaze)(s, e, h)
//#define GRInitTextureDB()   (_pGRInitTextureDB)()
#define GRLoadTextureToDB(h, p, pc, t) (_pGRLoadTextureToDB)(h, p, pc, t)
#define GRDeleteTextureFromDB(h) (_pGRDeleteTextureFromDB)(h)
#define GRSetZPrecision(p)  (_pGRSetZPrecision)(p)
#define GRSetBump(s,e) (_pGRSetBump)(s,e)
#define GRDrawParticle(x, y, size, iz, color)  (_pGRDrawParticle)(x, y, size, iz, color)
#define GRDrawAlphaSprite(sm) (_pGRDrawAlphaSprite)(sm)
#undef GLOBAL

#endif