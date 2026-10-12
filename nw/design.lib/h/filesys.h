
#ifndef	__FILESYS_H__
#define	__FILESYS_H__

#ifndef	_NOAUTOINCLUDE
	#define	_NOAUTOINCLUDE
	#if !defined(__STDIO_H) && !defined(_STDIO_H_INCLUDED)
		#include<stdio.h>
	#endif
	#if !defined(__STRING_H) && !defined(_STRING_H_INCLUDED)
		#include<string.h>
	#endif
	#ifndef	__STDTYPES_H__
		#include"stdtypes.h"
	#endif
	#ifndef	__DEBUGEXT_H__
		#include"debugext.h"
	#endif
	#undef	_NOAUTOINCLUDE
#endif


class	CConfigFile {
protected:
	struct	SSection {
		struct  SEntry {
			TCchar *name;
			TCchar *m_pData;
		};
		TCchar *name;
		int m_nEntries;
		SEntry *m_pEntries;
		TCchar *operator()(TCchar *entryName);
		void Read(char *&pData,char *pEnd,TCchar *name,int &line);
		~SSection() { delete [] m_pEntries; }
	};
	char *m_pData;
	SSection	*m_pSections;
	int	m_nSections;
public:
	CConfigFile(TCchar *name,bool bMustExist=TRUE);
	~CConfigFile() { delete [] m_pSections; delete [] m_pData; }
	TCchar *operator()(TCchar *sectionName,TCchar *entryName);
	int    operator()(TCchar *sectionName,TCchar *entryName,TCchar *szFmt,...);
	int    GetInt(TCchar *sectionName,TCchar *entryName,int nDefault=0);
	double GetDouble(TCchar *sectionName,TCchar *entryName,double fDefault=0);
};

class	CFileResource {
public:
	enum LOOKUP { LU_INNERONLY, LU_INNERFIRST, LU_OUTERFIRST };
protected:
	static CFileResource	*m_pCurrent;
	char	m_szName[80];

	#if	defined(__BORLANDC__)
		#pragma	option	-a-
	#elif defined(__WATCOMC__)
		#pragma pack(1)
	#elif !defined(__sgi)
		#error	Unknown compiler
	#endif
	struct	SEntry {
		char name[13];
		long pos,len;
	} *m_pEntries;
	#if	defined(__BORLANDC__)
		#pragma	option	-a.
	#elif defined(__WATCOMC__)
		#pragma pack()
	#endif

	int  m_nEntries;
	LOOKUP m_eLookUp;
	static LOOKUP m_eDefaultLookUp;
private:
	void Init(TCchar *pszName);
public:
	CFileResource(TCchar *pszName) { Init(pszName); }
	CFileResource(TCchar *pszName,LOOKUP eLookUp) {
										Init(pszName); m_eLookUp = eLookUp; }
	~CFileResource() { delete [] m_pEntries;
							if( this == m_pCurrent ) m_pCurrent = NULL; }
	void SetLookUp(LOOKUP eLookUp) { m_eLookUp = eLookUp; }
	LOOKUP GetLookUp() const { return m_eLookUp; }
	FILE *FOpen(TCchar *pszName,long *pLen = NULL);
	void SetCurrent() { m_pCurrent = this; }
	static void SetCurrent(CFileResource *p) { m_pCurrent = p; }
	static CFileResource *GetCurrent() { return m_pCurrent; }
	static FILE *FOpen(CFileResource *_this, TCchar *pszName,long *pLen);
	static FILE *FOpenCurrent(TCchar *pszName,long *pLen = NULL) {
								return FOpen(m_pCurrent,pszName,pLen); }
};

class	CCurrentFileResource : public CFileResource {
public:
	CCurrentFileResource(TCchar *pszName)
						: CFileResource(pszName) { SetCurrent(); }
	CCurrentFileResource(TCchar *pszName,LOOKUP eLookUp)
						: CFileResource(pszName,eLookUp) { SetCurrent(); }
};

class	CTaggedFile	{
protected:
	static bool m_bDefaultRTEFatal;	// Run-time errors are fatal

	enum MODE { M_NONE, M_READ, M_WRITE };
	MODE	m_eMode;
	FILE	*m_pFile;
	bool	m_bOK;
	int		m_nVer;

	struct	SStack {
		long	lCount, lPos0;
		uint	bRTEFatal:1;
		uint	bTerminal:1;
		uint	bDebug:1;
	};

	SStack	m_stackBottom, *m_pStack, *m_pStackTop, *m_pStackEnd;
	int		m_nStackDepth, m_nDummyStack;
	enum	{ STK_INITSIZE = 50, STK_INCRSIZE = 50 };

	// Change structure packing to 1 before processing file header
	#if	defined(__BORLANDC__)
		#pragma	option	-a-
	#elif defined(__WATCOMC__)
		#pragma pack(1)
	#elif !defined(__sgi)
		#error	Unknown compiler
	#endif
	struct	SHeader	{	// File header
		dword	tag;		// four-character tag
		word	ver;		// version
		word	flags;		// flags F_DEBUG, F_TERMINAL
		dword	len;
	};
	// Revert structure packing
	#if	defined(__BORLANDC__)
		#pragma	option	-a.
	#elif defined(__WATCOMC__)
		#pragma pack()
	#endif
	enum { F_TERMINAL = 1, F_DEBUG = 2, F_LNAME = 4 };
	enum { TF_VERSION = 0x0001 };

private:
	CTaggedFile(CTaggedFile &f) { (void)f; }	// never call
	void operator=(CTaggedFile &f) { (void)f; }	// never call

public:
	enum { TF_MAXNAME = 128 };

	static bool SetDefaultRTEFatal(bool bFatal);
	static bool GetDefaultRTEFatal() { return m_bDefaultRTEFatal; }

	CTaggedFile(int nRTECritical=-1);
	~CTaggedFile();

	bool Open(TCchar *pszFileName,int nRTEFatal=-1);
	#ifndef	_BIG_ENDIAN
		bool Create(TCchar *pszFileName,int nRTEFatal=-1);
	#endif
	bool Close(bool bFullAscend=TRUE);

	bool SetRTEFatal(bool bFatal);
	bool GetRTEFatal() const { return m_pStackTop->bRTEFatal; }
	#ifdef	_DEBUG
		void SetDebug();
		void SetNoDebug();
	#else
		void SetDebug() {}
		void SetNoDebug() {}
	#endif
	bool IsOK() const { return m_bOK; }
	bool SetOK(bool bOK);

	int  Level() const { return (m_pStackTop-m_pStack)+m_nDummyStack; }
	long BytesLeft() const;	// for current chunk and for reading only
							// -1 on error
	bool QueryChunk(char *pszTagBuf,bool &bTerminal,int &nVersion,
													int nRTEFatal=-1 );
							// Buffer must be TF_MAXNAME bytes long

	int  Descend(TCchar *pszTag,bool bTerminal,int nVersion,
													int nRTEFatal=-1 );
							// Returns version or -1
							// If version!=nVersion RTE is set/raised
	bool Ascend();
	int  SkipChunk(TCchar *pszTag,bool bTerminal,int nVersion,
													int nRTEFatal=-1 ) {
				int nRes = Descend(pszTag,bTerminal,nVersion,nRTEFatal);
				Ascend(); return nRes; }
	int  WriteChunkFrom(CTaggedFile &f,TCchar *pszTag,bool bTerminal,int nVersion,
													int nRTEFatal=-1 );

	#ifndef	_BIG_ENDIAN
		long Write(TCvoid *buf,long count);
		bool WriteBool(bool b);
		bool WriteByte(byte n);
		bool WriteWord(word w);
		bool WriteDWord(dword dw);
		bool WriteIByte(ibyte n);
		bool WriteIWord(iword iw);
		bool WriteIDWord(idword idw);
		bool WriteChar(char c);
		bool WriteUChar(uchar uc);
		bool WriteShort(short n);
		bool WriteUShort(ushort n);
		bool WriteInt(int i);
		bool WriteUInt(uint n);
		bool WriteLong(long l);
		bool WriteULong(ulong l);
	    bool WriteDouble(double f);
	#else
		long ReadBE(void *buf,long count);
	#endif

	long Read(void *buf,long count);
	bool ReadBool(bool &b);
	bool ReadByte(byte &n);
	bool ReadWord(word &w);
	bool ReadDWord(dword &w);
	bool ReadIByte(ibyte &n);
	bool ReadIWord(iword &iw);
	bool ReadIDWord(idword &idw);
	bool ReadChar(char &c);
	bool ReadUChar(uchar &uc);
	bool ReadShort(short &n);
	bool ReadUShort(ushort &n);
	bool ReadInt(int &i);
	bool ReadUInt(uint &n);
	bool ReadLong(long &l);
	bool ReadULong(ulong &l);
	bool ReadDouble(double &f);

	class	CDescend {
	protected:
		CTaggedFile &m_f;
		int res;
	public:
		CDescend(CTaggedFile &f,TCchar *pszTag,bool bTerminal,int nVersion,
						 						int nRTEFatal=-1) : m_f(f) {
			res = f.Descend(pszTag,bTerminal,nVersion,nRTEFatal); }
		~CDescend() { m_f.Ascend(); }
		operator int() const { return res; }
	};
};

template<class T> inline bool Write(CTaggedFile &f,const T &v) {
									return f.Write(&v,sizeof(T))==sizeof(T); }
template<class T> inline bool Read(CTaggedFile &f,T &v) {
									return f.Read(&v,sizeof(T))==sizeof(T); }
template<class T> inline bool WriteEnum(CTaggedFile &f,const T &v) {
													return f.WriteInt(v); }
template<class T> inline bool ReadEnum(CTaggedFile &f,T &v) {
				int n; if(!f.ReadInt(n)) return FALSE; v=n; return TRUE; }

#endif	//	__FILESYS_H__
