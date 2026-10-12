#ifndef __ROUTE_I__INCLUDED
#define __ROUTE_I__INCLUDED

#ifndef __MATHLIB_H__
#include "mathlib.h"
#endif

#include "kernel/h/krtypes.h"

enum
{
    IRouteObjectIID = 7
};

class IRouteObject{
public:
   virtual void      Load(const char * fName) = 0;
	virtual int       GetNodeCnt() const = 0;
	virtual int       GetLen() const = 0;
	virtual CFVector3 GetNode(int n) const = 0;  
	virtual CFVector3 GetPos(double part) const = 0;
        virtual KR_ObjectID       getID()             = 0;

        virtual void      AddRef() = 0;
        virtual void      DelRef() = 0;
};

#endif

