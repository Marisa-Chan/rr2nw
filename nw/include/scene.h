
class	CShot;

class	CMovingObject;
struct	SBonusDef	{
protected:
	static	CPhasedMovie	m_bonusMovies[];
	static	double			m_afBonusScale[];
public:
	enum TYPE { T_LASER, T_PLASMAGUN, T_MACHINEGUN, T_SUPERPLASMAGUN,
										T_ROCKET, T_SHIELD, T_SUP };
	TYPE	eType;
	int		nBonus;
	CMovingObject	*pOwner;

	double WrapTime(double fTime) {
		double fTimeTotal = m_bonusMovies[eType].TimeTotal();
		while( fTime >= fTimeTotal ) fTime -= fTimeTotal;
		return fTime;
	}
	void Draw(TCCFVector3 &pos,double fTime) {
		if( nBonus >= 0 )	return;
		m_bonusMovies[eType].Draw(fTime,pos,m_afBonusScale[eType]);
	}
	void  Draw(TCCFVector3 &pos);
	static void LoadMovies();
	static void CleanUpMovies();
};

class	CBonusObject : public CViewSphericDynamic	{
protected:
	SBonusDef	m_bonus;
	double		m_fDrawTime;
	double		m_fTimeOut;
	int			m_nCharge;
	friend class CBonusSet;
public:
	CBonusObject() { //DEBUGSTATEMENT(m_dwTagT|=0x4);
		m_eBumpability = BY_WEAK; }
	void Draw();
	bool Read(CTaggedFile &f);
	void Step(double fTime);
	void Restart();
	void SaveGame(CTaggedFile &f);
	void LoadGame(CTaggedFile &f);
};

class	CBonusSet {
protected:
	CBonusObject	*m_pBonuses;
	int			m_nBonuses;
public:
	CBonusSet() { m_nBonuses = 0; m_pBonuses = NULL; }
	~CBonusSet() { delete [] m_pBonuses; }
	bool Read(CTaggedFile &f);
	void Skip(CTaggedFile &f);
	void TakeBonus(int n);
	void InitDraw(double fTime,CViewDynamicList &list);
	void ReinitDraw();
	void Restart();
	void SaveGame(CTaggedFile &f);
	void LoadGame(CTaggedFile &f);
};

class	CMovingObject : public CViewSphericDynamic	{
	DECLARETYPEDOBJECT(CMovingObject,CViewSphericDynamic)
protected:
	CViewDynamicList	m_bangShots;
	double			m_fShield, m_fLifeTime, m_fRadius;
	enum	{ EF_VIEWOBJ = 1, EF_BOOM = 2, EF_DYNAMIC = 4, EF_BONUS = 0x80 };
	int				m_nExistFlags;
	bool			m_bHighlightShots;
	bool			m_bIsMain;

	static	CPhasedMovie	m_boomMovie;
	friend class CViewBody;
	friend class CShot;
	friend bool ProcessBonus(SBonusDef *p1,SBonusDef *p2);
protected:
	virtual void _Draw() {}
	virtual	void SetUpBonusCharge() {}
	void	Die(double fTime);
	static void	ResetLastDrawn() { CViewObject::m_bLastDrawn = FALSE; }
	static bool	IsLastDrawn() { return CViewObject::m_bLastDrawn; }
	void Init(double fRadius);

public:
	static void LoadMovie();
	static void CleanUpMovie();
	static CPhasedMovie &BoomMovie() { return m_boomMovie; }
	CMovingObject(double fRadius) { m_bHighlightShots = m_bIsMain = FALSE; Init(fRadius); }
	SBumpDef &BumpDef() { return m_bump; }
	void Draw();
	void SetViewCenter(TCCFVector3 &v) { m_dynBase = m_dynBase1 = v; }
	void Bang(CShot *ps);
	bool IsAlive() const { return m_fShield > 0; }
	bool IsMain() const { return m_bIsMain; }
	bool IsShotBumpable() const { return m_nExistFlags & EF_VIEWOBJ; }
	void Repromote() { if(m_nExistFlags & EF_DYNAMIC)
									m_pPromoteNode->AppendDyn(this); }
	void Depromote() { m_pPromoteNode->RemoveDyn(this); }
	double Radius() const { return m_fRadius; }
	void SaveGame(CTaggedFile &f);
	void LoadGame(CTaggedFile &f);
	virtual bool CollectBonus(SBonusDef &def) { (void)def; ASSERT(this); return FALSE; }
};

class	CShot : public CViewLinearDynamic	{
	DECLARETYPEDOBJECT(CShot,CViewLinearDynamic)
public:
	enum { S_OFF = 0, S_ONWAY = 0x80, S_BANG = 1, S_JUSTBANG = 0x81,
															S_INIT = 2 };
	enum TYPE { T_LASER, T_CHARGEBALL, T_BULLET, T_HICHARGEBALL,
														T_ROCKET, T_SUP  };
protected:
	#ifdef _DEBUG
		dword m_dwTime0;
	#endif
	CFVector3	m_oldend;
	CFVector3	m_bangOffset;
	double		m_fTime, m_fLifeTime, m_fStepTime;
	int			m_nStatus;
	TYPE		m_eType;
	CMovingObject	*m_pSource;
	CMovingObject	*m_pBangDynamic;

	enum { CLR_GREENLASER, CLR_REDLASER, CLR_SUP };
	static	int		m_nColors[CLR_SUP];
	static	double	m_fEnergies[T_SUP];
	static	CPhasedMovie	m_staticHitMovie, m_dynamicHitMovie,
								m_chargeBallMovie, m_hiChargeBallMovie;
	static	CViewObjectModel	*m_pRocketBase;
	static	CViewObjectRef	*m_pRocketRef;

	void	DrawLaserShot();
	void	DrawChargeBall();
	void	DrawHiChargeBall();
	void	DrawRocket();

	friend class CShotSequence;
	friend class CShooter;
	friend class CViewOrder;
	friend class CViewOrdered;
	friend class CViewBody;
	//friend class CViewDynamic;
	friend class CMovingObject;
public:
	static void LoadMovie(double fChargeTime, double fHiChargeTime);
	static void CleanUpMovie();
	CShot() {} // DEBUGSTATEMENT(m_dwTagT|=1); }
	void Init(TYPE eType,TCCFVector3 &start,
				TCCFVector3 &speed,TCCFVector3 &nZ,CMovingObject *pSource);
	bool Step(double fTime);
	void InitDraw();
	void Draw();
	SBumpDef &BumpDef() { return m_bump; }
	void LifeTimeToBump() { m_fLifeTime = Min(m_fLifeTime,m_bump.fTime); }
	void Bang();
	void Bang(CMovingObject *pm) {
				m_nStatus = S_JUSTBANG; m_pBangDynamic = pm; }
	bool IsBanged() const { return m_pBangDynamic != NULL; }
	void ApplyBang();
	bool OnWay() const { return m_nStatus == S_ONWAY; }
	bool JustOnWay() const { return MAKEBOOL(m_nStatus & 0x80); }
	bool IsJustBanged() const { return m_nStatus == S_JUSTBANG; }
	TCCFVector3 &BangOffset() const { return m_bangOffset; }
	bool IsOn() const { return m_nStatus != S_OFF; }
	bool Source(CViewDynamic *pd) const { return pd == m_pSource; }
	CMovingObject *Source() const { return m_pSource; }
	void SetOff() { m_nStatus = S_OFF; m_pBangDynamic = NULL; }
	void Repromote() { ASSERT(m_nStatus!=S_OFF);
					if(!m_pBangDynamic) m_pPromoteNode->AppendDyn(this); }
	double Energy() const;
};

class	CShotSequence	{
protected:
	int		m_nMaxShots;
	CShot	*m_pShots;
	int		m_nFirst, m_nLast;
public:
	CShotSequence(int nMaxShots) {
							m_pShots = new CShot[m_nMaxShots=nMaxShots];
							m_nFirst = m_nLast = 0; }
	~CShotSequence() { delete [] m_pShots; }
	void Step(double fTime);
	CShot *NewShot();
	void InitDraw(CViewDynamicList &shotList);
	void ReinitDraw();
	bool IsEmpty() const { return m_nFirst == m_nLast; }
	void Clear() { m_nFirst = m_nLast; }
};

class	CShooter {
protected:
	CShotSequence	*m_pSeq;
	int		m_nCharge, m_nMaxCharge;
	double	m_fShotRateTime;
	double	m_fRechargeTime;
	CMovingObject	*m_pOwner;
	CFVector3	m_offset[2];
	int		m_nGun;
	//CMusic::DIGI	m_eShootDIGI;
	int		m_nColor;
	CShot::TYPE	m_eType;
public:
	CShooter() {
		m_eType = CShot::T_LASER;
		m_nGun = 0; m_fRechargeTime = -1;
		m_nCharge = m_nMaxCharge = 0; }
	double Distance(double fRelSpeed) const;
	void Set(CShot::TYPE eType,CShotSequence *pSeq,CMovingObject *pOwner);
	CShot::TYPE GetType() const { return m_eType; }
	void SetMaxCharge(int nMaxCharge) { m_nMaxCharge = nMaxCharge; }
	int  MaxCharge() const { return m_nMaxCharge; }
	void LoadCharge(int nCharge) {
		m_nCharge = Min(m_nMaxCharge,m_nCharge+nCharge); }
	void ReloadCharge(int nCharge) { m_nCharge = nCharge; }
	int  ChargeRemaining() const { return m_nCharge; }
	void SetRate(double fShotRateTime ) { m_fShotRateTime = fShotRateTime; }
	void Recharge() { m_fRechargeTime = -1; }
	void Set(TCCFVector3 &offset1,TCCFVector3 offset2) {
							m_offset[0] = offset1; m_offset[1] = offset2; }
	void Shoot(TCCFMatrix3x4 &dir,TCCFVector3 &speed);
	void Step(double fDeltaT) { if((m_fRechargeTime-=fDeltaT)<=0)
													m_fRechargeTime = -1; }

	void SaveGame(CTaggedFile &f);
	void LoadGame(CTaggedFile &f);
};

#if 1
class	CVessel : public CMovingObject {
public:
	enum { SHOOTER_COUNT = 5 };
protected:
	CFMatrix3x4	m_dir;
//	double		m_fAngleX, m_fAngleY, m_fAngleZ;
	CFVector3	m_speed, m_position, m_offset;
	double		m_fFuelSpeed, m_fThrottle;
	double		m_fVertRotReq, m_fVertRot;
	double		m_fVertRotCtl;
	double		m_fAxisRotReq, m_fAxisRot;
	double		m_fInclineReq, m_fInclination, m_fInclineRot;
	double		m_fStrafeInclination, m_fStrafeInclineRot, m_fVertStrafe;
	bool		m_bForceage;
	bool		m_bStrafe;
	double		m_fStepTime;
	bool		m_bPrevBump;
	CShotSequence	m_shotSeq;
	CShooter		m_shooter[SHOOTER_COUNT];
	int				m_nCurrShooter;
	bool			m_bShoot;
	SBonusDef	m_bonus;
	CViewObjectModel	*m_pBase;
	CViewObjectRef	*m_pObj;
	int				m_nBaseColor;
	bool			m_bDrawn;
	void	(*m_DisplayMessage)(TCchar *);
	static double	m_fMaxShield;
	void _Draw();
	void SetUpBonusCharge();
	void DisplayMessage(TCchar *pszMsg) {
							if(m_DisplayMessage) m_DisplayMessage(pszMsg); }
	void	Init();
public:
	enum { VD_FORWARD, VD_BACK, VD_LEFT, VD_RIGHT };
	CVessel();
	~CVessel() { delete m_pObj; delete m_pBase; }
	void	Restart();
	void	Init(bool bMain,double fHue);
	void	BeginPreStep() { m_fStepTime = 0; m_offset = CFVector3(0,0,0); }
	void	AccumPreStep(double fDeltaT);
	void	QuickAccumPreStep(double fDeltaT);
	void	PreStep(double fDeltaT);
	void	ApplyStep();
	void	SetAsViewPoint(bool bCentered,double fFocus,double fAspRatio,
													int nDir = VD_FORWARD );
	void	Throttle(double fDelta);
	void	Raise(double fDeltaUp);
	void	Rotate(double fDeltaRight);
	void	SetStrafe(bool bStrafe);
	void	Incline(double fDeltaRight);
	void	SetForceage(bool b) { m_bForceage = MAKEBOOL(b); }
	void	SetPos(TCCFVector3 &v) { m_position = v; }
	TCCFVector3 &Pos() const { return m_position; }
	TCCFVector3 &Speed() const { return m_speed; }
	double	FuelSpeed() const { return m_fFuelSpeed; }
	double	ForwardSpeed() const;
	double	Shield() const { return m_fShield; }
	CFMatrix3x4 &GetDir() { return m_dir; }
	CShooter &Shooter() { return m_shooter[m_nCurrShooter]; }
	CShooter &Shooter(int n) { return m_shooter[n]; }
	void	SelectShooter(int n) { m_nCurrShooter = n; }
	int		SelectedShooter() const { return m_nCurrShooter; }
	void	StepShots(double fTime) { m_shotSeq.Step(fTime); }
	void	InitDrawShots(CViewDynamicList &shotList) {
										m_shotSeq.InitDraw(shotList); }
	void	ReinitDrawShots() { m_shotSeq.ReinitDraw(); }
	void	GetHits(bool bHits[6]);
	void	RemoveBangs() { m_bangShots.Clear(); }
	void	SetShoot(bool bShoot) { m_bShoot = bShoot; }
	byte	BaseColor() const { return (byte)m_nBaseColor; }
	bool	IsDrawn() const { return m_bDrawn; }
	void	SetDisplayMessage(void (*pDisplayMessage)(TCchar *)) {
									m_DisplayMessage = pDisplayMessage; }

	void SaveGame(CTaggedFile &f);
	void LoadGame(CTaggedFile &f);

	int	MakeControlPacket(byte *pBuffer,int len);
	int	ReadControlPacket(TCbyte *pBuffer,int len);
	int	MakeBangPacket(byte *pBuffer,int len,double fEnergy);
	int	ReadBangPacket(TCbyte *pBuffer,int len);
	//int	ReadOffsetPacket(TCbyte *pBuffer,int len);
	//int	MakeOffsetPacket(byte *pBuffer,int len);

	//static	bool	LoadImage();
	//static	void	UnloadImage();
	//void	RemoveDrawObj() { delete m_pObj; m_pObj = NULL; }
	bool CollectBonus(SBonusDef &def);
	void LoadPos(TCCFMatrix3x4 &startPos);

	CViewOrdered *GetPromoteNode() { ASSERT(m_pPromoteNode); return m_pPromoteNode; }
	void ClipMotion(TCCFVector3 &pt,TCCFVector3 &n) {
		if( m_bump.vel*n <= 0 ) return;
		CFVector3 e = m_bump.start+m_bump.vel*m_bump.fTime;
		double d1 = (e-pt)*n;
		if( d1 >= 0 ) {
			double d = (m_bump.start-pt)*n;
			if( d >= 0 ) { m_bump.fTime = 0; return; }
			m_bump.fTime *= -d/Max(d1-d,1e-3);
		}
	}
	void LimitMotion(TCCFRect2 &rect) {
		ClipMotion(CFVector3(rect.left,0,rect.top),CFVector3(-1,0,0));
		ClipMotion(CFVector3(rect.right,0,rect.top),CFVector3(1,0,0));
 		ClipMotion(CFVector3(rect.left,0,rect.top),CFVector3(0,0,-1));
 		ClipMotion(CFVector3(rect.left,0,rect.bottom),CFVector3(0,0,1));
	}
	void	Stop();

	static	void SetMaxShield(double f) { m_fMaxShield = f; }
	static	double MaxShield() { return m_fMaxShield; }
};
#else
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

class CVessel : public CViewSphericDynamic {
protected:
	CFMatrix3x4	m_dir;
	CFVector3	m_speed, m_position, m_offset, m_head;
	double		m_fFuelSpeed, m_fThrottle;
	double		m_fVertRotReq;
	double		m_fAxisRotReq;
	double		m_fInclineReq;
	bool		m_bForceage;
	bool		m_bStrafe;
	double		m_fStepTime;
	bool		m_bPrevBump;
	double		m_fMaxSpeed, m_fMaxInclSpeed, m_fMaxVertSpeed;
	CViewObjectModel	*m_pBase;
	CViewObjectRef	*m_pObj;
	double		m_fBumpRadius;
	void	Init();
public:
	CVessel() {}
	~CVessel() { delete m_pObj; delete m_pBase; }
	void	Draw();
	void	Restart() { Init(); }
	void	Init(bool,double) { Restart(); }
	void	BeginPreStep() { m_fStepTime = 0; m_offset = CFVector3(0,0,0); }
	void	AccumPreStep(double fDeltaT);
	void	QuickAccumPreStep(double fDeltaT) {
		m_offset += m_speed*fDeltaT; m_fStepTime += fDeltaT;
	}
	void	PreStep(double fDeltaT);
	void	ApplyStep();
	void	SetPos(TCCFVector3 &v) { m_position = v; }
	TCCFVector3 &Pos() const { return m_position; }
	TCCFVector3 &Speed() const { return m_speed; }
	CFMatrix3x4 &GetDir() { return m_dir; }
	void LoadPos(TCCFMatrix3x4 &startPos) {
		m_dir = startPos;
		CFMatrix3x4	t;
		t.LoadTransposed(m_dir);
		m_position = t.Offset();
		m_dir.LoadOffset(CFVector3(0,0,0));
	}

	CViewOrdered *GetPromoteNode() { ASSERT(m_pPromoteNode); return m_pPromoteNode; }
	void LimitMotion(TCCFRect2 &) {}


	void	Throttle(double fDelta) { m_fThrottle = MinMax(fDelta,-1.,1.); }
	void	Raise(double fDeltaUp) { m_fVertRotReq = MinMax(fDeltaUp,-1.,1.); }
	void	Incline(double fDeltaRight) { m_fInclineReq = MinMax(fDeltaRight,-1.,1.); }
	void	Rotate(double fDeltaRight) { m_fAxisRotReq = MinMax(fDeltaRight,-1.,1.); }
	void	SetStrafe(bool bStrafe) { m_bStrafe = MAKEBOOL(bStrafe); }
	void	SetForceage(bool b) { m_bForceage = MAKEBOOL(b); }
	void	Stop() { m_speed = CFVector3(0,0,0); m_fFuelSpeed = m_fThrottle = 0; }


	void Depromote() { m_pPromoteNode->RemoveDyn(this); }
};
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

#endif

class	CViewScene;
class	CVesselPack {
protected:
	int	m_nVessels;
	int m_nViewpoint, m_nMoveable;
	CVessel	*m_pVessels;
public:
	CVesselPack(int n) {
		m_pVessels = new CVessel[m_nVessels=n];
		for( int i = 0 ; i < n ; i++ ) m_pVessels[i].Init(FALSE,0);
		m_nViewpoint = 0;
		m_nMoveable = 0;
	}
	~CVesselPack() { delete [] m_pVessels; }
	CVessel	&Moveable() { return m_pVessels[m_nMoveable]; }
	CVessel &Viewpoint() { return m_pVessels[m_nViewpoint]; }
	void LoadTo(CViewDynamicList &list) {
		for( int i = 0 ; i < m_nVessels ; i++ )
			if( i != m_nViewpoint )	list.Load(&m_pVessels[i]);
	}
	void SetMoveable(int n) { if(n>=0&&n<m_nVessels) m_nMoveable = n; }
	void SetViewpoint(int n) { if(n>=0&&n<m_nVessels) m_nViewpoint = n; }
	void Restart() {
		for( int i = 0 ; i < m_nVessels ; i++ ) {
			m_pVessels[i].Restart();
			m_pVessels[i].GetDir().LoadIdentity();
		}
	}
	int Number() { return m_nVessels; }
	void SetPos(TCCFVector3 &pos) {
		for( int i = 0 ; i < m_nVessels ; i++ )
			m_pVessels[i].SetPos(pos+CFVector3(0,i*10,0));
	}
	void SetPosEqu(TCCFVector3 &pos) {
		for( int i = 0 ; i < m_nVessels ; i++ )
			m_pVessels[i].SetPos(pos);
	}
	void BeginPreStep() {
		for( int i = 0 ; i < m_nVessels ; i++ )
			m_pVessels[i].BeginPreStep();
	}
	void PreStep(double fTime) {
		for( int i = 0 ; i < m_nVessels ; i++ )
			m_pVessels[i].PreStep(fTime);
	}
	void ApplyStep() {
		for( int i = 0 ; i < m_nVessels ; i++ )
			m_pVessels[i].ApplyStep();
	}
	void RemoveLandDynamic(CViewScene *pScene);
	void LimitMotion(TCCFRect2 &r) {
		for( int i = 0 ; i < m_nVessels ; i++ )
			m_pVessels[i].LimitMotion(r);
	}
};

class	CMovingViewObject : public CMovingObject	{
protected:
	CViewObjectRef	m_obj;
protected:
	void _Draw() { m_obj.Draw(); }
public:
	CMovingViewObject(CViewObjectModel &set) : CMovingObject(set.Radius()),
						m_obj(&set) { m_nExistFlags |= EF_DYNAMIC; }
	virtual ~CMovingViewObject() {}
	CFMatrix3x4 &GetDir() { return m_obj.GetDirModify(); }
	void InitDraw() { SetViewCenter(m_obj.Center()); }
};

#if 0
class	CFoeSet {
protected:
	CFoe	**m_ppFoes, **m_ppNearestFoes;
	int		m_nFoes, m_nFoesAlive, m_nNearestFoes;
	int		m_nGens, m_nGensAlive;
	double	m_fStepTime;
public:
	CFoeSet() { m_ppFoes = m_ppNearestFoes = NULL;
				   m_nFoes = m_nNearestFoes = m_nGens = m_nGensAlive = 0; }
	~CFoeSet() {
		if(m_ppFoes) for(int i = 0 ; i < m_nFoes ; i++ ) delete m_ppFoes[i];
		delete [] m_ppFoes;	delete [] m_ppNearestFoes;
	}
	bool Read(CTaggedFile &f,CFoe::BEHAVIOUR *peBehaviours);
	void Skip(CTaggedFile &f);
	void PreStep(double fTime, CVessel &vessel,CViewOrdered *pBumpOrder);
	void ApplyStep();
	void InitDraw(CViewDynamicList &dynamicList,CViewDynamicList &shotList);
	void ReinitDraw();
	void DefaultGen(CViewObjectModel *pSet);
	CFoe *Foe(int i) { return m_ppFoes[i]; }
	int Count() { return m_nFoes; }
	int AliveCount() { return m_nFoesAlive; }
	int GensAliveCount() { return m_nGensAlive; }
	int GensCount() { return m_nGens; }
	void Hide();
	void SaveGame(CTaggedFile &f);
	void LoadGame(CTaggedFile &f);
	int	 FindNearest(int nMax,TCCFVector3 &center);
	int  NearestCount() const { return m_nNearestFoes; }
	CFoe *NearestFoe(int n) const {
					ASSERT(n<m_nNearestFoes); return m_ppNearestFoes[n]; }
	void Restart();
};
#endif
// -------------------------------------------------------------------------


class	CViewScenePiece	: public CViewOrdered {
protected:
	CRect2 m_rect;
public:
	bool Bump(SBumpDef0 &) {return FALSE;}
};

class	CViewSceneLandPiece : public CViewScenePiece {
protected:
	CLandscapeRect	m_land;
	bool			m_bBumpable;
public:
	CViewSceneLandPiece(CTaggedFile &f,
					CViewOrdered *(*ReadOrder)(CTaggedFile &f),
					int nSizeShift,double fCellSize,double fHRatio);
	bool Bump(SBumpDef0 &def) {
			return m_bBumpable ? m_land.Bump(def) : FALSE;
	}
	VORDTYPE Type() const { return VOT_LANDPIECE; }
	void Draw();
	void PromoteDynamic();
	virtual void ApplyModelUsage() { m_land.ApplyModelUsage(); }
	virtual void ComputeMaxHeight() { m_fMaxHeight = 0; }
	virtual double ComputeBSPError();
	virtual double ComputeBSPError(TCCFVector3 &pt,TCCFVector3 &n,double d);
	virtual void Split() {}
	void AdjustPlane(CFVector3 &pt,CFVector3 &n,double &d) { m_land.AdjustPlane(pt,n,d); }
};

class	CViewSceneCityPiece : public CViewScenePiece {
};

class	CViewScene	{
protected:
	int	m_nBases;
	struct {
		int nSizeShift;
		double fCellSize, fHRatio;
		int nReductions;
	} m_landParms;
	CViewObjectModel	*m_pBases;
	//#if !_KHAMIDULIN
		CViewTerrain	*m_pTerrain;
	//#endif
	CViewOrdered	*m_pOrder;
	CNameDecls	m_objRefNames;
	//CViewSceneLandPiece	*m_pLand;
	CLandDynamicMap	m_landDynamic;
	CViewObjectModel	m_skybase;
	CViewObjectRef		m_skyref;
	CFVector3			m_scale;
	static CViewScene	*m_pCurrent;
	static	CViewOrdered	*CViewScene::ReadOrder(CTaggedFile &f);
public:
	CViewScene(CTaggedFile &f);
	~CViewScene();
	void SetScale(double fScaleX,double fScaleY) { m_scale=CFVector3(fScaleX,-fScaleY,-1); }
	void Draw(TCSFMatrix3x4 &dir,CViewDynamicList &list);
	void PromoteDynamic(CViewDynamicList &list,bool bWillDraw=FALSE); // { m_pOrder->LoadDyns(list); m_pOrder->PromoteDynamic(); }
	void CheckDynamicMap() { ASSERT(m_landDynamic.IsEmpty()); }
	bool Bump(SBumpDef &def) { return m_pOrder->Bump(def); }
	int BaseCount() { return m_nBases; }
	CViewObjectModel &Base(int n) { ASSERT((uint)n<m_nBases); return m_pBases[n]; }
	CNameDecls &ObjRefNames() { return m_objRefNames; }
	void SetupLandDynamics(CViewDynamicList &list,TCSRect2 &rect) { m_landDynamic.SetupDynamics(list,rect,*m_pTerrain); }
	void RemoveLandDynamic(CViewDynamic *p) { m_landDynamic.RemoveDynamic(p); }
	void Attach(CLandObjectMap2 &map) { map.Attach(m_landDynamic); }
	//void Update(CLandObjectMap2 &map) { m_landDynamic.Update(map); }
	CViewOrdered *Order() { return m_pOrder; }
	//#if !_KHAMIDULIN
		CViewTerrain *GetTerrain() { return m_pTerrain; }
	//#endif
	static CViewScene *Current() { return m_pCurrent; }
};

inline void CVesselPack::RemoveLandDynamic(CViewScene *pScene) {
	for( int i = 0 ; i < m_nVessels ; i++ )
		if( i != m_nViewpoint ) pScene->RemoveLandDynamic(&m_pVessels[i]);
}


