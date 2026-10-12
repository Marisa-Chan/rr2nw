#ifndef		__OBJMAP_H__
#define		__OBJMAP_H__


#if _USE_OPENGL
void BeginDrawFromLand();
void EndDrawFromLand();
#elif _USE_OWNGR
#define BeginDrawFromLand() ((void)0)
inline void EndDrawFromLand() { GRSetZPrecision(0); }
#else
#define BeginDrawFromLand() ((void)0)
#define EndDrawFromLand() ((void)0)
#endif


class	CLandObjectMap2;

class	_CViewTerrain;

class	CLandDynamicMap {
protected:
	CViewDynamicListLoop	*m_pCells, *m_pOrigin;
	dword					*m_pCellsLight, *m_pOriginLight;
	double	m_fCellSize, m_f_1_CellSize;
	CRect2	m_rect, m_rectT, m_rect0;
	int		m_nOrigin;
	CVector2	m_size, m_viewpoint;
	CViewDynamicListLoop	m_afterList;
public:
	void Destroy() { delete [] m_pCells; m_pCells = NULL; delete [] m_pCellsLight; m_pCellsLight = NULL; }
	void SetOrigin(TCSVector2 &origin) {
		m_rect = m_rect0;
		m_rect.MoveBy(origin);
		m_rectT.left = m_rect.top;
		m_rectT.top = m_rect.left;
		m_rectT.right = m_rect.bottom;
		m_rectT.bottom = m_rect.right;
		m_nOrigin = -m_rect.left-m_rect.top*m_size.x;
		m_pOrigin = m_pCells + m_nOrigin;
		m_pOriginLight = m_pCellsLight + m_nOrigin;
		m_viewpoint = origin;
		//CLandObjectMap1::SetDynamicsOrg(m_nOrigin);
	}
	void SetOrigin(TCSFVector3 &origin);
	TCCRect2   &Rect() const { return m_rect; }
	TCCRect2   &RectT() const { return m_rectT; }
	TCCVector2	 &Size() const { return m_size; }
	CViewDynamicListLoop *&Origin() { return m_pOrigin; }
	dword *&OriginLight() { return m_pOriginLight; }
	bool IsEmpty();
	/*void SetOrigin(TCCFVector3 &origin) {
		m_rect = m_rect0;
		m_rect.MoveBy(CVector2(floor(origin.x/m_fCellSize),floor(-origin.z/m_fCellSize)));
		m_nOrigin = -m_rect.left-m_rect.top*m_size.x;
		CLandObjectMap1::SetDynamicsOrg(m_nOrigin);
	}*/
	void Create(TCCVector2 &size,double fCellSize) {
		Destroy();
		m_size = size;
		m_rect0 = CRect2(CVector2(0,0),CVector2(size));
		m_rect0.MoveBy(CVector2(-size.x/2,-size.y/2));
		m_pCells = new CViewDynamicListLoop[size.x*size.y];
		m_pCellsLight = new dword[size.x*size.y];
		memset(m_pCellsLight,0,size.x*size.y*sizeof(dword));
		m_fCellSize = fCellSize;
		m_f_1_CellSize = 1./fCellSize;
		SetOrigin(CVector2(0,0));
		ASSERT( IsEmpty() );
	}
	CLandDynamicMap() { m_pCells = NULL; m_pCellsLight = NULL; }
	~CLandDynamicMap() { Destroy(); }
	//void Attach(CLandObjectMap2 &map) { map.SetDynamicsInfo(m_pCells,m_size.x,m_size.y); }
	//void Update(CLandObjectMap2 &map) { map.SetDynamicsRect(m_rect); }
	void SetupDynamics(CViewDynamicList &list,TCSRect2 &rect,_CViewTerrain &terrain);
	void SetupLights(dword dwLights);
	void SetupLandSticks(CViewDynamicList &list,_CViewTerrain &terrain);
	void RemoveDynamic(CViewDynamic *p) { if( m_rect.Contains(p->m_cell) ) m_pOrigin[p->m_cell.x+p->m_cell.y*m_size.x].Clear(); }
	void RemoveLights(dword dwLights);
	CViewDynamicListLoop	&AfterList() { return m_afterList; }
	void ApplyLights(CViewDynamic *pd);
};


class	CLandObjectMap1	{
protected:
	class CSecondaryEntry {
	public:
		int  x;
		CViewOrdered *pObj;
		void Draw(CViewDynamicListLoop *pList,WATER_VIEW eWaterView,dword dwLights) {
			if( this ) {
				ASSERT(pObj);
				CViewObject::m_eWaterView = eWaterView;
				if( pList && pList->First()) {
					CViewDynamicList::WasteBox().Clear(FALSE);
					pObj->LoadDyns(*pList); pObj->LoadLights(dwLights);
					BeginDrawFromLand();
					pObj->Draw();
					EndDrawFromLand();
					//pList->Move(CViewDynamicList::WasteBox());
				}
				else {
					BeginDrawFromLand();
					pObj->LoadLights(dwLights);
					pObj->Draw();
					EndDrawFromLand();
				}
			}
			else if( pList && pList->First()) {
				CViewObject::m_eWaterView = eWaterView;
			 	BeginDrawFromLand();
				//pList->Draw(FALSE);
				CViewDynamicList::WasteBox().Clear(FALSE);
				pList->ZSortDrawNDrop();
				//pList->Move(CViewDynamicList::WasteBox());
			 	EndDrawFromLand();
			}
		}
		bool Bump(SBumpDef0 &def) { return this?pObj->Bump(def):FALSE; }
		CViewOrdered *Obj() { return this ? pObj : NULL; }
		//void LoadDyn(CViewDynamicListLoop *pList) { if(pList&&pList->First()) pObj->LoadDyns(*pList); }
	};
	class CPrimaryEntry {
	public:
		int		m_n, m_xmin, m_xmax, m_x, m_xreq;
		CSecondaryEntry	*m_pSE0, *m_pSE;
		CSecondaryEntry *___Find(int x);
		CSecondaryEntry *_Find(int x);
		CSecondaryEntry *Find(int x);
	};
	CPrimaryEntry	*m_pPrimaries;
	int m_xplow, m_xphigh;
	int m_viewxp, m_viewxs;
	bool m_bReduce;
	int m_nReduction, m_nReductionAdd;
	bool m_bIsCopy;
	CViewDynamicListLoop	**m_pDynamicsOrg;
	dword					**m_pLightsOrg;
	TCSRect2				*m_pDynamicsRect;
	int	m_nDynamicsXPStep, m_nDynamicsXSStep;
	//CRect2	m_dynamicsRect;
	//static	int m_nDynamicsOrigin;
	//#if _USE_OPENGL
		static bool m_bInitDrawObj;
	//#endif

	CLandObjectMap1() {}
	CLandObjectMap1(int xplow,int xphigh);
	void Read(CTaggedFile &f,int xplow,int xphigh,
							CViewOrdered *(*ReadOrder)(CTaggedFile &f),
							CLandObjectMap1 *pSrc = NULL);
	void Attach(CLandDynamicMap &map,TCSRect2 *pRect,int xpStep,int xsStep) {
		m_pDynamicsOrg = &map.Origin(); m_pDynamicsRect = pRect; m_nDynamicsXPStep = xpStep; m_nDynamicsXSStep = xsStep;
		m_pLightsOrg = &map.OriginLight(); }
	CSecondaryEntry *Find(int xp,int xs) { //(void)xp; (void)xs; return NULL; }
								//ASSERT(xp>=m_xplow&&xp<m_xphigh);
								if( xp < m_xplow || xp >= m_xphigh ) return NULL;
								return m_pPrimaries[xp-m_xplow].Find(xs); }
	void DrawReductionSec(CPrimaryEntry &prim,int xp,int xs,WATER_VIEW eWaterView);
	void DrawReductionPrim(CPrimaryEntry &prim,int xp,TCSRect2 &cellSP,WATER_VIEW eWaterView);
	void DrawReduction(TCSRect2 &cellSP,WATER_VIEW eWaterView);
	void ApplyModelUsage();
	CViewDynamicListLoop *FindDyn(int xp,int xs,dword &dwLights) {
		if( !m_pDynamicsRect->Contains(CVector2(xp,xs)) ) { dwLights = 0; return NULL; }
		dwLights = (*m_pLightsOrg)[xp*m_nDynamicsXPStep+xs*m_nDynamicsXSStep];
		return &(*m_pDynamicsOrg)[xp*m_nDynamicsXPStep+xs*m_nDynamicsXSStep];
	}
	friend class CLandObjectMap2;
public:
	~CLandObjectMap1();
	void Draw(TCSRect2 &cellSP,WATER_VIEW eWaterView) {
		dword dwLights;
		if( !m_bReduce ) {
			CViewDynamicListLoop *dyn = FindDyn(cellSP.top,cellSP.left,dwLights);
			Find(cellSP.top,cellSP.left)->Draw( dyn, eWaterView, dwLights);
		} else
			DrawReduction(cellSP,eWaterView);
	}
	bool Bump(SBumpDef0 &def, int xp,int xs) { return Find(xp,xs)->Bump(def); }
	void SetViewPoint(int xp,int xs) { m_viewxp = xp-m_xplow; m_viewxs = xs; }
	void SetReduction(int nReduction,double fNext) {
		m_bReduce = nReduction>0; (void)fNext;
		m_nReduction = nReduction; m_nReductionAdd = (1<<nReduction)-1; }
	//void SetDynamicsRect(TCCRect2 &rect) { m_dynamicsRect = rect; }
	//static void SetDynamicsOrg(int nOrigin) { m_nDynamicsOrigin = nOrigin; }
	//void Attach(CLandDynamicMap &dmap) { m_pDynamicsMap = &dmap; }
};

class	CLandObjectMap2	{
protected:
	CLandObjectMap1	m_mapX, m_mapY;	// name = outer loop (primary coord)
	dword	**m_pLightsOrigin;
	int		m_nLightsPitch;
	TCSRect2	*m_pDynamicsRect;
public:
	CLandObjectMap2() {}
	CLandObjectMap2(TCCRect2 &rect) :
				m_mapX(rect.left,rect.right), m_mapY(rect.top,rect.bottom) {} // m_pDynamics = NULL; }
	void Attach(CLandDynamicMap &dmap) {
		m_mapX.Attach(dmap,&dmap.Rect(),1,dmap.Size().x);
		m_mapY.Attach(dmap,&dmap.RectT(),dmap.Size().x,1);
		m_pLightsOrigin = &dmap.OriginLight();
		m_nLightsPitch = dmap.Size().x;
		m_pDynamicsRect = &dmap.Rect();
	}
	//void SetDynamicsInfo(CViewDynamicListLoop *pDynamics,int nDimX,int nDimY) {
	//	m_mapX.SetDynamicsInfo(pDynamics,1,nDimX);
	//	m_mapY.SetDynamicsInfo(pDynamics,nDimX,1); (void)nDimY;
	//}
	/*void SetDynamicsRect(TCCRect2 &rect) {
		m_mapX.SetDynamicsRect(rect);
		m_mapY.SetDynamicsRect(CRect2(rect.top,rect.left,rect.bottom,rect.right));
	}*/
	void Read(CTaggedFile &f,TCCRect2 &rect,
								CViewOrdered *(*ReadOrder)(CTaggedFile &f));
	CLandObjectMap1 &MapX() { return m_mapX; }
	CLandObjectMap1 &MapY() { return m_mapY; }
	bool Bump(SBumpDef0 &def,int x,int y) {
		return fabs(def.vel.x)>=fabs(def.vel.z)?m_mapY.Bump(def,y,x):
													m_mapX.Bump(def,x,y); }
	void ApplyModelUsage() { m_mapX.ApplyModelUsage(); }
	dword *&LightsOrigin() { return *m_pLightsOrigin; }
	int	  LightsPitch() { return m_nLightsPitch; }
	TCSRect2	&DynamicsRect() { return *m_pDynamicsRect; }
};

class	CLandscapeRect	{
public:
	typedef	byte THeight;
protected:
	CLandObjectMap2	m_map;
	CRect2		m_rect;
	enum { HEIGHT_MASK = 255 /*(1<<12)-1*/ };
	THeight	*m_pHeights;
	int		m_nSize, m_nAxisMask, m_nLandSizeShift;
	double	m_fCellSize, m_fScaleHeight, m_fCellSize0;
	// m_fHeightScale = m_fCellSize*hratio/256=m_fCellSize*fHRatio
	// m_fCellSize0 = m_fCellSize/m_fHeightScale=1/fHRatio;
	// m_fCellSize0 = 1/fHRatio

	bool _BumpLandCellAdvanced(SBumpDef &def,int nLandX,int nLandY);
	bool _BumpLandCell(SBumpDef &def,int nLandX,int nLandY);
	bool BumpLandCell(SBumpDef &def,int nLandX,int nLandY);
	virtual bool Bump(SBumpDef &def,int nLandX,int nLandY);
	bool BumpX(TCCRect2 &rect,SBumpDef &def);
	bool BumpY(TCCRect2 &rect,SBumpDef &def);
	void Bump(int nLandX,int nLandY,CViewDynamic *pd);
	//void BumpX(TCCRect2 &rect,CViewDynamic *pd);
	//void BumpY(TCCRect2 &rect,CViewDynamic *pd);
	void BumpX(TCCRect2 &rect,CViewDynamicList &list);
	void BumpY(TCCRect2 &rect,CViewDynamicList &list);
public:
	void Setup(THeight *pHeights,
						int nSizeShift,double fCellSize,double fHRatio);
	void Read(CTaggedFile &f,TCCRect2 &rect,
			CViewOrdered *(*ReadOrder)(CTaggedFile &f)) {
									m_map.Read(f,m_rect=rect,ReadOrder); }
	CLandObjectMap2 &Map() { return m_map; }
	//void Bump(CViewDynamic *pd) { BumpX(m_rect,pd); }
	void Bump(CViewDynamicList &list) { BumpX(m_rect,list); }
	bool Bump(SBumpDef0 &def) { ASSERT(def.eType == SBumpDef::T_BUMPDEF);
									return BumpX(m_rect,*(SBumpDef*)&def); }
	void ApplyModelUsage() { m_map.ApplyModelUsage(); }
	double ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d);
	void AdjustPlane(CFVector3 &pt,CFVector3 &n,double &d);
};


template<class T> class CArrayRef {
protected:
	int m_nL, m_nR;
	T *m_p0;
public:
	CArrayRef() { m_p0 = NULL; m_nL = 0; m_nR = -1; }
	CArrayRef( T *t0, int nCount ) { m_p0 = t0; m_nL = 0; m_nR = nCount; }
	CArrayRef( const CArrayRef<T> &r, int nShift ) { m_p0 = r.m_p0+nShift; m_nL = r.m_nL-nShift; m_nR = r.m_nR-nShift; }
	T &operator[](int i) { ASSERTMSG( i >= m_nL && i < m_nR, "Array bounds check"); return m_p0[i]; }
};

template<class T> class _CViewTerrainMap {
protected:
	int	m_nWidth, m_nHeight, m_nHeightShift;
	int m_nWidthMask, m_nHeightMask;
	T *m_pImage;
    GR_HTEXTURE m_hImage;
	#if _USE_GLIDE
		enum { EXTRA_INFO = 12 };
	#else
		enum { EXTRA_INFO = 12 };
	#endif
public:
	void Load(TCchar *name,TCchar *palName=NULL) {
		ASSERT(palName==NULL);
		FILE *f = CFileResource::FOpenCurrent(name);
		RTCHECK1(f,"File '%s' not found",name);
		word nWidth, nHeight; byte nTransp;
		RTCHECK1(fread(&nWidth,2,1,f)==1&&
				fread(&nHeight,2,1,f)==1&&
				fread(&nTransp,1,1,f)==1,"Err reading map '%s'",name);
		ReverseByteOrderVarBE(nWidth);
		ReverseByteOrderVarBE(nHeight);
		m_nWidth = nWidth; m_nHeight = nHeight;
		ASSERT((nWidth|(nWidth-1))==nWidth*2-1);
		ASSERT((nHeight|(nHeight-1))==nHeight*2-1);
		m_nWidthMask = m_nWidth-1; m_nHeightMask = m_nHeight-1;
		m_nHeightShift = 0;
		for( int i = 1 ; i < m_nHeight ; i<<=1 )	m_nHeightShift++;
		ASSERT( 1 << m_nHeightShift == m_nHeight );
		int nDim = m_nWidth*m_nHeight;
		m_pImage = (T*)(new byte[sizeof(T)*nDim+EXTRA_INFO]+EXTRA_INFO);
		RTCHECK1(fread(m_pImage,sizeof(T),nDim,f)==nDim,"Err reading map '%s'",name);
		fclose(f);
		((dword*)m_pImage)[-1] = nWidth;
		((dword*)m_pImage)[-2] = nHeight;
        ((dword*)m_pImage)[-3] = TEXTURE_UNLOAD | TEXTURE_PAL_FORMAT;
        m_hImage = NULL;
		if(palName) {
			paletteTranslator.DefineFile64(palName);
			//for( int i = 0 ; i < nDim ; i++ )
			//	m_pImage[i] = paletteTranslator(m_pImage[i]);
            m_hImage = GRLoadTextureToDB(NULL, (byte*)paletteTranslator.LocalPalette(), paletteTranslator.NumEntries(), (byte*)m_pImage);
            ASSERT(m_hImage);
		}
	}
	_CViewTerrainMap() { m_pImage = NULL; m_hImage = NULL; }
    ~_CViewTerrainMap() { Clear(); }
	void Clear() { if( m_pImage ) delete [] ((byte*)m_pImage-EXTRA_INFO);
                    GRDeleteTextureFromDB(m_hImage); m_pImage = NULL; m_hImage = NULL; }
	int Width() const { return m_nWidth; }
	int Height() const { return m_nHeight; }
	T *Image() const { return m_pImage; }
    GR_HTEXTURE Texture() const { return m_hImage; }
	T operator()(int x,int y) const { return m_pImage[(x&m_nWidthMask)+((y&m_nHeightMask)<<m_nHeightShift)]; }
	void CheckDimensions(int x,int y) const {
		RTCHECK(m_nWidth==x&&m_nHeight==y,"Map dimensions error");
	}
};

class	CViewTerrainRect {
public:
	struct	SEdge {
		int	n;
		int *p;		// first and last point must be out of rect
	};
	typedef const SEdge TCSEdge;
protected:
	CRect2	m_rect;
	SEdge	m_edges[4];	// left, top, right, bottom
	int		*m_pEdgePointsAll;
public:
	CViewTerrainRect() { m_pEdgePointsAll = NULL; }
	~CViewTerrainRect() { delete [] m_pEdgePointsAll; }
	TCCRect2 &Rect() { return m_rect; }
	TCSEdge &Edge(int i) { return m_edges[i]; }
};

class	CLandObjectMap2;

class	_CViewTerrain {
public:
    // By Green
    double m_curTime;
    int    m_curTimeX, m_curTimeY;
    int m_twBaseX[513], m_twBaseY[513];
    int m_twxPhaseX[513], m_twyPhaseX[513];
    int m_twxPhaseY[513], m_twyPhaseY[513];
    int m_tbxPhaseX[513], m_tbyPhaseX[513];
    int m_tbxPhaseY[513], m_tbyPhaseY[513];
    int m_twModulateX[8193], m_twModulateY[8193];
    int m_tbModulateX[8193], m_tbModulateY[8193];
    //int m_maxCosVals;

    class CWaterParams{
       public:
       int    m_transWater, m_transBump;
       double m_xwTimeP, m_ywTimeP, m_txwTimeP, m_tywTimeP;
       double m_xwTimeLandPx, m_xwTimeLandPy, m_ywTimeLandPx, m_ywTimeLandPy;
       int    m_xwStaticA, m_ywStaticA, m_xwDynamicA, m_ywDynamicA;
       double m_xbTimeP, m_ybTimeP, m_txbTimeP, m_tybTimeP;
       double m_xbTimeLandPx, m_xbTimeLandPy, m_ybTimeLandPx, m_ybTimeLandPy;
       int    m_xbStaticA, m_ybStaticA, m_xbDynamicA, m_ybDynamicA;
       int    m_maxPhaseVal;
       int    m_phaseMask;
    } m_wp;

    void SetCurTime(double curTime) {
       m_curTime  = curTime;
       m_curTimeX = curTime*m_wp.m_txwTimeP;
       m_curTimeY = curTime*m_wp.m_tywTimeP;
    }
    void InitWaterTabulation();
    void InitWaterParams(const CWaterParams &p) { m_wp = p;}
    void ChangeWaterTextureState() { m_wp.m_transWater = 1-m_wp.m_transWater;}
    void ChangeWaterBumpState()    { m_wp.m_transBump  = 1-m_wp.m_transBump;}
protected:
    inline bool IsWaterCell(int x, int y);
    inline bool IsWaterVertex(int x, int y);
    CVector2 CalcWaterCoords(int x, int y);
    void CalcWaterBumpCoords();
    // End by Green


protected:
	//class	CEdge {
	//public:
	//	int		l, r;		// l < r
	//};
	class CMipMapHeight {
	protected:
		int		m_nWidth, m_nHeight;
		byte	(*m_pImage)[2];
		int		m_nShiftR;
	public:
		CMipMapHeight() { m_pImage = NULL; }
		~CMipMapHeight() { delete [] m_pImage; }
		void Init(_CViewTerrainMap<byte> &map,int nShiftReduction,TCchar *name=NULL);
		byte   GetLimit(int x,int y,int nMinMax,int nShiftReduction);
		byte   *operator()(int x,int y);
	};

	/*struct SVert {
		int x,y,z,u,v;
		//void SetColor(word color) { u = (color&255)<<16; v = (color&0xFF00)<<8; }
		//void SetColor(TCCFVector2 &c); // { u = Round(c.y*65536); v = Round(c.x*65536); }
	};*/

	_CViewTerrainMap<byte>	m_heights, m_brightness;
	_CViewTerrainMap<word>	m_colors;
	GR_HTEXTURE		m_hMask, m_hBump, m_hMaskW, m_hBumpW, m_hMask1;
	_CViewTerrainMap<byte>	m_red, m_maskflag;
	enum { HRANGE_SHIFTREDUCTION = 3, MAX_CELLVERTS = 64 };
	CMipMapHeight			m_hrange;
	bool		m_bIsHardware;
	//int			*m_pMaskCache;
	int		m_nWidth, m_nHeight;
	long		m_polygonRectL[4];
	CRect2		m_polygonRect;
	long		m_edgesOuter[2];
	int			m_nReductions, m_nMaxEdges;
	bool		m_bTerrainInView;
	int			(**m_ppEdges)[2], (*m_pEdgeR)[2];
	IF_DEBUG(int  *m_pnEdges);
	int			m_viewpointL[2];
	CFVector2	m_viewpoint;
	CFMatrix3x4	m_viewPointDirMx;
	CFVector3	m_zCoeff;
	double		m_zCoeff0;
	int			m_nNearestHeight;
	CVector2	m_nearestOffset;
	//CVertex		*m_pCells, *m_pCellsPrev, *m_pCellsNext;
	double		m_fCellSize, m_fHScale, m_f_1_CellSize, m_fCellSize0, m_fHStep;

	double		m_fDistRed0, m_fDistRed0Sq2, m_fDistRed02;
	int			m_nHiResShift;

	static	GR_HTEXTURE	LoadMask(TCchar *name,int dim,GR_HTEXTURE hOld=NULL);
	static	GR_HTEXTURE ReloadMask(TCchar *name,GR_HTEXTURE hOld,void *);

	GR_HTEXTURE	LoadMaskLand(TCchar *name,bool bHiRes);

	bool IsViewAlongX();
	bool IsViewAlongY();
	bool IsViewAlongXP();
	bool IsViewAlongXN();
	bool IsViewAlongYP();
	bool IsViewAlongYN();
	//static int	ReductionStep(int d);
	double Reduction(double z);
	//double  CellZ(int x,int y);
	static	CVector2	V2Mul_SHRD(TCSVector2 &s,int mul,int shift);
	static	CVector2	Vector2(word w);
	//static	word		Word(TCCFVector2 &v);
	double Reduction(int x,int y);
	int	   ReductionInt(double z);
	int	   ReductionCell(int x,int y);
//	CVector2 CalcWaterCoords(int x, int y); //redeclare
	int	   _CalcPointReduction(int x,int y,int n,SVector2 &w,bool bLand,bool bWater);
	int    CalcPointReduction(int x,int y,int n,SVector2 &w,bool bLand,bool bWater);

	void DrawSealB(TCint rectL[4]);
	bool DrawLPatchB(TCint rectL[4]);
	bool DrawLRunB(int &y,int &ylower,int ypatchU_L,TCint rectL[4]);
	bool DrawRRunB(int &y,int &ylower,int ypatchL_L,TCint rectL[4]);
	bool DrawLinkRunB(int ylow,bool bDrawL,bool bDrawU);
	void DrawInitB(TCint rectL[4],bool bVis);
	void DrawFrontB(TCint rectL[4]);
	void DrawFullB(TCint rectL[4],CLandObjectMap1 &map);

	bool DrawLPatchT(TCint rectL[4]);
	bool DrawLRunT(int &y,int &ylower,int ypatchU_L,TCint rectL[4]);
	bool DrawRRunT(int &y,int &ylower,int ypatchL_L,TCint rectL[4]);
	bool DrawLinkRunT(int ylow,bool bDrawL,bool bDrawU);
	void DrawInitT(TCint rectL[4],bool bVis);
	void DrawFrontT(TCint rectL[4]);
	void DrawFullT(TCint rectL[4],CLandObjectMap1 &map);

	bool DrawLPatchL(TCint rectL[4]);
	bool DrawLRunL(int &y,int &ylower,int ypatchU_L,TCint rectL[4]);
	bool DrawRRunL(int &y,int &ylower,int ypatchL_L,TCint rectL[4]);
	bool DrawLinkRunL(int ylow,bool bDrawL,bool bDrawU);
	void DrawInitL(TCint rectL[4],bool bVis);
	void DrawFrontL(TCint rectL[4]);
	void DrawFullL(TCint rectL[4],CLandObjectMap1 &map);

	void DrawSealR(TCint rectL[4]);
	bool DrawLPatchR(TCint rectL[4]);
	bool DrawLRunR(int &y,int &ylower,int ypatchU_L,TCint rectL[4]);
	bool DrawRRunR(int &y,int &ylower,int ypatchL_L,TCint rectL[4]);
	bool DrawLinkRunR(int ylow,bool bDrawL,bool bDrawU);
	void DrawInitR(TCint rectL[4],bool bVis);
	void DrawFrontR(TCint rectL[4]);
	void DrawFullR(TCint rectL[4],CLandObjectMap1 &map);


	bool DrawFitInTrapezioidL(int *v);
	bool DrawFitInTrapezioidR(int *v);
	bool DrawFitInTrapezioidB(int *v);
	bool DrawFitInTrapezioidT(int *v);



	enum	{ VF_WATER_DATA = 1, VF_BELOW_WATER = 2 };

	struct	SVert3_0	{
		//double x3, y3, z3;
		SFVector3	v3;
		int cf;
		int u,v;
		int x, y, iz;
		void SetColor(word color) { u = (color&255)<<16; v = (color&0xFF00)<<8; }
		void SetColor(word color,int d) { u = (color&255)<<(16+d); v = (color&0xFF00)<<(8+d); }
		void SetColor(TCCVector2 &c); // { u = Round(c.y*65536); v = Round(c.x*65536); }
	};

	struct	SVert3 : public SVert3_0 {
		int		L[2];
		int		lf;
		int 	b;
		void SetLCoord(int x,int y);
		void Interpolate(SVert3 &v0,SVert3 &v1,int coord);
	};
	friend struct SVert3;

	struct	SVert3W : public SVert3 {
		int			wf;
		double		h0;
		SVert3_0	w;
	};

	enum { I_X = 0, I_Y = 1 };
	enum { I_L = 0, I_R = 1 };
	enum { I_LEFT = 0, I_TOP = 1, I_RIGHT = 2, I_BOTTOM = 3 };
	struct CCellVertex : public SVert3W {
		//SVert3	v;
		int		rc;
		int		nxt;
		int		nextrun;
	};
	static	void	Copy(SVert3W &d,const CCellVertex &s);

	int	 m_iCacheL_U, m_iCacheU_U;
	int	 m_iCacheL_L, m_iCacheU_L;
	CCellVertex	*m_pCallLinkL_U, *m_pCallLinkU_U, *m_pStallL_U, *m_pStallU_U;
	CCellVertex	*m_pCallLinkL_L, *m_pCallLinkU_L, *m_pStallL_L, *m_pStallU_L;
	CCellVertex *m_pCallPatchL_U, *m_pCallPatchU_U;
	CCellVertex	*m_pCallPatchL_L, *m_pCallPatchU_L;
	CCellVertex	*m_pCallSeal_L, *m_pCallSeal_U;

	CCellVertex  m_callPatchL_U, m_callPatchU_U, m_callPatchL_L, m_callPatchU_L;
	int  m_ynewL_L, m_ynewL_U;
	int  m_ynewR_L, m_ynewR_U;
	int  m_ylowerL_L, m_ylowerL_U;
	int  m_ylowerR_L, m_ylowerR_U;
	int  m_ylinkL_L, m_ylinkU_L;
	int  m_ylinkL_U, m_ylinkU_U;
	//int  m_ylink_L, m_ylink_U;
	int  m_xedgeL_L, m_xedgeU_L, m_xedgeL_U, m_xedgeU_U;
	int  m_xlinkL_L, m_xlinkU_L, m_xlinkL_U, m_xlinkU_U;
	int  m_xlinkInit_L, m_xlinkInit_U;
	int	m_ypatchL_L, m_ypatchU_L, m_ypatchL_U, m_ypatchU_U;
	bool m_bDrawLeft_L, m_bDrawLeft_U;
	bool m_bDrawRight_L, m_bDrawRight_U;
	bool m_bWaitPatch_L, m_bWaitPatch_U;
	bool m_bStartedPatch_L, m_bStartedPatch_U;
	bool m_bVisLeft_U, m_bVisRight_U, m_bVisLeft_L, m_bVisRight_L;
	bool m_bStallL_L, m_bStallU_L, m_bStallL_U, m_bStallU_U;
	bool m_bVis_U, m_bVis_L;
	int m_rectL[4];
	int m_lastStartL_U[2], m_lastStartU_U[2], m_lastStartL_L[2], m_lastStartU_L[2];
	//dword	m_dwLights;
	dword	**m_pLightsOrigin;
	int		m_nLightsPitch;
	TCSRect2	*m_pDynamicsRect;

	CCellVertex	*m_pCache0, *m_pCacheXL0, *m_pCacheXU0, *m_pCacheYL0, *m_pCacheYU0;
	//CCellVertex *m_pCacheS, *m_pCachePL, *m_pCachePU;
	CArrayRef<CCellVertex> m_pCacheXL, m_pCacheXU, m_pCacheYL, m_pCacheYU;
	SVert3W		m_cellVerts[MAX_CELLVERTS];
	SVert3_0	*m_apCellVerts[MAX_CELLVERTS], *m_apCellVertsW[MAX_CELLVERTS],
												*m_apCellVertsWR[MAX_CELLVERTS];
	SVector2	m_bumpVerts[MAX_CELLVERTS], m_bumpVertsW[MAX_CELLVERTS];
	int			m_nTriangleMark[4];
	int			m_nCellVerts;
	bool		m_bCellBump, m_bCellBumpW, m_bNeedLand;
	double		m_fWaterline0;
	bool		m_bIsAboveWater, m_bIsAboveWaterCpl, m_bIsBelowWaterCpl;

	int			m_nLandClipFlags, m_nWaterClipFlags;

	CLandObjectMap1	*m_pMap1;

	enum TRIANGULATE { TRIANG_NORMAL, TRIANG_CHANGED, TRIANG_NO };

	struct	SLightNormal { float a,b,c,d; };
	SLightNormal	m_waterNormal;

	void CreateCellVertex(int x,int y,SVert3W &v);
	dword CreateHWColor(dword b);
	void InterpolateClipped(double fh,double dh,const SVert3W &v0, TCSVector2 &bump0,
			const SVert3W &v1, TCSVector2 &bump1, int &ngr, bool bBump, bool bGouraud );
	void DrawTriangleSplit(bool bSplitUp,SVert3W **ppCellVerts,int iVert1,int iVertT,int iVert2,int nCellVerts,int mask,bool bCellBump,SVector2 *pBumpVerts,int nWater,bool bGouraud);
	void DrawTriangle(WATER_SPLIT eSplit,SVert3_0 **ppCellVerts,int iVert1,int iVertT,int iVert2,int nCellVerts,int mask,bool bCellBump,SVector2 *pBumpVerts,int nWater,bool bGouraud);
	void DrawRect(TRIANGULATE eTriangulate,WATER_SPLIT eSplit,
						SVert3_0 **ppCellVerts0,SVert3_0 **ppCellVerts,
						int nCellVerts,int mask,bool bBump,SVector2 *pBumpVerts,int nWater,int t0,int t1,int t2,int t3);
	bool CalcTriangleNormal(SVert3_0 **ppCellVerts,int t0,int t1,int t2,int nWater);
	void BeginDrawCell();
	void PutTriangleMark(int n);
	void AddCellVertex(CCellVertex &v);
	void EndDrawCell(int rc,TCSRect2 &cell);

public:
	_CViewTerrain(double fCellSize,double fHScale,double fViewDiameter);
	~_CViewTerrain();
	void SetViewPoint();
	void Draw(TCCRect2 &rect,CLandObjectMap2 &map);
	TCbyte *GetHeightMap() { return m_heights.Image(); }
	void SetReductionStart(double fDist) /* in cells */ {
		m_fDistRed0 = fDist; m_fDistRed02 = fDist*2; m_fDistRed0Sq2 = fDist*M_SQRT2; }
	void EndDrawTerrain();
	void GenRed();
	void VT_DrawPixel(int x,int y);
	CVector2 ViewpointL() { return CVector2(m_viewpointL[I_X],m_viewpointL[I_Y]); }
	void GetPlane(SFVector3 v,SFVector3 &normal,double &height);
	double CellSize() { return m_fCellSize; }
	bool FitInTrapezioid(SVector2 &v);
	double Waterline();

	void	LoadHWSW();
	void	ClearHWSW();
};

extern CRandomGenerator vt_randomRed, vt_randomPos, sc_randomRed, sc_randomPos;
extern int vt_countRed, sc_countRed;

#endif
