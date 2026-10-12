/*
 * File   : D:\GAME\OBASE\Skin\skin.h
 * Author : Suavik
 * Ver   1.0 
 */
#ifndef __SKIN_H__INCLUDED
#define __SKIN_H__INCLUDED

#define LAST_H__VIEW
#include "game.h"
#include "scene.h"

#include "storage/h/subject.h"
#include "kernel/h/active.h"
#include "i/skin.i"
#include "animpeop.h"

//===================================================================================
class AniCell
{
 public:
    CViewBaseModifier *m_fs;
    bool               m_use;
};

//===================================================================================
class AniCell0
{
 public:
    CViewBaseModifier0 *m_fs;
    bool                m_use;
};

//===================================================================================
class Skin : public ct_Object, public ISkin
{
 public:
    CViewObjectModel m_model; //CViewBaseModifier0
    bool             m_loaded;
    int              m_aniCnt,
                     m_aniCnt0;
    AniCell         *m_array;
    AniCell0        *m_array0;
    int              m_reducNum;
    int              m_isAutoAnim;
    AnimateInfo     *m_animProg;
    int              m_animProgCnt;
    int              m_animProgSP;

    void     startInitialize()
    {
         m_loaded      = false;
         m_array       = 0;
         m_array0      = 0;
         m_aniCnt      = 0;
         m_aniCnt0     = 0;
         m_reducNum    = 1;
         m_isAutoAnim  = 0;
         m_animProg    = 0;
         m_animProgCnt = 0;
         m_animProgSP  = 0;
    }

    void                load         ( const char *fileName );
    void                createAniSets( int aniCnt, int aniCnt0 );
    void                createAniSets( int aniCnt, int aniCnt0, int progLen );
    void                removeAniSets();
    void                set ( int set, const char *name );
    void                set0( int set, const char *name );
    CViewBaseModifier  *get ( int set, int reduc ); 
    CViewBaseModifier0 *get0( int set, int reduc ); 
    void                runAutoProg( CViewObjectBaseSet *bs, CViewObjectBase *ob, int reduc );
    void                skinSetAnimAuto(CViewObjectRef *ref);
    int                 isAutoAnim();

             Skin();
    virtual ~Skin();
    virtual int  receiveEvent( KR_Event &event );
    virtual void addNotify   ();
    virtual void removeNotify();
    virtual void *queryInterface(int iid );
    virtual bool  shouldDump () { return false; } 
};


#endif // ifndef __SKIN_H__INCLUDED
/* End of file D:\GAME\OBASE\Skin\skin.h */
