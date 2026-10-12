#define LAST_H__SCENE
#include "game.h"
#include "segment1.h"
#include "cnlmaps.h"

// -------------------------------------------------------------------------
void CSplineSegment1D::InitHermit( double p1, double p4, double r1, double r4 )
{
	a = 2*p1-2*p4+r1+r4;
	b  = -3*p1+3*p4-2*r1-r4;
	c = r1;
	d = p1;
}
// -------------------------------------------------------------------------
void CSplineSegment1D::InitBezier( double p1, double p2, double p3, double p4 )
{
	InitHermit( p1, p4, 3*(p2-p1), 3*(p4-p3) );
}
// -------------------------------------------------------------------------
void CSplineSegment1D::LinearCombine( double c1, CSplineSegment1D &seg1,
									  double c2, CSplineSegment1D &seg2 )
{
	a = c1*seg1.a + c2*seg2.a;
	b = c1*seg1.b + c2*seg2.b;
	c = c1*seg1.c + c2*seg2.c;
	d = c1*seg1.d + c2*seg2.d;
}
// -------------------------------------------------------------------------
double CSplineSegment1D::Value( double t ) const
{
		//	a*t^3+b*t^2+c*t+d = ((a*t+b)*t+c)*t+d
	return	((a*t+b)*t+c)*t+d;
}
// -------------------------------------------------------------------------
double CSplineSegment1D::Deriv1( double t ) const
{
		//	3*a*t^2+2*b*t+c = (3*a*t+2*b)*t+c
	return	(3*a*t+2*b)*t+c;
}
// -------------------------------------------------------------------------
double CSplineSegment1D::Deriv2( double t ) const
{
				//	6*a*t+2*b
	return	6*a*t+2*b;
}
// -------------------------------------------------------------------------
double CSplineSegment1D::Deriv3() const
{
	return	6*a;
}
// -------------------------------------------------------------------------
double CSplineSegment1D::GetBezier( int i ) const
{
	switch(i)  {
		case 1:	return	Value(0);
		case 2:	return	Value(0)+Deriv1(0)/3;
		case 3:	return	Value(1)-Deriv1(1)/3;
		case 4:	return	Value(1);
		default:
			// Valid point numbers are 1 to 4
			ASSERT(0);
			return 0;
	}
}
// -------------------------------------------------------------------------
void CSplineSegment1D::GetCut( CSplineSegment1D &cut,
												double t1, double t2 ) const
{
	double	t21 = t2 - t1;
	cut.InitHermit( Value(t1), Value(t2), Deriv1(t1)*t21, Deriv1(t2)*t21 );
}
// -------------------------------------------------------------------------
void CSplineSegment1D::Offset( double fOffset )
{
	d += fOffset;
}
// -------------------------------------------------------------------------
void CSplineSegment1D::Scale( double fScale )
{
	a *= fScale;
	b *= fScale;
	c *= fScale;
	d *= fScale;
}
// -------------------------------------------------------------------------
void CSplineSegment1D::Revert()
{
	d+= a+b+c;
	c = -3*a-2*b-c;
	b+= 3*a;
	a = -a;
}
// -------------------------------------------------------------------------
void  CSplineSegment1D::GetValueBounds( double &lower, double &upper ) const
{
	lower = upper = GetBezier(1);
	for( int  i = 2 ; i <= 4 ; i++ )  {
		double	p = GetBezier(i);
        if( p < lower )	  lower = p;
		if( p > upper )	  upper = p;
	}
}
// -------------------------------------------------------------------------
double CSplineSegment1D::MinDeriv1Len(double t1, double t2) const
{
	double	d1_1 = fabs(Deriv1(t1)), d1_2 = fabs(Deriv1(t2)),
			min_d1 = min(d1_1,d1_2);
	if( fabs(a) < 0.00001 ) {
		min_d1 -= 0.00001;
		return	min_d1 >= 0 ? min_d1 : 0;
	}
	else {
		double	t_min = -b/(3*a);
		if( t_min < t1 && t_min < t2 || t_min > t1 && t_min > t2 )
			return  min_d1;
		double   d1_mid = fabs(Deriv1(t_min));
		return	min(d1_mid,min_d1);
	}
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
// CChannelMap::NODE public:
// -------------------------------------------------------------------------
bool CChannelMap::NODE::operator==(const CChannelMap::NODE &other) const
{
	return	! ( *this != other );
}
// -------------------------------------------------------------------------
bool CChannelMap::NODE::operator!=(const CChannelMap::NODE &other) const
{
	return	fSpline != other.fSpline ||
						fabs(t-other.t) > 0.001 || fabs(f-other.f) > 0.001;
}
// -------------------------------------------------------------------------
// CChannelMap:: public:
// -------------------------------------------------------------------------
CChannelMap::CChannelMap(int n)
{
  m_pData       = (DATA *) new BYTE[sizeof(DATA)+sizeof(NODE)*n];
  m_maxNodesNum = n;
}
// -------------------------------------------------------------------------
CChannelMap::CChannelMap( const CChannelMap &other )
{
	m_pData = NULL;
	* this = other;
}
// -------------------------------------------------------------------------
const CChannelMap &CChannelMap::operator=( const CChannelMap &other )
{
	delete	[] (BYTE *) m_pData;
	if( other.m_pData == NULL )
		m_pData = NULL;
	else {
		m_pData = (DATA *)
			new BYTE[sizeof(DATA)+sizeof(NODE)*(other.m_pData->nNodes+1)];
		DATA	* pData = m_pData,  * pOtherData = other.m_pData;
		pData->nNodes = pOtherData->nNodes;
		pData->fOpenFactor = pOtherData->fOpenFactor;
		pData->bUpdated = pOtherData->bUpdated;
		memcpy( pData->aNodes, pOtherData->aNodes,
										sizeof(NODE)*(pData->nNodes+1) );
	}
	return	*this;
}
// -------------------------------------------------------------------------
const CChannelMap &CChannelMap::operator=( int n )
{
	ASSERT( n == 0 );	// Use =0 to clean up. Other values not supported.
	(void)n;
	delete	[] (BYTE*) m_pData;
	m_pData = 0;
	return	*this;
}
// -------------------------------------------------------------------------
CChannelMap::~CChannelMap()
{
	delete	[] (BYTE*) m_pData;
}
// -------------------------------------------------------------------------
bool CChannelMap::operator==( const CChannelMap &other ) const
{
	return	! ( *this == other );
}
// -------------------------------------------------------------------------
bool CChannelMap::operator!=( const CChannelMap &other ) const
{
	const DATA  * pData = m_pData,  * pOtherData = other.m_pData;
	if( pData == NULL && pOtherData == NULL )	return FALSE;
	if( pData != NULL && pOtherData != NULL ) {
		if( pData->nNodes != pOtherData->nNodes )	return TRUE;
		for( int  i = 0, nNodes = pData->nNodes ; i < nNodes ; i++ )
			if( pData->aNodes[i] != pOtherData->aNodes[i] )	return TRUE;
		return FALSE;
	}
	return	TRUE;
}
// -------------------------------------------------------------------------
/*CChannelMap::operator int() const
{
	return	m_pData != NULL;
}*/
// -------------------------------------------------------------------------
void CChannelMap::Create( int  nNodes, int  nHold,
										double fSpline, double fOpenFactor )
{
  ASSERT( nNodes >= 2 );
  ASSERT( nNodes <= m_maxNodesNum);

  m_pData->nNodes   = nNodes;
  m_pData->bUpdated = FALSE;
  m_pData->wFlags   = 0;
  m_pData->nHold    = nHold;
	m_pData->fOpenFactor = fOpenFactor;

	NODE	* pNodes = m_pData->aNodes;
	for( int  i = 0 ; i <= nNodes ; i++ )
		pNodes[i].fSpline = fSpline;
}
// -------------------------------------------------------------------------
void CChannelMap::CreateOpen( int  nNodes, int  nHold, double  fSplineFactor )
{
  Create( nNodes, nHold, fSplineFactor, 1 );
  m_pData->wFlags = CMF_OPEN;
}
// -------------------------------------------------------------------------
void CChannelMap::CreateLinOpen( int  nNodes )
{
  Create( nNodes, -1, 0, 1 );
  m_pData->wFlags = CMF_LINEAR|CMF_OPEN;
}
// -------------------------------------------------------------------------
int    CChannelMap::NodeCount() const
{
	ASSERT( m_pData != NULL );
	return	m_pData->nNodes;
}
// -------------------------------------------------------------------------
CChannelMap::NODE  & CChannelMap::Node( int  nNode )
{
	ASSERT( m_pData != NULL && nNode < m_pData->nNodes );
	m_pData->bUpdated = FALSE;
	return	*(NODE*)&m_pData->aNodes[nNode];
}
// ------------- BY GREEN ------------------------------------------------------------
const CChannelMap::NODE &CChannelMap::CNode( int  nNode ) const{
  ASSERT( m_pData != NULL && nNode < m_pData->nNodes );
	m_pData->bUpdated = FALSE;
  return  *(NODE*)&m_pData->aNodes[nNode];
}
// -------------------------------------------------------------------------
CChannelMap::NODE  & CChannelMap::operator[]( int  nNode )
{
	ASSERT( m_pData != NULL && nNode < m_pData->nNodes );
	m_pData->bUpdated = FALSE;
	return	*(NODE*)&m_pData->aNodes[nNode];
}
// -------------------------------------------------------------------------
bool   CChannelMap::Update()
				// Splining and other updating
{
	ASSERT( m_pData != NULL && m_pData->nNodes >= 2 );
	if( m_pData->bUpdated )  return TRUE;
	if( m_pData->wFlags & CMF_LINEAR ) {
		m_pData->aNodes[0].v = 0;
		m_pData->aNodes[m_pData->nNodes-1].v = 0;
		return	TRUE;
	}

	int  	nNodes = m_pData->nNodes;
	NODE	* pNodes = m_pData->aNodes;

	/* double	fMinF = pNodes[0].f, fMaxF = fMinF;
	for( int  i = 1 ; i < nNodes ; i++ ) {
		double	& f = pNodes[i].f;
		if( f < fMinF )  fMinF = f; else
		if( f > fMaxF )  fMaxF = f;
	}

	double	denom = fMaxF - fMinF;
	if( denom < 0.001 )	denom = 0.001;
	m_pData->fOpenFactor = fabs(pNodes[nNodes-1].f-pNodes[0].f)/denom; */

	// -------------------------------
	// ---------------- Form equations

	EqLine	*pLines = new EqLine[nNodes];
	if( pLines == NULL )	return FALSE;


		// Equation line 0
	NODE	*pNode = &pNodes[0], *pNode1 = &pNodes[1], *pNode2,
			*pNode_1 = &pNodes[nNodes-1],
			*pNode_2 = &pNodes[nNodes-2];

	double  h_1 = pNode_1->t-pNode_2->t,
		   h = pNode1->t-pNode->t,
		   of = m_pData->fOpenFactor, cf = 1-of;
	if( h_1 < 0.001 )	h_1 = 0.001;
	if( h < 0.001 )	h = 0.001;
	EqLine *pLine = &pLines[0];

	double	denom = 2*(h+h_1);
	pLine->a = -h/denom*cf;
	pLine->b = -0.5*of - h_1/denom*cf;
	pLine->f = (pNode1->f-pNode->f)*1.5/h*of +
			((pNode1->f-pNode->f)*h_1/h+
				(pNode_1->f-pNode_2->f)*h/h_1)*3/denom*cf;

		// Equation lines 1..N-2
	for( int  i = 1 ; i < nNodes-1 ; i++ )  {
		pNode_1 = pNode;  pNode = pNode1;	pNode1++;
		h_1 = h;  h = pNode1->t-pNode->t;
		if( i == m_pData->nHold )	h *= 1000;	// Hold correction
		if( h < 0.001 )  h = 0.001;
		pLine = &pLines[i];
		denom = 2*(h+h_1);
		pLine->a = -h/denom;
		pLine->b = -h_1/denom;
		pLine->f = ((pNode1->f-pNode->f)*h_1/h+
						(pNode->f-pNode_1->f)*h/h_1)*3/denom;
	}

		// Equation line N-1
	pNode_1 = pNode;  pNode = pNode1;
	pNode1 = &pNodes[0]; pNode2 = &pNodes[1];
	h_1 = h;  h = pNode2->t-pNode1->t;
	if( h < 0.001 )	h = 0.001;
	pLine = &pLines[nNodes-1];

	denom = 2*(h+h_1);
	pLine->a = -0.5*of - h/denom*cf;
	pLine->b = -h_1/denom*cf;
	pLine->f = (pNode->f-pNode_1->f)*1.5/h_1*of +
				((pNode2->f-pNode1->f)*h_1/h+
							(pNode->f-pNode_1->f)*h/h_1)*3/denom*cf;

	// -------------------------------
	// --------------- Running forward

	RunForw	  *pRunF = new RunForw[nNodes],
			  *pRF = &pRunF[0], *pRF_1;
	if( pRunF == NULL )	return delete [] pLines, FALSE;
	pLine = &pLines[0];
	pRF->alpha = pLine->a;
	pRF->beta = pLine->b;
	pRF->gamma = pLine->f;

	for( int i = 1 ; i < nNodes ; i++ )  {
		pRF_1 = pRF;		pRF = &pRunF[i];
		pLine = &pLines[i];
		denom = 1 - pLine->a * pRF_1->beta;
		pRF->alpha = pRF_1->alpha * pLine->a / denom;
		pRF->beta = pLine->b / denom;
		pRF->gamma = ( pLine->f + pRF_1->gamma * pLine->a ) / denom;
	}


	// -------------------------------
	// ------------------ Running back

	RunBack  *pRunB = new RunBack[nNodes],
			 *pRB_1 = &pRunB[nNodes-1], *pRB = pRB_1, *pRB1;
	if( pRunB == NULL )	return delete [] pRunF, delete [] pLines, FALSE;
	denom = 1 - pRF->alpha;
	pRB_1->kappa = pRF->beta / denom;
	pRB_1->mu = pRF->gamma / denom;

	for( int i = nNodes-2 ; i >= 0 ; i-- )  {
		pRB1 = pRB;	pRB = &pRunB[i];
		pRF = &pRunF[i];
		pRB->kappa = pRB_1->kappa * pRF->alpha + pRB1->kappa * pRF->beta;
		pRB->mu = pRB_1->mu * pRF->alpha + pRB1->mu * pRF->beta + pRF->gamma;
	}


	// -------------------------------
	// ----------- Filling in solution

	double	x0 = pRunB[0].mu/(1-pRunB[0].kappa);

	for( int i = 0 ; i < nNodes ; i++ )
		pNodes[i].v = x0*pRunB[i].kappa+pRunB[i].mu;

	delete	[] pLines;
	delete	[] pRunF;
	delete	[] pRunB;

	// ------------ Defining out the closing part of the spline

	pNodes[nNodes] = pNodes[0];
	double	alpha = 2*(pNodes[nNodes-1].f-pNodes[nNodes].f),
			beta = pNodes[nNodes-1].v+pNodes[nNodes].v, deltaT;
	deltaT = fabs( alpha / ( fabs(beta) > 0.001 ? beta : 0.001 ) );
	pNodes[nNodes].t = pNodes[nNodes-1].t + deltaT;

	return	m_pData->bUpdated = TRUE, TRUE;
}
// -------------------------------------------------------------------------
double   CChannelMap::Phase( double  fTime ) const
{
	ASSERT( m_pData != NULL && m_pData->nNodes >= 2 );
	if( ! m_pData->bUpdated )	((CChannelMap*)this)->Update();

	NODE	* pNodes = m_pData->aNodes;
	int		nNodes = m_pData->nNodes;

	double	fValue;
	if( fTime <= pNodes[0].t )
    fValue = pNodes[0].f+(fTime-pNodes[0].t)*pNodes[0].v*pNodes[0].fSpline; else
	if( fTime >= pNodes[nNodes-1].t )
		fValue = pNodes[nNodes-1].f+
          (fTime-pNodes[nNodes-1].t)*pNodes[nNodes-1].v*pNodes[nNodes-1].fSpline;
	else {
		for( int i = 1 ; i < nNodes ; i++ )
		if( fTime < pNodes[i].t ) {
			NODE	& node1 = pNodes[i-1],  & node2 = pNodes[i];
			double	h = node2.t - node1.t;
			if( h < 0.001 )
        fValue = (node1.f+node2.f)/2;
			else {
				fValue = (node1.f*(node2.t-fTime)+
											node2.f*(fTime-node1.t))/h;
				if( !(m_pData->wFlags & CMF_LINEAR) ) {
					CSplineSegment1D  seg;
          seg.InitHermit(node1.f,node2.f,node1.v*h*node1.fSpline,node2.v*h*node1.fSpline);
					double	fSplineVal = seg.Value((fTime-node1.t)/h);
          fValue = fSplineVal; //fValue*(1-node1.fSpline)+fSplineVal*node1.fSpline;
				}
			}
			break;
		}
		ASSERT( i < nNodes );
	}

	if( m_pData->wFlags & CMF_OPEN )	return fValue;
	double	fCyclicTime = fTime, fCyclicValue,
			fDuration = pNodes[nNodes].t - pNodes[0].t;
	while( fCyclicTime < pNodes[0].t )  fCyclicTime += fDuration;
	while( fCyclicTime > pNodes[nNodes].t )  fCyclicTime -= fDuration;
	if( fCyclicTime == fTime )	return fValue;

	for( int i = 1 ; i <= nNodes ; i++ )
	if( fCyclicTime < pNodes[i].t ) {
		NODE	& node1 = pNodes[i-1],  & node2 = pNodes[i];
		double	h = node2.t - node1.t;
		if( h < 0.001 )
			fCyclicValue = (node1.f+node2.f)/2;
		else {
			double	fLineVal = (node1.f*(node2.t-fCyclicTime)+
											node2.f*(fCyclicTime-node1.t))/h;
			CSplineSegment1D  seg;
			seg.InitHermit(node1.f,node2.f,node1.v*h,node2.v*h);
			double	fSplineVal = seg.Value((fCyclicTime-node1.t)/h);
			fCyclicValue =
					fLineVal*(1-node1.fSpline)+fSplineVal*node1.fSpline;
		}
		break;
	}
	ASSERT( i <= nNodes );
	return	fValue*m_pData->fOpenFactor +
									fCyclicValue*(1-m_pData->fOpenFactor);
}
// -------------------------------------------------------------------------
double  CChannelMap::StartTime() const
{
	ASSERT( m_pData != NULL );
	return	m_pData->aNodes[0].t;
}
// -------------------------------------------------------------------------
double  CChannelMap::EndTime() const
{
	ASSERT( m_pData != NULL );
	return	m_pData->aNodes[m_pData->nNodes].t;
}
// -------------------------------------------------------------------------
