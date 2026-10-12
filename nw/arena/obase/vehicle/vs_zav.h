#ifndef __VS_ZAV_H__INCLUDED
#define __VS_ZAV_H__INCLUDED

#include "mathlib.h"
#include "storage/h/subject.h"
#include"i/vessel.i"



class	CVessel0 : public CViewSphericDynamic, public IVessel {

 #ifndef __VESSEL_IGNORE_MASTER
 protected:
    ct_Subject *m_master;
    bool  (*m_MasterBumpCallBack)(ct_Subject*,SBumpDef &);
 public:
    void SetMaster ( ct_Subject *master, bool (*callBack)(ct_Subject*,SBumpDef &)  )   {
		m_master = master;
        m_MasterBumpCallBack = callBack;
		//m_master->setPosition(Pos());
		SetPos(m_master->getPosition());
    }
 #endif

protected:
	TCdouble	*m_pfBounds;
	bool	AllBump(SBumpDef &def,bool bChangeVel);
	void	ClipPos(CFVector3 &pos) {
		if( m_pfBounds ) {
			pos.x = MinMax(pos.x,m_pfBounds[0],m_pfBounds[1]);
			pos.y = MinMax(pos.y,m_pfBounds[2],m_pfBounds[3]);
			pos.z = MinMax(pos.z,m_pfBounds[4],m_pfBounds[5]);
		}
	}
	virtual bool InternalBump(SBumpDef &) { return FALSE; }

public:
	CVessel0() { m_pfBounds = NULL; }
	SBumpDef  &GetBumpDef() { return m_bump; }
	void SetBounds(TCdouble *p) { m_pfBounds = p; }
};

//  -------------------------------------------------------------------
//  ---------------------------------------
//  ---------------------------------------

//#include<stdio.h>

class   CRandomizer1F   {
protected:
    CRandomGenerator    m_rand;
    double  *m_pBuf;
    int     m_n;
    double  m_f;
    double  m_f0;
    double  m_fDeltaT;
    double  m_fTimeout;
public:
    CRandomizer1F(int n,double fDeltaT) {
        m_pBuf = new double[m_n=n];
        m_f = 0;
        for( int i = 0 ; i < m_n ; i++ )
            m_pBuf[i] = 0;
        m_fDeltaT = fDeltaT;
        m_fTimeout = 0;
        m_f0 = 0;
        m_f = 0;
    }
    double  operator()(double deltat) {
        m_fTimeout -= deltat;
        while( m_fTimeout < 0 ) {
            m_f0 = m_f;
            m_fTimeout += m_fDeltaT;
            m_f = 0;
            for( int i = 1 ; i < m_n ; i++ )    m_f += (m_pBuf[i-1] = m_pBuf[i]);
            m_f += (m_pBuf[m_n-1] = m_rand(-1,1));
			//printf("1f=%lg  (%lg)\n",m_f,m_pBuf[m_n-1]);
        }
        return  (m_f*m_fTimeout+m_f0*(m_fDeltaT-m_fTimeout))/(m_fDeltaT*m_n);
    }
	void	SetTime(double fDeltaT) {
		m_fDeltaT = fDeltaT;
		m_fTimeout = 0;
	}
};


#include<ctype.h>

inline int   EnumFromString(TCchar *szString,TCchar *szEnumPats) {
	for( int i = 0 ; *szEnumPats ; i++ ) {
		int n = strlen(szEnumPats);
		if( memcmp(szString,szEnumPats,n) == 0 &&
			( szString[n] == 0 || isspace(szString[n]) ) )
				return i;
		szEnumPats += n+1;
	}
	return -1;
}


enum	VAE_EMV_eGravityMode { VAE_EMV_GM_EMV, VAE_EMV_GM_PLANE };
#define	VAES_EMV_eGravityMode	"Emv\0Plane\0"

struct	SEmvAttrs	{
	double	fRadius;	// [2]
	double	fInclinationTime;	// estimation
	//double	fInclinationRestoreTime;	// estimation
	double	fInclineFrictionCoeff;
	double	fMaxIncline;
	double	fMaxTurnSpeedTime;	// estimation
	double	fMaxTurnStopTime;	// estim.
	double	fMaxTurnSpeed;
	double	fMaxRaiseSpeed;
	double	fMaxRaiseSpeedTime;	// estim.
	double	fMaxRaiseStopTime;	// estim.
	double	fAxisRotTime;
	double	fAxisRotFrictionCoeff; // >=1
	double	fAxisRotControlCoeff; // >=0
	double	fMaxSpeed;
	double	fMaxBackSpeed;
	double	fMaxSpeedTime;
	double	fForceageFactor;
	double	fHorzStrafeSpeed;
	double	fVertStrafeSpeed;
	double	fMouseHorzDecayFactor;
	double	fMouseVertDecayFactor;
	double	RFront, RSide, RVert;
	double	fGroundInclinationTime;
	double	fGroundInclinationFrictionFactor;
	double	fMinSlidingAngle;
	double	fMaxSlidingSpeed;
	double	fMinFlyingSpeed;
	double	fFallInclineTime;
	//double	fFallInclineTimeout;
	double	fMaxFallSpeed;
	double	fFallAngle;
	VAE_EMV_eGravityMode	eGravityMode;
	double	g;
    double  fWingSuspend;
    double  fWingAmpl;
    double  fWingTime;
    double  fSuspendOffset;
    double  fRandomizeWing;
	double	fRandomizeWingTime;
	double	fMaxPossibleHeight;

	double	_fMaxIncline;
	double	_fMaxTurnSpeed;
	double	_fMaxRaiseSpeed;
	double	_fMouseHorzDecayLog;
	double	_fMouseVertDecayLog;
	double	_f1GroundInclFrictionLog;
	double	_fGroundInclFrictionLog;
	double	_fGroundInclOmega2;
	double	_fMaxSlidingCos;
    double  _fWingAmpl;
    double  _fWingOmega;
    double  _fSuspendOffset;
	double	_fFallOmega;
	double	_fFallAngle;

	SEmvAttrs() {
		fRadius = 2;
		fMaxPossibleHeight = 1e10;
		fInclinationTime = 0.5;
		//fInclinationRestoreTime = 0.5;
		fInclineFrictionCoeff = 1;
		fMaxIncline = 30;
		fMaxTurnSpeedTime = 0.5;
		fMaxTurnStopTime = 0.1;
		fMaxTurnSpeed = 90;
		fMaxRaiseSpeed = 90.;
		fMaxRaiseSpeedTime = 0.5;
        fMaxRaiseStopTime = 0.2;
		fAxisRotTime = 2;
		fAxisRotFrictionCoeff = 1;
		fAxisRotControlCoeff = 1;
		fMaxSpeed = 30;
		fMaxSpeedTime = 0.5;
		fMaxBackSpeed = 15;
		fForceageFactor = 10;
		fHorzStrafeSpeed = 30;
		fVertStrafeSpeed = 30;
		fMouseHorzDecayFactor = 10;
		fMouseVertDecayFactor = 10;
		RFront = 0.001;
		RSide = 0.001;
		RVert = 0.1;
		fGroundInclinationTime = 0.5;
		fGroundInclinationFrictionFactor = 2;
		fMinSlidingAngle = 30;
		fMaxSlidingSpeed = 5;
		fMinFlyingSpeed = 10;
		fFallInclineTime = 0.5;
		//fFallInclineTimeout = 0.5;
		fMaxFallSpeed = 50;
		fFallAngle = 40;
        fSuspendOffset = 60; // greater values make acceleration/suspension earlier
		eGravityMode = VAE_EMV_GM_EMV;
		g = 9.8;
        fWingSuspend = 0.3;
        fWingAmpl = 0;
        fWingTime = 1.5;
        fRandomizeWing = 0.3;
		fRandomizeWingTime = 0.2;
		update();
	}
	void Read(CConfigFile &cfg,TCchar *szSection) {
		#define	LOADPARM_D(X)	X = cfg.GetDouble(szSection,#X,X);
		#define	LOADPARM_I(X)	X = cfg.GetInt(szSection,#X,X);
		#define	LOADPARM_E(X)	{ TCchar *s = cfg(szSection,#X); if(s) RTCHECK3( (X = (VAE_EMV_##X)EnumFromString(s,VAES_EMV_##X)) >= 0, "Unknown value: [%s] %s=%s",szSection,#X,s); }
		LOADPARM_D(fRadius)
		LOADPARM_D(fInclinationTime)
		//LOADPARM_D(fInclinationRestoreTime)
		LOADPARM_D(g)
		LOADPARM_D(fInclineFrictionCoeff)
		LOADPARM_D(fMaxIncline)
		LOADPARM_D(fMaxTurnSpeedTime)
		LOADPARM_D(fMaxTurnStopTime)
		LOADPARM_D(fMaxTurnSpeed)
		LOADPARM_D(fMaxRaiseSpeed)
		LOADPARM_D(fMaxRaiseSpeedTime)
		LOADPARM_D(fMaxRaiseStopTime)
		LOADPARM_D(fAxisRotTime)
		LOADPARM_D(fAxisRotFrictionCoeff)
		LOADPARM_D(fAxisRotControlCoeff)
		LOADPARM_D(fMaxSpeed)
		LOADPARM_D(fMaxBackSpeed)
		LOADPARM_D(fMaxSpeedTime)
		LOADPARM_D(fForceageFactor)
		LOADPARM_D(fMouseHorzDecayFactor)
		LOADPARM_D(fGroundInclinationTime)
		LOADPARM_D(fGroundInclinationFrictionFactor)
		LOADPARM_D(fMinSlidingAngle)
		LOADPARM_D(fMaxSlidingSpeed)
		LOADPARM_D(fMinFlyingSpeed)
		LOADPARM_D(fFallInclineTime)
		//LOADPARM_D(fFallInclineTimeout);
		LOADPARM_D(fMaxFallSpeed)
		LOADPARM_D(fFallAngle)
		LOADPARM_E(eGravityMode)
        LOADPARM_D(fWingSuspend)
        LOADPARM_D(fWingAmpl)
        LOADPARM_D(fWingTime)
        LOADPARM_D(fSuspendOffset)
        LOADPARM_D(fRandomizeWing)
        LOADPARM_D(fRandomizeWingTime)
		LOADPARM_D(fMaxPossibleHeight)
		#undef	LOADPARM_D
		#undef	LOADPARM_I
		#undef	LOADPARM_E
		update();
	}
	void update() {
		_fMaxIncline = fMaxIncline*M_PI/180;
		_fMaxTurnSpeed = fMaxTurnSpeed*M_PI/180;
		_fMaxRaiseSpeed = fMaxRaiseSpeed*M_PI/180;
		_fMouseHorzDecayLog = log(fMouseHorzDecayFactor);
		_fMouseVertDecayLog = log(fMouseVertDecayFactor);
		_fGroundInclFrictionLog = log(fGroundInclinationFrictionFactor);
		_f1GroundInclFrictionLog = 1/_fGroundInclFrictionLog;
		_fGroundInclOmega2 = M_PI*M_PI/(4*fInclinationTime*fInclinationTime);
		_fMaxSlidingCos = cos(M_PI/180*fMinSlidingAngle);
        _fWingAmpl = fWingAmpl*M_PI/180;
        _fWingOmega = 2*M_PI/fWingTime;
        _fSuspendOffset = fSuspendOffset*M_PI/180;
		_fFallOmega = 2*M_PI/fFallInclineTime;
		_fFallAngle = M_PI*fFallAngle/180;
	}
};


class	CVesselEmv : public CVessel0 {
public:
	enum { SHOOTER_COUNT = 5 };
protected:
	CFMatrix3x4	m_dir;
//	double		m_fAngleX, m_fAngleY, m_fAngleZ;
	CFVector3	m_speed, m_position, m_offset;
	double		m_fFuelSpeed, m_fThrottle;
	double		m_fVertRotReq, m_fVertRot;
	//double		m_fVertRotCtl;
	double		m_fAxisRotReq, m_fAxisRot;
	double		m_fInclineReq, m_fInclination, m_fInclineRot;
	double		m_fTurnRot;
	double		m_fStrafeReq;
	double		m_fStrafeInclination, m_fStrafeInclineRot, m_fVertStrafe;
	bool		m_bForceage;
	bool		m_bStrafe;
	double		m_fStrafeVertReq;
	double		m_fStepTime;
	bool		m_bPrevBump;
	double		m_fTurnRotMouse;
	double		m_fVertRotMouse;
	CFVector3	m_moment;
	CFVector3	m_fallSpeed;
	CViewObjectModel	*m_pBase;
	CViewObjectRef	*m_pObj;
	double	m_fBumpRadius;
    double  m_fWingAmpl, m_fWingRaise, m_fWingTime;
    //CGaussRandomGenerator   m_grand;
    CRandomizer1F   m_rand1f;
	bool			m_bTouchGround;
	int				m_nBumpCountStep;
	CFVector3		m_bumpNormal;

	SEmvAttrs	m_attrs0;
	SEmvAttrs	*m_attr;

	CFVector2	m_joy;
	CFVector2	m_mouse;

	bool InternalBump(SBumpDef &);
	void Draw();
public:
	void	Init();
	enum { VD_FORWARD, VD_BACK, VD_LEFT, VD_RIGHT };
	CVesselEmv();
	~CVesselEmv() { delete m_pObj; delete m_pBase; }
	void	Attach(SEmvAttrs *attrs) { m_attr = attrs; m_fBumpRadius = attrs->fRadius; m_rand1f.SetTime(attrs->fRandomizeWingTime); }
	void	Restart();
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
	void	StrafeVert(double fDelta) { m_fStrafeVertReq = MinMax(fDelta,-1.,1.); }
	void	StrafeHorz(double fDelta) { m_fStrafeReq = MinMax(fDelta,-1.,1.); }
	void	Incline(double fDeltaRight);
	void	Jump(double fDelta) { (void)fDelta; }
	void	SetForceage(bool b) { m_bForceage = MAKEBOOL(b); }
	void	SetPos(TCCFVector3 &v) {
		m_position = v;
		#ifndef __VESSEL_IGNORE_MASTER
			m_master->setPosition(m_position);
		#endif
	}
	TCCFVector3 Pos() { return m_position; }
	TCCFVector3 Speed() { return m_speed; }

	void	 MoveMouse(double dx,double dy) { m_mouse.x += dx/1e3; m_mouse.y += dy/5e2; }
    void     Joystick(double x,double y) { m_joy.x = x; m_joy.y = y; }

	double	FuelSpeed() const { return m_fFuelSpeed; }
	double	ForwardSpeed() const;
	CFMatrix3x4 &GetDir() { return m_dir; }
    void SetDir(TCSFMatrix3x4 &dir) { m_dir = dir; m_dir.LoadOffset(CFVector3(0,0,0)); }
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
    TCvoid *SaveGame();
    bool LoadGame(TCvoid *pSavedGame);
	void	Stop();

	void	NextFrame();

	void Depromote() { m_pPromoteNode->RemoveDyn(this); }
};
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
enum	VAE_WHEELS_eAccelControl { VAE_WHEELS_AC_THROTTLE, VAE_WHEELS_AC_WALK };
#define	VAES_WHEELS_eAccelControl	"Throttle\0Walk\0"

//P.I.N. Structure Start
struct	SWheelsAttrs	{
	// ------------------
	// outer parms
	double	fRadius;	// [2]
	double  g; // [9.8] gravity
	int		bAllowStrafe;		// [1]	0 = disallow
	int		bRotatableTower;
	int		bAutoStop;			// [0]  1 = stop when no key is pressed
	int		bFollowInclination;	// [1]  1 = incline, 0 = stand erect
	int		bInverseY;			// [0]
	bool	bRestoreRaise;
	bool	bJoyRestoreRaise;
	bool	bJump;				// [0]
	bool	bFlyControl;		// [0]
	enum VAE_WHEELS_eAccelControl eAccelControl; // "Throttle"
	double	fHead;				// [2.5]
	double	fMaxFlyAddSpeed;	// [10]
	double	fMaxFlyAddStrafeSpeed;
	double	fFlyFrictionFactor;	// [10]
	double	fForceageFactor;	// [10]	1 = no forceage
	double	fMaxSpeed;			// [10]	maximum linear speed m/sec
	double	fMaxBackSpeed;		// [1] maximum linear back speed m/sec
	double	fMaxStrafeSpeed;
	double	fMaxStrafeTime;
	double	fFullStopSpeed;		// [1] full stop at this speed if braking
	double	fMaxTurnSpeed;		// [90] maximum turn (left/right) speed deg/sec
    double  fMouseTurnSpeed;
	double	fMaxTurnSpeedTime;	// [1]
	double	fTurnStopTime;		// [0.2]
	double	fMaxSpeedTime;		// [1] time before reaching maximum speed
	double	fStopTime;			// [0.2]
	double	fFrictionFactor;	// [2] after 1 sec speed /= fFrictionFactor (exponentially)
	double	fSideFrictionFactor;	// [20]  THROTTLE ONLY
	double	fSideStopTime;	// [0.5] WALK ONLY
	double	fBrakingFrictionFactor; // [10]
	double	fInclinationTime;	// [1] the time to restore inclination with no friction
	double	fInclFrictionFactor; // [2] after 1 sec incl speed /= fFrictionFactor
	double	fMaxInclineAngle;	// [60]
	double	fMaxClimbAngle;		// [30]
	double	fNormalRessoreWay;	// [10]
	double	fMaxRessoreWay;		// [20]
	double	fRessoreFrictionFactor;	// [20]
	double	fMinSlideAngle;		// [30];
	double	fStepUp;		// [1]
	double	fMaxAccelAngle;	// [30]
	double	fWalkPendHeight;
	double	fWalkPendWidth;
	double	fWalkPendAngle;
	double	fWalkPendStep;
	double	fWalkPendMinStepTime;
	double	fWalkPendFadeTime;
	double	fMaxWalkSidePendSpeed;
	double	fCycloidHeight;		// [0.5]
	double	fCycloidAngle;		// [30]
	double	fCycloidStep;		// [1]
	double	fMaxRaise;			// [45]
	double	fMaxRaiseTime;		// [0.5]
    double  fMouseMaxRaiseTime;
	double	fRaiseAccelTime;	// [0.2]
	double	fRaiseStopTime;	// [1]
	double	fMaxJumpDist;
	double	fMaxJumpHeight;
	double	fJumpDownWay;
	double	fMaxJumpUpWayTime;
	double	fJumpTimeOut;
	double	fMouseHorzDecayFactor;
	double	fMaxTowerSpeedTime, fMaxTowerStopTime;
	double	fMaxTowerSpeed;
    double  fMouseInterpolateDecay;
    double  fSwimmingSpeed, fSwimmingStrafeSpeed;
    double  fSwimmingAccelTime;
	bool	bSwimming;

	// ------------------
	// internal parms
	double	_fMaxTurnSpeed;
	double	_fTurnAccel;
	double	_fTurnStopAccel;
	double	_fMaxAccel;
	double	_fMaxBackAccel;
	double	_fStopAccel;
	double	_fBackStopAccel;
	double	_fSideStopAccel;
	double	_fFrictionLog;
	double	_f1FrictionLog;
	double	_fBrakingFrictionLog;
	double	_fSideFrictionLog;
	double	_f1SideFrictionLog;
	double	_f1StopTime;
	double	_f1MaxSpeedTime;
	//double	_fInclSpeedFact;
	double	_fInclOmega2;
	double	_fInclFrictionLog;
	double	_f1InclFrictionLog;
	double	_fMaxInclineAngle;
	double	_fMinInclineCos;
	double	_fRessoreK;
	double	_fRessoreTime;
	double	_fRessoreFrictionLog;
	double	_fMinSlideSin;
	double	_fMinSlideCos;
	double	_f1SinMaxClimbAngle;
	double	_fCosMaxClimbAngle;
	double	_f1CosMaxClimbAngle;
	double	_fMaxClimbAngle;
	double	_fAccelAngleFactor;
	double	_fWalkPendAngle;
	double	_fWalkPendSpeed;
	double	_fMaxWalkPendEffSpeed;
	double	_fMaxRaise;
	double	_fRaiseSpeed;
    double  _fMouseRaiseSpeed;
	double	_fMaxJumpSpeedV;
	double	_fMaxJumpSpeedH;
	double	_fFlyFrictionLog;
	double	_fStrafeAccel;
	double	_fMouseHorzDecayLog;
	double	_fTowerAccel, _fTowerStopAccel;
	double	_fMaxTowerSpeed;
    double  _fMouseTurnSpeed;
    double  _fMouseDecayLog;
    double  _fSwimmingStopLog;

	SWheelsAttrs() {
		fRadius = 1;
		g = 9.8;
		bAllowStrafe = TRUE;
		bAutoStop = 0;
		bFollowInclination = FALSE;
		bRotatableTower = TRUE;
		bInverseY = 0;
		bRestoreRaise = FALSE;
		bJoyRestoreRaise = TRUE;
		bJump = TRUE;
        eAccelControl = VAE_WHEELS_AC_WALK;
		bFlyControl = FALSE;
		fHead = 2.5;
		fMaxFlyAddSpeed = 10;
		fMaxFlyAddStrafeSpeed = 5;
		fFlyFrictionFactor = 10;
		fForceageFactor = 10;
		fMaxSpeed = 10;
		fMaxBackSpeed = 10;
		fMaxStrafeSpeed = 5;
		fMaxStrafeTime = 0.2;
		fFullStopSpeed = 1;
        fMaxTurnSpeed = 90;
        fMaxTurnSpeedTime = 0.01;
        fMouseTurnSpeed = 100000.;
        fTurnStopTime = 0.01;
		fMaxSpeedTime = 1;
		fStopTime = 0.2;
		fFrictionFactor = 2;
		fSideFrictionFactor = 5;
		fSideStopTime = 0.5;
		fBrakingFrictionFactor = 50;
		fInclinationTime = 0.5;
		fInclFrictionFactor = 40; //20;
		fMaxClimbAngle = 30;
		fNormalRessoreWay = 0.2;
		fMaxRessoreWay = 0.5;
		fRessoreFrictionFactor = 50;
		fMaxInclineAngle = 60;
		fMinSlideAngle = 20; // 20
		fMaxAccelAngle = 30;
		fStepUp = 1;
		fWalkPendHeight = 0; //2; //8;
		fWalkPendWidth = 0; //1; //2;
		fWalkPendAngle = 40;
		fWalkPendStep = 5;
		fWalkPendMinStepTime = 0.9;
		fWalkPendFadeTime = 0.5;
		fMaxWalkSidePendSpeed = 5;
        fMaxRaise = 90;
        fMaxRaiseTime = 1;
        fRaiseAccelTime = 0.01;
        fRaiseStopTime = 0.01;
		fMaxJumpDist = 50; //25;
		fMaxJumpHeight = 20; //10;
		fJumpDownWay = 0.5;
		fMaxJumpUpWayTime = 0.5;
		fJumpTimeOut = 0.5;
		fMouseHorzDecayFactor = 2;
        fMouseMaxRaiseTime = 0.00001;
		fMaxTowerSpeed = 30;
		fMaxTowerSpeedTime = 1;
		fMaxTowerStopTime = 1;
        fMouseInterpolateDecay = 100;
		bSwimming = FALSE;
        fSwimmingSpeed = 10;
        fSwimmingStrafeSpeed = 5;
        fSwimmingAccelTime = 0.5;
		update();
	}
	void Read(CConfigFile &cfg,TCchar *szSection) {
		#define	LOADPARM_D(X)	X = cfg.GetDouble(szSection,#X,X);
		#define	LOADPARM_I(X)	X = cfg.GetInt(szSection,#X,X);
		#define	LOADPARM_E(X)	{ TCchar *s = cfg(szSection,#X); if(s) RTCHECK3( (X = (VAE_WHEELS_##X)EnumFromString(s,VAES_WHEELS_##X)) >= 0, "Unknown value: [%s] %s=%s",szSection,#X,s); }
		LOADPARM_D(fRadius)
		LOADPARM_D(g)
		LOADPARM_I(bAllowStrafe)
		LOADPARM_I(bRotatableTower)
		LOADPARM_I(bAutoStop)
		LOADPARM_I(bFollowInclination)
		LOADPARM_I(bInverseY)
		LOADPARM_I(bRestoreRaise)
		LOADPARM_I(bJoyRestoreRaise)
		LOADPARM_I(bFlyControl)
		LOADPARM_I(bJump)
		LOADPARM_E(eAccelControl)
		LOADPARM_D(fHead)
		LOADPARM_D(fMaxFlyAddSpeed)
		LOADPARM_D(fMaxFlyAddStrafeSpeed)
		LOADPARM_D(fFlyFrictionFactor)
		LOADPARM_D(fForceageFactor)
		LOADPARM_D(fMaxSpeed)
		LOADPARM_D(fMaxBackSpeed)
		LOADPARM_D(fMaxStrafeSpeed)
		LOADPARM_D(fMaxStrafeTime)
		LOADPARM_D(fFullStopSpeed)
		LOADPARM_D(fMaxTurnSpeed)
        LOADPARM_D(fMouseTurnSpeed)
		LOADPARM_D(fMaxTurnSpeedTime)
		LOADPARM_D(fTurnStopTime)
		LOADPARM_D(fMaxSpeedTime)
		LOADPARM_D(fStopTime)
		LOADPARM_D(fFrictionFactor)
		LOADPARM_D(fSideFrictionFactor)
		LOADPARM_D(fSideStopTime)
		LOADPARM_D(fBrakingFrictionFactor)
		LOADPARM_D(fInclinationTime)
		LOADPARM_D(fInclFrictionFactor)
		LOADPARM_D(fMaxClimbAngle)
		LOADPARM_D(fNormalRessoreWay)
		LOADPARM_D(fMaxRessoreWay)
		LOADPARM_D(fRessoreFrictionFactor)
		LOADPARM_D(fMaxInclineAngle)
		LOADPARM_D(fMinSlideAngle)
		LOADPARM_D(fMaxAccelAngle)
		LOADPARM_D(fStepUp)
		LOADPARM_D(fWalkPendHeight)
		LOADPARM_D(fWalkPendWidth)
		LOADPARM_D(fWalkPendAngle)
		LOADPARM_D(fWalkPendStep)
		LOADPARM_D(fWalkPendMinStepTime)
		LOADPARM_D(fWalkPendFadeTime)
		LOADPARM_D(fMaxWalkSidePendSpeed)
		LOADPARM_D(fMaxRaise)
		LOADPARM_D(fMaxRaiseTime)
        LOADPARM_D(fMouseMaxRaiseTime)
		LOADPARM_D(fRaiseStopTime)
		LOADPARM_D(fRaiseAccelTime)
		LOADPARM_D(fMaxJumpDist)
		LOADPARM_D(fMaxJumpHeight)
		LOADPARM_D(fJumpDownWay)
		LOADPARM_D(fMaxJumpUpWayTime)
		LOADPARM_D(fJumpTimeOut)
		LOADPARM_D(fMouseHorzDecayFactor)
		LOADPARM_D(fMaxTowerSpeed)
		LOADPARM_D(fMaxTowerSpeedTime)
		LOADPARM_D(fMaxTowerStopTime)
        LOADPARM_D(fMouseInterpolateDecay)
        LOADPARM_D(fSwimmingSpeed)
        LOADPARM_D(fSwimmingStrafeSpeed)
        LOADPARM_D(fSwimmingAccelTime)
		LOADPARM_I(bSwimming)
		#undef	LOADPARM_E
		#undef	LOADPARM_D
		#undef	LOADPARM_I
		update();
	}
	void update() {
		_fFlyFrictionLog = log(fFlyFrictionFactor);
		_fMaxTurnSpeed = fMaxTurnSpeed*M_PI/180;
		_fTurnAccel = _fMaxTurnSpeed/fMaxTurnSpeedTime;
		_fTurnStopAccel = _fMaxTurnSpeed/fTurnStopTime;
		_fFrictionLog = log(Max(1.001,fFrictionFactor));
		_f1FrictionLog = 1./_fFrictionLog;
		_fBrakingFrictionLog = log(Max(1.001,fBrakingFrictionFactor));
		_fSideFrictionLog = log(Max(1.001,fSideFrictionFactor));
		_f1SideFrictionLog = 1./_fSideFrictionLog;
		switch( eAccelControl ) {
			case VAE_WHEELS_AC_THROTTLE:
				_fMaxAccel = fMaxSpeed*_fFrictionLog;
				_fMaxBackAccel = fMaxBackSpeed*_fFrictionLog;
				break;
			case VAE_WHEELS_AC_WALK:
				_fMaxAccel = fMaxSpeed/fMaxSpeedTime;
				_fMaxBackAccel = fMaxBackSpeed/fMaxSpeedTime;
				_fStopAccel = fMaxSpeed/fStopTime;
				_fBackStopAccel = fMaxBackSpeed/fStopTime;
				_fSideStopAccel = fMaxSpeed/fSideStopTime;
				break;
			default: ASSERT(0);
		}
		_f1MaxSpeedTime = 1./fMaxSpeedTime;
		_f1StopTime = 1./fStopTime;
		//_fInclSpeedFact = 1./fInclinationTime;
		_fInclOmega2 = M_PI*M_PI/(4*fInclinationTime*fInclinationTime);
		_fInclFrictionLog = log(Max(1.001,fInclFrictionFactor));
		_f1InclFrictionLog = 1./_fInclFrictionLog;
		_fMaxInclineAngle = fMaxInclineAngle*M_PI/180;
		_fMinInclineCos = cos(_fMaxInclineAngle);
		_f1SinMaxClimbAngle = 1./sin(MinMax(fMaxClimbAngle,1.,89.)*M_PI/180);
		_fCosMaxClimbAngle = cos(MinMax(fMaxClimbAngle,1.,89.)*M_PI/180);
		_f1CosMaxClimbAngle = 1./_f1CosMaxClimbAngle;
		_fMaxClimbAngle = fMaxClimbAngle*M_PI/180;
		fNormalRessoreWay = Max(0.01,fNormalRessoreWay);
		fMaxRessoreWay = Max(fNormalRessoreWay+0.01,fMaxRessoreWay);
		_fRessoreK = g/fNormalRessoreWay;
		_fRessoreTime = M_PI/(2*sqrt(_fRessoreK))/2;
		_fRessoreFrictionLog = log(Max(1.001,fRessoreFrictionFactor));
		_fMinSlideSin = sin(fMinSlideAngle*M_PI/180); _fMinSlideCos = cos(fMinSlideAngle*M_PI/180);
		_fAccelAngleFactor = (fMaxAccelAngle*M_PI/180)/
			(Max(fMaxSpeed,fMaxBackSpeed)*Max(_fFrictionLog,_fBrakingFrictionLog));
		_fWalkPendAngle = fWalkPendAngle*M_PI/180;
		_fWalkPendSpeed = M_PI/fWalkPendStep;
		_fMaxWalkPendEffSpeed = M_PI/(fWalkPendMinStepTime*_fWalkPendSpeed);
		//_fCycloidAngle = fCycloidAngle*M_PI/180;
		//_fCycloidAmpl = fCycloidHeight/Max(1e-5,1-cos(_fCycloidAngle));
		//_fCycloidArgSpeed = M_PI*2/20; //(_fCycloidAngle*2)/fCycloidStep;
		_fMaxRaise = fMaxRaise*M_PI/180;
		_fRaiseSpeed = _fMaxRaise/fMaxRaiseTime;
        _fMouseRaiseSpeed = _fMaxRaise/fMouseMaxRaiseTime;
		_fMaxJumpSpeedV = sqrt(2*g*fMaxJumpHeight);
		_fMaxJumpSpeedH = fMaxJumpDist*g/_fMaxJumpSpeedV;
		_fStrafeAccel = fMaxStrafeSpeed/fMaxStrafeTime;
		_fMouseHorzDecayLog = log(fMouseHorzDecayFactor);
		_fMaxTowerSpeed = fMaxTowerSpeed*M_PI/180;
		_fTowerAccel = _fMaxTowerSpeed/fMaxTowerSpeedTime;
		_fTowerStopAccel = _fMaxTowerSpeed/fMaxTowerStopTime;
        _fMouseTurnSpeed = fMouseTurnSpeed*M_PI/180;
        _fMouseDecayLog = log(fMouseInterpolateDecay);
        _fSwimmingStopLog = 1./fSwimmingAccelTime;

		fStepUp = Max(0.,fHead-fRadius);
	}
};

class CVesselWheels : public CVessel0 {
protected:
	CFMatrix3x4	m_dir, m_base;
	CFVector3	m_speed, m_position, m_offset, m_head;
	CFVector3	m_groundNormal;
	//double		m_fYSpeed;
	CFVector3	m_moment, m_lastAccel;
	double		m_fFuelAccel, m_fThrottle;
	int			m_nForward;
	bool		m_bBrakingDone, m_bTouchGround;
	enum BRAKING_MODE { BRAKING_OFF, BRAKING_ON, BRAKING_DONE } m_eBraking;
	double		m_fTurnReq, m_fRaiseReq, m_fRaise;
	double		m_fRaiseSpeed;
	double		m_fTowerSpeed, m_fTower;
	double		m_fTurnSpeed, m_fJump;
	//double		m_fMaxHorzSpeed;
	CFVector3	m_maxHorzSpeed;
	double		m_fAccelFactor;
	double		m_fJumpOffsetFactor, m_fJumpTimeOut;
	bool		m_bForceage;
	bool		m_bStrafe;
	double		m_fStrafe;
	double		m_fStepTime;
	double		m_fRessoreWay;
	bool		m_bPrevBump;
	CViewObjectModel	*m_pBase;
	CViewObjectRef	*m_pObj;
	double		m_fBumpRadius;
	double		m_fWalkPendulumArg;
	double		m_fWalkPendulumCoeff;
	double		m_fWalkPendSpeedCoeff;
	double		m_fMouseTurnSpeed;
    double      m_fStrafeVert;
	double		m_fMouseRaise;
	double		m_fMouseRaiseSpeed;
	double		m_fJumpT, m_fJumpAccum, m_fJumpDownV, m_fLastFallV;
	CFVector3	m_jumpOffset, m_jumpSpeed, m_walkPendulumWay;
    CFVector2   m_mouse, m_joy, m_rawmouse;
	int			m_nJump; // -1 = downway, 1 = upway, 0 = off
	SWheelsAttrs	m_attrs0;
	SWheelsAttrs	*m_attr;
	void	AccumAccelThrottle(double fDeltaT);
	void	AccumAccelWalk(double fDeltaT);
	CFVector3	HeadOffset() { return m_head-CFVector3(0,m_fBumpRadius,0)+m_jumpOffset*m_fJumpOffsetFactor+m_walkPendulumWay; }
public:
	void	Init();
	CVesselWheels() { Attach(&m_attrs0); }
	~CVesselWheels() { delete m_pObj; delete m_pBase; }
	void	Attach(SWheelsAttrs *attrs) { m_attr = attrs; m_fBumpRadius = attrs->fRadius; m_head.y = attrs->fHead; }
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
	void	SetPos(TCCFVector3 &v) {
		m_position = v-m_base.MultTF(HeadOffset());
		#ifndef __VESSEL_IGNORE_MASTER
			m_master->setPosition(m_position);
		#endif
	}
	TCCFVector3 Pos() { return m_position+m_base.MultTF(HeadOffset()); }
	TCCFVector3 Speed() { return m_speed; }
	CFMatrix3x4 &GetDir() { return m_dir; }
    void SetDir(TCSFMatrix3x4 &dir) {
		double	fAngleX, fAngleY, fAngleZ;
		dir.RestoreEuler(CFMatrix3x4::AXIS_OY,fAngleY,CFMatrix3x4::AXIS_OX,fAngleX,CFMatrix3x4::AXIS_OZ,fAngleZ);
		//m_dir = dir;
		m_dir.LoadIdentity().EulerL(CFMatrix3x4::AXIS_OY,fAngleY,CFMatrix3x4::AXIS_OX,0,CFMatrix3x4::AXIS_OZ,0);
		//m_dir.LoadOffset(CFVector3(0,0,0));
		(m_base = m_dir).RotateOxL(-m_fRaise).RotateOyL(-m_fTower);
	}
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
	void	Jump(double fDelta) { m_fJump = MinMax(fDelta,-1.,1.); }
	void	Raise(double fDeltaUp)  { m_fRaiseReq = -MinMax(fDeltaUp,-1.,1.); }
	void	Incline(double fDeltaRight) { m_fTurnReq = MinMax(fDeltaRight,-1.,1.); }
	void	Rotate(double fDeltaRight) { (void)fDeltaRight; }
    void    StrafeVert(double fDelta) { m_fStrafeVert = MinMax(fDelta,-1.,1.); }
	void	StrafeHorz(double fDelta) { m_fStrafe = MinMax(fDelta,-1.,1.); }
	void	SetStrafe(bool bStrafe) { m_bStrafe = m_attr->bAllowStrafe && bStrafe; }
	void	SetForceage(bool b) { m_bForceage = MAKEBOOL(b); }
    void    MoveMouse(double dx,double dy) {
                m_rawmouse.x += dx/1e3; m_rawmouse.y += dy/5e2;
                //m_mouse.x += dx/1e3; m_mouse.y += dy/5e2;
                m_mouse = m_mouse*0.5+m_rawmouse*0.5;
            }
    void    Joystick(double x,double y) { m_joy.x = MinMax(x,-1.,1.); m_joy.y = -MinMax(y,-1.,1.); }
	void	Stop() {
		m_moment = m_speed = m_lastAccel = CFVector3(0,0,0); m_fFuelAccel = m_fThrottle = 0;
		m_eBraking = BRAKING_OFF;
        m_rawmouse = m_mouse = CFVector2(0,0);
	}
    TCvoid *SaveGame(); // { ASSERT(0); return NULL; }
    bool LoadGame(TCvoid *pSavedGame); // { (void)pSavedGame; ASSERT(0); return FALSE; }

	void	NextFrame();

	void Depromote() { m_pPromoteNode->RemoveDyn(this); }
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////



#endif
