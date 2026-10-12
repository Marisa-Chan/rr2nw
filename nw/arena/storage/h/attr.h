/*
   File:   Suavik\d:\game\storage\h\attr.h
   Autor:  Suavik
   Ver     1.0

   Префикс ct_

   Описание атрибутов
 */
#ifndef __ATTR_H__
#define __ATTR_H__

#include <string.h>

#ifndef __CLASSTAB_H__
#include "storage/h/classtab.h"
#endif



#define MAX_ATTR_NAME 40
typedef char ct_AttrStr[40];

 //==========================================================================
class ct_AttrItem
 {
 friend class ct_Attribute;
 friend class GameConsole;

 protected:
    char                   m_name[MAX_ATTR_NAME];
    s_EventDataItemStyle   m_type;
    void                  *m_variable;

 public:
    void set( const char *name, int            &v );
    void set( const char *name, unsigned       &v );

    void set( const char *name, signed char    &v );
    void set( const char *name, KR_byte        &v );

    void set( const char *name, short          &v );
    void set( const char *name, unsigned short &v );

    void set( const char *name, long           &v );
    void set( const char *name, unsigned  long &v );

    void set( const char *name, float          &v );
    void set( const char *name, double         &v );

    void set( const char *name, ct_AttrStr      v );
    void set( const char *name, KR_ObjectID    &v );

    void set(
             const char            *name,
             s_EventDataItemStyle   type,
             void                  *var
            );

	void getString(char * buffer);	// prints value in a buffer
 };

 //==========================================================================
class ct_Attribute: public ct_Object
 {

 friend class GameConsole;
 private:
    ct_AttrItem *m_list;
    int         m_attrQnty;

 protected:
    void        linkTable( ct_AttrItem *table, int cnt );
 public:
    virtual int           receiveEvent( KR_Event &event );
    virtual void          addNotify();
    virtual void          removeNotify();
    virtual void          update(double ts);
    virtual OBJECT_STYLE  style() const;

    ct_Attribute()
    {
        m_list     = NULL;
        m_attrQnty = 0;
    }
    ct_AttrItem  *searchItem( const char *attrName );

    void         set_char  ( const char *attrName, signed char val );
    void         set_byte  ( const char *attrName, KR_byte val );

    void         set_int   ( const char *attrName, int val );
    void         set_uint  ( const char *attrName, unsigned val );

    void         set_short ( const char *attrName, short val );
    void         set_ushort( const char *attrName, unsigned short val );

    void         set_long  ( const char *attrName, long val );
    void         set_ulong ( const char *attrName, unsigned long val );

    void         set_float ( const char *attrName, float val );
    void         set_double( const char *attrName, double val );

    void         set_str   ( const char *attrName, const ct_AttrStr val );
    void         set_ID    ( const char *attrName, const KR_ObjectID &ID );

    //--------------------------------------------------------------------

    signed char    get_char  ( const char *attrName );
    KR_byte        get_byte  ( const char *attrName );

    int            get_int   ( const char *attrName );
    unsigned       get_uint  ( const char *attrName );

    short          get_short ( const char *attrName );
    unsigned short get_ushort( const char *attrName );

    long           get_long  ( const char *attrName );
    unsigned long  get_ulong ( const char *attrName );

    float          get_float ( const char *attrName );
    double         get_double( const char *attrName );

    const char    *get_str   ( const char *attrName );
    KR_ObjectID    get_ID    ( const char *attrName );

    void          *get       ( const char *attrName );


	virtual bool shouldDump () { return false; } // we don't dump attributes
	virtual bool dump(PIN_SaveFile & ) {return false;}
	virtual bool load(PIN_SaveFile & ) {return false;};
	virtual void loadNotify() {};

 };

#endif
/* End of file ATTR.H */