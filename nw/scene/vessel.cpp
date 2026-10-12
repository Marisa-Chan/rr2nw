
#ifndef	__SCENE_CPP__
#include"_scene.h"
#endif

//#define	CHECK_DELTAT()	ASSERT(fDeltaT>=0&&fDeltaT<2)
#define	CHECK_DELTAT()	(void)0

//CViewObjectModel	*CVessel::m_pBase = NULL;

CViewObjectRef	*pVesselObj = NULL;

#if 1
// ------------------------------------------------------------------------
double CVessel::m_fMaxShield = 12.5;
CVessel::CVessel()
	:   CMovingObject(2),
		m_speed(0,0,0), m_position(0,2,20),
		m_shotSeq(100)
{
	m_fMaxShield = 12.5; //MISSIONCONFIG(m_vessel.fShield);
	m_pObj = NULL;
	m_pBase = NULL;
	m_bHighlightShots = TRUE;
	m_DisplayMessage = NULL;
	m_dynBase = m_dynBase1 = m_position;
}

void	CVessel::Init()
{
	ASSERT(SHOOTER_COUNT==CShot::T_SUP);
	CMovingObject::Init(m_fRadius);
	m_fShield = m_fMaxShield;
	for( int i = 0 ; i < SHOOTER_COUNT ; i++ ) {
		m_shooter[i].Set((CShot::TYPE)i,&m_shotSeq,this);
		m_shooter[i].SetMaxCharge(1000/*MISSIONCONFIG(m_shooter[i].nMaxCharge)*/);
		m_shooter[i].ReloadCharge(0);
		m_shooter[i].SetRate(0.1/*MISSIONCONFIG(m_shooter[i].fRate)*/);
	}
	m_nCurrShooter = 0; //MISSIONCONFIG(m_vessel.nShooter);
	m_shooter[m_nCurrShooter].LoadCharge(100
							/*MISSIONCONFIG(m_vessel.nCharge)*/);
	//m_shooter[0].Set(CFVector3(-1,-0.15,0),CFVector3(1,-0.15,0));
   	//m_shooter[1].Set(CFVector3(-1,-0.15,0),CFVector3(1,-0.15,0));
	//m_shooter[2].Set(CFVector3(-1,-0.15,0),CFVector3(1,-0.15,0));
	//m_shooter[3].Set(CFVector3(-1,-0.15,0),CFVector3(1,-0.15,0));
	//m_shooter[4].Set(CFVector3(-1,-0.15,0),CFVector3(1,-0.15,0));
	m_shotSeq.Clear();
	m_nExistFlags = EF_VIEWOBJ|EF_DYNAMIC;
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
	m_fStepTime = 0;
	m_bShoot = FALSE;
	m_bump.pBonus = &m_bonus;
	m_bonus.nBonus = 0;
	m_bonus.pOwner = this;
	m_bStrafe = FALSE;
}

void	CVessel::Restart()
{
	Init();
}

void	CVessel::Stop()
{
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
	m_speed = CFVector3(0,0,0);
	m_fStepTime = 0;
}

void	CVessel::Init(bool bMain,double fHue)
{
	m_bIsMain = m_bHighlightShots = MAKEBOOL(bMain);
	Init();
	#if 0
	if( !bMain ) {
		ASSERT(!m_pBase);
		m_pBase = new CViewObjectModel();
		if(!m_pBase) return;
		CTaggedFile	f(TRUE);
		f.Open("dm_emv.vbs");
		f.Descend("REM!",TRUE,0);
		f.Ascend();
		m_pBase->Read(f,TRUE);
		m_pBase->Split(TRUE);
		m_pBase->Hue(fHue);
		m_pObj = new CViewObjectRef(m_pBase);
	}
	#endif
	m_nBaseColor = (byte)paletteTranslator.Hue(
						(byte)paletteTranslator.Match(0x78,0x74,0x48),fHue);
	//for( int i = 0 ; i < SHOOTER_COUNT ; i++ )
	//	m_shooter[i].SetVesselType(bMain);
}

void	CVessel::_Draw()
{
	if( pVesselObj ) {
		m_fRadius = m_bump.fRadius = pVesselObj->Model()->Radius0();
		CFMatrix3x4	dt, dir;
		dt.LoadTransposed(m_dir);
		dir.LoadIdentity() /*.RotateOyL(M_PI)*/.TranslateL(-pVesselObj->Model()->Center());
		pVesselObj->GetDirModify().LoadMult(dt,dir).TranslateL(m_position);
		pVesselObj->LoadLights(m_dwLights);
		pVesselObj->Draw();
	}
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

void	CVessel::AccumPreStep(double fDeltaT)
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

	const	double	fMaxIncline = M_PI/6,
					fInclineAccel = 4*M_PI,
					fTurnIncline = M_PI;
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
				upN(m_dir.m[1][0],m_dir.m[1][1],m_dir.m[1][2]),
				backN(m_dir.m[2][0],m_dir.m[2][1],m_dir.m[2][2]);
	FPCheckNAN(fDeltaTurn);
	FPCheckNAN(fDeltaIncline);
	FPCheckNAN(m_fInclination);
	FPCheckNAN(fDeltaIncline);
	FPCheckNAN(m_fStrafeInclination);
	FPCheckNAN(fDeltaStrafeIncline);
	FPCheckNAN(fDeltaAxis);
	FPCheckNAN(fDeltaVert);
	m_dir.RotateOyL(fDeltaTurn).
			RotateOzL((m_fInclination+=fDeltaIncline)+
						(m_fStrafeInclination+=fDeltaStrafeIncline)+
														fDeltaAxis).
			RotateOxL(fDeltaVert);

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

	m_offset += m_speed*fDeltaT;
	m_fStepTime += fDeltaT;
}

void	CVessel::QuickAccumPreStep(double fDeltaT)
{
	CHECK_DELTAT();
	//ASSERT(fDeltaT>=0&&fDeltaT<2);
	m_offset += m_speed*fDeltaT;
	m_fStepTime += fDeltaT;
}

void CVessel::SetUpBonusCharge()
{
	m_bonus.nBonus = 0;
	//if( m_shooter.ChargeRemaining() > 0 )
	//	m_bonus.nBonus = -m_shooter.ChargeRemaining();
}

void	CVessel::PreStep(double fDeltaT)
{
	CHECK_DELTAT();
	//ASSERT(fDeltaT>=0&&fDeltaT<2);
	//WRITELOG2("Vessel %08X Charge=%d\n",this,m_shooter.ChargeRemaining());
	if( m_bShoot && IsAlive() ) {
		CFMatrix3x4	inv;
		inv.LoadTransposed(m_dir).TranslateL(m_position);
		//m_shooter[m_nCurrShooter].Shoot(inv, m_speed);
	}
	for( int i = 0 ; i < SHOOTER_COUNT ; i++ )
		m_shooter[i].Step(fDeltaT);
	m_shotSeq.Step(fDeltaT);

	m_bump.start = m_position;
	m_bump.vel = IsAlive() ? m_offset/(fDeltaT>1e-10?fDeltaT:1e-10) :
															CFVector3(0,0,0);
	//m_bump.vel = m_speed;
	m_bump.fTime = m_fStepTime = fDeltaT;
	//m_bump.fRadius = 2;
	if( pVesselObj )	m_bump.fRadius = pVesselObj->Model()->Radius();
	m_bump.nBumpFlags = 0;
	m_bonus.nBonus = 0;
	m_dynBase = m_dynBase1 = m_position;
	m_bDrawn = FALSE;
}

void	CVessel::ApplyStep()
{
	//WRITELOG2("Appl %08X: F=%X ",(unsigned)this,m_bump.nBumpFlags);
	//WRITELOG3("Vel=%lg,%lg,%lg ",m_bump.vel.x,m_bump.vel.y,m_bump.vel.z);
	//WRITELOG3("Vel1=%lg,%lg,%lg ",m_bump.vel1.x,m_bump.vel1.y,m_bump.vel1.z);
	//WRITELOG1("time=%lg",m_bump.fTime);
	//m_fShield = m_fMaxShield;
	if( m_nExistFlags & EF_VIEWOBJ ) {
		//WRITELOG3("Appl: pos0=%lg,%lg,%lg ",m_position.x,m_position.y,m_position.z);
		//WRITELOG4("vel=%lg,%lg,%lg t=%lg ",m_bump.vel.x,m_bump.vel.y,m_bump.vel.z,m_bump.fTime);
		m_position += m_bump.vel*m_bump.fTime;
		//WRITELOG3("pos=%lg,%lg,%lg\n",m_position.x,m_position.y,m_position.z);
		if( m_bump.nBumpFlags ) {
			if( m_bump.nBumpFlags == BF_BUMPSHOOT )
				//CMusic::SetDIGI(CMusic::DIGI_FOELASERHITDYNAMIC),
				m_bPrevBump = FALSE;
			else {
				if(m_bump.nBumpFlags==BF_BUMPSTATIC) {
					m_speed = CFVector3(0,0,0);
					m_bPrevBump = FALSE;
				} else {
					m_speed = m_bPrevBump ? CFVector3(0,0,0) : m_bump.vel1;
					//if( CMusic::IsActiveFX() )
					//	CMusic::SetDIGI(m_bump.nBumpFlags==BF_BUMPSTATIC?
					//		CMusic::DIGI_MYBUMPSTATIC:CMusic::DIGI_MYBUMPDYNAMIC,
					//		CViewObject::m_viewPointDirMx*m_position);
					m_bPrevBump = !m_bPrevBump;	//TRUE;
				}
			}
		} else m_bPrevBump = FALSE;
		if(!IsAlive()) {
			Die(m_fStepTime);
			//if( CMusic::IsActiveFX() )
			//	CMusic::SetDIGI(m_bIsMain?CMusic::DIGI_MYBOOM:CMusic::DIGI_FOEBOOM,
			//		CViewObject::m_viewPointDirMx*m_position);
		}
	}
	else {
		if( IsAlive() )	{
			m_nExistFlags = EF_DYNAMIC | EF_VIEWOBJ;
			m_eBumpability = BY_STRONG;
		}
		else {
			Die(m_fStepTime);
			if(!m_nExistFlags) {
				//m_nExistFlags = EF_DYNAMIC | EF_VIEWOBJ;
				/*if( this == &CViewCity::CurrCity()->Vessel() ) {
					m_fShield = m_fMaxShield;
					CViewCity::CurrCity()->GenRandomStartPos(*this);
				}*/
				for( int i= 0 ; i < SHOOTER_COUNT ; i++ )
					m_shooter[i].ReloadCharge(0);
				m_shooter[m_nCurrShooter=0].ReloadCharge(500);
				//m_position = CFVector3(0,100,0);
				m_speed = CFVector3(0,0,0);
				//m_dir.LoadIdentity();
			}
		}
	}
	m_fStepTime = 0;
}

bool CVessel::CollectBonus(SBonusDef &def)
{
	ASSERT(this);
	char szMsg[40];
	switch( def.eType ) {
		case SBonusDef::T_SHIELD:
			if( m_fShield >= m_fMaxShield*0.999 )	return FALSE;
			m_fShield = Min(m_fMaxShield,-def.nBonus/100.*m_fMaxShield+m_fShield);
			sprintf(szMsg,"‡€™ˆ’€ “‚…‹ˆ—…€ „Ž %d\n",(int)(m_fShield/m_fMaxShield*100+0.001));
			break;
		case SBonusDef::T_LASER:
			if( m_shooter[0].ChargeRemaining() >= m_shooter[0].MaxCharge() )	return FALSE;
			m_shooter[0].LoadCharge(-def.nBonus);
			sprintf(szMsg,"%d ‹€‡…›• ‡€Ÿ„Ž‚\n",-def.nBonus);
			break;
		case SBonusDef::T_PLASMAGUN:
			if( m_shooter[1].ChargeRemaining() >= m_shooter[1].MaxCharge() )	return FALSE;
			m_shooter[1].LoadCharge(-def.nBonus);
			sprintf(szMsg,"%d ‹€‡Œ…›• ‡€Ÿ„Ž‚\n",-def.nBonus);
			break;
		case SBonusDef::T_MACHINEGUN:
			if( m_shooter[2].ChargeRemaining() >= m_shooter[2].MaxCharge() )	return FALSE;
			m_shooter[2].LoadCharge(-def.nBonus);
			sprintf(szMsg,"%d €’ŽŽ‚\n",-def.nBonus);
			break;
		case SBonusDef::T_SUPERPLASMAGUN:
			if( m_shooter[3].ChargeRemaining() >= m_shooter[3].MaxCharge() )	return FALSE;
			m_shooter[3].LoadCharge(-def.nBonus);
			sprintf(szMsg,"%d ˆŽ›• ‡€Ÿ„Ž‚\n",-def.nBonus);
			break;
		case SBonusDef::T_ROCKET:
			if( m_shooter[4].ChargeRemaining() >= m_shooter[4].MaxCharge() )	return FALSE;
			m_shooter[4].LoadCharge(-def.nBonus);
			sprintf(szMsg,"%d €Š…’\n",-def.nBonus);
			break;
		default: ASSERT(0);
	}
	def.nBonus = 0;
	DisplayMessage(szMsg);
	return TRUE;
}

void	CVessel::SetAsViewPoint(bool bCentered,double fFocus,double fAspRatio,
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

void	CVessel::Throttle(double fDelta)
{
/*	m_fFuelSpeed = fDelta >= 0 ? Min(m_fFuelSpeed+0.1,1.) :
													Max(m_fFuelSpeed-0.1,0.);
	*/
	m_fThrottle = MinMax(fDelta,-1.,1.);
}

void	CVessel::Raise(double fDeltaUp)
{
//	m_dir.RotateOxL(-0.05*fDeltaUp);
	m_fVertRotReq = MinMax(fDeltaUp,-1.,1.);
}

void	CVessel::Incline(double fDeltaRight)
{
//	m_dir.RotateOzL(fDeltaRight);
	m_fInclineReq = MinMax(fDeltaRight,-1.,1.);
}

void	CVessel::Rotate(double fDeltaRight)
{
//	m_dir.RotateOzL(fDeltaRight);
	m_fAxisRotReq = MinMax(fDeltaRight,-1.,1.);
}

void	CVessel::SetStrafe(bool bStrafe)
{
	m_bStrafe = MAKEBOOL(bStrafe);
}

double	CVessel::ForwardSpeed() const
{
	CFVector3	normalZ(m_dir.m[2][0],m_dir.m[2][1],m_dir.m[2][2]);
	return	-(m_speed*normalZ);
}

void	CVessel::GetHits(bool bHits[6])
{
	for( int i = 0 ; i < 6 ; i++ )	bHits[i] = FALSE;
	for( CShot *ps = (CShot*) m_bangShots.Last() ;
			ps != NULL && ps->IsJustBanged() ; ps = (CShot *) ps->Prev() ) {
		CFVector3	n = Normal(m_dir*ps->BangOffset()),
					na = CFVector3(fabs(n.x),fabs(n.y),fabs(n.z));
		if( na.y > na.x ) {
			if( na.z > na.y )	bHits[n.z>0?5:0] = TRUE;
			else bHits[n.y>0?2:4] = TRUE;
		} else {
			if( na.z > na.x )	bHits[n.z>0?5:0] = TRUE;
			else bHits[n.x<0?1:3] = TRUE;
		}
	}
}

void	CVessel::LoadPos(TCCFMatrix3x4 &startPos)
{
	m_dir = startPos;
	CFMatrix3x4	t;
	t.LoadTransposed(m_dir);
	m_position = CFVector3(t.m[0][3],t.m[1][3],t.m[2][3]);
	m_dir.m[0][3] = m_dir.m[1][3] = m_dir.m[2][3] = 0;
}

#ifndef	_BIG_ENDIAN
void CVessel::SaveGame(CTaggedFile &f)
{
	f.Descend("VES_",FALSE,0);
		CMovingObject::SaveGame(f);
		f.Descend("VESG",TRUE,0);
			f.WriteDouble(m_fShield);
			f.WriteInt(m_nExistFlags);
			m_dir.Write(f);
			m_position.Write(f);
			m_speed.Write(f);
			f.WriteDouble(m_fFuelSpeed);
			f.WriteDouble(m_fThrottle);
			f.WriteDouble(m_fVertRot);
			f.WriteDouble(m_fVertRotCtl);
			f.WriteDouble(m_fAxisRot);
			f.WriteDouble(m_fInclination);
			f.WriteDouble(m_fInclineRot);
			f.WriteDouble(m_fStrafeInclination);
			f.WriteDouble(m_fStrafeInclineRot);
			f.WriteDouble(m_fVertStrafe);
			f.WriteInt(m_nCurrShooter);
		f.Ascend();
		for( int i = 0 ; i < SHOOTER_COUNT ; i++ )
			m_shooter[i].SaveGame(f);
	f.Ascend();
}
#endif

void CVessel::LoadGame(CTaggedFile &f)
{
	f.Descend("VES_",FALSE,0);
		CMovingObject::LoadGame(f);
		f.Descend("VESG",TRUE,0);
			f.ReadDouble(m_fShield);
			f.ReadInt(m_nExistFlags);
			if(!IsAlive()) m_nExistFlags = 0;
			m_dir.Read(f);
			m_position.Read(f);
			m_speed.Read(f);
			f.ReadDouble(m_fFuelSpeed);
			f.ReadDouble(m_fThrottle);
			f.ReadDouble(m_fVertRot);
			f.ReadDouble(m_fVertRotCtl);
			f.ReadDouble(m_fAxisRot);
			f.ReadDouble(m_fInclination);
			f.ReadDouble(m_fInclineRot);
			f.ReadDouble(m_fStrafeInclination);
			f.ReadDouble(m_fStrafeInclineRot);
			f.ReadDouble(m_fVertStrafe);
			f.ReadInt(m_nCurrShooter);
			m_fVertRotReq = 0;
			m_fVertRotReq = 0;
			m_fAxisRotReq = 0;
			m_fInclineReq = 0;
			m_bPrevBump = FALSE;
		f.Ascend();
		for( int i = 0 ; i < SHOOTER_COUNT ; i++ )
			m_shooter[i].LoadGame(f);
	f.Ascend();
	m_shotSeq.Clear();
}

/*
	Master asks each for (nVessel,SVesselControlPacket).
	In response it broadcasts (nVessel,SVesselOffsetPacket) for each one.
*/

struct	SVesselControlPacket {
	double		m[3][3];
	CFVector3	pos, speed;
	byte		nShoot;
	double		fShield;
};

int	CVessel::MakeControlPacket(byte *pBuffer,int len)
{
	(void)pBuffer; (void)len;
	#if !_DEMOVERSION
	ASSERT(len>=sizeof(SVesselControlPacket));
	SVesselControlPacket	*pPacket = (SVesselControlPacket*)pBuffer;
	pPacket->m[0][0] = m_dir.m[0][0];
	pPacket->m[0][1] = m_dir.m[0][1];
	pPacket->m[0][2] = m_dir.m[0][2];
	pPacket->m[1][0] = m_dir.m[1][0];
	pPacket->m[1][1] = m_dir.m[1][1];
	pPacket->m[1][2] = m_dir.m[1][2];
	pPacket->m[2][0] = m_dir.m[2][0];
	pPacket->m[2][1] = m_dir.m[2][1];
	pPacket->m[2][2] = m_dir.m[2][2];
	pPacket->pos = m_position;
	pPacket->speed = m_speed;
	pPacket->nShoot = m_bShoot &&
		m_shooter[m_nCurrShooter].ChargeRemaining() > 0 ? (byte)m_nCurrShooter : (byte)255;
	//pPacket->fFuelSpeed = m_fFuelSpeed;
	pPacket->fShield = m_fShield;
	#endif
	return	sizeof(SVesselControlPacket);
}

int	CVessel::ReadControlPacket(TCbyte *pBuffer,int len)
{
	(void)pBuffer; (void)len;
	#if !_DEMOVERSION
	ASSERT(len>=sizeof(SVesselControlPacket));
	SVesselControlPacket	*pPacket = (SVesselControlPacket*)pBuffer;
	m_dir.m[0][0] = pPacket->m[0][0];
	m_dir.m[0][1] = pPacket->m[0][1];
	m_dir.m[0][2] = pPacket->m[0][2];
	m_dir.m[1][0] = pPacket->m[1][0];
	m_dir.m[1][1] = pPacket->m[1][1];
	m_dir.m[1][2] = pPacket->m[1][2];
	m_dir.m[2][0] = pPacket->m[2][0];
	m_dir.m[2][1] = pPacket->m[2][1];
	m_dir.m[2][2] = pPacket->m[2][2];
	m_position = pPacket->pos;
	m_speed = pPacket->speed;
	m_bShoot = pPacket->nShoot != 255;
	if( m_bShoot )	{
		m_nCurrShooter = pPacket->nShoot;
		if( m_shooter[m_nCurrShooter].ChargeRemaining() <= 0 )
			m_shooter[m_nCurrShooter].LoadCharge(1);
	}
	//m_fFuelSpeed = pPacket->fFuelSpeed;
	m_fShield = pPacket->fShield;
	#endif
	return	len-sizeof(SVesselControlPacket);
}

struct	SVesselBangPacket {
	double		fEnergy;
};

int	CVessel::ReadBangPacket(TCbyte *pBuffer,int len)
{
	ASSERT(len>=sizeof(SVesselBangPacket));
	SVesselBangPacket	*pPacket = (SVesselBangPacket*)pBuffer;
	//WRITELOG1("Reading bang: %lg\n",pPacket->fEnergy);
	m_fShield -= pPacket->fEnergy;
	return	len-sizeof(SVesselBangPacket);
}

int	CVessel::MakeBangPacket(byte *pBuffer,int len,double fEnergy)
{
	ASSERT(len>=sizeof(SVesselBangPacket));
	SVesselBangPacket	*pPacket = (SVesselBangPacket*)pBuffer;
	pPacket->fEnergy = fEnergy;
	return	sizeof(SVesselBangPacket);
}

#else
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

CConfigFile	&ZAV_Config();

void	CVessel::Init()
{
	m_fMaxSpeed = ZAV_Config().GetDouble("Vessel","WalkSpeed",5.)*1000./3600.;
	m_fMaxInclSpeed = ZAV_Config().GetDouble("Vessel","WalkRot",90.)*M_PI/180.;
	m_fMaxVertSpeed = ZAV_Config().GetDouble("Vessel","WalkRaise",40.)*M_PI/180.;

	m_speed = CFVector3(0,0,0);
	m_position = CFVector3(0,0,0);
	m_head = CFVector3(0,1.7,0);
	m_dir.LoadIdentity();
	m_fVertRotReq = m_fAxisRotReq = m_fInclineReq = 0;
	m_fFuelSpeed = m_fThrottle = 0;
	m_bForceage = m_bStrafe = m_bPrevBump = FALSE;
	m_pBase = NULL; m_pObj = NULL;
	m_fBumpRadius = 2;
}

void	CVessel::AccumPreStep(double fDeltaT)
{
	CHECK_DELTAT();

	double	fSpeed = Abs(m_speed);
	double	fRotY = atan2(m_dir.m[2][0],m_dir.m[2][2]);
	double	fRaise = atan2(-m_dir.m[2][1],hypot(m_dir.m[2][0],m_dir.m[2][2]));

	fSpeed = m_fMaxSpeed*m_fThrottle*(MAKEBOOL(m_bForceage)*50+1);
	fRotY -= m_fInclineReq*fDeltaT*m_fMaxInclSpeed;
	fRaise += m_fVertRotReq*fDeltaT*m_fMaxVertSpeed;
	fRaise = MinMax(fRaise,-45*M_PI/180,45*M_PI/180);

	m_dir.LoadIdentity().RotateOyL(-fRotY);
	m_speed = -m_dir.Row(2)*fSpeed;
	m_offset += m_speed * fDeltaT;
	m_dir.RotateOxL(-fRaise);

	m_fStepTime += fDeltaT;
}

#define	VESSEL_STEPUP	1.

void	CVessel::PreStep(double fDeltaT)
{
	CHECK_DELTAT();

	m_bump.start = m_position-m_head+CFVector3(0,m_fBumpRadius+VESSEL_STEPUP,0);
	m_bump.vel = m_offset/(fDeltaT>1e-10?fDeltaT:1e-10);
	m_bump.fTime = m_fStepTime = fDeltaT;
	m_bump.fRadius = m_fBumpRadius;
	m_bump.nBumpFlags = 0;
	m_dynBase = m_dynBase1 = m_bump.start;
}

CViewScene *ZAV_Scene();

void	CVessel::ApplyStep()
{
	//CFVector3	oldpos = m_position;
	#if 0
	m_bump.start = m_position-m_head;
	m_bump.vel = m_offset/(m_fStepTime>1e-10?m_fStepTime:1e-10);
	m_bump.fTime = m_fStepTime;
	m_bump.fRadius = m_fBumpRadius;
	m_bump.nBumpFlags = 0;
	ZAV_Scene()->Order()->Bump(m_bump);
	#endif

	/*double fTime0 = m_fStepTime;
	if( ZAV_Scene()->Order()->Bump(m_bump) ) {
		CFVector3	ds = def.vel*def.fTime-0.001*Normal(def.vel)+
						(def.vel+def.vel1)/2*(fTime0-def.fTime);
		m_bump.vel = ds/(fTime0>1e-10?fTime0:1e-10);
	}*/

	//CFVector3	oldpos = m_position;
	m_position += m_bump.vel*m_bump.fTime*0.99;
	if( m_bump.nBumpFlags ) {
		m_speed = CFVector3(0,0,0);
	}
	m_bPrevBump = FALSE;
	m_fStepTime = 0;

	//CFVector3	d = m_position-oldpos;
	//if( atan2(d.y,hypot(d.x,d.z)) > 45*M_PI/180 )	m_position = oldpos;


	#if 1
	SBumpDef	def;
	def.start = m_position-m_head+CFVector3(0,m_fBumpRadius+VESSEL_STEPUP,0);
	def.vel = CFVector3(0,-1,0);
	def.fTime = 10000;
	def.nBumpFlags = 0;
	def.fRadius = m_fBumpRadius;
	VERIFY(ZAV_Scene()->Order()->Bump(def));
	if( def.nBumpFlags ) {
		m_position = def.start+def.vel*def.fTime-CFVector3(0,def.fRadius,0)+m_head;
		//CFVector3 n; double h;
		//ZAV_Scene()->GetTerrain()->GetPlane(m_position,n,h);
		//if( m_position.y-m_head.y < h ) m_position.y = h+m_head.y;
	}
	#endif
}

void CVessel::Draw()
{
	#if 0
	if( pVesselObj ) {
		//m_fRadius = m_bump.fRadius = pVesselObj->Model()->Radius0();
		CFMatrix3x4	dt, dir;
		dt.LoadTransposed(m_dir);
		dir.LoadIdentity() /*.RotateOyL(M_PI)*/.TranslateL(-pVesselObj->Model()->Center());
		pVesselObj->GetDirModify().LoadMult(dt,dir).TranslateL(m_position);
		pVesselObj->LoadLights(m_dwLights);
		pVesselObj->Draw();
	}
	#endif
}

#endif

