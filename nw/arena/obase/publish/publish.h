/*
 * File  : D:\GAME\OBASE\Publish\Publish.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __PUBLISH_H__INCLUDED
#define __PUBLISH_H__INCLUDED

#include "storage/h/subject.h"
#include "kernel/h/active.h"


typedef struct  {
    KR_EventLabel label;
    int           subscriberList;
    int          next;
}    EventListElem;

typedef struct 
{
   KR_ObjectID  subscriber;
   int          next;
}   SubscriberElem;



typedef struct {
    int                            m_eventDescrQnty;
    int                            m_eventFreeList;
    

    int                            m_subscriberQnty;
    int                            m_subscriberFreeList;
    

} PublisherData;

class Publisher :	public KR_Object, 
					public KR_ActiveObject,
					public PublisherData
{
	
	EventListElem       *m_events;
	SubscriberElem      *m_subscribers;

 public:
             Publisher( int events = 1024, int subscribers = 1024 );
    virtual ~Publisher();
    virtual int  receiveEvent( KR_Event &event );
    virtual void addNotify   ();
    virtual void removeNotify();

    //{{ACTIONF_DECLARE
    void wakeUp        ( KR_Event &event );
    void subscript     ( KR_Event &event );
    void unSubscript   ( KR_Event &event );
    void fullUnSubscipt( KR_Event &event );
    void dump          ( KR_Event &event );
    void removeAutor   ( KR_Event &event );
    //}}END_OF_ACTIONF_DECLARE

	virtual bool	shouldDump () { return false; }  // dedicated object
	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
	virtual void	loadNotify();  

 private:

    void    loadStateTransitionTable();
    int     registerEvent( KR_ObjectID author, KR_EventLabel label );
    int     subscript(
                       KR_ObjectID   author,
                       KR_EventLabel label,
                       KR_ObjectID   subscriber
                     );
    void    unSubscript(
                       KR_ObjectID   author,
                       KR_EventLabel label,
                       KR_ObjectID   subscriber
                      );
    void    removeAuthor(KR_ObjectID   author);


	
};

#endif // ifndef __PUBLISH_H__INCLUDED
/* End of file D:\GAME\OBASE\Publish\Publish.h */