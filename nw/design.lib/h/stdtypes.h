
#ifndef	__STDTYPES_H__
#define	__STDTYPES_H__

#define	__STDTYPES_IBOOL

#if	defined(__STDTYPES_IBOOL)
//#define	bool ibool
#endif

typedef	int ibool;
#define	TRUE	1
#define	FALSE	0

#define	ISBOOL(X)	((X)==TRUE||(X)==FALSE)
#define	MAKEBOOL(X)	((int)((X)!=0))

typedef	unsigned char byte;
typedef	unsigned short word;
typedef unsigned long dword;
typedef	unsigned uint;
typedef	unsigned char uchar;
typedef	unsigned short ushort;
typedef unsigned long ulong;
typedef	signed char ibyte;
typedef	short iword;
typedef	long idword;

typedef	const bool TCbool;
typedef	const ibool TCibool;
typedef	const void TCvoid;
typedef const int	TCint;
typedef	const char TCchar;
typedef	const byte TCbyte;
typedef const word TCword;
typedef	const dword TCdword;
typedef	const uint TCuint;
typedef	const uchar TCuchar;
typedef	const ushort TCushort;
typedef	const ulong TCulong;
typedef	const ibyte TCibyte;
typedef	const iword TCiword;
typedef	const idword TCidword;
typedef	const short TCshort;
typedef	const long TClong;
typedef const double TCdouble;

#ifdef	__cplusplus
	template<class T> void Swap(T &a,T &b) { T t; t=a; a=b; b=t; }
	template<class T1,class T2> void _Swap(T1 &a,T2 &b) { T1 t; t=a; a=b; b=t; }
	inline TCchar *SubstNULL(TCchar *szStr,TCchar *szDefault) { return szStr ? szStr : szDefault; }
#endif

#ifdef	__sgi
	#define	_BIG_ENDIAN
	#define	strcmpi strcasecmp
	#ifdef __cplusplus
		inline char *strlwr(char *s) { for(char *t=s;*t;t++) if(*t>='A'&&*t<='Z') *t|=0x20; return s; }
	#endif
#endif

#ifdef	__cplusplus
#ifdef	_BIG_ENDIAN
		inline void ReverseByteOrderBE(void *buf,int l) {
			int	l2 = l--/2;
			for( int i = 0 ; i < l2 ; i++ )	{
				byte	t = ((byte*)buf)[i];
				((byte*)buf)[i] = ((byte*)buf)[l-i];
				((byte*)buf)[l-i] = t;
			}
		}
	#define	ReverseByteOrderVarBE(X) ReverseByteOrderBE(&X,sizeof(X))
#else
	#define ReverseByteOrderBE(buf,l) ((void)0)
	#define	ReverseByteOrderVarBE(X) ((void)0)
#endif
#endif


#if defined(__cplusplus) && defined(__WATCOMC__)
#pragma warning 604 10
#pragma warning 594 10
#pragma warning 595 10
#pragma warning 549 10
#endif

#endif	// __STDTYPES_H__

