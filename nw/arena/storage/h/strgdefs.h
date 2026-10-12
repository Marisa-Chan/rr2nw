/*
     strg_TABLE_FUNC_IMPLEMENTATION(name,obj)
                 Макрос, реализующий функции allocObjects, freeObjects, getObjectPTR
                 Непосредственно не используется
                 name - Полное имя таблицы
                 obj  - имя объекта


     strg_SUBJECT_TABLE_IMPLEMENTATION(name,rend)
                 Полная реализация таблицы класса.
                 name - имя объекта, чья таблица делается (Brird, Tank)
                 rend - 1 если объект нужно рендерить, иначе 0



     strg_ATTRIBUTE_TABLE_IMPLEMENTATION(name,extName)
                 Реализация таблицы атрибутов
                 name    - Имя объекта (Bird,Tank...)
                 extName - строка-идентификатор типа

     strg_SUBJECT_DYNVIEW_DECLARE
                 Набор объявлений необходимых функций и 
                 переменных - членов класса для объекта, имеющего скин.

     strg_SUBJECT_DYNVIEW_IMPLEMENTATION(name)
                 Описание функций вывода объекта со скином. При использовании
                 потребуется дополнительно описать реализацию void $$::onRender()
                 и CFVector3 $$::realPosition()

     strg_UPDATE_ATTRIBUTE_SKIN(skinName,cacheSkin,timeSt)
                Набор действий по связыванию скина с объектом.

     strg_CONSTRUCTOR_DYNVIEW(name)
                Объявление конструктора объекта со скином
 */

#ifndef __STRGDEFS_H__INCLUDED
#define __STRGDEFS_H__INCLUDED

#define strg_TABLE_FUNC_IMPLEMENTATION(name,obj) void name::allocObjects( int objectQnty )\
 {                                                                            \
    m_table = new obj[ objectQnty ];                                          \
    if(  m_table == NULL  )                                                   \
         m_maxObjectQnty = 0;                                                 \
 }                                                                            \
void name::freeObjects()                                                      \
 {                                                                            \
    delete [] m_table;                                                        \
    m_table         = NULL;                                                   \
    m_maxObjectQnty = 0;                                                      \
 }                                                                            \
ct_Object *name::getObjectPTR( int index )                                    \
 {                                                                            \
    s_ASSERT1(index>=0 && index <m_maxObjectQnty,"%s::getObjectPTR()",#name); \
    return &(m_table[ index ]);                                               \
 }


#define strg_SUBJECT_TABLE_IMPLEMENTATION(name,rend)  class name##Table : public ct_SubjectTable{\
 private:                                                   \
  name *m_table;                                            \
 public:                                                    \
    name##Table()                                           \
    {                                                       \
      m_table = NULL;                                       \
      registerClass(#name);                                 \
    }                                                       \
    ~name##Table()                                          \
    {                                                       \
      delete [] m_table;                                    \
      m_table = NULL;                                       \
    }                                                       \
    virtual void       allocObjects( int objectQnty );      \
    virtual void       freeObjects ();                      \
    virtual ct_Object *getObjectPTR( int index );           \
    virtual bool       isRendering();                       \
};                                                          \
strg_TABLE_FUNC_IMPLEMENTATION(name##Table,name)            \
bool name##Table::isRendering() { return rend; }             



#define strg_ATTRIBUTE_TABLE_IMPLEMENTATION(name,extName) class AttributeTable##name : public ct_AttributeTable \
{                                                          \
 protected:                                                \
    Attribute##name *m_table;                              \
                                                           \
 public:                                                   \
    AttributeTable##name()                                 \
    {                                                      \
       m_table = NULL;                                     \
       registerClass( extName );                           \
    }                                                      \
    ~AttributeTable##name()                                \
    {                                                      \
       delete [] m_table;                                  \
       m_table = NULL;                                     \
    }                                                      \
    virtual void       allocObjects( int objectQnty );     \
    virtual void       freeObjects ();                     \
    virtual ct_Object *getObjectPTR( int index );          \
};                                                         \
strg_TABLE_FUNC_IMPLEMENTATION(AttributeTable##name,Attribute##name) \
static AttributeTable##name __attrTable;


#define strg_SUBJECT_DYNVIEW_DECLARE  CViewObjectRef  m_skin; \
    s_ViewDynamicObject    m_viewDynObj;                      \
    virtual void render   ( CViewDynamicList &list, double ); \
    virtual void endRender( CViewScene *scene );              \
    virtual CFVector3     realPosition();                     \
    void                  onRender(double ts);

#define strg_SUBJECT_DYNVIEW_IMPLEMENTATION(name) void   \
  name::render( CViewDynamicList &list, double ts)  \
{                                                 \
    onRender(ts);                                 \
    m_viewDynObj.prepareToRender();               \
    list.Load( &m_viewDynObj );                   \
}                                                 \
void name::endRender( CViewScene *scene )         \
{                                                 \
    scene->RemoveLandDynamic( &m_viewDynObj );    \
}                                               

#define strg_UPDATE_ATTRIBUTE_SKIN(skinName,cacheSkin,timeSt)       {      \
    KR_ObjectID skinID =  context->searchObject( skinName );               \
    s_ASSERT( !skinID.isNUL(), "Attribute::update" );                      \
    KR_Event event;                                                        \
    event.timeStamp   = timeSt;                                            \
    event.label       = sk_EV_QUERY_MODEL_PTR;                             \
    event.destination = skinID;                                            \
    context->sendEventNow( event );                                        \
    s_ASSERT(event.label==sk_EV_QUERY_MODEL_PTR_OK,"Attribute::update");   \
    event.data.open(EDO_READ)                                              \
                .get(&cacheSkin,sizeof(void*))                             \
              .close();                                                    \
 }


#define strg_CONSTRUCTOR_DYNVIEW(name) name::name() : m_viewDynObj(m_skin)

#endif                              
