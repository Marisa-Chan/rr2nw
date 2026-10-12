/*
 * File  : D:\GAME\OBASE\Publish\Publish.cpp
 * Autor :
 * Ver   1.0 
 */
#include "Publish.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/pubmsg.h"

#define HANDLE int
#include "storage\h\savefile.h"

#define END_LIST (-1)


//{{EVENT_LABEL_NAMES
static s_ELN elnTable[]=
{
   s_ELN(EVT_SUBSCRIPT_INFORM,"EVT_SUBSCRIPT_INFORM"),
   s_ELN(KR_WAKE_UP,"KR_WAKE_UP"),
   s_ELN(EVT_SUBSCRIPT_TO_EVENT,"EVT_SUBSCRIPT_TO_EVENT"),
   s_ELN(EVT_UNSUBSCRIPT_TO_EVENT,"EVT_UNSUBSCRIPT_TO_EVENT"),
   s_ELN(EVT_FULL_UNSUBSCRIPT_TO_AUTHOR,"EVT_FULL_UNSUBSCRIPT_TO_AUTHOR"),
   s_ELN(EVT_DUMP,"EVT_DUMP"),
   s_ELN(EVT_REMOVE_AUTHOR,"EVT_REMOVE_AUTHOR"),
   s_ELN(),
};

static s_ELNTable selnTable("Publisher",elnTable);

//}}END_OF_EVENT_LABEL_NAMES

 /*********************************
  *
  *   Publisher implementation
  *
  *********************************/

 //============================================================
Publisher::Publisher( int events, int subscribers )
 {
    // allocate non-changeble data
    m_eventDescrQnty  = events;
    m_events          = new EventListElem[m_eventDescrQnty];

    // move all events to free list
    m_eventFreeList = 0;
    for (int i = 0; i < m_eventDescrQnty - 1; i++)
    {
       m_events[i].subscriberList = -1;
       m_events[i].next = i + 1;
    }
    m_events[m_eventDescrQnty - 1].subscriberList = -1;
    m_events[m_eventDescrQnty - 1].next = -1;

    m_subscriberQnty  = subscribers;
    m_subscribers     = new SubscriberElem[m_subscriberQnty];

    // move all subscribers to free list
    m_subscriberFreeList = 0;
    for (int i = 0; i < m_subscriberQnty - 1; i++)
    {
       m_subscribers[i].next       = i + 1;
       m_subscribers[i].subscriber = KR_ObjectID::NUL();
    }

    m_subscribers[m_subscriberQnty - 1].next = -1;
    m_subscribers[m_subscriberQnty - 1].subscriber = KR_ObjectID::NUL();
 }

 //============================================================
Publisher::~Publisher()
 {
   delete m_subscribers;
   m_subscriberQnty = 0;
   m_subscribers    = NULL;

   delete m_events;
   m_eventDescrQnty = 0;
   m_events         = NULL;
 }

 //============================================================
int Publisher::receiveEvent( KR_Event &event )
 {
    return EVENTHANDLER( event );
 }

 //============================================================
void Publisher::addNotify()
 {
    loadStateTransitionTable();
    KR_Object::addNotify();
    // insert your code this
    resetState();
 }

void Publisher::removeNotify()
 {
    KR_Object::removeNotify();
    // insert your code this
 }


void Publisher::wakeUp( KR_Event & )
{
    //{{GET_EVENT(KR_WAKE_UP)
    //}}END_OF_GET_EVENT(KR_WAKE_UP)
}

 /*******************************
  *
  * Создает запись об авторе, если его небыло,
  * заносит подписчика в список приемщиков указанного
  * события
  *
  *******************************/
void Publisher::subscript( KR_Event &event )
{
    KR_EventLabel label;
    KR_ObjectID   author;
    KR_ObjectID   subscriber;
    int           informAuthor;

    //{{GET_EVENT(EVT_SUBSCRIPT_TO_EVENT)
    event.data.open(EDO_READ)
                   .getInt(label)
                   .getObjectID(author)
                   .getObjectID(subscriber)
                   .getInt(informAuthor)
              .close();
    //}}END_OF_GET_EVENT(EVT_SUBSCRIPT_TO_EVENT)

    registerEvent(author, label);
    if (subscript(author, label, subscriber) && informAuthor)
    {
       event.label       = EVT_SUBSCRIPT_INFORM;
       event.source      = subscriber;
       event.destination = author;
       //{{PUT_EVENT(EVT_SUBSCRIPT_INFORM)
       //}}END_OF_PUT_EVENT(EVT_SUBSCRIPT_INFORM)
       issueEvent(event);
    }
}

 /*******************************
  *
  * Удаляет автора из таблицы подписчиков
  *
  *******************************/
void Publisher::unSubscript( KR_Event &event )
{
    KR_EventLabel label;
    KR_ObjectID   author;
    KR_ObjectID   subscriber;

    //{{GET_EVENT(EVT_UNSUBSCRIPT_TO_EVENT)
    event.data.open(EDO_READ)
                   .getInt(label)
                   .getObjectID(author)
                   .getObjectID(subscriber)
              .close();
    //}}END_OF_GET_EVENT(EVT_UNSUBSCRIPT_TO_EVENT)

    unSubscript(author, label, subscriber);
}

 /*******************************
  *
  * Удаляет подписчика из списка рассылки на все события
  * от данного автора
  *
  *******************************/
void Publisher::fullUnSubscipt( KR_Event &event )
{
   KR_EventLabel label;
   KR_ObjectID   author;
   KR_ObjectID   subscriber;

   //{{GET_EVENT(EVT_FULL_UNSUBSCRIPT_TO_AUTHOR)
   event.data.open(EDO_READ)
                  .getObjectID(author)
                  .getObjectID(subscriber)
             .close();
   //}}END_OF_GET_EVENT(EVT_FULL_UNSUBSCRIPT_TO_AUTHOR)

   unSubscript(author, label, subscriber);
}

 /*******************************
  *
  * Сбрасывает содержимое всех таблиц на диск
  *
  *******************************/
void Publisher::dump( KR_Event &event )
{
    //{{GET_EVENT(EVT_DUMP)
    //}}END_OF_GET_EVENT(EVT_DUMP)
   char fname[128];
   sprintf(fname, "C:\\pump%d.dmp", ((int) event.timeStamp));
   FILE *dmp = fopen(fname, "wt");
   fprintf(dmp, "============= %d ===========\n", context->m_maxObjectQnty);

   for (int k = 0; k < context->m_maxObjectQnty; k++)
   {
      KR_ObjectList objectIndex = context->m_objectIndex;
      if (objectIndex[k].resendEventQnty != 0)
      {
         fprintf(dmp, "%s\n",objectIndex[k].symbolic);
         for(
              int i = objectIndex[k].resendEventList; 
              i != -1; 
              i = m_events[i].next
            )
         {
            fprintf(dmp, "\t%d/%s\n", m_events[i].label,
                                     s_FindLabelName(m_events[i].label));

            for(
                 int j = m_events[i].subscriberList; 
                 j != -1;
                 j = m_subscribers[j].next
               )
            {
                fprintf(dmp, "\t\t%d %s\n",
                     m_subscribers[j].subscriber,
                     context->searchObject(m_subscribers[j].subscriber));
            }
         }
      }
   }
   fclose(dmp);
}

 /*******************************
  *
  * Удаляет автора из таблицы и всех подписчиков на него
  *
  *******************************/
void Publisher::removeAutor( KR_Event &event )
{
    KR_ObjectID author;

    //{{GET_EVENT(EVT_REMOVE_AUTHOR)
    event.data.open(EDO_READ)
                   .getObjectID(author)
              .close();
    //}}END_OF_GET_EVENT(EVT_REMOVE_AUTHOR)

    removeAuthor(author);
}

//{{ACTIONF_IMPLEMENTATION

void Publisher::loadStateTransitionTable()
 {
    //{{TRANSLATION_TABLE
    static KR_ActiveObject::StateTransitionTableElem row0[6] =
    {
        {EVT_REMOVE_AUTHOR             , ST_NEW_STATE,  0, (ACTION)removeAutor    },
        {EVT_DUMP                      , ST_NEW_STATE,  0, (ACTION)dump           },
        {EVT_FULL_UNSUBSCRIPT_TO_AUTHOR, ST_NEW_STATE,  0, (ACTION)fullUnSubscipt },
        {EVT_UNSUBSCRIPT_TO_EVENT      , ST_NEW_STATE,  0, (ACTION)unSubscript    },
        {EVT_SUBSCRIPT_TO_EVENT        , ST_NEW_STATE,  0, (ACTION)subscript      },
        {KR_WAKE_UP                    , ST_NEW_STATE,  0, (ACTION)wakeUp         }
    };
    //}}END_OF_TRANSLATION_TABLE{{
    static StateElem STT[1]={
      StateElem(6,row0)
    };
    m_stateTable = STT;
    m_stateQnty  = 1;
    //}}END_OF_STATE_TABLE
 }

// ============================================================================
int Publisher::subscript(
                          KR_ObjectID   author,
                          KR_EventLabel label,
                          KR_ObjectID   subscriber
                        )
{
    s_ENTRY(Publisher::subscript)

    s_ASSERT(author.cachePos >=0, "Error cache pos" );
    int result = FALSE;
    KR_ObjectList objectIndex = context->m_objectIndex;
    int evtQnty  = objectIndex[author.cachePos].resendEventQnty;

    if (evtQnty > 0)
    {
       int evtPos   = -1;

       // search relevant event record
       int *eTail = &objectIndex[author.cachePos].resendEventList;
       for (; *eTail != -1; eTail = &m_events[*eTail].next)
       {
          if (m_events[*eTail].label == label)
          {
             evtPos = *eTail;
             break;
          }
       }

       if ( evtPos != -1)
       {
           int duplicateSubscriber = FALSE;

          // get last element of subscriber's list
          int *tail = &m_events[evtPos].subscriberList;
          for (; *tail != -1; tail = &m_subscribers[*tail].next)
          {
             if (m_subscribers[*tail].subscriber == subscriber)
             {
                duplicateSubscriber = TRUE;
                warning("Publisher:: duplicate subscribtion\n"
                     "author <%d>/%s/"
                     "\tevent <%d>"
                     "\tauthor <%d>/%s/\n",
                      author, context->searchObject(author),
                      m_events[evtPos].label,
                      subscriber, context->searchObject(subscriber));
                break;
             }
          }

          // check existance of free subscriber record
          if ( m_subscriberFreeList != -1 && !duplicateSubscriber)
          {
             *tail = m_subscriberFreeList;
             m_subscriberFreeList = m_subscribers[m_subscriberFreeList].next;

             m_subscribers[*tail].subscriber = subscriber;
             m_subscribers[*tail].next       = -1;
             result = TRUE;
          }
          else
             result = FALSE;
       }
       else
          warning("Publisher::in subscript absent event <%d> with author <%s>",
                                label, context->searchObject(author));
    }

    return (result);
}

// ============================================================================
void  Publisher::unSubscript(
                               KR_ObjectID   author,
                               KR_EventLabel label,
                               KR_ObjectID   subscriber
                            )
{
   s_ENTRY(Publisher::unSubscript)

   s_ASSERT(author.cachePos>=0,"Error author");
   KR_ObjectList objectIndex = context->m_objectIndex;
   int evtQnty  = objectIndex[author.cachePos].resendEventQnty;

   if (evtQnty > 0)
   {
      int evtIndex = objectIndex[author.cachePos].resendEventList;
      int evtPos   = -1;

      // search relevant event record
      int *eTail = &objectIndex[author.cachePos].resendEventList;
      for (; *eTail != -1; eTail = &m_events[*eTail].next)
      {
         if (m_events[*eTail].label == label)
         {
            evtPos = *eTail;
            break;
         }
      }

      if ( evtPos != -1)
      {
         // get relevant element of subscriber's list
         int *tail = &m_events[evtPos].subscriberList;
         for (; *tail != -1; tail = &m_subscribers[*tail].next)
         {
            if (m_subscribers[*tail].subscriber == subscriber)
            {
               int rest = m_subscribers[*tail].next;

               m_subscribers[*tail].next = m_subscriberFreeList;
               m_subscriberFreeList = *tail;
               *tail = rest;
               break;
            }
         }
      }
      else
         warning("\nPublisher::in unSubscribe absent event <%d> with author <%d>",
                                       label, author);
   }
}

// ============================================================================
int Publisher::registerEvent(
                               KR_ObjectID   author,
                               KR_EventLabel label
                            )
{
   s_ENTRY(Publisher::registerEvent)

   s_ASSERT(author.cachePos,"Error author");

   int result = (m_eventFreeList != (-1));

   if (result)
   {
      KR_ObjectList objectIndex = context->m_objectIndex;
      //KR_ObjectListElemName *authorRecord = &objectIndex[author.cachePos];
      //objectIndex[author.cachePos].source = author;

      // get last element of events list
      int *tail = &objectIndex[author.cachePos].resendEventList;
      for (; *tail != -1; tail = &m_events[*tail].next)
      {
         if (m_events[*tail].label == label)
            return(result);
      }

      *tail = m_eventFreeList;
      m_eventFreeList = m_events[m_eventFreeList].next;

      // fill event record
      m_events[*tail].label = label;
      m_events[*tail].next  = -1;

      objectIndex[author.cachePos].resendEventQnty++;
   }

   if (result == FALSE)
      warning("Publisher:: event list overflow !!");

   return (result);
}

// ============================================================================
void Publisher::removeAuthor( KR_ObjectID  author )
{
   s_ENTRY(Publisher::removeAuthor)
   s_ASSERT(author.cachePos>=0,"Error author");
   KR_ObjectList objectIndex = context->m_objectIndex;
   int evtQnty  = objectIndex[author.cachePos].resendEventQnty;

   if (evtQnty > 0)
   {
      int evtIndex = objectIndex[author.cachePos].resendEventList;

      // walk throught all event record

      int *eTail = &objectIndex[author.cachePos].resendEventList;
      for (; *eTail != -1; eTail = &m_events[*eTail].next)
      {
         // get relevant element of subscriber's list
         int tail = m_events[*eTail].subscriberList;
         m_events[*eTail].subscriberList = -1;

         while(tail != -1)
         {
            long tt = m_subscribers[tail].next;

            //  add to free list all subscribers
            m_subscribers[tail].next = m_subscriberFreeList;
            m_subscriberFreeList = tail;
            tail = tt;
         }

      }

      // add event labels to free list
      int eventList = objectIndex[author.cachePos].resendEventList;
      while (eventList != -1)
      {
         long eventTail = m_events[eventList].next;

         m_events[eventList].next = m_eventFreeList;
         m_eventFreeList = eventList;
         eventList = eventTail;
      }
   }

   objectIndex[author.cachePos].resendEventQnty = 0;
   objectIndex[author.cachePos].resendEventList = -1;
}


bool	Publisher::dump(PIN_SaveFile & sf)
{
	if (!KR_ActiveObject::dump(sf) ||		
		!sf.WriteData( (char *) & m_eventDescrQnty, sizeof(PublisherData)  ) ||
		!sf.WriteData( (char *) m_events, sizeof(EventListElem)  * m_eventDescrQnty ) ||
		!sf.WriteData( (char *) m_subscribers, sizeof(SubscriberElem)  * m_subscriberQnty ))
		return false;	

	return true;
}

bool	Publisher::load(PIN_SaveFile & sf)
{
	if (!KR_ActiveObject::load(sf) ||  		    
		!sf.GetData( (char *) & m_eventDescrQnty, sizeof(PublisherData)  ) ||
		!sf.GetData( (char *) m_events, sizeof(EventListElem)  * m_eventDescrQnty ) ||
		!sf.GetData( (char *) m_subscribers, sizeof(SubscriberElem)  * m_subscriberQnty ))
		return false;

	return true;
}


void	Publisher::loadNotify()
{	
}

/* End of file D:\GAME\OBASE\Publish\Publish.cpp */
