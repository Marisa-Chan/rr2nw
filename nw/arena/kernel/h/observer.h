            // ================================================================
            // FUNCTIONAL AREA:   MicroKernel
            // NAME:              KR_Observer.h
            // AUTHORS:
            // DESIGN REFERENCE:
            // MODIFICATION:      23 Feb 97 - creation
            // ================================================================
#ifndef _KR_Observer_H_
#define _KR_Observer_H_
// =================================================================== SYNOPSIS
#include "kernel/h/active.h"

// ================================================================= PROTOTYPES
class SE_SyntheticEnvironment;
class CDC;

class KR_Observer : public KR_ActiveObject
{
    public:
       virtual void draw( CDC &gc ) = 0;
};
#endif //_KR_Observer_H_

