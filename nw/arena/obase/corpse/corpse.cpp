/*
 * File  : C:\nw\ARENA\OBASE\Corpse\Corpse.cpp
 * Autor :
 * Ver   1.0 
 */


#include "Corpse.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/corpsemsg.h"
#include "message/skinmsg.h"
#include "h/phisics.h"
#include "message/fountmsg.h"
#include "storage/h/savefile.h"

 //===========================================================================
class AttributeCorpse : public ct_Attribute
{
 public:
    CViewObjectModel*   m_cacheSkin              ;  // 
    KR_ObjectID			m_skinID;
    virtual void    update(double ts);  

    KR_ObjectID     m_smokerAttrID;
	KR_ObjectID     m_fireAttrID;
    ct_ClassTableID m_smokerTableID;

//{{ATTRIBUTE
    ct_AttrItem  m_array[13];
    ct_AttrStr      m_skinName               ;  // 
    int             m_isBurning              ;  // 
    ct_AttrStr      m_smokerAttr             ;  // 
    double          m_fireOffsetX            ;  // 
    double          m_fireOffsetY            ;  // 
    double          m_fireOffsetZ            ;  // 
    double          m_minLifeTime            ;  // Минимальное время жизни. После истечения этого срока объект исчезнет после исчезновения из поля зрения
    ct_AttrStr      m_smokerTable            ;  // 
    int             m_isSmoking              ;  // 
    ct_AttrStr      m_fireAttr               ;  // 
    float           m_corpseOffsetX          ;  // Смещение модели обломков относительно точки соударения объекта с землей
    double          m_corpseOffsetY          ;  // 
    float           m_corpseOffsetZ          ;  // 

    AttributeCorpse()
    {
        strncpy(m_skinName,"sk.corpse.default", sizeof( ct_AttrStr )-1 );
        m_isBurning          = 1;
        strncpy(m_smokerAttr,"Smoker.Attr.Corpse", sizeof( ct_AttrStr )-1 );
        m_fireOffsetX        = 0;
        m_fireOffsetY        = 1;
        m_fireOffsetZ        = 0;
        m_minLifeTime        = 30;
        strncpy(m_smokerTable,"DynSmoker", sizeof( ct_AttrStr )-1 );
        m_isSmoking          = 1;
        strncpy(m_fireAttr,"Smoker.Attr.Fire.Corpse", sizeof( ct_AttrStr )-1 );
        m_corpseOffsetX      = 0;
        m_corpseOffsetY      = -1;
        m_corpseOffsetZ      = 0;

        m_array[0].set("m_skinName",m_skinName);
        m_array[1].set("m_isBurning",m_isBurning);
        m_array[2].set("m_smokerAttr",m_smokerAttr);
        m_array[3].set("m_fireOffsetX",m_fireOffsetX);
        m_array[4].set("m_fireOffsetY",m_fireOffsetY);
        m_array[5].set("m_fireOffsetZ",m_fireOffsetZ);
        m_array[6].set("m_minLifeTime",m_minLifeTime);
        m_array[7].set("m_smokerTable",m_smokerTable);
        m_array[8].set("m_isSmoking",m_isSmoking);
        m_array[9].set("m_fireAttr",m_fireAttr);
        m_array[10].set("m_corpseOffsetX",m_corpseOffsetX);
        m_array[11].set("m_corpseOffsetY",m_corpseOffsetY);
        m_array[12].set("m_corpseOffsetZ",m_corpseOffsetZ);

        linkTable(m_array,13);
    }
//}}END_OF_ATTRIBUTE
};


strg_SUBJECT_TABLE_IMPLEMENTATION(Corpse,true)
static CorpseTable __corpse;
strg_ATTRIBUTE_TABLE_IMPLEMENTATION(Corpse,"CorpseAttr")


 /*********************************
  *
  *   Corpse implementation
  *
  *********************************/

 //============================================================
strg_CONSTRUCTOR_DYNVIEW(Corpse)
 {
	m_attr = 0;
 }

 //============================================================
Corpse::~Corpse()
 {
 }


void Corpse::DestroyMe()
{
     if(  context == 0  )
          return;

	context->removeObject(m_smoke);
	context->removeObject(m_fire);
	context->removeObject(getObjectID() );
}

void Corpse::onHide(double )
{
	if (m_mustDieNow)
		DestroyMe();
}

 //============================================================
int Corpse::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case KR_WAKE_UP:
            break;
	case CORPSE_TIME_TO_DIE:	
			m_mustDieNow = 1;
			if (!m_isVisible)
				DestroyMe();
			break;
	case CORPSE_START_ROTTING:
		{
			//int			index;
			double		tm;
			
			KR_ObjectID oID;
			KR_ObjectID parentID;

			CFVector3 pos;

			event.data.open(EDO_READ)
						.getObjectID(parentID)
                        .getInt	  (m_attributeIndex)						
                        .getDouble(pos.x)
                        .getDouble(pos.y)
                        .getDouble(pos.z)
                      .close();


			if( checkCollision( 
                     pos /*+ CFVector3(0,5,0)*/ ,    // начало движения
                     CFVector3(0,-1,0),    // напрвление со скоростью
                     1, // радиус
                     50,  // время для проверки
                     parentID,  // кого игнорировать
                     tm, // время, через которое стукнемся
                     oID      // объект, о который стукнемся
                   ) )
			{				
				pos.y -= tm;
			}
			else
			{
				context->removeObject(getObjectID() );
				break;
			}
			
		
			m_mustDieNow = 0;

			__attrTable.setAttribute(m_attributeIndex,(ct_Attribute *&)m_attr);
			m_skin.Attach(m_attr->m_cacheSkin);

			pos += CFVector3(m_attr->m_corpseOffsetX,m_attr->m_corpseOffsetY,m_attr->m_corpseOffsetZ);
			setPosition(pos);

			m_smoke = KR_ObjectID::NUL();
			m_fire  = KR_ObjectID::NUL();

			if (m_attr->m_isSmoking)
			{
					m_smoke = g_arena.newObject(m_attr->m_smokerTableID,"Smoker.Corpse");
                                     
			        if(  m_smoke == KR_ObjectID::NUL()  )
					{
						echo("Warning! Smoker table overflow");						
						break;
					}
		
					event.label     = fou_EVCMD_START;	 					
					event.source    = g_arena.getObjectID();
					event.destination = m_smoke;
					event.data.open(EDO_WRITE)
                        .putObjectID(m_attr->m_smokerAttrID)
                        .putDouble(pos.x + m_attr->m_fireOffsetX) 
                        .putDouble(pos.y + m_attr->m_fireOffsetY)
                        .putDouble(pos.z + m_attr->m_fireOffsetZ)
                    .close();
			        g_arena.getContext()->sendEventNow(event);
			}

			if (m_attr->m_isBurning)
			{
					m_fire = g_arena.newObject(m_attr->m_smokerTableID,"Smoker.Corpse");
                                     
			        if(  m_fire == KR_ObjectID::NUL()  )
					{
						echo("Warning! Smoker table overflow");
						break;
					}
		
					event.label     = fou_EVCMD_START;	 					
					event.source    = g_arena.getObjectID();
					event.destination = m_fire;
					event.data.open(EDO_WRITE)
                        .putObjectID(m_attr->m_fireAttrID)
                        .putDouble(pos.x + m_attr->m_fireOffsetX)
                        .putDouble(pos.y + m_attr->m_fireOffsetY)
                        .putDouble(pos.z + m_attr->m_fireOffsetZ)
                    .close();
			        g_arena.getContext()->sendEventNow(event);
			}


			event.label			= CORPSE_TIME_TO_DIE;
			event.source		= getObjectID();
			event.destination	= getObjectID();
			event.timeStamp		+= m_attr->m_minLifeTime;
			issueEvent(event);

		}	
		break;

    default: return 0;
    }
    return 1;
 }



strg_SUBJECT_DYNVIEW_IMPLEMENTATION(Corpse)

 //============================================================
void Corpse::onRender (double)
{
    CFMatrix3x4 &m = m_skin.GetDirModify();
    m.LoadIdentity();
    //m.RotateOxL(m_rotateOx);
    //m.RotateOyL(m_rotateOy);
    m.TranslateL( getPosition() );   
}


 //============================================================
void Corpse::addNotify()
 {
    ct_Subject::addNotify();
    // insert your code this
 }

 //============================================================
void Corpse::removeNotify()
 {
    ct_Subject::removeNotify();
    // insert your code this
 }



CFVector3 Corpse::realPosition() { return getPosition() ; }

void AttributeCorpse::update(double ts)
{
	strg_UPDATE_ATTRIBUTE_SKIN(m_skinName,m_cacheSkin,ts);

	m_smokerTableID = g_arena.searchSeanceClassTable( m_smokerTable );

	if (m_isSmoking)
	{		
		m_smokerAttrID = context->searchObject(m_smokerAttr);
	}

	if (m_isBurning)
	{	
		m_fireAttrID	= context->searchObject(m_fireAttr);
	}

}


// ----- interface IDynamicObject
void      *Corpse::queryInterface( int interNum )
{
    switch( interNum )
    {
		case IUnknownIID:       return (KR_Object*)this;    
    }

    return 0;
}



bool	Corpse::dump(PIN_SaveFile & sf)
{
		if (!ct_Subject::dump(sf))
			return false;

		if (!sf.WriteData( (char *) & m_attributeIndex, sizeof(CorpseData)  ))
			return false;

		return true;
}

bool	Corpse::load(PIN_SaveFile & sf)
{
		if (!ct_Subject::load(sf))
			return false;


		if (!sf.GetData( (char *) & m_attributeIndex, sizeof(CorpseData)  ))
			return false;
		
		return true;
}

void	Corpse::loadNotify()
{
	ct_Subject::loadNotify(); 
	__attrTable.setAttribute(m_attributeIndex,(ct_Attribute *&)m_attr);
	m_skin.Attach(m_attr->m_cacheSkin);
}

/* End of file C:\nw\ARENA\OBASE\Corpse\Corpse.cpp */