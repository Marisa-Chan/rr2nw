/*
 * File  : C:\NW\ARENA\OBASE\Fly\Fly.cpp
 * Autor :
 * Ver   1.0 
 */
#include "Fly.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/flymsg.h"
 //===========================================================================
class AttributeFly : public ct_Attribute
{
 public:
    CViewObjectModel*   m_cacheSkin              ;  // 
    KR_ObjectID	        m_skinID;
    virtual void    update(double ts);  

//{{ATTRIBUTE
    ct_AttrItem  m_array[2];
    double          m_maxSpeed               ;  // 
    double          m_findEnemyDeltaT        ;  // 

    AttributeFly()
    {
        m_maxSpeed           = 30;
        m_findEnemyDeltaT    = 2;

        m_array[0].set("m_maxSpeed",m_maxSpeed);
        m_array[1].set("m_findEnemyDeltaT",m_findEnemyDeltaT);

        linkTable(m_array,2);
    }
//}}END_OF_ATTRIBUTE
};


 //===========================================================================
class FlyTable : public ct_SubjectTable
{
 private:
    Fly *m_table;
 public:
    FlyTable()
    {
        m_table = NULL;
        registerClass( "Fly" );
    }
    ~FlyTable()
    {
        delete [] m_table;
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
    virtual bool       isVisible();
};

 //===========================================================================
class AttributeTableFly : public ct_AttributeTable
{
 protected:
    AttributeFly *m_table;

 public:
    AttributeTableFly()
    {
       m_table = NULL;
       registerClass( "FlyAttr" );
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static FlyTable  __classTable;
static AttributeTableFly __attrTable;
 /*********************************
  *
  *   Fly implementation
  *
  *********************************/

bool FlyTable::isVisible()
{
  return true;
}



 //============================================================
Fly::Fly()
    : m_viewDynObj(m_skin)
 {
    m_attr = 0;
 }

 //============================================================
Fly::~Fly()
 {
 }

 //============================================================
int Fly::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case KR_WAKE_UP:
            break;

    case KR_SET_ATTR:
      {
        KR_ObjectID attrID;
        CFVector3   pos;

	event.data.open(EDO_READ)
                   .getObjectID(attrID)
                   .getDouble( pos.x )
                   .getDouble( pos.y )
                   .getDouble( pos.z )
                  .close();

         ct_Attribute *attr = __attrTable.searchAttribute(attrID);
         if( attr==NULL )
              echo( "Fly::receiveEvent: Unknown attribute %s",
                    context->searchObject(attrID));
         else m_attr = (AttributeBird*)attr;

        m_skin.Attach(m_attr->m_cacheSkin);
        m_viewDynObj.BumpDef().fRadius = m_skin.Model()->Radius();
      }
            break;

    default: return eventHandler( event, this );
    }
    return 1;
 }

 //============================================================
void Fly::addNotify()
 {
    loadStateTransitionTable();
    ct_Subject::addNotify();
    // insert your code this
 }

 //============================================================
void Fly::removeNotify()
 {
    ct_Subject::removeNotify();
    // insert your code this
 }

 //============================================================
void Fly::draw()
 {
 }

void Fly::from_STAY__to__PATROL__F( KR_Event &event )
{
    //{{GET_EVENT(fly_EVCMD_START_PATROL)
    //}}END_OF_GET_EVENT(fly_EVCMD_START_PATROL)
}

 /*******************************
  *
  * Приблизительно раз в 2 секунды (m_findEnemyDeltaT)
  *
  *******************************/
void Fly::findEnemyFC( KR_Event &event )
{
    //{{GET_EVENT(fly_EVC_FIND_ENEMY)
    //}}END_OF_GET_EVENT(fly_EVC_FIND_ENEMY)
}

void Fly::from_PATROL__to__ATTACK__F( KR_Event &event )
{
    //{{GET_EVENT(fly_EV_ENEMY_FOUNDED)
    //}}END_OF_GET_EVENT(fly_EV_ENEMY_FOUNDED)
}

//{{ACTIONF_IMPLEMENTATION

void Fly::loadStateTransitionTable()
 {
    //{{TRANSLATION_TABLE
    static KR_ActiveObject::StateTransitionTableElem row0[1] =
    {
        {fly_EVCMD_START_PATROL, ST_NEW_STATE,  1, (ACTION)from_STAY__to__PATROL__F   }
    };
    static KR_ActiveObject::StateTransitionTableElem row1[2] =
    {
        {fly_EV_ENEMY_FOUNDED  , ST_NEW_STATE,  2, (ACTION)from_PATROL__to__ATTACK__F },
        {fly_EVC_FIND_ENEMY    , ST_NEW_STATE,  1, (ACTION)findEnemyFC                }
    };
    //}}END_OF_TRANSLATION_TABLE{{
    static StateElem STT[3]={
      StateElem(1,row0),
      StateElem(2,row1),
      StateElem(0,NULL),
    };
    m_stateTable = STT;
    m_stateQnty  = 3;
    //}}END_OF_STATE_TABLE
 }
 /*************************************
  *
  *   FlyTable implementation
  *
  *************************************/

 //============================================================
void FlyTable::allocObjects( int objectQnty )
 {
    m_table = new Fly[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void FlyTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *FlyTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"FlyTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 /*************************************
  *
  *   AttributeTable implementation
  *
  *************************************/

 //============================================================
void AttributeTableFly::allocObjects( int objectQnty )
 {
    m_table = new AttributeFly[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void AttributeTableFly::freeObjects()
 {
    delete [] m_table;
    m_table         = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *AttributeTableFly::getObjectPTR( int index )
 {
    s_ASSERT(index>=0 && index <m_maxObjectQnty,"AttributeTable::getObjectPTR");
    return &(m_table[ index ]);
 }

void Fly::render   ( CViewDynamicList &list )
{
    CFMatrix3x4 &m = m_skin.GetDirModify();
    m.LoadIdentity();
    m.RotateOxL(m_rotateOx);
    m.RotateOyL(m_rotateOy);
    m.TranslateL( getPosition() );

    m_viewDynObj.prepareToRender();
    list.Load( &m_viewDynObj );
}


 //============================================================

void Fly::endRender( CViewScene *scene )
{
    scene->RemoveLandDynamic( &m_viewDynObj );
}

 //============================================================
CFVector3   Fly::realPosition() {  return getPosition();  }

void Fly::onView(double)
{
}

void AttributeFly::update(double ts)
{

   /*
    * Search and get skin
    */
    KR_ObjectID skinID =  context->searchObject( m_skinName );
    s_ASSERT( !skinID.isNUL(), "AttributeBird::update" );
    
    m_skinID = skinID;

    KR_Event event;
    event.timeStamp   = ts;
    event.label       = sk_EV_QUERY_MODEL_PTR;
    event.destination = skinID;
    context->sendEventNow( event );
    s_ASSERT(event.label==sk_EV_QUERY_MODEL_PTR_OK,"AttributeFly::update");
    event.data.open(EDO_READ)
                .get(&m_cacheSkin,sizeof(void*))
              .close();
}

/* End of file C:\NW\ARENA\OBASE\Fly\Fly.cpp */