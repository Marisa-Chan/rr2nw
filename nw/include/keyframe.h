#ifndef	__KEYFRAME_H__
#define	__KEYFRAME_H__


class	CViewBaseModifier0;

class	CViewBaseModifier {
protected:
	int m_nVertices, m_nFigures, m_nClusters;
	CFVector3	**m_ppVerticesUpdate;
	CViewFigure	**m_ppFiguresUpdate;
	CViewMorphedOrder  **m_ppClustersUpdate;
	void Init() { m_ppVerticesUpdate = NULL; m_ppClustersUpdate = NULL;
			m_ppFiguresUpdate = NULL; m_nVertices = m_nFigures = m_nClusters = 0; }
	static	CViewBaseModifier0	m_null;
public:
	CViewBaseModifier() { Init(); }
	void Clear() { delete [] m_ppVerticesUpdate; delete [] m_ppClustersUpdate;
					delete [] m_ppFiguresUpdate; Init(); }
	~CViewBaseModifier() { Clear(); }
	void Read(CTaggedFile &f,SViewObjectMorphNames &names);
	CFVector3 &Vertex(int i) { ASSERT((uint)i<m_nVertices); return *m_ppVerticesUpdate[i]; }
	int VertexCount() const { return m_nVertices; }
	void Update();

	CViewBaseModifier &Translate(TCCFVector3 &offset);
	CViewBaseModifier &RotateOx(double a,TCCFVector3 &center);
	CViewBaseModifier &RotateOy(double a,TCCFVector3 &center);
	CViewBaseModifier &RotateOz(double a,TCCFVector3 &center);
	CViewBaseModifier &Rotate(TCCFVector3 &axis,double a,TCCFVector3 &center);
	CViewBaseModifier &Rotate(TCCFVector3 &base,TCCFVector3 &arrow,double a);
	CViewBaseModifier &Modify(TCCFMatrix3x4 &m);

 	static	CViewBaseModifier	&Null();
};

class	CViewBaseModifier0 : public CViewBaseModifier {
protected:
	CFVector3	*m_pVertices0;
	void Init0() { m_pVertices0 = NULL; }
	void Clear0() { delete [] m_pVertices0; Init(); }
public:
	CViewBaseModifier0() { Init0(); }
	void Clear() { CViewBaseModifier::Clear(); Clear0(); }
	~CViewBaseModifier0() { Clear0(); }
	void Read(CTaggedFile &f,SViewObjectMorphNames &names);
	TCCFVector3 &Vertex0(int i) { ASSERT((uint)i<m_nVertices); return m_pVertices0[i]; }

	CViewBaseModifier0 &LoadIdentity();


	void operator%=(TCCFMatrix3x4 &dir);

	static	CViewBaseModifier0	&Null() { return m_null; }
};
inline	CViewBaseModifier	&CViewBaseModifier::Null() { return m_null; }


class	CViewKeySpan : public CViewBaseModifier {
protected:
	CFVector3	*m_pVerticesFrom, *m_pVerticesDelta;
public:
	CViewKeySpan() { m_pVerticesFrom = m_pVerticesDelta = NULL; }
	~CViewKeySpan() { delete [] m_pVerticesFrom; }
	void Read(CTaggedFile &f,SViewObjectMorphNames &names);
	void Tween(double f);
	void operator%=(TCCFMatrix3x4 &dir);
};

class	CViewKeyFrameSeq {
protected:
	int m_nSpans;
	CViewKeySpan	*m_pSpans;
	double	*m_pfDurations;
	double	m_fFullDuration;
public:
	CViewKeyFrameSeq() { m_pSpans = NULL; m_pfDurations = NULL; m_nSpans = 0; }
	~CViewKeyFrameSeq() { delete [] m_pSpans; delete [] m_pfDurations; }
	void Read(CTaggedFile &f,SViewObjectMorphNames &names);
	void Tween(double fTime);
	double Duration() const { return m_fFullDuration; }
	void operator%=(TCCFMatrix3x4 &dir);
};

class	CViewKeyFrameSet {
protected:
	int m_nSeqs, m_nMods, m_nMods0;
	CViewKeyFrameSeq	*m_pSeqs;
	CViewBaseModifier0	*m_pMods0;
	CViewBaseModifier	*m_pMods;
	char (*m_pNames)[40];
	int FindName(TCchar *szName,int nStartIdx,int nCount);
public:
	CViewKeyFrameSet() { m_pSeqs = NULL; m_pMods0 = NULL; m_pMods = NULL; m_nSeqs = m_nMods0 = m_nMods = 0; }
	~CViewKeyFrameSet() { delete [] m_pSeqs; delete [] m_pMods0; delete [] m_pMods; delete [] m_pNames; }
	void Read(CTaggedFile &f,SViewObjectMorphNames &names);
	CViewKeyFrameSeq &Seq(TCchar *szName) {	return m_pSeqs[FindName(szName,0,m_nSeqs)]; }
	CViewBaseModifier0 &Mod0(TCchar *szName) { return m_pMods0[FindName(szName,m_nSeqs,m_nMods0)]; }
	CViewBaseModifier &Mod(TCchar *szName) { return m_pMods[FindName(szName,m_nSeqs+m_nMods0,m_nMods)]; }

	void operator%=(TCCFMatrix3x4 &dir);
};

#endif	// __KEYFRAME_H__

