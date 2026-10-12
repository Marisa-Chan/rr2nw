#ifndef	__PATTERNS_H__
#define	__PATTERNS_H__

struct	SRGB {
	byte	r, g, b;
	//void _6to8() { r = (byte)Round(r*255./63.); g = (byte)Round(g*255./63.); b = (byte)Round(b*255./63.); }
};
typedef	const SRGB TCSRGB;

struct	SHazeDef : public SGRColorDef {
	int		nHazeMin, nHazeDist;
	SRGB	color;
	int		colorIdx;
	int		nColor;
	SHazeDef() { pTable = NULL; }
};

class	CPaletteTranslator	{
public:
	enum { MAX_ENTRIES = 4096, MAX_TRANSPENTRIES = 0 };
	enum { NUM_HAZES = 2 };
private:
	int		m_nEntries;
	byte	m_entry[MAX_ENTRIES];
	bool	m_bEnabled;
	SRGB	m_localPalette[MAX_ENTRIES];
	static	SRGB	m_palette[256];
	static	byte	m_palette64[768];
	static	int		m_nTransparentColors;
	static	byte	m_gouraud[16*256], *m_pTransparencies, *m_pLights;
	static	SHazeDef	m_haze[NUM_HAZES];
	static	SGRColorDef	*m_pTranspDefs;
	static	int		m_nLightColors;
	static	int		Mismatch(TCSRGB &match,TCSRGB &current);
	//static	void	InvertHaze(byte *pHaze);
public:
	CPaletteTranslator() { m_bEnabled = TRUE; m_nEntries = 0; }
	void	EnableMatch(bool bEnable) { m_bEnabled = bEnable;
								if(!bEnable) LoadIdentity(); }
	bool	IsMatchEnabled() const { return m_bEnabled; }
	void	LoadIdentity();
	int		Define(TCSRGB *pEntries, int nEntries, int nTransparent = -1);
	int		DefineFile64(TCchar *name, int nTransparent = -1);
	int		Define(CTaggedFile &f, int nTransparent = -1);
	byte	Match(int b) const { ASSERT(b<m_nEntries); return m_entry[b]; }
	//byte	MatchTransparent(int b) const { ASSERT(b<m_nEntries); return ; }
	byte	operator()(int b) const { return Match(b); }
	SRGB	*LocalPalette() { return m_localPalette; }
	int		NumEntries() { return m_nEntries; }
	static	byte	Match(int r,int g,int b);
	static	byte	Hue(byte color,double fModify);
	static	SRGB	*Palette() { return m_palette; }
	//static	void Load64Palette(TCbyte *pPal64);
	static	bool LoadPalettePack(TCchar *name);
	static	TCbyte *Gouraud() { return m_gouraud; }
	static	void	SetGRTables() {
		GRSetPaletteTables(m_pTranspDefs,m_nTransparentColors,m_gouraud);
		SetMixLightTable(m_pLights);
	}
	//static	TCbyte	*Transparency() { return m_pTransparency; }
	static	SHazeDef	&Haze(int i) { return m_haze[i]; }
	static	TCbyte	*Palette64() { return m_palette64; }

	static	void CleanUp() {
					delete [] m_pTransparencies; m_pTransparencies = NULL;
					delete [] m_pTranspDefs; m_pTranspDefs = NULL;
					for( int i = 0 ; i < NUM_HAZES ; i++ ) { delete [] m_haze[i].pTable; m_haze[i].pTable = NULL;
					delete [] m_pLights; m_pLights = NULL; }
		}
	static void ComputeHazeColors() {
		for( int i = 0 ; i < NUM_HAZES ; i++ )
			m_haze[i].nColor = GRFillColor(m_haze[i].color.r,m_haze[i].color.g,m_haze[i].color.b);
	}
};

extern	CPaletteTranslator	paletteTranslator;

// -------------------------------------------------------------------------
struct	PTNS2AlignedImageHeader {
	int		*pCache;	// Only for internal use. May be subject to change in future
	dword	flags;
	int		height;
	int		width;
};
byte *PTNCreate2AlignedImage(int w,int h,int bytesPixel,dword flags);
byte *PTNUnload2AlignedImage(byte *p);
byte *PTNLoad2AlignedImage(FILE *f,int w,int h,int bytesPixel,CPaletteTranslator *pTrans=NULL,dword flags=0);
byte *PTNLoad2AlignedImage(CTaggedFile &f,int w,int h,int bytesPixel,CPaletteTranslator *pTrans=NULL,dword flags=0);
inline PTNS2AlignedImageHeader *PTNGet2AlignedImageHeader(byte *p) { return (PTNS2AlignedImageHeader*)p-1; }

class	CSprite	{
protected:
	int	m_nWidth, m_nHeight;
	byte m_nTransparent, *m_pImage;
public:
	CSprite(TCchar *name);
	~CSprite() { PTNUnload2AlignedImage(m_pImage); }
	int Width() const { return m_nWidth; }
	int Height() const { return m_nHeight; }
	byte Transparent() const { return m_nTransparent; }
	byte *Image() const { return m_pImage; }
	byte operator()(int x,int y) { return m_pImage[x+y*m_nWidth]; }
	void DrawAt(int x,int y) const;
};
// -------------------------------------------------------------------------
class	CPhasedMovie	{
protected:
	struct	SPhase	{
		double	fTime;
		int		nWidth,nHeight;
		int		nTransp;
		byte	*pImage;
		//int		*pCache;
		SPhase() { pImage = NULL; }
		~SPhase() { PTNUnload2AlignedImage(pImage); }
		void Load(CTaggedFile &f);
	};
	SPhase	*m_pPhases;
	int		m_nPhases;
	//static CFMatrix3x4 m_drawInv;
//	static void TexturedCallBack(SGRTexturedCallBackData &tcbd);
public:
	CPhasedMovie() { m_pPhases = 0; }
	~CPhasedMovie() { delete [] m_pPhases; }
	void Load(CTaggedFile &f);
	void Load(TCchar *spmname);
	void Draw(double fTime,TCCFVector3 &pt,double fZoom);
	int  Count() const { return m_nPhases; }
	double  &Time(int i) { return m_pPhases[i].fTime; }
	double  TimeTotal() const { return m_pPhases[m_nPhases-1].fTime; }
	void CleanUp() { delete [] m_pPhases; m_pPhases = NULL; m_nPhases = 0; }
};

// -------------------------------------------------------------------------

#if 0 //_USE_OWNGR || _USE_GLIDE
class CFixedFont {
	private:
		long nFntWidth;
		long nFntHeight;
		long pSmFntTable[256*2]; //sx,offset
		unsigned char *pFntSpr;
		unsigned char pPal[48];
		long xs, ys;
	public:
		CFixedFont(const char *filename);
		~CFixedFont() {delete pFntSpr;}
		void PrintAt(long x, long y,TCchar *str)
			{ if (pFntSpr) GRPrintString(str, x, y, nFntWidth, nFntHeight, pSmFntTable, pFntSpr);}
		void Print(TCchar * str)
			{ if (pFntSpr) GRPrintString(str, xs, ys, nFntWidth, nFntHeight, pSmFntTable, pFntSpr);}
		//void PrintAtC(long x, long y,TCchar *str)
		//	{ if (pFntSpr) GRCPrintString(str, x, y, nFntWidth, nFntHeight, pSmFntTable, pFntSpr);}
		void PrintAtC(long x, long y,TCchar *str,unsigned char color = 15)
			{ if (pFntSpr) GRCPrintStringColor(str, x, y, nFntWidth, nFntHeight, pSmFntTable, pFntSpr,color);}
		void PrintC(TCchar * str)
			{ if (pFntSpr) GRCPrintString(str, xs, ys, nFntWidth, nFntHeight, pSmFntTable, pFntSpr);}
		void SetFntXY(long x, long y) {xs = x; ys = y;}
		long StringWidth(TCchar * string)
		{  for(long i=0,l=0;string[i] != 0;l += pSmFntTable[(int)string[i++]<<1]) ;
		   return l;
		}
};
#endif


// -------------------------------------------------------------------------
#if 0 //_USE_OWNGR
class	CPnlData	{
protected:
	byte	*m_pData;
public:
	CPnlData() { m_pData = NULL; }
	~CPnlData() { delete [] m_pData; }
	bool Read(TCchar *szFileName);
	void Draw(int x,int y) { GRDrawPanel(m_pData,x,y,
									CPaletteTranslator::Transparency()); }
};
#endif

#endif	// __PATTERNS_H__


