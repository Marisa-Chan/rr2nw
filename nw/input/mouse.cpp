#ifndef	__INPUT_CPP__
#include"_input.h"
#endif


#ifndef	MSDOS
#define	intr(X,Y) ((void)0)
#endif

bool	CMouse::m_bPresent = FALSE, CMouse::m_bEnabled = TRUE,
					CMouse::m_bActive = FALSE;
bool	CMouse::m_bFlipX = FALSE, CMouse::m_bFlipY = FALSE;
CRect2	CMouse::m_range, CMouse::m_screenRange(0,0,320,200);
byte	CMouse::m_nCursorColor[32];

#ifndef	_BIG_ENDIAN
void	CMouse::WriteParms(CTaggedFile &f)
{
	f.Descend("MOU_",TRUE,0);
		f.WriteInt(m_bEnabled);
		f.WriteInt(m_bFlipX);
		f.WriteInt(m_bFlipY);
	f.Ascend();
}
#endif

void	CMouse::ReadParms(CTaggedFile &f)
{
	f.Descend("MOU_",TRUE,0);
		f.ReadBool(m_bEnabled);	m_bActive = m_bPresent && m_bEnabled;
		f.ReadBool(m_bFlipX);
		f.ReadBool(m_bFlipY);
	f.Ascend();
}


bool	CMouse::Detect()
{
	#ifndef	MSDOS
		m_bPresent = m_bActive = FALSE;
		return FALSE;
	#else
	//ASSERT(*(byte*)0x449 == 0x13);
	REGPACK	r;
	r.w.ax = 0;
	r.w.ds = r.w.es = r.w.fs = r.w.gs = 0;
	intr(0x33,&r);
	m_bPresent = r.w.ax == 0xFFFF;
	if( m_bPresent ) {
		m_range = CRect2(0,0,640,200);
		r.w.ax = 4;
		r.w.cx = 320;
		r.w.dx = 100;
		r.w.ds = r.w.es = r.w.fs = r.w.gs = 0;
		intr(0x33,&r);
	}
	return	m_bPresent;
	#endif
}

void	CMouse::SetRange(TCCRect2 &range)
{
	m_range = range;
	if(!m_bActive)	return;

	#ifndef	MSDOS
		return;
	#else
	REGPACK	r;
	r.w.ax = 7;
	r.w.cx = word(range.left);
	r.w.dx = word(range.right-1);
	r.w.ds = r.w.es = r.w.fs = r.w.gs = 0;
	intr(0x33,&r);

	r.w.ax = 8;
	r.w.cx = word(range.top);
	r.w.dx = word(range.bottom-1);
	r.w.ds = r.w.es = r.w.fs = r.w.gs = 0;
	intr(0x33,&r);
	#endif

	//CenterMouse();
}

void	CMouse::CenterMouse()
{
	#ifdef	MSDOS
	if(!m_bActive)	return;
	REGPACK	r;
	r.w.ax = 4;
	r.w.cx = (word)m_range.CenterX();
	r.w.dx = (word)m_range.CenterY();
	r.w.ds = r.w.es = r.w.fs = r.w.gs = 0;
	intr(0x33,&r);
	#endif
}

void	CMouse::GetStatus(CMouse::SStatus &s)
{
	if( !m_bActive ) {
		s.x = s.y = 0;
		s.b1 = s.b2 = FALSE;
		return;
	}
	#ifdef	MSDOS
	REGPACK	r;
	r.w.ax = 3;
	r.w.ds = r.w.es = r.w.fs = r.w.gs = 0;
	intr(0x33,&r);
	s.x = (r.w.cx-m_range.CenterX())*2./m_range.Width();
	s.y = (r.w.dx-m_range.CenterY())*2./m_range.Height();
	s.b1 = MAKEBOOL(r.w.bx & 1);
	s.b2 = MAKEBOOL(r.w.bx & 2);
	if( m_bFlipX )	s.x = -s.x;
	if( m_bFlipY )	s.y = -s.y;
	#endif
}

void	CMouse::GetOffset(int &x,int &y)
{
	if(!m_bActive)	{ x = y = 0; return; }
	#ifdef MSDOS
	REGPACK	r;
	r.w.ax = 0xB;
	r.w.ds = r.w.es = r.w.fs = r.w.gs = 0;
	intr(0x33,&r);
	x = (short int)r.w.cx;
	y = (short int)r.w.dx;
	if( m_bFlipX )	x = -x;
	if( m_bFlipY )	y = -y;
	#endif
}

void	CMouse::ResetEvents()
{
	if(!m_bActive)	return;
	#ifdef MSDOS
	REGPACK	r;
	r.w.ax = 0xB;
	r.w.ds = r.w.es = r.w.fs = r.w.gs = 0;
	intr(0x33,&r);
	#endif
}

void	CMouse::Enable(bool b)
{
	m_bActive = m_bPresent && bool(m_bEnabled = b);
	ResetEvents();
}

void	CMouse::MoveTo(double x,double y)
{
	int mx = Round(x*0.5*m_range.Width()+m_range.CenterX());
	int my = Round(y*0.5*m_range.Height()+m_range.CenterY());
	mx = Max(m_range.left,Min(m_range.right-1,(long)mx));
	my = Max(m_range.top,Min(m_range.bottom-1,(long)my));
	#ifdef MSDOS
	REGPACK r;
	r.w.ax = 4;
	r.w.cx = (word)mx;
	r.w.dx = (word)my;
	r.w.ds = r.w.es = r.w.fs = r.w.gs = 0;
	intr(0x33,&r);
	#endif
}

void	CMouse::MatchCursor()
{
	const int nMax = 32, nMid = 16;
	for( int i = 0 ; i < nMid ; i++ ) {
		int nComp = Round(i*255./nMid);
		m_nCursorColor[i] = paletteTranslator.Match(nComp,nComp,nComp);
	}
	for( int i = nMid ; i < nMax ; i++ ) {
		int nComp = Round((nMax-i)*255./(nMax-nMid));
		m_nCursorColor[i] = paletteTranslator.Match(nComp,nComp,nComp);
	}
}

void	CMouse::DrawCursor(byte *pScreen,int nScrMaxX,int nScrMaxY)
{
	if(!m_bActive)	return;
	#ifndef MSDOS
		(void)pScreen; (void)nScrMaxX; (void)nScrMaxY;
	#else
	REGPACK	r;
	r.w.ax = 3;
	r.w.ds = r.w.es = r.w.fs = r.w.gs = 0;
	intr(0x33,&r);

	//int  x = (short)r.w.cx/2, y = (short)r.w.dx;
	int  x = ((short)r.w.cx*m_screenRange.Width()+m_range.Width()/2)
															/m_range.Width();
	int  y = ((short)r.w.dx*m_screenRange.Height()+m_range.Height()/2)
															/m_range.Height();

	const int dx = 6, dy = 5;
	int x1, x2, y1, y2;
	x1 = Max(0,Min(nScrMaxX,x-dx));
	x2 = Max(0,Min(nScrMaxX,x+dx+1));
	y1 = Max(0,Min(nScrMaxY,y-dy));
	y2 = Max(0,Min(nScrMaxY,y+dy+1));
	byte nColor = m_nCursorColor[*(byte*)0x46C&0x1F];
	if( x >= m_screenRange.left && x < m_screenRange.right )
		for( int y = y1 ; y < y2 ; y++ )
			pScreen[y*_gr_nScreenWidth+x] = nColor;
	if( y >= m_screenRange.top && y < m_screenRange.bottom )
		for( int x = x1 ; x < x2 ; x++ )
			pScreen[y*_gr_nScreenWidth+x] = nColor;
	#endif
}

void 	CMouse::GetDirectStatus(CMouse::SDirectStatus &s)
{
	if( !m_bActive ) {
		s.x = s.y = 0;
		s.b1 = s.b2 = FALSE;
		return;
	}
	#ifdef MSDOS
	REGPACK	r;
	r.w.ax = 3;
	r.w.ds = r.w.es = r.w.fs = r.w.gs = 0;
	intr(0x33,&r);
	s.x = r.w.cx;
	s.y = r.w.dx;
	s.b1 = MAKEBOOL(r.w.bx & 1);
	s.b2 = MAKEBOOL(r.w.bx & 2);
	#endif
}

void	CMouse::DirectMoveTo(int x,int y)
{
	#ifndef	MSDOS
		(void)x; (void)y;
	#else
	REGPACK r;
	r.w.ax = 4;
	r.w.cx = (word)x;
	r.w.dx = (word)y;
	r.w.ds = r.w.es = r.w.fs = r.w.gs = 0;
	intr(0x33,&r);
	#endif
}
