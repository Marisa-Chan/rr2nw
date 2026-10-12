/*
 * File  : C:\NW\ARENA\OBASE\Explosion\Explosion.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __EXPLOSION_H__INCLUDED
#define __EXPLOSION_H__INCLUDED

#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "..\DynObj\DynObj.h"
#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"

typedef enum
{
    expl_PARTICLE_SIMPLE,
    expl_PARTICLE_SNAKE,
    expl_PIECE_SIMPLE,
    expl_PIECE_WITH_SMOKE,
    expl_RAY,
    expl_SMOKE

} EXPLOSION_BRANCH_TYPE;

//====================================================================================
class Explosion;
class s_ExplosionObject : public  CViewSphericDynamic
 {
 public:
        Explosion *m_master;
        s_ExplosionObject()
        {
            m_master = 0;
        }
    void    prepareToRender();
	virtual void Draw();
 };


//====================================================================================
class ExplBranch
{
 public:
    EXPLOSION_BRANCH_TYPE m_type;

    double xT, zT, yT; // x = xT*t; 
                       // z = zT*t;  
                       // y = yT*t - t*t*g/2
    double rA, rB, rC; // r = rA*t*t + rB*t + rC
    double tA, tB, tC;
    int    u0,v0,u1,v1;
    double m_timeOfLife;
    double m_mulSpeed;
    unsigned long m_color;
    ExplBranch *m_next;
    ExplBranch *m_prev;
    ExplBranch *m_deleted;
    CFVector3   m_startPos;
    double      m_d_maxTime;
    CFVector3   m_ofsDir;
    double      m_rotOys, m_rotOxs;
    int         m_tailCnt;
    int		m_createPuffNow;
    
    CViewObjectRef         m_skin;
    s_ViewDynamicObject    m_viewDynObj;


    ExplBranch()
     : m_viewDynObj(m_skin)
    {
       xT = 0;
       zT = 0;
       yT = 0;
       rA = 1;
       rB = 1;
       rC = 1;
       m_timeOfLife = 1;
       m_next = 0;
       m_prev = 0;
       m_deleted = 0;
	   m_createPuffNow = 0;
    }
 };


class AttributeExplosion;
class Explosion : public ct_Subject
{
 public:
        
    AttributeExplosion   *m_attr;
    KR_ObjectID           m_snd;    
    KR_ObjectID           m_fromID;

    enum
    {
       MAX_BRANCH = 500
    };
    ExplBranch           m_list;
    static ExplBranch   *m_free;
    static ExplBranch    m_branch[MAX_BRANCH];
    double               m_prevTime;
    CFVector3            m_position;
    double               m_startTime;
    double    T, T2, d_Z, Z;
    double               m_landY;
    int                  m_started;

	static	int			 m_explsWithTraces;
	int					 m_hasTraces;

    s_ExplosionObject    m_viewObj;

    ExplBranch *add();
    void        del( ExplBranch &b, int &cnt );
    void        updateDel( int cnt );

    static  void createFreeList();
    void        removeAll();

             Explosion();
    virtual ~Explosion();
    virtual int  receiveEvent( KR_Event &event );
    virtual void draw        ();
    virtual void addNotify   ();
    virtual void removeNotify();

    virtual void render   ( CViewDynamicList &list, double ts );
    virtual void endRender( CViewScene *scene );
    virtual CFVector3     realPosition();

    void setDamage( double ts );

    void onCreatePARTICLE_SIMPLE ( ExplBranch &b );
    void onCreatePARTICLE_SNAKE  ( ExplBranch &b );
    void onCreatePIECE_SIMPLE    ( ExplBranch &b );
    void onCreatePIECE_WITH_SMOKE( ExplBranch &b );
    void onCreateSMOKE           ( ExplBranch &b );
    void onCreateRAY             ( ExplBranch &b );

    void onMovePARTICLE_SIMPLE   ( ExplBranch &b, int &delCnt );
    void onMovePARTICLE_SNAKE    ( ExplBranch &b, int &delCnt );
    void onMovePIECE_SIMPLE      ( ExplBranch &b, int &delCnt );
    void onMovePIECE_WITH_SMOKE  ( ExplBranch &b, int &delCnt );
    void onMoveSMOKE             ( ExplBranch &b, int &delCnt );

    void onDrawPARTICLE_SIMPLE   ( ExplBranch &b, int &delCnt );
    void onDrawPARTICLE_SNAKE    ( ExplBranch &b, int &delCnt );
    void onDrawPIECE_SIMPLE      ( CViewDynamicList &list, ExplBranch &b, int &delCnt );
    void onDrawPIECE_WITH_SMOKE  ( CViewDynamicList &list, ExplBranch &b, int &delCnt );
    void onDrawSMOKE             ( ExplBranch &b, int &delCnt );
    void onDrawRAY               ( ExplBranch &b, int &delCnt );

	// Actually we don't need to save such objects as explosions
	// and some other visual effects. The only thing we need to do
	// is to remove such objects from previous sessions. Load routine
	// will automatically do this for us if shouldDump returns true.

	virtual bool	shouldDump () { return true; } 	
	virtual bool	dump(PIN_SaveFile & ) { return true; }
	virtual bool	load(PIN_SaveFile & ) { return true; };
	virtual void	loadNotify() {};

};

#endif // ifndef __EXPLOSION_H__INCLUDED
/* End of file C:\NW\ARENA\OBASE\Explosion\Explosion.h */