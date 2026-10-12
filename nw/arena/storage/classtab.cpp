/*
   File:  Suavik\d:\game\storage\classtab.cpp
   Autor: Suavik
   Ver    1.0

   Создание списка таблиц классов, распределение памяти под объекты
   и создание объектов указанного типа.

   Префикс ct_

   ct_Object:                                        28.10.97
      addNotify()                                    *
      removeNotify()                                 *

   ct_ClassTable:
      registerClass()                                *
      create()                                       *
      remove()                                       *
      newObject()                                    *
      delObject()                                    *
      abstractLevel()

   ct_Storage
      searchClassTable()                             *
      openSeance()                                   *
      addClassTable()                                *
      closeSeance()                                  *
      receiveEvent()                                 *
      searchSeanceClassTable( ct_ClassTableID ctID ) 0
      searchSeanceClassTable( const char *name )     *
      newObject(
                const char *classTableName,
                const char *objectName
               )
      newObject(                                     *
                ct_ClassTableID   ctID,
                const char       *objectName
               )
      delObject()                                    *

      * тест на вызов процедуры

   ========================================================================
   1. Обработка ошибочных ситуаций встроена в код, необходимо перевести ее на
      ASSERT-ы.

   04.12.97  addNotify() из заглушки переведен на код коструктора.
             !! этого делать нельзя - объект выбивается из списков
             свободных и занятых элементов. по всем инициализациям
             есть код снаружи объекта. Откатил назад.

   11.12.97  Add control in create(): m_context!=NULL, classTablePool!=NULL

   12.12.97  Обнулил cacheTable по закpытию сеанса.
 */
#include <string.h>
#include "kernel/h/echo.h"
#include "kernel/h/context.h"
#include "storage/h/classtab.h"
#include "kernel/h/s_debug.h"
#include "kernel/h/session.h"
#include "storage/h/attr.h"
#include "storage/h/subject.h"

typedef ct_ClassTable *ct_ClassTablePTR;

ct_ClassTable *ct_Storage::m_classTableList = NULL;


  /******************************
   *
   *        ct_Object
   *
   ******************************/

//===========================================================================
void ct_Object::addNotify()
 {
 }

//===========================================================================
void ct_Object::removeNotify()
 {
    m_master->delObject(this);
 }

//===========================================================================
OBJECT_STYLE ct_Object::style() const
 {
    return OBJECT_BASE;
 }

//===========================================================================
void ct_Object::draw( CDC & )
 {
 }


  /******************************
   *
   *        ct_ClassTable
   *
   ******************************/


//===========================================================================
 /*
  * Регистрация таблицы класса в глобальном списке арены
  */
void ct_ClassTable::registerClass( const char *name )
 {
    if(  ct_Storage::searchClassTable( name ) != NULL  )
    {
         echo("ct_ClassTable::registerClass: Dublicate identefier %s\n",name);
         return;
    }
    strncpy(m_name,name,MAX_CLASS_NAME_LEN);

    m_nextClassTable = ct_Storage::m_classTableList;
    ct_Storage::m_classTableList = this;
 }

//===========================================================================
 /*
  * Выделение памяти под объекты и связывание их в список свободных.
  */
void ct_ClassTable::create(
                           int                  objectQnty,
                           SimulationContext   *context,
                           ct_Storage          &storage
                          )
 {
    s_ENTRY(ct_ClassTable::create)
    int i;

    m_maxObjectQnty = objectQnty;
    allocObjects( objectQnty );

    if(  is_ASSERT(context != NULL,"ct_ClassTable::create: context==NULL")  )
         return;

    if(  is_ASSERT(storage.m_seanceClassTablePool != NULL,
                   "ct_ClassTable::create: seans not opening")  )
         return;

    if(  m_maxObjectQnty <= 0  )
    {
         echo("ct_ClassTable::create: Memory too low\n");
         return;
    }

    for( i = 0; i < objectQnty-1; ++i )
    {
         getObjectPTR(i)->m_next    = getObjectPTR(i+1);
         getObjectPTR(i)->m_prev    = NULL;
         getObjectPTR(i)->m_isExist = 0;
         getObjectPTR(i)->m_master  = this;
         getObjectPTR(i)->m_index   = i;
    }

    getObjectPTR( objectQnty-1 )->m_next    = NULL;
    getObjectPTR( objectQnty-1 )->m_prev    = NULL;
    getObjectPTR( objectQnty-1 )->m_isExist = 0;
    getObjectPTR( objectQnty-1 )->m_master  = this;
    getObjectPTR( objectQnty-1 )->m_index   =  objectQnty-1;

    m_freeList = getObjectPTR(0);
    m_existList= NULL;

    m_context  = context;
    m_id       = storage.m_seanceClassTableQnty;

    s_ASSERT( storage.m_seanceClassTableQnty < storage.m_seanceClassTableMax,
              "ct_ClassTable::create: Seance class table too many" );
    storage.m_seanceClassTablePool[
                                  storage.m_seanceClassTableQnty
                                 ] = this;
    
	++storage.m_seanceClassTableQnty;
	
 }

//===========================================================================
void  ct_ClassTable::userFind(ct_CallBack cb,void *userParam)
 {
    if(  is_ASSERT(cb!=0,"Expected function")  )
         return;

    for( 
         const ct_Object *ob = m_existList; 
         ob!=NULL;
         ob = ob->m_next 
       )
         if(  !cb(ob->getObjectID(),userParam)  )
              break;
 }



//===========================================================================
 /*
  * Освобождение памяти и инициалицация пустой таблицы
  */
void ct_ClassTable::remove()
 {
    while( m_existList != NULL )
    {
          m_context->removeObject( m_existList->getObjectID() );
          //m_existList->removeNotify();
    }

    freeObjects();
    m_freeList      = NULL;
    m_existList     = NULL;
    m_maxObjectQnty = 0;
    m_context       = NULL;
 }

//===========================================================================
KR_ObjectID ct_ClassTable::newObject( int AddMode, const char *name )
{

    //if( !(   m_freeList != NULL
    //		&& (!m_context->isObjectPollFull()) )) // overflow
   if (m_freeList == NULL)
    {

		switch (AddMode) {

			case CT_OVERFLOW:

				warning("ct_ClassTable<%s>::newObject: Table overflow",m_name);
				return KR_ObjectID::NUL();

			case CT_KILLINVISIBLE:
				{
					OBJECT_STYLE style = objectsType();
					s_ASSERT(style == OBJECT_SUBJECT,"Not a subject");
					
					echo("Killing the invisible");
					
					
					ct_Subject * sbj = (ct_Subject *) m_existList;
					
					// most old are in the end of the list
					
					int counter = 0;
					while (sbj->m_next)
					{
						sbj = (ct_Subject *) sbj->m_next;
						counter++;
						s_ASSERT(counter < m_maxObjectQnty, "Scheise, counter >= m_maxObjectQnty");
					}
					
					while (sbj)
					{
						if (sbj->m_isVisible)
							sbj = (ct_Subject *) sbj->m_prev;
						else
						{
							sbj->forcedRemoveNotify();
							sbj->removeNotify();
							break;
						}
					}
					
					if (!sbj)	// all are visible
					{
						ct_Subject * sbj = (ct_Subject *) m_existList;
						sbj->forcedRemoveNotify();
						sbj->removeNotify();
					}

					if (Session::m_moment - sbj->m_creationTime < 0.1)
						warning("Possible overflow, INCREASE %s capacity",m_name);
				}

				break;

			default:
				echo("Not Yet Implemented, killing the first");

			case CT_KILLFIRST:
				m_existList->forcedRemoveNotify();
				m_existList->removeNotify();
				// this will call m_master->delObj
				break;
		}
				
	}

	ct_Object   *result = m_freeList;
	result->m_creationTime = Session::m_moment;
	m_freeList          = m_freeList->m_next;
	
	if(  m_existList != NULL  )
		m_existList->m_prev = result;
	
	result->m_next      = m_existList;
	result->m_prev      = NULL;
	m_existList         = result;
	
	result->m_isExist   = 1;
	result->m_tableName = m_name;
	
	return m_context->addObject( name, result );
	
}


//===========================================================================
KR_ObjectID ct_ClassTable::newObject( const char *name )
 {
	return newObject( m_defaultAddMode, name );


/*    if(    m_freeList != NULL
       && (!m_context->isObjectPollFull()) )
    {
         ct_Object   *result = m_freeList;
         m_freeList          = m_freeList->m_next;

         if(  m_existList != NULL  )
              m_existList->m_prev = result;

         result->m_next      = m_existList;
         result->m_prev      = NULL;
         m_existList         = result;

         result->m_isExist   = 1;
	 result->m_tableName = m_name;

         return m_context->addObject( name, result );
    }
    warning("ct_ClassTable<%s>::newObject: Table overflow",m_name);
    return KR_ObjectID::NUL();*/
 }

//===========================================================================
KR_ObjectID ct_ClassTable::newObject( const char *name, KR_ObjectID id )
 {
    if(    m_freeList != NULL
       && (!m_context->isObjectPollFull()) )
    {
         ct_Object   *result = m_freeList;
         m_freeList          = m_freeList->m_next;

         if(  m_existList != NULL  )
              m_existList->m_prev = result;

         result->m_next      = m_existList;
         result->m_prev      = NULL;
         m_existList         = result;

         result->m_isExist   = 1;
	 result->m_creationTime = Session::m_moment;

         return m_context->addObject( name, result, id );
    }
    warning("ct_ClassTable<%s>::newObject: Table overflow",m_name);
    return KR_ObjectID::NUL();
 }

//===========================================================================
void  ct_ClassTable::delObject( ct_Object *obj )
 {
    if(  obj->m_isExist  )
    {
         if(  obj->m_prev != NULL  )
         {
              if(  obj->m_next != NULL  )
                   obj->m_next->m_prev = obj->m_prev;

              obj->m_prev->m_next = obj->m_next;
         }
         else
         {
              m_existList = obj->m_next;

              if(  m_existList != NULL  )
                   m_existList->m_prev = NULL;
         }
         obj->m_next    = m_freeList;
         m_freeList     = obj;
         obj->m_prev    = NULL;
         obj->m_isExist = 0;
    }
 }

//===========================================================================
OBJECT_STYLE ct_ClassTable::objectsType() const
 {
    return OBJECT_BASE;
 }

//===========================================================================
int   ct_ClassTable::abstractLevel()
 {
    return 0;
 }

 /*******************************
  *
  *     ct_AttributeTable
  *
  *******************************/

//===========================================================================
OBJECT_STYLE ct_AttributeTable::objectsType() const
 {
    return OBJECT_ATTRIBUTE;
 }

//===========================================================================
void  ct_AttributeTable::update(double ts)
 {
    for( 
         ct_Object *ob = m_existList; 
         ob!=NULL ; 
         ob = ob->m_next 
       )
    {
         s_ASSERT1(ob->style() == OBJECT_ATTRIBUTE,"ct_AttributeTable<%s>::update",m_name);
         ((ct_Attribute*)(ob))->update(ts);
    }
 }

//===========================================================================
ct_Attribute *ct_AttributeTable::searchAttribute( const KR_ObjectID &oID ) const
 {
    for( 
         const ct_Object *ob = m_existList; 
         ob!=NULL ; 
         ob = ob->m_next 
       )
          if(  ob->getObjectID() == oID  )
          {
              s_ASSERT1(ob->style() == OBJECT_ATTRIBUTE,"ct_AttributeTable<%s>::searchAttribute",m_name);
              return ((ct_Attribute*)(ob));
          }
    return NULL;
 }

//===========================================================================
int ct_AttributeTable::getAttributeIndex( const KR_ObjectID &oID ) const
 {
    const ct_Object *ob = m_existList;

    for(; ob!=NULL ; ob = ob->m_next )
          if(  ob->getObjectID() == oID  )
          {
              s_ASSERT(ob->style() == OBJECT_ATTRIBUTE,"ct_AttributeTable::searchAttribute: Table is not ct_AttributeTable");
              return ob->m_index^((int)this);
          }
    return (-1);
 }

//===========================================================================
void ct_AttributeTable::setAttribute( int index, ct_AttributePTR &ptr )
 {
    if(  index==-1  )
    {
         ptr = 0;
         echo("ct_AttributeTable::setAttribute - index null");
    }
    else
    {
         index ^= (int)this;
         if(  index>=0 || index < m_maxObjectQnty )
         {
              if(  (ct_AttributePTR)getObjectPTR(index)->getObjectID().isNUL()  )
              {
                   echo("ct_AttributeTable::setAttribute error index <%s>",m_name);
                   ptr = 0;
              }
              else
              ptr = (ct_AttributePTR)getObjectPTR(index);
         }
         else 
         {
              ptr = 0;
              echo("ct_AttributeTable::setAttribute error index <%s>",m_name);
         }
    }
 }

 /*******************************
  *
  *     ct_Storage
  *
  *******************************/

//===========================================================================
ct_ClassTable *ct_Storage::searchClassTable( const char *name )
 {
    s_ENTRY(ct_Storage::searchClassTable)

    ct_ClassTable *list = m_classTableList;

    for(; list!=NULL ; list = list->m_nextClassTable )
         if(  strcmp(name, list->m_name) == 0  )
              return list;

    return NULL;
 }

//===========================================================================
void ct_Storage::openSeance( SimulationContext  *c )
 {
    int i;

    s_ENTRY(ct_Storage::openSeance)

    context = c;
    context->addObject("Storage",this);

    int qnty;
    ct_ClassTable *list = m_classTableList;
    for( qnty = 0 ; list!=NULL ; list = list->m_nextClassTable )
           ++qnty;

    m_seanceClassTablePool = new ct_ClassTablePTR[ qnty ];
    
    for( i = 0; i < qnty; ++i )
         m_seanceClassTablePool[i] = NULL;

    m_seanceClassTableQnty = 0;
    m_seanceClassTableMax  = qnty;

    /*
     * m_seanceCacheClassTable - member array[CACHE_SIZE];
     * Exist allways
     */
    for( i = 0; i<CACHE_SIZE; ++i )
         m_seanceCacheClassTable[i] = NULL;

    s_ASSERT(context!=NULL,"context is NULL");
 }

//===========================================================================
ct_ClassTableID ct_Storage::addClassTable( const char *name, int poolQnty )
 {
    s_ENTRY(ct_Storage::addClassTable)

    ct_ClassTable *ct = searchClassTable( name );

    if(  is_ASSERT1(ct!=NULL,"ClassTable '%s' not found", name)  )
         return ct_NULLID;

    if(  is_ASSERT1(searchSeanceClassTable(name) == ct_NULLID, 
                    "ct_Storage::addClassTable: Dublicate add classTable '%s'",name ) )
         return ct_NULLID;

    ct->create( poolQnty, context, *this );

    return ct->m_id;
 }

//===========================================================================
void ct_Storage::closeSeance()
 {
    int i;

    for( i = 0; i < m_seanceClassTableQnty ; ++i )
          m_seanceClassTablePool[i]->remove();

    for( i = 0; i < CACHE_SIZE; ++i )
         m_seanceCacheClassTable[i] = NULL;

    delete [] m_seanceClassTablePool;
    m_seanceClassTablePool = NULL;
    m_seanceClassTableQnty = 0;
    m_seanceClassTableMax  = 0;
    context->removeObject( getObjectID() );
 }

//===========================================================================
int ct_Storage::receiveEvent(KR_Event &event)
 {
    char buf[MAX_SYMBOLIC_LENGHT+1];
    ct_ClassTableID ctID;

    switch(event.label)
    {
    case KR_WAKE_UP:
           //printf("ct_Storage::receiveEvent KR_WAKE_UP\n");
           break;

    case STORAGE_NEW_OBJECT:
           event.data.open(EDO_READ)
                       .get( &ctID, sizeof(ct_ClassTableID) )
                       .getStr( buf, sizeof(buf) )
                     .close();
           newObject( ctID, buf );
           break;

    case STORAGE_NEW_OBJECT_N:
           {
           ct_ClassTableID ctID;

           event.data.open(EDO_READ)
                        .getStr( buf, sizeof(buf) );

           ctID = searchSeanceClassTable( buf );
           if(  ctID == ct_NULLID  )
           {
                echo("ct_Storage::receiveEvent: Unknown ClassTable name %s\n",
                         buf  );

                event.data
                        .getStr( buf, sizeof(buf) )
                      .close();
                return 1;
           }

           event.data   .getStr( buf, sizeof(buf) )
                     .close();
           newObject( ctID, buf );

           }
           break;

    case STORAGE_DEL_OBJECT:
           {
           KR_ObjectID oID;

           event.data.open(EDO_READ)
                       .getObjectID( oID )
                     .close();
           delObject( oID );
           }
           break;

    case STORAGE_DEL_OBJECT_AFTER_TIME:
           {
           KR_ObjectID oID;
           event.data.open(EDO_READ)
                       .getObjectID( oID )
                     .close();

           context->removeObject(oID);
           }
           break;

    default: return 0;
    }
    return 1;
 }

//===========================================================================
const char *ct_Storage::searchSeanceClassTable( ct_ClassTableID ctID )
 {
    if(  ctID >= 0  &&  ctID < m_seanceClassTableQnty  )
         return m_seanceClassTablePool[ctID]->m_name;

    return NULL;
 }

//===========================================================================
ct_ClassTableID ct_Storage::searchSeanceClassTable( const char *name )
 {
    int i;

    int            cv = SimulationContext::cacheVal(name)&(CACHE_SIZE-1);
    ct_ClassTable *ct = m_seanceCacheClassTable[cv];

    if(  ct != NULL  )
         if(  strcmp( name, ct->m_name) == 0  )
              return ct->m_id;


    for( i = 0; i<m_seanceClassTableQnty; ++i )
         if(  strcmp( name, m_seanceClassTablePool[i]->m_name ) == 0  )
         {
              m_seanceCacheClassTable[cv] = m_seanceClassTablePool[i];
              return i;
         }

    return ct_NULLID;
 }

//===========================================================================
KR_ObjectID  ct_Storage::newObject(
                                 const char *classTableName,
                                 const char *objectName
                                )
 {
    ct_ClassTableID ctID = searchSeanceClassTable( classTableName );

    if(  ctID == ct_NULLID  )
    {
         s_ASSERTNQ1("ct_Storage::newObject: Class Table %s not found",
                      classTableName);
         return KR_ObjectID::NUL();
    }

    return m_seanceClassTablePool[ctID]->newObject( objectName );
 }

//===========================================================================
KR_ObjectID  ct_Storage::newObject(
                                 ct_ClassTableID   ctID,
                                 const char       *objectName
                                )
 {
    if(  ctID >= 0  &&  ctID<m_seanceClassTableQnty  )
         return m_seanceClassTablePool[ctID]->newObject(objectName);

    s_ASSERTNQ1("ct_Storage::newObject: Bad index[%i]",ctID);
    return KR_ObjectID::NUL();
 }


//===========================================================================
KR_ObjectID  ct_Storage::newObject(
                               ct_ClassTableID   ctID,
                               const char       *objectName,
                               KR_ObjectID       id
                              )
{
    if(  ctID >= 0  &&  ctID<m_seanceClassTableQnty  )
         return m_seanceClassTablePool[ctID]->newObject(objectName,id);

    s_ASSERTNQ1("ct_Storage::newObject: Bad index[%i]",ctID);
    return KR_ObjectID::NUL();
}


//===========================================================================
KR_ObjectID  ct_Storage::newObject(
                               ct_ClassTableID   ctID,
                               const char       *objectName,
                               KR_Event         &notify
                              )
{
    if(  ctID < 0  || ctID>m_seanceClassTableQnty  )
     return KR_ObjectID::NUL();
    KR_ObjectID res(m_seanceClassTablePool[ctID]->newObject(objectName));

    if(  !res.isNUL()  )
    {
         notify.destination = res;
         context->sendEventNow( notify );
    }
    return res;
}

//===========================================================================
void  ct_Storage::delObject( const KR_ObjectID &id )
 {
    context->removeObject(id);
 }

//===========================================================================
int  ct_Storage::getAttributeIndex(
                                   ct_ClassTableID ctID,
                                   const KR_ObjectID &oID
                                  ) const
 {
    s_ASSERT(ctID>=0,"ct_Storage::getAttributeIndex: Bad index");
    ct_ClassTable *ct = m_seanceClassTablePool[ctID];
    s_ASSERT(ct->objectsType()==OBJECT_ATTRIBUTE,"ct_Storage::getAttributeIndex: Error type");
    return ((ct_AttributeTable*)ct)->getAttributeIndex(oID);
 }


//===========================================================================
void  ct_Storage::updateAttributes(double ts)
 {
     for( int i = 0; i<m_seanceClassTableQnty; ++i )
     {
          ct_ClassTable *ct = m_seanceClassTablePool[i];

          if(  ct->objectsType()==OBJECT_ATTRIBUTE )
               ((ct_AttributeTable*)ct)->update(ts);
     }
 }

//===========================================================================
void ct_Storage::delObject( const KR_ObjectID &id, double ts )
{
    KR_Event event;

    event.label       = STORAGE_DEL_OBJECT_AFTER_TIME;
    event.timeStamp   = ts;
    event.source      = getObjectID();
    event.destination = getObjectID();
    event.data.open(EDO_WRITE)
                  .putObjectID(id)
              .close();
    issueEvent( event );
}

//===========================================================================
void ct_Storage::userFind(const char *name,ct_CallBack cb,void *userParam)
{
    ct_ClassTable *table = searchClassTable( name );
    if(  table!=0  )
         table->userFind(cb,userParam);
}


//===========================================================================
void ct_Storage::userFind(ct_ClassTableID ctID,ct_CallBack cb,void *userParam)
{
    if(  ctID >= 0 && ctID < m_seanceClassTableMax  )
         m_seanceClassTablePool[ctID]->userFind(cb,userParam);
}

/* End of file CLASSTAB.CPP */