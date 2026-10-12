
#ifndef	__LANG_CPP__
	#include<stdio.h>
	#include<ctype.h>
	#include<string.h>
	#include<math.h>
	#include<stdlib.h>
	#define	_NOAUTOINCLUDE
	#include"stdtypes.h"
	#include"debugext.h"
	#include"mathlib.h"
	#include"lang.h"
#endif

#ifndef	M_PI
#define M_PI        3.14159265358979323846
#endif

// -------------------------------------------------------------------------
void CTextPos::operator+=(int c)
{
	if(m_row>0 && c=='\n')
		m_row++, m_col=1; else
	if(c=='\t')
		m_col = ((m_col+3)&0xFFFC)+1;
		//m_col = ((m_col+7)&0xFFF8)+1;
	else
	if(c)
		m_col++;
}
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
CCharStream::CCharStream(int (*PrintF)(TCchar *format,...))
{
	m_bUnget = m_bErr = FALSE;
	m_PrintF = PrintF;
}
// -------------------------------------------------------------------------
CCharStream::~CCharStream()
{
}
// -------------------------------------------------------------------------
bool  CCharStream::Open(const CTextPos &pos0)
{
	m_bUnget = FALSE;
	m_pos = pos0;
	m_c = 0;
	m_bErr = FALSE;
	return	TRUE;
}
// -------------------------------------------------------------------------
int  CCharStream::Get()
{
	if( m_bUnget )
		m_bUnget = FALSE;
	else {
		if( m_c == EOF )	return EOF;
		m_pos += m_c;
		m_c = GetNext();
		if( !m_c )	m_c = EOF;
	}
	return	m_c;
}
// -------------------------------------------------------------------------
bool  CCharStream::Unget()
{
	ASSERT( ! m_bUnget );
	m_bUnget = TRUE;
	return TRUE;
}
// -------------------------------------------------------------------------
void  CCharStream::Error(TCchar *msg, const CTextPos *pos)
{
	if( m_bErr )	return;
	m_bErr = TRUE;
	if(!pos) pos = &m_pos;
	if(!msg) msg = "Unknown error";
	if( pos->Row() > 0 )
		m_PrintF("Error in line %d col %d: %s\n",
										pos->Row(),pos->Col(),msg);
	else
		m_PrintF("Error in col %d: %s\n",pos->Col(), msg);
}
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
bool  CFileCharStream::Open(FILE *f,const CTextPos &pos0)
{
	ASSERT(!m_f);
	m_f = f;
	return	CCharStream::Open(pos0);
}
// -------------------------------------------------------------------------
bool  CFileCharStream::Close()
{
	bool	bRes = 0;
	if(m_f)	bRes = fclose(m_f);
	m_f = NULL;
	if(bRes)	return FALSE;
	return	CCharStream::Close();
}
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
CLangLex &CLangLex::operator=(const CLangLex &lx)
{
	switch( m_type = lx.m_type ) {
		case T_INTV:	m_v.i = lx.m_v.i; break;
		case T_FLOATV:	m_v.f = lx.m_v.f; break;
		case T_STRINGV:
		case T_LABEL:
		case T_ID:	strcpy(m_v.s,lx.m_v.s); break;
	}
	return *this;
}
// -------------------------------------------------------------------------
bool CLangLex::operator==(TCchar *lpsz) const
{
	if( m_type == T_ID )
		return	strcmp(m_v.s,lpsz) == 0;
	return FALSE;
}
// -------------------------------------------------------------------------
int  CLangLex::IsListedID(TCchar **lpList,int n) const
{
	if( *this != T_ID ) return	-1;
	for( int  i = 0 ; i < n ; i++ )
		if( *this == lpList[i] ) break;
	return	i >= n ? -1 : i;
}
// -------------------------------------------------------------------------
TCchar *CLangLex::Buffer() const
{
	ASSERT( *this == T_STRINGV || *this == T_ID || *this == T_LABEL );
	return	m_v.s;
}
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
CLangLexStream::CLangLexStream()
{
	m_pcs = NULL;
}
// -------------------------------------------------------------------------
CLangLexStream::~CLangLexStream()
{
	Close();
}
// -------------------------------------------------------------------------
bool  CLangLexStream::Open(CCharStream *pcs)
{
	ASSERT(!m_pcs);
	m_bUnget = FALSE;
	m_pcs = pcs;
	m_pos = m_pcs->Pos();
	return TRUE;
}
// -------------------------------------------------------------------------
bool  CLangLexStream::Close()
{
	m_pcs = NULL;
	return TRUE;
}
// -------------------------------------------------------------------------
CCharStream  *CLangLexStream::Suspend()
{
	ASSERT(m_pcs && !m_bUnget);
	CCharStream	*pcs = m_pcs;
	m_pcs = NULL;
	return pcs;
}
// -------------------------------------------------------------------------
const CLangLex &CLangLexStream::Get()
{
	const int nLexStr = 34;
	static	const char *szLexStr[nLexStr] = {
		"MOD", "NOT", "AND", "OR",   // 4
		"VAR", "FLOAT", "INT", "VECTOR", "VOID", "FUNC", "RETURN", // +7 (11)
		"IF", "THEN", "ELSE", "LOOP", "BREAK", "GOTO", "PRINT",	// +7 (18)
		"RUN", "FOR", "TO", "DOWNTO", "PI", "ABS", "SQRT", // +7 (25)
		"SIN", "COS", "TAN", "ASIN", "ACOS", "ATAN", "ATAN2",	// +7 (32)
		"LOG", "EXP"	// +2 (34)
	};
	static CLangLex::TYPE eLexType [nLexStr] = {
		CLangLex::T_MOD, CLangLex::T_NOT, CLangLex::T_AND, CLangLex::T_OR,
		CLangLex::T_VAR, CLangLex::T_FLOAT, CLangLex::T_INT,
					CLangLex::T_VECTOR, CLangLex::T_VOID,
								CLangLex::T_FUNC, CLangLex::T_RETURN,
		CLangLex::T_IF, CLangLex::T_THEN, CLangLex::T_ELSE, CLangLex::T_LOOP,
					CLangLex::T_BREAK, CLangLex::T_GOTO, CLangLex::T_PRINT,
		CLangLex::T_RUN, CLangLex::T_FOR, CLangLex::T_TO, CLangLex::T_DOWNTO,
		CLangLex::T_PI, CLangLex::T_ABS, CLangLex::T_SQRT,
					CLangLex::T_SIN, CLangLex::T_COS, CLangLex::T_TAN,
					CLangLex::T_ASIN, CLangLex::T_ACOS, CLangLex::T_ATAN,
						CLangLex::T_ATAN2, CLangLex::T_LOG, CLangLex::T_EXP
	};

	if( m_bUnget )	{
		return	m_bUnget = FALSE, m_lx;
	}

TryAgain:
	while(isspace(m_pcs->Get()));
	m_pos = m_pcs->Pos();
	switch(m_pcs->This()) {
		case EOF:	return	m_lx.m_type = CLangLex::T_EOF, m_lx;
		case ';':	return	m_lx.m_type = CLangLex::T_SEMICOLON, m_lx;
		case ',':	return	m_lx.m_type = CLangLex::T_COMMA, m_lx;
		case '{':	return	m_lx.m_type = CLangLex::T_LBRACE, m_lx;
		case '}':	return	m_lx.m_type = CLangLex::T_RBRACE, m_lx;
		case '[':	return	m_lx.m_type = CLangLex::T_LBRAK, m_lx;
		case ']':	return	m_lx.m_type = CLangLex::T_RBRAK, m_lx;
		case '(':	return	m_lx.m_type = CLangLex::T_LPAR, m_lx;
		case ')':	return	m_lx.m_type = CLangLex::T_RPAR, m_lx;
		case '=':	return	m_lx.m_type = CLangLex::T_EQ, m_lx;
		case '+':	return	m_lx.m_type = CLangLex::T_PLUS, m_lx;
		case '-':	return	m_lx.m_type = CLangLex::T_MINUS, m_lx;
		case '%':	return	m_lx.m_type = CLangLex::T_PERC, m_lx;
		case '.':	return	m_lx.m_type = CLangLex::T_PERIOD, m_lx;
		case '?':	return	m_lx.m_type = CLangLex::T_QST, m_lx;
		case '~':	return	m_lx.m_type = CLangLex::T_TILDE, m_lx;
		case ':':
			if( m_pcs->Get() == '=' )	m_lx.m_type = CLangLex::T_ASSIGN;
			else m_pcs->Unget(), m_lx.m_type = CLangLex::T_COLON; return m_lx;
		case '*':
			if( m_pcs->Get() == '*' )	m_lx.m_type = CLangLex::T_DBLSTAR;
			else m_pcs->Unget(), m_lx.m_type = CLangLex::T_STAR; return m_lx;
		case '>':
			if( m_pcs->Get() == '=' )	m_lx.m_type = CLangLex::T_GE;
			else m_pcs->Unget(), m_lx.m_type = CLangLex::T_GT; return m_lx;
		case '<':
			if( m_pcs->Get() == '>' )	m_lx.m_type = CLangLex::T_NEQ; else
			if( m_pcs->This() == '=' )	m_lx.m_type = CLangLex::T_LE;
			else m_pcs->Unget(), m_lx.m_type = CLangLex::T_LT; return m_lx;
		case '/':
			if( m_pcs->Get() == '/' ) {
				while( m_pcs->Get() != EOF && m_pcs->This() != '\n' );
				//m_pos = m_pcs->Pos();
				goto TryAgain;
			} else
			if( m_pcs->This() == '*' ) {
				int  nDepth = 1;
				while( m_pcs->Get() != EOF ) {
					if( m_pcs->This() == '*' ) {
						if( m_pcs->Get() == '/' )	{
							if( --nDepth == 0 ) {
								/*m_pos=m_pcs->Pos();*/ goto TryAgain; }
						} else m_pcs->Unget();
					} else
					if( m_pcs->This() == '/' ) {
						if( m_pcs->Get() == '*' )	nDepth++; else
						if( m_pcs->This() == '/' ) {
							while( m_pcs->Get() != EOF && m_pcs->This() != '\n' );
						} else m_pcs->Unget();
					}
				}
				break;
			} else
				return m_pcs->Unget(), m_lx.m_type = CLangLex::T_SLASH, m_lx;
		// case '/'
		case '"': {
			for( int i = 0 ; i < 256 ; i++ ) {
				int c=m_pcs->Get();
				if(isspace(c) && c!=32) {
					char	s[50];
					sprintf(s,"Bad symbol '%c' (%02Xh) inside string",c,c);
					return m_pcs->Error(s),
								m_lx.m_type = CLangLex::T_BAD, m_lx;
				}
				if(c=='"') break;
				m_lx.m_v.s[i] = c;
			}
			if( i >= 256 )	return m_pcs->Error("String too long"),
										m_lx.m_type = CLangLex::T_BAD, m_lx;
			m_lx.m_v.s[i] = 0;
			m_lx.m_type = CLangLex::T_STRINGV;
			return m_lx;
		} // case '"'
	} // switch
	if( isdigit(m_pcs->This()) || m_pcs->This() == '.' ) {
		char	buf[60]; bool bInt = TRUE;
		m_pcs->Unget();
		int  i = 0, c;
		while( i < 50 ) {	// Processing integer part
			c=m_pcs->Get();
			if(!isdigit(c)) { m_pcs->Unget(); break; }
			buf[i++] = c;
		}
		if( c == '.' )	{	// Processing fractional part
			buf[i++] = m_pcs->Get(); bInt = FALSE;
			while( i < 50 ) {
				c=m_pcs->Get();
				if(!isdigit(c)) { m_pcs->Unget(); break; }
				buf[i++] = c;
			}
		}
		if( c == 'e' || c == 'E' ) {	// Processing exp
			buf[i++] = m_pcs->Get(); bInt = FALSE;
			if( m_pcs->Get() == '+' || m_pcs->This() == '-' )
							buf[i++] = m_pcs->This(); else m_pcs->Unget();
			while( i < 50 ) {		// Processing exp digits
				c=m_pcs->Get();
				if(!isdigit(c)) { m_pcs->Unget(); break; }
				buf[i++] = c;
			}
		}
		if( i >= 50 )	return	m_pcs->Error("Number too long"),
										m_lx.m_type = CLangLex::T_BAD, m_lx;
		buf[i] = 0;
		if( bInt ) {
			if( atof(buf) > 2147483647. )
					return	m_pcs->Error("Integer too big"),
										m_lx.m_type = CLangLex::T_BAD, m_lx;
			m_lx.m_v.i = atol(buf);
			return m_lx.m_type = CLangLex::T_INTV, m_lx;
		}
		else {
			m_lx.m_v.f = atof(buf);
			return m_lx.m_type = CLangLex::T_FLOATV, m_lx;
		}
	} // isdigit
	else
	if( IsAlpha(m_pcs->This()) || m_pcs->This() == '@' ) {
		bool bLabel = m_pcs->This() == '@';
		if( !bLabel )	m_pcs->Unget();
		for( int i = 0 ; i < 256 ; i++ ) {
			int c=m_pcs->Get();
			if(!IsAlNum(c)) { m_pcs->Unget(); break; }
			m_lx.m_v.s[i] = c;
		}
		if( i == 0 )	return m_pcs->Error("Symbol too short"),
										m_lx.m_type = CLangLex::T_BAD, m_lx;
		if( i >= 256 )	return	m_pcs->Error("Symbol too long"),
										m_lx.m_type = CLangLex::T_BAD, m_lx;
		m_lx.m_v.s[i] = 0;
		if( bLabel )	return m_lx.m_type = CLangLex::T_LABEL, m_lx;
		m_lx.m_type = CLangLex::T_ID;
		if( (i = m_lx.IsListedID(szLexStr,nLexStr)) >= 0 )
							return m_lx.m_type = eLexType[i], m_lx;
		m_lx.m_type = CLangLex::T_ID;
		return m_lx;
	} else
	return	Error("Bad symbol"), m_lx.m_type = CLangLex::T_BAD, m_lx;
}
// -------------------------------------------------------------------------
bool  CLangLexStream::Unget()
{
	ASSERT( ! m_bUnget );
	m_bUnget = TRUE;
	return TRUE;
}
// -------------------------------------------------------------------------
