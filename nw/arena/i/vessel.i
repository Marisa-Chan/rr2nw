#ifndef __VESSEL_I__INCLUDED
#define __VESSEL_I__INCLUDED

enum
{
  IVesselIID = 8
};

class SBumpDef;
class ct_Subject;

class IVessel
{
 public:
 #ifndef __VESSEL_IGNORE_MASTER
    virtual void SetMaster ( ct_Subject *master, bool (*)(ct_Subject*,SBumpDef&) ) = 0;
 #endif
    virtual void         Restart          ()                        = 0;
    virtual void         BeginPreStep     ()                        = 0;
    virtual void         AccumPreStep     (double fDeltaT)          = 0;
    virtual void         QuickAccumPreStep(double fDeltaT)          = 0;
    virtual void         PreStep          (double fDeltaT)          = 0;
    virtual void         ApplyStep        ()                        = 0;

    virtual void         Throttle         (double fDelta)           = 0;
    virtual void         Raise            (double fDeltaUp)         = 0;
    virtual void         Rotate           (double fDeltaRight)      = 0;
    virtual void         StrafeVert       (double fDelta)           = 0;
    virtual void         StrafeHorz       (double fDelta)           = 0;
    virtual void         SetStrafe        (bool bStrafe)            = 0;
    virtual void         Incline          (double fDeltaRight)      = 0;
    virtual void         SetForceage      (bool b)                  = 0;
    virtual void         Stop             ()                        = 0;
    virtual void	     Jump(double fDelta) = 0;

    virtual	void		 MoveMouse(double dx,double dy) = 0;
    virtual void         Joystick(double x,double y) = 0;

    virtual void         SetPos           (TCCFVector3 &v)          = 0;
    virtual TCCFVector3  Pos              ()                        = 0;
    virtual TCCFVector3  Speed            ()                        = 0;
    virtual CFMatrix3x4 &GetDir           ()                        = 0;
    virtual void SetDir(TCSFMatrix3x4 &dir) = 0;

    virtual TCvoid *SaveGame() = 0;
    virtual bool LoadGame(TCvoid *pSavedGame) = 0;

    virtual void         NextFrame        () = 0;
    virtual void SetBounds(TCdouble *p=NULL) = 0; // xmin,xmax,ymin,ymax,zmin,zmax
											     // do not delete. only pointer is stored
};

#endif

