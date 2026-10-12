/*
 * File  : C:\NW\ARENA\OBASE\Smoke\smoker.cpp
 * Autor :
 * Ver   1.0 
 */
#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "graph.h"
#include "smoker.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/fountmsg.h"
#include "message/smokermsg.h"
#include "h/light.h"
#include "h/phisics.h"
#include "h/cachesmoke.h"

 //===========================================================================
class AttributeSmoker : public ct_Attribute
{
 public:
    KR_ObjectID     m_smokeAttrID;
    ct_ClassTableID m_smokeTableID;

    GR_HTEXTURE     m_coronaHText;
    unsigned long   m_coronaColor;

    virtual void    update(double ts);

//{{ATTRIBUTE
    ct_AttrItem  m_array[19];
    double          m_createIncMin           ;  // 
    double          m_createIncMax           ;  // 
    ct_AttrStr      m_smokeAttrName          ;  // 
    double          m_maxTimeLife            ;  // 
    ct_AttrStr      m_smokeTableName         ;  // 
    int             m_onLand                 ;  // 
    int             m_useLight               ;  // 
    int             m_lightColor             ;  // 
    double          m_minLightBright         ;  // 
    double          m_maxLightBright         ;  // 
    double          m_lightRadius            ;  // 
    double          m_lightBrightStep        ;  // 
    double          m_lightOffset            ;  // 
    int             m_useCorona              ;  // 
    int             m_coronaRGB              ;  // 
    double          m_coronaR                ;  // 
    int             m_coronaAlpha            ;  // 
    ct_AttrStr      m_coronaName             ;  // 
    double          m_maxCoronaR             ;  // 

    AttributeSmoker()
    {
        m_createIncMin       = 0.1;
        m_createIncMax       = 0.3;
        strncpy(m_smokeAttrName,"Smoke.Attr.Def", sizeof( ct_AttrStr )-1 );
        m_maxTimeLife        = -1;
        strncpy(m_smokeTableName,"Smoke", sizeof( ct_AttrStr )-1 );
        m_onLand             = 0;
        m_useLight           = 0;
        m_lightColor         = 7;
        m_minLightBright     = 20;
        m_maxLightBright     = 255;
        m_lightRadius        = 10.0;
        m_lightBrightStep    = 30.0;
        m_lightOffset        = 2.0;
        m_useCorona          = 0;
        m_coronaRGB          = 0xFFFFFF;
        m_coronaR            = 0.2;
        m_coronaAlpha        = 255;
        strncpy(m_coronaName,"Smoke.spr", sizeof( ct_AttrStr )-1 );
        m_maxCoronaR         = 5;

        m_array[0].set("m_createIncMin",m_createIncMin);
        m_array[1].set("m_createIncMax",m_createIncMax);
        m_array[2].set("m_smokeAttrName",m_smokeAttrName);
        m_array[3].set("m_maxTimeLife",m_maxTimeLife);
        m_array[4].set("m_smokeTableName",m_smokeTableName);
        m_array[5].set("m_onLand",m_onLand);
        m_array[6].set("m_useLight",m_useLight);
        m_array[7].set("m_lightColor",m_lightColor);
        m_array[8].set("m_minLightBright",m_minLightBright);
        m_array[9].set("m_maxLightBright",m_maxLightBright);
        m_array[10].set("m_lightRadius",m_lightRadius);
        m_array[11].set("m_lightBrightStep",m_lightBrightStep);
        m_array[12].set("m_lightOffset",m_lightOffset);
        m_array[13].set("m_useCorona",m_useCorona);
        m_array[14].set("m_coronaRGB",m_coronaRGB);
        m_array[15].set("m_coronaR",m_coronaR);
        m_array[16].set("m_coronaAlpha",m_coronaAlpha);
        m_array[17].set("m_coronaName",m_coronaName);
        m_array[18].set("m_maxCoronaR",m_maxCoronaR);

        linkTable(m_array,19);
    }
//}}END_OF_ATTRIBUTE
};

 //===========================================================================
class SmokerTable : public ct_SubjectTable
{
 private:
    Smoker *m_table;
 public:
    SmokerTable()
    {
        m_table = NULL;
        registerClass( "Smoker" );
    }

    SmokerTable(char * name)
    {
        m_table = NULL;
        registerClass(name);
    }

    ~SmokerTable()
    {
        delete [] m_table;
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
    virtual  bool      isRendering();
};

 //===========================================================================
bool SmokerTable::isRendering()
{
     return true;
}

 //===========================================================================
class AttributeTableSmoker : public ct_AttributeTable
{
 protected:
    AttributeSmoker *m_table;

 public:
    AttributeTableSmoker()
    {
       m_table = NULL;
       registerClass( "SmokerAttr" );
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static SmokerTable  __classTable;
static SmokerTable  __classTable2("DynSmoker");
static AttributeTableSmoker __attrTable;
 /*********************************
  *
  *   Smoker implementation
  *
  *********************************/

 //============================================================
Smoker::Smoker()
 {
    m_attr = 0;
 }

 //============================================================
Smoker::~Smoker()
 {
 }

 //============================================================
void Smoker::onView(double ts)
{
    if(  !m_sendedMove  ) 
    {
          KR_Event event;
          event.label       = sm_EV_MOVE;
          event.source      = getObjectID();
          event.destination = getObjectID();
          event.timeStamp   = ts;
          issueEvent( event );
          m_sendedMove = true;
    }
}

CFVector3 Smoker::realPosition()
{
     return m_position;
}
 //============================================================
int Smoker::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case sm_EV_MOVE:
            if(  m_isVisible  )
            {
                 double ts = event.timeStamp;

                 event.timeStamp += context->rnd_f(
                                                    m_attr->m_createIncMin,
                                                    m_attr->m_createIncMax
                                                  );
                 issueEvent( event );
                 m_sendedMove = true;
                 createSmoke( m_position,
                              getObjectID(),
                              ts,
                              m_attr->m_smokeTableID,
                              m_attr->m_smokeAttrID
                             );

                 if(  m_attr->m_useLight  )
                 {
                      m_brightness +=  context->rnd_f(m_attr->m_lightBrightStep) 
                                     - m_attr->m_lightBrightStep/2;
                      if(  m_brightness < m_attr->m_minLightBright  )
                           m_brightness = m_attr->m_minLightBright;
                      else
                      if(  m_brightness > m_attr->m_maxLightBright  )
                           m_brightness = m_attr->m_maxLightBright;
                      
                 }
            }
            else m_sendedMove = false;

            break;

    case KR_WAKE_UP:
            break;

    case KR_SET_ATTR:
            s_ASSERTNQ("Smoker:receiveEvent:case KR_SET_ATTR: do't supported");
            break;

    case sm_EV_REMOVE:
            context->removeObject(getObjectID());
            break;

    case fou_EVCMD_START:
     {
            KR_ObjectID oID;
            event.data.open(EDO_READ)
                        .getObjectID(oID)
                        .getDouble(m_position.x)
                        .getDouble(m_position.y)
                        .getDouble(m_position.z)
                      .close();

            ct_Attribute *attr = __attrTable.searchAttribute(oID);
            if( attr==NULL )
                 echo( "Fountain::receiveEvent: Unknown attribute %s",
                       context->searchObject(oID));
            else m_attr = (AttributeSmoker*)attr;

            if(  m_attr->m_onLand  )
            {
                 double height;
                 CFVector3 normal;
                 CViewScene::Current()->GetTerrain()->GetPlane(m_position,normal,height);
                 m_position.y = height;
            }
     }
     m_sendedMove = false;
     m_startTime = event.timeStamp;
     
     if(   m_attr->m_maxTimeLife > 0  )
     {
           event.label       = sm_EV_REMOVE;
           event.source      = getObjectID();
           event.destination = getObjectID();
           event.timeStamp   = m_startTime + m_attr->m_maxTimeLife;
           issueEvent( event );
     }

     break;


    default: return 0;
    }
    return 1;
 }

 //============================================================
void Smoker::addNotify()
 {
    ct_Subject::addNotify();
    // insert your code this
    m_attr = 0;
    m_brightness = 0;
 }

 //============================================================
void Smoker::removeNotify()
 {
    ct_Subject::removeNotify();
    // insert your code this
 }


 /*************************************
  *
  *   SmokerTable implementation
  *
  *************************************/

 //============================================================
void SmokerTable::allocObjects( int objectQnty )
 {
    m_table = new Smoker[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void SmokerTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *SmokerTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"SmokerTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 /*************************************
  *
  *   AttributeTable implementation
  *
  *************************************/

 //============================================================
void AttributeTableSmoker::allocObjects( int objectQnty )
 {
    m_table = new AttributeSmoker[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void AttributeTableSmoker::freeObjects()
 {
    delete [] m_table;
    m_table         = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *AttributeTableSmoker::getObjectPTR( int index )
 {
    s_ASSERT(index>=0 && index <m_maxObjectQnty,"AttributeTable::getObjectPTR");
    return &(m_table[ index ]);
 }

void  AttributeSmoker::update(double)
{
    m_smokeTableID = g_arena.searchSeanceClassTable( m_smokeTableName );
    m_smokeAttrID = context->searchObject(m_smokeAttrName);

    m_coronaHText = g_loadSmoke( m_coronaName, NULL );
    m_coronaColor = GRTransparentColor(m_coronaRGB>>16,(m_coronaRGB>>8)&255,m_coronaRGB&255);
}


void Smoker::render( CViewDynamicList &list, double )
{
    if(  m_attr->m_useLight  )
         g_lightChain.add( m_position+CFVector3(0,m_attr->m_lightOffset,0), 
                      m_attr->m_lightColor, 
                      (int)(m_brightness), 
                      m_attr->m_lightRadius );

    if(  m_attr->m_useCorona  )
    {             
         double r = Abs2(CViewObject::m_viewPointInvMx.Offset()-realPosition())*m_attr->m_coronaR;
         if(  r > m_attr->m_maxCoronaR  )
              r = m_attr->m_maxCoronaR;
         m_corona.prepareToRender(
                                realPosition(),
                                r,
                                2<<16, 2<<16,
                                126<<16, 126<<16,
                                m_attr->m_coronaAlpha,
                                m_attr->m_coronaColor,
                                m_attr->m_coronaHText
                                 );
         list.Load( &m_corona );
    }
}

void Smoker::endRender( CViewScene *scene )
{
     if(  m_attr->m_useCorona  )
          scene->RemoveLandDynamic( &m_corona );
}


/* End of file C:\NW\ARENA\OBASE\Smoke\smoker.cpp */