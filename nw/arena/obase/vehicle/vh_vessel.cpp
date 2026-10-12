#define LAST_H__SCENE
#include "game.h"
#include "vh_vessel.h"
#include "zav.h"

//#define	CHECK_DELTAT()	ASSERT(fDeltaT>=0&&fDeltaT<2)
#define	CHECK_DELTAT()	(void)0
#define ISNAN(x) (((x)*2==(x))&&((x)!=0))


// ------------------------------------------------------------------------
Vessel::Vessel()
{
	m_pObj = NULL;
	m_pBase = NULL;
}

void	Vessel::Init()
{

    m_offset = CFVector3(0,0,0);


	m_bPrevBump = FALSE;
	m_fFuelSpeed = 0;
	m_fThrottle = 0;
	m_fVertRotReq = m_fVertRot = 0;
	m_fVertRotCtl = 0;
	m_fAxisRotReq = m_fAxisRot = 0;
	m_fInclineReq = m_fInclination = m_fInclineRot = 0;
	m_fStrafeInclination = m_fStrafeInclineRot = 0;
	m_fVertStrafe = 0;
	m_bForceage = FALSE;
//	m_fAngleX = 0, m_fAngleY = 0, m_fAngleZ = 0;
	m_dir.LoadIdentity();
	m_speed = CFVector3(0,0,0);
	m_fStepTime   = 0;
	m_bump.pBonus = NULL;
	m_bStrafe = FALSE;
        Stop();
}

void	Vessel::Restart()
{
	Init();
}

void	Vessel::Stop()
{
	m_fFuelSpeed  = 0;
	m_fThrottle   = 0;
	m_fVertRotReq = m_fVertRot = 0;
	m_fVertRotCtl = 0;
	m_fAxisRotReq = m_fAxisRot = 0;
	m_fInclineReq = m_fInclination = m_fInclineRot = 0;
	m_fStrafeInclination = m_fStrafeInclineRot = 0;
	m_fVertStrafe = 0;
	m_bForceage = FALSE;
//	m_fAngleX = 0, m_fAngleY = 0, m_fAngleZ = 0;
	m_speed = CFVector3(0,0,0);
	m_fStepTime = 0;
}


void	Vessel::AccumPreStep(double fDeltaT)
{
	CHECK_DELTAT();
	//ASSERT(fDeltaT>=0&&fDeltaT<2);
	double	fAngleY = atan2(-m_dir.m[2][0],m_dir.m[2][2]);
	double	fAngleX = atan2(m_dir.m[2][1],hypot(m_dir.m[2][0],m_dir.m[2][2]));
	CFVector3	normalY(m_dir.m[1][0],m_dir.m[1][1],m_dir.m[1][2]);
	// fAngleZ=atan2(projX'(normalY),projY'(normalY))
	double	fAngleZ = atan2(CFVector3(cos(fAngleY),0,sin(fAngleY))*normalY,
				CFVector3(sin(fAngleX)*sin(fAngleY),
					cos(fAngleX),-sin(fAngleX)*cos(fAngleY))*normalY);

	double	fAxisAngle = fAngleZ-m_fInclination-m_fStrafeInclination;

    const   double  fMaxIncline   = M_PI/6,
					fInclineAccel = 4*M_PI,
                    fTurnIncline  = M_PI;
	double	R = fInclineAccel/fMaxIncline,
			D = sqrt(4*R)*1.1,
			d = sqrt(D*D-4*R),
			lambda1 = (-D+d)/2, lambda2 = (-D-d)/2,
			a0 = fInclineAccel*m_fInclineReq*(1-m_bStrafe),
			v0 = m_fInclineRot, s0 = m_fInclination;
	double	C1 = (v0-lambda2*s0)/d, C2 = (lambda1*s0-v0)/d;
	double	C1v = a0/d/lambda1, C2v = -a0/d/lambda2,
			C1s = C1+C1v, C2s = C2+C2v;
	double fDeltaIncline = C1s*exp(lambda1*fDeltaT)-C1v+
					C2s*exp(lambda2*fDeltaT)-C2v - s0;
	m_fInclineRot = lambda1*C1s*exp(lambda1*fDeltaT)+
					lambda2*C2s*exp(lambda2*fDeltaT);
	double fDeltaTurn = (C1s/lambda1*(exp(lambda1*fDeltaT)-1)+
				  C2s/lambda2*(exp(lambda2*fDeltaT)-1)-(C1v+C2v)*fDeltaT) * fTurnIncline;

	// !!!
	if (m_fInclineRot > fMaxIncline*10) {
		m_fInclineRot = fMaxIncline;
	}
	ASSERT(m_fInclineRot <= fMaxIncline*10);
	// !!!

	// ------------------------------------------------------------------

	//const	double fHorzStrafe = 15/fMaxIncline;	// Max=10mps
	a0 = fInclineAccel*m_fInclineReq*m_bStrafe;
	v0 = m_fStrafeInclineRot; s0 = m_fStrafeInclination;
	C1 = (v0-lambda2*s0)/d, C2 = (lambda1*s0-v0)/d;
	C1v = a0/d/lambda1, C2v = -a0/d/lambda2;
	C1s = C1+C1v, C2s = C2+C2v;
	double fDeltaStrafeIncline = C1s*exp(lambda1*fDeltaT)-C1v+
						  C2s*exp(lambda2*fDeltaT)-C2v - s0;
	m_fStrafeInclineRot = lambda1*C1s*exp(lambda1*fDeltaT)+
						   lambda2*C2s*exp(lambda2*fDeltaT);

	// !!!
	if (m_fStrafeInclineRot > fMaxIncline * 10) {
		m_fStrafeInclineRot = fMaxIncline;
	}
	ASSERT(m_fStrafeInclineRot <= fMaxIncline * 10);
	// !!!

	// ------------------------------------------------------------------

	const	double	fMaxAxisTurn = M_PI,
					fAxisAccel = 9*M_PI;
	double	fLevellingAngle = atan2(sin(fAxisAngle*2),cos(fAxisAngle*2))/2;
	double	RMax = fAxisAccel/fMaxAxisTurn;
	R = RMax*fabs(cos(fAngleX));
	D = sqrt(4*RMax)*1.1;
	d = sqrt(D*D-4*R);
	lambda1 = (-D+d)/2, lambda2 = (-D-d)/2;
	a0 = fAxisAccel*m_fAxisRotReq;
	v0 = m_fAxisRot; s0 = fLevellingAngle;
	C1 = (v0-lambda2*s0)/d; C2 = (lambda1*s0-v0)/d;
	C1v = a0/d/lambda1; C2v = -a0/d/lambda2;
	C1s = C1+C1v; C2s = C2+C2v;
	double fDeltaAxis = C1s*exp(lambda1*fDeltaT)-C1v+
					C2s*exp(lambda2*fDeltaT)-C2v - s0;
	m_fAxisRot = lambda1*C1s*exp(lambda1*fDeltaT)+
					lambda2*C2s*exp(lambda2*fDeltaT);

	// !!!
	if (m_fAxisRot > fMaxAxisTurn * 10) {
		m_fAxisRot = fMaxAxisTurn;
	}
	ASSERT(m_fAxisRot <= fMaxAxisTurn * 10);
	// !!!


	// -----------------------------------------------------------
	const double fMaxVertRot = M_PI/2, fVertAccel = M_PI*4;
	if((m_fVertRotReq-m_fVertRotCtl)*m_fVertRotCtl>=0)
		m_fVertRotCtl = m_fVertRotCtl+
				(m_fVertRotReq-m_fVertRotCtl)*(1-exp(-4*fDeltaT));
	else
		m_fVertRotCtl = m_fVertRotReq;
	a0 = m_fVertRotCtl*fVertAccel*(m_bStrafe-1);
	v0 = m_fVertRot; D = fVertAccel/fMaxVertRot;
	m_fVertRot = (v0-a0/D)*exp(-D*fDeltaT)+a0/D;
	double	fDeltaVert = a0/D*fDeltaT-(v0-a0/D)/D*(exp(-D*fDeltaT)-1);

	// !!!
	if (m_fVertRot > fMaxVertRot * 10) {
		m_fVertRot = fMaxVertRot;
	}
	ASSERT(m_fVertRot <= fMaxVertRot * 10);
	// !!!
	// -----------------------------------------------------------
	// m_dir.LoadIdentity().RotateOyL(fAngleY).RotateOxL(fAngleX).
	//					RotateOzL(fAngleZ);
	m_dir.LoadIdentity().RotateOyL(fAngleY).
			RotateOxL(fAngleX).RotateOzL(fAxisAngle);
	CFVector3	rightN(m_dir.m[0][0],m_dir.m[0][1],m_dir.m[0][2]),
                upN   (m_dir.m[1][0],m_dir.m[1][1],m_dir.m[1][2]),
                backN (m_dir.m[2][0],m_dir.m[2][1],m_dir.m[2][2]);


    //RTCHECK(!ISNAN(fDeltaTurn),"!!!!");
    if (ISNAN(fDeltaTurn)) fDeltaTurn = 0;

    m_dir.RotateOyL(fDeltaTurn);
    //return;
    m_fInclination+=fDeltaIncline;
    if (ISNAN(m_fInclination)) m_fInclination = 0;

    m_fStrafeInclination+=fDeltaStrafeIncline;
    if (ISNAN(m_fStrafeInclination)) m_fStrafeInclination = 0;

    if (ISNAN(fDeltaAxis)) {fDeltaAxis = 0; Stop(); }

    m_dir.RotateOzL(m_fInclination+m_fStrafeInclination+fDeltaAxis);

    if (ISNAN(fDeltaVert)) fDeltaVert = 0;

    m_dir.RotateOxL(fDeltaVert);

	// -----------------------------------------------------------
	double fForceage = MAKEBOOL(m_bForceage)*9+1;
	const double fSpeedFact = 1;
	const double fMaxSpeed = 30*fSpeedFact, fFuelAccel = 70, fMinSpeed = -15*fSpeedFact;
	m_fFuelSpeed = Max(-fMinSpeed*Min(m_fThrottle,0.),
			Min(fMaxSpeed,m_fFuelSpeed+m_fThrottle*fMaxSpeed*fDeltaT/1.5));

	if( !m_bPrevBump ) {
		const double fHorzStrafe = 30*fSpeedFact*fForceage, fVertStrafe = 30*fSpeedFact*fForceage;
		D = fFuelAccel/fMaxSpeed;
		CFVector3	_v0 = m_speed, _a0D =	// vect a0/D
					(-m_fFuelSpeed*fForceage)*backN+
					//(m_fStrafeInclination*fHorzStrafe/fMaxIncline)*rightN+
					(m_fInclineReq*m_bStrafe*fHorzStrafe)*rightN-
					(m_fVertRotReq*m_bStrafe*fVertStrafe)*upN;
		m_speed = (_v0-_a0D)*exp(-D*fDeltaT)+_a0D;
		ASSERT(Abs(m_speed) <= fMaxSpeed*20);
	}
	else m_bPrevBump = FALSE;

	// !!!
	if (Abs(m_speed) > fMaxSpeed*10) {
		m_speed = Normal(m_speed) * fMaxSpeed;
	}
	ASSERT(Abs(m_speed) <= fMaxSpeed*10);
	// !!!

    if (ISNAN(m_speed.x)) {m_speed.x = 0; Stop(); }
    if (ISNAN(m_speed.y)) {m_speed.y = 0; Stop(); }
    if (ISNAN(m_speed.z)) {m_speed.z = 0; Stop(); }

    m_offset += m_speed*fDeltaT;
	m_fStepTime += fDeltaT;
}

void	Vessel::QuickAccumPreStep(double fDeltaT)
{
	CHECK_DELTAT();
	//ASSERT(fDeltaT>=0&&fDeltaT<2);
	m_offset += m_speed*fDeltaT;
	m_fStepTime += fDeltaT;
}

void	Vessel::PreStep(double fDeltaT)
{
	CHECK_DELTAT();
	m_bump.start = m_master->getPosition();
	m_bump.vel = m_offset/(fDeltaT>1e-10?fDeltaT:1e-10);
	//m_bump.vel = m_speed;
	m_bump.fTime = m_fStepTime = fDeltaT;
	//m_bump.fRadius = 2;
	m_bump.fRadius = 2.0;//FIXME?????????????;
	m_bump.nBumpFlags = 0;
	m_dynBase = m_dynBase1 = m_master->getPosition();
}

void	Vessel::ApplyStep()
{
    CFVector3 newPos(m_master->getPosition() + m_bump.vel*m_bump.fTime);
    if(  newPos.y > 371  ) newPos.y = 371; //FIXME
    m_master->setPosition( newPos );

	if( m_bump.nBumpFlags )
        {
		ASSERT(m_bump.nBumpFlags==BF_BUMPSTATIC||m_bump.nBumpFlags==BF_BUMPLAND);
		m_speed = CFVector3(0,0,0);
		m_bPrevBump = FALSE;
	} else m_bPrevBump = FALSE;
	m_fStepTime = 0;
}


void	Vessel::Throttle(double fDelta)
{
	m_fThrottle = MinMax(fDelta,-1.,1.);
}

void	Vessel::Raise(double fDeltaUp)
{
	m_fVertRotReq = MinMax(fDeltaUp,-1.,1.);
}

void	Vessel::Incline(double fDeltaRight)
{
	m_fInclineReq = MinMax(fDeltaRight,-1.,1.);
}

void	Vessel::Rotate(double fDeltaRight)
{
	m_fAxisRotReq = MinMax(fDeltaRight,-1.,1.);
}

void	Vessel::SetStrafe(bool bStrafe)
{
	m_bStrafe = MAKEBOOL(bStrafe);
}


void Vessel::NextFrame( double fDeltaT, double radius )
{
    SBumpDef	&def = m_bump;
    def.fRadius    = radius;


	PreStep(fDeltaT);

	// ------------------------

	double	fTime0 = def.fTime;
	ZAV_Scene()->Bump(def);

	if(def.nBumpFlags) {
		CFVector3 ds = def.vel*def.fTime-0.001*Normal(def.vel)+
			 (def.vel+def.vel1)/2*(fTime0-def.fTime);
		def.vel = ds/(fTime0>1e-10?fTime0:1e-10);
		def.fTime = fTime0;
		def.nBumpFlags = 0;
 		//WRITELOG3("Bump1: v(%lg,%lg,%lg)\n",def.vel.x,def.vel.y,def.vel.z);
		ZAV_Scene()->Bump(def);
		if(def.nBumpFlags) {
			CFVector3	ds = def.vel*def.fTime-0.001*Normal(def.vel)+
							(def.vel+def.vel1)/2*(fTime0-def.fTime);
			def.vel = ds/(fTime0>1e-10?fTime0:1e-10);
			def.fTime = fTime0;
			def.nBumpFlags = 0;
	 		//WRITELOG3("Bump2: v(%lg,%lg,%lg)\n",def.vel.x,def.vel.y,def.vel.z);
			ZAV_Scene()->Bump(def);
		}
	}
	ApplyStep();
}

void Vessel::SetMaster( ct_Subject *master )
{
   m_master = master;
}

void Vessel::SetPos(TCCFVector3 &v)
{
   m_master->setPosition(v);
}

TCCFVector3 Vessel::Pos()
{
   return m_master->getPosition();
}

void Vessel::Draw()
{}