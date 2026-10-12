
#pragma warning 364 10

#define	_PATCH	1
#define	_SEAL	1
#define	_LEAVE_TRAPEZIOID	0
#define	_NODRAW_BREAK		0




#if	_PRECOMPUTE_REDS

// Reds array is viewpoint-oriented, that is m_pReds[P][S]
// Reduction may increase or decrease along primary positive semiaxis
// The same applies to secondary semiaxis
// !!! I changed my mind! Reds array is absolutely oriented ([I_Y][I_X])

#define	REDCELL_SP(s,p)	REDUCTIONCELL_PS(s,p)
#define	REDS_SP(s,p)	GetReds((s)-SAS,(p)-PAS))
#define	SET_REDS_SP(s,p,d,r)	SetReds((s)-SAS,(p)-PAS),d,r)
#define	MAX_REDCELL_SP(s,p)	(P?MAX_REDCELL((s)-SAS,(p)-PAS):MAX_REDCELL((p)-PAS,(s)-SAS))

#if P
	#if S
	#else
		#define	ZFROMXY_SP m_zCoeff.x*x+m_zCoeff.y*m_hrange.GetLimit(x,y,m_nNearestHeight,n)-m_zCoeff.z*y+m_zCoeff0
	#endif
#else
	#if S
	#else
		#define	ZFROMXY_SP m_zCoeff.x*x+m_zCoeff.y*m_hrange.GetLimit(x,y,m_nNearestHeight,n)-m_zCoeff.z*y+m_zCoeff0
	#endif
#endif

void _CViewTerrain::DRAW4(DefineReductions)(int ytop,int ybottom)
		// z distance increases as x and y increase
{
	int x, y;
	int xlprev, xrprev, dmin;
	xlprev = xrprev = m_pAEdges[ytop].r;
	if( PAS ) ytop -= PAS, ybottom -= PAS;
	for( y = ytop ; LTP(y,ybottom) ; ) {
		int xl = m_pAEdges[y].l, xr = m_pAEdges[y].r, d, r;
		int	xstart;
		x = xl;
		if( GES(x,xlprev) && LTS(x,xrprev) ) {
			d = 1<<(r=REDS_SP(x,y-1));
			if( _ALIGNP(y,d)==y )  goto InitLine;
			xstart = x;
		}
		else {
		InitLine:
			//REDCELL_SP - is wrong!!! Axes are different!!
			//y is primary axis, x is secondary
			// From the other point of view it shoud be OK for
			// REDCELL_SP translates x and y to real OX and OY.
			d = 1<<(r = REDCELL_SP(x,y));
			x = _ALIGNS(x,d);
			xstart = x;
			SET_REDS_SP(x,y,d,r);
			x = ADDS(x,d);
		}
		dmin = d;
		for( ; LTS(x,xlprev) ; x = ADDS(x,d) ) {
			d = 1<<(r = Min(ReductionInt(ZFROMXY_SP),MAX_REDCELL_SP(x,y)));
			SET_REDS_SP(x,y,d,r); if(d<dmin) dmin=d;
		}
		for( ; LTS(x,xrprev); x = ADDS(x,d) ) {
			d = 1<<(r=REDS_SP(x,y-1));
			if( (y&-d)!=y )	continue;
			d = 1<<(r = Min(ReductionInt(ZFROMXY_SP),MAX_REDCELL_SP(x,y)));
			SET_REDS_SP(x,y,d,r); if(d<dmin) dmin=d;
		}
		for( ; LTS(x,xr); x = ADDS(x,d) ) {
			d = 1<<(r = Min(ReductionInt(ZFROMXY_SP),MAX_REDCELL_SP(x,y)));
			SET_REDS_SP(x,y,d,r); if(d<dmin) dmin=d;
		}
		for( int y1 = ADDP(y,dmin) ; LTP(y,y1) ; y = ADDP(y,1) ) {
			m_pAEdges[y].l = xstart;
			m_pAEdges[y].r = x;
		}
	} // y loop
}

#undef REDCELL_SP
#undef REDS_SP
#undef SET_REDS_SP
#undef MAX_REDCELL_SP
#undef ZFROMXY_SP

#endif	// _PRECOMPUTE_REDS



#if _SEAL && !PAS
void _CViewTerrain::DRAW2(Seal)(TCint rectL[4])
{
	if(!M_(pCallSeal) && !M_L(pCallSeal)) return;

	(void)rectL;

	int y , yy, y1, x, d, dd, ynewl;
	int /*d0 = 1<<CACHE_PL[y].rc,*/ rc, rc0, xx;
	if( M_(pCallSeal) ) {
		y = M_(pCallSeal)->L[P];
		x = CACHE_PL[y].L[S];
		d = 1<<(rc0=rc=CACHE_PL[y].rc);
		ynewl = _NEXTP(y,d);//CACHE_SU[x].L[P];
		//ASSERT( ynewl == _ALIGNP(SUBP(y,1),1<<CACHE_PL[y].rc) );
		ASSERT( LEP( ynewl, m_viewpointL[P]+PAS ) && GTP( y, m_viewpointL[P]+PAS ) );
		/*while( GTP( ynewl, m_viewpointL[P]+PAS ) ) {
			ASSERT( !M_(bVis) && LEP(M_(ylowerL),m_viewpointL[P]+PAS) );
			y = ynewl;
			ynewl = SUBP( ynewl, 1<<CACHE_PL[y].rc );
		}
		ASSERT( GTP(ADDP(M_L(ynewR),1<<CACHE_PL[M_L(ynewR)].rc),m_viewpointL[P]+PAS) || !M_L(bVis) );*/
	}
	else {
		ASSERT( M_L(pCallSeal) );
		ynewl = M_L(pCallSeal)->L[P];
		x = CACHE_PU[ynewl].L[S];
		d = 1<<(rc0=rc=CACHE_PU[ynewl].rc);
		x = _NEXTS(x,d);
		y = NEXTP_(ynewl,d);//CACHE_SU[x].L[P];
		ASSERT( LEP( ynewl, m_viewpointL[P]+PAS ) && GTP( y, m_viewpointL[P]+PAS ) );
		/*ASSERT( y == _ALIGNP(ADDP(ynewl,1),1<<CACHE_PU[ynewl].rc) );
		while( LEP( y, m_viewpointL[P]+PAS ) ) {
			ASSERT( !M_L(bVis) && GTP(M_L(ylowerR),m_viewpointL[P]+PAS) );
			ynewl = y;
			y = ADDP( y, 1<<CACHE_PU[y].rc );
		}
		ASSERT( LEP(SUBP(M_(ynewL),1<<CACHE_PU[M_(ynewL)].rc),m_viewpointL[P]+PAS) || !M_(bVis) );*/
	}

	bool bResult = TRUE; //FALSE;

	WRITELOG2("LPatch entry: M_(iCacheL)=%d M_L(iCacheU)=%d\n",M_(iCacheL),M_L(iCacheU));

	bool	bCacheL_U = FALSE, bCacheU_U = FALSE;
	int		iCacheBrinkL_U = M_(iCacheL);
	/*while( LES(iCacheBrinkL_U,x) ) {
		ASSERT( GTS(CACHE_SU[iCacheBrinkL_U].nextrun,iCacheBrinkL_U) );
		iCacheBrinkL_U = CACHE_SU[iCacheBrinkL_U].nextrun;
		bCacheL_U = !bCacheL_U;
	}*/
	//M_(iCacheL)=iNewCacheL_U; //x;


	bool	bCacheL_L = FALSE, bCacheU_L = FALSE;
	int		iCacheBrinkL_L = M_L(iCacheU);
	/*while( LES(iCacheBrinkL_L,x) ) {
		ASSERT( GTS(CACHE_SL[iCacheBrinkL_L].nextrun,iCacheBrinkL_L) );
		iCacheBrinkL_L = CACHE_SL[iCacheBrinkL_L].nextrun;
		bCacheL_L = !bCacheL_L;
	}*/
	//M_L(iCacheU) = iNewCacheL_L; //x;




	// Draw a patch cell
	for(y1=y;;) {
		y1 = _NEXTP(y1,d);
		//if( d < d0 ) goto ContinueRun;	// patch can only expand
		break;
		//if( LTP(y1,rectL[P+2*(1-PAS)]) )	break; // patch is only one cell wide
		//if( LEP(y1,ynewl) ) goto End; ?????????
	}
	/*if( !bCacheL_U ) {
		for( yy = ADDP(y1,d) ; GTP(yy,y) ; ) {
			CREATECELLVERTEX_PS(yy,x,CACHE_PL[yy]);
			CACHE_PL[yy].rc = rc;
			yy = CACHE_PL[yy].nxt = MAXP(y,SUBP(yy,1<<(int)REDUCTIONCELL_PS(SUBP(yy,1),SUBS(x,1))));
		}
		ASSERT( yy == y || yy == ADDP(y1,d) );
	}*/
	y = ADDP(y1,d);

	CRect2	cellL;
	y = cellL.bottom = MINP( y, m_rectL[P_BOTTOM] );
	y1 = cellL.top = MAXP( y1, m_rectL[P_TOP] );
	cellL.right = MINS( NEXTS_(x,d), m_rectL[S_RIGHT] );
	x = cellL.left = MAXS( x, m_rectL[S_LEFT] );
	ASSERT( GTP( cellL.bottom, cellL.top ) && GTS( cellL.right, cellL.left ) );


	//if( LTP(y1,ynewl) ) { M_(pStallL) = &CACHE_PL[y]; break; }
	// reverse run link-through
	WRITELOG2("Seal: link-through x=%d iCacheBrinkL_U=%d\n",x,iCacheBrinkL_U);
	int iCacheBrink1L_U = x;
	/*ASSERT( LTS(x,iCacheBrinkL_U) );
	for( ; LES(iCacheBrinkL_U,m_viewpointL[S]) ; ) {
		ASSERT( GTS( CACHE_SU[iCacheBrinkL_U].nextrun, iCacheBrinkL_U ) );
		int iCacheBrinkTempL_U = CACHE_SU[iCacheBrinkL_U].nextrun;
		WRITELOG1("Seal: link-through Temp=%d\n",iCacheBrinkTempL_U);
		CACHE_SU[iCacheBrinkL_U].nextrun = iCacheBrink1L_U;
		iCacheBrink1L_U = iCacheBrinkL_U;
		iCacheBrinkL_U = iCacheBrinkTempL_U;
		bCacheL_U = !bCacheL_U;
	}*/
	bool bCache1L_U = bCacheL_U;


	BeginDrawCell();
	PUTTRIANGLEMARK(1);
	xx = cellL.right; //ADDS(x,d);
	// fill in right vertical
	for( yy = y1 ; /*LTP(yy,y)*/ ;) {
		if( ! bCacheU_U ) {
			CREATECELLVERTEX_PS(yy,xx,CACHE_PU[yy]);
			dd = 1<<(int)REDUCTIONCELL_PS(yy,xx);
			CACHE_PU[yy].nxt = MINP(y,NEXTP_(yy,dd));
		}
		if( GEP(yy,y) ) break;
		ASSERT( CACHE_PU[yy].L[S] == xx );
		AddCellVertex(CACHE_PU[yy]);
		yy = CACHE_PU[yy].nxt;
	}
	ASSERT(yy==y);

	PUTTRIANGLEMARK(2);
	AddCellVertex(CACHE_PU[y]);

	// fill in upper horizontal
	for( xx = cellL.right ;; ) {
		/*while( LTS(xx,iCacheBrink1L_U) ) {
			ASSERT( LTS( CACHE_SU[iCacheBrink1L_U].nextrun, iCacheBrink1L_U ) );
			iCacheBrink1L_U = CACHE_SU[iCacheBrink1L_U].nextrun;
			bCache1L_U = !bCache1L_U;
		}
		ASSERT( GES(xx,iCacheBrink1L_U) );*/
		if( !bCache1L_U || xx == iCacheBrink1L_U ) {
			dd = 1<<(int)REDUCTIONCELL_PS(y,SUBS(xx,1));
			CACHE_SU[xx].nxt = MAXS(x,_NEXTS(xx,dd));
		}
		ASSERT( LTS(CACHE_SU[xx].nxt,xx) );
		xx = CACHE_SU[xx].nxt;
		if( LES(xx,x) )	break;
		if( !bCache1L_U )
			CREATECELLVERTEX_PS(y,xx,CACHE_SU[xx]);
		LOGCACHE("Seal read",SU,xx);
		ASSERT(CACHE_SU[xx].L[P] == y );
		AddCellVertex( CACHE_SU[xx] );
	}
	ASSERT( xx == x );
	// fill in left vertical
	PUTTRIANGLEMARK(3);
	for( yy = y ;; yy = CACHE_PL[yy].nxt ) {
		if( ! bCacheU_U ) {
			CREATECELLVERTEX_PS(yy,xx,CACHE_PL[yy]);
			dd = 1<<(int)REDUCTIONCELL_PS(SUBP(yy,1),SUBS(xx,1));
			CACHE_PL[yy].nxt = MAXP(y1,_NEXTP(yy,dd));
		}
		ASSERT( CACHE_PL[yy].L[S] == x );
		if( LEP(yy,y1) ) break;
		LOGCACHE("Seal read",PL,yy);
		AddCellVertex( CACHE_PL[yy] );
		ASSERT( LTP(CACHE_PL[yy].nxt,yy) );
	}
	ASSERT( yy == y1 && CACHE_PL[y1].L[S] == x );
	PUTTRIANGLEMARK(0);
	LOGCACHE("Seal read",PL,y1);
	AddCellVertex( CACHE_PL[y1] );
	// CCellVertex *pStartNew = CACHE_PL[y1];
	//if( !bCache1L_U ) CACHE_SU[ADDS(x,d)].rc = (int)REDUCTIONCELL_PS(SUBP(y,1),ADDS(x,d));
	//CACHE_PL[y] = CACHE_SU[ADDS(x,d)]; // copy it here, for later it will be deleted
							// !!!!!!! Not the case for patch
	//LOGCACHE("Seal copy",PL,y);
	// copy horizontal from lower (fill in new horizontal in LRun)

	for( xx = x ;; ) {
		if( !bCacheL_L || GES(ADDS(xx,1),iCacheBrinkL_L)) {
			dd = 1<<(int)REDUCTIONCELL_PS(SUBP(y1,1),xx);
			CACHE_SL[xx].nxt = MINS(cellL.right,NEXTS_(xx,dd));
		}
		ASSERT( GTS( CACHE_SL[xx].nxt,xx) );
		xx = CACHE_SL[xx].nxt;
		/*while( GES(xx,iCacheBrinkL_L) ) {
			ASSERT( GTS( CACHE_SL[iCacheBrinkL_L].nextrun, iCacheBrinkL_L ) );
			iCacheBrinkL_L = CACHE_SL[iCacheBrinkL_L].nextrun;
			bCacheL_L = !bCacheL_L;
			WRITELOG3("Seal cache flip to %d at %d,%d\n",bCacheL_L,xx,y);
		}
		ASSERT( LTS(xx,iCacheBrinkL_L) );*/
		if( !bCacheL_L ) CREATECELLVERTEX_PS(y1,xx,CACHE_SL[xx]);
		LOGCACHE("Seal read",SL,xx);
		ASSERT(CACHE_SL[xx].L[P] == y1 );
		if( GES(xx,cellL.right) ) break;
		AddCellVertex( CACHE_SL[xx] );
	}
	ASSERT( xx == cellL.right );

	ENDDRAWCELL(rc0);
}
#endif

#if _PATCH
bool _CViewTerrain::DRAW4(LPatch)(TCint rectL[4])
{
	//if( ! M_(bCallPatchL) && M_(bVis) || ! M_L(bCallPatchU) && M_L(bVis) ) return FALSE;
	if( M_(pCallSeal) ) return FALSE;
	M_(ypatchL) = M_L(ypatchU) = m_viewpointL[P];
	//M_(pStallL) = M_L(pStallU) = NULL;	// !!!
	WRITELOG3("LPatch entry: M_(ylowerL)=%d M_(pCallPatchL)=%08X M_L(bCallPatchU)=%08X\n",M_(ylowerL),M_(pCallPatchL),M_L(pCallPatchU));
	#ifdef	_DEBUG
		if( M_(pCallPatchL) )	WRITELOG3("M_(pCallPatchL)=%d,%d (%d)\n",M_(pCallPatchL)->L[S],M_(pCallPatchL)->L[P],M_(pCallPatchL)->rc);
		if( M_L(pCallPatchU) )	WRITELOG3("M_L(pCallPatchU)=%d,%d (%d)\n",M_L(pCallPatchU)->L[S],M_L(pCallPatchU)->L[P],M_L(pCallPatchU)->rc);
	#endif
	//M_(bCallPatchL) = M_L(bCallPatchU) = FALSE;
	ASSERT( GTP(M_(ylowerL),m_viewpointL[P]+PAS) == LEP(M_L(ylowerR),m_viewpointL[P]+PAS) );
	int y = M_(ynewL)/*M_(ylowerL)*/, ylower0 = y, x, d, dd;
	int iNewCacheL_U = M_(iCacheL), iNewCacheL_L = M_L(iCacheU), i;
	(void)i;
	int nxt, y1, yy, rc0;
	CRect2	cellL;
	bool bFirst = TRUE;
	IF_DEBUG(bool bFirstEntry=FALSE);
	if( GTP( M_(ylowerL), m_viewpointL[P]+PAS) ) {
		IF_DEBUG(bFirstEntry=TRUE);
		WRITELOG("LPatch first entry\n");
		bFirst = FALSE;	// vert cache will be init here
		//y = m_viewpointL[P]+(1-PAS); //M_(ylowerL);
		//int i = MinMax(y+(PAS-1)-m_polygonRectL[P],0L,m_polygonRectL[P+2]-m_polygonRectL[P]-1);
		//int nRdL = m_pEdgeR[i][SAS]; d = 1<<nRdL;
		int nRdL;
		//ASSERT( M_(pCallPatchL) || M_L(pCallPatchU) );
		if( M_(pCallPatchL) ) {
			y = M_(pCallPatchL)->L[P];
			x = M_(pCallPatchL)->L[S];
			d = 1<<(nRdL = M_(pCallPatchL)->rc);
			if( M_L(pCallPatchU) && LTS(M_L(pCallPatchU)->L[S],x) ) {
				x = M_L(pCallPatchU)->L[S];
				d = 1<<(nRdL = M_L(pCallPatchU)->rc);
				y = NEXTP_(M_L(pCallPatchU)->L[P],d);
			}
		} else
		if( M_L(pCallPatchU) ) {
			x = M_L(pCallPatchU)->L[S];
			d = 1<<(nRdL = M_L(pCallPatchU)->rc);
			y = NEXTP_(M_L(pCallPatchU)->L[P],d);
		}
		else return M_(bWaitPatch);
		#if 0
		if( GTP(y,NEXTP_(m_polygonRectL[P]+i+PAS,d)) ||
			LEP( ALIGNP_(y,d), m_polygonRectL[P+PAS*2] ) ) { // if out of polygon
			//ASSERT(0); // We'll find out later what to do in this case
			// If no stall then need not draw even seal!!!
			//if(!M_(pStallL)&&!M_L(pStallU)) return FALSE;
			if(M_(pStallL)) { x = M_(pStallL)->L[S]; y = M_(pStallL)->L[P]; } else
			if(M_L(pStallU)) { x = M_L(pStallU)->L[S]; y = ADDP(M_L(pStallU)->L[P],1); }
			else {
				M_(pCallPatchL) = M_L(pCallPatchU) = NULL;
				return FALSE;
			}
		}
		else {
			i = ((y+PAS-1)>>nRdL)-(m_polygonRectL[P]>>nRdL);
			ASSERT( i >= 0 && i < m_pnEdges[nRdL] );
			int left = m_ppEdges[nRdL][i][SAS];
			x = MAXS(left,(int)rectL[S+2*SAS]);
			if( M_(pCallPatchL) && LTS(M_(pCallPatchL)->L[S],x) )	x = M_(pCallPatchL)->L[S];
			if( M_L(pCallPatchU) && LTS(M_L(pCallPatchU)->L[S],x) )	x = M_L(pCallPatchU)->L[S];
		}
		#endif
		M_(pCallPatchL) = M_L(pCallPatchU) = FALSE;
		/*if( x != left )*/
		//nRdL = (int)REDUCTIONCELL_PS(SUBP(y,1),x);
		//d = 1<<nRdL;
		x = _ALIGNS(x,d);
		// !!!!! ASSERT( y < ylower || (y & -d) == y || x < M_(iCacheL) || CACHE_SU[x].yL == y );
		if( M_(pStallL) ) {
			if( LTP(M_(pStallL)->L[P],SUBP(y,d)) ) {
				ASSERT(0); // I wonder how come we got here?
				y = SUBP(y,d); /*goto Again;*/ /*return TRUE;*/
			}
			if( LTS(M_(pStallL)->L[S],x) ) {
				x = M_(pStallL)->L[S];
				y = M_(pStallL)->L[P];
				d = 1<<(nRdL=M_(pStallL)->rc);
			}
		}
		if( M_L(pStallU) ) {
			if( GTP(M_L(pStallU)->L[P],y) ) {
				ASSERT(0); // I wonder how come we got here?
				y = SUBP(y,d); /*goto Again;*/ /*return TRUE;*/
			}
			if( LTS(M_L(pStallU)->L[S],x) ) {
				x = M_L(pStallU)->L[S];
				y = M_L(pStallU)->L[P];
				d = 1<<(nRdL=M_L(pStallU)->rc);
				y = NEXTP_(y,d);
			}
		}
		y = ALIGNP_(y,d);
		/*if( GTS(x,m_viewpointL[S]+SAS) ) {
			M_(ylinkL) = m_viewpointL[P]+(1-PAS);
			M_L(ylinkU) = m_viewpointL[P]+PAS;
			M_(iCacheL) = m_viewpointL[S]+(1-SAS);
			M_L(iCacheU) = m_viewpointL[S]+(1-SAS);
			M_(pStallL) = NULL; y = SUBP(y,d); return FALSE;
		}*/
		x = cellL.left = MAXS( x, m_rectL[S_LEFT] );
		if( GTS(x,MINS(M_(xlinkL),M_L(xlinkU))) ) {
			WRITELOG4("LPatch: stop from xlink x=%d %d=Min(%d,%d)\n",x,
							MINS(M_(xlinkL),M_L(xlinkU)),M_(xlinkL),M_L(xlinkU));
			return TRUE;
		}
		if( GTS(x,m_viewpointL[S]+SAS) || GES(x,rectL[S+2*(1-SAS)]) ) {
				//?????????????????
			M_(pStallL) = NULL; M_(iCacheL) = M_L(iCacheU) = m_viewpointL[S]+(1-SAS);
			M_(ylinkL) = m_viewpointL[P]+(1-PAS);
			M_L(ylinkU) = m_viewpointL[P]+PAS;
			y = SUBP(y,d);
			return FALSE;
		}
		M_(ylowerL) = SUBP(y,d);
		//ASSERT( M_(ylowerL) == M_L(ylowerR) || !M_(bVis) || !M_L(bVis));
		M_L(ylowerR) = y;
		int y0;
		y = cellL.bottom = MINP( y, m_rectL[P_BOTTOM] );
		cellL.top = MAXP( _NEXTP(y,d), m_rectL[P_TOP] );
		cellL.bottom = MINP( cellL.bottom, M_(ylinkL) );
		cellL.top = MAXP( cellL.top, M_L(ylinkU) );
		if( LTS( x, M_(lastStartL)[S] ) && GTP( cellL.bottom, M_(lastStartL)[P] ) )	cellL.bottom = M_(lastStartL)[P];
		if( LTS( x, M_L(lastStartU)[S] ) && LTP( cellL.top, M_L(lastStartU)[P] ) )	cellL.top = M_L(lastStartU)[P];
		bool bCache = TRUE;
		for( int iCache = M_(iCacheL) ;	LTS(iCache,m_viewpointL[S]+(1-SAS)); iCache = CACHE_SU[iCache].nextrun, bCache = !bCache ) if( bCache && GES(iCache,x) )
			if( LTP(CACHE_SU[iCache].L[P],cellL.bottom) ) cellL.bottom = CACHE_SU[iCache].L[P]; else break;
		bCache = TRUE;
		for( int iCache = M_L(iCacheU) ; LTS(iCache,m_viewpointL[S]+(1-SAS)); iCache = CACHE_SL[iCache].nextrun, bCache = !bCache ) if( bCache && GES(iCache,x) )
			if( GTP(CACHE_SL[iCache].L[P],cellL.top) ) cellL.top = CACHE_SL[iCache].L[P]; else break;

		ASSERT( GTP( cellL.bottom, cellL.top ) );
		y0 = y = cellL.bottom;
		//CREATECELLVERTEX_PS(M_(ylowerL)=SUBP(y,d),x,CACHE_SU[x]);
		//CACHE_SU[x].rc = (int)REDUCTIONCELL_PS(SUBP(M_(ylowerL),1),x);
		//LOGCACHE("LRun start",SU,x);
		iNewCacheL_L = iNewCacheL_U = x;
		yy = cellL.top; //M_(ylowerL);
		dd = 1<<Min((int)REDUCTIONCELL_PS(SUBP(yy,1),SUBS(x,1)),(int)REDUCTIONCELL_PS(SUBP(yy,1),x));
		nxt = CACHE_PL[yy].nxt; //_NEXTP(yy,dd);
		for(;;) {
			CREATECELLVERTEX_PS(yy,x,CACHE_PL[yy]);
			CACHE_PL[yy].rc = nRdL;
			CACHE_PL[yy].nxt = nxt; nxt = yy;
			LOGCACHE("LPatch start",PL,yy);
			if( GEP(yy,y0) ) break;
			dd = 1<<(int)REDUCTIONCELL_PS(yy,SUBS(x,1));
			yy = MINP(y,NEXTP_(yy,dd));
		}
		CACHE_SU[x].L[P] = cellL.top; CACHE_SL[x].L[P] = cellL.bottom;
		IF_DEBUG(CACHE_SU[x].L[S]=CACHE_SL[x].L[S]=x);
		M_(ynewL) = cellL.bottom;
		M_L(ynewR) = cellL.top;
		//CACHE_SL[x].nxt = ADDS(x,1<<Min(nRdL,(int)REDUCTIONCELL_PS(x,SUBP(M_(ylowerL),1))));
		//ASSERT(M_(ylowerL) == M_L(ylowerR));
		//M_L(ylowerR) = ylower0;

		//y = MINP( y, m_rectL[P_BOTTOM] );
		//x = CACHE_PL[y].L[S];
		ASSERT( GES( x, m_rectL[S_LEFT] ) );
		dd = 1<<(rc0=nRdL);
	}
	else {
		//ASSERT( M_(pCallPatchL) || M_L(pCallPatchU) );
		if( M_(pCallPatchL) ) {
			y = M_(pCallPatchL)->L[P];
			x = M_(pCallPatchL)->L[S];
			dd = 1<<(rc0=M_(pCallPatchL)->rc);
			if( M_L(pCallPatchU) && LTS(M_L(pCallPatchU)->L[S],x) ) {
				x = M_L(pCallPatchU)->L[S];
				dd = 1<<(rc0=M_L(pCallPatchU)->rc);
				y = NEXTP_(M_L(pCallPatchU)->L[P],dd);

			}
		} else
		if( M_L(pCallPatchU) ) {
			x = M_L(pCallPatchU)->L[S];
			dd = 1<<(rc0=M_L(pCallPatchU)->rc);
			y = NEXTP_(M_L(pCallPatchU)->L[P],dd);
		} else {
			return M_(bWaitPatch);
			/*y = M_(ynewL);
			y = MINP( y, m_rectL[P_BOTTOM] );
			x = CACHE_PL[y].L[S];
			ASSERT( GES( x, m_rectL[S_LEFT] ) );
			dd = 1<<(rc0=CACHE_PL[y].rc);*/
		}
		M_(pCallPatchL) = M_L(pCallPatchU) = FALSE;
	}



	//M_(pStallL) = NULL; ???????????

	//y = MINP( y, m_rectL[P_BOTTOM] );
	//x = CACHE_PL[y].L[S];
	ASSERT( GES( x, m_rectL[S_LEFT] ) );
	//dd = 1<<CACHE_PL[y].rc;
	int d0 = dd, rc, rrc, xx, ynewl;
	ynewl = MAXP(_NEXTP(y,dd),m_rectL[P_TOP]);
	while( GTP( ynewl, m_viewpointL[P]+PAS ) ) {
		ASSERT(0);
		ASSERT( !M_(bVis) && LEP(M_(ylowerL),m_viewpointL[P]+PAS) );
		y = ynewl;
		ynewl = SUBP( ynewl, 1<<CACHE_PL[y].rc );
	}
	//ASSERT( !M_(bVis) || CACHE_PL[M_L(ynewR)].L[P] == M_L(ynewR) &&
	//	GTP(NEXTP_(M_L(ynewR),1<<CACHE_PL[M_L(ynewR)].rc),m_viewpointL[P]+PAS) );
	bool bResult = TRUE; //FALSE;

	WRITELOG2("LPatch entry: M_(iCacheL)=%d M_L(iCacheU)=%d\n",M_(iCacheL),M_L(iCacheU));

	#ifdef _DEBUG
	{
		WRITELOG("### LPATCH M_(iCacheL) RUN:");
		for( int i_ = M_(iCacheL) ; LTS(i_,m_viewpointL[S]+(1-SAS)) ;
											i_ = CACHE_SU[i_].nextrun ) {
			WRITELOG1(" %d", i_ );
			ASSERT( GTS(CACHE_SU[i_].nextrun, i_ ) );
		}
		ASSERT( i_ == m_viewpointL[S]+(1-SAS) );
		WRITELOG1(" %d\n", i_ );
	}
	#endif


	bool	bCacheL_U = FALSE;
	int		iCacheBrinkL_U = M_(iCacheL), *piCacheL_U = &M_(iCacheL);
	while( LES(iCacheBrinkL_U,x) ) {
		ASSERT( GTS(CACHE_SU[iCacheBrinkL_U].nextrun,iCacheBrinkL_U) );
		piCacheL_U = &CACHE_SU[iCacheBrinkL_U].nextrun;
		iCacheBrinkL_U = CACHE_SU[iCacheBrinkL_U].nextrun;
		bCacheL_U = !bCacheL_U;
	}
	ASSERT( *piCacheL_U == iCacheBrinkL_U );
	if( iNewCacheL_U != M_(iCacheL) ) {
		ASSERT(iNewCacheL_U==x);
		piCacheL_U = &CACHE_SU[iNewCacheL_U].nextrun;
	}
	ASSERT( piCacheL_U != &M_(iCacheL) );
	M_(iCacheL)=iNewCacheL_U; //x;
	bool	bCacheStartL_U = bCacheL_U;
	int		iCacheStartL_U = x;



	bool	bCacheL_L = FALSE;
	int		iCacheBrinkL_L = M_L(iCacheU), *piCacheL_L = &M_L(iCacheU);
	while( LES(iCacheBrinkL_L,x) ) {
		ASSERT( GTS(CACHE_SL[iCacheBrinkL_L].nextrun,iCacheBrinkL_L) );
		piCacheL_L = &CACHE_SL[iCacheBrinkL_L].nextrun;
		iCacheBrinkL_L = CACHE_SL[iCacheBrinkL_L].nextrun;
		bCacheL_L = !bCacheL_L;
	}
	ASSERT( *piCacheL_L == iCacheBrinkL_L );
	if( iNewCacheL_L != M_L(iCacheU) ) {
		ASSERT(iNewCacheL_L==x);
		piCacheL_L = &CACHE_SL[iNewCacheL_L].nextrun;
	}
	ASSERT( piCacheL_L != &M_L(iCacheU) );
	M_L(iCacheU) = iNewCacheL_L; //x;
	bool	bCacheStartL_L = bCacheL_L;
	int		iCacheStartL_L = x;

	for(;; ) {

		/*y =*/ cellL.bottom = MINP(y, m_rectL[P_BOTTOM] );
		ynewl = cellL.top = MAXP( ynewl, m_rectL[P_TOP] );

		// Draw a patch cell
		if( GES(x,rectL[S+2*(1-SAS)]) ) {
			// bResult = FALSE; break;
			//ASSERT( cnt > 0 );	// If we drew nothing it's not correct cache setting below
			M_(iCacheL) = M_L(iCacheU) = m_viewpointL[S]+(1-SAS);
			M_(ylinkL) = m_viewpointL[P]+(1-PAS);
			M_L(ylinkU) = m_viewpointL[P]+PAS;
			return FALSE;	// Stop if came to border
		}
		for(y1=cellL.bottom;;) {
			d=1<<(rc=rc0/*CACHE_PL[y1].rc*/);
			y1 = _NEXTP(y1,d);
			if( d < d0 ) {
				//M_(ylinkL) = MAXP(M_(ylinkL),cellL.bottom); //m_viewpointL[P]+PAS; //(1-PAS);
				//M_L(ylinkU) = MINP(M_L(ylinkU),cellL.top); //m_viewpointL[P]+(1-PAS); //PAS;
				//M_(xlinkL) = MAXS(M_(xlinkL),cellL.left);
				//M_L(xlinkU) = MAXS(M_L(xlinkU),cellL.left);
				goto ContinueRun;	// patch can only expand
			}
			break;
			//if( LTP(y1,rectL[P+2*(1-PAS)]) )	break; // patch is only one cell wide
			//if( LEP(y1,ynewl) ) goto End; ?????????
		}
		d0 = d;
		WRITELOG1("LPatch: d=%d\n",d);

		/*y =*/ cellL.bottom = MINP( ADDP(y1,d), m_rectL[P_BOTTOM] );
		cellL.top = MAXP( y1, m_rectL[P_TOP] );
		int iCacheBrink1L_U;
		if( LTS(iCacheBrinkL_U,m_viewpointL[S]+(1-SAS)) ) {
			if( !bCacheL_U ) cellL.bottom = MINP( cellL.bottom, CACHE_SU[iCacheBrinkL_U].L[P] ); else
			if( ADDS(x,1) == iCacheBrinkL_U && LTS(iCacheBrink1L_U=CACHE_SU[iCacheBrinkL_U].nextrun,m_viewpointL[S]+(1-SAS)) )
				cellL.bottom = MINP( cellL.bottom, CACHE_SU[iCacheBrink1L_U].L[P] );
		}
		cellL.bottom = MINP( cellL.bottom, M_(ylinkL) );
			// if got here, then ylinkL is still valid (bWaitPatch==TRUE)
		if( LTS(iCacheBrinkL_L,m_viewpointL[S]+(1-SAS)) ) {
			if( !bCacheL_L ) cellL.top = MAXP( cellL.top, CACHE_SL[iCacheBrinkL_L].L[P] ); else
			if( ADDS(x,1) == iCacheBrinkL_L && LTS(iCacheBrink1L_U=CACHE_SL[iCacheBrinkL_L].nextrun,m_viewpointL[S]+(1-SAS)) )
				cellL.top = MAXP( cellL.top, CACHE_SL[iCacheBrink1L_U].L[P] );
		}
		cellL.top = MAXP( cellL.top, M_L(ylinkU) );
			// if got here, then ylinkU is still valid (bWaitPatch==TRUE)
		ASSERT( GTP( cellL.bottom, cellL.top ) );
		// Reset stall flag if necessary
		if( M_(pStallL) ) {
			ASSERT( GES(M_(pStallL)->L[S],x) );
			if( LES(M_(pStallL)->L[S],x) )	{
				ASSERT( LEP(M_(pStallL)->L[P],cellL.bottom) );
				M_(pStallL) = NULL;
			}
		}
		if( M_L(pStallU) ) {
			ASSERT( GES(M_L(pStallU)->L[S],x) );
			if( LES(M_L(pStallU)->L[S],x) )	{
				ASSERT( GEP(M_L(pStallU)->L[P],cellL.top) );
				M_L(pStallU) = NULL;
			}
		}
		if( !bCacheL_U ) {
			if( bFirst )	y = _ALIGNP(m_viewpointL[P],d);
			for( yy = cellL.bottom /*NEXTP_(y1,d)*/ ; GTP(yy,y) ; ) {
				CREATECELLVERTEX_PS(yy,x,CACHE_PL[yy]);
				CACHE_PL[yy].rc = rc;
				dd = 1<<(int)REDUCTIONCELL_PS(SUBP(yy,1),SUBS(x,1));
				yy = CACHE_PL[yy].nxt = MAXP(y,_NEXTP(yy,dd));
			}
			ASSERT( yy == y || yy == cellL.bottom /*NEXTP_(y1,d)*/ );
		}
		if( !bCacheL_L ) {
			if( bFirst )	ynewl = ALIGNP_(m_viewpointL[P],d);
			for( yy = ynewl ; GTP(yy,cellL.top) ; ) {
				dd = 1<<(int)REDUCTIONCELL_PS(SUBP(yy,1),SUBS(x,1));
				yy = CACHE_PL[yy].nxt = MAXP(cellL.top,_NEXTP(yy,dd));
				CREATECELLVERTEX_PS(yy,x,CACHE_PL[yy]);
				CACHE_PL[yy].rc = rc;
			}
			ASSERT( yy == cellL.top || yy == ynewl );
		}
		bFirst = FALSE;
		y1 = cellL.top;
		y = cellL.bottom;
		//if( LTP(y1,ynewl) ) { M_(pStallL) = &CACHE_PL[y]; break; }
		cellL.left = x;
		cellL.right = MINS( NEXTS_(x,d), m_rectL[S_RIGHT] );
		ASSERT( GTS( cellL.right, cellL.left ) );
		if( GTS(cellL.right,MINS(M_(xlinkL),M_L(xlinkU))) ) {
			WRITELOG4("LPatch: stop from xlink cellL.right=%d %d=Min(%d,%d)\n",cellL.right,
							MINS(M_(xlinkL),M_L(xlinkU)),M_(xlinkL),M_L(xlinkU));
			break;
		}
		#if _LEAVE_TRAPEZIOID
			// Trapezioid check
			bool bLeaveTrapezioid = FALSE;
			if( GEP(cellL.top,m_edgesOuter[1-SAS]) ) {
				i = MinMax( cellL.top,m_polygonRectL[P],m_polygonRectL[P+2]-1)-m_polygonRectL[P];
				if( GTS(cellL.left,m_ppEdges[0][i][1-SAS]) ) bLeaveTrapezioid = TRUE;
			} else
			if( LEP(cellL.bottom,m_edgesOuter[1-SAS]) ) {
				i = MinMax( cellL.bottom,m_polygonRectL[P],m_polygonRectL[P+2]-1)-m_polygonRectL[P];
				if( GTS(cellL.left,m_ppEdges[0][i][1-SAS]) ) bLeaveTrapezioid = TRUE;
			} else { // getting across outer point
				i = m_edgesOuter[1-SAS]-m_polygonRectL[P];
				ASSERT( i >= 0 && i < m_pnEdges[0] );
				if( GTS(cellL.left,m_ppEdges[0][i][1-SAS]) ) bLeaveTrapezioid = TRUE;
			}
			if( bLeaveTrapezioid ) {
				LOGCACHE("LPatch leaving trapezioid",PL,y);
				M_(bWaitPatch) = FALSE;
				M_(ylinkL) = cellL.top; //MAXP(m_rectL[P_TOP],ynewl);
				M_(ylinkU) = cellL.bottom;
				bResult = FALSE;
				//bCache = FALSE;
				//iCacheBrink = m_viewpointL[S]+(1-SAS);
				break;
			}
		#endif
		if(	GTS(cellL.right,m_viewpointL[S]+SAS) ) {
			M_(pCallLinkL) = M_(pCallSeal) = &CACHE_PL[y];
			M_L(pCallLinkU) = &CACHE_PL[cellL.top];
			LOGCACHE("Call seal",PL,y);
			M_(ylinkL) = cellL.top;
			M_L(ylinkU) = cellL.bottom;
			/*if( LEP(M_(ylinkL),cellL.bottom) ) {
				?????????
				M_(ylinkL) = cellL.bottom; //m_viewpointL[P]+PAS; //(1-PAS);
				M_(xlinkL) = ????;
			}
			if( GEP(M_L(ylinkU),cellL.top) ) {
				M_L(ylinkU) = cellL.top; //m_viewpointL[P]+(1-PAS); //PAS;
				M_L(xlinkU) = ?????;
			}*/
			bResult = FALSE;
			break;
		}
		// reverse run link-through
		WRITELOG2("LPatch: link-through x=%d iCacheBrinkL_U=%d\n",x,iCacheBrinkL_U);
		iCacheBrink1L_U = x;
		ASSERT( LTS(x,iCacheBrinkL_U) );
		for( ; LES(iCacheBrinkL_U,cellL.right) ; ) {
			ASSERT( GTS( CACHE_SU[iCacheBrinkL_U].nextrun, iCacheBrinkL_U ) );
			int iCacheBrinkTempL_U = CACHE_SU[iCacheBrinkL_U].nextrun;
			WRITELOG1("LRun: link-through Temp=%d\n",iCacheBrinkTempL_U);
			CACHE_SU[iCacheBrinkL_U].nextrun = iCacheBrink1L_U;
			iCacheBrink1L_U = iCacheBrinkL_U;
			iCacheBrinkL_U = iCacheBrinkTempL_U;
			bCacheL_U = !bCacheL_U;
		}
		bool bCache1L_U = bCacheL_U;
		BeginDrawCell();
		PUTTRIANGLEMARK(2);
		// fill in horizontal
		for( xx = cellL.right ; GTS(xx,x) ; xx = CACHE_SU[xx].nxt ) {
			while( LTS(xx,iCacheBrink1L_U) ) {
				ASSERT( LTS( CACHE_SU[iCacheBrink1L_U].nextrun, iCacheBrink1L_U ) );
				iCacheBrink1L_U = CACHE_SU[iCacheBrink1L_U].nextrun;
				bCache1L_U = !bCache1L_U;
			}
			ASSERT( GES(xx,iCacheBrink1L_U) );
			if( !bCache1L_U )
				CREATECELLVERTEX_PS(y,xx,CACHE_SU[xx]);
			if( !bCache1L_U || xx == iCacheBrink1L_U ) {
				dd = 1<<(int)REDUCTIONCELL_PS(y,SUBS(xx,1));
				CACHE_SU[xx].nxt = MAXS(x,_NEXTS(xx,dd));
			}
			LOGCACHE("LPatch read",SU,xx);
			ASSERT(CACHE_SU[xx].L[P] == y );
			AddCellVertex( CACHE_SU[xx] );
			ASSERT( LTS(CACHE_SU[xx].nxt,xx) );
		}
		ASSERT( xx == x );
		// fill in vertical
		PUTTRIANGLEMARK(3);
		for( yy = y ; GTP(yy,y1) ; yy = CACHE_PL[yy].nxt ) {
			LOGCACHE("LPatch read",PL,yy);
			ASSERT( CACHE_PL[yy].L[S] == x );
			AddCellVertex( CACHE_PL[yy] );
			ASSERT( LTP(CACHE_PL[yy].nxt,yy) );
		}
		ASSERT( yy == y1 && CACHE_PL[y1].L[S] == x );
		PUTTRIANGLEMARK(0);
		LOGCACHE("LPatch read",PL,y1);
		AddCellVertex( CACHE_PL[y1] );
		// CCellVertex *pStartNew = CACHE_PL[y1];
		if( !bCacheL_U ) CACHE_SU[cellL.right].rc = (int)REDUCTIONCELL_PS(SUBP(y,1),cellL.right);
		CACHE_PL[y] = CACHE_SU[cellL.right]; // copy it here, for later it will be deleted
								// !!!!!!! Not the case for patch
		LOGCACHE("LPatch copy",PL,y);
		// copy horizontal from lower (fill in new horizontal in LRun)

		for( xx = x ;; ) {
			if( !bCacheL_L || GES(ADDS(xx,1),iCacheBrinkL_L)) {
				dd = 1<<(int)REDUCTIONCELL_PS(SUBP(y1,1),xx);
				CACHE_SL[xx].nxt = MINS(cellL.right,NEXTS_(xx,dd));
			}
			ASSERT( GTS( CACHE_SL[xx].nxt,xx) );
			xx = CACHE_SL[xx].nxt;
			while( GES(xx,iCacheBrinkL_L) ) {
				ASSERT( GTS( CACHE_SL[iCacheBrinkL_L].nextrun, iCacheBrinkL_L ) );
				iCacheBrinkL_L = CACHE_SL[iCacheBrinkL_L].nextrun;
				bCacheL_L = !bCacheL_L;
				WRITELOG3("LPatch cache flip to %d at %d,%d\n",bCacheL_L,xx,y);
			}
			ASSERT( LTS(xx,iCacheBrinkL_L) );
			if( !bCacheL_L ) CREATECELLVERTEX_PS(y1,xx,CACHE_SL[xx]);
			LOGCACHE("LPatch read",SL,xx);
			ASSERT(CACHE_SL[xx].L[P] == y1 );
			if( GES(xx,cellL.right) ) break;
			AddCellVertex( CACHE_SL[xx] );
		}
		ASSERT( xx == cellL.right );
		if( !bCacheL_L ) {
			CACHE_SL[xx].rc = (int)REDUCTIONCELL_PS(y1,xx);
			dd = Min(CACHE_SL[xx].rc,(int)REDUCTIONCELL_PS(SUBP(y1,1),xx));
			CACHE_SL[xx].nxt = NEXTS_(xx,dd);
				// MINS(ADDS(x,1<<CACHE_SL[xx].rc),ADDS(xx,1<<(int)REDUCTIONCELL_PS(SUBP(y1,1),xx)));
		}


		PUTTRIANGLEMARK(1);
		ASSERT(CACHE_SL[xx].L[P]==y1);
		nxt = CACHE_PL[y1].nxt;
		CACHE_PL[y1] = CACHE_SL[xx];
		CACHE_PL[y1].nxt = nxt;
		AddCellVertex(CACHE_PL[y1]);
		rrc = CACHE_PL[y1].rc;
		// fill in new vertical
		dd = 1<<CACHE_PL[y].rc;
		int y11 = _NEXTP(y,dd); // Last point to fill in
		for( nxt = yy = y1 ;;) {
			if( GEP(yy,y11) ) goto EndNewVert;
			dd = 1<<(rc=CACHE_PL[yy].rc);
			WRITELOG1("Advance by %d\n",dd);
			yy = MINP(NEXTP_(yy,dd),y11);
			if( GTP( yy, m_viewpointL[P]+PAS ) ) break;
			CREATECELLVERTEX_PS(yy,xx,CACHE_PL[yy]);
			CACHE_PL[yy].rc=(int)REDUCTIONCELL_PS(yy,xx);
			CACHE_PL[yy].nxt = nxt; nxt = yy;
			LOGCACHE("LPatch low create low",PL,yy);
			AddCellVertex(CACHE_PL[yy]);
		}
		WRITELOG1("LPatch low create break at %d\n",yy);
		if( LEP(yy,y11) ) for(;;) {
			CREATECELLVERTEX_PS(yy,xx,CACHE_PL[yy]);
			CACHE_PL[yy].nxt = nxt;
			nxt = yy; CACHE_PL[yy].rc = rc;  //!!!!! Reduction computed for wrong yy below the viewpoint !!!!
			LOGCACHE("LPatch low create",PL,yy);
			AddCellVertex(CACHE_PL[yy]);
			if( GEP(yy,y11) ) break;
			dd = 1<<(rc=(int)REDUCTIONCELL_PS(yy,xx));
			yy = MINP(NEXTP_(yy,dd),y11);
		}
		EndNewVert:
		//ASSERT(yy==y11||yy==y1);
		CACHE_PL[y].nxt = nxt;
		ENDDRAWCELL(rc0);
		x = cellL.right;
		bResult = TRUE;
		ynewl = y1;
		M_(ynewL) = y;
		M_L(ynewR) = y1;
		CACHE_SU[x].L[P] = cellL.top; CACHE_SL[x].L[P] = cellL.bottom;
		IF_DEBUG(CACHE_SU[x].L[S]=CACHE_SL[x].L[S]=x);
		rc0 = rrc;
	}

ContinueRun:
	y = NEXTP_(y1,d);
	//CACHE_SU[x].L[P] = y1; // Workaround for correct run continuation
	//CACHE_SL[x].L[P] = y;
	ASSERT( LES(iCacheBrinkL_U,m_viewpointL[S]+(1-SAS)) );
	ASSERT( bCacheStartL_U || bFirstEntry );
	if( !bCacheL_U ) {
		if( LTS(ADDS(x,1),iCacheBrinkL_U) ) {
			CACHE_SU[ADDS(x,1)].nextrun = iCacheBrinkL_U;
			*piCacheL_U = ADDS(x,1);
		} else
		if( GES(iCacheBrinkL_U,m_viewpointL[S]+(1-SAS)) )
			*piCacheL_U = m_viewpointL[S]+(1-SAS);
		else {
			ASSERT( ADDS(x,1) == iCacheBrinkL_U );
			CACHE_SU[iCacheBrinkL_U].nxt = x;
			*piCacheL_U = CACHE_SU[iCacheBrinkL_U].nextrun;
		}
	}
	else {
		*piCacheL_U = iCacheBrinkL_U;
	}

	ASSERT( LES(iCacheBrinkL_L,m_viewpointL[S]+(1-SAS)) );
	ASSERT( bCacheStartL_L || bFirstEntry );
	if( !bCacheL_L ) {
		if( LTS(ADDS(x,1),iCacheBrinkL_L) ) {
			CACHE_SL[ADDS(x,1)].nextrun = iCacheBrinkL_L;
			*piCacheL_L = ADDS(x,1);
		} else
		if( GES(iCacheBrinkL_L,m_viewpointL[S]+(1-SAS)) )
			*piCacheL_L = m_viewpointL[S]+(1-SAS);
		else {
			ASSERT( ADDS(x,1) == iCacheBrinkL_L );
			CACHE_SL[x].nxt = iCacheBrinkL_L;
			*piCacheL_L = CACHE_SL[iCacheBrinkL_L].nextrun;
		}
	}
	else {
		*piCacheL_L = iCacheBrinkL_L;
	}



	#if	0 //def _DEBUG
	{
		WRITELOG("### LPATCH RUN:");
		for( int i_ = M_(iCacheL) ; LTS(i_,m_viewpointL[S]+(1-SAS)) ;
											i_ = CACHE_SU[i_].nextrun ) {
			WRITELOG1(" %d", i_ );
			ASSERT( GTS(CACHE_SU[i_].nextrun, i_ ) );
		}
		ASSERT( i_ == m_viewpointL[S]+(1-SAS) );
		WRITELOG1(" %d\n", i_ );
	}
	#endif

	#if	0 //def _DEBUG
	{
		WRITELOG("### LPATCH RUN:");
		for( int i_ = M_L(iCacheU) ; LTS(i_,m_viewpointL[S]+(1-SAS)) ;
											i_ = CACHE_SL[i_].nextrun ) {
			WRITELOG1(" %d", i_ );
			ASSERT( GTS(CACHE_SL[i_].nextrun, i_ ) );
		}
		ASSERT( i_ == m_viewpointL[S]+(1-SAS) );
		WRITELOG1(" %d\n", i_ );
	}
	#endif

	if( GES(M_(xedgeL),m_viewpointL[S]+(1-SAS)) || GTS(x,M_(xedgeL)) )	M_(xedgeL) = x;
	if( GES(M_L(xedgeU),m_viewpointL[S]+(1-SAS)) || GTS(x,M_L(xedgeU)) )	M_L(xedgeU) = x;

	return bResult;
}
#endif



// ****************************************




/*inline*/ bool _CViewTerrain::DRAW4(LRun)(int &y,int &ylower,int ypatchU_L,TCint rectL[4]) //,int P,int S,int PAS,int SAS)
{
	(void)ypatchU_L;
Again:
	if( M_(pCallLinkL) && !M_(bStartedPatch) || M_(pCallPatchL) || LTP(y,m_viewpointL[P]+(1-PAS))  ) return FALSE;
	int x, xx, x1, d, dd, yy, y1, y0, nxt, rc, rc0;
	(void)x1;
	bool bCache;
	IF_DEBUG(bool bNewLine=FALSE);
	WRITELOG3("LRun: y=%d ylower=%d stall=%08X\n",y,ylower,M_(pStallL));
	int iNewCacheL = M_(iCacheL), i;
	CRect2	cellL;
	int		lastStartL[2];
	lastStartL[S] = M_(lastStartL)[S]; lastStartL[P] = M_(lastStartL)[P];
	if( LEP(y,ylower) ) { // Need to create new hash start (vert)
		IF_DEBUG(bNewLine=TRUE);
		int nRdL;
		#if _ALIGN_EDGES
			i = MinMax(y+(PAS-1)-m_polygonRectL[P],0L,m_polygonRectL[P+2]-m_polygonRectL[P]-1);
			nRdL = m_pEdgeR[i][SAS]; d = 1<<nRdL;
			ASSERT( LEP(y,NEXTP_(m_polygonRectL[P]+i,d)) ); // y does not exceed cell found
			if( LEP(ylower,m_rectL[P_TOP]) || LEP( ALIGNP_(y,d), rectL[P+PAS*2] ) ) { // if out of polygon (lower)
		#else
			if( LEP(ylower,m_rectL[P_TOP]) || LEP(y,rectL[P_TOP]) ) {
		#endif
			if(!M_(pStallL)) { // if not stalled do nothing
				//M_(bCallPatchL) = TRUE;
				M_(bWaitPatch) = FALSE;
				M_(ylinkL) = y = SUBP(y,1<<nRdL);
				//if( LEP( y, m_viewpointL[P]+PAS ) )	M_(bCallPatchL) = TRUE;
				return _NODRAW_BREAK; //TRUE; //FALSE;
			}
			x = M_(pStallL)->L[S]; y = M_(pStallL)->L[P]; // else continue from stall
			nRdL = M_(pStallL)->rc;
			ASSERT( nRdL == (int)REDUCTIONCELL_PS(SUBP(y,1),x) );
			d = 1<<nRdL;
			ASSERT( GTP(y,m_rectL[P_TOP]) );
		}
		else { // if inside polygon
			#if _ALIGN_EDGES
				i = ((y+PAS-1)>>nRdL)-(m_polygonRectL[P]>>nRdL);
				ASSERT( i >= 0 && i < m_pnEdges[nRdL] );
				int left = m_ppEdges[nRdL][i][SAS]; // find polygon's left edge
				WRITELOG1("LRun: left=%d\n",left);
			#else
				i = MinMax(y+(PAS-1)-m_polygonRectL[P],0L,m_polygonRectL[P+2]-m_polygonRectL[P]-1);
				y = m_polygonRectL[P]+i-(PAS-1);
				int left = m_ppEdges[0][i][SAS];
			#endif
			x = MAXS(left,(int)rectL[S_LEFT]); // clip by rect
			#if !_ALIGN_EDGES
				nRdL = (int)REDUCTIONCELL_PS(SUBP(y,1),x); d = 1<<nRdL;
				left = x = _ALIGNS(x,d); y0 = ALIGNP_(y,d);
				int lastY = MAXP(m_viewpointL[P]+PAS,rectL[P_TOP]);
				for(;;) {
					WRITELOG2("LRun: left=%d y=%d\n",x,y);
					y1 = MAXP(_NEXTP(y,d),lastY/*m_viewpointL[P]+PAS*/);
					for( yy = SUBP(y,1) ; GTP(yy,y1) ; yy = SUBP(yy,1) ) {
						xx = MAXS(m_ppEdges[0][yy+(PAS-1)-m_polygonRectL[P]][SAS],(int)rectL[S_LEFT]);
						if( LTS(xx,x) ) break;
					}
					if( LEP(yy,y1) ) break;
					y = yy; x = xx;
					nRdL = (int)REDUCTIONCELL_PS(SUBP(y,1),x); d = 1<<nRdL;
					x = _ALIGNS(x,d);
				}
				y = ALIGNP_(y,d);
				if( LTP(y,y0) ) {
					//ASSERT( LTP( y, y0 ) );
					bCache = FALSE; left = ADDS(left,1);
					for( xx = M_(iCacheL) ; LTS(xx,m_viewpointL[S]+(1-SAS)) ; x1 = xx, xx = CACHE_SU[xx].nextrun, bCache = !bCache ) {
						ASSERT(GTS(CACHE_SU[xx].nextrun,xx));
						if( GTS(xx,left) ) break;
					}
					if( bCache ) {
						xx = SUBS(xx,1);
						for( int x2 = x1 ; LTS(x2,left) ; ) {
							x2 = ADDS(x2,1<<CACHE_SU[x2].rc);
							if( GES(x2,m_viewpointL[S]+SAS) ) { M_(iCacheL)=m_viewpointL[S]+(1-SAS); goto EndCacheErase; }
							x2 = MINS(x2,xx);
						}
						for( ; GTS(x2,left) ; x2 = x1 ) {
							x1 = CACHE_SU[x2].nxt;
							if( LTS(x1,left) ) break;
						}
						ASSERT( GES(x2,left) );
						CACHE_SU[M_(iCacheL) = x2].nextrun = ADDS(xx,1);
					}
					else
						M_(iCacheL) = xx;
					EndCacheErase:
					WRITELOG1("LRun: trapezioid skip M_(iCacheL)=%d\n",M_(iCacheL));
				} // if ( y != y0
			#endif
		}
		#if _ALIGN_EDGES
			/*if( x != left )*/ nRdL = (int)REDUCTIONCELL_PS(SUBP(y,1),x);
			d = 1<<nRdL;
			x = _ALIGNS(x,d);
		#endif
		// !!!!! ASSERT( y < ylower || (y & -d) == y || x < M_(iCacheL) || CACHE_SU[x].yL == y );
		if( M_(pStallL) ) {
			if( LTP(M_(pStallL)->L[P],SUBP(y,d)) ) {
				ASSERT(0); // ??? can we really get here
				y = SUBP(y,d); goto Again; /*return TRUE;*/
			}
			if( LTS(M_(pStallL)->L[S],x) ) { // if we get past stall due to polygon edge
				x = M_(pStallL)->L[S]; // start drawing from the stall
				y = M_(pStallL)->L[P];
				d = 1<<(nRdL=M_(pStallL)->rc);
			}
		}
		#if _ALIGN_EDGES
			y = ALIGNP_(y,d);
		#else
			ASSERT( y == ALIGNP_(y,d) );
		#endif
		cellL.bottom = MINP( m_rectL[P_BOTTOM], y );
		cellL.top = MAXP( m_rectL[P_TOP], _NEXTP(y,d) );
		ASSERT( GTP(cellL.bottom,cellL.top) );
		y = cellL.bottom;
		x = cellL.left = MAXS( m_rectL[S_LEFT], x );

		cellL.bottom = MINP(cellL.bottom,M_(ylinkL));
		if( LTS( x, lastStartL[S] ) && GTP( cellL.bottom, lastStartL[P] ) )	cellL.bottom = lastStartL[P];
		bCache = TRUE;
		for( int iCache = M_(iCacheL) ;	LTS(iCache,m_viewpointL[S]+(1-SAS)); iCache = CACHE_SU[iCache].nextrun, bCache = !bCache ) if( bCache && GES(iCache,x) )
			if( LTP(CACHE_SU[iCache].L[P],cellL.bottom) ) cellL.bottom = CACHE_SU[iCache].L[P]; else break;
		ASSERT(GTP(cellL.bottom,cellL.top));
		y0 = y = cellL.bottom;

		if( GTS(x,M_(xlinkL)) )	{
			WRITELOG2("LRun: x=%d cancelled by xlink=%d\n",x,M_(xlinkL));
			//if( GTS(x,m_viewpointL[S]+SAS) || GES(x,rectL[S+2*(1-SAS)]) ) { M_(bStallL) = FALSE; M_(pStallL) = NULL; M_(iCacheL) = m_viewpointL[S]+(1-SAS); M_(ylinkL) = y = _NEXTP(y,d); }
			return FALSE;
		}
		#if _PATCH
			if( LEP( cellL.top /*SUBP(y,d)*/, m_viewpointL[P]+PAS ) ) {
				M_(callPatchL).L[S] = x;
				M_(callPatchL).L[P] = y;
				M_(callPatchL).rc = nRdL;
				M_(pCallPatchL) = &M_(callPatchL);
				WRITELOG4("LRun init patch setup at x=%d y=%d d=%d top=%d\n",x,y,d,cellL.top);
				if( GTS(x,m_viewpointL[S]+SAS) ) { M_(bStallL) = FALSE; M_(pStallL) = NULL; M_(iCacheL) = m_viewpointL[S]+(1-SAS);/*y = SUBP(y,d);*/ return FALSE; }
										// y = SUBP(y,d) commented out for else it would cause
										// incorrect entry ynewl for LPatch
				return FALSE;
			}
		#endif
		//if(	LEP(y,ypatchU_L) && GTS(x,M_L(pCallPatchU)->L[S]) ) { y = _NEXTP(y,d); M_(iCacheL) = m_viewpointL[S]+(1-SAS); return FALSE; }
		M_(lastStartL)[S] = x; M_(lastStartL)[P] = cellL.top;
		if( GTS(x,m_viewpointL[S]+SAS) || GES(x,rectL[S+2*(1-SAS)]) ) { M_(bStallL) = FALSE; M_(pStallL) = NULL; M_(iCacheL) = m_viewpointL[S]+(1-SAS); M_(ylinkL) = y = _NEXTP(y,d); return FALSE; }

		//cellL.right = MINS( m_rectL[S_RIGHT], ADDP(x,d) );
		ylower = cellL.top; //SUBP(y,d);
		//x = cellL.left;
		CREATECELLVERTEX_PS(cellL.top,x,CACHE_SU[x]);
		CACHE_SU[x].rc = (int)REDUCTIONCELL_PS(SUBP(cellL.top,1),x);
		LOGCACHE("LRun start",SU,x);
		iNewCacheL = x;
		for(nxt=yy=cellL.top;;) {
			dd = 1<<(int)REDUCTIONCELL_PS(yy,SUBS(x,1));
			yy = MINP(y,NEXTP_(yy,dd));
			CREATECELLVERTEX_PS(yy,x,CACHE_PL[yy]);
			CACHE_PL[yy].rc = nRdL;
			CACHE_PL[yy].nxt = nxt; nxt = yy;
			LOGCACHE("LRun start",PL,yy);
			if( GEP(yy,y0) ) break;
		}
		//if( GTS(ADDS(x,d),m_viewpointL[S]+SAS) ) { y = ylower; M_(pCallLinkL)=&CACHE_PL[y0]; return TRUE; }
		//iCacheLNew = x;
	}
	M_(pStallL) = NULL;


	//Need to set flag whether new run shall be init
	//Then compute iCacheBrink and upper clipping
	//Then init new run

	x = CACHE_PL[y].L[S];
	LOGCACHE("LRun turnpoint",PL,y);
	ASSERT( GES( x, m_rectL[S_LEFT] ) );
	//int ynewl = CACHE_SU[x].L[P];
	d = 1<<CACHE_PL[y].rc;
	ASSERT( CACHE_SU[x].L[S]==x && LTP(CACHE_SU[x].L[P],y) );
	int ynewl = MAXP(_NEXTP(y,d),m_rectL[P_TOP]);
	WRITELOG1("ynewl=%d\n",ynewl);
	y1 = CACHE_SU[x].L[P];
	//ASSERT( ynewl == _ALIGNP(SUBP(y,1),d) );
	#if _PATCH
		if( GEP(ynewl,y1) ) {
			if( LEP( MAXP(ynewl,m_rectL[P_TOP]), m_viewpointL[P]+PAS ) ) {
			ContPatch:
				WRITELOG1("LRun patch setup at ynewl=%d\n",ynewl);
				M_(callPatchL).L[S] = x;
				M_(callPatchL).L[P] = y;
				M_(callPatchL).rc = CACHE_PL[y].rc;
				M_(pCallPatchL) = &CACHE_PL[y];
				return FALSE;
			}
		}
		else {
			WRITELOG2("LRun: stall restore at %d,%d\n",CACHE_PL[y].L[S],CACHE_PL[y].L[P]);
			M_(bStallL) = TRUE; M_(pStallL) = &CACHE_PL[y];
			if( LEP( MAXP(y1,m_rectL[P_TOP]), m_viewpointL[P]+PAS ) ) goto ContPatch;
			y = y1; goto Again;
		}
	#endif
	y1 = y;

	bool bResult = TRUE; //FALSE;


	#ifdef _DEBUG
	{
		WRITELOG("### LCACHE RUN:");
		for( int i_ = M_(iCacheL) ; LTS(i_,m_viewpointL[S]+(1-SAS)) ;
											i_ = CACHE_SU[i_].nextrun ) {
			WRITELOG1(" %d", i_ );
			ASSERT( GTS(CACHE_SU[i_].nextrun, i_ ) );
		}
		ASSERT( i_ == m_viewpointL[S]+(1-SAS) );
		WRITELOG1(" %d\n", i_ );
	}
	#endif


	bCache = FALSE;
	int		iCacheBrink = M_(iCacheL), *piCacheL = &M_(iCacheL);
	while( LES(iCacheBrink,x) ) {
		ASSERT( GTS(CACHE_SU[iCacheBrink].nextrun,iCacheBrink) );
		piCacheL = &CACHE_SU[iCacheBrink].nextrun;
		WRITELOG1("LRun: piCacheL->%d\n",iCacheBrink);
		iCacheBrink = CACHE_SU[iCacheBrink].nextrun;
		bCache = !bCache;
	}
	ASSERT( *piCacheL == iCacheBrink );
	if( iNewCacheL != M_(iCacheL) ) {
		ASSERT(iNewCacheL==x);
		piCacheL = &CACHE_SU[iNewCacheL].nextrun;
		WRITELOG1("LRun: piCacheL->%d\n",iNewCacheL);
	}
	ASSERT( piCacheL != &M_(iCacheL) );
	M_(iCacheL)=iNewCacheL; //x;
	bool	bCacheStart = bCache;
	int		iCacheStart = x;


	// Drawing from left to center
	for( int cnt = 0 ;; cnt++ ) {
		if( GES(x,rectL[S+2*(1-SAS)]) ) {
			ASSERT( cnt > 0 );	// If we drew nothing it's not correct cache setting below
			bCache = FALSE;
			iCacheBrink = m_viewpointL[S]+(1-SAS);
			break;
			#if 0
			*piCacheL = x;
			CACHE_SU[x].nextrun = m_viewpointL[S]+(1-SAS);
			WRITELOG2("### LRUN CACHE SET: %d -> %d -> viewpoint\n",M_(iCacheL),x);
			y = ynewl;
			return bResult;	// Stop if came to border
			#endif
		}
		for(y1=y;;) {	// Wait util we get into the drawing rect from upper border
						// This is necessary if previous cell was bigger
			d=1<<(rc0=rc=CACHE_PL[y1].rc);	// Cell size
			y1 = _NEXTP(y1,d);		// Lower edge
			if( LTP(y1,rectL[P+2*(1-PAS)]) ) break;	// Got in? Then break
			if( LTP(y1,ynewl) ) goto End; // Can we really get here ???
		}
		y1 = cellL.top = MAXP( y1, m_rectL[P_TOP] );
		cellL.bottom = MINP( NEXTP_(y1,d), m_rectL[P_BOTTOM] );
		int iCacheBrink1;
		if( LTS(iCacheBrink,m_viewpointL[S]+(1-SAS)) ) {
			if( !bCache ) cellL.bottom = MINP( cellL.bottom, CACHE_SU[iCacheBrink].L[P] ); else
			if( ADDS(x,1) == iCacheBrink && LTS(iCacheBrink1=CACHE_SU[iCacheBrink].nextrun,m_viewpointL[S]+(1-SAS)) )
				cellL.bottom = MINP( cellL.bottom, CACHE_SU[iCacheBrink1].L[P] );
		}
		cellL.bottom = MINP( cellL.bottom, M_(ylinkL) );
		//if( LTS( x, lastStartL[S] ) && GTP( cellL.bottom, lastStartL[P] ) )	cellL.bottom = lastStartL[P];
		ASSERT( GTP( cellL.bottom, cellL.top ) );
		if( !bCache ) {		// If no cache, create cache
			// y = upper edge of the previous cell
			for( yy = cellL.bottom ; GTP(yy,y) ; ) {
				CREATECELLVERTEX_PS(yy,x,CACHE_PL[yy]);
				CACHE_PL[yy].rc = rc;
				dd = 1<<(int)REDUCTIONCELL_PS(SUBP(yy,1),SUBS(x,1));
				yy = CACHE_PL[yy].nxt = MAXP(y,_NEXTP(yy,dd));
			}
			ASSERT( yy == y || yy == cellL.bottom );
		}
		y = cellL.bottom;	// Upper edge
		if( LTP(y1,ynewl) ) {
			WRITELOG2("LRun: stall at %d,%d\n",CACHE_PL[y].L[P],CACHE_PL[y].L[S]);
			M_(ypatchL) = MINP( M_(ypatchL), y1 );
			WRITELOG1("ypatch=%d\n",M_(ypatchL));
			M_(bStallL) = TRUE; M_(pStallL) = &CACHE_PL[y]; /*ASSERT(M_(pStallL)->rc>0);*/ break;
		}
		cellL.left = x;
		cellL.right = MINS( NEXTS_(x,d), m_rectL[S_RIGHT] );
		ASSERT( GTS( cellL.right, cellL.left ) );
		if( GTS(cellL.right,M_(xlinkL)) ) {
			ASSERT( LES(cellL.left,M_(xlinkL)) );
			ASSERT( LES(M_(xlinkL), M_(xedgeL)) );
			M_(xlinkL) = cellL.left;
			ynewl = cellL.top;
			WRITELOG4("LRun: cellL.right=%d cancelled by xlink=%d, cell.left=%d ynewl=%d\n",cellL.right,M_(xlinkL),cellL.left,ynewl);
			break;
		}
		#if _LEAVE_TRAPEZIOID
			// Trapezioid check
			bool bLeaveTrapezioid = FALSE;
			if( GEP(cellL.top,m_edgesOuter[1-SAS]) ) {
				i = MinMax( cellL.top,m_polygonRectL[P],m_polygonRectL[P+2]-1)-m_polygonRectL[P];
				if( GTS(cellL.left,m_ppEdges[0][i][1-SAS]) ) bLeaveTrapezioid = TRUE;
			} else
			if( LEP(cellL.bottom,m_edgesOuter[1-SAS]) ) {
				i = MinMax( cellL.bottom,m_polygonRectL[P],m_polygonRectL[P+2]-1)-m_polygonRectL[P];
				if( GTS(cellL.left,m_ppEdges[0][i][1-SAS]) ) bLeaveTrapezioid = TRUE;
			} else { // getting across outer point
				i = m_edgesOuter[1-SAS]-m_polygonRectL[P];
				ASSERT( i >= 0 && i < m_pnEdges[0] );
				if( GTS(cellL.left,m_ppEdges[0][i][1-SAS]) ) bLeaveTrapezioid = TRUE;
			}
			if( bLeaveTrapezioid ) {
				LOGCACHE("LRun leaving trapezioid",PL,y);
				M_(bStallL) = FALSE;
				M_(ylinkL) = ynewl = cellL.top; //MAXP(m_rectL[P_TOP],ynewl);
				//bCache = FALSE;
				//iCacheBrink = m_viewpointL[S]+(1-SAS);
				break;
			}
		#endif
		if(	GTS(cellL.right,m_viewpointL[S]+SAS) ) {
			M_(pCallLinkL) = &CACHE_PL[y];
			LOGCACHE("LRun link set",PL,y);
			M_(bStallL) = FALSE; M_(xlinkL) = cellL.left;
			M_(ylinkL) = ynewl = cellL.top; //MAXP(m_rectL[P_TOP],ynewl);
			M_(xedgeL) = cellL.left;
			//ynewl = MAXP(m_rectL[P_TOP],SUBP(y,1<<CACHE_PL[y].rc));
			break;
		}
		// reverse run link-through
		WRITELOG2("LRun: link-through x=%d iCacheBrink=%d\n",x,iCacheBrink);
		iCacheBrink1 = x;
		ASSERT( LTS(x,iCacheBrink) );
		for( ; LES(iCacheBrink,cellL.right) ; ) {
			int iCacheBrinkTemp = CACHE_SU[iCacheBrink].nextrun;
			WRITELOG1("LRun: link-through Temp=%d\n",iCacheBrinkTemp);
			CACHE_SU[iCacheBrink].nextrun = iCacheBrink1;
			iCacheBrink1 = iCacheBrink;
			iCacheBrink = iCacheBrinkTemp;
			bCache = !bCache;
		}
		bool bCache1 = bCache;
		BeginDrawCell();
		PUTTRIANGLEMARK(2);
		// fill in horizontal
		for( xx = cellL.right ; GTS(xx,x) ; xx = CACHE_SU[xx].nxt ) {
			while( LTS(xx,iCacheBrink1) ) {
				iCacheBrink1 = CACHE_SU[iCacheBrink1].nextrun;
				bCache1 = !bCache1;
			}
			ASSERT( GES(xx,iCacheBrink1) );
			if( !bCache1 )
				CREATECELLVERTEX_PS(y,xx,CACHE_SU[xx]);
			if( !bCache1 || xx == iCacheBrink1 ) {
				dd = 1<<(int)REDUCTIONCELL_PS(y,SUBS(xx,1));
				CACHE_SU[xx].nxt = MAXS(x,_NEXTS(xx,dd));
			}
			LOGCACHE("LRun read",SU,xx);
			ASSERT(CACHE_SU[xx].L[P] == y );
			AddCellVertex( CACHE_SU[xx] );
			ASSERT( LTS(CACHE_SU[xx].nxt,xx) );
		}
		ASSERT( xx == x );
		// fill in vertical
		PUTTRIANGLEMARK(3);
		for( yy = y ; GTP(yy,cellL.top) ; yy = CACHE_PL[yy].nxt ) {
			LOGCACHE("LRun read",PL,yy);
			ASSERT( CACHE_PL[yy].L[S] == x );
			AddCellVertex( CACHE_PL[yy] );
			ASSERT( LTP(CACHE_PL[yy].nxt,yy) );
		}
		ASSERT( yy == cellL.top );
		PUTTRIANGLEMARK(0);
		CCellVertex *pStartNew;
		if( CACHE_SU[x].L[P] == cellL.top )
			LOGCACHE("LRun read startnew",SU,x);
		else
			LOGCACHE("LRun read startnew",PL,cellL.top);
		AddCellVertex( *(pStartNew = CACHE_SU[x].L[P] == cellL.top ? &CACHE_SU[x] : &CACHE_PL[cellL.top]) );
		if( !bCache ) CACHE_SU[cellL.right].rc = (int)REDUCTIONCELL_PS(SUBP(y,1),cellL.right);
		CACHE_PL[y] = CACHE_SU[cellL.right]; // copy it here, for later it will be deleted
		LOGCACHE("LRun copy",PL,y);
		// fill in new horizontal
		nxt = x;
		dd = 1<<pStartNew->rc;
		for( xx = NEXTS_(nxt,dd) ; ; ) {
			xx = MINS(cellL.right,xx);
			CREATECELLVERTEX_PS(cellL.top,xx,CACHE_SU[xx]);
			CACHE_SU[xx].rc = REDUCTIONCELL_PS(SUBP(cellL.top,1),xx);
			CACHE_SU[xx].nxt = nxt; nxt = xx;
			LOGCACHE("LRun create",SU,xx);
			if( GES(xx,cellL.right) )	break;
			AddCellVertex(CACHE_SU[xx]);
			dd = 1<<CACHE_SU[xx].rc;
			xx = NEXTS_(xx,dd);
		}
		ASSERT( xx == cellL.right );
		PUTTRIANGLEMARK(1);
		AddCellVertex(CACHE_SU[xx]);
		// fill in new vertical
		dd = 1<<CACHE_PL[y].rc;
		int y11 = _NEXTP(y,dd); // Last vertex to fill in
			// because corner point was already filled at the beginning
		for( nxt = yy = cellL.top ; LTP(yy,y11) ;) {
			dd=1<<(rc=(int)REDUCTIONCELL_PS(yy,xx));
			yy = MINP(NEXTP_(yy,dd),y11);
			CACHE_PL[yy].nxt = nxt;
			//if( GEP(yy,y) ) { ASSERT( yy == y ); break; }
			CREATECELLVERTEX_PS(yy,xx,CACHE_PL[yy]);
			nxt = yy; CACHE_PL[yy].rc = rc;
			LOGCACHE("LRun create",PL,yy);
			AddCellVertex(CACHE_PL[yy]);
		}
		//ASSERT(yy==y11||yy==y);
		CACHE_PL[y].nxt = nxt;
		ENDDRAWCELL(rc0);
		x = cellL.right;
		bResult = TRUE;
		ynewl = y1;
	} // left drawing loop
End:
	y = ynewl;


	ASSERT( LES(iCacheBrink,m_viewpointL[S]+(1-SAS)) );
	ASSERT( bCacheStart || bNewLine );
	if( !bCache ) {
		if( LTS(ADDS(x,1),iCacheBrink) ) {
			CACHE_SU[ADDS(x,1)].nextrun = iCacheBrink;
			*piCacheL =  ADDS(x,1);
		} else
		if( GES(iCacheBrink,m_viewpointL[S]+(1-SAS)) )
			*piCacheL = m_viewpointL[S]+(1-SAS);
		else {
			ASSERT( ADDS(x,1) == iCacheBrink );
			CACHE_SU[iCacheBrink].nxt = x;
			*piCacheL = CACHE_SU[iCacheBrink].nextrun;
		}
	}
	else {	 // if bCache
		*piCacheL = iCacheBrink;
	}

	WRITELOG1("LRun: M_(iCacheL)=%d\n",M_(iCacheL));

	#ifdef _DEBUG
	{
		WRITELOG("### LCACHE RUN:");
		for( int i_ = M_(iCacheL) ; LTS(i_,m_viewpointL[S]+(1-SAS)) ;
											i_ = CACHE_SU[i_].nextrun ) {
			WRITELOG1(" %d", i_ );
			ASSERT( GTS(CACHE_SU[i_].nextrun, i_ ) );
		}
		ASSERT( i_ == m_viewpointL[S]+(1-SAS) );
		WRITELOG1(" %d\n", i_ );
	}
	#endif

	if( GES(M_(xedgeL),m_viewpointL[S]+(1-SAS)) || GTS(x,M_(xedgeL)) )	M_(xedgeL) = x;

	//ASSERT(cnt>0);
	if( M_(pStallL) ) goto Again;
	return bResult;
}

/*inline*/ bool _CViewTerrain::DRAW4(RRun)(int &y,int &ylower,int ypatchL_L,TCint rectL[4])
{
	(void)ypatchL_L;
Again:
	if( M_(pCallLinkU) && !M_(bStartedPatch) || M_(pCallPatchU) || LTP(y,m_viewpointL[P]+(1-PAS)) ) return FALSE;
	int x, xx, d, dd, yy, y1, y0, x1, nxt, rc, rc0;
	bool bCache;
	IF_DEBUG(bool bNewLine=FALSE);
	WRITELOG2("RRun: y=%d ylower=%d\n",y,ylower);
	int iNewCacheU = M_(iCacheU), i;
	CRect2	cellL;
	int		lastStartL[2];
	lastStartL[S] = M_(lastStartU)[S]; lastStartL[P] = M_(lastStartU)[P];
	if( LEP(y,ylower) ) { // Need to create new hash start
		IF_DEBUG(bNewLine=TRUE);
		int nRdR;
		#if _ALIGN_EDGES
			i = MinMax(y+(PAS-1)-m_polygonRectL[P],0L,m_polygonRectL[P+2]-m_polygonRectL[P]-1);
			nRdR = m_pEdgeR[i][1-SAS]; d = 1<<nRdR;
			ASSERT( LEP(y,NEXTP_(m_polygonRectL[P]+i,d)) );
			if( LEP(ylower,m_rectL[P_TOP]) || LEP( ALIGNP_(y,d), rectL[P+PAS*2] ) ) {
		#else
			if( LEP(ylower,m_rectL[P_TOP]) || LEP( y, rectL[P_TOP] ) ) {
		#endif
			if(!M_(pStallU)) {
				//M_(bCallPatchU) = TRUE;
				M_L(bWaitPatch) = FALSE;
				M_(ylinkU) = y = SUBP(y,d);
				//if( LEP( y, m_viewpointL[P]+PAS ) )	M_(bCallPatchU) = TRUE;
				return _NODRAW_BREAK; //TRUE; //FALSE;
			}
			x = M_(pStallU)->L[S]; y = M_(pStallU)->L[P];
			nRdR = M_(pStallU)->rc;
			ASSERT( nRdR == (int)REDUCTIONCELL_PS(SUBP(y,1),SUBS(x,1)) );
			d = 1<<nRdR;
			ASSERT( GTP(y,m_rectL[P_TOP]) );
		}
		else {
			#if _ALIGN_EDGES
				i = ((y+PAS-1)>>nRdR)-(m_polygonRectL[P]>>nRdR);
				ASSERT( i >= 0 && i < m_pnEdges[nRdR] );
				int right = m_ppEdges[nRdR][i][1-SAS];
				WRITELOG1("RRun: right=%d\n",right);
			#else
				i = MinMax(y+(PAS-1)-m_polygonRectL[P],0L,m_polygonRectL[P+2]-m_polygonRectL[P]-1);
				y = m_polygonRectL[P]+i-(PAS-1);
				int right = m_ppEdges[0][i][1-SAS];
			#endif
			x = MINS(right,(int)rectL[S_RIGHT]);
			#if !_ALIGN_EDGES
				nRdR = (int)REDUCTIONCELL_PS(SUBP(y,1),SUBS(x,1)); d = 1<<nRdR;
				right = x = ALIGNS_(x,d); y0 = ALIGNP_(y,d);
				int lastY = MAXP(m_viewpointL[P]+PAS,rectL[P_TOP]);
				for(;;) {
					WRITELOG2("RRun: right=%d y=%d\n",x,y);
					y1 = MAXP(_NEXTP(y,d),lastY/*m_viewpointL[P]+PAS*/);
					for( yy = SUBP(y,1) ; GTP(yy,y1) ; yy = SUBP(yy,1) ) {
						xx = MINS(m_ppEdges[0][yy+(PAS-1)-m_polygonRectL[P]][1-SAS],(int)rectL[S_RIGHT]);
						if( GTS(xx,x) ) break;
					}
					if( LEP(yy,y1) ) break;
					y = yy; x = xx;
					nRdR = (int)REDUCTIONCELL_PS(SUBP(y,1),SUBS(x,1)); d = 1<<nRdR;
					x = ALIGNS_(x,d);
				}
				y = ALIGNP_(y,d);
				if( LTP(y,y0) ) {
					//ASSERT( LTP( y, y0 ) );
					bCache = FALSE; right = SUBS(right,1);
					for( xx = M_(iCacheU) ; GTS(xx,m_viewpointL[S]+SAS) ; x1 = xx, xx = CACHE_SU[xx].nextrun, bCache = !bCache ) {
						ASSERT(LTS(CACHE_SU[xx].nextrun,xx));
						if( LTS(xx,right) ) break;
					}
					if( bCache ) {
						//xx = ADDS(xx,1);
						//What if right (or left) crosses viewpoint ???
						for( int x2 = MAXS(right,m_viewpointL[S]+SAS) ; GTS(x1,x2) ; x1 = CACHE_SU[x1].nxt )
							ASSERT( LTS( CACHE_SU[x1].nxt, x1 ) );
						//ASSERT( LES(x2,right) );
						if( LES( x1, m_viewpointL[S]+SAS ) )
							M_(iCacheU) = m_viewpointL[S]+SAS;
						else {
							ASSERT( LTS(xx,x1) );
							CACHE_SU[M_(iCacheU) = x1].nextrun = xx;
						}
					}
					else
						M_(iCacheU) = xx;
					WRITELOG1("RRun: trapezioid skip M_(iCacheU)=%d\n",M_(iCacheU));
				} // if ( y != y0
			#endif
		}
		#if _ALIGN_EDGES
			/*if( x != right )*/ nRdR = (int)REDUCTIONCELL_PS(SUBP(y,1),SUBS(x,1));
			d = 1<<nRdR;
			x = ALIGNS_(x,d); // x = (x+((1<<nRdR)-1)) & (-1<<nRdR);
		#endif
		//!!!! ASSERT( y < ylower || (y & -d) == y || x > M_(iCacheU) || CACHE_SU[x].yL == y );
		if( M_(pStallU) ) {
			if( LTP(M_(pStallU)->L[P],SUBP(y,d)) )	{
				ASSERT(0); // I forgot why I placed here this if, so let's find out
				y=SUBP(y,d); goto Again; /*return TRUE;*/
			}
			if( GTS(M_(pStallU)->L[S],x) ) {
				x = M_(pStallU)->L[S];
				y = M_(pStallU)->L[P];
				d = 1<<(nRdR=M_(pStallU)->rc);
			}
		}
		#if _ALIGN_EDGES
			y = ALIGNP_(y,d); // (y+d-1)&-d;
		#else
			ASSERT( y == ALIGNP_(y,d) );
		#endif
		cellL.bottom = MINP( m_rectL[P_BOTTOM], y );
		cellL.top = MAXP( m_rectL[P_TOP], _NEXTP(y,d) );
		y = cellL.bottom;
		ASSERT( GTP(cellL.bottom,cellL.top) );
		x = cellL.right = MINS( m_rectL[S_RIGHT], x );

		cellL.bottom = MINP(cellL.bottom,M_(ylinkU));
		if( GTS( x, lastStartL[S] ) && GTP( cellL.bottom, lastStartL[P] ) )	cellL.bottom = lastStartL[P];
		bCache = TRUE;
		for( int iCache = M_(iCacheU) ;	GTS(iCache,m_viewpointL[S]+SAS); iCache = CACHE_SU[iCache].nextrun, bCache = !bCache ) if( bCache && LES(iCache,x) )
			if( LTP(CACHE_SU[iCache].L[P],cellL.bottom) ) cellL.bottom = CACHE_SU[iCache].L[P]; else break;
		//cellL.left = MAXS( m_rectL[S_LEFT], ADDP(x,d) );
		y0 = y = cellL.bottom;

		if( LTS(x,M_(xlinkU)) )	{
			WRITELOG2("RRun: x=%d cancelled by xlink=%d\n",x,M_(xlinkU));
			//if( LES(x,m_viewpointL[S]+SAS) || LES(x,rectL[S+2*SAS]) ) { M_(bStallU) = FALSE; M_(pStallU) = NULL; M_(iCacheU) = m_viewpointL[S]+SAS; M_(ylinkU) = y = _NEXTP(y,d); }
			return FALSE;
		}
		#if _PATCH
			if( LEP( cellL.top /*SUBP(y,d)*/, m_viewpointL[P]+PAS ) ) {
				M_(callPatchU).L[S] = x;
				M_(callPatchU).L[P] = y;
				M_(callPatchU).rc = nRdR;
				M_(pCallPatchU) = &M_(callPatchU);
				WRITELOG4("RRun init patch setup at x=%d y=%d d=%d top=%d\n",x,y,d,cellL.top);
				if( LES(x,m_viewpointL[S]+SAS) ) { M_(bStallU) = FALSE; M_(pStallU) = NULL; M_(iCacheU) = m_viewpointL[S]+SAS;/*y = SUBP(y,d);*/ return FALSE; }
										// y = SUBP(y,d) commented out for else it would cause
										// incorrect entry ynewl for LPatch
				return FALSE;
			}
		#endif
		//if(	LEP(y,ypatchL_L) && LTS(x,M_L(pCallPatchL)->L[S]) ) { y = _NEXTP(y,d); M_(iCacheU) = m_viewpointL[S]+SAS; return FALSE; }
		M_(lastStartU)[S] = x; M_(lastStartU)[P] = cellL.top;
		if( LES(x,m_viewpointL[S]+SAS) || LES(x,rectL[S+2*SAS]) ) { M_(bStallU) = FALSE; M_(pStallU) = NULL; M_(iCacheU) = m_viewpointL[S]+SAS; M_(ylinkU) = y = _NEXTP(y,d); return FALSE; }

		ylower = cellL.top; //SUBP(y,d);
		//x = cellL.right;
		//int y0 = y;
		//ylower = SUBP(y,d);
		CREATECELLVERTEX_PS(ylower,x,CACHE_SU[x]);
		CACHE_SU[x].rc = (int)REDUCTIONCELL_PS(SUBP(ylower,1),SUBS(x,1));
		LOGCACHE("RRun start",SU,x);
		iNewCacheU = x;
		for(nxt=yy=cellL.top;;) {
			dd = 1<<(int)REDUCTIONCELL_PS(yy,x);
			yy = MINP(y,NEXTP_(yy,dd));
			CREATECELLVERTEX_PS(yy,x,CACHE_PU[yy]);
			CACHE_PU[yy].rc = nRdR;
			CACHE_PU[nxt].nxt = yy; nxt = yy;
			LOGCACHE("RRun start corr nxt",PU,nxt);
			LOGCACHE("RRun start",PU,yy);
			if( GEP(yy,y0) ) break;
		}
		//if( LES(SUBS(x,d),m_viewpointL[S]+SAS) ) { y = ylower; return TRUE; }
		//iCacheUNew = x;
	}
	M_(pStallU) = NULL;


	x = CACHE_PU[y].L[S];
	ASSERT( LES(x, m_rectL[S_RIGHT]) );
	//int ynewl = CACHE_SU[x].L[P];
	d = 1<<CACHE_PU[y].rc;
	ASSERT( CACHE_SU[x].L[S]==x && LTP(CACHE_SU[x].L[P],y) );
	y1 = CACHE_SU[x].L[P];
	int ynewl = MAXP(_NEXTP(y,d),m_rectL[P_TOP]);
	//if( LTP( ynewl, y1 ) )	{ y = y1; goto Again; }	// stall check
	LOGCACHE("RRun turnpoint",PU,y);
	WRITELOG1("ynewl=%d\n",ynewl);
	#if _PATCH
		if( GEP(ynewl,y1) ) {
			if( LEP( MAXP(ynewl,m_rectL[P_TOP]), m_viewpointL[P]+PAS ) ) {
			ContPatch:
				WRITELOG1("RRun patch setup at ynewl=%d\n",ynewl);
				M_(callPatchU).L[S] = x;
				M_(callPatchU).L[P] = y;
				M_(callPatchU).rc = CACHE_PU[y].rc;
				M_(pCallPatchU) = &M_(callPatchU);
				return FALSE;
			}
		}
		else {
			WRITELOG2("RRun: stall restore at %d,%d\n",CACHE_PU[y].L[S],CACHE_PU[y].L[P]);
			M_(bStallU) = TRUE; M_(pStallU) = &CACHE_PU[y];
			if( LEP( MAXP(y1,m_rectL[P_TOP]), m_viewpointL[P]+PAS ) ) goto ContPatch;
			y = y1; goto Again;
		}
	#endif
	y1 = y;
	bool bResult = TRUE; //FALSE;


	bCache = FALSE;
	int		iCacheBrink = M_(iCacheU), *piCacheU = &M_(iCacheU);
	while( GES(iCacheBrink,x) ) {
		ASSERT( LTS(CACHE_SU[iCacheBrink].nextrun,iCacheBrink) );
		piCacheU = &CACHE_SU[iCacheBrink].nextrun;
		iCacheBrink = CACHE_SU[iCacheBrink].nextrun;
		bCache = !bCache;
	}
	ASSERT( bCache || bNewLine );
	//ASSERT( *piCacheU == iCacheBrink );
	if( iNewCacheU != M_(iCacheU) ) {
		ASSERT(iNewCacheU==x);
		piCacheU = &CACHE_SU[iNewCacheU].nextrun;
	}
	ASSERT( piCacheU != &M_(iCacheU) );
	M_(iCacheU) = iNewCacheU; //x;
	bool	bCacheStart = bCache;
	int 	iCacheStart = x;


	// Drawing from right to center
	for( int cnt = 0 ;; cnt++ ) {
		if( LES(x,rectL[S+2*SAS]) ) {
			ASSERT( cnt > 0 );	// If we drew nothing it's not correct cache setting below
			bCache = FALSE;
			iCacheBrink = m_viewpointL[S]+SAS;
			break;
			#if 0
			CACHE_SU[M_(iCacheU)].nextrun = x;
			CACHE_SU[x].nextrun = m_viewpointL[S]+SAS;
			WRITELOG2("### RRUN CACHE SET: %d -> %d -> viewpoint\n",M_(iCacheU),x);
			y = ynewl;
			return bResult;	// Stop if came to border
			#endif
		}
		for(y1=y;;) {
			d=1<<(rc0=rc=CACHE_PU[y1].rc);
			y1 = _NEXTP(y1,d);
			if( LTP(y1,rectL[P+2*(1-PAS)]) )	break;
			if( LTP(y1,ynewl) ) goto End;
		}
		y1 = cellL.top = MAXP( y1, m_rectL[P_TOP] );
		cellL.bottom = MINP( NEXTP_(y1,d), m_rectL[P_BOTTOM] );
		int iCacheBrink1;
		if( GTS(iCacheBrink,m_viewpointL[S]+SAS) ) {
			if( !bCache ) cellL.bottom = MINP( cellL.bottom, CACHE_SU[iCacheBrink].L[P] ); else
			if( SUBS(x,1) == iCacheBrink && GTS(iCacheBrink1=CACHE_SU[iCacheBrink].nextrun,m_viewpointL[S]+SAS) )
				cellL.bottom = MINP( cellL.bottom, CACHE_SU[iCacheBrink1].L[P] );
		}
		cellL.bottom = MINP( M_(ylinkU), cellL.bottom );
		//if( GTS( x, lastStartL[S] ) && GTP( cellL.bottom, lastStartL[P] ) )	cellL.bottom = lastStartL[P];
		ASSERT( GTP( cellL.bottom, cellL.top ) );
		if( !bCache ) {
			for( yy = y ; LTP(yy,cellL.bottom) ; ) {
				dd = 1<<(int)REDUCTIONCELL_PS(yy,x);
				yy = CACHE_PU[yy].nxt = MINP(cellL.bottom,NEXTP_(yy,dd));
				CREATECELLVERTEX_PS(yy,x,CACHE_PU[yy]);
				CACHE_PU[yy].rc = rc;
			}
			ASSERT( yy == cellL.bottom || yy == y );
		}
		y = cellL.bottom;	// Upper edge
		if( LTP(y1,ynewl) ) {
			WRITELOG2("RRun: stall at %d,%d\n",CACHE_PU[y].L[S],CACHE_PU[y].L[P]);
			M_(ypatchU) = MINP( M_(ypatchU), y1 );
			WRITELOG1("ypatch=%d\n",M_(ypatchU));
			M_(bStallU) = TRUE; M_(pStallU) = &CACHE_PU[y]; /*ASSERT(M_(pStallU)->rc>0);*/ break;
		}
		cellL.right = x;
		cellL.left = MAXS( _NEXTS(x,d), m_rectL[S_LEFT] );
		ASSERT( GTS( cellL.right, cellL.left ) );
		if( LTS(cellL.left,M_(xlinkU)) ) {
			ASSERT( GES(cellL.right,M_(xlinkU)) );
			ASSERT( GES(M_(xlinkU), M_(xedgeU)) );
			M_(xlinkU) = cellL.right;
			ynewl = cellL.top;
			WRITELOG4("RRun: cellL.left=%d cancelled by xlink=%d, cell.right=%d ynewl=%d\n",cellL.left,M_(xlinkU),cellL.right,ynewl);
			break;
		}
		#if _LEAVE_TRAPEZIOID
			// Trapezioid check
			bool bLeaveTrapezioid = FALSE;
			if( GEP(cellL.top,m_edgesOuter[SAS]) ) {
				i = MinMax( cellL.top,m_polygonRectL[P],m_polygonRectL[P+2]-1)-m_polygonRectL[P];
				if( LTS(cellL.right,m_ppEdges[0][i][SAS]) ) bLeaveTrapezioid = TRUE;
			} else
			if( LEP(cellL.bottom,m_edgesOuter[SAS]) ) {
				i = MinMax( cellL.bottom,m_polygonRectL[P],m_polygonRectL[P+2]-1)-m_polygonRectL[P];
				if( LTS(cellL.right,m_ppEdges[0][i][SAS]) ) bLeaveTrapezioid = TRUE;
			} else { // getting across outer point
				i = m_edgesOuter[SAS]-m_polygonRectL[P];
				ASSERT( i >= 0 && i < m_pnEdges[0] );
				if( LTS(cellL.right,m_ppEdges[0][i][SAS]) ) bLeaveTrapezioid = TRUE;
			}
			if( bLeaveTrapezioid ) {
				LOGCACHE("RRun leaving trapezioid",PU,y);
				M_(bStallU) = FALSE;
				M_(ylinkU) = ynewl = cellL.top; //MAXP(m_rectL[P_TOP],ynewl);
				//bCache = FALSE;
				//iCacheBrink = m_viewpointL[S]+SAS;
				break;
			}
		#endif
		if(	LES(cellL.left,m_viewpointL[S]+SAS) ) {
			M_(pCallLinkU) = &CACHE_PU[y];
			LOGCACHE("RRun link set",PU,y);
			M_(bStallU) = FALSE; M_(xlinkU) = cellL.right;
			M_(ylinkU) = ynewl = cellL.top; //MAXP(m_rectL[P_TOP],ynewl);
			M_(xedgeU) = cellL.right;
			//ynewl = MAXP(m_rectL[P_TOP],SUBP(y,1<<CACHE_PU[y].rc));
			break;
		}
		BeginDrawCell();
		PUTTRIANGLEMARK(1);
		if( CACHE_SU[x].L[P] == cellL.top )
			LOGCACHE("RRun read",SU,x);
		else
			LOGCACHE("RRun read",PU,y1);
		AddCellVertex( CACHE_SU[x].L[P] == cellL.top ? CACHE_SU[x] : CACHE_PU[cellL.top] );
		// fill in vertical
		for( yy = CACHE_PU[cellL.top].nxt ;; yy = CACHE_PU[yy].nxt ) {
			LOGCACHE("RRun read",PU,yy);
			ASSERT( CACHE_PU[yy].L[S] == x );
			if( GEP(yy,y) ) { ASSERT( yy == y ); break; }
			AddCellVertex( CACHE_PU[yy] );
			ASSERT( GTP(CACHE_PU[yy].nxt,yy) );
		}
		PUTTRIANGLEMARK(2);
		AddCellVertex( CACHE_PU[y] );
		// fill in horizontal
		for( xx = x ; /*GTS(xx,SUBS(x,d))*/ ; ) {
			if( !bCache || LES(SUBS(xx,1),iCacheBrink)) {
				dd = 1<<(int)REDUCTIONCELL_PS(y,SUBS(xx,1));
				CACHE_SU[xx].nxt = MAXS(cellL.left,_NEXTS(xx,dd));
			}
			ASSERT( LTS( CACHE_SU[xx].nxt,xx) );
			xx = CACHE_SU[xx].nxt;
			while( LES(xx,iCacheBrink) ) {
				iCacheBrink = CACHE_SU[iCacheBrink].nextrun;
				bCache = !bCache;
				WRITELOG3("RRun cache flip to %d at %d,%d\n",bCache,xx,y);
			}
			ASSERT( GTS(xx,iCacheBrink) );
			if( !bCache ) CREATECELLVERTEX_PS(y,xx,CACHE_SU[xx]);
			LOGCACHE("RRun read",SU,xx);
			ASSERT(CACHE_SU[xx].L[P] == y );
			if( LES(xx,cellL.left) ) break;
			AddCellVertex( CACHE_SU[xx] );
		}
		ASSERT( xx == cellL.left );
		PUTTRIANGLEMARK(3);
		AddCellVertex( CACHE_SU[xx] );
		// fill in new vertical
		nxt = CACHE_PU[y].nxt;
		xx = cellL.left; //SUBS(x,d);
		if( !bCache ) {
			WRITELOG(bCache?"RRun cache on\n":"RRun cache off\n");
			CACHE_SU[xx].rc = (int)REDUCTIONCELL_PS(SUBP(y,1),SUBS(xx,1));
		}
		CACHE_PU[y] = CACHE_SU[xx];
		CACHE_PU[y].nxt = nxt; nxt = y;
		LOGCACHE("RRun copy",PU,y);
		for( yy = y ; ; ) {
										//!!! need align yy due to rectL clip !!!
			dd = 1<<CACHE_PU[yy].rc;
			if( LEP(yy = MAXP(cellL.top,_NEXTP(yy,dd)),cellL.top) ) break;
			CREATECELLVERTEX_PS(yy,xx,CACHE_PU[yy]);
			CACHE_PU[yy].rc = REDUCTIONCELL_PS(SUBP(yy,1),SUBS(xx,1));
			CACHE_PU[yy].nxt = nxt; nxt = yy;
			LOGCACHE("RRun create",PU,yy);
			AddCellVertex(CACHE_PU[yy]);
		}
		ASSERT(yy == cellL.top);
		yy = cellL.top;
		CACHE_PU[yy].nxt = nxt;
		PUTTRIANGLEMARK(0);
		// fill in new horizontal
		rc = (int)REDUCTIONCELL_PS(SUBP(cellL.top,1),SUBS(xx,1));
		dd = 1<<(CACHE_SU[x].L[P]==cellL.top?CACHE_SU[x].rc:CACHE_PU[cellL.top].rc);
		x1 = _NEXTS(x,dd);
		for( nxt = CACHE_SU[xx].nxt ; /*LTS(xx,x)*/ ; ) {
			CREATECELLVERTEX_PS(cellL.top,xx,CACHE_SU[xx]);
			CACHE_SU[xx].rc = rc;
			CACHE_SU[xx].nxt = nxt; nxt = xx;
			LOGCACHE("RRun create",SU,xx);
			AddCellVertex(CACHE_SU[xx]);
			if( GES(xx,x1) ) break;
			dd = 1<<(rc=REDUCTIONCELL_PS(SUBP(cellL.top,1),xx));
			xx = MINS(x1,NEXTS_(xx,dd));
		}
		CACHE_SU[x].nxt = nxt;
		LOGCACHE("RRun corr nxt",SU,x);
		ENDDRAWCELL(rc0);
		x = cellL.left;
		ynewl = y1;
		bResult = TRUE;
	} // right drawing loop
End:
	y = ynewl;

	ASSERT( GES(iCacheBrink,m_viewpointL[S]+SAS) );
	ASSERT( bCacheStart || bNewLine );
	if( !bCache ) {
		if( GTS(SUBS(x,1),iCacheBrink) ) {
			CACHE_SU[SUBS(x,1)].nextrun = iCacheBrink;
			*piCacheU = SUBS(x,1);
		}
		else
		if( LES(iCacheBrink,m_viewpointL[S]+SAS) )
			*piCacheU = m_viewpointL[S]+SAS;
		else {
			ASSERT( SUBS(x,1) == iCacheBrink );
			CACHE_SU[x].nxt = iCacheBrink;
			*piCacheU = CACHE_SU[iCacheBrink].nextrun;
		}
	}
	else {	 // if bCache
		*piCacheU = iCacheBrink;
	}

	WRITELOG1("RRun: M_(iCacheU)=%d\n",M_(iCacheU));
	#ifdef _DEBUG
	{
		WRITELOG("### RCACHE RUN:");
		for( int i_ = M_(iCacheU) ; GTS(i_,m_viewpointL[S]+SAS) ;
											i_ = CACHE_SU[i_].nextrun ) {
			ASSERT( LTS(CACHE_SU[i_].nextrun, i_ ) );
			WRITELOG1(" %d", i_ );
		}
		ASSERT( i_ == m_viewpointL[S] + SAS );
		WRITELOG1(" %d\n", i_ );
	}
	#endif
	//ASSERT( cnt > 0 );

	if( LES(M_(xedgeU),m_viewpointL[S]+SAS) || LTS(x,M_(xedgeU)) )	M_(xedgeU) = x;

	if( M_(pStallU) )	goto Again;
	return bResult;
}

/*inline*/ bool _CViewTerrain::DRAW4(LinkRun)(int ylow,bool bDrawL,bool bDrawU) //,int P,int S,int PAS,int SAS)
{
	static int c1 = 1;
	//ASSERT( M_(pCallLinkL) || M_(pCallLinkU) );
	(void)bDrawL; (void)bDrawU;
	(void)ylow;
	WRITELOG4("Link: M_(pCallLinkL)=%08X bDrawL=%d M_(pCallLinkU)=%08X bDrawU=%d\n",
						M_(pCallLinkL), bDrawL, M_(pCallLinkU), bDrawU );
	WRITELOG2("Link: M_(ylinkL)=%d M_(ylinkU)=%d\n",M_(ylinkL),M_(ylinkU));
	//if( !M_(pCallLinkL) && !M_(pCallLinkU) ) return FALSE;
	//if( !M_(pCallLinkL) && GTP(M_(ylinkL),M_(ylink)) ) return FALSE;
	//if( !M_(pCallLinkU) && GTP(M_(ylinkU),M_(ylink)) ) return FALSE;
	int y, d, dd, xl, xr, rc0;
	if( M_(pCallLinkL) && GEP(M_(ylinkL),M_(ylinkU)) ) {
		ASSERT( M_(pCallLinkL) );
		y = M_(pCallLinkL)->L[P];
		xl = M_(pCallLinkL)->L[S]; //CACHE_PL[y].xL;
		//yl = CACHE_SU[xl].yL,
		d=1<<(rc0=M_(pCallLinkL)->rc);
		xr = NEXTS_(xl,d);
		ASSERT( !M_(pCallLinkU) || LEP(M_(pCallLinkU)->L[P],_NEXTP(M_(pCallLinkL)->L[P],d)) || M_(pCallLinkU)->L[S] == MINS(xr,m_rectL[S_RIGHT]) );
								// && LEP(M_(pCallLinkU)->L[P],y) );
								//LEP(M_(pCallLinkU)->L[P],SUBP(M_(pCallLinkL)->L[P],d)) );
	} else
	if( M_(pCallLinkU) && GEP(M_(ylinkU),M_(ylinkL)) ) {
		ASSERT( M_(pCallLinkU) );
		y = M_(pCallLinkU)->L[P];
		xr = M_(pCallLinkU)->L[S]; //CACHE_PU[y].xL,
		//yr = CACHE_SU[xr].yL;
		d=1<<(rc0=M_(pCallLinkU)->rc);
		xl = _NEXTS(xr,d);
		ASSERT( !M_(pCallLinkL) || LEP(M_(pCallLinkL)->L[P],_NEXTP(M_(pCallLinkU)->L[P],d)) || M_(pCallLinkL)->L[S] == MAXS(xl,m_rectL[S_LEFT]) );
								// && LEP(M_(pCallLinkL)->L[P],y) );
								//LEP(M_(pCallLinkL)->L[P],SUBP(M_(pCallLinkU)->L[P],d)) );
	}
	else
		return FALSE;


	int rcall0 = __reductionCall;

	WRITELOG4("Linking at ylow=%d y=%d xl=%d xr=%d\n",ylow,y,xl,xr);
	int x, y1, yy, xx, rc;

	CRect2	cellL;
	y = cellL.bottom = MINP( y, m_rectL[P_BOTTOM] );
	y1 = cellL.top = MAXP( _NEXTP(y,d), m_rectL[P_TOP] );
	if( LEP(y1,m_viewpointL[P]+PAS) )	return FALSE;
	xl = cellL.left = MAXS( xl, m_rectL[S_LEFT] );
	xr = cellL.right = MINS( xr, m_rectL[S_RIGHT] );
	bool bCallLinkL = M_(pCallLinkL) && GTP(M_(pCallLinkL)->L[P],y1);
	bool bCallLinkU = M_(pCallLinkU) && GTP(M_(pCallLinkU)->L[P],y1);
	if( !bCallLinkL )	xl = cellL.left = m_viewpointL[S]+SAS;
	if( !bCallLinkU )	xr = cellL.right = m_viewpointL[S]+(1-SAS);
	ASSERT( GTS( cellL.right , cellL.left ) && GTP( cellL.bottom , cellL.top ) );

	M_(pCallPatchL) = M_(pCallPatchU) = NULL;

	int ylinkL, ylinkU;
	int xlinkL, xlinkU;
	if( LES(M_(xedgeL),m_viewpointL[S]+SAS) )
		ylinkL = CACHE_SU[xlinkL=M_(xedgeL)].L[P];
	else ylinkL = M_(ylinkL), xlinkL = m_viewpointL[S]+SAS;
	ASSERT( !M_(pCallLinkL) || xlinkL==M_(pCallLinkL)->L[S] );
	if( GES(M_(xedgeU),m_viewpointL[S]+(1-SAS)) )
		ylinkU = CACHE_SU[xlinkU=M_(xedgeU)].L[P];
	else ylinkU = M_(ylinkU), xlinkU = m_viewpointL[S]+(1-SAS);
	ASSERT( !M_(pCallLinkU) || xlinkU==M_(pCallLinkU)->L[S] );
	WRITELOG4("LinkRun: ylinkL=%d ylinkU=%d xlinkL=%d xlinkU=%d\n",ylinkL,ylinkU,xlinkL,xlinkU);


	x = xr;
	//y1 = cellL.top; //SUBP(y,d);
	//y = cellL.bottom;
	bool bCancelLinkL = bCallLinkL; //!M_(pCallLinkL) || GEP(CACHE_SU[M_(pCallLinkL)->L[S]].L[P],y1);
	bool bCancelLinkU = bCallLinkU; //!M_(pCallLinkU) || GEP(CACHE_SU[M_(pCallLinkU)->L[S]].L[P],y1);
	//CCellVertex	*pNewCallL = NULL, *pNewCallU = NULL;
	//if( y-d < yl || y-d < yr ) return;
	BeginDrawCell();
	//ASSERT( !M_(pCallLinkU) || M_(pCallLinkU)->L[S] == x );
	/*if( !M_(pCallLinkU) && GEP(M_(ylinkU),y1) ) {
		CREATECELLVERTEX_PS(cellL.top,x,CACHE_SU[x]);
		CACHE_SU[x].rc = REDUCTIONCELL_PS(SUBP(cellL.top,1),SUBS(x,1));
		LOGCACHE("Link create",SU,x);
	} else*/
	CCellVertex *pStartNew;
	/*if( !M_(pCallLinkU) || LTP(CACHE_SU[M_(pCallLinkU)->L[S]].L[P],y1) ) {
		CREATECELLVERTEX_PS(cellL.top,x,CACHE_PU[cellL.top]);
		CACHE_PU[cellL.top].rc = REDUCTIONCELL_PS(SUBP(cellL.top,1),SUBS(x,1));
		pStartNew = &CACHE_PU[cellL.top];
		LOGCACHE("Link create",PU,cellL.top);
	}
	else
		pStartNew = &CACHE_SU[x];*/
	//if( !M_(pCallLinkU) || GEP(CACHE_SU[M_(pCallLinkU)->L[S]].L[P],cellL.top) ||
	//						GTS(M_(pCallLinkU)->L[S],cellL.right) )
	if( GEP( ylinkU,cellL.top ) || GTS( xlinkU, cellL.right ) )
		pStartNew = &CACHE_SU[cellL.right];
	else
		pStartNew = &CACHE_PU[cellL.top];
	if( !bCallLinkU ) {
		CREATECELLVERTEX_PS(cellL.top,xr,*pStartNew);
		pStartNew->rc = REDUCTIONCELL_PS(SUBP(cellL.top,1),SUBS(xr,1));
		#ifdef _DEBUG
			//if( !M_(pCallLinkU) || GEP(CACHE_SU[M_(pCallLinkU)->L[S]].L[P],cellL.top) ||
			//						GTS(M_(pCallLinkU)->L[S],cellL.right) )
			if( GEP( ylinkU,cellL.top ) || GTS( xlinkU, cellL.right ) )
				LOGCACHE("Link new ",SU,cellL.right); else
				LOGCACHE("Link new ",PU,cellL.top);
		#endif
	}
	ASSERT(pStartNew->L[P]==cellL.top);
	PUTTRIANGLEMARK(1);
	AddCellVertex( *pStartNew );
	// fill in vertical
	for( yy = cellL.top ; ; ) {
		if( c1 /*!M_(pCallLinkU)*/ ) {
			dd = 1<<(int)REDUCTIONCELL_PS(yy,x);
			yy = MINP( NEXTP_(yy,dd), y );
			CREATECELLVERTEX_PS(yy,x,CACHE_PU[yy]);
		} else
			yy = CACHE_PU[yy].nxt;
		LOGCACHE("Link",PU,yy);
		ASSERT( CACHE_PU[yy].L[S] == x );
		if( GEP(yy,y) ) { ASSERT( yy == y ); break; }
		AddCellVertex( CACHE_PU[yy] );
	}
	PUTTRIANGLEMARK(2);
	AddCellVertex( CACHE_PU[y] );
	// fill in horizontal
	for( xx = x ;; ) {
		if( c1 /*xx > M_(iCacheU) || xx < M_(iCacheL)*/ ) {
			dd = 1<<(int)REDUCTIONCELL_PS(y,SUBS(xx,1));
			xx = MAXS(xl, _NEXTS(xx,dd));
			if( LES(xx,xl) ) break;
			CREATECELLVERTEX_PS(y,xx,CACHE_SU[xx]);
		}
		else {
			xx = CACHE_SU[xx].nxt;
			if( LES(xx,xl) ) break;
		}
		LOGCACHE("Link",SU,xx);
		ASSERT(CACHE_SU[xx].L[P] == y );
		AddCellVertex( CACHE_SU[xx] );
	}
	ASSERT( xx == xl );
	// fill in vertical
	PUTTRIANGLEMARK(3);
	for( yy = y ; GTP(yy,cellL.top) ; yy = CACHE_PL[yy].nxt ) {
		if( c1 /*!M_(pCallLinkL)*/ ) {
			CREATECELLVERTEX_PS(yy,xl,CACHE_PL[yy]);
			dd = 1<<(int)REDUCTIONCELL_PS(SUBP(yy,1),SUBS(xl,1));
			CACHE_PL[yy].nxt = MAXP( _NEXTP(yy,dd), cellL.top );
		}
		LOGCACHE("Link",PL,yy);
		ASSERT( CACHE_PL[yy].L[S] == xl );
		AddCellVertex( CACHE_PL[yy] );
	}
	ASSERT( yy == cellL.top );
	PUTTRIANGLEMARK(0);
	//ASSERT( !M_(pCallLinkL) || M_(pCallLinkL)->L[S] == xl );
	/*if( GEP(M_(ylinkL),cellL.top) ) {
		CREATECELLVERTEX_PS(cellL.top,xl,CACHE_SU[xl] );
		LOGCACHE("Link create",SU,xl);
		CACHE_SU[xl].rc = REDUCTIONCELL_PS(SUBP(cellL.top,1),xl);
	} else*/
	/*if( !M_(pCallLinkL) || LTP(CACHE_SU[M_(pCallLinkL)->L[S]].L[P],cellL.top) ) {
		CREATECELLVERTEX_PS(cellL.top,xl,CACHE_PL[cellL.top]);
		CACHE_PL[cellL.top].rc = REDUCTIONCELL_PS(SUBP(cellL.top,1),xl);
		pStartNew = &CACHE_PL[cellL.top];
	}
	else
		pStartNew = &CACHE_SU[xl];*/
	//if( !M_(pCallLinkL) || GEP(CACHE_SU[M_(pCallLinkL)->L[S]].L[P],cellL.top) ||
	//						LTS(M_(pCallLinkL)->L[S],cellL.left) )
	if( GEP( ylinkL, cellL.top ) || LTS( xlinkL, cellL.left ) )
		pStartNew = &CACHE_SU[cellL.left];
	else
		pStartNew = &CACHE_PL[cellL.top];
	if( !bCallLinkL ) {
		WRITELOG("Link vertex create\n");
		CREATECELLVERTEX_PS(cellL.top,xl,*pStartNew);
		pStartNew->rc = REDUCTIONCELL_PS(SUBP(cellL.top,1),xl);
		#ifdef _DEBUG
			//if( !M_(pCallLinkL) || GEP(CACHE_SU[M_(pCallLinkL)->L[S]].L[P],cellL.top) ||
			//						LTS(M_(pCallLinkL)->L[S],cellL.left) )
			if( GEP( ylinkL, cellL.top ) || LTS( xlinkL, cellL.left ) )
				LOGCACHE("Link new ",SU,cellL.left); else
				LOGCACHE("Link new ",PL,cellL.top);
		#endif
	}
	ASSERT(pStartNew->L[P]==cellL.top);
	AddCellVertex( *pStartNew );
	//CACHE_PL[y] = CACHE_SU[x+d]; // copy it here, for later it will be deleted
	//LOGCACHE("Link copy",PL,y);
	// fill in new horizontal
	int nxt = xl;
										//!!! WRONG. Uses adjacent cell instead
	dd = 1<<(rc=pStartNew->rc);
	for( xx = NEXTS_(nxt,dd) ; LTS(xx,xr) ; ) {
		//xx = Min(xr,xx);
		CREATECELLVERTEX_PS(cellL.top,xx,CACHE_SU[xx]);
		CACHE_SU[xx].rc = LES(xx,m_viewpointL[S]+SAS) ? REDUCTIONCELL_PS(SUBP(cellL.top,1),xx) : rc;
		CACHE_SU[xx].nxt = nxt; nxt = xx;
		LOGCACHE("Link create",SU,xx);
		AddCellVertex(CACHE_SU[xx]);
		dd = 1<<(rc=LES(xx,m_viewpointL[S]+SAS)?CACHE_SU[xx].rc:REDUCTIONCELL_PS(SUBP(cellL.top,1),xx));
		xx = NEXTS_(xx,dd);
	}
	//ASSERT( xx == xr );
	CACHE_SU[xr].nxt = nxt;
	LOGCACHE("Link corr nxt",SU,xr);
	ENDDRAWCELL(rc0);



	if( bCallLinkL ) {
		//CACHE_SU[M_(iCacheL) = M_(pCallLinkL) ? MINS(M_(iCacheL),xl) : xl].nextrun = m_viewpointL[S]+(1-SAS);
		if( GES(M_(iCacheL),xl) )	CACHE_SU[M_(iCacheL)=xl].nextrun = m_viewpointL[S]+(1-SAS);
		else {
			ASSERT( LTS(M_(iCacheL),m_viewpointL[S]+(1-SAS)) );
			bool bCache = TRUE;
			int i;
			for( i = M_(iCacheL) ; LES(CACHE_SU[i].nextrun,xl) ; i=CACHE_SU[i].nextrun,bCache=!bCache ) ASSERT( GTS(CACHE_SU[i].nextrun,i) );
			if( !bCache )	i = CACHE_SU[i].nextrun = xl;
			CACHE_SU[i].nextrun = m_viewpointL[S]+(1-SAS);
		}
	}
	if( bCallLinkU ) {
		//CACHE_SU[M_(iCacheU) = M_(pCallLinkU) ? MAXS(M_(iCacheU),xr) : xr].nextrun = m_viewpointL[S]+SAS;
		if( LES(M_(iCacheU),xr) )	CACHE_SU[M_(iCacheU)=xr].nextrun = m_viewpointL[S]+SAS;
		else {
			ASSERT( GTS(M_(iCacheU),m_viewpointL[S]+SAS) );
			bool bCache = TRUE;
			int i;
			for( i = M_(iCacheU) ; GES(CACHE_SU[i].nextrun,xr) ; i=CACHE_SU[i].nextrun,bCache=!bCache ) ASSERT( LTS(CACHE_SU[i].nextrun,i) );
			if( !bCache )	i = CACHE_SU[i].nextrun = xr;
			CACHE_SU[i].nextrun = m_viewpointL[S]+SAS;
		}
	}

	#ifdef _DEBUG
	{
		WRITELOG("### LCACHE RUN:");
		for( int i_ = M_(iCacheL) ; LTS(i_,m_viewpointL[S]+(1-SAS)) ;
											i_ = CACHE_SU[i_].nextrun ) {
			WRITELOG1(" %d", i_ );
			ASSERT( GTS(CACHE_SU[i_].nextrun, i_ ) );
		}
		ASSERT( i_ == m_viewpointL[S]+(1-SAS) );
		WRITELOG1(" %d\n", i_ );
	}
	#endif


	if( bCallLinkL )	M_(ynewL) = M_(ylinkL) = cellL.top;
	//else if( M_(pCallLinkL) )	M_(ynewL) = M_(ylinkL) = M_(pCallLinkL)->L[P];
	if( bCallLinkU )	M_(ynewR) = M_(ylinkU) = cellL.top;
	//else if( M_(pCallLinkU) ) 	M_(ynewR) = M_(ylinkU) = M_(pCallLinkU)->L[P];
	if( bCancelLinkL ) {
		M_(pCallLinkL) = NULL;
		M_(xedgeL) = m_viewpointL[S]+(1-SAS);
		M_(xlinkL) = M_(xlinkInit);
			// ADDS(m_viewpointL[S]+(1-SAS),(1<<nTerrainMaxReduction)+10);
	}
	if( bCancelLinkU ) {
		M_(pCallLinkU) = NULL;
		M_(xedgeU) = m_viewpointL[S]+SAS;
		M_(xlinkU) = M_L(xlinkInit);
			// SUBS(m_viewpointL[S]+SAS,(1<<nTerrainMaxReduction)+10);
	}
	//M_(iCacheU) = M_(iCacheL) = m_viewpointL.x;

	//M_(ylink) = y1;
	__reductionCallLink += __reductionCall - rcall0;
	return M_(pCallLinkL) || M_(pCallLinkU);
}

void _CViewTerrain::DRAW4(Init)(TCint rectL[4],bool bVis)
{
	int x, y, yy;

	M_(bStartedPatch) = FALSE;
	M_(ypatchL) = M_(ypatchU) = m_viewpointL[P];
	M_(xlinkL) = M_(xlinkInit);
		//ADDS(m_viewpointL[S]+(1-SAS),(1<<nTerrainMaxReduction)+10);
	M_(xlinkU) = M_L(xlinkInit);
		//SUBS(m_viewpointL[S]+SAS,(1<<nTerrainMaxReduction)+10);
	M_(bStallL) = M_(bStallU) = FALSE;
	M_(xedgeL) = m_viewpointL[S]+(1-SAS);
	M_(xedgeU) = m_viewpointL[S]+SAS;
	M_(ynewL) = M_(ynewR) = m_viewpointL[P]+(1-PAS);
	M_(ylowerL) = ADDP(M_(ynewL),1<<nTerrainMaxReduction);
	M_(ylowerR) = ADDP(M_(ynewR),1<<nTerrainMaxReduction);
	M_(ynewL) = M_(ynewR) = M_(ylinkL) = M_(ylinkU) = m_viewpointL[P]+(1-PAS);
	M_(iCacheL) = m_viewpointL[S]+(1-SAS); M_(iCacheU) = m_viewpointL[S]+SAS;
	M_(pCallLinkL) = M_(pCallLinkU) = M_(pStallL) = M_(pStallU) = NULL;
	M_(pCallPatchL) = M_(pCallPatchU) = NULL;
	M_(bVisLeft) = LTS( rectL[S+2*SAS], m_viewpointL[S] ),
	M_(bVisRight) = GTS( rectL[S+2*(1-SAS)], m_viewpointL[S] );
	ASSERT( M_(bVisLeft) || M_(bVisRight) );

	M_(lastStartL)[P] = M_(ylowerL);
	M_(lastStartL)[S] = SUBS(rectL[S_LEFT],1<<nTerrainMaxReduction);
	M_(lastStartU)[P] = M_(ylowerR);
	M_(lastStartU)[S] = ADDS(rectL[S_RIGHT],1<<nTerrainMaxReduction);

	M_(bDrawLeft) = M_(bVisLeft);
	M_(bDrawRight) = M_(bVisRight);

	if( !M_(bVisLeft) )	M_(ylinkL) = M_(ynewL) = SUBP(rectL[P+2*PAS],1<<nTerrainMaxReduction);
	if( !M_(bVisRight) )	M_(ylinkU) = M_(ynewR) = SUBP(rectL[P+2*PAS],1<<nTerrainMaxReduction);

	if( !bVis) return;


	{
		y = rectL[P+2*(1-PAS)] + (PAS-1); yy = y - m_polygonRectL[P];
		ASSERT( yy >= 0 && yy < m_polygonRectL[P+2]-m_polygonRectL[P] );
		int nRdL = 0, nRdR = 0; //m_pEdgeR[yy][SAS], nRdR = m_pEdgeR[yy][1-SAS];
		int left = m_ppEdges[nRdL][(y>>nRdL)-(m_polygonRectL[P]>>nRdL)][SAS];
		int right = m_ppEdges[nRdR][(y>>nRdR)-(m_polygonRectL[P]>>nRdR)][1-SAS];
		left = MAXS(left,(int)rectL[S+2*SAS]);
		right = MINS(right,(int)rectL[S+2*(SAS-1)]);
		nRdL = (int)REDUCTIONCELL_PS(y,x=left);
		x = _ALIGNS(x,1<<nRdL);
		//y = ALIGNP_(y,1<<nRdL); //ADDP(_ALIGNP(y,1<<nRdL),1<<nRdL);
		M_(ynewL) = rectL[P+2*(1-PAS)]; //y;
		if( M_(bVisLeft) )	{
			//M_(ylinkL) = M_(ynewL);
			M_(ylinkL) = ADDP(M_(ynewL),1<<nTerrainMaxReduction);
			//M_(lastStartL)[P] = M_(ylowerL);
			//M_(lastStartL)[S] = SUBS(rectL[S_LEFT],1<<nTerrainMaxReduction);
							//m_viewpointL[S]+(1-SAS);
		}

		y = rectL[P+2*(1-PAS)] + (PAS-1);
		nRdR = (int)REDUCTIONCELL_PS(y,x=right);
		x = ALIGNS_(x,1<<nRdR); // x = (x+((1<<nRdR)-1)) & (-1<<nRdR);
		//y = ALIGNP_(y,1<<nRdR); // y = ADDP(_ALIGNP(y,1<<nRdR),1<<nRdR);
		M_(ynewR) = rectL[P+2*(1-PAS)]; //y;
		if( M_(bVisRight) )	{
			//M_(ylinkU) = M_(ynewR);
			M_(ylinkU) = ADDP(M_(ynewR),1<<nTerrainMaxReduction);
			//M_(lastStartU)[P] = M_(ylowerR);
			//M_(lastStartU)[S] = m_viewpointL[S]+SAS;
			//M_(lastStartU)[S] = ADDS(rectL[S_RIGHT],1<<nTerrainMaxReduction);
		}
	}

	M_(ylowerL) = ADDP(M_(ynewL),1<<nTerrainMaxReduction);
	M_(ylowerR) = ADDP(M_(ynewR),1<<nTerrainMaxReduction);

	//M_(ylowerL) = M_(ynewL);
	//M_(ylowerR) = M_(ynewR);
	//M_(ylink) = rectL[P+2*(1-PAS)];
}

//inline
void _CViewTerrain::DRAW4(Front)(TCint rectL[4])
{
	if( M_(pCallPatchL) && M_(pCallPatchU) )	return;
	int y;
	int ypatchL_L = M_L(ypatchL), ypatchU_L = M_L(ypatchU);
	if( M_L(pCallPatchL) ) {
		int d = 1<<M_L(pCallPatchL)->rc;
		ypatchL_L = MAXP(ypatchL_L,NEXTP_(M_L(pCallPatchL)->L[P],d));
	}
	if( M_L(pCallPatchU) ) {
		int d = 1<<M_L(pCallPatchU)->rc;
		ypatchU_L = MAXP(ypatchU_L,NEXTP_(M_L(pCallPatchU)->L[P],d));
	}
	for( bool bDrewSomething = FALSE ;; ) {
		do { // draw until both semifronts return false
			//if( M_(pCallLinkL) || M_(bCallPatchL) )	{ /*ASSERT(!M_(pCallLinkU) || M_(bCallPatchL));*/ y = M_(ynewR); } else
			//if( M_(pCallLinkU) || M_(bCallPatchU) )	{ /*ASSERT(!M_(pCallLinkL) || M_(bCallPatchU));*/ y = M_(ynewL); } else {
			//	y = MAXP(M_(ynewL),M_(ynewR));
				//if( LEP(y,rectL[P+2*PAS]) ) return;
			//}
			y = MAXP(M_(ylinkL),M_(ylinkU));
			WRITELOG3("<<<<<<<<<<<<<<<<<<<<<<<<<< y = %d (of %d,%d)\n",y,M_(ynewL),M_(ynewR));
			WRITELOG2("M_(ylinkL)=%d M_(ylinkU)=%d\n",M_(ylinkL),M_(ylinkU));
			//if( M_(bVisLeft) && (GEP(M_(ylinkL),y)||!M_L(bWaitPatch)||LTP(M_(ylinkL),ypatchU_L)) ) {
			if( M_(bVisLeft) && (GEP(M_(ylinkL),y)||!M_L(bWaitPatch)||M_(bStartedPatch)) ) {
				M_(bDrawLeft) = DRAW4(LRun)(M_(ynewL),M_(ylowerL),ypatchU_L,rectL);
				//ASSERT(!M_(pStallL)||M_(pCallPatchL));
				ASSERT(!M_(bDrawLeft) || GES(m_viewpointL[S],rectL[S+2*SAS]) || M_(pCallLinkL));
				if( M_(bDrawLeft) )	bDrewSomething = TRUE;
			}
			#if _NODRAW_BREAK
				else M_(bDrawLeft) = FALSE;
			#endif
			//if( M_(bVisRight) && (GEP(M_(ylinkU),y)||!M_(bWaitPatch)||LTP(M_(ylinkU),ypatchL_L)) ) {
			if( M_(bVisRight) && (GEP(M_(ylinkU),y)||!M_(bWaitPatch)||M_(bStartedPatch)) ) {
				M_(bDrawRight) = DRAW4(RRun)(M_(ynewR),M_(ylowerR),ypatchL_L,rectL);
				//ASSERT(!M_(pStallU)||M_(pCallPatchU));
				ASSERT(!M_(bDrawRight) || LES(m_viewpointL[S],rectL[S+2*(1-SAS)]) || M_(pCallLinkU));
				if( M_(bDrawRight) )	bDrewSomething = TRUE;
			}
			#if _NODRAW_BREAK
				else M_(bDrawRight) = FALSE;
			#endif
			if( !M_(bVisLeft) || M_(pCallLinkL) || M_(pCallPatchL) )	{
				M_(bDrawLeft) = FALSE;
				//if( GTP( M_(ylinkL), M_(ylinkU) ) )	M_(bDrawRight) = FALSE;
			}
			if( !M_(bVisRight) || M_(pCallLinkU) || M_(pCallPatchU) )	{
				M_(bDrawRight) = FALSE;
				//if( GTP( M_(ylinkU), M_(ylinkL) ) )	M_(bDrawLeft) = FALSE;
			}
			if( !M_(bVisLeft) ) M_(ynewL) = M_(ynewR);
			if( !M_(bVisRight) ) M_(ynewR) = M_(ynewL);
			if( LEP(y,rectL[P_TOP]) ) break;
		} while( M_(bDrawLeft) || M_(bDrawRight) );
		WRITELOG2("ynewl = %d, ynewr = %d\n",M_(ynewL),M_(ynewR));
		//if( M_(bDrawLeft) || M_(bDrawRight) )
		y=MAXP(M_(ynewL),M_(ynewR));
		for(;;) {
			bool bDrawLink = DRAW4(LinkRun)(y,M_(bDrawLeft),M_(bDrawRight));
			y=MAXP(M_(ynewL),M_(ynewR));
			if( bDrawLink )
				bDrewSomething = TRUE;
			else /*if( !bDrawLink )*/ {
				//if( LEP(y,rectL[P+2*PAS]) ) return;
				break;
			}
			if( LEP(y,rectL[P_TOP]) ) return;
		}
		//else
		if( M_(pCallPatchL) || M_(pCallPatchU) ) return;
		//if( !bDrewSomething ) return;
		if( LEP(y,rectL[P+2*PAS]) || LTP(y,m_viewpointL[P]+(1-PAS)) ) return;
	}

}

#if _INIT_CACHE
void	_CViewTerrain::DRAW4(LCacheInit)(TCint rectL[4])
{
	int	(*pEdges)[2] = m_ppEdges[0];
	CRect2	cellL;
	bool bFirst = TRUE;
	int rightBorder = MINS(m_viewpointL[S]+SAS,m_rectL[S_RIGHT]);
	for( cellL.top = MAXP(m_edgesOuter[SAS],rectL[P_TOP]) ;
						LTP(cellL.top,rectL[P_BOTTOM]);
								cellL.top = cellL.bottom ) {
		int i = cellL.top-PAS-m_polygonRectL[P];
		cellL.left = MAXS(pEdges[i][SAS],m_rectL[S_LEFT]);
		// common init
		int rc = REDUCTIONCELL_PS(cellL.top,cellL.left), d = 1<<rc;
		cellL.left = MAXS(_ALIGNS(cellL.left,d),m_rectL[S_LEFT];
		cellL.bottom = MINP(NEXTP_(cellL.top,d),m_rectL[P_BOTTOM]);
		cellL.top = MAXP(_ALIGNP(cellL.top,d),m_rectL[P_TOP]);
		if( bFirst && GEP(cellL.top,m_viewpointL[P]+(1-PAS)) ) {
			CREATECELLVERTEX_PS(cellL.top,cellL.left,CACHE_SU[cellL.left]);
			CACHE_SU[cellL.left].rc = (int)REDUCTIONCELL_PS(SUBS(cellL.top,1),cellL.left);
		}
		bFirst = FALSE;
	VertRun:
		int nxt, y, x, dd;
		if( LTS(cellL.left,rightBorder) ) {
			M_(ynewL) = M_(ylinkL) = cellL.bottom;
			for(nxt=y=cellL.top;;) {
				if( GEP(cellL.top,m_viewpointL[P]+(1-PAS)) ) {
					dd = 1<<(int)REDUCTIONCELL_PS(y,SUBS(cellL.left,1));
					y = MINP(cellL.bottom,NEXTP_(y,dd));
				}
				if( LTP(cellL.bottom,m_viewpointL[P]+PAS) && GEP(y,cellL.bottom) ) break;
				CREATECELLVERTEX_PS(y,cellL.left,CACHE_PL[y]);
				CACHE_PL[y].rc = rc;
				CACHE_PL[y].nxt = nxt; nxt = y;
				LOGCACHE("LCache init",PL,y);
				if( GEP(y,cellL.bottom) ) break;
			}
		}

		int left1 = cellL.left;
		if( LTP(cellL.bottom,rectL[S_BOTTOM]) ) {
			i = cellL.bottom-PAS-m_polygonRectL[P];
			left1 = MINS(MAXS(pEdges[i],m_rectL[S_LEFT]),m_rectL[S_RIGHT]);
		}
		cellL.right = MINS(NEXTS_(cellL.left,d),m_rectL[S_RIGHT]);
		int right1 = cellL.left;
		int bottom1 = cellL.bottom;
		int rc1 = rc;

		for(nxt=x=cellL.left;LTS(x,left1);) {
			if( GES(x,right1) ) {
				ASSERT(x==right1);
				dd = 1<<(rc1=(int)REDUCTIONCELL_PS(cellL.bottom,x));
				right1 = NEXTS_(right1,dd);
			}
			if( GES(x,cellL.right) ) {
				ASSERT(x==cellL.right);
				d = 1<<rc=((int)REDUCTIONCELL_PS(SUBS(cellL.bottom,1),x));
				cellL.right = NEXTS_(cellL.right,d);
			}
			if( GES(cellL.bottom,m_viewpointL[P]+(1-PAS)) || x == cellL.left ) {
				CREATECELLVERTEX_PS(cellL.bottom,x,CACHE_SU[x]);
				CACHE_SU[x].rc = rc;
				CACHE_SU[x].nxt = nxt; nxt = x;
				LOGCACHE("LCache init",SU,x);
			}
			x = MINS(cellL.right,right1);
			bottom1 = ALIGNP_(cellL.bottom,dd);
			if( GTP(bottom1,cellL.bottom) ) {
				cellL.top = cellL.bottom;
				x = MAXS(_ALIGNS(x,rc1),m_rectL[S_LEFT]);
				if( LES(x,cellL.left) )	bFirst = TRUE;
				cellL.left = x;
				cellL.bottom = bottom1;
				rc = rc1; d = dd;
				goto VertRun;
			}
		}

	} // y scan loop
	M_L(ylowerU) = cellL.top;
}
#endif

#if _INIT_CACHE
void	_CViewTerrain::DRAW4(RCacheInit)(TCint rectL[4])
{
	int	(*pEdges)[2] = m_ppEdges[0];
	CRect2	cellL;
	for( cellL.top = m_edgesOuter[1-SAS] ; LTP(cellL.top,rectL[P_BOTTOM]);
								cellL.top = cellL.bottom ) {
		int i = cellL.top-PAS-m_polygonRectL[P];
		cellL.right = MINS(pEdges[i][1-SAS],m_rectL[S_RIGHT]);
		// common init
		int rc = REDUCTIONCELL_PS(cellL.top,SUBS(cellL.right,1)), d = 1<<rc;
		cellL.bottom = MINP(NEXTP_(cellL.top,d),m_rectL[P_BOTTOM]);
		cellL.top = MAXP(_ALIGNP(cellL.top,d),m_rectL[P_TOP]);
	VertRun:
		int nxt, y, x, dd;
		if( GTS(cellL.right,m_viewpointL[S]+SAS) ) {
			for(nxt=y=cellL.top;;) {
				if( GEP(cellL.top,m_viewpointL[P]+(1-PAS)) ) {
					dd = 1<<(int)REDUCTIONCELL_PS(y,SUBS(cellL.left,1));
					y = CACHE_PU[y].nxt = MINP(cellL.bottom,NEXTP_(y,dd));
				}
				if( LTP(cellL.bottom,m_viewpointL[P]+PAS) && GEP(y,cellL.bottom) ) break;
				CREATECELLVERTEX_PS(y,cellL.right,CACHE_PU[y]);
				CACHE_PU[y].rc = rc;
				LOGCACHE("RCache init",PU,y);
				if( GEP(y,cellL.bottom) ) break;
			}
		}

		int right1 = cellL.right;
		if( LTP(cellL.bottom,m_rectL[S_BOTTOM]) ) {
			i = cellL.bottom-PAS-m_polygonRectL[P];
			right1 = MAXS(MINS(pEdges[i],m_rectL[S_RIGHT]),m_rectL[S_LEFT]);
		}
		cellL.right = NEXTS_(cellL.right,d);
		int left1 = cellL.left;
		int bottom1 = cellL.bottom;
		int rc1 = rc;

		for(nxt=x=cellL.right;LTS(x,right1);) {
			if( LES(x,left1) ) {
				ASSERT(x==left1);
				dd = 1<<(rc1=(int)REDUCTIONCELL_PS(cellL.bottom,SUBS(x,1)));
				left1 = NEXTS_(left1,dd);
			}
			if( LES(x,cellL.left) ) {
				ASSERT(x==cellL.left);
				d = 1<<rc=((int)REDUCTIONCELL_PS(SUBS(cellL.bottom,1),SUBS(x,1)));
				cellL.left = NEXTS_(cellL.left,d);
			}
			if( GES(cellL.bottom,m_viewpointL[P]+(1-PAS)) || x == cellL.right ) {
				CREATECELLVERTEX_PS(cellL.bottom,x,CACHE_SU[x]);
				CACHE_SU[x].rc = rc;
				CACHE_SU[x].nxt = nxt; nxt = x;
				LOGCACHE("RCache init",SU,x);
			}
			x = MAXS(cellL.right,right1);
			bottom1 = ALIGNP_(cellL.bottom,dd);
			if( GTP(bottom1,cellL.bottom) ) {
				cellL.top = cellL.bottom;
				cellL.right = x;
				cellL.bottom = bottom1;
				rc = rc1; d = dd;
				goto VertRun;
			}
		}

	} // y scan loop
	M_L(ylowerL) = cellL.top;
}
#endif

void _CViewTerrain::DRAW4(Full)(TCint rectL[4],CLandObjectMap1 &map)
{
	m_pMap1 = &map;
	map.SetViewPoint(m_viewpointL[P],m_viewpointL[S]);

	M_(pCallPatchL) = M_(pCallPatchU) = NULL;
	M_L(pCallPatchL) = M_L(pCallPatchU) = NULL;
	m_pCallSeal_L = m_pCallSeal_U = NULL;
	//M_(bVis) = TRUE;
	//M_L(bVis) = TRUE;
	M_(bVis) = GTP( rectL[P+2*(1-PAS)], m_viewpointL[P] );
	M_L(bVis) = LTP( rectL[P+2*PAS], m_viewpointL[P] ),
	ASSERT( M_(bVis) || M_L(bVis) );

	M_(xlinkInit) = ADDS(MAXS(m_polygonRectL[S_RIGHT],m_viewpointL[S]+(1-SAS)),(1<<nTerrainMaxReduction)+10);
	M_L(xlinkInit) = SUBS(MINS(m_polygonRectL[S_LEFT],m_viewpointL[S]+SAS),(1<<nTerrainMaxReduction)+10);

	DRAW4(Init)(rectL,M_(bVis));
	#if _PATCH
		DRAW4L(Init)(rectL,M_L(bVis));
		ASSERT( M_(bVisLeft) == M_L(bVisRight) && M_(bVisRight) == M_L(bVisLeft) );
		M_(bWaitPatch) = TRUE;
		M_L(bWaitPatch) = TRUE;
	#endif

	for(;; ) {
		WRITELOG("============ Upper front\n");
		if( M_(bVis) )	{
			DRAW4(Front)(rectL);
			// if no patch call, then no patch and no other front
			M_(bStartedPatch) = TRUE;
			if( !M_(pCallPatchL) && !M_(pCallPatchU) &&
				!M_L(pCallLinkL) && !M_L(pCallLinkU) ) break;
		}
		#if _PATCH
			WRITELOG("============ Lower front\n");
			if( M_L(bVis) )	{
				DRAW4L(Front)(rectL);
				// if no patch call, then no patch and no other front
				M_L(bStartedPatch) = TRUE;
				if( !M_L(pCallPatchL) && !M_L(pCallPatchU) &&
					!M_L(pCallLinkL) && !M_L(pCallLinkU) ) break;
			}
			//bool bPatchU = M_(bCallPatchL) || M_(bCallPatchU),
			//	 bPatchL = M_L(bCallPatchL) || M_L(bCallPatchU);
			WRITELOG("============ Upper patch\n");
			if( M_(bVisLeft) && M_(bWaitPatch) &&
						(!M_(bVis) || M_(pCallPatchL) || LTP(M_(ynewL),m_viewpointL[P]+(1-PAS))) &&
						(!M_L(bVis) || M_L(pCallPatchU) || GTP(M_L(ynewR),m_viewpointL[P]+PAS)) )
				M_(bWaitPatch) = DRAW4(LPatch)(rectL);
			//ASSERT(M_(bWaitPatch));
			WRITELOG("============ Lower patch\n");
			if( M_(bVisRight) && M_L(bWaitPatch) &&
						(!M_(bVis) || M_(pCallPatchU) || LTP(M_(ynewR),m_viewpointL[P]+(1-PAS))) &&
						(!M_L(bVis) || M_L(pCallPatchL) || GTP(M_L(ynewL),m_viewpointL[P]+PAS)) )
				M_L(bWaitPatch) = DRAW4L(LPatch)(rectL);
			//if( !bPatchU && !bPatchL ) break;
			if( (LEP(M_(ynewL),m_viewpointL[P]+PAS)||!M_(bVis)) && (GTP(M_L(ynewR),m_viewpointL[P]+PAS)||!M_L(bVis)) ) M_(bWaitPatch) = FALSE;
			if( (LEP(M_(ynewR),m_viewpointL[P]+PAS)||!M_(bVis)) && (GTP(M_L(ynewL),m_viewpointL[P]+PAS)||!M_L(bVis)) ) M_L(bWaitPatch) = FALSE;
			if( (M_(pCallSeal)||!M_(bVisLeft)||!M_(bWaitPatch)) && (M_L(pCallSeal)||!M_(bVisRight)||!M_L(bWaitPatch)) ) break;
		#else
			break;
		#endif
	}

	#if _SEAL
		DRAW2(Seal)(rectL);
	#endif
}

inline bool _CViewTerrain::DRAW4(FitInTrapezioid)(int *v)
{
	if( !m_bTerrainInView || LTP( v[P]+PAS, m_polygonRectL[P_TOP]) )
		return FALSE;
	if( GEP( v[P]+PAS, m_polygonRectL[P_BOTTOM]) )
		v[P] = m_polygonRectL[P_BOTTOM]+(PAS-1);

	int i = v[P]-m_polygonRectL[P];
	ASSERT( i >= 0 && i < m_polygonRectL[P+2]-m_polygonRectL[P] );

	if( LTS(v[S],m_ppEdges[0][i][SAS]-SAS) )	v[S] = m_ppEdges[0][i][SAS]-SAS; else
	if( GTS(v[S],m_ppEdges[0][i][1-SAS]+(SAS-1)) )	v[S] = m_ppEdges[0][i][1-SAS]+(SAS-1);
	return TRUE;
}

#undef	DRAW4
#undef	DRAW4L
#undef	DRAW2
#undef	P
#undef	S
#undef	PAS
#undef	SAS
#undef	CACHE_SL
#undef	CACHE_SU
#undef	CACHE_PL
#undef	CACHE_PU
#undef	M_
#undef	M_L
//#undef	MQ_
#undef	PUTTRIANGLEMARK

