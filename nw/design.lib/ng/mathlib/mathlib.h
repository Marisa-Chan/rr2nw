
#ifndef	__MATHLIB_H__
#define	__MATHLIB_H__

#ifndef	_NOAUTOINCLUDE
	#define	_NOAUTOINCLUDE
	#if !defined(__MATH_H) && !defined(_MATH_H_INCLUDED)
		#include<math.h>
	#endif
	#ifndef	__STDTYPES_H__
		#include"stdtypes.h"
	#endif
	#ifndef	__DEBUGEXT_H__
		#include"debugext.h"
	#endif
	#undef	_NOAUTOINCLUDE
#endif

// ------------------------------------------------------------------------
#if	defined(__BORLANDC__)
	#pragma	option	-a-
#elif	defined(__WATCOMC__)
	#pragma pack(1)
#else
	#error	Unknown compiler
#endif
// ------------------------------------------------------------------------
inline int    Max(int a,int b) { return a>=b?a:b; }
inline long   Max(long a,long b) { return a>=b?a:b; }
inline uint   Max(uint a,uint b) { return a>=b?a:b; }
inline ulong  Max(ulong a,ulong b) { return a>=b?a:b; }
inline double Max(double a,double b) { return a>=b?a:b; }

inline int    Min(int a,int b) { return a<=b?a:b; }
inline long   Min(long a,long b) { return a<=b?a:b; }
inline uint   Min(uint a,uint b) { return a<=b?a:b; }
inline ulong  Min(ulong a,ulong b) { return a<=b?a:b; }
inline double Min(double a,double b) { return a<=b?a:b; }
// ------------------------------------------------------------------------
#ifdef	__FILESYS_H__
	#define	___MATHLIB_DECLARE_TAGGIO	\
		bool Read(CTaggedFile &f);	\
		bool Write(CTaggedFile &f) const;
#else
	#define	___MATHLIB_DECLARE_TAGGIO
#endif
// ------------------------------------------------------------------------
struct SVector2 {
	long	x,y;
};
typedef	const SVector2 TCSVector2;

class	CVector2;
typedef	const CVector2 TCCVector2;

class	CVector2 : public SVector2 {
public:
	CVector2() {}
	CVector2(long _x, long _y) { x = _x; y = _y; }
	CVector2(TCSVector2 &v)	{ *this = *(TCCVector2*)&v; }
	CVector2 operator+() const { return *this; }
	CVector2 operator-() const { return CVector2(-x,-y); }
	CVector2 operator+(TCSVector2 &v) const { return CVector2(x+v.x,y+v.y); }
	CVector2 operator-(TCSVector2 &v) const { return CVector2(x-v.x,y-v.y); }
	CVector2 &operator+=(TCSVector2 &v) { x+=v.x; y+=v.y; return *this; }
	CVector2 &operator-=(TCSVector2 &v) { x-=v.x; y-=v.y; return *this; }

	int  operator==(TCSVector2 &v) const { return x==v.x && y==v.y; }
	int  operator!=(TCSVector2 &v) const { return x!=v.x || y!=v.y; }

	___MATHLIB_DECLARE_TAGGIO
};
// ------------------------------------------------------------------------
struct SRect2 {
	long  left,top,right,bottom;	// right>=left, bottom>=top
};
typedef const SRect2 TCSRect2;

class	CRect2;
typedef	const CRect2 TCCRect2;

class	CRect2 : public SRect2 {
public:
	CRect2() {}
	CRect2(long _left,long _top,long _right,long _bottom) {
						left=_left; top=_top; right=_right; bottom=_bottom; }
	CRect2(TCSVector2 &v1,TCSVector2 &v2) {
							left=v1.x; top=v1.y; right=v2.x; bottom=v2.x; }
	CRect2(TCSRect2 &r) { *this = *(TCCRect2*)&r; }
	TCCVector2 &LeftTop() const { return *(TCCVector2*)(void*)&left; }
	TCCVector2 &RightBottom() const { return *(TCCVector2*)(void*)&right; }
	long  Width() const { return right-left; }
	long  Height() const { return bottom-top; }
	bool  Contains(TCSVector2 &v) const {
				return v.x>=left && v.x<right && v.y>=top && v.y<bottom; }
	bool  IsEmpty() const { return left==right && top==bottom; }
	void  SetEmpty() { left=top=right=bottom=0; }
	bool  SetIntersect(TCSRect2 &r1,TCSRect2 &r2) {
		if( r1.left > r2.right || r2.left > r1.right ||
				r1.top > r2.bottom || r2.top > r1.bottom )	return FALSE;
		left = Max(r1.left,r2.left); right = Min(r1.right,r2.right);
		top = Max(r1.top,r2.top); bottom = Min(r1.bottom,r2.bottom);
		return TRUE;
	}
	void  SetUnion(TCSRect2 &r1,TCSRect2 &r2) {
		left = Min(r1.left,r2.left);	top = Min(r1.top,r2.top);
		right = Max(r1.right,r2.right);	bottom = Max(r1.bottom,r2.bottom);	}
	void  MoveBy(TCSVector2 &v) {
							left+=v.x; right+=v.x; top+=v.y; bottom+=v.y; }
	void  MoveTo(TCSVector2 &v) {
			right += v.x-left; bottom += v.y-top; left = v.x; top = v.y; }
	void  ExtendBy(TCSVector2 &v) { right += v.x; bottom += v.y; }
	void  ExtendTo(TCSVector2 &v) { right = left+v.x; bottom = top+v.y; }
	void  InflateBy(TCSVector2 &v) {
			left-=v.x/2; right+=(v.x+1)/2; top-=v.y/2; bottom+=(v.y+1)/2; }
	void  InflateTo(TCSVector2 &v) {
			right=(left+right+v.x)/2; left=right-v.x;
			bottom=(top+left+v.y)/2; top=bottom-v.y ; }

	int  operator==(TCSRect2 &r) const { return left==r.left && top==r.top &&
										right==r.right && bottom==r.bottom; }
	int  operator!=(TCSRect2 &r) const { return left!=r.left || top!=r.top ||
										right!=r.right || bottom!=r.bottom; }

	___MATHLIB_DECLARE_TAGGIO
};
// ------------------------------------------------------------------------
struct	SFVector2 {
	double	x, y;
};
typedef	const SFVector2 TCSFVector2;

class	CFVector2;
typedef	const CFVector2 TCCFVector2;

class	CFVector2 : public SFVector2 {
public:
	CFVector2() {}
	CFVector2(double _x, double _y) { x = _x; y = _y; }
	CFVector2(TCSFVector2 &v) { *this = *(TCCVector2*)&v; }
	CFVector2 operator+() const { return *this; }
	CFVector2 operator-() const { return CFVector2(-x,-y); }
	double operator*(TCSFVector2 &v) const { return x*v.x+y*v.y; }
	double operator%(TCSFVector2 &v) const { return x*v.y-y*v.x; }
	CFVector2 operator+(TCSFVector2 &v) const {
										return CFVector2(x+v.x,y+v.y); }
	CFVector2 operator-(TCSFVector2 &v) const {
										return CFVector2(x-v.x,y-v.y); }
	CFVector2 &operator+=(TCSFVector2 &v) { x+=v.x; y+=v.y; return *this; }
	CFVector2 &operator-=(TCSFVector2 &v) { x-=v.x; y-=v.y; return *this; }
	CFVector2 operator*(double f) const { return CFVector2(x*f,y*f); }
	CFVector2 &operator*=(double f) { x*=f; y*=f; return *this; }
	CFVector2 operator/(double f) const { return CFVector2(x/f,y/f); }
	CFVector2 &operator/=(double f) { x/=f; y/=f; return *this; }

	int operator==(TCSFVector2 &v) { return x==v.x&&y==v.y; }
	int operator!=(TCSFVector2 &v) { return x!=v.x||y!=v.y; }

	friend CFVector2 operator*(double f,TCCFVector2 &v) {
											return CFVector2(f*v.x,f*v.y); }
	friend double Abs(TCCFVector2 &v) { return sqrt(v.x*v.x+v.y*v.y); }

	___MATHLIB_DECLARE_TAGGIO
};
// ------------------------------------------------------------------------
struct	SFVector3 {
	double	x, y, z;
};
typedef	const SFVector3 TCSFVector3;

class	CFVector3;
typedef	const CFVector3 TCCFVector3;

class	CFVector3 : public SFVector3 {
public:
	CFVector3() {}
	CFVector3(double _x, double _y, double _z) { x = _x; y = _y; z = _z; }
	CFVector3(TCSFVector3 &v) { *this = *(TCCFVector3*)&v; }
	CFVector3	operator+() const { return *this; }
	CFVector3	operator-() const { return CFVector3(-x,-y,-z); }
	double		operator*(TCSFVector3 &v) const {
									return	x*v.x+y*v.y+z*v.z; }
	CFVector3	operator%(TCSFVector3 &v) const {
					return CFVector3(y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x); }
	CFVector3	operator+(TCSFVector3 &v) const {
									return CFVector3(x+v.x,y+v.y,z+v.z); }
	CFVector3	operator-(TCSFVector3 &v) const {
									return CFVector3(x-v.x,y-v.y,z-v.z); }
	CFVector3 &operator+=(TCSFVector3 &v) {
									x+=v.x; y+=v.y; z+=v.z; return *this; }
	CFVector3 &operator-=(TCSFVector3 &v) {
									x-=v.x; y-=v.y; z-=v.z; return *this; }
	CFVector3 operator*(double f) const { return CFVector3(x*f,y*f,z*f); }
	CFVector3 &operator*=(double f) { x*=f; y*=f; z*=f; return *this; }
	CFVector3 operator/(double f) const { return CFVector3(x/f,y/f,z/f); }
	CFVector3 &operator/=(double f) { x/=f; y/=f; z/=f; return *this; }

	int operator==(TCSFVector3 &v) { return x==v.x&&y==v.y&&z==v.z; }
	int operator!=(TCSFVector3 &v) { return x!=v.x||y!=v.y||z!=v.z; }

	friend CFVector3 operator*(double f,TCCFVector3 &v) {
										return CFVector3(f*v.x,f*v.y,f*v.z); }
	friend double Abs(TCSFVector3 &v) {
					return sqrt(v.x*v.x+v.y*v.y+v.z*v.z); }
	___MATHLIB_DECLARE_TAGGIO
};
// ------------------------------------------------------------------------
struct	SFRect2 {
	double	left,top,right,bottom;	// right>=left, bottom>=top
};
typedef	const SFRect2 TCSFRect2;

class	CFRect2;
typedef	const CFRect2 TCCFRect2;

class	CFRect2 : public SFRect2 {
public:
	CFRect2() {}
	CFRect2(double _left,double _top,double _right,double _bottom) {
						left=_left; top=_top; right=_right; bottom=_bottom; }
	CFRect2(TCCFVector2 &v1,TCCFVector2 &v2) {
							left=v1.x; top=v1.y; right=v2.x; bottom=v2.x; }
	CFRect2(TCSFRect2 &r) { *this = *(TCCRect2*)&r; }
	TCCFVector2 &LeftTop() const { return *(TCCFVector2*)(void*)&left; }
	TCCFVector2 &RightBottom() const { return *(TCCFVector2*)(void*)&right; }
	long  Width() const { return right-left; }
	long  Height() const { return bottom-top; }
	bool  Contains(TCSFVector2 &v) const {
				return v.x>=left && v.x<right && v.y>=top && v.y<bottom; }
	bool  IsEmpty() const { return left==right && top==bottom; }
	void  SetEmpty() { left=top=right=bottom=0; }
	bool  SetIntersect(TCSFRect2 &r1,TCSFRect2 &r2);
	void  SetUnion(TCSFRect2 &r1,TCSFRect2 &r2) {
		left = Min(r1.left,r2.left);	top = Min(r1.top,r2.top);
		right = Max(r1.right,r2.right);	bottom = Max(r1.bottom,r2.bottom);	}

	void  MoveBy(TCSFVector2 &v) {
							left+=v.x; right+=v.x; top+=v.y; bottom+=v.y; }
	void  MoveTo(TCSFVector2 &v) {
			right += v.x-left; bottom += v.y-top; left = v.x; top = v.y; }
	void  ExtendBy(TCSFVector2 &v) { right += v.x; bottom += v.y; }
	void  ExtendTo(TCSFVector2 &v) { right = left+v.x; bottom = top+v.y; }
	void  InflateBy(TCSFVector2 &v) {
			left-=v.x/2; right+=(v.x+1)/2; top-=v.y/2; bottom+=(v.y+1)/2; }
	void  InflateTo(TCSFVector2 &v) {
			right=(left+right+v.x)/2; left=right-v.x;
			bottom=(top+left+v.y)/2; top=bottom-v.y ; }

	int  operator==(TCSFRect2 &r) const { return left==r.left && top==r.top &&
										right==r.right && bottom==r.bottom; }
	int  operator!=(TCSFRect2 &r) const { return left!=r.left || top!=r.top ||
										right!=r.right || bottom!=r.bottom; }

	___MATHLIB_DECLARE_TAGGIO
};

// ------------------------------------------------------------------------
class	CFMatrix3x4;
typedef	const CFMatrix3x4 TCCFMatrix3x4;

class	CFMatrix3x4 {
public:
	double		m[3][4];
	CFMatrix3x4() {}
	CFMatrix3x4(double m00,double m01,double m02,double m03,
				double m10,double m11,double m12,double m13,
				double m20,double m21,double m22,double m23) {
					m[0][0]=m00; m[0][1]=m01; m[0][2]=m02; m[0][3]=m03;
					m[1][0]=m10; m[1][1]=m11; m[1][2]=m12; m[1][3]=m13;
					m[2][0]=m20; m[2][1]=m21; m[2][2]=m22; m[2][3]=m23; }
	void	Mult(TCCFVector3 &in,CFVector3 &out) const;
	void	MultF(TCCFVector3 &in,CFVector3 &out) const; // free vectors
	CFVector3	operator*(TCCFVector3 &v) const;
	CFVector3	operator%(TCCFVector3 &v) const;	// free vectors
	CFMatrix3x4	&LoadMult(TCCFMatrix3x4 &m1,TCCFMatrix3x4 &m2);
	CFMatrix3x4	operator*(TCCFMatrix3x4 &mx) const;
	CFMatrix3x4	&MultR(TCCFMatrix3x4 &mx);
	void		LoadInverse(TCCFMatrix3x4 &s);
	CFMatrix3x4	&LoadIdentity();
	CFMatrix3x4	&TranslateL(TCCFVector3 &d);
	CFMatrix3x4	&TranslateR(TCCFVector3 &d);
	CFMatrix3x4	&ScaleL(TCCFVector3 &s);
	CFMatrix3x4	&ScaleR(TCCFVector3 &s);
	CFMatrix3x4	&RotateOxL(double a);
	CFMatrix3x4	&RotateOxR(double a);
	CFMatrix3x4	&RotateOyL(double a);
	CFMatrix3x4	&RotateOyR(double a);
	CFMatrix3x4	&RotateOzL(double a);
	CFMatrix3x4	&RotateOzR(double a);

	___MATHLIB_DECLARE_TAGGIO
};
// ------------------------------------------------------------------------
#undef	___MATHLIB_DECLARE_TAGGIO
// ------------------------------------------------------------------------
// Revert structure packing
#if	defined(__BORLANDC__)
	#pragma	option	-a.
#elif	defined(__WATCOMC__)
	#pragma pack()
#endif
// ------------------------------------------------------------------------

#endif	//	__MATHLIB_H__
