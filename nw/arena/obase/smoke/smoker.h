/*
 * File  : C:\NW\ARENA\OBASE\Smoke\smoker.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __SMOKER_H__INCLUDED
#define __SMOKER_H__INCLUDED

#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"
#include "obase/dynobj/dyncorona.h"

class AttributeSmoker;
class Smoker : public ct_Subject
{
 public:
    AttributeSmoker   *m_attr;
    CFVector3          m_position;
    int                m_cnt;
    double             m_startTime;
    bool               m_sendedMove;
    double             m_brightness;
    s_ViewDynamicCorona m_corona;

    virtual void       onView( double ts );
             Smoker();
    virtual ~Smoker();
    virtual int  receiveEvent( KR_Event &event );
    virtual void addNotify   ();
    virtual void removeNotify();
    virtual CFVector3 realPosition();
    virtual void render( CViewDynamicList &,double);
    virtual void endRender( CViewScene *scene );
    virtual bool	shouldDump () { return false; } 
};

#endif // ifndef __SMOKER_H__INCLUDED
/* End of file C:\NW\ARENA\OBASE\Smoke\smoker.h */