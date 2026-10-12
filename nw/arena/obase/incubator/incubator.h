/*
 * File  : C:\NW\ARENA\OBASE\Incubator\Incubator.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __INCUBATOR_H__INCLUDED
#define __INCUBATOR_H__INCLUDED

#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"

class AttributeIncubator;
class Incubator : public ct_Subject
{
 public:
    AttributeIncubator   *m_attr;
    CFVector3             m_position;
    KR_ObjectID           m_friend;

             Incubator();
    virtual ~Incubator();
    virtual int  receiveEvent( KR_Event &event );
    virtual void draw        ();
    virtual void addNotify   ();
    virtual void removeNotify();

    virtual void          render      ( CViewDynamicList &list );
    virtual void          endRender   ( CViewScene *scene );
    virtual CFVector3     realPosition();

    bool ready();

    void createObject( 
                       ct_ClassTableID    ctID, 
                       const char        *name, 
                       const KR_ObjectID  &attrID,
                       double              ts
                     );
    void createPeople( 
                       ct_ClassTableID    ctID, 
                       const char        *name, 
                       const KR_ObjectID &attrID,
                       const char        *routeName,
                       double             ts
                     );

    virtual bool shouldDump () { return false; } // we don't dump attributes
};

#endif // ifndef __INCUBATOR_H__INCLUDED
/* End of file C:\NW\ARENA\OBASE\Incubator\Incubator.h */