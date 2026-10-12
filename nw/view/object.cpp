#ifndef	__VIEW_CPP__
#include"_view.h"
#endif

#include"brend.h"

void CNameDecl::Read(CTaggedFile &f)
{
	f.ReadInt(m_nDimension);
	RTCHECK(m_nDimension>0,"namedef dim < 0");
	char szName[40];
	f.Read(szName,40);
	int nLen = strlen(szName);
	m_pData = new byte[m_nDimension*sizeof(void*)+nLen+1];
	for( int i = 0 ; i < m_nDimension ; i++ ) Elem(i) = NULL;
	memcpy((char*)Name(),szName,nLen+1);
}

void CNameDecls::Read(CTaggedFile &f)
{
	f.Descend("NAME",TRUE,0);
		f.ReadInt(m_nDecls);
		ASSERT(m_nDecls>=0);
		if( m_nDecls >= 0 ) m_pDecls = new CNameDecl[m_nDecls];
		for( int i = 0 ; i < m_nDecls ; i++ )
			m_pDecls[i].Read(f);
	f.Ascend();
}
CNameDecl &CNameDecls::operator[](TCchar *szName)
{
	for( int i = 0 ; i < m_nDecls ; i++ )
		if( strcmp(szName,m_pDecls[i].Name()) == 0 )	return m_pDecls[i];
	RTCHECK1(0,"Name bind error: '%s'",szName);
	return *(CNameDecl*)NULL;
}
CNameDecl *CNameDecls::Lookup(TCchar *szName)
{
	for( int i = 0 ; i < m_nDecls ; i++ )
		if( strcmp(szName,m_pDecls[i].Name()) == 0 )	return &m_pDecls[i];
	return NULL;
}

// ------------------------------------------------------------------------


CFVector3	CViewBaseCluster::m_overallCenter;
double		CViewBaseCluster::m_fOverallRadius2;
double		CViewBaseCluster::m_fOverallRadius02;
CFVector3	CViewBaseCluster::m_overallMax;
CFVector3	CViewBaseCluster::m_overallMin;
bool		CViewBaseCluster::m_bReadNamesAlways = FALSE;

CViewBaseCluster	*CViewBaseCluster::m_pCurrCluster = NULL;
CViewOrdered	* CViewBaseCluster::ReadOrder(CTaggedFile &f)
{
	f.Descend("ORDR",FALSE,1);
		f.Descend("ORDH",TRUE,1);
			int  type;
			f.ReadInt(type);
			CNameDef name(f); name.AssertEmpty();
			CViewOrdered	* pOrdered;
			switch(type) {
				case CViewOrdered::VOT_ORDER: {
					CViewOrder	*pOrder = new CViewOrder();
					CFVector3	pt, n; double d; int nIdx;
					pt.Read(f); f.ReadInt(nIdx); n.Read(f); f.ReadDouble(d);
					pOrder->SetPlane(pt,n,d);
					f.Ascend(); // ORDH
					pOrder->SetOrdered(0,ReadOrder(f));
					pOrder->SetOrdered(1,ReadOrder(f));
					pOrdered = pOrder;
				} break;
				case CViewOrdered::VOT_BASESET: {
					int  nBaseSet;
					f.ReadInt(nBaseSet);
					ASSERT(nBaseSet>=0&&nBaseSet<m_pCurrCluster->m_nBaseSets);
					pOrdered = &m_pCurrCluster->m_pBaseSets[nBaseSet];
					f.Ascend();	// ORDH
				} break;
				default: RTCHECK(0,"Bad VOT in VBC Ord"); break;
			} // switch type
		// f.Ascend();	// ORDH
	f.Ascend();	// ORDR
	return	pOrdered;
}

void CViewBaseCluster::Read(CTaggedFile &f,bool bDynamic)
{
	m_pMatrix0 = NULL;
	m_bIdentity = FALSE;
	m_pBaseSets = NULL;
	m_pNames = NULL;
	IF_DEBUG(m_bAnimate = FALSE);
	f.Descend("VBC_",FALSE,0);
		f.Descend("VBCH",TRUE,0);
			f.ReadInt(m_nBaseSets);
			RTCHECK(m_nBaseSets>0,"0 basesets");
			bool bNeedNames;
			f.ReadBool(bNeedNames);
			if( m_bReadNamesAlways ) bNeedNames = TRUE;
			if( bNeedNames ) m_pNames = new char[m_nBaseSets][40];
			for( int i = 0 ; i < m_nBaseSets ; i++ ) {
				char	szName[40];
				f.Read(bNeedNames?m_pNames[i]:szName,40);
			}
		f.Ascend();
		f.Descend("NRMS",TRUE,0);
		f.Ascend();
		if( m_nBaseSets > 0 )	m_pBaseSets = new CViewObjectBaseSet[m_nBaseSets];
		m_fOverallRadius02 = 0;
		m_fOverallRadius2 = 0;
		m_overallMin = CFVector3(1e10,1e10,1e10);
		m_overallMax = CFVector3(-1e10,-1e10,-1e10);
		m_fHeight = 0;
		m_bHasBushes = FALSE;
		for( int i = 0 ; i < m_nBaseSets ; i++ ) {
			m_pBaseSets[i].Read(f,bDynamic);
			if( m_pBaseSets[i].m_bHasBushes )	m_bHasBushes = TRUE;
		}
		m_overallCenter = (m_overallMin+m_overallMax)/2;
		m_fHeight = m_overallMax.y-m_overallMin.y;
		for( int i = 0 ; i < m_nBaseSets ; i++ )
			m_pBaseSets[i].CalcClusterRadius();
		m_center = m_overallCenter;
		m_fRadius0 = sqrt(m_fOverallRadius02);
		m_fRadius = sqrt(m_fOverallRadius2);
		m_pCurrCluster = this;
		CViewOrdered	*pOldTop = CViewOrdered::SetCurrentTop(NULL);
		m_pOrder = ReadOrder(f);
		CViewOrdered::SetCurrentTop(pOldTop);
	f.Ascend();
}

bool	__bDrawObjects = TRUE;

inline void CViewBaseCluster::Draw(TCCFMatrix3x4Y &dir,CViewDynamicList *pDynamicList,dword dwLights)
{
	if(!__bDrawObjects) { pDynamicList->ZSortDrawNDrop(); return; }
	ASSERT(!m_bAnimate||CViewObjectRef::m_pCurrRef->m_AnimationCallback);
	m_pOrder->LoadDyns(*pDynamicList);
	TCCFMatrix3x4Y *pOldDir = CViewObjectBaseSet::m_pCurrDir;
	ASSERT(!pOldDir||IsDynamic());
	CViewObjectBaseSet::m_pCurrDir = &dir;
	CViewOrder::SetGlobalOrder(FALSE);
	m_pOrder->LoadLights(dwLights);
	m_pOrder->Draw();
	CViewOrder::SetGlobalOrder(TRUE);
	CViewObjectBaseSet::m_pCurrDir = pOldDir;
}

void CViewBaseCluster::operator%=(TCCFMatrix3x4 &dir)
{
	for( int i = 0 ; i < m_nBaseSets ; i++ )
		m_pBaseSets[i] %= dir;
	m_center = dir % m_center;
}

double CViewBaseCluster::ComputeBSPError()
{
	double fMin = 1e10;
	for( int i = 0 ; i < m_nBaseSets ; i++ )	fMin = Min(fMin,m_pBaseSets[i].ComputeBSPError());
	return fMin;
}
double CViewBaseCluster::ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d)
{
	double fMin = 1e10;
	for( int i = 0 ; i < m_nBaseSets ; i++ )	fMin = Min(fMin,m_pBaseSets[i].ComputeBSPError(pt,n,d));
	return fMin;
}

void CViewBaseCluster::Split(bool bSplit)
{
	for( int i = 0 ; i < m_nBaseSets ; i++ )
		m_pBaseSets[i].Split(bSplit);
}

void CViewBaseCluster::Hue(double fModify)
{
	for( int i = 0 ; i < m_nBaseSets ; i++ )
		m_pBaseSets[i].Hue(fModify);
}
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
TCCFMatrix3x4Y *CViewObjectBaseSet::m_pCurrDir = NULL;
double	CViewObjectBaseSet::m_fDistanceModify = 1;
double	CViewObjectBaseSet::m_fCutAngleCotangent = 0;
CViewObjectBaseSet	*CViewObjectBaseSet::m_pCurrBaseSet = NULL;

void CViewObjectBaseSet::Read(CTaggedFile &f,bool bDynamic)
{
	m_pfThresholds = NULL;
	m_pNames = NULL;
	f.Descend("VBS_",FALSE,0);
		f.Descend("VBSH",TRUE,1);
			f.ReadInt(m_nBases);
			RTCHECK(m_nBases>0,"0 bases");
			if( m_nBases > 1 )	m_pfThresholds = new double[m_nBases-1];
			for( int  i = 0 ; i < m_nBases-1 ; i++ )
				f.ReadDouble(m_pfThresholds[i]);
			bool bNeedNames;
			f.ReadBool(bNeedNames);
			if( CViewBaseCluster::m_bReadNamesAlways ) bNeedNames = TRUE;
			if( bNeedNames ) m_pNames = new char[m_nBases][40];
			for( int i = 0 ; i < m_nBases ; i++ ) {
				char	szName[40];
				f.Read(bNeedNames?m_pNames[i]:szName,40);
			}
		f.Ascend();
		m_pBases = new CViewObjectBase[m_nBases];
		m_bHasBushes = FALSE;
		for( int i = 0 ; i < m_nBases ; i++ ) {
			m_pBases[i].Read(f,bDynamic);
			if( m_pBases[i].NBushes() )	m_bHasBushes = TRUE;
			m_pBases[i].m_nReductionNum = i;
		}
	f.Ascend();
}

void	CViewObjectBaseSet::Draw()
{
	ASSERT(m_pCurrDir);
	m_pBases[0].CalcCenterDist(*m_pCurrDir);
	double	fDist = m_pBases[0].CenterDist() /**fIntegerPrec_1*/; //-dir.m[2][3];
	if( fDist > CViewFigure::HazeMax() + m_pBases[0].Radius() +
								m_pBases[0].Height()*m_fCutAngleCotangent ) {
		m_dynamicList.ZSortDrawNDrop();
		return;
	}
	double fDistM = fDist*m_fDistanceModify;
	int i;
	for( i = 0 ; i < m_nBases-1 ; i++ )
		if( fDistM < m_pfThresholds[i] )	break;
	//i = 0;
	CViewObjectBase	*pBase = m_pBases+i;
	//m_pCurrBase = m_pBases+m_nBases-1;
	pBase->SetCenterDist(fDist);
	m_pCurrBaseSet = this;
	pBase->Draw(m_dynamicList,m_dwLights);
}

double CViewObjectBaseSet::ComputeBSPError()
{
	double fMin = 1e10;
	for( int i = 0 ; i < m_nBases ; i++ )	{
		WRITELOG1("\n\n\n### Base %d:\n",i);
		double fErr = m_pBases[i].ComputeBSPError();
		fMin = Min(fMin,fErr);
	}
	return fMin;
}
double CViewObjectBaseSet::ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d)
{
	double fMin = 1e10;
	for( int i = 0 ; i < m_nBases ; i++ )	fMin = Min(fMin,m_pBases[i].ComputeBSPError(pt,n,d));
	return fMin;
}

void	CViewObjectBaseSet::CalcClusterRadius()
{
	for( int i = 0 ; i < m_nBases ; i++ )	m_pBases[i].CalcClusterRadius();
}

void	CViewObjectBaseSet::operator%=(TCCFMatrix3x4 &dir)
{
	for( int i = 0 ; i < m_nBases ; i++ )
		m_pBases[i]%=dir;
}

void	CViewObjectBaseSet::Split(bool bSplit)
{
	for( int i = 0 ; i < m_nBases ; i++ )	m_pBases[i].Split(bSplit);
}

void CViewObjectBaseSet::Hue(double fModify)
{
	for( int i = 0 ; i < m_nBases ; i++ )
		m_pBases[i].Hue(fModify);
}
// -------------------------------------------------------------------------
void SViewObjectMorphNames::Read(CTaggedFile &f)
{
	f.Descend("NAMS",FALSE,0);
		vertices.Read(f);
		figures.Read(f);
		clusters.Read(f);
	f.Ascend();
}
void SViewObjectMorphNames::FinishResolve()
{
	vertices.FinishResolve();
	figures.FinishResolve();
	clusters.FinishResolve();
}

CViewObjectBase	*CViewObjectBase::m_pCurrBase = NULL;
int		CViewObjectBase::m_z1Shift;
double	CViewObjectBase::m_fz1Scale;
CViewOrdered	* CViewObjectBase::ReadOrder(CTaggedFile &f)
{
	ASSERT(m_pCurrBase);
	f.Descend("ORDR",FALSE,1);
		f.Descend("ORDH",TRUE,1);
			int  type;
			f.ReadInt(type);
			CNameDef name(f);
			CViewOrdered	* pOrdered;
			switch(type) {
				case CViewOrdered::VOT_ORDER: {
					name.AssertEmpty();
					CViewOrder	*pOrder = new CViewOrder();
					CFVector3	pt, n; double d; int nIdx;
					pt.Read(f); f.ReadInt(nIdx); n.Read(f); f.ReadDouble(d);
					pOrder->SetPlane(pt,n,d);
					f.Ascend(); // ORDH
					pOrder->SetOrdered(0,ReadOrder(f));
					pOrder->SetOrdered(1,ReadOrder(f));
					pOrdered = pOrder;
				} break;
				case CViewOrdered::VOT_BODY: {
					name.AssertEmpty();
					int  nBody;
					f.ReadInt(nBody);
					ASSERT((uint)nBody < m_pCurrBase->m_nBodies);
					pOrdered = &m_pCurrBase->m_pBodies[nBody];
					f.Ascend();	// ORDH
				} break;
				case CViewOrdered::VOT_BUSH: {
					name.AssertEmpty();
					int  nBush;
					f.ReadInt(nBush);
					ASSERT((uint)nBush < m_pCurrBase->m_nBushes);
					pOrdered = &m_pCurrBase->m_pBushes[nBush];
					f.Ascend();	// ORDH
				} break;
				case CViewOrdered::VOT_MORPHEDORDER: {
					CViewMorphedOrder	*pOrder = new CViewMorphedOrder();
					CFVector3	pt, n; double d; int nIdx;
					pt.Read(f); f.ReadInt(nIdx); n.Read(f); f.ReadDouble(d);
					ASSERT(d==0);
					pOrder->SetPlane(pt,n,d);
					pOrder->ReadIndices(f);
					f.Ascend(); // ORDH
					pOrder->SetOrdered(0,ReadOrder(f));
					pOrder->SetOrdered(1,ReadOrder(f));
					pOrdered = pOrder;
					name.Resolve(pOrdered,m_pCurrBase->m_names.clusters);
				} break;
				default: RTCHECK(0,"Bad VOT in VOB Ord"); break;
			} // switch type
		// f.Ascend();	// ORDH
	f.Ascend();	// ORDR
	return	pOrdered;
}

void CViewObjectBase::Read(CTaggedFile &f,bool bDynamic)
{
	m_pBumpF = NULL;
	m_nBumpF = 0;

	if( bDynamic ) {
		//m_pVertImages3 = &CViewObject::m_aVertImages3[0];
												//CViewObject::MAX_VERTICES];
		CViewObject::m_paVertImages = m_pVertImages =
			&CViewObject::m_aVertImages[CViewObject::MAX_VERTICES];
		CViewObject::m_paInfVertImages = m_pInfVertImages =
			&CViewObject::m_aInfVertImages[CViewObject::MAX_EDGEVERTICES];
		m_pCurrentInvMx = &CViewObject::m_currentInvMx[1];
	}
	else {
		//m_pVertImages3 = CViewObject::m_aVertImages3;
		CViewObject::m_paVertImages = m_pVertImages =
			CViewObject::m_aVertImages;
		CViewObject::m_paInfVertImages = m_pInfVertImages =
			CViewObject::m_aInfVertImages;
		m_pCurrentInvMx = &CViewObject::m_currentInvMx[0];
	}
	f.Descend("VOB_",FALSE,2);
		f.Descend("VOBH",TRUE,1);
			int type;
			f.ReadInt(type);
			RTCHECK(type==0,"Bad VOB type");
			f.ReadInt(m_nBodies);
			f.ReadInt(m_nVertices);
			RTCHECK(m_nVertices<=CViewObject::MAX_VERTICES,
												"VOB R Vert Many");
			f.ReadInt(m_nRenderedVertices);
			RTCHECK( m_nRenderedVertices <= m_nVertices, "Bad num rend verts");
			f.ReadInt(m_nSplitVertices);
			RTCHECK( m_nSplitVertices <= m_nVertices, "Bad num split verts");
			RTCHECK(m_nRenderedVertices>=m_nSplitVertices,"Bad num split/rend verts");
			f.SetRTEFatal(FALSE);
			m_nBushes = 0;
			f.ReadInt(m_nBushes);
			f.SetOK(TRUE);
		f.Ascend();
		m_names.Read(f);
		f.Descend("VOBV",TRUE,0);
			m_pVertices = m_nVertices > 0 ? new CFVector3[m_nVertices] : NULL;
			m_pBodies = m_nBodies > 0 ? new CViewBody[m_nBodies] : NULL;
			m_pVertEdges = m_nVertices > 0 ? new SVertexEdges[m_nVertices] : NULL;
			m_pBushes = m_nBushes > 0 ? new CViewBush[m_nBushes] : NULL;
			CFVector3	minBound(1e10,1e10,1e10), maxBound(-1e10,-1e10,-1e10);
			for( int  i = 0 ; i < m_nVertices ; i++ ) {
				m_pVertices[i].Read(f);
				CalcBounds(m_pVertices[i],minBound,maxBound);
				CalcBounds(m_pVertices[i],CViewBaseCluster::m_overallMin,CViewBaseCluster::m_overallMax);
				CViewBaseCluster::m_fOverallRadius02 = Max(Abs2(m_pVertices[i]),CViewBaseCluster::m_fOverallRadius02);
			}
			m_center = (maxBound+minBound)/2;
			m_fHeight = Max(maxBound.y - minBound.y,0.);
			m_fRadius = 0;
			int  nEdgeVerts = 0;
			for( int i = 0 ; i < m_nRenderedVertices ; i++ ) {
				m_fRadius = Max(m_fRadius,Abs2(m_pVertices[i]-m_center));
				f.ReadInt(m_pVertEdges[i].n);
				nEdgeVerts += m_pVertEdges[i].n;
			}
			m_fRadius = sqrt(m_fRadius);
			RTCHECK(nEdgeVerts<CViewObject::MAX_EDGEVERTICES,"VOB Many Edges");
			m_pEdgeOtherVertices=nEdgeVerts>0?new TCSFVector3Y*[nEdgeVerts]:NULL;
			nEdgeVerts = 0;
			for( int i = 0 ; i < m_nRenderedVertices ; i++ ) {
				m_pVertEdges[i].oth = m_pEdgeOtherVertices+nEdgeVerts;
				m_pVertEdges[i].im = m_pInfVertImages+nEdgeVerts;
				nEdgeVerts += m_pVertEdges[i].n;
				for( int  j = 0 ; j < m_pVertEdges[i].n ; j++ ) {
					int n; f.ReadInt(n);
					//m_pVertEdges[i].oth[j] = &m_pVertImages3[n];
					m_pVertEdges[i].oth[j] = &m_pVertImages[n].v3;
				}
			}
			for( int i = 0 ; i < m_nVertices ; i++ ) {
				CNameDef	name(f);
				ASSERTMSG(i>=m_nSplitVertices||name.IsEmpty(),"Split vertex can't be named");
				name.Resolve(&m_pVertices[i],m_names.vertices);
			}
		f.Ascend(); // VOBV
		paletteTranslator.Define(f);
		/*f.Descend("PAL_",TRUE,0);
			int  nPalEntries;
			f.ReadInt(nPalEntries);
			RTCHECK((unsigned)nPalEntries<=256,"VOB BadPal");
			SRGB	palette[256];
			f.Read(palette,sizeof(SRGB)*nPalEntries);
			paletteTranslator.Define(palette,nPalEntries);
		f.Ascend();*/
		f.Descend("NRMS",TRUE,0);
		f.Ascend();
		m_pCurrBase = this;
		for( int i = 0 ; i < m_nBodies ; i++ )
			m_pBodies[i].Read(f);
		for( int i = 0 ; i < m_nBushes ; i++ ) {
			m_pBushes[i].Read(f);
			CFVector3	vmin, vmax;
			vmin = vmax = m_pBushes[i].Center();
			vmin.x -= m_pBushes[i].Radius();
			vmin.y -= m_pBushes[i].Radius();
			vmin.z -= m_pBushes[i].Radius();
			vmax.x += m_pBushes[i].Radius();
			vmax.y += m_pBushes[i].Radius();
			vmax.z += m_pBushes[i].Radius();
			CalcBounds(vmin,CViewBaseCluster::m_overallMin,CViewBaseCluster::m_overallMax);
			CalcBounds(vmax,CViewBaseCluster::m_overallMin,CViewBaseCluster::m_overallMax);
			CViewBaseCluster::m_fOverallRadius02 = Max(m_pBushes[i].Radius0()*m_pBushes[i].Radius0(),CViewBaseCluster::m_fOverallRadius02);
			m_fHeight = Max(m_fHeight,vmax.y-vmin.y);
			m_fRadius = Abs(vmax-vmin)/2;
			m_center = m_pBushes[i].Center();
		}


		CViewOrdered	*pOldTop = CViewOrdered::SetCurrentTop(NULL);
		m_pOrder = ReadOrder(f); //CViewOrdered::Read(f);
		CViewOrdered::SetCurrentTop(pOldTop);
		m_pCurrBase = NULL;
		m_names.FinishResolve();
		m_keyFrameSet.Read(f,m_names);
		/*f.Descend("VKFS",FALSE,0);
			f.Descend("VKSH",TRUE,0);
				int nKeyFSets;
				f.ReadInt(nKeyFSets);
				ASSERT(nKeyFSets >= 0 && nKeyFSets < 100 );
				// suspicious if > 100
			f.Ascend();
		f.Ascend();*/
	f.Ascend();
}
// -------------------------------------------------------------------------
void CViewObjectBase::CalcClusterRadius()
{
	for( int i = 0 ; i < m_nVertices ; i++ )
		CViewBaseCluster::m_fOverallRadius2 =
			Max(Abs2(m_pVertices[i]-CViewBaseCluster::m_overallCenter),
										CViewBaseCluster::m_fOverallRadius2);
	for( int i = 0 ; i < m_nBushes ; i++ ) {
		double fRadius = Abs(m_pBushes[i].Center()-CViewBaseCluster::m_overallCenter)+m_pBushes[i].Radius();
		CViewBaseCluster::m_fOverallRadius2 =
			Max(fRadius*fRadius,	CViewBaseCluster::m_fOverallRadius2);
	}
}

// -------------------------------------------------------------------------
CViewObjectBase::CViewObjectBase()
{
	m_pVertices = NULL;
	m_pBodies = NULL;
	m_pVertEdges = NULL;
	m_pEdgeOtherVertices = NULL;
	m_pOrder = NULL;
	m_nSplitX = m_nSplitY = m_nSplitZ = 0;
	m_pSplitX = NULL;
	m_pSplits = NULL;
	m_pBushes = NULL;
}
// -------------------------------------------------------------------------
CViewObjectBase::~CViewObjectBase()
{
	m_pOrder->Delete();
	delete	[] m_pVertices;
	delete	[] m_pBodies;
	delete	[] m_pBushes;
	delete [] m_pVertEdges;
	delete [] m_pEdgeOtherVertices;
	delete [] m_pSplitX;
	delete [] m_pSplits;
	delete [] m_pBumpF;
}
// -------------------------------------------------------------------------
bool	CViewObjectBase::IsVisible(TCCFMatrix3x4 &dir)
{
	CFVector3	center;
	dir.Mult(m_center,center);
	if( center.z>=m_fRadius )	return FALSE;
	#if _VIEW_ORTHO
		if( !CViewObject::m_bViewOrtho )
	#endif
	{
		double	d0;
		d0 = center.x+CViewObject::m_clipRect0.right*center.z;
		if( d0 > 0 && d0*CViewObject::m_clipRect0N.right > m_fRadius )
																return FALSE;
		d0 = center.x+CViewObject::m_clipRect0.left*center.z;
		if( d0 < 0 && d0*CViewObject::m_clipRect0N.left > m_fRadius )
																return FALSE;
		d0 = center.y+CViewObject::m_clipRect0.bottom*center.z;
		if( d0 > 0 && d0*CViewObject::m_clipRect0N.bottom > m_fRadius )
																return FALSE;
		d0 = center.y+CViewObject::m_clipRect0.top*center.z;
		if( d0 < 0 && d0*CViewObject::m_clipRect0N.top > m_fRadius )
																return FALSE;
	}
	CViewObject::m_pCurrentInvMx = m_pCurrentInvMx;
	return	TRUE;
}
// -------------------------------------------------------------------------
//void Double2Int(double f,int *p);
//#pragma aux Double2Int = "fistp dword ptr [ebx]" parm [8087 ebx];
inline void MultSplitsLoop(TCdouble *pDir,int nCount,
					double *pfSplit,CViewObjectBase::USplitVect *pSplit)
{
	while(nCount--) {
		pSplit->v.x=pDir[0]**pfSplit;
		pSplit->v.y=pDir[4]**pfSplit;
		pSplit->v.z=pDir[8]**pfSplit;
		pfSplit++;
		pSplit++;
	}
}
/*#ifdef	__WATCOMC__
#pragma aux MultSplitsLoop = \
	"test 	ebx,ebx"	\
	"jle	L1"	\
	"L0:"\
	"fld	qword ptr [eax]"	\
	"fmul	qword ptr [ecx]"	\
	"fistp	dword ptr [edx]"	\
	"fld	qword ptr [eax+20h]"	\
	"fmul	qword ptr [ecx]"	\
	"fistp	dword ptr [edx+4]"	\
	"fld	qword ptr [eax+40h]"	\
	"fmul	qword ptr [ecx]"	\
    "dec	ebx"	\
	"add	ecx,8"	\
	"add	edx,16"	\
	"fistp	dword ptr [edx-8]"	\
	"test	ebx,ebx"	\
	"jg		L0"	\
	"L1:"\
	parm [eax] [ebx] [ecx] [edx] modify exact [ebx ecx edx];
#endif*/
void CViewObjectBase::MultSplits(TCCFMatrix3x4 &dir)
{
	/*int		i;
	double	*pfSplit, f;
	USplitVect	*pSplit;*/
	MultSplitsLoop(&dir.m[0][0],m_nSplitX,m_pSplitX,m_splitX);
	MultSplitsLoop(&dir.m[0][1],m_nSplitY,m_pSplitY,m_splitY);
	MultSplitsLoop(&dir.m[0][2],m_nSplitZ,m_pSplitZ,m_splitZ);
	/*for( i = m_nSplitX, pfSplit = m_pSplitX, pSplit = m_splitX ;
					  			i > 0 ; --i, ++pfSplit, ++pSplit ) {
		f = *pfSplit;
		pSplit->v.x = dir.m[0][0]*f;
		pSplit->v.y = dir.m[1][0]*f;
		pSplit->v.z = dir.m[2][0]*f;
	}
	for( i = m_nSplitY, pfSplit = m_pSplitY, pSplit = m_splitY ;
					  			i > 0 ; --i, ++pfSplit, ++pSplit ) {
		f = *pfSplit;
		pSplit->v.x = dir.m[0][1]*f;
		pSplit->v.y = dir.m[1][1]*f;
		pSplit->v.z = dir.m[2][1]*f;
	}
	for( i = m_nSplitZ, pfSplit = m_pSplitZ, pSplit = m_splitZ ;
					  			i > 0 ; --i, ++pfSplit, ++pSplit ) {
		f = *pfSplit;
		pSplit->v.x = dir.m[0][2]*f;
		pSplit->v.y = dir.m[1][2]*f;
		pSplit->v.z = dir.m[2][2]*f;
	}*/
}
void CViewObjectBase::AddSplits(int nOffsetX,int nOffsetY,int nOffsetZ)
{
	int i;
	//CVector3	*pVI3 = m_pVertImages3;
	CViewObject::SFinVertImage	*pVI = m_pVertImages;
	SSplitRef	*pSplit = m_pSplits;
	for( i = m_nSplitVertices ; i > 0 ; --i, ++pVI, ++pSplit ) {
		int x = nOffsetX;
		int y = nOffsetY;
		int z = nOffsetZ;

		SFVector3Y	*p = pSplit->px;
		x += p->x;
		y += p->y;
		z += p->z;

		p = pSplit->py;
		x += p->x;
		y += p->y;
		z += p->z;

		p = pSplit->pz;
		x += p->x;
		y += p->y;
		z += p->z;

		pVI->v3.x = x;
		pVI->v3.y = y;
		pVI->v3.z = z;
	}
}
// ------------------------------------------------------------------------
inline void MultSplitsLoopY(TCdouble *pDir,int nCount,
					double *pfSplit,CViewObjectBase::USplitVect *pSplit)
{
	while(nCount--) {
		pSplit->v.x=pDir[0]**pfSplit;
		pSplit->v.y=pDir[4]**pfSplit;
		pSplit->v.z=pDir[8]**pfSplit;
		pSplit->v.y0=pDir[12]**pfSplit;
		pfSplit++;
		pSplit++;
	}
}
/*#ifdef	__WATCOMC__
#pragma aux MultSplitsLoopY = \
	"test 	ebx,ebx"	\
	"jle	L1"	\
	"L0:"\
	"fld	qword ptr [eax]"	\
	"fmul	qword ptr [ecx]"	\
	"fistp	dword ptr [edx]"	\
	"fld	qword ptr [eax+20h]"	\
	"fmul	qword ptr [ecx]"	\
	"fistp	dword ptr [edx+4]"	\
	"fld	qword ptr [eax+40h]"	\
	"fmul	qword ptr [ecx]"	\
	"fistp	dword ptr [edx+8]"	\
	"fld	qword ptr [eax+60h]"	\
	"fmul	qword ptr [ecx]"	\
    "dec	ebx"	\
	"add	ecx,8"	\
	"add	edx,16"	\
	"fistp	dword ptr [edx-4]"	\
	"test	ebx,ebx"	\
	"jg		L0"	\
	"L1:"\
	parm [eax] [ebx] [ecx] [edx] modify exact [ebx ecx edx];
#endif*/
void CViewObjectBase::MultSplitsY(TCCFMatrix3x4 &dir)
{
	MultSplitsLoopY(&dir.m[0][0],m_nSplitX,m_pSplitX,m_splitX);
	MultSplitsLoopY(&dir.m[0][1],m_nSplitY,m_pSplitY,m_splitY);
	MultSplitsLoopY(&dir.m[0][2],m_nSplitZ,m_pSplitZ,m_splitZ);
}
// -------------------------------------------------------------------------
void CViewObjectBase::AddSplitsY(int nOffsetX,int nOffsetY,int nOffsetZ,int nOffsetY0)
{
	int i;
	//CVector3	*pVI3 = m_pVertImages3;
	CViewObject::SFinVertImage	*pVI = m_pVertImages;
	SSplitRef	*pSplit = m_pSplits;
	for( i = m_nSplitVertices ; i > 0 ; --i, ++pVI, ++pSplit ) {
		int x = nOffsetX;
		int y = nOffsetY;
		int z = nOffsetZ;
		int y0 = nOffsetY0;

		SFVector3Y	*p = pSplit->px;
		x += p->x;
		y += p->y;
		z += p->z;
		y0 += p->y0;

		p = pSplit->py;
		x += p->x;
		y += p->y;
		z += p->z;
		y0 += p->y0;

		p = pSplit->pz;
		x += p->x;
		y += p->y;
		z += p->z;
		y0 += p->y0;

		pVI->v3.x = x;
		pVI->v3.y = y;
		pVI->v3.z = z;
		pVI->v3.y0 = y0;
	}
}
// -------------------------------------------------------------------------
CViewObjectBase::USplitVect	CViewObjectBase::m_splitX[CViewObject::MAX_VERTICES];
CViewObjectBase::USplitVect	CViewObjectBase::m_splitY[CViewObject::MAX_VERTICES];
CViewObjectBase::USplitVect	CViewObjectBase::m_splitZ[CViewObject::MAX_VERTICES];
#if	_VIEW_ORTHO
	#define	ZNORM(x,z)	(CViewObject::m_bViewOrtho?(x)>>nIntegerShift:(x)/(z))
#else
	#define	ZNORM(x,z)	((x)/(z))
#endif
int __nVertCnt;

inline	int CViewObjectBase::PreDraw(bool bWaterSplit)
{
	int	nZClip = 0;

	#if !_USE_OPENGL
		if( m_nSplitVertices > 0 ) {
			ASSERT(0);
			ASSERT(m_pSplits&&m_pSplitX);
			if( bWaterSplit ) {
				MultSplitsY(*CViewObjectBaseSet::m_pCurrDir);
				AddSplitsY(CViewObjectBaseSet::m_pCurrDir->m[0][3],
								CViewObjectBaseSet::m_pCurrDir->m[1][3],
								CViewObjectBaseSet::m_pCurrDir->m[2][3],
								CViewObjectBaseSet::m_pCurrDir->y[3]);
			}
			else {
				MultSplits(*CViewObjectBaseSet::m_pCurrDir);
				AddSplits(CViewObjectBaseSet::m_pCurrDir->m[0][3],
								CViewObjectBaseSet::m_pCurrDir->m[1][3],
								CViewObjectBaseSet::m_pCurrDir->m[2][3]);
			}
		}
		ASSERT( m_nSplitVertices == m_nRenderedVertices || m_pVertices );
		for( int i = m_nSplitVertices ; i < m_nRenderedVertices ; i++ ) {
			CFVector3	&v0 = m_pVertices[i];
			//CFVector3	v; //CVector3	&vi = m_pVertImages3[i];
			SFVector3Y 	&vi = m_pVertImages[i].v3;
			CViewObjectBaseSet::m_pCurrDir->Mult(v0,vi);
			//D2INT(&vi.x,v.x); //*fIntegerPrec);
			//D2INT(&vi.y,v.y); //*fIntegerPrec);
			//D2INT(&vi.z,v.z); //*fIntegerPrec);
			if( bWaterSplit ) vi.y0 = CViewObjectBaseSet::m_pCurrDir->Y(v0);
				// D2INT(&vi.y0,CViewObjectBaseSet::m_pCurrDir->Y(v0));
		}
		#ifdef _DEBUG
			__nVertCnt += m_nVertices;
		#endif

		//int	 nFrontClip = CViewObject::m_fFrontClip*fIntegerPrec;

		for( int i = 0 ; i < m_nVertices ; i++ ) {
			//CVector3 &v = m_pVertImages3[i];
			CViewObject::SFinVertImage	&im = m_pVertImages[i];
			SFVector3Y	&v = im.v3;
			double	fClipD;
			if( (fClipD = CViewObject::m_fFrontClip-v.z) > 0 ) { // clipped front
				nZClip |= (im.cf = CViewObject::ZCF_FRONT);
				int  nEdges = m_pVertEdges[i].n;
				TCSFVector3Y	**pOth = m_pVertEdges[i].oth;
				CViewObject::SInfVertImage	*pIm = m_pVertEdges[i].im;
				for( int  j = 0 ; j < nEdges ; pOth++, pIm++, j++ ) {
					TCSFVector3Y	& v1 = **pOth;
					if( v1.z >= CViewObject::m_fFrontClip ) {
						double		v1z_vz = v1.z-v.z;
						//int	t = SHLD_Div(nClipD,v1z_vz,CViewObject::CLIP_SHIFT);
						ASSERT(v1z_vz>=0);
						double t = fClipD/Max(v1z_vz,1e-5);
						//pIm->v.x = ZNORM(pIm->v3.x=v.x+Mul_SHRD(v1.x-v.x,t,
						//	CViewObject::CLIP_SHIFT),CViewObject::m_nFrontClip);
						D2INT(&pIm->v.x,(pIm->v3.x=v.x+(v1.x-v.x)*t)*CViewObject::m_f1FrontClip);
						//pIm->v.y = ZNORM(pIm->v3.y=v.y+Mul_SHRD(v1.y-v.y,t,
						//	CViewObject::CLIP_SHIFT),CViewObject::m_nFrontClip);
						D2INT(&pIm->v.y,(pIm->v3.y=v.y+(v1.y-v.y)*t)*CViewObject::m_f1FrontClip);
						//pIm->v.z = ((65536*nIntegerPrec)/CViewObject::m_nFrontClip);
						//pIm->v.z = SHLD_Div(65536*nIntegerPrec,pIm->v3.z=CViewObject::m_nFrontClip,m_z1Shift);
						pIm->v3.z=CViewObject::m_fFrontClip;
						D2INT(&pIm->v.z,CViewObject::m_f1FrontClip*m_fz1Scale);
						if( bWaterSplit ) pIm->v3.y0=v.y0+(v1.y0-v.y0)*t;
							// pIm->v3.y0=v.y0+Mul_SHRD(v1.y0-v.y0,t,CViewObject::CLIP_SHIFT);
						#if _USE_OWNGR
							ASSERT((uint)pIm->v.z<70000);
						#endif
						D2INT(&pIm->nRatio,t*CViewObject::CLIP_PREC);
					}
				}
			} else
			if( (fClipD = v.z-CViewObject::m_fBackClip) > 0 ) { // clipped back
				nZClip |= (im.cf = CViewObject::ZCF_BACK);
				int  nEdges = m_pVertEdges[i].n;
				TCSFVector3Y	**pOth = m_pVertEdges[i].oth;
				CViewObject::SInfVertImage	*pIm = m_pVertEdges[i].im;
				for( int  j = 0 ; j < nEdges ; pOth++, pIm++, j++ ) {
					TCSFVector3Y	& v1 = **pOth;
					if( v1.z <= CViewObject::m_fBackClip ) {
						double		vz_v1z = v.z-v1.z;
						//int	t = SHLD_Div(nClipD,vz_v1z,CViewObject::CLIP_SHIFT);
						ASSERT(vz_v1z>=0);
						double t = fClipD/Max(vz_v1z,1e-5);
						//pIm->v.x = ZNORM(pIm->v3.x=v.x-Mul_SHRD(v.x-v1.x,t,
						//	CViewObject::CLIP_SHIFT),CViewObject::m_nBackClip);
						D2INT(&pIm->v.x,(pIm->v3.x=v.x-(v.x-v1.x)*t)*CViewObject::m_f1BackClip);
						//pIm->v.y = ZNORM(pIm->v3.y=v.y-Mul_SHRD(v.y-v1.y,t,
						//	CViewObject::CLIP_SHIFT),CViewObject::m_nBackClip);
						D2INT(&pIm->v.y,(pIm->v3.y=v.y-(v.y-v1.y)*t)*CViewObject::m_f1BackClip);
						//pIm->v.z = ((65536*nIntegerPrec)/CViewObject::m_nBackClip);
						//pIm->v.z = SHLD_Div(65536*nIntegerPrec,pIm->v3.z=CViewObject::m_nBackClip,m_z1Shift);
						pIm->v3.z=CViewObject::m_fBackClip;
						D2INT(&pIm->v.z,CViewObject::m_f1BackClip*m_fz1Scale);
						if( bWaterSplit ) pIm->v3.y0=v.y0+(v1.y0-v.y0)*t;
							// pIm->v3.y0=v.y0+Mul_SHRD(v1.y0-v.y0,t,CViewObject::CLIP_SHIFT);
						#if _USE_OWNGR
							ASSERT((uint)pIm->v.z<70000);
						#endif
						D2INT(&pIm->nRatio,t*CViewObject::CLIP_PREC);
					}
				}
			}
			else { // if not clipped
				double _1z = 1./v.z;
				D2INT(&im.v.x,v.x*_1z); //ZNORM(v.x,v.z);
				D2INT(&im.v.y,v.y*_1z); //ZNORM(v.y,v.z);
				//im.v.z = ((65536*nIntegerPrec)/v.z);
				//im.v.z = SHLD_Div(65536*nIntegerPrec,v.z,m_z1Shift);
				D2INT(&im.v.z,_1z*m_fz1Scale);
				#if _USE_OWNGR
					ASSERT((uint)im.v.z<70000);
				#endif
				im.cf = 0;
				nZClip |= 0x80;
			}
		}
	#endif

	return nZClip;
}

void	CViewObjectBase::Draw(CViewDynamicList &dynamicList,dword dwLights)
{
	bool bHaze = CViewObject::m_bHaze &&
						m_fRadius+m_fCenterDist >= CViewFigure::HazeMin();
	#if _USE_OWNGR
		CalcCenterDist(*CViewObjectBaseSet::m_pCurrDir);
		//double	fDist = m_pBases[0].CenterDist() /**fIntegerPrec_1*/; //-dir.m[2][3];
		m_z1Shift = Max(CViewObject::m_z1MinShift,Round(log2(Max(CViewObject::m_fFrontClip,m_fCenterDist-m_fRadius))-0.5));
	#else
		ASSERT(0);	// Working no more
		m_z1Shift = 0;
	#endif
	m_fz1Scale = 1<<(m_z1Shift+16); //*65536;

	//int nZClip = PreDraw(TRUE);
	Animate();
	int nZClip = CViewFigure::m_eWaterSplit == WATER_SPLIT_NONE ? PreDraw(FALSE) : PreDraw(TRUE);

	if( (uint)(nZClip-1)>=2 )
	{
		CViewFigure::SetHaze( bHaze );
		GRSetZPrecision(m_z1Shift);
		CViewObject::m_bLastDrawn = TRUE;
		ASSERT(m_pOrder);
		/*if( m_nBushes > 0 ) {
			double fBushCenter = Abs2(m_pBushes[0].Center()-CViewObject::m_currObjViewPoint);
			for( CViewDynamic *pd = dynamicList.First(), *pd1 ; pd!=NULL ; pd = pd1 ) {
				pd1 = pd->Next();
				if( Abs2(((CViewSphericDynamic*)pd)->m_dynBase-CViewObject::m_currObjViewPoint) > fBushCenter ) {
					dynamicList.Remove(pd);
					pd->Draw();
				}
			}
		} else*/
		m_pOrder->LoadDyns(dynamicList);
		m_pOrder->LoadLights(dwLights);

		#if _USE_OPENGL
			glPushAttrib(GL_COLOR_BUFFER_BIT|GL_LIGHTING_BIT);
			glDisable(GL_BLEND);
			glShadeModel(GL_FLAT);
		#endif
		m_pOrder->Draw();

		//if( m_nBushes > 0 ) dynamicList.Draw();
		#if _USE_OPENGL
			glPopAttrib();
		#endif
	}
	else {
		dynamicList.ZSortDrawNDrop();
	}
}
#undef	ZNORM
// -------------------------------------------------------------------------
void CViewObjectBase::operator%=(TCCFMatrix3x4 &dir)
{
	ASSERT(m_pVertices);
	for( int  i = 0 ; i < m_nVertices ; i++ )
		m_pVertices[i] = dir%m_pVertices[i];
	m_center = dir%m_center;
	*m_pOrder %= dir;
	m_keyFrameSet %= dir;
}
// -------------------------------------------------------------------------
double CViewObjectBase::ComputeBSPError()
{
	ASSERT(m_pVertices||!m_nBodies);	// ComputeBSPError must be called before Split()
	m_pCurrBase = this;
	return m_pOrder->ComputeBSPError();
}
double CViewObjectBase::ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d)
{
	ASSERT(m_pVertices||!m_nBodies);	// ComputeBSPError must be called before Split()
	double fMin = 1e10;
	for( int i = 0 ; i < m_nVertices ; i++ ) fMin = Min(fMin,(m_pVertices[i]-pt)*n-d);
	return fMin;
}
// -------------------------------------------------------------------------
void CViewObjectBase::Hue(double fModify)
{
	for( int i = 0 ; i < m_nBodies ; i++ )
		m_pBodies[i].Hue(fModify);
}
// -------------------------------------------------------------------------
void CViewObjectBase::Split(bool bSplit)
{
	(void)bSplit;
	bSplit = FALSE;

	#if 1

	if( m_nBushes > 0 && m_nBodies == 0 ) return;
	//bSplit = FALSE;
	#if _USE_OPENGL
		(void) bSplit;
		m_nSplitVertices = 0;
	#else
		if( !bSplit )	{ m_nSplitVertices = 0; return; }
		const double fSplitPrec = 1e-3;
		ASSERT(m_pVertices);
		m_pSplits = new SSplitRef[m_nVertices];
		m_nSplitX = m_nSplitY = m_nSplitZ = 0;
		for( int  i = 0 ; i < m_nVertices ; i++ ) {
			int j = 0;
			for( j = 0 ; j < m_nSplitX ; j++ )
				if( fabs(m_splitX[j].f-m_pVertices[i].x) < fSplitPrec ) break;
			if( j == m_nSplitX ) m_splitX[m_nSplitX++].f = m_pVertices[i].x;
			m_pSplits[i].px = &m_splitX[j].v;
			for( j = 0 ; j < m_nSplitY ; j++ )
				if( fabs(m_splitY[j].f-m_pVertices[i].y) < fSplitPrec ) break;
			if( j == m_nSplitY ) m_splitY[m_nSplitY++].f = m_pVertices[i].y;
			m_pSplits[i].py = &m_splitY[j].v;
			for( j = 0 ; j < m_nSplitZ ; j++ )
				if( fabs(m_splitZ[j].f-m_pVertices[i].z) < fSplitPrec ) break;
			if( j == m_nSplitZ ) m_splitZ[m_nSplitZ++].f = m_pVertices[i].z;
			m_pSplits[i].pz = &m_splitZ[j].v;
		}
		#if 0
		// cannot delete m_pVertices, used by figures now
		if( m_nSplitVertices == m_nVertices ) {
			delete [] m_pVertices; m_pVertices = NULL;
		}
		#endif
		m_pSplitX = new double[m_nSplitX+m_nSplitY+m_nSplitZ];
		m_pSplitY = m_pSplitX+m_nSplitX;
		m_pSplitZ = m_pSplitY+m_nSplitY;
		for( int i = 0 ; i < m_nSplitX ; i++ )	m_pSplitX[i] = m_splitX[i].f;
		for( int i = 0 ; i < m_nSplitY ; i++ )	m_pSplitY[i] = m_splitY[i].f;
		for( int i = 0 ; i < m_nSplitZ ; i++ )	m_pSplitZ[i] = m_splitZ[i].f;
	#endif

	#endif

}
// -------------------------------------------------------------------------
void CViewObjectBase::SetOrderToBody(int n)
{
	ASSERT( ((byte*)m_pOrder-(byte*)m_pBodies)%sizeof(CViewBody)==0&&
			(uint)((CViewBody*)m_pOrder-m_pBodies)<m_nBodies);
	ASSERT(n>=0&&n<m_nBodies);
	m_pOrder = &m_pBodies[n];
}


void	CViewObjectBase::AllocBumpFigures(int nSets)
{
	ASSERT(!m_pBumpF);
	m_nBumpF = nSets;
	m_pBumpF = nSets > 0 ? new SBumpFigures[nSets] : NULL;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
CFMatrix3x4	*CViewObject::m_pCurrentInvMx;
CFMatrix3x4 CViewObject::m_currentInvMx[2];
CFMatrix3x4	CViewObject::m_viewPointDirMx, CViewObject::m_viewPointInvMx,
											CViewObject::m_viewPointDirSMx;
CFVector3	CViewObject::m_viewPointScale, CViewObject::m_viewPointScale2,
			CViewObject::m_viewPointScale_1; //, CViewObject::m_viewPointScaleInt;
CViewObject::SFinVertImage	CViewObject::m_aVertImages[MAX_VERTICES*2];
CViewObject::SInfVertImage	CViewObject::m_aInfVertImages[MAX_EDGEVERTICES*2];
SFVector3		CViewObject::m_aModelLights[LIGHT_SOURCE_COUNT*2];
SFVector3		CViewObject::m_aGlobalLights[LIGHT_SOURCE_COUNT];
SRect2			CViewObject::m_aGlobalLightsRect[LIGHT_SOURCE_COUNT];
double			CViewObject::m_terrainScale_1;
dword			CViewObject::m_dwLights = 0;

CViewObject::SFinVertImage *CViewObject::m_paVertImages;
CViewObject::SInfVertImage *CViewObject::m_paInfVertImages;
//CVector3	CViewObject::m_aVertImages3[MAX_VERTICES*2];
CFRect2		CViewObject::m_clipRect(-160,-100,160,100);
CFRect2		CViewObject::m_clipRect2(160*160,100*100,160*160,100*100);
CFRect2		CViewObject::m_clipRect0, CViewObject::m_clipRect0N;
CFVector3	CViewObject::m_clipRays[4];
double		CViewObject::m_fFrontClip = 1;
//int			CViewObject::m_nFrontClip = 1<<nIntegerShift;
double		CViewObject::m_fBackClip = 100000;
double		CViewObject::m_f1FrontClip = 1;
double		CViewObject::m_f1BackClip = 1./100000;
//int			CViewObject::m_nBackClip = 100000<<nIntegerShift;
CFVector3	CViewObject::m_currObjViewPoint;
bool		CViewObject::m_bViewOrtho = FALSE;
bool		CViewObject::m_bHaze = FALSE;
bool		CViewObject::m_bLastDrawn = FALSE;
WATER_VIEW	CViewObject::m_eWaterView = WATER_VIEW_ALL;
bool		CViewObject::m_bIsBelowWater = FALSE;
int			CViewObject::m_z1MinShift = 0;
double		CViewObject::m_fz1MinScale = 65536.;
// -------------------------------------------------------------------------
void CViewObject::SetViewPoint(TCCFMatrix3x4 &viewPointDirMx,
													TCCFVector3 &scale )
{
	m_viewPointDirMx = viewPointDirMx;
	m_viewPointScale = scale;
	m_viewPointInvMx.LoadTransposed(viewPointDirMx);
	//m_viewPointScaleInt.x = m_viewPointScale.x*fIntegerPrec;
	//m_viewPointScaleInt.y = m_viewPointScale.y*fIntegerPrec;
	//m_viewPointScaleInt.z = m_viewPointScale.z*fIntegerPrec;
	m_viewPointScale_1.x = 1/m_viewPointScale.x;
	m_viewPointScale_1.y = 1/m_viewPointScale.y;
	m_viewPointScale_1.z = 1/m_viewPointScale.z;
	m_viewPointScale2.x = m_viewPointScale.x*m_viewPointScale.x;
	m_viewPointScale2.y = m_viewPointScale.y*m_viewPointScale.y;
	m_viewPointScale2.z = m_viewPointScale.z*m_viewPointScale.z;
	ASSERT(m_viewPointScale.y<0);
	m_clipRect0.left = m_clipRect.left/m_viewPointScale.x;
	m_clipRect0.top = m_clipRect.bottom/m_viewPointScale.y;
	m_clipRect0.right = m_clipRect.right/m_viewPointScale.x;
	m_clipRect0.bottom = m_clipRect.top/m_viewPointScale.y;
	m_viewPointInvMx.MultF(CFVector3(m_clipRect0.left,
										m_clipRect0.top,-1),m_clipRays[0]);
	m_viewPointInvMx.MultF(CFVector3(m_clipRect0.right,
										m_clipRect0.top,-1),m_clipRays[1]);
	m_viewPointInvMx.MultF(CFVector3(m_clipRect0.right,
										m_clipRect0.bottom,-1),m_clipRays[2]);
	m_viewPointInvMx.MultF(CFVector3(m_clipRect0.left,
										m_clipRect0.bottom,-1),m_clipRays[3]);
	m_clipRect0N.left = -1/sqrt(1+m_clipRect0.left*m_clipRect0.left);
	m_clipRect0N.top = -1/sqrt(1+m_clipRect0.bottom*m_clipRect0.bottom);
	m_clipRect0N.right = 1/sqrt(1+m_clipRect0.right*m_clipRect0.right);
	m_clipRect0N.bottom = 1/sqrt(1+m_clipRect0.top*m_clipRect0.top);
	(m_viewPointDirSMx = viewPointDirMx).ScaleL(scale);
	#if 0 //_USE_GLIDE
		m_viewPointDirSMx.TranslateL(CFVector3(_gr_nScreenOriginX,_gr_nScreenOriginY,0));
	#endif

	#if _USE_OPENGL
		//static bool b = FALSE;
		//if(!b) {
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glFrustum(m_clipRect0.left,m_clipRect0.right,
				m_clipRect0.top,m_clipRect0.bottom,m_fFrontClip,m_fBackClip);
			//b = TRUE;
		//}
		//glMultMatrix(m_viewPointDirMx);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrix(m_viewPointDirMx);
	#endif
}
// -------------------------------------------------------------------------
void CViewObject::SetOrderViewPointToGlobal()
{
	#if _VIEW_ORTHO
		if( ! m_bViewOrtho )
	#endif
		CViewOrder::SetOrderViewPoint(CFVector3(m_viewPointInvMx.m[0][3],
					m_viewPointInvMx.m[1][3],m_viewPointInvMx.m[2][3]));
	#if _VIEW_ORTHO
	else
		CViewOrder::SetOrderViewPoint(CFVector3(m_viewPointInvMx.m[0][2],
					m_viewPointInvMx.m[1][2],m_viewPointInvMx.m[2][2])*
													m_viewPointScale.z);
	#endif
}
// -------------------------------------------------------------------------
bool	CViewObject::IsVisible(TCSFVector3 &centerW,double fRadius)
{
	CFVector3	center;
	center.z = CViewObject::m_viewPointDirMx.MultRow(2,centerW);
	if( center.z>=fRadius )	return FALSE;
	{
		double	d0;
		center.x = CViewObject::m_viewPointDirMx.MultRow(0,centerW);
		d0 = center.x+CViewObject::m_clipRect0.right*center.z;
		if( d0 > 0 && d0*CViewObject::m_clipRect0N.right > fRadius )
																return FALSE;
		d0 = center.x+CViewObject::m_clipRect0.left*center.z;
		if( d0 < 0 && d0*CViewObject::m_clipRect0N.left > fRadius )
																return FALSE;
		center.y = CViewObject::m_viewPointDirMx.MultRow(1,centerW);
		d0 = center.y+CViewObject::m_clipRect0.bottom*center.z;
		if( d0 > 0 && d0*CViewObject::m_clipRect0N.bottom > fRadius )
																return FALSE;
		d0 = center.y+CViewObject::m_clipRect0.top*center.z;
		if( d0 < 0 && d0*CViewObject::m_clipRect0N.top > fRadius )
																return FALSE;
	}
	return	TRUE;
}
// -------------------------------------------------------------------------
void CViewObject::SetLight(int i,int color,TCSFVector3 &pos,double radius,int brightness)
{
	m_aGlobalLights[i] = pos;
	D2INT((int*)&m_aGlobalLightsRect[i].left,(pos.x-radius)*m_terrainScale_1-0.5);
	D2INT((int*)&m_aGlobalLightsRect[i].top,(-pos.z-radius)*m_terrainScale_1-0.5);
	D2INT((int*)&m_aGlobalLightsRect[i].right,(pos.x+radius)*m_terrainScale_1+0.5);
	D2INT((int*)&m_aGlobalLightsRect[i].bottom,(-pos.z+radius)*m_terrainScale_1+0.5);
	_gr_pLights[i].r = radius;
	_gr_pLights[i].power0 = brightness;
	_gr_pLights[i].color = color;
}
// -------------------------------------------------------------------------
void CViewObject::BeginDraw()
{
	SetOrderViewPointToGlobal();
	SetLightsToGlobal();
	CViewOrder::SetGlobalOrder(TRUE);
	for( dword i = 0, dwLights = m_dwLights ; dwLights ; i++, dwLights >>= 1 )
		if( dwLights & 1 ) _gr_pLights[i].Set(CViewObject::m_viewPointDirMx*m_aGlobalLights[i]);
}
// -------------------------------------------------------------------------
void CViewObject::SetClipRect(TCCRect2 &rect)
{
	m_clipRect2.left = (m_clipRect.left = rect.left)*rect.left;
	m_clipRect2.right = (m_clipRect.right = rect.right)*rect.right;
	m_clipRect2.top = (m_clipRect.top = rect.top)*rect.top;
	m_clipRect2.bottom = (m_clipRect.bottom = rect.bottom)*rect.bottom;
}
void CViewObject::SetClipPlanes(double fFront,double fBack)
{
	//ASSERT( fFront >= 1. );
	m_fFrontClip = fFront;
	m_f1FrontClip = 1./fFront;
	//m_nFrontClip = fFront*fIntegerPrec;
	m_fBackClip = fBack;
	m_f1BackClip = 1./fBack;
	//m_nBackClip = fBack*fIntegerPrec;
	m_z1MinShift = Round(floor(log(fFront)/log(2)));
	m_fz1MinScale = 1<<(m_z1MinShift+16);
	_gr_fFrontClip = fFront;
}
#if 1
bool CViewObject::Create3DPoly(int nVertices,TCSFVector3 *pVertices3D,TCdword *pGouraud,TCSFVector2 *pTexture)
{
	ASSERT(nVertices<=GR_MAX_VERTEX-2);
	static	SFVector3	v[GR_MAX_VERTEX-2];
	static	int			nClip[GR_MAX_VERTEX-2];
	for( int i = 0 ; i < nVertices ; i++ ) {
		m_viewPointDirSMx.Mult(pVertices3D[i],v[i]);
 		if( v[i].z <= m_fFrontClip ) nClip[i] = 1; else
		if(	v[i].z >= m_fBackClip ) nClip[i] = 2; else
		nClip[i] = 0;
	}
	_gr_polygon.dwAddType = 0;
	GRSetZPrecision(0);
	int ngr = 0;
	for( int i = 0 ; i < nVertices ; i++ ) {
		if( v[i].z >= CViewFigure::HazeMin() ) _gr_polygon.dwAddType = GR_POLY_ADD_HAZE;
		if( nClip[i] ) {
			int iOth;
			double fClipD = nClip[i] != 2 ? m_fFrontClip : m_fBackClip;
			if( nClip[iOth=i>0?i-1:3] != nClip[i] ) {
				double	t = (fClipD-v[i].z)/AbsNoLess(v[iOth].z-v[i].z,1e-5);
				double	_1d = 1./fClipD;
				D2INT(&_gr_vertices[ngr].any.x,(v[iOth].x+(v[iOth].x-v[i].x)*t) * _1d);
				D2INT(&_gr_vertices[ngr].any.y,(v[iOth].y+(v[iOth].y-v[i].y)*t) * _1d);
				if( pTexture ) {
					D2INT(&_gr_vertices[ngr].texture.u,(pTexture[iOth].x-pTexture[i].x)*t);
					_gr_vertices[ngr].texture.u += pTexture[iOth].x;
					D2INT(&_gr_vertices[ngr].texture.v,(pTexture[iOth].y-pTexture[i].y)*t);
					_gr_vertices[ngr].texture.v += pTexture[iOth].y;
				}
				if( pGouraud )
					D2INT(&_gr_vertices[ngr].gouraud.b,pGouraud[iOth]+int(pGouraud[iOth]-pGouraud[i])*t);
				D2INT(&_gr_vertices[ngr++].any.iz,65536*_1d);
			}
			if( nClip[iOth=i<nVertices?i+1:0] != nClip[i] ) {
				double	t = (fClipD-v[i].z)/AbsNoLess(v[iOth].z-v[i].z,1e-5);
				double	_1d = 1./fClipD;
				D2INT(&_gr_vertices[ngr].any.x,(v[iOth].x+(v[iOth].x-v[i].x)*t) * _1d);
				D2INT(&_gr_vertices[ngr].any.y,(v[iOth].y+(v[iOth].y-v[i].y)*t) * _1d);
				if( pTexture ) {
					D2INT(&_gr_vertices[ngr].texture.u,(pTexture[iOth].x-pTexture[i].x)*t);
					_gr_vertices[ngr].texture.u += pTexture[iOth].x;
					D2INT(&_gr_vertices[ngr].texture.v,(pTexture[iOth].y-pTexture[i].y)*t);
					_gr_vertices[ngr].texture.v += pTexture[iOth].y;
				}
				if( pGouraud )
					D2INT(&_gr_vertices[ngr].gouraud.b,pGouraud[iOth]+int(pGouraud[iOth]-pGouraud[i])*t);
				D2INT(&_gr_vertices[ngr++].any.iz,65536*_1d);
			}
		} else {
			double _1z = 1./v[i].z;
			D2INT(&_gr_vertices[ngr].any.x,v[i].x*_1z);
			D2INT(&_gr_vertices[ngr].any.y,v[i].y*_1z);
			if( pTexture ) {
				_gr_vertices[ngr].texture.u = pTexture[i].x;
				_gr_vertices[ngr].texture.v = pTexture[i].y;
			}
			if( pGouraud )
				_gr_vertices[ngr].gouraud.b = pGouraud[i];
			D2INT(&_gr_vertices[ngr++].any.iz,65536*_1z);
		}
	}
	_gr_polygon.nVertices = ngr;
	return ngr >= 3;
}
#endif
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
CViewObjectRef *CViewObjectRef::m_pCurrRef = NULL;
CFMatrix3x4   *CViewObjectRef::m_pCurrRefDir = NULL;
void CViewObjectRef::Attach(CViewObjectModel *pBase)
{
	m_bInvUpdated = TRUE; m_bCenterUpdated = FALSE;
	m_pBase = pBase;
	m_dir.LoadIdentity(); m_inv3 = CFVector3(0,0,0);
	m_pCurrentInvMx = CViewObject::m_currentInvMx+MAKEBOOL(pBase->IsDynamic());
	m_pLights = &CViewObject::m_aModelLights[0+MAKEBOOL(pBase->IsDynamic())*LIGHT_SOURCE_COUNT];
	m_AnimationCallback = NULL;
	m_pUserAttrib = NULL;
	m_pBFDefs = 0; m_nBFDefs = NULL;
}
// -------------------------------------------------------------------------
bool CViewObjectRef::Bump(SBumpDef0 &def) {
	ASSERT(def.eType==SBumpDef0::T_BUMPDEF);
	ASSERT(m_pBase);
	UpdateInv();
	m_pCurrRef = this;
	if( m_pBase->IsIdentity() )	{
		CFVector3	start = def.start;
		def.start += m_inv3;
		bool bRes = MAKEBOOL(m_pBase->Bump(def)|BumpFigures(def));
		def.start = start;
		if( bRes )	{
			((SBumpDef*)&def)->pBumpRef = this;
			((SBumpDef*)&def)->nBumpFlags = BF_BUMPSTATIC;
		}
		return bRes;
	}
	SBumpDef0	def1;
	def1.fTime = def.fTime;
	def1.fRadius = def.fRadius;
	def1.start.x = m_dir.m[0][0]*def.start.x+m_dir.m[1][0]*def.start.y+
										m_dir.m[2][0]*def.start.z+m_inv3.x;
	def1.start.y = m_dir.m[0][1]*def.start.x+m_dir.m[1][1]*def.start.y+
										m_dir.m[2][1]*def.start.z+m_inv3.y;
	def1.start.z = m_dir.m[0][2]*def.start.x+m_dir.m[1][2]*def.start.y+
										m_dir.m[2][2]*def.start.z+m_inv3.z;
	def1.vel.x = m_dir.m[0][0]*def.vel.x+m_dir.m[1][0]*def.vel.y+
													m_dir.m[2][0]*def.vel.z;
	def1.vel.y = m_dir.m[0][1]*def.vel.x+m_dir.m[1][1]*def.vel.y+
													m_dir.m[2][1]*def.vel.z;
	def1.vel.z = m_dir.m[0][2]*def.vel.x+m_dir.m[1][2]*def.vel.y+
													m_dir.m[2][2]*def.vel.z;
	def1.dwFlags1 = def.dwFlags1;
	if( m_pBase->Bump(def1) | BumpFigures(def) ) {
		def.fTime = def1.fTime;
		((SBumpDef*)&def)->pBumpRef = this;
		((SBumpDef*)&def)->nBumpFlags = BF_BUMPSTATIC;
		m_dir.MultF(def1.vel1,def.vel1);
		return TRUE;
	}
	return FALSE;
}
// -------------------------------------------------------------------------
void CViewObjectRef::Draw()
{
	UpdateCenter();
	// Visibility check
	CFVector3	center;

	switch( CViewObject::m_eWaterView ) {
		case WATER_VIEW_ABOVE:
			if( m_center.y >= CViewFigure::m_fWaterline ) {
				if( m_center.y-m_pBase->Radius() >= CViewFigure::m_fWaterline )
					CViewFigure::m_eWaterSplit = WATER_SPLIT_NONE;
				else
					CViewFigure::m_eWaterSplit = WATER_SPLIT_UP;
			}
			else {
				if( m_center.y+m_pBase->Radius() < CViewFigure::m_fWaterline )
					goto Invisible;
				else
					CViewFigure::m_eWaterSplit = WATER_SPLIT_UP;
			} break;
		case WATER_VIEW_BELOW:
			if( m_center.y < CViewFigure::m_fWaterline ) {
				if( m_center.y+m_pBase->Radius() < CViewFigure::m_fWaterline )
					CViewFigure::m_eWaterSplit = WATER_SPLIT_NONE;
				else
					CViewFigure::m_eWaterSplit = WATER_SPLIT_DOWN;
			}
			else {
				if( m_center.y-m_pBase->Radius() >= CViewFigure::m_fWaterline )
					goto Invisible;
				else
					CViewFigure::m_eWaterSplit = WATER_SPLIT_DOWN;
			} break;
		default:
			CViewFigure::m_eWaterSplit = WATER_SPLIT_NONE;
	}


	//CViewObject::m_viewPointDirSMx.Mult(m_center,center);
	center.z = CViewObject::m_viewPointDirMx.m[2][0]*m_center.x+
				CViewObject::m_viewPointDirMx.m[2][1]*m_center.y+
				CViewObject::m_viewPointDirMx.m[2][2]*m_center.z+
				CViewObject::m_viewPointDirMx.m[2][3];
	if( center.z>=m_pBase->Radius() )	{
	Invisible:
		m_dynamicList.ZSortDrawNDrop();
		m_afterList.ZSortDrawNDrop();
		return;
	}
	#if _VIEW_ORTHO
		if( !CViewObject::m_bViewOrtho )
	#endif
	{
		double	d0;
		center.x = CViewObject::m_viewPointDirMx.m[0][0]*m_center.x+
					CViewObject::m_viewPointDirMx.m[0][1]*m_center.y+
					CViewObject::m_viewPointDirMx.m[0][2]*m_center.z+
					CViewObject::m_viewPointDirMx.m[0][3];
		d0 = center.x+CViewObject::m_clipRect0.right*center.z;
		if( d0 > 0 && d0*CViewObject::m_clipRect0N.right > m_pBase->Radius() ) goto Invisible;
		d0 = center.x+CViewObject::m_clipRect0.left*center.z;
		if( d0 < 0 && d0*CViewObject::m_clipRect0N.left > m_pBase->Radius() ) goto Invisible;
		center.y = CViewObject::m_viewPointDirMx.m[1][0]*m_center.x+
					CViewObject::m_viewPointDirMx.m[1][1]*m_center.y+
					CViewObject::m_viewPointDirMx.m[1][2]*m_center.z+
					CViewObject::m_viewPointDirMx.m[1][3];
		d0 = center.y+CViewObject::m_clipRect0.bottom*center.z;
		if( d0 > 0 && d0*CViewObject::m_clipRect0N.bottom > m_pBase->Radius() ) goto Invisible;
		d0 = center.y+CViewObject::m_clipRect0.top*center.z;
		if( d0 < 0 && d0*CViewObject::m_clipRect0N.top > m_pBase->Radius() ) goto Invisible;
	}
	// If we come here, then visible

	CViewObject::m_pCurrentInvMx = m_pCurrentInvMx;

	CFMatrix3x4 dir; CFVector3 offset;
	if( m_pBase->m_bIdentity ) {
		//dir = m_viewPointDirMx;
		//dir.TranslateR(
		//		CFVector3(m_dir.m[0][3],m_dir.m[1][3],m_dir.m[2][3]));
		dir.m[0][0] = CViewObject::m_viewPointDirMx.m[0][0];
		dir.m[0][1] = CViewObject::m_viewPointDirMx.m[0][1];
		dir.m[0][2] = CViewObject::m_viewPointDirMx.m[0][2];
		dir.m[0][3] = CViewObject::m_viewPointDirMx.m[0][3]+
						dir.m[0][0]*m_dir.m[0][3]+dir.m[0][1]*m_dir.m[1][3]+
													dir.m[0][2]*m_dir.m[2][3];
		dir.m[1][0] = CViewObject::m_viewPointDirMx.m[1][0];
		dir.m[1][1] = CViewObject::m_viewPointDirMx.m[1][1];
		dir.m[1][2] = CViewObject::m_viewPointDirMx.m[1][2];
		dir.m[1][3] = CViewObject::m_viewPointDirMx.m[1][3]+
						dir.m[1][0]*m_dir.m[0][3]+dir.m[1][1]*m_dir.m[1][3]+
													dir.m[1][2]*m_dir.m[2][3];
		dir.m[2][0] = CViewObject::m_viewPointDirMx.m[2][0];
		dir.m[2][1] = CViewObject::m_viewPointDirMx.m[2][1];
		dir.m[2][2] = CViewObject::m_viewPointDirMx.m[2][2];
		dir.m[2][3] = CViewObject::m_viewPointDirMx.m[2][3]+
						dir.m[2][0]*m_dir.m[0][3]+dir.m[2][1]*m_dir.m[1][3]+
													dir.m[2][2]*m_dir.m[2][3];
	} else
		dir.LoadMult(CViewObject::m_viewPointDirMx,m_dir);

	#if _USE_OPENGL
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		//glLoadIdentity();
		glMultMatrix(m_dir);

		//glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();
		//glFrustum(-0.8,0.8,-0.5,0.5,1,200);
	#endif

	UpdateInv();
	if( m_pBase->m_bIdentity ) {
		*m_pCurrentInvMx = CViewObject::m_viewPointInvMx;
		m_pCurrentInvMx->TranslateL(m_inv3);
	}
	else {
		double	(*od)[4] = m_dir.m, (*pi)[4] = CViewObject::m_viewPointInvMx.m;
		double	(*inv)[4] = m_pCurrentInvMx->m;
		CFVector3	&i = m_inv3;
		inv[0][0] = od[0][0]*pi[0][0]+od[1][0]*pi[1][0]+od[2][0]*pi[2][0];
		inv[0][1] = od[0][0]*pi[0][1]+od[1][0]*pi[1][1]+od[2][0]*pi[2][1];
		inv[0][2] = od[0][0]*pi[0][2]+od[1][0]*pi[1][2]+od[2][0]*pi[2][2];
		inv[0][3] = od[0][0]*pi[0][3]+od[1][0]*pi[1][3]+od[2][0]*pi[2][3]+i.x;
		inv[1][0] = od[0][1]*pi[0][0]+od[1][1]*pi[1][0]+od[2][1]*pi[2][0];
		inv[1][1] = od[0][1]*pi[0][1]+od[1][1]*pi[1][1]+od[2][1]*pi[2][1];
		inv[1][2] = od[0][1]*pi[0][2]+od[1][1]*pi[1][2]+od[2][1]*pi[2][2];
		inv[1][3] = od[0][1]*pi[0][3]+od[1][1]*pi[1][3]+od[2][1]*pi[2][3]+i.y;
		inv[2][0] = od[0][2]*pi[0][0]+od[1][2]*pi[1][0]+od[2][2]*pi[2][0];
		inv[2][1] = od[0][2]*pi[0][1]+od[1][2]*pi[1][1]+od[2][2]*pi[2][1];
		inv[2][2] = od[0][2]*pi[0][2]+od[1][2]*pi[1][2]+od[2][2]*pi[2][2];
		inv[2][3] = od[0][2]*pi[0][3]+od[1][2]*pi[1][3]+od[2][2]*pi[2][3]+i.z;
	}
	m_pCurrentInvMx->ScaleR(CViewObject::m_viewPointScale_1);
	#if	_VIEW_ORTHO
		if( ! CViewObject::m_bViewOrtho )
	#endif
	{
		CViewObject::m_currObjViewPoint.x = m_pCurrentInvMx->m[0][3];
		CViewObject::m_currObjViewPoint.y = m_pCurrentInvMx->m[1][3];
		CViewObject::m_currObjViewPoint.z = m_pCurrentInvMx->m[2][3];
		// Coordinate center image
	}
	#if _VIEW_ORTHO
	else {
		CViewObject::m_currObjViewPoint.x = m_pCurrentInvMx->m[0][2];
		CViewObject::m_currObjViewPoint.y = m_pCurrentInvMx->m[1][2];
		CViewObject::m_currObjViewPoint.z = m_pCurrentInvMx->m[2][2];
		// Oz direction image
	}
	#endif

	dword	dwLights = m_dwLights;
	if( ! m_pBase->m_bIdentity ) {
		if( m_dynamicList.First() || m_dwLights ) {
			CFMatrix3x4	invMx;
			invMx.LoadTransposed(m_dir);
			if( m_dynamicList.First() )	m_dynamicList.TransformInv(invMx);
			for( dword i = 0 ; m_dwLights ; i++, m_dwLights >>= 1 )
				if( m_dwLights & 1 ) m_pLights[i] = invMx*CViewObject::m_aGlobalLights[i];
		}
		ASSERT(!m_shotList.First());
	}
	else {
		if( m_dynamicList.First() )
			m_dynamicList.TransformInv(m_inv3);
		for( dword i = 0 ; m_dwLights ; i++, m_dwLights >>= 1 )
			if( m_dwLights & 1 ) m_pLights[i] = CViewObject::m_aGlobalLights[i]+m_inv3;
		ASSERT(!m_shotList.First());
	}

	//dir.ScaleL(CViewObject::m_viewPointScaleInt);
	#if 1
	CFMatrix3x4Y	dirs;
	dirs.m[0][0] = CViewObject::m_viewPointScale.x*dir.m[0][0];
	dirs.m[0][1] = CViewObject::m_viewPointScale.x*dir.m[0][1];
	dirs.m[0][2] = CViewObject::m_viewPointScale.x*dir.m[0][2];
	dirs.m[0][3] = CViewObject::m_viewPointScale.x*dir.m[0][3];
	dirs.m[1][0] = CViewObject::m_viewPointScale.y*dir.m[1][0];
	dirs.m[1][1] = CViewObject::m_viewPointScale.y*dir.m[1][1];
	dirs.m[1][2] = CViewObject::m_viewPointScale.y*dir.m[1][2];
	dirs.m[1][3] = CViewObject::m_viewPointScale.y*dir.m[1][3];
	dirs.m[2][0] = CViewObject::m_viewPointScale.z*dir.m[2][0];
	dirs.m[2][1] = CViewObject::m_viewPointScale.z*dir.m[2][1];
	dirs.m[2][2] = CViewObject::m_viewPointScale.z*dir.m[2][2];
	dirs.m[2][3] = CViewObject::m_viewPointScale.z*dir.m[2][3];
	dirs.YFrom(m_dir);
	//dirs.YScale(fIntegerPrec);
	#endif
	//dir.YFrom(m_dir);

	CViewOrder::SetOrderViewPoint(CViewObject::m_currObjViewPoint);
	TCSFVector3	*oldLights = CViewOrder::GetLights();
	CViewOrder::SetLights(m_pLights);


	m_pCurrRef = this;
	m_pCurrRefDir = &dir;


	#ifdef	_DEBUG
		static	dword s_dwFlags = 0;
		dword	dwOldFlags = s_dwFlags;
		dword	dwFlags = m_pBase->IsDynamic() ? 2 : 1;
		ASSERT( !(s_dwFlags & dwFlags) );
		s_dwFlags |= dwFlags*2-1;
	#endif
	m_pBase->Draw(dirs,&m_dynamicList,dwLights);
	#ifdef _DEBUG
		s_dwFlags = dwOldFlags;
	#endif

	CViewOrder::SetLights(oldLights);

	#if _USE_OPENGL
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	#endif

	CViewObject::SetOrderViewPointToGlobal();
	m_afterList.ZSortDrawNDrop();
	ASSERT(!m_shotList.First());
}
// -------------------------------------------------------------------------
void	CViewObjectRef::DoUpdateCenter()
{
	m_dir.Mult(m_pBase->m_center,m_center);
	m_bCenterUpdated = TRUE;
}
void	CViewObjectRef::UpdateInv()
{
	if( m_bInvUpdated )	return;
	if( m_pBase->m_bIdentity ) {
		m_inv3.x = -m_dir.m[0][3];
		m_inv3.y = -m_dir.m[1][3];
		m_inv3.z = -m_dir.m[2][3];
		m_bInvUpdated = TRUE;
		return;
	}
	m_inv3.x = -(m_dir.m[0][0]*m_dir.m[0][3]+m_dir.m[1][0]*m_dir.m[1][3]+
											m_dir.m[2][0]*m_dir.m[2][3]);
	m_inv3.y = -(m_dir.m[0][1]*m_dir.m[0][3]+m_dir.m[1][1]*m_dir.m[1][3]+
											m_dir.m[2][1]*m_dir.m[2][3]);
	m_inv3.z = -(m_dir.m[0][2]*m_dir.m[0][3]+m_dir.m[1][2]*m_dir.m[1][3]+
											m_dir.m[2][2]*m_dir.m[2][3]);
	m_bInvUpdated = TRUE;
}
// -------------------------------------------------------------------------
void CViewObjectRef::ClaimModelUsage()
{
	if(m_pBase->m_pMatrix0) {
		if(m_pBase->m_bIdentity) {
			double	fDiff = 0;
			for( int  i = 0 ; i < 3 ; i++ )
			for( int  j = 0 ; j < 3 ; j++ ) {
				double d0 = m_dir.m[i][j]-m_pBase->m_pMatrix0->m[i][j];
				fDiff += d0*d0;
			}
			if( fDiff > 1e-4 )	m_pBase->m_bIdentity = FALSE;
		}
	}
	else {
		m_pBase->m_pMatrix0 = &m_dir;
		m_pBase->m_bIdentity = !m_pBase->HasBushes(); //TRUE;
	}
}
// -------------------------------------------------------------------------
void CViewObjectRef::ApplyModelUsage()
{
	if( m_pBase->m_bIdentity ) {
		if( m_pBase->m_pMatrix0 ) {
			*m_pBase %= *m_pBase->m_pMatrix0;
			m_pBase->m_pMatrix0 = NULL;
		}
		m_dir.LoadIdentityKernel();
	}
}
// -------------------------------------------------------------------------
void CViewObjectRef::ComputeMaxHeight()
{
	m_fMaxHeight = m_pBase->Height();
}
double CViewObjectRef::ComputeBSPError()
{
	return 1e10;
}
double CViewObjectRef::ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d)
{
	CFMatrix3x4	inv;
	inv.LoadTransposed(m_dir);
	return m_pBase->ComputeBSPError(inv*pt,inv%n,d);
}
// -------------------------------------------------------------------------
void	CViewObjectRef::AllocBumpFigures()
{
	ASSERT( m_pBase && m_pBase->NumBaseSets() == 1 && !m_pBFDefs);
	m_nBFDefs = m_pBase->BaseSet(0).Base(0).BumpFiguresCount();
	m_pBFDefs = m_nBFDefs > 0 ? new SBumpFiguresRef[m_nBFDefs] : NULL;
	for( int i = 0 ; i < m_nBFDefs ; i++ )	{
		m_pBFDefs[i].bActive = FALSE;
		m_pBFDefs[i].pBase = &m_pBase->BaseSet(0).Base(0).BumpFigures(i);
	}
}
// -------------------------------------------------------------------------


