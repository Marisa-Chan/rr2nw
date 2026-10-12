#define LAST_H__SCENE
#include "game.h"
#include "filesys.h"
#include "vs_zav.h"

//#define	CHECK_DELTAT()	ASSERT(fDeltaT>=0&&fDeltaT<2)
//#define	CHECK_DELTAT()	(void)0
#define	CHECK_DELTAT()	RTCHECK1(fDeltaT>=0&&fDeltaT<24*3600.,"Bad deltaT=%lg in dynamics",fDeltaT)

//CViewObjectModel	*CVessel::m_pBase = NULL;

CViewObjectRef	*pVesselObj = NULL;

CViewScene *ZAV_Scene();
double		__t = 0;
CFVector3	__sp, __spfall;



#if 0
#define	BEGIN_LOG_VESSEL()	dword dwOldDebugFlags = SetDebugFlags(GetDebugFlags()|DBGF_LOG_MSG);
#define	END_LOG_VESSEL()	SetDebugFlags(dwOldDebugFlags);
#else
#define	BEGIN_LOG_VESSEL()
#define	END_LOG_VESSEL()
#endif

static	bool	BumpPlane(SBumpDef &def,double a,double b)
{
	if( a >= 0 ) return FALSE;
	if( b <= 0 ) {
		def.fTime = 0;
		def.vel1 = def.vel;
		def.nBumpFlags = BF_BUMPSTATIC;
		return TRUE;
	}
	double	t = b/Max(1e-5,-a);
	if( t < def.fTime ) {
		def.fTime = t;
		def.vel1 = def.vel;
		def.nBumpFlags = BF_BUMPSTATIC;
		return TRUE;
	}
	return FALSE;
}

bool	CVessel0::AllBump(SBumpDef &def,bool bChangeVel)
{
	(void)bChangeVel;
	bool bRes =	ZAV_Scene()->Bump(def);


	bool	bRes1 = InternalBump(def);
	bRes = 	bRes || bRes1;

	if( m_pfBounds ) {
		if( bRes |= BumpPlane(def,def.vel.x,def.start.x-m_pfBounds[0]) )
			def.vel1.x = -def.vel1.x;
		if( bRes |= BumpPlane(def,-def.vel.x,m_pfBounds[1]-def.start.x) )
			def.vel1.x = -def.vel1.x;
		if( bRes |= BumpPlane(def,def.vel.y,def.start.y-m_pfBounds[2]) )
			def.vel1.y = -def.vel1.y;
		if( bRes |= BumpPlane(def,-def.vel.y,m_pfBounds[3]-def.start.y) )
			def.vel1.y = -def.vel1.y;
		if( bRes |= BumpPlane(def,def.vel.z,def.start.z-m_pfBounds[4]) )
			def.vel1.z = -def.vel1.z;
		if( bRes |= BumpPlane(def,-def.vel.z,m_pfBounds[5]-def.start.z) )
			def.vel1.z = -def.vel1.z;
	}

	#ifdef __VESSEL_IGNORE_MASTER
		return bRes;
	#else
		bRes1 = m_MasterBumpCallBack(m_master,def);
		return bRes || bRes1;
	#endif
}

void	DSolve2HReal(double &x,double &v,double R,double omega,double xoff,double t)
// x''+R*x'+K*x == 0,  K = omega*omega
{
	ASSERT( omega > 1e-10 );
	double  K = omega*omega;
	R = Max( R, 2.001*omega );
	// R*R > 4*omega*omega,  R > 2*omega, R > freq*4*PI,  R > 4*PI/time

	double	x0 = x-xoff;
	double	v0 = v;

	double	D = R*R-4*K;
	ASSERT( D > Max(fabs(R*R),fabs(4*K))*1e-5 && K > 1e-10 && R >= 0 );
	D = sqrt(D);

	double	L1 = -R-D, L2 = -R+D;
	double	E1 = exp(L1*0.5*t), E2 = exp(L2*0.5*t);

	x = xoff + x0*0.5*(E1+E2)+(v0+R*x0*0.5)*(E2-E1)/D;

	v = (R*(v0*0.5+R*x0*0.25)/D-D*x0*0.25)*(E1-E2)+v0*0.5*(E1+E2);
}

void DSolve2NHO0(double &x,double &v,double R,double F,double t)
// x''+R*x' == F
{
	ASSERT( R > 1e-10 );
	double x0 = x, v0 = v;
	double E = exp(-R*t);
	x = x0 + ((v0-F/R)*(1-E)+F*t)/R;
	v = E*v0 + F/R*(1-E);
}

void DSolve2NHO0(CFVector3 &x,CFVector3 &v,double R,TCSFVector3 &F,double t)
// x''+R*x' == F
{
	ASSERT( R > 1e-10 );
	CFVector3 x0 = x, v0 = v;
	double E = exp(-R*t);
	x = x0 + ((v0-F/R)*(1-E)+F*t)/R;
	v = E*v0 + F/R*(1-E);
}

void DSolve2NHO0(CFVector3 &x,CFVector3 &v,TCSFVector3 &R,TCSFVector3 &F,double t)
// x''+R*x' == F
{
	ASSERT( R.x > 1e-10 );
	ASSERT( R.y > 1e-10 );
	ASSERT( R.z > 1e-10 );
	CFVector3 x0 = x, v0 = v;
	CFVector3 E(exp(-R.x*t),exp(-R.y*t),exp(-R.z*t));
	CFVector3 _1_E(1-E.x,1-E.y,1-E.z);
	CFVector3 _R(1/R.x,1/R.y,1/R.z);
	x = x0 + ((  ((v0-(F&_R))&_1_E) + F*t)&_R);
	v = (E&v0) + (F&_R&_1_E);
}

void	PullValue(double &x,double &v,double xtarg,double fMaxSpeed,double fAccelTime,double fStopTime,double t)
{
	//fTurnReq = m_mouse.x;
	double	vtarg;
	double	xmax, xmin;

	//double fMouseCoeff = m_attr->_fMaxTurnSpeed;
	//double fTargetTurn = x + fTurnReq*fMouseCoeff;
	//fTargetTurnSpeed = 0 < fTargetTurn ? m_attr->_fRaiseSpeed : -m_attr->_fRaiseSpeed;
	if( x /*0*/ < xtarg /*fTargetTurn*/ )
		xmax = xtarg, xmin = x, vtarg = fMaxSpeed;
	else
		xmax = x, xmin = xtarg, vtarg = -fMaxSpeed;

	double	fMaxAccel = fMaxSpeed/fAccelTime;
	double	fMaxStopAccel = fMaxSpeed/fStopTime;
	double fAccel = v < vtarg ? fMaxAccel : -fMaxAccel;
	double fStopAccel;
	if( v >= 0 ) {
		fStopAccel = -v*v*0.5/Max(1e-5,xmax);
		if( fStopAccel > -fMaxStopAccel )	fStopAccel = 1e10;
		if( fAccel >= fStopAccel ) {
			fAccel = fStopAccel;
			vtarg = 0;
		}
	}
	else {
		fStopAccel = -v*v*0.5/Min(-1e-5,xmin);
		if( fStopAccel < fMaxStopAccel )	fStopAccel = -1e10;
		if( fAccel <= fStopAccel ) {
			fAccel = fStopAccel;
			vtarg = 0;
		}
	}

	double	fEndSpeed = v+fAccel*t;
	if( fAccel >= 0 )
		fEndSpeed = Min(vtarg,fEndSpeed);
	else
		fEndSpeed = Max(vtarg,fEndSpeed);

	double	deltax = (v+fEndSpeed)*0.5*t;  // fMouseTurn
	v = fEndSpeed;
	x = MinMax(x+deltax,xmin,xmax);
	//m_mouse.x -= fMouseTurn/fMouseCoeff;
	//m_mouse.x *= exp(-m_attr->_fMouseHorzDecayLog*fDeltaT);
	v = MinMax(v,-fMaxSpeed,fMaxSpeed);
}

// ------------------------------------------------------------------------
//double CVesselEmv::m_fMaxShield = 12.5;
CVesselEmv::CVesselEmv()
	:   /*CMovingObject(2),*/
        m_speed(0,0,0), m_position(0,2,20), m_rand1f(5,1)
		/*m_shotSeq(100)*/
{
	Attach(&m_attrs0);
	//m_fMaxShield = 12.5; //MISSIONCONFIG(m_vessel.fShield);
	m_pObj = NULL;
	m_pBase = NULL;
 	//m_bHighlightShots = TRUE;
	//m_DisplayMessage = NULL;
	m_dynBase = m_dynBase1 = m_position;
}

void	CVesselEmv::Init()
{
	ASSERT(SHOOTER_COUNT==CShot::T_SUP);
	m_bPrevBump = FALSE;
	m_fFuelSpeed = 0;
	m_fThrottle = 0;
	m_fVertRotReq = m_fVertRot = 0;
	//m_fVertRotCtl = 0;
	m_fAxisRotReq = m_fAxisRot = 0;
	m_fInclineReq = m_fInclination = m_fInclineRot = 0;
	m_fStrafeInclination = m_fStrafeInclineRot = 0;
	m_fTurnRot = 0;
	m_fStrafeReq = 0;
	m_fStrafeVertReq = 0;
	m_fVertStrafe = 0;
	m_bForceage = FALSE;
//	m_fAngleX = 0, m_fAngleY = 0, m_fAngleZ = 0;
	m_dir.LoadIdentity();
	m_speed = CFVector3(0,0,0);
	m_fallSpeed = m_speed;
	m_fStepTime = 0;
	m_bump.pBonus = NULL;
	//m_bonus.nBonus = 0;
	//m_bonus.pOwner = this; 3
	m_bStrafe = FALSE;
	m_joy.x = m_joy.y = 0;
	m_mouse.x = m_mouse.y = 0;
	m_fTurnRotMouse = 0;
	m_fVertRotMouse = 0;
	m_moment = CFVector3(0,0,0);
    m_fWingRaise = 0;
    m_fWingAmpl = 0;
    m_fWingTime = 0;
	m_bTouchGround = FALSE;
	m_bumpNormal = CFVector3(0,1,0);
	m_nBumpCountStep = 0;
}

void	CVesselEmv::Restart()
{
	Init();
}

void	CVesselEmv::Stop()
{
	m_fFuelSpeed = 0;
	m_fThrottle = 0;
	m_fVertRotReq = m_fVertRot = 0;
	//m_fVertRotCtl = 0;
	m_fAxisRotReq = m_fAxisRot = 0;
	m_fInclineReq = m_fInclination = m_fInclineRot = 0;
	m_fStrafeInclination = m_fStrafeInclineRot = 0;
	m_fStrafeReq = 0;
	m_bStrafe = FALSE;
	m_fTurnRot = 0;
	m_fStrafeVertReq = 0;
	m_fVertStrafe = 0;
	m_bForceage = FALSE;
//	m_fAngleX = 0, m_fAngleY = 0, m_fAngleZ = 0;
	m_speed = CFVector3(0,0,0);
	m_fallSpeed = m_speed;
	m_moment = CFVector3(0,0,0);
	m_fStepTime = 0;
	m_joy.x = m_joy.y = 0;
	m_mouse.x = m_mouse.y = 0;
	m_fTurnRotMouse = 0;
	m_fVertRotMouse = 0;
}

bool	CVesselEmv::InternalBump(SBumpDef &def)
{
	if( def.vel.y <= 0 ) return FALSE;
	if( def.start.y >= m_attr->fMaxPossibleHeight ) {
		def.fTime = 0;
		def.vel1 = def.vel;
		def.vel1.y = -def.vel.y;
		def.nBumpFlags = BF_BUMPSTATIC;
		return TRUE;
	}
	double	t = (m_attr->fMaxPossibleHeight-def.start.y)/Max(1e-5,def.vel.y);
	if( t < def.fTime ) {
		def.fTime = t;
		def.vel1 = def.vel;
		def.vel1.y = -def.vel.y;
		def.nBumpFlags = BF_BUMPSTATIC;
		return TRUE;
	}
	return FALSE;
}

void	CVesselEmv::Draw()
{
	#ifdef __VESSEL_IGNORE_MASTER
	if( pVesselObj ) {
		//m_fBumpRadius = m_bump.fRadius = pVesselObj->Model()->Radius0();
		CFMatrix3x4	dt, dir;
		dt.LoadTransposed(m_dir);
		dir.LoadIdentity() /*.RotateOyL(M_PI)*/.TranslateL(-pVesselObj->Model()->Center());
		pVesselObj->GetDirModify().LoadMult(dt,dir).TranslateL(m_position);
		pVesselObj->LoadLights(m_dwLights);
		pVesselObj->Draw();
	}
	#endif
	#if 0
	if(!(m_nExistFlags & EF_VIEWOBJ))	return;
	if( m_pObj ) {
		m_pObj->GetDirModify().LoadTransposed(m_dir).TranslateL(m_position);
		ResetLastDrawn();
		m_pObj->Draw();
		m_bDrawn = IsLastDrawn();
	}
	#endif
}

void	CVesselEmv::AccumPreStep(double fDeltaT)
{
	CHECK_DELTAT();
	BEGIN_LOG_VESSEL()
	//ASSERT(fDeltaT>=0&&fDeltaT<2);
	double	fAngleY = atan2(-m_dir.m[2][0],m_dir.m[2][2]);
	double	fAngleX = atan2(m_dir.m[2][1],hypot(m_dir.m[2][0],m_dir.m[2][2]));
	CFVector3	normalY(m_dir.m[1][0],m_dir.m[1][1],m_dir.m[1][2]);
	// fAngleZ=atan2(projX'(normalY),projY'(normalY))
	double	fAngleZ = atan2(CFVector3(cos(fAngleY),0,sin(fAngleY))*normalY,
				CFVector3(sin(fAngleX)*sin(fAngleY),
					cos(fAngleX),-sin(fAngleX)*cos(fAngleY))*normalY);

	double fSpeedNormalized = Min(1.,fabs(m_speed*m_dir.Row(2))/m_attr->fMaxSpeed);
	if( m_attr->eGravityMode != VAE_EMV_GM_PLANE )	fSpeedNormalized = 1;

	//double fInclination = m_fInclination*m_fStrafeInclination<0?
	//	m_fInclination+m_fStrafeInclination:
	//	(fabs(m_fInclination)>fabs(m_fStrafeInclination)?m_fInclination:m_fStrafeInclination);
	double fInclination = m_fInclination;
	double	fAxisAngle = fAngleZ-fInclination; //m_fInclination-m_fStrafeInclination;

	//double	fDeltaIncline = 0;
	double fMouseInclineReq = m_mouse.x/Max(1e-5,fDeltaT);
	double	fInclineReq = MinMax(m_fInclineReq+m_joy.x+fMouseInclineReq,-1.,1.);
	double	fDeltaTurn = 0;
	{
		double fTargetSpeed = m_bStrafe ? 0 : m_attr->_fMaxTurnSpeed*fInclineReq;
		bool	bInclineAccel = m_fTurnRot*m_fTurnRot < fTargetSpeed*m_fTurnRot;
		//double	fIncline = m_fInclination;
		{
			//double T = bInclineAccel ? m_attr->fInclinationTime : m_attr->fInclinationRestoreTime;
			double T = m_attr->fInclinationTime;
			T *= 4;
			double omega = 2*M_PI/T;
			double R = 2*omega*m_attr->fInclineFrictionCoeff;
			double xoff = MinMax(fInclineReq+m_fStrafeReq,-1.,1.)*m_attr->_fMaxIncline;
			xoff *= fSpeedNormalized;
			DSolve2HReal(m_fInclination,m_fInclineRot,R,omega,xoff,fDeltaT);
		}
		//fDeltaIncline = fIncline - m_fInclination;
		{
			double fTime = bInclineAccel ?
								m_attr->fMaxTurnSpeedTime : m_attr->fMaxTurnStopTime;
			double R = 1/fTime,
				   F = fTargetSpeed*R;
			DSolve2NHO0(fDeltaTurn,m_fTurnRot,R,F,fDeltaT);

			//void	PullValue(double &x,double &v,double xtarg,double fMaxSpeed,double fAccelTime,double fStopTime,double t)
			double fDeltaTurnMouse = 0;
			double fTurnReq = m_mouse.x*m_attr->_fMaxTurnSpeed;
			PullValue(fDeltaTurnMouse,m_fTurnRotMouse,fTurnReq,m_attr->_fMaxTurnSpeed,
					m_attr->fMaxTurnSpeedTime,m_attr->fMaxTurnStopTime,fDeltaT);
			m_mouse.x -= fDeltaTurnMouse/m_attr->_fMaxTurnSpeed;
			m_mouse.x *= exp(-m_attr->_fMouseHorzDecayLog*fDeltaT);
			fDeltaTurn = AbsNoGreater(fDeltaTurn+fDeltaTurnMouse,m_attr->_fMaxTurnSpeed*fDeltaT);
		}
	}
	// ------------------------------------------------------------------
	double fDeltaAxis = 0;
	{
		double  omega = (2*M_PI)/m_attr->fAxisRotTime;
		double	R = 2.01*omega*m_attr->fAxisRotFrictionCoeff;
		double	fLevellingAngle = atan2(sin(fAxisAngle*2),cos(fAxisAngle*2))/2;
		//double	a0 = fAxisAccel*m_fAxisRotReq;
		double	xoff = -m_fAxisRotReq*fSpeedNormalized*M_PI/2*(m_attr->fAxisRotControlCoeff+1);
		double  coeff = fabs(cos(fAngleX));
		xoff /= coeff*coeff;
		double  fLevellingAngleNew = fLevellingAngle;
		DSolve2HReal(fLevellingAngleNew,m_fAxisRot,R,
									omega*coeff,xoff,fDeltaT);
		fDeltaAxis = fLevellingAngleNew-fLevellingAngle;

		/*
		const	double	fMaxAxisTurn = M_PI,
						fAxisAccel = 9*M_PI;
		double	fLevellingAngle = atan2(sin(fAxisAngle*2),cos(fAxisAngle*2))/2;
		double	RMax = fAxisAccel/fMaxAxisTurn;
		double R = RMax*fabs(cos(fAngleX));
		double D = sqrt(4*RMax)*1.1;
		double d = sqrt(D*D-4*R);
			// D = (2*sqrt(RMax))^2-4*R = 4*RMax-4*R  (R'=2*RMax, K'=R) F=a0
		double lambda1 = (-D+d)/2, lambda2 = (-D-d)/2;
		double a0 = fAxisAccel*m_fAxisRotReq;
		double v0 = m_fAxisRot; double s0 = fLevellingAngle;
		double C1 = (v0-lambda2*s0)/d; double C2 = (lambda1*s0-v0)/d;
		double C1v = a0/d/lambda1; double C2v = -a0/d/lambda2;
		double C1s = C1+C1v; double C2s = C2+C2v;
		fDeltaAxis = C1s*exp(lambda1*fDeltaT)-C1v+
						C2s*exp(lambda2*fDeltaT)-C2v - s0;
		m_fAxisRot = lambda1*C1s*exp(lambda1*fDeltaT)+
						lambda2*C2s*exp(lambda2*fDeltaT);
		*/
	}

	// -----------------------------------------------------------

	double	fVertRotReq = MinMax(m_fVertRotReq+m_joy.y,-1.,1.)*fSpeedNormalized;
	double	fDeltaVert = 0;
	double	fFallAngle = 0;
	double	fFallFactor = 0;
	{
		double	fTargetSpeed = m_bStrafe ? 0 : -fVertRotReq*m_attr->_fMaxRaiseSpeed;
		bool	bRaiseAccel = m_fVertRot*m_fVertRot < fTargetSpeed*m_fVertRot;
		double fTime = bRaiseAccel ?
							m_attr->fMaxRaiseSpeedTime : m_attr->fMaxRaiseStopTime;
		double R = 1/fTime,
				F = fTargetSpeed*R;
		#if 1
		double	s = -((m_speed-m_fallSpeed)*m_dir.Row(2));

		if( m_attr->eGravityMode == VAE_EMV_GM_PLANE && s < m_attr->fMinFlyingSpeed && m_nBumpCountStep <= 0 ) {
			fFallFactor = Max(0.,1-s/m_attr->fMinFlyingSpeed);
			fFallAngle = m_attr->_fFallAngle*fFallFactor;
			double omega = MinMax(fFallFactor,0.0001,1.)*m_attr->_fFallOmega;
			if( omega*2.001 > R )	omega = R/2.001;
			//fabs(s)<1e-10 ? M_PI/2 : atan2(fabs(m_fallSpeed.y),fabs(s));
			double	fTargetDelta = fFallAngle-fAngleX;
			DSolve2HReal(fDeltaVert,m_fVertRot,R,omega,fTargetDelta+F/(omega*omega),fDeltaT);
		}
		else
		#endif
			DSolve2NHO0(fDeltaVert,m_fVertRot,R,F,fDeltaT);

		//void	PullValue(double &x,double &v,double xtarg,double fMaxSpeed,double fAccelTime,double fStopTime,double t)
		double fDeltaVertMouse = 0;
		double fVertReq = m_mouse.y*m_attr->_fMaxRaiseSpeed*fSpeedNormalized;
		PullValue(fDeltaVertMouse,m_fVertRotMouse,fVertReq,m_attr->_fMaxRaiseSpeed,
				m_attr->fMaxRaiseSpeedTime,m_attr->fMaxRaiseStopTime,fDeltaT);
		m_mouse.y -= fDeltaVertMouse/m_attr->_fMaxRaiseSpeed;
		m_mouse.y *= exp(-m_attr->_fMouseVertDecayLog*fDeltaT);
		fDeltaVert = AbsNoGreater(fDeltaVert+fDeltaVertMouse,m_attr->_fMaxRaiseSpeed*fDeltaT);

	}

	// -----------------------------------------------------------
	// m_dir.LoadIdentity().RotateOyL(fAngleY).RotateOxL(fAngleX).
	//					RotateOzL(fAngleZ);
	m_dir.LoadIdentity().RotateOyL(fAngleY).
			RotateOxL(fAngleX).RotateOzL(fAxisAngle);
	CFVector3	rightN(m_dir.m[0][0],m_dir.m[0][1],m_dir.m[0][2]),
				upN(m_dir.m[1][0],m_dir.m[1][1],m_dir.m[1][2]),
				backN(m_dir.m[2][0],m_dir.m[2][1],m_dir.m[2][2]);
	/*FPCheckNAN(fDeltaTurn);
	FPCheckNAN(fDeltaIncline);
	FPCheckNAN(m_fInclination);
	FPCheckNAN(fDeltaIncline);
	FPCheckNAN(m_fStrafeInclination);
	FPCheckNAN(fDeltaStrafeIncline);
	FPCheckNAN(fDeltaAxis);
	FPCheckNAN(fDeltaVert);*/
	/*fInclination = m_fInclination*m_fStrafeInclination<0?
		m_fInclination+m_fStrafeInclination:
		(fabs(m_fInclination)>fabs(m_fStrafeInclination)?m_fInclination:m_fStrafeInclination);*/
	//double fInclination = m_fInclination+m_fStrafeInclination;
	// -----------------------------------------------------------
	double fForceage = MAKEBOOL(m_bForceage)*(m_attr->fForceageFactor-1)+1;
	const double fSpeedFact = 1;
	double fMaxSpeed = m_attr->fMaxSpeed; //30*fSpeedFact,
	double fFuelAccel = m_attr->fMaxSpeed/m_attr->fMaxSpeedTime; //70;
	double fMinSpeed = -m_attr->fMaxBackSpeed;
	m_fFuelSpeed = Max(-fMinSpeed*Min(m_fThrottle,0.),
			Min(fMaxSpeed,m_fFuelSpeed+m_fThrottle*fMaxSpeed*fDeltaT/1.5));

	if( !m_bPrevBump ) {
		//const double fHorzStrafe = 30*fSpeedFact*fForceage, fVertStrafe = 30*fSpeedFact*fForceage;
		double fHorzStrafe = m_attr->fHorzStrafeSpeed*fForceage, fVertStrafe = m_attr->fVertStrafeSpeed*fForceage;
		fHorzStrafe *= fSpeedNormalized;
		fVertStrafe *= fSpeedNormalized;
		double R = fFuelAccel/fMaxSpeed;  // 1/MaxSpeedTime
		CFVector3	vReq =
					(-m_fFuelSpeed*fForceage)*backN+
					//(m_fStrafeInclination*fHorzStrafe/fMaxIncline)*rightN+
					(MinMax(fInclineReq*m_bStrafe+m_fStrafeReq,-1.,1.)*fHorzStrafe)*rightN+
					(MinMax(-fVertRotReq*m_bStrafe+m_fStrafeVertReq,-1.,1.)*fVertStrafe)*upN;
		/*CFVector3	F = CFVector3(0,-m_attr->g,0);
		double	FRaise = -(m_speed*backN);
		if( FRaise < 0 )	FRaise = 0;
		F += upN*(FRaise**/
		//static	CFVector3	fallSpeed(0,0,0);
		#if 1
		if( m_attr->eGravityMode == VAE_EMV_GM_PLANE ) {
			CFVector3	F = CFVector3(0,0,0);
			//double	fMinSpeed = 10, fMaxFallSpeed = 50;
			double	s = Abs(m_speed-m_fallSpeed);
			double	Rfall = m_attr->g/m_attr->fMaxFallSpeed;
			if( s < m_attr->fMinFlyingSpeed ) {
				double	f = fFallFactor;
				if( m_nBumpCountStep <= 0 ) {
					CFVector2	dirHorz = Normal(CFVector2(-m_dir.m[2][0],-m_dir.m[2][2]));
					F = (m_attr->g*f*cos(fFallAngle))*CFVector3(dirHorz.x,0,dirHorz.y);
				}
				F.y = -m_attr->g*f*sin(m_attr->_fFallAngle);
			}
			CFVector3 temp(0,0,0);
			DSolve2NHO0(temp,m_fallSpeed,Rfall,F,fDeltaT);
			vReq += m_fallSpeed;
		}
		#endif
		DSolve2NHO0(m_offset,m_speed,R,vReq*R,fDeltaT);
		/*{
			CFVector3	offset(0,0,0);
			CFVector3	speed = m_dir*m_fallSpeed;
			CFVector3	RFallY(1e-5,0.1,1e-5);
			CFVector3	g = m_dir*CFVector3(0,-m_attr->g,0);
			DSolve2NHO0(offset,speed,RFallY,g,fDeltaT);
			m_offset += m_dir.MultTF(offset);
			m_fallSpeed = m_dir.MultTF(speed);
		}*/
		//ASSERT(Abs(m_speed) <= fMaxSpeed*20);
	}
	else m_bPrevBump = FALSE;

	/*
	if (Abs(m_speed) > fMaxSpeed*10) {
		m_speed = Normal(m_speed) * fMaxSpeed;
	}
	ASSERT(Abs(m_speed) <= fMaxSpeed*10);
	*/
	fInclination = m_fInclination;
	m_dir.RotateOyL(fDeltaTurn).
			RotateOzL(fInclination+
						//(m_fStrafeInclination+=0/*fDeltaStrafeIncline*/)+
														fDeltaAxis).
			RotateOxL(fDeltaVert);


    m_fWingTime = m_fWingTime + fDeltaT*(1+MinMax(m_rand1f(fDeltaT)*m_attr->fRandomizeWing,-0.5,0.5));
    double  fOldRaise = m_fWingRaise*m_fWingAmpl;
    double  x = m_fWingTime*m_attr->_fWingOmega;
    while( x > 2*M_PI ) x -= 2*M_PI;
    x *= (1./(2*M_PI));
    m_fWingRaise = -cos(2*M_PI*(x+m_attr->fWingSuspend*(1./(2*M_PI))*sin(2*M_PI*x+m_attr->_fSuspendOffset)));
	m_fWingAmpl = (fabs(m_fFuelSpeed)/m_attr->fMaxSpeed)*m_attr->_fWingAmpl;
    m_dir.RotateOxL(-(fOldRaise-m_fWingRaise*m_fWingAmpl));

	//m_offset += m_speed*fDeltaT;
	m_fStepTime += fDeltaT;

	END_LOG_VESSEL()
}

void	CVesselEmv::QuickAccumPreStep(double fDeltaT)
{
	CHECK_DELTAT();
	//ASSERT(fDeltaT>=0&&fDeltaT<2);
	m_offset += m_speed*fDeltaT;
	m_fStepTime += fDeltaT;
}

void	CVesselEmv::PreStep(double fDeltaT)
{
	CHECK_DELTAT();
	//ASSERT(fDeltaT>=0&&fDeltaT<2);

	m_bump.start = m_position;
	m_bump.vel = m_offset/(fDeltaT>1e-10?fDeltaT:1e-10);
	//m_bump.vel = m_speed;
	m_bump.fTime = m_fStepTime = fDeltaT;
	//m_bump.fRadius = 2;
	m_bump.fRadius = m_fBumpRadius;
	#ifdef __VESSEL_IGNORE_MASTER
		//if( pVesselObj )	m_bump.fRadius = pVesselObj->Model()->Radius();
	#endif
	m_bump.nBumpFlags = 0;
	m_bump.dwFlags1 = BF1_ADVANCED;
	//m_bonus.nBonus = 0;
	m_dynBase = m_dynBase1 = m_position;
	//m_bDrawn = FALSE;
}

#define	_NEXTFRAME_TYPE	1

void	CVesselEmv::NextFrame()
{
	// ------------------------
	SBumpDef	&def = m_bump, def0 = def;
	double	fTime0 = def.fTime;
	//SetDebugFlags(DBGF_DEFAULT|DBGF_LOG_MSG);
	//CViewScene	*pScene = ZAV_Scene();
	//pScene->Bump(def);
	AllBump(def,TRUE);
	//def.nBumpFlags = 0; //
	//def.fTime = fTime0; //
	//const	int		__s = 1;

	double	R = log(10);

	bool		bBumpNormal = FALSE;
	CFVector3	bumpNormal;

	if(def.nBumpFlags) {
		#if _NEXTFRAME_TYPE
		{
			m_position += def.vel*def.fTime-0.001*Normal(def.vel);
			def.start += def.vel*def.fTime-0.001*Normal(def.vel);
			CFVector3	n = def.vel1-def.vel;
			def.vel = (def.vel+def.vel1)/2;
			def.fTime = fTime0-def.fTime;
			if( m_attr->eGravityMode == VAE_EMV_GM_PLANE &&
							Abs2(n) >= 1e-10 && Abs2(def.vel1) >= 1e-10 ) {
				n = Normal(n);
				bBumpNormal = TRUE;
				bumpNormal = n;
				double	fFriction = exp(-R*def.fTime*(Normal(def.vel1)*n));
				ASSERT(fFriction>=0);
				m_fallSpeed -= ((m_fallSpeed*n))*n;
				def.vel *= fFriction;
				m_fallSpeed *= fFriction;
				if( n.y >= m_attr->_fMaxSlidingCos && Max(Abs(def.vel),fabs(m_fFuelSpeed)) < m_attr->fMaxSlidingSpeed )
					def.vel = m_speed = m_fallSpeed = CFVector3(0,0,0);
			}
		}
		#else
		{
			CFVector3	ds = def.vel*def.fTime-0.001*Normal(def.vel)+
						(def.vel+def.vel1)/2*(fTime0-def.fTime);
			def.vel = ds/(fTime0>1e-10?fTime0:1e-10);
			def.fTime = fTime0;
		}
		#endif
		def.nBumpFlags = 0;
		//pScene->Bump(def);
		AllBump(def,TRUE);
		if(def.nBumpFlags) {
			#if _NEXTFRAME_TYPE
			{
				m_position += def.vel*def.fTime-0.001*Normal(def.vel);
				def.start += def.vel*def.fTime-0.001*Normal(def.vel);
				CFVector3	n = def.vel1-def.vel;
				def.vel = (def.vel+def.vel1)/2;
				def.fTime = fTime0-def.fTime;
				if( m_attr->eGravityMode == VAE_EMV_GM_PLANE &&
								Abs2(n) >= 1e-10 && Abs2(def.vel1) >= 1e-10 ) {
					n = Normal(n);
					bBumpNormal = TRUE;
					bumpNormal = n;
					double	fFriction = exp(-R*def.fTime*(Normal(def.vel1)*n));
					ASSERT(fFriction>=0);
					m_fallSpeed -= ((m_fallSpeed*n))*n;
					def.vel *= fFriction;
					m_fallSpeed *= fFriction;
					if( n.y >= m_attr->_fMaxSlidingCos && Max(Abs(def.vel),fabs(m_fFuelSpeed)) < m_attr->fMaxSlidingSpeed )
						def.vel = m_speed = m_fallSpeed = CFVector3(0,0,0);
				}
			}
			#else
			{
				CFVector3	ds = def.vel*def.fTime-0.001*Normal(def.vel)+
								(def.vel+def.vel1)/2*(fTime0-def.fTime);
				def.vel = ds/(fTime0>1e-10?fTime0:1e-10);
				def.fTime = fTime0;
			}
			#endif
			def.nBumpFlags = 0;
			//pScene->Bump(def);
			AllBump(def,TRUE);
			if(def.nBumpFlags) {
				CFVector3	n = def.vel1-def.vel;
				n = Normal(n);
				bBumpNormal = TRUE;
				bumpNormal = n;
			}
		}
	}

	bool bTouchGround = bBumpNormal;
	if( bTouchGround ) {
		m_nBumpCountStep = 2;
		m_bumpNormal = bumpNormal;
	}
	else
		if( m_nBumpCountStep > 0 ) m_nBumpCountStep--;
	if( m_nBumpCountStep > 0 && !bBumpNormal ) {
		bumpNormal = m_bumpNormal;
		bBumpNormal = TRUE;
	}
	m_bTouchGround = bTouchGround;

	if( m_attr->eGravityMode == VAE_EMV_GM_PLANE ) {
		if( bBumpNormal ) {
			CFVector3	n = m_dir.Row(1);
			double	a = acos(n*bumpNormal);
			CFVector3	axis = Normal(bumpNormal%n);
			m_dir.RotateR(axis,a*(1-exp(-m_fStepTime*2)));
		}
		CFVector3	n1 = bumpNormal, axis, n0;

		for( double t = m_fStepTime ; /*t >= 0*/ ; ) {
			if( bTouchGround ) {
				n0 = m_dir.Row(1);
				axis = n1%n0;
			}
			double t1 = m_attr->fGroundInclinationTime*0.5;
			if( t < t1 ) {
				if( bTouchGround ) m_moment += Normal(axis)*atan2(Abs(axis),n0*n1)*m_attr->_fGroundInclOmega2*t;
				double e = exp(-m_attr->_fGroundInclFrictionLog*t);
				m_dir.RotateR(Normal(m_moment),Abs(m_moment)*m_attr->_f1GroundInclFrictionLog*(1-e));
				m_moment *= e;
				break;
			}
			if( bTouchGround ) m_moment += Normal(axis)*atan2(Abs(axis),n0*n1)*m_attr->_fGroundInclOmega2*t1;
			double e = exp(-m_attr->_fGroundInclFrictionLog*t1);
			double a = Abs(m_moment)*m_attr->_f1GroundInclFrictionLog*(1-e);
			m_dir.RotateR(Normal(m_moment),a);
			m_moment *= e;
			t -= t1;
		}
		#if 0
		if( m_base.m[1][1] < m_attr->_fMinInclineCos ) { // inclined too much
			n0 = m_base.Row(1);
			n1 = CFVector3(0,1,0);
			axis = n1%n0;
			m_base.RotateR(Normal(axis),Max(0.,atan2(Abs(axis),n1*n0)-m_attr->_fMaxInclineAngle));
		}
		#endif
	}
}

void	CVesselEmv::ApplyStep()
{
	//m_fShield = m_fMaxShield;
	m_position += m_bump.vel*m_bump.fTime;
	if( m_bump.nBumpFlags ) {
		if( m_bump.nBumpFlags == BF_BUMPSHOOT )
			//CMusic::SetDIGI(CMusic::DIGI_FOELASERHITDYNAMIC),
			m_bPrevBump = FALSE;
		else {
			if(m_bump.nBumpFlags!=BF_BUMPDYNAMIC) {
				m_speed = CFVector3(0,0,0);
				m_fallSpeed = m_speed;
				m_bPrevBump = FALSE;
			} else {
				m_speed = m_bPrevBump ? CFVector3(0,0,0) : m_bump.vel1;
				//if( CMusic::IsActiveFX() )
				//	CMusic::SetDIGI(m_bump.nBumpFlags==BF_BUMPSTATIC?
				//		CMusic::DIGI_MYBUMPSTATIC:CMusic::DIGI_MYBUMPDYNAMIC,
				//		CViewObject::m_viewPointDirMx*m_position);
				m_fallSpeed = m_speed;
				m_bPrevBump = !m_bPrevBump;	//TRUE;
			}
		}
	} else m_bPrevBump = FALSE;

	//ClipPos(m_position);
	//m_position.y = Min(m_position.y,m_attr->fMaxPossibleHeight);
	#ifndef __VESSEL_IGNORE_MASTER
		m_master->setPosition(m_position);
	#endif
	m_fStepTime = 0;


	//__sp = m_speed;
	//__spfall = m_fallSpeed;
}

void	CVesselEmv::SetAsViewPoint(bool bCentered,double fFocus,double fAspRatio,
															int nDir)
{
	CFMatrix3x4	dir;
	(dir = m_dir).TranslateR(-m_position);
	switch(nDir) {
		case VD_BACK:	dir.RotateOyL(M_PI); break;
		case VD_LEFT:	dir.RotateOyL(-M_PI/2); break;
		case VD_RIGHT:	dir.RotateOyL(M_PI/2); break;
	}
	if( bCentered )	dir.m[0][3] = dir.m[1][3] = dir.m[2][3] = 0;
	CViewObject::SetViewPoint(dir,CFVector3(fFocus,-fFocus*fAspRatio,-1));
}

void	CVesselEmv::Throttle(double fDelta)
{
/*	m_fFuelSpeed = fDelta >= 0 ? Min(m_fFuelSpeed+0.1,1.) :
													Max(m_fFuelSpeed-0.1,0.);
	*/
	m_fThrottle = MinMax(fDelta,-1.,1.);
}

void	CVesselEmv::Raise(double fDeltaUp)
{
//	m_dir.RotateOxL(-0.05*fDeltaUp);
	m_fVertRotReq = MinMax(fDeltaUp,-1.,1.);
}

void	CVesselEmv::Incline(double fDeltaRight)
{
//	m_dir.RotateOzL(fDeltaRight);
	m_fInclineReq = MinMax(fDeltaRight,-1.,1.);
}

void	CVesselEmv::Rotate(double fDeltaRight)
{
//	m_dir.RotateOzL(fDeltaRight);
	m_fAxisRotReq = MinMax(fDeltaRight,-1.,1.);
}

void	CVesselEmv::SetStrafe(bool bStrafe)
{
	m_bStrafe = MAKEBOOL(bStrafe);
}

double	CVesselEmv::ForwardSpeed() const
{
	CFVector3	normalZ(m_dir.m[2][0],m_dir.m[2][1],m_dir.m[2][2]);
	return	-(m_speed*normalZ);
}

void	CVesselEmv::LoadPos(TCCFMatrix3x4 &startPos)
{
	m_dir = startPos;
	CFMatrix3x4	t;
	t.LoadTransposed(m_dir);
	m_position = CFVector3(t.m[0][3],t.m[1][3],t.m[2][3]);
	m_dir.m[0][3] = m_dir.m[1][3] = m_dir.m[2][3] = 0;
}

struct SEmvSave {
	dword			dwSize;
	SFMatrix3x4		dir;
	SFVector3		pos;
	SFVector3	speed;
	double	fFuelSpeed;
	double	fThrottle;
	double	fVertRot;
	double	fAxisRot;
	double	fInclination;
	double	fInclineRot;
	double	fTurnRot;
	double	fStrafeInclination;
	double	fStrafeInclineRot;
	double	fVertStrafe;
	double	fTurnRotMouse;
	double	fVertRotMouse;
};

TCvoid *CVesselEmv::SaveGame()
{
	static SEmvSave	s;
	s.dwSize = sizeof(s);
	s.dir 		   		= m_dir;
	s.pos 		   		= m_position;
	s.speed 	   		= m_speed;
	s.fFuelSpeed   		= m_fFuelSpeed;
	s.fThrottle    		= m_fThrottle;
	s.fVertRot 	   		= m_fVertRot;
	s.fAxisRot 	   		= m_fAxisRot;
	s.fInclination 		= m_fInclination;
	s.fInclineRot 		= m_fInclineRot;
	s.fTurnRot 			= m_fTurnRot;
	s.fStrafeInclination= m_fStrafeInclination;
	s.fStrafeInclineRot = m_fStrafeInclineRot;
	s.fVertStrafe 		= m_fVertStrafe;
	s.fTurnRotMouse 	= m_fTurnRotMouse;
	s.fVertRotMouse 	= m_fVertRotMouse;
	//CFVector3	m_fallSpeed;

	return &s;
}

bool CVesselEmv::LoadGame(TCvoid *pSavedGame)
{
	SEmvSave	&s = *(SEmvSave*)pSavedGame;
	if( s.dwSize != sizeof(s) ) {
		ASSERT(0);
		return FALSE;
	}
	Stop();
	m_dir = s.dir;
	m_position = s.pos;

	m_speed               = s.speed;
	m_fFuelSpeed          = s.fFuelSpeed;
	m_fThrottle           = s.fThrottle;
	m_fVertRot            = s.fVertRot;
	m_fAxisRot            = s.fAxisRot;
	m_fInclination        = s.fInclination;
	m_fInclineRot         = s.fInclineRot;
	m_fTurnRot            = s.fTurnRot;
	m_fStrafeInclination  = s.fStrafeInclination;
	m_fStrafeInclineRot   = s.fStrafeInclineRot;
	m_fVertStrafe         = s.fVertStrafe;
	m_fTurnRotMouse       = s.fTurnRotMouse;
	m_fVertRotMouse       = s.fVertRotMouse;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

CConfigFile	&ZAV_Config();

void	CVesselWheels::Init()
{
	m_speed = CFVector3(0,0,0);
	m_position = CFVector3(0,0,0);
	m_moment = CFVector3(0,0,0);
	m_head = CFVector3(0,2.5,0);
	m_dir.LoadIdentity();
	m_base.LoadIdentity();
	m_fRaiseSpeed = 0;
	m_fTurnReq = m_fRaiseReq = m_fRaise = 0;
	m_fFuelAccel = m_fThrottle = 0;
	m_bForceage = m_bStrafe = m_bPrevBump = FALSE;
	m_pBase = NULL; m_pObj = NULL;
	//m_fBumpRadius = 2;
	m_fTurnSpeed = 0;
	//m_fYSpeed = 0;
	m_fRessoreWay = 0;
	m_fAccelFactor = 1;
	m_groundNormal = CFVector3(0,1,0);
	m_nForward = 0;
	m_eBraking = BRAKING_OFF;
	m_lastAccel = CFVector3(0,0,0);
	m_bTouchGround = FALSE;
	m_fWalkPendulumArg = 0;
	m_fJump = 0;
	m_nJump = 0;
	m_fJumpT = 0;
	m_jumpOffset = CFVector3(0,0,0);
	m_fJumpAccum = 0;
	m_fLastFallV = 0;
	//m_fMaxHorzSpeed = 0;
	m_maxHorzSpeed = CFVector3(0,0,0);
	m_walkPendulumWay = CFVector3(0,0,0);
	m_fWalkPendulumCoeff = 0;
	m_fWalkPendSpeedCoeff = 0;
	m_fStrafe = 0;
	m_fJumpOffsetFactor = 1;
	m_fJumpTimeOut = 0;
    m_rawmouse = m_joy = m_mouse = CFVector2(0,0);
	m_fMouseTurnSpeed = m_fMouseRaiseSpeed = 0;
	m_fMouseRaise = 0;
	m_fTower = 0;
	m_fTowerSpeed = 0;
    m_fStrafeVert = 0;
}


void	CVesselWheels::AccumAccelThrottle(double fDeltaT)
{
	if( m_eBraking != BRAKING_DONE && fabs(m_fThrottle) >= 0.01 ) {
		if( !m_nForward ) m_nForward = m_fThrottle > 0 ? 1 : -1;
		double fAccel =
			(m_nForward?(m_nForward>=0):(m_fThrottle>=0))?
					m_attr->_fMaxAccel:m_attr->_fMaxBackAccel;

		double f1Time = m_fThrottle*m_nForward>=0.01 ?
			m_attr->_f1MaxSpeedTime : f1Time = m_attr->_f1StopTime;

		m_fFuelAccel += m_fThrottle*fAccel*fDeltaT*f1Time;
		m_fFuelAccel = MinMax(m_fFuelAccel,-m_attr->_fMaxBackAccel,m_attr->_fMaxAccel);
		if( m_nForward > 0 ) {
			if( m_fThrottle < 0 && m_fFuelAccel < 0 )	{
				m_eBraking = BRAKING_ON; m_fFuelAccel = 0;
			}
		}
		if( m_nForward < 0 ) {
			if( m_fThrottle > 0 && m_fFuelAccel > 0 )	{
				m_eBraking = BRAKING_ON; m_fFuelAccel = 0;
			}
		}
	}
	else
	if( m_eBraking == BRAKING_DONE && fabs(m_fThrottle) < 0.01 ) {
		m_eBraking = BRAKING_OFF;
		m_nForward = 0;
	}
	CFVector3 &groundNormal = m_groundNormal;
	CFVector3	slide = (CFVector3(0,1,0)%groundNormal)%groundNormal,
				slideN = Normal(slide);
	double fSlideCos = Normal(m_base.Row(2)-(m_base.Row(2)*groundNormal)*groundNormal)*slideN;
	m_fAccelFactor = Max(1+(fSlideCos*(m_fFuelAccel>=0?1:-1))*((groundNormal.y-1)/(1-m_attr->_fCosMaxClimbAngle)),0.);

	double	fForceage = 1+MAKEBOOL(m_bForceage)*(m_attr->fForceageFactor-1);
	// s' = -f*s+a
	double	fFrictionLog = m_attr->_fFrictionLog;
	if( m_eBraking == BRAKING_ON )
		fFrictionLog = m_attr->_fBrakingFrictionLog;
	if( m_bTouchGround ) {
		CFVector3	row2 = m_base.Row(2);
		row2.y = 0;
		CFVector3	speedNorm = Normal(m_groundNormal%CFVector3(-m_base.m[2][2],0,m_base.m[2][0]));
		CFVector3	a_f = (-m_fFuelAccel*fForceage*m_fAccelFactor/fFrictionLog)*speedNorm;
		double	e = exp(-fFrictionLog*fDeltaT);
		//CFVector3	a_f = (-m_fFuelAccel*fForceage*m_fAccelFactor*m_attr->_f1FrictionLog)*m_base.Row(2);
		//double	e = exp(-m_attr->_fFrictionLog*fDeltaT);
		CFVector3	s0 = (m_speed*speedNorm)*speedNorm;
		CFVector3	s0_a_f = s0-a_f;
		//m_offset += a_f*fDeltaT+s0_a_f*(m_attr->_f1FrictionLog*(1-e));
		m_offset += a_f*fDeltaT+s0_a_f*((1-e)/fFrictionLog);

		double eSide = exp(-m_attr->_fSideFrictionLog*fDeltaT);
		CFVector3	s0Side = m_speed-s0;
		m_offset += s0Side*(m_attr->_f1SideFrictionLog*(1-eSide));

		CFVector3	newspeed = a_f+e*s0_a_f + eSide*s0Side;
		m_lastAccel = (newspeed-m_speed)/Max(fDeltaT,1e-5);
		m_speed = newspeed;
		s0 = (m_speed*speedNorm)*speedNorm;
		if( m_eBraking == BRAKING_ON && Abs(s0)<m_attr->fFullStopSpeed ) {
			m_speed -= s0;
			m_eBraking = BRAKING_DONE;
		}
		//m_moment += accel%CFVector3(0,m_attr->_fAccelMoment,0);
	}
	else { // don't touch ground
		m_offset += m_speed*fDeltaT;
		m_lastAccel = CFVector3(0,0,0);
	}

	if( m_attr->bRotatableTower ) {
		double	fReq = AbsNoGreater(m_fStrafe + (m_bStrafe ? m_fTurnReq : 0),1.);
		double	fTowerAccel;
		if( fabs(fReq)>=0.01 ) {
			if( m_fTowerSpeed*(fReq>0?1:-1) >= 0 )
				fTowerAccel = m_attr->_fTowerAccel;
			else
				fTowerAccel = m_attr->_fTowerStopAccel;
		}
		else
			fTowerAccel = m_attr->_fTowerStopAccel;
		m_fTowerSpeed += AbsNoGreater(fReq*m_attr->_fMaxTowerSpeed-m_fTowerSpeed,
				fTowerAccel*fDeltaT);
		m_fTower +=  AbsNoGreater(m_fTowerSpeed,m_attr->_fMaxTowerSpeed)*fDeltaT;
	}
}

void	CVesselWheels::AccumAccelWalk(double fDeltaT)
{
	(void)fDeltaT;

	BEGIN_LOG_VESSEL()

	double	fForceage = 1+MAKEBOOL(m_bForceage)*(m_attr->fForceageFactor-1);

	if( m_attr->bJump ) {
		if( m_fJumpTimeOut > 0 ) {
			m_fJumpTimeOut -= fDeltaT;
			if( m_fJumpOffsetFactor < 1 )
				m_fJumpOffsetFactor = 1-Min(m_fJumpTimeOut,1.)/m_attr->fJumpTimeOut;
		}
		else {
			m_fJumpOffsetFactor = 1;
			if( !m_nJump )	m_jumpOffset = CFVector3(0,0,0);
		}
	}

    bool bSwimming = m_attr->bSwimming && m_position.y < ZAV_Scene()->GetTerrain()->Waterline();
    if( bSwimming ) {
        double  fVertStrafe = MinMax((m_bStrafe?m_fRaiseReq:0)+m_fStrafe+m_fJump,-1.,1.),
                fHorzStrafe = MinMax((m_bStrafe?m_fTurnReq:0)+m_fStrafe,-1.,1.);
        CFVector3   vReq =
            (-m_attr->fSwimmingSpeed*m_fThrottle)*m_dir.Row(2)+
            (m_attr->fSwimmingStrafeSpeed*fVertStrafe)*m_dir.Row(1)+
            (m_attr->fSwimmingStrafeSpeed*fHorzStrafe)*m_dir.Row(0);
        double  R = m_attr->_fSwimmingStopLog;
        vReq *= fForceage;
        DSolve2NHO0(m_offset,m_speed,R,vReq*R,fDeltaT);
        goto Exit;
    }

	if( m_bTouchGround ) {

		if( m_attr->bJump ) {
			if( m_fJumpTimeOut <= 0 ) {
				m_fJumpOffsetFactor = 1;
				if( !m_nJump )	m_jumpOffset = CFVector3(0,0,0);
				if( !m_nJump && m_fJump > 0.01 ) {
					/*CFVector3	jumpSpeedH = (-m_attr->_fMaxJumpSpeedH)*m_base.Row(2),
								jumpSpeedV = m_attr->_fMaxJumpSpeedV*m_base.Row(1);
					m_offset += (jumpSpeedH+jumpSpeedV)*fDeltaT;
					newspeed = jumpSpeedH+jumpSpeedV;
					// m_fYSpeed = m_attr->_fMaxJumpSpeedV;
					m_bTouchGround = FALSE;*/
					m_fJumpT = m_fJumpAccum = 0;
					m_nJump = -1;
					m_jumpOffset = CFVector3(0,0,0);
					m_jumpSpeed = CFVector3(0,m_attr->_fMaxJumpSpeedV,-m_attr->_fMaxJumpSpeedH);
					double fJumpFactor = (m_fRaise/m_attr->_fMaxRaise+1)*0.5;	// [0..1]
					m_jumpSpeed.z *= fJumpFactor;
					m_fJumpDownV = Max(2.5,m_fLastFallV);
				}
				if( m_nJump < 0 && m_fJump >= 0.01 ) {
					m_fJumpAccum += fDeltaT;
				}
			}
		}


		CFVector3 &groundNormal = m_groundNormal;
		CFVector3	slide = (CFVector3(0,1,0)%groundNormal)%groundNormal,
					slideN = Normal(slide);	// direction opposed to sliding force (-slideDir)
		double fSlideCos = Normal(m_base.Row(2)-(m_base.Row(2)*groundNormal)*groundNormal)*slideN;
				// slideCos = 1 if going up (against slide)
		m_fAccelFactor = 1; //Max(1+(fSlideCos*(m_fThrottle>=0?1:-1))*((groundNormal.y-1)/(1-m_attr->_fCosMaxClimbAngle)),0.);
		//m_fAccelFactor = 1+fSlideCos

		//CFVector3	speedNorm = Normal(m_groundNormal%CFVector3(m_base.m[2][2],0,-m_base.m[2][0]));
		//double		s0 = m_speed*speedNorm;
		CFVector3	speedNorm = Normal(m_groundNormal%CFVector3(m_base.m[2][2],0,-m_base.m[2][0]));
		double		s0 = m_speed*speedNorm;
		//double	fForceage = 1+MAKEBOOL(m_bForceage)*(m_attr->fForceageFactor-1);

		double	fStrafeReq = MinMax((m_bStrafe?m_fTurnReq:0)+m_fStrafe,-1.,1.);
		double	fThrottle = m_fThrottle;
		double	fHorzSpeedNorm = Max(1e-5,hypot(fStrafeReq,fThrottle));
		fStrafeReq /= fHorzSpeedNorm;
		fThrottle /= fHorzSpeedNorm;
		//ASSERT(fabs(fStrafeReq)<=1&&fabs(fThrottle)<=1);
		double	starg = 0; int nStop = 0;
		double	fAccel = 0;
		if( fabs(s0)>0.01 && s0*m_nForward < 0 ) m_nForward = s0 >= 0 ? 1 : -1;
		if( fabs(s0)<=0.01) m_nForward = 0;
		if( fabs(m_fThrottle) >= 0.01 ) {
			if( m_fThrottle > 0 ) {
				if( m_nForward < 0 ) { nStop = 1; starg = 0; }
				else { m_nForward = 1; starg = m_attr->fMaxSpeed*fForceage*m_fAccelFactor*fThrottle; }
			}
			else {
				if( m_nForward > 0 )	{ nStop = -1; starg = 0; }
				else { m_nForward = -1; starg = m_attr->fMaxBackSpeed*fForceage*m_fAccelFactor*fThrottle; }
			}
		}
		else
			nStop = -m_nForward;

		int nForward = m_nForward;
		if( !nForward )	nForward = s0 >= 0 ? 1 : -1;
		if( nForward > 0 ) {
			if( s0 > starg ) {
				fAccel = -m_attr->_fStopAccel;
			}
			else {
				fAccel = m_attr->_fMaxAccel;
			}
			if( fabs(s0) > m_attr->fMaxSpeed ) fAccel *= fabs(s0)/m_attr->fMaxSpeed;
		} else
		if( nForward < 0 ) {
			if( s0 > starg ) {
				fAccel = -m_attr->_fMaxBackAccel;
			}
			else {
				fAccel = m_attr->_fBackStopAccel;
			}
			if( fabs(s0) > m_attr->fMaxBackSpeed ) fAccel *= fabs(s0)/m_attr->fMaxBackSpeed;
		}
		double fMin = Min(s0,starg), fMax = Max(s0,starg);
		m_fAccelFactor = Max(1+(fSlideCos*(fAccel>=0?1:-1))*((groundNormal.y-1)/(1-m_attr->_fCosMaxClimbAngle)),0.);
		fAccel *= 1; //m_fAccelFactor;
		//m_fAccelFactor = Max(1+(fSlideCos*(m_fThrottle>=0?1:-1))*((groundNormal.y-1)/(1-m_attr->_fCosMaxClimbAngle)),0.);



		double s01 = s0+fAccel*fDeltaT;
		if( s01 < fMin ) { s01 = fMin; if( nStop < 0 ) m_nForward = 0; }
		if( s01 > fMax ) { s01 = fMax; if( nStop > 0 ) m_nForward = 0; }
		if( m_nJump )	s01 = s0;
		//WRITELOG2("s0=%lg,%lg,%lg  ->  %lg,%lg,%lg\n",speedNorm*s0,speedNorm*s01);

		//m_offset += speedNorm*((s0 + s01)*fDeltaT*0.5);

		// Strafe

		CFVector3	s0YX = (m_speed-s0*speedNorm);
		CFVector3	strafeNorm = Normal(m_groundNormal%CFVector3(-m_base.m[0][2],0,m_base.m[0][0]));
		double		fStrafeSpeed = s0YX*strafeNorm, fStrafeSpeed0 = fStrafeSpeed;
		if( m_attr->bAllowStrafe ) {
			//double		fStrafeAccel = m_attr->_fStrafeAccel;
			double		fStrafeSpeedTarg = fStrafeReq*fForceage*m_attr->fMaxStrafeSpeed;
			if( fStrafeSpeed < fStrafeSpeedTarg ) {
				double		fStrafeSlideCos = strafeNorm*slideN;
				double fStrafeAccelFactor = 1; //Max(1-fStrafeSlideCos*((groundNormal.y-1)/(1-m_attr->_fCosMaxClimbAngle)),0.);
				fStrafeSpeed = Min(fStrafeSpeedTarg,fStrafeSpeed+m_attr->_fStrafeAccel*fStrafeAccelFactor*fForceage*fDeltaT);
			}
			else {
				double		fStrafeSlideCos = strafeNorm*slideN;
				double fStrafeAccelFactor = 1; //Max(1+fStrafeSlideCos*((groundNormal.y-1)/(1-m_attr->_fCosMaxClimbAngle)),0.);
				fStrafeSpeed = Max(fStrafeSpeedTarg,fStrafeSpeed-m_attr->_fStrafeAccel*fStrafeAccelFactor*fForceage*fDeltaT);
			}
			//m_offset += strafeNorm*((fStrafeSpeed+fStrafeSpeed0)*(0.5*fDeltaT));
		}

		// Rest

		CFVector3	s0Rest = s0YX;
		if( m_attr->bAllowStrafe )	s0Rest -= fStrafeSpeed0*strafeNorm;
		CFVector3	s0GroundNorm = (s0Rest*groundNormal)*groundNormal;
		//m_offset += s0GroundNorm*fDeltaT;


		CFVector3	s0Side = s0Rest-s0GroundNorm;
		double		fSide = Abs(s0Side);
		double		fSideSlideCos = Normal(s0Side)*slideN;
					// slidecos = 1 if going against slide
		double fSideAccelFactor = 1; //Max(1+fSideSlideCos*((groundNormal.y-1)/(1-m_attr->_fCosMaxClimbAngle)),0.);
		fSide = Max(0.,fSide-m_attr->_fSideStopAccel*fDeltaT*fSideAccelFactor /**groundNormal.y*/);
		CFVector3	s0Side1 = Normal(s0Side)*fSide;
		//m_offset += (s0Side+s0Side1)*(0.5*fDeltaT);

		CFVector3	newspeed = speedNorm*s01+s0Side1+s0GroundNorm;
		if( m_attr->bAllowStrafe )	newspeed += strafeNorm*fStrafeSpeed;

		// inclination speed correction
		{
			//WRITELOG1("newspeed=%lg,%lg,%lg\n",newspeed);
			CFVector3	deltaSpeed = newspeed-m_speed;
			double		fSlideDelta = deltaSpeed*slideN;
			CFVector3	slideDelta = fSlideDelta*slideN;
			double		fSlideCos = fSlideDelta >= 0 ? 1 : -1; // Normal(deltaSpeed)*slideN; // > 0 if down the slope
			double fAccelFactor = Max(1-fSlideCos*((groundNormal.y-1)/(1-m_attr->_fCosMaxClimbAngle)),0.);
			/*if( fAccelFactor < 0.5 ) {
				WRITELOG2("Abs(slideN)=%lg fSlideDelta
				WRITELOG3("slideDelta=%lg,%lg,%lg slideN=%lg,%lg,%lg fAccelFactor=%lg\n",slideDelta,slideN,fAccelFactor);
			}*/
			//if( Abs(deltaSpeed) > 0.1 )
			//	printf("<<<<  %10lg %10lg       %lg,%lg,%lg\n",fSlideCos,fAccelFactor,slideN);
			deltaSpeed = deltaSpeed + slideDelta*(fAccelFactor-1);
			newspeed = m_speed+deltaSpeed;
			m_offset += (m_speed+newspeed)*(fDeltaT*0.5);
			//printf("newspeed=%lg,%lg,%lg\toffset=%lg,%lg,%lg\n",newspeed,m_offset);
			//WRITELOG4("m_speed=%lg,%lg,%lg newspeed=%lg,%lg,%lg offset=%lg,%lg,%lg  fAccelFactor=%lg\n",m_speed,newspeed,m_offset,fAccelFactor);
		}

		m_lastAccel = (newspeed-m_speed)/Max(fDeltaT,1e-5);
		m_speed = newspeed;
	} // touch ground
	else { // not touch ground

		if( m_attr->bFlyControl ) {

			//double	fHorzSpeedAdd = Abs(CFVector2(m_speed.x,m_speed.z))-m_fMaxHorzSpeed;
			double	fStrafeReq = MinMax((m_bStrafe?m_fTurnReq:0)+m_fStrafe,-1.,1.);
			double	fThrottle = m_fThrottle;
			double	fHorzSpeedNorm = Max(1e-5,hypot(fStrafeReq,fThrottle));
			fStrafeReq /= fHorzSpeedNorm;
			fThrottle /= fHorzSpeedNorm;
			CFVector3	flyHorzDir = Normal(CFVector3(-m_base.m[2][0],0,-m_base.m[2][2])),
						flyStrafeDir(-flyHorzDir.z,0,flyHorzDir.x);
						//flyStrafeDir = Normal(CFVector3(m_base.m[0][0],0,m_base.m[0][2]));
			double	fMaxHorzF = m_maxHorzSpeed*flyHorzDir,
					fMaxHorzS = m_maxHorzSpeed*flyStrafeDir;
			ASSERT( Abs((fMaxHorzF*flyHorzDir+fMaxHorzS*flyStrafeDir)-m_maxHorzSpeed)<1e-3);
			{
				double	fMaxF = Max(1e-3,(fMaxHorzF >= 0 ? m_attr->fMaxSpeed : m_attr->fMaxBackSpeed)*m_attr->fForceageFactor),
						fMaxS = Max(1e-3,m_attr->fMaxStrafeSpeed*m_attr->fForceageFactor);

				double	h = hypot(fMaxHorzF/fMaxF,fMaxHorzS/fMaxS);
				if( h > 1 ) {
					fMaxHorzF /= h;
					fMaxHorzS /= h;
				}
			}
			m_maxHorzSpeed = fMaxHorzF*flyHorzDir+fMaxHorzS*flyStrafeDir;
			/*m_fMaxHorzSpeed = MinMax(m_fMaxHorzSpeed,-m_attr->fMaxBackSpeed*m_attr->fForceageFactor,
														m_attr->fMaxSpeed*m_attr->fForceageFactor);*/
			CFVector3	vmax = m_attr->fMaxFlyAddSpeed*flyHorzDir,
						vstrafemax = m_attr->fMaxFlyAddStrafeSpeed*flyStrafeDir,
						vbase = m_maxHorzSpeed, //*flyHorzDir,
						horzspeed = CFVector3(m_speed.x,0,m_speed.z),
						v = horzspeed-vbase,
						a_f = vmax*(fThrottle*fForceage)+vstrafemax*(fStrafeReq*fForceage);

			double		e = exp(-m_attr->_fFlyFrictionLog*fDeltaT);
			m_offset += (vbase+a_f)*fDeltaT+(v-a_f)*((1-e)/m_attr->_fFlyFrictionLog);
			m_offset.y += m_speed.y * fDeltaT;
			horzspeed = vbase+a_f+(v-a_f)*e;
			m_speed.x = horzspeed.x; m_speed.z = horzspeed.z;

			//m_fMaxHorzSpeed = AbsNoGreater(m_fMaxHorzSpeed,fabs(m_speed*flyHorzDir));
			fMaxHorzF = AbsNoGreater(fMaxHorzF,fabs(m_speed*flyHorzDir));
			fMaxHorzS = AbsNoGreater(fMaxHorzS,fabs(m_speed*flyStrafeDir));
			m_maxHorzSpeed = fMaxHorzF*flyHorzDir+fMaxHorzS*flyStrafeDir;
			m_lastAccel = CFVector3(0,0,0);

			__sp = m_speed;
		}
		else {
			m_offset += m_speed*fDeltaT;
			m_lastAccel = CFVector3(0,0,0);
		}
	}


Exit:
	if( m_nJump )	m_fJumpT += fDeltaT;

	END_LOG_VESSEL()
}

void	CVesselWheels::AccumPreStep(double fDeltaT)
{
	CHECK_DELTAT();

	double	fTurnAccel, fTurnReq = MinMax( (m_bStrafe ? 0 : m_fTurnReq)+m_joy.x, -1., 1.);
	if( fabs(fTurnReq)>=0.01 ) {
		if( m_fTurnSpeed*(fTurnReq>0?1:-1) >= 0 )
			fTurnAccel = m_attr->_fTurnAccel;
		else
			fTurnAccel = m_attr->_fTurnStopAccel;
	}
	else
		fTurnAccel = m_attr->_fTurnStopAccel;
	m_fTurnSpeed += AbsNoGreater(fTurnReq*m_attr->_fMaxTurnSpeed-m_fTurnSpeed,
			fTurnAccel*fDeltaT);

	double	fMouseTurnSpeed = 0;
	{ // mouse turn

        double fMouseCoeff = m_attr->_fMaxTurnSpeed;
        double fTargetTurn = m_mouse.x*fMouseCoeff;
        fMouseTurnSpeed = AbsNoGreater(fTargetTurn/Max(1e-5,fDeltaT),m_attr->_fMouseTurnSpeed);
        double dx = (fMouseTurnSpeed*fDeltaT)/fMouseCoeff;
        m_mouse.x -= dx;
        m_rawmouse.x -= dx;

        #if 0
		fTurnReq = m_mouse.x;
		double	fTargetTurnSpeed;
		double	fMaxTurn, fMinTurn;

		double fMouseCoeff = m_attr->_fMaxTurnSpeed;
		double fTargetTurn = fTurnReq*fMouseCoeff;
		//fTargetTurnSpeed = 0 < fTargetTurn ? m_attr->_fRaiseSpeed : -m_attr->_fRaiseSpeed;
		if( 0 < fTargetTurn )
			fMaxTurn = fTargetTurn, fMinTurn = 0, fTargetTurnSpeed = m_attr->_fMaxTurnSpeed;
		else
			fMaxTurn = 0, fMinTurn = fTargetTurn, fTargetTurnSpeed = -m_attr->_fMaxTurnSpeed;

		double	fMaxTurnAccel = m_attr->_fTurnAccel;
		double	fMaxTurnStopAccel = m_attr->_fTurnStopAccel;
		fTurnAccel = m_fMouseTurnSpeed < fTargetTurnSpeed ? fMaxTurnAccel : -fMaxTurnAccel;
		double fStopAccel;
		if( m_fMouseTurnSpeed >= 0 ) {
			fStopAccel = -m_fMouseTurnSpeed*m_fMouseTurnSpeed*0.5/Max(1e-5,fMaxTurn);
			if( fStopAccel > -fMaxTurnStopAccel )	fStopAccel = 1e10;
			if( fTurnAccel >= fStopAccel ) {
				fTurnAccel = fStopAccel;
				fTargetTurnSpeed = 0;
			}
		}
		else {
			fStopAccel = -m_fMouseTurnSpeed*m_fMouseTurnSpeed*0.5/Min(-1e-5,fMinTurn);
			if( fStopAccel < fMaxTurnStopAccel )	fStopAccel = -1e10;
			if( fTurnAccel <= fStopAccel ) {
				fTurnAccel = fStopAccel;
				fTargetTurnSpeed = 0;
			}
		}

		double	fEndTurnSpeed = m_fMouseTurnSpeed+fTurnAccel*fDeltaT;
		if( fTurnAccel >= 0 )
			fEndTurnSpeed = Min(fTargetTurnSpeed,fEndTurnSpeed);
		else
			fEndTurnSpeed = Max(fTargetTurnSpeed,fEndTurnSpeed);

		double	fMouseTurn = (m_fMouseTurnSpeed+fEndTurnSpeed)*0.5*fDeltaT;
		m_fMouseTurnSpeed = fEndTurnSpeed;
		fMouseTurn = MinMax(fMouseTurn,fMinTurn,fMaxTurn);
		m_mouse.x -= fMouseTurn/fMouseCoeff;
		m_mouse.x *= exp(-m_attr->_fMouseHorzDecayLog*fDeltaT);
		m_fMouseTurnSpeed = MinMax(m_fMouseTurnSpeed,-m_attr->_fMaxTurnSpeed,m_attr->_fMaxTurnSpeed);
		fMouseTurnSpeed = fMouseTurn/Max(1e-5,fDeltaT);
        #endif
	}

    //!!! modif
    m_base.RotateOyL(AbsNoGreater(m_fTurnSpeed+fMouseTurnSpeed,Max(m_attr->_fMaxTurnSpeed,m_attr->_fMouseTurnSpeed))*fDeltaT);

	//double fRotY = m_fTurnReq*fDeltaT*m_attr->_fMaxTurnSpeed;
	//m_base.RotateOyL(fRotY);

	//m_moment += CFVector3(0,m_fTurnReq*fDeltaT*m_attr->_fTurnAccel,0);


	/*{
		double	fRaiseSign = m_attr->bInverseY?-1:1;
		//double	fMaxRaise = M_PI/4;
		if( m_attr->bRestoreRaise ) {
			double	fTargetRaise = MinMax(m_fRaiseReq*fRaiseSign*m_attr->_fMaxRaise,-m_attr->_fMaxRaise,m_attr->_fMaxRaise);
			if( m_fRaise < fTargetRaise )
				m_fRaise = Min(fTargetRaise,m_fRaise+m_attr->_fRaiseSpeed*fDeltaT);
			else
				m_fRaise = Max(fTargetRaise,m_fRaise-m_attr->_fRaiseSpeed*fDeltaT);
		}
		else {
			m_fRaise = MinMax(m_fRaise+m_fRaiseReq*fRaiseSign*fDeltaT*m_attr->_fRaiseSpeed,-m_attr->_fMaxRaise,m_attr->_fMaxRaise);
		}

	}*/

	{
        double  fRaiseReq = MinMax((m_bStrafe?0:m_fRaiseReq)+(m_attr->bJoyRestoreRaise?0.:m_joy.y),-1.,1.);
		double	fTargetSpeed;
		double	fMaxRaise, fMinRaise;
		if( m_attr->bRestoreRaise ) {
			double fTargetRaise = fRaiseReq*m_attr->_fMaxRaise;
			fTargetSpeed = m_fRaise < fRaiseReq*m_attr->_fMaxRaise ? m_attr->_fRaiseSpeed : -m_attr->_fRaiseSpeed;
			if( m_fRaise < fTargetRaise )
				fMaxRaise = fTargetRaise, fMinRaise = -m_attr->_fMaxRaise, fTargetSpeed = m_attr->_fRaiseSpeed;
			else
				fMaxRaise = m_attr->_fMaxRaise, fMinRaise = fTargetRaise, fTargetSpeed = -m_attr->_fRaiseSpeed;
		}
		else {
			fTargetSpeed = fRaiseReq*m_attr->_fRaiseSpeed;
			fMinRaise = -m_attr->_fMaxRaise;
			fMaxRaise = m_attr->_fMaxRaise;
			//fTargetRaise = fTargetSpeed >= 0 ? m_attr->_fMaxRaise : -m_attr->_fMaxRaise;
		}
		double	fMaxRaiseAccel = m_attr->_fRaiseSpeed/m_attr->fRaiseAccelTime;
		double	fMaxRaiseStopAccel = m_attr->_fRaiseSpeed/m_attr->fRaiseStopTime;
		if( !m_attr->bRestoreRaise && fabs(fTargetSpeed) < m_attr->_fRaiseSpeed )
			fMaxRaiseAccel = fMaxRaiseStopAccel;
		double fRaiseAccel = m_fRaiseSpeed < fTargetSpeed ? fMaxRaiseAccel : -fMaxRaiseAccel;
		double fStopAccel;
		if( m_fRaiseSpeed >= 0 ) {
			fStopAccel = -m_fRaiseSpeed*m_fRaiseSpeed*0.5/Max(1e-5,fMaxRaise-m_fRaise);
			if( fStopAccel > -fMaxRaiseStopAccel )	fStopAccel = 1e10;
			if( fRaiseAccel >= fStopAccel ) {
				fRaiseAccel = fStopAccel;
				fTargetSpeed = 0;
			}
		}
		else {
			fStopAccel = -m_fRaiseSpeed*m_fRaiseSpeed*0.5/Min(-1e-5,fMinRaise-m_fRaise);
			if( fStopAccel < fMaxRaiseStopAccel )	fStopAccel = -1e10;
			if( fRaiseAccel <= fStopAccel ) {
				fRaiseAccel = fStopAccel;
				fTargetSpeed = 0;
			}
		}

		double	fEndSpeed = m_fRaiseSpeed+fRaiseAccel*fDeltaT;
		if( fRaiseAccel >= 0 )
			fEndSpeed = Min(fTargetSpeed,fEndSpeed);
		else
			fEndSpeed = Max(fTargetSpeed,fEndSpeed);
		m_fRaise += (m_fRaiseSpeed+fEndSpeed)*0.5*fDeltaT;
		m_fRaiseSpeed = fEndSpeed;
		m_fRaise = MinMax(m_fRaise,fMinRaise,fMaxRaise);
		m_fRaiseSpeed = MinMax(m_fRaiseSpeed,-m_attr->_fRaiseSpeed,m_attr->_fRaiseSpeed);
	}


	{	// mouse raise
        #if 1
        m_rawmouse.y = MinMax(m_rawmouse.y,-1.,1.);
		m_mouse.y = MinMax(m_mouse.y,-1.,1.);
		double	fRaiseCoeff = m_attr->_fMaxRaise;
        double fTargetRaise = (m_mouse.y+(m_attr->bJoyRestoreRaise?m_joy.y:0.))*fRaiseCoeff;
        double fMaxDeltaRaise = m_attr->_fMouseRaiseSpeed*fDeltaT;
        double fMouseDeltaRaise = AbsNoGreater(fTargetRaise-m_fMouseRaise,fMaxDeltaRaise);
        m_fMouseRaise = MinMax(m_fMouseRaise+fMouseDeltaRaise,-m_attr->_fMaxRaise,m_attr->_fMaxRaise);
        #endif
        #if 0

        double  fTargetSpeed;
		double	fMaxRaise, fMinRaise;

		double	fRaiseCoeff = m_attr->_fMaxRaise;
		//double	fMouseRaiseReq = m_fMouseRaise/fRaiseCoeff+m_mouse.y;

        m_mouse.y = MinMax(m_mouse.y,-1.,1.); // !!!moved
		double fTargetRaise0 = (m_mouse.y+(m_attr->bJoyRestoreRaise?m_joy.y:0.))*fRaiseCoeff;
		double fTargetRaise = MinMax(fTargetRaise0,-m_attr->_fMaxRaise,m_attr->_fMaxRaise);
		//m_mouse.y -= (fTargetRaise0-fTargetRaise)/fRaiseCoeff;
		//fTargetSpeed = m_fMouseRaise < fMouseRaiseReq*m_attr->_fMaxRaise ? m_attr->_fRaiseSpeed : -m_attr->_fRaiseSpeed;
		if( m_fMouseRaise < fTargetRaise )
			fMaxRaise = fTargetRaise, fMinRaise = -m_attr->_fMaxRaise, fTargetSpeed = m_attr->_fRaiseSpeed;
		else
			fMaxRaise = m_attr->_fMaxRaise, fMinRaise = fTargetRaise, fTargetSpeed = -m_attr->_fRaiseSpeed;
        //fMinRaise = -m_attr->_fMaxRaise; // !!!
        //fMaxRaise = m_attr->_fMaxRaise;  // !!!

		double	fMaxRaiseAccel = m_attr->_fRaiseSpeed/m_attr->fRaiseAccelTime;
		double	fMaxRaiseStopAccel = m_attr->_fRaiseSpeed/m_attr->fRaiseStopTime;
        double fRaiseAccel = m_fMouseRaiseSpeed < fTargetSpeed ? fMaxRaiseAccel : -fMaxRaiseAccel;
        double fRaiseAccelCoeff = Min(1.,fabs(fTargetRaise-m_fMouseRaise)/(5*M_PI/180));
        double fRaiseAccel1 = fRaiseAccel*fRaiseAccelCoeff;
        double fStopAccel;
        if( m_fMouseRaiseSpeed >= 0 ) {
			fStopAccel = -m_fMouseRaiseSpeed*m_fMouseRaiseSpeed*0.5/Max(1e-5,fMaxRaise-m_fMouseRaise);
			if( fStopAccel > -fMaxRaiseStopAccel )	fStopAccel = 1e10;
            if( fRaiseAccel1 >= fStopAccel ) {
                fRaiseAccel1 = fStopAccel;
				fTargetSpeed = 0;
			}
		}
		else {
			fStopAccel = -m_fMouseRaiseSpeed*m_fMouseRaiseSpeed*0.5/Min(-1e-5,fMinRaise-m_fMouseRaise);
			if( fStopAccel < fMaxRaiseStopAccel )	fStopAccel = -1e10;
            if( fRaiseAccel1 <= fStopAccel ) {
                fRaiseAccel1 = fStopAccel;
				fTargetSpeed = 0;
			}
		}

        double  fEndSpeed = m_fMouseRaiseSpeed+fRaiseAccel1*fDeltaT;
        if( m_fMouseRaiseSpeed < fTargetSpeed ) // !!!
			fEndSpeed = Min(fTargetSpeed,fEndSpeed);
		else
			fEndSpeed = Max(fTargetSpeed,fEndSpeed);
		double fDeltaRaise = (m_fMouseRaiseSpeed+fEndSpeed)*0.5*fDeltaT;
		m_fMouseRaise += fDeltaRaise;
		//m_mouse.y -= fDeltaRaise/fRaiseCoeff;
		m_fMouseRaiseSpeed = fEndSpeed;
		double	fUnclippedRaise = m_fMouseRaise;
		m_fMouseRaiseSpeed = MinMax(m_fMouseRaiseSpeed,-m_attr->_fRaiseSpeed,m_attr->_fRaiseSpeed);
        m_fMouseRaise = MinMax(m_fMouseRaise,-m_attr->_fMaxRaise,m_attr->_fMaxRaise); // !!!
        if( m_fMouseRaise < fMinRaise ) { // !!!moved
            m_fMouseRaiseSpeed = 0;
            m_mouse.y += (m_fMouseRaise-fMinRaise)/fRaiseCoeff;
            //m_fMouseRaise = fMinRaise;
        }
        else
        if( m_fMouseRaise > fMaxRaise ) {
            m_fMouseRaiseSpeed = 0;
            m_mouse.y += (m_fMouseRaise-fMaxRaise)/fRaiseCoeff;
            //m_fMouseRaise = fMaxRaise;
        }
         //= MinMax(m_fMouseRaise,fMinRaise,fMaxRaise);
		//m_mouse.y -= fDeltaRaise/fRaiseCoeff;
		//m_fMouseRaise = 0;
        #endif
	}


	switch( m_attr->eAccelControl ) {
		case VAE_WHEELS_AC_THROTTLE:	AccumAccelThrottle(fDeltaT); break;
		case VAE_WHEELS_AC_WALK:		AccumAccelWalk(fDeltaT); break;
		default: ASSERT(0);
	}

    {
        double f;
        f = exp(-m_attr->_fMouseHorzDecayLog*fDeltaT);
        m_mouse.x *= f;
        m_rawmouse.x *= f;
        m_mouse.y = (m_fMouseRaise+(m_mouse.y*m_attr->_fMaxRaise-m_fMouseRaise)*f)/m_attr->_fMaxRaise;
        m_rawmouse.y = (m_fMouseRaise+(m_rawmouse.y*m_attr->_fMaxRaise-m_fMouseRaise)*f)/m_attr->_fMaxRaise;
        f = exp(-m_attr->_fMouseDecayLog*fDeltaT);
        m_mouse = m_mouse*f+m_rawmouse*(1-f);
    }

	m_fStepTime += fDeltaT;
}

//#define	VESSEL_STEPUP	1.
void	CVesselWheels::PreStep(double fDeltaT)
{
	CHECK_DELTAT();

	BEGIN_LOG_VESSEL()
	//__t = m_nJump;

	if( !m_bTouchGround && m_nJump )	{
		m_nJump = 0;
		m_fJumpTimeOut = m_attr->fJumpTimeOut;
		m_fJumpOffsetFactor = 1;
	}
	if( m_nJump ) {
		double a = m_attr->fJumpDownWay; //0.5;
		//CFVector3	jumpSpeed(0,m_attr->_fMaxJumpSpeedV,-m_attr->_fMaxJumpSpeedH);
		//double fJumpFactor = (m_fRaise/m_attr->_fMaxRaise-1)*0.5;	// [0..1]
		//jumpSpeed.x *= fJumpFactor;
		if( m_nJump < 0 ) {
			double v = m_fJumpDownV;
			//if( m_speed.y < -v )
			//	v = -m_speed.y;
			CFVector3	jumpSpeed = m_jumpSpeed-m_base*m_speed;
			m_jumpOffset = jumpSpeed
								/m_attr->_fMaxJumpSpeedV*-a*sin(m_fJumpT*v/a);
			//m_jumpOffset = CFVector3(0,-a*sin(m_fJumpT*v/a),0);
			if( m_fJumpT*v/a > M_PI/2 ) {
				double	t0 = M_PI/2/v*a;
				m_nJump = 1; m_fJumpT -= t0;
				m_fJumpAccum = MinMax(m_fJumpAccum/t0,0.,1.);
			}
		}
		if( m_nJump > 0 ) {
			double fMaxDelay = m_attr->fMaxJumpUpWayTime; //0.5;
			double A = Max(m_fJumpAccum,M_PI/2/m_attr->_fMaxJumpSpeedV*a/fMaxDelay);
			double t = m_fJumpT*m_attr->_fMaxJumpSpeedV/a*A;
			CFVector3	jumpSpeed = m_jumpSpeed-m_base*m_speed;
			m_jumpOffset = jumpSpeed
								/m_attr->_fMaxJumpSpeedV*-a*cos(t);
			if( t > M_PI/2 ) {
				m_fJumpT -= M_PI/2/m_attr->_fMaxJumpSpeedV*a/A;
				m_nJump = 0;
				m_jumpOffset = CFVector3(0,0,0);
				//m_speed = m_base.MultTF(m_jumpSpeed*A);
				m_speed += m_base.MultTF(jumpSpeed*A);
				/*CFVector3	speedLocal = m_base%m_speed+m_jumpSpeed*A;
				double	fSpeedHorz = hypot(speedLocal.z,speedLocal.x);
				if( fSpeedHorz > m_attr->_fMaxJumpSpeedH) {
					double	fHorzCoeff = m_attr->_fMaxJumpSpeedH/fSpeedHorz;
					speedLocal.z /= fHorzCoeff;
					speedLocal.x /= fHorzCoeff;
				}
				m_speed = m_base.MultTF(speedLocal);
				m_speed.y = Min(m_speed.y,m_attr->_fMaxJumpSpeedV);*/
				m_offset += (m_speed-CFVector3(0,m_attr->g*0.5,0)*m_fJumpT)*m_fJumpT;
				m_speed.y -= m_attr->g*m_fJumpT;
				m_fJumpTimeOut = m_attr->fJumpTimeOut;
			}
		}
	} // jump processing

	m_bump.start = m_position+CFVector3(0,m_attr->fStepUp*m_fAccelFactor,0);
	m_bump.vel = m_offset/(fDeltaT>1e-10?fDeltaT:1e-10);
	m_bump.fTime = m_fStepTime = fDeltaT;
	m_bump.fRadius = m_fBumpRadius;
	m_bump.nBumpFlags = 0;
	m_bump.dwFlags1 = BF1_ADVANCED;
	m_dynBase = m_dynBase1 = m_bump.start;

	END_LOG_VESSEL()
}

//double	__y[3], __x[3];
double	__ysp, __y, __d;
double	__ress, __incl = 0;

void	CVesselWheels::ApplyStep()
{
	BEGIN_LOG_VESSEL()
	//SetDebugFlags(GetDebugFlags()|DBGF_LOG_MSG);

	bool bOldTouchGround = m_bTouchGround;
	m_bTouchGround = FALSE;
	CFVector3	&groundNormal = m_groundNormal;
	double fFallTime = m_fStepTime;

    m_position += m_bump.vel*m_bump.fTime; //*0.99;

    bool	bSwimming = m_attr->bSwimming && m_position.y < ZAV_Scene()->GetTerrain()->Waterline();
    if( bSwimming ) {
        goto Exit;
    }


	if( m_bump.nBumpFlags ) {
		fFallTime -= m_bump.fTime;
		//m_speed.x = m_speed.z = 0;
		/*if( m_bump.nBumpFlags == BF_BUMPDYNAMIC ) {
			m_speed.x = m_speed.z = 0;
			m_speed.y = 0;
		}*/
	}
	m_bPrevBump = FALSE;

    //bool bOldTouchGround = m_bTouchGround;
    //m_bTouchGround = FALSE;
    //CFVector3   &groundNormal = m_groundNormal;
	groundNormal = CFVector3(0,1,0);
	// Falling down
	{ // not bumped by dynamics, compute ressore
		SBumpDef def;
		//ASSERT( m_fRessoreWay >= -1 && m_fRessoreWay <= 1 );
		def.start = m_position+CFVector3(0,m_attr->fStepUp+m_fRessoreWay,0);
		def.vel = CFVector3(0,-1,0);
		def.fTime = 10000;
		def.nBumpFlags = 0;
		def.dwFlags1 = BF1_ADVANCED;
		def.fRadius = m_fBumpRadius-0.1;
		//def.start.y += def.fRadius-m_fBumpRadius;
		if( m_bump.nBumpFlags != BF_BUMPDYNAMIC ) VERIFY(ZAV_Scene()->Order()->Bump(def));
		__ysp = m_speed.y; //m_fYSpeed;
		__ress = m_fRessoreWay;
		__d = def.fTime;
		//ASSERT( __t >= 0 );
		if( def.nBumpFlags || m_bump.nBumpFlags == BF_BUMPDYNAMIC ) {
			def.fTime -= m_fBumpRadius-def.fRadius;
			if( def.nBumpFlags ) groundNormal = Normal(def.vel1-def.vel);
			def.fTime -= m_attr->fStepUp+m_fRessoreWay;  // way time ressoreway=0

			if( m_bump.nBumpFlags == BF_BUMPDYNAMIC ) {
				def.fTime = -m_fRessoreWay;
				//groundNormal = Normal(m_bump.vel1-m_bump.vel);
				groundNormal = CFVector3(0,1,0);
				def.vel1 = def.vel-((def.vel*groundNormal)*2)*groundNormal;
			}


			double y = (m_speed.y-m_attr->g*fFallTime*0.5)*fFallTime; //(m_fYSpeed - m_attr->g*m_fStepTime*0.5)*m_fStepTime; // freefall y
			__y = -y;
			//ASSERT( __t >= 0 );
			if( def.fTime >= -y ) {	// does not touch ground
				m_fRessoreWay = 0;
				/*m_fYSpeed -=*/ m_speed.y -= m_attr->g*m_fStepTime;
				def.fTime = -y;
				//__t = 1;
				m_fLastFallV = -m_speed.y;
			}
			else {
				m_fLastFallV = m_bTouchGround ? 0 :	-m_speed.y;
				m_bTouchGround = TRUE;
				//__t = 0;
				double t = m_fStepTime;
				//double fDeltaRessore;
				if( def.fTime >= 0 ) { // if fell for a while
					// v = sqrt(v0^2+g*s);
					//double fYSpeed = -sqrt(m_fYSpeed*m_fYSpeed+2*m_attr->g*def.fTime);
					double fYSpeed = -sqrt(m_speed.y*m_speed.y+2*m_attr->g*def.fTime);
					//double t1 = (m_fYSpeed-fYSpeed)/m_attr->g;
					double t1 = (m_speed.y-fYSpeed)/m_attr->g;
					ASSERT( t1 < t+0.001 );  // after t seconds we bumped in
					t -= t1; /*m_fYSpeed =*/ m_speed.y = fYSpeed; m_fRessoreWay = 0;
				}
				else {
					m_fRessoreWay = -def.fTime;
					//def.fTime = 0;
				}
					//fDeltaRessore = -(def.fTime-m_fRessoreWay);
				//m_fRessoreWay += fDeltaRessore;
				if( m_fRessoreWay > m_attr->fMaxRessoreWay ) { // happens only if not fell
					//def.fTime -= m_fRessoreWay-m_attr->fMaxRessoreWay;
					m_fRessoreWay = m_attr->fMaxRessoreWay;
				}
				for(;;) {
					double a = m_fRessoreWay*m_attr->_fRessoreK - m_attr->g;
					bool bLeave = t < m_attr->_fRessoreTime;
					double t1 = bLeave ? t : m_attr->_fRessoreTime;
					double e = exp(-m_attr->_fRessoreFrictionLog*t1);
					//m_fRessoreWay -= (m_fYSpeed+a*t1*0.5)/m_attr->_fRessoreFrictionLog*(1-e);
					m_fRessoreWay -= (m_speed.y+a*t1*0.5)/m_attr->_fRessoreFrictionLog*(1-e);
					//m_fYSpeed=(m_fYSpeed+a*t1)*e;
					m_speed.y=(m_speed.y+a*t1)*e;
					if( m_fRessoreWay > m_attr->fMaxRessoreWay) {
						m_fRessoreWay = m_attr->fMaxRessoreWay;
						//m_fYSpeed = 0;
						//m_speed.y = 0;
					} else
					if( m_fRessoreWay < 0 )	m_fRessoreWay = 0;
					if( bLeave ) break;
					t -= m_attr->_fRessoreTime;
				}
				def.fTime += m_fRessoreWay;
			} // ground touch computing
			m_position += def.vel*def.fTime;
		} // if bumped
	} // falling down

	if( bOldTouchGround && !m_bTouchGround )	m_maxHorzSpeed = CFVector3(m_speed.x,0,m_speed.z);
	if( m_bTouchGround ) m_maxHorzSpeed = CFVector3(0,0,0);


	// **********************
	//#define	_ALL_BUMP_3

	#ifdef	_ALL_BUMP_3
	if( m_bTouchGround ) {  // normal to ground
		double	fBumpRadius = m_fBumpRadius;
		if( m_attr->eAccelControl == VAE_WHEELS_AC_WALK ) fBumpRadius *= 0.3;
		CFVector3	pos[3];
		for( int i = 0 ; i < 3 ; i++ ) {
			SBumpDef	def;
			def.start = m_position+CFVector3(0,m_fRessoreWay,0);
			CFVector3	wheel = (fBumpRadius*0.79-0.2)*CFVector3(sin((i-0.5)*M_PI*2/3.),0,-cos((i-0.5)*M_PI*2/3.));
			CFVector3 offs = m_base.MultTF(wheel/*+CFVector3(0,1+m_fBumpRadius,0)*/);
			//__x[i] = offs.x;
			offs.y = 0; def.start += offs;
			def.vel = CFVector3(0,-1,0);
			def.fTime = 10000;
			def.nBumpFlags = 0;
			def.dwFlags1 = BF1_ADVANCED;
			def.fRadius = fBumpRadius*0.2;
			ZAV_Scene()->Order()->Bump(def);
			if( def.fTime > m_fBumpRadius*2 )	def.fTime = fBumpRadius;
			//if( def.nBumpFlags ) {
				pos[i] = def.start+def.vel*def.fTime;
				//__y[i] = pos[i].y;
			//} else __y[i] = 10000;
		}
		groundNormal = Normal((pos[1]-pos[2])%(pos[0]-pos[2]));
	}
	#endif

	// **********************



	// Inclination
	//bool bTouchGround = m_fRessoreWay >= 1e-3;
	//m_fAccelFactor = 1;
	__incl = 0;
	if( m_bTouchGround ) {
		__incl = acos(groundNormal.y)*180/M_PI;
		CFVector3	slide = (CFVector3(0,1,0)%groundNormal)%groundNormal,
					slideN = Normal(slide);
		if( groundNormal.y < m_attr->_fMinSlideCos ) {
			ASSERT( Abs(slide) > 0.001 );
			CFVector3	a = slideN*(Max(Abs(slide)-m_attr->_fMinSlideSin,0.)*m_attr->g);

			double		fSlideCos = 1;
						// slidecos = 1 if going against slide, but now accelfactor is against sliding
			m_fAccelFactor = Max(1+fSlideCos*((groundNormal.y-1)/(1-m_attr->_fCosMaxClimbAngle)),0.);
			// s' = -f*s+a
			double f = Max(m_attr->_fFrictionLog*m_fAccelFactor,0.001);
			CFVector3	a_f = a/f;
			double	e = exp(-f*m_fStepTime);
			m_speed = a_f+(m_speed-a_f)*e;
			//CFVector3	s0 = m_speed;
			//CFVector3	s0_a_f = s0-a_f;
			//m_offset += a_f*m_fStepTime+s0_a_f*(m_attr->_f1FrictionLog*(1-e));

		}

	}
	//else {
		//m_fCycloidArg = 0;
	//}

	// Cycloid calc
	if( m_attr->eAccelControl == VAE_WHEELS_AC_WALK ) {
		double	fSpeed = m_speed*m_base.Row(2);
		double	fSideSpeed = fabs(m_speed*m_base.Row(0));
		if( fSpeed >= 0 )
			fSpeed = hypot(fSpeed,fSideSpeed);
		else
			fSpeed = hypot(fSpeed,fSideSpeed);
		double s0 = -m_fStepTime*AbsNoGreater(fSpeed,m_attr->_fMaxWalkPendEffSpeed);
		double fSpeedCoeffTarg = Min(fabs(fSpeed)/m_attr->fMaxWalkSidePendSpeed,1.);
		if( m_fWalkPendSpeedCoeff < fSpeedCoeffTarg )
			m_fWalkPendSpeedCoeff = Min(m_fWalkPendSpeedCoeff+m_fStepTime/m_attr->fWalkPendFadeTime,fSpeedCoeffTarg);
		else
			m_fWalkPendSpeedCoeff = Max(m_fWalkPendSpeedCoeff-m_fStepTime/m_attr->fWalkPendFadeTime,fSpeedCoeffTarg);
		if( m_bTouchGround ) {
			m_fWalkPendulumCoeff = Min(1.,m_fWalkPendulumCoeff+m_fStepTime/m_attr->fWalkPendFadeTime);
		}
		else {
			m_fWalkPendulumCoeff = Max(0.,m_fWalkPendulumCoeff-m_fStepTime/m_attr->fWalkPendFadeTime);
		}
		m_fWalkPendulumArg += m_attr->_fWalkPendSpeed*s0; //*m_fWalkPendulumCoeff;
		/*while(m_fCycloidArg > m_attr->_fCycloidAngle)
			m_fCycloidArg -= m_attr->_fCycloidAngle*2;
		while(m_fCycloidArg < -m_attr->_fCycloidAngle)
			m_fCycloidArg += m_attr->_fCycloidAngle*2;
		m_fCycloidWay = (1-cos(m_fCycloidArg))*m_attr->_fCycloidAmpl;*/
		while( m_fWalkPendulumArg > M_PI*2 )	m_fWalkPendulumArg -= M_PI*2;
		double	alpha = sin(m_fWalkPendulumArg)*m_attr->_fWalkPendAngle;
		double a = cos(m_attr->_fWalkPendAngle);
		m_walkPendulumWay = Min(m_fWalkPendSpeedCoeff,m_fWalkPendulumCoeff)*CFVector3(m_attr->fWalkPendWidth*sin(alpha),m_attr->fWalkPendHeight*(cos(alpha)-a),0);
	}


	if( m_attr->bFollowInclination ) { // follow inclination
		CFVector3	pos[3];
		CFVector3 n0, axis, n1;
		if( m_bTouchGround ) {  // normal to ground
			#ifndef	_ALL_BUMP_3
			for( int i = 0 ; i < 3 ; i++ ) {
				SBumpDef	def;
				def.start = m_position+CFVector3(0,m_fRessoreWay,0);
				CFVector3	wheel = (m_fBumpRadius*0.79-0.2)*CFVector3(sin((i-0.5)*M_PI*2/3.),0,-cos((i-0.5)*M_PI*2/3.));
				CFVector3 offs = m_base.MultTF(wheel/*+CFVector3(0,1+m_fBumpRadius,0)*/);
				//__x[i] = offs.x;
				offs.y = 0; def.start += offs;
				def.vel = CFVector3(0,-1,0);
				def.fTime = 10000;
				def.nBumpFlags = 0;
				def.dwFlags1 = BF1_ADVANCED;
				def.fRadius = m_fBumpRadius*0.2;
				ZAV_Scene()->Order()->Bump(def);
				if( def.fTime > m_fBumpRadius )	def.fTime = m_fBumpRadius;
				//if( def.nBumpFlags ) {
					pos[i] = def.start+def.vel*def.fTime;
					//__y[i] = pos[i].y;
				//} else __y[i] = 10000;
			}
			n1 = Normal((pos[1]-pos[2])%(pos[0]-pos[2]));
			#endif
			n1 = groundNormal;
			// adjust n1 according to accel
			{
				CFVector3	accelaxis = m_lastAccel%n1;
				double		fAccelAxis = Abs(accelaxis),
							fAccelAngle = fAccelAxis*m_attr->_fAccelAngleFactor;
				if( fAccelAngle > 1e-5 ) {
					CFMatrix3x4	m;
					m.LoadIdentity().RotateL(accelaxis,Min(fAccelAngle,m_attr->_fMaxInclineAngle));
					n1 = m%n1;
				}
			}
			//n1 = groundNormal;
		}
		for( double t = m_fStepTime ; /*t >= 0*/ ; ) {
			if( m_bTouchGround ) {
				n0 = m_base.Row(1);
				axis = n1%n0;
			}
			double t1 = m_attr->fInclinationTime*0.5;
			if( t < t1 ) {
				if( m_bTouchGround ) m_moment += Normal(axis)*atan2(Abs(axis),n0*n1)*m_attr->_fInclOmega2*t;
				double e = exp(-m_attr->_fInclFrictionLog*t);
				m_base.RotateR(Normal(m_moment),Abs(m_moment)*m_attr->_f1InclFrictionLog*(1-e));
				m_moment *= e;
				break;
			}
			if( m_bTouchGround ) m_moment += Normal(axis)*atan2(Abs(axis),n0*n1)*m_attr->_fInclOmega2*t1;
			double e = exp(-m_attr->_fInclFrictionLog*t1);
			double a = Abs(m_moment)*m_attr->_f1InclFrictionLog*(1-e);
			m_base.RotateR(Normal(m_moment),a);
			m_moment *= e;
			t -= t1;
		}
		if( m_base.m[1][1] < m_attr->_fMinInclineCos ) { // inclined too much
			n0 = m_base.Row(1);
			n1 = CFVector3(0,1,0);
			axis = n1%n0;
			m_base.RotateR(Normal(axis),Max(0.,atan2(Abs(axis),n1*n0)-m_attr->_fMaxInclineAngle));
		}
	}

Exit:
	(m_dir = m_base).RotateOyL(m_fTower).RotateOxL(MinMax(m_fRaise+m_fMouseRaise,-m_attr->_fMaxRaise,m_attr->_fMaxRaise));
	m_base.Orthogonalize();

	m_fStepTime = 0;

	//ClipPos(m_position);
	#ifndef __VESSEL_IGNORE_MASTER
		m_master->setPosition(Pos()/*m_position*/);
	#endif

	//SetDebugFlags(GetDebugFlags()&~DBGF_LOG_MSG);
	END_LOG_VESSEL()
}

extern CViewScene *ZAV_Scene();

void CVesselWheels::NextFrame()
{
	BEGIN_LOG_VESSEL()
	SBumpDef	&def = m_bump, def0 = def;
	double	fTime0 = def.fTime;
	//SetDebugFlags(DBGF_DEFAULT|DBGF_LOG_MSG);
	//CViewScene	*pScene = ZAV_Scene();

    double  fWaterline = ZAV_Scene()->GetTerrain()->Waterline();

	//pScene->Bump(def);
	AllBump(def,TRUE);
	//def.nBumpFlags = 0; //
	//def.fTime = fTime0; //
	for( int i = 0 ; i < 4 && def.nBumpFlags ; i++ ) {
		CFVector3 groundNormal = Normal(def.vel1-def.vel);
		CFVector3 slide = (CFVector3(0,1,0)%groundNormal)%groundNormal;
		CFVector3 vel1 = (def.vel+def.vel1)*0.5;
		double fAccelFactor = Max(1-(Normal(vel1)*Normal(slide))*((groundNormal.y-1)/(1-m_attr->_fCosMaxClimbAngle)),0.);
		m_fAccelFactor = Min(fAccelFactor,m_fAccelFactor);
        if( !(m_attr->bSwimming && m_position.y < fWaterline) && vel1.y >= 0 )   vel1.y *= fAccelFactor;
		#if  _NEXTFRAME_TYPE
		{
			m_position += def.vel*def.fTime-0.001*Normal(def.vel);
			def.start += def.vel*def.fTime-0.001*Normal(def.vel);
			def.vel = vel1; //(def.vel+def.vel1)/2;
			m_speed = vel1;
			//if( def.vel.y >= 0 )	def.vel.y *= fAccelFactor;
			def.fTime = fTime0-def.fTime;
		}
		#else
		{
			CFVector3	ds = def.vel*def.fTime-0.001*Normal(def.vel),
						ds1 = vel1*(fTime0-def.fTime); //*m_fAccelFactor;
			m_speed = vel1;
			//if( ds1.y >= 0 )	ds1.y *= m_fAccelFactor;
			ds += ds1;
			def.vel = ds/(fTime0>1e-10?fTime0:1e-10);
			def.fTime = fTime0; //-def.fTime;
		}
		#endif
		def.nBumpFlags = 0;
		//pScene->Bump(def);
		AllBump(def,TRUE);
	}

	#if 0

	if(def.nBumpFlags) {
		CFVector3 groundNormal = Normal(def.vel1-def.vel);
		CFVector3 slide = (CFVector3(0,1,0)%groundNormal)%groundNormal;
		CFVector3 vel1 = (def.vel+def.vel1)*0.5;
		double fAccelFactor = Max(1-(Normal(vel1)*Normal(slide))*((groundNormal.y-1)/(1-m_attr->_fCosMaxClimbAngle)),0.);
		m_fAccelFactor = Min(fAccelFactor,m_fAccelFactor);
        if( !(m_attr->bSwimming && m_position.y < fWaterline) && vel1.y >= 0 )   vel1.y *= fAccelFactor;
		#if  _NEXTFRAME_TYPE
		{
			m_position += def.vel*def.fTime-0.001*Normal(def.vel);
			def.start += def.vel*def.fTime-0.001*Normal(def.vel);
			def.vel = vel1; //(def.vel+def.vel1)/2;
			m_speed = vel1;
			//if( def.vel.y >= 0 )	def.vel.y *= fAccelFactor;
			def.fTime = fTime0-def.fTime;
		}
		#else
		{
			CFVector3	ds = def.vel*def.fTime-0.001*Normal(def.vel),
						ds1 = vel1*(fTime0-def.fTime); //*m_fAccelFactor;
			m_speed = vel1;
			//if( ds1.y >= 0 )	ds1.y *= m_fAccelFactor;
			ds += ds1;
			def.vel = ds/(fTime0>1e-10?fTime0:1e-10);
			def.fTime = fTime0; //-def.fTime;
		}
		#endif
		def.nBumpFlags = 0;
		//pScene->Bump(def);
		AllBump(def,TRUE);
		if(def.nBumpFlags) {
			//m_position += def.vel*def.fTime;
			CFVector3 groundNormal = Normal(def.vel1-def.vel);
			CFVector3 slide = (CFVector3(0,1,0)%groundNormal)%groundNormal;
			CFVector3 vel1 = (def.vel+def.vel1)*0.5;
			//		double fSlideCos = Normal(m_base.Row(2)-(m_base.Row(2)*groundNormal)*groundNormal)*slideN;
			//		m_fAccelFactor = Max(1+(fSlideCos*(m_fFuelAccel>=0?1:-1))*((groundNormal.y-1)/(1-m_attr->_fCosMaxClimbAngle)),0.);
			double fAccelFactor = Max(1-(Normal(vel1)*Normal(slide))*((groundNormal.y-1)/(1-m_attr->_fCosMaxClimbAngle)),0.);
			m_fAccelFactor = Min(fAccelFactor,m_fAccelFactor);
            if( !(m_attr->bSwimming && m_position.y < fWaterline) && vel1.y >= 0 )   vel1.y *= fAccelFactor;
			#if _NEXTFRAME_TYPE
			{
				m_position += def.vel*def.fTime-0.001*Normal(def.vel);
				def.start += def.vel*def.fTime-0.001*Normal(def.vel);
				def.vel = vel1; //(def.vel+def.vel1)/2;
				m_speed = vel1;
				//if( def.vel.y >= 0 )	def.vel.y *= m_fAccelFactor;
				def.fTime = fTime0-def.fTime;
			}
			#else
			{
				CFVector3	ds = def.vel*def.fTime-0.001*Normal(def.vel),
							ds1 = vel1*(fTime0-def.fTime); //*m_fAccelFactor;
				m_speed = vel1;
				//if( ds1.y >= 0 )	ds1.y *= m_fAccelFactor;
				ds += ds1;
				def.vel = ds/(fTime0>1e-10?fTime0:1e-10);
				def.fTime = fTime0; //-def.fTime;
			}
			#endif
			def.nBumpFlags = 0;
			AllBump(def,TRUE);
			//pScene->Bump(def);
			//if(def.nBumpFlags) {
			//}
		}
	}
	#endif
	//SetDebugFlags(DBGF_DEFAULT&~DBGF_LOG_MSG);
	END_LOG_VESSEL()
}

void CVesselWheels::Draw()
{
	#ifdef	__VESSEL_IGNORE_MASTER
	if( pVesselObj ) {
		//m_fRadius = m_bump.fRadius = pVesselObj->Model()->Radius0();
		CFMatrix3x4	dt, dir;
		dt.LoadTransposed(m_dir);
		dir.LoadIdentity() /*.RotateOyL(M_PI)*/.TranslateL(-pVesselObj->Model()->Center());
		pVesselObj->GetDirModify().LoadMult(dt,dir).TranslateL(Pos());
		pVesselObj->LoadLights(m_dwLights);
		pVesselObj->Draw();
	}
	#endif
}

struct SWheelsSave {
	dword			dwSize;

	SFMatrix3x4	dir;
	SFMatrix3x4 base;
	SFVector3   pos;
	SFVector3	speed;
	SFVector3   moment;
	SFVector3   groundNormal;
	SFVector3   lastAccel;
	double      fFuelAccel;
	double      fThrottle;
	int         nForward;
	bool		bBrakingDone;
	bool		bTouchGround;
	int			iBraking;
	double		fRaise;
	double      fRaiseSpeed;
	double      fTurnSpeed;
	SFVector3	maxHorzSpeed;
	double		fAccelFactor;
	double		fJumpOffsetFactor;
	double		fJumpTimeOut;
	double		fRessoreWay;
	double		fWalkPendulumArg;
	double		fWalkPendulumCoeff;
	double		fWalkPendSpeedCoeff;
	double		fMouseTurnSpeed;
	double		fMouseRaise;
	double		fMouseRaiseSpeed;
	double		fJumpT;
	double		fJumpAccum;
	double		fJumpDownV;
	double		fLastFallV;
	SFVector3	jumpOffset;
	SFVector3	jumpSpeed;
	SFVector3	walkPendulumWay;
	int			nJump;
};

TCvoid *CVesselWheels::SaveGame()
{
	static SWheelsSave	s;
	s.dwSize = sizeof(s);
	s.dir 		   			= m_dir;
	s.base					= m_base;
	s.pos 		   			= m_position;
	s.speed					= m_speed;
	s.moment				= m_moment;
	s.groundNormal			= m_groundNormal;
	s.lastAccel				= m_lastAccel;
	s.fFuelAccel			= m_fFuelAccel;
	s.fThrottle				= m_fThrottle;
	s.nForward				= m_nForward;
	s.bBrakingDone			= m_bBrakingDone;
	s.bTouchGround			= m_bTouchGround;
	s.iBraking				= m_eBraking;
	s.fRaise				= m_fRaise;
	s.fRaiseSpeed			= m_fRaiseSpeed;
	s.fTurnSpeed			= m_fTurnSpeed;
	s.maxHorzSpeed			= m_maxHorzSpeed;
	s.fAccelFactor			= m_fAccelFactor;
	s.fJumpOffsetFactor 	= m_fJumpOffsetFactor;
	s.fJumpTimeOut			= m_fJumpTimeOut;
	s.fRessoreWay			= m_fRessoreWay;
	s.fWalkPendulumArg  	= m_fWalkPendulumArg;
	s.fWalkPendulumCoeff	= m_fWalkPendulumCoeff;
	s.fWalkPendSpeedCoeff	= m_fWalkPendSpeedCoeff;
	s.fMouseTurnSpeed 		= m_fMouseTurnSpeed;
	s.fMouseRaise			= m_fMouseRaise;
	s.fMouseRaiseSpeed 		= m_fMouseRaiseSpeed;
	s.fJumpT				= m_fJumpT;
	s.fJumpAccum 			= m_fJumpAccum;
	s.fJumpDownV			= m_fJumpDownV;
	s.fLastFallV			= m_fLastFallV;
	s.jumpOffset 			= m_jumpOffset;
	s.jumpSpeed				= m_jumpSpeed;
	s.walkPendulumWay		= m_walkPendulumWay;
	s.nJump					= m_nJump;

	return &s;
}

bool CVesselWheels::LoadGame(TCvoid *pSavedGame)
{
	SWheelsSave	&s = *(SWheelsSave*)pSavedGame;
	if( s.dwSize != sizeof(s) ) {
		ASSERT(0);
		return FALSE;
	}
	Stop();

	m_dir                  =   s.dir;
	m_base                 =   s.base;
	m_position             =   s.pos;
	m_speed				   =   s.speed;
	m_moment               =   s.moment;
	m_groundNormal         =   s.groundNormal;
	m_lastAccel            =   s.lastAccel;
	m_fFuelAccel           =   s.fFuelAccel;
	m_fThrottle            =   s.fThrottle;
	m_nForward             =   s.nForward;
	m_bBrakingDone         =   s.bBrakingDone;
	m_bTouchGround         =   s.bTouchGround;
	m_eBraking             =   (BRAKING_MODE)s.iBraking;
	m_fRaise               =   s.fRaise;
	m_fRaiseSpeed          =   s.fRaiseSpeed;
	m_fTurnSpeed           =   s.fTurnSpeed;
	m_maxHorzSpeed         =   s.maxHorzSpeed;
	m_fAccelFactor         =   s.fAccelFactor;
	m_fJumpOffsetFactor    =   s.fJumpOffsetFactor;
	m_fJumpTimeOut         =   s.fJumpTimeOut;
	m_fRessoreWay          =   s.fRessoreWay;
	m_fWalkPendulumArg     =   s.fWalkPendulumArg;
	m_fWalkPendulumCoeff   =   s.fWalkPendulumCoeff;
	m_fWalkPendSpeedCoeff  =   s.fWalkPendSpeedCoeff;
	m_fMouseTurnSpeed      =   s.fMouseTurnSpeed;
	m_fMouseRaise          =   s.fMouseRaise;
	m_fMouseRaiseSpeed     =   s.fMouseRaiseSpeed;
	m_fJumpT               =   s.fJumpT;
	m_fJumpAccum           =   s.fJumpAccum;
	m_fJumpDownV           =   s.fJumpDownV;
	m_fLastFallV           =   s.fLastFallV;
	m_jumpOffset           =   s.jumpOffset;
	m_jumpSpeed            =   s.jumpSpeed;
	m_walkPendulumWay      =   s.walkPendulumWay;
	m_nJump                =   s.nJump;

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

