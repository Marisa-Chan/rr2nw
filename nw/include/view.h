#ifndef	__VIEW_H__
#define	__VIEW_H__

#include"namerefs.h"
#include"dynamic.h"
// -------------------------------------------------------------------------
#define	MAX_BSP_ERROR	1e-5

class CVector3 {
public:
	int		x,y,z;
};
typedef const CVector3 TCCVector3;

class SFVector3Y : public SFVector3 {
public:
	//int		x,y,z;
	double	y0;
};
typedef const SFVector3Y TCSFVector3Y;

enum WATER_SPLIT	{ WATER_SPLIT_UP, WATER_SPLIT_DOWN, WATER_SPLIT_NONE };
enum WATER_VIEW	{ WATER_VIEW_ABOVE, WATER_VIEW_BELOW, WATER_VIEW_ALL };

class	CFMatrix3x4Y : public CFMatrix3x4 {
public:
	double	y[4];
	double  Y(TCCFVector3 &v) const { return y[0]*v.x+y[1]*v.y+y[2]*v.z+y[3]; }
	void	YFrom(CFMatrix3x4 &m) {
		y[0]=m.m[1][0]; y[1]=m.m[1][1]; y[2]=m.m[1][2]; y[3]=m.m[1][3]; }
	void	YScale(double f) { y[0]*=f; y[1]*=f; y[2]*=f; y[3]*=f; }
};
typedef	const CFMatrix3x4Y TCCFMatrix3x4Y;

// -------------------------------------------------------------------------
class	CViewOrdered	{
protected:
	bool	m_bEmbedded, m_bDynamics;
	CViewDynamicList	m_dynamicList, m_afterList, m_shotList;
	double				m_fMaxHeight;
	CViewOrdered	*m_pTopOrder;
	dword			m_dwLights;
	virtual void DrainDynamicFlag() {}
	static CViewOrdered	*m_pCurrentTopOrdered;
	static CViewDynamicList &DynamicList(CViewOrdered *po) { return po->m_dynamicList; }
	static CViewDynamicList &ShotList(CViewOrdered *po) { return po->m_shotList; }
	friend class CViewOrder;
	friend class CShelterOrder;
public:
	CViewOrdered();
	//void SetEmbedded(bool bEmbed) { m_bEmbedded = bEmbed; }
	virtual	~CViewOrdered() {};
	virtual void Draw() = 0;
	virtual void DrawViewStartHere() { Draw(); }
	void DrawView() { m_pTopOrder->DrawViewStartHere(); }
	void LoadDyns(CViewDynamicList &list) { m_dynamicList.Move(list); }
	void LoadShots(CViewDynamicList &list) { m_shotList.Move(list); }
	void LoadDyns(CViewDynamicListLoop &list) { m_dynamicList.Move(list); }
	void AppendDyn(CViewDynamic *pd) { m_dynamicList.Load(pd); }
	void AppendShot(CViewDynamic *ps) { m_shotList.Load(ps); }
	void RemoveDyn(CViewDynamic *pd) { m_dynamicList.Remove(pd); }
	//void LoadDyns(CViewDynamicListLoop &list) { m_dynamicList.Move(list); }
	void LoadLights(dword dwLights) { m_dwLights = dwLights; }
	virtual void PromoteDynamic();
	virtual	bool Bump(SBumpDef0 &def) = 0;
	virtual void RemoveDynamics();
	static CViewOrdered *SetCurrentTop(CViewOrdered *pOrd) { CViewOrdered *pOld = m_pCurrentTopOrdered; m_pCurrentTopOrdered = pOrd; return pOld; }
	static CViewOrdered *GetCurrentTop() { return m_pCurrentTopOrdered; }
	static CViewOrdered *Read(CTaggedFile &f);
	enum  VORDTYPE { VOT_ORDER = 0, VOT_BODY = 1, VOT_OBJECT = 2,
		VOT_LANDPIECE = 3, VOT_BASESET = 4, VOT_SHELTERORDER = 5,
		VOT_EMPTY = 6, VOT_MORPHEDORDER = 7, VOT_BUSH = 8, VOT_USER = 10 };
									// This enum values cannot be changed!!!
	virtual VORDTYPE Type() const = 0;
	void Bang(CViewDynamicList &shotList);
	void Delete() { if(this!=NULL&&m_bEmbedded) delete this; }
	virtual void operator%=(TCCFMatrix3x4 &dir) { (void)dir; ASSERT(0); }
	virtual void ApplyModelUsage() { ASSERT(0); }
	virtual void ComputeMaxHeight() { ASSERT(0); }
	//virtual void Split() { ASSERT(0); }
	#ifdef	_DEBUG
		virtual void CheckNoDynamics();
	#endif
	virtual	double ComputeBSPError() = 0;
	virtual double ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d) = 0;
};

class	CViewOrder : public CViewOrdered {
protected:
	//bool	m_bSingle;
	CViewOrdered	*m_apOrdered[2];

	virtual void DrainDynamicFlag() {
		m_apOrdered[0]->m_bDynamics &= m_bDynamics;
		/*if(!m_bSingle)*/	m_apOrdered[1]->m_bDynamics &= m_bDynamics; }

	// enum	{ MAX_ORDERED = 100 };
public:
	// Change structure packing to 1 before processing file header
	#if	defined(__BORLANDC__)
		#pragma	option	-a-
	#elif	defined(__WATCOMC__)
		#pragma pack(1)
	#elif defined(__sgi)
	#else
		#error	Unknown compiler
	#endif
	struct SNormal {
		CFVector3	pt,n;
		double		d, npt;
		uint		ntype;
	};
	// Revert structure packing
	#if	defined(__BORLANDC__)
		#pragma	option	-a.
	#elif	defined(__WATCOMC__)
		#pragma pack()
	#endif
protected:

	SNormal m_normal;
	static	CFVector3		m_currOrdViewPoint;
	static	bool			m_bIsGlobalOrder;
	static	TCSFVector3		*m_pLights;

	void DrawDrainLinear(CViewLinearDynamic *pd,bool b0,bool b1);
	void DrawDrainSpheric(CViewSphericDynamic *pd,
		double fOrdDiff,int n0,int n1,bool bn0,bool bn1,bool b0,bool b1,
											CViewDynamicList &beforeList);
	void DrawDrainDynamics(double fOrdDiff,
						int n0,int n1,bool bn0,bool bn1,bool b0,bool b1);
	void PromoteLinear(CViewLinearDynamic *pd);
	void PromoteSpheric(CViewSphericDynamic *pd,CViewDynamicList &list,
						CViewDynamicList &list0,CViewDynamicList &list1);
	void Promote(CViewDynamicList &list,
						CViewDynamicList &list0,CViewDynamicList &list1);
	friend class CViewBody;
	friend class CViewDynamicList;
	friend class CViewOrdered;
	friend class CViewRenderState;

	friend	void	__AdjustPlane(CViewOrdered *p);

public:
	//CViewOrder(CTaggedFile &f);
	CViewOrder()  { /*m_bSingle = FALSE;*/ }
		//{ m_ppOrdered = new CViewOrdered*[(m_nOrdered = nOrdered)*2];
		//	m_pNormals = new SNormal[nOrdered*(nOrdered-1)/2]; }
	void SetOrdered(int n,CViewOrdered *pOrdered) {
							ASSERT((uint)n<2); m_apOrdered[n] = pOrdered; }
	void SetPlane(TCCFVector3 &pt,TCCFVector3 &n,double d);
	virtual ~CViewOrder();
	virtual void Draw();
	CViewOrdered &Ordered(int i) {
							ASSERT(i<2 /*-m_bSingle*/); return *m_apOrdered[i]; }
	virtual	bool Bump(SBumpDef0 &def);
	virtual void PromoteDynamic();
	virtual void RemoveDynamics();
	void DrainDynamic(int nOrdered);
	void Bang(CViewDynamicList &shotList);
	virtual VORDTYPE Type() const { return VOT_ORDER; }
	virtual void operator%=(TCCFMatrix3x4 &dir);
	virtual void ApplyModelUsage();
	virtual void ComputeMaxHeight();
	//virtual void Split();
	static	void SetOrderViewPoint(TCCFVector3 &v) { m_currOrdViewPoint = v; }
	static	TCCFVector3 &GetOrderViewPoint() { return m_currOrdViewPoint; }
	static	void SetGlobalOrder(bool bIsGlobal) {
											m_bIsGlobalOrder = bIsGlobal; }
	static  void SetLights(TCSFVector3 *pLights) { m_pLights = pLights; }
	static	TCSFVector3	*GetLights() { return m_pLights; }
	#ifdef	_DEBUG
		virtual void CheckNoDynamics();
	#endif
	virtual	double ComputeBSPError();
	virtual double ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d);
};

class	CViewEmptyOrder : public CViewOrdered	{
protected:
public:
	CViewEmptyOrder() {}
	virtual void Draw() { m_dynamicList.ZSortDrawNDrop(); }
	virtual	bool Bump(SBumpDef0 &def) { (void)def; return FALSE; }
	virtual VORDTYPE Type() const { return VOT_EMPTY; }
	virtual void operator%=(TCCFMatrix3x4 &dir) { (void)dir; }
	virtual void ApplyModelUsage() {}
	virtual void ComputeMaxHeight() { m_fMaxHeight = 0; }
	virtual	double ComputeBSPError() { return 1e10; }
	virtual double ComputeBSPError(TCCFVector3 &,TCCFVector3 &,double) { return 1e10; }
	//virtual void Split() {}
};

class	CViewMorphedOrder : public CViewOrder {
protected:
	TCCFVector3	*m_pVertex[3];
public:
	void ReadIndices(CTaggedFile &f);
	VORDTYPE Type() const { return VOT_MORPHEDORDER; }
	void RecalcNormal();
};
// -------------------------------------------------------------------------

class	CViewFigure;

class	CViewBody : public CViewOrdered	{	// 3D convex body
protected:
	int		m_nFigures, m_nViewFigures, m_nFirstBumpFigure, m_nFirstCutFigure;
	CFVector3	m_center;
	CViewFigure	**m_ppFigures;
	bool m_bHide;
	static	CFVector3	m_minBound, m_maxBound;
	virtual VORDTYPE Type() const { return VOT_BODY; }
	friend class CViewFigure;
	bool AdvancedBump(SBumpDef0 &def);
	bool PreDrawLinear(CViewLinearDynamic *pd,TCCFVector3 &objViewPoint);
	bool PreDrawSpheric(CViewSphericDynamic *pd,TCCFVector3 &objViewPoint);
	bool PreDrawDynamic(CViewDynamic *pd,TCCFVector3 &objViewPoint);
public:
	CViewBody() { m_nFigures = 0; m_ppFigures = NULL; m_bHide = FALSE; m_bEmbedded = FALSE; }
	void Read(CTaggedFile &f);
	virtual	void Draw();
	virtual	bool Bump(SBumpDef0 &def);
	bool BumpShelter(SBumpDef0 &def,bool &bBump);
	virtual void PromoteDynamic() {ASSERT(0);}
	virtual void RemoveDynamics() {ASSERT(0);}
	~CViewBody();
	void Hide() { m_bHide = TRUE; }
	virtual void operator%=(TCCFMatrix3x4 &dir);
	void Hue(double fModify);
	virtual	double ComputeBSPError() { return 1e10; }
	virtual double ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d);
};

// -------------------------------------------------------------------------
class	CViewObject {
protected:
	// ------------------- Now come static ones ----------------------
	struct	SFinVertImage {	// Essential that v comes first!!!
		CVector3	v;
		int			cf;
		//int			nIdx;
		SFVector3Y	v3;
	};
	struct	SInfVertImage {
		CVector3	v;
		int			cf;
		//int			nIdx;
		int			nRatio;
		SFVector3Y	v3;
	};
	enum { CLIP_SHIFT = 16, CLIP_PREC = 1 << CLIP_SHIFT };
	enum { ZCF_FRONT = 1, ZCF_BACK = 2 };	// Z clipping flags
	typedef	const SFinVertImage TCSFinVertImage;
	typedef	const SInfVertImage TCSInfVertImage;

	static	CFMatrix3x4	*m_pCurrentInvMx;
	static	CFMatrix3x4	m_currentInvMx[2];
public:
	enum { MAX_VERTICES = 1000, MAX_EDGEVERTICES = 3000 };
	static	CFMatrix3x4	m_viewPointDirMx, m_viewPointInvMx,
													m_viewPointDirSMx;
	static	CFVector3	m_viewPointScale, m_viewPointScale2,
						m_viewPointScale_1; //, m_viewPointScaleInt;
	static	CFRect2			m_clipRect, m_clipRect2,
									m_clipRect0, m_clipRect0N;
	static	CFVector3		m_clipRays[4];
	static	double			m_fFrontClip, m_fBackClip;	// clipping planes z coord
	static	double			m_f1FrontClip, m_f1BackClip; // 1/front, 1/back
	//static	int				m_nFrontClip, m_nBackClip;
	static	WATER_VIEW		m_eWaterView;
	static	int				m_z1MinShift;
	static	double			m_fz1MinScale;
protected:
	//static	CVector3		m_aVertImages3[];	// MAX_VERTICES*2
	static	SFinVertImage	m_aVertImages[]; // MAX_VERTICES*2
	static	SInfVertImage 	m_aInfVertImages[]; // MAX_EDGEVERTICES*2
	static	SFinVertImage	*m_paVertImages;
	static	SInfVertImage	*m_paInfVertImages;
	static	CFVector3		m_currObjViewPoint;
	static	SFVector3		m_aModelLights[];	// LIGHT_SOURCE_COUNT*2
	static	SFVector3		m_aGlobalLights[];	// LIGHT_SOURCE_COUNT
	static	SRect2			m_aGlobalLightsRect[];
	static	double			m_terrainScale_1;
	static	dword			m_dwLights;
									// view point in current object's coords
	static	bool			m_bViewOrtho, m_bHaze, m_bLastDrawn;
	static	bool			m_bIsBelowWater;

	// ---------------------------------------------------------

	friend  class CViewFigure;
	friend	class CViewBody;
	friend  class CViewTexturedFigure;
	friend  class CViewFlatGouraudFigure;
	friend	class CViewObjectBase;
	friend	class CViewBush;
	friend	class CShot;
	friend	class CShooter;
	friend	class CMovingObject;
	friend	class CVessel;
	friend	class CFoe;
	friend	class CViewCity;
	//friend	class CViewDynamic;
	//friend	class CViewDynamicList;
	friend	class CPhasedMovie;
	friend	class CViewObjectRef;
	friend	class CViewOrder;
	friend	class CLandDynamicMap;
	friend	class _CViewTerrain;
	friend	class CViewRenderState;
public:
	static	void SetViewPoint(TCCFMatrix3x4 &viewPointDirMx,
													TCCFVector3 &scale );
	static	void SetOrderViewPointToGlobal();
	static	void SetLightsToGlobal() { CViewOrder::SetLights(m_aGlobalLights); }
	static	void BeginDraw();
	static	void SetClipRect(TCCRect2 &rect);
	static	void SetClipPlanes(double fFront,double fBack);
	static	void SetViewOrtho(bool bViewOrtho) { m_bViewOrtho = bViewOrtho; }
	static	bool ViewOrtho() { return m_bViewOrtho; }
	static	void SetHaze(bool b) { m_bHaze = b; }
	static	bool HazeOn() { return m_bHaze; }
	static	void EnableLights(dword dwLights) { m_dwLights = dwLights; }
	static	dword EnabledLights() { return m_dwLights; }
	static	void SetTerrainCellSize(double fCellSize) { m_terrainScale_1 = 1./fCellSize; }
	static	void SetLight(int i,int color,TCSFVector3 &pos,double radius,int brightness);
	static	bool IsVisible(TCSFVector3 &centerW,double fRadius);
	static	void SetBelowWater(bool bBelow) { m_bIsBelowWater = bBelow; }
	static	bool IsBelowWater() { return m_bIsBelowWater; }
	static	bool Create3DPoly(int nVertices,TCSFVector3 *pVertices3D,TCdword *pGouraud=NULL,TCSFVector2 *pTexture=NULL);
};
// --------------------------------------------------------------------------
struct SViewObjectMorphNames {
	CNameDecls	vertices, figures, clusters;
	void Read(CTaggedFile &f);
	void FinishResolve();
};

#include"keyframe.h"
// --------------------------------------------------------------------------
//class	b_TBush;
class	CViewBush;
class	CViewObjectBase	{
protected:
	int			m_nVertices, m_nSplitX, m_nSplitY, m_nSplitZ;
	int			m_nSplitVertices, m_nRenderedVertices;
	double		*m_pSplitX, *m_pSplitY, *m_pSplitZ;
	struct	SSplitRef	{ SFVector3Y *px, *py, *pz; };
	SSplitRef	*m_pSplits;
	CFVector3	*m_pVertices;	// All vertices in object's coordinates
	CFVector3	m_center;
	double		m_fHeight, m_fRadius, m_fCenterDist;
	int			m_nBodies, m_nBushes;
	CViewBody	*m_pBodies;
	CViewBush	*m_pBushes;
	CViewOrdered	*m_pOrder;

	//CVector3	*m_pVertImages3;
	CFMatrix3x4	*m_pCurrentInvMx;
	CViewObject::SFinVertImage  *m_pVertImages;
	CViewObject::SInfVertImage	*m_pInfVertImages;

	struct	SVertexEdges {
		int  n;		// Number of edges connected to the vertex
		CViewObject::SInfVertImage	*im;	// Points to first of the edges
		TCSFVector3Y	**oth;	// Points to array of pointers to connected
							// vertices
	} *m_pVertEdges;	// For each vertex points to connected sequence
						// in m_pVertEdgeIndices
	TCSFVector3Y	**m_pEdgeOtherVertices;

	SViewObjectMorphNames	m_names;
	CViewKeyFrameSet		m_keyFrameSet;

	SBumpFigures			*m_pBumpF;
	int						m_nBumpF;

	int						m_nReductionNum;

	static	CViewObjectBase	*m_pCurrBase;
	static	int				m_z1Shift;
	static	double			m_fz1Scale;
	static	CViewOrdered	* ReadOrder(CTaggedFile &f);
	union USplitVect {
		SFVector3Y	v;
		double		f;
	};
	static	USplitVect	m_splitX[], m_splitY[], m_splitZ[];
	void MultSplits(TCCFMatrix3x4 &dir);
	void AddSplits(int nOffsetX,int nOffsetY,int nOffsetZ);
	void MultSplitsY(TCCFMatrix3x4 &dir);
	void AddSplitsY(int nOffsetX,int nOffsetY,int nOffsetZ,int nOffsetY0);
	int  PreDraw(bool bWaterSplit);

	void	CalcClusterRadius();
	friend	class CViewObjectBaseSet;
	friend	class CViewFigure;
	friend	class CViewBody;
	friend	class CViewRenderState;

public:
	bool	IsDynamic() const { return m_pVertImages != CViewObject::m_aVertImages; }
	void	Read(CTaggedFile &f,bool bDynamic=FALSE);
	CViewObjectBase();
	CViewObjectBase(CTaggedFile &f,bool bDynamic=FALSE) { Read(f,bDynamic); }
	~CViewObjectBase();

	void operator%=(TCCFMatrix3x4 &dir);
	double ComputeBSPError();
	double ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d);
	void Split(bool bSplit);

	int		NumVertices() const { return m_nVertices; }
	TCCFVector3 *GetVertexPtr(int i) {
		#if _USE_OPENGL
			ASSERT((uint)i<m_nVertices);
		#else
			ASSERT((uint)i<m_nVertices); //&&m_nSplitVertices<m_nVertices);
		#endif
		return m_pVertices+i;
	}
	CFVector3 GetVertex(int i) { ASSERT((uint)i<m_nVertices);
		if( m_pVertices ) return m_pVertices[i];
		SSplitRef	*pSplit = m_pSplits+i; return CFVector3(
								m_pSplitX[(USplitVect*)pSplit->px-m_splitX],
								m_pSplitY[(USplitVect*)pSplit->py-m_splitY],
								m_pSplitZ[(USplitVect*)pSplit->pz-m_splitZ]); }
	static	CViewObjectBase *GetCurrBase() { ASSERT(m_pCurrBase); return m_pCurrBase; }
	CViewBody &GetBody(int i) {	ASSERT(this); ASSERT((uint)i<m_nBodies);
									return m_pBodies[i]; }
	bool IsVisible(TCCFMatrix3x4 &dir);
	void CalcCenterDist(TCCFMatrix3x4 &dir /*view scaled matrix*/) {
		m_fCenterDist = dir.m[2][0]*m_center.x+dir.m[2][1]*m_center.y+
										dir.m[2][2]*m_center.z+dir.m[2][3]; }
	double CenterDist() const { return m_fCenterDist; }
	double Radius() const { return m_fRadius; }
	double Height() const { return m_fHeight; }
	void SetCenterDist(double f) { m_fCenterDist = f; }
	void SetRadius(double f) { m_fRadius = f; }
	void Draw(CViewDynamicList &dynamicList,dword dwLights);
	bool Bump(SBumpDef0 &def);
	bool BumpShelter(SBumpDef0 &def,bool &bBump) { return m_pBodies[0].BumpShelter(def,bBump); }
	void Hue(double fModify);
	void SetOrderToBody(int n);
	SViewObjectMorphNames &Names() { return m_names; }
	CViewKeyFrameSet &KFSet() { return m_keyFrameSet; }
	void Animate();
	int  NBushes() const { return m_nBushes; }

	int  ReductionNum() const { return m_nReductionNum; }

	void			AllocBumpFigures(int nSets);
	SBumpFigures	&BumpFigures(int i) { return m_pBumpF[i]; }
	int				BumpFiguresCount() { return m_nBumpF; }
};

class	CViewObjectBaseSet : public CViewOrdered {
protected:
	int		m_nBases;
	CViewObjectBase	*m_pBases, *m_pCurrBase;
	double	*m_pfThresholds;
	char	(*m_pNames)[40];
	bool	m_bHasBushes;

	friend class CViewObject;
	friend class CViewOrder;
	friend class CViewBaseCluster;
	friend class CViewObjectBase;

	static	TCCFMatrix3x4Y *m_pCurrDir;
	static	double	m_fDistanceModify, m_fCutAngleCotangent;

	static	CViewObjectBaseSet	*m_pCurrBaseSet;

	void	CalcClusterRadius();

public:
	bool IsDynamic() const { ASSERT(m_nBases>0); return m_pBases[0].IsDynamic(); }
	void Read(CTaggedFile &f,bool bDynamic=FALSE);
	CViewObjectBaseSet() { m_pBases = NULL; m_pfThresholds = NULL; m_nBases = 0; m_bEmbedded = FALSE; m_pNames = NULL; }
	CViewObjectBaseSet(CTaggedFile &f,bool bDynamic=FALSE) { m_bEmbedded = FALSE; Read(f,bDynamic); }
	~CViewObjectBaseSet() { delete [] m_pBases; delete [] m_pfThresholds; delete [] m_pNames; }
	VORDTYPE Type() const { return VOT_BASESET; }
	void operator%=(TCCFMatrix3x4 &dir);
	double ComputeBSPError();
	double ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d);
	void Split(bool bSplit);
	static void SetDistanceModify(double f) { m_fDistanceModify = f; }
	static void SetCutAngle(double a) { m_fCutAngleCotangent = 1/tan(a); }
	void SetRadius(double f) { for(int i = 0 ; i < m_nBases ; i++ ) m_pBases[i].SetRadius(f); }
	bool IsVisible(TCCFMatrix3x4 &dir);
	virtual void PromoteDynamic() { ASSERT(0); }
	virtual void RemoveDynamics() { ASSERT(0); }
	void Draw(); // { m_pCurrBase->Draw(&m_dynamicList); }
	bool Bump(SBumpDef0 &def) { ASSERT(m_pBases); m_pCurrBaseSet = this; return m_pBases[0].Bump(def); }
	bool BumpShelter(SBumpDef0 &def,bool &bBump) { return m_pBases[0].BumpShelter(def,bBump); }
	void Hue(double fModify);
	int NumBases() const { return m_nBases; }
	CViewObjectBase &Base(int n) { ASSERT(n>=0&&n<m_nBases); return m_pBases[n]; }
	bool HasBushes() const { return m_bHasBushes; }
	static TCCFMatrix3x4Y *CurrDir() { return m_pCurrDir; }
};

class	CViewBaseCluster {
protected:
	int		m_nBaseSets;
	CViewObjectBaseSet	*m_pBaseSets;
	CViewOrdered		*m_pOrder;
	CFVector3	m_center;
	double		m_fRadius;
	double		m_fRadius0;
	double		m_fHeight;
	char		(*m_pNames)[40];

	CFMatrix3x4	*m_pMatrix0;	// Comparison matrix for identity check
	bool		m_bIdentity;	// Base is used with same orientation every
								// time
	bool		m_bHasBushes;


	IF_DEBUG(bool m_bAnimate);

	static	CViewBaseCluster	*m_pCurrCluster;
	static	CViewOrdered	*ReadOrder(CTaggedFile &f);

	static	CFVector3	m_overallCenter, m_overallMin, m_overallMax;
	static	double		m_fOverallRadius2;
	static	double		m_fOverallRadius02;
	static	bool		m_bReadNamesAlways;

	static	void SetReadNamesAlways(bool b) { m_bReadNamesAlways = b; }
	static	bool GetReadNamesAlways() { return m_bReadNamesAlways; }

	friend class CViewObjectRef;
	friend class CViewOrder;
	friend class CViewObjectBase;
	friend class CViewObjectBaseSet;
public:
	bool IsDynamic() const { ASSERT(m_nBaseSets>0); return m_pBaseSets[0].IsDynamic(); }
	void Read(CTaggedFile &f,bool bDynamic=FALSE);
	CViewBaseCluster() { m_pBaseSets = NULL; m_pOrder = NULL; m_pMatrix0 = NULL; m_bIdentity = FALSE;
			 m_nBaseSets = 0; m_pNames = NULL; IF_DEBUG(m_bAnimate = FALSE); }
	CViewBaseCluster(CTaggedFile &f,bool bDynamic=FALSE) { Read(f,bDynamic); }
	~CViewBaseCluster() { m_pOrder->Delete(); delete [] m_pBaseSets; delete [] m_pNames; }
	void operator%=(TCCFMatrix3x4 &dir);
	double ComputeBSPError();
	double ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d);
	void Split(bool bSplit);
	bool IsIdentity() const { return m_bIdentity; }
	double Radius() const { return m_fRadius; }
	double Radius0() const { return m_fRadius0; }
	double Height() const { return m_fHeight; }
	TCCFVector3 &Center() const { return m_center; }
	void SetRadius(double f,bool bAll=FALSE) {
		m_fRadius = f; if(bAll) for( int i = 0 ; i < m_nBaseSets ; i++ ) m_pBaseSets[i].SetRadius(f); }
	void Draw(TCCFMatrix3x4Y &dir,CViewDynamicList *pDynamicList,dword dwLights);
	bool Bump(SBumpDef0 &def);
	CViewObjectBaseSet &BaseSet(int n) const { ASSERT(n>=0&&n<m_nBaseSets); return m_pBaseSets[n]; }
	int NumBaseSets() const { return m_nBaseSets; }
	void Hue(double fModify);
	bool HasBushes() const { return m_bHasBushes; }
};

class	CViewObjectModel : public CViewBaseCluster {
public:
	CViewObjectModel() {}
	CViewObjectModel(CTaggedFile &f,bool bDynamic=FALSE) : CViewBaseCluster(f,bDynamic) {}
};

class	CViewObjectRef : public CViewOrdered
								// Reference to CViewObjectBase
{
protected:
	CViewObjectModel	*m_pBase;
	CFMatrix3x4	m_dir;		// Direct transformation orthogonal matrix
	CFVector3	m_inv3;		// Inverse transformation matrix' 3rd column
	bool		m_bCenterUpdated, m_bInvUpdated;

	CFVector3	m_center;
	CFMatrix3x4	*m_pCurrentInvMx;
	SFVector3	*m_pLights;

	SBumpFiguresRef	*m_pBFDefs;
	int				m_nBFDefs;

	friend	class CViewObjectBase;
	friend	class CViewBody;
	friend	class CViewBaseCluster;
	friend	class CViewFigure;
	friend	class CViewRenderState;

	void	(*m_AnimationCallback)(CViewObjectBaseSet *pBaseSet,CViewObjectBase *pBase,CViewObjectRef *pRef);
	void	*m_pUserAttrib;
	static	CViewObjectRef	*m_pCurrRef;
	static	CFMatrix3x4		*m_pCurrRefDir;

	bool	IsDynamic() const { return m_pCurrentInvMx != CViewObject::m_currentInvMx; }
	void	DoUpdateCenter();

	bool	BumpFigures(SBumpDef0 &def) {
		int res = 0;
		for( int i = 0 ; i < m_nBFDefs ; i++ )
			if( m_pBFDefs[i].bActive ) res |= m_pBFDefs[i].Bump(def);
		return MAKEBOOL(res); }

public:
	CViewObjectRef() { m_pBFDefs = NULL; m_nBFDefs = 0; }
	CViewObjectRef(CViewObjectModel *pBase) { m_pBFDefs = NULL; m_nBFDefs = 0; Attach(pBase); }
	virtual ~CViewObjectRef() { delete [] m_pBFDefs; }
	CViewObjectModel *Model() const { return m_pBase; }
	void	Attach(CViewObjectModel *pBase);
	virtual void Draw();
	virtual	bool Bump(SBumpDef0 &def);
	//virtual void PromoteDynamic();
	void	UpdateCenter() { if(!m_bCenterUpdated) DoUpdateCenter(); }
	TCCFVector3	&Center() { UpdateCenter(); return m_center; }
	CFMatrix3x4	&GetDirModify() {			// Must be orthogonal!!!
			m_pBase->m_bIdentity = FALSE;
			m_bCenterUpdated = m_bInvUpdated = FALSE; return m_dir; }
	TCCFMatrix3x4	&GetDir() const { return m_dir; }
	void		UpdateInv();
	virtual VORDTYPE Type() const { return VOT_OBJECT; }
	void		ClaimModelUsage();
	double ComputeBSPError();
	double ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d);
	virtual void ApplyModelUsage();
	virtual void ComputeMaxHeight();
	void	SetAnimationCallback(void (*pCallback)(CViewObjectBaseSet *pBaseSet,CViewObjectBase *pBase,CViewObjectRef *pRef)) {
				m_AnimationCallback = pCallback; IF_DEBUG(m_pBase->m_bAnimate = TRUE); }
	void	SetUserAttrib(void *pAttrib) { m_pUserAttrib = pAttrib; }
	void	*GetUserAttrib() { return m_pUserAttrib; }

	void	AllocBumpFigures();	// count is taken from base(0)
	void	EnableBumpFigures(int i,TCSFVector3 &vel,double fTime) {
				ASSERT((uint)i<m_nBFDefs);
				m_pBFDefs[i].vel = vel; m_pBFDefs[i].fTime = fTime;
				m_pBFDefs[i].bActive = TRUE; }
	void	DisableBumpFigures() {
				for( int i = 0 ; i < m_nBFDefs ; i++ ) m_pBFDefs[i].bActive = FALSE; }
};

class	CShelterOrder : public CViewObjectRef {
protected:
	CViewOrdered	*m_pInnerOrder;
	bool			m_bInhibitDraw;
public:
	CShelterOrder(CViewObjectModel *pBase);
	void SetInner(CViewOrdered *pOrd) { m_pInnerOrder = pOrd; }
	void Draw();
	void DrawViewStartHere();
	virtual void PromoteDynamic();
	bool Bump(SBumpDef0 &def);
};

inline void CViewObjectBase::Animate()
{
	ASSERT(CViewObjectRef::m_pCurrRef);
	if( CViewObjectRef::m_pCurrRef->m_AnimationCallback )
		CViewObjectRef::m_pCurrRef->m_AnimationCallback(
			CViewObjectBaseSet::m_pCurrBaseSet,this,
							CViewObjectRef::m_pCurrRef );
}
inline bool CViewObjectBase::Bump(SBumpDef0 &def) {
	ASSERT(m_pOrder); Animate(); return m_pOrder->Bump(def);
}

inline bool CViewBaseCluster::Bump(SBumpDef0 &def) {
	ASSERT(!m_bAnimate||CViewObjectRef::m_pCurrRef->m_AnimationCallback);
	ASSERT(m_pOrder); return m_pOrder->Bump(def);
}

// -------------------------------------------------------------------------

class	CViewFigure {	// Planar convex figure in 3D space
public:
	enum	{ MAX_VERTICES = 50 };
	static	WATER_SPLIT	m_eWaterSplit;
	//static	int		m_nWaterline;
	static	double	m_fWaterline;
protected:
	enum TYPE { T_DUMMY = -1, T_FLAT = 0, T_OWNTEXTURED = 1, T_FLATGOURAUD = 2,
		T_FLATTRANSPARENT = 3, T_LIBTEXTURED = 4, T_FLATTRANSPARENTFLASH = 5 };
	int		m_nVertices;
	#if !_USE_OPENGL
		TCCFVector3	**m_ppVertices;
		CViewObject::TCSFinVertImage	**m_ppVertImages;
		CViewObject::TCSInfVertImage	**m_ppInfVertImages; // Infinite images
										// Prec,Succ,Prec,Succ ...
										// - twice as many
										// (use if m_pVertImages[i]->cf.f)
	#endif
	#if _USE_OPENGL
		TCCFVector3	**m_ppVertices;
	#endif
	CFVector3	m_vertex0;	// Vertex 0 in object coords
	CFVector3	m_normal;
	double		m_fNormalPt;
	uint		m_nNormalType;
	struct	SMorphData {
		int			m_nNamed0Idx;
		TCCFVector3	*m_pVertex[3];
	} *m_pMorphData;
	struct	STextVert {
		#if _USE_OPENGL
			double u,v;
		#else
		 	int u,v;
		#endif
	};

	#if !_USE_OPENGL
		static	dword	m_dwHazeFlag;
	#endif
	static	double	m_fHazeMult, m_fHazeAdd, m_fHazeMin, m_fHazeMax;

	virtual	void _Read(CTaggedFile &f) { (void)f; }
	virtual TYPE Type() const { return T_DUMMY; }
	bool IsVisibleFront();
	#if !_USE_OPENGL
		void InterpolateClipped(double fy,double dy,TCSFVector3Y &vc,
		 			int ic,TCSFVector3Y &vnc,int inc,int &nCI,
								bool bGouraud,dword *pGourauds,
								bool bTextured,STextVert *pTextVerts);
		int	 FormVertImagesSplit(int nSplit,bool bGouraud,
				dword *pGourauds,bool bTextured,STextVert *pTextVerts);
		int	 FormVertImages(bool bGouraud,
				dword *pGourauds,bool bTextured,STextVert *pTextVerts);
	#endif
	int  NamedIdx(int n) { ASSERT(m_pMorphData&&(uint)n<m_nVertices); return (n+m_pMorphData->m_nNamed0Idx)%m_nVertices; }
	//bool m_bRecalc;
public:
	CViewFigure() {
		#if _USE_OPENGL
			m_ppVertices = NULL;
		#else
			m_ppVertices = NULL;
			m_ppVertImages = NULL; m_ppInfVertImages = NULL;
		#endif
		m_pMorphData = NULL;
		//m_bRecalc = FALSE;
	}
	virtual ~CViewFigure() {
		#if _USE_OPENGL
			delete [] m_ppVertices;
		#else
			delete [] (m_ppVertices-1);
			delete [] (m_ppVertImages-1); delete [] m_ppInfVertImages;
		#endif
		delete m_pMorphData;
	}
	virtual void Draw() { ASSERT(0); }
	TCCFVector3 &Normal() const { return m_normal; }
	TCCFVector3 &Vertex0() const { return m_vertex0; }
	virtual void operator%=(TCCFMatrix3x4 &dir);
	virtual void Hue(double fModify) { (void)fModify; }
	double ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d);
	void RecalcNormal();
	void SetNormal(TCCFVector3 &pt,TCCFVector3 &n) { m_fNormalPt = (m_vertex0=pt)*(m_normal=n); m_nNormalType=1000; }
	static CViewFigure *Read(CTaggedFile &f,bool bDummy);
	static void SetHaze(bool b);
	static bool GetHaze();
	static void SetHaze(SHazeDef &haze);
	static double HazeMin() { return m_fHazeMin; }
	static double HazeMax() { return m_fHazeMax; }
//	static double TranslateHaze(double z) { return m_fHazeMult*z+m_fHazeAdd; }
//	static double TranslateHaze(double z) { return z; }
	static	void SetWaterline(double y);
	static	double Waterline() { return m_fWaterline; }
	bool AdvancedBump(SBumpDef0 &def);
};
#if !_USE_OPENGL
	inline void CViewFigure::SetHaze(bool b) { m_dwHazeFlag = b ? GR_POLY_ADD_HAZE : 0; }
	inline bool CViewFigure::GetHaze() { return m_dwHazeFlag != 0; }
#endif

class	CViewFlatFigure	: public CViewFigure {
protected:
	dword  m_nColor;
	virtual void _Read(CTaggedFile &f);
	virtual void Draw();
	virtual void Hue(double fModify);
	virtual TYPE Type() const { return T_FLAT; }
public:
	void SetColor(dword dwColor) { m_nColor = dwColor; }
};

class	CViewFlatGouraudFigure : public CViewFigure {
protected:
	dword  m_nColor;
	#if !_USE_OPENGL
		dword  *m_pGourauds;
	#endif
	#if _USE_OPENGL
		double	*m_pglGourauds;
	#endif
	virtual void _Read(CTaggedFile &f);
	virtual void Draw();
	virtual void Hue(double fModify);
	virtual TYPE Type() const { return T_FLATGOURAUD; }
public:
	CViewFlatGouraudFigure() {
		#if _USE_OPENGL
			m_pglGourauds = NULL;
		#else
			m_pGourauds = NULL; m_pGourauds++;
		#endif
	}
	~CViewFlatGouraudFigure() {
		#if _USE_OPENGL
			delete [] m_pglGourauds;
		#else
			delete [] (m_pGourauds-1);
		#endif
	}
	void SetNamedGouraud(int n,double f) { f = MinMax(f,-1.,1.); int idx = NamedIdx(n);
		#if _USE_OPENGL
			m_pglGourauds[idx] = (f+1)/255.;
		#else
			m_pGourauds[idx] = (dword) Round( 8+7*f ) << 16;
			if( idx == 0 )	m_pGourauds[m_nVertices] = m_pGourauds[idx]; else
			if( idx == m_nVertices-1 )	m_pGourauds[-1] = m_pGourauds[idx];
		#endif
	}
	double GetNamedGouraud(int n) {
		#if _USE_OPENGL
			return m_pglGourauds[NamedIdx(n)]*255.-1;
		#else
			return	(m_pGourauds[NamedIdx(n)] - 8*65536)/(65536*7.);
		#endif
	}
};

class	CViewFlatTransparentFigure : public CViewFigure {
protected:
	dword  m_nColor;
	dword  m_nOpacity;
	virtual void _Read(CTaggedFile &f);
	virtual void Draw();
	virtual TYPE Type() const { return T_FLATTRANSPARENT; }
public:
};

class	CViewFlatTransparentFlashFigure : public CViewFigure {
protected:
	static	dword  m_nColor;
	static	dword  m_nOpacity;
	virtual void Draw();
	virtual TYPE Type() const { return T_FLATTRANSPARENTFLASH; }
public:
	static void SetColor(byte nColor) { m_nColor = nColor; }
	static void SetOpacity(double fOpacity) {
					m_nOpacity = MinMax(0,255,Round(fOpacity*255.)); }
};

class	CViewTexture {
public:
	enum { TXR_LINEAR = 1, TXR_HASMATRIX = 2, TXR_INDEX16 = 4, TXR_SPRITE = 8,
				TXR_ALPHA = 16 };
												// texture flags
protected:
	dword	m_dwFlags;
	int		m_nWidth, m_nHeight;
	#if 1 //_USE_GLIDE
		struct SImageInfo {
			dword flag;
			dword height;
			dword width;
		};
		enum { SIZE_IMAGEINFO = sizeof(SImageInfo) };
	#else
		enum { SIZE_IMAGEINFO = 0 };
	#endif
	#if _USE_OPENGL
		#ifdef __sgi
			uint	m_nBindIndex;
		#else
			uint	m_nCallList;
		#endif
	#endif
	//byte	*m_pImage;
	//int		*m_pImageCache;
    GR_HTEXTURE  m_hImage;
	byte	m_nTransparentColor;
	static	dword m_dwDefaultInfoFlags;
	void	ReadImage(CTaggedFile &f,CPaletteTranslator &palTrans,void (*pCallBack)(byte *pImage,SRGB *pPal),GR_HTEXTURE hOld=NULL);
	void Read(CTaggedFile &f,bool bTagged=TRUE,void (*pCallBack)(byte *pImage,SRGB *pPal)=NULL,GR_HTEXTURE hOld=NULL);
	static GR_HTEXTURE Reload(TCchar *szName,GR_HTEXTURE hOld,void *pUser);
	friend	class CViewTexturedFigure;
public:
	CViewTexture() { m_hImage = NULL; /*m_pImageCache = NULL;*/ }
	~CViewTexture();
	int Width() const { return m_nWidth; }
	int Height() const { return m_nHeight; }
	GR_HTEXTURE	HImage() const { return m_hImage; }
	static dword SetDefaultInfoFlags(dword dwFlags) { dword dwFlagsOld = m_dwDefaultInfoFlags;
						m_dwDefaultInfoFlags = dwFlags; return dwFlagsOld; }
	static dword GetDefaultInfoFlags() { return m_dwDefaultInfoFlags; }
	void Read(TCchar *szName,bool bTagged=TRUE,void (*pCallBack)(byte *pImage,SRGB *pPal)=NULL,GR_HTEXTURE hOld=NULL);
};

class	CViewTexturedFigure	: public CViewFigure {
protected:
	dword	m_dwFlags;
	#if _USE_OWNGR || _USE_GLIDE
		dword m_dwDrawType;
	#endif
	STextVert *m_pTextVerts;
	CViewTexture	*m_pTexture;
	virtual	void _ReadTexture(CTaggedFile &f) = 0;
	virtual void _Read(CTaggedFile &f);
	virtual void Draw();
	static	int m_nFlatColor;
	static	double m_fFlatDeltaRange;
public:
	CViewTexturedFigure() { m_pTextVerts = NULL; }
	~CViewTexturedFigure() { if(m_pTextVerts) delete [] (m_pTextVerts-1); }
	void SetNamedTextCoord(int n,TCCFVector2 &v) { int idx = NamedIdx(n);
		#if _USE_OPENGL
			m_pTextVerts[idx].u = MinMax(v.x,1.,double(m_pTexture->m_nWidth-1))/m_pTexture->m_nWidth;
			m_pTextVerts[idx].v = MinMax(v.y,1.,double(m_pTexture->m_nHeight-1))/m_pTexture->m_nHeight;
		#else
			m_pTextVerts[idx].u = Round( MinMax(v.x,1.,double(m_pTexture->m_nWidth-1))*65536.);
			m_pTextVerts[idx].v = Round( MinMax(v.y,1.,double(m_pTexture->m_nHeight-1))*65536.);
		#endif
			if( idx == 0 )	m_pTextVerts[m_nVertices] = m_pTextVerts[idx]; else
			if( idx == m_nVertices-1 )	m_pTextVerts[-1] = m_pTextVerts[idx]; }
	CFVector2 GetNamedTextCoord(int n) { int idx = NamedIdx(n);
		#if _USE_OPENGL
			return CFVector2( m_pTextVerts[idx].u*m_pTexture->m_nWidth, m_pTextVerts[idx].v*m_pTexture->m_nHeight );
		#else
			return CFVector2( m_pTextVerts[idx].u/65536., m_pTextVerts[idx].v/65536.);
		#endif
	}
	static void SetFlatColor(int nColor=-1,double fDeltaRange=0) { m_nFlatColor = nColor; m_fFlatDeltaRange = fDeltaRange; }
	static void XchgFlatColor(int &nColor,double &fDeltaRange) { Swap(nColor,m_nFlatColor); Swap(fDeltaRange,m_fFlatDeltaRange); }
};

class	CViewOwnTexturedFigure	: public CViewTexturedFigure {
protected:
	CViewTexture	m_texture;
	virtual void _ReadTexture(CTaggedFile &f);
	virtual TYPE Type() const { return T_OWNTEXTURED; }
	virtual CViewTexture &_Texture() { return m_texture; }
public:
	CViewOwnTexturedFigure() { m_pTexture = &m_texture; }
};

class	CViewLibTexturedFigure	: public CViewTexturedFigure {
protected:
	virtual void _ReadTexture(CTaggedFile &f);
	virtual TYPE Type() const { return T_LIBTEXTURED; }
	virtual CViewTexture &_Texture() { return *m_pTexture; }
	struct  SLibTexture {
		CViewTexture	texture;
		char	szName[9];
	};
	static	SLibTexture	*m_pLibTextures;
	static	int	m_nLibTextures, m_nMaxLibTextures;
public:
	static	void ClearLib();
	static	void LoadLib(CTaggedFile &f);
	static	void LoadLib(TCchar *szName);
};

int bush_Init();
void bush_Done();

class	CViewRenderState {
protected:
	CFVector3	viewPoint;
	CFMatrix3x4	*pInv;
	CViewObjectRef *pRef;
	CFMatrix3x4 *pRefDir;
	bool	bHaze;
	int		z1Shift;
	double	fz1Scale;
	WATER_SPLIT	eWaterSplit;
public:
	void Load() {
		ASSERT(!CViewOrder::m_bIsGlobalOrder);
		viewPoint = CViewObject::m_currObjViewPoint;
		pInv = CViewObject::m_pCurrentInvMx;
		pRef = CViewObjectRef::m_pCurrRef;
		pRefDir = CViewObjectRef::m_pCurrRefDir;
		bHaze = CViewFigure::GetHaze();
		z1Shift = CViewObjectBase::m_z1Shift;
		fz1Scale = CViewObjectBase::m_fz1Scale;
		eWaterSplit = CViewFigure::m_eWaterSplit;
		CViewObject::SetOrderViewPointToGlobal();
	}
	void Set(bool bFull=TRUE) {
		CViewObject::m_currObjViewPoint = viewPoint;
		CViewObject::m_pCurrentInvMx = pInv;
		CViewFigure::SetHaze(bHaze);
		CViewFigure::m_eWaterSplit = eWaterSplit;
		CViewObjectRef::m_pCurrRefDir = pRefDir;
		CViewObjectBase::m_z1Shift = z1Shift;
		CViewObjectBase::m_fz1Scale = fz1Scale;
		if( bFull ) {
			CViewOrder::m_currOrdViewPoint = viewPoint;
			CViewObjectRef::m_pCurrRef = pRef;
			//CViewObject::m_eWaterView = eWaterView;
			CViewOrder::SetGlobalOrder(FALSE); // because set to true
		}
		GRSetZPrecision(z1Shift);
	}
	TCSFVector3 &Viewpoint() const { return viewPoint; }
};


#include"objmap.h"
// -------------------------------------------------------------------------
#endif	//	__VIEW_H__


