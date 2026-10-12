
class	CChannelMap {
public:
	struct NODE {
		double	f, t;	// [f]unction and [t]ime
		double	v;	// [v]elocity
		double	fSpline;
		bool operator==(const NODE &other) const;
		bool operator!=(const NODE &other) const;
	};

private:
	enum   { CMF_OPEN = 1, CMF_LINEAR = 2 };
	struct DATA {
		int  	nNodes, nHold;
		bool	bUpdated;
    dword  wFlags;
		double	fOpenFactor;
		NODE	aNodes[1];
	};

	// The following three structures are used in splining
	struct	EqLine	{ double  a, b, f; };
	struct	RunForw	{ double  alpha, beta, gamma; };
	struct	RunBack	{ double  kappa, mu;	};

	DATA  * m_pData;
  int     m_maxNodesNum;

public:
  CChannelMap(int n);
	CChannelMap( const CChannelMap &other );
	const CChannelMap &operator=( const CChannelMap &other );
	const CChannelMap &operator=( int n );	// use =0 to clean up
	~CChannelMap();

	bool operator==( const CChannelMap &other ) const;
	bool operator!=( const CChannelMap &other ) const;
	operator int() const	{ return  m_pData != NULL; }

  void   Create( int  nNodes, int  nHold,
									double fSpline, double fOpenFactor );
  void   CreateOpen( int  nNodes, int  nHold, double fSpline );
  void   CreateLinOpen( int  nNodes );
	int    NodeCount() const;
	NODE   & Node( int  nNode );
  const NODE &CNode( int  nNode ) const;
	NODE   & operator[]( int  nNode );

	bool   Update();
	double  Phase( double  fTime ) const;
	double  StartTime() const;
	double  EndTime() const;

};
