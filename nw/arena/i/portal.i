#ifndef __PORTAL_I__INCLUDED
#define __PORTAL_I__INCLUDED

enum
{
   IPortalIID = 17
};

class IPortal
{
 public:
     virtual CFVector3 portalGetCoord()                       = 0;
     virtual int       portalGetSlotCnt()                     = 0;
     virtual int       portalGetOccupiedSlot()                = 0;
     virtual void      portalAddArtefact( KR_ObjectID artID ) = 0;
     virtual void      portalInit(CFVector3 pos, int sc)      = 0;
     virtual void      portalSetPortalPoint(CFVector3 pos)    = 0;
};

#endif
