/*
   File:   Suavik\d:\game\storage\attr.h
   Autor:  Suavik
   Ver     1.0

   Префикс ct_
 */

#include <string.h>
#include "storage/h/attr.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "message/attrmsg.h"


void ct_AttrItem::getString(char * buffer)	// prints value in a buffer
{
	buffer[0] = 0;

	switch (m_type) {
	case EDI_BYTE:
			sprintf(buffer, "%d", * ((unsigned char *)m_variable)    );
		break;
	case EDI_CHAR:
			sprintf(buffer,"%c",* ((char *)m_variable));
		break;
	case EDI_INT:
			sprintf(buffer,"%d",* ((int *)m_variable));
		break;
	case EDI_UINT:
			sprintf(buffer,"%u",* ((unsigned *)m_variable));
		break;
	case EDI_LONG:
			sprintf(buffer,"%l",* ((long *)m_variable));
		break;
	case EDI_ULONG:
			sprintf(buffer,"%ul",* ((unsigned long *)m_variable));
		break;
	case EDI_SHORT:
			sprintf(buffer,"%d",* ((short *)m_variable));
		break;
	case EDI_USHORT:
			sprintf(buffer,"%u",* ((unsigned short *)m_variable));
		break;
	case EDI_DOUBLE:
			sprintf(buffer,"%lf",* ((double *)m_variable));
		break;
	case EDI_FLOAT:
			sprintf(buffer,"%lf",* ((float *) m_variable));
		break;
	case EDI_STR:
			sprintf(buffer,"%s", (char *) m_variable);
		break;
	case EDI_OBJECTID:
			//sprintf(buffer,"%d",(EDI_OBJECTID) * m_variable);
			sprintf(buffer,"This is ObjectID");
		break;
	default: sprintf(buffer,"--- INCORRECT!!! ---");
	};

}

 //==========================================================================
void  ct_AttrItem::set(
                       const char            *name,
                       s_EventDataItemStyle   type,
                       void                  *var
                     )
 {
    strncpy(m_name,name,sizeof(m_name)-1);
    m_type     = type;
    m_variable = var;
 }

 //==========================================================================
void ct_AttrItem::set( const char *name, int            &v )
 {
    strncpy( m_name, name, sizeof(m_name)-1 );
    m_type     = EDI_INT;
    m_variable = &v;
 }

 //==========================================================================
void ct_AttrItem::set( const char *name, unsigned       &v )
 {
    strncpy( m_name, name, sizeof(m_name)-1 );
    m_type     = EDI_UINT;
    m_variable = &v;
 }

 //==========================================================================
void ct_AttrItem::set( const char *name, signed char    &v )
 {
    strncpy( m_name, name, sizeof(m_name)-1 );
    m_type     = EDI_CHAR;
    m_variable = &v;
 }
 //==========================================================================
void ct_AttrItem::set( const char *name, KR_byte        &v )
 {
    strncpy( m_name, name, sizeof(m_name)-1 );
    m_type     = EDI_BYTE;
    m_variable = &v;
 }

 //==========================================================================
void ct_AttrItem::set( const char *name, short          &v )
 {
    strncpy( m_name, name, sizeof(m_name)-1 );
    m_type     = EDI_SHORT;
    m_variable = &v;
 }
 //==========================================================================
void ct_AttrItem::set( const char *name, unsigned short &v )
 {
    strncpy( m_name, name, sizeof(m_name)-1 );
    m_type     = EDI_USHORT;
    m_variable = &v;
 }

 //==========================================================================
void ct_AttrItem::set( const char *name, long           &v )
 {
    strncpy( m_name, name, sizeof(m_name)-1 );
    m_type     = EDI_LONG;
    m_variable = &v;
 }
 //==========================================================================
void ct_AttrItem::set( const char *name, unsigned  long &v )
 {
    strncpy( m_name, name, sizeof(m_name)-1 );
    m_type     = EDI_ULONG;
    m_variable = &v;
 }

 //==========================================================================
void ct_AttrItem::set( const char *name, float          &v )
 {
    strncpy( m_name, name, sizeof(m_name)-1 );
    m_type     = EDI_FLOAT;
    m_variable = &v;
 }
 //==========================================================================
void ct_AttrItem::set( const char *name, double         &v )
 {
    strncpy( m_name, name, sizeof(m_name)-1 );
    m_type     = EDI_DOUBLE;
    m_variable = &v;
 }

 //==========================================================================
void ct_AttrItem::set( const char *name, ct_AttrStr v )
 {
    strncpy( m_name, name, sizeof(m_name)-1 );
    m_type     = EDI_STR;
    m_variable = v;
 }

 //==========================================================================
void ct_AttrItem::set( const char *name, KR_ObjectID &v )
 {
    strncpy( m_name, name, sizeof(m_name)-1 );
    m_type     = EDI_OBJECTID;
    m_variable = &v;
 }

/****************************************
 *
 *         ct_Attribute
 *
 ****************************************/

 //==========================================================================
ct_AttrItem *ct_Attribute::searchItem( const char *attrName )
 {
    int i;

    if( m_attrQnty == 0 )
         return 0;

    if( m_list == NULL )
    {
         echo("Attribute list is NULL. Need initialize\n");
         return NULL;
    }

    for( i = 0; i < m_attrQnty; ++i )
    {
         ct_AttrItem &item = m_list[i];

         if( strcmp( item.m_name, attrName ) == 0 )
         {
             if( item.m_variable == NULL )
             {
                  echo("Atrribute %s don't link\n",attrName);
                  return NULL;
             }
             return &item;
         }
    }
    return NULL;
 }

 //==========================================================================
#define ct_SET_VAL(fname,edi,type,val)  ct_AttrItem *item            \
      = searchItem(attrName);                                      \
      if( item == NULL ) return;                                   \
      if( item->m_type==edi ) *((type*)(item->m_variable))=val;    \
      else echo("%s:Atrribute %s type mistmach. "                  \
               "Expected %s\n" ,fname,attrName,ed_Tag2Msg(item->m_type));


 //==========================================================================
void ct_Attribute::set_char( const char *attrName, signed char val )
 {
    ct_SET_VAL("set_char",EDI_CHAR,char,val)
 }

 //==========================================================================
void ct_Attribute::set_byte( const char *attrName, KR_byte val )
 {
    ct_SET_VAL("set_byte",EDI_BYTE,KR_byte,val)
 }

 //==========================================================================
void ct_Attribute::set_int( const char *attrName, int val )
 {
    ct_SET_VAL("set_int",EDI_INT,int,val)
 }

 //==========================================================================
void ct_Attribute::set_uint( const char *attrName, unsigned val )
 {
    ct_SET_VAL("set_uint",EDI_UINT,unsigned,val)
 }

 //==========================================================================
void ct_Attribute::set_float( const char *attrName, float val )
 {
    ct_SET_VAL("set_float",EDI_FLOAT,float,val)
 }

 //==========================================================================
void ct_Attribute::set_double( const char *attrName, double val )
 {
    ct_SET_VAL("set_double",EDI_DOUBLE,double,val)
 }

 //==========================================================================
void ct_Attribute::set_long( const char *attrName, long val )
 {
    ct_SET_VAL("set_long",EDI_LONG,long,val)
 }

 //==========================================================================
void ct_Attribute::set_ulong( const char *attrName, unsigned long val )
 {
    ct_SET_VAL("set_ulong",EDI_ULONG,unsigned long,val)
 }

 //==========================================================================
void ct_Attribute::set_short( const char *attrName, short val )
 {
    ct_SET_VAL("set_short",EDI_SHORT,short,val)
 }

 //==========================================================================
void ct_Attribute::set_ushort( const char *attrName, unsigned short val )
 {
    ct_SET_VAL("set_ushort",EDI_SHORT,unsigned short,val)
 }

 //==========================================================================
void ct_Attribute::set_ID( const char *attrName, const KR_ObjectID &val )
 {
    ct_SET_VAL("set_ID",EDI_OBJECTID,KR_ObjectID,val)
 }

 //==========================================================================
void ct_Attribute::set_str( const char *attrName, const ct_AttrStr str )
 {
    ct_AttrItem *item = searchItem(attrName);

    if( item==NULL )
         return;

    if( item->m_type != EDI_STR )
    {
         echo("ct_Attribute::set_str:Atrribute %s type mistmach. "
               "Expected %s\n" ,attrName,ed_Tag2Msg(item->m_type));
         return;
    }
    strncpy( ((char*)(item->m_variable)),str,sizeof(ct_AttrStr)-1);
 }

 //==========================================================================
#define GET_VAL(fname,edi,defaultVal,type) ct_AttrItem *item            \
   = searchItem(attrName);                                              \
   if( item==NULL ) return defaultVal;                                  \
   if( item->m_type != edi )                                            \
   {                                                                    \
        echo("ct_Attribute::%s: Attribute %s type mistmach. "           \
             "Expected %s\n",fname,attrName,ed_Tag2Msg(item->m_type));  \
        return defaultVal;                                              \
   }                                                                    \
   return *((type)(item->m_variable));


 //==========================================================================
signed char    ct_Attribute::get_char( const char *attrName )
 {
    GET_VAL("get_char",EDI_CHAR,0,signed char*)
 }

 //==========================================================================
KR_byte        ct_Attribute::get_byte( const char *attrName )
 {
    GET_VAL("get_byte",EDI_BYTE,0,KR_byte*)
 }

 //==========================================================================
int            ct_Attribute::get_int( const char *attrName )
 {
    GET_VAL("get_int",EDI_INT,0,int*)
 }

 //==========================================================================
unsigned       ct_Attribute::get_uint( const char *attrName )
 {
    GET_VAL("get_uint",EDI_UINT,0,unsigned*)
 }

 //==========================================================================
short          ct_Attribute::get_short( const char *attrName )
 {
    GET_VAL("get_short",EDI_SHORT,0,short*)
 }

 //==========================================================================
unsigned short ct_Attribute::get_ushort( const char *attrName )
 {
    GET_VAL("get_ushort",EDI_USHORT,0,unsigned short*)
 }

 //==========================================================================
long           ct_Attribute::get_long( const char *attrName )
 {
    GET_VAL("get_long",EDI_LONG,0L,long*)
 }

 //==========================================================================
unsigned long  ct_Attribute::get_ulong( const char *attrName )
 {
    GET_VAL("get_ulong",EDI_ULONG,0UL,unsigned long*)
 }

 //==========================================================================
float          ct_Attribute::get_float( const char *attrName )
 {
    GET_VAL("get_float",EDI_FLOAT,0.0,float*)
 }

 //==========================================================================
double         ct_Attribute::get_double( const char *attrName )
 {
    GET_VAL("get_double",EDI_DOUBLE,0.0,double*)
 }

 //==========================================================================
KR_ObjectID    ct_Attribute::get_ID( const char *attrName )
 {
    GET_VAL("get_ID",EDI_OBJECTID,KR_ObjectID(-1,-1),KR_ObjectID*)
 }

 //==========================================================================
const char    *ct_Attribute::get_str( const char *attrName )
 {
    ct_AttrItem *item = searchItem(attrName);

    if( item==NULL )
         return NULL;

    if( item->m_type != EDI_STR )
    {
         echo("ct_Attribute::get_str: Attribute %s type mistmach. "
              "Expected %s\n",attrName,ed_Tag2Msg(item->m_type));
         return NULL;
    }

    return ((const char *)(item->m_variable));

 }

 //==========================================================================
void   *ct_Attribute::get( const char *attrName )
 {
    ct_AttrItem *item = searchItem(attrName);

    if( item==NULL )
         return NULL;

    return item->m_variable;
 }

 //==========================================================================
void ct_Attribute::linkTable( ct_AttrItem *table, int cnt )
 {
    int i, j;

    m_list     = table;
    m_attrQnty = cnt;

    for( i = 0; i < cnt; ++i )
    {
         ct_AttrItem &i0 = m_list[i];

         for( j = 0; j < cnt; ++j )
         if( i!=j )
         {
              if( strcmp( i0.m_name, m_list[j].m_name ) == 0 )
              {
                   echo("ct_Attribute::linkTable: Dublicate identefier %s\n",
                         i0.m_name);
              }
         }
    }
 }

 //==========================================================================
int   ct_Attribute::receiveEvent( KR_Event &event )
 {
    char   attrName[MAX_ATTR_NAME];

    switch( event.label )
    {
    case KR_WAKE_UP:
            break;

    case ATTR_MSG_SET_BYTE:
            {
                unsigned char v;
                event.data.open(EDO_READ)
                            .getStr(attrName,sizeof(attrName))
                            .getByte(v)
                          .close();
                set_byte(attrName,v);
            }
            break;
    case ATTR_MSG_SET_CHAR:
            {
                char v;
                event.data.open(EDO_READ)
                            .getStr(attrName,sizeof(attrName))
                            .getChar(v)
                          .close();
                set_char(attrName,v);
            }
            break;

    case ATTR_MSG_SET_SHORT:
            {
                short v;
                event.data.open(EDO_READ)
                            .getStr(attrName,sizeof(attrName))
                            .getShort(v)
                          .close();
                set_short(attrName,v);
            }
            break;
    case ATTR_MSG_SET_USHORT:
            {
                unsigned short v;
                event.data.open(EDO_READ)
                            .getStr(attrName,sizeof(attrName))
                            .getUShort(v)
                          .close();
                set_ushort(attrName,v);
            }
            break;

    case ATTR_MSG_SET_INT:
            {
                int v;
                event.data.open(EDO_READ)
                            .getStr(attrName,sizeof(attrName))
                            .getInt(v)
                          .close();
                set_int(attrName,v);
            }
            break;
    case ATTR_MSG_SET_UINT:
            {
                unsigned int v;
                event.data.open(EDO_READ)
                            .getStr(attrName,sizeof(attrName))
                            .getUInt(v)
                          .close();
                set_uint(attrName,v);
            }
            break;

    case ATTR_MSG_SET_LONG:
            {
                long v;
                event.data.open(EDO_READ)
                            .getStr(attrName,sizeof(attrName))
                            .getLong(v)
                          .close();
                set_long(attrName,v);
            }
            break;
    case ATTR_MSG_SET_ULONG:
            {
                unsigned long v;
                event.data.open(EDO_READ)
                            .getStr(attrName,sizeof(attrName))
                            .getULong(v)
                          .close();
                set_ulong(attrName,v);
            }
            break;

    case ATTR_MSG_SET_FLOAT:
            {
                float v;
                event.data.open(EDO_READ)
                            .getStr(attrName,sizeof(attrName))
                            .getFloat(v)
                          .close();
                set_float(attrName,v);
            }
            break;
    case ATTR_MSG_SET_DOUBLE:
            {
                double v;
                event.data.open(EDO_READ)
                            .getStr(attrName,sizeof(attrName))
                            .getDouble(v)
                          .close();
                set_double(attrName,v);
            }
            break;

    case ATTR_MSG_SET_STR:
            {
                char v[s_EventData::BUFF_SIZE];
                event.data.open(EDO_READ)
                            .getStr(attrName,sizeof(attrName))
                            .getStr(v,sizeof(v))
                          .close();
                set_str(attrName,v);
            }
            break;

    case ATTR_MSG_SET_OBJECTID:
            {
                KR_ObjectID ID;
                event.data.open(EDO_READ)
                            .getStr(attrName,sizeof(attrName))
                            .getObjectID(ID)
                          .close();
                set_ID(attrName,ID);
            }
            break;

    default: return 0;
    }
    return 1;
 }
 //==========================================================================
void   ct_Attribute::addNotify()
 {
    ct_Object::addNotify();
 }

 //==========================================================================
void   ct_Attribute::removeNotify()
 {
    ct_Object::removeNotify();
 }
//===========================================================================
OBJECT_STYLE ct_Attribute::style() const
 {
    return OBJECT_ATTRIBUTE;
 }

//===========================================================================
void ct_Attribute::update(double)
 {
 }

//===========================================================================
void          draw( CDC & )
 {
 }
/* End of file ATTR.CPP */