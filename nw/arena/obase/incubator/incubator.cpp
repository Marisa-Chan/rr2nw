/*
 * File  : C:\NW\ARENA\OBASE\Incubator\Incubator.cpp
 * Autor :
 * Ver   1.0 
 */
#include "Incubator.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/incubmsg.h"
#include "message/peopmsg.h"

 //===========================================================================
class AttributeIncubator : public ct_Attribute
{
 public:
//{{ATTRIBUTE
    ct_AttrItem  m_array[2];
    int             m_isHidden               ;  // 
    double          m_nextTimeInc            ;  // 

    AttributeIncubator()
    {
        m_isHidden           = 0;
        m_nextTimeInc        = 1;

        m_array[0].set("m_isHidden",m_isHidden);
        m_array[1].set("m_nextTimeInc",m_nextTimeInc);

        linkTable(m_array,2);
    }
//}}END_OF_ATTRIBUTE
};

static AttributeIncubator __defaultAttr;

 //===========================================================================
class IncubatorTable : public ct_SubjectTable
{
 private:
    Incubator *m_table;
 public:
    IncubatorTable()
    {
        m_table = NULL;
        registerClass( "Incubator" );
    }
    ~IncubatorTable()
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
class AttributeTableIncubator : public ct_AttributeTable
{
 protected:
    AttributeIncubator *m_table;

 public:
    AttributeTableIncubator()
    {
       m_table = NULL;
       registerClass( "IncubAttr" );
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static IncubatorTable  __classTable;
static AttributeTableIncubator __attrTable;

bool IncubatorTable::isRendering()
{
   return true;
}

 /*********************************
  *
  *   Incubator implementation
  *
  *********************************/

 //============================================================
Incubator::Incubator()
 {
    m_attr = &__defaultAttr;
 }

 //============================================================
Incubator::~Incubator()
 {
 }

 //============================================================
bool Incubator::ready()
 {
   return !( m_attr->m_isHidden ^ m_isVisible);
 }

 //============================================================
int Incubator::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case KR_WAKE_UP:
            break;

    case KR_SET_ATTR:
            {
            KR_ObjectID oID;

            event.data.open(EDO_READ)
                        .getObjectID(oID)
                        .getDouble(m_position.x)
                        .getDouble(m_position.y)
                        .getDouble(m_position.z)
                      .close();

            ct_Attribute *attr = __attrTable.searchAttribute(oID);
            m_attr = (AttributeIncubator*)attr;

            }
            break;

    case incub_CREATE_IS:
     {
         if(  ready()  )
         {
              ct_ClassTableID    ctID;
              char   name[40];
              char   attrName[80];

              event.data.open(EDO_READ)
                     .getInt( ctID ) // идентификатор таблицы
                     .getStr( name, sizeof(name) )
                     .getStr( attrName, sizeof(attrName) )
                   .close();

              createObject( ctID, name, context->searchObject(attrName), event.timeStamp );
         }
         else
         if(  !m_friend.isNUL()  )
         {
              event.destination = m_friend;
              event.timeStamp += m_attr->m_nextTimeInc;
              issueEvent( event );
         }
     }
     break;

    case incub_CREATE_II:
     {
         if(  ready()  )
         {
              int           ctID;
              char          name[40];
              KR_ObjectID   attrID;

              event.data.open(EDO_READ)
                     .getInt( ctID ) // идентификатор таблицы
                     .getStr( name, sizeof(name) )
                     .getObjectID( attrID )
                   .close();

              createObject( ctID, name, attrID, event.timeStamp );
         }
         else
         if(  !m_friend.isNUL()  )
         {
              event.destination = m_friend;
              event.timeStamp += m_attr->m_nextTimeInc;
              issueEvent( event );
         }
     }
      break;

    case incub_CREATE_IS_PEOPLE:
         if(  ready()  )
         {
              int           ctID;
              char          name[40];
              char          attrName[40];
              char          routeName[40];

              event.data.open(EDO_READ)
                     .getInt( ctID ) // идентификатор таблицы
                     .getStr( name, sizeof(name) )
                     .getStr( attrName, sizeof(attrName) )
                     .getStr( routeName, sizeof(routeName) )
                   .close();

              createPeople( ctID, name, 
                            context->searchObject(attrName), 
                            routeName, 
                            event.timeStamp );
         }
         else
         if(  !m_friend.isNUL()  )
         {
              event.destination = m_friend;
              event.timeStamp += m_attr->m_nextTimeInc;
              issueEvent( event );
         }
      break;

    case incub_SET_FRIEND:
    {
           event.data.open(EDO_READ)
                       .getObjectID(m_friend)
                     .close();
    }
    break;

    default: return 0;
    }
    return 1;
 }

 //============================================================
void Incubator::addNotify()
 {
    ct_Subject::addNotify();
    // insert your code this
    m_position = CFVector3(0,0,0);
    m_friend   = KR_ObjectID::NUL();
 }

 //============================================================
void Incubator::removeNotify()
 {
    ct_Subject::removeNotify();
    // insert your code this
 }

void Incubator::render( CViewDynamicList & )
{
}

void Incubator::endRender( CViewScene * )
{
}

CFVector3  Incubator::realPosition()
{
    return m_position;
}


 //============================================================
void Incubator::draw()
 {
 }

 /*************************************
  *
  *   IncubatorTable implementation
  *
  *************************************/

 //============================================================
void IncubatorTable::allocObjects( int objectQnty )
 {
    m_table = new Incubator[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void IncubatorTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *IncubatorTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"IncubatorTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 /*************************************
  *
  *   AttributeTable implementation
  *
  *************************************/

 //============================================================
void AttributeTableIncubator::allocObjects( int objectQnty )
 {
    m_table = new AttributeIncubator[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void AttributeTableIncubator::freeObjects()
 {
    delete [] m_table;
    m_table         = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *AttributeTableIncubator::getObjectPTR( int index )
 {
    s_ASSERT(index>=0 && index <m_maxObjectQnty,"AttributeTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 //============================================================
void Incubator::createObject( 
                       ct_ClassTableID    ctID, 
                       const char        *name, 
                       const KR_ObjectID  &attrID,
                       double              ts
                     )
{
   KR_ObjectID oID = g_arena.newObject(ctID,name);

   if(  !oID.isNUL()  )
   {
        KR_Event   event;

        event.label       = incub_ON_CREATE_OBJECT;
        event.destination = oID;
        event.source      = getObjectID();
        event.timeStamp   = ts;
        event.data.open( EDO_WRITE )
                    .putObjectID( attrID )
                    .putDouble( m_position.x )
                    .putDouble( m_position.y )
                    .putDouble( m_position.z )
                  .close();

        context->sendEventNow( event );
   }
}

void Incubator::createPeople( 
                       ct_ClassTableID    ctID, 
                       const char        *name, 
                       const KR_ObjectID  &attrID,
                       const char        *routeName,
                       double              ts
                     )
{
   KR_ObjectID oID = g_arena.newObject(ctID,name);

   if(  !oID.isNUL()  )
   {
        KR_Event   event;


        event.label       = pe_EVCMD_START;
        event.destination = oID;
        event.source      = getObjectID();
        event.timeStamp   = ts;
        event.data.open( EDO_WRITE )
                    .putObjectID( attrID )
                    .putStr     ( routeName )
                    .putDouble  ( ts )
                  .close();

        context->sendEventNow( event );
   }
}

/* End of file C:\NW\ARENA\OBASE\Incubator\Incubator.cpp */
