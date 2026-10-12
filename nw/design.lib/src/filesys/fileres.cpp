

#ifndef	__FILESYS_CPP__
	#include<stdio.h>
	#include<string.h>
	#define _NOAUTOINCLUDE
	#include"stdtypes.h"
	#include"debugext.h"
	#include"filesys.h"
#endif


CFileResource	*CFileResource::m_pCurrent = NULL;
CFileResource::LOOKUP	CFileResource::m_eDefaultLookUp = LU_INNERONLY;


void CFileResource::Init(TCchar *pszName)
{
	FILE *f = fopen(pszName,"rb");
	RTCHECK1(f,"Cannot open file resource '%s'",pszName);
	fseek(f,-4L,SEEK_END);
	long pos;
	RTCHECK1(fread(&pos,4,1,f)==1,"Error in resource '%s'",pszName);
	ReverseByteOrderBE(&pos,4);
	fseek(f,pos,SEEK_SET);
	RTCHECK1(fread(&m_nEntries,4,1,f)==1,"Error in resource '%s'",pszName);
	ReverseByteOrderBE(&m_nEntries,4);
	if( m_nEntries > 0 ) {
		m_pEntries = new SEntry[m_nEntries];
		RTCHECK1(fread(m_pEntries,sizeof(SEntry)*m_nEntries,1,f)==1,"Error in resource '%s'",pszName);
	}
	else m_pEntries = NULL;
	fclose(f);
	#ifdef _BIG_ENDIAN
		for( int i = 0 ; i < m_nEntries ; i++ ) {
			ReverseByteOrderBE(&m_pEntries->pos,4);
			ReverseByteOrderBE(&m_pEntries->len,4);
		}
	#endif
	strcpy(m_szName,pszName);
	m_eLookUp = m_eDefaultLookUp;
}

FILE *CFileResource::FOpen(CFileResource *_this, TCchar *pszName,long *pLen)
{
	if(_this) {
		if(_this->m_eLookUp==LU_OUTERFIRST) {
			FILE *f = fopen(pszName,"rb");
			if(f) {
				if(pLen) {
					RTCHECK(fseek(f,0,SEEK_END)==0 &&
						(*pLen = ftell(f)) >= 0 &&
						fseek(f,0,SEEK_SET)==0,"OuterOpen Len");
				}
				return f;
			}
		}
		int n = -1;
		for( int u = _this->m_nEntries-1, l = 0 ; l <= u ; ) {
			int m = (u+l)/2;
			int res = strcmpi(_this->m_pEntries[m].name,pszName);
			if(res == 0) { n = m; break; }
			if(res<0) l = m+1; else u = m-1;
		}
		if(n>=0) {
			FILE *f = fopen(_this->m_szName,"rb");
			fseek(f,_this->m_pEntries[n].pos,SEEK_SET);
			if(pLen) *pLen = _this->m_pEntries[n].len;
			return f;
		}
	}
	if(_this==NULL||_this->m_eLookUp==LU_INNERFIRST) {
		FILE *f = fopen(pszName,"rb");
		if(f) {
			if(pLen) {
				RTCHECK(fseek(f,0,SEEK_END)==0 &&
					(*pLen = ftell(f)) >= 0 &&
					fseek(f,0,SEEK_SET)==0,"OuterOpen Len");
			}
			return f;
		}
	}
	return NULL;
}

FILE *CFileResource::FOpen(TCchar *pszName,long *pLen)
{
	return FOpen(this, pszName, pLen);
}