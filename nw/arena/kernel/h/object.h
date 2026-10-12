#ifndef _KR_Object_HPP_
#define _KR_Object_HPP_

#include <string.h>
#ifndef __KRTYPES_H__
#include "kernel/h/krtypes.h"
#endif

#ifndef __S_EVDATA_H__
#include "kernel/h/s_evdata.h"
#endif




#define KR_WAKE_UP          0
#define KR_SET_ATTR         1

#define UNKNOWN_OBJECT_ID  -1
#define HARDWARE_OBJECT_ID -2

#define IUnknownIID         0

class PIN_SaveFile;

class KR_Event
{
public:
	KR_EventLabel  label;
	KR_ObjectID    destination,
		source;
	KR_TimeDelta   timeStamp;
	s_EventData    data;
	KR_Event() { timeStamp = -1.0;}
	
	void init( KR_EventLabel      _label,
		KR_TimeDelta       _time,
		const KR_ObjectID &_source,
		const KR_ObjectID &_destination )
	{
		label       = _label;
		timeStamp   = _time;
		source      = _source;
		destination = _destination;
	}
	
	void getCopy( const KR_Event &ev )
	{
		label       = ev.label;
		destination = ev.destination;
		source      = ev.source;
		timeStamp   = ev.timeStamp;
		data.getCopy( ev.data );
	}
	
	KR_Event
		(
		KR_EventLabel       _label,
		KR_TimeDelta        _timeStamp,
		const KR_ObjectID  &_source,
		const KR_ObjectID  &_destination
		)
		: label      (_label),
		timeStamp  (_timeStamp),
		source     (_source),
		destination(_destination)
	{}
	
	bool	dump(PIN_SaveFile & sf);
	bool	load(PIN_SaveFile & sf);
};

class SimulationContext;
class KR_Object
{
	friend class SimulationContext;
private:
	KR_ObjectID id;
	
	

public:
	SimulationContext  *context;
	double		   m_creationTime;
	KR_Object();
	virtual            ~KR_Object() {}
	virtual int         receiveEvent(KR_Event &event) = 0;
	void                issueEvent (const KR_Event &event);
	
	KR_ObjectID         getObjectID() const { return (id); }
	SimulationContext  *getContext() const  { return (context); }

	char				* m_tableName;

	virtual void        addNotify()    {}
	virtual void        removeNotify() {}
	virtual void       *queryInterface( int interNum );
	
	virtual bool		  dump(PIN_SaveFile & sf);
	virtual bool		  load(PIN_SaveFile & sf);
	virtual void		  loadNotify () {};
	virtual bool		  shouldDump () = 0;
};

typedef KR_Object *KR_ObjectPTR;


#endif

/* _KR_Object_H_ */