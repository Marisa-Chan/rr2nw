/*
 * File  : C:\NW\ARENA\OBASE\StaticObj\StaticObj.cpp
 * Autor :
 * Ver   1.0 
 */
#include "StaticObj.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
 //===========================================================================
class AttributeStaticObj : public ct_Attribute
{
 public:

//{{ATTRIBUTE
    ct_AttrItem  m_array[5];
    double          m_speed0                 ;  // 
    double          m_speed1                 ;  // 
    double          m_speed2                 ;  // 
    double          m_minPhase               ;  // 
    double          m_maxPhase               ;  // 

    AttributeStaticObj()
    {
        m_speed0             = 1;
        m_speed1             = 2;
        m_speed2             = 3;
        m_minPhase           = 0;
        m_maxPhase           = 3;

        m_array[0].set("m_speed0",m_speed0);
        m_array[1].set("m_speed1",m_speed1);
        m_array[2].set("m_speed2",m_speed2);
        m_array[3].set("m_minPhase",m_minPhase);
        m_array[4].set("m_maxPhase",m_maxPhase);

        linkTable(m_array,5);
    }
//}}END_OF_ATTRIBUTE
};

static AttributeStaticObj __defaultAttr;

 //===========================================================================
class StaticObjTable : public ct_ClassTable
{
 private:
    StaticObj *m_table;
 public:
    StaticObjTable()
    {
        m_table = NULL;
        registerClass( "StaticObj" );
    }
    ~StaticObjTable()
    {
        delete [] m_table;
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

 //===========================================================================
class AttributeTableStaticObj : public ct_AttributeTable
{
 protected:
    AttributeStaticObj *m_table;

 public:
    AttributeTableStaticObj()
    {
       m_table = NULL;
       registerClass( "StaticObjAttr" );
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static StaticObjTable  __classTable;
static AttributeTableStaticObj __attrTable;
 /*********************************
  *
  *   StaticObj implementation
  *
  *********************************/

 //============================================================
StaticObj::StaticObj()
 {
    m_attr = &__defaultAttr;
 }

 //============================================================
StaticObj::~StaticObj()
 {
 }

 //============================================================
int StaticObj::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case KR_WAKE_UP:
            break;

    case KR_SET_ATTR:
            s_ASSERTNQ("StaticObj:receiveEvent:case KR_SET_ATTR: do't supported");
            break;

    default: return 0;
    }
    return 1;
 }

 //============================================================
void StaticObj::addNotify()
 {
    ct_Object::addNotify();
    // insert your code this
 }

 //============================================================
void StaticObj::removeNotify()
 {
    ct_Object::removeNotify();
    // insert your code this
 }

 //============================================================
void StaticObj::draw()
 {
 }

 /*************************************
  *
  *   StaticObjTable implementation
  *
  *************************************/

 //============================================================
void StaticObjTable::allocObjects( int objectQnty )
 {
    m_table = new StaticObj[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void StaticObjTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *StaticObjTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"StaticObjTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 /*************************************
  *
  *   AttributeTable implementation
  *
  *************************************/

 //============================================================
void AttributeTableStaticObj::allocObjects( int objectQnty )
 {
    m_table = new AttributeStaticObj[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void AttributeTableStaticObj::freeObjects()
 {
    delete [] m_table;
    m_table         = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *AttributeTableStaticObj::getObjectPTR( int index )
 {
    s_ASSERT(index>=0 && index <m_maxObjectQnty,"AttributeTable::getObjectPTR");
    return &(m_table[ index ]);
 }

void *StaticObj::queryInterface( int IID )
{
    switch( IID )
    {
    case IStaticObjIID: return &m_callBackData;
    }

    return 0;
}

/* End of file C:\NW\ARENA\OBASE\StaticObj\StaticObj.cpp */
