#ifndef	__SCENE_CPP__
#include"_scene.h"
#endif


// ------------------------------------------------------------------------
bool	ProcessBonus(SBonusDef *p1,SBonusDef *p2)
{
	if (!p1) {
		if (!p2)
			return FALSE;
		if (!p2->pOwner || (p2->pOwner->m_nExistFlags & CMovingObject::EF_BONUS))
			return TRUE;
		return FALSE;
	} else if (!p2) {
		if (!p1->pOwner || (p1->pOwner->m_nExistFlags & CMovingObject::EF_BONUS))
			return TRUE;
		return FALSE;
	}
//	if(!p1||!p2) return FALSE;
	if( p1->nBonus < 0 ) {
		if( p2->nBonus >= 0 ) {
			if (p2->pOwner == NULL)
				return TRUE;
			if (p2->pOwner->CollectBonus(*p1))	{
				/*p2->nBonus -= p1->nBonus;*/ p1->nBonus = 0;
				//CMusic::SetDIGI(CMusic::DIGI_GETBONUS);
			}
		}
		return TRUE;
	} else
	if( p2->nBonus < 0 ) {
		if( p1->pOwner == NULL)
			return TRUE;
		if (p1->pOwner->CollectBonus(*p2)) {
			/*p1->nBonus -= p2->nBonus;*/ p2->nBonus = 0;
			//CMusic::SetDIGI(CMusic::DIGI_GETBONUS);
		}
		return TRUE;
	}
	return FALSE;
}

// ------------------------------------------------------------------------
CPhasedMovie	SBonusDef::m_bonusMovies[SBonusDef::T_SUP];
double	SBonusDef::m_afBonusScale[SBonusDef::T_SUP] = {
									0.04, 0.04, 0.04, 0.04, 0.04, 0.04 };

void SBonusDef::LoadMovies()
{
	static char *szBonusMovies[T_SUP] = {
		"laser.spm", "plasma.spm", "machine.spm", "splasma.spm",
		"rocket.spm", "life.spm"
	};
	for(int nBonus = 0 ; nBonus < T_SUP ; nBonus++ ) {
		m_bonusMovies[nBonus].Load(szBonusMovies[nBonus]);
		for( int i = 0 ; i < m_bonusMovies[nBonus].Count() ; i++ )
			m_bonusMovies[nBonus].Time(i) = (i+1)/4.;
	}
}
// ------------------------------------------------------------------------
void SBonusDef::CleanUpMovies()
{
	for( int i = 0 ; i < SBonusDef::T_SUP ; i++ )
		m_bonusMovies[i].CleanUp();
}
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
inline bool CBonusObject::Read(CTaggedFile &f)
{
	int nType;
	f.Descend("BOBJ",TRUE,0);
	f.ReadInt(nType);
	f.ReadInt(m_nCharge);
	m_bump.start.Read(f);
	f.Ascend();
	m_fDrawTime = 0;
	m_fTimeOut = -1;
	m_bonus.eType = (SBonusDef::TYPE)nType;
	m_bonus.nBonus = -m_nCharge;
	m_bonus.pOwner = NULL;
	m_bump.pBonus = &m_bonus;
	m_bump.vel = m_bump.vel1 = CFVector3(0,0,0);
	m_bump.fRadius = 2.0; m_bump.fMass = 1;
	return f.IsOK();
}
// ------------------------------------------------------------------------
inline void CBonusObject::Restart()
{
	m_bonus.nBonus = -m_nCharge;
	m_fTimeOut = -1;
}
// ------------------------------------------------------------------------
#if 0
inline void CBonusObject::Step(double fTime)
{
	if( /*CViewCity::IsNetwork() &&*/ m_bonus.nBonus >= 0 && m_fTimeOut < 0 ) {
		m_fTimeOut = 60;
	}
	if( m_fTimeOut >= 0 && (m_fTimeOut -= fTime) < 0 ) {
		m_bonus.nBonus = -m_nCharge;
	}
}
#endif
// ------------------------------------------------------------------------
void CBonusObject::Draw()
{
	ASSERT(0);
	//if( Abs(m_bump.start-CViewCity::CurrCity()->Vessel().Pos()) <
	//										CViewFigureSide::HazeMax() )
	//	m_bonus.Draw(m_bump.start,m_fDrawTime);
}
// ------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
inline void CBonusObject::SaveGame(CTaggedFile &f)
{
	f.Descend("BOBJ",TRUE,0);
		f.WriteInt(m_bonus.nBonus);
		f.WriteDouble(m_fTimeOut);
	f.Ascend();
}
#endif
// ------------------------------------------------------------------------
inline void CBonusObject::LoadGame(CTaggedFile &f)
{
	f.Descend("BOBJ",TRUE,0);
		f.ReadInt(m_bonus.nBonus);
		f.ReadDouble(m_fTimeOut);
	f.Ascend();
}
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
bool CBonusSet::Read(CTaggedFile &f)
{
	delete [] m_pBonuses;
	m_pBonuses = NULL;
	m_nBonuses = 0;
	f.Descend("BSET",FALSE,0);
		f.Descend("BSEH",TRUE,0);
			f.ReadInt(m_nBonuses);
		f.Ascend();
		m_pBonuses = new CBonusObject[m_nBonuses];
		for( int i = 0 ; i < m_nBonuses ; i++ )
			m_pBonuses[i].Read(f);
	f.Ascend();
	return f.IsOK();
}
// ------------------------------------------------------------------------
void CBonusSet::Skip(CTaggedFile &f)
{
	f.Descend("BSET",FALSE,0);
	f.Ascend();
}
// ------------------------------------------------------------------------
void CBonusSet::Restart()
{
	for( int i = 0 ; i < m_nBonuses ; i++ )	m_pBonuses[i].Restart();
}
// ------------------------------------------------------------------------
const double fBonusTimeOut = 60;
void CBonusSet::TakeBonus(int n)
{
	CBonusObject	&bobj = m_pBonuses[n];
	bobj.m_bonus.nBonus = 0;
	bobj.m_fTimeOut = fBonusTimeOut;
}
// ------------------------------------------------------------------------
void CBonusSet::InitDraw(double fTime,CViewDynamicList &list)
{
	(void)fTime; (void)list;
	ASSERT(0);
	#if 0
	CFVector3	pos0;
	bool	bNet = CViewCity::IsNetwork();
	if( !bNet ) pos0 = CViewCity::CurrCity()->Vessel().Pos();
	if( bNet )	{
		for( int i = 0 ; i < m_nBonuses ; i++ ) {
			//m_pBonuses[i].Step(fTime);
			CBonusObject	&bobj = m_pBonuses[i];
			if( bobj.m_bonus.nBonus >= 0 && bobj.m_fTimeOut < 0 ) {
				bobj.m_fTimeOut = fBonusTimeOut;
				CViewCity::NetBroadcastTakeBonus(i);
			}
			if( bobj.m_fTimeOut >= 0 && (bobj.m_fTimeOut -= fTime) < 0 ) {
				bobj.m_bonus.nBonus = -bobj.m_nCharge;
			}
		}
	}
	for( int i = 0 ; i < m_nBonuses ; i++ ) {
		m_pBonuses[i].m_pPromoteNode = NULL;
		if( m_pBonuses[i].m_bonus.nBonus < 0 &&
				(bNet || Abs(pos0-m_pBonuses[i].m_bump.start) < 300) ) {
			m_pBonuses[i].m_fDrawTime = m_pBonuses[i].m_bonus.WrapTime(
									m_pBonuses[i].m_fDrawTime+fTime);
			m_pBonuses[i].m_dynBase = m_pBonuses[i].m_dynBase1 =
				m_pBonuses[i].m_bump.start;
			list.Load(&m_pBonuses[i]);
		}
	}
	#endif
}
// ------------------------------------------------------------------------
void CBonusSet::ReinitDraw()
{
	for( int i = 0 ; i < m_nBonuses ; i++ )
		if( m_pBonuses[i].m_pPromoteNode ) {
			m_pBonuses[i].m_dynBase = m_pBonuses[i].m_dynBase1 =
				m_pBonuses[i].m_bump.start;
			m_pBonuses[i].m_pPromoteNode->AppendDyn(&m_pBonuses[i]);
		}
}
// ------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
void CBonusSet::SaveGame(CTaggedFile &f)
{
	f.Descend("BSET",FALSE,0);
		f.Descend("BSEH",TRUE,0);
			f.WriteInt(m_nBonuses);
		f.Ascend();
		for( int i = 0 ; i < m_nBonuses ; i++ )
			m_pBonuses[i].SaveGame(f);
	f.Ascend();
}
#endif
// ------------------------------------------------------------------------
void CBonusSet::LoadGame(CTaggedFile &f)
{
	f.Descend("BSET",FALSE,0);
		f.Descend("BSEH",TRUE,0);
			int nBonuses;
			f.ReadInt(nBonuses);
			if( nBonuses != m_nBonuses )	f.SetOK(FALSE);
		f.Ascend();
		for( int i = 0 ; i < m_nBonuses ; i++ )
			m_pBonuses[i].LoadGame(f);
	f.Ascend();
}
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
CPhasedMovie	CMovingObject::m_boomMovie;
// ------------------------------------------------------------------------
void CMovingObject::Init(double fRadius)
{
	CViewSphericDynamic::Init();
	m_bump.fRadius = m_fRadius = fRadius; m_bump.fMass = 1.;
	//m_bHighlightShots = FALSE; m_bIsMain = FALSE;
	m_fShield = 1.; m_nExistFlags = 0;
	m_bangShots.Clear();
}
// ------------------------------------------------------------------------
void CMovingObject::LoadMovie()
{
	m_boomMovie.Load("boom.spm");
	for(int i = 0 ; i < m_boomMovie.Count() ; i++ )
		m_boomMovie.Time(i) = (i+1)/10.;
}
// ------------------------------------------------------------------------
void CMovingObject::CleanUpMovie()
{
	m_boomMovie.CleanUp();
}
// ------------------------------------------------------------------------
//bool ValidDynamic(CViewDynamic *pd);
void CMovingObject::Draw()
{
	//ASSERT(ValidDynamic(this));
	if( m_bangShots.First() ) {
		CFVector3	viewDir(
				m_dynBase.x - CViewObject::m_viewPointInvMx.m[0][3],
				m_dynBase.y - CViewObject::m_viewPointInvMx.m[1][3],
				m_dynBase.z - CViewObject::m_viewPointInvMx.m[2][3] );
		for( CShot  *ps = (CShot*)m_bangShots.First(), *ps1 ; ps != NULL ;
							ps = ps1 ) {
			ps1 = (CShot*)ps->Next();
			if( !ps->IsOn() ) { /*m_bangShots.Remove(ps);*/ continue; }
			if( ps->m_bangOffset*viewDir > 0 ) {
				ps->m_dynBase = m_dynBase+ps->m_bangOffset;
				ps->Draw();
			}
		}
		if( ! m_bIsMain ) {
			if( m_nExistFlags & EF_DYNAMIC )	{
				//WRITELOG1("EF_DYNAMIC: %08X\n",this);
				_Draw();
			}
			if( m_nExistFlags & EF_BONUS )
				m_bump.pBonus->Draw(m_dynBase,m_fLifeTime);
			//	m_bonusMovie.Draw(m_fLifeTime,m_dynBase,0.04);
			if( m_nExistFlags & EF_BOOM )	m_boomMovie.Draw(
				m_boomMovie.Time(m_boomMovie.Count()-1)-m_fLifeTime,
											m_dynBase, m_fRadius/40 );
		}
		for( CShot  *ps = (CShot*) m_bangShots.First() ; ps != NULL ;
												ps = (CShot*) ps->Next() ) {
			if( ps->m_bangOffset*viewDir <= 0 && ps->IsOn()) {   //!!!!!!!!
				ps->m_dynBase = m_dynBase+ps->m_bangOffset;
				ps->Draw();
			}
		}
	} else {
		if( !m_bIsMain ) {
			if( m_nExistFlags & EF_DYNAMIC )	{
				//WRITELOG1("EF_DYNAMIC: %08X\n",this);
				_Draw();
			}
			if( m_nExistFlags & EF_BONUS )
					m_bump.pBonus->Draw(m_dynBase,m_fLifeTime);
			//if( m_nExistFlags & EF_BONUS )
			//	m_bonusMovie.Draw(m_fLifeTime,m_dynBase,0.04);
			if( m_nExistFlags & EF_BOOM )	m_boomMovie.Draw(
				m_boomMovie.Time(m_boomMovie.Count()-1)-m_fLifeTime,
												m_dynBase, m_fRadius/40 );
		}
	}
}
// ------------------------------------------------------------------------
void CMovingObject::Die(double fTime)
{
	if( m_nExistFlags & EF_VIEWOBJ ) {
		//WRITELOG1("Dying %08X\n",this);
		m_nExistFlags &= ~EF_VIEWOBJ;
		m_nExistFlags |= EF_BOOM;
		m_eBumpability = BY_NONE;
		m_fLifeTime = m_boomMovie.Time(m_boomMovie.Count()-1);
		for( CShot *ps = (CShot *) m_bangShots.First() ; ps != NULL ;
												ps = (CShot *) ps->Next() ) {
		   ps->SetOff();
		}
		m_bangShots.Clear();
	}
	else {
		if( m_nExistFlags & EF_BOOM ) {
			if( (m_fLifeTime -= fTime) < 0 ) {
				m_nExistFlags &= ~EF_BOOM;
				m_bump.pBonus->nBonus = 0;
				SetUpBonusCharge();
				if( m_bump.pBonus->nBonus < 0 ) {
					m_nExistFlags |= EF_BONUS;
					m_fRadius = m_bump.fRadius = Min(2.0,m_fRadius); //0.5;
					m_fLifeTime = 0;
				} else m_nExistFlags &= ~EF_DYNAMIC;
			}
		} else
		if( m_nExistFlags & EF_BONUS ) {
			if( m_bump.pBonus->nBonus >= 0 )
				m_nExistFlags &= ~(EF_BONUS|EF_DYNAMIC); else
			m_fLifeTime = m_bump.pBonus->WrapTime(m_fLifeTime+fTime);
		}
	}
}
// ------------------------------------------------------------------------
void CMovingObject::Bang(CShot *ps)
{
	ASSERT(!m_bangShots.Contains(ps));
	m_bangShots.Load(ps);
	/*if( CViewCity::IsNetwork() ) {
		if( ps->Source()->IsMain() ) {
			//WRITELOG1("Broadcasting bang: %lg\n",ps->Energy());
			//CViewCity::NetBroadcastBang(this,ps->Energy()*10.0);
		}
	}
	else*/ {
		//if (CViewCity::IsNetwork()) m_fShield -= ps->Energy()*10.0;
		//else {
		if (ps->m_pSource->m_bHighlightShots || m_bHighlightShots)
		   m_fShield -= ps->Energy();
		else {
		if (m_bump.fMass < 1e5)
		  	m_fShield -= ps->Energy()*0.3;

		}
		//}
	}
}
// ------------------------------------------------------------------------
#ifndef	_BIG_ENDIAN
void CMovingObject::SaveGame(CTaggedFile &f)
{
	f.Descend("MOVO",TRUE,0);
		f.WriteDouble(m_fShield);
		f.WriteInt(m_nExistFlags);
		f.WriteDouble(m_fRadius);
	f.Ascend();
}
#endif
void CMovingObject::LoadGame(CTaggedFile &f)
{
	f.Descend("MOVO",TRUE,0);
		f.ReadDouble(m_fShield);
		f.ReadInt(m_nExistFlags);
		f.SetRTEFatal(FALSE);
		f.ReadDouble(m_fRadius);
		f.SetOK(TRUE);
		if( !IsAlive() )	m_nExistFlags = 0;
		m_bangShots.Clear();
	f.Ascend();
}
// ------------------------------------------------------------------------


