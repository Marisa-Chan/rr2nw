/*
 * File  : D:\GAME\OBASE\Spark\Spark.cpp
 * Autor :
 * Ver   1.0 
 */
#include "Spark.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/sparkmsg.h"
#include "message/skinmsg.h"
#include "enum/spaceenum.h"
#include "scene.h"
#include "h/light.h"

#ifdef  __TRACE_NW__
#include "afxwin.h"
#else
class CDC{};
#endif


//{{EVENT_LABEL_NAMES
static s_ELN elnTable[]=
{
   s_ELN(sp_EVC_LIFE,"sp_EVC_LIFE"),
   s_ELN(sp_EV_CREATE,"sp_EV_CREATE"),
   s_ELN(sp_EV_SET_PHASE_COUNT,"sp_EV_SET_PHASE_COUNT"),
   s_ELN(sp_EV_SET_PHASE,"sp_EV_SET_PHASE"),
   s_ELN(),
};

static s_ELNTable selnTable("Default",elnTable);

//}}END_OF_EVENT_LABEL_NAMES

 //===========================================================================
class AttributeSpark : public ct_Attribute
{
 public:
    virtual void update(double ts);
    virtual int  receiveEvent( KR_Event &event );
    SparkPhase             m_phase[Spark::MAX_PHASE];
    int                    m_phaseCnt;
//{{ATTRIBUTE
    ct_AttrItem  m_array[3];
    double          m_maxRadius              ;  // Максимальный радиус
    ct_AttrStr      m_skin                   ;  // 
    CViewTexture*   m_cacheSkin              ;  // 

    AttributeSpark()
    {
        m_maxRadius          = 2.5;
        strncpy(m_skin,"sk.Fusion.0", sizeof( ct_AttrStr )-1 );
        m_cacheSkin          = 0;

        m_array[0].set("m_maxRadius",m_maxRadius);
        m_array[1].set("m_skin",m_skin);
        m_array[2].set("m_cacheSkin",EDI_NONE,&m_cacheSkin);

        linkTable(m_array,3);
        m_phaseCnt = 1;
        m_phase[0].init(2,2,24*2-1, 45*2-2,0.8,200,0,1);
    }
//}}END_OF_ATTRIBUTE
/*
        m_phaseCnt = 1;
        m_phase[0].init(2,2,24*2-1, 45*2-2,0.8);

 */
};

static AttributeSpark __defaultAttr;

 //===========================================================================
class SparkTable : public ct_SubjectTable
{
 private:
    Spark *m_table;
 public:
    SparkTable()
    {
        m_table = NULL;
        registerClass( "Spark" );
    }
    ~SparkTable()
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
class AttributeTableSpark : public ct_AttributeTable
{
 protected:
    AttributeSpark *m_table;

 public:
    AttributeTableSpark()
    {
       m_table = NULL;
       registerClass( "SparkAttr" );
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static SparkTable          __classTable;
static AttributeTableSpark __attrTable;

 /*********************************
  *
  *   Spark implementation
  *
  *********************************/

 //============================================================
Spark::Spark()
 {
    m_attr = &__defaultAttr;
    m_curPhase = 0;
 }

 //============================================================
Spark::~Spark()
 {
 }

 //============================================================
int Spark::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case KR_WAKE_UP:
            break;

    case sp_EV_CREATE:
            {
            int attrIndex;

            //{{GET_EVENT(sp_EV_CREATE)
            event.data.open(EDO_READ)
                           .descend(VECTOR3D_F , 0 )
                             .getDouble(m_position.x)
                             .getDouble(m_position.y)
                             .getDouble(m_position.z)
                           .ascend()
                           .getInt(attrIndex)
                      .close();
            //}}END_OF_GET_EVENT(sp_EV_CREATE)
            __attrTable.setAttribute(attrIndex,(ct_Attribute*&)m_attr);

            event.label      = sp_EVC_LIFE;
            event.source     = getObjectID();
            event.timeStamp += m_attr->m_phase[0].time;
            //{{PUT_EVENT(sp_EVC_LIFE)
            //}}END_OF_PUT_EVENT(sp_EVC_LIFE)
            issueEvent( event );
            m_curPhase = 0;
            }
            break;

    case sp_EVC_LIFE:
            {
            //{{GET_EVENT(sp_EVC_LIFE)
            //}}END_OF_GET_EVENT(sp_EVC_LIFE)
            if(  m_curPhase >= m_attr->m_phaseCnt-1  )
                 getContext()->removeObject(getObjectID());
            else
            {
                 event.source     = getObjectID();
                 event.timeStamp += m_attr->m_phase[m_curPhase].time;
                 //{{PUT_EVENT(sp_EVC_LIFE)
                 //}}END_OF_PUT_EVENT(sp_EVC_LIFE)
                 issueEvent( event );

                 m_curPhase++;
            }

            }
            break;

    case KR_SET_ATTR:
            {
            KR_ObjectID oID;
            event.data.open(EDO_READ)
                        .getObjectID(oID)
                      .close();
            ct_Attribute *attr = __attrTable.searchAttribute(oID);
            if( attr==NULL )
                 echo( "Spark::receiveEvent: Unknown attribute %s",
                       context->searchObject(oID));
            else m_attr = (AttributeSpark*)attr;
            m_viewDynSpr.initRef( ((AttributeSpark*)attr)->m_cacheSkin );
            }
            break;


    default: return 0;
    }
    return 1;
 }

 //============================================================
void Spark::addNotify()
 {
    ct_Object::addNotify();
    // insert your code this
 }

 //============================================================
void Spark::removeNotify()
 {
    ct_Object::removeNotify();
    // insert your code this
 }

 //============================================================
#ifdef  __TRACE_NW__
void Spark::draw( CDC &gc )
 {
    switch( m_attr->m_skin )
    {
    case 0:
            {
            CBrush brush(RGB(255,255,0));
            CBrush *ob = gc.SelectObject(&brush);
            double x =  m_position.x,
                   y = -m_position.z;
            double r = (m_curTime-m_startTime)*10 / (m_attr->m_timeOfLife*2);
            gc.Ellipse( (int)(x-r), (int)(y-r), (int)(x+r), (int)(y+r) );
            gc.SelectObject(ob);
            }
            break;
    default: s_ASSERTNQ("Spark::draw()");
    }
 }
#else
void Spark::draw( CDC & ) {}
#endif

CFVector3 Spark::realPosition()
 {
    return m_position;
 }

 /*************************************
  *
  *   SparkTable implementation
  *
  *************************************/

 //============================================================
void SparkTable::allocObjects( int objectQnty )
 {
    m_table = new Spark[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void SparkTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *SparkTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"SparkTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 //============================================================
bool SparkTable::isRendering()
 {
    return true;
 }

 /*************************************
  *
  *   AttributeTable implementation
  *
  *************************************/

 //============================================================
void AttributeSpark::update(double ts)
 {
    KR_Event event;

    KR_ObjectID spr = context->searchObject(m_skin);
    s_ASSERT1(!spr.isNUL(),"AttributeSpark::update. Unknown texture object %s",m_skin);

    event.label       = sk_EV_QUERY_MODEL_PTR;
    event.timeStamp   = ts;
    event.source      = getObjectID();
    event.destination = spr;
    context->sendEventNow( event );
    s_ASSERT(event.label==sk_EV_QUERY_MODEL_PTR_OK,"AttributeSpark::update");
    event.data.open(EDO_READ)
                 .get(&m_cacheSkin,sizeof(void*))
              .close();
 }

 //============================================================
void AttributeTableSpark::allocObjects( int objectQnty )
 {
    m_table = new AttributeSpark[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void AttributeTableSpark::freeObjects()
 {
    delete [] m_table;
    m_table         = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *AttributeTableSpark::getObjectPTR( int index )
 {
    s_ASSERT(index>=0 && index <m_maxObjectQnty,"AttributeTable::getObjectPTR");
    return &(m_table[ index ]);
 }



 //============================================================
void Spark::render(  CViewDynamicList &list, double  )
 {
    const SparkPhase &cur = m_attr->m_phase[m_curPhase];
    m_viewDynSpr.prepareToRender(m_position,m_attr->m_maxRadius, 
              cur.u0,cur.v0, cur.u1, cur.v1, 
              m_attr->m_cacheSkin);
    list.Load( &m_viewDynSpr );
    g_lightChain.add( m_position, 
                      cur.color, 
                      cur.brightness, 
                      cur.radius );
 }

 //============================================================
void Spark::endRender( CViewScene *scene )
 {
    scene->RemoveLandDynamic( &m_viewDynSpr );
 }


int  AttributeSpark::receiveEvent( KR_Event &event )
 {
    if( !ct_Attribute::receiveEvent(event) )
    switch( event.label )
    {
    case sp_EV_SET_PHASE_COUNT:
            {
            int count;
            //{{GET_EVENT(sp_EV_SET_PHASE_COUNT)
            event.data.open(EDO_READ)
                           .getInt(count)
                      .close();
            //}}END_OF_GET_EVENT(sp_EV_SET_PHASE_COUNT)
            s_ASSERT(count <= Spark::MAX_PHASE && count >=0,"Spark::receiveEvent::sp_EV_SET_PHASE_COUNT");
            m_phaseCnt = count;
            }
            break;

    case sp_EV_SET_PHASE:
            {
            int u0,v0,u1,v1,num;
            double time;
            int brightness;
            int color;
            double radius;
            //{{GET_EVENT(sp_EV_SET_PHASE)
            event.data.open(EDO_READ)
                           .getInt(num)
                           .descend( RECT2D_I, 0 )
                             .getInt(u0)
                             .getInt(v0)
                             .getInt(u1)
                             .getInt(v1)
                           .ascend()
                           .getDouble(time)
                           .getInt(brightness)
                           .getInt(color)
                           .getDouble(radius)
                      .close();
            //}}END_OF_GET_EVENT(sp_EV_SET_PHASE)
            s_ASSERT(num>=0 && num < m_phaseCnt,"Spark::receiveEvent::sp_EV_SET_PHASE");
            m_phase[num].init(u0,v0,u1,v1,time,brightness,color,radius);
            }
            break;
    default: return 0;
    }
    return 1;
 }

/* End of file D:\GAME\OBASE\Spark\Spark.cpp */
