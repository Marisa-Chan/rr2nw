#ifndef	__PATTERNS_CPP__
#include"_pattern.h"
#endif

CPaletteTranslator	paletteTranslator;

SRGB	CPaletteTranslator::m_palette[256];
byte	CPaletteTranslator::m_palette64[768];

byte	CPaletteTranslator::m_gouraud[16*256];
byte	*CPaletteTranslator::m_pTransparencies = NULL;
SHazeDef	CPaletteTranslator::m_haze[CPaletteTranslator::NUM_HAZES];
SGRColorDef	*CPaletteTranslator::m_pTranspDefs = NULL;
byte	*CPaletteTranslator::m_pLights = NULL;
int		CPaletteTranslator::m_nLightColors = 0;
int		CPaletteTranslator::m_nTransparentColors = 0;

#if 0
void	CPaletteTranslator::InvertHaze(byte *pHaze)
{
	for(int i = 0 ; i < 8 ; i++ ) {
		byte *p0 = pHaze+i*256, *p1 = pHaze+(15-i)*256;
		for( int j = 0 ; j < 256 ; j++, p0++, p1++ ) {
			byte t = *p0; *p0 = *p1; *p1 = t;
		}
	}
}
#endif

bool	CPaletteTranslator::LoadPalettePack(TCchar *name)
{
	CTaggedFile	f(TRUE);
	f.Open(name);
	CleanUp();
	f.Descend("PTP_",FALSE,2);
		f.Descend("PAL8",TRUE,0); {
			f.Read(m_palette,768);
			if( !f.IsOK() )	{ f.Close(FALSE); return FALSE; }
			for( int  i = 0 ; i < 256 ; i++ ) {
				m_palette64[i*3]   = (byte)Round(m_palette[i].r*63./255);
				m_palette64[i*3+1] = (byte)Round(m_palette[i].g*63./255);
				m_palette64[i*3+2] = (byte)Round(m_palette[i].b*63./255);
			}
		} f.Ascend();
		{
			f.Descend("GOUR",TRUE,1);
				f.Read(m_gouraud,16*256);
			f.Ascend();
		}
		{
			f.Descend("TRAN",TRUE,1);
				delete [] m_pTransparencies; m_pTransparencies = NULL;
				delete [] m_pTranspDefs; m_pTranspDefs = NULL;
				f.ReadInt(m_nTransparentColors);
				ASSERT( m_nTransparentColors > 0 );
				m_pTransparencies = new byte[m_nTransparentColors*16*256];
				m_pTranspDefs = new SGRColorDef[m_nTransparentColors];
				for( int i = 0 ; i < m_nTransparentColors ; i++ ) {
					SRGB	color;
					f.Read(&color,3);
					m_pTranspDefs[i].r = color.r;
					m_pTranspDefs[i].g = color.g;
					m_pTranspDefs[i].b = color.b;
				}
				f.Read(m_pTransparencies,m_nTransparentColors*16*256);
				for( int i = 0 ; i < m_nTransparentColors ; i++ )
					m_pTranspDefs[i].pTable = m_pTransparencies+i*16*256;
			f.Ascend();
		}
		for( int i = 0 ; i < NUM_HAZES ; i++ ) {
			delete [] m_haze[i].pTable;
			m_haze[i].pTable = NULL;
			f.Descend("HAZE",TRUE,0);
				int nHazeColors;
				f.Read(&m_haze[i].color,3);
				//m_haze[i].color._6to8();
				m_haze[i].r = m_haze[i].color.r;
				m_haze[i].g = m_haze[i].color.g;
				m_haze[i].b = m_haze[i].color.b;
				m_haze[i].colorIdx = Match(m_haze[i].color.r,m_haze[i].color.g,m_haze[i].color.b);
				f.ReadInt(nHazeColors);
				ASSERT(nHazeColors == 16);
				m_haze[i].pTable = new byte[256*16];
				f.Read(m_haze[i].pTable,256*16);
				//InvertHaze(m_pHaze);
			f.Ascend();
		}
		{
			delete [] m_pLights; m_pLights = NULL;
			f.Descend("LIG_",TRUE,0);
				f.ReadInt(m_nLightColors);
				ASSERT( m_nLightColors == LIGHT_COLOR_COUNT );
				m_pLights = new byte[256*32*m_nLightColors];
				f.Read(m_pLights,256*32*m_nLightColors);
			f.Ascend();
		}
	f.Ascend();
	return f.IsOK();
}

inline int PalTrans_Pow2(int a) { return a*a; }

inline int	CPaletteTranslator::Mismatch(TCSRGB &match, TCSRGB &current)
{
	return	PalTrans_Pow2(match.r-current.r)+
			PalTrans_Pow2(match.g-current.g)+
			PalTrans_Pow2(match.b-current.b);
}

byte	CPaletteTranslator::Match(int r,int g,int b)
{
	SRGB	rgb;
	rgb.r = (byte)r; rgb.g = (byte)g; rgb.b = (byte)b;
	int  nBest = 0;
	int  nBestVal = Mismatch(rgb,m_palette[0]);
	for( int  i = 1 ; i < 256 ; i++ ) {
		double	nMismatch = Mismatch(rgb,m_palette[i]);
		if( nMismatch < nBestVal ) { nBest = i; nBestVal = nMismatch; }
	}
	return	(byte)nBest;
}

byte	CPaletteTranslator::Hue(byte color,double fModify)
{
/*	static	double sqrt3 = sqrt(3), sqrt32 = sqrt3/2;
	SRGB	&c = m_palette[color];
	int	l = c.r+c.g+c.b;
	double x = (c.b-c.g)*sqrt32, y = c.r-(c.b+c.g)/2.;
	double r = hypot(x,y), a = atan2(y,x)+fModify;
	double x3 = r*cos(a)/sqrt3, ly = (l-r*sin(a))/3;
	return	Match(Round(l-2*ly),Round(ly-x3),Round(ly+x3));*/

	const int l0 = 255;
	static	double sqrt3 = sqrt(3), sqrt32 = sqrt3/2, sqrt3l0 = l0*sqrt3;
	SRGB	&c = m_palette[color];
	int	l = c.r+c.g+c.b, _l = 3*l0-l;
	int	 rs = c.r*2-(c.b+c.g), gs = l-3*c.g, bs = l-3*c.b;
	double x = (c.b-c.g)*sqrt32, y = rs/2.;
	double rad = hypot(x,y), a = atan2(y,x);
	double rmax = sqrt3l0;
	if( rs != 0 )
		rmax = Min(rmax,(rs > 0 ? _l/(double)rs : -l/(double)rs)*rad);
	if( gs != 0 )
		rmax = Min(rmax,(gs > 0 ? l/(double)gs : -_l/(double)gs)*rad);
	if( bs != 0 )
		rmax = Min(rmax,(bs > 0 ? l/(double)bs : -_l/(double)bs)*rad);
	rad /= rmax;
	a += fModify; rad *= 2;
	x = cos(a); y = sin(a);
	rmax = sqrt3l0;
	double x3 = x*sqrt3;
	double	k = y*2;
	if( fabs(k) > 1e-10 )
		rmax = Min(rmax,k > 0 ? _l/k : -l/k);
	k = y+x3;
	if( fabs(k) > 1e-10 )
		rmax = Min(rmax,k > 0 ? l/k : -_l/k);
	k = y-x3;
	if( fabs(k) > 1e-10 )
		rmax = Min(rmax,k > 0 ? l/k : -_l/k);
	rad *= rmax;
	double	x_3 = rad*x/sqrt3, ly = (l-rad*y)/3;
	return Match(Round(l-2*ly),Round(ly-x_3),Round(ly+x_3));

}

void	CPaletteTranslator::LoadIdentity()
{
	for( int  i = 0 ; i < MAX_ENTRIES ; i++ )
		m_entry[i] = (byte)i;
	m_nEntries = MAX_ENTRIES;
}

int	CPaletteTranslator::Define(TCSRGB *pEntries, int nEntries,
														int nTransparent)
{
	for( int i = 0 ; i < nEntries ; i++ ) {
		m_localPalette[i] = pEntries[i];
	}
	if( !m_bEnabled ) return nTransparent;
	ASSERT( (unsigned)nEntries <= MAX_ENTRIES );
	byte	bUsed[256];
	if( nTransparent >= 0 ) {
		for( int  i = 0 ; i < 256 ; i++ )	bUsed[i] = FALSE;
	}
	for( int i = 0 ; i < nEntries ; i++ ) {
		m_localPalette[i] = pEntries[i];
		if( i == nTransparent ) continue;
		int  nBest = nTransparent >= 0 ? 1 : 0; // to reserve 0 for transparent
		int  nBestVal = Mismatch(pEntries[i],m_palette[nBest]);
		for( int  j = nBest+1 ; j < 256 ; j++ ) {
			double	nMismatch = Mismatch(pEntries[i],m_palette[j]);
			if( nMismatch < nBestVal ) { nBest = j; nBestVal = nMismatch; }
		}
		m_entry[i] = (byte)nBest;
		if( nTransparent >= 0 )	bUsed[nBest] = TRUE;
	}
	m_nEntries = nEntries;
	if( nTransparent >= 0 ) {
		//for( i = 0 ; i < 256 ; i++ )
		//	if( !bUsed[i] )	break;
		//RTCHECK(i<256,"No unused color for transparent");
		int i = 0;
		m_entry[nTransparent]=(byte)i;
		return i;
	}
	return -1;
}

int	CPaletteTranslator::DefineFile64(TCchar *name,int nTransparent)
{
	FILE *f = CFileResource::FOpenCurrent(name);
	RTCHECK1(f,"File not open: '%s'",name);
	byte pal[768];
	RTCHECK1(fread(pal,768,1,f)==1,"Error reading '%s'",name);
	fclose(f);
	for(int i = 0 ; i < 768 ; i++ )
		pal[i] = (byte)Round(pal[i]*255./63);
	return Define((TCSRGB*)pal,256,nTransparent);
}

int	CPaletteTranslator::Define(CTaggedFile &f,int nTransparent)
{
	SRGB	entries[MAX_ENTRIES];
	//SRGB	transpEntries[MAX_TRANSPENTRIES];
	int	nEntries,nTranspEntries;
	f.Descend("PAL_",TRUE,1);
		f.ReadInt(nEntries); f.ReadInt(nTranspEntries);
		RTCHECK(nEntries>=0&&nEntries<=MAX_ENTRIES,"Bad num pal entries");
		RTCHECK(nTranspEntries>=0&&nTranspEntries<=MAX_TRANSPENTRIES,"Bad num pal transp entries");
		f.Read(entries,nEntries*sizeof(entries[0]));
		//f.Read(transpEntries,nTranspEntries*sizeof(transpEntries[0]));
	f.Ascend();
	return Define(entries,nEntries,nTransparent);
}
