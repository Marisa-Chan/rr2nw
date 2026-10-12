/*
 * File  :  C:\NW\ARENA\OBASE\Fountain\Fountain.h
 * Author : Suavik
 * Ver   1.0 
 */
#ifndef __FOUNTAIN_H__INCLUDED
#define __FOUNTAIN_H__INCLUDED

#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"

class CDC;



//====================================================================================
class Fountain;
class s_FountainObject : public  CViewSphericDynamic
 {
 public:
        Fountain *m_master;
        s_FountainObject()
        {
            m_master = 0;
        }
    void    prepareToRender();
	virtual void Draw();
 };


typedef struct {
	 double m_phase;
     double xT, zT, yT; // x = xT*t; 
                        // z = zT*t;  
                        // y = yT*t - t*t*g/2
     double rA, rB, rC; // r = rA*t*t + rB*t + rC
     double m_timeOfLife;
     unsigned long m_color;
} FountBranchData;

//====================================================================================
class FountBranch : public FountBranchData
{
 public:
     
     FountBranch *m_next;
     FountBranch *m_prev;
     FountBranch *deleteCommand;

    FountBranch()
    {
       m_phase = 0;
       xT = 0;
       zT = 0;
       yT = 0;
       rA = 1;
       rB = 1;
       rC = 1;
       m_timeOfLife = 1;
       m_next = 0;
       m_prev = 0;
       deleteCommand = 0;
    }

	bool	dump(PIN_SaveFile & sf);
	bool	load(PIN_SaveFile & );
 };

//====================================================================================
class AttributeFountain;


typedef struct {
	KR_ObjectID			 m_fountainAttrID;
	CFVector3            m_fountainPosition;
	double               m_prevTimeStamp;
    double               m_brightness;
    double               m_addSlipPeriod;    
} FountainData;

class Fountain : public ct_Subject,
				 public FountainData
{
 
 void setFountainAttr();

 public:
    
    s_FountainObject     m_viewObj;
    AttributeFountain   *m_attr;    
    FountBranch          m_branchList;
	int                  m_branchCnt;

    enum
    {
         MAX_BRANCH = 2000
    };

    static FountBranch  m_branch[MAX_BRANCH];
    static FountBranch *m_freeList;
	static void  createFreeList();

             Fountain();
    virtual ~Fountain();
    virtual int  receiveEvent( KR_Event &event );
    void         moving( FountBranch &br, int &dc );
    virtual void draw        ( CDC &gc );
    virtual void draw        ();
    virtual void addNotify   ();
    virtual void removeNotify();
    virtual void render   ( CViewDynamicList &list, double ts );
    virtual void endRender( CViewScene *scene );
    virtual CFVector3     realPosition();

    
 
	void          addBranch( double time0 );
	FountBranch * addBranch();
    void         delCommand( FountBranch *node, int &deleteCommandCnt )
    {
        m_branch[deleteCommandCnt].deleteCommand = node;
        deleteCommandCnt++;
    }
    void         delBranch( FountBranch *node );

	virtual bool	shouldDump () { return true; } 	
	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
	virtual void	loadNotify();  
};

#endif // ifndef __FOUNTAIN_H__INCLUDED
/* End of file C:\NW\ARENA\OBASE\Fountain\Fountain.h */
