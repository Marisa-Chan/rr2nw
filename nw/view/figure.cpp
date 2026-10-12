#ifndef	__VIEW_CPP__
#include"_view.h"
#endif
// --------------------------------------------------------------------------
int __nNormalsCnt, __nNormalsOpt;
uint DefineNormalType(TCCFVector3 &n)
{
	const double	fPrec = 1e-3;
	static	CFVector3	standardNormals[6] = {
		CFVector3(1,0,0),
		CFVector3(0,1,0),
		CFVector3(0,0,1),
		CFVector3(-1,0,0),
		CFVector3(0,-1,0),
		CFVector3(0,0,-1)
	};
	__nNormalsCnt++;
	for( uint i = 0 ; i < 6 ; i++ ) {
		if( fabs(standardNormals[i].x-n.x) < fPrec &&
			fabs(standardNormals[i].y-n.y) < fPrec &&
			fabs(standardNormals[i].z-n.z) < fPrec ) { __nNormalsOpt++; return i; }
	}
	return	1000;
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
#if !_USE_OPENGL
dword	CViewFigure::m_dwHazeFlag = 0;
#endif

double	CViewFigure::m_fHazeMult = 1,
		CViewFigure::m_fHazeAdd = 0,
		CViewFigure::m_fHazeMin = 200,
		CViewFigure::m_fHazeMax = 228;
double	CViewFigure::m_fWaterline = -1000;
//int		CViewFigure::m_nWaterline = -1000*nIntegerPrec;
static	WATER_SPLIT	CViewFigure::m_eWaterSplit = WATER_SPLIT_NONE;

//extern "C" {
//	void SetHaze(int,int);
//}

void CViewFigure::SetWaterline(double y)
{
	m_fWaterline = y;
	//m_nWaterline = Round(m_fWaterline=y)*fIntegerPrec;
}

void CViewFigure::SetHaze(SHazeDef &haze)
{
	m_fHazeMin = haze.nHazeMin;
	m_fHazeMax = haze.nHazeMin+haze.nHazeDist;
	m_fHazeMult = 128/(m_fHazeMax-m_fHazeMin);
	m_fHazeAdd = 200-m_fHazeMin*m_fHazeMult;
	#if _USE_OPENGL
		glFogi(GL_FOG_MODE,GL_LINEAR);
		glFogf(GL_FOG_START,m_fHazeMin);
		glFogf(GL_FOG_END,m_fHazeMax);
		byte nHazeColor = paletteTranslator.HazeColorIdx();
		GLfloat	haze[4];
		haze[0]=_gr_glPal[nHazeColor].r/255.;
		haze[1]=_gr_glPal[nHazeColor].g/255.;
		haze[2]=_gr_glPal[nHazeColor].b/255.;
		haze[3]=1;
		glFogfv(GL_FOG_COLOR,haze);
	#else
		GRSetHaze(haze.nHazeMin,haze.nHazeDist,&haze);
	#endif
}

#if _USE_OPENGL
void CViewFigure::SetHaze(bool b) {
	if(b)
		glEnable(GL_FOG);
	else
		glDisable(GL_FOG);
}
#endif
#if _USE_OPENGL
bool CViewFigure::GetHaze() {
	return	glIsEnabled(GL_FOG);
}
#endif
// --------------------------------------------------------------------------
#if 0 //!_USE_OPENGL
inline int CViewFigure::FormVertImages()
{
	ASSERT(0);
	for( int i = 0, nCI = 0 ; i < m_nVertices ; i++ ) {
		int cf;
		if( (cf=m_ppVertImages[i]->cf)!=0 ) {
			if( m_ppVertImages[i-1]->cf!=cf ) {
				const CViewObject::SInfVertImage *pI = m_ppInfVertImages[i*2];
				_gr_vertices[nCI].any.x = pI->v.x;
				_gr_vertices[nCI].any.y = pI->v.y;
				_gr_vertices[nCI].flat.iz = pI->v.z;
				nCI++;
			}
			if( m_ppVertImages[i+1]->cf!=cf ) {
				const CViewObject::SInfVertImage *pI = m_ppInfVertImages[i*2+1];
				_gr_vertices[nCI].any.x = pI->v.x;
				_gr_vertices[nCI].any.y = pI->v.y;
				_gr_vertices[nCI].flat.iz = pI->v.z;
				nCI++;
			}
		}
		else {
			_gr_vertices[nCI].any.x = m_ppVertImages[i]->v.x;
			_gr_vertices[nCI].any.y = m_ppVertImages[i]->v.y;
			_gr_vertices[nCI].flat.iz = m_ppVertImages[i]->v.z;
			nCI++;
		}
	}
	return	nCI;
}
#endif
// --------------------------------------------------------------------------
inline bool CViewFigure::IsVisibleFront()
{
	#if	_VIEW_ORTHO
		return CViewObject::m_bViewOrtho ?
				m_normal*CViewObject::m_currObjViewPoint <= 0 :
				m_normal*(m_vertex0-CViewObject::m_currObjViewPoint)<=0;
	#else
		if(	m_fNormalPt <= ScalarProdNormal(m_nNormalType,
								m_normal,CViewObject::m_currObjViewPoint) ) {
			if( _gr_polygon.nLights ) {
				SFVector3 n; double d;
				n = (*CViewObjectRef::m_pCurrRefDir)%m_normal;
				//pt = (*CViewObjectRef::m_pCurrRefDir)*m_vertex0;
				d = m_fNormalPt+n*CViewObjectRef::m_pCurrRefDir->Offset();
				_gr_polygon.d = d;
				_gr_polygon.a = n.x;
				_gr_polygon.b = n.y;
				_gr_polygon.c = n.z;
				//ASSERT( fabs(d-n*pt) < 0.001 );
			}
			return TRUE;
		}
		else
			return FALSE;
		// m_fNormalPt <= m_normal*CViewObject::m_currObjViewPoint
	#endif
}
// --------------------------------------------------------------------------
void CViewFigure::RecalcNormal()
{
	ASSERT(m_pMorphData);
	m_vertex0 = *m_pMorphData->m_pVertex[0];
	m_normal = ::Normal((*m_pMorphData->m_pVertex[1]-m_vertex0)%
							(*m_pMorphData->m_pVertex[2]-m_vertex0));
	m_fNormalPt = m_vertex0 * m_normal;
	m_nNormalType = 1000;
	//m_bRecalc = TRUE;
}
// --------------------------------------------------------------------------
inline void CViewFigure::InterpolateClipped(double fy,double dy,TCSFVector3Y &v0,
 			int i0,TCSFVector3Y &v1,int i1,int &nCI,
						bool bGouraud,dword *pGourauds,
						bool bTextured,CViewFigure::STextVert *pTextVerts)
{
	//int t = SHLD_Div(fy,dy,CViewObject::CLIP_SHIFT);
	ASSERT( dy >= 0 && fy <= dy && fy >= 0 );
	double t = fy/Max(dy,1e-5), _1_t = 1.-t;
	int    nt;
	D2INT(&nt,t*CViewObject::CLIP_PREC);
	//int z =	Mul_SHRD(v1.z,t,CViewObject::CLIP_SHIFT)+
	//		Mul_SHRD(v0.z,(CViewObject::CLIP_PREC-t),CViewObject::CLIP_SHIFT);
	double _1z = 1./(v1.z*t+v0.z*_1_t);
	//_gr_vertices[nCI].any.x =
	//	(Mul_SHRD(v1.x,t,CViewObject::CLIP_SHIFT)+
	//		Mul_SHRD(v0.x,(CViewObject::CLIP_PREC-t),CViewObject::CLIP_SHIFT))/z;
	D2INT(&_gr_vertices[nCI].any.x,(v1.x*t+v0.x*_1_t)*_1z);
	//_gr_vertices[nCI].any.y =
	//	(Mul_SHRD(v1.y,t,CViewObject::CLIP_SHIFT)+
	//		Mul_SHRD(v0.y,(CViewObject::CLIP_PREC-t),CViewObject::CLIP_SHIFT))/z;
	D2INT(&_gr_vertices[nCI].any.y,(v1.y*t+v0.y*_1_t)*_1z);
	//_gr_vertices[nCI].any.iz =
	//	SHLD_Div(65536*nIntegerPrec,z,CViewObjectBase::m_z1Shift);
	D2INT(&_gr_vertices[nCI].any.iz,_1z*CViewObjectBase::m_fz1Scale);
	if( bGouraud ) {
		_gr_vertices[nCI].gouraud.b =
			Mul_SHRD(pGourauds[i1],nt,CViewObject::CLIP_SHIFT)+
			Mul_SHRD(pGourauds[i0],(CViewObject::CLIP_PREC-nt),CViewObject::CLIP_SHIFT);
	}
	if( bTextured ) {
		_gr_vertices[nCI].texture.u =
			Mul_SHRD(pTextVerts[i1].u,nt,CViewObject::CLIP_SHIFT)+
			Mul_SHRD(pTextVerts[i0].u,(CViewObject::CLIP_PREC-nt),CViewObject::CLIP_SHIFT);
		_gr_vertices[nCI].texture.v =
			Mul_SHRD(pTextVerts[i1].v,nt,CViewObject::CLIP_SHIFT)+
			Mul_SHRD(pTextVerts[i0].v,(CViewObject::CLIP_PREC-nt),CViewObject::CLIP_SHIFT);
	}
	nCI++;
}
// --------------------------------------------------------------------------
#define	SPLITDIFF(X,Y)	(nSplitUp?X-Y:Y-X)
inline int CViewFigure::FormVertImagesSplit(int nSplitUp,bool bGouraud,dword *pGourauds,
									bool bTextured,CViewFigure::STextVert *pTextVerts)
{
	TCSFVector3Y	*pTemp = NULL;
	bool		ycfTemp;
	int cf;
	double dy;
	int nCI;
	for( int i = 0, nCI = 0, pass = 0 ; i < m_nVertices ; ) {
		ASSERT( i < m_nVertices );
		if( (cf=m_ppVertImages[i]->cf)!=0 ) { // if Z-clipped
			if( m_ppVertImages[i-1]->cf!=cf ) { // prev Z-clipped
				const CViewObject::SInfVertImage *pI = m_ppInfVertImages[i*2];
				if( (dy = SPLITDIFF(pI->v3.y0,m_fWaterline)) >= 0 ) {
					if( pTemp && ycfTemp )
						InterpolateClipped(dy,SPLITDIFF(pI->v3.y0,pTemp->y0),pI->v3,i,*pTemp,i-1,nCI,
										bGouraud,pGourauds,bTextured,pTextVerts);
					if( pass ) break;
					_gr_vertices[nCI].any.x = pI->v.x;
					_gr_vertices[nCI].any.y = pI->v.y;
					_gr_vertices[nCI].any.iz = pI->v.z;
					if( bGouraud )
						_gr_vertices[nCI].gouraud.b =
							Mul_SHRD(pGourauds[i-1],pI->nRatio,CViewObject::CLIP_SHIFT)+
							Mul_SHRD(pGourauds[i],(CViewObject::CLIP_PREC-pI->nRatio),CViewObject::CLIP_SHIFT);
					if( bTextured ) {
						_gr_vertices[nCI].texture.u =
							Mul_SHRD(pTextVerts[i-1].u,pI->nRatio,CViewObject::CLIP_SHIFT)+
							Mul_SHRD(pTextVerts[i].u,(CViewObject::CLIP_PREC-pI->nRatio),CViewObject::CLIP_SHIFT);
						_gr_vertices[nCI].texture.v =
							Mul_SHRD(pTextVerts[i-1].v,pI->nRatio,CViewObject::CLIP_SHIFT)+
							Mul_SHRD(pTextVerts[i].v,(CViewObject::CLIP_PREC-pI->nRatio),CViewObject::CLIP_SHIFT);
					}
					pTemp = &pI->v3; ycfTemp = FALSE;
					nCI++;
				} // if above waterline
				else { // if below waterline
					if( pTemp && !ycfTemp )
						InterpolateClipped(-dy,SPLITDIFF(pTemp->y0,pI->v3.y0),pI->v3,i,*pTemp,i-1,nCI,
										bGouraud,pGourauds,bTextured,pTextVerts);
					if( pass ) break;
					pTemp = &pI->v3; ycfTemp = TRUE;
				}
			} // previous Z-clipped
			if( m_ppVertImages[i+1]->cf!=cf ) { // next Z-clipped
				const CViewObject::SInfVertImage *pI = m_ppInfVertImages[i*2+1];
				if( (dy = SPLITDIFF(pI->v3.y0,m_fWaterline)) >= 0 ) {
					if( pTemp && ycfTemp )
						InterpolateClipped(dy,SPLITDIFF(pI->v3.y0,pTemp->y0),pI->v3,i,*pTemp,i-1,nCI,
										bGouraud,pGourauds,bTextured,pTextVerts);
					if( pass ) break;
					_gr_vertices[nCI].any.x = pI->v.x;
					_gr_vertices[nCI].any.y = pI->v.y;
					_gr_vertices[nCI].any.iz = pI->v.z;
					if( bGouraud )
						_gr_vertices[nCI].gouraud.b =
							Mul_SHRD(pGourauds[i+1],pI->nRatio,CViewObject::CLIP_SHIFT)+
							Mul_SHRD(pGourauds[i],(CViewObject::CLIP_PREC-pI->nRatio),CViewObject::CLIP_SHIFT);
					if( bTextured ) {
						_gr_vertices[nCI].texture.u =
							Mul_SHRD(pTextVerts[i+1].u,pI->nRatio,CViewObject::CLIP_SHIFT)+
							Mul_SHRD(pTextVerts[i].u,(CViewObject::CLIP_PREC-pI->nRatio),CViewObject::CLIP_SHIFT);
						_gr_vertices[nCI].texture.v =
							Mul_SHRD(pTextVerts[i+1].v,pI->nRatio,CViewObject::CLIP_SHIFT)+
							Mul_SHRD(pTextVerts[i].v,(CViewObject::CLIP_PREC-pI->nRatio),CViewObject::CLIP_SHIFT);
					}
					pTemp = &pI->v3; ycfTemp = FALSE;
					nCI++;
				} // if above waterline
				else { // below waterline
					if( pTemp && !ycfTemp )
						InterpolateClipped(-dy,SPLITDIFF(pTemp->y0,pI->v3.y0),pI->v3,i,*pTemp,i-1,nCI,
										bGouraud,pGourauds,bTextured,pTextVerts);
					if( pass ) break;
					pTemp = &pI->v3; ycfTemp = TRUE;
				}
			} // next Z-clipped
		} // Z-clipped
		else { // not Z-clipped
			const CViewObject::SFinVertImage *pI = m_ppVertImages[i];
			if( (dy = SPLITDIFF(pI->v3.y0,m_fWaterline)) >= 0 ) {
				if( pTemp && ycfTemp )
					InterpolateClipped(dy,SPLITDIFF(pI->v3.y0,pTemp->y0),pI->v3,i,*pTemp,i-1,nCI,
									bGouraud,pGourauds,bTextured,pTextVerts);
				if( pass ) break;
				_gr_vertices[nCI].any.x = pI->v.x;
				_gr_vertices[nCI].any.y = pI->v.y;
				_gr_vertices[nCI].any.iz = pI->v.z;
				if ( bGouraud )
					_gr_vertices[nCI].gouraud.b = pGourauds[i];
				if( bTextured ) {
					_gr_vertices[nCI].texture.u = pTextVerts[i].u;
					_gr_vertices[nCI].texture.v = pTextVerts[i].v;
				}
				pTemp = &pI->v3; ycfTemp = FALSE;
				nCI++;
			} // if above waterline
			else {
				if( pTemp && !ycfTemp )
					InterpolateClipped(-dy,SPLITDIFF(pTemp->y0,pI->v3.y0),pI->v3,i,*pTemp,i-1,nCI,
									bGouraud,pGourauds,bTextured,pTextVerts);
				if( pass ) break;
				pTemp = &pI->v3; ycfTemp = TRUE;
			} // below waterline
		} // if not Z-clipped
		if( ++i >= m_nVertices )	{
			if( nCI <= 0 ) break;
			ASSERT( nCI >= 2 );
			i = 0; pass = 1;
		}
	}
	ASSERT( nCI == 0 || nCI >= 3 );

	return _gr_polygon.nVertices = nCI;
}
#undef	SPLITDIFF

inline int CViewFigure::FormVertImages(bool bGouraud,dword *pGourauds,
									bool bTextured,CViewFigure::STextVert *pTextVerts)
{
	//eSplit = WATER_SPLIT_DOWN;
	switch( m_eWaterSplit ) {
		case WATER_SPLIT_UP:
			return FormVertImagesSplit(1,bGouraud,pGourauds,bTextured,pTextVerts);
		case WATER_SPLIT_DOWN:
			return FormVertImagesSplit(0,bGouraud,pGourauds,bTextured,pTextVerts);
	}
	#if _USE_OWNGR || _USE_GLIDE
		int nCI;
		for( int i = 0, nCI = 0 ; i < m_nVertices ; i++ ) {
			int cf;
			if( (cf=m_ppVertImages[i]->cf)!=0 ) {
				if( m_ppVertImages[i-1]->cf!=cf ) {
					const CViewObject::SInfVertImage *pI = m_ppInfVertImages[i*2];
					_gr_vertices[nCI].any.x = pI->v.x;
					_gr_vertices[nCI].any.y = pI->v.y;
					_gr_vertices[nCI].any.iz = pI->v.z;
					if( bGouraud )
						_gr_vertices[nCI].gouraud.b =
							Mul_SHRD(pGourauds[i-1],pI->nRatio,CViewObject::CLIP_SHIFT)+
							Mul_SHRD(pGourauds[i],(CViewObject::CLIP_PREC-pI->nRatio),CViewObject::CLIP_SHIFT);
					if( bTextured ) {
						_gr_vertices[nCI].texture.u =
							Mul_SHRD(pTextVerts[i-1].u,pI->nRatio,CViewObject::CLIP_SHIFT)+
							Mul_SHRD(pTextVerts[i].u,(CViewObject::CLIP_PREC-pI->nRatio),CViewObject::CLIP_SHIFT);
						_gr_vertices[nCI].texture.v =
							Mul_SHRD(pTextVerts[i-1].v,pI->nRatio,CViewObject::CLIP_SHIFT)+
							Mul_SHRD(pTextVerts[i].v,(CViewObject::CLIP_PREC-pI->nRatio),CViewObject::CLIP_SHIFT);
					}
					nCI++;
				}
				if( m_ppVertImages[i+1]->cf!=cf ) {
					const CViewObject::SInfVertImage *pI = m_ppInfVertImages[i*2+1];
					_gr_vertices[nCI].any.x = pI->v.x;
					_gr_vertices[nCI].any.y = pI->v.y;
					_gr_vertices[nCI].any.iz = pI->v.z;
					if( bGouraud )
						_gr_vertices[nCI].gouraud.b =
							Mul_SHRD(pGourauds[i+1],pI->nRatio,CViewObject::CLIP_SHIFT)+
							Mul_SHRD(pGourauds[i],(CViewObject::CLIP_PREC-pI->nRatio),CViewObject::CLIP_SHIFT);
					if( bTextured ) {
						_gr_vertices[nCI].texture.u =
							Mul_SHRD(pTextVerts[i+1].u,pI->nRatio,CViewObject::CLIP_SHIFT)+
							Mul_SHRD(pTextVerts[i].u,(CViewObject::CLIP_PREC-pI->nRatio),CViewObject::CLIP_SHIFT);
						_gr_vertices[nCI].texture.v =
							Mul_SHRD(pTextVerts[i+1].v,pI->nRatio,CViewObject::CLIP_SHIFT)+
							Mul_SHRD(pTextVerts[i].v,(CViewObject::CLIP_PREC-pI->nRatio),CViewObject::CLIP_SHIFT);
					}
					nCI++;
				}
			}
			else {
				const CViewObject::SFinVertImage *pI = m_ppVertImages[i];
				_gr_vertices[nCI].any.x = pI->v.x;
				_gr_vertices[nCI].any.y = pI->v.y;
				_gr_vertices[nCI].any.iz = pI->v.z;
				if ( bGouraud )
					_gr_vertices[nCI].gouraud.b = pGourauds[i];
				if( bTextured ) {
					_gr_vertices[nCI].texture.u = pTextVerts[i].u;
					_gr_vertices[nCI].texture.v = pTextVerts[i].v;
				}
				nCI++;
			}
		}
		return _gr_polygon.nVertices = nCI;
	#else
		#error	Unsupported target
	#endif
}
// --------------------------------------------------------------------------
CViewFigure *CViewFigure::Read(CTaggedFile &f, bool bDummy)
{
	int nNormalIdx;

	CViewFigure	*pFig = NULL;

	f.Descend("FIG_",FALSE,1);
		f.Descend("FIGH",TRUE,3);
			int type;
			f.ReadInt(type);
			CNameDef	name(f);
			switch(type) {
				case T_FLAT:		pFig =	new CViewFlatFigure; break;
				case T_OWNTEXTURED:	pFig =	new CViewOwnTexturedFigure; break;
				case T_LIBTEXTURED:	pFig =	new CViewLibTexturedFigure; break;
				case T_FLATGOURAUD:
						pFig = new CViewFlatGouraudFigure; break;
				case T_FLATTRANSPARENT:
						pFig =	new CViewFlatTransparentFigure; break;
				case T_FLATTRANSPARENTFLASH:
						pFig =	new CViewFlatTransparentFlashFigure; break;
				case T_DUMMY:
						pFig = new CViewFigure; break;
				default:	RTCHECK(0,"Bad FigT");
			}
			if(!name.IsEmpty())	pFig->m_pMorphData = new SMorphData;
			name.Resolve(pFig,CViewObjectBase::GetCurrBase()->Names().figures);
			f.ReadInt(pFig->m_nVertices);
			RTCHECK(pFig->m_nVertices>=3&&pFig->m_nVertices<=MAX_VERTICES,"Fig R Num Vert");
			f.ReadInt(nNormalIdx);
			pFig->m_normal.Read(f);
            ASSERT(fabs(Abs2(pFig->m_normal) - 1) < 1e-5);

			if( type == T_DUMMY ) {
				(void)bDummy; ASSERT(bDummy);
				#if _USE_OPENGL
					pFig->m_ppVertices = NULL;
				#else
					pFig->m_ppVertices = new TCCFVector3*[pFig->m_nVertices+2]+1;
					pFig->m_ppVertImages = NULL; pFig->m_ppInfVertImages = NULL;
					pFig->m_ppVertImages++;
				#endif
				int i = 0;
				for( i = 0 ; i < pFig->m_nVertices ; i++ ) {
					int  n;
					f.ReadInt(n);
					pFig->m_ppVertices[i] = CViewObjectBase::GetCurrBase()->GetVertexPtr(n);
					if( i == 0 ) pFig->m_vertex0 = CViewObjectBase::GetCurrBase()->GetVertex(n);
					if( pFig->m_pMorphData && i < 3 )	pFig->m_pMorphData->m_pVertex[i] = CViewObjectBase::GetCurrBase()->GetVertexPtr(n);
				}
				pFig->m_ppVertices[i] = pFig->m_ppVertices[0];
				pFig->m_ppVertices[-1] = pFig->m_ppVertices[pFig->m_nVertices-1];
			} else {	// !dummy
				#if _USE_OPENGL
					pFig->m_ppVertices = new TCCFVector3*[pFig->m_nVertices];
				#else
					pFig->m_ppVertices = new TCCFVector3*[pFig->m_nVertices+2]+1;
					pFig->m_ppVertImages =
						new CViewObject::TCSFinVertImage*[pFig->m_nVertices+2]+1;
					pFig->m_ppInfVertImages = new CViewObject::TCSInfVertImage*[
														(pFig->m_nVertices+1)*2];
				#endif
				int i;
				for( i = 0 ; i < pFig->m_nVertices ; i++ ) {
					int  n;
					f.ReadInt(n);
					if( i == 0 ) pFig->m_vertex0 = CViewObjectBase::GetCurrBase()->GetVertex(n);
					if( pFig->m_pMorphData && i < 3 )	pFig->m_pMorphData->m_pVertex[i] = CViewObjectBase::GetCurrBase()->GetVertexPtr(n);
					#if _USE_OPENGL
						pFig->m_ppVertices[i] = CViewObjectBase::GetCurrBase()->GetVertexPtr(n);
					#else
						pFig->m_ppVertices[i] = CViewObjectBase::GetCurrBase()->GetVertexPtr(n);
						pFig->m_ppVertImages[i] = &CViewObject::m_paVertImages[n];
					#endif
					CalcBounds(CViewObjectBase::GetCurrBase()->GetVertex(n),
								CViewBody::m_minBound, CViewBody::m_maxBound);
				}
				for( i = 0 ; i < pFig->m_nVertices ; i++ ) {
					int  nPred, nSucc;
					f.ReadInt(nPred);	f.ReadInt(nSucc);
					#if !_USE_OPENGL
						pFig->m_ppInfVertImages[i*2] =
										&CViewObject::m_paInfVertImages[nPred];
						pFig->m_ppInfVertImages[i*2+1] =
										&CViewObject::m_paInfVertImages[nSucc];
					#endif
				}
				#if !_USE_OPENGL
					pFig->m_ppVertImages[i] = pFig->m_ppVertImages[0];
					pFig->m_ppVertImages[-1] = pFig->m_ppVertImages[pFig->m_nVertices-1];
					pFig->m_ppInfVertImages[i*2] = pFig->m_ppInfVertImages[0];
					pFig->m_ppInfVertImages[i*2+1] = pFig->m_ppInfVertImages[1];
					pFig->m_ppVertices[i] = pFig->m_ppVertices[0];
					pFig->m_ppVertices[-1] = pFig->m_ppVertices[pFig->m_nVertices-1];
				#endif
			} // if(dummy)
			{
				int nNamed0Idx;
				f.ReadInt(nNamed0Idx);
				ASSERT((uint)nNamed0Idx <= pFig->m_nVertices);
				if(pFig->m_pMorphData) pFig->m_pMorphData->m_nNamed0Idx = nNamed0Idx;
			}
		f.Ascend();	// FIGH
		pFig->m_fNormalPt = pFig->m_vertex0*pFig->m_normal;
		pFig->m_nNormalType = DefineNormalType(pFig->m_normal);
		if( pFig->m_pMorphData ) {
			CFVector3	n = pFig->m_normal;
			double		pt = pFig->m_fNormalPt;
			pFig->RecalcNormal();
			//ASSERT(fabs(n*pFig->m_normal-1)<1e-3);
			//ASSERT(fabs(pt-pFig->m_fNormalPt)<1e-3);
		}
		if( type != T_DUMMY ) {
			f.Descend("FIGS",TRUE,1);
				pFig->_Read(f);
			f.Ascend(); // FIGS
		}
	f.Ascend(); // FIG_
	return pFig;
}
// --------------------------------------------------------------------------
bool SolveDynamicSq(double a,double b_2,double c,double &t)
{
	// a == v2, b_2 == s*v == b/2, c == s2-r2

	//      2    b
	//  a x   +  - x  + c == 0
	//           2

	ASSERT( a >= 0 );
	if( b_2 >= 0 ) return FALSE; // distance increasing (vel*dist>=0)
	if( c <= 0 )	{
		t = 0;
		return	TRUE;
	}
	double	t1;
	if( a < 1e-12 ) {
		// b_2 < 0, c > 0
		t1 = c/Max(-b_2,1e-12);
	}
	else {
		double	d = b_2*b_2-a*c;
		if( d <= 0 )	return FALSE;
		t1 = (-b_2-sqrt(d))/a;
	}
	if( t1 > t ) return FALSE;
	t = t1;
	ASSERT( t >= 0 );
	return	TRUE;
}

bool CViewFigure::AdvancedBump(SBumpDef0 &def)
{
	if( m_nVertices < 3 ) return FALSE;

	// d = fa*t+fb
	double	fa = def.vel*m_normal;
	//if( fa >= 0 ) return FALSE;	// going outside, do not bump
	double	fb = m_normal*def.start-m_fNormalPt;
	//if( hypot(def.vel.x,def.vel.z) > 0.1 )
	//	printf("%lg*t+%lg=%lg at t=%lg  (vel=%lg)\n",fa,fb,def.fRadius,(def.fRadius-fb)/AbsNoLess(fa,1e-10),Abs(def.vel));
	if( fa >= 0 ) return FALSE;	// going outside, do not bump
	if( fb < 0 ) return FALSE;	// center point is inside plane, do not bump

	bool	bRes = FALSE;

	// tl <= t <= tr  - is inside infinite prism built on the figure
	double	tl = 0, tr = def.fTime;
	for( int i = 0 ; i < m_nVertices ; i++ ) {
		TCCFVector3	&v = *m_ppVertices[i];
		CFVector3	dv = *m_ppVertices[i+1]-v;
		if( Abs2(dv) < 1e-8 ) continue;
		CFVector3	n = dv%m_normal;
		double	a = def.vel*n;
		double	b = n*(def.start-v);
		if( a >= 0 ) {
			if( b < 0 )
				tr = Min(tr,-b/Max(a,1e-10));
			else
				goto BumpEdges; // never gets inside
		}
		else {
			if( b >= 0 )
				tl = Max(tl,-b/Min(a,-1e-10));
		}
		if( tl >= tr ) goto BumpEdges;
	}

	fb += fa*tl;	// at this point coming into prism
	if( fb < 0 ) goto BumpEdges;
	//if( fb < def.fRadius ) goto BumpEdges;	// crosses plane. Will bump on edge or vertex

	tl += Max(0.,(def.fRadius-fb)/Min(fa,-1e-10));	// will bump plane at this time
	if( tl < tr ) {		// if inside prism when bumped plane then bumped indeed
		//printf("Bump plane after %lg",tl);
		def.fTime = tl;
		def.vel1 = def.vel-m_normal*((def.vel*m_normal)*2);
		//def.vel1 = -def.vel;
		bRes = TRUE;
		if( tl > 0 && fb < def.fRadius )	{
			//printf("   :edges\n");
			goto BumpEdges; // coming into prism
		}
		//printf("\n");
		return	TRUE;	// and does not bump edges
	}
	// otherwise still chances that bumps on edges or vertices

BumpEdges:
	double	RR = def.fRadius *def.fRadius;
	for( int i = 0 ; i < m_nVertices ; i++ ) {

		TCCFVector3	&v = *m_ppVertices[i];
		CFVector3	n = (*m_ppVertices[i+1]-v);
		double		L = Abs(n);
		if( L < 1e-5 ) continue;
		n *= (1./L);

		CFVector3	s = def.start-v;
		double	b = s*n;
		double	a = def.vel*n;
		// projections of start and stop points on edge
		double	l = b, lr = a*def.fTime, r = l + lr;
		double	tl = 0, tr = def.fTime;
		if( lr > 0 ) {	// l < r
			if( r <= 0 ) continue;
			if( l >= L ) continue;
			lr = Max(1e-4,lr);
			if( l < 0 )	{ tl = -l/lr; l = 0; }
			if( r > L ) { tr -= (r-L)/lr; r = L; }
		}
		else {
			if( l <= 0 ) continue;
			if( r >= L ) continue;
			lr = Min(-1e-4,lr);
			if( r < 0 )	{ tl = r/lr; r = 0; }
			if( l > L ) { tr -= (L-l)/lr; l = L; }
		}
		if( tl >= tr ) continue;

		//CFVector3	_1_n2 = CFVector3(1,1,1)-(n&n);
		CFVector3	_a = def.vel%n;
		CFVector3	_b = s%n;
		// d = Abs(_a*t+_b)
		if( SolveDynamicSq(_a*_a,_b*_a,_b*_b-RR,tr) && tl < tr ) {
			def.fTime = tr;
			s += def.vel*def.fTime;
			s = s-(s*n)*n;
			ASSERT( Abs(s) > 1e-4 );	// if radius > 0 then Abs(s) must be > 0
			s *= 1/Abs(s);
			def.vel1 = def.vel-s*((def.vel*s)*2);
			//def.vel1 = -def.vel;
			bRes = TRUE;
			//printf("Edge bump\n");
		}
	}

	for( int i = 0 ; i < m_nVertices ; i++ ) {

		TCCFVector3	&v = *m_ppVertices[i];
		CFVector3	s = def.start-v;

		if( SolveDynamicSq(def.vel*def.vel,def.vel*s,s*s-RR,def.fTime) ) {
			s += def.vel*def.fTime;
			ASSERT( Abs(s) > 1e-4 );
			s *= 1/Abs(s);
			def.vel1 = def.vel-s*((def.vel*s)*2);
			//def.vel1 = -def.vel;
			bRes = TRUE;
			//printf("Vertex bump\n");
		}
	}

	return bRes;
}
// --------------------------------------------------------------------------
void CViewFigure::operator%=(TCCFMatrix3x4 &dir)
{
	m_normal = dir%m_normal;
	__nNormalsCnt--;
	if( m_nNormalType < 6 )	__nNormalsOpt--;
	m_nNormalType = DefineNormalType(m_normal);
	m_vertex0 = dir%m_vertex0;
}
double CViewFigure::ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d)
{
	#if _USE_OPENGL
		ASSERT(0);	// No implementation yet
	#endif
	double fErr = 1e10;
	if( m_ppVertImages-1 )
	for( int i = 0 ; i < m_nVertices ; i++ ) {
		TCCFVector3	&v = CViewObjectBase::m_pCurrBase->m_pVertices[
				m_ppVertImages[i]-CViewObjectBase::m_pCurrBase->m_pVertImages];
		fErr = Min(fErr,(v-pt)*n-d);
	}
	return fErr;
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
void CViewFlatFigure::_Read(CTaggedFile &f)
{
	dword dwColor;
	f.ReadDWord(dwColor);
	//m_nColor = paletteTranslator(dwColor);
    SRGB &pal = paletteTranslator.LocalPalette()[dwColor];
    m_nColor = GRCreateColor(pal.r,pal.g,pal.b);
}
// --------------------------------------------------------------------------
extern dword nWhiteColor;
void CViewFlatFigure::Draw()
{
	if( !IsVisibleFront() ) return;
	#if _USE_OPENGL
		glBegin(GL_POLYGON);
			glColor3ub(_gr_glPal[m_nColor].r,_gr_glPal[m_nColor].g,_gr_glPal[m_nColor].b);
			for( int i = 0 ; i < m_nVertices ; i++ )
				glVertex3d(m_ppVertices[i]->x,m_ppVertices[i]->y,m_ppVertices[i]->z);
		glEnd();
	#else
		//if( (_gr_polygon.nVertices = FormVertImages()) < 3 ) return;
		if( FormVertImages(FALSE,NULL,FALSE,NULL) < 3 )	return;
		_gr_polygon.dwFullType = GR_POLY_FLAT; // | m_dwHazeFlag; //GR_POLY_ADD_HAZE;
		_gr_polygon.dwAddType = m_dwHazeFlag;
        _gr_polygon.dwColor.color = m_nColor;
		/*if( m_bRecalc ) {
			_gr_polygon.dwColor.color = nWhiteColor;
			_gr_polygon.dwFullType = GR_POLY_FLAT;
		}*/
		GRDrawPolygonPCCW();
	#endif
}
// --------------------------------------------------------------------------
void CViewFlatFigure::Hue(double fModify)
{
	ASSERT(0); (void)fModify;
	//m_nColor = (byte)paletteTranslator.Hue(m_nColor,fModify);
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
void CViewFlatGouraudFigure::_Read(CTaggedFile &f)
{
	dword dwColor;
	f.ReadDWord(dwColor);
	//m_nColor = paletteTranslator(dwColor);
    SRGB &pal = paletteTranslator.LocalPalette()[dwColor];
    m_nColor = GRCreateColor(pal.r,pal.g,pal.b);
	#if _USE_OPENGL
		m_pglGourauds = new double[m_nVertices];
	#else
		m_pGourauds = (new dword[m_nVertices+2])+1;
	#endif
	for( int   i = 0 ; i < m_nVertices ; i++ ) {
		double b;
		f.ReadDouble(b); b = MinMax(b,-1.,1.);
		#if _USE_OPENGL
			m_pglGourauds[i] = (b+1)/255.;
		#else
			m_pGourauds[i] = (dword) Round( 8+7*b ) << 16;
		#endif
	}
	#if !_USE_OPENGL
		m_pGourauds[m_nVertices] = m_pGourauds[0];
		m_pGourauds[-1] = m_pGourauds[m_nVertices-1];
	#endif
}
// --------------------------------------------------------------------------
void CViewFlatGouraudFigure::Draw()
{
	if( !IsVisibleFront() )	return;

	#if _USE_OPENGL
		glPushAttrib(GL_LIGHTING_BIT);
		glShadeModel(GL_SMOOTH);
		glBegin(GL_POLYGON);
			for( int i = 0 ; i < m_nVertices ; i++ ) {
				glColor3d(_gr_glPal[m_nColor].r*m_pglGourauds[i],
								_gr_glPal[m_nColor].g*m_pglGourauds[i],
								_gr_glPal[m_nColor].b*m_pglGourauds[i]);
				glVertex3d(m_ppVertices[i]->x,m_ppVertices[i]->y,m_ppVertices[i]->z);
			}
		glEnd();
		glPopAttrib();
	#else
		#if 0
		for( int i = 0, nCI = 0 ; i < m_nVertices ; i++ ) {
			int cf;
			if( (cf=m_ppVertImages[i]->cf)!=0 ) {
				if( m_ppVertImages[i-1]->cf!=cf ) {
					const CViewObject::SInfVertImage *pI = m_ppInfVertImages[i*2];
					_gr_vertices[nCI].gouraud.x = pI->v.x;
					_gr_vertices[nCI].gouraud.y = pI->v.y;
					_gr_vertices[nCI].gouraud.iz = pI->v.z;
					_gr_vertices[nCI].gouraud.b =
						Mul_SHRD(m_pGourauds[i-1],pI->nRatio,CViewObject::CLIP_SHIFT)+
						Mul_SHRD(m_pGourauds[i],(CViewObject::CLIP_PREC-pI->nRatio),CViewObject::CLIP_SHIFT);
					nCI++;
				}
				if( m_ppVertImages[i+1]->cf!=cf ) {
					const CViewObject::SInfVertImage *pI = m_ppInfVertImages[i*2+1];
					_gr_vertices[nCI].gouraud.x = pI->v.x;
					_gr_vertices[nCI].gouraud.y = pI->v.y;
					_gr_vertices[nCI].gouraud.iz = pI->v.z;
					_gr_vertices[nCI].gouraud.b =
						Mul_SHRD(m_pGourauds[i+1],pI->nRatio,CViewObject::CLIP_SHIFT)+
						Mul_SHRD(m_pGourauds[i],(CViewObject::CLIP_PREC-pI->nRatio),CViewObject::CLIP_SHIFT);
					nCI++;
				}
			}
			else {
				_gr_vertices[nCI].gouraud.x = m_ppVertImages[i]->v.x;
				_gr_vertices[nCI].gouraud.y = m_ppVertImages[i]->v.y;
				_gr_vertices[nCI].gouraud.iz = m_ppVertImages[i]->v.z;
				_gr_vertices[nCI].gouraud.b = m_pGourauds[i];
				nCI++;
			}
		}
		if( nCI < 3 ) return;
		#endif

		if( FormVertImages(TRUE,m_pGourauds,FALSE,NULL) < 3 )	return;

		_gr_polygon.dwFullType = GR_POLY_GOURAUD; // | m_dwHazeFlag; //GR_POLY_ADD_HAZE;
		_gr_polygon.dwAddType = m_dwHazeFlag;
        _gr_polygon.dwColor.color = m_nColor;
		//_gr_polygon.nVertices = nCI;
		/*if( m_bRecalc ) {
			_gr_polygon.dwColor.color = nWhiteColor;
			_gr_polygon.dwFullType = GR_POLY_FLAT;
		}*/
		GRDrawPolygonPCCW();
	#endif
}
// --------------------------------------------------------------------------
void CViewFlatGouraudFigure::Hue(double fModify)
{
	ASSERT(0); (void)fModify;
	//m_nColor = (byte)paletteTranslator.Hue(m_nColor,fModify);
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
void CViewFlatTransparentFigure::_Read(CTaggedFile &f)
{
	dword dwColor;
	f.ReadDWord(dwColor);
	//m_nColor = paletteTranslator(dwColor);
    SRGB &pal = paletteTranslator.LocalPalette()[dwColor];
    m_nColor = GRTransparentColor(pal.r,pal.g,pal.b);
	f.SetRTEFatal(FALSE);
	double fOpacity = 0.5;
	f.ReadDouble(fOpacity);
	m_nOpacity = MinMax(Round(fOpacity*255.),0,255);
	f.SetOK(TRUE);
}
// --------------------------------------------------------------------------
void CViewFlatTransparentFigure::Draw()
{
	if( !IsVisibleFront() ) return;
	#if _USE_OPENGL
		glPushAttrib(GL_COLOR_BUFFER_BIT);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_POLYGON);
			glColor4ub(_gr_glPal[m_nColor].r,_gr_glPal[m_nColor].g,_gr_glPal[m_nColor].b,128);
			for( int i = 0 ; i < m_nVertices ; i++ )
				glVertex3d(m_ppVertices[i]->x,m_ppVertices[i]->y,m_ppVertices[i]->z);
		glEnd();
		glPopAttrib();
	#else
		//if( (_gr_polygon.nVertices = FormVertImages()) < 3 ) return;
		if( FormVertImages(FALSE,NULL,FALSE,NULL) < 3 )	return;
		_gr_polygon.dwFullType = GR_POLY_TRANSPARENT; // | m_dwHazeFlag; //GR_POLY_ADD_HAZE;
		_gr_polygon.dwAddType = m_dwHazeFlag;
        _gr_polygon.dwColor.color = m_nColor;
		_gr_polygon.dwOpacity = m_nOpacity;
		GRDrawPolygonPCCW();
	#endif
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
dword CViewFlatTransparentFlashFigure::m_nColor;
dword CViewFlatTransparentFlashFigure::m_nOpacity;
void CViewFlatTransparentFlashFigure::Draw()
{
	if( !IsVisibleFront() ) return;
	#if _USE_OPENGL
		glPushAttrib(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBegin(GL_POLYGON);
			glColor4ub(_gr_glPal[m_nColor].r,_gr_glPal[m_nColor].g,_gr_glPal[m_nColor].b,128);
			for( int i = 0 ; i < m_nVertices ; i++ )
				glVertex3d(m_ppVertices[i]->x,m_ppVertices[i]->y,m_ppVertices[i]->z);
		glEnd();
		glPopAttrib();
	#else
		//if( (_gr_polygon.nVertices = FormVertImages()) < 3 ) return;
		if( FormVertImages(FALSE,NULL,FALSE,NULL) < 3 )	return;
		_gr_polygon.dwFullType = GR_POLY_TRANSPARENT; // | m_dwHazeFlag;
		_gr_polygon.dwAddType = m_dwHazeFlag;
        _gr_polygon.dwColor.color = m_nColor;
		_gr_polygon.dwOpacity = m_nOpacity;
		GRDrawPolygonPCCW();
	#endif
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
dword CViewTexture::m_dwDefaultInfoFlags = 0;
CViewTexture::~CViewTexture()
{
	//if(m_pImageCache) delete [] (m_pImageCache-m_nHeight);
	//if(m_pImage) delete [] (m_pImage-SIZE_IMAGEINFO);
    if( m_hImage ) GRDeleteTextureFromDB(m_hImage);
	#if 0 //_USE_OPENGL
		glDeleteLists(m_nCallList,1);
	#endif
	#ifdef __sgi
		glDeleteTexturesEXT(1,&m_nBindIndex);
	#endif
}

extern SDeviceList _dL;
int __loadTextures = 1;
void CViewTexture::ReadImage(CTaggedFile &f,CPaletteTranslator &palTrans,void (*pCallBack)(byte *pImage,SRGB *pPal),GR_HTEXTURE hOld)
{
	int  nWidth = m_nWidth, nHeight = m_nHeight;
    if (_dL.currDevice->swHw == GR_HARDWARE) {
		RTCHECK( m_nWidth <= _dL.currDevice->textureW && m_nHeight <= _dL.currDevice->textureH, "Texture dims too large" );
		#if 1 //_USE_GLIDE || _USE_OPENGL
			m_nWidth = 1; while( m_nWidth < nWidth ) m_nWidth <<= 1;
			m_nHeight = 1; while( m_nHeight < nHeight ) m_nHeight <<= 1;
		#endif
	}
	int  nDims = m_nWidth*m_nHeight;
	//m_pImage = new byte[nDims+SIZE_IMAGEINFO]+SIZE_IMAGEINFO;
	int  bytesPixel = m_dwFlags&TXR_INDEX16 ? 2 : 1;
    int  flags;
    byte *pImage = new byte[nDims*bytesPixel+SIZE_IMAGEINFO]+SIZE_IMAGEINFO;

	if (bytesPixel == 2)  flags = TEXTURE_TXR_FORMAT;
    else flags = TEXTURE_PAL_FORMAT;
    if(m_dwFlags & TXR_SPRITE) flags |= TEXTURE_SPRITE;
	if(m_dwFlags & TXR_ALPHA) flags |= TEXTURE_ALPHA;

	#if 1 //_USE_GLIDE
		((dword*)pImage)[-1] = m_nWidth;
		((dword*)pImage)[-2] = m_nHeight;
		((dword*)pImage)[-3] = flags; //m_dwDefaultInfoFlags;
	#endif

	//if(!(m_dwFlags&TXR_INDEX16)) {
	if( nWidth == m_nWidth )
        f.Read(pImage,nWidth*nHeight*bytesPixel);
	else
	for( int i = 0 ; i < nHeight ; i++ )
        f.Read(pImage+m_nWidth*i*bytesPixel,nWidth*bytesPixel);

	if( pCallBack )	pCallBack(pImage,palTrans.LocalPalette());

	m_hImage = GRLoadTextureToDB(hOld, (byte*)palTrans.LocalPalette(), palTrans.NumEntries(), pImage);
	RTCHECK(m_hImage,"Cannot load texture");

	delete [] (pImage-SIZE_IMAGEINFO);

}
void CViewTexture::Read(CTaggedFile &f,bool bTagged,void (*pCallBack)(byte *pImage,SRGB *pPal),GR_HTEXTURE hOld)
{
	if( bTagged ) {
		CPaletteTranslator	palTrans;
		f.Descend("TXR_",FALSE,0);
			f.Descend("TXRH",TRUE,0);
				f.ReadDWord(m_dwFlags);
				//m_dwFlags |= dwExtraFlags;
				//RTCHECK(!(m_dwFlags&TXR_INDEX16)||paletteTranslator.IsMatchEnabled(),"Idx16 Txr when match off");
				f.ReadInt(m_nWidth);
				f.ReadInt(m_nHeight);
				ASSERT(m_nWidth>=2 && m_nHeight>=2);
				//int  nDims = m_nWidth*m_nHeight;
				f.SetRTEFatal(FALSE);
				int nTransparentColor = 0;
				f.ReadInt(nTransparentColor);
				f.SetOK(TRUE);
			f.Ascend();
			palTrans.EnableMatch( paletteTranslator.IsMatchEnabled() );
			m_nTransparentColor = (byte)palTrans.Define(f,(m_dwFlags&TXR_SPRITE)?nTransparentColor:-1);
			f.Descend("TXRD",TRUE,0);
				ReadImage(f,palTrans,pCallBack,hOld);
			f.Ascend();
		f.Ascend();
	}
	else {
		f.ReadDWord(m_dwFlags);
		//m_dwFlags |= dwExtraFlags;
		ASSERT(!(m_dwFlags&TXR_SPRITE));
		f.ReadInt(m_nWidth);
		f.ReadInt(m_nHeight);
		ReadImage(f,paletteTranslator,pCallBack,hOld);
	}
	//m_pImageCache = new int[m_nHeight*2+1]+m_nHeight;
	//for( int i = -m_nHeight ; i <= m_nHeight ; i++ ) m_pImageCache[i] = m_nWidth*i;
}

void CViewTexture::Read(TCchar *szName,bool bTagged,void (*pCallBack)(byte *pImage,SRGB *pPal),GR_HTEXTURE hOld)
{
	ASSERT(bTagged);	// Ilyha typitsa!
	CTaggedFile f(TRUE);
	f.Open(szName);
	Read(f,bTagged,pCallBack,hOld);
	f.Close();
	GRSetTextureLoadFunc( m_hImage, szName, Reload, this );
}

GR_HTEXTURE CViewTexture::Reload(TCchar *szName,GR_HTEXTURE hOld,void *pUser)
{
	ASSERT(hOld);
	CViewTexture	*pTexture = (CViewTexture*)pUser;
	pTexture->Read(szName,TRUE,NULL,hOld);
	return hOld;
}


int   CViewTexturedFigure::m_nFlatColor = -1;
double CViewTexturedFigure::m_fFlatDeltaRange = 0;
void CViewTexturedFigure::_Read(CTaggedFile &f)
{
	f.ReadDWord(m_dwFlags);
	#if _USE_OWNGR || _USE_GLIDE
		m_dwDrawType = (m_dwFlags & CViewTexture::TXR_SPRITE) ?
									GR_POLY_SPRITE_PERSP : GR_POLY_TEXTURE_PERSP;
	#endif
	CFMatrix3x4	inv;
	if( m_dwFlags & CViewTexture::TXR_HASMATRIX ) {
		inv.Read(f);
	}
	m_pTextVerts = new STextVert[m_nVertices+2]+1;
	for( int i = 0 ; i < m_nVertices ; i++ ) {
		CFVector2	v;
		v.Read(f);
		#if _USE_OPENGL
			m_pTextVerts[i].u = v.x;
			m_pTextVerts[i].v = v.y;
		#else
			m_pTextVerts[i].u = Round(v.x*65536);
			m_pTextVerts[i].v = Round(v.y*65536);
		#endif
	}
	_ReadTexture(f);

	RTCHECK( !(m_dwFlags & CViewTexture::TXR_SPRITE) ||
			(m_pTexture->m_dwFlags & CViewTexture::TXR_SPRITE), "Sprited figure must refer to sprited texture");
	#ifdef	_DEBUG
		if( m_dwFlags & CViewTexture::TXR_HASMATRIX )
		//#if _USE_OPENGL
			for( i = 0 ; i < m_nVertices ; i++ )
		//#else
		//	for( i = -1 ; i <= m_nVertices ; i++ )
		//#endif
		{
			#if _USE_OPENGL
				TCCFVector3	&v = inv * *m_ppVertices[i];
				ASSERT(fabs(v.z)<1e-3);
				ASSERT( fabs(m_pTextVerts[i].u-v.x) <= 1 &&
						fabs(m_pTextVerts[i].v-v.y) <= 1 );
			#else
				CFVector3	v = inv * CViewObjectBase::GetCurrBase()->GetVertex(m_ppVertImages[i] - CViewObject::m_paVertImages);
				ASSERT(abs(v.z)<1e-3);
				ASSERT( abs(m_pTextVerts[i].u-Round(v.x*65536)) <= 2 &&
						abs(m_pTextVerts[i].v-Round(v.y*65536)) <= 2 );
			#endif
		}
	#endif

	#if _USE_OWNGR
 		for( int i = 0 ; i < m_nVertices ; i++ ) {
			STextVert v = m_pTextVerts[i];
			m_pTextVerts[i].u = MinMax(v.u,2<<16,(m_pTexture->m_nWidth-2)<<16);
			m_pTextVerts[i].v = MinMax(v.v,2<<16,(m_pTexture->m_nHeight-2)<<16);
			if( m_pTextVerts[i].u != v.u || m_pTextVerts[i].v != v.v ) WRITELOG("\tTEXTURE BOUNDS EXCEEDED\n");
		}
	#endif

	#if _USE_OPENGL
 		for( int i = 0 ; i < m_nVertices ; i++ ) {
			m_pTextVerts[i].u /= m_pTexture->m_nWidth;
			m_pTextVerts[i].v /= m_pTexture->m_nHeight;
		}
	#endif
	#if _USE_GLIDE
 		for( int i = 0 ; i < m_nVertices ; i++ ) {
			m_pTextVerts[i].u = Round(m_pTextVerts[i].u*256./m_pTexture->m_nWidth);
			m_pTextVerts[i].v = Round(m_pTextVerts[i].v*256./m_pTexture->m_nHeight);
		}
	#endif

	m_pTextVerts[-1] = m_pTextVerts[m_nVertices-1];
	m_pTextVerts[m_nVertices] = m_pTextVerts[0];
}
// --------------------------------------------------------------------------
#if defined(__sgi)
	#define	_USE_TEXTURES 1
#endif
void CViewTexturedFigure::Draw()
{
	if( !IsVisibleFront() )	return;

	#if _USE_OPENGL
		#if _USE_TEXTURES
			glPushAttrib(GL_ENABLE_BIT);
			glEnable(GL_TEXTURE_2D);
			#ifdef __sgi
				glBindTextureEXT(GL_TEXTURE_2D,m_pTexture->m_nBindIndex);
				//glPushAttrib(GL_ENABLE_BIT);
				//glEnable(GL_TEXTURE_2D);
				//glTexImage2D(GL_TEXTURE_2D,0,3,m_pTexture->m_nWidth,m_pTexture->m_nHeight,0,
			 	//	GL_RGB,GL_UNSIGNED_BYTE,m_pTexture->m_pImage);
			#else
				//glTexImage2D(GL_TEXTURE_2D,0,3,m_pTexture->m_nWidth,m_pTexture->m_nHeight,0,
			 	//	GL_COLOR_INDEX,GL_UNSIGNED_BYTE,m_pTexture->m_pImage);
				glCallList(m_pTexture->m_nCallList);
			#endif
		#endif
		glBegin(GL_POLYGON);
			#if !_USE_TEXTURES
				byte nColor = m_pTexture->m_pImage[0];
				glColor3ub(_gr_glPal[nColor].r,_gr_glPal[nColor].g,_gr_glPal[nColor].b);
			#endif
			for( int i = 0 ; i < m_nVertices ; i++ ) {
				#if _USE_TEXTURES
					glTexCoord2d(m_pTextVerts[i].u,m_pTextVerts[i].v);
				#endif
				glVertex3d(m_ppVertices[i]->x,m_ppVertices[i]->y,m_ppVertices[i]->z);
			}
		glEnd();
		#if _USE_TEXTURES
			glPopAttrib();
		#endif
	#elif _USE_OWNGR || _USE_GLIDE
		#if 0
		for( int i = 0, nCI = 0 ; i < m_nVertices ; i++ ) {
			int cf;
			if( (cf=m_ppVertImages[i]->cf)!=0 ) {
				if( m_ppVertImages[i-1]->cf!=cf ) {
					const CViewObject::SInfVertImage *pI = m_ppInfVertImages[i*2];
					_gr_vertices[nCI].texture.x = pI->v.x;
					_gr_vertices[nCI].texture.y = pI->v.y;
					_gr_vertices[nCI].texture.iz = pI->v.z;
					_gr_vertices[nCI].texture.u =
						Mul_SHRD(m_pTextVerts[i-1].u,pI->nRatio,CViewObject::CLIP_SHIFT)+
						Mul_SHRD(m_pTextVerts[i].u,(CViewObject::CLIP_PREC-pI->nRatio),CViewObject::CLIP_SHIFT);
					_gr_vertices[nCI].texture.v =
						Mul_SHRD(m_pTextVerts[i-1].v,pI->nRatio,CViewObject::CLIP_SHIFT)+
						Mul_SHRD(m_pTextVerts[i].v,(CViewObject::CLIP_PREC-pI->nRatio),CViewObject::CLIP_SHIFT);
					nCI++;
				}
				if( m_ppVertImages[i+1]->cf!=cf ) {
					const CViewObject::SInfVertImage *pI = m_ppInfVertImages[i*2+1];
					_gr_vertices[nCI].texture.x = pI->v.x;
					_gr_vertices[nCI].texture.y = pI->v.y;
					_gr_vertices[nCI].texture.iz = pI->v.z;
					_gr_vertices[nCI].texture.u =
						Mul_SHRD(m_pTextVerts[i+1].u,pI->nRatio,CViewObject::CLIP_SHIFT)+
						Mul_SHRD(m_pTextVerts[i].u,(CViewObject::CLIP_PREC-pI->nRatio),CViewObject::CLIP_SHIFT);
					_gr_vertices[nCI].texture.v =
						Mul_SHRD(m_pTextVerts[i+1].v,pI->nRatio,CViewObject::CLIP_SHIFT)+
						Mul_SHRD(m_pTextVerts[i].v,(CViewObject::CLIP_PREC-pI->nRatio),CViewObject::CLIP_SHIFT);
					nCI++;
				}
			}
			else {
				_gr_vertices[nCI].texture.x = m_ppVertImages[i]->v.x;
				_gr_vertices[nCI].texture.y = m_ppVertImages[i]->v.y;
				_gr_vertices[nCI].texture.iz = m_ppVertImages[i]->v.z;
				_gr_vertices[nCI].texture.u = m_pTextVerts[i].u;
				_gr_vertices[nCI].texture.v = m_pTextVerts[i].v;
				nCI++;
			}
		}
		if( nCI < 3 ) return;
		#endif

		if( FormVertImages(FALSE,NULL,TRUE,m_pTextVerts) < 3 )	return;

	//	WRITELOG("DrawText: ");
	//	for( i = 0 ; i < nCI ; i++ ) WRITELOG2("(%X,%X) ",_gr_vertices[i].fill.texture.u,_gr_vertices[i].fill.texture.v);
	//	WRITELOG("\n");

		if( m_nFlatColor < 0 )
			_gr_polygon.dwFullType = m_dwDrawType; // | m_dwHazeFlag;
		else {
			_gr_polygon.dwFullType = GR_POLY_FLAT; // | m_dwHazeFlag;
			_gr_polygon.dwColor.color = MinMax(0,255,Round(Random(
										m_nFlatColor-m_fFlatDeltaRange,
										m_nFlatColor+m_fFlatDeltaRange)));
		}
		_gr_polygon.dwAddType = m_dwHazeFlag;
		_gr_polygon.hTexture = m_pTexture->m_hImage;
		//_gr_polygon.fill.texture.pTextureCache = m_pTexture->m_pImageCache;
		#if _USE_GLIDE
			_gr_polygon.dwColor.color = m_pTexture->m_nTransparentColor;
		#endif
		//_gr_polygon.nVertices = nCI;
		/*if( m_bRecalc ) {
			_gr_polygon.dwColor.color = nWhiteColor;
			_gr_polygon.dwFullType = GR_POLY_FLAT;
		}*/
		GRDrawPolygonPCCW();
	#else
		#error	Unknown target
	#endif
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
CViewLibTexturedFigure::SLibTexture	*CViewLibTexturedFigure::m_pLibTextures = 0;
int	CViewLibTexturedFigure::m_nLibTextures = 0;
int	CViewLibTexturedFigure::m_nMaxLibTextures = 0;
void CViewLibTexturedFigure::_ReadTexture(CTaggedFile &f)
{
	char	szName[9];
	f.Read(szName,9); szName[8]=0;
	strlwr(szName);
	//WRITELOG1("    Referencing texture '%s'\n",szName);
	for( int i = 0 ; i < m_nLibTextures ; i++ )
		if( strcmpi(m_pLibTextures[i].szName,szName) == 0 ) {
			m_pTexture = &m_pLibTextures[i].texture;
			return;
		}
	if( m_nMaxLibTextures >= 0 ) {
		RTCHECK1( m_nLibTextures < m_nMaxLibTextures, "TextureLib count > %d", m_nMaxLibTextures );
		char	szNameExt[15];
		sprintf(szNameExt,"%s.txr",szName);
		//CTaggedFile f1(TRUE);
		//f1.Open(szNameExt);
		strncpy(m_pLibTextures[m_nLibTextures].szName,szName,9);
		m_pLibTextures[m_nLibTextures].texture.Read(szNameExt); //(f1);
		//f1.Close();
		m_pTexture = &m_pLibTextures[m_nLibTextures++].texture;
	}
}

#if 0
void CViewLibTexturedFigure::LoadLib(CTaggedFile &f)
{
	ASSERT(0);	// Does not work since f$#&*$ing ilya asks for texture reloads
	ClearLib();
	m_nMaxLibTextures = -1;
	f.Descend("TXL_",FALSE,0);
		f.Descend("TXLH",TRUE,0);
			f.ReadInt(m_nLibTextures);
			if( m_nLibTextures > 0 ) m_pLibTextures = new SLibTexture[m_nLibTextures];
			for( int i = 0 ; i < m_nLibTextures ; i++ )
				f.Read(m_pLibTextures[i].szName,9);
		f.Ascend();
		for( i = 0 ; i < m_nLibTextures ; i++ )
			m_pLibTextures[i].texture.Read(f);
	f.Ascend();
}
#endif

void CViewLibTexturedFigure::LoadLib(TCchar *szName)
{
	if(!szName) {
		ClearLib();
		m_pLibTextures = new SLibTexture[m_nMaxLibTextures = 100];
		return;
	}

	ASSERT(0);	// Does not work since f$#&*$ing ilya asks for texture reloads
	#if 0
	CTaggedFile f(TRUE);
	f.Open(szName);
	LoadLib(f);
	f.Close();
	#endif
}
void CViewLibTexturedFigure::ClearLib()
{
	delete	[] m_pLibTextures; m_pLibTextures = NULL; m_nLibTextures = 0;
}

void CViewOwnTexturedFigure::_ReadTexture(CTaggedFile &f)
{
	//WRITELOG("    Reading built-in texture\n");
	ASSERT(0);	// Does not work since f$#&*$ing ilya asks for texture reloads
	(void)f;
	#if 0
	m_texture.Read(f,FALSE);
	#endif
}

// --------------------------------------------------------------------------
