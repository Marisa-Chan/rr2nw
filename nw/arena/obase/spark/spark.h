/*
 * File  : D:\GAME\OBASE\Spark\Spark.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __SPARK_H__INCLUDED
#define __SPARK_H__INCLUDED

#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "kernel/h/active.h"

#define LAST_H__VIEW
#include "game.h"
#include "..\DynObj\DynSpr.h"


#define sp_TIME_INCREMENT (0.04)

class CDC;
class AttributeSpark;

class SparkPhase
{
public:
    int u0,v0,u1,v1;
    double time;

    int  brightness, color;
    double radius;

    void init( int u0l, int v0l, int u1l, int v1l, double t,
               int b, int c, double r )
    {
         u0 = u0l;
         v0 = v0l;
         u1 = u1l;
         v1 = v1l;
         time = t;
         brightness = b;
         color      = c;
         radius     = r;
    }
};

class Spark : public ct_Subject
{
 public:
    enum
    {
         MAX_PHASE = 15
    };

    AttributeSpark        *m_attr;
    CFVector3              m_position;
    int                    m_curPhase;

    s_ViewDynamicSprite    m_viewDynSpr;

             Spark();
    virtual ~Spark();
    virtual int  receiveEvent( KR_Event &event );
    virtual void draw        ( CDC &gc );
    virtual void addNotify   ();
    virtual void removeNotify();
    virtual CFVector3 realPosition();
    virtual void render( CViewDynamicList &list, double ts );
    virtual void endRender( CViewScene *scene );
   
    virtual bool shouldDump () { return true; } // see comments for explosions
};

#endif // ifndef __SPARK_H__INCLUDED
/* End of file D:\GAME\OBASE\Spark\Spark.h */