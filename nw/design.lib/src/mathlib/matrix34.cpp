
#ifndef	__MATHLIB_CPP__
	#include<stdio.h>
	#include<math.h>
	#include<string.h>
	#include<stdlib.h>
	#define	_NOAUTOINCLUDE
	#include"stdtypes.h"
	#include"debugext.h"
	#include"filesys.h"
	#include"mathlib.h"
#endif

// ------------------------------------------------------------------------
void  SFMatrix3x4::Mult(TCSFVector3 &in,SFVector3 &out) const
{
	out.x = m[0][0]*in.x+m[0][1]*in.y+m[0][2]*in.z+m[0][3];
	out.y = m[1][0]*in.x+m[1][1]*in.y+m[1][2]*in.z+m[1][3];
	out.z = m[2][0]*in.x+m[2][1]*in.y+m[2][2]*in.z+m[2][3];
}

void  SFMatrix3x4::MultF(TCSFVector3 &in,SFVector3 &out) const
{
	out.x = m[0][0]*in.x+m[0][1]*in.y+m[0][2]*in.z;
	out.y = m[1][0]*in.x+m[1][1]*in.y+m[1][2]*in.z;
	out.z = m[2][0]*in.x+m[2][1]*in.y+m[2][2]*in.z;
}

void  SFMatrix3x4::MultT(TCSFVector3 &in,SFVector3 &out) const
{
	CFVector3	in3;
	in3.x = in.x-m[0][3];
	in3.y = in.x-m[1][3];
	in3.z = in.x-m[2][3];
	out.x = m[0][0]*in3.x+m[1][0]*in3.y+m[2][0]*in3.z;
	out.y = m[0][1]*in3.x+m[1][1]*in3.y+m[2][1]*in3.z;
	out.z = m[0][2]*in3.x+m[1][2]*in3.y+m[2][2]*in3.z;
}

void  SFMatrix3x4::MultTF(TCSFVector3 &in,SFVector3 &out) const
{
	out.x = m[0][0]*in.x+m[1][0]*in.y+m[2][0]*in.z;
	out.y = m[0][1]*in.x+m[1][1]*in.y+m[2][1]*in.z;
	out.z = m[0][2]*in.x+m[1][2]*in.y+m[2][2]*in.z;
}

CFVector3 SFMatrix3x4::operator*(TCSFVector3 &v) const {
	return	CFVector3(
		m[0][0]*v.x+m[0][1]*v.y+m[0][2]*v.z+m[0][3],
		m[1][0]*v.x+m[1][1]*v.y+m[1][2]*v.z+m[1][3],
		m[2][0]*v.x+m[2][1]*v.y+m[2][2]*v.z+m[2][3] );
}

CFVector3 SFMatrix3x4::operator%(TCSFVector3 &v) const {
	return	CFVector3(
		m[0][0]*v.x+m[0][1]*v.y+m[0][2]*v.z,
		m[1][0]*v.x+m[1][1]*v.y+m[1][2]*v.z,
		m[2][0]*v.x+m[2][1]*v.y+m[2][2]*v.z );
}

CFVector3  SFMatrix3x4::MultT(TCSFVector3 &in) const
{
	CFVector3	in3;
	in3.x = in.x-m[0][3];
	in3.y = in.x-m[1][3];
	in3.z = in.x-m[2][3];
	return CFVector3(
		m[0][0]*in3.x+m[1][0]*in3.y+m[2][0]*in3.z,
		m[0][1]*in3.x+m[1][1]*in3.y+m[2][1]*in3.z,
		m[0][2]*in3.x+m[1][2]*in3.y+m[2][2]*in3.z );
}

CFVector3  SFMatrix3x4::MultTF(TCSFVector3 &in) const
{
	return CFVector3(
		m[0][0]*in.x+m[1][0]*in.y+m[2][0]*in.z,
		m[0][1]*in.x+m[1][1]*in.y+m[2][1]*in.z,
		m[0][2]*in.x+m[1][2]*in.y+m[2][2]*in.z );
}

SFMatrix3x4	&SFMatrix3x4::LoadMult(TCSFMatrix3x4 &m1,TCSFMatrix3x4 &m2)
{
	m[0][0]=m1.m[0][0]*m2.m[0][0]+m1.m[0][1]*m2.m[1][0]+m1.m[0][2]*m2.m[2][0];
	m[0][1]=m1.m[0][0]*m2.m[0][1]+m1.m[0][1]*m2.m[1][1]+m1.m[0][2]*m2.m[2][1];
	m[0][2]=m1.m[0][0]*m2.m[0][2]+m1.m[0][1]*m2.m[1][2]+m1.m[0][2]*m2.m[2][2];
	m[0][3]=m1.m[0][0]*m2.m[0][3]+m1.m[0][1]*m2.m[1][3]+m1.m[0][2]*m2.m[2][3]+
																m1.m[0][3];
	m[1][0]=m1.m[1][0]*m2.m[0][0]+m1.m[1][1]*m2.m[1][0]+m1.m[1][2]*m2.m[2][0];
	m[1][1]=m1.m[1][0]*m2.m[0][1]+m1.m[1][1]*m2.m[1][1]+m1.m[1][2]*m2.m[2][1];
	m[1][2]=m1.m[1][0]*m2.m[0][2]+m1.m[1][1]*m2.m[1][2]+m1.m[1][2]*m2.m[2][2];
	m[1][3]=m1.m[1][0]*m2.m[0][3]+m1.m[1][1]*m2.m[1][3]+m1.m[1][2]*m2.m[2][3]+
																m1.m[1][3];
	m[2][0]=m1.m[2][0]*m2.m[0][0]+m1.m[2][1]*m2.m[1][0]+m1.m[2][2]*m2.m[2][0];
	m[2][1]=m1.m[2][0]*m2.m[0][1]+m1.m[2][1]*m2.m[1][1]+m1.m[2][2]*m2.m[2][1];
	m[2][2]=m1.m[2][0]*m2.m[0][2]+m1.m[2][1]*m2.m[1][2]+m1.m[2][2]*m2.m[2][2];
	m[2][3]=m1.m[2][0]*m2.m[0][3]+m1.m[2][1]*m2.m[1][3]+m1.m[2][2]*m2.m[2][3]+
																m1.m[2][3];
	return	*this;
}

CFMatrix3x4	SFMatrix3x4::operator*(TCSFMatrix3x4 &mx) const
{
	return	CFMatrix3x4(
		m[0][0]*mx.m[0][0]+m[0][1]*mx.m[1][0]+m[0][2]*mx.m[2][0],
		m[0][0]*mx.m[0][1]+m[0][1]*mx.m[1][1]+m[0][2]*mx.m[2][1],
		m[0][0]*mx.m[0][2]+m[0][1]*mx.m[1][2]+m[0][2]*mx.m[2][2],
		m[0][0]*mx.m[0][3]+m[0][1]*mx.m[1][3]+m[0][2]*mx.m[2][3]+m[0][3],
		m[1][0]*mx.m[0][0]+m[1][1]*mx.m[1][0]+m[1][2]*mx.m[2][0],
		m[1][0]*mx.m[0][1]+m[1][1]*mx.m[1][1]+m[1][2]*mx.m[2][1],
		m[1][0]*mx.m[0][2]+m[1][1]*mx.m[1][2]+m[1][2]*mx.m[2][2],
		m[1][0]*mx.m[0][3]+m[1][1]*mx.m[1][3]+m[1][2]*mx.m[2][3]+m[1][3],
		m[2][0]*mx.m[0][0]+m[2][1]*mx.m[1][0]+m[2][2]*mx.m[2][0],
		m[2][0]*mx.m[0][1]+m[2][1]*mx.m[1][1]+m[2][2]*mx.m[2][1],
		m[2][0]*mx.m[0][2]+m[2][1]*mx.m[1][2]+m[2][2]*mx.m[2][2],
		m[2][0]*mx.m[0][3]+m[2][1]*mx.m[1][3]+m[2][2]*mx.m[2][3]+m[2][3]);
}

SFMatrix3x4	&SFMatrix3x4::MultR(TCSFMatrix3x4 &mx)
{
	SFMatrix3x4	t;
	t.m[0][0]=m[0][0]*mx.m[0][0]+m[0][1]*mx.m[1][0]+m[0][2]*mx.m[2][0];
	t.m[0][1]=m[0][0]*mx.m[0][1]+m[0][1]*mx.m[1][1]+m[0][2]*mx.m[2][1];
	t.m[0][2]=m[0][0]*mx.m[0][2]+m[0][1]*mx.m[1][2]+m[0][2]*mx.m[2][2];
	t.m[0][3]=m[0][0]*mx.m[0][3]+m[0][1]*mx.m[1][3]+m[0][2]*mx.m[2][3]+m[0][3];
	t.m[1][0]=m[1][0]*mx.m[0][0]+m[1][1]*mx.m[1][0]+m[1][2]*mx.m[2][0];
	t.m[1][1]=m[1][0]*mx.m[0][1]+m[1][1]*mx.m[1][1]+m[1][2]*mx.m[2][1],
	t.m[1][2]=m[1][0]*mx.m[0][2]+m[1][1]*mx.m[1][2]+m[1][2]*mx.m[2][2];
	t.m[1][3]=m[1][0]*mx.m[0][3]+m[1][1]*mx.m[1][3]+m[1][2]*mx.m[2][3]+m[1][3];
	t.m[2][0]=m[2][0]*mx.m[0][0]+m[2][1]*mx.m[1][0]+m[2][2]*mx.m[2][0];
	t.m[2][1]=m[2][0]*mx.m[0][1]+m[2][1]*mx.m[1][1]+m[2][2]*mx.m[2][1];
	t.m[2][2]=m[2][0]*mx.m[0][2]+m[2][1]*mx.m[1][2]+m[2][2]*mx.m[2][2];
	t.m[2][3]=m[2][0]*mx.m[0][3]+m[2][1]*mx.m[1][3]+m[2][2]*mx.m[2][3]+m[2][3];
	*this = t;
	return	*this;
}

SFMatrix3x4 &SFMatrix3x4::LoadInverse(TCSFMatrix3x4 &s)
{
	double det = s.m[0][0]*(s.m[1][1]*s.m[2][2]-s.m[2][1]*s.m[1][2])+
				 s.m[0][1]*(s.m[1][2]*s.m[2][0]-s.m[1][0]*s.m[2][2])+
				 s.m[0][2]*(s.m[1][0]*s.m[2][1]-s.m[1][1]*s.m[2][0]);
	m[0][0] = (s.m[1][1]*s.m[2][2]-s.m[2][1]*s.m[1][2])/det;
	m[0][1] = (s.m[0][2]*s.m[2][1]-s.m[0][1]*s.m[2][2])/det;
	m[0][2] = (s.m[0][1]*s.m[1][2]-s.m[1][1]*s.m[0][2])/det;
	m[1][0] = (s.m[1][2]*s.m[2][0]-s.m[1][0]*s.m[2][2])/det;
	m[1][1] = (s.m[0][0]*s.m[2][2]-s.m[2][0]*s.m[0][2])/det;
	m[1][2] = (s.m[1][0]*s.m[0][2]-s.m[0][0]*s.m[1][2])/det;
	m[2][0] = (s.m[1][0]*s.m[2][1]-s.m[2][0]*s.m[1][1])/det;
	m[2][1] = (s.m[2][0]*s.m[0][1]-s.m[0][0]*s.m[2][1])/det;
	m[2][2] = (s.m[0][0]*s.m[1][1]-s.m[1][0]*s.m[0][1])/det;
	m[0][3] = -(m[0][0]*s.m[0][3]+m[0][1]*s.m[1][3]+m[0][2]*s.m[2][3]);
	m[1][3] = -(m[1][0]*s.m[0][3]+m[1][1]*s.m[1][3]+m[1][2]*s.m[2][3]);
	m[2][3] = -(m[2][0]*s.m[0][3]+m[2][1]*s.m[1][3]+m[2][2]*s.m[2][3]);
	return *this;
}

SFMatrix3x4 &SFMatrix3x4::LoadTransposed(TCSFMatrix3x4 &s)
{
	m[0][0] = s.m[0][0]; m[0][1] = s.m[1][0]; m[0][2] = s.m[2][0];
	m[1][0] = s.m[0][1]; m[1][1] = s.m[1][1]; m[1][2] = s.m[2][1];
	m[2][0] = s.m[0][2]; m[2][1] = s.m[1][2]; m[2][2] = s.m[2][2];
	m[0][3] = -(s.m[0][0]*s.m[0][3]+s.m[1][0]*s.m[1][3]+s.m[2][0]*s.m[2][3]);
	m[1][3] = -(s.m[0][1]*s.m[0][3]+s.m[1][1]*s.m[1][3]+s.m[2][1]*s.m[2][3]);
	m[2][3] = -(s.m[0][2]*s.m[0][3]+s.m[1][2]*s.m[1][3]+s.m[2][2]*s.m[2][3]);
	return	*this;
}

SFMatrix3x4	&SFMatrix3x4::LoadIdentity()
{
	m[0][0]=1; m[0][1]=0; m[0][2]=0; m[0][3]=0;
	m[1][0]=0; m[1][1]=1; m[1][2]=0; m[1][3]=0;
	m[2][0]=0; m[2][1]=0; m[2][2]=1; m[2][3]=0;
	return	*this;
}

SFMatrix3x4	&SFMatrix3x4::TranslateL(TCSFVector3 &d)
{
	m[0][3]+=d.x;
	m[1][3]+=d.y;
	m[2][3]+=d.z;
	return	*this;
}

SFMatrix3x4	&SFMatrix3x4::TranslateR(TCSFVector3 &d)
{
	m[0][3] += m[0][0]*d.x+m[0][1]*d.y+m[0][2]*d.z;
	m[1][3] += m[1][0]*d.x+m[1][1]*d.y+m[1][2]*d.z;
	m[2][3] += m[2][0]*d.x+m[2][1]*d.y+m[2][2]*d.z;
	return	*this;
}

SFMatrix3x4	&SFMatrix3x4::ScaleL(TCSFVector3 &s)
{
	m[0][0] = s.x*m[0][0];
    m[0][1] = s.x*m[0][1];
	m[0][2] = s.x*m[0][2];
	m[0][3] = s.x*m[0][3];
	m[1][0] = s.y*m[1][0];
	m[1][1] = s.y*m[1][1];
	m[1][2] = s.y*m[1][2];
	m[1][3] = s.y*m[1][3];
	m[2][0] = s.z*m[2][0];
	m[2][1] = s.z*m[2][1];
	m[2][2] = s.z*m[2][2];
	m[2][3] = s.z*m[2][3];
	return	*this;
}

SFMatrix3x4	&SFMatrix3x4::ScaleR(TCSFVector3 &s)
{
	m[0][0] = m[0][0]*s.x;
	m[1][0] = m[1][0]*s.x;
	m[2][0] = m[2][0]*s.x;
	m[0][1] = m[0][1]*s.y;
	m[1][1] = m[1][1]*s.y;
	m[2][1] = m[2][1]*s.y;
	m[0][2] = m[0][2]*s.z;
	m[1][2] = m[1][2]*s.z;
	m[2][2] = m[2][2]*s.z;
	return	*this;
}

SFMatrix3x4	&SFMatrix3x4::RotateOxL(double a)
{
	double	t[3][4];
	t[1][0] = cos(a)*m[1][0]-sin(a)*m[2][0];
	t[1][1] = cos(a)*m[1][1]-sin(a)*m[2][1];
	t[1][2] = cos(a)*m[1][2]-sin(a)*m[2][2];
	t[1][3] = cos(a)*m[1][3]-sin(a)*m[2][3];
	t[2][0] = sin(a)*m[1][0]+cos(a)*m[2][0];
	t[2][1] = sin(a)*m[1][1]+cos(a)*m[2][1];
	t[2][2] = sin(a)*m[1][2]+cos(a)*m[2][2];
	t[2][3] = sin(a)*m[1][3]+cos(a)*m[2][3];
	m[1][0]=t[1][0]; m[1][1]=t[1][1]; m[1][2]=t[1][2]; m[1][3]=t[1][3];
	m[2][0]=t[2][0]; m[2][1]=t[2][1]; m[2][2]=t[2][2]; m[2][3]=t[2][3];
	return	*this;
}

SFMatrix3x4	&SFMatrix3x4::RotateOxR(double a)
{
	double	t[3][4];
	t[0][1] = m[0][1]*cos(a)+m[0][2]*sin(a);
	t[1][1] = m[1][1]*cos(a)+m[1][2]*sin(a);
	t[2][1] = m[2][1]*cos(a)+m[2][2]*sin(a);
	t[0][2] = m[0][2]*cos(a)-m[0][1]*sin(a);
	t[1][2] = m[1][2]*cos(a)-m[1][1]*sin(a);
	t[2][2] = m[2][2]*cos(a)-m[2][1]*sin(a);
    m[0][1]=t[0][1]; m[1][1]=t[1][1]; m[2][1]=t[2][1];
	m[0][2]=t[0][2]; m[1][2]=t[1][2]; m[2][2]=t[2][2];
	return	*this;
}


SFMatrix3x4	&SFMatrix3x4::RotateOyL(double a)
{
	double	t[3][4];
	t[0][0] = cos(a)*m[0][0]+sin(a)*m[2][0];
	t[0][1] = cos(a)*m[0][1]+sin(a)*m[2][1];
	t[0][2] = cos(a)*m[0][2]+sin(a)*m[2][2];
	t[0][3] = cos(a)*m[0][3]+sin(a)*m[2][3];
	t[2][0] = cos(a)*m[2][0]-sin(a)*m[0][0];
	t[2][1] = cos(a)*m[2][1]-sin(a)*m[0][1];
	t[2][2] = cos(a)*m[2][2]-sin(a)*m[0][2];
	t[2][3] = cos(a)*m[2][3]-sin(a)*m[0][3];
	m[0][0]=t[0][0]; m[0][1]=t[0][1]; m[0][2]=t[0][2]; m[0][3]=t[0][3];
	m[2][0]=t[2][0]; m[2][1]=t[2][1]; m[2][2]=t[2][2]; m[2][3]=t[2][3];
	return	*this;
}

SFMatrix3x4	&SFMatrix3x4::RotateOyR(double a)
{
	double	t[3][4];
	t[0][0] = m[0][0]*cos(a)-m[0][2]*sin(a);
	t[1][0] = m[1][0]*cos(a)-m[1][2]*sin(a);
	t[2][0] = m[2][0]*cos(a)-m[2][2]*sin(a);
	t[0][2] = m[0][0]*sin(a)+m[0][2]*cos(a);
	t[1][2] = m[1][0]*sin(a)+m[1][2]*cos(a);
	t[2][2] = m[2][0]*sin(a)+m[2][2]*cos(a);
	m[0][0]=t[0][0]; m[1][0]=t[1][0]; m[2][0]=t[2][0];
	m[0][2]=t[0][2]; m[1][2]=t[1][2]; m[2][2]=t[2][2];
	return	*this;
}

SFMatrix3x4	&SFMatrix3x4::RotateOzL(double a)
{
	double	t[3][4];
	t[0][0] = cos(a)*m[0][0]-sin(a)*m[1][0];
	t[0][1] = cos(a)*m[0][1]-sin(a)*m[1][1];
	t[0][2] = cos(a)*m[0][2]-sin(a)*m[1][2];
	t[0][3] = cos(a)*m[0][3]-sin(a)*m[1][3];
	t[1][0] = sin(a)*m[0][0]+cos(a)*m[1][0];
	t[1][1] = sin(a)*m[0][1]+cos(a)*m[1][1];
	t[1][2] = sin(a)*m[0][2]+cos(a)*m[1][2];
	t[1][3] = sin(a)*m[0][3]+cos(a)*m[1][3];
	m[0][0]=t[0][0]; m[0][1]=t[0][1]; m[0][2]=t[0][2]; m[0][3]=t[0][3];
	m[1][0]=t[1][0]; m[1][1]=t[1][1]; m[1][2]=t[1][2]; m[1][3]=t[1][3];
	return	*this;
}

SFMatrix3x4	&SFMatrix3x4::RotateOzR(double a)
{
	double	t[3][4];
	t[0][0] = m[0][0]*cos(a)+m[0][1]*sin(a);
	t[1][0] = m[1][0]*cos(a)+m[1][1]*sin(a);
	t[2][0] = m[2][0]*cos(a)+m[2][1]*sin(a);
	t[0][1] = m[0][1]*cos(a)-m[0][0]*sin(a);
	t[1][1] = m[1][1]*cos(a)-m[1][0]*sin(a);
	t[2][1] = m[2][1]*cos(a)-m[2][0]*sin(a);
	m[0][0]=t[0][0]; m[1][0]=t[1][0]; m[2][0]=t[2][0];
	m[0][1]=t[0][1]; m[1][1]=t[1][1]; m[2][1]=t[2][1];
	return	*this;
}

SFMatrix3x4	&SFMatrix3x4::RotateL(TCCFVector3 &axis,double a)
{
	double fAxisAz = atan2(axis.x,axis.z),	// 0 for Oz, CCW-Oy
		   fAxisTang = atan2(axis.y,hypot(axis.x,axis.z));
	RotateOyL(-fAxisAz);
	RotateOxL(fAxisTang);
	RotateOzL(a);
	RotateOxL(-fAxisTang);
	RotateOyL(fAxisAz);
	return *this;
}

SFMatrix3x4	&SFMatrix3x4::RotateR(TCCFVector3 &axis,double a)
{
	double fAxisAz = atan2(axis.x,axis.z),	// 0 for Oz, CCW-Oy
		   fAxisTang = atan2(axis.y,hypot(axis.x,axis.z));
	RotateOyR(fAxisAz);
	RotateOxR(-fAxisTang);
	RotateOzR(a);
	RotateOxR(fAxisTang);
	RotateOyR(-fAxisAz);
	return *this;
}

SFMatrix3x4	&SFMatrix3x4::LoadIdentityKernel()
{
	m[0][0]=1; m[0][1]=0; m[0][2]=0;
	m[1][0]=0; m[1][1]=1; m[1][2]=0;
	m[2][0]=0; m[2][1]=0; m[2][2]=1;
	return	*this;
}

void SFMatrix3x4::RestoreEuler(int XH,double &HA,
		int XP,double &PA,int XR,double &RA) const
{
	static	int	vp_[7][7] = {	// vector product
		{        0,-AXIS_OX, AXIS_OY,       0,-AXIS_OY, AXIS_OX,       0 },	// -OZ
		{  AXIS_OX,       0,-AXIS_OZ,       0, AXIS_OZ,       0,-AXIS_OX },	// -OY
		{ -AXIS_OY, AXIS_OZ,       0,       0,       0,-AXIS_OZ, AXIS_OY },	// -OX
		{        0,       0,       0,       0,       0,       0,       0 },	//   O
		{  AXIS_OY,-AXIS_OZ,       0,       0,       0, AXIS_OZ,-AXIS_OY },	//  OX
		{ -AXIS_OX,       0, AXIS_OZ,       0,-AXIS_OZ,       0, AXIS_OX },	//  OY
		{        0, AXIS_OX,-AXIS_OY,       0, AXIS_OY,-AXIS_OX,       0 }	//  OZ
		//     -OZ      -OY      -OX        O       OX       OY       OZ
	},
	sp_[7][7] = {	// scalar product
		{    1,  0,  0,  0,  0,  0, -1 }, //  OZ
		{    0,  1,  0,  0,  0, -1,  0 }, //  OY
		{    0,  0,  1,  0, -1,  0,  0 }, // -OX
		{    0,  0,  0,  0,  0,  0,  0 }, //   O
		{    0,  0, -1,  0,  1,  0,  0 }, //  OX
		{    0, -1,  0,  0,  0,  1,  0 }, //  OY
		{   -1,  0,  0,  0,  0,  0,  1 } //  OZ
		// -OZ -OY -OX   O  OX  OY  OZ
	};
	//g_[7] = {		// sign
	//	-1, -1, -1, 0, 1, 1, 1
	//};
	static int	(*const vp)[7] = (int (*)[7])&vp_[3][3],
				(*const sp)[7] = (int (*)[7])&sp_[3][3];
				//*const g = &g_[3];
	ASSERT( XP && XR && XH && abs(vp[XP][XR]) == abs(XH) );
	int		UH = abs(XH), H = UH-1,
			UP = abs(XP), P = UP-1,
			UR = abs(XR), R = UR-1;
	// Heading = atan2( m[H%R][R],m[R][R] );
	HA = atan2(sp[vp[XH][UR]][UP]*m[P][R],m[R][R]);
	// Pitch = atan2( m[P%R][R], hypot(m[H%R][R],m[R][R]) );
	PA = atan2(sp[vp[XP][UR]][UH]*m[H][R],hypot(m[P][R],m[R][R]));
	// Roll = atan2( P'*(P%R)'', P'*P'')
	//		vector = (proj.P,proj.H,proj.R)
	//		H'' = (m[P][H],m[H][H],m[R][H]);
	//		P'' = (m[P][P],m[H][P],m[R][P]);
	//		P' = ( cos(HA), 0, sp[vp[XH][UP]][UR]*sin(HA) )
	RA = atan2( sp[vp[UP][XR]][UH]*(cos(HA)*m[P][H]+
							(sp[vp[XH][UP]][UR]*sin(HA))*m[R][H]),
				cos(HA)*m[P][P]+(sp[vp[XH][UP]][UR]*sin(HA))*m[R][P] );
}

SFMatrix3x4	&SFMatrix3x4::Orthogonalize()
{
	double	fAngleY = atan2(-m[2][0],m[2][2]);
	double	fAngleX = atan2(m[2][1],hypot(m[2][0],m[2][2]));
	CFVector3	normalY(m[1][0],m[1][1],m[1][2]);
	// fAngleZ=atan2(projX'(normalY),projY'(normalY))
	double	fAngleZ = atan2(CFVector3(cos(fAngleY),0,sin(fAngleY))*normalY,
				CFVector3(sin(fAngleX)*sin(fAngleY),
					cos(fAngleX),-sin(fAngleX)*cos(fAngleY))*normalY);
	LoadIdentity();
	RotateOyL(fAngleY);
	RotateOxL(fAngleX);
	RotateOzL(fAngleZ);
	return *this;
}


// ------------------------------------------------------------------------
