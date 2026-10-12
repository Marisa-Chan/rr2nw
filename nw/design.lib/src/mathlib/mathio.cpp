
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
#ifndef	_BIG_ENDIAN
bool SVector2::Write(CTaggedFile &f) const
{
	return	f.Write(this,sizeof(*this))==sizeof(*this);
}
#endif
bool SVector2::Read(CTaggedFile &f)
{
	if(f.Read(this,sizeof(*this))==sizeof(*this)) {
		ReverseByteOrderVarBE(x);
		ReverseByteOrderVarBE(y);
		return TRUE;
	}
	else return FALSE;
}
// ------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool SRect2::Write(CTaggedFile &f) const
{
	return	f.Write(this,sizeof(*this))==sizeof(*this);
}
#endif
bool SRect2::Read(CTaggedFile &f)
{
	if(f.Read(this,sizeof(*this))==sizeof(*this)) {
		ReverseByteOrderVarBE(left);
		ReverseByteOrderVarBE(top);
		ReverseByteOrderVarBE(right);
		ReverseByteOrderVarBE(bottom);
		return TRUE;
	}
	else return FALSE;
}
// ------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool SFVector2::Write(CTaggedFile &f) const
{
	return	f.Write(this,sizeof(*this))==sizeof(*this);
}
#endif
bool SFVector2::Read(CTaggedFile &f)
{
	if(f.Read(this,sizeof(*this))==sizeof(*this)) {
		ReverseByteOrderVarBE(x);
		ReverseByteOrderVarBE(y);
		return TRUE;
	}
	else return FALSE;
}
// ------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool SFVector3::Write(CTaggedFile &f) const
{
	return	f.Write(this,sizeof(*this))==sizeof(*this);
}
#endif
bool SFVector3::Read(CTaggedFile &f)
{
	if(f.Read(this,sizeof(*this))==sizeof(*this)) {
		ReverseByteOrderVarBE(x);
		ReverseByteOrderVarBE(y);
		ReverseByteOrderVarBE(z);
		return TRUE;
	}
	else return FALSE;
}
// ------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool SFRect2::Write(CTaggedFile &f) const
{
	return	f.Write(this,sizeof(*this))==sizeof(*this);
}
#endif
bool SFRect2::Read(CTaggedFile &f)
{
	if(f.Read(this,sizeof(*this))==sizeof(*this)) {
		ReverseByteOrderVarBE(left);
		ReverseByteOrderVarBE(top);
		ReverseByteOrderVarBE(right);
		ReverseByteOrderVarBE(bottom);
		return TRUE;
	}
	else return FALSE;
}
// ------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool SFMatrix3x4::Write(CTaggedFile &f) const
{
	return	f.Write(this,sizeof(*this))==sizeof(*this);
}
#endif
bool SFMatrix3x4::Read(CTaggedFile &f)
{
	if(f.Read(this,sizeof(*this))==sizeof(*this)) {
		ReverseByteOrderVarBE(m[0][0]);
		ReverseByteOrderVarBE(m[0][1]);
		ReverseByteOrderVarBE(m[0][2]);
		ReverseByteOrderVarBE(m[0][3]);
		ReverseByteOrderVarBE(m[1][0]);
		ReverseByteOrderVarBE(m[1][1]);
		ReverseByteOrderVarBE(m[1][2]);
		ReverseByteOrderVarBE(m[1][3]);
		ReverseByteOrderVarBE(m[2][0]);
		ReverseByteOrderVarBE(m[2][1]);
		ReverseByteOrderVarBE(m[2][2]);
		ReverseByteOrderVarBE(m[2][3]);
		return TRUE;
	}
	else return FALSE;
}
// ------------------------------------------------------------------------
