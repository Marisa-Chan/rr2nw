            // ================================================================
            // FUNCTIONAL AREA:   MicroKernel
            // NAME:              KR_ActiveObject.cpp
            // AUTHORS:           MKrylov
            // DESIGN REFERENCE:
            // MODIFICATION:      23 Feb 97 - creation
            // ================================================================
/*
   receiveEvent() - not correct return value.
 */
#include <stdio.h>
#include "Kernel\h\Active.h"
#include "Kernel\h\Context.h"
#include "Kernel\h\Echo.h"

#define HANDLE int
#include "storage/h/savefile.h"

// ============================================================================
KR_ActiveObject::KR_ActiveObject()
 {
   m_stateQnty    = 0;
   m_currentState = 0;
   m_stateTable   = NULL;
 }

// ============================================================================
void KR_ActiveObject::resetState()
 {
   m_currentState = 0;
 }


bool	KR_ActiveObject::dump(PIN_SaveFile & sf)
{
		if (!sf.WriteData( (char *) & m_currentState, sizeof(ActiveObjectData)  ))
			return false;
		
		return true;
}

bool	KR_ActiveObject::load(PIN_SaveFile & sf)
{
		if (!sf.GetData( (char *) & m_currentState, sizeof(ActiveObjectData)  ))
			return false;
		
		return true;
}


// ============================================================================
#ifdef __WATCOMC__
int KR_ActiveObject::eventHandler( KR_Event &event )
#else
int KR_ActiveObject::eventHandler( KR_Event &event, const void *self )
#endif
 {
    // look through 'state transition table'

    if( m_stateTable != NULL )
    {
        StateTransitionTableElem *row  = m_stateTable[m_currentState].tableRow;
        int                       size = m_stateTable[m_currentState].qnty;

        for( int i = 0; i < size; i++, row++ )
             if( row->label == event.label)
             {
                  switch( row->type )
                  {
                  case ST_NEW_STATE:
                          m_currentState = row->state;
                          if( row->action != NULL )
                          {
#ifdef __WATCOMC__
                               (this->*(row->action))(event);
#else
                               (((KR_ActiveObject*)self)->*(row->action))(event);
#endif
                          }
                          // FIXME - ASSERT
                          break;

                  case ST_IGNORE:
                          break;

                  case ST_IMPOSSIBLE:
                          return 0;
                }

                return 1;
             }
    }

    return 0;
 }

/* End of file ACTIVE.CPP */
