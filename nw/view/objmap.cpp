#ifndef	__VIEW_CPP__
#include"_view.h"
#endif

#if _USE_OPENGL
void BeginDrawFromLand()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrix(CViewObject::m_viewPointDirMx);

	glPushAttrib(GL_COLOR_BUFFER_BIT|GL_LIGHTING_BIT|GL_TEXTURE_BIT|GL_ENABLE_BIT);
	glBlendFunc(GL_ONE,GL_ZERO);
	glShadeModel(GL_FLAT);
	glDisable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glDisable(GL_CULL_FACE);
}

void EndDrawFromLand()
{
	glPopMatrix();
	glPopAttrib();
}
#endif

#if 0
void glDrawFromLand(CViewOrdered *pOrd)
{
	glBeginDrawFromLand();
	pOrd->Draw();
	glEndDrawFromLand();
}

inline void glDrawFromLand(CLandObjectMap1::CSecondaryEntry *pEnt,bool &bInitGL)
{
	if(!pEnt) return;
	ASSERT(pEnt->pObj);
	if(!bInitGL) { glBeginDrawFromLand(); bInitGL = TRUE; }
	pEnt->pObj->Draw();
}
#endif




void CLandDynamicMap::SetupDynamics(CViewDynamicList &list,TCSRect2 &rect,_CViewTerrain &terrain)
{
	for( CViewDynamic *pd = list.First(), *p1 ; pd != NULL ; pd = p1 ) {
		ASSERT( pd->IsBumpDynamic() );
		p1 = pd->Next();
		//CVector2  cell( floor( ((CViewSphericDynamic*)pd)->m_dynBase.x/m_fCellSize ),
		//					floor(-((CViewSphericDynamic*)pd)->m_dynBase.z/m_fCellSize ));
		CFVector2  cell( ((CViewSphericDynamic*)pd)->m_dynBase.x*m_f_1_CellSize,
							-((CViewSphericDynamic*)pd)->m_dynBase.z*m_f_1_CellSize);
		CVector2	cellL( cell.x, cell.y );
		if( cellL.x >= m_viewpoint.x )
			cellL.x = Max( (long)floor(cell.x-((CViewBumpDynamic*)pd)->m_bump.fRadius*m_f_1_CellSize), m_viewpoint.x );
		else
			cellL.x = Min( (long)floor(cell.x+((CViewBumpDynamic*)pd)->m_bump.fRadius*m_f_1_CellSize), m_viewpoint.x );
								//ceil
		if( cellL.y >= m_viewpoint.y )
			cellL.y = Max( (long)floor(cell.y-((CViewBumpDynamic*)pd)->m_bump.fRadius*m_f_1_CellSize), m_viewpoint.y );
		else
			cellL.y = Min( (long)floor(cell.y+((CViewBumpDynamic*)pd)->m_bump.fRadius*m_f_1_CellSize), m_viewpoint.y );
								//ceil
		cellL.x = MinMax(cellL.x,rect.left,rect.right);
		cellL.y = MinMax(cellL.y,rect.top,rect.bottom);

		//static	SRect2	terrainRect = { 0,0,512,512 };

		if( !terrain.FitInTrapezioid(cellL) ) {
			m_afterList.Move(pd,list);
			pd->m_cell = CVector2(m_rect.left-1, 0); // optimizing m_rect.Contains(cell) in removal
		} else
		if( !rect.Contains(cellL) ) {
			pd->m_cell = CVector2(m_rect.left-1, 0); // optimizing m_rect.Contains(cell) in removal
			// leave remaining in list to be drawn after the land rect
		} else
		if( m_rect.Contains(cellL) ) {
			m_pOrigin[cellL.x+cellL.y*m_size.x].Move(pd,list);
			pd->m_cell = cellL;
		}
		else {
			list.Remove(pd);
			pd->m_cell = CVector2(m_rect.left-1, 0); // optimizing m_rect.Contains(cell) in removal
		}
	}
}

void CLandDynamicMap::SetupLandSticks(CViewDynamicList &list,_CViewTerrain &terrain)
{
	for( CViewDynamic *pd = list.First(), *p1 ; pd != NULL ; pd = p1 ) {
		p1 = pd->Next();
		if( pd->IsBumpDynamic() ) continue;
		CVector2	cellL = pd->m_cell;

		//static	SRect2	terrainRect = { 0,0,512,512 };

		if( !terrain.FitInTrapezioid(cellL) || !m_rect.Contains(cellL) ) {
			list.Remove(pd);
		}
		else {
			m_pOrigin[cellL.x+cellL.y*m_size.x].Move(pd,list);
			pd->m_cell = cellL;
		}
	}
}

void CLandDynamicMap::SetOrigin(TCSFVector3 &origin)
{
	CVector2	originCell;
	D2INT((int*)&originCell.x,origin.x*m_f_1_CellSize);
	D2INT((int*)&originCell.y,-origin.z*m_f_1_CellSize);
	SetOrigin(originCell);
}

bool CLandDynamicMap::IsEmpty()
{
	for( int y = m_rect.top ; y < m_rect.bottom ; y++ )
	for( int x = m_rect.left ; x < m_rect.right ; x++ )
		if( m_pOriginLight[x+y*m_size.x] ) return FALSE;
	return TRUE;
}

void CLandDynamicMap::SetupLights(dword dwLights)
{
	for( dword i = 0, dwMask = 1 ; dwLights ; dwLights >>= 1, dwMask <<= 1, i++ )
	if( dwLights & 1 ) {
		SRect2	cell;
		cell.DoSetIntersect(m_rect,CViewObject::m_aGlobalLightsRect[i]);
		for( int y = cell.top ; y < cell.bottom ; y++ )
		for( int x = cell.left ; x < cell.right ; x++ )
			m_pOriginLight[x+y*m_size.x] |= dwMask;
	}
}

void CLandDynamicMap::RemoveLights(dword dwLights)
{
	for( dword i = 0, dwMask = 1 ; dwLights ; dwLights >>= 1, dwMask <<= 1, i++ )
	if( dwLights & 1 ) {
		//SRect2	&cell = CViewObject::m_aGlobalLightsRect[i];
		SRect2	cell;
		cell.DoSetIntersect(m_rect,CViewObject::m_aGlobalLightsRect[i]);
		for( int y = cell.top ; y < cell.bottom ; y++ )
		for( int x = cell.left ; x < cell.right ; x++ )
			m_pOriginLight[x+y*m_size.x] = 0;
	}
}

void CLandDynamicMap::ApplyLights(CViewDynamic *pd)
{
	if( !pd->IsBumpDynamic() ) {
		if( m_rect.Contains(pd->m_cell) )
			pd->m_dwLights = m_pOriginLight[pd->m_cell.x+pd->m_cell.y*m_size.x];
		return;
	}
	SRect2	celld;
	D2INT((int*)&celld.left,( ((CViewSphericDynamic*)pd)->m_dynBase.x-((CViewBumpDynamic*)pd)->m_bump.fRadius)*m_f_1_CellSize-0.5);
	D2INT((int*)&celld.top,(-((CViewSphericDynamic*)pd)->m_dynBase.z-((CViewBumpDynamic*)pd)->m_bump.fRadius)*m_f_1_CellSize-0.5);
	D2INT((int*)&celld.right,( ((CViewSphericDynamic*)pd)->m_dynBase.x+((CViewBumpDynamic*)pd)->m_bump.fRadius)*m_f_1_CellSize+0.5);
	D2INT((int*)&celld.bottom,(-((CViewSphericDynamic*)pd)->m_dynBase.z+((CViewBumpDynamic*)pd)->m_bump.fRadius)*m_f_1_CellSize+0.5);
	celld.DoSetIntersect(celld,m_rect);
	pd->m_dwLights = 0;
	for(;celld.top<celld.bottom;celld.top++)
	for(;celld.left<celld.right;celld.left++)
		pd->m_dwLights |= m_pOriginLight[celld.left+celld.top*m_size.x];
	pd->m_dwLights &= pd->m_dwLightMask;
}



//int CLandObjectMap1::m_nDynamicsOrigin;
//#if _USE_OPENGL
bool CLandObjectMap1::m_bInitDrawObj;
//#endif

#if 1
CLandObjectMap1::CSecondaryEntry  *
							CLandObjectMap1::CPrimaryEntry::___Find(int x)
{
	for( int l = 0, u = m_n-1; l <= u ; ) {
		int m = (u+l)/2;
		CLandObjectMap1::CSecondaryEntry *pSE = &m_pSE0[m];
		if( x == pSE->x ) return pSE;
		if( x > pSE->x ) l = m+1; else u=m-1;
	}
	return NULL;
}
#endif

inline CLandObjectMap1::CSecondaryEntry  *
							CLandObjectMap1::CPrimaryEntry::_Find(int x)
{
	ASSERT(!m_pSE0 || m_x == m_pSE->x);
	if( x < m_xmin || x > m_xmax ) return NULL;
	CLandObjectMap1::CSecondaryEntry *pSE_0 = m_pSE;
	int dx = abs(x-m_xreq); m_xreq = x;
	if( /*dx < 5*/ dx < 20 && (1<<dx) < m_n ) {
		if( x < m_x ) {
			while( x < m_x )	m_x = (--m_pSE)->x;
			if( x == m_x ) { ASSERT(___Find(x)); return m_pSE; }
		}
		else {
			while( x > m_x )	m_x = (++m_pSE)->x;
			if( x == m_x ) { ASSERT(___Find(x)); return m_pSE; }
		}
		return NULL;
	}
	for( int l = 0, u = m_n-1; l <= u ; ) {
		int m = (u+l)/2;
		m_pSE = &m_pSE0[m];
		if( x == m_pSE->x ) { m_x = x; ASSERT(___Find(x)); return m_pSE; }
		if( x > m_pSE->x ) l = m+1; else u=m-1;
	}
	m_x = m_pSE->x;
	ASSERT(!___Find(x));
	return NULL;
}

CLandObjectMap1::CSecondaryEntry  *
							CLandObjectMap1::CPrimaryEntry::Find(int x)
{
	return _Find(x);
}

CLandObjectMap1::CLandObjectMap1(int xplow,int xphigh)
{
	m_bIsCopy = FALSE;
	m_xplow = xplow;
	m_xphigh = xphigh;
	m_pPrimaries = new CPrimaryEntry[xphigh-xplow];
	for( int i = xplow ; i < xphigh ; i++ ) {
		CPrimaryEntry	&prim = m_pPrimaries[i-xplow];
		prim.m_n = 0;
		prim.m_pSE = prim.m_pSE0 = NULL;
		prim.m_xreq = 0;
		prim.m_xmin = 1024*1024; prim.m_xmax = -1024*1024;
		prim.m_x = 0;
	}
}

CLandObjectMap1::~CLandObjectMap1()
{
	for( int i = m_xplow ; i < m_xphigh ; i++ ) {
		CPrimaryEntry &prim = m_pPrimaries[i-m_xplow];
		if( !m_bIsCopy )
			for( int j = 0 ; j < prim.m_n ; j++ ) delete prim.m_pSE0[j].pObj;
		delete [] prim.m_pSE0;
	}
	delete [] m_pPrimaries;
}

void	CLandObjectMap1::Read(CTaggedFile &f,int xplow,int xphigh,
								CViewOrdered *(*ReadOrder)(CTaggedFile &f),
								CLandObjectMap1 *pSrc)
{
	m_bIsCopy = pSrc != NULL;
	f.Descend("MAP1",FALSE,0);
		m_xplow = xplow;
		m_xphigh = xphigh;
		m_pPrimaries = new CPrimaryEntry[xphigh-xplow];
		for( int i = xplow ; i < xphigh ; i++ ) {
			CPrimaryEntry	&prim = m_pPrimaries[i-xplow];
			f.Descend("M1PE",FALSE,0);
				f.Descend("M1PH",TRUE,0);
					f.ReadInt(prim.m_n);
					prim.m_pSE = prim.m_pSE0 = prim.m_n > 0 ?
									new CSecondaryEntry[prim.m_n] : NULL;
					prim.m_xreq = 0;
					prim.m_xmin = 1024*1024; prim.m_xmax = -1024*1024;
				f.Ascend();
				for( int j = 0 ; j < prim.m_n ; j++ ) {
					int x;
					f.Descend("M1SE",FALSE,0);
						f.Descend("M1SH",TRUE,0);
							f.ReadInt(x); prim.m_pSE0[j].x = x;
							if( x < prim.m_xmin ) prim.m_xmin = x;
							if( x > prim.m_xmax ) prim.m_xmax = x;
						f.Ascend();
						CViewOrdered *pObj = m_bIsCopy ?
									pSrc->Find(x,i)->Obj() : ReadOrder(f);
						RTCHECK(pObj,"ObjMap obj not read");
						prim.m_pSE0[j].pObj = pObj;
					f.Ascend();
				}
				prim.m_x = prim.m_pSE ? prim.m_pSE->x : 0;
			f.Ascend(); // M1PE
		} // for( i
	f.Ascend(); // MAP1
}

void CLandObjectMap1::ApplyModelUsage()
{
	for( int i = m_xplow ; i < m_xphigh ; i++ ) {
		CPrimaryEntry	&prim = m_pPrimaries[i-m_xplow];
		for( int j = 0 ; j < prim.m_n ; j++ )
			prim.m_pSE0[j].pObj->ApplyModelUsage();
	}
}

inline void CLandObjectMap1::DrawReductionSec(CPrimaryEntry &prim,int xp,int xs,WATER_VIEW eWaterView)
{
	dword	dwLights;
	CViewDynamicListLoop *dyn = FindDyn(xp,xs,dwLights);
	prim._Find(xs)->Draw( dyn, eWaterView, dwLights );
	#if 0
	CSecondaryEntry *pSec=prim._Find(xs);
	if(pSec) {
		//CViewDynamicListLoop &dyn = m_pDynamic[
		//	xp*m_nDynamicsXPStep+xs*m_nDynamicsXSStep+m_nDynamicsOrigin];
		//if( pDynamics->First() ) pSec->m_pObj->LoadDyns(dyn);
		(void)xp;
		//#if _USE_OPENGL
			if( !m_bInitDrawObj ) { BeginDrawFromLand(); m_bInitDrawObj = TRUE; }
		//#endif
		pSec->pObj->Draw();
	}
	#endif
}

inline void	CLandObjectMap1::DrawReductionPrim(CLandObjectMap1::CPrimaryEntry &prim,
													int xp,TCSRect2 &cellSP,WATER_VIEW eWaterView)
{
	int	xs = cellSP.left;
	int xs1 = cellSP.right-1;
	//int xs1 = xs+m_nReductionAdd;
	//#if	_USE_OPENGL
		m_bInitDrawObj = FALSE;
	//#endif
	if( xs >= m_viewxs ) {	// xs1--
		do DrawReductionSec(prim,xp,xs1,eWaterView); while( xs <= --xs1 );
	} else
	if( xs1 <= m_viewxs ) {	// xs++
		do DrawReductionSec(prim,xp,xs,eWaterView); while( ++xs <= xs1 );
	}
	else {	// xs < m_viewxs < xs1; xs++, xs1--
		do DrawReductionSec(prim,xp,xs,eWaterView); while(++xs<m_viewxs);
		do DrawReductionSec(prim,xp,xs1,eWaterView); while(--xs1>=m_viewxs);
	}
	//#if _USE_OPENGL
		if( m_bInitDrawObj )	EndDrawFromLand();
	//#endif
}

void	CLandObjectMap1::DrawReduction(TCSRect2 &cellSP,WATER_VIEW eWaterView)
{
	//ASSERT(xp>=m_xplow&&xp+m_nReductionAdd<m_xphigh);
	int xp = cellSP.top - m_xplow;
	//xp -= m_xplow;
	int xp1 = cellSP.bottom-1 - m_xplow; //xp+m_nReductionAdd;

	if( xp < 0 ) xp = 0;
	if( xp1 >= m_xphigh-m_xplow )	xp1 = m_xphigh-m_xplow-1;
	if( xp > xp1 ) return;

	if( xp >= m_viewxp ) {
		do DrawReductionPrim(m_pPrimaries[xp1],xp1+m_xplow,cellSP,eWaterView); while(xp<=--xp1);
	} else
	if( xp1 <= m_viewxp ) {
		do DrawReductionPrim(m_pPrimaries[xp],xp+m_xplow,cellSP,eWaterView); while(++xp<=xp1);
	}
	else { // xp < m_viewxp < xp1
		do DrawReductionPrim(m_pPrimaries[xp],xp+m_xplow,cellSP,eWaterView); while(++xp<m_viewxp);
		do DrawReductionPrim(m_pPrimaries[xp1],xp1+m_xplow,cellSP,eWaterView); while(--xp1>=m_viewxp);
	}
}

void	CLandObjectMap2::Read(CTaggedFile &f,TCCRect2 &rect,
								CViewOrdered *(*ReadOrder)(CTaggedFile &f))
{
	f.Descend("MAP2",FALSE,0);
		m_mapX.Read(f,rect.left,rect.right,ReadOrder);
		m_mapY.Read(f,rect.top,rect.bottom,ReadOrder,&m_mapX);
	f.Ascend(); // MAP1
}
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
bool SolveDynamicSq(double a,double b_2,double c,double &t);
bool CLandscapeRect::_BumpLandCellAdvanced(SBumpDef &def,int nLandX,int nLandY)
{
	int	nLandXIdx = nLandX&m_nAxisMask,
		nLandX1Idx = (nLandX+1)&m_nAxisMask,
		nLandYIdx = (nLandY&m_nAxisMask)<<m_nLandSizeShift,
		nLandY1Idx = ((nLandY+1)&m_nAxisMask)<<m_nLandSizeShift;

	// 3-------2
	// | U   / |
	// |   /   |
	// | /  L  |
	// 0-------1

	int	y0 = m_pHeights[nLandYIdx+nLandXIdx]&HEIGHT_MASK,
		y1 = m_pHeights[nLandYIdx+nLandX1Idx]&HEIGHT_MASK,
		y2 = m_pHeights[nLandY1Idx+nLandX1Idx]&HEIGHT_MASK,
		y3 = m_pHeights[nLandY1Idx+nLandXIdx]&HEIGHT_MASK;

	double	h0 = y0*m_fScaleHeight,
			h1 = y1*m_fScaleHeight,
			h2 = y2*m_fScaleHeight,
			h3 = y3*m_fScaleHeight;
	CFVector3	nL = CFVector3(h0-h1,m_fCellSize,h2-h1);
	CFVector3	nU = CFVector3(h3-h2,m_fCellSize,h3-h0);
	nL *= 1./Abs(nL);
	nU *= 1./Abs(nU);
	double	x = nLandX*m_fCellSize, x1 = x+m_fCellSize;
	double	z = -nLandY*m_fCellSize, z1 = z-m_fCellSize;
	CFVector3	v0 = CFVector3(x,h0,z),
				v1 = CFVector3(x1,h1,z),
				v2 = CFVector3(x1,h2,z1),
				v3 = CFVector3(x,h3,z1);

	bool	bRes = FALSE;
	bool	bEdgesL = FALSE, bEdgesU = FALSE;


	int  i;
	double	tl, tr;

	// ---------------------------- Lower
	double	fa = def.vel*nL;
	if( fa >= 0 ) goto BumpU;	// does not bump this plane

	tl = 0, tr = def.fTime;
	for( i = 0 ; i < 3 ; i++ ) {
		CFVector3	*pt1, *pt2;
		switch(i) {
			case 0: pt1 = &v0; pt2 = &v1; break;
			case 1: pt1 = &v1; pt2 = &v2; break;
			case 2: pt1 = &v2; pt2 = &v0; break;
			default: ASSERT(0);
		}
		CFVector3	l = *pt2-*pt1;
		CFVector3	n = l%nL;
		double	a = def.vel*n,
				b = (def.start-*pt1)*n;
		if( a >= 0 ) {
			if( b < 0 )
				tr = Min(tr,-b/Max(a,1e-10));
			else
				{ bEdgesL = TRUE; goto BumpU; }
		}
		else {
			if( b >= 0 )
				tl = Max(tl,-b/Min(a,-1e-10));
		}
		if( tl >= tr ) { bEdgesL = TRUE; goto BumpU; }
	}

	{
		double	fb = (def.start-CFVector3(x,h0,z))*nL + fa*tl;
		if( fb < def.fRadius ) bEdgesL = TRUE;

		tl += Max(0.,(def.fRadius-fb)/Min(fa,-1e-10));
		if( tl < tr ) {
			def.fTime = tl;
			def.vel1 = def.vel-nL*((def.vel*nL)*2);
			bRes = TRUE;
			goto	BumpU;	// ignore edges
		}
		bEdgesL = TRUE;
	}


	// ---------------------------- Upper

BumpU:
	fa = def.vel*nU;
	if( fa >= 0 ) goto BumpEdges;	// does not bump this plane

	tl = 0, tr = def.fTime;
	for( i = 0 ; i < 3 ; i++ ) {
		CFVector3	*pt1, *pt2;
		switch(i) {
			case 0: pt1 = &v2; pt2 = &v3; break;
			case 1: pt1 = &v3; pt2 = &v0; break;
			case 2: pt1 = &v0; pt2 = &v2; break;
			default: ASSERT(0);
		}
		CFVector3	l = *pt2-*pt1;
		CFVector3	n = l%nU;
		double	a = def.vel*n,
				b = (def.start-*pt1)*n;
		if( a >= 0 ) {
			if( b < 0 )
				tr = Min(tr,-b/Max(a,1e-10));
			else
				{ bEdgesU = TRUE; goto BumpEdges; }
		}
		else {
			if( b >= 0 )
				tl = Max(tl,-b/Min(a,-1e-10));
		}
		if( tl >= tr ) { bEdgesU = TRUE; goto BumpEdges; }
	}

	{
		double fb = (def.start-CFVector3(x,h0,z))*nU + fa*tl;
		if( fb < def.fRadius ) bEdgesU = TRUE;

		tl += Max(0.,(def.fRadius-fb)/Min(fa,-1e-10));
		if( tl > 0 ) bEdgesU = TRUE;
		if( tl < tr ) {
			def.fTime = tl;
			def.vel1 = def.vel-nU*((def.vel*nU)*2);
			bRes = TRUE;
			goto	BumpEdges;	// ignore edges (for this plane, may still bump them for lower one)
		}
		bEdgesU = TRUE;
	}

	// ------------------ Edges

BumpEdges:
	double	RR = def.fRadius *def.fRadius;
	if( bEdgesL || bEdgesU ) // don't remove or add condition for case 4
	for( i = 0 ; i < 5 ; i++ ) {
		//  --- 2 ---
		// |  U    / |
		// 3    4    1
		// | /    L  |
		//  --- 0 ---
		CFVector3	*pt1,  *pt2;
		switch(i) {
			case 0: if(!bEdgesL) continue; pt1 = &v0; pt2 = &v1; break;
			case 1: if(!bEdgesL) continue; pt1 = &v1; pt2 = &v2; break;
			case 2: if(!bEdgesU) continue; pt1 = &v2; pt2 = &v3; break;
			case 3: if(!bEdgesU) continue; pt1 = &v3; pt2 = &v0; break;
			case 4: pt1 = &v0; pt2 = &v2; break;
		}
		CFVector3	n = (*pt2-*pt1);
		double		L = Abs(n);
		if( L < 1e-5 ) continue;
		n *= (1./L);

		CFVector3	s = def.start-*pt1;
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
		}
	}


	// 3-------2
	// | U   / |
	// |   /   |
	// | /  L  |
	// 0-------1

	if( bEdgesL || bEdgesU )
	for( i = 0 ; i < 4 ; i++ ) {

		CFVector3	*pt;
		switch(i) {
			case 0: pt = &v0; break;
			case 1: if(!bEdgesL) continue; pt = &v1; break;
			case 2: pt = &v2; break;
			case 3: if(!bEdgesU) continue; pt = &v3; break;
		}

		CFVector3	s = def.start-*pt;

		if( SolveDynamicSq(def.vel*def.vel,def.vel*s,s*s-RR,def.fTime) ) {
			s += def.vel*def.fTime;
			ASSERT( Abs(s) > 1e-4 );
			s *= 1/Abs(s);
			def.vel1 = def.vel-s*((def.vel*s)*2);
			//def.vel1 = -def.vel;
			bRes = TRUE;
		}
	}

	if( bRes )
		def.nBumpFlags = BF_BUMPLAND;
	return	bRes;
}

#if 1
inline bool CLandscapeRect::_BumpLandCell(SBumpDef &def,int nLandX,int nLandY)
{
	if( def.dwFlags1 & BF1_ADVANCED )
		return _BumpLandCellAdvanced(def,nLandX,nLandY);

	bool bRes = FALSE;
	// z2        /|6~~~/|7         H0' = y0+n034*R
	//         /  |  /  |          H1' = y1+n120*R
	//       /____|/____|          H2' = y2+n273*R
	// z1  5|    /|3   /|2         H3' = y3+n365*R
	//      |  /  |U / L|          nL = N012'
	//  z  4|/___0|/____|1         nU = N023'
	//      x_    x     x1         (landY=-z)
	int	nLandXIdx = nLandX&m_nAxisMask,
		nLandX1Idx = (nLandX+1)&m_nAxisMask,
 		nLandX_Idx = (nLandX-1)&m_nAxisMask,
		nLandYIdx = (nLandY&m_nAxisMask)<<m_nLandSizeShift,
		nLandY1Idx = ((nLandY+1)&m_nAxisMask)<<m_nLandSizeShift,
		nLandY2Idx = ((nLandY+2)&m_nAxisMask)<<m_nLandSizeShift;
	int	y0 = m_pHeights[nLandYIdx+nLandXIdx]&HEIGHT_MASK,
		y1 = m_pHeights[nLandYIdx+nLandX1Idx]&HEIGHT_MASK,
		y2 = m_pHeights[nLandY1Idx+nLandX1Idx]&HEIGHT_MASK,
		y3 = m_pHeights[nLandY1Idx+nLandXIdx]&HEIGHT_MASK,
		y4 = m_pHeights[nLandYIdx+nLandX_Idx]&HEIGHT_MASK,
		y5 = m_pHeights[nLandY1Idx+nLandX_Idx]&HEIGHT_MASK,
		y6 = m_pHeights[nLandY2Idx+nLandXIdx]&HEIGHT_MASK,
		y7 = m_pHeights[nLandY2Idx+nLandX1Idx]&HEIGHT_MASK;

	CFVector3 n034=CFVector3(y4-y0,m_fCellSize0,y3-y0);
	CFVector3 n120=CFVector3(y0-y1,m_fCellSize0,y2-y1);
	CFVector3 n273=CFVector3(y3-y2,m_fCellSize0,y7-y2);
	CFVector3 n365=CFVector3(y5-y3,m_fCellSize0,y6-y3);
	double	h0 = y0*m_fScaleHeight+Abs(n034)*def.fRadius/n034.y,
			h1 = y1*m_fScaleHeight+Abs(n120)*def.fRadius/n120.y,
			h2 = y2*m_fScaleHeight+Abs(n273)*def.fRadius/n273.y,
			h3 = y3*m_fScaleHeight+Abs(n365)*def.fRadius/n365.y;
	CFVector3	nL = CFVector3(h0-h1,m_fCellSize,h2-h1);
	CFVector3	nU = CFVector3(h3-h2,m_fCellSize,h3-h0);

	//            /|                 0,0,-1
	//  -1,0,-1 /  | 1,0,0           |~~~~/
	//        /____|          -1,0,0 |  /  1,0,1
	//        0,0,1                  |/
	double	x = nLandX*m_fCellSize, x1 = x+m_fCellSize;
	double	z = -nLandY*m_fCellSize, z1 = z-m_fCellSize;

	// ---------------------------------------------------
	double	fTimeL, fTimeR, bL, a, b;
	int i0, iL, i; bool aLlt0;
	// nL
	fTimeL = 0; fTimeR = def.fTime;
	i0 = -1; iL = -1;
	for( i = 0 ; i < 4 ; i++ ) {
		switch(i) {
			case 0:	b = def.start.z-z; a = def.vel.z; break;
			case 1:	b = def.start.x-x1; a = def.vel.x; break;
			case 2: b = x-def.start.x+z-def.start.z; a = -def.vel.x-def.vel.z; break;
			default: b = (def.start-CFVector3(x,h0,z))*nL; a = def.vel*nL; break;
		}
		if( i0 < 0 ) {
			if( iL < 0 ) { iL = i; bL = b; aLlt0 = a < 0; }
			else if( b > bL ) { iL = i; bL = b; aLlt0 = a < 0; }
		}
		if( a >= 0 ) {	// adj fTimeR if necessary
			if( a*fTimeR+b > 0 )	{ // t0[i] < tR, do adj fTimeR
				if( b < 0 ) {	// calc min in case we've lost precision
					double t = -b/a;
					if( t < fTimeR ) {
						if( t <= fTimeL ) goto NoBumpL;
						fTimeR = t;
					}
				} else goto NoBumpL; // a >= 0, b >= 0 => Max >= 0
			}
		}
		else { // a < 0, adj fTimeL if necessary
			if( i == 3 || a*fTimeL+b > -def.fRadius )	{ // t0[i] > tL, do adj fTimeL
				if( a*fTimeR+b < 0 ) { // calc max in case we've lost precision
					double t = -b/AbsNoLess(a,1e-10);
					if( t > fTimeL ) {
						if( t >= fTimeR )	goto NoBumpL;
						fTimeL = t; i0 = i;
					} else if(i==3) i0=3;
				} else goto NoBumpL; // a*fTimeR+b >= 0 => Max >= 0
			}
		}
	}
	if( i0 >= 0 || (i0=iL) >= 0 && aLlt0 && bL <= 0 ) {
		ASSERT(fTimeL <= fTimeR && fTimeR <= def.fTime);
		double	s = def.vel*nL;
		if( s < 0 ) {
			def.vel1 = def.vel-nL*((def.vel*nL)*2/Abs2(nL));
			def.fTime = fTimeL;
			def.nBumpFlags = BF_BUMPLAND;
			//__pBumpBody = this;
			bRes = TRUE;
		}
	}
NoBumpL:
	// nU
	fTimeL = 0; fTimeR = def.fTime;
	i0 = -1; iL = -1;
	for( i = 0 ; i < 4 ; i++ ) {
		switch(i) {
			case 0:	b = x-def.start.x; a = -def.vel.x; break;
			case 1:	b = z1-def.start.z; a = -def.vel.z; break;
			case 2: b = def.start.z-z+def.start.x-x; a = def.vel.x+def.vel.z; break;
			default: b = (def.start-CFVector3(x,h0,z))*nU; a = def.vel*nU; break;
		}
		if( i0 < 0 ) {
			if( iL < 0 ) { iL = i; bL = b; aLlt0 = a < 0; }
			else if( b > bL ) { iL = i; bL = b; aLlt0 = a < 0; }
		}
		if( a >= 0 ) {	// adj fTimeR if necessary
			if( a*fTimeR+b > 0 )	{ // t0[i] < tR, do adj fTimeR
				if( b < 0 ) {	// calc min in case we've lost precision
					double t = -b/a;
					if( t < fTimeR ) {
						if( t <= fTimeL ) goto NoBumpR;
						fTimeR = t;
					}
				} else goto NoBumpR; // a >= 0, b >= 0 => Max >= 0
			}
		}
		else { // a < 0, adj fTimeL if necessary
			if( i == 3 || a*fTimeL+b > -def.fRadius )	{ // t0[i] > tL, do adj fTimeL
				if( a*fTimeR+b < 0 ) { // calc max in case we've lost precision
					double t = -b/AbsNoLess(a,1e-10);
					if( t > fTimeL ) {
						if( t >= fTimeR )	goto NoBumpR;
						fTimeL = t; i0 = i;
					} else if(i==3) i0=3;
				} else goto NoBumpR; // a*fTimeR+b >= 0 => Max >= 0
			}
		}
	}
	if( i0 >= 0 || (i0=iL) >= 0 && aLlt0 && bL <= 0 ) {
		ASSERT(fTimeL <= fTimeR && fTimeR <= def.fTime);
		double s = def.vel*nU;
		if( s < 0 ) {
			def.vel1 = def.vel-nU*((def.vel*nU)*2/Abs2(nU));
			def.fTime = fTimeL;
			def.nBumpFlags = BF_BUMPLAND;
			//__pBumpBody = this;
			bRes = TRUE;
		}
	}
NoBumpR:
	return bRes;
}
#else
inline bool CLandscapeRect::_BumpLandCell(SBumpDef &def,int nLandX,int nLandY)
{
	bool bRes = FALSE;
	// z2  6|\~~~7|\         H0' = y0+n013*R
	//      |  \  |  \       H1' = y1+n142*R
	//      |____\|____\     H2' = y2+n257*R
	// z1  3|\   2|\    |5   H3' = y3+n326*R
	//      |L \ U|  \  |    nL = n013'
	//  z  0|____\|1___\|4   nU = n123'
	//      x     x1    x2   (landY=-z)
	int	nLandXIdx = nLandX&m_nAxisMask,
		nLandX1Idx = (nLandX+1)&m_nAxisMask,
 		nLandX2Idx = (nLandX+2)&m_nAxisMask,
		nLandYIdx = (nLandY&m_nAxisMask)<<m_nLandSizeShift,
		nLandY1Idx = ((nLandY+1)&m_nAxisMask)<<m_nLandSizeShift,
		nLandY2Idx = ((nLandY+2)&m_nAxisMask)<<m_nLandSizeShift;
	int	y0 = m_pHeights[nLandYIdx+nLandXIdx]&HEIGHT_MASK,
		y1 = m_pHeights[nLandYIdx+nLandX1Idx]&HEIGHT_MASK,
		y2 = m_pHeights[nLandY1Idx+nLandX1Idx]&HEIGHT_MASK,
		y3 = m_pHeights[nLandY1Idx+nLandXIdx]&HEIGHT_MASK,
		y4 = m_pHeights[nLandYIdx+nLandX2Idx]&HEIGHT_MASK,
		y5 = m_pHeights[nLandY1Idx+nLandX2Idx]&HEIGHT_MASK,
		y6 = m_pHeights[nLandY2Idx+nLandXIdx]&HEIGHT_MASK,
		y7 = m_pHeights[nLandY2Idx+nLandX1Idx]&HEIGHT_MASK;
	CFVector3 n013=CFVector3(y0-y1,m_fCellSize0,y3-y0);
	CFVector3 n142=CFVector3(y1-y4,m_fCellSize0,y2-y1);
	CFVector3 n257=CFVector3(y2-y5,m_fCellSize0,y7-y2);
	CFVector3 n326=CFVector3(y3-y2,m_fCellSize0,y6-y3);
	double	h0 = y0*m_fScaleHeight+Abs(n013)*def.fRadius/n013.y,
			h1 = y1*m_fScaleHeight+Abs(n142)*def.fRadius/n142.y,
			h2 = y2*m_fScaleHeight+Abs(n257)*def.fRadius/n257.y,
			h3 = y3*m_fScaleHeight+Abs(n326)*def.fRadius/n326.y;
	CFVector3	nL = CFVector3(h0-h1,m_fCellSize,h3-h0);
	CFVector3	nU = CFVector3(h3-h2,m_fCellSize,h2-h1);

	//        |\                      0,0,-1
	// -1,0,0 |  \ 1,0,-1             \~~~~|
	//        |____\             -1,0,1 \  | 1,0,0
	//         0,0,1                      \|
	double	x = nLandX*m_fCellSize, x1 = x+m_fCellSize;
	double	z = -nLandY*m_fCellSize, z1 = z-m_fCellSize;

	// ---------------------------------------------------
	double	fTimeL, fTimeR, bL, a, b;
	int i0, iL, i; bool aLlt0;
	// nL
	fTimeL = 0; fTimeR = def.fTime;
	i0 = -1; iL = -1;
	for( i = 0 ; i < 4 ; i++ ) {
		switch(i) {
			case 0:	b = def.start.z-z; a = def.vel.z; break;
			case 1:	b = x-def.start.x; a = -def.vel.x; break;
			case 2: b = def.start.x-x+z1-def.start.z; a = def.vel.x-def.vel.z; break;
			default: b = (def.start-CFVector3(x,h0,z))*nL; a = def.vel*nL; break;
		}
		if( i0 < 0 ) {
			if( iL < 0 ) { iL = i; bL = b; aLlt0 = a < 0; }
			else if( b > bL ) { iL = i; bL = b; aLlt0 = a < 0; }
		}
		if( a >= 0 ) {	// adj fTimeR if necessary
			if( a*fTimeR+b > 0 )	{ // t0[i] < tR, do adj fTimeR
				if( b < 0 ) {	// calc min in case we've lost precision
					double t = -b/a;
					if( t < fTimeR ) {
						if( t <= fTimeL ) goto NoBumpL;
						fTimeR = t;
					}
				} else goto NoBumpL; // a >= 0, b >= 0 => Max >= 0
			}
		}
		else { // a < 0, adj fTimeL if necessary
			if( a*fTimeL+b > 0 )	{ // t0[i] > tL, do adj fTimeL
				if( a*fTimeR+b < 0 ) { // calc max in case we've lost precision
					double t = -b/a;
					if( t > fTimeL ) {
						if( t >= fTimeR )	goto NoBumpL;
						fTimeL = t; i0 = i;
					}
				} else goto NoBumpL; // a*fTimeR+b >= 0 => Max >= 0
			}
		}
	}
	if( i0 >= 0 || (i0=iL) >= 0 && aLlt0 && bL <= 0 ) {
		ASSERT(fTimeL <= fTimeR && fTimeR <= def.fTime);
		//double	s = def.vel*nL;
		//if( s < 0 ) {
			def.vel1 = def.vel-nL*((def.vel*nL)*2/Abs2(nL));
			def.fTime = fTimeL;
			def.nBumpFlags = BF_BUMPLAND;
			//__pBumpBody = this;
			bRes = TRUE;
		//}
	}
NoBumpL:
	// nR
	fTimeL = 0; fTimeR = def.fTime;
	i0 = -1; iL = -1;
	for( i = 0 ; i < 4 ; i++ ) {
		switch(i) {
			case 0:	b = def.start.x-x1; a = def.vel.x; break;
			case 1:	b = z1-def.start.z; a = -def.vel.z; break;
			case 2: b = def.start.z-z+x1-def.start.x; a = def.vel.z-def.vel.x; break;
			default: b = (def.start-CFVector3(x1,h1,z))*nU; a = def.vel*nU; break;
		}
		if( i0 < 0 ) {
			if( iL < 0 ) { iL = i; bL = b; aLlt0 = a < 0; }
			else if( b > bL ) { iL = i; bL = b; aLlt0 = a < 0; }
		}
		if( a >= 0 ) {	// adj fTimeR if necessary
			if( a*fTimeR+b > 0 )	{ // t0[i] < tR, do adj fTimeR
				if( b < 0 ) {	// calc min in case we've lost precision
					double t = -b/a;
					if( t < fTimeR ) {
						if( t <= fTimeL ) goto NoBumpR;
						fTimeR = t;
					}
				} else goto NoBumpR; // a >= 0, b >= 0 => Max >= 0
			}
		}
		else { // a < 0, adj fTimeL if necessary
			if( a*fTimeL+b > 0 )	{ // t0[i] > tL, do adj fTimeL
				if( a*fTimeR+b < 0 ) { // calc max in case we've lost precision
					double t = -b/a;
					if( t > fTimeL ) {
						if( t >= fTimeR )	goto NoBumpR;
						fTimeL = t; i0 = i;
					}
				} else goto NoBumpR; // a*fTimeR+b >= 0 => Max >= 0
			}
		}
	}
	if( i0 >= 0 || (i0=iL) >= 0 && aLlt0 && bL <= 0 ) {
		ASSERT(fTimeL <= fTimeR && fTimeR <= def.fTime);
		//double	s = def.vel*nL;
		//if( s < 0 ) {
			def.vel1 = def.vel-nU*((def.vel*nU)*2/Abs2(nU));
			def.fTime = fTimeL;
			def.nBumpFlags = BF_BUMPLAND;
			//__pBumpBody = this;
			bRes = TRUE;
		//}
	}
NoBumpR:
	return bRes;
}
#endif

bool CLandscapeRect::BumpLandCell(SBumpDef &def,int nLandX,int nLandY)
{
	return _BumpLandCell(def,nLandX,nLandY);
}
bool CLandscapeRect::Bump(SBumpDef &def,int nLandX,int nLandY)
{
	return MAKEBOOL(m_map.Bump(def,nLandX,nLandY)|_BumpLandCell(def,nLandX,nLandY));
}
// -------------------------------------------------------------------------
// !!! Further optimizations are possible to eliminate dummy calls to
// BumpX/BumpY
bool CLandscapeRect::BumpX(TCCRect2 &rect,SBumpDef &def)
{
	if( rect.Width() <= 0 || rect.Height() <= 0 ) return FALSE;
	if( rect.Width() <= 1 )
		return	rect.Height() > 1 ? BumpY(rect,def) :
										Bump(def,rect.left,rect.top);
	int nCenterX = rect.CenterX();
	double	d0 = def.start.x-nCenterX*m_fCellSize; // dd = def.vel.x
	if( d0 < 0 ) {		// starting from left
		if( d0 < -def.fRadius )  {	// starting entirely from left
			if(	def.vel.x <= 0 || d0+def.fRadius < -def.vel.x*def.fTime ) // not reaching right
				return	BumpY(CRect2(rect.left,rect.top,nCenterX,rect.bottom),def);
		}
		return MAKEBOOL(BumpY(CRect2(rect.left,rect.top,nCenterX,rect.bottom),def)|
					BumpY(CRect2(nCenterX,rect.top,rect.right,rect.bottom),def));
	}
	else {	// starting from right
		if( d0 > def.fRadius )  {	// starting entirely from right
			if(	def.vel.x >= 0 || d0-def.fRadius > -def.vel.x*def.fTime ) // not reaching left
				return	BumpY(CRect2(nCenterX,rect.top,rect.right,rect.bottom),def);
		}
		return MAKEBOOL(BumpY(CRect2(nCenterX,rect.top,rect.right,rect.bottom),def)|
					BumpY(CRect2(rect.left,rect.top,nCenterX,rect.bottom),def));
	}
}

bool CLandscapeRect::BumpY(TCCRect2 &rect,SBumpDef &def)
{
	if( rect.Width() <= 0 || rect.Height() <= 0 ) return FALSE;
	if( rect.Height() <= 1 )	return	BumpX(rect,def);
	int nCenterY = rect.CenterY();
	double	d0 = def.start.z+nCenterY*m_fCellSize; // dd = def.vel.z (z=-nLandY)
	if( d0 < 0 ) {		// starting from top
		if( d0 < -def.fRadius )  {	// starting entirely from top
			if(	def.vel.z <= 0 || d0+def.fRadius < -def.vel.z*def.fTime ) // not reaching bottom
				return	BumpX(CRect2(rect.left,nCenterY,rect.right,rect.bottom),def);
		}
		return MAKEBOOL(BumpX(CRect2(rect.left,nCenterY,rect.right,rect.bottom),def)|
					BumpX(CRect2(rect.left,rect.top,rect.right,nCenterY),def));
	}
	else {	// starting from bottom
		if( d0 > def.fRadius )  {	// starting entirely from bottom
			if(	def.vel.z >= 0 || d0-def.fRadius > -def.vel.z*def.fTime ) // not reaching top
				return	BumpX(CRect2(rect.left,rect.top,rect.right,nCenterY),def);
		}
		return MAKEBOOL(BumpX(CRect2(rect.left,rect.top,rect.right,nCenterY),def)|
				BumpX(CRect2(rect.left,nCenterY,rect.right,rect.bottom),def));
	}
}

void CLandscapeRect::BumpX(TCCRect2 &rect,CViewDynamicList &list)
{
	if( !list.First() )	return;
	if( rect.Width() <= 1 ) {
		if( rect.Height() > 1 ) {
			BumpY(rect,list);
			return;
		}
 		for( CViewDynamic *pd = list.First() ; pd != NULL ; pd = pd->Next() ) {
			ASSERT(pd->IsBumpDynamic());
			Bump(((CViewBumpDynamic*)pd)->BumpDef(),rect.left,rect.top);
		}
		list.Clear();
		return;
	}

	CViewDynamicList left,right;
	int nCenterX = rect.CenterX();
	CRect2	rectL(rect.left,rect.top,nCenterX,rect.bottom),
			rectR(nCenterX,rect.top,rect.right,rect.bottom);
	for( CViewDynamic *pd = list.First() ; pd != NULL ; pd = pd->Next() ) {
		ASSERT(pd->IsBumpDynamic());
		SBumpDef &def = ((CViewBumpDynamic*)pd)->BumpDef();
		double	d0 = def.start.x-nCenterX*m_fCellSize; // dd = def.vel.x
		if( d0 < 0 ) {		// starting from left
			if( d0 < -def.fRadius )  {	// starting entirely from left
				if(	def.vel.x <= 0 || d0+def.fRadius < -def.vel.x*def.fTime ) // not reaching right
					left.Move(pd,list);
			}
		}
		else {	// starting from right
			if( d0 > def.fRadius )  {	// starting entirely from right
				if(	def.vel.x >= 0 || d0-def.fRadius > -def.vel.x*def.fTime ) // not reaching left
					right.Move(pd,list);
			}
		}
	}
	for( CViewDynamic *pd = list.First() ; pd != NULL ; pd = pd->Next() ) {
		CViewDynamic *pd1;
		for( pd1 = pd->Next(); pd1 != NULL; pd1=pd1->Next() ) ::Bump((CViewBumpDynamic*)pd,(CViewBumpDynamic*)pd1);
		for( pd1 = left.First() ;pd1!=NULL;pd1=pd1->Next() ) ::Bump((CViewBumpDynamic*)pd,(CViewBumpDynamic*)pd1);
		for( pd1 = right.First() ;pd1!=NULL;pd1=pd1->Next() ) ::Bump((CViewBumpDynamic*)pd,(CViewBumpDynamic*)pd1);
		BumpY(rectL,((CViewBumpDynamic*)pd)->BumpDef());
		BumpY(rectR,((CViewBumpDynamic*)pd)->BumpDef());
	}
	list.Clear();
	BumpY(rectL,left);
	BumpY(rectR,right);
}

void CLandscapeRect::BumpY(TCCRect2 &rect,CViewDynamicList &list)
{
	if( !list.First() )	return;
	if( rect.Height() <= 1 ) {
		if( rect.Width() > 1 ) {
			BumpX(rect,list);
			return;
		}
 		for( CViewDynamic *pd = list.First() ; pd != NULL ; pd = pd->Next() ) {
			ASSERT(pd->IsBumpDynamic());
			Bump(((CViewBumpDynamic*)pd)->BumpDef(),rect.left,rect.top);
		}
		list.Clear();
		return;
	}

	CViewDynamicList up,down;
	int nCenterY = rect.CenterY();
	CRect2	rectU(rect.left,nCenterY,rect.right,rect.bottom),
			rectD(rect.left,rect.top,rect.right,nCenterY);
	for( CViewDynamic *pd = list.First() ; pd != NULL ; pd = pd->Next() ) {
		ASSERT(pd->IsBumpDynamic());
		SBumpDef &def = ((CViewBumpDynamic*)pd)->BumpDef();
		double	d0 = def.start.z+nCenterY*m_fCellSize; // dd = def.vel.z (z=-nLandY)
		if( d0 < 0 ) {		// starting from up
			if( d0 < -def.fRadius )  {	// starting entirely from up
				if(	def.vel.z <= 0 || d0+def.fRadius < -def.vel.z*def.fTime ) // not reaching down
					up.Move(pd,list);
			}
		}
		else {	// starting from down
			if( d0 > def.fRadius )  {	// starting entirely from down
				if(	def.vel.z >= 0 || d0-def.fRadius > -def.vel.z*def.fTime ) // not reaching up
					down.Move(pd,list);
			}
		}
	}
	for( CViewDynamic *pd = list.First() ; pd != NULL ; pd = pd->Next() ) {
		CViewDynamic *pd1;
		for( pd1 = pd->Next(); pd1 != NULL; pd1=pd1->Next() ) ::Bump((CViewBumpDynamic*)pd,(CViewBumpDynamic*)pd1);
		for( pd1 = up.First() ;pd1!=NULL;pd1=pd1->Next() ) ::Bump((CViewBumpDynamic*)pd,(CViewBumpDynamic*)pd1);
		for( pd1 = down.First() ;pd1!=NULL;pd1=pd1->Next() ) ::Bump((CViewBumpDynamic*)pd,(CViewBumpDynamic*)pd1);
		BumpY(rectU,((CViewBumpDynamic*)pd)->BumpDef());
		BumpY(rectD,((CViewBumpDynamic*)pd)->BumpDef());
	}
	list.Clear();
	BumpY(rectU,up);
	BumpY(rectD,down);
}
// -------------------------------------------------------------------------
void CLandscapeRect::Setup(THeight *pHeights,
						int nSizeShift,double fCellSize,double fHRatio)
{
	m_pHeights = pHeights;
	m_nLandSizeShift = nSizeShift;
	m_nSize = 1<<nSizeShift;
	m_nAxisMask = m_nSize-1;
	m_fCellSize = fCellSize;
	m_fScaleHeight = fCellSize*fHRatio;
	m_fCellSize0 = 1/fHRatio;
}
// -------------------------------------------------------------------------
double CLandscapeRect::ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d)
{
	double fMin = 1e10;
	for( int x = m_rect.left ; x <= m_rect.right ; x++ )
	for( int y = m_rect.top ; y <= m_rect.bottom ; y++ ) {
		CFVector3	v(x*m_fCellSize,
							m_pHeights[(x&511)+(y&511)*512]*m_fScaleHeight,
							-y*m_fCellSize);
		fMin = Min(fMin,(v-pt)*n-d);
	}
	return	fMin;
}
void CLandscapeRect::AdjustPlane(CFVector3 &pt,CFVector3 &n,double &d)
{
	if( fabs(n.y) < 0.01 )	return;
	ASSERT(n.y>=0.99);
	double f = -1e10;
	for( int x = m_rect.left ; x <= m_rect.right ; x++ )
	for( int y = m_rect.top ; y <= m_rect.bottom ; y++ )
		f = Max(f,m_pHeights[(x&511)+(y&511)*512]*m_fScaleHeight);
	double d0 = (pt.y-f)*n.y-d;
	if( d0 < 0 ) {
		WRITELOG5("LANDRECT: (%d,%d)-(%d,%d) is off by %lg\n",
			m_rect.left,m_rect.top,m_rect.right,m_rect.bottom,d0);
		if( (pt.y-f)*n.y > 0 )	d = Min(d,(pt.y-f)*n.y);
		//d = Max(0.,-d0);
		double d1 = (pt.y-f)*n.y-d;
		if( d1 < 0 ) pt.y -= d1;
		double d2 = (pt.y-f)*n.y-d;
		(void)d2;
		ASSERT(d2>-0.0001);
	}
}

