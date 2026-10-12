#ifndef __VH_VESSEL_H__INCLUDED
#define __VH_VESSEL_H__INCLUDED


#include "storage/h/subject.h"
#include "i/vessel.i"

class	Vessel :
        public CViewSphericDynamic,
        public IVessel
{
protected:
        ct_Subject     *m_master;
	CFMatrix3x4	m_dir;
//	double		m_fAngleX, m_fAngleY, m_fAngleZ;
	CFVector3	m_speed, m_offset;
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

	CViewObjectModel *m_pBase;
	CViewObjectRef	 *m_pObj;

	void	Init();
public:
	//enum { VD_FORWARD, VD_BACK, VD_LEFT, VD_RIGHT };
	Vessel();
	~Vessel() { delete m_pObj; delete m_pBase; }
	void	Restart();
	void	BeginPreStep() { m_fStepTime = 0; m_offset = CFVector3(0,0,0); }
	void	AccumPreStep(double fDeltaT);
	void	QuickAccumPreStep(double fDeltaT);
	void	PreStep(double fDeltaT);
	void	ApplyStep();

	void	Throttle (double fDelta);
	void	Raise    (double fDeltaUp);
	void	Rotate   (double fDeltaRight);
	void	SetStrafe(bool bStrafe);
	void	Incline  (double fDeltaRight);
	void	SetForceage(bool b) { m_bForceage = MAKEBOOL(b); }
	void	Stop();


	void	SetPos(TCCFVector3 &v);
	TCCFVector3  Pos();
	TCCFVector3  Speed()           { return m_speed; }
	CFMatrix3x4 &GetDir()          { return m_dir; }


        virtual void    Draw();
        void            NextFrame        (double fDeltaT,  double radius );
        void            SetMaster        ( ct_Subject *master );
};

#endif
