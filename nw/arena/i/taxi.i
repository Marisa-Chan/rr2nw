#ifndef __TAXI_I__INCLUDED
#define __TAXI_I__INCLUDED

#include "kernel/h/object.h"

enum
{
   ITaxiIID = 10
};

class ITaxi
{
 public:
      virtual KR_ObjectID getAttributeForVehicle() = 0;
      virtual CFVector3   taxiPos()                = 0;
      virtual void        taxiSetBulletCnt(int cnt)= 0;
      virtual int         taxiGetBulletCnt()       = 0;
};

#endif
