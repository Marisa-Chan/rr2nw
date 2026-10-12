#ifndef __DYNOBJ_I__INCLUDED
#define __DYNOBJ_I__INCLUDED

#ifndef __MATHLIB_H__
#include "mathlib.h"
#endif

enum
{
    IDynamicObjectIID = 1
};

class IDynamicObject
{
public:
    virtual CFVector3  getPos      () = 0;
    virtual double     getHAngle   () = 0;
    virtual CFVector3  getUpVector () = 0;
    virtual CFVector3  getCenter   () = 0; // Относительно 0 объекта
    virtual double     getRadius   () = 0; // Относительно центра
    virtual double     getRadius0  () = 0; // Относительно 0 объекта
    virtual CFVector3  getMoveDir  () = 0; // Направление движения
    virtual double     getMoveSpeed() = 0; // Скорость
    virtual void       getMatrix   ( CFMatrix3x4 &m) = 0;

    virtual double     getMass	   () = 0;	// Масса
    virtual TCCFMatrix3x4 &GetDir  () = 0;
    virtual void SetDir(TCSFMatrix3x4 &dir) = 0;

};

#endif

