
#ifndef	__FILESYS_CPP__
	#include<stdio.h>
	#include<string.h>
	#define _NOAUTOINCLUDE
	#include"stdtypes.h"
	#include"debugext.h"
	#include"filesys.h"
#endif

// TaggedFile vesion changes
//		0->1. Long names introduced

//	Stack fields:
//		lPos0 = Hdr end
//		Read:	lCount = remaining bytes
//		Write:	lCount = Hdr/LName end

// --------------------------------------------------------------------------
// ------------------- CTaggedFile  private: --------------------------------
// --------------------------------------------------------------------------
bool CTaggedFile::m_bDefaultRTEFatal = FALSE;
// --------------------------------------------------------------------------
bool CTaggedFile::SetDefaultRTEFatal(bool bFatal)
{
	ASSERT(ISBOOL(bFatal));
	bool	bPrev = m_bDefaultRTEFatal;
	m_bDefaultRTEFatal = bFatal;
	return	bPrev;
}
// --------------------------------------------------------------------------
// ------------------ CTaggedFile  public: ----------------------------------
// --------------------------------------------------------------------------
CTaggedFile::CTaggedFile(int nRTEFatal)
{
	m_eMode = M_NONE;
	m_pFile = NULL;
	m_pStackTop = &m_stackBottom;
	m_pStack = NULL;
	m_stackBottom.bRTEFatal = nRTEFatal >= 0 ? nRTEFatal :
													m_bDefaultRTEFatal;
	m_nStackDepth = m_nDummyStack = 0;
	m_bOK = TRUE;
}
// --------------------------------------------------------------------------
CTaggedFile::~CTaggedFile()
{
	if(m_pFile)	Close();
}
// --------------------------------------------------------------------------
bool CTaggedFile::Open(TCchar *pszFileName,int nRTEFatal)
{
	RTCHECK(m_pFile==NULL,"TF Open Already");
	ASSERT(m_eMode==M_NONE);
	ASSERT(!m_pStack && m_pStackTop==&m_stackBottom);
	m_bOK = TRUE;
	m_stackBottom.bTerminal = FALSE;
	m_stackBottom.bDebug = FALSE;
    m_nDummyStack = 0;
	if( nRTEFatal < 0 )	nRTEFatal = m_stackBottom.bRTEFatal;
	//if(!(m_pFile = fopen(pszFileName,"rb"))) {
	long fileLen;
	if((m_pFile = CFileResource::FOpenCurrent(pszFileName,&fileLen))==NULL) {
		RTCHECK1( !nRTEFatal, "Cannot open file '%s'", pszFileName);
		return m_bOK=FALSE;
	}
	m_stackBottom.lCount = m_stackBottom.lPos0 = fileLen;
	m_eMode = M_READ;
	m_nVer = Descend("TFBF",FALSE,TF_VERSION,FALSE);
	if( (unsigned)m_nVer > TF_VERSION ) {
		if( m_pStack ) Ascend();
		fclose(m_pFile); m_pFile = NULL;
		m_eMode = M_NONE;
		RTCHECK(!nRTEFatal,"TF Ver");
		return	m_bOK=FALSE;
	}
	SetOK(TRUE);
	SetRTEFatal(nRTEFatal);
	//ASSERT(m_bOK);
	return TRUE;
}
// --------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool CTaggedFile::Create(TCchar *pszFileName,int nRTEFatal)
{
	RTCHECK(m_pFile==NULL,"TF Create Already");
	ASSERT(m_eMode==M_NONE);
	ASSERT(!m_pStack && m_pStackTop==&m_stackBottom);
	m_bOK = TRUE;
	m_stackBottom.bTerminal = FALSE;
	m_stackBottom.bDebug = FALSE;
	m_nDummyStack = 0;
	if( nRTEFatal < 0 )	nRTEFatal = m_stackBottom.bRTEFatal;
	if((m_pFile = fopen(pszFileName,"wb"))==NULL) {
		RTCHECK1( !nRTEFatal, "Cannot create file '%s'", pszFileName);
		return FALSE;
	}
	m_eMode = M_WRITE;
	m_stackBottom.lPos0 = 0;
	m_stackBottom.lCount = -1L;
	if( Descend("TFBF",FALSE,TF_VERSION,nRTEFatal) < 0 )	{
		if( m_pStack )	Ascend();
		fclose(m_pFile); m_pFile = NULL;
		m_eMode = M_NONE;
		return	m_bOK=FALSE;
	}
	ASSERT(m_bOK);
	return	TRUE;
}
#endif
// --------------------------------------------------------------------------
bool CTaggedFile::Close(bool bFullAscend)
{
	ASSERT(m_pFile!=NULL);
	ASSERT(m_pStack!=NULL);
	RTCHECK(m_pStackTop==m_pStack||!bFullAscend,"TF NoAsc");
								// User must ascend from all chunks
	if( bFullAscend ) {
		Ascend();	// Ascend from TFBF chunk
	} else {
		while(m_pStack)	Ascend();	// Ascend from all chunks
	}
	RTCHECK(!m_pStack,"TF Close");
	if( fclose(m_pFile) != 0 )
		return m_pFile=NULL, m_eMode=M_NONE, m_bOK=FALSE;
	return	m_pFile=NULL, m_eMode=M_NONE, m_bOK;
}
// --------------------------------------------------------------------------
bool CTaggedFile::SetRTEFatal(bool bFatal)
{
	ASSERT(ISBOOL(bFatal));
	bool  bPrev = m_pStackTop->bRTEFatal;
	m_pStackTop->bRTEFatal = bFatal;
	if( bFatal )	RTCHECK(m_bOK,"TF Set Fatal");
	return	bPrev;
}
// --------------------------------------------------------------------------
#ifdef	_DEBUG
void CTaggedFile::SetDebug()
{
	ASSERT(m_eMode==M_WRITE);
	ASSERT(!m_pStackTop->bTerminal);
	m_pStackTop->bDebug = TRUE;
}
#endif
// --------------------------------------------------------------------------
#ifdef	_DEBUG
void CTaggedFile::SetNoDebug()
{
	ASSERT(m_eMode==M_WRITE);
	ASSERT(!m_pStackTop->bTerminal);
	m_pStackTop->bDebug = FALSE;
}
#endif
// --------------------------------------------------------------------------
bool CTaggedFile::SetOK(bool bOK)
{
	ASSERT(ISBOOL(bOK));
	RTCHECK(bOK||!m_pStackTop->bRTEFatal,"TF SetErr");
	if( m_nDummyStack <= 0 )
		m_bOK = bOK;
	return	m_bOK;
}
// --------------------------------------------------------------------------
long CTaggedFile::BytesLeft() const
{
	ASSERT( m_pFile && m_eMode==M_READ );
	ASSERT( m_nDummyStack == 0 || ! m_bOK );

	if( m_nDummyStack != 0 || !m_bOK )	return -1L;
	return	m_pStackTop->lCount;
}
// --------------------------------------------------------------------------
bool  CTaggedFile::QueryChunk(char *pszTagBuf,bool &bTerminal,int &nVersion,
															int nRTEFatal )
{
	ASSERT( m_pFile && m_eMode==M_READ );

	ASSERT( m_nDummyStack == 0 || ! m_bOK );
	if( nRTEFatal < 0 )	nRTEFatal = m_pStackTop->bRTEFatal;
	if(m_pStackTop->bTerminal) {
		RTCHECK(!nRTEFatal,"TF Query term");
		return m_bOK = FALSE;
	}

	SHeader	hdr;

	if( m_pStackTop->lCount < sizeof(SHeader) ) {
		RTCHECK(!nRTEFatal,"TF QueryRH");
		return m_bOK=FALSE;
	}

	if( fread(&hdr,1,sizeof(hdr),m_pFile) != sizeof(hdr) ) {
		RTCHECK(FALSE,"TF QueryRHR");
						// We just have checked remaining length, so if
						// we did not read then it's something very very
						// special!
		return	m_bOK=FALSE;
						// Just for compiler and for sure
	}
	long lSeekOffset = -(int)sizeof(hdr);
	*(dword*)pszTagBuf = hdr.tag; pszTagBuf[4] = 0;
	if(hdr.flags & F_LNAME) {
		uint  l = ((byte*)&hdr.tag)[3];
		pszTagBuf[3] = 0; pszTagBuf[l] = 0;
		if(l>=128) {
			RTCHECK(!nRTEFatal,"TF QNameLen");
			return m_bOK=FALSE;
		}
		if( l > 3 ) {
			if( m_pStackTop->lCount < sizeof(SHeader)+l ) {
				RTCHECK(!nRTEFatal,"TF QueryRH");
				return m_bOK=FALSE;
			}
			if( fread(pszTagBuf,1,l,m_pFile) != l ) {
				RTCHECK(FALSE,"TF QueryRHR");	// Fatal!!!
				return m_bOK=FALSE;
			}
			lSeekOffset -= l;
		}
	}
	bTerminal = hdr.flags & F_TERMINAL;
	nVersion = hdr.ver;
	fseek(m_pFile,lSeekOffset,SEEK_CUR);
	return TRUE;
}
// --------------------------------------------------------------------------
int  CTaggedFile::Descend(TCchar *pszTag,bool bTerminal,int nVersion,
															int nRTEFatal )
									// Returns version or -1
									// If version<nVersion return -1
{
	//ASSERT(strlen(pszTag)==4);
	ASSERT(ISBOOL(bTerminal));
	ASSERT(m_pFile);

	ASSERT( m_nDummyStack == 0 || ! m_bOK );
	if( ! m_bOK )	{ RTCHECK(!m_pStackTop->bRTEFatal,"TF Write !OK"); return m_nDummyStack++, FALSE; }

	if( nRTEFatal < 0 )	nRTEFatal = m_pStackTop->bRTEFatal;

	if(m_pStackTop->bTerminal) {
		RTCHECK1(!nRTEFatal,"TF Desc(%s) term",pszTag);
		return m_nDummyStack++,m_bOK=FALSE,-1;
	}

	SHeader	hdr;
	long	lHdrEndPos = -1;
	uint	nNameLen = 4;

	switch(m_eMode) {
		case M_READ: {
			if( m_pStackTop->lCount < sizeof(SHeader) ) {
			ErrRLen:
				RTCHECK1(!nRTEFatal,"TF Desc(%s) RH",pszTag);
				return	m_nDummyStack++,m_bOK=FALSE,-1;
			}
			if( fread(&hdr,1,sizeof(hdr),m_pFile) != sizeof(hdr) ) {
			ErrRHR:
				RTCHECK1(FALSE,"TF Desc(%s) RHR",pszTag);
						// We just have checked remaining length, so if
						// we did not read then it's something very very
						// special!
				return	m_nDummyStack++,m_bOK=FALSE,-1;
						// Just for compiler and for sure
			}
			ReverseByteOrderBE(&hdr.ver,sizeof(hdr.ver));
			ReverseByteOrderBE(&hdr.flags,sizeof(hdr.flags));
			ReverseByteOrderBE(&hdr.len,sizeof(hdr.len));
			lHdrEndPos = ftell(m_pFile);
			if( lHdrEndPos < 0 ) goto ErrRHR;
			if(hdr.flags & F_LNAME) {
				ASSERT(m_nVer>0);
				if(((byte*)&hdr.tag)[3]>=128) {
					RTCHECK1(!nRTEFatal,"TF RNameLen (%s)",pszTag);
					return	m_nDummyStack++,m_bOK=FALSE,-1;
				}
				uint l = strlen(pszTag);
				if(l!=((byte*)&hdr.tag)[3]) goto ErrRTag;
				switch(l) {
					case 3: if(((char*)&hdr.tag)[2]!=pszTag[2]) goto ErrRTagS;
					case 2: if(((char*)&hdr.tag)[1]!=pszTag[1]) goto ErrRTagS;
					case 1: if(((char*)&hdr.tag)[0]!=pszTag[0]) goto ErrRTagS;
					case 0: break;
					ErrRTagS: RTCHECK3(!nRTEFatal,"TF Desc(%s) RTag(%.*s)",pszTag,&hdr.tag,l); goto ErrRTag;
					default: {
						if(m_pStackTop->lCount<sizeof(SHeader)+l) goto ErrRLen;
						char	buf[128]; buf[l]=0;
						if(fread(buf,1,l,m_pFile)!=l) goto ErrRHR;
						if(strcmpi(buf,pszTag)!=0) {
							RTCHECK2(!nRTEFatal,"TF Desc(%s) RTag(%s)",pszTag,buf);
							goto ErrRTag;
						}
					} break;
				}
			} // if LNAME
			else // if !LNAME
			if(hdr.tag!=*(dword*)pszTag) {
				RTCHECK2(!nRTEFatal,"TF Desc(%s) RTag(%.4s)",pszTag,&hdr.tag);
			ErrRTag:
				return m_nDummyStack++,m_bOK=FALSE,-1;
			}
			RTCHECK3(hdr.ver==(word)nVersion||!nRTEFatal,"TF Desc(%s:%d) RVer(:%d)",pszTag,nVersion,hdr.ver);
			if((long)hdr.len<0) {
				RTCHECK1(!nRTEFatal,"TF Desc(%s) RLen",pszTag);
				return m_nDummyStack++, m_bOK=FALSE,-1;
			}
			#ifndef	_DEBUG
			//	RTCHECK(!(hdr.flags&F_DEBUG),"TF DescRDbg");
			#endif
			if(MAKEBOOL(hdr.flags&F_TERMINAL)!=bTerminal) {
				RTCHECK3(!nRTEFatal,"TF Desc(%s:%c) Term(%c)",pszTag,bTerminal?'T':'N',(hdr.flags&F_TERMINAL)?'T':'N');
				return m_nDummyStack++, m_bOK=FALSE,-1;
			}
		} break;
	case M_WRITE: {
			hdr.flags = (word)((m_pStackTop->bDebug?F_DEBUG:0)|
								(bTerminal?F_TERMINAL:0));
			nNameLen = strlen(pszTag);
			if(nNameLen>=128) {
				RTCHECK1(!nRTEFatal,"TF Desc(%s) WNameLen",pszTag);
				return	m_nDummyStack++,m_bOK=FALSE,-1;
			}
			if(nNameLen==4)
				hdr.tag = *(dword*)pszTag;
			else {
				switch(nNameLen) {
					case 3: ((char*)&hdr.tag)[2] = pszTag[2];
					case 2: ((char*)&hdr.tag)[1] = pszTag[1];
					case 1: ((char*)&hdr.tag)[0] = pszTag[0];
					case 0: break;
					default: hdr.tag=0x4E4C40L; //"@LN"
				}
				((byte*)&hdr.tag)[3] = (byte)nNameLen;
				hdr.flags |= F_LNAME;
			}
			hdr.ver = (word)nVersion;
			hdr.len = (dword)-1L;
			if( fwrite(&hdr,1,sizeof(hdr),m_pFile) != sizeof(hdr) ||
				(lHdrEndPos = ftell(m_pFile)) < 0 || nNameLen > 4 &&
							fwrite(pszTag,1,nNameLen,m_pFile)!=nNameLen ) {
				RTCHECK1(!nRTEFatal,"TF Desc(%s) WH",pszTag);
				return	m_nDummyStack++,m_bOK=FALSE,-1;
			}
		} break;
		default:
			ASSERT(FALSE);
	}

	if( !m_pStack ) {
		m_pStack = new SStack[m_nStackDepth=STK_INITSIZE];
		if(!m_pStack)	{
			RTCHECK1(!nRTEFatal,"TF Desc(%s) Stk",pszTag);
			ASSERT(m_nDummyStack == 0 );
			return	m_nDummyStack++, m_bOK = FALSE, -1;
		}
		else {
			m_pStackTop = m_pStack;
			m_pStackEnd = m_pStack+m_nStackDepth;
			*m_pStackTop = m_stackBottom;
		}
	}
	else
	if( ++m_pStackTop == m_pStackEnd ) {
		int		nNewStackDepth = m_nStackDepth+STK_INCRSIZE;
		SStack	*pNewStack = new SStack[nNewStackDepth];
		if(!pNewStack)	{
			RTCHECK1(!nRTEFatal,"TF Desc(%s) Stk",pszTag);
			return	m_nDummyStack++, m_bOK = FALSE, -1;
		}
		else {
			memcpy(pNewStack,m_pStack,sizeof(SStack)*m_nStackDepth);
			pNewStack[m_nStackDepth] = *m_pStackTop;
			delete	[] m_pStack;
			m_pStack = pNewStack;
			m_pStackTop = pNewStack + m_nStackDepth;
			m_pStackEnd = pNewStack + nNewStackDepth;
			m_nStackDepth = nNewStackDepth;
		}
	}

	ASSERT(m_bOK);

	m_pStackTop->lPos0 = lHdrEndPos;
	if( m_eMode == M_WRITE )
		m_pStackTop->lCount = lHdrEndPos + (nNameLen > 4?nNameLen:0);
	else
		m_pStackTop->lCount = (long)hdr.len;
	m_pStackTop->bTerminal = bTerminal;
	m_pStackTop->bDebug = MAKEBOOL(hdr.flags&F_DEBUG);
	m_pStackTop->bRTEFatal = nRTEFatal;
	return	m_bOK ? (int)hdr.ver : -1;
}
// --------------------------------------------------------------------------
bool	CTaggedFile::Ascend()
{
	// Do not check m_bOK on entry here for Ascend() must be possible
	// to perform even if errors occured. Still return value not better
	// than m_bOK on entry.
	ASSERT(m_pStack||m_nDummyStack);
	ASSERT(m_pFile);

	if( m_nDummyStack > 0 )  return ASSERT(!m_bOK), m_nDummyStack--, m_bOK;

	long	lCount;

	if( m_bOK )
	switch(m_eMode) {
		case M_READ: {
			long	lPos;
			if( m_pStackTop->lCount > 0 ) {
				if( fseek(m_pFile,m_pStackTop->lCount,SEEK_CUR) != 0 ) {
					RTCHECK(!m_pStackTop->bRTEFatal,"TF AscRSk");
					return 	m_bOK=FALSE;
				}
			}
			if( (lPos=ftell(m_pFile)) < 0 )	{
				RTCHECK(!m_pStackTop->bRTEFatal,"TF AscWTl");
				return m_bOK = FALSE;
			}
			lCount = lPos - m_pStackTop->lPos0;
		} break;
		case M_WRITE: {
			long	lPos;
			if( (lPos=ftell(m_pFile)) < 0 )	{
				RTCHECK(!m_pStackTop->bRTEFatal,"TF AscWTl");
				return m_bOK = FALSE;
			}
			if( fseek(m_pFile,m_pStackTop->lPos0-4,SEEK_SET) < 0 ) {
				RTCHECK(!m_pStackTop->bRTEFatal,"TF AscWSk");
				return m_bOK = FALSE;
			}
			dword	dwLen = (dword)(lPos-m_pStackTop->lCount);
			if( fwrite(&dwLen,1,4,m_pFile)!=4 ) {
				RTCHECK(!m_pStackTop->bRTEFatal,"TF AscWL");
				return m_bOK = FALSE;
			}
			if( fseek(m_pFile,lPos,SEEK_SET) < 0 ) {
				RTCHECK(!m_pStackTop->bRTEFatal,"TF AscWSk1");
				return m_bOK = FALSE;
			}
		} break;
		default:
			ASSERT(FALSE);
	}

	if( m_pStackTop == m_pStack ) {
		delete	[] m_pStack;
		m_pStack = NULL;
		m_pStackTop = &m_stackBottom;
	}
	else
		m_pStackTop--;
	if( m_bOK && m_eMode == M_READ ) {
		if((m_pStackTop->lCount-=lCount+sizeof(SHeader))<0) {
			RTCHECK(!m_pStackTop->bRTEFatal,"Asc UpCnt");
			return m_bOK = FALSE;
		}
	}
	return	m_bOK;
}
// --------------------------------------------------------------------------
int  CTaggedFile::WriteChunkFrom(CTaggedFile &f,TCchar *pszTag,bool bTerminal,int nVersion,
												int nRTEFatal )
{
	RTCHECK(m_eMode==M_WRITE,"TF Wmode");
	RTCHECK(f.m_eMode==M_READ,"TF Rmode");
	int ver = f.Descend(pszTag,bTerminal,nVersion,nRTEFatal);
	if( ver < 0 )	return ver;
	bool bOldDebug = m_pStackTop->bDebug;
	m_pStackTop->bDebug = f.m_pStackTop->bDebug;
	// here res contains version of source chunk
	int res = Descend(pszTag,bTerminal,ver,nRTEFatal);
	if( res < 0 )	{ f.Ascend(); Ascend(); m_pStackTop->bDebug = bOldDebug; return res; }

	while( f.m_pStackTop->lCount > 0 ) {
		byte c;
		if(fread(&c,1,1,f.m_pFile)!=1) {
			RTCHECK(!m_pStackTop->bRTEFatal,"TF R");
			m_bOK=FALSE; f.Ascend(); Ascend();
			m_pStackTop->bDebug = bOldDebug; return -1;
		}
		f.m_pStackTop->lCount--;
		if(fwrite(&c,1,1,m_pFile)!=1) {
			RTCHECK(!m_pStackTop->bRTEFatal,"TF W");
			m_bOK=FALSE; f.Ascend(); Ascend();
			m_pStackTop->bDebug = bOldDebug; return -1;
		}
	}
	if( !f.Ascend() )	ver = -1;
	if( !Ascend() )	ver = -1;
	m_pStackTop->bDebug = bOldDebug;
	if( ver < 0 )	m_bOK = FALSE;
	return ver;
}
// --------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
long CTaggedFile::Write(TCvoid *buf,long count)
{
	RTCHECK(m_eMode==M_WRITE,"TF Wmode");
    RTCHECK(m_pStackTop->bTerminal, "TF Nonterm");
	ASSERT(m_pStack);
	ASSERT(m_pFile);
	if( !m_bOK )	{ RTCHECK(!m_pStackTop->bRTEFatal,"TF Write !OK"); return -1; }
	#ifdef	 _DEBUG
		if( m_pStackTop->bDebug ) {
			if( fwrite(&count,1,4,m_pFile)!=4 ) {
				RTCHECK(!m_pStackTop->bRTEFatal,"TF W");
				return 	m_bOK=FALSE;
			}
		}
	#endif
	if( fwrite(buf,1,count,m_pFile)!=count ) {
		RTCHECK(!m_pStackTop->bRTEFatal,"TF W");
		return 	m_bOK=FALSE;
	}
	//m_pStackTop->lCount += count;
	return	count;
}
#endif
// --------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool CTaggedFile::WriteBool(bool b)
{
	long	l = b;
	return	Write(&l,4)==4;
}
#endif
// --------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool CTaggedFile::WriteByte(byte n)
{
	return	Write(&n,1)==1;
}
#endif
// --------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool CTaggedFile::WriteWord(word w)
{
	return	Write(&w,2)==2;
}
#endif
// --------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool CTaggedFile::WriteDWord(dword dw)
{
	return	Write(&dw,4)==4;
}
#endif
// --------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool CTaggedFile::WriteIByte(ibyte n)
{
	return	Write(&n,1)==1;
}
#endif
// --------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool CTaggedFile::WriteIWord(iword iw)
{
	return	Write(&iw,2)==2;
}
#endif
// --------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool CTaggedFile::WriteIDWord(idword idw)
{
	return	Write(&idw,4)==4;
}
#endif
// --------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool CTaggedFile::WriteChar(char c)
{
	return	Write(&c,1)==1;
}
#endif
// --------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool CTaggedFile::WriteUChar(uchar uc)
{
	return	Write(&uc,1)==1;
}
#endif
// --------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool CTaggedFile::WriteShort(short n)
{
	return	Write(&n,2)==2;
}
#endif
// --------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool CTaggedFile::WriteUShort(ushort n)
{
	return	Write(&n,2)==2;
}
#endif
// --------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool CTaggedFile::WriteInt(int i)
{
	#if defined(__386__) || defined(_BIG_ENDIAN)
		return	Write(&i,4)==4;
	#else
		long	l = i;
		return	Write(&l,4)==4;
	#endif
}
#endif
// --------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool CTaggedFile::WriteUInt(uint n)
{
	#if defined(__386__) || defined(_BIG_ENDIAN)
		return	Write(&n,4)==4;
	#else
		ulong	nl = n;
		return	Write(&nl,4)==4;
	#endif
}
#endif
// --------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool CTaggedFile::WriteLong(long l)
{
	return	Write(&l,4)==4;
}
#endif
// --------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool CTaggedFile::WriteULong(ulong n)
{
	return	Write(&n,4)==4;
}
#endif
// --------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
bool CTaggedFile::WriteDouble(double f)
{
	return	Write(&f,8)==8;
}
#endif
// --------------------------------------------------------------------------
long CTaggedFile::Read(void *buf,long count)
{
	RTCHECK(m_eMode==M_READ,"TF Rmode");
	if( !m_bOK )	{ RTCHECK(!m_pStackTop->bRTEFatal,"TF Read !OK"); return -1; }
	RTCHECK(m_pStackTop->bTerminal, "TF Nonterm");
	ASSERT(m_pStack);
	ASSERT(m_pFile);
	//#ifdef	 _DEBUG
		if( m_pStackTop->bDebug ) {
			if( count+4 > m_pStackTop->lCount ) {
				RTCHECK(!m_pStackTop->bRTEFatal,"TF Rcnt");
				return 	m_bOK=FALSE;
			}
			long	cnt1;
			if( fread(&cnt1,1,4,m_pFile)!=4 ) {
				RTCHECK(!m_pStackTop->bRTEFatal,"TF R dbgcnt");
				return 	m_bOK=FALSE;
			}
			ReverseByteOrderBE(&cnt1,4);
			m_pStackTop->lCount -= 4;
			RTCHECK(cnt1==count,"TF R dbgcnt!=");
		} else
	//#endif
	if( count > m_pStackTop->lCount ) {
		RTCHECK(!m_pStackTop->bRTEFatal,"TF Rcnt");
		return 	m_bOK=FALSE;
	}
	if( fread(buf,1,count,m_pFile)!=count ) {
		RTCHECK(!m_pStackTop->bRTEFatal,"TF R");
		return 	m_bOK=FALSE;
	}
	m_pStackTop->lCount -= count;
	return	count;
}
// --------------------------------------------------------------------------
#ifdef _BIG_ENDIAN
inline long CTaggedFile::ReadBE(void *buf,long count)
{
	long	l = Read(buf,count);
	ReverseByteOrderBE(buf,count);
	return l;
}
#else
#define	ReadBE(buf,count) Read(buf,count)
#endif
// --------------------------------------------------------------------------
bool CTaggedFile::ReadBool(bool &b)
{
	long	l;
	if( Read(&l,4)!=4 )	return FALSE;
	return	b = (bool)l, TRUE;

}
// --------------------------------------------------------------------------
bool CTaggedFile::ReadByte(byte &n)
{
	return	Read(&n,1)==1;
}
// --------------------------------------------------------------------------
bool CTaggedFile::ReadWord(word &w)
{
	return	ReadBE(&w,2)==2;
}
// --------------------------------------------------------------------------
bool CTaggedFile::ReadDWord(dword &dw)
{
	return	ReadBE(&dw,4)==4;
}
// --------------------------------------------------------------------------
bool CTaggedFile::ReadIByte(ibyte &n)
{
	return	Read(&n,1)==1;
}
// --------------------------------------------------------------------------
bool CTaggedFile::ReadIWord(iword &iw)
{
	return	ReadBE(&iw,2)==2;
}
// --------------------------------------------------------------------------
bool CTaggedFile::ReadIDWord(idword &idw)
{
	return	ReadBE(&idw,4)==4;
}
// --------------------------------------------------------------------------
bool CTaggedFile::ReadChar(char &c)
{
	return	Read(&c,1)==1;
}
// --------------------------------------------------------------------------
bool CTaggedFile::ReadUChar(uchar &uc)
{
	return	Read(&uc,1)==1;
}
// --------------------------------------------------------------------------
bool CTaggedFile::ReadShort(short &n)
{
	return	ReadBE(&n,2)==2;
}
// --------------------------------------------------------------------------
bool CTaggedFile::ReadUShort(ushort &n)
{
	return	ReadBE(&n,2)==2;
}
// --------------------------------------------------------------------------
bool CTaggedFile::ReadInt(int &i)
{
	#if defined(__386__) || defined(_BIG_ENDIAN)
		return	ReadBE(&i,4)==4;
	#else
		long	l;
		if( Read(&l,4)!=4 )	return FALSE;
		return	i = (int)l, TRUE;
	#endif
}
// --------------------------------------------------------------------------
bool CTaggedFile::ReadUInt(uint &n)
{
	#if defined(__386__) || defined(_BIG_ENDIAN)
		return	ReadBE(&n,4)==4;
	#else
		ulong	nl;
		if( Read(&nl,4)!=4 )	return FALSE;
		return	n = (uint)nl, TRUE;
	#endif
}
// --------------------------------------------------------------------------
bool CTaggedFile::ReadLong(long &l)
{
	return	ReadBE(&l,4)==4;
}
// --------------------------------------------------------------------------
bool CTaggedFile::ReadULong(ulong &n)
{
	return	ReadBE(&n,4)==4;
}
// --------------------------------------------------------------------------
bool CTaggedFile::ReadDouble(double &f)
{
	return	ReadBE(&f,8)==8;
}
// --------------------------------------------------------------------------
