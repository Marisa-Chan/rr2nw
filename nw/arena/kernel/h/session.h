            // ================================================================
            // FUNCTIONAL AREA:    MicroKernel
            // NAME:               Session.hpp
            // AUTHORS:            MKrylov
            // DESIGN REFERENCE:
            // MODIFICATION:       23 Feb 97 - creation
            // ================================================================
#ifndef _SESSION_HPP_
#define _SESSION_HPP_

#include "kernel/h/timer.h"
#include "kernel/h/context.h"

class KR_Observer;
class KR_Hardware;

typedef struct ObserverElemName
   {
       KR_Observer             *observer;
       SimulationContext       *context;
       struct ObserverElemName *next;
   }
   ObserverElem;

// ============================================================================
class Session
 {
    public:
      Session()
      {
          m_contextList  = NULL;
          m_observerList = NULL;
      }
            Session(
                      KR_RealTimer *realTimer,
                      KR_Hardware  *hardware
                   );
           ~Session();

       void Add   (SimulationContext *context);
       int  Remove(SimulationContext *context);

       void AddObserver   (KR_Observer *observer);
       int  RemoveObserver(KR_Observer *observer);

       int  poll();


       static KR_RealTimer *m_realTimer;
       static KR_Hardware  *m_hardware;
       static double        m_moment;
       static double        m_viewTime;
       static double        m_frameSec;

       struct SimulationContextElem
       {
          SimulationContext     *context;
          SimulationContextElem *next;
       }
          *m_contextList;

       struct ObserverElemName  *m_observerList;

 };
#endif
/* _SESSION_HPP_ */