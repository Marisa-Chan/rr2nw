#ifndef	__PATTERNS_CPP__
#include"_pattern.h"
#endif

#if 0 //_USE_OWNGR
bool CPnlData::Read(TCchar	*szFileName)
{
	long len;
	FILE *f = CFileResource::FOpenCurrent(szFileName,&len);
	RTCHECK(f,szFileName);
	m_pData = new byte[len];
	fread(m_pData,len,1,f);
	fclose(f);
	//Match(m_pData,len);
	if( !paletteTranslator.IsMatchEnabled() ) return TRUE;
	byte *p = m_pData;
	for(int i = 0 ; i < len ; ) {
		i++;
		if(!*p++) p+=2, i+=2;
		else {
			int l = *(word*)p; p+=2; i+=2;
			i+=l;
			while(l--) {
				*p = paletteTranslator(*p);
				p++;
			}
		}
	}
	return TRUE;
}
#endif
// ------------------------------------------------------------------------
byte *PTNCreate2AlignedImage(int w,int h,int bytesPixel,dword flags)
{
	int  w1 = w, h1 = h;
	#if _USE_GLIDE
		RTCHECK( w <= 256 && h <= 256, "3DFX Image dims too large" );
	#elif _USE_OPENGL
		RTCHECK( w <= 4096 && h <= 4096, "GL Texture dims too large" );
	#endif
	#if _USE_GLIDE || _USE_OPENGL
		w1 = 1; while( w1 < w ) w1 <<= 1;
		h1 = 1; while( h1 < h ) h1 <<= 1;
	#endif
	int  nDims = w1*h1;
	byte *p = new byte[nDims*bytesPixel+sizeof(PTNS2AlignedImageHeader)];
	if(p) {
		PTNS2AlignedImageHeader	*ph = (PTNS2AlignedImageHeader*)p;
		p += sizeof(PTNS2AlignedImageHeader);
		ph->width = w1;
		ph->height = h1;
		ph->flags = flags;
		ph->pCache = NULL;

		#if _USE_OWNGR
			ph->pCache = new int[h1*2+1];
			if( !ph->pCache )	return PTNUnload2AlignedImage(p);
			ph->pCache += h1;
			for( int i = -h1 ; i <= h1 ; i++ )	ph->pCache[i] = i*w1;
		#endif
	}
	return p;
}

byte *PTNUnload2AlignedImage(byte *p) {
	if(p) {
		PTNS2AlignedImageHeader	*ph = PTNGet2AlignedImageHeader(p);
		if( ph->pCache )	delete [] (ph->pCache-ph->height);
		delete [] (byte*)ph;
	}
	return NULL;
}

byte *PTNLoad2AlignedImage(FILE *f,int w,int h,int bytesPixel,CPaletteTranslator *pTrans,dword flags)
{
	ASSERT(bytesPixel>0 && bytesPixel<=4);
	RTCHECK(pTrans&&pTrans->IsMatchEnabled()||bytesPixel==1,"match off or no translator specified at loading image >1 byte/pixel");
	if(pTrans&&!pTrans->IsMatchEnabled()) pTrans = NULL;
	byte *pImage = PTNCreate2AlignedImage(w,h,bytesPixel,flags);
	if(!pImage) return NULL;
	byte *p = pImage;
	int w1 = PTNGet2AlignedImageHeader(pImage)->width;
	for( int i = 0 ; i < h ; i++, p += w1-w )
	for( int j = 0 ; j < w ; j++, p++ ) {
		dword pixel = 0;
		if( fread(&pixel,bytesPixel,1,f)!=1 ) return PTNUnload2AlignedImage(pImage);
		*p = pTrans ? (*pTrans)(pixel) : (byte)pixel;
	}
	return pImage;
}
byte *PTNLoad2AlignedImage(CTaggedFile &f,int w,int h,int bytesPixel,CPaletteTranslator *pTrans,dword flags)
{
	ASSERT(bytesPixel>0 && bytesPixel<=4);
	RTCHECK(pTrans&&pTrans->IsMatchEnabled()||bytesPixel==1,"match off or no translator specified at loading image >1 byte/pixel");
	if(pTrans&&!pTrans->IsMatchEnabled()) pTrans = NULL;
	byte *pImage = PTNCreate2AlignedImage(w,h,bytesPixel,flags);
	if(!pImage) return NULL;
	byte *p = pImage;
	int w1 = PTNGet2AlignedImageHeader(pImage)->width;
	for( int i = 0 ; i < h ; i++, p += w1-w )
	for( int j = 0 ; j < w ; j++, p++ ) {
		dword pixel = 0;
		if( f.Read(&pixel,bytesPixel) != bytesPixel )	return PTNUnload2AlignedImage(pImage);
		*p = pTrans ? (*pTrans)(pixel) : (byte)pixel;
	}
	return pImage;
}
// ------------------------------------------------------------------------
CSprite::CSprite(TCchar *name)
{
	FILE *f = CFileResource::FOpenCurrent(name);
	RTCHECK1(f,"Cannot open '%s'",name);
	m_nWidth = m_nHeight = 0;
	fread(&m_nWidth,2,1,f);
	fread(&m_nHeight,2,1,f);
	fread(&m_nTransparent,1,1,f);
	//m_pImage = new byte[m_nWidth*m_nHeight];
	//fread(m_pImage,m_nWidth*m_nHeight,1,f);
	m_pImage = PTNLoad2AlignedImage(f,m_nWidth,m_nHeight,1,NULL,0);
	RTCHECK1(m_pImage,"Sprite '%s' not loaded",name);
	fclose(f);
}
// ------------------------------------------------------------------------
void CSprite::DrawAt(int x,int y) const
{
	#if 1 //!_USE_OWNGR
		(void)x; (void)y;
	#else
		int	nDY = GRGetScreenWidth() - m_nWidth;
		byte *pd = GRGetPixelPtr(x,y), *ps = m_pImage;
		for( int i = 0 ; i < m_nHeight ; i++ ) {
			for( int j = 0 ; j < m_nWidth ; j++,ps++,pd++ )
				if(*ps!=m_nTransparent)	*pd=*ps;
			pd+=nDY;
		}
	#endif
}
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void CPhasedMovie::SPhase::Load(CTaggedFile &f)
{
	f.Descend("SPR8",FALSE,0);
		f.Descend("SPRH",TRUE,0);
			f.ReadInt(nWidth);
			f.ReadInt(nHeight);
			f.ReadInt(nTransp);
			nTransp = paletteTranslator((byte)nTransp);
		f.Ascend();
		f.Descend("SPRD",TRUE,0);
			pImage = PTNLoad2AlignedImage(f,nWidth,nHeight,1,&paletteTranslator,0);
			RTCHECK(pImage,"SPM Phase load error");
		f.Ascend();
		#if 0
		if( paletteTranslator.IsMatchEnabled() ) {
			for( byte *p = pImage ; len-->0 ; p++ )
				*p = paletteTranslator(*p);
		}
		#endif
	f.Ascend();

	//pCache = new int[nHeight*2+1]+nHeight;
	//for( int i = -nHeight ; i <= nHeight ; i++ )	pCache[i] = i*PTNGet2AlignedImageWidth(pImage);
}
// ------------------------------------------------------------------------
void CPhasedMovie::Load(CTaggedFile &f)
{
	f.Descend("SPM_",FALSE,0);
		f.Descend("SPMH",TRUE,0);
			f.ReadInt(m_nPhases);
			int nTransp;
			f.ReadInt(nTransp);
		f.Ascend();
		delete [] m_pPhases;
		m_pPhases = m_nPhases > 0 ? new SPhase[m_nPhases] : NULL;
		ASSERT(m_pPhases);
		f.Descend("PAL8",TRUE,0); {
			int numEntries;
			f.ReadInt(numEntries);
			byte	pal[768];
			f.Read(pal,numEntries*3);
			nTransp=paletteTranslator.Define((const SRGB *)pal,256,nTransp);
		} f.Ascend();
		f.Descend("FRMS",FALSE,0);
			for(int i = 0 ; i < m_nPhases ; i++ ) m_pPhases[i].Load(f);
		f.Ascend();
	f.Ascend();
}
// ------------------------------------------------------------------------
void CPhasedMovie::Load(TCchar *spmname)
{
	CTaggedFile f(TRUE);
	f.Open(spmname);
	f.SkipChunk("REM!",TRUE,0);
	Load(f);
}
// ------------------------------------------------------------------------
//CFMatrix3x4 CPhasedMovie::m_drawInv;
#if 0
inline void D2INT(int *p,double d) { *p=d; }
#pragma aux D2INT = \
	"fistp	dword ptr [eax]"	parm [eax] [8087]
void CPhasedMovie::TexturedCallBack(SGRTexturedCallBackData &tcbd)
{
	//double db,dd,dg,tmp,a,c,f;
	//const double FACTOR = 65536;
	CFMatrix3x4 &inv = m_drawInv;
	/*
	tcbd.a = ((a=inv.m[0][0]*inv.m[2][3]-inv.m[2][0]*inv.m[0][3]))*FACTOR;
	tcbd.c = ((c=inv.m[1][0]*inv.m[2][3]-inv.m[2][0]*inv.m[1][3]))*FACTOR;
	tcbd.f = (f=-inv.m[2][0])*FACTOR;
	db = inv.m[0][1]*inv.m[2][3]-inv.m[2][1]*inv.m[0][3];
	dd = inv.m[1][1]*inv.m[2][3]-inv.m[2][1]*inv.m[1][3];
	dg = -inv.m[2][1];
	tmp = inv.m[0][2]*inv.m[2][3]-inv.m[2][2]*inv.m[0][3];
	tcbd.b1 = (db*tcbd.y1+tmp)*FACTOR;
	tcbd.b2 = (db*tcbd.y2+tmp)*FACTOR;
	tmp = inv.m[1][2]*inv.m[2][3]-inv.m[2][2]*inv.m[1][3];
	tcbd.d1 = (dd*tcbd.y1+tmp)*FACTOR;
	tcbd.d2 = (dd*tcbd.y2+tmp)*FACTOR;
	tcbd.g1 = (dg*tcbd.y1-inv.m[2][2])*FACTOR;
	tcbd.g2 = (dg*tcbd.y2-inv.m[2][2])*FACTOR;
	tcbd.signx1 = a*tcbd.g1-f*tcbd.b1;
	tcbd.signx2 = a*tcbd.g2-f*tcbd.b2;
	tcbd.signy1 = c*tcbd.g1-f*tcbd.d1;
	tcbd.signy2 = c*tcbd.g2-f*tcbd.d2;
	*/
	double a = inv.m[0][0]*inv.m[2][3]-inv.m[2][0]*inv.m[0][3];
	double db = inv.m[0][1]*inv.m[2][3]-inv.m[2][1]*inv.m[0][3];
	double b0 = inv.m[0][2]*inv.m[2][3]-inv.m[2][2]*inv.m[0][3];
	double c = inv.m[1][0]*inv.m[2][3]-inv.m[2][0]*inv.m[1][3];
	double dd = inv.m[1][1]*inv.m[2][3]-inv.m[2][1]*inv.m[1][3];
	double d0 = inv.m[1][2]*inv.m[2][3]-inv.m[2][2]*inv.m[1][3];
	double f = -inv.m[2][0];
	double dg = -inv.m[2][1];
	double g0 = -inv.m[2][2];
	double fMaxFact = Max(fabs(a),Max(fabs(db),Max(fabs(c),Max(fabs(dd),Max(fabs(f),fabs(dg))))));
	double fMaxOffs = Max(fabs(b0),Max(fabs(d0),fabs(g0)));
	double fCurrFactor = 5E7/Max(fMaxFact*400,fMaxOffs);
	D2INT(&tcbd.a,a*fCurrFactor);
	D2INT(&tcbd.c,c*fCurrFactor);
	D2INT(&tcbd.f,f*fCurrFactor);
	D2INT(&tcbd.b1,(db*tcbd.y1+b0)*fCurrFactor);
	D2INT(&tcbd.b2,(db*tcbd.y2+b0)*fCurrFactor);
	D2INT(&tcbd.d1,(dd*tcbd.y1+d0)*fCurrFactor);
	D2INT(&tcbd.d2,(dd*tcbd.y2+d0)*fCurrFactor);
	D2INT(&tcbd.g1,(dg*tcbd.y1+g0)*fCurrFactor);
	D2INT(&tcbd.g2,(dg*tcbd.y2+g0)*fCurrFactor);
	D2INT(&tcbd.signx1,a*tcbd.g1-f*tcbd.b1);
	D2INT(&tcbd.signx2,a*tcbd.g2-f*tcbd.b2);
	D2INT(&tcbd.signy1,c*tcbd.g1-f*tcbd.d1);
	D2INT(&tcbd.signy2,c*tcbd.g2-f*tcbd.d2);

}
#endif

extern int __terrainLin;

void CPhasedMovie::Draw(double fTime,TCCFVector3 &pt,double fZoom)
{
	(void)fTime; (void)pt; (void)fZoom;
	#if 0
	for( int  n = 0 ; n < m_nPhases ; n++ )
		if( fTime < m_pPhases[n].fTime )	break;
	if( n >= m_nPhases )	return;
	static CFVector3	verts3[4];
	verts3[0].x = verts3[3].x = -m_pPhases[n].nWidth/2;
	verts3[1].x = verts3[2].x = m_pPhases[n].nWidth/2;
	verts3[0].y = verts3[1].y = -m_pPhases[n].nHeight/2;
	verts3[2].y = verts3[3].y = m_pPhases[n].nHeight/2;
	verts3[0].z = verts3[1].z = verts3[2].z = verts3[3].z = 0;
	static	CVector2	vertsT[4];
	vertsT[0].x = vertsT[0].y = vertsT[1].y = vertsT[3].x = 1<<16;
	vertsT[1].x = vertsT[2].x = (m_pPhases[n].nWidth-2)<<16;
	vertsT[2].y = vertsT[3].y = (m_pPhases[n].nHeight-2)<<16;
	CFMatrix3x4	dir;
	CFVector3  pt1;
	CViewObject::m_viewPointDirMx.Mult(pt,pt1);
	double	theta = atan2(pt1.y,hypot(pt1.x,pt1.z)),
			phi = atan2(-pt1.x,-pt1.z);
	dir.LoadIdentity().
					RotateOxL(theta).RotateOyL(phi).TranslateL(pt1);
	//m_drawInv.LoadTransposed(dir);
	dir.ScaleL(CViewObject::m_viewPointScale).
									ScaleR(CFVector3(fZoom,fZoom,1));
	//m_drawInv.ScaleR(CViewObject::m_viewPointScale_1).
	//				ScaleL(CFVector3(1/fZoom,1/fZoom,1)).
	//												TranslateL(verts3[2]);

	static	CFVector3	ptIm3[4];
	static	int			nClip[4];

	for( int i = 0 ; i < 4 ; i++ ) {
		dir.Mult(verts3[i],ptIm3[i]);
 		if( ptIm3[i].z <= CViewObject::m_fFrontClip ) nClip[i] = 1; else
		if(	ptIm3[i].z >= CViewObject::m_fBackClip ) nClip[i] = 2; else
		nClip[i] = 0;
	}

	//static	CVector2	ptIm2[8];
	//static	TCCVector2	*pPtIm2[8] = {
	//	ptIm2, ptIm2+1, ptIm2+2, ptIm2+3, ptIm2+4, ptIm2+5, ptIm2+6, ptIm2+7
	//};
	//CVector2	*pIm2 = ptIm2;

	//WriteLog("Movie: ");
	dword	dwHazeFlag = GR_POLY_ADD_NONE;
	int ngr = 0;
	int  iOth;
	for( i = 0 ; i < 4 ; i++ ) {
		if( ptIm3[i].z >= CViewFigure::HazeMin() ) dwHazeFlag = GR_POLY_ADD_HAZE;
		if( nClip[i] ) {
			double fClipD = nClip[i] != 2 ? CViewObject::m_fFrontClip : CViewObject::m_fBackClip;
			if( nClip[iOth=i>0?i-1:3] != nClip[i] ) {
				double	zDiff = ptIm3[iOth].z-ptIm3[i].z;
				double	t = zDiff > 1e-3 ? (fClipD-ptIm3[i].z)/zDiff : 0.5;
				_gr_vertices[ngr].texture.x = (ptIm3[iOth].x+(ptIm3[iOth].x-ptIm3[i].x)*t) / fClipD;
				_gr_vertices[ngr].texture.y = (ptIm3[iOth].y+(ptIm3[iOth].y-ptIm3[i].y)*t) / fClipD;
				_gr_vertices[ngr].texture.u = vertsT[iOth].x+int((vertsT[iOth].x-vertsT[i].x)*t);
				_gr_vertices[ngr].texture.v = vertsT[iOth].y+int((vertsT[iOth].y-vertsT[i].y)*t);
				_gr_vertices[ngr++].texture.iz = 65536/fClipD;
			}
			if( nClip[iOth=i<3?i+1:0] != nClip[i] ) {
				double	zDiff = ptIm3[iOth].z-ptIm3[i].z;
				double	t = zDiff > 1e-3 ? (fClipD-ptIm3[i].z)/zDiff : 0.5;
				_gr_vertices[ngr].texture.x = (ptIm3[iOth].x+(ptIm3[iOth].x-ptIm3[i].x)*t) / fClipD;
				_gr_vertices[ngr].texture.y = (ptIm3[iOth].y+(ptIm3[iOth].y-ptIm3[i].y)*t) / fClipD;
				_gr_vertices[ngr].texture.u = vertsT[iOth].x+int((vertsT[iOth].x-vertsT[i].x)*t);
				_gr_vertices[ngr].texture.v = vertsT[iOth].y+int((vertsT[iOth].y-vertsT[i].y)*t);
				_gr_vertices[ngr++].texture.iz = 65536/fClipD;
			}
		} else {
			_gr_vertices[ngr].texture.x = ptIm3[i].x/ptIm3[i].z;
			_gr_vertices[ngr].texture.y = ptIm3[i].y/ptIm3[i].z;
			_gr_vertices[ngr].texture.u = vertsT[i].x;
			_gr_vertices[ngr].texture.v = vertsT[i].y;
			_gr_vertices[ngr++].texture.iz = 65536/ptIm3[i].z;
		}
	}
	//WriteLog(".\n");
	if( ngr < 3 )	return;

	_gr_polygon.dwFullType =  GR_POLY_SPRITE_PERSP;
		//(__terrainLin ? GR_POLY_SPRITE_PERSP : GR_POLY_SPRITE_PERSP); // | dwHazeFlag;
	_gr_polygon.dwAddType = dwHazeFlag;
	_gr_polygon.nVertices = ngr;

	_gr_polygon.fill.texture.pTexture = m_pPhases[n].pImage;
	_gr_polygon.fill.texture.pTextureCache = PTNGet2AlignedImageHeader(m_pPhases[n].pImage)->pCache;
	#if _USE_GLIDE
		_gr_polygon.fill.texture.nColor = 0;
	#endif

	GRSetZPrecision(0);

	GRDrawPolygonPCCW();
//	GRCFillFlatPolyPCCW(pPtIm2,pIm2-ptIm2,15);


	//GRCFillTexturedSprPolyPCCW(pPtIm2,pIm2-ptIm2,TexturedCallBack,
	//			m_pPhases[n].nWidth,m_pPhases[n].nHeight,m_pPhases[n].pImage,
	//														m_pPhases[n].nTransp);
	#endif
}
// ------------------------------------------------------------------------

