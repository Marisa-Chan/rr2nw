#ifndef	__VIEW_CPP__
#include"_view.h"
#endif

#include"stdarg.h"
#if 0
void WriteLog(char *fmt,...)
{
	#if 0
	va_list	va;
	va_start(va,fmt);
	static FILE *f=fopen("logfile","wt");
	vfprintf(f,fmt,va);
	fflush(f);
	va_end(va);
	#endif
}
#endif

// ------------------------------------------------------------------------
CViewBody::~CViewBody() {
	for(int i = 0 ; i < m_nFigures ; i++ )	delete m_ppFigures[i];
	delete [] m_ppFigures;
}

double CViewBody::ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d)
{
	double fErr = 1e10;
	for( int i = 0 ; i < m_nFigures ; i++ )	fErr = Min(fErr,m_ppFigures[i]->ComputeBSPError(pt,n,d));
	return fErr;
}

CFVector3	CViewBody::m_minBound, CViewBody::m_maxBound;
void CViewBody::Read(CTaggedFile &f)
{
	m_minBound = CFVector3(1e10,1e10,1e10);
	m_maxBound = CFVector3(-1e10,-1e10,-1e10);
	f.Descend("BODY",FALSE,0);
		f.Descend("BODH",TRUE,2);
			f.ReadInt(m_nFigures);
			f.ReadInt(m_nViewFigures);
			f.ReadInt(m_nFirstBumpFigure);
			f.ReadInt(m_nFirstCutFigure);
		f.Ascend();
		m_ppFigures = new CViewFigure*[m_nFigures];
		if(!m_ppFigures)	f.SetOK(FALSE);
		for(int i = 0 ; i < m_nFigures ; i++ )
			m_ppFigures[i] = CViewFigure::Read(f,i>=m_nViewFigures);
	f.Ascend();
	m_center = (m_minBound+m_maxBound)/2;
}
// ------------------------------------------------------------------------
//bool bLogBump = FALSE;
inline	bool	CViewBody::PreDrawLinear(CViewLinearDynamic *pd,
												TCCFVector3 &objViewPoint)
{
	SBumpDef	bumpDef;
	bumpDef=pd->m_bump;
	bumpDef.fTime=1000;
	if(Bump(bumpDef)) {
		pd->m_dynBase1 = bumpDef.start+bumpDef.vel*
												(bumpDef.fTime-0.0001);
		int i0 = 0;
		double f0 = (pd->m_dynBase1-m_ppFigures[0]->Vertex0())*
												m_ppFigures[0]->Normal();
		for( int  i = 1 ; i < m_nFigures ; i++ ) {
			double f = (pd->m_dynBase1-m_ppFigures[i]->Vertex0())*
												m_ppFigures[i]->Normal();
			if( f > f0 )	{ f0 = f; i0 = i; }
		}
		return ((objViewPoint-m_ppFigures[i0]->Vertex0())*
									m_ppFigures[i0]->Normal())*f0 < 0;
	}
	else {
		CFVector3	n = Normal(pd->m_bump.vel),
					p1 = m_center - pd->m_dynBase1,
					p2 = objViewPoint - pd->m_dynBase1;
					// (p1-(p1*n)*n)*(p2-(p2*n)*n)==p1*p2-(p1*n)*(p2*n)
					// Scalar product of projections to plane normal to
					// m_speed1
		return p1*p2 >= (p1*n)*(p2*n);
	}
}
// ------------------------------------------------------------------------
inline	bool CViewBody::PreDrawSpheric(CViewSphericDynamic *pd,
											TCCFVector3 &objViewPoint)
{
	int i0 = 0;
	double f0 = (pd->m_dynBase1-m_ppFigures[0]->Vertex0())*
													m_ppFigures[0]->Normal();
	for( int  i = 1 ; i < m_nFigures ; i++ ) {
		double f = (pd->m_dynBase1-m_ppFigures[i]->Vertex0())*
											m_ppFigures[i]->Normal();
		if( f > f0 )	{ f0 = f; i0 = i; }
	}
	return	((objViewPoint-m_ppFigures[i0]->Vertex0())*
											m_ppFigures[i0]->Normal())*f0 < 0;
}
// ------------------------------------------------------------------------
inline	bool CViewBody::PreDrawDynamic(CViewDynamic *pd,
												TCCFVector3 &objViewPoint)
{
	switch(pd->m_eDType) {
		case CViewDynamic::DT_SPHERIC: return PreDrawSpheric(
								(CViewSphericDynamic*)pd,objViewPoint);
		case CViewDynamic::DT_LINEAR: return PreDrawLinear(
								(CViewLinearDynamic*)pd,objViewPoint);
		default: ASSERT(0); return FALSE;
	}
}
// ------------------------------------------------------------------------
CViewBody *__pBumpBody = NULL;
int		__bViewHide = 0;

void	CViewBody::Draw()
{
//	m_bHide = FALSE;
	//m_nViewFigures = 1;
	ASSERT(!m_shotList.First());
	if( m_dynamicList.First() || m_afterList.First() ) {
		/*CFVector3	oldViewPoint = CViewObject::m_currObjViewPoint;
		CFMatrix3x4	*pOldInv = CViewObject::m_pCurrentInvMx;
		CViewObjectRef *pOldRef = CViewObjectRef::m_pCurrRef;
		CFMatrix3x4 *pOldRefDir = CViewObjectRef::m_pCurrRefDir;
		CViewObject::SetOrderViewPointToGlobal();
		bool	bHaze = CViewFigure::GetHaze();
		int		z1Shift = CViewObjectBase::m_z1Shift;
		//WATER_VIEW	eWaterView = CViewObject::m_eWaterView;
		WATER_SPLIT	eWaterSplit = CViewFigure::m_eWaterSplit;*/
		CViewRenderState	rs;
		rs.Load();
		m_dynamicList.SortZOrder();
		m_afterList.SortZOrder();
		for( CViewDynamic *pd = m_dynamicList.First(), *pd1 ; pd!=NULL ;
														pd = pd1 ) {
			pd1 = pd->Next();
			if( PreDrawDynamic(pd,rs.Viewpoint()) ) {
				m_dynamicList.Remove(pd);
				pd->Draw();
			}
		}
		/*CViewObject::m_currObjViewPoint = oldViewPoint;
		CViewObject::m_pCurrentInvMx = pOldInv;
		CViewFigure::SetHaze(bHaze);
		CViewFigure::m_eWaterSplit = eWaterSplit;
		GRSetZPrecision(z1Shift);
		CViewObjectRef::m_pCurrRefDir = pOldRefDir;*/
		rs.Set(FALSE);

		_gr_polygon.nLights = m_dwLights;
		if(!__bViewHide)
		for( int  i = 0 ; i < m_nViewFigures ; i++ )
			m_ppFigures[i]->Draw();
		//m_bHide = FALSE;

		#if _USE_OPENGL
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadMatrix(CViewObject::m_viewPointDirMx);
		#endif
		CViewObject::SetOrderViewPointToGlobal();
		m_dynamicList.Draw();
		m_afterList.Draw();
		/*CViewObject::m_currObjViewPoint = oldViewPoint;
		CViewObject::m_pCurrentInvMx = pOldInv;
		CViewOrder::m_currOrdViewPoint = oldViewPoint;
		CViewObjectRef::m_pCurrRef = pOldRef;
		CViewFigure::SetHaze(bHaze);
		CViewObjectBase::m_z1Shift = z1Shift;
		CViewObjectRef::m_pCurrRefDir = pOldRefDir;
		//CViewObject::m_eWaterView = eWaterView;
		CViewFigure::m_eWaterSplit = eWaterSplit;
		CViewOrder::SetGlobalOrder(FALSE); // because set to true
											// in CViewObjectBase::Draw*/
		rs.Set();
		#if _USE_OPENGL
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		#endif
	}
	else {
		//if(!m_bHide)
		_gr_polygon.nLights = m_dwLights;
		for( int  i = 0 ; i < m_nViewFigures ; i++ )
			m_ppFigures[i]->Draw();
		//m_bHide = FALSE;
	}
}
// ------------------------------------------------------------------------
bool CViewBody::AdvancedBump(SBumpDef0 &def)
{
	ASSERT( m_nFirstCutFigure == m_nFigures );
	bool	bRes = FALSE;
	for( int i = 0 ; i < m_nViewFigures ; i++ ) {
		CViewFigure	&fig = *m_ppFigures[i];
		bRes = bRes | (fig.AdvancedBump(def)!=0);
	}
	return	bRes;
}

int  __advanced = 1;

bool CViewBody::Bump(SBumpDef0 &def)
{
	//if( hypot(def.vel.x,def.vel.z) > 0.1 )
	//	printf("Body %08X   (%d)\n",this,m_nViewFigures);
	if( __advanced && (def.dwFlags1 & BF1_ADVANCED) ) {
		if(	AdvancedBump(def) ) {
			__pBumpBody = this;
			return TRUE;
		}
		else
			return FALSE;
	}
	//WRITELOG("Normal Bump");
	double	fTimeL = 0, fTimeR = def.fTime, bL;
	int i0 = -1, iL = -1; bool aLlt0;
	for( int i = m_nFirstBumpFigure ; i < m_nFirstCutFigure ; i++ ) {
		CViewFigure	&fig = *m_ppFigures[i];
		double b = (def.start-fig.Vertex0())*fig.Normal() - def.fRadius;
		double a = def.vel*fig.Normal();
		if( i0 < 0 ) {
			if( iL < 0 ) { iL = i; bL = b; aLlt0 = a < 0; }
			else if( b > bL ) { iL = i; bL = b; aLlt0 = a < 0; }
		}
		if( a >= 0 ) {	// going outside this plane
			// if sphere center is inside the plane, don't let
			// go further outside ???
						// adj fTimeR if necessary
			if( a*fTimeR+b > 0 )	{ // t0[i] < tR, do adj fTimeR
				if( b < 0 ) {	// calc min in case we've lost precision
					double t = -b/a;
					if( t < fTimeR ) {
						if( t <= fTimeL ) return FALSE;
						fTimeR = t;
					}
				} else return FALSE; // a >= 0, b >= 0 => Max >= 0
			}
		}
		else { // a < 0, adj fTimeL if necessary
			if( a*fTimeL+b > -def.fRadius )	{ // t0[i] > tL, do adj fTimeL
				if( a*fTimeR+b < 0 ) { // calc max in case we've lost precision
					double t = -b/AbsNoLess(a,1e-10);
					if( t > fTimeL ) {
						if( t >= fTimeR )	return FALSE;
						fTimeL = t; i0 = i;
					}
				} else return FALSE; // a*fTimeR+b >= 0 => Max >= 0
			}
		}
	}
	if( i0 >= 0 || (i0=iL) >= 0 && aLlt0 && bL <= 0 ) {
		ASSERT(fTimeL <= fTimeR && fTimeR <= def.fTime);
		if( m_nFirstCutFigure < m_nFigures ) {
			CFVector3	v = def.start+def.vel*fTimeL;
			for( int i = m_nFirstCutFigure ; i < m_nFigures ; i++ ) {
				CViewFigure	&fig = *m_ppFigures[i];
				if( (v-fig.Vertex0())*fig.Normal() > def.fRadius )	return FALSE;
			}
		}
		def.vel1 = def.vel-m_ppFigures[i0]->Normal()*
								((def.vel*m_ppFigures[i0]->Normal())*2);
		def.fTime = fTimeL;
		//def.nBumpFlags = BF_BUMPSTATIC;
		__pBumpBody = this;
		return TRUE;
	}
	return FALSE;
}
// ------------------------------------------------------------------------
void CViewBody::operator%=(TCCFMatrix3x4 &dir)
{
	m_center = dir%m_center;
	for( int  i = 0 ; i < m_nFigures ; i++ )
		*m_ppFigures[i] %= dir;
}
// ------------------------------------------------------------------------
void CViewBody::Hue(double fModify)
{
	for( int  i = 0 ; i < m_nFigures ; i++ )
		m_ppFigures[i]->Hue(fModify);
}
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
bool CViewBody::BumpShelter(SBumpDef0 &def, bool &bBump)
			// Returns whether some part of object is inside
			// if &bBump does bump and returns result in bBump
{
	double	fTimeL = 0, fTimeR = def.fTime, bL;
	double	fTimeI = def.fTime;
	bool bOuter = FALSE, bOuterBump = TRUE, bAllInner = TRUE;
	int i0i = -1;
	int i0 = -1, iL = -1; bool aLlt0;
	for( int i = m_nFirstBumpFigure ; i < m_nFirstCutFigure ; i++ ) {
		CViewFigure	&fig = *m_ppFigures[i];
		double b0 = (def.start-fig.Vertex0())*fig.Normal(),
				b = b0-def.fRadius, bI = b0+def.fRadius;
		double a = def.vel*fig.Normal();
		if( b >= 0 ) {
			if( bOuterBump ) {
				bOuter = TRUE;
				if( i0 < 0 ) {
					if( iL < 0 ) { iL = i; bL = b; aLlt0 = a < 0; }
					else if( b > bL ) { iL = i; bL = b; aLlt0 = a < 0; }
				}
				if( a >= 0 ) {	// adj fTimeR if necessary
					if( a*fTimeR+b > 0 )	{ // t0[i] < tR, do adj fTimeR
						if( b < 0 ) {	// calc min in case we've lost precision
							double t = -b/a;
							if( t < fTimeR ) {
								if( t <= fTimeL ) bOuterBump = FALSE;
								else fTimeR = t;
							}
						} else bOuterBump = FALSE; // a >= 0, b >= 0 => Max >= 0
					}
				}
				else { // a < 0, adj fTimeL if necessary
					if( a*fTimeL+b > 0 )	{ // t0[i] > tL, do adj fTimeL
						if( a*fTimeR+b < 0 ) { // calc max in case we've lost precision
							double t = -b/a;
							if( t > fTimeL ) {
								if( t >= fTimeR )	bOuterBump = FALSE;
								else { fTimeL = t; i0 = i; }
							}
						} else bOuterBump = FALSE; // a*fTimeR+b >= 0 => Max >= 0
					}
				} // if a >= 0
			} // if bOuterBump
		} // if b >= 0
		if( bI < 0 ) {
			double b = b0+def.fRadius;
			if( a > 0 ) {	// found approaching plane
				if( b >= 0 ) {	// already in from inside
					fTimeI = 0; i0i = i; break;
				} else // b < 0
				if( a*fTimeI+b > 0 ) { // bumps from inside
					fTimeI = -b/a; i0i = i;
				}
			}
		} // if bI < 0
		else bAllInner = FALSE;
	} // for each figure

	if( !bBump && bAllInner )	{ /*WRITELOG("1: Inside\n");*/ return TRUE; }

	if( bOuterBump ) {
		if( i0 >= 0 || (i0=iL) >= 0 && aLlt0 && bL <= 0 ) {
			ASSERT(fTimeL <= fTimeR && fTimeR <= def.fTime);
			if( m_nFirstCutFigure < m_nFigures ) {
				CFVector3	v = def.start+def.vel*fTimeL;
				for( int i = m_nFirstCutFigure ; i < m_nFigures ; i++ ) {
					CViewFigure	&fig = *m_ppFigures[i];
					if( (v-fig.Vertex0())*fig.Normal() > def.fRadius )	{
						i0 = -1; break;
					}
				}
			}
		} // i0 >= 0
	} else i0 = -1;

	if( !bBump ) { /*WRITELOG1("2: %s",i0>=0?"inside":"outside");*/ return i0 >= 0; }

	if( !bAllInner && i0 < 0 ) i0i = -1;	// if from outside and does not bump, cannot bump inner
	if( i0i >= 0 ) {
		ASSERT(fTimeI <= def.fTime);
		if( m_nFirstCutFigure < m_nFigures ) {
			CFVector3	v = def.start+def.vel*fTimeL;
			for( int i = m_nFirstCutFigure ; i < m_nFigures ; i++ ) {
				CViewFigure	&fig = *m_ppFigures[i];
				if( (v-fig.Vertex0())*fig.Normal() > def.fRadius )	{
					i0i = -1; break;
				}
			}
		}
	}
	if( i0i >= 0 || i0 >= 0 ) {
		if( i0i < 0 || i0 >= 0 && fTimeL < fTimeI )	{ i0i = i0; fTimeI = fTimeL; }
		def.vel1 = def.vel-m_ppFigures[i0i]->Normal()*
								((def.vel*m_ppFigures[i0i]->Normal())*2);
		def.fTime = fTimeL;
		//def.nBumpFlags = BF_BUMPSTATIC;
		// bBump = TRUE; already true
		__pBumpBody = this;
		//WRITELOG1("3: %s",bAllInner || i0 >= 0?"inside":"outside");
		return bAllInner || i0 >= 0;
	}
	bBump = FALSE;
	//WRITELOG1("4: %s",bAllInner?"inside":"outside");
	return bAllInner;
}


