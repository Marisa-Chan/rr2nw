            // ================================================================
            // FUNCTIONAL AREA:    MicroKernel
            // NAME:               KR_Timer.hpp
            // AUTHORS:            MKrylov
            // DESIGN REFERENCE:
            // MODIFICATION:       23 Feb 97 - creation
            // ================================================================
#ifndef _KR_TIMER_HPP_
#define _KR_TIMER_HPP_

// =================================================================== INCLUDES
// =================================================================== SYNOPSIS
// ================================================================== CONSTANTS
// ====================================================================== TYPES
                                                                     // STRUCTS
                                                                     //   UNION
                                                                     //    ENUM
// ================================================================= PROTOTYPES

// ============================================================================
class KR_RealTimer
{
    public:
       virtual double GetTime() = 0;
       virtual double GetTimeDiff(double timeStamp) = 0;
	   virtual double ConvertSysTime(unsigned long tick) = 0; 
	   virtual void   Wait(double interval) = 0;
	virtual void   SetCurTime(double interval) = 0;
};
// ============================================================================
#endif //_KR_TIMER_HPP_
