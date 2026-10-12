#ifndef	__VIEW_CPP__
#include"_view.h"
#endif
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
bool bOrderOptimize = TRUE;
bool bDumpNormals = FALSE;
// ------------------------------------------------------------------------
void CViewOrdered::RemoveDynamics()
{
	//m_shotList.Clear();
	ASSERT(!m_shotList.First());
	m_dynamicList.Clear();
	m_afterList.Clear();
}
// -------------------------------------------------------------------------
void CViewOrdered::PromoteDynamic()
{
	//ASSERT(0);
	if(!m_dynamicList.First()&&!m_shotList.First()) {
		m_bDynamics = FALSE;
		return;
	}
	for(CViewDynamic *p = m_dynamicList.First() ; p ; p = p->Next() ) {
		ASSERT(p->IsBumpDynamic());
		((CViewBumpDynamic*)p)->SetPromoteNode(this);
	}
	m_bDynamics = TRUE;
}
// ------------------------------------------------------------------------
CViewOrdered *CViewOrdered::m_pCurrentTopOrdered = NULL;
CViewOrdered::CViewOrdered()
{
	m_bEmbedded = TRUE; m_bDynamics = FALSE;
	if(!m_pCurrentTopOrdered) m_pCurrentTopOrdered = this;
	m_pTopOrder = m_pCurrentTopOrdered;
	m_dwLights = 0;
}
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
CFVector3	CViewOrder::m_currOrdViewPoint;
bool		CViewOrder::m_bIsGlobalOrder = TRUE;
TCSFVector3	*CViewOrder::m_pLights = NULL;
// ------------------------------------------------------------------------
CViewOrder::~CViewOrder()
{
	//for( int  i = 0 ; i < m_nOrdered ; i++ )
	m_apOrdered[0]->Delete();
	/*if(!m_bSingle)*/	m_apOrdered[1]->Delete();
	//delete [] m_ppOrdered;
	//delete [] m_pNormals;
}
// ------------------------------------------------------------------------
void CViewOrder::SetPlane(TCCFVector3 &pt,TCCFVector3 &n,double d)
{
	m_normal.pt=pt; m_normal.n=n; m_normal.d=d;
	m_normal.npt = m_normal.n*m_normal.pt;
	m_normal.ntype = DefineNormalType(m_normal.n);
}
// ------------------------------------------------------------------------
#ifdef _DEBUG
void CViewOrder::CheckNoDynamics()
{
	//ASSERT(!m_bDynamics);
	ASSERT(!m_shotList.First());
	ASSERT(!m_dynamicList.First());
	ASSERT(!m_afterList.First());
	//for( int  i = 0 ; i < m_nOrdered ; i++ )
	m_apOrdered[0]->CheckNoDynamics();
	/*if(!m_bSingle)*/	m_apOrdered[1]->CheckNoDynamics();
}
void CViewOrdered::CheckNoDynamics()
{
	//ASSERT(!m_bDynamics);
	ASSERT(!m_shotList.First());
	ASSERT(!m_dynamicList.First());
	ASSERT(!m_afterList.First());
}
#endif
// ------------------------------------------------------------------------
double CViewOrder::ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d)
{
	return Min(m_apOrdered[0]->ComputeBSPError(pt,n,d),
				m_apOrdered[1]->ComputeBSPError(pt,n,d));
}
double CViewOrder::ComputeBSPError()
{
	double fErr = Min(Min(m_apOrdered[0]->ComputeBSPError(m_normal.pt,m_normal.n,m_normal.d),
					m_apOrdered[1]->ComputeBSPError(m_normal.pt,-m_normal.n,m_normal.d)),
				Min(m_apOrdered[0]->ComputeBSPError(),
					m_apOrdered[1]->ComputeBSPError()));
	if( fErr < -MAX_BSP_ERROR ) {
		WRITELOG4("\nBSP ERROR %lg: p=(%lg,%lg,%lg) n=(%lg,%lg,%lg) d=%lg",fErr,m_normal.pt,m_normal.n,m_normal.d);
	}
	return fErr;
}
void CViewOrder::RemoveDynamics()
{
	if( !m_bDynamics ) {
		#ifdef _DEBUG
			CheckNoDynamics();
		#endif
		return;
	}
	//m_shotList.Clear();
	ASSERT(!m_shotList.First());
	m_dynamicList.Clear();
	m_afterList.Clear();
	//for( int  i = 0 ; i < m_nOrdered ; i++ )
	//ASSERT(!m_bSingle);
	m_apOrdered[0]->RemoveDynamics();
	/*if(!m_bSingle)*/	m_apOrdered[1]->RemoveDynamics();
}
// ------------------------------------------------------------------------
inline void CViewOrder::DrawDrainLinear(CViewLinearDynamic *pd,
															bool b0,bool b1)
		// b0 = m_ppOrdered[0] is drawn
		// b1 = m_ppOrdered[1] is drawn
{
	double	d1 = ScalarProdNormal(m_normal.ntype,
					m_normal.n,pd->m_dynEnd1)-m_normal.npt,
	d2 = ScalarProdNormal(m_normal.ntype,
					m_normal.n,pd->m_dynBase1)-m_normal.npt;
	if( d1 >= 0 ) {
		if( d2 < -0.001 ) {
			if( d2 < -d1 ) {
				if( b1 ) {
					double t = d1/(d1-d2)*(pd->m_fDrawT2-pd->m_fDrawT1);
					pd->m_dynEnd1 += pd->m_bump.vel*t;
					pd->m_fDrawT1 += t;
					m_apOrdered[1]->m_dynamicList.Move(pd,m_dynamicList);
				} else m_dynamicList.Remove(pd);
			} else {
				if( b0 ) {
					double t = d2/(d2-d1)*(pd->m_fDrawT2-pd->m_fDrawT1);
					pd->m_dynBase1 -= pd->m_bump.vel*t;
					pd->m_fDrawT2 -= t;
					m_apOrdered[0]->m_dynamicList.Move(pd,m_dynamicList);
				} else m_dynamicList.Remove(pd);
			}
		} else
			if( b0 )	m_apOrdered[0]->m_dynamicList.Move(pd,m_dynamicList);
			else		m_dynamicList.Remove(pd);
	} else { // d1 < 0
		if( d2 > 0.001 ) {
			if( d2 > -d1 ) {
				if( b0 ) {
					double t = d1/(d1-d2)*(pd->m_fDrawT2-pd->m_fDrawT1);
					pd->m_dynEnd1 += pd->m_bump.vel*t;
					pd->m_fDrawT1 += t;
					m_apOrdered[0]->m_dynamicList.Move(pd,m_dynamicList);
				} else m_dynamicList.Remove(pd);
			} else {
				if( b1 ) {
					double t = d2/(d2-d1)*(pd->m_fDrawT2-pd->m_fDrawT1);
					pd->m_dynBase1 -= pd->m_bump.vel*t;
					pd->m_fDrawT2 -= t;
					m_apOrdered[1]->m_dynamicList.Move(pd,m_dynamicList);
				} else m_dynamicList.Remove(pd);
			}
		} else
			if( b1 ) m_apOrdered[1]->m_dynamicList.Move(pd,m_dynamicList);
			else	 m_dynamicList.Remove(pd);
	}
}
// ------------------------------------------------------------------------
inline void	CViewOrder::DrawDrainSpheric(CViewSphericDynamic *pd,
		double fOrdDiff,int n0,int n1,bool bn0,bool bn1,bool b0,bool b1,
											CViewDynamicList &beforeList)
		// m_ppOrdered[n0] - firstly displayed (if bn0)
		// m_ppOrdered[n1] - secondly displayed (if bn1)
		// b0 = m_ppOrdered[0] is drawn
		// b1 = m_ppOrdered[1] is drawn
		// (bn0 = b[n0], bn1 = b[n1])
		// Assuming at least one of b0 and b1 is true
{
	double fOrd = m_normal.npt-ScalarProdNormal(m_normal.ntype,
									m_normal.n,pd->m_dynBase1);
	double fRDiff = m_normal.d-pd->m_bump.fRadius;
	if( fabs(fOrd) <= fRDiff ) { // Whole sphere inside plane
		if(fabs(fOrdDiff)<=m_normal.d) { // view point inside plane
			if(bn1)	m_apOrdered[n1]->m_afterList.Move(pd,m_dynamicList);
			else m_apOrdered[n0]->m_afterList.Move(pd,m_dynamicList);
		} else { // view point outside plane
			if(bn0)	m_apOrdered[n0]->m_afterList.Move(pd,m_dynamicList);
			else m_apOrdered[n1]->m_afterList.Move(pd,m_dynamicList);
		}
	} else
	if( fabs(fOrd) >= -fRDiff ) { // One of clusters is not crossed by sphere
		// Notice. If cluster is not drawn (optimized), it is drawn first
		// (unoptimized). See CViewOrder::Draw()
		if( fOrd>0 ) {	// [0] is not crossed
			if(b1) m_apOrdered[1]->m_dynamicList.Move(pd,m_dynamicList);
			else if(fOrd>pd->m_bump.fRadius) m_dynamicList.Remove(pd);
			else (fOrdDiff>-m_normal.d?&m_afterList:&beforeList)->
												Move(pd,m_dynamicList);
		} else {
			if(b0) m_apOrdered[0]->m_dynamicList.Move(pd,m_dynamicList);
			else if(fOrd<-pd->m_bump.fRadius) m_dynamicList.Remove(pd);
			else (fOrdDiff<m_normal.d?&m_afterList:&beforeList)->
												Move(pd,m_dynamicList);
		}
	} else {	// Sphere crosses both clusters
		if( (pd->m_dynBase1-m_normal.pt)*
								(m_currOrdViewPoint-m_normal.pt)<0 ) {
			if(bn0) m_apOrdered[n0]->m_dynamicList.Move(pd,m_dynamicList);
			else beforeList.Move(pd,m_dynamicList);
		} else {
			if(bn1) m_apOrdered[n1]->m_dynamicList.Move(pd,m_dynamicList);
			else m_afterList.Move(pd,m_dynamicList);
		}
	}
}
// ------------------------------------------------------------------------
inline void	CViewOrder::DrawDrainDynamics(double fOrdDiff,
			int n0,int n1,bool bn0,bool bn1,bool b0,bool b1)
		// m_ppOrdered[n0] - firstly displayed (if bn0)
		// m_ppOrdered[n1] - secondly displayed (if bn1)
		// b0 = m_ppOrdered[0] is drawn
		// b1 = m_ppOrdered[1] is drawn
		// (bn0 = b[n0], bn1 = b[n1])
		// Assuming at least one of b0 and b1 is true
{
	CViewDynamicList beforeList;
	for( CViewDynamic *pd = m_dynamicList.First(), *pd1;
												pd!=NULL ; pd = pd1 ) {
		pd1 = pd->Next();
		switch(pd->m_eDType) {
			case CViewDynamic::DT_SPHERIC:
				DrawDrainSpheric((CViewSphericDynamic*)pd,fOrdDiff,
					n0,n1,bn0,bn1,b0,b1,beforeList); break;
			case CViewDynamic::DT_LINEAR:
				DrawDrainLinear((CViewLinearDynamic*)pd,b0,b1); break;
			default: ASSERT(0);
		}
	}
	if(beforeList.First()) {
		CViewRenderState	rs;
		if( !m_bIsGlobalOrder ) rs.Load();
		beforeList.SortZOrder();
		beforeList.Draw();
		if( !m_bIsGlobalOrder ) rs.Set();
	}
}
// ------------------------------------------------------------------------
void CViewOrder::Draw()
{
	//m_bIsGlobalOrder = FALSE;
	//ASSERT(!m_bSingle);
	//ASSERT(!m_afterList.First());
	SNormal	&normal = m_normal;


	// Lights promotion
	m_apOrdered[0]->m_dwLights = m_apOrdered[1]->m_dwLights = 0;
	if( m_dwLights ) {
		for( dword i = 0, dwMask = 1 ; m_dwLights ; i++, m_dwLights >>= 1, dwMask <<= 1 )
		if( m_dwLights & 1 ) {
			double d = ScalarProdNormal(normal.ntype,normal.n,m_pLights[i])-m_normal.npt;
			if( d > -_gr_pLights[i].r ) m_apOrdered[0]->m_dwLights |= dwMask;
			if( d < _gr_pLights[i].r ) m_apOrdered[1]->m_dwLights |= dwMask;
		}
	}


	// Dynamics insertion and orders sorting
	#if	_VIEW_ORTHO
		if( CViewObject::ViewOrtho() ?
				normal.n*m_currOrdViewPoint<=0 :
				normal.n*(normal.pt-m_currOrdViewPoint)<=0 ) {
			m_apOrdered[1]->Draw();
			m_apOrdered[0]->Draw();
		}
		else {
			m_apOrdered[0]->Draw();
			m_apOrdered[1]->Draw();
		}
	#else
	double fOrdDiff = normal.npt-ScalarProdNormal(
						normal.ntype,normal.n,m_currOrdViewPoint);
	//double fOrdDiff = normal.npt-normal.n*m_currOrdViewPoint;
	if( fOrdDiff <= 0 )
	{	// [1] is farther and displayed first
		double t0, rn;
		if( !m_bIsGlobalOrder ||
				(t0 = CViewFigure::HazeMax()+m_apOrdered[1]->m_fMaxHeight*
							CViewObjectBaseSet::m_fCutAngleCotangent,
				(rn=normal.n*CViewObject::m_clipRays[0])<=0) && fOrdDiff >= t0*rn ||
				(rn=normal.n*CViewObject::m_clipRays[1])<=0 && fOrdDiff >= t0*rn ||
				(rn=normal.n*CViewObject::m_clipRays[2])<=0 && fOrdDiff >= t0*rn ||
				(rn=normal.n*CViewObject::m_clipRays[3])<=0 && fOrdDiff >= t0*rn ) {
			// m_ppOrdered[1] must be drawn
			DrawDrainDynamics(fOrdDiff,1,0,TRUE,TRUE,TRUE,TRUE);
			if(/*m_bIsGlobalOrder &&*/ !m_bDynamics)	{
				m_apOrdered[0]->m_bDynamics = FALSE;
				m_apOrdered[1]->m_bDynamics = FALSE;
			}
			m_apOrdered[1]->Draw();
		} else {
			m_apOrdered[1]->RemoveDynamics();
			DrawDrainDynamics(fOrdDiff,1,0,FALSE,TRUE,TRUE,FALSE);
			ASSERT(!m_apOrdered[1]->m_dynamicList.First());
			ASSERT(!m_apOrdered[1]->m_afterList.First());
			ASSERT(!m_apOrdered[1]->m_shotList.First());
			if(!m_bDynamics)	m_apOrdered[0]->m_bDynamics = FALSE;
		}
		m_apOrdered[0]->Draw();
	}
	else { // [0] is farther and displayed first
		double t0, rn;
		if( !m_bIsGlobalOrder ||
				(t0 = CViewFigure::HazeMax()+m_apOrdered[0]->m_fMaxHeight*
							CViewObjectBaseSet::m_fCutAngleCotangent,
				(rn=normal.n*CViewObject::m_clipRays[0])>=0) && fOrdDiff <= t0*rn ||
				(rn=normal.n*CViewObject::m_clipRays[1])>=0 && fOrdDiff <= t0*rn ||
				(rn=normal.n*CViewObject::m_clipRays[2])>=0 && fOrdDiff <= t0*rn ||
				(rn=normal.n*CViewObject::m_clipRays[3])>=0 && fOrdDiff <= t0*rn ) {
			// m_ppOrdered[0] must be drawn
			DrawDrainDynamics(fOrdDiff,0,1,TRUE,TRUE,TRUE,TRUE);
			if(/*m_bIsGlobalOrder &&*/ !m_bDynamics)	{
				m_apOrdered[1]->m_bDynamics = FALSE;
				m_apOrdered[0]->m_bDynamics = FALSE;
			}
			m_apOrdered[0]->Draw();
		} else {
			m_apOrdered[0]->RemoveDynamics();
			DrawDrainDynamics(fOrdDiff,0,1,FALSE,TRUE,FALSE,TRUE);
			ASSERT(!m_apOrdered[0]->m_dynamicList.First());
			ASSERT(!m_apOrdered[0]->m_afterList.First());
			ASSERT(!m_apOrdered[0]->m_shotList.First());
			if(!m_bDynamics)	m_apOrdered[1]->m_bDynamics = FALSE;
		}
		m_apOrdered[1]->Draw();
	}
	if(m_afterList.First()) {
		CViewRenderState	rs;
		if( !m_bIsGlobalOrder )	rs.Load();
		m_afterList.SortZOrder();
		m_afterList.Draw();
		if( !m_bIsGlobalOrder )	rs.Set();
	}
	#endif
}
// ------------------------------------------------------------------------
bool CViewOrder::Bump(SBumpDef0 &def)
{
	//ASSERT(!m_bSingle);
	//if( m_bSingle )	return m_apOrdered[0]->Bump(def);

	SNormal	&normal = m_normal;
	//double	d0 = def.start*normal.n-normal.npt;
	double	d0 = ScalarProdNormal(normal.ntype,normal.n,def.start)-normal.npt;
	if( d0 < 0 ) {		// starting from [1]
		double	dd = ScalarProdNormal(normal.ntype,normal.n,def.vel);
		if( d0 < -def.fRadius )  {	// starting entirely from [1]
			//double	dd = def.vel*normal.n;
			if(	dd <= 0 || d0+def.fRadius < -dd*def.fTime )	// not reaching [0]
				return	m_apOrdered[1]->Bump(def);	//	p[0] = NULL;
		}
		return MAKEBOOL(m_apOrdered[1]->Bump(def)|m_apOrdered[0]->Bump(def));
	}
	else {	// starting from [0]
		if( d0 > def.fRadius )  {	// starting entirely from [0]
			//double	dd = def.vel*normal.n;
			double	dd = ScalarProdNormal(normal.ntype,normal.n,def.vel);
			if(	dd >= 0 || d0-def.fRadius > -dd*def.fTime )	// not reaching [0]
				return	m_apOrdered[0]->Bump(def);	//	p[0] = NULL;
		}
		return MAKEBOOL(m_apOrdered[0]->Bump(def)|m_apOrdered[1]->Bump(def));
	}
}
// ------------------------------------------------------------------------
inline void CViewOrder::PromoteLinear(CViewLinearDynamic *pd)
{
	//double	d0 = pd->m_bump.start*m_normal.n-m_normal.npt,
	//		d1 = pd->m_dynEnd1*m_normal.n-m_normal.npt;
	double	d0 = ScalarProdNormal(m_normal.ntype,m_normal.n,pd->m_bump.start)-m_normal.npt,
			d1 = ScalarProdNormal(m_normal.ntype,m_normal.n,pd->m_dynBase1)-m_normal.npt;
	//WRITELOG3("Promoting linear at %08X d0=%lg d1=%lg\n",(int)this,d0,d1);
	//WRITELOG2("StartY=%lg End1Y=%lg\n",pd->m_bump.start.y,pd->m_dynBase1.y);
	if( d0 >= 0 ) {
		if( d1 >= 0 )	{
			m_apOrdered[0]->m_shotList.Move(pd,m_shotList);
			//WRITELOG("Move 0\n");
		}
		else {
			pd->SetPromoteNode(this);
			//WRITELOG("SetProm\n");
		}
	} else {
		if( d1 <= 0 )	{
			m_apOrdered[1]->m_shotList.Move(pd,m_shotList);
			//WRITELOG("Move 1\n");
		}
		else {
			pd->SetPromoteNode(this);
			//WRITELOG("SetProm\n");
		}
	}
}
// ------------------------------------------------------------------------
inline void CViewOrder::PromoteSpheric(CViewSphericDynamic *pd,
						CViewDynamicList &list,
						CViewDynamicList &list0,CViewDynamicList &list1)
{
	//double	d0 = pd->m_bump.start*m_normal.n-m_normal.npt;
	double	d0 = ScalarProdNormal(m_normal.ntype,m_normal.n,pd->m_bump.start)-m_normal.npt;
	if( d0 >= 0 ) {
		if( d0 > pd->m_bump.fRadius ) {
			//double	dd = pd->m_bump.vel*m_normal.n;
			double	dd = ScalarProdNormal(m_normal.ntype,m_normal.n,pd->m_bump.vel);
			if( dd >= 0 || d0-pd->m_bump.fRadius >
										-dd*pd->m_bump.fTime )
				list0.Move(pd,list);
			else pd->SetPromoteNode(this);
		} else pd->SetPromoteNode(this);
	}
	else {
		if( d0 < -pd->m_bump.fRadius )  {
			//double	dd = pd->m_bump.vel*m_normal.n;
			double	dd = ScalarProdNormal(m_normal.ntype,m_normal.n,pd->m_bump.vel);
			if(	dd <= 0 || d0+pd->m_bump.fRadius <
										-dd*pd->m_bump.fTime )
				list1.Move(pd,list);
			else pd->SetPromoteNode(this);
		} else pd->SetPromoteNode(this);
	}
}
// ------------------------------------------------------------------------
inline void CViewOrder::Promote(CViewDynamicList &list,
						CViewDynamicList &list0,CViewDynamicList &list1)
{
	for( CViewDynamic *pd = list.First(), *pd1;
											pd != NULL; pd = pd1 ) {
		pd1 = pd->Next();
		switch(pd->m_eDType) {
			case CViewDynamic::DT_SPHERIC:PromoteSpheric(
							(CViewSphericDynamic*)pd,list,list0,list1); break;
			case CViewDynamic::DT_LINEAR: PromoteLinear((CViewLinearDynamic*)pd); break;
			default: ASSERT(0);
		}
	}
}
// ------------------------------------------------------------------------
void CViewOrder::PromoteDynamic()
{
//	ASSERT(0);
	if(!m_dynamicList.First() && !m_shotList.First()) {
		m_bDynamics = FALSE;
		return;
	}

	m_bDynamics = TRUE;
	ASSERT(!m_shotList.First());

	//Promote(m_shotList,m_apOrdered[0]->m_shotList,m_apOrdered[1]->m_shotList);
	Promote(m_dynamicList,m_apOrdered[0]->m_dynamicList,m_apOrdered[1]->m_dynamicList);

	m_apOrdered[0]->PromoteDynamic();
	m_apOrdered[1]->PromoteDynamic();
}
// ------------------------------------------------------------------------
void CViewOrder::DrainDynamic(int nOrdered)
{
	CViewOrdered	*po = m_apOrdered[nOrdered];
	po->DrainDynamicFlag();
	po->m_dynamicList.Append(m_dynamicList);
	po->m_afterList.Append(m_afterList);
	//po->m_shotList.Append(m_shotList);
	ASSERT(!po->m_shotList.First() && !m_shotList.First());
	po->m_bDynamics |= m_bDynamics;
}
// ------------------------------------------------------------------------
void CViewOrder::operator%=(TCCFMatrix3x4 &dir)
{
	//int  nNormals = m_nOrdered*(m_nOrdered-1)/2;
	//if(!m_bSingle) {
		m_normal.pt = dir%m_normal.pt;
		m_normal.n = dir%m_normal.n;
		__nNormalsCnt--;
		if(m_normal.ntype<6) __nNormalsOpt--;
		m_normal.ntype = DefineNormalType(m_normal.n);
		(*m_apOrdered[1])%=dir;
	//}
	(*m_apOrdered[0])%=dir;
}
// ------------------------------------------------------------------------
void CViewOrder::ApplyModelUsage()
{
	m_apOrdered[0]->ApplyModelUsage();
	/*if(!m_bSingle)*/	m_apOrdered[1]->ApplyModelUsage();
}
// ------------------------------------------------------------------------
void CViewOrder::ComputeMaxHeight()
{
	//ASSERT(!m_bSingle);
	m_apOrdered[0]->ComputeMaxHeight();
	m_fMaxHeight = m_apOrdered[0]->m_fMaxHeight;
	//if(!m_bSingle) {
		m_apOrdered[1]->ComputeMaxHeight();
		m_fMaxHeight = Max(m_fMaxHeight,m_apOrdered[1]->m_fMaxHeight);
	//}
}
// ------------------------------------------------------------------------
#if 0
void CViewOrder::Split()
{
	m_apOrdered[0]->Split();
	/*if(!m_bSingle)*/	m_apOrdered[1]->Split();
}
#endif
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
CShelterOrder::CShelterOrder(CViewObjectModel *pBase)
			: CViewObjectRef(pBase)
{
	m_pInnerOrder = NULL;
	m_bInhibitDraw = FALSE;
}
void CShelterOrder::DrawViewStartHere()
{
	ASSERT(m_pInnerOrder);
	m_bInhibitDraw = TRUE;
	m_pTopOrder->Draw();
	m_bInhibitDraw = FALSE;
	m_pBase->BaseSet(0).Base(0).SetOrderToBody(1);
	CViewObjectRef::Draw();
	m_pInnerOrder->Draw();
}
void CShelterOrder::Draw()
{
	ASSERT(m_pInnerOrder);
	if( m_bInhibitDraw ) return;
	m_pBase->BaseSet(0).Base(0).SetOrderToBody(1);
	CViewObjectRef::Draw();
	m_pInnerOrder->Draw();
	m_pBase->BaseSet(0).Base(0).SetOrderToBody(0);
	CViewObjectRef::Draw();
}
void CShelterOrder::PromoteDynamic()
{
	for( CViewDynamic *pd = m_dynamicList.First() ; pd ; pd = pd->Next() ) {
		m_bDynamics = TRUE;
		bool bBump = TRUE;
		ASSERT(pd->IsBumpDynamic());
		if( m_pBase->BaseSet(0).Base(0).GetBody(0).BumpShelter(((CViewBumpDynamic*)pd)->BumpDef(),bBump) )
			m_pInnerOrder->m_dynamicList.Move(pd,m_dynamicList);
		else
			((CViewBumpDynamic*)pd)->SetPromoteNode(this);
	}
	m_pInnerOrder->PromoteDynamic();
}
bool CShelterOrder::Bump(SBumpDef0 &def) {
	ASSERT(def.eType == SBumpDef0::T_BUMPDEF);
	ASSERT(m_pBase);
	UpdateInv();
	if( m_pBase->IsIdentity() )	{
		CFVector3	start = def.start;
		def.start += m_inv3;
		bool bRes = TRUE;
		m_pBase->BaseSet(0).Base(0).GetBody(0).BumpShelter(def,bRes);
		def.start = start;
		if( bRes ) {
			((SBumpDef*)&def)->nBumpFlags = BF_BUMPSHELTER;
			((SBumpDef*)&def)->pBumpRef = this;
		}
		return bRes;
	}
	SBumpDef0	def1;
	def1.fTime = def.fTime;
	def1.fRadius = def.fRadius;
	//def1.fMass = def.fMass;
	//def1.nBumpFlags = def.nBumpFlags;
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
	bool bRes = TRUE;
	m_pBase->BaseSet(0).Base(0).GetBody(0).BumpShelter(def1,bRes);
	if( bRes ) {
		def.fTime = def1.fTime;
		((SBumpDef*)&def)->nBumpFlags = BF_BUMPSHELTER;
		((SBumpDef*)&def)->pBumpRef = this;
		m_dir.MultF(def1.vel1,def.vel1);
		return TRUE;
	}
	return FALSE;
}
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void CViewMorphedOrder::RecalcNormal()
{
	m_normal.pt = *m_pVertex[0];
	m_normal.npt = m_normal.pt*
		(m_normal.n = Normal((*m_pVertex[1]-m_normal.pt)%(*m_pVertex[2]-m_normal.pt)));
	m_normal.ntype = 1000;
}
// ------------------------------------------------------------------------
void CViewMorphedOrder::ReadIndices(CTaggedFile &f)
{
	for( int i = 0 ; i < 3 ; i++ ) {
		int nIdx;
		f.ReadInt(nIdx);
		m_pVertex[i] = CViewObjectBase::GetCurrBase()->GetVertexPtr(nIdx);
	}
	SNormal n = m_normal;
	RecalcNormal();
	//ASSERT(fabs(m_normal.n*n.n-1)<1e-3);
	//ASSERT(fabs(m_normal.npt-n.npt)<1e-3);
}
// ------------------------------------------------------------------------


