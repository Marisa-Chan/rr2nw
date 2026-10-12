            // ================================================================
            // FUNCTIONAL AREA:    MicroKernel
            // NAME:               SimulationContext.h
            // AUTHORS:            MKrylov
            // DESIGN REFERENCE:
            // MODIFICATION:       01 March 97 - creation
            // ================================================================
#ifndef _SC_SimulationContext_H_
#define _SC_SimulationContext_H_

#include <stdlib.h>
#include "kernel/h/object.h"

const int  MAX_SYMBOLIC_LENGHT = 127;


class PIN_SaveFile;

// ====================================================================== TYPES

typedef struct
{
   char         symbolic[MAX_SYMBOLIC_LENGHT + 1];
   KR_Object   *object;
   int          next;
   // publisher data
   int          resendEventQnty;
   int          resendEventList;
}
   KR_ObjectListElem,
  *KR_ObjectList;


typedef int SC_Flag;


typedef struct {
		
	  int             m_started;
	  long            m_currentObjectId;
  
	  //KR_TimeDelta                  m_timeRatio;
      //KR_TimeDelta                  m_timeStart;     
      //KR_TimeDelta                  m_lastTime;
} SimulationContextData ;

// ============================================================================
class SimulationContext : public SimulationContextData
{
   friend	class Publisher;
   
// ============================================================= Don't look !!!
   private:
      

      KR_EventID      popEvent(KR_TimeDelta timeStamp);
      int             freeEvent(KR_EventID eventID);

      int                           m_maxEventQnty;
      KR_Event                     *m_eventPool;
      struct KR_EventListElemName  *m_eventIndex;

      KR_EventID                    m_freeEventList,
                                    m_eventQueue;

   protected:
      int                           m_maxObjectQnty;
      KR_ObjectListElem            *m_objectIndex;

	  int                           m_freeObjectList,
                                    m_objectQueue;

   private:

      enum{
         CACHE_POW  = 8,
         CACHE_SIZE = (1<<CACHE_POW)
      };
      
      KR_ObjectListElem            *m_cacheObject[CACHE_SIZE];
   public:
      

      SimulationContext(
                        /*  KR_TimeDelta initModelTime,
                          KR_TimeDelta ratio,*/
                          int          maxEventQnty,
                          int          maxObjectQnty
                       );
      ~SimulationContext();

      void         addEvent       (const KR_Event &event);
      void         sendEventNow   ( KR_Event &event );
      void        *queryInterface ( const KR_ObjectID &ID, int interfaceNum );
      int          removeEvent    (
                                    KR_EventLabel label,
                                    KR_ObjectID   source
                                   );
      KR_ObjectID  addObject       (
                                    const char name[],
                                    KR_Object *object
                                   );

	  KR_ObjectID  addObject	   (
                                    const char  name[],
                                    KR_Object  *object,
                                    KR_ObjectID id
                                   );

      void         removeObject			(const KR_ObjectID &objectID );
      int          isObjectPollFull(); // inline
      int          poll            (KR_TimeDelta  time);

      const char  *searchObject    (const KR_ObjectID  &objectID);
      KR_ObjectID  searchObject    (const char name[]);
      int          isExist         ( const char  name[] );
      int          isExist         ( KR_ObjectID id );

      void         start           (KR_TimeDelta timeStamp);

      static  int  cacheVal        ( const char *str );
      long         uniqueID()      { return m_currentObjectId +=1 ; }

      int          rnd_i();
      int          rnd_i( int max );
      int          rnd_i( int min, int max );
      double       rnd_f();
      double       rnd_f(double max);
      double       rnd_f( double min, double max );


  	  bool		   dump(PIN_SaveFile & sf);
	  bool		   load(PIN_SaveFile & sf);


	  void			clearEvents();
	  void			clearObjects();	  
	  void			initObjects();

};


// ============================================================================

#define END_LIST (-1)

inline int  SimulationContext::isObjectPollFull()
 {
    return m_freeObjectList == END_LIST;
 }
inline int     SimulationContext::rnd_i()         { return rand(); }
inline int     SimulationContext::rnd_i( int max ){ if(max==0)return 0;return rand()%max; }
inline int     SimulationContext::rnd_i( int min, int max ) {  return min+rnd_i(max-min); }
inline double  SimulationContext::rnd_f()         { return ((double)(rand()))/RAND_MAX; }
inline double  SimulationContext::rnd_f(double max) { if(max==0)return 0; return rnd_f()*max; }
inline double  SimulationContext::rnd_f( double min, double max ) { return min+rnd_f(max-min); }

#endif

/* _SC_SimulationContext_H_ */