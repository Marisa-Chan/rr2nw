
#include"input.h"
#include"controls.h"

#include"titles.h"

// ------------------------------------------------------------------------
class	CPanel {
public:
	enum VIEW { VIEW_FRONT, VIEW_LEFT, VIEW_RIGHT };
protected:
	double	m_fDir, m_fMinSpeed, m_fSpeed, m_fEffSpeed, m_fHeight,
			m_fShield;
	bool	m_bHits[6];
	int		m_nBaseColor, m_nWeapon, m_nKillPoints, m_nCharge;
	int		m_nAimX, m_nAimY;
	bool	m_bDeathMatch;
    CCompassControl *m_pCompass;
    CSpeedControl *m_pSpeed;
    CAltControl 	*m_pAlt;
    CShieldControl *m_pShield;
    CHitControl *m_pHits;
    CChargeControl *m_pCharge;
    CMsgBoardControl *m_pMsg;
	CRadarControl	*m_pRadar;
	CKillPointControl	*m_pKills;
	CPnlData	m_panel, m_panelF, m_panelL, m_panelR;
	VIEW	m_eView;
	void	DrawPanel(bool bRedraw);
public:
	CPanel();
	virtual ~CPanel();
	virtual void Draw(bool bRedraw) = 0;
	virtual int  Bottom() const = 0;
	virtual CVector2 Center() const = 0;
	virtual	CRect2	RearRect() const = 0;
	void SetShield(double f) { m_fShield = f; }
	void SetHeight(double f) { m_fHeight = f; }
	void SetSpeed(double f) { m_fSpeed = f; }
	void SetEffectiveSpeed(double f) { m_fEffSpeed = f; }
	void SetLowestSpeed(double f);
	void SetDirection(double f) { m_fDir = f; }
	void SetCharge(int nCharge) { m_nCharge = nCharge; }
	//void SetID(int nID) { m_nID = nID; }
	void SetBaseColor(byte nColor)	{ m_nBaseColor = nColor; }
	void SetKillPoints(int nKillPoints) { m_nKillPoints = nKillPoints; }
	bool *GetHits() { return m_bHits; }
	void SetWeapon(int n) { m_nWeapon = n; }
	void SetAim(int x,int y) { m_nAimX = x; m_nAimY = y; }
	void AddMessage(TCchar *pszMsg) { m_pMsg->InsertMsg(pszMsg); }
	void ClearMessages() { m_pMsg->CleanMsgBoard(); }
	void Step(double fTime) { m_pMsg->Step(fTime); }
	void SetDeathMatch(bool b) { m_bDeathMatch = b; }
	void SetView(VIEW eView) { m_eView = eView; }
	CRadarControl &Radar() const { return *m_pRadar; }
};

class	CPanel320x200 : public CPanel {
protected:
	CSprite	*m_pPricel;
public:
	CPanel320x200();
	~CPanel320x200();
	virtual void Draw(bool bRedraw);
	int  Bottom() const;
	CVector2 Center() const;
	CRect2	RearRect() const;
};
class	CPanel640x480 : public CPanel {
protected:
	CSprite	*m_pPricel;
public:
	CPanel640x480();
	~CPanel640x480();
	virtual void Draw(bool bRedraw);
	int  Bottom() const;
	CVector2 Center() const;
	CRect2	RearRect() const;
};
// ------------------------------------------------------------------------
class	CMission	{
public:
	enum RESOLUTION	{ RES_NONE = -1, RES_320x200 = 0, RES_640x480, RES_SUP };
	enum RESULT	{ R_DEAD, R_COMPLETE, R_MENU };
protected:
	enum { AUX_REARVIEW, AUX_RADAR, AUX_SUP, AUX_NONE };
	CViewCity	*m_pCity;
	void		(*m_DumpScreen)();
	byte		*m_pVirtualScreen;
	int			m_nMission;
	bool		m_bPrePause;
	CRect2		m_screenRect;
	double		m_fAspectRatio;
	RESOLUTION	m_eResolution;
	int			m_nViewDir;
	dword			m_dwPrevTime;
	double			m_fDeltaT;
	int				m_nRearTag;
	bool			m_bSky;
	bool			m_bPrepause;
	CPanel			*m_pPanel;
	static	CMission *m_pCurrMission;
	static	CUserCtlDefs	m_ctl;
	static	void	DumpScreen320x200();
	static	void	DumpScreen640x480();
	#if !_RELEASE
		void ProcessPrepause();
	#endif
	void SetCtlKeys(CUserKeyDefs &keys);
	void ResetVessel();
	void ProcessCtlInput();
	MENU_PLAYTIMEACTION ProcessKeyboardInput();
	void SelectResolution(RESOLUTION eResolution);
	void SetPanelView();
	void DrawPanel();
	bool DrawDead(bool &bDead,double &fBoomTime);
	void DrawView(bool bPanel);
	static void DisplayMessage(TCchar *pszMsg) {
							m_pCurrMission->m_pPanel->AddMessage(pszMsg); }
public:
	CMission(int nMission,int nPlayers=1,int nMyID=0,bool bNetwork=FALSE);
	~CMission();
	int  Mission() const { return m_nMission; }
	void SetOptions();
	void SetViewDir(int nDir) { m_nViewDir = nDir; }
	void SyncroneousStart();
	RESULT Play();
	void SaveGame(CTaggedFile &f);
	void LoadGame(CTaggedFile &f);
	void Restart();
	void ResetPanel();
	static CUserCtlDefs &CtlDefs() { return m_ctl; }
	static CMission *Current() { return m_pCurrMission; }
};

class	CGame	{
protected:
	enum	{ MAX_MISSIONS = 6 };
	bool	m_bNetwork;
	int		m_nVessel0, m_nVessels;
	bool	m_bComplete[MAX_MISSIONS];
	CMission	*m_pMission;
	bool	m_bIsMission;
	int		m_nVesselCharge[CVessel::SHOOTER_COUNT];
	bool	m_bSavedMouse, m_bSavedJoystick;
	CUserCtlDefs	m_savedCtls;
	int		CompleteCount();
	bool	LastPossible();
	void	SetUpGraphics();
	void	SetUpPlayBack(bool bDemo);
	void	SetDownPlayBack(bool bDemo);
public:
	CGame();
	bool Play();
	~CGame();
	bool SaveGame(CTaggedFile &f);
	bool LoadGame(CTaggedFile &f);
};

