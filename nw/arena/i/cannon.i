#ifndef __CANNON_I__INCLUDED
#define __CANNON_I__INCLUDED

#ifndef __MATHLIB_H__
#include "mathlib.h"
#endif

enum
{
    ICannonIID = 3
};

class ICannon
{
public:
    virtual void       setLocalHAngle(double hAngle) = 0;
	virtual double     getLocalHAngle() = 0;
	virtual void       setLocalVAngle(double vAngle) = 0;
    virtual double     getLocalVAngle() = 0;
	virtual double     getHAngle     () = 0;
	virtual double     getVAngle     () = 0;
	virtual int        isFixed       () = 0;
  
    virtual void       rotate        ( 
	                                   double prevTimeStamp, 
									   double hAngle,
									   double vAngle
									 ) = 0;
    virtual void       rotateAndShoot(
	                                   double prevTimeStamp, 
									   double hAngle,
									   double vAngle,
									   int count,
									   int bulletAttrIndex
	                                 ) = 0;
};

#endif

