            // ================================================================
            // FUNCTIONAL AREA:   MicroKernel
            // NAME:              SimulationContext.cpp
            // AUTHORS:           MKrylov
            // DESIGN REFERENCE:
            // MODIFICATION:      23 Feb 97 - creation
            //                    09 Aug 97 - add class `Publisher` for
            //                                subscribed events
            //                    13 Aug 97 - fix bugs in event poping
            // ================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "kernel\h\context.h"
#include "kernel\h\echo.h"
#include "kernel\h\s_debug.h"
#include "kernel\h\session.h"
#include "storage\h\subject.h"
#include "h/olevel.h"


#define LAST_H__VIEW
#include "game.h"
#include "storage\h\savefile.h"
#include "h\super.h"
#include "hardware.h"



#include "obase\route\route.h"
#include "obase\fountain\fountain.h"
#include "vehicle.h"




#define S_WAKE_UP 0

typedef struct KR_EventListElemName
{
   KR_Event *event;
   int       next;
}
   KR_EventListElem,
  *KR_EventList;

// ================================================================ Constructor
SimulationContext::SimulationContext(
                                      /*KR_TimeDelta initModelTime,
                                      KR_TimeDelta ratio,*/
                                      int          maxEventQnty,
                                      int          maxObjectQnty
                                    )
 {
    s_ENTRY(SimulationContext)
    int i;


    m_started   = 0;
    //m_timeRatio = ratio;
    //m_timeStart = initModelTime;
    //m_lastTime  = m_timeStart;
    
	//m_currentObjectId = 0xFACEBEDA ^(  (long)(clock())  );
	
	m_currentObjectId = 0xFACEBEDA;
	
    // alloc main data
    m_maxEventQnty  = maxEventQnty;
    m_maxObjectQnty = maxObjectQnty;

    /*
     * Make object pool
     */
    m_objectIndex = new KR_ObjectListElem[m_maxObjectQnty];
    s_ASSERT(m_objectIndex!=NULL,"Memory too low(object)");

	initObjects();


    /*
     *  Make event pool
     */
    m_eventPool  = new KR_Event        [m_maxEventQnty];
    m_eventIndex = new KR_EventListElem[m_maxEventQnty];

	clearEvents();    

    for(i = 0; i < CACHE_SIZE; ++i )
         m_cacheObject[i] = NULL;
 }

// ================================================================= Destructor
SimulationContext::~SimulationContext()
 {
    delete [] m_eventPool;
    m_eventPool = NULL;

    delete [] m_eventIndex;
    m_eventIndex = NULL;
 }

// ============================================================================
void SimulationContext::sendEventNow( KR_Event  &event )
 {
    s_ASSERT( event.timeStamp >= 0.1, "SimulationContext::sendEventNow: Not initialize timeStamp" );
    if( is_ASSERT( event.destination.cachePos >= 0
             && event.destination.cachePos <= m_maxObjectQnty,
             "SimulationContext::sendEventNow. Range check error" ) )
        return;

    KR_Object *object = m_objectIndex[event.destination.cachePos].object;
    if( object!=0   )
    if( object->getObjectID() == event.destination )
    {
         if(  !object->receiveEvent(event)  )
              echo(
                    "Object %s do't recognize message [%i/\"%s\"] from %s",
                    searchObject(event.destination),
                    event.label,
                    s_FindLabelName(event.label),
                    searchObject(event.source)
                  );
    }
 }

// ============================================================================
void *SimulationContext::queryInterface(
                                        const KR_ObjectID &ID, 
                                        int           interfaceNum 
                                       )
 {
    if( is_ASSERT( ID.cachePos >= 0
             && ID.cachePos <= m_maxObjectQnty,
             "SimulationContext::queryInterface. Range check error" ) )
        return 0;

    KR_Object *object = m_objectIndex[ID.cachePos].object;

    if(  object!=0  )
    if(  object->getObjectID() == ID  )
    {
         return object->queryInterface( interfaceNum );
    }
    //else echo("SimulationContext::queryInterface. Unknown object");
    return 0;
 }

// ============================================================================
void SimulationContext::addEvent( const  KR_Event  &event )
 {
   s_ASSERT( event.timeStamp >= 0.1, "SimulationContext::addEvent: Not initialize timeStamp" );
   KR_EventID  inserted = -1;
//!!!!!!!FIXME   s_ASSERT( event.timeStamp > Session::m_moment-5.0 && event.timeStamp > 0.01 ,"SimulationContext::addEvent: Temporaly moshine" );
   s_ASSERT((int)this > 1000,"SimulationContext::addEvent");
   s_ASSERT( event.destination.cachePos>=0, "SimulationContext::addEvent" );

   if (m_freeEventList != END_LIST)
   {
      //
      // get free event block
      //
      inserted = m_freeEventList;

      KR_EventListElem *evtIndex = &m_eventIndex[inserted];
      m_freeEventList = m_eventIndex[m_freeEventList].next;

      //
      // insert in ordered list
      //
      evtIndex->next  = END_LIST;
      evtIndex->event = &m_eventPool[inserted];
      evtIndex->event->getCopy( event );
      
      int *ptr = &m_eventQueue;

      for( ; *ptr != END_LIST; 
           ptr = &m_eventIndex[*ptr].next)
           if (event.timeStamp <= m_eventPool[*ptr].timeStamp)
           {
               evtIndex->next = *ptr;
               break;
           }

      *ptr = inserted;
   }
//   m_totalEvents++;
 }

// ============================================================================
int SimulationContext::removeEvent(
                                   KR_EventLabel label,
                                   KR_ObjectID   source
                                  )
 {

   int         result = FALSE;
   KR_EventID *ref    = &m_eventQueue;

   // walk along event queue
   while(*ref != END_LIST)
   {
      if ((m_eventPool[*ref].source == source) &&
          (m_eventPool[*ref].label  == label )    )
      {
         KR_EventID nextInQueue = m_eventIndex[*ref].next;

         // add event to free list
         m_eventIndex[*ref].event = NULL;
         m_eventIndex[*ref].next  = m_freeEventList;
         m_freeEventList = *ref;

         // remove from event queue
         *ref = nextInQueue;
      }
      else
         ref = &m_eventIndex[*ref].next;
   }
   return(result);
 }

// ============================================================================
KR_EventID SimulationContext::popEvent( KR_TimeDelta timeStamp )
 {
   KR_EventID poped = END_LIST;

   if ( m_eventQueue != END_LIST &&
          m_eventPool[m_eventQueue].timeStamp < timeStamp )
   {
      poped        = m_eventQueue;
      m_eventQueue = m_eventIndex[poped].next;
   }

   return(poped);
 }

// ============================================================================
int SimulationContext::freeEvent( KR_EventID  id )
 {
   int result = 0;

   if(id>=0 && id<m_maxEventQnty)
   {
      if(  m_eventIndex[id].event != NULL  )
      {
           m_eventIndex[id].event = NULL;
           m_eventIndex[id].next  = m_freeEventList;

           m_freeEventList = id;
           result = 1;
      }
      else warning("<Context>::Event with id <%d> does not exist, can not delete it.\n",(int)id);
   }
   else s_ASSERTNQ1("<Context>::Event id <%d> is invalid. Can not delete the event.\n",(int)id);

   return(result);
 }

#include "graph.h"
extern CFixedColorFont	font5;

// ============================================================================
int SimulationContext::poll( KR_TimeDelta timeStamp )
 {
   int result = 1;
   // look through time-ordered list every time from the begining
#if 0
int x=0,y=0;
unsigned color = GRFillColor(255,255,255);
int lastlabel = -1, labelcnt = 0;
#endif
//   int counter=0;

   for (int poped = popEvent(timeStamp);
        poped != END_LIST;
        poped = popEvent(timeStamp))
   {
       KR_Event *event = &m_eventPool[poped];

//-----------------------------
#if 0
if(  lastlabel==event->label  ) labelcnt++;
else
{

if( labelcnt > 5 )
{
char text[30];
sprintf(text,"%i    %i",event->label, labelcnt+1);
font5.PrintClipColorAt(x-319,y-220,text,color);
x+= 100;
if(  x > 500 ) { x = 0; y+=8; }
}

lastlabel = event->label;
labelcnt = 0;
}
#endif
#if 0
counter++;

if(  counter>1000  ) 
if(  event->source.isNUL() ) 
     echo("%i  %lg  [?]",event->label,event->timeStamp );
else
     echo("%i  %lg  [%s/%i]",event->label,event->timeStamp, searchObject(event->source), event->source.cachePos );
#endif
//-----------------------------

       // call object taker
       if(   event->destination.cachePos >= 0
          && event->destination.cachePos < m_maxObjectQnty)
       {
          KR_Object *object = m_objectIndex[event->destination.cachePos].object;

          if(   object != NULL
             && object->getObjectID()==event->destination )
          {
             Session::m_moment = event->timeStamp;
             if( !object->receiveEvent(*event) )
             {
                 echo(
                       "Object <%s> do't recognize message [%i/\"%s\"] from %s",
                       m_objectIndex[event->destination.cachePos].symbolic,
                       event->label,
                       s_FindLabelName(event->label),
                       searchObject(event->source) );
             }
          }
          else 
		if (event->source.id != event->destination.id)
		echo("<Context>::Absent destination object for the event.\n"
                           "           Event: label [%d/\"%s\"], source %d/, destination %d\n",
                           (int)event->label,
                           s_FindLabelName(event->label),
                           (int)event->source.id,
                           (int)event->destination.id);
       }
       else s_ASSERTNQ5("<Context>::Event destination not in the ObjectPool.\n"
                        "           Event: label [%d/\"%s\"], source %d/\"%s\", destination %d\n",
                        (int)event->label,
                         s_FindLabelName(event->label),
                        (int)event->source.id,
                        searchObject(event->source),
                        (int)event->destination.id);

       freeEvent(poped);
   }

   //m_lastTime = timeStamp;

   return result;
 }

// ============================================================================
KR_ObjectID SimulationContext::addObject(
                                         const char name[],
                                         KR_Object *object
                                        )
 {
   int  inserted = -1;
   long cp       = -1;

   //ASSERT(object->m_tableName);
   
   /*if (!object->m_tableName)
	warning("Absent Table Name, %s\n", name);*/

   if(  m_freeObjectList != END_LIST  )
   {
        // get free object block
        inserted = m_freeObjectList;
        cp       = uniqueID();

        KR_ObjectListElem &obj = m_objectIndex[inserted];
        m_freeObjectList = obj.next;

        obj.object = object;
        s_ASSERT(strlen(name)<MAX_SYMBOLIC_LENGHT,"SimulationContext::addObject: Name too big");
        strcpy( obj.symbolic, name );

        obj.next = m_objectQueue;
        m_objectQueue = inserted;

        object->context = this;
        object->id.Init( cp , inserted);
        object->addNotify();

        //echo("SimulationContext::addObject() Object <%s> add",name);
        /*
         * Если был общий старт
         */
        if( m_started )
        {
             KR_Event event;
             event.label       = S_WAKE_UP;
             event.source.Init( 0, 0 );
             event.destination = object->id;
             //event.timeStamp   = m_lastTime;
			 event.timeStamp   = Session::m_moment ;
             obj.object->receiveEvent(event);
        }
   }
   else echo("SimulationContext::addObject: Object pool is fool");

   return KR_ObjectID( cp, inserted );
 }

// ============================================================================

KR_ObjectID  SimulationContext::addObject(
                                    const char  name[],
                                    KR_Object  *object,
                                    KR_ObjectID id
                                   )
{
   if(  m_freeObjectList != END_LIST  )
   {
        int *curItem = &m_freeObjectList;
        int  found = 0;

        for(  ; *curItem!=END_LIST; curItem = &(m_objectIndex[*curItem].next) )
             if(  *curItem==id.cachePos  )
             {
                  *curItem = m_objectIndex[*curItem].next;
                  found = 1;
                  break;
             }

        (void)found;
        //s_ASSERT(!found,"SimulationContext::addObject() Dublicate add object");
        KR_ObjectListElem &obj = m_objectIndex[id.cachePos];

        obj.object = object;
        s_ASSERT(strlen(name)<MAX_SYMBOLIC_LENGHT,"SimulationContext::addObject: Name too big");
        strcpy( obj.symbolic, name );

        obj.next = m_objectQueue;
        m_objectQueue = id.cachePos;

        object->context = this;
        object->id = id;

        //obj.object->/*load */ addNotify();
   }
   else echo("SimulationContext::addObject: Object pool is fool");

   return id;
}

// ============================================================================
KR_ObjectID SimulationContext::searchObject( const char  name[] )
 {
    int result = m_objectQueue;
    int cv = cacheVal( name )&(CACHE_SIZE-1);
    KR_ObjectList &co = m_cacheObject[cv];

    if( co != NULL )
    {
         if(  strcmp(co->symbolic, name) == 0  )
         if(  co->object!= 0  )
              return co->object->id;
    }

    // look through
    for( result = 0 ; result < m_maxObjectQnty ; result++ )
       if ( strcmp(m_objectIndex[result].symbolic, name) == 0 )
       if ( m_objectIndex[result].object != 0   )
       {
          co = &(m_objectIndex[result]);
          return m_objectIndex[result].object->id;
       }

    if (*name)	
    warning("Context::saerchObject(name) Unable to get ObjectID for object <%s>", name);

    return KR_ObjectID::NUL();
 }

// ============================================================================
int SimulationContext::isExist( const char  name[] )
 {
    int result = m_objectQueue;
    int cv = cacheVal( name )&(CACHE_SIZE-1);
    KR_ObjectList &co = m_cacheObject[cv];

    if( co != NULL )
    {
         if( strcmp(co->symbolic, name) == 0 )
         if(  co->object!= 0  )
              return 1;
    }

    // look through
    for( result =0; result<m_maxObjectQnty ; result++ )
       if ( strcmp(m_objectIndex[result].symbolic, name) == 0 )
       if ( m_objectIndex[result].object != 0   )
       {
          co = &(m_objectIndex[result]);
          return 1;
       }

    return 0;
 }

// ============================================================================
const char * SimulationContext::searchObject(
                                              const KR_ObjectID &objectID
                                            )
{
	char *result = NULL;
	if(  objectID.cachePos >= 0 && objectID.cachePos < m_maxObjectQnty  )
	{
        KR_Object *object = m_objectIndex[ objectID.cachePos ].object;
        if(  object!=0  )
		{
			
			if(  object->id == objectID )
				result = m_objectIndex[objectID.cachePos].symbolic;
			
		}
	}
	
	return result;
}

// ============================================================================
void SimulationContext::removeObject(
                                     const KR_ObjectID &objectID
                                    )
 {
    if( objectID.cachePos >=0  && objectID.cachePos < m_maxObjectQnty )
    {
       KR_ObjectListElem &elem  = m_objectIndex[objectID.cachePos];

       if( elem.object != 0 )
       if( elem.object->id == objectID )
       {
            elem.object->removeNotify();

            elem.next = m_freeObjectList;
            m_freeObjectList     = objectID.cachePos;
            elem.object->context = NULL;
            elem.object->id      = KR_ObjectID::NUL();
            elem.object          = NULL;
            elem.symbolic[0]     = 0;
       }
    }
 }

// ============================================================================
void SimulationContext::start(KR_TimeDelta timeStamp)
 {
   KR_Event event;

   event.label       = S_WAKE_UP;
   event.source      = KR_ObjectID::NUL();
   event.timeStamp   = timeStamp;

   // walk through object Pool
   int oid = m_objectQueue;
   for(; oid != END_LIST; oid = m_objectIndex[oid].next )
        m_objectIndex[oid].object->receiveEvent(event);

   m_started = 1;
 }

//=============================================================================
int SimulationContext::cacheVal( const char *str )
 {
    register unsigned   b = 0;
    register int        i;
    register const char*s = str;

    for( i = 0; *s != 0 ; ++i, ++s )
    {
         b += i;
         b += *s;
         b ^= ~((*s) << 2);
    }

    return (int)b;
 }

//=============================================================================
int  SimulationContext::isExist( KR_ObjectID id )
{
    if(    id.cachePos>=0 
        && id.cachePos<m_maxObjectQnty  )
    {
         if( m_objectIndex[id.cachePos].object  )
             return m_objectIndex[id.cachePos].object->id == id;
    }
    return 0;
}


void SimulationContext::clearEvents()
{
	
	s_ASSERT(m_eventPool!=NULL && m_eventIndex!=NULL,"Memory too low(event)");
    // fill event index
    for( int i = 0 ; i < m_maxEventQnty; i++)
    {
         m_eventIndex[i].event = NULL;
         m_eventIndex[i].next  = i + 1;
    }
    m_eventIndex[m_maxEventQnty - 1].next = END_LIST;

    // initialize pointers
    m_freeEventList = 0;
    m_eventQueue    = END_LIST;

}

void SimulationContext::initObjects()
{
    for( int i = 0; i < m_maxObjectQnty; i++)
    {
         m_objectIndex[i].object = NULL;
         m_objectIndex[i].next   = i + 1;

         // initialize publisher data
         m_objectIndex[i].resendEventList = -1;
         m_objectIndex[i].resendEventQnty =  0;
    }
    m_objectIndex[m_maxObjectQnty - 1].next = END_LIST;

    m_freeObjectList = 0;
    m_objectQueue    = END_LIST;
}


void SimulationContext::clearObjects()
{
	// walk through object Pool
	
	for( int i = 0; i < m_maxObjectQnty; i++)
    {
        if (m_objectIndex[i].object == NULL)
			 continue;
		removeObject(m_objectIndex[i].object->id);
	}

    
	initObjects();
}



bool SimulationContext::dump(PIN_SaveFile & sf)
{
	static PIN_SaveItemPrefix prefix;
	
	prefix.m_Type = PIN_SaveItemPrefix::IP_CONTEXT;
	strcpy(prefix.m_Check,PIN_check);


	if ( !sf.WriteData( (char *) & prefix, sizeof(PIN_SaveItemPrefix)) ||
		 !sf.WriteData( (char *) & m_started, sizeof(SimulationContextData)) ||
         !g_timer.dump(sf) ||
		 !sf.WriteData( (char *) & Session::m_moment, sizeof(double)) ||
		 !sf.WriteData( (char *) & Session::m_viewTime, sizeof(double)) ||
// Level Number
 		 !sf.WriteData( (char *) & ol_Level::m_levelNumber, sizeof(int)))

		return false;



	
	
// Dedicated objects

	if (! g_hardware.dump(sf) ||
		! g_super.m_publisher->dump(sf))
		return false;

	// Dedicated objects


	// Save All Static Data
	// Route

	if (!Vehicle::SaveStaticData(sf) ||
		!Route::SaveStaticData(sf) ||
		!KR_Hardware::SaveStaticData(sf))
		return false;

	// Save All Static Data



	// walk through event Pool
	KR_EventID *ref    = &m_eventQueue;
	
	while(*ref != END_LIST)
	{
		if (!m_eventPool[*ref].dump(sf))
			return false;
		ref = &m_eventIndex[*ref].next;
	}



	static char dummy[MAX_CLASS_NAME_LEN+1] = "";
	
	// walk through object Pool
    
    for(int oid = 0; oid < m_maxObjectQnty; oid ++)
	{

		if (m_objectIndex[oid].object == NULL)
			continue;

		if (! m_objectIndex[oid].object->shouldDump())
			continue;


		prefix.m_Type = PIN_SaveItemPrefix::IP_OBJECT;				
		bool result = sf.WriteData( (char *) & prefix, sizeof(PIN_SaveItemPrefix)  );
	
		if (!result)
			return false;


		if (m_objectIndex[oid].object->m_tableName)
			sf.WriteData(m_objectIndex[oid].object->m_tableName,MAX_CLASS_NAME_LEN+1);
		else
			sf.WriteData(dummy,MAX_CLASS_NAME_LEN+1);

		sf.WriteData(m_objectIndex[oid].symbolic,MAX_SYMBOLIC_LENGHT + 1);
		sf.WriteData((char *) & m_objectIndex[oid].resendEventQnty, sizeof(int));
		sf.WriteData((char *) & m_objectIndex[oid].resendEventList, sizeof(int));
		sf.WriteData((char *) & m_objectIndex[oid].object->id,sizeof(KR_ObjectID));

        if (!m_objectIndex[oid].object->dump(sf))
			return false;
	}
		
	prefix.m_Type = PIN_SaveItemPrefix::IP_FINITALACOMEDIA;
	if (!sf.WriteData( (char *) & prefix, sizeof(PIN_SaveItemPrefix)  ))
		return false;
	
	
	return true;
}

bool SimulationContext::load(PIN_SaveFile & sf)
{
	static PIN_SaveItemPrefix prefix;
	
	sf.GetData((char *) & prefix, sizeof(PIN_SaveItemPrefix));
	
	if (strcmp(prefix.m_Check,PIN_check) != 0 ||
		prefix.m_Type != PIN_SaveItemPrefix::IP_CONTEXT)
		return false;

	int levelNumber;
	

	if ( ! sf.GetData((char *) & m_started, sizeof(SimulationContextData)) ||
		! g_timer.load(sf) ||
		! sf.GetData( (char *) & Session::m_moment, sizeof(double)) ||
		! sf.GetData( (char *) & Session::m_viewTime, sizeof(double)) ||
                ! sf.GetData( (char *) & levelNumber, sizeof(int)))
		return false;

	if (ol_Level::m_levelNumber != levelNumber)
	{
		echo("Level number mismatch");	
		return false;
	}
	
	
	
	// Clearing Object Cache
    for(int i = 0; i < CACHE_SIZE; ++i )
		m_cacheObject[i] = NULL;


	
	// clearing object Pool
    
    for( int i = 0; i < m_maxObjectQnty; i ++)
	{
		
		if (m_objectIndex[i].object == NULL)
			continue;
		
	
		if (m_objectIndex[i].object->shouldDump())
			removeObject(m_objectIndex[i].object->id);
		
	}

	// Checking


	for ( int i = 0; i < g_super.m_level.m_howitzersLoaded; i++)
	{
		ASSERT( g_super.m_level.m_howitzerPool[i].occupant.isNUL());
	}

	
	ct_ClassTable *list = ct_Storage::m_classTableList;
		
	for(; list!=NULL ; list = list->m_nextClassTable )
	{
		if (list->m_existList)
		 s_ASSERT(!list->m_existList->shouldDump(),"Table is not clear");
	}

	// Checking
	
	// Dedicated objects
	if (! g_hardware.load(sf)||
		! g_super.m_publisher->load(sf))
		return false;
	// Dedicated objects


	
	// Load All Static Data
	// Route

	if (!Vehicle::LoadStaticData(sf) ||
		!Route::LoadStaticData(sf) ||
		!KR_Hardware::LoadStaticData(sf))
		return false;

	ct_Arena::clearCache();
	Fountain::createFreeList();
	
	// now walk through event messages saved
	
	int type;
	
	clearEvents();
	while ( (type = sf.GetNextDataItemType()) == PIN_SaveItemPrefix::IP_EVENT)
	{
		KR_Event theEvent;		
		theEvent.load(sf);
		addEvent(theEvent);
	}
	
	if (type == PIN_SaveItemPrefix::IP_ERROR)
		return false;


	static char classTableName [MAX_CLASS_NAME_LEN+1];
	static char symbolicName   [MAX_CLASS_NAME_LEN+1];
	
	while ( (type = sf.GetNextDataItemType()) == PIN_SaveItemPrefix::IP_OBJECT)
	{
		
		PIN_SaveItemPrefix prefix;

	
		bool result = sf.GetData( (char *) & prefix, sizeof(PIN_SaveItemPrefix)  );
		
		if (!result)
			return false;
		
		if ( prefix.m_Type != PIN_SaveItemPrefix::IP_OBJECT)
			return false;
		
		
		int resendEventQnty;
		int resendEventList;
		KR_ObjectID objectID;
		
		sf.GetData(classTableName, MAX_CLASS_NAME_LEN+1);		
		sf.GetData(symbolicName,MAX_SYMBOLIC_LENGHT + 1);
		
		sf.GetData((char *) & resendEventQnty, sizeof(int));
		sf.GetData((char *) & resendEventList, sizeof(int));
		sf.GetData((char *) & objectID,sizeof(KR_ObjectID));
		
		
		ct_ClassTable * theTable = g_arena.searchClassTable( classTableName );				
		
		ASSERT(theTable);
		
		ASSERT(*symbolicName);
		
		KR_ObjectID theID	= theTable->newObject(symbolicName, objectID);
		
		ASSERT( !theID.isNUL()		);
		ASSERT(  theID == objectID	);
		
		
		int oID = theID.getCachePos();
		m_objectIndex[oID].resendEventQnty = resendEventQnty;
		m_objectIndex[oID].resendEventList = resendEventList;
		
		if (!m_objectIndex[oID].object->load(sf))
			return false;		
		
	}
	
	
	if (type != PIN_SaveItemPrefix::IP_FINITALACOMEDIA)
	{
		return false;
	}
	
	// walk through object Pool
    
    for(int oid = 0; oid < m_maxObjectQnty; oid ++)
	{
		
		if (m_objectIndex[oid].object == NULL)
			continue;
		
		m_objectIndex[oid].object->loadNotify();
	}

	g_timer.m_prevTime  = ::GetTickCount();
	g_timer.m_startTick = g_timer.m_prevTime - g_timer.m_deltaTime;
	
	return true;
}
