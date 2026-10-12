
// -------------------------------------------------------------------------
/////////////////////////////////// 1D /////////////////////////////////////
// -------------------------------------------------------------------------
class	CSplineSegment1D {
public:
	double	a, b, c, d;

	void	 InitHermit( double p1, double p4, double r1, double r4 );
	void	 InitBezier( double p1, double p2, double p3, double p4 );
	void	 LinearCombine( double c1, CSplineSegment1D &seg1,
						   double c2, CSplineSegment1D &seg2 );
	double	 Value( double t ) const;
	double	operator()( double t ) const { return Value(t); }
	double	 Deriv1( double t ) const;
	double	 Deriv2( double t ) const;
	double	 Deriv3() const;
	double	 GetBezier( int i ) const;
	void	 GetCut( CSplineSegment1D &cut, double t1, double t2 ) const;
	void	 MergeFrom( CSplineSegment1D &cut1, CSplineSegment1D &cut2 );

	void	 Offset( double fOffset );
	void	 Scale( double fScale );
	void	 Revert();

	void	 GetValueBounds( double &lower, double &upper ) const;
	double	 MinDeriv1Len(double t1, double t2) const;
};
// -------------------------------------------------------------------------
class	CHermitSegment1D : public CSplineSegment1D {
public:
	CHermitSegment1D( double p1, double p4, double r1, double r4 )
				{	InitHermit( p1, p4, r1, r4 );  }
};
// -------------------------------------------------------------------------
class	CBezierSegment1D : public CSplineSegment1D {
public:
	CBezierSegment1D( double p1, double p2, double p3, double p4 )
				{	InitBezier( p1, p2, p3, p4 );  }
};
