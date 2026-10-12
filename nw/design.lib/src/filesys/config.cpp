
#ifndef	__FILESYS_CPP__
	#include<stdio.h>
	#include<string.h>
	#define _NOAUTOINCLUDE
	#include"stdtypes.h"
	#include"debugext.h"
	#include"filesys.h"
#endif

#include<ctype.h>
#include<stdarg.h>

static bool IsEol(char c)
{
	return c == '\n' || c == '\r';
}

static bool SkipLine(char *&p,char *pEnd)
{
	bool bNonZ = FALSE;
	while( p < pEnd && !IsEol(*p) ) if( !isspace(*p++) )	bNonZ = TRUE;
	while( p < pEnd ) if( *p++ == '\n') break;
	return bNonZ;
}

void CConfigFile::SSection::Read(char *&pData,char *pEnd,TCchar *name,int &line)
{
	char *p = pData;
	int l = line;
	for( m_nEntries = 0 ; p < pEnd && *p != '[' ; ) {
		if( !isspace(*p) ) {
			if( *p != '#' )	m_nEntries++;
			SkipLine(p,pEnd);
		} else
			RTCHECK2( !SkipLine(p,pEnd), "Line must begin at first position in '%s' line %d",name,l);
		l++;
	}
	m_pEntries = m_nEntries > 0 ? new SEntry[m_nEntries] : NULL;

	p = pData;
	for( int i = -1 ; p < pEnd && *p != '['; line++ ) {
		if( !isspace(*p) ) {
			if( *p == '#' )
				SkipLine(p,pEnd);
			else {
				i++;
				ASSERT( i < m_nEntries );
				m_pEntries[i].name = p;
				while( p < pEnd && !IsEol(*p) && *p != '=' ) p++;
				RTCHECK2( p < pEnd && ! IsEol(*p), "Missing '=' in '%s' line %d",name,line);
				*p++ = 0;
				m_pEntries[i].m_pData = p;
				while( p < pEnd && !IsEol(*p) ) p++;
				*p++ = 0;
				while( p < pEnd ) if( *p++ == '\n' ) break;
			}
		} else
			VERIFY( !SkipLine(p,pEnd) );
	}
	ASSERT( i == m_nEntries-1 );
	pData = p;
}

CConfigFile::CConfigFile(TCchar *name,bool bMustExist)
{
	long len;
	FILE *f = CFileResource::FOpenCurrent(name,&len);
	if(!f && !bMustExist) {
		m_pData = NULL;
		m_nSections = 0;
		m_pSections = NULL;
	}
	RTCHECK1(f,"File not found: %s",name);
	m_pData = new char[len+1];
	RTCHECK1(fread(m_pData,len,1,f)==1,"Error reading %s",name);
	fclose(f);

	char *p = m_pData, *pEnd = m_pData+len;

	for( ; p < pEnd ; p++ ) if( *p == '\x1A' ) { pEnd = p; break; }

	p = m_pData;
	for( m_nSections = 0 ; p < pEnd ; ) {
		if( *p == '[' ) m_nSections++;
		SkipLine(p,pEnd);
	}

	m_pSections = m_nSections > 0 ? new SSection[m_nSections] : NULL;
	p = m_pData;
	int line = 1;
	for( int i = -1 ; p < pEnd ; ) {
		if( *p == '[' ) {
			i++;
			ASSERT( i < m_nSections );
			m_pSections[i].name = ++p;
			while( p < pEnd && !IsEol(*p) && *p != ']' ) p++;
			RTCHECK2( p < pEnd && ! IsEol(*p), "Missing ']' in '%s' line %d",name,line);
			*p++ = 0;
			RTCHECK2( !SkipLine(p,pEnd), "Extra characters in '%s' line %d",name,line);
			line++;
			m_pSections[i].Read(p,pEnd,name,line);
		} else {
			ASSERT( i < 0 );
			if( p < pEnd && *p != '#' )
				RTCHECK2( !SkipLine(p,pEnd), "Section beginning expected in '%s' line %d",name,line);
			else
				SkipLine(p,pEnd);
			line++;
		}
	}
}

TCchar *CConfigFile::SSection::operator()(TCchar *entryName)
{
	for( int i = 0 ; i < m_nEntries ; i++ ) {
		if( strcmp(entryName,m_pEntries[i].name) == 0 )
			return m_pEntries[i].m_pData;
	}
	return NULL;
}

TCchar *CConfigFile::operator()(TCchar *sectionName,TCchar *entryName)
{
	for( int i = 0 ; i < m_nSections ; i++ ) {
		if( strcmp(sectionName,m_pSections[i].name) == 0 )
			return m_pSections[i](entryName);
	}
	return NULL;
}

int  CConfigFile::operator()(TCchar *sectionName,TCchar *entryName,TCchar *szFmt,...)
{
	va_list va;
	va_start(va,szFmt);
	TCchar *pData = (*this)(sectionName,entryName);
	int		nRes = -1;
	if( pData )
		nRes = vsscanf(pData,szFmt,va);
	va_end(va);
	return nRes;
}

int  CConfigFile::GetInt(TCchar *sectionName,TCchar *entryName,int nDefault)
{
	int n;
	return (*this)(sectionName,entryName,"%d",&n) > 0 ? n : nDefault;
}
double CConfigFile::GetDouble(TCchar *sectionName,TCchar *entryName,double fDefault)
{
	double f;
	return (*this)(sectionName,entryName,"%lg",&f) > 0 ? f : fDefault;
}

