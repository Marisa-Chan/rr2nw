/*
* File  : C:\NW\ARENA\OBASE\Orphan\Orphan.cpp
* Autor :
* Ver   1.0 
*/
#include "Orphan.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "kernel/h/session.h"
#include "message/skinmsg.h"
#include "message/Unitmsg.h"
#include "message/vehiclemsg.h"
#include "zav.h"
#include "h/phisics.h"
#include "h/vehicle.h"
#include "../taxi/taxi.h"
#include "message/sndmsg.h"
#include "storage/h/savefile.h"


static void AnimateCallBack1(CViewObjectBaseSet *,CViewObjectBase *pBase,CViewObjectRef *ref);
static void AnimateCallBack2(CViewObjectBaseSet *,CViewObjectBase *pBase,CViewObjectRef *ref);


//===========================================================================
class OrphanTable : public ct_SubjectTable
{
private:
    Orphan *m_table;
public:
    OrphanTable()
    {
        m_table = NULL;
        registerClass( "Orphan" );
    }
    ~OrphanTable()
    {
        delete [] m_table;
        m_table = NULL;
    }
	
    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
    virtual bool       isRendering();
    virtual bool       isAudible();	
};

bool OrphanTable::isRendering()
{
	return true;
}

bool OrphanTable::isAudible()
{
	return true;
}


//===========================================================================
class AttributeOrphan : public ct_Attribute
{
public:
    CViewObjectModel*   m_cacheSkin              ;  // 
    KR_ObjectID			m_skinID;
    KR_ObjectID         m_attrForVehicle;
    virtual void    update(double ts);  
	
    int             m_cacheExplAttr;
    int             m_cacheExplosionTable;

    ct_ClassTableID     m_smokeTableID;
    KR_ObjectID         m_smokeAttrID;
	
	
	//{{ATTRIBUTE
    ct_AttrItem  m_array[7];
    ct_AttrStr      m_explAttrName           ;  // 
    double          m_deltaT                 ;  // 
    double          m_collisionT             ;  // 
    double          m_explosionTime          ;  // 
    double          m_minSpeed               ;  // 
    ct_AttrStr      m_smokeAttrName          ;  // 
    double          m_smokeDamage            ;  // Значение Damage при котором надо дымиться

    AttributeOrphan()
    {
        strncpy(m_explAttrName,"Expl.Attr.Default", sizeof( ct_AttrStr )-1 );
        m_deltaT             = 0.2;
        m_collisionT         = 0.1;
        m_explosionTime      = 0.5;
        m_minSpeed           = 1;
        strncpy(m_smokeAttrName,"Smoke.Attr.Small", sizeof( ct_AttrStr )-1 );
        m_smokeDamage        = 0.5;

        m_array[0].set("m_explAttrName",m_explAttrName);
        m_array[1].set("m_deltaT",m_deltaT);
        m_array[2].set("m_collisionT",m_collisionT);
        m_array[3].set("m_explosionTime",m_explosionTime);
        m_array[4].set("m_minSpeed",m_minSpeed);
        m_array[5].set("m_smokeAttrName",m_smokeAttrName);
        m_array[6].set("m_smokeDamage",m_smokeDamage);

        linkTable(m_array,7);
    }
	//}}END_OF_ATTRIBUTE
	
};



void AttributeOrphan::update(double )
{
	m_cacheExplosionTable = g_arena.searchSeanceClassTable( "Explosion" );
	m_cacheExplAttr	   = g_arena.getAttributeIndex( 
		g_arena.searchSeanceClassTable( "ExplosionAttr" ),
		context->searchObject(m_explAttrName) 
		);

    m_smokeTableID = g_arena.searchSeanceClassTable( "Smoke" );
    m_smokeAttrID = context->searchObject(m_smokeAttrName);
}


//===========================================================================
class AttributeTableOrphan : public ct_AttributeTable
{
protected:
    AttributeOrphan *m_table;
	
public:
    AttributeTableOrphan()
    {
		m_table = NULL;
		registerClass( "OrphanAttr" );
    }
	
    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static OrphanTable  __classTable;
static AttributeTableOrphan __attrTable;


/*********************************
*
*   Orphan implementation
*
*********************************/



//============================================================
Orphan::Orphan()
: m_viewDynObj(m_skin)     
{
    m_taxiAttr	= 0;
	m_attr		= 0;
}

//============================================================
Orphan::~Orphan()
{
}


void Orphan::KillMe(CFVector3 & newPos, double ts)
{
	createExplosion(newPos, ts,
					getObjectID(),
					m_attr->m_cacheExplosionTable,
					m_attr->m_cacheExplAttr
					);

	double clzTime;
	KR_ObjectID oID;
	CFVector3 dSpeed(0,-9.8,0);


	if(	checkCollision( 
			newPos ,				// начало движения
			dSpeed,    // напрвление со скоростью
				1,          // g_walkAttr0.fRadius,      // радиус
				1,          // время для проверки
				getObjectID(),  // кого игнорировать
				clzTime,                   // время, через которое стукнемся
				oID                        // объект, о который стукнемся
				) && clzTime < m_attr->m_collisionT)	
	{
		// мы на земле, создать труп
		// если мы на земле а не на динамическом объекте
		
		if (oID.isNUL())
			createCorpse(	getPosition() + dSpeed * clzTime,
							ts,
							getObjectID(),
							m_taxiAttr->m_cacheCorpseTable, 
							m_taxiAttr->m_cacheCorpseAttr);
	}

			
	context->removeObject( getObjectID() );
}



void Orphan::setOrphanAttr()
{
	ct_Attribute *attr = __attrTaxiTable.searchAttribute(m_orphanAttrID);
	
	if( attr==NULL )
		echo( "Orphan::receiveEvent: Unknown attribute %s",
		context->searchObject(m_orphanAttrID));
	else 
		m_taxiAttr = (AttributeTaxi *)attr;
	
	
	// Sound stuff, we can determine sound scheme for the orphan from
	// appropriate vehicle engine sound, which is accessable through
	// m_attrForVehicleName attribute of taxi
	
	
	KR_ObjectID	vehicleAttrID = context->searchObject(m_taxiAttr->m_attrForVehicleName);
	
	attr =__attrVehicleTable.searchAttribute(vehicleAttrID);
	if( attr==NULL )
		echo( "Vehicle::receiveEvent: Unknown attribute %s",
		context->searchObject(vehicleAttrID));
	else 
	{
		AttributeVehicle * vehicleAttr = (AttributeVehicle *) attr;
		
		if (!SetSoundAttr(	getObjectID(), 
			context,
			vehicleAttr->m_soundName,
			m_ctsndID, (void *)&m_wav))
			m_wav = NULL;
		
		if (m_wav)
		{
			updateSound( getObjectID(),
				context,
				m_ctsndID,
				m_wav,
				m_snd );
			
			m_audibleThisFrame = 1;
			onEnterAudibleZone(m_lastEventTime = Session::m_moment);
		}		
	}
	

	attr = __attrTable.searchAttribute(context->searchObject("Orphan.Attr.Default"));
	if( attr==NULL )
		echo( "Orphan::receiveEvent: Unknown attribute Orphan.Attr.Default");
	else 
		m_attr = (AttributeOrphan *)attr;

	m_skin.Attach(m_taxiAttr->m_cacheSkin);
	m_skin.GetDirModify().LoadIdentity();
	
	m_skin.SetUserAttrib(this);
	if(  strcmp(m_taxiAttr->m_skinName,"sk.Taxi.cln_f01")==0 )
		m_skin.SetAnimationCallback(AnimateCallBack1); 
	else                       
	if(  strcmp(m_taxiAttr->m_skinName,"sk.Taxi.cln_f08")==0 )
		m_skin.SetAnimationCallback(AnimateCallBack2); 

        ISkin *askin = (ISkin*)(context->queryInterface(m_taxiAttr->m_skinID, ISkinIID));

        if( askin && askin->isAutoAnim()  )
            askin->skinSetAnimAuto(&m_skin);
}


//============================================================
int Orphan::receiveEvent( KR_Event &event )
{
    switch( event.label )
    {
    case t_EVC_MOVING:
		{
			double dt = event.timeStamp - m_lastEventTime;
			m_lastMovePos    = getPosition();
                        m_lastMoveDeltaT = dt;
                        m_lastMoveTimeStamp = event.timeStamp;

			double clzTime;
			KR_ObjectID oID;
			
			CFVector3 newPos(getPosition()+m_speed * dt);
                        m_speed.y += -5*dt;
			if( 
				checkCollision( 
				newPos ,    // начало движения
				m_speed,    // напрвление со скоростью
				1,          // g_walkAttr0.fRadius,      // радиус
				1,          // время для проверки
				getObjectID(),  // кого игнорировать
				clzTime,                   // время, через которое стукнемся
				oID                        // объект, о который стукнемся
				) && clzTime < m_attr->m_collisionT)
			     KillMe(newPos, event.timeStamp);
			else
			{
				m_lastEventTime = Session::m_moment;
				//event.timeStamp += m_attr->m_deltaT;
				
				double haze = CViewFigure::HazeMax();
				event.timeStamp += m_attr->m_deltaT*(Abs(g_vehicle->getPos()-getPosition())+haze*0.2)/haze;
				
				issueEvent(event);					
				setPosition(newPos);
				
				// Sound
				if(  !m_snd.isNUL() && m_audibleThisFrame)
				{
					CFVector3 pos(getPosition());
					event.label = snd_EV_MOVE_TO;
					event.destination = m_snd;
					event.source      = getObjectID();
					event.data.open(EDO_WRITE)
						.putDouble(pos.x)
						.putDouble(pos.y)
						.putDouble(pos.z)
						.close();
					context->sendEventNow( event );
				}


				if (m_damage < m_attr->m_smokeDamage )
				{					
					createSmoke( getPosition(),
						         getObjectID(),
							     m_lastEventTime,
								 m_attr->m_smokeTableID,
								m_attr->m_smokeAttrID
								);
				}

				
			}
		}
		break;

    case KR_WAKE_UP:
		break;
		
		// FIXME	
    case t_EV_ONCOLLISION:
		
		createExplosion(getPosition(), event.timeStamp,
			getObjectID(),
			m_attr->m_cacheExplosionTable,
			m_attr->m_cacheExplAttr
			);
		
		context->removeObject( getObjectID() );
		break;
		
		
    case EV_VEHICLE_DROP_TAXI:	
		{
			CFVector3 pos;
			
			//KR_ObjectID oID;
                        int bcnt; // skolko bilo pul :(

			event.data.open(EDO_READ)
				.getObjectID(m_orphanAttrID)
				.getDouble(pos.x)
				.getDouble(pos.y)
				.getDouble(pos.z)
				.getDouble(m_damage)
                                .getInt(bcnt)
				.close();
			
			m_lastMovePos    = pos;
                        m_lastMoveDeltaT = 0;
                        m_lastMoveTimeStamp = event.timeStamp;

			setOrphanAttr();
			
			m_lastEventTime = event.timeStamp;
									
				
			//CFMatrix3x4 newDir;
			m_dir.LoadIdentity().LoadTransposed(g_vehicle->GetDir());
			
			SetDir(m_dir);
			setPosition(pos);
				
				event.label			= t_EVC_MOVING;
				event.source		= getObjectID();
				event.destination	= getObjectID();
				event.timeStamp	    = m_lastEventTime + m_attr->m_deltaT;
				
				issueEvent(event);
				
				m_speed = g_vehicle->Speed();
				

				
				if (Abs2(m_speed) < m_attr->m_minSpeed)
				{
					IDynamicObject *dobj=g_vehicle;
					CFMatrix3x4 m;
					dobj->getMatrix(m);

					m_speed = - m.Column(2) * 30;
					m_speed.y = -10;
				}
				
		}
		break;
		
    case KR_SET_ATTR: ASSERT(0);
		break;
		
    default: return 0;
    }
    return 1;
 }
 
 //============================================================
 void Orphan::addNotify()
 {
	 ct_Subject::addNotify();
	 m_snd = KR_ObjectID::NUL();
 }
 
 //============================================================
 void Orphan::removeNotify()
 {
	 if(  !m_snd.isNUL()  )
         context->removeObject( m_snd );
	 
	 ct_Subject::removeNotify();
 }
 
 //============================================================
 void Orphan::draw()
 {
 }
 
 /*************************************
 *
 *   OrphanTable implementation
 *
 *************************************/
 
 //============================================================
 void OrphanTable::allocObjects( int objectQnty )
 {
	 m_table = new Orphan[ objectQnty ];
	 
	 if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }
 
 //============================================================
 void OrphanTable::freeObjects()
 {
	 delete [] m_table;
	 m_table = NULL;
	 m_maxObjectQnty = 0;
 }
 
 //============================================================
 ct_Object *OrphanTable::getObjectPTR( int index )
 {
	 s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"OrphanTable::getObjectPTR");
	 return &(m_table[ index ]);
 }
 
 
 
 //============================================================
 void Orphan::render   ( CViewDynamicList &list, double ts)
 {
	 CFMatrix3x4 &m = m_skin.GetDirModify();
         CFVector3 p(getPosition());

         if(  m_lastMoveDeltaT >0.001 && m_lastMoveDeltaT < 0.2 )
              p += (getPosition()-m_lastMovePos)*
                   ((ts-m_lastMoveTimeStamp)/m_lastMoveDeltaT);
	 m.LoadOffset(p);
	 
	 m_viewDynObj.prepareToRender();
	 list.Load( &m_viewDynObj );
 }
 
 
 //============================================================
 
 void Orphan::endRender( CViewScene *scene )
 {
	 scene->RemoveLandDynamic( &m_viewDynObj );
 }
 
 //============================================================
 CFVector3  Orphan::realPosition() {  return getPosition();  }
 
 
 
 
 /*********************************
 *
 *   Orphan interfaces
 *
 *********************************/
 
 // IDynamicObject interface
 
 //============================================================
 CFVector3  Orphan::getPos      ()
 {
	 return getPosition();
 }
 
 //============================================================
 double     Orphan::getHAngle   ()
 {
	 return M_PI/2; // FIXME
 }
 
 //============================================================
 CFVector3  Orphan::getUpVector ()
 {
	 return CFVector3(0,1,0); // FIXME
 }
 
 //============================================================
 CFVector3  Orphan::getCenter   () // Относительно 0 объекта
 {
	 return m_skin.Model()->Center();
 }
 
 //============================================================
 double     Orphan::getRadius   () // Относительно центра
 {
	 return m_skin.Model()->Radius();
 }
 
 //============================================================
 double     Orphan::getRadius0  () // Относительно 0 объекта
 {
	 return m_skin.Model()->Radius0();
 }
 
 //============================================================
 CFVector3  Orphan::getMoveDir  () // Направление движения
 {
	 return CFVector3(0,1,0);
 }
 
 //============================================================
 double     Orphan::getMoveSpeed() // Скорость
 {
	 return 0;
 }
 
 //============================================================
 void      Orphan::getMatrix   ( CFMatrix3x4 &m )
 {
	 m.LoadIdentity(); 
 }
 
 // IUnit interface
 
 //============================================================
 
 void *Orphan::queryInterface( int interNum )
 {
	 switch( interNum )
	 {
	 case IUnknownIID:       return (KR_Object*)this;
	 case IDynamicObjectIID: return (IDynamicObject*)this;
	 }
	 return 0;
 }
 
 
 //============================================================
 
 
 void Orphan::onExitAudibleZone(double ts) 
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
 
 void Orphan::onEnterAudibleZone(double ts) 
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
 
 
 static
	 void AnimateCallBack1(CViewObjectBaseSet *,CViewObjectBase */*pBase*/,CViewObjectRef */*ref*/)
 {
 }
 
 static
	 void AnimateCallBack2(CViewObjectBaseSet *,CViewObjectBase */*pBase*/,CViewObjectRef */*ref*/)
 {
 }
 
 
 /*************************************
 *
 *   AttributeTable implementation
 *
 *************************************/
 
 //============================================================
 void AttributeTableOrphan::allocObjects( int objectQnty )
 {
	 m_table = new AttributeOrphan[ objectQnty ];
	 
	 if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }
 
 //============================================================
 void AttributeTableOrphan::freeObjects()
 {
	 delete [] m_table;
	 m_table         = NULL;
	 m_maxObjectQnty = 0;
 }
 
 //============================================================
 ct_Object *AttributeTableOrphan::getObjectPTR( int index )
 {
	 s_ASSERT(index>=0 && index <m_maxObjectQnty,"AttributeTable::getObjectPTR");
	 return &(m_table[ index ]);
 }
 
 
 void Orphan::onHide(double)
 {
	 context->removeObject( getObjectID() );
 }


bool	Orphan::dump(PIN_SaveFile & sf)
{
		if (!ct_Subject::dump(sf))
			return false;

		if (!sf.WriteData( (char *) & m_orphanAttrID, sizeof(OrphanData)  ))
			return false;

		return true;
}

bool	Orphan::load(PIN_SaveFile & sf)
{
		if (!ct_Subject::load(sf))
			return false;


		if (!sf.GetData( (char *) & m_orphanAttrID, sizeof(OrphanData)  ))
			return false;
		
		return true;
}

void	Orphan::loadNotify()
{
	ct_Subject::loadNotify(); 
	setOrphanAttr();
	SetDir(m_dir);
}
 
 /* End of file C:\NW\ARENA\OBASE\Orphan\Orphan.cpp */
