#ifndef	__SCENE_CPP__
#include"_scene.h"
#endif

// ------------------------------------------------------------------------
CPhasedMovie CShot::m_staticHitMovie;
CPhasedMovie CShot::m_dynamicHitMovie;
CPhasedMovie CShot::m_chargeBallMovie;
CPhasedMovie CShot::m_hiChargeBallMovie;
CViewObjectModel	*CShot::m_pRocketBase = NULL;
CViewObjectRef		*CShot::m_pRocketRef = NULL;
int    CShot::m_nColors[CShot::CLR_SUP];
double CShot::m_fEnergies[CShot::T_SUP] = { 0.25, 1, 0.25, 4, 3 };
// ------------------------------------------------------------------------
double CShot::Energy() const
{
	return m_fEnergies[m_eType];
	//return MISSIONCONFIG(m_shooter[m_eType].fShield);
}
// ------------------------------------------------------------------------
//const double fRocketSpeed = 40;
void CShot::Init(CShot::TYPE eType,TCCFVector3 &start,TCCFVector3 &speed,
								TCCFVector3 &nZ, CMovingObject *pSource)
{
    ASSERT(!m_pList);
    #ifdef _DEBUG
//	m_dwTime0 = CTimer::Time();
	#endif
	double fSpeed;
	m_bump.fMass = 0;
	fSpeed = 40; m_fTime = 0.05; m_fLifeTime = 1.5;
	//fSpeed = MISSIONCONFIG(m_shooter[eType].fSpeed);
	//m_fTime = MISSIONCONFIG(m_shooter[eType].fTime);
	//m_fLifeTime = MISSIONCONFIG(m_shooter[eType].fLifeTime);
	switch(m_eType=eType) {
		case T_LASER:
			// speed 40, time 0.05, lifetime 1.5, rate 0.1
			m_eDType = DT_LINEAR;
			//fSpeed=40; m_fTime=0.05; m_fLifeTime=1.5;
			m_bump.fRadius=0; break;
		case T_CHARGEBALL:
			m_eDType = DT_SPHERIC;
			//fSpeed=20; m_fTime=0; m_fLifeTime=3;
			m_bump.fRadius=0.5; break;
		case T_BULLET:
			m_eDType = DT_SPHERIC;
			//fSpeed=200; m_fTime=0; m_fLifeTime=1;
			m_bump.fRadius=0; break;
		case T_HICHARGEBALL:
			m_eDType = DT_SPHERIC;
			//fSpeed=20; m_fTime=0; m_fLifeTime=3;
			m_bump.fRadius=0.5; break;
		case T_ROCKET:
			m_eDType = DT_SPHERIC;
			//fSpeed=fRocketSpeed; m_fTime=0; m_fLifeTime=3;
			m_bump.fRadius=0.5; break;
	}
	m_dynBase = (m_oldend=m_dynEnd=start)
						+(m_dynVel=nZ*(speed*nZ-fSpeed))*m_fTime;
	//m_dynBase = (m_oldend=m_dynEnd=start)
	//					+(m_dynVel=speed-nZ*fSpeed)*m_fTime;
	m_nStatus = S_INIT; m_pSource = pSource;
}
// ------------------------------------------------------------------------
void CShot::InitDraw()
{
	m_fDrawT1 = 0; m_fDrawT2 = m_fTime;
	m_dynBase1 = m_dynBase; m_dynEnd1 = m_dynEnd; m_bump.vel = m_dynVel;
	m_bump.start = m_oldend; m_bump.fTime = m_fStepTime;
	if(m_nStatus==S_INIT) {
		if( m_fStepTime == 0 ) return;
		m_nStatus = S_ONWAY;
		ASSERT(m_fStepTime>1e-3&&m_fStepTime<100);
		m_bump.vel *= m_fTime/m_fStepTime;
	}
	//m_bump.pBody = NULL;
	m_bump.nBumpFlags = 0;
	if(m_nStatus==S_ONWAY) m_pBangDynamic = NULL;
}
// ------------------------------------------------------------------------
bool	CShot::Step(double fTime)
{
	if( m_nStatus == S_OFF )	return FALSE;
//	ASSERT(CTimer::Diff(CTimer::Time(),m_dwTime0)/CTimer::TicksD() < 6.0);
	if( m_nStatus == S_INIT ) { m_fStepTime = fTime; return TRUE; }
	if( m_fLifeTime <= 0 )	{
		//WRITELOG1("OFF %08X",this);
		m_nStatus = S_OFF;
		if (m_pBangDynamic) {
			m_pBangDynamic->m_bangShots.Remove(this);
		}
        ASSERT(!m_pList);
		return FALSE; }
	if( (m_fLifeTime-=fTime) <= 0 )	{
		//if (m_nStatus == S_ONWAY)
		fTime = Max(0.,fTime+m_fLifeTime);
	}
	switch( m_nStatus ) {
		case S_ONWAY: {
			#if 0
			if( m_eType == T_ROCKET ) {
				WRITELOG3("Targetting (%lg,%lg,%lg):\n",m_dynBase.x,m_dynBase.y,m_dynBase.z);
				WRITELOG3("dynVel=%lg,%lg,%lg\n",m_dynVel.x,m_dynVel.y,m_dynVel.z);
				CViewCity	*pCurrCity = CViewCity::CurrCity();
				pCurrCity->StartEnumTargets();
				CFVector3 pos, vel, v0 = Normal(m_dynVel);
				double fBest = 1e10; bool bRes = FALSE;
				int i = 0;
				while( pCurrCity->NextTarget(pos) ) {
					WRITELOG4("Target[%d]=%lg,%lg,%lg ",i,pos.x,pos.y,pos.z);
					CFVector3	offset = pos-m_dynBase;
					double	fScalar = v0*offset;
					double	f = Abs(offset)-fScalar;
					WRITELOG1("f=%lg\n",f);
					if( !bRes || f < fBest ) {
						vel = Normal(offset)*fRocketSpeed;
						WRITELOG3("Besting: %lg,%lg,%lg\n",vel.x,vel.y,vel.z);
						fBest = f;
					}
					bRes = TRUE;
					i++;
				}
				if( bRes )	m_dynVel += PhaseDelta(m_dynVel,vel,fTime,1.);
				WRITELOG3("dynVel=%lg,%lg,%lg\n",m_dynVel.x,m_dynVel.y,m_dynVel.z);
			} // if ROCKET
			#endif
			m_oldend = m_dynBase; //m_dynEnd;
			CFVector3	delta;
			m_dynEnd += (delta=m_dynVel*fTime);	//+m_speed*m_fTime;
			m_dynBase += delta;
			m_fDrawT1 = 0;
			m_fDrawT2 = m_fTime;
			m_fStepTime = fTime;
			return TRUE;
		}
		case S_BANG:
			return TRUE;
		case S_JUSTBANG:
			//WRITELOG1("BANGED %08X",this);
			//if( CMusic::IsActiveFX() )
			//CMusic::SetDIGI(m_pBangDynamic?CMusic::DIGI_MYLASERHITDYNAMIC:
			// 					CMusic::DIGI_MYLASERHITSTATIC,
			//						CViewObject::m_viewPointDirMx*m_dynBase);
			m_nStatus = S_BANG;
			return TRUE;
	}
	ASSERT(0);
	return FALSE;
}
// ------------------------------------------------------------------------
int nBangCnt = 0;
void CShot::Bang()
{
	m_bump.fTime -= 0.0001;
	if(m_bump.fTime<m_fDrawT2) m_fDrawT2 = m_bump.fTime;
	//if(m_fDrawT1>m_fDrawT2) m_fDrawT1 = m_fDrawT2;
	m_nStatus = S_JUSTBANG; m_fTime = 0;
	CFVector3 newbase = m_oldend+m_dynVel*m_bump.fTime;
	m_dynEnd += newbase-m_dynBase;
	m_dynBase = newbase;
	//WRITELOG3("Bang: DynB=%lg,%lg,%lg\n",m_dynBase.x,m_dynBase.y,m_dynBase.z);
	//WRITELOG3("Bang: DynE=%lg,%lg,%lg\n",m_dynEnd.x,m_dynEnd.y,m_dynEnd.z);
	nBangCnt = (nBangCnt+1)%100; m_pBangDynamic = NULL;
	m_fLifeTime =
			m_staticHitMovie.Time(m_staticHitMovie.Count()-1);
	//WRITELOG3("Bang=%lg,%lg,%lg\n",
	//		m_bump.start.x+m_bump.vel.x*m_bump.fTime,
	//		m_bump.start.y+m_bump.vel.y*m_bump.fTime,
	//		m_bump.start.z+m_bump.vel.z*m_bump.fTime);
}
// ------------------------------------------------------------------------
void CShot::ApplyBang() {
	ASSERT(m_pBangDynamic);
	double	fDrawTime = m_bump.fTime-m_fStepTime+m_fTime;
	if(fDrawTime<m_fDrawT2)
		m_fDrawT2 = Max(m_fDrawT1,fDrawTime);
    m_fTime = 0;
	CFVector3 newbase = m_oldend+m_dynVel*m_bump.fTime;
	m_dynEnd += newbase-m_dynBase;
	m_dynBase = newbase;
    //m_dynEnd = m_oldend+m_dynVel*m_bump.fTime;
    nBangCnt = (nBangCnt+1)%100;
    m_bangOffset = m_dynBase-m_pBangDynamic->m_dynBase;
	//CFVector3 end = m_dynBase+m_dynVel*m_fDrawT2;
    m_fLifeTime =
		    m_dynamicHitMovie.Time(m_dynamicHitMovie.Count()-1);
    m_pBangDynamic->Bang(this);
}
// ------------------------------------------------------------------------
void	CShot::DrawLaserShot()
{
	#if 0
	double fWidth = 0.1/2;

	//WRITELOG2("DrawLaserShot: %lg,%lg\n",m_fDrawT1,m_fDrawT2);
	//WRITELOG3("P0=%lg,%lg,%lg ",m_dynBase.x,m_dynBase.y,m_dynBase.z);
	//WRITELOG3("P1=%lg,%lg,%lg\n",
	//	m_dynBase.x+m_dynVel.x*m_fDrawT2,m_dynBase.y+m_dynVel.y*m_fDrawT2,
	//		m_dynBase.z+m_dynVel.z*m_fDrawT2);

	CFVector3	startIm, endIm;
	CViewObject::m_viewPointDirMx.Mult(
									m_dynEnd+m_dynVel*m_fDrawT1,startIm);
	CViewObject::m_viewPointDirMx.Mult(m_dynEnd+m_dynVel*m_fDrawT2,endIm);
	CFVector3	endStartIm = endIm-startIm;
	CFVector3	widthOffset =
					Normal(startIm % endStartIm)*fWidth,
				widthOffset1 = fWidth*Normal(endStartIm);
	startIm &= CViewObject::m_viewPointScale;
	endIm &= CViewObject::m_viewPointScale;
	widthOffset &= CViewObject::m_viewPointScale;
	widthOffset1 &= CViewObject::m_viewPointScale;

	const	int		nVerts = 5;

	static	CFVector3	ptIm3[nVerts];
	ptIm3[0] = startIm;
	ptIm3[1] = endIm+widthOffset-widthOffset1;
	ptIm3[2] = endIm+widthOffset/2;
	ptIm3[3] = endIm-widthOffset/2;
	ptIm3[4] = endIm-widthOffset-widthOffset1;
	static	bool	bClip[nVerts];

	int i;
	bClip[0] = ptIm3[0].z < CViewObject::m_fFrontClip;
	bClip[1] = ptIm3[1].z < CViewObject::m_fFrontClip;
	bClip[2] = ptIm3[2].z < CViewObject::m_fFrontClip;
	bClip[3] = ptIm3[3].z < CViewObject::m_fFrontClip;
	bClip[4] = ptIm3[4].z < CViewObject::m_fFrontClip;

	static	CVector2	ptIm2[nVerts*2];
	static	TCCVector2	*pPtIm2[nVerts*2] = {
		ptIm2, ptIm2+1, ptIm2+2, ptIm2+3, ptIm2+4, ptIm2+5, ptIm2+6, ptIm2+7,
		ptIm2+8, ptIm2+9
	};
	CVector2	*pIm2 = ptIm2;

	for( i = 0 ; i < nVerts ; i++ ) {
		if( bClip[i] ) {
			int  iOth;
			if( !bClip[iOth=i>0?i-1:nVerts-1] ) {
				double	zDiff = ptIm3[iOth].z-ptIm3[i].z;
				double	t = zDiff > 1e-3 ?
					(CViewObject::m_fFrontClip-ptIm3[i].z)/zDiff : 0.5;
				pIm2->x = (ptIm3[i].x+(ptIm3[iOth].x-ptIm3[i].x)*t)
												/ CViewObject::m_fFrontClip;
				pIm2++->y = (ptIm3[i].y+(ptIm3[iOth].y-ptIm3[i].y)*t)
												/ CViewObject::m_fFrontClip;
			}
			if( !bClip[iOth=i<nVerts-1?i+1:0] ) {
				double	zDiff = ptIm3[iOth].z-ptIm3[i].z;
				double	t = zDiff > 1e-3 ?
					(CViewObject::m_fFrontClip-ptIm3[i].z)/zDiff : 0.5;
				pIm2->x = (ptIm3[i].x+(ptIm3[iOth].x-ptIm3[i].x)*t)
												/ CViewObject::m_fFrontClip;
				pIm2++->y = (ptIm3[i].y+(ptIm3[iOth].y-ptIm3[i].y)*t)
												/ CViewObject::m_fFrontClip;
			}
		} else {
			pIm2->x = ptIm3[i].x/ptIm3[i].z;
			pIm2++->y = ptIm3[i].y/ptIm3[i].z;
		}
	}
	if( pIm2 == ptIm2 )	return;
	GRCFillFlatPolyPCCW(pPtIm2,pIm2-ptIm2,
		(byte)m_nColors[((CMovingObject*)m_pSource)->m_bHighlightShots?
												CLR_GREENLASER:CLR_REDLASER]);
	#endif
}
// ------------------------------------------------------------------------
void	CShot::DrawChargeBall()
{
	//if(m_nStatus==S_JUSTBANG) return;
	CPhasedMovie &movie = m_chargeBallMovie;
	movie.Draw(movie.Time(movie.Count()-1)-m_fLifeTime,m_dynBase,
		0.04);
}
// ------------------------------------------------------------------------
void	CShot::DrawHiChargeBall()
{
	//if(m_nStatus==S_JUSTBANG) return;
	CPhasedMovie &movie = m_hiChargeBallMovie;
	movie.Draw(movie.Time(movie.Count()-1)-m_fLifeTime,m_dynBase,
		0.04);
}
// ------------------------------------------------------------------------
void	CShot::DrawRocket()
{
	//if(m_nStatus==S_JUSTBANG) return;
	m_pRocketRef->GetDirModify().LoadIdentity().RotateOxL(
			atan2(m_dynVel.y,hypot(m_dynVel.x,m_dynVel.z))-M_PI/2).
			RotateOyL(atan2(-m_dynVel.x,-m_dynVel.z)).TranslateL(m_dynBase);
	m_pRocketRef->Draw();
}
// ------------------------------------------------------------------------
void	CShot::Draw()
{
	if( m_nStatus == S_OFF || m_nStatus == S_INIT )	return;
	if( m_nStatus == S_BANG || m_nStatus == S_JUSTBANG ) {
		//WRITELOG("Bang draw\n");
		CPhasedMovie &movie = m_pBangDynamic?m_dynamicHitMovie:
														m_staticHitMovie;
		double fTime = movie.Time(movie.Count()-1);
		//if( m_nStatus == S_JUSTBANG )
		//	WriteLog("Bang shot: %lg,%lg,%lg",m_end.x,m_end.y,m_end.z);
		movie.Draw(movie.Time(movie.Count()-1)-m_fLifeTime,m_dynBase,
				m_pBangDynamic ? 0.02 : 0.04);
		if( m_nStatus == S_BANG ) return;
	}

	switch(m_eType) {
		case T_LASER:		DrawLaserShot(); break;
		case T_BULLET:	break;
		case T_CHARGEBALL:	DrawChargeBall(); break;
		case T_HICHARGEBALL:	DrawHiChargeBall(); break;
		case T_ROCKET:		DrawRocket(); break;
		default: ASSERT(0);
	}
}
// ------------------------------------------------------------------------
void CShot::LoadMovie(double fChargeTime,double fHiChargeTime)
{
	m_staticHitMovie.Load("hitd.spm");
	for(int i = 0 ; i < m_staticHitMovie.Count() ; i++ )
		m_staticHitMovie.Time(i) = (i+1)/12.;
	m_dynamicHitMovie.Load("hitsh.spm");
	for(int i = 0 ; i < m_dynamicHitMovie.Count() ; i++ )
		m_dynamicHitMovie.Time(i) = (i+1)/8.;
	m_chargeBallMovie.Load("chrgball.spm");
	for(int i = 0 ; i < m_chargeBallMovie.Count() ; i++ )
		m_chargeBallMovie.Time(i) = (i+1)*fChargeTime/m_chargeBallMovie.Count();
	m_hiChargeBallMovie.Load("hichball.spm");
	for(int i = 0 ; i < m_hiChargeBallMovie.Count() ; i++ )
		m_hiChargeBallMovie.Time(i) = (i+1)*fHiChargeTime/m_hiChargeBallMovie.Count();
	m_nColors[CLR_GREENLASER] = CPaletteTranslator::Match(174,252,3);
	m_nColors[CLR_REDLASER] = CPaletteTranslator::Match(253,142,184);
	delete m_pRocketRef;
	delete m_pRocketBase;
	m_pRocketRef = NULL;
	m_pRocketBase = NULL;
	m_pRocketBase = new CViewObjectModel();
	if(!m_pRocketBase) return;
	CTaggedFile	f(TRUE);
	f.Open("rocket.vbs");
	f.Descend("REM!",TRUE,0);
	f.Ascend();
	m_pRocketBase->Read(f,TRUE);
	m_pRocketBase->Split(TRUE);
	m_pRocketRef = new CViewObjectRef(m_pRocketBase);
}
// ------------------------------------------------------------------------
void CShot::CleanUpMovie()
{
	m_staticHitMovie.CleanUp();
	m_dynamicHitMovie.CleanUp();
	m_chargeBallMovie.CleanUp();
	m_hiChargeBallMovie.CleanUp();
	delete m_pRocketRef;
	delete m_pRocketBase;
	m_pRocketRef = NULL;
	m_pRocketBase = NULL;
}
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
CShot *CShotSequence::NewShot()
{
	int  nLast = (m_nLast+1)%m_nMaxShots;
	if( m_nFirst == nLast )	{
	  ASSERT(0);
	  return NULL;
	}
	CShot *pShot = &m_pShots[m_nLast];
	m_nLast = nLast;
	return	pShot;
}
// ------------------------------------------------------------------------
void CShotSequence::Step(double fTime)
{
	for( int  i = m_nFirst ; i != m_nLast ; i = (i+1)%m_nMaxShots ) {
		if( !m_pShots[i].Step(fTime) && m_nFirst == i )
			m_nFirst = (m_nFirst+1)%m_nMaxShots;
	}
}
// ------------------------------------------------------------------------
void	CShotSequence::InitDraw(CViewDynamicList &list)
{
	for( int  i = m_nFirst ; i != m_nLast ; i = (i+1)%m_nMaxShots )
	if( m_pShots[i].m_nStatus != CShot::S_OFF ) {
		m_pShots[i].InitDraw();
		if(!m_pShots[i].m_pBangDynamic) list.Load(&m_pShots[i]);
	}
}
// ------------------------------------------------------------------------
void	CShotSequence::ReinitDraw()
{
	for( int  i = m_nFirst ; i != m_nLast ; i = (i+1)%m_nMaxShots ) {
		if( m_pShots[i].m_nStatus != CShot::S_OFF ) {
			m_pShots[i].InitDraw();
			if(!m_pShots[i].m_pBangDynamic) m_pShots[i].Repromote();
		}
	}
}
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
double CShooter::Distance(double fRelSpeed) const
{
//	CMissionConfig::SShootDef	&def = MISSIONCONFIG(m_shooter[m_eType]);
//	return (def.fSpeed+fRelSpeed)*def.fLifeTime;
	(void)fRelSpeed;
	return 0;
}

void CShooter::Set(CShot::TYPE eType,CShotSequence *pSeq,CMovingObject *pOwner)
{
	m_eType = eType;
	double fOffset = 1; //MISSIONCONFIG(m_shooter[eType].fOffset);
	Set(CFVector3(-fOffset,-0.15,0),CFVector3(fOffset,-0.15,0));
	/*switch(eType) {
		case CShot::T_LASER:	Set(CFVector3(-1,-0.15,0),CFVector3(1,-0.15,0)); break;
		case CShot::T_CHARGEBALL:	Set(CFVector3(-1,-0.15,0),CFVector3(1,-0.15,0)); break;
		case CShot::T_BULLET:	Set(CFVector3(-1,-0.15,0),CFVector3(1,-0.15,0)); break;
		case CShot::T_HICHARGEBALL:	Set(CFVector3(-1,-0.15,0),CFVector3(1,-0.15,0)); break;
		case CShot::T_ROCKET:	Set(CFVector3(-1,-0.15,0),CFVector3(1,-0.15,0)); break;
	}*/
	m_pSeq = pSeq; m_pOwner = pOwner;
/*	if( pOwner->IsMain() ) {
		switch(eType) {
			case CShot::T_LASER:	m_eShootDIGI = CMusic::DIGI_MYLASERSHOOT; break;
			case CShot::T_CHARGEBALL:	m_eShootDIGI = CMusic::DIGI_MYBALLSHOOT; break;
			case CShot::T_BULLET:	m_eShootDIGI = CMusic::DIGI_MYBULLETSHOOT; break;
			case CShot::T_HICHARGEBALL:	m_eShootDIGI = CMusic::DIGI_MYHIBALLSHOOT; break;
			case CShot::T_ROCKET:	m_eShootDIGI = CMusic::DIGI_MYROCKETSHOOT; break;
		}
	}
	else {
		switch(eType) {
			case CShot::T_LASER:	m_eShootDIGI = CMusic::DIGI_FOELASERSHOOT; break;
			case CShot::T_CHARGEBALL:	m_eShootDIGI = CMusic::DIGI_FOEBALLSHOOT; break;
			case CShot::T_BULLET:	m_eShootDIGI = CMusic::DIGI_FOEBULLETSHOOT; break;
			case CShot::T_HICHARGEBALL:	m_eShootDIGI = CMusic::DIGI_FOEHIBALLSHOOT; break;
			case CShot::T_ROCKET:	m_eShootDIGI = CMusic::DIGI_FOEROCKETSHOOT; break;
		}
	}*/
}
// ------------------------------------------------------------------------
void CShooter::Shoot(TCCFMatrix3x4 &dir, TCCFVector3 &speed )
{
	if( m_nCharge <= 0 || m_fRechargeTime >= 0 ) return;
	CShot *pShot = m_pSeq->NewShot();
	if(!pShot) { /*WRITELOG("NOSHOT!!!\n");*/ return; }
	m_fRechargeTime = m_fShotRateTime;
	m_nGun ^= 1;
	//WRITELOG1("Shooter m_nGun=%d\n",m_nGun);
	CFVector3	start, normalZ(dir.m[0][2],dir.m[1][2],dir.m[2][2]),
												&offset = m_offset[m_nGun];
	dir.Mult(offset,start);
	pShot->Init(m_eType,start,speed,normalZ,m_pOwner);
	//if( CMusic::IsActiveFX() )
	//	CMusic::SetDIGI(m_eShootDIGI,CViewObject::m_viewPointDirMx*start);
	m_nCharge--;
}
// ------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
void CShooter::SaveGame(CTaggedFile &f)
{
	f.Descend("SHOO",TRUE,0);
		f.WriteInt(m_nCharge);
	f.Ascend();
}
#endif
// ------------------------------------------------------------------------
void CShooter::LoadGame(CTaggedFile &f)
{
	f.Descend("SHOO",TRUE,0);
		f.ReadInt(m_nCharge);
		m_fRechargeTime = -1;
		m_nGun = 0;
	f.Ascend();
}
// ------------------------------------------------------------------------
/* */

