/*
     File:  Suavik\D:\GAME\STORAGE\H\CLASSTAB.H
     Autor: Suavik
     Ver    1.0

     Описание хранилища всех таблиц классов программы и
     методов набора необходимых для одного сеанса работы


     Форматы данных сообщениям ct_Storage:

     создание объекта указанного типа с данным именем
     STORAGE_NEW_OBJECT
            write(ct_ClassTableID*,sizeof(ct_ClassTableID))
            writeStr(objectName)

     создание объекта класса с указанным именем
     STORAGE_NEW_OBJECT_N
            writeStr(classTableName)
            writeStr(objectName)

     STORAGE_DEL_OBJECT
            writeObjectID(KR_ObjectID)

     11.12.97 ~ct_Storege() { closeSeans();-Error!(use virtual functions) }

 */
#ifndef __CLASSTAB_H__
#define __CLASSTAB_H__

#include <stdio.h>
#include "kernel/h/object.h"
#include "message/strgmsg.h"

#define ct_NULLID (-1)
#define MAX_CLASS_NAME_LEN 80

typedef int ct_ClassTableID;

class ct_ClassTable;
class ct_Storage;
class ct_Attribute;
class CDC;

typedef enum {
    OBJECT_BASE,
    OBJECT_SUBJECT,
    OBJECT_ATTRIBUTE
}  OBJECT_STYLE;
//===========================================================================

class ct_Object: public KR_Object
 {
 friend class ct_ClassTable;
 friend class ct_AttributeTable;
 friend class GameConsole;
 protected:
    ct_Object     *m_next,
                  *m_prev;
    int            m_isExist;
    ct_ClassTable *m_master;
    int            m_index;
 public:
    ct_Object            *next() const { return m_next; }
    virtual void          addNotify();
	virtual void          forcedRemoveNotify() {};
    virtual void          removeNotify();
    virtual OBJECT_STYLE  style() const;
    virtual void          draw        ( CDC &gc );

    ct_Object()
    {
       m_next    = NULL;
       m_prev    = NULL;
       m_isExist = 0;
       m_master  = NULL;
       m_index   = -1;
    }
    virtual ~ct_Object() {}
 };


//===========================================================================
typedef bool (*ct_CallBack)(KR_ObjectID oID,void *userParam);

typedef enum {	CT_OVERFLOW,  // cause overflow	
				CT_KILLLRU,   // destroy least recently used
				CT_KILLFIRST, // kill first available
				CT_KILLINVISIBLE, // kill first invisible
				CT_KILLINVISIBLELRU
} ct_addMode;

class ct_ClassTable
 {
 friend class ct_Storage;
 friend class ct_Object;
 friend class GameConsole;
 friend class SimulationContext;
 protected:
    char                 m_name[MAX_CLASS_NAME_LEN+1];
    ct_ClassTableID      m_id;

	
	
	ct_addMode			 m_defaultAddMode;
    int                  m_maxObjectQnty;
    ct_Object           *m_freeList;
    ct_Object           *m_existList;

    ct_ClassTable       *m_nextClassTable; // global class table list

    SimulationContext   *m_context;



    void                 registerClass( const char *name );
    virtual  void        allocObjects( int objectQnty ) = 0;
    virtual  void        freeObjects()                  = 0;
    virtual  ct_Object  *getObjectPTR( int index )      = 0;

    virtual  void        create(
                                 int                  objectQnty,
                                 SimulationContext   *context,
                                 ct_Storage          &storage
                               );
    void                 remove();

    void                 delObject( ct_Object  *obj  );

 public:
	ct_ClassTable() {
		m_defaultAddMode = CT_OVERFLOW;
    m_name[0]              = 0;
    m_maxObjectQnty        = 0;
    m_freeList             = NULL;
    m_existList            = NULL;
    m_context              = NULL;
    m_id                   = ct_NULLID;
    m_nextClassTable       = NULL;

	}

    KR_ObjectID          newObject( const char *name );
	virtual KR_ObjectID  newObject( int AddMode,    const char *name );
    KR_ObjectID          newObject( const char *name, KR_ObjectID id );


    virtual OBJECT_STYLE objectsType() const;
    virtual int          abstractLevel();
    void                 userFind(ct_CallBack cb,void *userParam);

	void				 setAddMode(ct_addMode mode) 
	{ m_defaultAddMode = mode; }

             //ct_ClassTable();  // inline
    virtual ~ct_ClassTable() {}
    ct_ClassTableID      getClassTableID() { return m_id; }
 };

typedef ct_ClassTable   *ct_ClassTablePTR;
typedef ct_Attribute    *ct_AttributePTR;

//===========================================================================
class  ct_AttributeTable: public ct_ClassTable
 {
 public:
       virtual OBJECT_STYLE objectsType() const;
                       ct_AttributeTable() {}
       ct_Attribute   *searchAttribute( const KR_ObjectID &oID ) const;
       int             getAttributeIndex( const KR_ObjectID &oID ) const;
       void            setAttribute( int index, ct_AttributePTR &ptr );
       void            update(double ts);
 };

//===========================================================================

class ct_Storage: public KR_Object
 {
 friend class ct_ClassTable;
 friend class GameConsole;
 friend class SimulationContext;
 private:
    enum {
        CACHE_POW  = 8,
        CACHE_SIZE = 1<<CACHE_POW
    };
    ct_ClassTablePTR           m_seanceCacheClassTable[CACHE_SIZE];

 protected:
    static ct_ClassTable      *m_classTableList; // Don't init!

    ct_ClassTable            **m_seanceClassTablePool;
    int                        m_seanceClassTableQnty;
    int                        m_seanceClassTableMax;
  

 public:

    static ct_ClassTable      *searchClassTable( const char *name );

    virtual int      receiveEvent(KR_Event &event);
    void             openSeance( SimulationContext  *context );
    ct_ClassTableID  searchSeanceClassTable( const char *name );
    const char      *searchSeanceClassTable( ct_ClassTableID ctID );

    virtual 
    ct_ClassTableID  addClassTable( const char *name, int poolQnty );
    KR_ObjectID      newObject(
                               const char *classTableName,
                               const char *objectName
                              );
    KR_ObjectID      newObject(
                               ct_ClassTableID   ctID,
                               const char *objectName
                              );
    KR_ObjectID      newObject(
                               ct_ClassTableID   ctID,
                               const char       *objectName,
                               KR_ObjectID       id
                              );
    KR_ObjectID      newObject(
                               ct_ClassTableID   ctID,
                               const char       *objectName,
                               KR_Event         &notify
                              );
    void             delObject( const KR_ObjectID &id );
    void             delObject( const KR_ObjectID &id, double ts );
    void             closeSeance();

    int              getAttributeIndex(
                                       ct_ClassTableID ct,
                                       const KR_ObjectID &oID
                                      ) const;
    void             updateAttributes(double ts);
    void             userFind(const char *name,ct_CallBack cb,void *userParam);
    void             userFind(ct_ClassTableID ctID,ct_CallBack cb,void *userParam);
    ct_Storage();  // inline
    ~ct_Storage() { }
 };

//===========================================================================




inline ct_Storage::ct_Storage()
 {
    m_seanceClassTablePool = NULL;
    m_seanceClassTableQnty = 0;
    m_seanceClassTableMax  = 0;

    for( int i = 0; i < CACHE_SIZE; ++i )
         m_seanceCacheClassTable[i] = NULL;
 }

#endif
/* End of CLASSTAB.H */