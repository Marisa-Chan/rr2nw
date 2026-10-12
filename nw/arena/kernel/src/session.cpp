            // ================================================================
            // FUNCTIONAL AREA:   MicroKernel
            // NAME:              Session.cpp
            // AUTHORS:           MKrylov
            // DESIGN REFERENCE:
            // MODIFICATION:      23 Feb 97 - creation
            // ================================================================

// =================================================================== INCLUDES
#include <stdio.h>
// =================================================================== SYNOPSIS
#include "Kernel\h\Session.h"
#include "Kernel\h\Context.h"
#include "Kernel\h\Timer.h"
#include "Kernel\h\Observer.h"

double  Session::m_moment = 0;
double  Session::m_viewTime = 0;
double  Session::m_frameSec = 0;
// ============================================================================
Session::Session(
                 KR_RealTimer *realTimer,
                 KR_Hardware  *hardware
                 )
// ============================================================================
{
    m_moment       = 0;

    m_contextList  = NULL;
    m_observerList = NULL;

    m_realTimer    = realTimer;
    m_hardware     = hardware;

    m_contextList  = NULL;
}
// ============================================================================
Session::~Session()
// ============================================================================
{
    m_contextList = NULL;
}
// ============================================================================
void Session::Add(
                     SimulationContext *context
                  )
// ============================================================================
{
    SimulationContextElem  *elem = new SimulationContextElem,
                           *list = m_contextList;
    // fill new node
    elem->next    = NULL;
    elem->context = context;

    // insert the node in the end of list
    if (list!=NULL)
    {
        // search last element
        for(;(list->next!=NULL); list = list->next);

        // set after last elem
        list->next = elem;
    }
    else
      m_contextList = elem;

    fprintf(stdout, "\nContext added\n");
}
// ============================================================================
int Session::Remove(
                      SimulationContext *context
                   )
{
   int founded = 0;
   if (m_contextList)
   {
      SimulationContextElem *list  =  m_contextList;
      SimulationContextElem **prev = &m_contextList;

      for(;list && !founded; list = list->next)
      {
         if (list->context == context)
         {
            founded = 1;
            *prev   = list->next;
         }

         prev = &(list->next);
      }
   }

   if (founded)
   {
      fprintf(stdout, "\nContext Removed\n");
   }
   return(founded);
}
// ============================================================================
void Session::AddObserver(
                            KR_Observer *observer
                         )
{
    ObserverElem  *elem = new ObserverElem,
                  *list = m_observerList;

    // fill new node
    elem->next     = NULL;
    elem->observer = observer;

    // insert the node in the end of list
    if (list)
    {
        // search last element
        for(;list->next; list = list->next);

        // set after last elem
        list->next    = elem;
        elem->context = NULL;
    }
    else
      m_observerList = elem;

    fprintf(stdout, "\n Observer added\n");
}
// ============================================================================
int Session::RemoveObserver(
                              KR_Observer *observer
                           )
{
   int founded = 0;
   if (m_observerList)
   {
      ObserverElem  *list =  m_observerList;
      ObserverElem **prev = &m_observerList;

      for(;list && !founded; list = list->next)
      {
         if (list->observer == observer)
         {
            founded = 1;
            *prev   = list->next;
         }

         prev = &(list->next);
      }
   }

   if (founded)
   {
      fprintf(stdout, "\n Observer Removed\n");
   }

   return(founded);
}

// ============================================================================
int Session::poll()
{
    int result = (m_contextList != NULL);

    // check hardware
    if ( m_hardware != NULL )
    {
        //result = m_hardware->Scan();
    }

    // look through context list
    for (SimulationContextElem *elem = m_contextList; elem!=NULL; elem = elem->next)
    {
        double oldTime = m_viewTime;
        m_viewTime = m_realTimer->GetTime();
        m_frameSec = m_viewTime - oldTime;
        elem->context->poll(m_viewTime);
    }
    // call viewports for the context
    return result;
}
// ============================================================================



