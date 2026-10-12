#define	LAST_H__VIEW
#include"game.h"

#include<stdlib.h>

#if _USE_OPENGL
	#ifdef	__NT__
		#include<windows.h>
	#endif
	#include<GL/gl.h>
	#include<gl3de.h>
#endif

#include"brend.h"

#ifndef	_VIEW_ORTHO
	#define	_VIEW_ORTHO 0
#else
	#undef	_VIEW_ORTHO
	#define	_VIEW_ORTHO 1
#endif

//const int nIntegerShift = 8;
//const int nIntegerPrec = 1<<nIntegerShift;
//const double fIntegerPrec = nIntegerPrec;
//const double fIntegerPrec_1 = 1./nIntegerPrec;

inline void D2INT(int *p,double d) { *p=d; }
/*#pragma aux D2INT = \
	"fistp	dword ptr [eax]"	parm [eax] [8087]*/

inline int	SHLD_Div(int num,int den,int shift) { return (int)((1<<shift)*(double)num/den); }
/*#ifdef __WATCOMC__
#pragma aux SHLD_Div = \
	"sub  edx,edx"	\
	"shld edx,eax,cl"	\
	"shl  eax,cl"	\
	"idiv ebx"	\
	parm [eax] [ebx] [ecx] modify [edx] value [eax]
#endif*/

inline int	SHLD_IDiv(int num,int den,int shift) { return (int)((1<<shift)*(double)num/den); }
/*#ifdef __WATCOMC__
#pragma aux SHLD_IDiv = \
	"cdq"	\
	"shld edx,eax,cl"	\
	"shl  eax,cl"	\
	"idiv ebx"	\
	parm [eax] [ebx] [ecx] modify [edx] value [eax]
#endif*/

inline int	Mul_SHRD(int mul1,int mul2,int shift) { return (int)(mul1*(double)mul2/(1<<shift)); }
/*#ifdef __WATCOMC__
#pragma aux Mul_SHRD = \
	"imul ebx"	\
	"shrd eax,edx,cl"	\
	parm [eax] [ebx] [ecx] modify [edx] value [eax]
#endif*/

inline double ScalarProdNormal(uint nNormalType,TCCFVector3 &n,TCCFVector3 &v)
{
	switch(nNormalType) {
		case 0:	return v.x;
		case 1:	return v.y;
		case 2: return v.z;
		case 3: return -v.x;
		case 4: return -v.y;
		case 5: return -v.z;
		default:	return n*v;
	}
}

inline double ScalarProdNormal(uint nNormalType,TCCFVector3 &n,SGRLight &l)
{
	switch(nNormalType) {
		case 0:	return l.x;
		case 1:	return l.y;
		case 2: return l.z;
		case 3: return -l.x;
		case 4: return -l.y;
		case 5: return -l.z;
		default:	return n.x*l.x+n.y*l.y+n.z*l.z;
	}
}

//inline	int DummyInlineForWatcom();
uint	DefineNormalType(TCCFVector3 &n);
extern int __nNormalsCnt,__nNormalsOpt;


inline void	CalcBounds(TCCFVector3 &v,CFVector3 &minBound,CFVector3 &maxBound) {
	if( v.x < minBound.x )	minBound.x = v.x;
	if( v.x > maxBound.x )	maxBound.x = v.x;
	if( v.y < minBound.y )	minBound.y = v.y;
	if( v.y > maxBound.y )	maxBound.y = v.y;
	if( v.z < minBound.z )	minBound.z = v.z;
	if( v.z > maxBound.z )	maxBound.z = v.z;
}

extern	int	__bViewHide;


class	CViewBush : public CViewOrdered {
protected:
	b_TBush	m_bush;

	virtual VORDTYPE Type() const { return VOT_BUSH; }

public:
	CViewBush() { m_bEmbedded = FALSE; }
    double     Radius0() const { return m_bush.Radius0(); }
    double     Radius()  const { return m_bush.Radius(); }
    CFVector3  Center()  const { return m_bush.Center(); }

    void Read( CTaggedFile &f ) { m_bush.Read(f); }
    virtual void Draw() {
		bool bDynamics = m_dynamicList.First() || m_afterList.First();
		double fBushCenter = Abs2(Center()-CViewObject::m_currObjViewPoint);
		m_dynamicList.SortZOrder();
		for( CViewDynamic *pd = m_dynamicList.First(), *pd1 ; pd!=NULL ; pd = pd1 ) {
			pd1 = pd->Next();
			if( Abs2(((CViewSphericDynamic*)pd)->m_dynBase-CViewObject::m_currObjViewPoint) > fBushCenter ) {
				m_dynamicList.Remove(pd);
				pd->Draw();
			}
		}
		if( !bDynamics || !__bViewHide ) m_bush.Draw();
		m_dynamicList.Draw();
		m_afterList.ZSortDrawNDrop();
	}
	virtual	bool Bump(SBumpDef0 &def) { (void)def; return FALSE; }
	virtual	double ComputeBSPError() { return 1e10; }
	virtual double ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d) { (void)pt; (void)n; (void)d; return 1e10; }
};

