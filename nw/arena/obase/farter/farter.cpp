/*
 * File  : C:\NW\ARENA\OBASE\Farter\Farter.cpp
 * Autor :
 * Ver   1.0 
 */
#include "Farter.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/fartmsg.h"
#include "Sound.h"
#include "obase/sound/wavobj.h"
#include "message/sndmsg.h"

 //===========================================================================
class AttributeFarter : public ct_Attribute
{
 public:

    WAVObj            * m_wav;	
    ct_ClassTableID     m_ctsndID;

//{{ATTRIBUTE
    ct_AttrItem  m_array[1];
    ct_AttrStr      m_soundName              ;  // 

    AttributeFarter()
    {
        strncpy(m_soundName,"", sizeof( ct_AttrStr )-1 );

        m_array[0].set("m_soundName",m_soundName);

        linkTable(m_array,1);
    }
//}}END_OF_ATTRIBUTE

   virtual void update(double ts);
};

static AttributeFarter __defaultAttr;

 //===========================================================================
class FarterTable : public ct_SubjectTable
{
 private:
    Farter *m_table;
 public:
    FarterTable()
    {
        m_table = NULL;
        registerClass( "Farter" );
    }
    ~FarterTable()
    {
        delete [] m_table;
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
    virtual  bool      isAudible();
};


bool FarterTable::isAudible()
{
  return true;
}

 //===========================================================================
class AttributeTableFarter : public ct_AttributeTable
{
 protected:
    AttributeFarter *m_table;

 public:
    AttributeTableFarter()
    {
       m_table = NULL;
       registerClass( "FarterAttr" );
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static FarterTable  __classTable;
static AttributeTableFarter __attrTable;
 /*********************************
  *
  *   Farter implementation
  *
  *********************************/

 //============================================================
Farter::Farter()
 {
    m_attr = &__defaultAttr;
 }

 //============================================================
Farter::~Farter()
 {
 }

 //============================================================
int Farter::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case KR_WAKE_UP:
            break;

    case KR_SET_ATTR:
            s_ASSERTNQ("Farter:receiveEvent:case KR_SET_ATTR: do't supported");
            break;

    case START_FARTING:
       {
        KR_ObjectID oID;
        double ts = event.timeStamp;

        event.data.open(EDO_READ)
		    .getObjectID(oID)  	
                    .getDouble(m_position.x)
                    .getDouble(m_position.y)
                    .getDouble(m_position.z)
                  .close();
        

        ct_Attribute *attr = __attrTable.searchAttribute(oID);

        if( attr==NULL )
           echo( "Farter::receiveEvent: Unknown attribute %s",
               context->searchObject(oID));
        else 
           m_attr = (AttributeFarter*)attr;


	 updateSound( getObjectID(),
	    	     context,
		     m_attr->m_ctsndID,
		     m_attr->m_wav,
		     m_snd );


         if (!m_snd.isNUL())
	 {
          event.label = snd_EV_MOVE_TO;
          event.destination = m_snd;
          event.source      = getObjectID();
	  event.timeStamp   = ts;
          event.data.open(EDO_WRITE)
                     .putDouble(m_position.x)
                     .putDouble(m_position.y)
                     .putDouble(m_position.z)
                   .close();
          context->sendEventNow( event );
        }


      }
      break;
    default: return 0;
    }
    return 1;
 }

 //============================================================

void Farter::onExitAudibleZone(double ts) 
{

        if(  !m_snd.isNUL()  )
         {
	  KR_Event event;
          event.destination = m_snd;
          event.source      = getObjectID();
	  event.timeStamp   = ts;
          event.label       = snd_EV_END;
          context->sendEventNow( event );
         }
}

 //============================================================

void Farter::onEnterAudibleZone(double ts) 
{
        // Sound
         if(  !m_snd.isNUL() )
         {
	  KR_Event event;

          event.destination = m_snd;
          event.source      = getObjectID();
	  event.timeStamp   = ts;
          event.label       = snd_EV_START;
          event.data.open(EDO_WRITE)
                 .putInt(0)
              .close();
          context->sendEventNow( event );
         } 
} 

 //============================================================

void Farter::addNotify()
 {
    ct_Object::addNotify();
    // insert your code this
 }

 //============================================================
void Farter::removeNotify()
 {

    if(  !m_snd.isNUL()  )
         context->removeObject( m_snd );

    ct_Object::removeNotify();
    // insert your code this
 }

 //============================================================
CFVector3     Farter::realPosition() {  return m_position;  }

 //============================================================
void Farter::draw()
 {
 }

 /*************************************
  *
  *   FarterTable implementation
  *
  *************************************/

 //============================================================
void FarterTable::allocObjects( int objectQnty )
 {
    m_table = new Farter[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void FarterTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *FarterTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"FarterTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 /*************************************
  *
  *   AttributeTable implementation
  *
  *************************************/

 //============================================================
void AttributeTableFarter::allocObjects( int objectQnty )
 {
    m_table = new AttributeFarter[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void AttributeTableFarter::freeObjects()
 {
    delete [] m_table;
    m_table         = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *AttributeTableFarter::getObjectPTR( int index )
 {
    s_ASSERT(index>=0 && index <m_maxObjectQnty,"AttributeTable::getObjectPTR");
    return &(m_table[ index ]);
 }


void AttributeFarter::update(double)
{
  if (!SetSoundAttr(getObjectID(), context,m_soundName,m_ctsndID, (void *) & m_wav))
	m_wav = NULL;
}
/* End of file C:\NW\ARENA\OBASE\Farter\Farter.cpp */