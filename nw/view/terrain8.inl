

#if P8	// P == Y
	#define	ZFROMXY_SP(RH)	m_zCoeff.x*x+m_zCoeff.y*m_hrange.GetLimit(x,y,m_nNearestHeight,RH)-m_zCoeff.z*y+m_zCoeff0
	#if !PAS	// Y plus
		#if !SAS	// Y plus X plus
			#define	DRAW8(NAME)	Draw##NAME##YPXP
		#else		// Y plus X minus
			#define	DRAW8(NAME)	Draw##NAME##YPXM
		#endif
	#else
		#if !SAS	// Y minus X plus
			#define	DRAW8(NAME)	Draw##NAME##YMXP
		#else		// Y minus X minus
			#define	DRAW8(NAME)	Draw##NAME##YMXM
		#endif
	#endif
#else
	#define	ZFROMXY_SP(RH)	m_zCoeff.x*y+m_zCoeff.y*m_hrange.GetLimit(y,x,m_nNearestHeight,RH)-m_zCoeff.z*x+m_zCoeff0
	#if !PAS	// X plus
		#if !SAS	// X plus Y plus
			#define	DRAW8(NAME)	Draw##NAME##XPYP
		#else		// X plus Y minus
			#define	DRAW8(NAME)	Draw##NAME##XPYM
		#endif
	#else
		#if !SAS	// X minus Y plus
			#define	DRAW8(NAME)	Draw##NAME##XMYP
		#else		// X minus Y minus
			#define	DRAW8(NAME)	Draw##NAME##XMYM
		#endif
	#endif
#endif


void DRAW8(DefineReductions)(int ytop,int ybottom)
		// z distance increases as x and y increase
{
	int x, y;
	int xlprev, xrprev, dmin;
	xlprev = xrprev = m_pAEdges[ytop].r;
	if( PAS ) ytop -= PAS, ybottom -= PAS;
	for( y = ytop ; LTP(y,ybottom) ; ) {
		int rlinemax = Powerof2(y);
		int xl = m_pAEdges[y].l, xr = m_pAEdges[y].r, d, r, y1;
		int	xstart;	// reduction-aligned start of the currenty processed line
		x = xl;
		// define first cell of the line
		if( GES(x,xlprev) && LTS(x,xrprev) ) {	// if x within inited range
			d = 1<<(r=REDS_SP8(x,y-1));	// if previous row does not extrude up
			if( (y & (d-1)) == 0 )  goto InitLine;	// at x, init new cell
			xstart = x = _ALIGNS(x,d);
			x = ADDS(x,d);
		}
		else {
		InitLine: // first cell need to be created
			d = 1<<(r = REDCELL_SP8(x,y));
			x0 = _ALIGNS(x,d);
			y0 = _ALIGNP(y,d);
			x1 = ADDP(x0,d);
			if( GTS(x1,xlprev) && LTS(x0,xrprev) ) { // if x intersects init area
				if( LTP(y0,y) )	{ // if y intersects init area need lower reduction
					if( minimum by y ) d = 1<<(r=rlinemax);
					Needa take minimum span from x and y, not just y!!!
					Consider minimum by x only if LTS(x0,xlprev)
					If minimum by x and still intersection with y needa
					rewrite left edge -> reconstruction of lower runs???
				}
			}
			xstart = x;
			SET_REDS_SP8(x,y,d,r);
			for( y1 = ????ADDP(y,d) ; LTP(y,y1) ; y = ADDP(y,1) )
				m_pAEdges[y].l = x;
			x = ADDS(x,d);
		}
		dmin = d; dmax = d;
		for( ; LTS(x,xlprev) ; x = ADDS(x,d) ) {
			int rmax = Min(m_pPowerof2[x&POWEROF2_MASK],MAX_REDCELL_SP(x,y));
			d = 1<<(r = Min(ReductionInt(ZFROMXY_SP(rmax)),rmax)));
			SET_REDS_SP(x,y,d,r); if(d<dmin) dmin=d;
			if( d > dmax ) ?????
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
		for( y1 = ADDP(y,dmin) ; LTP(y,y1) ; y = ADDP(y,1) ) {
			m_pAEdges[y].l = xstart;
			m_pAEdges[y].r = x;
		}
	} // y loop
}

