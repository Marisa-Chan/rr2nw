/*
 * File  : C:\NW\ARENA\OBASE\Fly\Fly.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __FLY_H__INCLUDED
#define __FLY_H__INCLUDED

#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"

class AttributeFly;
class Fly : public ct_Subject, public KR_ActiveObject
{
 public:
    AttributeFly          *m_attr;
    CViewObjectRef         m_skin;
    s_ViewDynamicObject    m_viewDynObj;

             Fly();
    virtual ~Fly();
    virtual int  receiveEvent( KR_Event &event );
    virtual void draw        ();
    virtual void addNotify   ();
    virtual void removeNotify();

    //{{ACTIONF_DECLARE
    void from_STAY__to__PATROL__F  ( KR_Event &event );
    void findEnemyFC               ( KR_Event &event );
    void from_PATROL__to__ATTACK__F( KR_Event &event );
    //}}END_OF_ACTIONF_DECLARE


    virtual void render   ( CViewDynamicList &list );
    virtual void endRender( CViewScene *scene );
    virtual CFVector3     realPosition();
    void                  onView(double time);

    void         loadStateTransitionTable();
};

#endif // ifndef __FLY_H__INCLUDED
/* End of file C:\NW\ARENA\OBASE\Fly\Fly.h */