/*
 * File  : C:\NW\ARENA\OBASE\StaticObj\StaticObj.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __STATICOBJ_H__INCLUDED
#define __STATICOBJ_H__INCLUDED

#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"
#include "i/skin.i"
#include "i/staticobj.i"


class AttributeStaticObj;
class StaticObj : public ct_Object
{
 public:
    AttributeStaticObj   *m_attr;
    IStaticObj            m_callBackData;
    /*
    ISkin                *m_askin;
    double                m_startTime;
    KR_ObjectID           m_skinID;
    */
             StaticObj();
    virtual ~StaticObj();
    virtual void*queryInterface( int IID );
    virtual int  receiveEvent( KR_Event &event );
    virtual void draw        ();
    virtual void addNotify   ();
    virtual void removeNotify();
    virtual bool	shouldDump () { return false; } 
};

#endif // ifndef __STATICOBJ_H__INCLUDED
/* End of file C:\NW\ARENA\OBASE\StaticObj\StaticObj.h */