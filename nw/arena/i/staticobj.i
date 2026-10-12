#ifndef __STATIC_OBJ__INCLUDED
#define __STATIC_OBJ__INCLUDED

enum
{
  IStaticObjIID = 11
};

class CViewBaseModifier;
class CViewBaseModifier0;
class CViewFigure;

class IStaticObj
{
public:
   double    phase;
   double    speed;
   CFVector3 axis0, axis1, axis2, axis3, axis4, axis5, axis6, axis7;
   CViewBaseModifier0 *vbmi0, *vbmi1, *vbmi2, *vbmi3, *vbmi4, *vbmi5, *vbmi6, *vbmi7;
   CViewBaseModifier  *vbm0, *vbm1, *vbm2;
   CViewFigure *fig0;
};

#endif
