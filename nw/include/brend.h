#ifndef __BREND_H__
#define __BREND_H__


#if 0
#include "filesys.h"
#include "mathlib.h"



#include "stdtypes.h"

#include "debugext.h"
#include "filesys.h"
#include "mathlib.h"

#include "graph.h"

#include "patterns.h"
#include "view.h"
#include "land.h"    // Will be obsolete
                    // as new land is created

#endif
//#include "zav.h"

const int r_MAX_CACHE_VERTEX    = 300;
const int r_MAX_CACHE_WIDTH     = 300;
const int r_MAX_CACHE_LEADING   = 300;
const int r_MAX_NODE            = 600;
const int r_MAX_RECT_SIZE       = 40; // максимальный размер скомпилированной р.
const int r_RECT_MINUS          = 4;  // запас отрицательной области
#define r_MIN_VIEW_DIST 14.0
       // расстояние, с которого проверяется на
       // полную видимость

/*#define r_ORD_VIEW_POINT  mm_viewPoint
#define r_POINT_DIR_MATR  mm_dirMatr
#define r_FRONT_CLIP      mm_frontClip
#define r_FRONT_CLIP_DEN  mm_frontClipDen
#define r_PROJ_DIST       mm_projDist
#define r_CLIP2D          mm_clip2D
#define r_YCACHE          mm_yCache
#define r_VSCR_WIDTH      640
#define r_PALETTE6        mm_palette*/

//extern CFMatrix3x4 mm_dirMatrx;

#define r_ORD_VIEW_POINT  (CViewOrder::GetOrderViewPoint())
//#define r_POINT_DIR_MATR  (*CViewObjectBaseSet::CurrDir())
#define r_POINT_DIR_MATR  (mm_dirMatrx)
#define r_FRONT_CLIP      (CViewObject::m_fFrontClip)
#define r_FRONT_CLIP_DEN  (1. / CViewObject::m_fFrontClip)
#define r_PROJ_DIST       (CViewObject::m_viewPointScale.x)
#define r_CLIP2D          (_gr_clipRect)
#define r_YCACHE          (_gr_pYCache)
#define r_VSCR_WIDTH      (_gr_nScreenWidth)
#define r_PALETTE6        (CPaletteTranslator::Palette64())


#define r_DIST_ORTOGONAL  90
#define r_DIST_AS_SPRITE  200

inline void SUA_D2INT(int *p,double d) { *p=d; }
/*#pragma aux SUA_D2INT = \
	"fistp	dword ptr [eax]"	parm [eax] [8087]*/

#define D2LNG(x,y) SUA_D2INT(((int*)(x)),(y))

typedef void (*bsh_TSingleLineFunc)(int x0,int y0, int x1, int y1);
typedef void (*bsh_TOutComileRectFunc)(byte *dest,int scrWidth);

class b_TCacheVert;
class b_TCacheWidt;
class b_TBushRect;

class bsh_TCodeStream
 {
 public:
    byte  *m_stream;
    int    m_size;
    int    m_pos;

    bsh_TCodeStream()
    {
       m_stream = NULL;
       m_size   = 0;
       m_pos    = 0;
    }
    bsh_TCodeStream( const bsh_TCodeStream &sour )
    {
       int i;

       m_stream = new byte[sour.m_pos];
       m_size = sour.m_pos;
       m_pos  = sour.m_pos;

       for(i=0; i<sour.m_pos; ++i) m_stream[i] = sour.m_stream[i];
    }
    bsh_TCodeStream &operator = ( const bsh_TCodeStream &sour )
    {
       int i;
       delete m_stream;
       m_stream = new byte[sour.m_pos];
       m_size = sour.m_pos;
       m_pos  = sour.m_pos;

       for(i=0; i<sour.m_pos; ++i) m_stream[i] = sour.m_stream[i];
       return *this;
    }

    void Create( int size )
    {
       delete m_stream;
       m_stream = new byte[size];
       m_size = size;
       m_pos  = 0;
    }

    ~bsh_TCodeStream()
    {
       delete m_stream;
    }
 };


class b_TSprite
 {
 public:
    int   x0, y0, x1, y1;
    double w;
    double cacheW;
 };

class b_TVector3D_i
 {
 public:
    int  x,y,z;
 };

class b_TVert
 {
 public:
    CFVector3     f;
    b_TVector3D_i i;
 };

class b_TWidt
 {
 public:
    b_TCacheWidt *m_cache;
    double        f;
    int           i;
 };

class b_TBranch
 {
 public:
    b_TVert      *m_vert0;
    b_TCacheVert *m_vertCh0;

    b_TWidt      *m_widt0;
    b_TCacheWidt *m_widtCh0;

    //....................

    b_TVert      *m_vert1;
    b_TCacheVert *m_vertCh1;

    b_TWidt      *m_widt1;
    b_TCacheWidt *m_widtCh1;
 };

 #define B_LEFTLINKED  1
 #define B_RIGHTLINKED 2


class b_TNode
 {
 public:
    int       m_type;
    int       m_flag;
    double    m_plane;

    int       m_cnt;
    b_TBranch*m_chain;

    b_TSprite m_spr;
    b_TNode  *m_left,
             *m_right;

    int      m_linkChain;
    int      m_nextNodeLinked;          // 0 - not linked, 1 - left linked, 2 - right linked

    CVector2 m_linkCache;
    CVector2 m_linkCacheUpper;
    CVector2 m_linkCacheLower;
    int      m_linkCacheValid;


    void      MarkLink();
     b_TNode();
    ~b_TNode();
 };

class b_TCacheVert
 {
 public:
    CFVector3 p;
    CVector2  s;
    int       m_isCalcScr;
    int       m_isNoFrontClip;
 };

class b_TCacheWidt
 {
 public:
    int       s;
    int       m_calc;
 };

class b_TCacheBush
 {
 public:
    b_TCacheVert *m_vert;
    b_TCacheWidt *m_widt;
    int           m_vertCnt;
    int           m_lastVert;
    int           m_widtCnt;
    int           m_lastWidt;
    b_TCacheBush()
    {
       m_vert = NULL;
       m_widt = NULL;
       m_vertCnt  = 0;
       m_lastVert = 0;
       m_widtCnt  = 0;
       m_lastWidt = 0;
    }
 };

class b_TBush //: public CViewOrdered
 {
 protected:
    CFVector3  m_optCenter;
    double     m_optRadius;

	//virtual VORDTYPE Type() const { return VOT_BUSH; }

 public:
    b_TVert   *m_vert;
    b_TWidt   *m_widt;
    int        m_vertCnt;
    int        m_widtCnt;
    b_TNode   *m_node;
    double     m_maxWidth;

    static b_TCacheBush  m_cache;
    static b_TBushRect   m_bushRect;
    static byte          m_singleCol;
	static int			 m_hSingleCol;
    static double        m_widtAspect;
    static double        m_vertAspect;
    static bsh_TSingleLineFunc SingleLineLRH;
    static bsh_TSingleLineFunc SingleLineRLH;
    static bsh_TSingleLineFunc SingleLineLRVDU;
    static bsh_TSingleLineFunc SingleLineRLVDU;
    static bsh_TOutComileRectFunc CompileRectAr[r_MAX_RECT_SIZE+r_RECT_MINUS];
    static bsh_TCodeStream         m_compileRectBody;
    static int                     m_visAll;

    CFVector3  m_center;
    double     m_radius;

    double     Radius0() const { return m_radius; }
    double     Radius()  const { return m_optRadius; }
    CFVector3  Center()  const { return m_optCenter; }

     b_TBush();
    ~b_TBush();
    void Read( CTaggedFile &f );
    void Draw();
	//virtual	bool Bump(SBumpDef0 &def) { (void)def; return FALSE; }
	//virtual	double ComputeBSPError() { return 1e10; }
	//virtual double ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d) { (void)pt; (void)n; (void)d; return 1e10; }
 };


class b_PinLeadingPoint {
public:
    /*int m_TextureIndex;
    int m_UpperTextureCoord;*/

    CVector2 m_LowerPoint;
    CVector2 m_UpperPoint;
    CVector2 m_LowerPoint2;
    CVector2 m_UpperPoint2;

    int m_Length;
    int m_Valid;
    int m_endClip;

    enum IntersectionType {IT_UPPER, IT_LOWER, IT_NONE};
    IntersectionType m_IntersectionType;
};

class b_TBushRect
 {
 public:
    int   m_size;
    byte *data0;
    byte *data1;
    byte *data2;
    byte *data3;

     b_TBushRect()
     {
        m_size = 0;
        data0 = NULL;
        data1 = NULL;
        data2 = NULL;
        data3 = NULL;
     }
    ~b_TBushRect()
     {
        delete data0;
        delete data1;
        delete data2;
        delete data3;
     }
 };


int b_LoadBushTrunk();
void b_CreateCacheBush();
void b_DeleteCacheBush();
void bsh_PutLineRL(int x0,int y0, int x1, int y1, int w0, int w1 );
void bsh_PutLineLR(int x0,int y0, int x1, int y1, int w0, int w1 );
void bsh_PutRect( int xcoord, int ycoord, int sq_size );

void bsh_SingleLineLRH_nclp(int x0,int y0, int x1, int y1);
void bsh_SingleLineLRH_clip(int x0,int y0, int x1, int y1);
void bsh_SingleLineRLH_nclp(int x0,int y0, int x1, int y1);
void bsh_SingleLineRLH_clip(int x0,int y0, int x1, int y1);


void bsh_SingleLineLRVDU_clip( int x0,int y0, int x1, int y1 );
void bsh_SingleLineLRVDU_nclp( int x0,int y0, int x1, int y1 );
void bsh_SingleLineRLVDU_clip( int x0,int y0, int x1, int y1 );
void bsh_SingleLineRLVDU_nclp( int x0,int y0, int x1, int y1 );

void bsh_SingleLineLRVUD_clip( int x0,int y0, int x1, int y1 );
void bsh_SingleLineRLVUD_clip( int x0,int y0, int x1, int y1 );

void bsh_CompileRectLODs( bsh_TCodeStream &s );

extern b_TBush      g_bush;
extern CViewTexture g_bushTexture;
extern CViewTexture b_Leaves;


extern CRect2  mm_clip2D;
extern byte    mm_palette[768];
extern byte  **mm_yCache;
extern CRect2  mm_clip2D;
extern double  mm_frontClipDen;

#endif
/* End of BREND.H */
