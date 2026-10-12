#ifndef	__VIEW_CPP__
#include"_view.h"
#endif

static	CViewBaseModifier0	CViewBaseModifier::m_null;

void CViewBaseModifier::Read(CTaggedFile &f,SViewObjectMorphNames &names)
{
	Clear();
	f.Descend("VBM_",TRUE,0);
		f.ReadInt(m_nVertices);
		f.ReadInt(m_nFigures);
		f.ReadInt(m_nClusters);
		m_ppVerticesUpdate = m_nVertices > 0 ? new CFVector3*[m_nVertices] : NULL;
		m_ppFiguresUpdate = m_nFigures > 0 ? new CViewFigure*[m_nFigures] : NULL;
		m_ppClustersUpdate = m_nClusters > 0 ? new CViewMorphedOrder*[m_nClusters] : NULL;
		for( int i = 0 ; i < m_nVertices ; i++ ) {
			CNameRef	name(f);
			m_ppVerticesUpdate[i] = (CFVector3*)name.Resolve(names.vertices);
		}
		for( int i = 0 ; i < m_nFigures ; i++ ) {
			CNameRef	name(f);
			m_ppFiguresUpdate[i] = (CViewFigure*)name.Resolve(names.figures);
		}
		for( int i = 0 ; i < m_nClusters ; i++ ) {
			CNameRef	name(f);
			m_ppClustersUpdate[i] = (CViewMorphedOrder*)name.Resolve(names.clusters);
		}
	f.Ascend();
}

void CViewBaseModifier::Update()
{
	for( int i = 0 ; i < m_nFigures ; i++ )
		m_ppFiguresUpdate[i]->RecalcNormal();
	for( int i = 0 ; i < m_nClusters ; i++ )
		m_ppClustersUpdate[i]->RecalcNormal();
}

CViewBaseModifier &CViewBaseModifier::Translate(TCCFVector3 &offset)
{
	for( int i = 0 ; i < m_nVertices ; i++ ) *m_ppVerticesUpdate[i] += offset;
	return *this;
}

CViewBaseModifier &CViewBaseModifier::RotateOx(double a,TCCFVector3 &center)
{
	if( m_nVertices ) {
		double _cos = cos(a), _sin = sin(a);
		double y0 = center.y*(1-_cos)+center.z*_sin,
			   z0 = center.z*(1-_cos)-center.y*_sin;
		for( int i = 0 ; i < m_nVertices ; i++ ) {
			CFVector3 &v = *m_ppVerticesUpdate[i];
			double y = v.y*_cos-v.z*_sin+y0;
				 v.z = v.z*_cos+v.y*_sin+z0;
			v.y = y;
		}
	}
	return *this;
}

CViewBaseModifier &CViewBaseModifier::RotateOy(double a,TCCFVector3 &center)
{
	if( m_nVertices ) {
		double _cos = cos(a), _sin = sin(a);
		double x0 = center.x*(1-_cos)-center.z*_sin,
			   z0 = center.z*(1-_cos)+center.x*_sin;
		for( int i = 0 ; i < m_nVertices ; i++ ) {
			CFVector3 &v = *m_ppVerticesUpdate[i];
			double x = v.x*_cos+v.z*_sin+x0;
				 v.z = v.z*_cos-v.x*_sin+z0;
			v.x = x;
		}
	}
	return *this;
}

CViewBaseModifier &CViewBaseModifier::RotateOz(double a,TCCFVector3 &center)
{
	if( m_nVertices ) {
		double _cos = cos(a), _sin = sin(a);
		double x0 = center.x*(1-_cos)+center.y*_sin,
			   y0 = center.y*(1-_cos)-center.x*_sin;
		for( int i = 0 ; i < m_nVertices ; i++ ) {
			CFVector3 &v = *m_ppVerticesUpdate[i];
			double x = v.x*_cos-v.y*_sin+x0;
				 v.y = v.y*_cos+v.x*_sin+y0;
			v.x = x;
		}
	}
	return *this;
}

CViewBaseModifier &CViewBaseModifier::Modify(TCCFMatrix3x4 &m)
{
	for( int i = 0 ; i < m_nVertices ; i++ ) {
		CFVector3 v;
		m.Mult(*m_ppVerticesUpdate[i],v);
		*m_ppVerticesUpdate[i] = v;
	}
	return *this;
}

CViewBaseModifier &CViewBaseModifier::Rotate(TCCFVector3 &axis,double a,TCCFVector3 &center)
{
	CFMatrix3x4 m;
	m.LoadIdentity().TranslateL(-center).RotateL(axis,a).TranslateL(center);
	return Modify(m);
}
CViewBaseModifier &CViewBaseModifier::Rotate(TCCFVector3 &base,TCCFVector3 &arrow,double a)
{
	return Rotate(arrow-base,a,base);
}
// -------------------------------------------------------------------------
void	CViewBaseModifier0::Read(CTaggedFile &f,SViewObjectMorphNames &names)
{
	#if 0
	Clear0();
	CViewBaseModifier::Read(f,names);
	m_pVertices0 = m_nVertices > 0 ? new CFVector3[m_nVertices] : NULL;
	for( int i = 0 ; i < m_nVertices ; i++ )
		m_pVertices0[i] = *m_ppVerticesUpdate[i];

	#else
	Clear0();
	f.Descend("VBM0",FALSE,0);
		CViewBaseModifier::Read(f,names);
		//f.Descend("VBMV",TRUE,0);
			m_pVertices0 = m_nVertices > 0 ? new CFVector3[m_nVertices] : NULL;
			for( int i = 0 ; i < m_nVertices ; i++ )
				m_pVertices0[i] = *m_ppVerticesUpdate[i];
				//m_pVertices0[i].Read(f);
		//f.Ascend();
	f.Ascend();
	#endif
}

CViewBaseModifier0 &CViewBaseModifier0::LoadIdentity()
{
	for( int i = 0 ; i < m_nVertices ; i++)
		*m_ppVerticesUpdate[i] = m_pVertices0[i];
	return *this;
}

void CViewBaseModifier0::operator%=(TCCFMatrix3x4 &dir)
{
	for( int i = 0 ; i < m_nVertices ; i++)
		m_pVertices0[i] = dir % m_pVertices0[i];
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
void CViewKeySpan::Tween(double f)
{
	for( int i = 0 ; i < m_nVertices ; i++ )
		*m_ppVerticesUpdate[i] = m_pVerticesFrom[i]+m_pVerticesDelta[i]*f;
	Update();
}

void CViewKeySpan::Read(CTaggedFile &f,SViewObjectMorphNames &names)
{
	f.Descend("VKFP",FALSE,0);
		CViewBaseModifier::Read(f,names);
		f.Descend("VKPV",TRUE,0);
			m_pVerticesFrom = new CFVector3[m_nVertices*2];
			m_pVerticesDelta = m_pVerticesFrom + m_nVertices;
			for( int i = 0 ; i < m_nVertices ; i++ )
				m_pVerticesFrom[i].Read(f);
			for( int i = 0 ; i < m_nVertices ; i++ ) {
				CFVector3	v;
				v.Read(f);
				m_pVerticesDelta[i] = v - m_pVerticesFrom[i];
			}
		f.Ascend();
	f.Ascend();
}

void CViewKeySpan::operator%=(TCCFMatrix3x4 &dir)
{
	for( int i = 0 ; i < m_nVertices ; i++ ) {
		m_pVerticesFrom[i] = dir % m_pVerticesFrom[i];
		m_pVerticesDelta[i] = dir % m_pVerticesDelta[i];
	}
}

// --------------------------------------------------------------------------
void CViewKeyFrameSeq::Read(CTaggedFile &f,SViewObjectMorphNames &names)
{
	f.Descend("VKFQ",FALSE,0);
		f.Descend("VKQH",TRUE,0);
			f.ReadInt(m_nSpans);
			RTCHECK(m_nSpans>0,"spans<=0 in VKFQ");
			bool bNames;
			f.ReadBool(bNames);
			RTCHECK(!bNames,"Obj names not supp in VKFQ");
			m_pfDurations = new double[m_nSpans];
			m_fFullDuration = 0;
			for( int i = 0 ; i < m_nSpans ; i++ ) {
				f.ReadDouble(m_pfDurations[i]);
				m_fFullDuration += m_pfDurations[i];
			}
		f.Ascend();
		m_pSpans = new CViewKeySpan[m_nSpans];
		for( int i = 0 ; i < m_nSpans ; i++ )	m_pSpans[i].Read(f,names);
	f.Ascend();
}

void CViewKeyFrameSeq::Tween(double fTime)
{
	ASSERT(m_nSpans>0);
	double fTime0 = 0;
	int i;
	for(i = 0 ; i < m_nSpans-1 ; i++ ) {
		if( fTime < m_pfDurations[i] ) break;
		fTime0 += m_pfDurations[i];
	}
	m_pSpans[i].Tween((fTime-fTime0)/m_pfDurations[i]);
}

void CViewKeyFrameSeq::operator%=(TCCFMatrix3x4 &dir)
{
	for( int i = 0 ; i < m_nSpans ; i++ )	m_pSpans[i] %= dir;
}

// --------------------------------------------------------------------------
void CViewKeyFrameSet::Read(CTaggedFile &f,SViewObjectMorphNames &names)
{
	f.Descend("VKFS",FALSE,0);
		f.Descend("VKSH",TRUE,0);
			f.ReadInt(m_nSeqs);
			f.SetRTEFatal(FALSE);
			m_nMods0 = 0; m_nMods = 0;
			f.ReadInt(m_nMods0);
			f.ReadInt(m_nMods);
			f.SetOK(TRUE); f.SetRTEFatal(TRUE);
			int nNames = m_nSeqs + m_nMods0 + m_nMods;
			m_pNames = nNames > 0 ? new char[nNames][40] : NULL;
			for( int i = 0 ; i < nNames ; i++ )
				f.Read(m_pNames[i],40);
		f.Ascend();
		m_pSeqs = m_nSeqs > 0 ? new CViewKeyFrameSeq[m_nSeqs] : NULL;
		for( int i = 0 ; i < m_nSeqs ; i++ )	m_pSeqs[i].Read(f,names);
		m_pMods0 = m_nMods0 > 0 ? new CViewBaseModifier0[m_nMods0] : NULL;
		for( int i = 0 ; i < m_nMods0 ; i++ )	m_pMods0[i].Read(f,names);
		m_pMods = m_nMods > 0 ? new CViewBaseModifier[m_nMods] : NULL;
		for( int i = 0 ; i < m_nMods ; i++ )	m_pMods[i].Read(f,names);
	f.Ascend();
}

int CViewKeyFrameSet::FindName(TCchar *szName,int nStartIdx,int nCount)
{
	for( int i = 0 ; i < nCount ; i++ )
		if( strcmp(m_pNames[i+nStartIdx],szName)==0 )	return i;
	RTCHECK1(0,"Unknown KFSet Name %s",szName);
	return 0;
}

void CViewKeyFrameSet::operator%=(TCCFMatrix3x4 &dir)
{
	int i;
	for( i = 0 ; i < m_nSeqs ; i++ )	m_pSeqs[i] %= dir;
	for( i = 0 ; i < m_nMods0 ; i++ )	m_pMods0[i] %= dir;
}

