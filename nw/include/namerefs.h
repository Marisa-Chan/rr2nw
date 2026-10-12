#ifndef	__NAMEREFS_H__
#define	__NAMEREFS_H__

class CNameDecl {
protected:
	int		m_nDimension;
	byte	*m_pData;
public:
	TCchar *Name() { return (char*)(m_pData+m_nDimension*sizeof(void*)); }
protected:
	void *&Elem(int n) { RTCHECK2((uint)n<(uint)m_nDimension,"Name dimension exceeded: %s[%d]",Name(),n); return ((void**)m_pData)[n]; }
public:
	CNameDecl() { m_nDimension = 0; m_pData = NULL; }
	~CNameDecl() { delete [] m_pData; }
	void Read(CTaggedFile &f);
	void Resolve(int n,void *pObject) { ASSERTMSG2((uint)n<(uint)m_nDimension,"Name dimension exceeded: %s[%d]",Name(),n); ASSERTMSG2(!Elem(n),"Duplicated: %s[%d]",Name(),n); Elem(n) = pObject; }
	void FinishResolve() { IF_DEBUG(for(int i = 0 ; i < m_nDimension ; i++ ) ASSERTMSG2(Elem(i),"Unresolved: %s[%d]",Name(),i)); }
	void *operator[](int n) { return Elem(n); }
	int  Count() const { return m_nDimension; }
};
class CNameDecls {
protected:
	int			m_nDecls;
	CNameDecl	*m_pDecls;
	friend class CNameDef;
	friend class CNameRef;
	void Resolve(CNameDef &ref,void *pObject);
	void *Resolve(CNameRef &ref);
public:
	CNameDecls() { m_nDecls = 0; m_pDecls = NULL; }
	~CNameDecls() { delete [] m_pDecls; }
	void Read(CTaggedFile &f);
	void FinishResolve() { IF_DEBUG(for( int i = 0 ; i < m_nDecls ; i++ ) m_pDecls[i].FinishResolve()); }
	CNameDecl &operator[](TCchar *szName);
	CNameDecl &operator[](int n) { RTCHECK((uint)n<(uint)m_nDecls,"Decl idx exceeded"); return m_pDecls[n]; }
	CNameDecl *Lookup(TCchar *szName);
	int Count() const { return m_nDecls; }
};
class CNameDef {
protected:
	int		m_nName, m_nIdx;
	friend class CNameDecls;
public:
	CNameDef(CTaggedFile &f) { f.ReadInt(m_nName); f.ReadInt(m_nIdx); }
	~CNameDef() { ASSERTMSG(m_nName<0,"Unresolved nameref"); }
	void Resolve(void *pObject,CNameDecls &decls);
	bool IsEmpty() const { if(m_nName==-1) { ASSERT(m_nIdx==-1); return TRUE; } return FALSE; }
	void AssertEmpty() const { ASSERTMSG(IsEmpty(),"This object can't be named"); }
};
class CNameRef {
protected:
	int		m_nName, m_nIdx;
	friend class CNameDecls;
public:
	CNameRef(CTaggedFile &f) { f.ReadInt(m_nName); f.ReadInt(m_nIdx); }
	void *Resolve(CNameDecls &decls);
};
inline void CNameDecls::Resolve(CNameDef &def,void *pObject) {
	if( def.m_nName >= 0 ) { ASSERT(def.m_nIdx >= 0); ASSERT(def.m_nName<m_nDecls);
		m_pDecls[def.m_nName].Resolve(def.m_nIdx,pObject); } }
inline void *CNameDecls::Resolve(CNameRef &ref) {
	ASSERT( ref.m_nName >= 0 && ref.m_nIdx >= 0); ASSERT(ref.m_nName < m_nDecls);
	return m_pDecls[ref.m_nName][ref.m_nIdx]; }
inline void CNameDef::Resolve(void *pObject,CNameDecls &decls) {
	decls.Resolve(*this,pObject); IF_DEBUG(m_nName = -1); }
inline void *CNameRef::Resolve(CNameDecls &decls) {
	return decls.Resolve(*this); }

#endif	// __NAMEREFS_H__

