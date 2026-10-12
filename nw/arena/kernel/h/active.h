#ifndef KR_ActiveObject_H
#define KR_ActiveObject_H

#ifndef __OBJECT_H__
#include "kernel/h/object.h"
#endif

#include "kernel/h/s_debug.h"
class  KR_ActiveObject;

typedef enum ST_TypeEnum
{
   ST_IMPOSSIBLE = 0,
   ST_IGNORE,
   ST_NEW_STATE,
}
ST_Type;

typedef int StateID;

#ifdef __WATCOMC__
#   define EVENTHANDLER(x) eventHandler(x)
#else
#   define EVENTHANDLER(x) eventHandler(x,this)
#endif
// ============================================================================


class PIN_SaveFile;


typedef struct {
	StateID     m_currentState;
} ActiveObjectData ;


class KR_ActiveObject : public ActiveObjectData
{
    public:
        // ==================================================== Types
       typedef void (KR_ActiveObject::ActionF)(KR_Event &event);
       typedef void (KR_ActiveObject::*Action)(KR_Event &event);
       typedef KR_ActiveObject* (*NewFunction)(void);

       typedef struct
       {
          KR_EventLabel  label;
          int            type;
          int            state;
          Action         action;
       }
          StateTransitionTableElem;

       class StateElem
       {
       friend class KR_ActiveObject;
       protected:
          int                        qnty;
          StateTransitionTableElem  *tableRow;
       public:
          StateElem(int l_qnty, StateTransitionTableElem  *l_tableRow )
          {
              s_ASSERT( l_tableRow!=NULL, "StateElem" );
              qnty       = l_qnty;
              tableRow   = l_tableRow;
          }
       };

 
        // ==================================================== Functions

                    KR_ActiveObject();
       virtual void loadStateTransitionTable() = 0;
       virtual void resetState();
    protected:
       int         m_stateQnty;

       StateElem  *m_stateTable;
       

    public:
#ifdef __WATCOMC__
       int         eventHandler( KR_Event &event );
#else
       int         eventHandler( KR_Event &event, const void *self );
#endif

	bool	dump(PIN_SaveFile & sf);
	bool	load(PIN_SaveFile & );


};

// ===================================================================== MACROS

#define ACTION                    KR_ActiveObject::Action
#define EVENT_IS_IMPOSSIBLE       ST_IMPOSSIBLE, 0, (ACTION) NULL
#define EVENT_IS_IGNORED          ST_IGNORE,     0, (ACTION) NULL

#define MAX_OBJECT_NAME_LENGTH 128

#endif

/* End of file ACTIVE.H */