/*
 * File  : C:\NW\ARENA\OBASE\Smoke\Smoke.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __SMOKE_H__INCLUDED
#define __SMOKE_H__INCLUDED

#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"
#include "storage/h/savefile.h"

#define MAX_ITER_WAIT 100

class Smoke;
class s_SmokeObject : public  CViewSphericDynamic
 {
 public:
        Smoke   *m_master;
        bool     m_visible;
        int      m_z;

        s_SmokeObject()
        {
            m_master = 0;
        }

    void    prepareToRender();
	virtual void Draw();
 };


class SmokeBlob
{
 public:
     double        m_phase;
     unsigned long m_color;
     int u0, v0, u1, v1;

     CFVector3     m_startPos;
     CFVector3     m_position;
     CFVector3     m_dir;
     CFVector3     m_ofsDir;
     
     double        m_dirIncrement;
     double        m_maxTimeLife;
     double        m_radius;
     int           m_alpha;
     GR_HTEXTURE    m_ref;
     double        rA;
     double        rB;
     double        rC;
     double        tA; // alpha = tA*t*t + tB*t + tC
     double        tB;
     double        tC;
     double        a0,a1,a2,a3;
     
     
     SmokeBlob()
     {
     m_phase     = 0;
     m_color     = 0;
     m_startPos  = CFVector3(0,0,0);
     m_ofsDir    = CFVector3(1,1,0);
     m_dirIncrement = 0;
     m_maxTimeLife  = 0;
     rA = 0;
     rB = 0;
     rC = 2.0;
     tA = 0; // alpha = tA*t*t + tB*t + tC
     tB = 0;
     tC = 2.0;
     }
};

class CDC;
class AttributeSmoke;


typedef struct {
enum
    {
      MAXSMOKEBLOB = 4
    };

	KR_ObjectID		  m_smokeAttrID;
	int               m_viewIter;
	SmokeBlob         m_blob[MAXSMOKEBLOB];
    CFVector3         m_pos;
    double            m_prevTimeStamp;
    int               m_setRemove;
    int				  m_cnt;

} SmokeData;

class Smoke :	public ct_Subject,
				public SmokeData
{

	void setSmokeAttr();
 
public:
    
    
    AttributeSmoke   *m_attr;
    s_SmokeObject     m_viewObj;
    
    int      addBlob();
    void     delBlob(int index);

             Smoke();
    virtual ~Smoke();
    virtual int  receiveEvent( KR_Event &event );
    virtual void draw        ( CDC &gc );
    virtual void draw        ();
    virtual void addNotify   ();
    virtual void removeNotify();
    virtual void onHide(double ts);

    virtual void render( CViewDynamicList &list, double ts );
    virtual void endRender( CViewScene *scene );
    virtual CFVector3     realPosition();

    void    preCreate(SmokeBlob &b);
    void    onCreate();
    void    onCreate( const CFVector3 &dir );
    int     onMove( double t );

	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
	virtual void	loadNotify();
	virtual bool	shouldDump () { return true; } 	
};

#endif // ifndef __SMOKE_H__INCLUDED
/* End of file C:\NW\ARENA\OBASE\Smoke\Smoke.h */