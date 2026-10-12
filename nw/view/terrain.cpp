//#ifndef	__VIEW_CPP__
#include"_view.h"
//#endif

#include<windows.h>

int __revCnt, __fullCnt;


#define	MAX_REDCELL(X,Y)	(m_red(X,Y)&7)
#define	MAX_REDVERT(X,Y)	((m_red(X,Y)>>3)&7)
#define	MASK_CELL(X,Y)		((m_red(X,Y)>>6)&3)


#define	_RC_COUNT	0
#define	_STATIC_REDUCTIONS	0
#define	_INIT_CACHE	0
#define	_PRECOMPUTE_REDS	0
#define	_ALIGN_EDGES	1
#define	_INTERPOLATE_VERTICES	1
#define	_INTERPOLATE_COLORS 1
#define	_TERRAIN_WATER	1
#define	_WATER_NOREDRESTRICT	0
#define	_REDUCTION_IMMEDIATE	0
//#pragma inline_depth	10

const	int		nTerrainMaxReduction = 4;
//const int nTerrainPrec = nIntegerPrec;
//const int nTerrainHPrec = 65536;
double __terrainWaterline = 20;
int	   __nTerrainWaterHighest;
int	   __nTerrainLandLowest;

int		__terrainWhiteColor;
int		__terrainCoords = 0;
CFixedColorFont	terrFnt;



double	__terrainCell = 10;
int __polygonReductionCall, __reductionCall, __cellCount, __reductionCallLink;

#if _RC_COUNT
byte	(*rcCount)[512] = new byte[512][512];
#endif
int		__countRc = 0;

#define	TERRAIN_HEIGHT_SHIFT	16


CRandomGenerator	vt_randomPos, vt_randomRed;
int vt_countRed = 0;


void _CViewTerrain::CMipMapHeight::Init(_CViewTerrainMap<byte> &map,int nShiftReduction,TCchar *name)
{
	RTCHECK1(__terrainWaterline <= 255, "Waterline (%d) exceeds 255",__terrainWaterline);
	int  nReduction = 1<<nShiftReduction;
	m_nShiftR = nShiftReduction;
	RTCHECK(map.Width()%nReduction==0&&map.Height()%nReduction==0,"Mipmap reduction is not dimensions divider");
	m_nWidth = map.Width()/nReduction;
	m_nHeight = map.Height()/nReduction;
	m_pImage = new byte[m_nWidth*m_nHeight][2];
	if( name ) {
		FILE *f;
		long len;
		f = CFileResource::FOpenCurrent(name,&len);
		RTCHECK(f,name);
		RTCHECK1(len==5+m_nWidth*m_nHeight*2,"Bad %s length",name);
		word w,h; byte t;
		RTCHECK1(fread(&w,2,1,f)==1&&w==m_nWidth&&
			fread(&h,2,1,f)==1&&h==m_nHeight&&
			fread(&t,1,1,f)==1&&
			fread(m_pImage,m_nWidth*m_nHeight*2,1,f)==1&&
			fclose(f)==0,"Error reading %s",name);
	}
	for( int i = 0 ; i < m_nWidth ; i++ )
	for( int j = 0 ; j < m_nHeight ; j++ ) {
		byte nmin = (byte)-1, nmax = 0;
		if( name ) {
			nmin = m_pImage[j*m_nWidth+i][0];
			nmax = m_pImage[j*m_nWidth+i][1];
		}
		else {
			for( int i1 = 0 ; i1 < nReduction ; i1++ )
			for( int j1 = 0 ; j1 < nReduction ; j1++ ) {
				byte n = map(i*nReduction+i1,j*nReduction+j1);
				nmin = Min(nmin,n); nmax = Max(nmax,n);
			}
		}
		ASSERT(nmin<=nmax);
		if( (uint)nmax < __terrainWaterline )	nmax = (byte)__terrainWaterline;
		ASSERT(nmin<=nmax);
		m_pImage[j*m_nWidth+i][0] = nmin;
		m_pImage[j*m_nWidth+i][1] = nmax;
	}
}

inline byte  *_CViewTerrain::CMipMapHeight::operator()(int x,int y)
{
	return m_pImage[m_nWidth*(y&(m_nHeight-1))+(x&(m_nWidth-1))];
}

inline byte  _CViewTerrain::CMipMapHeight::GetLimit(int x,int y,int nMinMax,int nShiftReduction)
{
	if( nShiftReduction <= m_nShiftR )
		return m_pImage[m_nWidth*((y>>m_nShiftR)&(m_nHeight-1))+
									((x>>m_nShiftR)&(m_nWidth-1))][nMinMax];
	int n = 1<<nShiftReduction-m_nShiftR;
	x >>= m_nShiftR;
	y >>= m_nShiftR;
	byte nBest = (byte)(nMinMax-1);
	for( int i = 0 ; i < n ; i++ )
	for( int j = 0 ; j < n ; j++ ) {
		byte b = m_pImage[m_nWidth*((y+j)&(m_nHeight-1))+
								((x+i)&(m_nWidth-1))][nMinMax];
		nBest = nMinMax ? Max(nBest,b) : Min(nBest,b);
	}
	return nBest;
}
// ----------------------------------------------------------------------
inline void _CViewTerrain::SVert3_0::SetColor(TCCVector2 &c)
{
	u = c.y;
	v = c.x;
	//D2INT(&u,c.y); D2INT(&v,c.x);	// *65536
}

inline void _CViewTerrain::SVert3::SetLCoord(int x,int y)
{
	L[I_X] = x;
	L[I_Y] = y;
}

inline void _CViewTerrain::SVert3::Interpolate(_CViewTerrain::SVert3 &v0,_CViewTerrain::SVert3 &v1,int coord)
{
	int d = v1.L[coord] - v0.L[coord]; ASSERT(d>0);
	double t = (L[coord]-v0.L[coord])/(double)d;
	//D2INT(&x3, v0.x3+(v1.x3-v0.x3)*t );
	//D2INT(&y3, v0.y3+(v1.y3-v0.y3)*t );
	//D2INT(&z3, v0.z3+(v1.z3-v0.z3)*t );
	v3.x = v0.v3.x+(v1.v3.x-v0.v3.x)*t;
	v3.y = v0.v3.y+(v1.v3.y-v0.v3.y)*t;
	v3.z = v0.v3.z+(v1.v3.z-v0.v3.z)*t;
	D2INT(&u, v0.u+(v1.u-v0.u)*t );
	D2INT(&v, v0.v+(v1.v-v0.v)*t );
	if( v3.z <= CViewObject::m_fFrontClip ) cf = 1; else
	if(	v3.z >= CViewObject::m_fBackClip ) cf = 2; else {
		double	_1z = 1./v3.z;
		D2INT(&x,v3.x*_1z); // z3;
		D2INT(&y,v3.y*_1z); // z3;
		//iz = (65536*nTerrainPrec)/z3;
		D2INT(&iz,_1z*CViewObject::m_fz1MinScale /*65536.*/);
		cf = 0;
	}
}

inline void	_CViewTerrain::Copy(_CViewTerrain::SVert3W &d,const _CViewTerrain::CCellVertex &s)
{
	*(SVert3*)&d = *(SVert3*)&s;
	if( (d.wf = s.wf) & VF_WATER_DATA ) d.h0 = s.h0, d.w = s.w;
}


inline bool _CViewTerrain::IsViewAlongX()
{
	return fabs(CViewObject::m_viewPointInvMx.m[0][2]) > fabs(CViewObject::m_viewPointInvMx.m[2][2]);
}

inline bool _CViewTerrain::IsViewAlongY()
{
	return !IsViewAlongX();
}

inline bool _CViewTerrain::IsViewAlongXP()
{
	//ASSERT(IsViewAlongX());
	return	CViewObject::m_viewPointInvMx.m[0][2] < 0;
}

inline bool _CViewTerrain::IsViewAlongXN()
{
	return	!IsViewAlongXP();
}

inline bool _CViewTerrain::IsViewAlongYP()
{
	//ASSERT(IsViewAlongY());
	return	CViewObject::m_viewPointInvMx.m[2][2] > 0;
}

inline bool _CViewTerrain::IsViewAlongYN()
{
	return	!IsViewAlongYP();
}

GR_HTEXTURE _CViewTerrain::LoadMask(TCchar *name,int dim,GR_HTEXTURE hOld)
{
	FILE *f = CFileResource::FOpenCurrent(name);
	RTCHECK1(f,"Cannot open '%s'",name);

	BITMAPFILEHEADER	bf;
	BITMAPINFOHEADER	bi;

	RTCHECK1(
		fread(&bf,sizeof(bf),1,f)==1		&&
		bf.bfType == 0x4D42					&&
		fread(&bi,sizeof(bi),1,f)==1  ,"Error reading '%s'",name);

	RTCHECK1( bi.biCompression == BI_RGB, "'%s' is compressed", name );
	RTCHECK3( dim < 0 || bi.biWidth == dim && bi.biHeight == dim, "'%s' dimensions are not %dx%d", name, dim, dim );
	RTCHECK1( bi.biBitCount == 8, "8-bit image expected in '%s'", name );
	RTCHECK1( bi.biClrUsed <= 256 /* && (m_bi.biClrImportant & 255) == 0*/, "Bad color count in '%s'",name);
	RGBQUAD	  pal[256];
	memset(pal,0,sizeof(RGBQUAD)*256);
	if( bi.biClrUsed == 0 )	bi.biClrUsed = 256;
	RTCHECK1(fread(pal,sizeof(RGBQUAD),bi.biClrUsed,f)==bi.biClrUsed, "Error reading palette from '%s'", name);
	if( bi.biClrUsed == 256 )	bi.biClrUsed = 0;
	RTCHECK1(dim < 0 || bi.biSizeImage == dim*dim, "Bad image size in '%s'", name );

	byte *pImage = PTNCreate2AlignedImage(bi.biWidth,bi.biHeight,1,TEXTURE_UNLOAD | TEXTURE_PAL_FORMAT);

	RTCHECK1(ftell(f)==bf.bfOffBits, "Unexpected image offset in '%s'", name );
	RTCHECK1(fread(pImage,1,bi.biSizeImage,f)==bi.biSizeImage, "Error reading image '%s'",name);

	fclose(f);

	SRGB	pal3[256];
	for( int i = 0 ; i < 256 ; i++ ) {
		pal3[i].r = pal[i].rgbRed;
		pal3[i].g = pal[i].rgbGreen;
		pal3[i].b = pal[i].rgbBlue;
	}

    GR_HTEXTURE hImage = GRLoadTextureToDB(hOld, (byte*)pal3, 256, (byte*)pImage);
	PTNUnload2AlignedImage(pImage);

	GRSetTextureLoadFunc( hImage, name, ReloadMask, NULL );

	return	hImage;
}

GR_HTEXTURE _CViewTerrain::ReloadMask(TCchar *name,GR_HTEXTURE hOld,void *)
{
	return LoadMask(name,-1,hOld);
}

extern SDeviceList _dL;

GR_HTEXTURE _CViewTerrain::LoadMaskLand( TCchar *name, bool bHiRes )
{
	char	szName[100];
	TCchar *pszHardware = m_bIsHardware ? "h" : "";
	TCchar *pszDims = bHiRes ? "9" : "";
	sprintf(szName,"%s%s%s.bmp", pszHardware, name, pszDims);
	return LoadMask(szName, bHiRes ? 512 : 256 );
}

void _CViewTerrain::LoadHWSW()
{
	GRDeleteTextureFromDB(m_hMask);
	GRDeleteTextureFromDB(m_hMask1);
	bool bHiRes = !m_bIsHardware || _dL.currDevice->textureW >= 512;
	m_nHiResShift = bHiRes ? 1 : 0;
	m_hMask = LoadMaskLand("mask",bHiRes);
	m_hMask1 = LoadMaskLand("mask1",bHiRes);
	m_colors.Clear();
	m_colors.Load(m_bIsHardware?"hmapc7.spr":"mapc7.spr");
	m_brightness.Clear();
	if( m_bIsHardware ) m_brightness.Load("bright7.spr");
}

void _CViewTerrain::ClearHWSW()
{
	GRDeleteTextureFromDB(m_hMask);
	GRDeleteTextureFromDB(m_hMask1);
	m_colors.Clear();
	m_brightness.Clear();
}

_CViewTerrain::_CViewTerrain(double fCellSize,double fHScale,double fViewDiameter)
//	: m_heights("covh7.spr"), m_colors("mapc7.spr"),
//		m_mask("mask.spr","mask.pal"), m_bump("bump.spr","bump.pal"),
//		m_mask2("mask2.spr","mask2.pal"), m_bump2("bump2.spr","bump2.pal"),
//		m_hrange(m_heights,_CViewTerrain::HRANGE_SHIFTREDUCTION),
//		m_red("red.spr")
{
	terrFnt.Read("..\\figs5x3c.fnt");
	m_bIsHardware = GRIsHardware();

	m_hMask = NULL;
	m_hMask1 = NULL;
	LoadHWSW();

	m_heights.Load("covh7.spr");

	m_hBump = LoadMask("bump.bmp",256);
	m_hMaskW = LoadMask("maskw.bmp",256);
	m_hBumpW = LoadMask("bumpw.bmp",256);
	m_hrange.Init(m_heights,_CViewTerrain::HRANGE_SHIFTREDUCTION,"hrange.spr");
		//.Load("hrange.spr");
	m_red.Load("red.spr");
	m_maskflag.Load("maskflag.spr");
	//m_hrange.Init(m_heights,_CViewTerrain::HRANGE_SHIFTREDUCTION),
	//m_red("red.spr")

	SetReductionStart(4); //m_fDistRed0 = 4;
	m_nWidth = m_nHeight = 512;
	m_heights.CheckDimensions(m_nWidth,m_nHeight);
	m_colors.CheckDimensions(m_nWidth,m_nHeight);
	//m_mask.CheckDimensions(256,256);
	//m_pMaskCache = new int[256*2+1]+256;
	//for( int i = -256 ; i <= 256 ; i++ )	m_pMaskCache[i] = i*256;
	m_fCellSize = fCellSize;
	m_fHScale = fHScale;
	m_f_1_CellSize = 1./fCellSize;
	m_fHStep = fHScale*fCellSize;
	m_fCellSize0 = 1/fHScale;
	m_nMaxEdges = (int)ceil(fViewDiameter/fCellSize);
	m_nReductions = 1;
	for( int i = m_nMaxEdges-1 ; i > 0 ; m_nReductions++ )	i >>= 1;
	m_ppEdges = new (int (*[m_nReductions])[2]); // CEdge*[m_nReductions];
	IF_DEBUG(m_pnEdges = new int[m_nReductions]);
	m_pEdgeR = new (int [m_nMaxEdges][2]);// CEdge[nMaxEdges];
	m_pCache0 = new CCellVertex[1024*4];
	m_pCacheXL0 = m_pCache0; m_pCacheXU0 = m_pCache0+1024;
	m_pCacheYL0 = m_pCache0+2048; m_pCacheYU0 = m_pCache0+3072;
	int n = m_nMaxEdges;
	for( int i = 0 ; i < m_nReductions ; i++ ) {
		m_ppEdges[i] = new (int [n][2]); // CEdge[n];
		IF_DEBUG(m_pnEdges[i]=n);
		n = n+2>>1;
	}
	for( int i = 0 ; i < MAX_CELLVERTS ; i++ )	{
		m_apCellVerts[i] = &m_cellVerts[i];
		m_apCellVertsW[i] = &m_cellVerts[i].w;
		m_apCellVertsWR[MAX_CELLVERTS-1-i] = &m_cellVerts[i].w;
	}
	m_fWaterline0 = __terrainWaterline; //Round(__terrainWaterline * nTerrainHPrec);
	__nTerrainWaterHighest = Round(__terrainWaterline-0.501);
	__nTerrainLandLowest = Round(__terrainWaterline+0.501);

	#if _PRECOMPUTE_REDS
		m_nRedsP = m_nMaxEdges+(1<<nTerrainMaxReduction)*2;
		m_pReds0 = new byte[m_nRedsP*m_nRedsP];
		//								Y offset		X offset
		//m_pRedsOrg0 = m_pReds0+m_nRedsP*(m_nRedsP/2)+m_nRedsP/2;
		m_pRedsOrg0 = m_pReds0+(m_nRedsP+1)*(m_nRedsP/2);
	#endif

	__terrainWhiteColor = GRFillColor(255,255,255);
	terrFnt.RecreateFont();
}

_CViewTerrain::~_CViewTerrain()
{
	//delete [] (m_pMaskCache-256);
	for( int i = 0 ; i < m_nReductions ; i++ )
		delete [] m_ppEdges[i];
	delete [] m_ppEdges;
	IF_DEBUG(delete [] m_pnEdges);
	delete [] m_pEdgeR;
	delete [] m_pCache0;

	GRDeleteTextureFromDB(m_hMask);
	GRDeleteTextureFromDB(m_hBump);
	GRDeleteTextureFromDB(m_hMaskW);
	GRDeleteTextureFromDB(m_hBumpW);
}


#if _PRECOMPUTE_REDS
inline void _CViewTerrain::SetReds(int s,int p,int r)
{
	ASSERT( m_redsRect.Contains(CVector2(s,p)) );
	m_pReds[p*m_nRedsP+s] = r;
	/*byte &n = m_pReds[y*m_nRedsW+(x>>1)];
	if( x & 1 ) {
		n &= 0xF0;
		n |= r<<4;
	}
	else {
		n &= 0xF;
		n |= r;
	}*/
}

inline int _CViewTerrain::GetReds(int s,int p)
{
	ASSERT( m_redsRect.Contains(CVector2(s,p)) );
	return	m_pReds[p*m_nRedsP+s];
	//byte &n = m_pReds[p*m_nRedsP+(s>>1)];
	//return	s & 1 ? n>>4 : n & 15;
}

inline void _CViewTerrain::SetReds(s,p,d,r)
{
	int sd = s+d, pd = p+d;
	for( int ss = s ; ss < sd ; ss++ )
	for( int pp = p ; pp < pd ; pp++ )	SetReds(s,p,r);
}
#endif

void _CViewTerrain::SetViewPoint()
{
	m_fCellSize = __terrainCell;
	__polygonReductionCall = __reductionCall = 0;
	__cellCount = __reductionCallLink = 0;
	// Generate projection data
	(m_viewPointDirMx = CViewObject::m_viewPointDirSMx).ScaleR(CFVector3(m_fCellSize,m_fCellSize*m_fHScale,-m_fCellSize));

	CFVector3	viewpoint = CViewObject::m_viewPointInvMx.Offset();
	m_viewpoint = CFVector2(viewpoint.x/m_fCellSize,-viewpoint.z/m_fCellSize);
	m_viewpointL[I_X] = floor(viewpoint.x/m_fCellSize);
	m_viewpointL[I_Y] = floor(-viewpoint.z/m_fCellSize);
	m_nearestOffset = CVector2(IsViewAlongXN() ? 1 : 0, IsViewAlongYN() ? 1 : 0);


	// compute water visibility flags
	double	fWater = __terrainWaterline*m_fCellSize*m_fHScale;
	m_bIsAboveWater = !CViewObject::IsBelowWater();
		// CViewObject::m_viewPointInvMx.m[1][3] >= fWater;
	m_bIsAboveWaterCpl = m_bIsAboveWater;
			// CViewObject::m_viewPointInvMx.m[1][3] >= fWater+2;
	m_bIsBelowWaterCpl = !m_bIsAboveWater;
			// CViewObject::m_viewPointInvMx.m[1][3] <= fWater-2;
	CViewObject::m_eWaterView = m_bIsAboveWater ? WATER_VIEW_ABOVE : WATER_VIEW_BELOW;



	// Create view frustum and its projections
	CFVector3	pyramidVerts3[8];
	CFVector2	pyramidVerts[24];
	for( int i = 0 ; i < 4 ; i++ ) {
		pyramidVerts3[i] = viewpoint+CViewObject::m_clipRays[i]*CViewObject::m_fFrontClip;
		pyramidVerts[i] = CFVector2(pyramidVerts3[i].x,-pyramidVerts3[i].z)/m_fCellSize;
		pyramidVerts3[i+4] = viewpoint+CViewObject::m_clipRays[i]*CViewObject::m_fBackClip;
		pyramidVerts[i+4] = CFVector2(pyramidVerts3[i+4].x,-pyramidVerts3[i+4].z)/m_fCellSize;
	}
	int  nPyramidVerts = 8;

	// Optimize view frustum
	for( int nBoundSquare = 32000*32000+1 ;; ) {

		if( nPyramidVerts < 1 ) { m_bTerrainInView = FALSE; return; }

		// Compute bounding rect
		CRect2	boundRect;
		boundRect.left = (int)floor(pyramidVerts[0].x);
		boundRect.top = (int)floor(pyramidVerts[0].y);
		boundRect.right = boundRect.left+1;
		boundRect.bottom = boundRect.top+1;
		for( int i = 1 ; i < nPyramidVerts ; i++ ) {
			int x = (int)floor(pyramidVerts[i].x);
			int y = (int)floor(pyramidVerts[i].y);
			if( boundRect.left > x )	boundRect.left = x;
			if( boundRect.top > y )		boundRect.top = y;
			x++; y++;
			if( boundRect.right < x )	boundRect.right = x;
			if( boundRect.bottom < y )	boundRect.bottom = y;
		}
		boundRect.left >>= HRANGE_SHIFTREDUCTION;
		boundRect.top >>= HRANGE_SHIFTREDUCTION;
		boundRect.right = (boundRect.right+(1<<HRANGE_SHIFTREDUCTION)-1) >> HRANGE_SHIFTREDUCTION;
		boundRect.bottom = (boundRect.bottom+(1<<HRANGE_SHIFTREDUCTION)-1) >> HRANGE_SHIFTREDUCTION;
		ASSERT( boundRect.Width() <= 32000 && boundRect.Height() <= 32000 );

		// If gain == nothing leave
		int nNewSquare = boundRect.Width()*boundRect.Height();
		if( nNewSquare >= nBoundSquare )	break;
		nBoundSquare = nNewSquare;

		// Compute clipping height
		double	fClip[2];
		int     nClipL = (int)(byte)-1, nClipH = 0;
		for( int x = boundRect.left ; x < boundRect.right ; x ++ )
		for( int y = boundRect.top ; y < boundRect.bottom ; y++ ) {
			byte	*p = m_hrange(x,y);
			nClipH = Max(nClipH,(int)p[1]);
			nClipL = Min(nClipL,(int)p[0]);
		}
		ASSERT( nClipL <= nClipH );
		#if 1
		if( m_bIsAboveWater ) {
			if( nClipL < __nTerrainWaterHighest )	nClipL = __nTerrainWaterHighest;
			if( nClipH < __nTerrainWaterHighest )	nClipH = __nTerrainWaterHighest;
		}
		else {
			if( nClipL > __nTerrainLandLowest )	nClipL = __nTerrainLandLowest;
			if( nClipH > __nTerrainLandLowest )	nClipH = __nTerrainLandLowest;
		}
		ASSERT( nClipL <= nClipH );
		#endif
		fClip[0] = nClipL*m_fHScale*m_fCellSize;
		fClip[1] = nClipH*m_fHScale*m_fCellSize;

		// Clip frustum
		int cf[8];
		for( int i = 0 ; i < 8 ; i++ ) {
			if( pyramidVerts3[i].y < fClip[0] )	cf[i] = 1; else
			if( pyramidVerts3[i].y > fClip[1] )	cf[i] = 2; else
			cf[i] = 0;
		}
		nPyramidVerts = 0;
		for( int i = 0 ; i < 8 ; i++ ) {
			if( cf[i] ) {
				int i1;
				if( cf[i1=(i&3)>0?i-1:i+3] != cf[i] ) {
					ASSERT(nPyramidVerts<24);
					pyramidVerts[nPyramidVerts++] =
						(CFVector2(pyramidVerts3[i].x,-pyramidVerts3[i].z)+
						CFVector2(pyramidVerts3[i1].x-pyramidVerts3[i].x,
								pyramidVerts3[i].z-pyramidVerts3[i1].z)*
						((fClip[cf[i]-1]-pyramidVerts3[i].y)/
							AbsNoLess(pyramidVerts3[i1].y-pyramidVerts3[i].y,1e-5)))/m_fCellSize;
				}
				if( cf[i1=(i&3)<3?i+1:i-3] != cf[i] ) {
					ASSERT(nPyramidVerts<24);
					pyramidVerts[nPyramidVerts++] =
						(CFVector2(pyramidVerts3[i].x,-pyramidVerts3[i].z)+
						CFVector2(pyramidVerts3[i1].x-pyramidVerts3[i].x,
								pyramidVerts3[i].z-pyramidVerts3[i1].z)*
						((fClip[cf[i]-1]-pyramidVerts3[i].y)/
							AbsNoLess(pyramidVerts3[i1].y-pyramidVerts3[i].y,1e-5)))/m_fCellSize;
				}
				if( cf[i1=i^4] != cf[i] ) {
					ASSERT(nPyramidVerts<24);
					pyramidVerts[nPyramidVerts++] =
						(CFVector2(pyramidVerts3[i].x,-pyramidVerts3[i].z)+
						CFVector2(pyramidVerts3[i1].x-pyramidVerts3[i].x,
								pyramidVerts3[i].z-pyramidVerts3[i1].z)*
						((fClip[cf[i]-1]-pyramidVerts3[i].y)/
							AbsNoLess(pyramidVerts3[i1].y-pyramidVerts3[i].y,1e-5)))/m_fCellSize;
				}
			}
			else {
				ASSERT(nPyramidVerts<24);
				pyramidVerts[nPyramidVerts++] = CFVector2(pyramidVerts3[i].x,-pyramidVerts3[i].z)/m_fCellSize;
			}
		} // clip frustum by layer
		if( nPyramidVerts < 1 ) { m_bTerrainInView = FALSE; return; }
		ASSERT(nPyramidVerts>=3);

	} // optimizing frustum

	m_bTerrainInView = TRUE;


	// Sort all projections in ascending y order
	for( int i = 0 ; i < nPyramidVerts-1 ; i++ ) {
		int i0 = i;
		for( int j = i+1 ; j < nPyramidVerts ; j++ )
			if( pyramidVerts[j].y < pyramidVerts[i0].y ) i0 = j;
		if( i0 != i ) {
			CFVector2	temp = pyramidVerts[i];
			pyramidVerts[i] = pyramidVerts[i0];
			pyramidVerts[i0] = temp;
		}
	}

	// Begin forming a convex polygon on pyramid's vertices projections
	// starting from the least y
	CFVector2	polygonVertices[48];
	int nPolygonVertices = 0;
	polygonVertices[nPolygonVertices++] = pyramidVerts[0];

	// First we go along the right side
	for( int i = 1 ;; i++) {
		int		nNext = -1;
		double	fMaxAngle;
		for( int j = i ; j < nPyramidVerts ; j++ ) {
			CFVector2	edge = pyramidVerts[j]-polygonVertices[nPolygonVertices-1];
			double		fEdgeAbs = Abs(edge);
			if( fEdgeAbs > 1e-3 ) {
				double	fAngle = edge.x/fEdgeAbs; // sin
				if( nNext < 0 || fAngle > fMaxAngle ) {
					nNext = j;
					fMaxAngle = fAngle;
				}
			}
		}
		if( nNext < 0 ) break;
		ASSERT(nPolygonVertices<48);
		polygonVertices[nPolygonVertices++] = pyramidVerts[i=nNext];
	}

	// Then we return along the left one
	for( int i = nPyramidVerts-2 ;; i--) {
		int		nNext = -1;
		double	fMinAngle;
		for( int j = i ; j >= 0 ; j-- ) {
			CFVector2	edge = pyramidVerts[j]-polygonVertices[nPolygonVertices-1];
			double		fEdgeAbs = Abs(edge);
			if( fEdgeAbs > 1e-3 ) {
				double	fAngle = edge.x/fEdgeAbs; // sin
				if( nNext < 0 || fAngle < fMinAngle ) {
					nNext = j;
					fMinAngle = fAngle;
				}
			}
		}
		if( nNext < 1 )	break;	// 0th vertex already included
		if( Abs(polygonVertices[0]-pyramidVerts[nNext]) < 2e-3 ) break;
		ASSERT(nPolygonVertices<48);
		polygonVertices[nPolygonVertices++] = pyramidVerts[i=nNext];
	}
	if( nPolygonVertices < 3 ) { m_bTerrainInView = FALSE; return; }
	//ASSERT(nPolygonVertices>=3);

	WRITELOG2("ViewpointL=(%d,%d)\n",m_viewpointL[I_X],m_viewpointL[I_Y]);

	// Find out indices of the outmost vertices
	CRect2	polygonRectIdx(0,0,0,0);
	for( int i = 1 ; i < nPolygonVertices ; i++ ) {
		if( polygonVertices[i].x < polygonVertices[polygonRectIdx.left].x )	polygonRectIdx.left = i;
		if( polygonVertices[i].x > polygonVertices[polygonRectIdx.right].x )	polygonRectIdx.right = i;
		if( polygonVertices[i].y < polygonVertices[polygonRectIdx.top].y )	polygonRectIdx.top = i;
		if( polygonVertices[i].y > polygonVertices[polygonRectIdx.bottom].y )	polygonRectIdx.bottom = i;
	}
	m_polygonRect = CRect2((int)floor(polygonVertices[polygonRectIdx.left].x),
							(int)floor(polygonVertices[polygonRectIdx.top].y),
							(int)floor(polygonVertices[polygonRectIdx.right].x+1),
							(int)floor(polygonVertices[polygonRectIdx.bottom].y)+1);

	// Create zero reduction edges
	double	fMajorAxisZStep, fMinorAxisZStep;
	int		iMajorAxisStart0, iMajorAxisEnd0;
	int iL, iR, iL1, iR1, l, r, l1, r1, i; 
	double tanL, tanR;
	m_edgesOuter[I_L] = 0, m_edgesOuter[I_R] = 0;
	if( IsViewAlongX() ) { // creating edges alongX
		RTCHECK(m_polygonRect.Width()+(1<<nTerrainMaxReduction)*3<m_nMaxEdges,"Not enough memory for terrain");
		fMajorAxisZStep = CViewObject::m_viewPointDirMx.m[2][0];
		fMinorAxisZStep = -CViewObject::m_viewPointDirMx.m[2][2];
		iMajorAxisStart0 = m_polygonRect.left;
		iMajorAxisEnd0 = m_polygonRect.right;
		iL = iR = iL1 = iR1 = polygonRectIdx.left;
		l1 = l = (int)floor(polygonVertices[iL].y); r1 = r = l+1;
		int x = (int)floor(polygonVertices[iL].x)+1;
		bool	bLeaveEdgesLoop = FALSE;
		WRITELOG1("m_polygonRect = CRect2(%d,%d,%d,%d)\n",m_polygonRect);
		for( i = 0 ;; i++, x++ ) {	// edge addition loop
			while( polygonVertices[iL1].x < x ) {	// search for next edgeL span
				l = Min(l,(int)floor(polygonVertices[iL1].y));
				if( iL1 == polygonRectIdx.right ) { bLeaveEdgesLoop = TRUE; break; } //goto FinishCreateEdges;
				iL = iL1; if( ++iL1 >= nPolygonVertices ) iL1 = 0;
				CFVector2	d = polygonVertices[iL1]-polygonVertices[iL];
				tanL = d.y/Max(d.x,1e-3);
			}
			while( polygonVertices[iR1].x < x ) {	// search for next edgeR span
				r = Max(r,(int)floor(polygonVertices[iR1].y)+1);
				if( iR1 == polygonRectIdx.right ) { bLeaveEdgesLoop = TRUE; break; } //goto FinishCreateEdges;
				iR = iR1; if( iR1 <= 0 ) iR1 = nPolygonVertices; iR1--;
				CFVector2	d = polygonVertices[iR1]-polygonVertices[iR];
				tanR = d.y/Max(d.x,1e-3);
			}
			if( bLeaveEdgesLoop ) break;
			l = Min(l,l1=(int)floor(polygonVertices[iL].y+tanL*(x-polygonVertices[iL].x)));
			r = Max(r,r1=(int)floor(polygonVertices[iR].y+tanR*(x-polygonVertices[iR].x))+1);
			ASSERT(i<m_pnEdges[0]);
			m_ppEdges[0][i][I_L] = l; 
			m_ppEdges[0][i][I_R] = r;
			if( m_ppEdges[0][i][I_L] < m_ppEdges[0][m_edgesOuter[I_L]][I_L] ) m_edgesOuter[I_L] = i;
			if( m_ppEdges[0][i][I_R] > m_ppEdges[0][m_edgesOuter[I_R]][I_R] ) m_edgesOuter[I_R] = i;
			WRITELOG3("Edge[%d]: %d,%d\n",x-1,l,r);
			l = l1; r = r1;
		} // edge addition loop
	} // creating edges along X
	else { // creating edges along Y
		RTCHECK(m_polygonRect.Height()+(1<<nTerrainMaxReduction)*3<m_nMaxEdges,"Not enough memory for terrain");
		fMajorAxisZStep = -CViewObject::m_viewPointDirMx.m[2][2];
		fMinorAxisZStep = CViewObject::m_viewPointDirMx.m[2][0];
		iMajorAxisStart0 = m_polygonRect.top;
		iMajorAxisEnd0 = m_polygonRect.bottom;
		iL = iR = iL1 = iR1 = polygonRectIdx.top;
		l1 = l = (int)floor(polygonVertices[iL].x); r1 = r = l+1;
		int y = (int)floor(polygonVertices[iL].y)+1;
		bool	bLeaveEdgesLoop = FALSE;
		WRITELOG1("m_polygonRect = CRect2(%d,%d,%d,%d)\n",m_polygonRect);
		for( i = 0 ; /*y < m_polygonRect.bottom*/; i++, y++ ) {	// edge addition loop
			while( polygonVertices[iL1].y < y ) {	// search for next edgeL span
				l = Min(l,(int)floor(polygonVertices[iL1].x));
				if( iL1 == polygonRectIdx.bottom ) { bLeaveEdgesLoop = TRUE; break; } //goto FinishCreateEdges;
				iL = iL1; if( iL1 <= 0 ) iL1 = nPolygonVertices; iL1--;
				CFVector2	d = polygonVertices[iL1]-polygonVertices[iL];
				tanL = d.x/Max(d.y,1e-3);
			}
			while( polygonVertices[iR1].y < y ) {	// search for next edgeR span
				r = Max(r,(int)floor(polygonVertices[iR1].x)+1);
				if( iR1 == polygonRectIdx.bottom ) { bLeaveEdgesLoop = TRUE; break; } //goto FinishCreateEdges;
				iR = iR1; if( ++iR1 >= nPolygonVertices ) iR1 = 0;
				CFVector2	d = polygonVertices[iR1]-polygonVertices[iR];
				tanR = d.x/Max(d.y,1e-3);
			}
			if( bLeaveEdgesLoop ) break;
			l = Min(l,l1=(int)floor(polygonVertices[iL].x+tanL*(y-polygonVertices[iL].y)));
			r = Max(r,r1=(int)floor(polygonVertices[iR].x+tanR*(y-polygonVertices[iR].y))+1);
			ASSERT(i<m_pnEdges[0]);
			m_ppEdges[0][i][I_L] = l; m_ppEdges[0][i][I_R] = r;
			if( m_ppEdges[0][i][I_L] < m_ppEdges[0][m_edgesOuter[I_L]][I_L] ) m_edgesOuter[I_L] = i;
			if( m_ppEdges[0][i][I_R] > m_ppEdges[0][m_edgesOuter[I_R]][I_R] ) m_edgesOuter[I_R] = i;
			WRITELOG3("Edge[%d]: %d,%d\n",y-1,l,r);
			l = l1; r = r1;
		} // edge addition loop
	} // creating edges along Y
	//FinishCreateEdges:
	ASSERT(i==(IsViewAlongX()?m_polygonRect.Width():m_polygonRect.Height())-1);
	m_ppEdges[0][i][I_L] = l; m_ppEdges[0][i][I_R] = r;
	WRITELOG2("Edge[last]: %d,%d\n",l,r);

	int nEdges0 = i+1;
	ASSERT(nEdges0 < m_pnEdges[0]);
	ASSERT(nEdges0 == iMajorAxisEnd0-iMajorAxisStart0);


	// Needa provide convexibility of edges along y !!!
	{
		int outer = m_ppEdges[0][0][I_L];
		for( i = 1 ; i < m_edgesOuter[I_L] ; i++ ) {
			if( m_ppEdges[0][i][I_L] > outer )
				m_ppEdges[0][i][I_L] = outer;
			else
				outer = m_ppEdges[0][i][I_L];
		}

		outer = m_ppEdges[0][0][I_R];
		for( i = 1 ; i < m_edgesOuter[I_R] ; i++ ) {
			if( m_ppEdges[0][i][I_R] < outer )
				m_ppEdges[0][i][I_R] = outer;
			else
				outer = m_ppEdges[0][i][I_R];
		}

		outer = m_ppEdges[0][nEdges0-1][I_L];
		for( i = nEdges0-2 ; i > m_edgesOuter[I_L] ; i-- ) {
			if( m_ppEdges[0][i][I_L] > outer )
				m_ppEdges[0][i][I_L] = outer;
			else
				outer = m_ppEdges[0][i][I_L];
		}

		outer = m_ppEdges[0][nEdges0-1][I_R];
		for( i = nEdges0-2 ; i > m_edgesOuter[I_R] ; i-- ) {
			if( m_ppEdges[0][i][I_R] < outer )
				m_ppEdges[0][i][I_R] = outer;
			else
				outer = m_ppEdges[0][i][I_R];
		}
	}
	m_edgesOuter[I_L] += iMajorAxisStart0;
	m_edgesOuter[I_R] += iMajorAxisStart0;

	(void)iMajorAxisEnd0; (void)iMajorAxisStart0;
	(void)fMinorAxisZStep; (void)fMajorAxisZStep;
	{
		CFVector3	viewOffset = CViewObject::m_viewPointInvMx.Offset();
		#if 1
		m_zCoeff.x = -CViewObject::m_viewPointDirMx.m[2][0];
		m_zCoeff.y = -CViewObject::m_viewPointDirMx.m[2][1];
		m_zCoeff.z = -CViewObject::m_viewPointDirMx.m[2][2];
		m_zCoeff0 = m_zCoeff*viewOffset/-m_fCellSize;
		m_zCoeff.y *= m_fHScale;
		m_nNearestHeight = m_zCoeff.y < 0;
		// -CViewObject::m_viewPointDirMx.m[2][3]/m_fCellSize;
		#else
		CFVector2	view2( -CViewObject::m_viewPointDirMx.m[2][0],
							CViewObject::m_viewPointDirMx.m[2][2] );
		view2 = Normal(view2);
		m_zCoeff.x = view2.x;
		m_zCoeff.y = view2.y;
		m_zCoeff.z = 0;
		m_zCoeff0 = (m_zCoeff.x*viewOffset.x-m_zCoeff.y*viewOffset.z)/-m_fCellSize;
		#endif
	}


	#if _PRECOMPUTE_REDS
	{
		if( IsViewAlongX() ) {
			m_pReds = m_pRedsOrg0 - m_viewpointL[I_X]*m_nRedsP - m_viewpointL[I_Y];
		} else {
			m_pReds = m_pRedsOrg0 - m_viewpointL[I_Y]*m_nRedsP - m_viewpointL[I_X];
			// P=X  S=Y
		}
		// m_zCoeff.z has negative sign!!!!
		if( m_zCoeff.z < 0 ) {
			if( m_zCoeff.x >= 0 )	// Y+ X+
				DefineReductionsB();
			else					// Y+ X-
				DefineReductionsR();
		}
		else {
			if( m_zCoeff.x >= 0 )	// P- S+
				DefineReductionsL();
			else					// P- S-
				DefineReductionsT();
		}
	}
	#endif


	// Compute reductions for edges
	#if _ALIGN_EDGES		// !_INIT_CACHE
	if( IsViewAlongX() ) {
		for( i = 0 ; i < nEdges0 ; i++ ) {
			m_pEdgeR[i][I_L] = ReductionCell(m_polygonRect.left+i,m_ppEdges[0][i][I_L]);
			m_pEdgeR[i][I_R] = ReductionCell(m_polygonRect.left+i,m_ppEdges[0][i][I_R]-1);
		}
	}
	else {
		for( i = 0 ; i < nEdges0 ; i++ ) {
			m_pEdgeR[i][I_L] = ReductionCell(m_ppEdges[0][i][I_L],m_polygonRect.top+i);
			m_pEdgeR[i][I_R] = ReductionCell(m_ppEdges[0][i][I_R]-1,m_polygonRect.top+i);
		}
	}
	for( i = 0 ; i < nEdges0 ; i++ ) {
		int i1, l, r;
		l = Max((i+iMajorAxisStart0&-1<<m_pEdgeR[i][I_L])-iMajorAxisStart0,0);
		r = Min(l+(1<<m_pEdgeR[i][I_L]),nEdges0);
		for( i1 = l ; i1 < r ; i1++ )
			m_pEdgeR[i1][I_L] = Max(m_pEdgeR[i1][I_L],m_pEdgeR[i][I_L]);
		l = Max((i+iMajorAxisStart0&-1<<m_pEdgeR[i][I_R])-iMajorAxisStart0,0);
		r = Min(l+(1<<m_pEdgeR[i][I_R]),nEdges0);
		for( i1 = l ; i1 < r ; i1++ )
			m_pEdgeR[i1][I_R] = Max(m_pEdgeR[i1][I_R],m_pEdgeR[i][I_R]);
	}
	__polygonReductionCall = __reductionCall;

	// Create edges for non-zero reductions (mind nEdges0, iMajorAxisStart and
	// iMajorAxisEnd are modified and no longer valid after this)
	for( int red = 1 ; red < m_nReductions ; red++ ) {
		int red0 = red-1;
		int iMajorAxisStart = iMajorAxisStart0>>1,
			iMajorAxisEnd = iMajorAxisEnd0>>1;
		int nEdges = iMajorAxisEnd-iMajorAxisStart;
		ASSERT(nEdges<=m_pnEdges[red]);
		int i0 = 0;
		i = 0;
		if( iMajorAxisStart0 & 1 ) {
			m_ppEdges[red][i][I_L] = m_ppEdges[red0][i0][I_L];
			m_ppEdges[red][i++][I_R] = m_ppEdges[red0][i0++][I_R];
		}
		for( ; i < nEdges ; i++, i0+=2 ) {
			//CEdge	&e = m_ppEdges[red0][i];
			m_ppEdges[red][i][I_L] = Min(m_ppEdges[red0][i0][I_L],m_ppEdges[red0][i0+1][I_L]);
			m_ppEdges[red][i][I_R] = Max(m_ppEdges[red0][i0][I_R],m_ppEdges[red0][i0+1][I_R]);
		}
		if( iMajorAxisEnd0 & 1 ) {
			m_ppEdges[red][i][I_L] = m_ppEdges[red0][i0][I_L];
			m_ppEdges[red][i][I_R] = m_ppEdges[red0][i0][I_R];
			nEdges++; iMajorAxisEnd++;
		}
		ASSERT( i0 + (iMajorAxisEnd0 & 1) == nEdges0 );
		nEdges0 = nEdges;
		iMajorAxisStart0 = iMajorAxisStart;
		iMajorAxisEnd0 = iMajorAxisEnd;
	}
	#endif

	m_polygonRectL[I_LEFT] = m_polygonRect.left;
	m_polygonRectL[I_TOP] = m_polygonRect.top;
	m_polygonRectL[I_RIGHT] = m_polygonRect.right;
	m_polygonRectL[I_BOTTOM] = m_polygonRect.bottom;


	// normal = CViewObject::m_viewPointDirMx%CFVector3(0,1,0)
	m_waterNormal.a = CViewObject::m_viewPointDirMx.m[0][1];
	m_waterNormal.b = CViewObject::m_viewPointDirMx.m[1][1];
	m_waterNormal.c = CViewObject::m_viewPointDirMx.m[2][1];
	// d0 = normal0*CFVector3(0,terrainWLine*fHScale,0) = terrainWLine*fHScale
	// d = d0+normal*CViewObject::m_viewPointDirMx.Offset();
	m_waterNormal.d = (CViewFigure::Waterline()+
		CViewObject::m_viewPointDirMx.m[0][3]*CViewObject::m_viewPointDirMx.m[0][1]+
		CViewObject::m_viewPointDirMx.m[1][3]*CViewObject::m_viewPointDirMx.m[1][1]+
		CViewObject::m_viewPointDirMx.m[2][3]*CViewObject::m_viewPointDirMx.m[2][1]
		);


	#if _RC_COUNT
	if( __countRc ) {
		for( int i = 0 ; i < 512 ; i++ )
		for( int j = 0 ; j < 512 ; j++ )
			rcCount[i][j] = 255;
	}
	#endif

}

int __terrainLin = 0;
int __terrainVerts = 0;
int __terrainIntC = 1;
int	__terrainObjs = 1;

//double	fTerrainDistRed0 = 4;


inline	double	FASTLOG2(double x) { return log2(x); }
/*#pragma aux FASTLOG2 = \
		"fld1"	\
		"fxch"	\
		"fyl2x"	\
		parm [8087] value [8087]*/

inline	void FASTLOG2INT(double x, int *p) { *p = (int)log2(x); }
/*#pragma aux FASTLOG2INT = \
		"fld1"	\
		"fxch"	\
		"fyl2x"	\
		"fistp dword ptr [eax]" \
		parm [8087] [eax]*/


inline double	_CViewTerrain::Reduction(double z)
{
	#if _STATIC_REDUCTIONS
	(void)z;
	return 0;
	#else
	if( z < m_fDistRed0 ) return 0;
	return FASTLOG2(z/m_fDistRed0);
	//return	Min(log2(z/m_fDistRed0),(double)nTerrainMaxReduction);
	#endif
}

inline int	_CViewTerrain::ReductionInt(double z)
{
	#if _STATIC_REDUCTIONS
	(void)z;
	return 0;
	#else
	if( z < m_fDistRed02 ) return 0;
	int n;
	FASTLOG2INT(z/m_fDistRed0Sq2,&n);
	return n;
	//return	Min(log2(z/m_fDistRed0Sq2),(double)nTerrainMaxReduction);
	#endif
}

inline double	_CViewTerrain::Reduction(int x,int y)
{
	#if _REDUCTION_IMMEDIATE
	{
		double z = m_zCoeff.x*x+
							m_zCoeff.y*m_hrange.GetLimit(x,y,m_nNearestHeight,0)-
							m_zCoeff.z*y+m_zCoeff0;
		return	z < m_fDistRed0 ? 0 : (double)MAX_REDVERT(x,y);
	}
	#else



	#if _STATIC_REDUCTIONS
	(void)x, (void)y;
	return MAX_REDVERT(x,y);	//m_red(x,y)>>4;
	//return 0;
	#else // not static
	double r = Reduction(m_zCoeff.x*x+
							m_zCoeff.y*m_hrange.GetLimit(x,y,m_nNearestHeight,0)-
							m_zCoeff.z*y+m_zCoeff0);
	//ASSERT( MAX_REDVERT(x,y) <= nTerrainMaxReduction );
	#if _WATER_NOREDRESTRICT
	if( m_bIsAboveWaterCpl&& m_heights(x,y) <= __nTerrainWaterHighest ) return r;
	#endif // water

	return Min(r,(double)MAX_REDVERT(x,y));
	// Wrong way but who says things are easy
	#endif // static

	#endif	// reduction immediate
}

int  _CViewTerrain::ReductionCell(int x,int y)
{
	__reductionCall++;
	#if _STATIC_REDUCTIONS
		WRITELOG3("ReductionCell(%d,%d)=%d\n",x,y,(int)m_red(x,y)&15);
		(void)x; (void)y;
		return MAX_REDCELL(x,y); //m_red(x,y)&15;
	#else
	//int	x = m_vertexL[I_X], y = m_vertexL[I_Y];
	//if( __countRc && y >= 0 && y < 512 && x >= 0 && x < 512 && rcCount[y][x]<255 )
	//	rcCount[y][x]++;
	int maxRed = MAX_REDCELL(x,y);
	#if _WATER_NOREDRESTRICT
	if( m_bIsAboveWaterCpl /*&& m_heights(x,y) <= __nTerrainWaterHighest*/ ) {
		while( maxRed < nTerrainMaxReduction ) {
			int r = maxRed+1, d = 1<<r, mask=-d,
				x0 = x&mask, y0=y&mask, x1 = x0+d, y1 = y0+d;
			for( int yy = y0 ; yy <= y1 ; yy++ )
			for( int xx = x0 ; xx <= x1 ; xx++ )
				if( m_heights(xx,yy) > __nTerrainWaterHighest ) goto NoLandInvis;
			maxRed = r;
		}
	}
NoLandInvis:
	#endif
	if( maxRed == 0 ) return 0;
	#if _REDUCTION_IMMEDIATE
	for( int r = 1 ; r <= maxRed ; r++ ) {
		int mask = -1<<r;
		int x0 = x&mask, y0 = y&mask;
		//if( m_red(x0,y0) <= 0xC0 ) jlkjl
		double z = m_zCoeff.x*(x0+(m_nearestOffset.x<<r))+
							m_zCoeff.y*m_hrange.GetLimit(x0,y0,m_nNearestHeight,r)-
							m_zCoeff.z*(y0+(m_nearestOffset.y<<r))+m_zCoeff0;
		if( z < m_fDistRed0 ) return r-1;
	}
	return maxRed;
	#else

 	for( int n = 1 ; ; ) {
		ASSERT(n<20);
		int mask = -1<<n;
		int x0 = x&mask, y0 = y&mask;
		double z = m_zCoeff.x*(x0+(m_nearestOffset.x<<n))+
							m_zCoeff.y*m_hrange.GetLimit(x0,y0,m_nNearestHeight,n)-
							m_zCoeff.z*(y0+(m_nearestOffset.y<<n))+m_zCoeff0;
		int s = Min(ReductionInt(z),maxRed);
		//int s = (int)r;
		ASSERT( s >= n-1 );
		if( s <= n ) {
			ASSERT( s < m_nReductions );
			WRITELOG3("ReductionCell(%d,%d)=%d\n",x,y,s);
			return s;
			//return Min(s,MAX_REDCELL(x,y)); //Min(r,8.);
		}
		n = s;
	}
	#endif // immediate
	#endif // static
}

inline CVector2	_CViewTerrain::V2Mul_SHRD(TCSVector2 &s,int mul,int shift)
{
	return	CVector2( Mul_SHRD(s.x,mul,shift), Mul_SHRD(s.y,mul,shift) );
}

inline CVector2 _CViewTerrain::Vector2(word w)
{
	return	CVector2((w&0xFF00)<<8,(w&255)<<16);
}

//inline word _CViewTerrain::Word(TCCFVector2 &v)
//{
//	return	word(((word)(Round(v.x)<<8))+(byte)Round(v.y));
//}

#if 0
CVector2 _CViewTerrain::CalcWaterCoords(int x, int y)
{
   double xPhase, yPhase;
   double xAmpl, yAmpl;
   CVector2 base, add(0, 0);
   int tx, ty;
   int x0, y0, x1, y1;
   int n;
   int d;
   int fx, fy;
   int bx, by;

   d  = 16;
   n  = 3;

   bx = ((x/d)/n)*n;
   by = ((y/d)/n)*n;

   fx = bx % 2 == 0 ? 0 : 1;
   fy = by % 2 == 0 ? 0 : 1;

   x0 = fx  ? bx*d : bx*d+n*d;
   y0 = fy  ? by*d : by*d+n*d;
   x1 = !fx ? bx*d : bx*d+n*d;
   y1 = !fy ? by*d : by*d+n*d;

   tx = 1+(x-x0)*224/(x1-x0);
   ty = 1+(y-y0)*224/(y1-y0);

   base = CVector2(tx, ty);
   if(m_wp.m_transWater){
      xPhase = (m_curTime/m_wp.m_xwTimeP*2*M_PI) +
               (x % int(m_wp.m_xwTimeLandPx))/m_wp.m_xwTimeLandPx*2*M_PI+
               (y % int(m_wp.m_xwTimeLandPy))/m_wp.m_xwTimeLandPy*2*M_PI;
      yPhase = (m_curTime/m_wp.m_ywTimeP*2*M_PI) +
               (x % int(m_wp.m_ywTimeLandPx))/m_wp.m_ywTimeLandPx*2*M_PI+
               (y % int(m_wp.m_ywTimeLandPy))/m_wp.m_ywTimeLandPy*2*M_PI;
      xAmpl  = (m_wp.m_xwStaticA) + (m_wp.m_xwDynamicA)*sin(xPhase);
      yAmpl  = (m_wp.m_ywStaticA) + (m_wp.m_ywDynamicA)*cos(yPhase);
      add.x  = xAmpl*cos(xPhase);
      add.y  = yAmpl*sin(yPhase);
      base  += add;
      base.x = MinMax(long(base.x), long(1), long(254));
      base.y = MinMax(long(base.y), long(1), long(254));
   }
   ASSERT(base.x >= 1   && base.y >= 1 &&
          base.y <= 254 && base.y <= 254);

   base.x <<= 16;
   base.y <<= 16;

   return(base);
}
#endif


inline CVector2 _CViewTerrain::CalcWaterCoords(int x, int y)
{
   int xPhase, yPhase;
   CVector2 base;

   base = CVector2(m_twBaseX[x], m_twBaseY[y]);

   if(m_wp.m_transWater){
      xPhase  = m_curTimeX + m_twxPhaseX[x]+m_twyPhaseX[y];
      yPhase  = m_curTimeY + m_twxPhaseY[x]+m_twyPhaseY[y];
      base.x += m_twModulateX[xPhase & m_wp.m_phaseMask];
      base.y += m_twModulateY[yPhase & m_wp.m_phaseMask];
   }
   ASSERT(base.x >= 1 << 16   && base.y >= 1 << 16);
   ASSERT(base.x <= 254 << 16 && base.y <= 254 << 16);

   return(base);
}

inline void _CViewTerrain::CalcWaterBumpCoords()
{
	int xPhase, yPhase;
	int lx, ly;
	int i;

	if(m_bIsAboveWater)
	for(i = 0; i < 4; ++i){
	    lx = m_cellVerts[i].L[I_X];
	    ly = m_cellVerts[i].L[I_Y];

	    xPhase  = m_curTimeX + m_tbxPhaseX[lx]+m_tbyPhaseX[ly];
	    yPhase  = m_curTimeY + m_tbxPhaseY[lx]+m_tbyPhaseY[ly];

	    m_bumpVertsW[i].x = m_tbModulateX[xPhase & m_wp.m_phaseMask];
	    m_bumpVertsW[i].y = m_tbModulateY[yPhase & m_wp.m_phaseMask];
		ASSERT(m_bumpVertsW[i].x >= 1 << 16   && m_bumpVertsW[i].y >= 1 << 16);
		ASSERT(m_bumpVertsW[i].x <= 254 << 16 && m_bumpVertsW[i].y <= 254 << 16);
	}
	else
	for(i = 0; i < 4; ++i){
	    lx = m_cellVerts[3-i].L[I_X];
	    ly = m_cellVerts[3-i].L[I_Y];

	    xPhase  = m_curTimeX + m_tbxPhaseX[lx]+m_tbyPhaseX[ly];
	    yPhase  = m_curTimeY + m_tbxPhaseY[lx]+m_tbyPhaseY[ly];

	    m_bumpVertsW[i].x = m_tbModulateX[xPhase & m_wp.m_phaseMask];
	    m_bumpVertsW[i].y = m_tbModulateY[yPhase & m_wp.m_phaseMask];
		ASSERT(m_bumpVertsW[i].x >= 1 << 16   && m_bumpVertsW[i].y >= 1 << 16);
		ASSERT(m_bumpVertsW[i].x <= 254 << 16 && m_bumpVertsW[i].y <= 254 << 16);
	}
}

void _CViewTerrain::InitWaterTabulation()
{
   int i;
   int x0, x1;
   int n, d, f, b;
   int xAmpl, yAmpl;
   int wBegin, wEnd;
   int bBegin, bEnd;

   m_wp.m_phaseMask = m_wp.m_maxPhaseVal-1;
   m_wp.m_txwTimeP  = m_wp.m_maxPhaseVal/m_wp.m_xwTimeP;
   m_wp.m_tywTimeP  = m_wp.m_maxPhaseVal/m_wp.m_ywTimeP;
   m_wp.m_txbTimeP  = m_wp.m_maxPhaseVal/m_wp.m_xbTimeP;
   m_wp.m_tybTimeP  = m_wp.m_maxPhaseVal/m_wp.m_ybTimeP;

   d = 16; n  = 3;
   wBegin = Max(m_wp.m_xwStaticA+m_wp.m_xwDynamicA, m_wp.m_ywStaticA+m_wp.m_ywDynamicA);
   wEnd   = 254-wBegin;
   bBegin = Max(m_wp.m_xbStaticA+m_wp.m_xbDynamicA, m_wp.m_ybStaticA+m_wp.m_ybDynamicA);
   bEnd   = 254-bBegin;
   for(i = 0; i <= 512; ++i){
      b  = ((i/d)/n)*n;
      f  = b % 2 == 0 ? 0 : 1;
      x0 = f  ? b*d : b*d+n*d;
      x1 = !f ? b*d : b*d+n*d;

      m_twBaseX[i] = (wBegin+(i-x0)*224/(x1-x0)) << 16;
      m_twBaseY[i] = m_twBaseX[i];

      m_twBaseX[i] = MinMax(m_twBaseX[i], wBegin << 16, wEnd << 16);
      m_twBaseY[i] = MinMax(m_twBaseY[i], wBegin << 16, wEnd << 16);

      m_twxPhaseX[i] = m_wp.m_maxPhaseVal*(i % int(m_wp.m_xwTimeLandPx))/m_wp.m_xwTimeLandPx;
      m_twyPhaseX[i] = m_wp.m_maxPhaseVal*(i % int(m_wp.m_xwTimeLandPy))/m_wp.m_xwTimeLandPy;
      m_twxPhaseY[i] = m_wp.m_maxPhaseVal*(i % int(m_wp.m_ywTimeLandPx))/m_wp.m_ywTimeLandPx;
      m_twyPhaseY[i] = m_wp.m_maxPhaseVal*(i % int(m_wp.m_ywTimeLandPy))/m_wp.m_ywTimeLandPy;
      m_tbxPhaseX[i] = m_wp.m_maxPhaseVal*(i % int(m_wp.m_xbTimeLandPx))/m_wp.m_xbTimeLandPx;
      m_tbyPhaseX[i] = m_wp.m_maxPhaseVal*(i % int(m_wp.m_xbTimeLandPy))/m_wp.m_xbTimeLandPy;
      m_tbxPhaseY[i] = m_wp.m_maxPhaseVal*(i % int(m_wp.m_ybTimeLandPx))/m_wp.m_ybTimeLandPx;
      m_tbyPhaseY[i] = m_wp.m_maxPhaseVal*(i % int(m_wp.m_ybTimeLandPy))/m_wp.m_ybTimeLandPy;
   }
   for(i = 0; i < m_wp.m_maxPhaseVal; ++i){
      double phase;

      phase = double(i)/m_wp.m_maxPhaseVal*2*M_PI;

      xAmpl = (m_wp.m_xwStaticA << 16) + (m_wp.m_xwDynamicA << 16)*sin(phase);
      yAmpl = (m_wp.m_ywStaticA << 16) + (m_wp.m_ywDynamicA << 16)*cos(phase);
      m_twModulateX[i] = xAmpl*cos(phase);
      m_twModulateY[i] = yAmpl*sin(phase);
      xAmpl = (m_wp.m_xbStaticA << 16) + (m_wp.m_xbDynamicA << 16)*sin(phase);
      yAmpl = (m_wp.m_ybStaticA << 16) + (m_wp.m_ybDynamicA << 16)*cos(phase);
      m_tbModulateX[i] = xAmpl*cos(phase)+ (127 << 16);
      m_tbModulateY[i] = yAmpl*sin(phase)+ (127 << 16);
   }
}

inline int _CViewTerrain::_CalcPointReduction(int x,int y,int n,SVector2 &w,bool bLand,bool bWater)
{
	(void)w; (void)bWater;
	if( n <= 0 ) {
		#if _INTERPOLATE_COLORS
		if( bWater )
			w = CalcWaterCoords(x,y);
		#endif
		return !bLand ? 0 : m_heights(x,y)<<TERRAIN_HEIGHT_SHIFT;
	}
	int		d = 1<<n, mask = -1<<n,
			//x0 = d.x ? x&mask : x, y0 = d.y ? y&mask : y;
			x0 = x&mask, y0 = y&mask;
	int		dx = x-x0, dy = y-y0;
	//double	fx = dx/double(d), fy = dy/double(d);
	if( dx == dy ) {
		if( dx == 0 )	{
			#if _INTERPOLATE_COLORS
			if( bWater )
				w = CalcWaterCoords(x,y);
			#endif
			return !bLand ? 0 : m_heights(x0,y0)<<TERRAIN_HEIGHT_SHIFT;
		}
		//double	fy = dx/double(d);
		#if _INTERPOLATE_COLORS
		if( bWater )
			w = V2Mul_SHRD(CalcWaterCoords(x0+d,y0+d),dx,n)+
				V2Mul_SHRD(CalcWaterCoords(x0,y0),d-dx,n);
		#endif
		return !bLand ? 0 :
				Mul_SHRD(m_heights(x0+d,y0+d)<<TERRAIN_HEIGHT_SHIFT,dx,n)+
				Mul_SHRD(m_heights(x0,y0)<<TERRAIN_HEIGHT_SHIFT,d-dx,n);
	} else
	if( dx < dy ) {
		if( dx == 0 ) {
			//double	fy = dy/double(d);
			#if _INTERPOLATE_COLORS
			if( bWater )
				w = V2Mul_SHRD(CalcWaterCoords(x0,y0+d),dy,n)+
					V2Mul_SHRD(CalcWaterCoords(x0,y0),d-dy,n);
			#endif
			return !bLand ? 0 :
					Mul_SHRD(m_heights(x0,y0+d)<<TERRAIN_HEIGHT_SHIFT,dy,n)+
					Mul_SHRD(m_heights(x0,y0)<<TERRAIN_HEIGHT_SHIFT,d-dy,n);
		}
		//double	fx = dx/double(d), fy = dy/double(d);
		#if _INTERPOLATE_COLORS
		if( bWater )
			w = V2Mul_SHRD(CalcWaterCoords(x0+d,y0+d),dx,n)+
				V2Mul_SHRD(CalcWaterCoords(x0,y0+d),dy-dx,n)+
				V2Mul_SHRD(CalcWaterCoords(x0,y0),d-dy,n);
		#endif
		return !bLand ? 0 :
				Mul_SHRD(m_heights(x0+d,y0+d)<<TERRAIN_HEIGHT_SHIFT,dx,n)+
				Mul_SHRD(m_heights(x0,y0+d)<<TERRAIN_HEIGHT_SHIFT,dy-dx,n)+
				Mul_SHRD(m_heights(x0,y0)<<TERRAIN_HEIGHT_SHIFT,d-dy,n);
	} else
	{ // dx > dy
		if( dy == 0 ) {
			//double	fx = dx/double(d);
			#if _INTERPOLATE_COLORS
			if( bWater )
				w = V2Mul_SHRD(CalcWaterCoords(x0+d,y0),dx,n)+
					V2Mul_SHRD(CalcWaterCoords(x0,y0),d-dx,n);
			#endif
			return !bLand ? 0 :
					Mul_SHRD(m_heights(x0+d,y0)<<TERRAIN_HEIGHT_SHIFT,dx,n)+
					Mul_SHRD(m_heights(x0,y0)<<TERRAIN_HEIGHT_SHIFT,d-dx,n);
		}
		//double	fx = dx/double(d), fy = dy/double(d);
		#if _INTERPOLATE_COLORS
		if( bWater )
			w = V2Mul_SHRD(CalcWaterCoords(x0+d,y0+d),dy,n)+
				V2Mul_SHRD(CalcWaterCoords(x0+d,y0),dx-dy,n)+
				V2Mul_SHRD(CalcWaterCoords(x0,y0),d-dx,n);
		#endif
		return !bLand ? 0 :
				Mul_SHRD(m_heights(x0+d,y0+d)<<TERRAIN_HEIGHT_SHIFT,dy,n)+
				Mul_SHRD(m_heights(x0+d,y0)<<TERRAIN_HEIGHT_SHIFT,dx-dy,n)+
				Mul_SHRD(m_heights(x0,y0)<<TERRAIN_HEIGHT_SHIFT,d-dx,n);
	}
}


inline bool  _CViewTerrain::IsWaterCell(int x, int y)
{
    /*int h0, h1, h2, h3;
	int d;

	d = 1 << (m_red(x, y) & 0x07);

	h0 = m_heights(x  , y  ) <= __terrainWaterline;
	h1 = m_heights(x+d, y  ) <= __terrainWaterline;
	h2 = m_heights(x  , y+d) <= __terrainWaterline;
	h3 = m_heights(x+d, y+d) <= __terrainWaterline;

    return(h0 || h1 || h2 || h3);
	*/

    /*int r;

    r = m_red(x, y) >> 6;

    return(r <= 0x01);*/
	//int i1 = m_red(x,y) <= byte(0x01*0x80-1);
	//int i2 = (m_red(x,y)>>6) <= 0x01;
	//ASSERT( i1 == i2 );
	return	m_red(x,y) <= byte(0x01*0x80-1);
	//return	(m_red(x,y)>>6) <= 0x01;
}

inline bool _CViewTerrain::IsWaterVertex(int x, int y)
{
	(void)x; (void)y;
    /*int r;

    r = m_red(x, y) >> 6;

    return(!(r == 0x03));*/
	//int i1 = (m_red(x,y) & 0xC0) != 0xC0;
	//int i2 = (m_red(x,y)>>6) != 3;
	//ASSERT( i1 == i2 );
	//return	(m_red(x,y) & 0xC0) != 0xC0;
	return	m_red(x,y) < 0xC0;
	//return (m_red(x,y)>>6)!=3;
	//return 1;
}

inline	int	_CViewTerrain::CalcPointReduction(int x,int y,int n,SVector2 &w,bool bLand,bool bWater)
{
	if( bWater ) {
		if( bLand )
			return	_CalcPointReduction(x,y,n,w,1,1);
		else
			return	_CalcPointReduction(x,y,n,w,0,1);
	}
	else {
		if( bLand )
			return	_CalcPointReduction(x,y,n,w,1,0);
		else
			return	_CalcPointReduction(x,y,n,w,0,0);
	}
}

void _CViewTerrain::CreateCellVertex(int x,int y,_CViewTerrain::SVert3W &v)
{
	bool	bIsWater = IsWaterVertex(x,y);
	bool	bNeedLand = TRUE;
	if( m_bIsAboveWaterCpl ) {
		if( m_red(x,y) < 0x40 )	bNeedLand = 0;
		//if( (m_red(x,y) & 0xC0) == 0 )	bNeedLand = 0;
	} else
	if( m_bIsBelowWaterCpl ) {
		if( m_red(x,y) >= 0xC0 )	bNeedLand = 0;
		//if( (m_red(x,y) & 0xC0) == 0xC0 )	bNeedLand = 0;
	}
	//bNeedLand = TRUE;

	//double		h0;
	//CFVector3	v3;
	#if _INTERPOLATE_VERTICES
	//int		x = m_vertexL[I_X], y = m_vertexL[I_Y];
	double	r = Reduction(x,y);
	//WRITELOG3("\t[%d,%d]=%lg\n",x,y,r);
	int		n = (int)r;
	#if !_STATIC_REDUCTIONS
	int		f;
	D2INT(&f,(r-n)*65536);

	//f = (f-=32768)<=0?0:(f<<1);

	if( /*((x|y)&((1<<n+1)-1)) &&*/ f ) {
		CVector2	w, wpp;
		int			h, hpp;
		h = CalcPointReduction(x,y,n,w,bNeedLand,bIsWater);
		if( bIsWater ) hpp = CalcPointReduction(x,y,n+1,wpp,0 /*bNeedLand*/,bIsWater);
		v.v3 = m_viewPointDirMx*CFVector3(x,v.h0=h/double(1<<TERRAIN_HEIGHT_SHIFT),y);
		//v3 = m_viewPointDirMx*CFVector3(x,
		//			h0=(Mul_SHRD(h,65536-f,16)+Mul_SHRD(hpp,f,16))/double(1<<TERRAIN_HEIGHT_SHIFT),y);
		if( bIsWater ) v.w.SetColor( V2Mul_SHRD(w,65536-f,16)+V2Mul_SHRD(wpp,f,16) );
		/*#if _INTERPOLATE_COLORS
		if( __terrainIntC )
			v.SetColor( V2Mul_SHRD(w,65536-f,16)+V2Mul_SHRD(wpp,f,16) );
		else
		#endif
			v.SetColor(m_colors(x,y));
		*/
	}
	else
	#endif
	if( n > 0 ) {
		CVector2	w;
		int h;
		h = CalcPointReduction(x,y,n,w,bNeedLand,bIsWater);
		v.v3 = m_viewPointDirMx*CFVector3(x,v.h0=h/double(1<<TERRAIN_HEIGHT_SHIFT),y);
		if( bIsWater ) v.w.SetColor(w);
		/*#if _INTERPOLATE_COLORS
		if( __terrainIntC )
			v.SetColor(w);
		else
		#endif
			v.SetColor(m_colors(x,y));
		*/
	}
	else
	#endif // INTERPOLATE_VERTICES
	{
		v.v3 = m_viewPointDirMx*CFVector3(x,v.h0=m_heights(x,y),y);
		if( bIsWater ) v.w.SetColor(CalcWaterCoords(x,y));
	}


	//w = FVector2(m_colors(x,y));
	//h = m_heights(x,y);

	if( bNeedLand ) {
		v.SetColor(m_colors(x,y),m_nHiResShift);
		if( m_bIsHardware ) v.b = m_brightness(x,y);

		//v.x3 = v3.x; //D2INT(&v.x3,v3.x*nTerrainPrec);
		//v.y3 = v3.y; //D2INT(&v.y3,v3.y*nTerrainPrec);
		//v.z3 = v3.z; //D2INT(&v.z3,v3.z*nTerrainPrec);
		if( v.v3.z <= CViewObject::m_fFrontClip ) v.cf = 1; else
		if(	v.v3.z >= CViewObject::m_fBackClip ) v.cf = 2; else {
			double _1z = 1./v.v3.z;
			D2INT(&v.x,v.v3.x*_1z); //v.x3/v.z3;
			D2INT(&v.y,v.v3.y*_1z); //v.y3/v.z3;
			//v.iz = (65536*nTerrainPrec)/v.z3;
			D2INT(&v.iz,_1z*CViewObject::m_fz1MinScale /*65536.*/);
			v.cf = 0;
		}
		v.lf = 1;
	}
	else
		v.lf = 0;
	v.L[I_X] = x;
	v.L[I_Y] = y;


	#if _TERRAIN_WATER
		if( bIsWater ) {		// x, y  - in cells
			v.wf = 1; //h0 >= __terrainWaterline ? VF_WATER_DATA :
					  //				VF_WATER_DATA | VF_BELOW_WATER;
			//D2INT(&v.h0,h0*nTerrainHPrec);
			// __terrainWaterline in terrain units of height
			v.w.v3 = m_viewPointDirMx*CFVector3(x,__terrainWaterline,y);
			//D2INT(&v.w.x3,v3.x*nTerrainPrec);
			//D2INT(&v.w.y3,v3.y*nTerrainPrec);
			//D2INT(&v.w.z3,v3.z*nTerrainPrec);
			if( v.w.v3.z <= CViewObject::m_fFrontClip ) v.w.cf = 1; else
			if(	v.w.v3.z >= CViewObject::m_fBackClip ) v.w.cf = 2; else {
				double _1z = 1./v.w.v3.z;
				D2INT(&v.w.x,v.w.v3.x*_1z); //v.w.x = v.w.x3/v.w.z3;
				D2INT(&v.w.y,v.w.v3.y*_1z); //v.w.y = v.w.y3/v.w.z3;
				//v.w.iz = (65536*nTerrainPrec)/v.w.z3;
				D2INT(&v.w.iz,_1z*CViewObject::m_fz1MinScale /*65536.*/);
				v.w.cf = 0;
			}
			#if 0

	        double xPhase, yPhase;
	        double xAmpl, yAmpl;
	        CVector2 base, add(0, 0);

			base = CVector2((x % 250) << 16, (y % 250) << 16);  // fixme
	        if(m_wp.m_transWater){
	           ASSERT(m_red(x, y) & 0x40);

	           xPhase = (m_curTime/m_wp.m_xwTimeP*2*M_PI) +
	                    (x % int(m_wp.m_xwTimeLandPx))/m_wp.m_xwTimeLandPx*2*M_PI+
	                    (y % int(m_wp.m_xwTimeLandPy))/m_wp.m_xwTimeLandPy*2*M_PI;
	           yPhase = (m_curTime/m_wp.m_ywTimeP*2*M_PI) +
	                    (x % int(m_wp.m_ywTimeLandPx))/m_wp.m_ywTimeLandPx*2*M_PI+
	                    (y % int(m_wp.m_ywTimeLandPy))/m_wp.m_ywTimeLandPy*2*M_PI;
	           xAmpl  = (m_wp.m_xwStaticA << 16) + (m_wp.m_xwDynamicA << 16)*sin(xPhase);
	           yAmpl  = (m_wp.m_ywStaticA << 16) + (m_wp.m_ywDynamicA << 16)*cos(yPhase);
	           add.x  = xAmpl*cos(xPhase);
	           add.y  = yAmpl*sin(yPhase);
	           base  += add;
	           base.x = MinMax(long(base.x), long(1 << 16), long(254 << 16));
	           base.y = MinMax(long(base.y), long(1 << 16), long(254 << 16));
	        }
	        v.w.u = base.x;
	        v.w.v = base.y;
			#endif
		}
		else
	#endif
			v.wf = 0;
}

inline dword _CViewTerrain::CreateHWColor(dword b)
{
	return	(b<<24)+(b<<16)+(b<<8);
}

inline void _CViewTerrain::InterpolateClipped(double fh,double dh,
		const _CViewTerrain::SVert3W &v0, TCSVector2 &bump0,
		const _CViewTerrain::SVert3W &v1, TCSVector2 &bump1, int &ngr, bool bBump, bool bGouraud )
{
	ASSERT( dh >= 0 && fh <= dh && fh >= 0 );
	//int t = SHLD_Div(fh,dh,CViewObject::CLIP_SHIFT);
	double t = fh/Max(dh,1e-5), _1_t = 1.-t;
	int nt;
	D2INT(&nt,CViewObject::CLIP_PREC*t);
	//int z3 = Mul_SHRD(v1.z3,t,CViewObject::CLIP_SHIFT)+
	//		  Mul_SHRD(v0.z3,(CViewObject::CLIP_PREC-t),CViewObject::CLIP_SHIFT);
	double _1z = 1./(v1.v3.z*t+v0.v3.z*_1_t);
	//_gr_vertices[ngr].any.x =
	//	(Mul_SHRD(v1.x3,t,CViewObject::CLIP_SHIFT)+
	//		Mul_SHRD(v0.x3,(CViewObject::CLIP_PREC-t),CViewObject::CLIP_SHIFT))/z3;
	D2INT(&_gr_vertices[ngr].any.x,(v1.v3.x*t+v0.v3.x*_1_t)*_1z);
	//_gr_vertices[ngr].any.y =
	//	(Mul_SHRD(v1.y3,t,CViewObject::CLIP_SHIFT)+
	//		Mul_SHRD(v0.y3,(CViewObject::CLIP_PREC-t),CViewObject::CLIP_SHIFT))/z3;
	D2INT(&_gr_vertices[ngr].any.y,(v1.v3.y*t+v0.v3.y*_1_t)*_1z);
	//_gr_vertices[ngr].any.iz = (65536*nTerrainPrec)/z3;
	D2INT(&_gr_vertices[ngr].any.iz,_1z*CViewObject::m_fz1MinScale /*65536.*/);
	_gr_vertices[ngr].texture.u =
		Mul_SHRD(v1.u,nt,CViewObject::CLIP_SHIFT)+
		Mul_SHRD(v0.u,(CViewObject::CLIP_PREC-nt),CViewObject::CLIP_SHIFT);
	_gr_vertices[ngr].texture.v =
		Mul_SHRD(v1.v,nt,CViewObject::CLIP_SHIFT)+
		Mul_SHRD(v0.v,(CViewObject::CLIP_PREC-nt),CViewObject::CLIP_SHIFT);
	if( bGouraud )
		_gr_vertices[ngr].gouraud.b = CreateHWColor(
			Mul_SHRD(v1.b,nt,CViewObject::CLIP_SHIFT)+
			Mul_SHRD(v0.b,(CViewObject::CLIP_PREC-nt),CViewObject::CLIP_SHIFT));
	if( bBump ) {
		_gr_vertices[ngr].bump.u =
			Mul_SHRD(bump1.x,nt,CViewObject::CLIP_SHIFT)+
			Mul_SHRD(bump0.x,(CViewObject::CLIP_PREC-nt),CViewObject::CLIP_SHIFT);
		_gr_vertices[ngr].bump.v =
			Mul_SHRD(bump1.y,nt,CViewObject::CLIP_SHIFT)+
			Mul_SHRD(bump0.y,(CViewObject::CLIP_PREC-nt),CViewObject::CLIP_SHIFT);
	}
	ngr++;
}

inline bool _CViewTerrain::CalcTriangleNormal(_CViewTerrain::SVert3_0 **ppCellVerts,int t0,int t1,int t2,int nWater)
{
	//if( !_gr_polygon.nLights )	return TRUE;
	if( nWater ) {
		if( !_gr_polygon.nLights )	return TRUE;
		if( nWater > 0 ) {
			_gr_polygon.a = m_waterNormal.a;
			_gr_polygon.b = m_waterNormal.b;
			_gr_polygon.c = m_waterNormal.c;
			_gr_polygon.d = m_waterNormal.d;
		}
		else {
			_gr_polygon.a = -m_waterNormal.a;
			_gr_polygon.b = -m_waterNormal.b;
			_gr_polygon.c = -m_waterNormal.c;
			_gr_polygon.d = -m_waterNormal.d;
		}
		#if 0
		// normal = CViewObject::m_viewPointDirMx%CFVector3(0,1,0)
		_gr_polygon.a = CViewObject::m_viewPointDirMx.m[0][1];
		_gr_polygon.b = CViewObject::m_viewPointDirMx.m[1][1];
		_gr_polygon.c = CViewObject::m_viewPointDirMx.m[2][1];
		// d0 = normal0*CFVector3(0,terrainWLine*fHScale,0) = terrainWLine*fHScale
		// d = d0+normal*CViewObject::m_viewPointDirMx.Offset();
		_gr_polygon.d = (CViewFigure::Waterline()+
			CViewObject::m_viewPointDirMx.m[0][3]*CViewObject::m_viewPointDirMx.m[0][1]+
			CViewObject::m_viewPointDirMx.m[1][3]*CViewObject::m_viewPointDirMx.m[1][1]+
			CViewObject::m_viewPointDirMx.m[2][3]*CViewObject::m_viewPointDirMx.m[2][1]
			);
		#endif
		return TRUE;
	}

	SVector2 n;
	if( !_gr_polygon.nLights && m_nLandClipFlags == 1 ) {	// fully out of clip
		n.x = ppCellVerts[t2]->y-ppCellVerts[t0]->y;
		n.y = ppCellVerts[t2]->x-ppCellVerts[t0]->x;
		if( n.y*(ppCellVerts[t1]->y-ppCellVerts[t0]->y) <
			n.x*(ppCellVerts[t1]->x-ppCellVerts[t0]->x)
			- abs(n.x)-abs(n.y)
		) return FALSE;
	}

    //CFVector3   v0(m_cellVerts[t0].x3*CViewObject::m_viewPointScale_1.x,m_cellVerts[t0].y3*CViewObject::m_viewPointScale_1.y,m_cellVerts[t0].z3*CViewObject::m_viewPointScale_1.z);
    //CFVector3   v1(m_cellVerts[t1].x3*CViewObject::m_viewPointScale_1.x,m_cellVerts[t1].y3*CViewObject::m_viewPointScale_1.y,m_cellVerts[t1].z3*CViewObject::m_viewPointScale_1.z);
    //CFVector3   v2(m_cellVerts[t2].x3*CViewObject::m_viewPointScale_1.x,m_cellVerts[t2].y3*CViewObject::m_viewPointScale_1.y,m_cellVerts[t2].z3*CViewObject::m_viewPointScale_1.z);
    CFVector3   v0(ppCellVerts[t0]->v3.x*CViewObject::m_viewPointScale_1.x,ppCellVerts[t0]->v3.y*CViewObject::m_viewPointScale_1.y,ppCellVerts[t0]->v3.z*CViewObject::m_viewPointScale_1.z);
    CFVector3   v1(ppCellVerts[t1]->v3.x*CViewObject::m_viewPointScale_1.x,ppCellVerts[t1]->v3.y*CViewObject::m_viewPointScale_1.y,ppCellVerts[t1]->v3.z*CViewObject::m_viewPointScale_1.z);
    CFVector3   v2(ppCellVerts[t2]->v3.x*CViewObject::m_viewPointScale_1.x,ppCellVerts[t2]->v3.y*CViewObject::m_viewPointScale_1.y,ppCellVerts[t2]->v3.z*CViewObject::m_viewPointScale_1.z);
    CFVector3	normal = (v1-v0)%(v2-v1);
	//double  d = normal*v0;
	if(!_gr_polygon.nLights) return TRUE; //d<=0;
	double	n0 = 1./Abs(normal);
	_gr_polygon.a = normal.x = normal.x*n0;
	_gr_polygon.b = normal.y = normal.y*n0;
	_gr_polygon.c = normal.z = normal.z*n0;
    _gr_polygon.d = /*(d*n0)*/ (normal*v0); // *(1./nTerrainPrec);

	if( m_nLandClipFlags == 1 ) {	// fully out of clip
		n.x = ppCellVerts[t2]->y-ppCellVerts[t0]->y;
		n.y = ppCellVerts[t2]->x-ppCellVerts[t0]->x;
		if( n.y*(ppCellVerts[t1]->y-ppCellVerts[t0]->y) <
			n.x*(ppCellVerts[t1]->x-ppCellVerts[t0]->x)
			- abs(n.x)-abs(n.y)
		) return FALSE;
	}

	return TRUE; //d <= 0;
}


#define	SPLITDIFF(X,Y)	(nSplitUp?X-Y:Y-X)
void _CViewTerrain::DrawTriangleSplit(bool nSplitUp,_CViewTerrain::SVert3W **ppCellVerts,int iVert1,int iVertT,int iVert2,int nCellVerts,int mask,
											bool bCellBump,SVector2 *pBumpVerts,int nWater,bool bGouraud)
{
	(void)iVertT;
	if( nWater ) {
        _gr_polygon.hTexture = m_hMaskW; //.Texture();
        _gr_polygon.hBump = m_hBumpW; //.Texture();
	} else
	if( mask == 0 ) {
        _gr_polygon.hTexture = m_hMask; //.Texture();
        _gr_polygon.hBump = m_hBump; //.Texture();
	}
	else {
        _gr_polygon.hTexture = m_hMask1; //.Texture();
        _gr_polygon.hBump = m_hBump; //.Texture();
	}
	#if _USE_GLIDE
		_gr_polygon.fill.texture.nColor = 0;
	#endif

	int   i, ngr;
	double	dh;

	dword	dwHazeFlag = GR_POLY_ADD_NONE;
	ngr = 0;
	int		pass = 0, iTemp = 0;
	bool	hcfTemp;
	SVert3W	temps[2], *pTemp = NULL;
	SVector2	bumps[2], *pBump;
	for( i = iVert1 ; ; ) {
		SVert3W	&vert = *ppCellVerts[i];
		if( vert.v3.z >= CViewFigure::HazeMin() /**nTerrainPrec*/ ) dwHazeFlag = GR_POLY_ADD_HAZE;
		int iP, iN;
		if( vert.cf ) {
			double	 fClipD = vert.cf != 2 ? CViewObject::m_fFrontClip : CViewObject::m_fBackClip;
			SVert3W  &vertP = *ppCellVerts[iP = i == iVert1 ? iVert2 : (i > 0 ? i-1 : nCellVerts-1)],
					 &vertN = *ppCellVerts[iN = i==iVert2 ? iVert1 : (i+1 < nCellVerts ? i+1 : 0)];
			if( vertP.cf != vert.cf ) {
				//int	t = SHLD_IDiv(nClipD-vert.z3,vertP.z3-vert.z3,CViewObject::CLIP_SHIFT);
				double  t = (fClipD-vert.v3.z)/AbsNoLess(vertP.v3.z-vert.v3.z,1e-5);
				int nt; D2INT(&nt,t*CViewObject::CLIP_PREC);
				SVert3W &temp = temps[iTemp]; SVector2 *pbump;
				temp.v3.x = vert.v3.x+(vertP.v3.x-vert.v3.x)*t;
				temp.v3.y = vert.v3.y+(vertP.v3.y-vert.v3.y)*t;
				//temp.y3 = vert.y3+Mul_SHRD(vertP.y3-vert.y3,t,CViewObject::CLIP_SHIFT);
				temp.v3.z = fClipD;
				temp.u = vert.u+Mul_SHRD(vertP.u-vert.u,nt,CViewObject::CLIP_SHIFT);
				temp.v = vert.v+Mul_SHRD(vertP.v-vert.v,nt,CViewObject::CLIP_SHIFT);
				temp.h0 = vert.h0+(vertP.h0-vert.h0)*t;
				if( bGouraud )
					temp.b = vert.b+Mul_SHRD(vertP.b-vert.b,nt,CViewObject::CLIP_SHIFT);
				if( bCellBump ) {
					pbump = &bumps[iTemp];
					pbump->x = pBumpVerts[i].x+Mul_SHRD(pBumpVerts[iP].x-pBumpVerts[i].x,nt,CViewObject::CLIP_SHIFT);
					pbump->y = pBumpVerts[i].y+Mul_SHRD(pBumpVerts[iP].y-pBumpVerts[i].y,nt,CViewObject::CLIP_SHIFT);
				}
				iTemp ^= 1;
				if( (dh = SPLITDIFF(temp.h0,m_fWaterline0)) >= 0 ) {
					if( pTemp && hcfTemp )
						InterpolateClipped(dh,SPLITDIFF(temp.h0,pTemp->h0),temp,*pbump,*pTemp,*pBump,ngr,bCellBump,bGouraud);
					if( pass ) break;
					double _1z = 1./temp.v3.z;
					D2INT(&_gr_vertices[ngr].texture.x,temp.v3.x*_1z);
					D2INT(&_gr_vertices[ngr].texture.y,temp.v3.y*_1z);
					_gr_vertices[ngr].texture.u = temp.u;
					_gr_vertices[ngr].texture.v = temp.v;
					//_gr_vertices[ngr].texture.iz = (65536*nTerrainPrec)/temp.z3;
					D2INT(&_gr_vertices[ngr].texture.iz,/*65536**/ _1z*CViewObject::m_fz1MinScale);
					if( bGouraud )
						_gr_vertices[ngr].gouraud.b = CreateHWColor(temp.b);
					if( bCellBump ) {
						_gr_vertices[ngr].bump.u = pbump->x;
						_gr_vertices[ngr].bump.v = pbump->y;
						pBump = pbump;
					}
					pTemp = &temp; hcfTemp = FALSE;
					ngr++;
				} // if above
				else { // if below
					if( pTemp && !hcfTemp )
						InterpolateClipped(-dh,SPLITDIFF(pTemp->h0,temp.h0),temp,*pbump,*pTemp,*pBump,ngr,bCellBump,bGouraud);
					if( pass ) break;
					pTemp = &temp; pBump = pbump; hcfTemp = TRUE;
				}
			} // prev Z-clipped
			if( vertN.cf != vert.cf ) {
				//int	t = SHLD_IDiv(nClipD-vert.z3,vertN.z3-vert.z3,CViewObject::CLIP_SHIFT);
				double t = (fClipD-vert.v3.z)/AbsNoLess(vertN.v3.z-vert.v3.z,1e-5);
				int nt; D2INT(&nt,t*CViewObject::CLIP_PREC);
				SVert3W &temp = temps[iTemp]; SVector2 *pbump;
				temp.v3.x = vert.v3.x+(vertN.v3.x-vert.v3.x)*t;
				temp.v3.y = vert.v3.y+(vertN.v3.y-vert.v3.y)*t;
				temp.v3.z = fClipD;
				temp.u = vert.u+Mul_SHRD(vertN.u-vert.u,nt,CViewObject::CLIP_SHIFT);
				temp.v = vert.v+Mul_SHRD(vertN.v-vert.v,nt,CViewObject::CLIP_SHIFT);
				temp.h0 = vert.h0+(vertN.h0-vert.h0)*t;
				if( bGouraud )
					temp.b = vert.b+Mul_SHRD(vertN.b-vert.b,nt,CViewObject::CLIP_SHIFT);
				if( bCellBump ) {
					pbump = &bumps[iTemp];
					pbump->x = pBumpVerts[i].x+Mul_SHRD(pBumpVerts[iN].x-pBumpVerts[i].x,nt,CViewObject::CLIP_SHIFT);
					pbump->y = pBumpVerts[i].y+Mul_SHRD(pBumpVerts[iN].y-pBumpVerts[i].y,nt,CViewObject::CLIP_SHIFT);
				}
				iTemp ^= 1;
				if( (dh = SPLITDIFF(temp.h0,m_fWaterline0)) >= 0 ) {
					if( pTemp && hcfTemp )
						InterpolateClipped(dh,SPLITDIFF(temp.h0,pTemp->h0),temp,*pbump,*pTemp,*pBump,ngr,bCellBump,bGouraud);
					if( pass ) break;
					double _1z = 1./temp.v3.z;
					D2INT(&_gr_vertices[ngr].texture.x,temp.v3.x*_1z);
					D2INT(&_gr_vertices[ngr].texture.y,temp.v3.y*_1z);
					_gr_vertices[ngr].texture.u = temp.u;
					_gr_vertices[ngr].texture.v = temp.v;
					//_gr_vertices[ngr].texture.iz = (65536*nTerrainPrec)/temp.z3;
					D2INT(&_gr_vertices[ngr].texture.iz,_1z*/*65536.*/CViewObject::m_fz1MinScale);
					if( bGouraud )
						_gr_vertices[ngr].gouraud.b = CreateHWColor(temp.b);
					if( bCellBump ) {
						_gr_vertices[ngr].bump.u = pbump->x;
						_gr_vertices[ngr].bump.v = pbump->y;
						pBump = pbump;
					}
					pTemp = &temp; hcfTemp = FALSE;
					ngr++;
				} // if above
				else { // if below
					if( pTemp && !hcfTemp )
						InterpolateClipped(-dh,SPLITDIFF(pTemp->h0,temp.h0),temp,*pbump,*pTemp,*pBump,ngr,bCellBump,bGouraud);
					if( pass ) break;
					pTemp = &temp; pBump = pbump; hcfTemp = TRUE;
				}
			} // next Z-clipped
		}
		else {
			SVert3W &temp = temps[iTemp]; SVector2 *pbump;
			*(SVert3_0*)&temp = *(SVert3_0*)&vert;
			temp.h0 = vert.h0;
			if( bGouraud )
				temp.b = vert.b;
			if( bCellBump ) {
				pbump = &bumps[iTemp];
				pbump->x = pBumpVerts[i].x;
				pbump->y = pBumpVerts[i].y;
			}
			iTemp ^= 1;
			if( (dh = SPLITDIFF(temp.h0,m_fWaterline0)) >= 0 ) {
				if( pTemp && hcfTemp )
					InterpolateClipped(dh,SPLITDIFF(temp.h0,pTemp->h0),temp,*pbump,*pTemp,*pBump,ngr,bCellBump,bGouraud);
				if( pass ) break;
				_gr_vertices[ngr].texture.x = temp.x;
				_gr_vertices[ngr].texture.y = temp.y;
				_gr_vertices[ngr].texture.u = temp.u;
				_gr_vertices[ngr].texture.v = temp.v;
				_gr_vertices[ngr].texture.iz = temp.iz;
				if( bGouraud )
					_gr_vertices[ngr].gouraud.b = CreateHWColor(temp.b);
				if( bCellBump ) {
					_gr_vertices[ngr].bump.u = pbump->x;
					_gr_vertices[ngr].bump.v = pbump->y;
					pBump = pbump;
				}
				pTemp = &temp; hcfTemp = FALSE;
				ngr++;
			} // if above waterline
			else {
				if( pTemp && !hcfTemp )
					InterpolateClipped(-dh,SPLITDIFF(pTemp->h0,temp.h0),temp,*pbump,*pTemp,*pBump,ngr,bCellBump,bGouraud);
				if( pass ) break;
				pTemp = &temp; pBump = pbump; hcfTemp = TRUE;
			} // if below waterline
		} // not Z-clipped
		if( i == iVert2 )	{
			if( ngr <= 0 ) break;
			ASSERT( ngr >= 2 );
			i = iVert1; pass = 1;
		} else
			i = i+1 < nCellVerts ? i+1 : 0;
		//if( i == iVert2 ) break;
	}
	ASSERT( ngr == 0 || ngr >= 3 );

	static SVector2	v[MAX_CELLVERTS];
	if( __terrainVerts )
	for( i = 0 ; i < ngr ; i++ )
		v[i].x = _gr_vertices[i].texture.x,
		v[i].y = _gr_vertices[i].texture.y;

	_gr_polygon.dwFullType = bGouraud ? GR_POLY_TEXTURE_GOURAUD :
		(__terrainLin ? GR_POLY_TEXTURE_LIN : GR_POLY_TEXTURE_PERSP); // | dwHazeFlag;
	_gr_polygon.dwAddType = dwHazeFlag;
	if( bCellBump )	_gr_polygon.dwAddType |= GR_POLY_ADD_BUMP;
	/*if( nWater )*/	_gr_polygon.dwAddType |= GR_POLY_ADD_LIGHTTHROUGH;
	_gr_polygon.nVertices = ngr;
	if( ngr >= 3 ) {
		//_gr_polygon.nLights = 0;
		//ASSERT( mask == 0 || nWater != 0 );
		//if( !CalcTriangleNormal((SVert3_0**)ppCellVerts,iVert1,iVertT,iVert2,nWater) ) __revCnt++;
		//__fullCnt++;
		GRDrawPolygonPCCW();
		#if !_USE_GLIDE
		if( __terrainVerts )
		for( i = 0 ; i < ngr ; i++ ) {
			if( _gr_clipRect.Contains(v[i]) )
				_gr_pOrigin[v[i].x+v[i].y*_gr_nScreenWidth] = 255;
		}
		#if 0
		if( __terrainCoords )
		for( i = 0 ; i < nCellVerts ; i++ ) {
			SVert3	&vertC = *ppCellVerts[i];
			if( !vertC.cf &&
					_gr_clipRect.Contains(CVector2(vertC.x,vertC.y)) ) {
				char s[100];
				sprintf(s,"%d.%d",vertC.L[0],vertC.L[1]);
				GREndScene();
				terrFnt.PrintClipColorAt(vertC.x+1,vertC.y+1,s,__terrainWhiteColor);
				GRStartScene();
			}
		}
		#endif
		#endif
	}
}
#undef	SPLITDIFF


void _CViewTerrain::DrawTriangle(WATER_SPLIT eSplit,
				_CViewTerrain::SVert3_0 **ppCellVerts,int iVert1,int iVertT,int iVert2,int nCellVerts,int mask,
											bool bCellBump,SVector2 *pBumpVerts,int nWater,bool bGouraud)
{
	__fullCnt++;
	if( !CalcTriangleNormal(ppCellVerts,iVert1,iVertT,iVert2,nWater) ) {
		__revCnt++;
		return;
	}
	switch(eSplit) {
		case WATER_SPLIT_UP:
			DrawTriangleSplit(1,(SVert3W**)ppCellVerts,iVert1,iVertT,iVert2,nCellVerts,mask,bCellBump,pBumpVerts,nWater,bGouraud);
			return;
		case WATER_SPLIT_DOWN:
			DrawTriangleSplit(0,(SVert3W**)ppCellVerts,iVert1,iVertT,iVert2,nCellVerts,mask,bCellBump,pBumpVerts,nWater,bGouraud);
			return;
	}

	if( nWater ) {
        _gr_polygon.hTexture = m_hMaskW; //.Texture();
        _gr_polygon.hBump = m_hBumpW; //.Texture();
	} else
	if( mask == 0 ) {
        _gr_polygon.hTexture = m_hMask; //.Texture();
        _gr_polygon.hBump = m_hBump; //.Texture();
	}
	else {
        _gr_polygon.hTexture = m_hMask1; //.Texture();
        _gr_polygon.hBump = m_hBump; //.Texture();
	}

	#if _USE_GLIDE
		_gr_polygon.fill.texture.nColor = 0;
	#endif

	//static	CVector2	bumpVerts[64];
	int   i, ngr; //, cnt;
	//int   iVert1, iVert2; // , bump1, bump2, bump12;

	//   3    2
	//
	//   0    1

	#if 0
	if( bUpper ) {
		iVert1 = m_nTriangleMark[2];
		//iVertT = m_nTriangleMark[3];
		iVert2 = m_nTriangleMark[0];
		//bump1 = 253<<16;
		//bump2 = 2<<16;
	}
	else {
		iVert1 = m_nTriangleMark[0];
		//iVertT = m_nTriangleMark[1];
		iVert2 = m_nTriangleMark[2];
		//bump1 = 2<<16;
		//bump2 = 253<<16;
	}
	#endif

	//if( mask == 1 )	FufelBump();

	dword	dwHazeFlag = GR_POLY_ADD_NONE;
	ngr = 0; //cnt = 0;
	for( i = iVert1 ; ; i = i+1 < nCellVerts ? i+1 : 0 ) {
		//cnt ++;
		SVert3	&vert = *(SVert3*)ppCellVerts[i];
		if( vert.v3.z >= CViewFigure::HazeMin() /**nTerrainPrec*/ ) dwHazeFlag = GR_POLY_ADD_HAZE;
		int iP, iN;
		if( vert.cf ) {
			double	 fClipD, f1ClipD;
			if( vert.cf != 2 )
				fClipD=CViewObject::m_fFrontClip,f1ClipD=CViewObject::m_f1FrontClip;
			else
				fClipD=CViewObject::m_fBackClip,f1ClipD=CViewObject::m_f1BackClip;
			SVert3 &vertP = *(SVert3*)(ppCellVerts[iP = i == iVert1 ? iVert2 : (i > 0 ? i-1 : nCellVerts-1)]),
				   &vertN = *(SVert3*)(ppCellVerts[iN = i==iVert2 ? iVert1 : (i+1 < nCellVerts ? i+1 : 0)]);
			if( vertP.cf != vert.cf ) {
				//int	t = SHLD_IDiv(nClipD - vert.z3,vertP.z3 - vert.z3,CViewObject::CLIP_SHIFT);
				double t = (fClipD-vert.v3.z)/AbsNoLess(vertP.v3.z-vert.v3.z,1e-5);
				int nt; D2INT(&nt,t*CViewObject::CLIP_PREC);
				D2INT(&_gr_vertices[ngr].texture.x,(vert.v3.x+(vertP.v3.x-vert.v3.x)*t)*f1ClipD);
				D2INT(&_gr_vertices[ngr].texture.y,(vert.v3.y+(vertP.v3.y-vert.v3.y)*t)*f1ClipD);
				_gr_vertices[ngr].texture.u = vert.u+
								Mul_SHRD(vertP.u-vert.u,nt,CViewObject::CLIP_SHIFT);
				_gr_vertices[ngr].texture.v = vert.v+
								Mul_SHRD(vertP.v-vert.v,nt,CViewObject::CLIP_SHIFT);
				D2INT(&_gr_vertices[ngr].texture.iz,f1ClipD* /*65536.*/CViewObject::m_fz1MinScale);
				if( bGouraud )
					_gr_vertices[ngr].gouraud.b = CreateHWColor( vert.b+
								Mul_SHRD(vertP.b-vert.b,nt,CViewObject::CLIP_SHIFT) );
				if( bCellBump ) {
					_gr_vertices[ngr].bump.u = pBumpVerts[i].x+
									Mul_SHRD(pBumpVerts[iP].x-pBumpVerts[i].x,nt,CViewObject::CLIP_SHIFT);
					_gr_vertices[ngr].bump.v = pBumpVerts[i].y+
									Mul_SHRD(pBumpVerts[iP].y-pBumpVerts[i].y,nt,CViewObject::CLIP_SHIFT);
				}
				ngr++;
			}
			if( vertN.cf != vert.cf ) {
				//int	t = SHLD_IDiv(nClipD - vert.z3,vertN.z3 - vert.z3,CViewObject::CLIP_SHIFT);
				double t = (fClipD-vert.v3.z)/AbsNoLess(vertN.v3.z-vert.v3.z,1e-5);
				int nt; D2INT(&nt,t*CViewObject::CLIP_PREC);
				D2INT(&_gr_vertices[ngr].texture.x,(vert.v3.x+(vertN.v3.x-vert.v3.x)*t)*f1ClipD);
				D2INT(&_gr_vertices[ngr].texture.y,(vert.v3.y+(vertN.v3.y-vert.v3.y)*t)*f1ClipD);
				_gr_vertices[ngr].texture.u = vert.u+
								Mul_SHRD(vertN.u-vert.u,nt,CViewObject::CLIP_SHIFT);
				_gr_vertices[ngr].texture.v = vert.v+
								Mul_SHRD(vertN.v-vert.v,nt,CViewObject::CLIP_SHIFT);
				D2INT(&_gr_vertices[ngr].texture.iz,f1ClipD* /*65536.*/ CViewObject::m_fz1MinScale);
				if( bGouraud )
					_gr_vertices[ngr].gouraud.b = CreateHWColor( vert.b+
								Mul_SHRD(vertN.b-vert.b,nt,CViewObject::CLIP_SHIFT) );
				if( bCellBump ) {
					_gr_vertices[ngr].bump.u = pBumpVerts[i].x+
									Mul_SHRD(pBumpVerts[iN].x-pBumpVerts[i].x,nt,CViewObject::CLIP_SHIFT);
					_gr_vertices[ngr].bump.v = pBumpVerts[i].y+
									Mul_SHRD(pBumpVerts[iN].y-pBumpVerts[i].y,nt,CViewObject::CLIP_SHIFT);
				}
				ngr++;
			}
		}
		else {
			_gr_vertices[ngr].texture.x = vert.x;
			_gr_vertices[ngr].texture.y = vert.y;
			_gr_vertices[ngr].texture.u = vert.u;
			_gr_vertices[ngr].texture.v = vert.v;
			_gr_vertices[ngr].texture.iz = vert.iz;
			if( bGouraud )
				_gr_vertices[ngr].gouraud.b = CreateHWColor(vert.b);
			if( bCellBump ) {
				_gr_vertices[ngr].bump.u = pBumpVerts[i].x;
				_gr_vertices[ngr].bump.v = pBumpVerts[i].y;
			}
			ngr++;
		}
		if( i == iVert2 ) break;
	}

	ASSERT( ngr <= 0 || ngr >= 3 ); //cnt >= 3 );

	static SVector2	v[MAX_CELLVERTS];
	if( __terrainVerts )
	for( i = 0 ; i < ngr ; i++ )
		v[i].x = _gr_vertices[i].texture.x,
		v[i].y = _gr_vertices[i].texture.y;

	// GR_POLY_TEXTURE_GOURAUD
	// _gr_vertices[i].gouraud.b = GRCreateColor(r,g,b);  0xrrggbb00

	_gr_polygon.dwFullType = bGouraud ? GR_POLY_TEXTURE_GOURAUD :
		(__terrainLin ? GR_POLY_TEXTURE_LIN : GR_POLY_TEXTURE_PERSP); // | dwHazeFlag;
	_gr_polygon.dwAddType = dwHazeFlag;
	if( bCellBump )	_gr_polygon.dwAddType |= GR_POLY_ADD_BUMP;
	/*if( nWater )*/	_gr_polygon.dwAddType |= GR_POLY_ADD_LIGHTTHROUGH;
	_gr_polygon.nVertices = ngr;
	if( ngr >= 3 ) {
		//_gr_polygon.nLights = 0;
		//ASSERT( mask == 0 || nWater != 0 );
		GRDrawPolygonPCCW();
		#if !_USE_GLIDE
		if( __terrainVerts )
		for( i = 0 ; i < ngr ; i++ ) {
			if( _gr_clipRect.Contains(v[i]) )
				_gr_pOrigin[v[i].x+v[i].y*_gr_nScreenWidth] = 255;
		}
		#if 0
		if( __terrainCoords )
		for( i = 0 ; i < nCellVerts ; i++ ) {
			SVert3	&vertC = *ppCellVerts[i];
			if( !vertC.cf &&
					_gr_clipRect.Contains(CVector2(vertC.x,vertC.y)) ) {
				char s[100];
				sprintf(s,"%d.%d",vertC.L[0],vertC.L[1]);
				GREndScene();
				terrFnt.PrintClipColorAt(vertC.x+1,vertC.y+1,s,__terrainWhiteColor);
				GRStartScene();
			}
		}
		#endif
		#endif
	}
}

inline void _CViewTerrain::BeginDrawCell()
{
	WRITELOG("########## BeginDrawCell\n");
	m_nCellVerts = 0;
	m_bCellBump = m_bCellBumpW = FALSE;
	m_bNeedLand = TRUE;
	m_nLandClipFlags = m_nWaterClipFlags = 0;
}

inline void _CViewTerrain::PutTriangleMark(int n)
{
	m_nTriangleMark[n&3] = m_nCellVerts;
	WRITELOG1("### Triangle Mark %d\n",n);
}

CRect2	__cellR;
CVector2	__cellD(0,0); //-200,100);
const int terrainMaxBump = 50;
inline void _CViewTerrain::AddCellVertex(CCellVertex &v)
{
	#if _RC_COUNT
	if( __countRc ) {
		//int x = v.L[I_X]-__cellD.x, y = v.L[I_Y]-__cellD.y;
		//if( y >= 0 && y < 512 && x >= 0 && x < 512 ) //&& rcCount[511-y][x]<255 )
		//	rcCount[511-y][x]=2;
		if( m_nCellVerts < 1 )
			__cellR = CRect2(v.L[I_X],v.L[I_Y],v.L[I_X],v.L[I_Y]);
		else {
			__cellR.left = Min(__cellR.left,(long)v.L[I_X]);
			__cellR.right = Max(__cellR.right,(long)v.L[I_X]);
			__cellR.top = Min(__cellR.top,(long)v.L[I_Y]);
			__cellR.bottom = Max(__cellR.bottom,(long)v.L[I_Y]);
		}
	}
	#endif

	//ASSERT(v.xL!=0);
	ASSERT(m_nCellVerts<MAX_CELLVERTS);
	//m_cellVerts[m_nCellVerts++] = v;
	Copy(m_cellVerts[m_nCellVerts++],v);
	if( v.lf ) {
		m_nLandClipFlags |= 1<<v.cf;
		if( v.v3.z < terrainMaxBump /**nTerrainPrec*/ )	m_bCellBump = TRUE;
	}
	else
		m_bNeedLand = FALSE;
	if( v.wf ) {
		m_nWaterClipFlags |= 1<<v.w.cf;
		if( v.w.v3.z < terrainMaxBump /**nTerrainPrec*/ )	m_bCellBumpW = TRUE;
	}
	WRITELOG2("# Add Cell %d,%d\n",v.L[I_X],v.L[I_Y]);
}

inline void _CViewTerrain::DrawRect(_CViewTerrain::TRIANGULATE eTriangulate,
					WATER_SPLIT eSplit,
					_CViewTerrain::SVert3_0 **ppCellVerts0,_CViewTerrain::SVert3_0 **ppCellVerts,
					int nCellVerts,int mask,bool bBump,SVector2 *pBumpVerts,int nWater,int t0,int t1,int t2,int t3)
{
	bool bGouraud = m_bIsHardware ? !nWater : 0;
	switch( eTriangulate ) {
		case TRIANG_NORMAL:
			DrawTriangle(eSplit,ppCellVerts0,t0,t1,t2,nCellVerts,mask,bBump,pBumpVerts,nWater,bGouraud); //lower
			DrawTriangle(eSplit,ppCellVerts0,t2,t3,t0,nCellVerts,mask,bBump,pBumpVerts,nWater,bGouraud); //upper
			break;
		case TRIANG_CHANGED:
			DrawTriangle(eSplit,ppCellVerts,t0,t1,t2,nCellVerts,mask,bBump,pBumpVerts,nWater,bGouraud); //lower
			DrawTriangle(eSplit,ppCellVerts,t2,t3,t0,nCellVerts,mask,bBump,pBumpVerts,nWater,bGouraud); //upper
			break;
		case TRIANG_NO:
			DrawTriangle(eSplit,ppCellVerts0,1,2,0,nCellVerts,mask,bBump,pBumpVerts,nWater,bGouraud); //entire
			break;
		default:
			ASSERT(0);
	}
}


void _CViewTerrain::EndDrawCell(int rc,TCSRect2 &cellSP)
{
	ASSERT( m_nLandClipFlags >= 0 && m_nLandClipFlags < 8 &&
			m_nWaterClipFlags >= 0 && m_nWaterClipFlags < 8 );
	static	bool	bOutOfClip[8] = { 0, 1, 0, 1, 0, 1, 1, 1 };
	bool	bLandOutOfClip, bWaterOutOfClip;
	bLandOutOfClip = bOutOfClip[m_nLandClipFlags];
	bWaterOutOfClip = bOutOfClip[m_nWaterClipFlags];
	__cellCount++;
	ASSERT( m_nCellVerts >= 4 );
	/*int mask = MASK_CELL(m_cellVerts[m_nTriangleMark[0]].L[I_X],
						m_cellVerts[m_nTriangleMark[0]].L[I_Y]);*/

	int t0 = m_nTriangleMark[0],
		t1 = m_nTriangleMark[1],
		t2 = m_nTriangleMark[2],
		t3 = m_nTriangleMark[3] ;


	SRect2	cell0;
	cell0.left = m_cellVerts[m_nTriangleMark[0]].L[I_X];
	cell0.top = m_cellVerts[m_nTriangleMark[0]].L[I_Y];
	cell0.right = m_cellVerts[m_nTriangleMark[2]].L[I_X];
	cell0.bottom = m_cellVerts[m_nTriangleMark[2]].L[I_Y];
	int nMask = m_maskflag(cell0.left,cell0.top) >> 7;

	#if _TERRAIN_WATER
		int isWater = IsWaterCell(cell0.left,cell0.top);
	#endif


	static SVert3_0 *ppCellVerts[MAX_CELLVERTS], *ppCellVertsW[MAX_CELLVERTS];
	//enum { TRIANG_NORMAL, TRIANG_CHANGED, TRIANG_NO } eTriangulate = TRIANG_NORMAL;
	TRIANGULATE eTriangulate = TRIANG_NORMAL;
	int nCellVerts = m_nCellVerts, i;
	{
		int d = 1<<rc;
		int width = m_cellVerts[t2].L[I_X] - m_cellVerts[t0].L[I_X];
		int height = m_cellVerts[t2].L[I_Y] - m_cellVerts[t0].L[I_Y];
		ASSERT( width > 0 && height > 0 );
		if( width < d || height < d ) {		// diagonal check
			CRect2	rect(m_cellVerts[t0].L[I_X],m_cellVerts[t0].L[I_Y],
							m_cellVerts[t2].L[I_X],m_cellVerts[t2].L[I_Y]);
			CVector2	align(rect.left&-d,rect.top&-d);
			CRect2	rect0 = rect;
			rect0.MoveBy(-align);
			if( rect0.right <= rect0.top || rect0.left >= rect0.bottom )
				eTriangulate = TRIANG_NO; // completely upper or lower than diagonal
			else { // diagonal recalc
				eTriangulate = TRIANG_CHANGED;

				int iSrc = t0, iDest = 0;
				m_nTriangleMark[2] = -1;

				m_nTriangleMark[0] = iDest;
				if( rect0.left < rect0.top ) {	// left top is above diagonal
					// must be crossing on 0->1 side
					int d0 = rect0.top+align.x;
					while( m_cellVerts[iSrc].L[I_X] < d0 ) {
						ppCellVerts[iDest++] = &m_cellVerts[iSrc++];
						if( iSrc >= nCellVerts ) iSrc = 0;
					}
					if( m_cellVerts[iSrc].L[I_X] > d0 ) { // Add 0->1 crossing
						ASSERT(m_nCellVerts<MAX_CELLVERTS && iDest > 0);
						//m_cellVerts[m_nCellVerts].SetLCoord(d0,rect.top);
						//m_cellVerts[m_nCellVerts].Interpolate(*ppCellVerts[iDest-1],m_cellVerts[iSrc],I_X);
						CreateCellVertex(d0,rect.top,m_cellVerts[m_nCellVerts]);
						if( !m_cellVerts[m_nCellVerts].lf )	m_bNeedLand = FALSE;
						ppCellVerts[m_nTriangleMark[0]=iDest++] = &m_cellVerts[m_nCellVerts++];
					}
				}
				while( iSrc != t1 ) {
					ppCellVerts[iDest++] = &m_cellVerts[iSrc++];
					if( iSrc >= nCellVerts ) iSrc = 0;
				}

				t1 = m_nTriangleMark[1] = iDest;
				if( rect0.right < rect0.bottom ) {	// right bottom is above diagonal
					// must be crossing on 1->2 side
					int d0 = rect0.right+align.y;
					while( m_cellVerts[iSrc].L[I_Y] < d0 ) {
						ppCellVerts[iDest++] = &m_cellVerts[iSrc++];
						if( iSrc >= nCellVerts ) iSrc = 0;
					}
					if( m_cellVerts[iSrc].L[I_Y] > d0 ) { // Add 1->2 crossing
						ASSERT(m_nCellVerts<MAX_CELLVERTS && iDest > 0);
						//m_cellVerts[m_nCellVerts].SetLCoord(rect.right,d0);
						//m_cellVerts[m_nCellVerts].Interpolate(*ppCellVerts[iDest-1],m_cellVerts[iSrc],I_Y);
						CreateCellVertex(rect.right,d0,m_cellVerts[m_nCellVerts]);
						if( !m_cellVerts[m_nCellVerts].lf )	m_bNeedLand = FALSE;
						ppCellVerts[m_nTriangleMark[2]=iDest++] = &m_cellVerts[m_nCellVerts++];
					}
				}
				while( iSrc != t2 ) {
					ppCellVerts[iDest++] = &m_cellVerts[iSrc++];
					if( iSrc >= nCellVerts ) iSrc = 0;
				}

				if( m_nTriangleMark[2] < 0 ) m_nTriangleMark[2] = iDest;
				//t2 = iDest;
				if( rect0.right > rect0.bottom ) {	// right bottom is below diagonal
					// must be crossing on 2->3 side
					int d0 = rect0.bottom+align.x;
					while( m_cellVerts[iSrc].L[I_X] > d0 ) {
						ppCellVerts[iDest++] = &m_cellVerts[iSrc++];
						if( iSrc >= nCellVerts ) iSrc = 0;
					}
					if( m_cellVerts[iSrc].L[I_X] < d0 ) { // Add 2->3 crossing
						ASSERT(m_nCellVerts<MAX_CELLVERTS && iDest > 0);
						//m_cellVerts[m_nCellVerts].SetLCoord(d0,rect.bottom);
						//m_cellVerts[m_nCellVerts].Interpolate(m_cellVerts[iSrc],*ppCellVerts[iDest-1],I_X);
						CreateCellVertex(d0,rect.bottom,m_cellVerts[m_nCellVerts]);
						if( !m_cellVerts[m_nCellVerts].lf )	m_bNeedLand = FALSE;
						ppCellVerts[m_nTriangleMark[2]=iDest++] = &m_cellVerts[m_nCellVerts++];
					}
				}
				while( iSrc != t3 ) {
					ppCellVerts[iDest++] = &m_cellVerts[iSrc++];
					if( iSrc >= nCellVerts ) iSrc = 0;
				}

				t3 = m_nTriangleMark[3] = iDest;
				if( rect0.left > rect0.top ) {	// left top is below diagonal
					// must be crossing on 3->0 side
					int d0 = rect0.left+align.y;
					while( m_cellVerts[iSrc].L[I_Y] > d0 ) {
						ppCellVerts[iDest++] = &m_cellVerts[iSrc++];
						if( iSrc >= nCellVerts ) iSrc = 0;
					}
					if( m_cellVerts[iSrc].L[I_Y] < d0 ) { // Add 3->0 crossing
						ASSERT(m_nCellVerts<MAX_CELLVERTS && iDest > 0);
						//m_cellVerts[m_nCellVerts].SetLCoord(rect.left,d0);
						//m_cellVerts[m_nCellVerts].Interpolate(m_cellVerts[iSrc],*ppCellVerts[iDest-1],I_Y);
						CreateCellVertex(rect.left,d0,m_cellVerts[m_nCellVerts]);
						if( !m_cellVerts[m_nCellVerts].lf )	m_bNeedLand = FALSE;
						ppCellVerts[m_nTriangleMark[0]=iDest++] = &m_cellVerts[m_nCellVerts++];
					}
				}
				while( iSrc != t0 ) {
					ppCellVerts[iDest++] = &m_cellVerts[iSrc++];
					if( iSrc >= nCellVerts ) iSrc = 0;
				}
				t0 = m_nTriangleMark[0];
				t2 = m_nTriangleMark[2];
				ASSERT( iDest == m_nCellVerts );
			} // diagonal recalc
		} // diagonal check
	} // clip check

#if 1
	if( rc > 0 )	m_bCellBump = m_bCellBumpW = 0;
	ASSERT( eTriangulate != TRIANG_CHANGED || rc > 0 );
	if( m_bCellBump ) {
		m_bumpVerts[t0].x = m_bumpVerts[t0].y = m_bumpVerts[t3].x = m_bumpVerts[t1].y = 2<<16;
		m_bumpVerts[t2].x = m_bumpVerts[t2].y = m_bumpVerts[t1].x = m_bumpVerts[t3].y = 253<<16;
		#if 0
		const int bump1 = 2<<16, bump2 = 253<<16, bump12 = bump2 - bump1;
		int   i0, i1, bumpD;
		i0 = t0; i1 = t1; //m_nTriangleMark[0]; i1 = m_nTriangleMark[1];
		bumpD = m_cellVerts[i1].L[I_X] - m_cellVerts[i0].L[I_X]; ASSERT(bumpD>0);
		for( i = i0 ; i != i1 ;	++i < m_nCellVerts ? (void)0 : (void)(i=0) ) {
			double t = (m_cellVerts[i].L[I_X]-m_cellVerts[i0].L[I_X])/(double)bumpD;
			D2INT((int*)&m_bumpVerts[i].x, bump1+bump12*t );
			m_bumpVerts[i].y = bump1;
		}
		i0 = t1; i1 = t2; //m_nTriangleMark[1]; i1 = m_nTriangleMark[2];
		bumpD = m_cellVerts[i1].L[I_Y] - m_cellVerts[i0].L[I_Y]; ASSERT(bumpD>0);
		for( i = i0 ; i != i1 ;	++i < m_nCellVerts ? (void)0 : (void)(i=0) ) {
			double t = (m_cellVerts[i].L[I_Y]-m_cellVerts[i0].L[I_Y])/(double)bumpD;
			D2INT((int*)&m_bumpVerts[i].y, bump1+bump12*t );
			m_bumpVerts[i].x = bump2;
		}
		i0 = t2; i1 = t3; //m_nTriangleMark[2]; i1 = m_nTriangleMark[3];
		bumpD = m_cellVerts[i0].L[I_X] - m_cellVerts[i1].L[I_X]; ASSERT(bumpD>0);
		for( i = i0 ; i != i1 ;	++i < m_nCellVerts ? (void)0 : (void)(i=0) ) {
			double t = (m_cellVerts[i].L[I_X]-m_cellVerts[i1].L[I_X])/(double)bumpD;
			D2INT((int*)&m_bumpVerts[i].x, bump1+bump12*t );
			m_bumpVerts[i].y = bump2;
		}
		i0 = t3; i1 = t0; //m_nTriangleMark[3]; i1 = m_nTriangleMark[0];
		bumpD = m_cellVerts[i0].L[I_Y] - m_cellVerts[i1].L[I_Y]; ASSERT(bumpD>0);
		for( i = i0 ; i != i1 ;	++i < m_nCellVerts ? (void)0 : (void)(i=0) ) {
			double t = (m_cellVerts[i].L[I_Y]-m_cellVerts[i1].L[I_Y])/(double)bumpD;
			D2INT((int*)&m_bumpVerts[i].y, bump1+bump12*t );
			m_bumpVerts[i].x = bump1;
		}
		#endif
	}	// if bump
#endif

	dword	dwLights = 0;
	{
		int d = 1<<rc;
		int x1 = Min(cell0.right, m_pDynamicsRect->right);
		int y1 = Min(cell0.bottom, m_pDynamicsRect->bottom);
		for( int y = Max(cell0.top,m_pDynamicsRect->top) ; y < y1 ; y++)
		for( int x = Max(cell0.left,m_pDynamicsRect->left) ; x < x1 ; x++)
			dwLights |= (*m_pLightsOrigin)[x+m_nLightsPitch*y];
	}

	m_pMap1->SetReduction(rc,0);
	/*if(	__bViewHide && m_pMap1->Hide(prim,sec) ) {
		m_pMap1->Draw(prim,sec,WATER_VIEW_ALL);
		GRSetZPrecision(0);
		return;
	}*/

	#if _TERRAIN_WATER
	if( isWater ) {
		if( !m_wp.m_transBump ) m_bCellBumpW = FALSE;
		if( m_bCellBumpW && bWaterOutOfClip ) CalcWaterBumpCoords();
		// Draw terrain from the other side of water
		bool bDropOtherSide = m_bIsAboveWater ? m_bIsAboveWaterCpl : m_bIsBelowWaterCpl;
		if( m_bNeedLand && !bDropOtherSide && bLandOutOfClip ) {
			#ifdef _DEBUG
				for( i = 0 ; i < m_nCellVerts ; i++ )
					ASSERT(((SVert3**)(eTriangulate!=TRIANG_CHANGED?m_apCellVerts:ppCellVerts))[i]->lf);
			#endif
			_gr_polygon.nLights = dwLights;
			DrawRect(eTriangulate,m_bIsAboveWater?WATER_SPLIT_DOWN:WATER_SPLIT_UP,
				m_apCellVerts,ppCellVerts,m_nCellVerts,nMask,m_bCellBump,m_bumpVerts,0,t0,t1,t2,t3);
		}
		if( __terrainObjs && !bDropOtherSide ) m_pMap1->Draw(cellSP,m_bIsAboveWater?WATER_VIEW_BELOW:WATER_VIEW_ABOVE);
		// Draw water
		if( bWaterOutOfClip ) {
			if( m_bIsAboveWater ) { // draw water from above
				if( eTriangulate == TRIANG_CHANGED )
					for( i = 0 ; i < m_nCellVerts ; i++ )	ppCellVertsW[i] = &((SVert3W*)ppCellVerts[i])->w;
				_gr_polygon.nLights = dwLights;
				DrawRect(eTriangulate,WATER_SPLIT_NONE,
					m_apCellVertsW,ppCellVertsW,m_nCellVerts,1,m_bCellBumpW,m_bumpVertsW,1,t0,t1,t2,t3);
			}
			else { // draw water from below
				//DrawTriangle(m_apCellVertsWR+(MAX_CELLVERTS-nCellVerts),1,0,nCellVerts,1,FALSE,m_bumpVertsW);
				if( eTriangulate == TRIANG_CHANGED )
					for( i = 0 ; i < m_nCellVerts ; i++ )	ppCellVertsW[MAX_CELLVERTS-1-i] = &((SVert3W*)ppCellVerts[i])->w;
				int nCellVerts1 = m_nCellVerts-1;
				_gr_polygon.nLights = dwLights;
				DrawRect(eTriangulate,WATER_SPLIT_NONE,
					m_apCellVertsWR+(MAX_CELLVERTS-m_nCellVerts),ppCellVertsW+(MAX_CELLVERTS-m_nCellVerts),m_nCellVerts,1,m_bCellBumpW,m_bumpVertsW,-1,
					nCellVerts1-t0,nCellVerts1-t1,nCellVerts1-t2,nCellVerts1-t3);
			}
		}
		// Draw terrain from the this side of water
		if( m_bNeedLand && bLandOutOfClip ) {
			#ifdef _DEBUG
				for( i = 0 ; i < m_nCellVerts ; i++ )
					ASSERT(((SVert3**)(eTriangulate!=TRIANG_CHANGED?m_apCellVerts:ppCellVerts))[i]->lf);
			#endif
			_gr_polygon.nLights = dwLights;
			DrawRect(eTriangulate,m_bIsAboveWater?WATER_SPLIT_UP:WATER_SPLIT_DOWN,
				m_apCellVerts,ppCellVerts,m_nCellVerts,nMask,m_bCellBump,m_bumpVerts,0,t0,t1,t2,t3);
		}
		if( __terrainObjs ) {
			//if( bDropOtherSide )
			//	m_pMap1->Draw(prim,sec,WATER_VIEW_ALL);
			//else
			m_pMap1->Draw(cellSP,m_bIsAboveWater?WATER_VIEW_ABOVE:WATER_VIEW_BELOW);
		}
		#if 0
		if( __terrainCoords )
		for( i = 0 ; i < nCellVerts ; i++ ) {
			SVert3W	&vertC = *(SVert3W*)m_apCellVerts[i];
			if( !vertC.w.cf &&
					_gr_clipRect.Contains(CVector2(vertC.w.x,vertC.w.y)) ) {
				char s[100];
				sprintf(s,"%d.%d",vertC.L[0],vertC.L[1]);
				GREndScene();
				terrFnt.PrintClipColorAt(vertC.x+1,vertC.y+1,s,__terrainWhiteColor);
				GRStartScene();
			}
		}
		#endif
	}
	else
	#endif
	if( !m_bIsBelowWaterCpl ) {
		if( bLandOutOfClip ) {
			ASSERT( m_bNeedLand );
			#ifdef _DEBUG
				for( i = 0 ; i < m_nCellVerts ; i++ )
					ASSERT(((SVert3**)(eTriangulate!=TRIANG_CHANGED?m_apCellVerts:ppCellVerts))[i]->lf);
			#endif
			_gr_polygon.nLights = dwLights;
			DrawRect(eTriangulate,WATER_SPLIT_NONE,
				m_apCellVerts,ppCellVerts,m_nCellVerts,nMask,m_bCellBump,m_bumpVerts,0,t0,t1,t2,t3);
		}
		m_pMap1->Draw(cellSP,WATER_VIEW_ABOVE);
	}

	#if 1
	//__terrainCoords = 1;
	if( m_bNeedLand && __terrainCoords ) {
		WRITELOG1("Cell verts (%d): ",m_nCellVerts);
		for( i = 0 ; i < m_nCellVerts ; i++ ) {
			SVert3W	&vertC = *(
				eTriangulate==TRIANG_CHANGED?(SVert3W*)ppCellVerts[i]:
					(SVert3W*)m_apCellVerts[i]
			);
			WRITELOG2("%d,%d ",vertC.L[0],vertC.L[1]);
			if( !vertC.cf ) WRITELOG2("(%d,%d) ",vertC.x,vertC.y);
			if( !vertC.cf &&
					_gr_clipRect.Contains(CVector2(vertC.x,vertC.y)) ) {
				char s[100];
				sprintf(s,"%d.%d",vertC.L[0],vertC.L[1]);
				GREndScene();
				terrFnt.PrintClipColorAt(vertC.x+1,vertC.y+1,s,__terrainWhiteColor);
				GRStartScene();
			}
		}
		WRITELOG("\n");
	}
	if( isWater && __terrainCoords )
	for( i = 0 ; i < nCellVerts ; i++ ) {
		SVert3W	&vertC = *(SVert3W*)m_apCellVerts[i];
		if( !vertC.cf &&
				_gr_clipRect.Contains(CVector2(vertC.w.x,vertC.w.y)) ) {
			char s[100];
			sprintf(s,"%d.%d",vertC.L[0],vertC.L[1]);
			GREndScene();
			terrFnt.PrintClipColorAt(vertC.w.x+1,vertC.w.y+1,s,__terrainWhiteColor);
			GRStartScene();
		}
	}
	#endif


	#if _RC_COUNT
	if( __countRc ) {
		for( int xx = __cellR.left ; xx < __cellR.right ; xx++ )
		for( int yy = __cellR.top ; yy < __cellR.bottom ; yy++ ) {
			int x = xx/*__cellR.CenterX()*/-__cellD.x, y = yy/*__cellR.CenterY()*/-__cellD.y;
			if( x >= 0 && x < 512 && y >= 0 && y < 512 ) {
				ASSERT( rcCount[y][x] == 255 );
				rcCount[/*511-*/y][x] = (byte)rc; //|= 4;
				//ASSERT( MAX_REDCELL(x,y) == rc );
				//if( !b1 || !b2 ) rcCount[511-y][x]++;
			}
		}
	}
	#endif
	WRITELOG("########## End Draw Cell\n");
	/*if( __terrainObjs ) {
		//m_pMap1->SetReduction(rc,0);
		m_pMap1->Draw(prim,sec);
		//GRSetZPrecision(0);
	}*/
	GRSetZPrecision(CViewObject::m_z1MinShift);
}

// --------------------------------------------------------------------
#define	LOGCACHE(S,V,d) \
	WRITELOG5(S " CACHE_" #V "[%d]: xL=%d yL=%d rc=%d nxt=%d\n", d,  \
	CACHE_##V[d].L[I_X], CACHE_##V[d].L[I_Y], CACHE_##V[d].rc, CACHE_##V[d].nxt )


#define	CREATECELLVERTEX_PS(p,s,c)	(P?CreateCellVertex(s,p,c):CreateCellVertex(p,s,c))
				//	INDEX_PS(p,s,CreateCellVertex(c))
#define	REDUCTIONCELL_PS(p,s)	(P?ReductionCell((s)-SAS,(p)-PAS):ReductionCell((p)-PAS,(s)-SAS))
				//	INDEX_PS((p)-PAS,(s)-SAS,ReductionCell())
#define	PUTTRIANGLEMARK_X(n)	(S?PutTriangleMark(n):(void)0)
#define	PUTTRIANGLEMARK_Y(n)	(P?PutTriangleMark(n):(void)0)
#define	GTP(a,b) (PAS?(a)<(b):(a)>(b))
#define	GTS(a,b) (SAS?(a)<(b):(a)>(b))
#define	GEP(a,b) (PAS?(a)<=(b):(a)>=(b))
#define	GES(a,b) (SAS?(a)<=(b):(a)>=(b))
#define	LTP(a,b) (PAS?(a)>(b):(a)<(b))
#define	LTS(a,b) (SAS?(a)>(b):(a)<(b))
#define	LEP(a,b) (PAS?(a)>=(b):(a)<=(b))
#define	LES(a,b) (SAS?(a)>=(b):(a)<=(b))
#define	MAXP(a,b)	(PAS?Min((int)(a),(int)(b)):Max((int)(a),(int)(b)))
#define	MAXS(a,b)	(SAS?Min((int)(a),(int)(b)):Max((int)(a),(int)(b)))
#define	MINP(a,b)	(PAS?Max((int)(a),(int)(b)):Min((int)(a),(int)(b)))
#define	MINS(a,b)	(SAS?Max((int)(a),(int)(b)):Min((int)(a),(int)(b)))
#define	ADDP(a,b)	(PAS?(a)-(b):(a)+(b))
#define	ADDS(a,b)	(SAS?(a)-(b):(a)+(b))
#define	SUBP(a,b)	(PAS?(a)+(b):(a)-(b))
#define	SUBS(a,b)	(SAS?(a)+(b):(a)-(b))
#define	_ALIGNP(p,d)	(PAS?((p)+(d)-1)&-(d):(p)&-(d))
#define	ALIGNP_(p,d)	(PAS?(p)&-(d):((p)+(d)-1)&-(d))
#define	_ALIGNS(s,d)	(SAS?((s)+(d)-1)&-(d):(s)&-(d))
#define	ALIGNS_(s,d)	(SAS?(s)&-(d):((s)+(d)-1)&-(d))
#define	_NEXTP(p,d)		(PAS?((p)+(d))&-(d):((p)-1)&-(d))
#define	NEXTP_(p,d)		(PAS?((p)-1)&-(d):((p)+(d))&-(d))
#define	_NEXTS(s,d)		(SAS?((s)+(d))&-(d):((s)-1)&-(d))
#define	NEXTS_(s,d)		(SAS?((s)-1)&-(d):((s)+(d))&-(d))
#define	P_TOP		(P+2*PAS)
#define	P_BOTTOM	(P+2*(1-PAS))
#define	S_LEFT		(S+2*SAS)
#define	S_RIGHT		(S+2*(1-SAS))
#define	ENDDRAWCELL(rc)	EndDrawCell(rc,CRect2(m_cellVerts[m_nTriangleMark[0]].L[S],m_cellVerts[m_nTriangleMark[0]].L[P],m_cellVerts[m_nTriangleMark[2]].L[S],m_cellVerts[m_nTriangleMark[2]].L[P]))


// Bottom
#define	DRAW4(X)	Draw##X##B
#define	DRAW4L(X)	Draw##X##T
#define	DRAW2(X)	Draw##X##B
#define	P I_Y
#define	S I_X
#define	PAS 0
#define	SAS 0
#define	CACHE_SL	m_pCacheXL
#define	CACHE_SU	m_pCacheXU
#define	CACHE_PL	m_pCacheYL
#define	CACHE_PU	m_pCacheYU
#define	M_(NAME)	m_##NAME##_U
#define	M_L(NAME)	m_##NAME##_L
//#define	M2_(NAME)	m_##NAME##B
#define	PUTTRIANGLEMARK(N)	PutTriangleMark(N)
#include"terrain4.inl"

// Top
#define	DRAW4(X)	Draw##X##T
#define	DRAW4L(X)	Draw##X##B
#define	DRAW2(X)	Draw##X##B
#define	P I_Y
#define	S I_X
#define	PAS 1
#define	SAS 1
#define	CACHE_SL	m_pCacheXU
#define	CACHE_SU	m_pCacheXL
#define	CACHE_PL	m_pCacheYU
#define	CACHE_PU	m_pCacheYL
#define	M_(NAME)	m_##NAME##_L
#define	M_L(NAME)	m_##NAME##_U
//#define	M2_(NAME)	m_##NAME##B
#define	PUTTRIANGLEMARK(N)	PutTriangleMark(N+2)
#include"terrain4.inl"

// Left
#define	DRAW4(X)	Draw##X##L
#define	DRAW4L(X)	Draw##X##R
#define	DRAW2(X)	Draw##X##R
#define	P I_X
#define	S I_Y
#define	PAS 1
#define	SAS 0
#define	CACHE_SL	m_pCacheYL
#define	CACHE_SU	m_pCacheYU
#define	CACHE_PL	m_pCacheXU
#define	CACHE_PU	m_pCacheXL
#define	M_(NAME)	m_##NAME##_L
#define	M_L(NAME)	m_##NAME##_U
//#define	M2_(NAME)	m_##NAME##R
#define	PUTTRIANGLEMARK(N)	PutTriangleMark(N+1)
#include"terrain4.inl"

// Right
#define	DRAW4(X)	Draw##X##R
#define	DRAW4L(X)	Draw##X##L
#define	DRAW2(X)	Draw##X##R
#define	P I_X
#define	S I_Y
#define	PAS 0
#define	SAS 1
#define	CACHE_SL	m_pCacheYU
#define	CACHE_SU	m_pCacheYL
#define	CACHE_PL	m_pCacheXL
#define	CACHE_PU	m_pCacheXU
#define	M_(NAME)	m_##NAME##_U
#define	M_L(NAME)	m_##NAME##_L
#define	PUTTRIANGLEMARK(N)	PutTriangleMark(N+3)
#include"terrain4.inl"

//extern "C" GRSetBump(int,int);

#if _RC_COUNT
inline void RcPut(int x,int y,int c)
{
	if( x >= 0 && x < 512 && y >= 0 && y < 512 ) rcCount[y][x] = (byte)c;
}
#endif

void _CViewTerrain::Draw(TCCRect2 &rect,CLandObjectMap2 &map)
{
	//int i1 = ReductionCell(48,143);
	//int i2 = ReductionCell(48,128);
	//(void)i1; (void)i2;


	(void)map;
	//  YL ^      top (bottomL)
	// (-Z)|
	//     | left      right
	//     |
	//     |     bottom (topL)
	//      -------------->
	//                   XL (X)
	CRect2	rect1;
	if( !m_bTerrainInView )	return;
	if( !rect1.SetIntersectNZ(rect,m_polygonRect) )	return;


	GRSetZPrecision(CViewObject::m_z1MinShift);
	GRSetBump(1,terrainMaxBump);
	int		rectL[4];
	rectL[I_LEFT] = rect1.left;
	rectL[I_TOP] = rect1.top;
	rectL[I_RIGHT] = rect1.right;
	rectL[I_BOTTOM] = rect1.bottom;
	m_rectL[I_LEFT] = rect.left;
	m_rectL[I_TOP] = rect.top;
	m_rectL[I_RIGHT] = rect.right;
	m_rectL[I_BOTTOM] = rect.bottom;
	//m_pCacheS = m_pCacheS0 - (rectL.left - 32);
	//m_pCachePL = m_pCachePL0 - (rectL.top - 32);
	//m_pCachePU = m_pCachePU0 - (rectL.top - 32);
	m_pCacheXL = CArrayRef<CCellVertex>(CArrayRef<CCellVertex>(m_pCacheXL0,1024), - (rectL[I_LEFT] - 300) );
	m_pCacheXU = CArrayRef<CCellVertex>(CArrayRef<CCellVertex>(m_pCacheXU0,1024), - (rectL[I_LEFT] - 300) );
	m_pCacheYL = CArrayRef<CCellVertex>(CArrayRef<CCellVertex>(m_pCacheYL0,1024), - (rectL[I_TOP] - 300) );
	m_pCacheYU = CArrayRef<CCellVertex>(CArrayRef<CCellVertex>(m_pCacheYU0,1024), - (rectL[I_TOP] - 300) );


	WRITELOG1("\n[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[\n"
			  "                      Drawing (%d,%d,%d,%d)\n\n",rect1);

	CViewFigure::SetWaterline(__terrainWaterline*m_fCellSize*m_fHScale);


	//m_dwLights = dwLights;
	m_pLightsOrigin = &map.LightsOrigin();
	m_nLightsPitch = map.LightsPitch();
	m_pDynamicsRect = &map.DynamicsRect();
	//(void)dwLights;

	if( IsViewAlongX() ) {
		if( IsViewAlongXN() ) {
			DrawFullL(rectL,map.MapX());
		}
		else {
			DrawFullR(rectL,map.MapX());
		}
	}
	else {
		if( IsViewAlongYP() ) {
			DrawFullB(rectL,map.MapY());
		}
		else {
			DrawFullT(rectL,map.MapY());
		}
	}

	//CViewObject::m_eWaterView = WATER_VIEW_ALL;
	CViewObject::m_eWaterView = m_bIsAboveWater ? WATER_VIEW_ABOVE : WATER_VIEW_BELOW;

	#if _RC_COUNT
		for( int x = rect.left ; x < rect.right ; x++ ) {
			RcPut(x,rect.top,4); RcPut(x,rect.bottom-1,4);
		}
		for( int y = rect.top ; y < rect.bottom ; y++ ) {
			RcPut(rect.left,y,4); RcPut(rect.right-1,y,4);
		}
	#endif
}

void _CViewTerrain::VT_DrawPixel(int x,int y)
{
	CFVector3	v3 = m_viewPointDirMx*CFVector3(x,m_heights(x,y),y);
	if( v3.z < CViewObject::m_fFrontClip )	return;
	CVector2	v(v3.x/v3.z,v3.y/v3.z);
	#if !_USE_GLIDE
	if( _gr_clipRect.Contains(v) )
		_gr_pOrigin[v.x+v.y*_gr_nScreenWidth] = 255;
	#endif
}

void _CViewTerrain::EndDrawTerrain()
{
	#if 0
	int _p;
	if( IsViewAlongX() ) {
		for( _p = m_polygonRect.left ; _p < m_polygonRect.right ; _p++ ) {
			int p = _p;
			int i = _p-m_polygonRect.left;
			int sL = m_ppEdges[0][i][0]-__cellD.y,
				sU = m_ppEdges[0][i][1]-__cellD.y;
			VT_DrawPixel(p,sL); VT_DrawPixel(p,sU);
		}
	}
	else {
		for( _p = m_polygonRect.top ; _p < m_polygonRect.bottom ; _p++ ) {
			int p = _p;
			int i = _p-m_polygonRect.top;
			int sL = m_ppEdges[0][i][0]-__cellD.x,
				sU = m_ppEdges[0][i][1]-__cellD.x;
			VT_DrawPixel(sL,p); VT_DrawPixel(sU,p);
		}
	}
	#endif

	#if _RC_COUNT
	if( __countRc ) {
		int _p;
		if( IsViewAlongX() ) {
			for( _p = m_polygonRect.left ; _p < m_polygonRect.right ; _p++ ) {
				int p = _p-__cellD.x;
				if( p < 0 || p > 511 ) continue;
				int i = _p-m_polygonRect.left;
				int sL = m_ppEdges[0][i][0]-__cellD.y,
					sU = m_ppEdges[0][i][1]-__cellD.y;
				if( sL >= 0 && sL <= 511 )	rcCount[/*511-*/sL][p] = byte(15-rcCount[511-sL][p]);
				if( sU >= 0 && sU <= 511 )	rcCount[/*511-*/sU][p] = byte(15-rcCount[511-sU][p]);
			}
		}
		else {
			for( _p = m_polygonRect.top ; _p < m_polygonRect.bottom ; _p++ ) {
				int p = _p-__cellD.y;
				if( p < 0 || p > 511 ) continue;
				int i = _p-m_polygonRect.top;
				int sL = m_ppEdges[0][i][0]-__cellD.x,
					sU = m_ppEdges[0][i][1]-__cellD.x;
				if( sL >= 0 && sL <= 511 )	rcCount[/*511-*/p][sL] = byte(15-rcCount[511-p][sL]);
				if( sU >= 0 && sU <= 511 )	rcCount[/*511-*/p][sU] = byte(15-rcCount[511-p][sU]);
			}
		}
		int i = 512;
		FILE *f = fopen("rc.spr","wb");
		fwrite(&i,2,1,f);
		fwrite(&i,2,1,f);
		fwrite(&i,1,1,f);
		fwrite(rcCount,512,512,f);
		fclose(f);
		__countRc = 0;
	}
	#endif

	//(void)fIntegerPrec;
	//(void)fIntegerPrec_1;
}

void _CViewTerrain::GetPlane(SFVector3 v,SFVector3 &normal,double &height)
{
	bool bRes = FALSE;
	// z2        /|6~~~/|7         H0' = y0+n034*R
	//         /  |  /  |          H1' = y1+n120*R
	//       /____|/____|          H2' = y2+n273*R
	// z1  5|    /|3   /|2         H3' = y3+n365*R
	//      |  /  |U / L|          nL = N012'
	//  z  4|/___0|/____|1         nU = N023'
	//      x_    x     x1         (landY=-z)
	int nLandX, nLandY;
	v.x = v.x*m_f_1_CellSize;
	v.y = -v.z*m_f_1_CellSize;
	D2INT(&nLandX,v.x-0.5);
	D2INT(&nLandY,v.y-0.5);
	v.x -= nLandX;
	v.y -= nLandY;
	int	nLandXIdx = nLandX&511,
		nLandX1Idx = (nLandX+1)&511,
		nLandYIdx = (nLandY&511)<<9,
		nLandY1Idx = ((nLandY+1)&511)<<9;
	if( v.x > v.y ) {
		int	y0 = m_heights.Image()[nLandYIdx+nLandXIdx],
			y1 = m_heights.Image()[nLandYIdx+nLandX1Idx],
			y2 = m_heights.Image()[nLandY1Idx+nLandX1Idx];
		height = (y2*v.y+y1*(v.x-v.y)+y0*(1.-v.x))*m_fHStep;
		normal=CFVector3(y0-y1,m_fCellSize0,y2-y0);
	}
	else {
		int	y0 = m_heights.Image()[nLandYIdx+nLandXIdx],
			y3 = m_heights.Image()[nLandY1Idx+nLandXIdx],
			y2 = m_heights.Image()[nLandY1Idx+nLandX1Idx];
		height = (y2*v.x+y3*(v.y-v.x)+y0*(1.-v.y))*m_fHStep;
		normal=CFVector3(y3-y2,m_fCellSize0,y3-y0);
	}
}


bool _CViewTerrain::FitInTrapezioid(SVector2 &v)
{
	if( IsViewAlongX() ) {
		if( IsViewAlongXN() ) {
			return DrawFitInTrapezioidL((int*)&v.x);
		}
		else {
			return DrawFitInTrapezioidR((int*)&v.x);
		}
	}
	else {
		if( IsViewAlongYP() ) {
			return DrawFitInTrapezioidB((int*)&v.x);
		}
		else {
			return DrawFitInTrapezioidT((int*)&v.x);
		}
	}
}


void _CViewTerrain::GenRed()
{
	static	int	entry = 0;
	const int count = 100;

	vt_countRed++;
	if( entry && vt_countRed < count ) return;
	entry = 1;
	if( vt_countRed >= count ) vt_countRed = 0;

	sc_randomRed = vt_randomRed;

	const	int	nMaxRed = 4;

	int x, y;
	for( x = 0 ; x < 512 ; x++ )
	for( y = 0 ; y < 512 ; y++ ) m_red.Image()[x+y*512] = 0;

	for( int r = nMaxRed ; r > 0 ; r-- ) {
		int d = 1<<r;
		for( x = 0 ; x < 512 ; x+=d )
		for( y = 0 ; y < 512 ; y+=d ) {
			if( m_red.Image()[x+y*512] == 0 && vt_randomRed(0,nMaxRed+1) < 1 ) {
				for( int x1 = x ; x1 < x+d ; x1++ )
				for( int y1 = y ; y1 < y+d ; y1++ ) m_red.Image()[x1+y1*512] = (byte)r;
			}
		}
	}

	for( x = 0 ; x < 512 ; x++ )
	for( y = 0 ; y < 512 ; y++ ) m_red.Image()[x+y*512] += 8*(byte)Max(
		Max(MAX_REDCELL(x,y),MAX_REDCELL(x-1,y)),Max(MAX_REDCELL(x,y-1),MAX_REDCELL(x-1,y-1)));
}

double _CViewTerrain::Waterline()
{
	return	__terrainWaterline*m_fCellSize*m_fHScale;
}

