#ifndef	__VIEW_CPP__
#include"_view.h"
#endif

//static	bool (*ProcessBonusProc)(SBonusDef *,SBonusDef *);
static	TPProcessBonusProc	ProcessBonusProc;
void SetProcessBonusProc(TPProcessBonusProc Proc
							/*bool (*Proc)(SBonusDef *,SBonusDef *)*/)
{
	ProcessBonusProc = Proc;
}
// ------------------------------------------------------------------------
bool Bump(SBumpDef &def, SBumpDef &def1,int nFlags)
{
	CFVector3	v10 = def1.vel - def.vel;
	//if( v10.x == 0 && v10.y == 0 && v10.z == 0 )	return FALSE;
	CFVector3	s01 = def.start - def1.start;
	double s2 = s01*s01, sv = s01*v10;
	if( s2 < 1e-3 )	return FALSE;
	if( sv < 0 ) { return FALSE; }	// distance increasing
	double	r2 = def.fRadius+def1.fRadius;
	r2 = s2-r2*r2;
	if( r2 < 0 ) {
		//if(ProcessBonusProc&&ProcessBonusProc(def.pBonus,def1.pBonus)) return FALSE;
		def.fTime = 0;
		if( nFlags != BF_BUMPSHOOT )
			def1.fTime = 0;
		double m = def.fMass+def1.fMass;
		if( m == 0 ) { return FALSE; }
		CFVector3 V = (def.fMass*def.vel+def1.fMass*def1.vel)/m;
		CFVector3 n = Normal(def.start-def1.start);
		def.vel1 = def.vel-V; def.vel1 -= ((def.vel1*n)*2)*n; def.vel1 += V;
		def1.vel1 = def1.vel-V; def1.vel1 -= ((def1.vel1*n)*2)*n; def1.vel1 += V;
		//def.vel1 = V-def.vel; def1.vel1 = V-def1.vel;
		//def1.vel1 = def.vel; def.vel1 = def1.vel;
		//WriteLog("BUMP\n");
		return def.nBumpFlags = def1.nBumpFlags = nFlags;
	}
	double	v2 = v10*v10;
	if( v2 < 1e-6 ) { /*WriteLog("v2<=1e-6\n");*/ return FALSE; }
	double	d = sv*sv-v2*r2;
	if( d <= 0 )	{ /*WriteLog("d<=0\n");*/ return FALSE;	} // does not bump
	double t = (sv-sqrt(d))/v2;
	if( t < Min(def.fTime,def1.fTime) ) {
		//if(ProcessBonusProc&&ProcessBonusProc(def.pBonus,def1.pBonus)) return FALSE;
		def.fTime = t;
		if( nFlags != BF_BUMPSHOOT )
			def1.fTime = t;
		double m = def.fMass+def1.fMass;
		if( m == 0 ) { return FALSE; }
		CFVector3 V = (def.fMass*def.vel+def1.fMass*def1.vel)/m;
		CFVector3 n = Normal(def.start-def1.start);
		def.vel1 = def.vel-V; def.vel1 -= ((def.vel1*n)*2)*n; def.vel1 += V;
		def1.vel1 = def1.vel-V; def1.vel1 -= ((def1.vel1*n)*2)*n; def1.vel1 += V;
		//def.vel1 = V-def.vel; def1.vel1 = V-def1.vel;
		//def1.vel1 = def.vel; def.vel1 = def1.vel;
		//WriteLog("BUMP\n");
		//printf("%lg,%lg,%lg->%lg,%lg,%lg\n",def.vel,def.vel1);
		return def.nBumpFlags = def1.nBumpFlags = nFlags;
	}
	return FALSE;
}
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
inline	void	CViewSphericDynamic::TransformInv(TCCFMatrix3x4 &inv)
{
	CFVector3 v;
	inv.Mult(m_dynBase1,v);
	m_dynBase1 = v;
}
// ------------------------------------------------------------------------
inline	void	CViewSphericDynamic::TransformInv(TCCFVector3 &inv)
{
	m_dynBase1 += inv;
}
// ------------------------------------------------------------------------
inline	void	CViewLinearDynamic::TransformInv(TCCFMatrix3x4 &inv)
{
	CFVector3 v;
	inv.Mult(m_dynBase1,v);
	m_dynBase1 = v;
	inv.Mult(m_dynEnd1,v);
	m_dynEnd1 = v;
	inv.MultF(m_bump.vel,v);
	m_bump.vel = v;
	inv.Mult(m_bump.start,v);
	m_bump.start = v;
}
// ------------------------------------------------------------------------
inline	void	CViewLinearDynamic::TransformInv(TCCFVector3 &inv)
{
	m_dynBase1 += inv;
	m_dynEnd1 += inv;
	m_bump.start += inv;
}
// ------------------------------------------------------------------------
inline	void	CViewDynamic::TransformInv(TCCFMatrix3x4 &inv)
{
	switch(m_eDType) {
		case DT_SPHERIC:
			((CViewSphericDynamic*)this)->TransformInv(inv); break;
		case DT_LINEAR:
			((CViewLinearDynamic*)this)->TransformInv(inv); break;
		default: ASSERT(0);
	}
}
// ------------------------------------------------------------------------
inline	void	CViewDynamic::TransformInv(TCCFVector3 &inv)
{
	switch(m_eDType) {
		case DT_SPHERIC:
			((CViewSphericDynamic*)this)->TransformInv(inv); break;
		case DT_LINEAR:
			((CViewLinearDynamic*)this)->TransformInv(inv); break;
		default: ASSERT(0);
	}
}
// ------------------------------------------------------------------------

CViewDynamicList	CViewDynamicList::m_wasteBox;

void CViewDynamicList::TransformInv(TCCFMatrix3x4 &inv)
{
	for( CViewDynamic *pd = m_pFirst ; pd != NULL ; pd = pd->m_pNext )
		pd->TransformInv(inv);
}
// ------------------------------------------------------------------------
void CViewDynamicList::TransformInv(TCCFVector3 &inv)
{
	for( CViewDynamic *pd = m_pFirst ; pd != NULL ; pd = pd->m_pNext )
		pd->TransformInv(inv);
}
// ------------------------------------------------------------------------
void CViewDynamicList::SortZOrder()
{
	if( m_pFirst == m_pLast ) return;
	CViewDynamic	*pd;
	CViewDynamicList	linears;
	{
		CViewDynamic *pd1;
		for( pd = m_pFirst ; pd != NULL ; pd = pd1 ) {
			pd1 = pd->Next();
			switch(pd->m_eDType) {
				case CViewDynamic::DT_LINEAR: linears.Move(pd,*this); break;
				case CViewDynamic::DT_SPHERIC:
					((CViewSphericDynamic*)pd)->m_fSortValue = Abs2(
						((CViewSphericDynamic*)pd)->m_dynBase-
											CViewOrder::m_currOrdViewPoint);
					break;
				default: ASSERT(0);
			}
		}
	}
	CViewDynamicList	list;
	list.Move(*this);
	for(;;) {
		CViewDynamic	*pdBest = list.m_pFirst;
		if(!pdBest)	break;
		if( pdBest->IsBumpDynamic() ) {
			for( pd = pdBest->m_pNext ; pd != NULL ; pd = pd->m_pNext )
				if( ((CViewBumpDynamic*)pd)->m_fSortValue >
						((CViewBumpDynamic*)pdBest)->m_fSortValue ) pdBest = pd;
			//list.Remove(pdBest);
			//Load(pdBest);
		}
		Move(pdBest,list);
	}
	Append(linears);
}
// ------------------------------------------------------------------------

SBumpFigures::~SBumpFigures()
{
	delete [] ppFigures;
}


bool SBumpFiguresRef::Bump(SBumpDef0 &def)
{
	(void)def;
	return FALSE;
	#if 0
	CFVector3	r_vel = def.vel - vel;

	double	fTimeL = 0, fTimeR = fTime, bL;
	int i0 = -1, iL = -1; bool aLlt0;
	for( int i = 0 ; i < pBase->nFigures ; i++ ) {
		CViewFigure	&fig = *pBase->ppFigures[i];
		double b = (def.start-fig.Vertex0())*fig.Normal() - def.fRadius;
		double a = r_vel*fig.Normal();
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
					double t = -b/a;
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
		ASSERT( def.eType == SBumpDef::T_BUMPDEF );
		def.vel1 = def.vel-ppFigures[i0]->Normal()*
								((r_vel*ppFigures[i0]->Normal())*2);
		def.fTime = fTimeL;
		fjdlfjdslkfjlk
		// vel1 = vel+n%(n%vel) = vel+n*(n*vel)-v*(n*vel)=vel+(n-vel)*(n*vel)
		def.newstart = (def.start+def.vel*fTimeL)+
			(vel+(ppFigures[i0]->Normal()-vel)*(ppFigures[i0]->Normal()*vel))*(fTime-fTimeL);
		//def.nBumpFlags = BF_BUMPSTATIC;
		__pBumpBody = this;
		return TRUE;
	}
	return FALSE;
	#endif
}

// ------------------------------------------------------------------------

