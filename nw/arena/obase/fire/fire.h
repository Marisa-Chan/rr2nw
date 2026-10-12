/*
 * File  : C:\NW\ARENA\OBASE\Fire\Fire.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __FIRE_H__INCLUDED
#define __FIRE_H__INCLUDED

#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"

//====================================================================================
class Fire;
class s_FireObject : public  CViewSphericDynamic
 {
 public:
    double m_radius;
    Fire  *m_master;

    s_FireObject()
    {
        m_master = 0;
    }
    void    prepareToRender();
	virtual void Draw();
 };


//====================================================================================
class FireBranch
{
 public:
    double m_phase;
    double m_timeOfLife;
    unsigned long m_color;
    FireBranch *m_next;
    FireBranch *m_prev;
    FireBranch *deleteCommand;

    FireBranch()
    {
       m_phase       = 0;
       m_timeOfLife  = 1;
       m_next        = 0;
       m_prev        = 0;
       deleteCommand = 0;
    }
 };

class AttributeFire;
class Fire : public ct_Subject
{
 public:
    CFVector3              m_position;
    s_FireObject           m_viewObj;
    const AttributeFire   *m_attr;
    FireBranch             m_branchList;
    int                    m_branchCnt;
    double                 m_prevTimeStamp;
    bool                   m_moved;

    enum
    {
         MAX_BRANCH = 1000
    };

    static FireBranch  m_branch[MAX_BRANCH];
    static FireBranch *m_freeList;

             Fire();
    virtual ~Fire();
    virtual int  receiveEvent( KR_Event &event );
    virtual void draw        ();
    virtual void addNotify   ();
    virtual void removeNotify();

    virtual void render   ( CViewDynamicList &list, double ts );
    virtual void endRender( CViewScene *scene );
    virtual CFVector3     realPosition();

    
    static void  createFreeList();
    void         addBranch( double time0 );
    void         delCommand( FireBranch *node, int &deleteCommandCnt )
    {
        m_branch[deleteCommandCnt].deleteCommand = node;
        deleteCommandCnt++;
    }
    void         delBranch( FireBranch *node );
    void         deleteBranches( int deleteCommandCnt );

    virtual void          onView(double time);
    void                  onMove( double ts );
};

#endif // ifndef __FIRE_H__INCLUDED
/* End of file C:\NW\ARENA\OBASE\Fire\Fire.h */
