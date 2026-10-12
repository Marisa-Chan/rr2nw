#ifndef	__BUSH_CPP__
#include"_bush.h"
#endif

#include <stdlib.h>
//#include <string.h>
//#include "brend.h"

#define SCRW r_VSCR_WIDTH
#define SCRH 480



#define b_MATR_PRES_POW 8
#define b_MATR_PRES     (1<<b_MATR_PRES_POW)
#define b_STYLE_INT_VERT 1
#define b_STYLE_INT_WIDT 0

double b_cacheZ;
double b_cache1Z;
int    b_useOrtogonal; // Флаг для спрайтов

b_PinLeadingPoint b_LeadingCache[r_MAX_CACHE_LEADING];

typedef void (*r_TDrawBranchFunc)( int step, b_TBranch &b );

b_TCacheBush b_TBush::m_cache;
b_TBushRect  b_TBush::m_bushRect;
byte         b_TBush::m_singleCol;
int			 b_TBush::m_hSingleCol;
double       b_TBush::m_widtAspect;
double       b_TBush::m_vertAspect;
bsh_TSingleLineFunc b_TBush::SingleLineLRH;
bsh_TSingleLineFunc b_TBush::SingleLineRLH;
bsh_TSingleLineFunc b_TBush::SingleLineLRVDU;
bsh_TSingleLineFunc b_TBush::SingleLineRLVDU;
bsh_TCodeStream     b_TBush::m_compileRectBody;
bsh_TOutComileRectFunc b_TBush::CompileRectAr[r_MAX_RECT_SIZE+r_RECT_MINUS];
int                    b_TBush::m_visAll;
b_TNode::b_TNode()
 {
    m_type = -1;
    m_flag = -1;
    m_plane=  0;

    m_cnt  =  0;
    m_chain= NULL;

    m_left = NULL;
    m_right= NULL;
 }

b_TNode::~b_TNode()
 {
    delete m_chain;
    delete m_left;
    delete m_right;
 }

b_TBush::b_TBush()
 {
    m_vert = NULL;
    m_widt = NULL;
    m_vertCnt = 0;
    m_widtCnt = 0;
    m_node = NULL;
    m_center = CFVector3(0,0,0);
    m_radius = 0;

    SingleLineLRH = bsh_SingleLineLRH_clip;
    SingleLineLRH = bsh_SingleLineRLH_clip;

    SingleLineLRVDU = bsh_SingleLineLRVDU_clip;
 }

b_TBush::~b_TBush()
 {
    delete m_vert;
    delete m_widt;
    delete m_node;
 }

void b_ReadSpr( b_TSprite &spr, CTaggedFile &f )
 {
    f.Descend( "Spr_", TRUE, 0 );
       f.ReadDouble( spr.w );
       f.ReadInt( spr.x0 );
       f.ReadInt( spr.y0 );
       f.ReadInt( spr.x1 );
       f.ReadInt( spr.y1 );
    f.Ascend();

    int num = 1+rand()%2;
    int x = num % 2;
    int y = num / 2;
    int width =  (256/2);

    spr.x0 = (x*width+2)<<16;
    spr.y0 = (y*width+2)<<16;
    spr.x1 = (x*width+width-2)<<16;
    spr.y1 = (y*width+width-2)<<16;
    //spr.w += spr.w*((double)(rand()))/RAND_MAX;
 }

#pragma pack(0)
typedef struct {
    byte r,g,b;
} b_RGB;
extern int  g_MidleR, g_MidleG, g_MidleB;

int b_LoadBushTrunk()
 {
    word  *data;
    int    cnt;
    int    width,height,t;
    b_RGB  lpal[2048];
    int    i,j;
    byte   xlat[2048];

    CTaggedFile fs(TRUE);
    fs.Open("B_TRUNK.TXR");
	 fs.Descend("TXR_",FALSE,0);
	   fs.Descend("TXRH",TRUE,0);
	     fs.ReadInt(t);
	     fs.ReadInt(width);
	     fs.ReadInt(height);
         if( width!=height )
         {
              ASSERT(0);
              return 0;
         }
	   fs.Ascend();

	   fs.Descend("PAL_",TRUE,1);
	     fs.ReadInt(cnt);
	     fs.ReadInt(t);
	     fs.Read(lpal,cnt*3);

         for( i=0; i<cnt; ++i )
         {
              lpal[i].r >>= 2;
              lpal[i].g >>= 2;
              lpal[i].b >>= 2;
              double min=1e10,dif;

              for( j = 0; j <256; ++j)
              {
                   double dr= ((int)r_PALETTE6[j*3  ])-(int)lpal[i].r;
                   double dg= ((int)r_PALETTE6[j*3+1])-(int)lpal[i].g;
                   double db= ((int)r_PALETTE6[j*3+2])-(int)lpal[i].b;
                   dif = dr*dr + dg*dg + db*db;
                   if( dif<min )
                   {
                        min = dif;
                        xlat[i] = (byte)(j);
                   }
              }
         }

	   fs.Ascend();

	   fs.Descend("TXRD",TRUE,0);
	     data = new word[width*height];
	     fs.Read( data, width*height*2 );
         b_TBush::m_bushRect.data0 = new byte[width];
         b_TBush::m_bushRect.data1 = new byte[width];
         b_TBush::m_bushRect.data2 = new byte[width];
         b_TBush::m_bushRect.data3 = new byte[width];
	   fs.Ascend();
	 fs.Ascend();
    fs.Close();

    for( i = 0; i<width; ++i )
    {
         b_TBush::m_bushRect.data0[i] = xlat[data[i]];
         b_TBush::m_bushRect.data1[i] = xlat[data[i*width+width-1]];
         b_TBush::m_bushRect.data2[i] = xlat[data[i+width*(width-1)]];
         b_TBush::m_bushRect.data3[i] = xlat[data[i*width]];
    }

    b_TBush::m_bushRect.m_size = width<<16;
    b_TBush::m_singleCol = b_TBush::m_bushRect.data2[width>>1];

    b_TBush::m_hSingleCol = GRCreateColor(int(g_MidleR),int(g_MidleG),int(g_MidleB));
    bsh_CompileRectLODs( b_TBush::m_compileRectBody );

    delete [] data;
	return 1;
 }

void b_CreateCacheBush()
 {
    b_TCacheBush *cache = &(b_TBush::m_cache);

    cache->m_vert    = new b_TCacheVert[ r_MAX_CACHE_VERTEX ];
    cache->m_vertCnt = r_MAX_CACHE_VERTEX;

    cache->m_widt    = new b_TCacheWidt[ r_MAX_CACHE_WIDTH ];
    cache->m_widtCnt = r_MAX_CACHE_WIDTH;
 }

void b_DeleteCacheBush()
 {
    b_TCacheBush *cache = &(b_TBush::m_cache);

    delete [] cache->m_vert;
    cache->m_vert     = 0;
    cache->m_vertCnt  = 0;

    delete [] cache->m_widt;
    cache->m_widt    = 0;
    cache->m_widtCnt = 0;
 }

b_TNode *b_ReadORDR( b_TBush &bush, CTaggedFile &f )
 {
    b_TNode *node = new b_TNode();
    int  i;
    b_TCacheBush &cache = bush.m_cache;

    f.Descend( "ORDR", FALSE, 0 );
        f.Descend( "Info", TRUE, 0 );
            f.ReadInt( node->m_cnt  );
            f.ReadInt( node->m_flag );
            f.ReadInt( node->m_type );
            f.ReadDouble( node->m_plane );


            node->m_chain = new b_TBranch[ node->m_cnt ];

            for( i = 0; i < node->m_cnt; ++i )
            {
                 int pNum, wNum;

                 f.ReadInt(pNum);  ASSERT(pNum<r_MAX_CACHE_VERTEX);
                 f.ReadInt(wNum);  ASSERT(wNum<r_MAX_CACHE_WIDTH);
                 node->m_chain[i].m_vert0   = &(bush.m_vert[pNum]);

                 node->m_chain[i].m_vertCh0 = &(cache.m_vert[pNum]);
                 node->m_chain[i].m_widt0   = &(bush.m_widt [wNum]);
                 node->m_chain[i].m_widtCh0 = &(cache.m_widt[wNum]);

                 f.ReadInt(pNum);  ASSERT(pNum<r_MAX_CACHE_VERTEX);
                 f.ReadInt(wNum);  ASSERT(wNum<r_MAX_CACHE_WIDTH);
                 node->m_chain[i].m_vert1   = &(bush.m_vert[pNum]);

                 node->m_chain[i].m_vertCh1 = &(cache.m_vert[pNum]);
                 node->m_chain[i].m_widt1   = &(bush.m_widt [wNum]);
                 node->m_chain[i].m_widtCh1 = &(cache.m_widt[wNum]);
            }
        f.Ascend();

        switch( node->m_flag )
        {
        case 1: node->m_left  = b_ReadORDR(bush, f); break;
        case 2: node->m_right = b_ReadORDR(bush, f); break;
        case 3:
                node->m_left  = b_ReadORDR(bush, f);
                node->m_right = b_ReadORDR(bush, f);
                break;
        case 4: b_ReadSpr( node->m_spr, f );
        case 0: break;
        default: ASSERT(0);
        }
        node->m_flag += node->m_type*5;
        RTCHECK(node->m_flag<10,"r_ReadORDR(): Bad flag");
    f.Ascend();

    return node;
 }

void b_TBush::Read( CTaggedFile &f )
 {
    int i, cnt;

    f.Descend( "BUSH", FALSE, 0 );
       f.Descend( "SIZE",TRUE,0 );
         f.ReadInt( cnt );
         f.ReadInt( m_vertCnt );
         f.ReadInt( m_widtCnt );
       f.Ascend();

       f.Descend( "OTHR", TRUE, 0 );
         f.ReadDouble(m_center.x);
         f.ReadDouble(m_center.y);
         f.ReadDouble(m_center.z);
         f.ReadDouble(m_radius);
         m_optCenter.Read(f);
         f.ReadDouble(m_optRadius);
       f.Ascend();

       m_vert = new b_TVert  [ m_vertCnt ];
       m_widt = new b_TWidt  [ m_widtCnt ];

       f.Descend( "Vert", TRUE, 0);
          for( i = 0; i < m_vertCnt; ++i )
          {
               m_vert[i].f.Read(f);
               m_vert[i].i.x = (int)(m_vert[i].f.x * b_MATR_PRES);
               m_vert[i].i.y = (int)(m_vert[i].f.y * b_MATR_PRES);
               m_vert[i].i.z = (int)(m_vert[i].f.z * b_MATR_PRES);
          }
       f.Ascend();

       f.Descend( "Widt", TRUE, 0 );
          m_maxWidth = -10;
          for( i = 0; i< m_widtCnt; ++i )
          {
               f.ReadDouble( m_widt[i].f );
               m_widt[i].i = (int)(m_widt[i].f * b_MATR_PRES);
               m_widt[i].m_cache = &(m_cache.m_widt[i]);
               if( m_widt[i].f>m_maxWidth ) m_maxWidth = m_widt[i].f;
          }
       f.Ascend();

       m_node = b_ReadORDR(*this,f);
    f.Ascend();

//    m_vert[0].f.y += m_widt[0].f/2;
//    m_vert[0].i.y += m_widt[0].i/2;
    m_radius    += m_widt[0].f;
    m_optRadius += m_widt[0].f;

    m_node->m_linkChain = 0;
    m_node->MarkLink();

 }

void b_TNode::MarkLink()
{
    //FIXME ASSERT(CNT!=0)

   b_TBranch &last = m_chain[m_cnt-1];
   m_nextNodeLinked = 0;

    if(  m_left != NULL  )
    {
         b_TBranch &first = m_left->m_chain[0];
         if(  first.m_vert0 == last.m_vert1)
         {
              m_left->m_linkChain = 1;
              m_nextNodeLinked |= B_LEFTLINKED;
         }
         else
              m_left->m_linkChain = 0;
         m_left->MarkLink();

    }

    if(  m_right != NULL  )
    {
         b_TBranch &first = m_right->m_chain[0];
         if(  first.m_vert0 == last.m_vert1)
         {
            m_right->m_linkChain = 1;
            m_nextNodeLinked |= B_RIGHTLINKED;

         }
         else
            m_right->m_linkChain = 0;
         m_right->MarkLink();
    }
}

typedef long gr_WorldCoordType;
typedef double gr_SMulType;

//typedef
enum gr_SEG_INTERSECT { GR_SEG_INTERSECT_NONE_FORWARD,
                        GR_SEG_INTERSECT_NONE_BACK,
                        GR_SEG_INTERSECT_YES };

inline int gr_Sign(gr_SMulType a)
{
    if(a<0) return -1;
    if(a>0) return  1;
    return 0;
}


gr_SEG_INTERSECT gr_SplitSeg(gr_WorldCoordType Xa0,
							 gr_WorldCoordType Za0,

							 gr_WorldCoordType Xa1,
							 gr_WorldCoordType Za1,


							 gr_WorldCoordType Xb0,
							 gr_WorldCoordType Zb0,

							 gr_WorldCoordType Xb1,
							 gr_WorldCoordType Zb1,

							 gr_WorldCoordType &interX,
							 gr_WorldCoordType &interZ)
{
    gr_WorldCoordType Ax,Az;
    gr_WorldCoordType Nx,Nz; // Вектор нормали
    gr_WorldCoordType Fx,Fz; // Вектор к началу второго вектора
    gr_WorldCoordType Gx,Gz; // Вектор к концу  второго вектора
    gr_SMulType       f,g;
    int               sig_f,sig_g;


    Ax = Xa1 - Xa0;   // Направляющий вектор первого отрезка
    Az = Za1 - Za0;

    Fx = Xb0 - Xa0;
    Fz = Zb0 - Za0;

    Gx = Xb1 - Xa0;
    Gz = Zb1 - Za0;


    Nx = -Az;         // Вектор нормали первого отрезка
    Nz =  Ax;         // (Вектор направлен вверх)

    f = Fx*Nx + Fz*Nz;
    g = Gx*Nx + Gz*Nz;

    sig_f = gr_Sign(f);
    sig_g = gr_Sign(g);


    if( sig_f==sig_g )
    {
        //
        // Отрезок b либо лежит на одной прямой с a,
        // либо не пересекает ее
        //
        if( sig_f==0 )
            return GR_SEG_INTERSECT_NONE_FORWARD;

        if( sig_f==1 )
            return GR_SEG_INTERSECT_NONE_FORWARD;

        return GR_SEG_INTERSECT_NONE_BACK;
    }

    if( sig_f==0 )
    {
        if( sig_g>0 ) // Только одна точка(0) лежит на прямой
            return GR_SEG_INTERSECT_NONE_FORWARD;
        return GR_SEG_INTERSECT_NONE_BACK;
    }

    if( sig_g==0 )
    {
        if( sig_f>0 )  // Только одна точка(1) лежит на прямой
            return GR_SEG_INTERSECT_NONE_FORWARD;

        return GR_SEG_INTERSECT_NONE_BACK;
    }
    {//--------------------------------------------------------------
		gr_SMulType       fa,ga;
		gr_SMulType       len;
		gr_WorldCoordType lenA;

		fa = Fx*Ax + Fz*Az;
		ga = Gx*Ax + Gz*Az;

		len = fa + f*(ga-fa)/(f-g); // Деление на 0 невозможно, поскольку f и g
		// имеют разные знаки и неравны 0
		if( len<0)
		{
			//
			// Отрезок b прошел левее
			//
			if( Gz<0 )
				return GR_SEG_INTERSECT_NONE_BACK;
			return GR_SEG_INTERSECT_NONE_FORWARD;
		}

		lenA = Ax*Ax + Az*Az;


		if( len > lenA )
		{
			if( f<0 )
				return GR_SEG_INTERSECT_NONE_BACK;
			return GR_SEG_INTERSECT_NONE_FORWARD;
		}


		interX = (gr_WorldCoordType)(Xa0 + Ax*len/lenA);
		interZ = (gr_WorldCoordType)(Za0 + Az*len/lenA);
    }
    return GR_SEG_INTERSECT_YES;
}

unsigned short psi_sqrt(unsigned long v)
{
    register long t = 1L << 30, r = 0, s;

#define STEP(k) \
    s = t + r; \
    r >>= 1; \
    if (s <= v) { \
        v -= s; \
        r |= t; \
    }

    STEP(15); t >>= 2;
    STEP(14); t >>= 2;
    STEP(13); t >>= 2;
    STEP(12); t >>= 2;
    STEP(11); t >>= 2;
    STEP(10); t >>= 2;
    STEP(9); t >>= 2;
    STEP(8); t >>= 2;
    STEP(7); t >>= 2;
    STEP(6); t >>= 2;
    STEP(5); t >>= 2;
    STEP(4); t >>= 2;
    STEP(3); t >>= 2;
    STEP(2); t >>= 2;
    STEP(1); t >>= 2;
    STEP(0);

    return (unsigned short) r;
}



void pin_CalcLeadingCache(int index, int x0,int y0, int x1, int y1, int w0, int w1, int endclip)
{
    b_PinLeadingPoint & thePoint = b_LeadingCache[index];

    thePoint.m_endClip = endclip;

    int dx = x1 - x0;
    int dy = y1 - y0;

    if ( !dx && !dy)
    {
        thePoint.m_Valid = 0;
        return;
    }

    double ratio;

    thePoint.m_Length = psi_sqrt(dx * dx + dy * dy);

    if ( w0 )
    {
        ratio = (double) w0 / (thePoint.m_Length * 2);

        thePoint.m_UpperPoint.x = x0 + ( - dy * ratio);
        thePoint.m_UpperPoint.y = y0 + ( dx    * ratio);

        thePoint.m_LowerPoint.x = x0 + ( dy    * ratio);
        thePoint.m_LowerPoint.y = y0 + ( - dx * ratio);
    }
    else
    {
        thePoint.m_UpperPoint.x = x0;
        thePoint.m_UpperPoint.y = y0;

        thePoint.m_LowerPoint.x = x0;
        thePoint.m_LowerPoint.y = y0;
    }

    if ( w1 )
    {
        ratio = (double) w1 / (thePoint.m_Length * 2);

        thePoint.m_UpperPoint2.x = x1 + ( - dy * ratio);
        thePoint.m_UpperPoint2.y = y1 + ( dx    * ratio);

        thePoint.m_LowerPoint2.x = x1 + ( dy    * ratio);
        thePoint.m_LowerPoint2.y = y1 + ( - dx * ratio);
    }
    else
    {
        thePoint.m_UpperPoint2.x = x1;
        thePoint.m_UpperPoint2.y = y1;

        thePoint.m_LowerPoint2.x = x1;
        thePoint.m_LowerPoint2.y = y1;
    }

    thePoint.m_Valid = 1;
}


//extern CViewTexture theTexture;


void DrawLinkChainTxr(CVector2 p0,  CVector2 pUpper, CVector2 pLower)
{

    b_PinLeadingPoint & p = b_LeadingCache[0];

    /*if ( !p.m_Valid || p.m_endClip)
        return;*/

	GRSetZPrecision(0);

	int iz = 65536 / b_cacheZ;

    _gr_vertices[0].texture.u  = 126 << 16;
    _gr_vertices[0].texture.v  = 126 << 16;

    _gr_polygon.dwFullType = GR_POLY_TEXTURE_LIN;
    _gr_polygon.dwAddType  = 0;
    _gr_polygon.nVertices  = 3;
	_gr_polygon.nLights = 0;

    _gr_vertices[2].texture.x  = pLower.x;
    _gr_vertices[2].texture.y  = pLower.y;
    _gr_vertices[2].texture.iz = iz;
    _gr_vertices[2].texture.u  = 1;
    _gr_vertices[2].texture.v  = 1;


    _gr_vertices[1].texture.x  = p.m_LowerPoint.x;
    _gr_vertices[1].texture.y  = p.m_LowerPoint.y;
    _gr_vertices[1].texture.iz = iz;
    _gr_vertices[1].texture.u  = 1;
    _gr_vertices[1].texture.v  = 126 << 16;



    _gr_vertices[0].texture.x  = p0.x;
    _gr_vertices[0].texture.y  = p0.y;
    _gr_vertices[0].texture.iz = iz;
    _gr_vertices[0].texture.u  = 126 << 16;
    _gr_vertices[0].texture.v  = 126 << 16;


    GRDrawPolygonPCCW();

    _gr_vertices[0].texture.x  = pUpper.x;
    _gr_vertices[0].texture.y  = pUpper.y;
    _gr_vertices[0].texture.iz = iz;
    _gr_vertices[0].texture.u  = 1;
    _gr_vertices[0].texture.v  = 1;

    _gr_vertices[1].texture.x  = p.m_UpperPoint.x;
    _gr_vertices[1].texture.y  = p.m_UpperPoint.y;
    _gr_vertices[1].texture.iz = iz;
    _gr_vertices[1].texture.u  = 1;
    _gr_vertices[1].texture.v  = 126 << 16;


    _gr_vertices[2].texture.x  = p0.x;
    _gr_vertices[2].texture.y  = p0.y;
    _gr_vertices[2].texture.iz = iz;
    _gr_vertices[2].texture.u  = 126 << 16;
    _gr_vertices[2].texture.v  = 126 << 16;

    GRDrawPolygonPCCW();

}


void DrawBranchCacheTxr(int length)
{
	int iz = 65536 / b_cacheZ;
	GRSetZPrecision(0);

    for ( int i = 0; i < length ; i++)
    {

        b_PinLeadingPoint & p = b_LeadingCache[i];

        if (!p.m_Valid )
            continue;

        _gr_polygon.dwFullType = GR_POLY_TEXTURE_LIN;
        //_gr_polygon.dwFullType = GR_POLY_FLAT;
        _gr_polygon.dwAddType  = 0;
        _gr_polygon.nVertices  = 4;
		_gr_polygon.nLights = 0;

        //_gr_polygon.fill.flat.nColor = currentColor;

        _gr_polygon.hTexture = b_Leaves.HImage();
        //_gr_polygon.fill.texture.pTextureCache = theTexture.GetImageCache();

        _gr_vertices[3].texture.x  = p.m_LowerPoint.x;
        _gr_vertices[3].texture.y  = p.m_LowerPoint.y;
        _gr_vertices[3].texture.iz = iz;
        _gr_vertices[3].texture.u  = 1;
        _gr_vertices[3].texture.v  = 1;

        _gr_vertices[2].texture.x  = p.m_LowerPoint2.x;
        _gr_vertices[2].texture.y  = p.m_LowerPoint2.y;
        _gr_vertices[2].texture.iz = iz;
        _gr_vertices[2].texture.u  = 1;
        _gr_vertices[2].texture.v  = 126 << 16; //b.m_UpperTextureCoord << 16;


        _gr_vertices[1].texture.x  = p.m_UpperPoint2.x;
        _gr_vertices[1].texture.y  = p.m_UpperPoint2.y;
        _gr_vertices[1].texture.iz = iz;
        _gr_vertices[1].texture.u  = 126 << 16;
        _gr_vertices[1].texture.v  = 126 << 16; //b.m_UpperTextureCoord << 16;


        _gr_vertices[0].texture.x  = p.m_UpperPoint.x;
        _gr_vertices[0].texture.y  = p.m_UpperPoint.y;
        _gr_vertices[0].texture.iz = iz;
        _gr_vertices[0].texture.u  = 126 << 16;
        _gr_vertices[0].texture.v  = 1;

        GRDrawPolygonPCCW();

        if ( i < length - 1 && p.m_endClip)
        {
            b_PinLeadingPoint p1 = b_LeadingCache[i + 1];

            if ( p1.m_Valid)
            {
                //_gr_polygon.fill.flat.nColor = currentColor;

                _gr_vertices[3].texture.x  = p.m_LowerPoint2.x;
                _gr_vertices[3].texture.y  = p.m_LowerPoint2.y;
                _gr_vertices[3].texture.iz = iz;
                _gr_vertices[3].texture.u  = 1;
                _gr_vertices[3].texture.v  = 1;

                _gr_vertices[2].texture.x  = p1.m_LowerPoint.x;
                _gr_vertices[2].texture.y  = p1.m_LowerPoint.y;
                _gr_vertices[2].texture.iz = iz;
                _gr_vertices[2].texture.u  = 1;
                _gr_vertices[2].texture.v  = 126 << 16; //b.m_UpperTextureCoord << 16;


                _gr_vertices[1].texture.x  = p1.m_UpperPoint.x;
                _gr_vertices[1].texture.y  = p1.m_UpperPoint.y;
                _gr_vertices[1].texture.iz = iz;
                _gr_vertices[1].texture.u  = 126 << 16;
                _gr_vertices[1].texture.v  = 126 << 16; //b.m_UpperTextureCoord << 16;


                _gr_vertices[0].texture.x  = p.m_UpperPoint2.x;
                _gr_vertices[0].texture.y  = p.m_UpperPoint2.y;
                _gr_vertices[0].texture.iz = iz;
                _gr_vertices[0].texture.u  = 126 << 16;
                _gr_vertices[0].texture.v  = 1;

                GRDrawPolygonPCCW();
            }
        }
    }
//    currentColor++;
}



void DrawBranchCacheFlat(int length)
{

	int hazeMin = CViewFigure::HazeMin();
	int addType = (hazeMin < b_cacheZ) ? GR_POLY_ADD_HAZE : 0;
	int iz = 65536 / b_cacheZ;

	GRSetZPrecision(0);

    for ( int i = 0; i < length ; i++)
    {

        b_PinLeadingPoint p = b_LeadingCache[i];

        if (!p.m_Valid )
            continue;

        _gr_polygon.dwFullType = GR_POLY_FLAT;
        _gr_polygon.dwAddType  = addType;
        _gr_polygon.nVertices  = 4;
        _gr_polygon.dwColor.color = b_TBush::m_hSingleCol;


        _gr_vertices[3].texture.x  = p.m_LowerPoint.x;
        _gr_vertices[3].texture.y  = p.m_LowerPoint.y;
        _gr_vertices[3].texture.iz = iz;

        _gr_vertices[2].texture.x  = p.m_LowerPoint2.x;
        _gr_vertices[2].texture.y  = p.m_LowerPoint2.y;
        _gr_vertices[2].texture.iz = iz;

        _gr_vertices[1].texture.x  = p.m_UpperPoint2.x;
        _gr_vertices[1].texture.y  = p.m_UpperPoint2.y;
        _gr_vertices[1].texture.iz = iz;

        _gr_vertices[0].texture.x  = p.m_UpperPoint.x;
        _gr_vertices[0].texture.y  = p.m_UpperPoint.y;
        _gr_vertices[0].texture.iz = iz;

        GRDrawPolygonPCCW();

        if ( i < length - 1)
        {
            b_PinLeadingPoint p1 = b_LeadingCache[i + 1];

            if ( p1.m_Valid)
            {
                _gr_vertices[3].texture.x  = p.m_LowerPoint2.x;
                _gr_vertices[3].texture.y  = p.m_LowerPoint2.y;
                _gr_vertices[3].texture.iz = iz;

                _gr_vertices[2].texture.x  = p1.m_LowerPoint.x;
                _gr_vertices[2].texture.y  = p1.m_LowerPoint.y;
                _gr_vertices[2].texture.iz = iz;


                _gr_vertices[1].texture.x  = p1.m_UpperPoint.x;
                _gr_vertices[1].texture.y  = p1.m_UpperPoint.y;
                _gr_vertices[1].texture.iz = iz;


                _gr_vertices[0].texture.x  = p.m_UpperPoint2.x;
                _gr_vertices[0].texture.y  = p.m_UpperPoint2.y;
                _gr_vertices[0].texture.iz = iz;

                GRDrawPolygonPCCW();
            }
        }
    }
}


void pin_CalcLeadingIntersections(int length)
{

    return;
    // Finding first valid point
    int index = 0;
    int found = 0;

    for ( ; index < length - 1; index++)
    {
        if (b_LeadingCache[index].m_Valid)
        {
            found = 1;
            break;
        }
    }

    if ( ! found)
    {
        return;
    }

    b_PinLeadingPoint * current = & b_LeadingCache[index++];
    b_PinLeadingPoint * next;


    for ( ; index < length; index++)
    {
        next = & b_LeadingCache[index];

        CVector2 intPoint;

        if (gr_SplitSeg(current->m_UpperPoint.x,
                        current->m_UpperPoint.y,

                        current->m_UpperPoint2.x,
                        current->m_UpperPoint2.y,


                        next->m_UpperPoint.x,
                        next->m_UpperPoint.y,

                        next->m_UpperPoint2.x,
                        next->m_UpperPoint2.y,

                        intPoint.x,
                        intPoint.y)==GR_SEG_INTERSECT_YES)
        // Upper Intersects
        {
            current->m_UpperPoint2 = intPoint;
            next->m_UpperPoint = intPoint;
            current->m_IntersectionType = b_PinLeadingPoint::IT_UPPER;
        }
            else
        if (gr_SplitSeg( current->m_LowerPoint.x,
                         current->m_LowerPoint.y,

                         current->m_LowerPoint2.x,
                         current->m_LowerPoint2.y,

                         next->m_LowerPoint.x,
                         next->m_LowerPoint.y,

                         next->m_LowerPoint2.x,
                         next->m_LowerPoint2.y,

                         intPoint.x,
                         intPoint.y) == GR_SEG_INTERSECT_YES)
        // Lower Intersects
        {
            current->m_LowerPoint2 = intPoint;
            current->m_IntersectionType = b_PinLeadingPoint::IT_LOWER;
            next->m_LowerPoint = intPoint;
        }
        else
        // No intersections
        {
            current->m_IntersectionType = b_PinLeadingPoint::IT_NONE;
        }

        current = next;
    }
}



//**************************************************************************

#define b_CACHE (b_TBush::m_cache)
void b_DrawBranch( int step, b_TBranch &b )
 {
    switch(   b.m_vertCh0->m_isNoFrontClip
           +( b.m_vertCh1->m_isNoFrontClip <<1) )
    {
    case 3:
            {
            b_TCacheVert &chVert0 = *(b.m_vertCh0);
            b_TCacheVert &chVert1 = *(b.m_vertCh1);
            double aspect1 = 1./chVert1.p.z;
            int x0,x1;


            if( !b.m_widtCh0->m_calc )
            {
                SUA_D2INT( &(b.m_widtCh0->s),
                                 b.m_widt0->f*r_PROJ_DIST/ chVert0.p.z);
                b.m_widtCh0->m_calc = 1;
            }
                SUA_D2INT( &(b.m_widtCh1->s),
                                 b.m_widt1->f*r_PROJ_DIST*aspect1);
                b.m_widtCh1->m_calc = 1;

            if( !chVert0.m_isCalcScr )
            {
                double aspect = 1./chVert0.p.z;
                D2LNG( &(chVert0.s.x),  chVert0.p.x * aspect );
                D2LNG( &(chVert0.s.y),  chVert0.p.y * aspect );
                chVert0.m_isCalcScr = 1;
            }
            x0 = chVert0.s.x;

                D2LNG( &(chVert1.s.x),  chVert1.p.x * aspect1 );
                D2LNG( &(chVert1.s.y),  chVert1.p.y * aspect1 );
                chVert1.m_isCalcScr = 1;
            x1 = chVert1.s.x;

            pin_CalcLeadingCache( step, x0,chVert0.s.y,
                                   x1,chVert1.s.y,
                                   b.m_widtCh0->s,
                                   b.m_widtCh1->s, 1);

            }
            return;
    case 1:
            {
            //
            // Отрезаем 1-ую точку
            //
            b_TCacheVert &chVert0 = *(b.m_vertCh0);
            b_TCacheVert &chVert1 = *(b.m_vertCh1);
            double       &chW0    = b.m_widt0->f;
            double       &chW1    = b.m_widt1->f;
            double        newW;

            double len   = chVert0.p.z - r_FRONT_CLIP,
                   aspect= len / (chVert0.p.z - chVert1.p.z + 1e-4);

            CFVector3 newVert = chVert0.p + (chVert1.p - chVert0.p)*aspect;
                      newW    = chW0      + (chW1      - chW0     )*aspect;

            D2LNG(&(chVert1.s.x), newVert.x * r_FRONT_CLIP_DEN);
            D2LNG(&(chVert1.s.y), newVert.y * r_FRONT_CLIP_DEN);
            int sq_size;
            SUA_D2INT(&sq_size, newW * r_PROJ_DIST * r_FRONT_CLIP_DEN);

            if( !b.m_widtCh0->m_calc )
            {

                SUA_D2INT( &(b.m_widtCh0->s),b.m_widt0->f
                                       *r_PROJ_DIST/ b.m_vertCh0->p.z);
                b.m_widtCh0->m_calc = 1;
            }

            if( !chVert0.m_isCalcScr )
            {
                aspect = 1./chVert0.p.z;
                D2LNG( &(chVert0.s.x), chVert0.p.x * aspect );
                D2LNG( &(chVert0.s.y), chVert0.p.y * aspect );
                chVert0.m_isCalcScr = 1;
            }

            pin_CalcLeadingCache( step, chVert0.s.x,chVert0.s.y,
                                   chVert1.s.x,chVert1.s.y,
                                   b.m_widtCh0->s,
                                   sq_size, 1 );


            }
            return;
    case 2:
            {
            //
            // Отрезаем 0-ую точку
            //
            b_TCacheVert &chVert0 = *(b.m_vertCh0);
            b_TCacheVert &chVert1 = *(b.m_vertCh1);
            double       &chW0    = b.m_widt0->f;
            double       &chW1    = b.m_widt1->f;
            double        newW;

            double len   = chVert1.p.z - r_FRONT_CLIP,
                   aspect= len / (chVert1.p.z - chVert0.p.z + 1e-4);

            CFVector3 newVert = chVert1.p + (chVert0.p - chVert1.p)*aspect;
                      newW    = chW1      + (chW0      - chW1     )*aspect;

            D2LNG( &(chVert0.s.x),  newVert.x * r_FRONT_CLIP_DEN );
            D2LNG( &(chVert0.s.y),  newVert.y * r_FRONT_CLIP_DEN );
            int sq_size;
            SUA_D2INT( &sq_size, newW * r_PROJ_DIST * r_FRONT_CLIP_DEN );

            SUA_D2INT( &(b.m_widtCh1->s),  b.m_widt1->f
                                   *r_PROJ_DIST/ b.m_vertCh1->p.z);
            b.m_widtCh1->m_calc = 1;

            aspect = 1./chVert1.p.z;
            D2LNG( &(chVert1.s.x), chVert1.p.x * aspect);
            D2LNG( &(chVert1.s.y), chVert1.p.y * aspect);
            chVert1.m_isCalcScr = 1;

            pin_CalcLeadingCache( step, chVert0.s.x,chVert0.s.y,
                                   chVert1.s.x,chVert1.s.y,
                                   sq_size,
                                   b.m_widtCh1->s, 0);

            }
            return;
    case 0: b_LeadingCache[step].m_Valid   = 0;
            b_LeadingCache[step].m_endClip = 0;
            return;
    }

 }

void r_DrawBranch_nclp( int step, b_TBranch &b )
 {
    b_TCacheVert &chVert0 = *(b.m_vertCh0);
    b_TCacheVert &chVert1 = *(b.m_vertCh1);


    if( !b.m_widtCh0->m_calc )
    {
        SUA_D2INT( &(b.m_widtCh0->s),
                         b.m_widt0->f*r_PROJ_DIST/ chVert0.p.z);
        b.m_widtCh0->m_calc = 1;
    }
        SUA_D2INT( &(b.m_widtCh1->s),
                         b.m_widt1->f*r_PROJ_DIST/ chVert1.p.z);
        b.m_widtCh1->m_calc = 1;

    pin_CalcLeadingCache ( step, chVert0.s.x,chVert0.s.y,
                           chVert1.s.x,chVert1.s.y,
                           b.m_widtCh0->s,
                           b.m_widtCh1->s,1);

 }

void b_DrawBranchORT( int step, b_TBranch &b )
 {
    b_TCacheVert *chVert0 = (b.m_vertCh0);
    b_TCacheVert *chVert1 = (b.m_vertCh1);
    int x0 = chVert0->s.x;
    int x1 = chVert1->s.x;

    pin_CalcLeadingCache( step, x0,chVert0->s.y,
                           x1,chVert1->s.y,
                           b.m_widtCh0->s,
                           b.m_widtCh1->s,1);

 }

void b_DrawSprite( b_TSprite &spr, b_TBranch &br )
 {
    CVector2 &scrCoord = br.m_vertCh1->s;

    double z,dz;
    if(  b_useOrtogonal  )
    {
         z  = b_cacheZ;
         if(  z < CViewObject::m_fFrontClip  ) return;
         dz = b_cache1Z;
    }
    else
    {
         TCCFMatrix3x4 &m = r_POINT_DIR_MATR;
         CFVector3 v(br.m_vert1->f);
         z = m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z + m.m[2][3];
         if(  z < CViewObject::m_fFrontClip  ) return;
         dz = 1./z;
    }
    int		screen_width  = Round(spr.w*CViewObject::m_viewPointScale.x*dz);

    int x0 = scrCoord.x-screen_width/2,
        y0 = scrCoord.y-screen_width/2,
        x1 = x0+screen_width,
        y1 = y0+screen_width;

    GRDrawSprite( x0,y0, x1,y1, spr.x0,spr.y0, spr.x1,spr.y1, 65536*dz,
                  b_Leaves.HImage());
 }

void b_Calc3DVertex( b_TBush &b )
 {
    int i;
    {
         b_TVert      *sour = b.m_vert;
         b_TCacheVert *dest = b.m_cache.m_vert;

         for( i = b.m_vertCnt; i>0; --i, ++sour, ++dest )
         {
              dest->p               = r_POINT_DIR_MATR*(sour->f);
              dest->m_isNoFrontClip = dest->p.z > r_FRONT_CLIP;
              dest->m_isCalcScr     = 0;
         }
    }

    {
         b_TCacheWidt *widt = b.m_cache.m_widt;

         for( i = b.m_widtCnt; i>0; --i, ++widt )
              widt->m_calc = 0;
    }

    b.m_cache.m_lastVert = b.m_vertCnt;
    b.m_cache.m_lastWidt = b.m_widtCnt;
 }


void b_CalcScrWidthORT( b_TBush &b )
 {
    int i;
    b_TCacheWidt *widt = b.m_cache.m_widt;
    b_TWidt      *sour = b.m_widt;


#if b_STYLE_INT_WIDT
    int aspect = (int)(b_TBush::m_widtAspect*b_MATR_PRES);

    for( i = b.m_widtCnt; i>0; --i, ++widt, ++sour )
       widt->s = (sour->i*aspect)>>(b_MATR_PRES_POW*2);
#else
    for( i = b.m_widtCnt; i>0; --i, ++widt, ++sour )
        SUA_D2INT( &(widt->s), sour->f*b_TBush::m_widtAspect );
#endif
 }

void b_CalcScrCoord( b_TBush &b )
 {
    register b_TCacheVert *dest = b.m_cache.m_vert;
    int i;

    for( i = b.m_vertCnt; i>0; --i, ++dest )
    {
        double aspect = 1./dest->p.z;
        D2LNG( &(dest->s.x),  dest->p.x * aspect );
        D2LNG( &(dest->s.y),  dest->p.y * aspect );
    }
 }

void b_CalcScrCoordORT( b_TBush &b,
                        int &minx, int &miny,
                        int &maxx, int &maxy )
 {
    register b_TCacheVert *dest = b.m_cache.m_vert;
    register b_TVert      *sour = b.m_vert;
    int i;
#if b_STYLE_INT_VERT
    //----------------------- INT --------------------------------
    int m[2][4];
    double aspect = b_TBush::m_vertAspect*b_MATR_PRES;
#define LOAD(y,x) SUA_D2INT(&(m[y][x]), r_POINT_DIR_MATR.m[y][x]*aspect )
#define LOADB(y,x) SUA_D2INT(&(m[y][x]), r_POINT_DIR_MATR.m[y][x]*aspect*b_MATR_PRES )
    LOAD(0,0); LOAD(0,1); LOAD(0,2); LOADB(0,3);
    LOAD(1,0); LOAD(1,1); LOAD(1,2); LOADB(1,3);
#undef  LOAD
#undef  LOADB


    for( i = b.m_vertCnt; i>0; --i, ++dest, ++sour )
    {
         b_TVector3D_i &v = sour->i;
         dest->s.x = (m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3]) >> (b_MATR_PRES_POW*2);
         dest->s.y = (m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3]) >> (b_MATR_PRES_POW*2);
    }
#else
    //--------------------- DOUBLE -------------------------------
    double m[2][4];
    double aspect = b_TBush::m_vertAspect;
#define LOAD(y,x)  m[y][x] = r_POINT_DIR_MATR.m[y][x]*aspect
    LOAD(0,0); LOAD(0,1); LOAD(0,2); LOAD(0,3);
    LOAD(1,0); LOAD(1,1); LOAD(1,2); LOAD(1,3);
#undef  LOAD

    for( i = b.m_vertCnt; i>0; --i, ++dest, ++sour )
    {
         CFVector3 &v = sour->f;
         D2LNG(&(dest->s.x), m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3] );
         D2LNG(&(dest->s.y), m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3] );
    }

#endif

    int lminx, lminy, lmaxx, lmaxy;

    lminx = lminy = 1<<30;
    lmaxx = lmaxy = -(1<<30);
    dest = b.m_cache.m_vert;
    for( i = b.m_vertCnt; i>0; --i, ++dest )
    {
        int crd = dest->s.x;

        if( crd < lminx )
             lminx = crd;
        if( crd > lmaxx )
             lmaxx = crd;

            crd = dest->s.y;
        if( crd < lminy )
             lminy = crd;
        if( crd > maxy )
             lmaxy = crd;
    }
    minx = lminx;
    miny = lminy;
    maxx = lmaxx;
    maxy = lmaxy;
 }

void r_CalicMinMaxScr( b_TBush &b,
                       int &minx, int &miny,
                       int &maxx, int &maxy )
 {
    register b_TCacheVert *dest = b.m_cache.m_vert;
    int i;
    int lminx, lminy, lmaxx, lmaxy;

    lminx = lminy = 1<<30;
    lmaxx = lmaxy = -(1<<30);

    for( i = b.m_vertCnt; i>0; --i, ++dest )
    {
        int crd = dest->s.x;

        if( crd < lminx )
             lminx = crd;
        if( crd > lmaxx )
             lmaxx = crd;

            crd = dest->s.y;
        if( crd < lminy )
             lminy = crd;
        if( crd > maxy )
             lmaxy = crd;
    }
    minx = lminx;
    miny = lminy;
    maxx = lmaxx;
    maxy = lmaxy;
 }

r_TDrawBranchFunc b_StartCalc( b_TBush &b )
 {
    r_TDrawBranchFunc drawBranch = b_DrawBranch;
    b_useOrtogonal =0;

    CFVector3   v = b.Center();//b.m_vert[0].f;
    TCCFMatrix3x4 &m = r_POINT_DIR_MATR;
    //double
	b_cacheZ = m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z + m.m[2][3];
	b_cache1Z = 1. / b_cacheZ;
    b.m_visAll = 0;

    b.SingleLineLRH   = bsh_SingleLineLRH_clip;
    b.SingleLineRLH   = bsh_SingleLineRLH_clip;

    b.SingleLineLRVDU = bsh_SingleLineLRVDU_clip;
    b.SingleLineRLVDU = bsh_SingleLineRLVDU_clip;

    if( b_cacheZ - b.m_radius-1 > r_FRONT_CLIP )
    {
         if( b_cacheZ > r_DIST_ORTOGONAL )
         {
             if(  b_cacheZ > r_DIST_AS_SPRITE )
             {
                  CFVector3	v = m*b.Center();
                  int  screen_width  = Round(b.Radius()*CViewObject::m_viewPointScale.x*b_cache1Z*2);
                  int  screen_x = Round(v.x*b_cache1Z),
		               screen_y = Round(v.y*b_cache1Z);

                  int x0 = screen_x-(screen_width>>1),
                      y0 = screen_y-(screen_width>>1),
                      x1 = x0+screen_width,
                      y1 = y0+screen_width;

                  GRDrawSprite( x0,y0, x1,y1, (128+2)<<16,(128+2)<<16, (256-2)<<16,(256-2)<<16,
                                65536*b_cache1Z,
                                b_Leaves.HImage() );
                  return 0;
             }
			 b.m_vertAspect = b_cache1Z;
             b.m_widtAspect = r_PROJ_DIST * b.m_vertAspect;
             drawBranch = b_DrawBranchORT;

             int minx,miny,maxx,maxy;
             b_CalcScrCoordORT( b, minx, miny, maxx, maxy );
             b_CalcScrWidthORT( b );
             b_useOrtogonal = 1;

             if(   minx>=r_CLIP2D.left && minx<r_CLIP2D.right
                && miny>=r_CLIP2D.top  && miny<r_CLIP2D.bottom
                && maxx>=r_CLIP2D.left && maxx<r_CLIP2D.right
                && maxy>=r_CLIP2D.top  && maxy<r_CLIP2D.bottom
                )
             {
                  b.SingleLineLRH = bsh_SingleLineLRH_nclp;
                  b.SingleLineRLH = bsh_SingleLineRLH_nclp;
                  b.SingleLineLRVDU = bsh_SingleLineLRVDU_nclp;
                  b.SingleLineRLVDU = bsh_SingleLineRLVDU_nclp;


                  int w, c;
                  SUA_D2INT( &w, b.m_maxWidth * r_PROJ_DIST /(b_cacheZ-b.m_radius));
                  w += 1;
                  c = minx - w;
                  if( c>=r_CLIP2D.left && c<r_CLIP2D.right )
                  {
                    c = miny - w;
                    if( c>=r_CLIP2D.top && c<r_CLIP2D.bottom )
                    {
                      c = maxx + w;
                      if( c>=r_CLIP2D.left && c<r_CLIP2D.right )
                      {
                        c = maxy + w;
                        if( c>=r_CLIP2D.top && c<r_CLIP2D.bottom )
                        {
                             b.m_visAll = 1;
                        }
                      }
                    }
                  }

             }
         }
         else
         {
             b_Calc3DVertex( b );
             b_CalcScrCoord( b );
             drawBranch = r_DrawBranch_nclp;

             if( b_cacheZ > r_MIN_VIEW_DIST )
             {
                  int minx,miny,maxx,maxy;
                  r_CalicMinMaxScr( b, minx, miny, maxx, maxy );

                  if(   minx>=r_CLIP2D.left && minx<r_CLIP2D.right
                     && miny>=r_CLIP2D.top  && miny<r_CLIP2D.bottom
                     && maxx>=r_CLIP2D.left && maxx<r_CLIP2D.right
                     && maxy>=r_CLIP2D.top  && maxy<r_CLIP2D.bottom
                     )
                  {
                       b.SingleLineLRH = bsh_SingleLineLRH_nclp;
                       b.SingleLineRLH = bsh_SingleLineRLH_nclp;
                       b.SingleLineLRVDU = bsh_SingleLineLRVDU_nclp;
                       b.SingleLineRLVDU = bsh_SingleLineRLVDU_nclp;
                  }
             }
         }
    }
    else b_Calc3DVertex( b );

    return drawBranch;
 }


CFMatrix3x4 mm_dirMatrx;

void b_TBush::Draw()
 {
    r_POINT_DIR_MATR = *CViewObjectBaseSet::CurrDir();
    //r_POINT_DIR_MATR.ScaleL(CFVector3(1./256, 1./256, 1./256));
    r_TDrawBranchFunc DrawBranch = b_StartCalc( *this );

    if(  DrawBranch==0  ) return;

    b_TNode   *stack[r_MAX_NODE];
    register b_TNode   **stackPTR = stack,
                        *curNode;
    b_TBranch *branch;

    //currentColor = 1;


    *stackPTR = m_node;

    for(; stackPTR >= stack ;)
    {
        curNode = (*stackPTR);
        branch = curNode->m_chain;

        for (int i = 0; i < curNode->m_cnt; i++ )
        {
            DrawBranch(i, *branch );
            ++branch;
        }


        pin_CalcLeadingIntersections(curNode->m_cnt);

        if (curNode->m_nextNodeLinked & B_LEFTLINKED)
        {
            if (b_LeadingCache[curNode->m_cnt - 1].m_Valid == 0)
            {
                curNode->m_left->m_linkCacheValid = 0;
            }
            else
            {
                curNode->m_left->m_linkCacheUpper = b_LeadingCache[curNode->m_cnt - 1].m_UpperPoint2;
                curNode->m_left->m_linkCacheLower = b_LeadingCache[curNode->m_cnt - 1].m_LowerPoint2;
                branch = & (curNode->m_chain[curNode->m_cnt - 1]);
                curNode->m_left->m_linkCache  = branch->m_vertCh1->s;
                curNode->m_left->m_linkCacheValid = 1;
            }
        }

        if (curNode->m_nextNodeLinked & B_RIGHTLINKED)
        {
            if (b_LeadingCache[curNode->m_cnt - 1].m_Valid == 0)
            {
                curNode->m_right->m_linkCacheValid = 0;
            }
            else
            {
                curNode->m_right->m_linkCacheUpper = b_LeadingCache[curNode->m_cnt - 1].m_UpperPoint2;
                curNode->m_right->m_linkCacheLower = b_LeadingCache[curNode->m_cnt - 1].m_LowerPoint2;
                branch = & (curNode->m_chain[curNode->m_cnt - 1]);
                curNode->m_right->m_linkCache  = branch->m_vertCh1->s;
                curNode->m_right->m_linkCacheValid = 1;
            }
        }


        if ( curNode->m_linkChain && curNode->m_linkCacheValid)
        {
            branch = & (curNode->m_chain[0]);
            if (DrawBranch != b_DrawBranchORT)
                    DrawLinkChainTxr(curNode->m_linkCache,curNode->m_linkCacheUpper,curNode->m_linkCacheLower);
        }


        if (DrawBranch == b_DrawBranchORT)
        DrawBranchCacheFlat(curNode->m_cnt);
            else
        DrawBranchCacheTxr(curNode->m_cnt);


        /*b_LastBranchCacheUpper  = b_LeadingCache[curNode->m_cnt - 1].m_UpperPoint2;
        b_LastBranchCacheLower  = b_LeadingCache[curNode->m_cnt - 1].m_LowerPoint2;
        branch = & (curNode->m_chain[curNode->m_cnt - 1]);
        b_LastBranchCache       = branch->m_vertCh1->s;*/


        switch( curNode->m_flag )
        {
        case 0: case 5: --stackPTR; continue;
        case 3:
                if( r_ORD_VIEW_POINT.x >= curNode->m_plane )
                {
                     stackPTR[0] = curNode->m_left;
                     stackPTR[1] = curNode->m_right;
                     ++stackPTR;
                     break;
                }
                stackPTR[0] = curNode->m_right;
                stackPTR[1] = curNode->m_left;
                ++stackPTR;
                continue;
        case 8:
                if( r_ORD_VIEW_POINT.z >= curNode->m_plane )
                {
                     stackPTR[0] = curNode->m_left;
                     stackPTR[1] = curNode->m_right;
                     ++stackPTR;
                     continue;
                }
                stackPTR[0] = curNode->m_right;
                stackPTR[1] = curNode->m_left;
                ++stackPTR;
                continue;

        case 1: case 6:
                *stackPTR = curNode->m_left;
                continue;
        case 2: case 7:
                *stackPTR = curNode->m_right;
                continue;

        case 4: case 9:
                b_DrawSprite( curNode->m_spr, curNode->m_chain[curNode->m_cnt-1] );
                --stackPTR;
                continue;

        default: RTCHECK(0,"b_TBush::Draw(): Bad flag");
        }

    }
 }

typedef byte *bytePTR;

void GETCYCLE(unsigned *p);
#pragma aux GETCYCLE = \
    "mov  ebx,eax " \
    ".586p " "rdtsc "        \
    "mov [ebx],eax "\
    "mov [ebx+4],edx "	parm [eax] modify [eax ebx edx]


double GetCycle()
 {
    unsigned u[2];
    double   m = (unsigned)(-1);
    GETCYCLE(u);
    return ((double)u[0]) +((double)u[1])*(m+1) ;
 }
