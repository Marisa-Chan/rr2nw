
#ifndef	__MATHLIB_H__
#define	__MATHLIB_H__

#include<stdlib.h>

#ifndef	_NOAUTOINCLUDE
	#define	_NOAUTOINCLUDE
	#if !defined(__STDIO_H) && !defined(_STDIO_H_INCLUDED)
		#include<stdio.h>
	#endif

	#if !defined(__MATH_H) && !defined(_MATH_H_INCLUDED)
		#include<math.h>
	#endif
	#ifndef	__STDTYPES_H__
		#include"stdtypes.h"
	#endif
	//#ifndef	__DEBUGEXT_H__
	//	#include"debugext.h"
	//#endif
	#undef	_NOAUTOINCLUDE
#endif

// ------------------------------------------------------------------------
#if	defined(__BORLANDC__)
	#pragma	option	-a-
#elif defined(__WATCOMC__)
	#pragma pack(1)
#elif !defined(__sgi)
	#error	Unknown compiler
#endif
// ------------------------------------------------------------------------
/* Constants rounded for 21 decimals. */
#if defined(__sgi)
	#undef	M_E
	#undef  M_LOG2E
	#undef  M_LOG10E
	#undef  M_LN2
	#undef  M_LN10
	#undef  M_PI
	#undef  M_PI_2
	#undef  M_PI_4
	#undef  M_1_PI
	#undef  M_2_PI
	#undef  M_2_SQRTPI
	#undef  M_SQRT2
	#undef  M_SQRT1_2
#endif
#if	!defined(__BORLANDC__)
	#define M_E         2.71828182845904523536
	#define M_LOG2E     1.44269504088896340736
	#define M_LOG10E    0.434294481903251827651
	#define M_LN2       0.693147180559945309417
	#define M_LN10      2.30258509299404568402
	#define M_PI        3.14159265358979323846
	#define M_PI_2      1.57079632679489661923
	#define M_PI_4      0.785398163397448309616
	#define M_1_PI      0.318309886183790671538
	#define M_2_PI      0.636619772367581343076
	#define M_1_SQRTPI  0.564189583547756286948
	#define M_2_SQRTPI  1.12837916709551257390
	#define M_SQRT2     1.41421356237309504880
	#define M_SQRT_2    0.707106781186547524401
#endif
// ------------------------------------------------------------------------
inline byte   Max(byte a,byte b) { return a>=b?a:b; }
inline int    Max(int a,int b) { return a>=b?a:b; }
inline long   Max(long a,long b) { return a>=b?a:b; }
inline uint   Max(uint a,uint b) { return a>=b?a:b; }
inline ulong  Max(ulong a,ulong b) { return a>=b?a:b; }
inline double Max(double a,double b) { return a>=b?a:b; }

inline byte   Min(byte a,byte b) { return a<=b?a:b; }
inline int    Min(int a,int b) { return a<=b?a:b; }
inline long   Min(long a,long b) { return a<=b?a:b; }
inline uint   Min(uint a,uint b) { return a<=b?a:b; }
inline ulong  Min(ulong a,ulong b) { return a<=b?a:b; }
inline double Min(double a,double b) { return a<=b?a:b; }

inline int	  MinMax(int x,int l,int r) { return x<l?l:(x>r?r:x); }
inline long	  MinMax(long x,long l,long r) { return x<l?l:(x>r?r:x); }
inline uint	  MinMax(uint x,uint l,uint r) { return x<l?l:(x>r?r:x); }
inline ulong  MinMax(ulong x,ulong l,ulong r) { return x<l?l:(x>r?r:x); }
inline double MinMax(double x,double l,double r) { return x<l?l:(x>r?r:x); }

//template<class T> inline T	Max(T a,T b) { return a>=b?a:b; }
//template<class T> inline T	Min(T a,T b) { return a<=b?a:b; }
//template<class T> inline T	MinMax(T x,T l,T r) { return x<l?l:(x>r?r:x); }

inline double AbsNoLess(double val,double limit) { return val >= 0 ? Max(val,limit) : Min(val,-limit); }
inline double AbsNoGreater(double val,double limit) { return val >= 0 ? Min(val,limit) : Max(val,-limit); }

inline int	  Round(double f) { return (int)(f>=0?f+0.5:f-0.5); }
inline double Random(double l,double r) { return l+(r-l)*rand()/(double)RAND_MAX; }
#ifdef	__DEBUGEXT_H__
	inline void FPCheckNAN(TCdouble &d) { (void)d; ASSERT( (*((word*)&d+3)&0x7FF0)!=0x7FF0 ); }
#endif
// ------------------------------------------------------------------------
#ifdef	__FILESYS_H__
	#ifdef	_BIG_ENDIAN
		#define	___MATHLIB_DECLARE_TAGGIO	\
			bool Read(CTaggedFile &f);
	#else
		#define	___MATHLIB_DECLARE_TAGGIO	\
			bool Read(CTaggedFile &f);	\
			bool Write(CTaggedFile &f) const;
	#endif
#else
	#define	___MATHLIB_DECLARE_TAGGIO
#endif

#define	__MATHLIB_INHERIT_STRUCT(NAME)	\
		C##NAME() {}	\
		C##NAME(TCS##NAME &s)	{ *(S##NAME*)this = s; }	\
		C##NAME &operator=(TCS##NAME &s) { *(S##NAME*)this = s; return *this; }
// ------------------------------------------------------------------------
struct	SVector2;
typedef	const SVector2 TCSVector2;
class	CVector2;
typedef	const CVector2 TCCVector2;

struct SVector2 {
	long	x,y;

	CVector2 operator+() const;
	CVector2 operator-() const;
	CVector2 operator+(TCSVector2 &v) const;
	CVector2 operator-(TCSVector2 &v) const;
	CVector2 operator*(int n) const;
	CVector2 operator/(int n) const;
	CVector2 operator<<(int n) const;
	CVector2 operator>>(int n) const;
	CVector2 operator&(int n) const;
	CVector2 operator|(int n) const;
	SVector2 &operator+=(TCSVector2 &v) { x+=v.x; y+=v.y; return *this; }
	SVector2 &operator-=(TCSVector2 &v) { x-=v.x; y-=v.y; return *this; }
	SVector2 &operator*=(int n) { x*=n; y*=n; return *this; }
	SVector2 &operator/=(int n) { x/=n; y/=n; return *this; }
	SVector2 &operator<<=(int n) { x<<=n; y<<=n; return *this; }
	SVector2 &operator>>=(int n) { x>>=n; y>>=n; return *this; }
	SVector2 &operator&=(int n) { x&=n; y&=n; return *this; }
	SVector2 &operator|=(int n) { x|=n; y|=n; return *this; }

	int  operator==(TCSVector2 &v) const { return x==v.x && y==v.y; }
	int  operator!=(TCSVector2 &v) const { return x!=v.x || y!=v.y; }

	___MATHLIB_DECLARE_TAGGIO
};
class	CVector2 : public SVector2 {
public:
	__MATHLIB_INHERIT_STRUCT(Vector2)
	CVector2(long _x, long _y) { x = _x; y = _y; }
};
inline CVector2 SVector2::operator+() const { return *this; }
inline CVector2 SVector2::operator-() const { return CVector2(-x,-y); }
inline CVector2 SVector2::operator+(TCSVector2 &v) const { return CVector2(x+v.x,y+v.y); }
inline CVector2 SVector2::operator-(TCSVector2 &v) const { return CVector2(x-v.x,y-v.y); }
inline CVector2 SVector2::operator*(int n) const { return CVector2(x*n,y*n); }
inline CVector2 SVector2::operator/(int n) const { return CVector2(x/n,y/n); }
inline CVector2 SVector2::operator<<(int n) const { return CVector2(x<<n,y<<n); }
inline CVector2 SVector2::operator>>(int n) const { return CVector2(x>>n,y>>n); }
inline CVector2 SVector2::operator&(int n) const { return CVector2(x&n,y&n); }
inline CVector2 SVector2::operator|(int n) const { return CVector2(x|n,y|n); }
// ------------------------------------------------------------------------
struct	SRect2;
typedef const SRect2 TCSRect2;
class	CRect2;
typedef	const CRect2 TCCRect2;

struct SRect2 {
	long  left,top,right,bottom;	// right>=left, bottom>=top

	TCCVector2 &LeftTop() const { return *(TCCVector2*)(void*)&left; }
	TCCVector2 &RightBottom() const { return *(TCCVector2*)(void*)&right; }
	CVector2 &LeftTop() { return *(CVector2*)(void*)&left; }
	CVector2 &RightBottom() { return *(CVector2*)(void*)&right; }
	long  Width() const { return right-left; }
	long  Height() const { return bottom-top; }
	long  CenterX() const { return (left+right)/2; }
	long  CenterY() const { return (top+bottom)/2; }
	CVector2 Center() const { return CVector2((left+right)/2,(top+bottom)/2); }
	bool  Contains(TCSVector2 &v) const { return v.x>=left && v.x<right && v.y>=top && v.y<bottom; }
	bool  IsEmpty() const { return left==right && top==bottom; }
	void  SetEmpty() { left=top=right=bottom=0; }
	void  DoSetIntersect(TCSRect2 &r1,TCSRect2 &r2) {
		left = Max(r1.left,r2.left); right = Min(r1.right,r2.right);
		top = Max(r1.top,r2.top); bottom = Min(r1.bottom,r2.bottom);
	}
	bool  SetIntersect(TCSRect2 &r1,TCSRect2 &r2) {
		if( r1.left > r2.right || r2.left > r1.right ||
				r1.top > r2.bottom || r2.top > r1.bottom )	return FALSE;
		left = Max(r1.left,r2.left); right = Min(r1.right,r2.right);
		top = Max(r1.top,r2.top); bottom = Min(r1.bottom,r2.bottom);
		return TRUE;
	}
	bool  SetIntersectNZ(TCSRect2 &r1,TCSRect2 &r2) {
		if( r1.left >= r2.right || r2.left >= r1.right ||
				r1.top >= r2.bottom || r2.top >= r1.bottom )    return FALSE;
		left = Max(r1.left,r2.left); right = Min(r1.right,r2.right);
		top = Max(r1.top,r2.top); bottom = Min(r1.bottom,r2.bottom);
		return TRUE;
	}
	void  SetUnion(TCSRect2 &r1,TCSRect2 &r2) {
		left = Min(r1.left,r2.left);	top = Min(r1.top,r2.top);
		right = Max(r1.right,r2.right);	bottom = Max(r1.bottom,r2.bottom);	}
	void  MoveBy(TCSVector2 &v) { left+=v.x; right+=v.x; top+=v.y; bottom+=v.y; }
	void  MoveTo(TCSVector2 &v) { right += v.x-left; bottom += v.y-top; left = v.x; top = v.y; }
	void  ExtendBy(TCSVector2 &v) { right += v.x; bottom += v.y; }
	void  ExtendTo(TCSVector2 &v) { right = left+v.x; bottom = top+v.y; }
	void  InflateBy(TCSVector2 &v) { left-=v.x/2; right+=v.x-v.x/2; top-=v.y/2; bottom+=v.y-v.y/2; }
	void  InflateTo(TCSVector2 &v) { right=(left+right+v.x)/2; left=right-v.x;
									 bottom=(top+bottom+v.y)/2; top=bottom-v.y ; }

	int  operator==(TCSRect2 &r) const { return left==r.left && top==r.top &&
										right==r.right && bottom==r.bottom; }
	int  operator!=(TCSRect2 &r) const { return left!=r.left || top!=r.top ||
										right!=r.right || bottom!=r.bottom; }

	___MATHLIB_DECLARE_TAGGIO
};

class	CRect2 : public SRect2 {
public:
	__MATHLIB_INHERIT_STRUCT(Rect2)
	CRect2(long _left,long _top,long _right,long _bottom) { left=_left; top=_top; right=_right; bottom=_bottom; }
	CRect2(TCSVector2 &v1,TCSVector2 &v2) { left=v1.x; top=v1.y; right=v2.x; bottom=v2.y; }
};
// ------------------------------------------------------------------------
struct	SFVector2;
typedef	const SFVector2 TCSFVector2;
class	CFVector2;
typedef	const CFVector2 TCCFVector2;

struct	SFVector2 {
	double	x, y;

	CFVector2 operator+() const;
	CFVector2 operator-() const;
	double operator*(TCSFVector2 &v) const { return x*v.x+y*v.y; }
	double operator%(TCSFVector2 &v) const { return x*v.y-y*v.x; }
	CFVector2 operator+(TCSFVector2 &v) const;
	CFVector2 operator-(TCSFVector2 &v) const;
	SFVector2 &operator+=(TCSFVector2 &v) { x+=v.x; y+=v.y; return *this; }
	SFVector2 &operator-=(TCSFVector2 &v) { x-=v.x; y-=v.y; return *this; }
	CFVector2 operator*(double f) const;
	SFVector2 &operator*=(double f) { x*=f; y*=f; return *this; }
	CFVector2 operator/(double f) const;
	SFVector2 &operator/=(double f) { x/=f; y/=f; return *this; }
	CFVector2 operator&(TCSFVector2 &v) const;
	SFVector2 &operator&=(TCSFVector2 &v) { x*=v.x; y*=v.y; return *this; }

	int operator==(TCSFVector2 &v) { return x==v.x&&y==v.y; }
	int operator!=(TCSFVector2 &v) { return x!=v.x||y!=v.y; }

	friend CFVector2 operator*(double f,TCSFVector2 &v);
	friend double Abs(TCSFVector2 &v) { return sqrt(v.x*v.x+v.y*v.y); }
	friend double Abs2(TCSFVector2 &v) { return v.x*v.x+v.y*v.y; }
	friend CFVector2 Normal(TCSFVector2 &v);

	___MATHLIB_DECLARE_TAGGIO
};

class	CFVector2 : public SFVector2 {
public:
	__MATHLIB_INHERIT_STRUCT(FVector2)
	CFVector2(double _x, double _y) { x = _x; y = _y; }
};

inline CFVector2 SFVector2::operator+() const { return *this; }
inline CFVector2 SFVector2::operator-() const { return CFVector2(-x,-y); }
inline CFVector2 SFVector2::operator+(TCSFVector2 &v) const { return CFVector2(x+v.x,y+v.y); }
inline CFVector2 SFVector2::operator-(TCSFVector2 &v) const {	return CFVector2(x-v.x,y-v.y); }
inline CFVector2 SFVector2::operator*(double f) const { return CFVector2(x*f,y*f); }
inline CFVector2 SFVector2::operator/(double f) const { return CFVector2(x/f,y/f); }
inline CFVector2 SFVector2::operator&(TCSFVector2 &v) const {	return CFVector2(x*v.x,y*v.y); }
inline CFVector2 operator*(double f,TCSFVector2 &v) { return CFVector2(f*v.x,f*v.y); }
inline CFVector2 Normal(TCSFVector2 &v) { double a = atan2(v.y,v.x); return CFVector2(cos(a), sin(a)); }
// ------------------------------------------------------------------------
struct	SFVector3;
typedef	const SFVector3 TCSFVector3;
class	CFVector3;
typedef	const CFVector3 TCCFVector3;

struct	SFVector3 {
	double	x, y, z;

	CFVector3	operator+() const;
	CFVector3	operator-() const;
	double		operator*(TCSFVector3 &v) const { return x*v.x+y*v.y+z*v.z; }
	CFVector3	operator%(TCSFVector3 &v) const;
	CFVector3	operator+(TCSFVector3 &v) const;
	CFVector3	operator-(TCSFVector3 &v) const;
	SFVector3 &operator+=(TCSFVector3 &v) { x+=v.x; y+=v.y; z+=v.z; return *this; }
	SFVector3 &operator-=(TCSFVector3 &v) { x-=v.x; y-=v.y; z-=v.z; return *this; }
	CFVector3 operator*(double f) const;
	SFVector3 &operator*=(double f) { x*=f; y*=f; z*=f; return *this; }
	CFVector3 operator/(double f) const;
	SFVector3 &operator/=(double f) { x/=f; y/=f; z/=f; return *this; }
	CFVector3 operator&(TCSFVector3 &v) const;
	SFVector3 &operator&=(TCSFVector3 &v) { x*=v.x; y*=v.y; z*=v.z; return *this; }

	int operator==(TCSFVector3 &v) { return x==v.x&&y==v.y&&z==v.z; }
	int operator!=(TCSFVector3 &v) { return x!=v.x||y!=v.y||z!=v.z; }

	friend CFVector3 operator*(double f,TCSFVector3 &v);
	friend double Abs(TCSFVector3 &v) {	return sqrt(v.x*v.x+v.y*v.y+v.z*v.z); }
	friend double Abs2(TCSFVector3 &v) { return v.x*v.x+v.y*v.y+v.z*v.z; }
	friend CFVector3 Normal(TCSFVector3 &v);

	___MATHLIB_DECLARE_TAGGIO
};

class	CFVector3 : public SFVector3 {
public:
	__MATHLIB_INHERIT_STRUCT(FVector3)
	CFVector3(double _x, double _y, double _z) { x = _x; y = _y; z = _z; }
};

inline CFVector3 SFVector3::operator+() const { return *this; }
inline CFVector3 SFVector3::operator-() const { return CFVector3(-x,-y,-z); }
inline CFVector3 SFVector3::operator%(TCSFVector3 &v) const { return CFVector3(y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x); }
inline CFVector3 SFVector3::operator+(TCSFVector3 &v) const { return CFVector3(x+v.x,y+v.y,z+v.z); }
inline CFVector3 SFVector3::operator-(TCSFVector3 &v) const { return CFVector3(x-v.x,y-v.y,z-v.z); }
inline CFVector3 SFVector3::operator*(double f) const { return CFVector3(x*f,y*f,z*f); }
inline CFVector3 SFVector3::operator/(double f) const { return CFVector3(x/f,y/f,z/f); }
inline CFVector3 SFVector3::operator&(TCSFVector3 &v) const { return CFVector3(x*v.x,y*v.y,z*v.z); }
inline CFVector3 operator*(double f,TCSFVector3 &v) { return CFVector3(f*v.x,f*v.y,f*v.z); }
inline CFVector3 Normal(TCSFVector3 &v) { return CFVector3(	sin(atan2(v.x,hypot(v.y,v.z))),
						sin(atan2(v.y,hypot(v.z,v.x))), sin(atan2(v.z,hypot(v.x,v.y))) ); }

#ifdef	__DEBUGEXT_H__
	inline	void FPCheckNAN(TCSFVector3 &v)	{ FPCheckNAN(v.x); FPCheckNAN(v.y); FPCheckNAN(v.z); }
#endif
// ------------------------------------------------------------------------
struct	SFRect2;
typedef	const SFRect2 TCSFRect2;
class	CFRect2;
typedef	const CFRect2 TCCFRect2;

struct	SFRect2 {
	double	left,top,right,bottom;	// right>=left, bottom>=top

	TCCFVector2 &LeftTop() const { return *(TCCFVector2*)(void*)&left; }
	TCCFVector2 &RightBottom() const { return *(TCCFVector2*)(void*)&right; }
	double  Width() const { return right-left; }
	double  Height() const { return bottom-top; }
	double  CenterX() const { return (left+right)/2; }
	double  CenterY() const { return (top+bottom)/2; }
	CFVector2 Center() const { return CFVector2((left+right)/2,(top+bottom)/2); }
	bool  Contains(TCSFVector2 &v) const { return v.x>=left && v.x<right && v.y>=top && v.y<bottom; }
	bool  IsEmpty() const { return left==right && top==bottom; }
	void  SetEmpty() { left=top=right=bottom=0; }
	//bool  SetIntersect(TCSFRect2 &r1,TCSFRect2 &r2);
	void  SetUnion(TCSFRect2 &r1,TCSFRect2 &r2) {
				left = Min(r1.left,r2.left);	top = Min(r1.top,r2.top);
				right = Max(r1.right,r2.right);	bottom = Max(r1.bottom,r2.bottom);	}

	void  MoveBy(TCSFVector2 &v) { left+=v.x; right+=v.x; top+=v.y; bottom+=v.y; }
	void  MoveTo(TCSFVector2 &v) { right += v.x-left; bottom += v.y-top; left = v.x; top = v.y; }
	void  ExtendBy(TCSFVector2 &v) { right += v.x; bottom += v.y; }
	void  ExtendTo(TCSFVector2 &v) { right = left+v.x; bottom = top+v.y; }
	void  InflateBy(TCSFVector2 &v) { left-=v.x/2; right+=(v.x+1)/2; top-=v.y/2; bottom+=(v.y+1)/2; }
	void  InflateTo(TCSFVector2 &v) { right=(left+right+v.x)/2; left=right-v.x;
									  bottom=(top+left+v.y)/2; top=bottom-v.y ; }

	int  operator==(TCSFRect2 &r) const { return left==r.left && top==r.top &&
										right==r.right && bottom==r.bottom; }
	int  operator!=(TCSFRect2 &r) const { return left!=r.left || top!=r.top ||
										right!=r.right || bottom!=r.bottom; }

	___MATHLIB_DECLARE_TAGGIO
};


class	CFRect2 : public SFRect2 {
public:
	__MATHLIB_INHERIT_STRUCT(FRect2)
	CFRect2(double _left,double _top,double _right,double _bottom) { left=_left; top=_top; right=_right; bottom=_bottom; }
	CFRect2(TCCFVector2 &v1,TCCFVector2 &v2) { left=v1.x; top=v1.y; right=v2.x; bottom=v2.y; }
};

// ------------------------------------------------------------------------
struct	SFMatrix3x4;
typedef	const SFMatrix3x4 TCSFMatrix3x4;
class	CFMatrix3x4;
typedef	const CFMatrix3x4 TCCFMatrix3x4;

struct	SFMatrix3x4	{
	double		m[3][4];

	enum	{ AXIS_OX = 1, AXIS_OY = 2, AXIS_OZ = 3 };

	void	Mult(TCSFVector3 &in,SFVector3 &out) const;
	void	MultF(TCSFVector3 &in,SFVector3 &out) const; // free vectors
	void	MultT(TCSFVector3 &in,SFVector3 &out) const; // mult transposed matrix
	void	MultTF(TCSFVector3 &in,SFVector3 &out) const; // mult transposed matrix and free vectors
	CFVector3	operator*(TCSFVector3 &v) const;
	CFVector3	operator%(TCSFVector3 &v) const;	// free vectors
	CFVector3	MultT(TCSFVector3 &in) const;	// mult transposed matrix
	CFVector3	MultTF(TCSFVector3 &in) const; // mult transposed matrix and free vectors
	SFMatrix3x4	&LoadMult(TCSFMatrix3x4 &m1,TCSFMatrix3x4 &m2);
	CFMatrix3x4	operator*(TCSFMatrix3x4 &mx) const;
	SFMatrix3x4	&MultR(TCSFMatrix3x4 &mx);
	SFMatrix3x4	&LoadInverse(TCSFMatrix3x4 &s);
	SFMatrix3x4	&LoadTransposed(TCSFMatrix3x4 &s);
	SFMatrix3x4	&LoadIdentity();
	SFMatrix3x4	&TranslateL(TCSFVector3 &d);
	SFMatrix3x4	&TranslateR(TCSFVector3 &d);
	SFMatrix3x4	&ScaleL(TCSFVector3 &s);
	SFMatrix3x4	&ScaleR(TCSFVector3 &s);
	SFMatrix3x4	&RotateOxL(double a);
	SFMatrix3x4	&RotateOxR(double a);
	SFMatrix3x4	&RotateOyL(double a);
	SFMatrix3x4	&RotateOyR(double a);
	SFMatrix3x4	&RotateOzL(double a);
	SFMatrix3x4	&RotateOzR(double a);
	SFMatrix3x4 &RotateL(TCCFVector3 &axis,double a);
	SFMatrix3x4 &RotateR(TCCFVector3 &axis,double a);
	SFMatrix3x4 &RotateL(int axis,double a) {
		if( axis == AXIS_OX )	return RotateOxL(a); else
		if( axis == AXIS_OY )	return RotateOyL(a); else
		if( axis == AXIS_OZ )	return RotateOzL(a); else
		if( axis == -AXIS_OX )	return RotateOxL(-a); else
		if( axis == -AXIS_OY )	return RotateOyL(-a); else
		if( axis == -AXIS_OZ )	return RotateOzL(-a);
		return *this;
	}
	SFMatrix3x4 &RotateR(int axis,double a) {
		if( axis == AXIS_OX )	return RotateOxR(a); else
		if( axis == AXIS_OY )	return RotateOyR(a); else
		if( axis == AXIS_OZ )	return RotateOzR(a); else
		if( axis == -AXIS_OX )	return RotateOxR(-a); else
		if( axis == -AXIS_OY )	return RotateOyR(-a); else
		if( axis == -AXIS_OZ )	return RotateOzR(-a);
		return *this;
	}
	SFMatrix3x4	&LoadIdentityKernel();
	SFMatrix3x4	&LoadOffset(TCCFVector3 &v) { m[0][3]=v.x; m[1][3]=v.y; m[2][3]=v.z; return *this; }
	CFVector3	Offset() const { return CFVector3(m[0][3],m[1][3],m[2][3]); }
	CFVector3	Column(int i) const { return CFVector3(m[0][i],m[1][i],m[2][i]); }
	CFVector3	Row(int i) const { return CFVector3(m[i][0],m[i][1],m[i][2]); }
	double		MultRow(int i,TCSFVector3 &v) const { return m[i][0]*v.x+m[i][1]*v.y+m[i][2]*v.z+m[i][3]; }

	SFMatrix3x4 &EulerL(int XH,double HA,int XP,double PA,int XR,double RA) {
						return RotateL(XR,RA).RotateL(XP,PA).RotateL(XH,HA); }
	SFMatrix3x4 &EulerR(int XH,double HA,int XP,double PA,int XR,double RA) {
						return RotateR(XH,HA).RotateR(XP,PA).RotateR(XR,RA); }
	void	RestoreEuler(int XH,double &HA,int XP,double &PA,int XR,double &RA) const;

	SFMatrix3x4	&Orthogonalize();
	#ifdef	__DEBUGEXT_H__
    void CheckOrtho() const {
		#ifdef _DEBUG
	    double f = 0;
	    for (int i = 0; i < 3; i++)  {
	        f = Max(f, Max(fabs(m[i][0] * m[i][0] + m[i][1]*m[i][1] + m[i][2]*m[i][2] - 1),
	                        fabs(m[0][i] * m[0][i] + m[1][i]*m[1][i] + m[2][i]*m[2][i] - 1)));
	        for (int j = 0; j < 3; j++)
	            if (i != j) {
	                f = Max(f, Max(fabs(m[i][0] * m[j][0] + m[i][1]*m[j][1] + m[i][2]*m[j][2]),
	                                fabs(m[0][i] * m[0][j] + m[1][i]*m[1][j] + m[2][i]*m[2][j])));
	            }
	    }
	    ASSERT(f < 1e-5);
		#endif
    }
	#endif

	___MATHLIB_DECLARE_TAGGIO
};

#ifdef	__DEBUGEXT_H__
	inline void FPCheckNAN(TCSFMatrix3x4 &m) {
	    FPCheckNAN(m.m[0][0]);
	    FPCheckNAN(m.m[0][1]);
	    FPCheckNAN(m.m[0][2]);
	    FPCheckNAN(m.m[0][3]);
	    FPCheckNAN(m.m[1][0]);
	    FPCheckNAN(m.m[1][1]);
	    FPCheckNAN(m.m[1][2]);
	    FPCheckNAN(m.m[1][3]);
	    FPCheckNAN(m.m[2][0]);
	    FPCheckNAN(m.m[2][1]);
	    FPCheckNAN(m.m[2][2]);
	    FPCheckNAN(m.m[2][3]);
	}
#endif

class	CFMatrix3x4 : public SFMatrix3x4 {
public:
	__MATHLIB_INHERIT_STRUCT(FMatrix3x4)
	CFMatrix3x4(double m00,double m01,double m02,double m03,
				double m10,double m11,double m12,double m13,
				double m20,double m21,double m22,double m23) {
					m[0][0]=m00; m[0][1]=m01; m[0][2]=m02; m[0][3]=m03;
					m[1][0]=m10; m[1][1]=m11; m[1][2]=m12; m[1][3]=m13;
					m[2][0]=m20; m[2][1]=m21; m[2][2]=m22; m[2][3]=m23; }
};

// --------------------------------------------------------------------

class	CRandomGenerator;
typedef	const CRandomGenerator TCCRandomGenerator;

class	CRandomGenerator	{
protected:
	long	m_lSeed;
public:
	void Restart() { m_lSeed = 1; }
	CRandomGenerator() { Restart(); }

	double operator()() { return ((int)((m_lSeed = 0x015a4e35L * m_lSeed + 1) >> 16) & 0x7fff)/32768.0; }
	double operator()(double fMin,double fMax) { return (*this)()*(fMax-fMin)+fMin; }

	double	Last() const { return ((int)(m_lSeed >> 16) & 0x7fff)/32768.0; }
	double  Last(double fMin,double fMax) const { return Last()*(fMax-fMin)+fMin; }

	bool IsInitialState() const { return m_lSeed == 1; }
	bool operator==(TCCRandomGenerator &r) const { return m_lSeed == r.m_lSeed; }
};

// ------------------------------------------------------------------------
#undef	___MATHLIB_DECLARE_TAGGIO
#undef	__MATHLIB_INHERIT_STRUCT
// ------------------------------------------------------------------------
// Revert structure packing
#if	defined(__BORLANDC__)
	#pragma	option	-a.
#elif defined(__WATCOMC__)
	#pragma pack()
#endif
// ------------------------------------------------------------------------

#endif	//	__MATHLIB_H__
