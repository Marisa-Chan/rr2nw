#ifndef	__DYNAMIC_H__
#define	__DYNAMIC_H__

enum {
	BF_NONE = 0,
	BF_BUMPSTATIC,
	BF_BUMPDYNAMIC,
	BF_BUMPSHOOT,
	BF_BUMPLAND,
	BF_BUMPSHELTER,

	BF1_ADVANCED = 1
};
struct	SBonusDef;
struct	SBumpDef0	{
	#ifdef _DEBUG
		enum TYPE { T_BUMPDEF0, T_BUMPDEF, T_USER } eType;
	#endif
	CFVector3	start, vel, vel1;
	double		fTime;
	double		fRadius;
	dword		dwFlags1;
	SBumpDef0() { IF_DEBUG(eType = T_BUMPDEF0); }
};
struct	SBumpDef : public SBumpDef0	{
public:
	double		fMass;
	int			nBumpFlags;
	SBonusDef	*pBonus;
	void		*pBumpRef;
	SBumpDef() { IF_DEBUG(eType = T_BUMPDEF); dwFlags1 = 0; }
};
class	CBumpFigures;
bool	Bump(SBumpDef &def,SBumpDef &def1,int nFlag = BF_BUMPDYNAMIC);
bool	Bump(SBumpDef &def,CBumpFigures &figs);
//void SetProcessBonusProc(bool (*Proc)(SBonusDef *,SBonusDef *));
typedef	bool (*TPProcessBonusProc)(SBonusDef *,SBonusDef *);
void SetProcessBonusProc(TPProcessBonusProc Proc);
// When using Bump for shoot/dynamic shoot must be first arg!!!

// -------------------------------------------------------------------------
class	CViewOrdered;

#define	ASSERT_VD(X)	ASSERT((X)->m_dwVDTag==0x0BADDAB0)
#define	ASSERT_VD0(X)	ASSERT(!(X)||(X)->m_dwVDTag==0x0BADDAB0)

class	CViewDynamic : public CTypedObject	{
	DECLARETYPEDOBJECT(CViewDynamic,CTypedObject)
public:
	enum DFORM { DT_NONE, DT_STICKLAND, DT_SPHERIC, DT_LINEAR };
	enum { DT_FIRSTBUMP = DT_SPHERIC };
protected:
	IF_DEBUG(dword m_dwVDTag);
	CViewDynamic	*m_pNext, *m_pPrev;
	CVector2		m_cell;
	dword			m_dwLights, m_dwLightMask;
	enum DFORM m_eDType;
	IF_DEBUG(TCvoid	*m_pList);
	void Init() { m_dwLightMask = -1L; }
	CViewDynamic() { m_eDType = DT_NONE; Init();
						IF_DEBUG(m_pList=NULL);	IF_DEBUG(m_dwVDTag=0x0BADDAB0); }
	virtual ~CViewDynamic() {}
	friend class CViewDynamicList;
	friend class CViewDynamicListLoop;
	friend class CViewOrder;
	friend class CViewBody;
	friend class CViewObjectRef;
	friend class CLandDynamicMap;
public:
	bool	IsBumpDynamic() { return m_eDType >= DT_FIRSTBUMP; }
	void	TransformInv(TCCFMatrix3x4 &inv);
	void	TransformInv(TCCFVector3 &inv);
	CViewDynamic *Next() const {
		ASSERT(m_pList&&(!m_pNext||m_pNext->m_pList==m_pList)); return m_pNext; }
	CViewDynamic *Prev() const {
		ASSERT(m_pList&&(!m_pPrev||m_pPrev->m_pList==m_pList)); return m_pPrev; }
	virtual void Draw() = 0;
	CVector2 &Cell() { return m_cell; }
	DFORM DForm() const { return m_eDType; }
};

class	CViewStickLandDynamic : public CViewDynamic {
protected:
	CViewStickLandDynamic() : CViewDynamic() { m_eDType = DT_STICKLAND; }
public:
};

class	CViewBumpDynamic : public CViewDynamic {
public:
	enum BUMPABILITY { BY_NONE = 0, BY_WEAK = 1, BY_STRONG = 2 };
	BUMPABILITY m_eBumpability;
	enum { BY_THRESHOLD = 3 };
protected:
	SBumpDef		m_bump;
	CViewOrdered	*m_pPromoteNode;
	double			m_fSortValue;
	void Init() { CViewDynamic::Init(); m_bump.pBonus = NULL; m_eBumpability = BY_NONE; }
	friend class CViewDynamicList;
	friend class CViewDynamicListLoop;
	friend class CLandDynamicMap;

public:
	SBumpDef &BumpDef() { return m_bump; }
	void SetPromoteNode(CViewOrdered *p) { ASSERT_VD(this); m_pPromoteNode = p; }
	BUMPABILITY Bumpability() const { ASSERT_VD(this); return m_eBumpability; }
	bool IsBumpable(BUMPABILITY eBump) const { ASSERT_VD(this);
							  	return m_eBumpability+eBump>=BY_THRESHOLD; }
	friend bool	Bump(CViewBumpDynamic *pd1,CViewBumpDynamic *pd2,
										int nFlag = BF_BUMPDYNAMIC) {
			ASSERT_VD(pd1); ASSERT_VD(pd2);
			ASSERT(pd1->IsBumpDynamic() && pd2->IsBumpDynamic());
			if( pd1->m_eBumpability+pd2->m_eBumpability<BY_THRESHOLD) return FALSE;
			return ::Bump(pd1->m_bump,pd2->m_bump,nFlag); }
};

class	CViewDynamicList	{
protected:
	CViewDynamic	*m_pFirst, *m_pLast;
	static	CViewDynamicList	m_wasteBox;
	friend class CViewDynamic;
	friend class CViewDynamicListLoop;
	void AssertContains(CViewDynamic *p) {
		#ifdef _DEBUG
			for( CViewDynamic *pd = m_pFirst ; pd ; pd = pd->m_pNext ) {
				ASSERT(pd->m_pList==this); if(pd==p) return; }
			ASSERT(0);
		#endif
	}
	void ChangeOwner(void *pNewOwner) {
		(void)pNewOwner;
		#ifdef _DEBUG
			if(m_pFirst)
			for( CViewDynamic *pd = m_pFirst ;; pd = pd->m_pNext ) {
				ASSERT(pd->m_pList==this); pd->m_pList = pNewOwner;
				if(pd==m_pLast) { ASSERT(!pd->m_pNext); break; }
			}
		#endif
	}
public:
	CViewDynamicList() { m_pFirst = m_pLast = NULL; }
	void Clear(bool bToWasteBox=TRUE) {
		if( bToWasteBox )
			m_wasteBox.Append(*this);
		else {
			ChangeOwner(NULL); m_pFirst = m_pLast = NULL;
		}
	}
	void Move(CViewDynamicList &list) {
		list.ChangeOwner(this);
		m_pFirst = list.m_pFirst; list.m_pFirst = NULL;
		m_pLast = list.m_pLast; list.m_pLast = NULL;
	}
	void Move(CViewDynamicListLoop &list);
	void Append(CViewDynamicList &list) {
		ASSERT(!m_pFirst||m_pFirst->m_pList==this);
		ASSERT(!m_pLast||m_pLast->m_pList==this);
		list.ChangeOwner(this);
		m_pLast ? m_pLast->m_pNext = list.m_pFirst : m_pFirst = list.m_pFirst;
		if( list.m_pFirst )	{ list.m_pFirst->m_pPrev = m_pLast; m_pLast = list.m_pLast; }
		list.m_pFirst = list.m_pLast = NULL;
	}
	void Move(CViewDynamic *pd,CViewDynamicList &list) {
		ASSERT(!m_pFirst||m_pFirst->m_pList==this);
		ASSERT(!m_pLast||m_pLast->m_pList==this);
		list.AssertContains(pd); IF_DEBUG(pd->m_pList = this);
		pd->m_pNext ? pd->m_pNext->m_pPrev = pd->m_pPrev : list.m_pLast = pd->m_pPrev;
		pd->m_pPrev ? pd->m_pPrev->m_pNext = pd->m_pNext : list.m_pFirst = pd->m_pNext;
		pd->m_pNext = NULL; pd->m_pPrev = m_pLast;
		m_pLast ? m_pLast->m_pNext = pd : m_pFirst = pd; m_pLast = pd;
	}
	void Load(CViewDynamic *pd) {
		ASSERT(!m_pFirst||m_pFirst->m_pList==this);
		ASSERT(!m_pLast||m_pLast->m_pList==this);
		ASSERT_VD(pd); ASSERT(!pd->m_pList); IF_DEBUG(pd->m_pList=this);
		pd->m_pPrev = m_pLast; pd->m_pNext = NULL;
		m_pLast = (m_pLast ? m_pLast->m_pNext = pd : m_pFirst = pd); }
	void Remove(CViewDynamic *pd,bool bToWasteBox=TRUE) {
		if( bToWasteBox ) m_wasteBox.Move(pd,*this);
		else {
			AssertContains(pd); IF_DEBUG(pd->m_pList=NULL);
			pd->m_pNext ? pd->m_pNext->m_pPrev = pd->m_pPrev : m_pLast = pd->m_pPrev;
			pd->m_pPrev ? pd->m_pPrev->m_pNext = pd->m_pNext : m_pFirst = pd->m_pNext;
		}
	}
	bool Contains(const CViewDynamic *p) const {
		for( CViewDynamic *pd = m_pFirst ; pd ; pd = pd->m_pNext ) {
			ASSERT(pd->m_pList==this); if(pd==p) return TRUE; }
		return FALSE; }
	CViewDynamic *First() const { ASSERT(!m_pFirst||m_pFirst->m_pList==this); return m_pFirst; }
	CViewDynamic *Last() const { ASSERT(!m_pLast||m_pLast->m_pList==this); return m_pLast; }
	void Draw() { for( CViewDynamic *pd = m_pFirst ; pd != NULL ;
		pd = pd->m_pNext ) { ASSERT(pd->m_pList==this); pd->Draw(); } Clear(); }
	void SortZOrder();
	void ZSortDrawNDrop() { if(m_pFirst) { SortZOrder(); Draw(); } }
	void	TransformInv(TCCFMatrix3x4 &inv);
	void	TransformInv(TCCFVector3 &inv);
	static	CViewDynamicList &WasteBox() { return m_wasteBox; }
};

class	CViewDynamicListLoop {
protected:
	CViewDynamic	*m_pFirst;
	friend class CViewDynamicList;
	void ChangeOwner(void *pNewOwner) {
		(void)pNewOwner;
		#ifdef _DEBUG
			ASSERT(!m_pFirst||m_pFirst->m_pNext);
			if(m_pFirst) {
				for( CViewDynamic *pd = m_pFirst ;;) {
					ASSERT(pd->m_pList==this); pd->m_pList = pNewOwner;
					if( (pd = pd->m_pNext) == m_pFirst ) break;
				}
			}
		#endif
	}
public:
	CViewDynamicListLoop() { m_pFirst = NULL; }
	void Clear() { ChangeOwner(NULL); m_pFirst = NULL; }
	CViewDynamic *First() { return m_pFirst; }
	void Move(CViewDynamicList &list) {
		list.ChangeOwner(this);
		if( list.m_pFirst ) {
			ASSERT(list.m_pFirst->m_pPrev==NULL); list.m_pFirst->m_pPrev = list.m_pLast;
			ASSERT(list.m_pLast->m_pNext==NULL); list.m_pLast->m_pNext = list.m_pFirst;
		}
		m_pFirst = list.m_pFirst; list.m_pFirst = list.m_pLast = NULL;
		ASSERT(!m_pFirst||m_pFirst->m_pNext);
	}
	void Move(CViewDynamic *pd,CViewDynamicList &list) {
		ASSERT(!m_pFirst||m_pFirst->m_pList==this);
		ASSERT(!m_pFirst||m_pFirst->m_pNext);
		list.AssertContains(pd); IF_DEBUG(pd->m_pList = this);
		pd->m_pNext ? pd->m_pNext->m_pPrev = pd->m_pPrev : list.m_pLast = pd->m_pPrev;
		pd->m_pPrev ? pd->m_pPrev->m_pNext = pd->m_pNext : list.m_pFirst = pd->m_pNext;
		if( m_pFirst ) {
			CViewDynamic *pLast = m_pFirst->m_pPrev;
			pd->m_pNext = m_pFirst; pd->m_pPrev = pLast;
			pLast->m_pNext = m_pFirst->m_pPrev = pd;
		}
		else
			m_pFirst = pd->m_pNext = pd->m_pPrev = pd;
		ASSERT(!m_pFirst||m_pFirst->m_pNext);
	}
	void Draw(bool bReleaseObjects) {
		if( m_pFirst )
		for( CViewDynamic *pd = m_pFirst ;;	) {	ASSERT(pd->m_pList==this);
			pd->Draw(); if((pd = pd->m_pNext)==m_pFirst) break; } if( bReleaseObjects ) Clear(); }
	void ZSortDrawNDrop() { if(m_pFirst) { CViewDynamicList list; list.Move(*this); list.ZSortDrawNDrop(); } }
};
inline void CViewDynamicList::Move(CViewDynamicListLoop &list)
{
	list.ChangeOwner(this);
	m_pFirst = list.m_pFirst;
	if( list.m_pFirst ) {
		m_pLast = list.m_pFirst->m_pPrev;
		m_pFirst->m_pPrev = m_pLast->m_pNext = NULL;
	} else m_pLast = NULL;
	list.m_pFirst = NULL;
}

class	CViewSphericDynamic	: public CViewBumpDynamic {
protected:
	CFVector3		m_dynBase, m_dynBase1;
						// m_dynBase1 is modified through order
						// radius = m_bump.fRadius
	friend class CViewDynamicList;
	friend class CViewOrder;
	friend class CViewBody;
	friend class CLandDynamicMap;
	friend class CViewBush;
public:
	CViewSphericDynamic() { m_eDType = DT_SPHERIC; }
	void	TransformInv(TCCFMatrix3x4 &inv);
	void	TransformInv(TCCFVector3 &inv);
};

class	CViewLinearDynamic : public CViewSphericDynamic	{
protected:
	CFVector3	m_dynEnd, m_dynEnd1, m_dynVel;
	double		m_fDrawT1, m_fDrawT2;
						// m_dynEnd1 is modified through order
	friend class CViewOrder;
	friend class CViewBody;
public:
	CViewLinearDynamic() { m_eDType = DT_LINEAR; }
	void	TransformInv(TCCFMatrix3x4 &inv);
	void	TransformInv(TCCFVector3 &inv);
};


class	CViewFigure;

struct	SBumpFigures	{
	CViewFigure	**ppFigures;
	int			nFigures;
	SBumpFigures() { ppFigures = NULL; nFigures = 0; }
	~SBumpFigures();
	void Alloc(int nFigures);
};

struct	SBumpFiguresRef {
	SBumpFigures	*pBase;
	CFVector3		vel;
	double			fTime;
	bool			bActive;
	SBumpFiguresRef() { bActive = FALSE; }
	bool	Bump(SBumpDef0 &def);
};



#endif	// __DYNAMIC_H__

