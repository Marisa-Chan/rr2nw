#ifndef __STYPES_H__
#define __STYPES_H__

typedef unsigned int   UINT;
#ifndef __STDTYPES_H__
typedef unsigned int   uint;
#endif
typedef unsigned short WORD;
#ifndef __STDTYPES_H__
typedef unsigned short word;
#endif
typedef unsigned char  BYTE;
#ifndef __STDTYPES_H__
typedef unsigned char  byte;
#endif
typedef unsigned char  UCHAR;
#ifndef __STDTYPES_H__
typedef unsigned char  uchar;
#endif
typedef unsigned long  DWORD;
#ifndef __STDTYPES_H__
typedef unsigned long  dword;
#endif
typedef unsigned long  ULONG;
#ifndef __STDTYPES_H__
typedef unsigned long  ulong;
#endif
typedef char *         STR;
typedef const char *   CSTR;

typedef int BOOL;
#ifndef __STDTYPES_H__
typedef int bool;
enum {
	FALSE = 0,
	TRUE  = 1
};
#endif
#define _BYTE(X)  ((BYTE)(X))
#define _WORD(X)  ((WORD)(X))
#define _UINT(X)  ((UINT)(X))
#define _ULONG(X) ((ULONG)(X))
#define _DWORD(X) ((DWORD)(X))

#define _CHAR(X)  ((char)(X))
#define _SHORT(X) ((short)(X))
#define _INT(X)   ((int)(X))
#define _LONG(X)  ((long)(X))

#define _CHAR2WORD(X)  _WORD(_BYTE(X))
#define _CHAR2UINT(X)  _UINT(_BYTE(X))
#define _CHAR2DWORD(X) _DWORD(_CHAR2WORD(X))
#define _CHAR2ULONG(X) _ULONG(_CHAR2WORD(X))

#define _INT2ULONG(X)  _ULONG(_UINT(X))
#define _INT2DWORD(X)  _DWORD(_UINT(X))

#define _SHORT2ULONG(X) _ULONG(_WORD(X))
#define _SHORT2DWORD(X) _DWORD(_WORD(X))

#define _DOUBLE(X) ((double)(X))
#define _FLOAT(X)  ((float)(X))

#define _CSTR(X) ((CSTR)(X))
#define _STR(X)  ((STR)(X))

#define _TO(SOUR,DEST) ((SOUR)(DEST))

#endif

