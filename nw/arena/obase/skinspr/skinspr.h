/*
 * File  : C:\NW\ARENA\OBASE\SkinSpr\SkinSpr.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __SKINSPR_H__INCLUDED
#define __SKINSPR_H__INCLUDED

#include "storage/h/subject.h"
#include "kernel/h/active.h"

#define LAST_H__VIEW
#include "game.h"


class SkinSpr : public ct_Object
{
 public:
    CViewTexture m_texture;
    bool         m_loaded;

    void startInitialize()
    {
        m_loaded = false;
    }

             SkinSpr();
    virtual ~SkinSpr();
    virtual int  receiveEvent( KR_Event &event );
    virtual void addNotify   ();
    virtual void removeNotify();
    virtual bool	shouldDump () { return false; } 
};

#endif // ifndef __SKINSPR_H__INCLUDED
/* End of file C:\NW\ARENA\OBASE\SkinSpr\SkinSpr.h */