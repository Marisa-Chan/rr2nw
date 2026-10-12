/*
 * File  : C:\NW\ARENA\OBASE\Taxi\Taxi.cpp
 * Autor :
 * Ver   1.0 
 */

#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "Taxi.h"

#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/sndmsg.h"
#include "message/skinmsg.h"
#include "message/Unitmsg.h"
#include "message/vehiclemsg.h"
#include "kernel/h/session.h"
#include "message/dcrossmsg.h"

#include "zav.h"

#include "h/phisics.h"
#include "h/vehicle.h"
#include "h/olevel.h"
#include "storage/h/savefile.h"

static void AnimateCallBack1(CViewObjectBaseSet *,CViewObjectBase *pBase,CViewObjectRef *ref);
static void AnimateCallBack2(CViewObjectBaseSet *,CViewObjectBase *pBase,CViewObjectRef *ref);


 //===========================================================================
class TaxiTable : public ct_SubjectTable
{
 private:
    Taxi *m_table;
 public:
    TaxiTable()
    {
        m_table = NULL;
        registerClass( "Taxi" );
    }
    ~TaxiTable()
    {
        delete [] m_table;
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
    virtual bool       isRendering();
	virtual bool	   isAudible();
};

bool TaxiTable::isRendering()
{
   return true;
}

bool TaxiTable::isAudible()
{
	return true;
}



static TaxiTable  __classTable;
AttributeTableTaxi __attrTaxiTable;

 /*********************************
  *
  *   Taxi implementation
  *
  *********************************/

 //============================================================
Taxi::Taxi()
    : m_viewDynObj(m_skin)     
 {
    m_attr = 0;//&__defaultAttr;
 }

 //============================================================
Taxi::~Taxi()
 {
 }


void Taxi::setPosition ( const CFVector3 &pos )
{
	ct_Subject::setPosition (pos);
	KR_Event event;
	
	// Sound
	if(  !m_snd.isNUL() && m_audibleThisFrame)
	{					
		event.label = snd_EV_MOVE_TO;
		event.timeStamp   = Session::m_moment;
		event.destination = m_snd;
		event.source      = getObjectID();		
		event.data.open(EDO_WRITE)
			.putDouble(pos.x)
			.putDouble(pos.y)
			.putDouble(pos.z)
			.close();
		context->sendEventNow( event );
	}
}

void Taxi::onExitAudibleZone(double ts) 
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

void Taxi::onEnterAudibleZone(double ts) 
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



void Taxi::setTaxiAttr()
{
        ct_Attribute *attr = __attrTaxiTable.searchAttribute(m_taxiAttrID);
        if( attr==NULL )
             echo( "Taxi::receiveEvent: Unknown attribute %s",
                   context->searchObject(m_taxiAttrID));
        else 
			m_attr = (AttributeTaxi*)attr;

        m_skin.Attach(m_attr->m_cacheSkin);
		m_skin.GetDirModify().LoadIdentity();

		m_skin.SetUserAttrib(this);
        if(  strcmp(m_attr->m_skinName,"sk.Taxi.cln_f01")==0 )
             m_skin.SetAnimationCallback(AnimateCallBack1); 
        else
        if(  strcmp(m_attr->m_skinName,"sk.Taxi.cln_f08")==0 )
			m_skin.SetAnimationCallback(AnimateCallBack2); 

	m_askin = (ISkin*)(context->queryInterface(m_attr->m_skinID, ISkinIID));
        if( m_askin->isAutoAnim()  )
            m_askin->skinSetAnimAuto(&m_skin);

		if (m_attr->m_buzzing)
		{
			KR_ObjectID	vehicleAttrID = context->searchObject(m_attr->m_attrForVehicleName);
			
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
					onEnterAudibleZone(Session::m_moment);
				}		
			}	// attribute found
		} // buzzing

}

 //============================================================
int Taxi::receiveEvent( KR_Event &event )
 {

	CFVector3 pos;


    switch( event.label )
    {
    case KR_WAKE_UP:
          break;

    // FIXME	
    case t_EV_ONCOLLISION:
	    break;

    case t_EVC_MOVING:
           {
#define TOUP 4.0
#if 1
             CFVector3 from(getPosition()+CFVector3(0,TOUP,0)),
                       toDir(0,-100,0);
             double    clzTime;
             KR_ObjectID oID;

             if( 
                 checkCollision( 
                     from,    // начало движения
                     toDir,    // напрвление со скоростью
                     1,                        //g_walkAttr0.fRadius,      // радиус
                     500,                        // время для проверки
                     getObjectID(),  // кого игнорировать
                     clzTime,                   // время, через которое стукнемся
                     oID                        // объект, о который стукнемся
                   ))
             {
                   KR_Event ev;

                   ev.label = t_EVC_MOVING;
                   ev.source = getObjectID();
                   ev.destination = getObjectID();
                   ev.timeStamp = event.timeStamp + 0.06+(!m_isVisible);
                   issueEvent( ev );

                   setPosition(from + clzTime*toDir+CFVector3(0,-1.0,0)  );
             }
#endif
           }
           break;

    case EV_VEHICLE_DROP_TAXI:
		{
			
			
			//KR_ObjectID oID;
			event.data.open(EDO_READ)
				.getObjectID(m_taxiAttrID)
				.getDouble(pos.x)
				.getDouble(pos.y)
				.getDouble(pos.z)
				.getDouble(m_damage)
                                .getInt   (m_bulletCnt)
				.close();
			
			setTaxiAttr();
			
			
			SBumpDef def;
			def.start		= pos+CFVector3(0,TOUP,0);
			def.vel			= CFVector3(0,-100,0);
			def.fRadius		= 1.;// FIXME
			def.nBumpFlags	= 0;
			def.fMass		= 1;// FIXME
			def.fTime		= 10000.;
			
			if (ZAV_Scene()->Order()->Bump(def))
			{
				CFVector3 normal = def.vel1 - def.vel;
				
				double  fAngleX, fAngleY, fAngleZ;
				
				//CFMatrix3x4 m_taxiDir;

				m_taxiDir.LoadIdentity()
					.LoadTransposed(g_vehicle->GetDir())
					.RestoreEuler(CFMatrix3x4::AXIS_OY,fAngleY,CFMatrix3x4::AXIS_OX,fAngleX,CFMatrix3x4::AXIS_OZ,fAngleZ);
				
				GetMatrixByAngles( m_taxiDir, normal, -fAngleY - M_PI_2);
				
				
				SetDir(m_taxiDir);
				
				pos.y -= def.fTime*100-TOUP+1.0;// put to the ground
				setPosition(pos);

			
				switch(def.nBumpFlags)
				{
				case BF_BUMPSTATIC:
					{
						KR_Event ev;
						
						ev.label = t_EVC_MOVING;
						ev.source = getObjectID();
						ev.destination = getObjectID();
						ev.timeStamp = event.timeStamp + 0.06;
						issueEvent( ev );
					}
					break;
				}
			}
		}
     break;

    case KR_SET_ATTR:
		{
			
		
			KR_ObjectID oID;
			event.data.open(EDO_READ)
				.getObjectID(m_taxiAttrID)
				.getDouble(pos.x)
				.getDouble(pos.z)
				.close();
			
			setTaxiAttr();
			
			m_damage = m_attr->m_initialDamage;
			
			pos.y =  500;
			CFVector3 from(pos),
				toDir(0,-100,0);
			double    clzTime;
			KR_ObjectID ignored;
			
			if(  g_vehicle == 0  )
				ignored = getObjectID();
			else 
				ignored = g_vehicle->getObjectID();
			
			if( 
				checkCollision( 
				from,    // начало движения
				toDir,    // напрвление со скоростью
				1,                        //g_walkAttr0.fRadius,      // радиус
				500,                        // время для проверки
				ignored,  // кого игнорировать
				clzTime,                   // время, через которое стукнемся
				oID                        // объект, о который стукнемся
				))
			{
				
				for( int i = 1; !oID.isNUL(); ++i )
				{
					from = pos+CFVector3(i,500,0);
					
					if(  !checkCollision( 
						from,    // начало движения
						toDir,    // напрвление со скоростью
						1,                        //g_walkAttr0.fRadius,      // радиус
						500,                        // время для проверки
						ignored,                   // кого игнорировать
						clzTime,                   // время, через которое стукнемся
						oID                        // объект, о который стукнемся
						))
					{
						double height;
						CFVector3 normal;
						CViewScene::Current()->GetTerrain()->GetPlane(m_position,normal,height);
						pos.y = height;
						setPosition(pos);
						break;
					}
					
				}
				
				setPosition(from + clzTime*toDir +CFVector3(0,-1,0)  );
			}
			else 
			{
				double height;
				CFVector3 normal;
				CViewScene::Current()->GetTerrain()->GetPlane(m_position,normal,height);
				pos.y = height;
				setPosition(pos);
			}
			
	
			
		}
     break;

    default: return 0;
    }
    return 1;
 }

 //============================================================
void Taxi::addNotify()
 {
    ct_Subject::addNotify();
	m_taxiDir.LoadIdentity();
    // insert your code this
    m_bulletCnt = g_levelAttr.m_maxSecBulletCnt;
 }

 //============================================================
void Taxi::removeNotify()
 {
	 if(  !m_snd.isNUL()  )
         context->removeObject( m_snd );

    ct_Subject::removeNotify();
    // insert your code this
 }

 //============================================================
void Taxi::draw()
 {
 }

 /*************************************
  *
  *   TaxiTable implementation
  *
  *************************************/

 //============================================================
void TaxiTable::allocObjects( int objectQnty )
 {
    m_table = new Taxi[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void TaxiTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *TaxiTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"TaxiTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 /*************************************
  *
  *   AttributeTable implementation
  *
  *************************************/

 //============================================================
void AttributeTableTaxi::allocObjects( int objectQnty )
 {
    m_table = new AttributeTaxi[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void AttributeTableTaxi::freeObjects()
 {
    delete [] m_table;
    m_table         = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *AttributeTableTaxi::getObjectPTR( int index )
 {
    s_ASSERT(index>=0 && index <m_maxObjectQnty,"AttributeTable::getObjectPTR");
    return &(m_table[ index ]);
 }


 //============================================================
void Taxi::render   ( CViewDynamicList &list, double )
{
    CFMatrix3x4 &m = m_skin.GetDirModify();
    m.LoadOffset(getPosition()+CFVector3(0,m_attr->m_yOffset,0));

    m_viewDynObj.prepareToRender();
    list.Load( &m_viewDynObj );
}


 //============================================================

void Taxi::endRender( CViewScene *scene )
{
    scene->RemoveLandDynamic( &m_viewDynObj );
}

 //============================================================
CFVector3  Taxi::realPosition() {  return getPosition();  }


 /*************************************
  *
  *   AttributeBird implementation
  *
  *************************************/

 //============================================================


void AttributeTaxi::update(double ts)
{

   /*
    * Search and get skin
    */
    KR_ObjectID skinID =  context->searchObject( m_skinName );
    s_ASSERT( !skinID.isNUL(), "AttributeTaxi::update" );
    
    m_skinID = skinID;

    KR_Event event;
    event.timeStamp   = ts;
    event.label       = sk_EV_QUERY_MODEL_PTR;
    event.destination = skinID;
    context->sendEventNow( event );
    s_ASSERT(event.label==sk_EV_QUERY_MODEL_PTR_OK,"AttributeTaxi::update");
    event.data.open(EDO_READ)
                .get(&m_cacheSkin,sizeof(void*))
              .close();

    m_attrForVehicle = context->searchObject(m_attrForVehicleName);

    m_cacheCorpseTable	= g_arena.searchSeanceClassTable( "Corpse" );
    m_cacheCorpseAttr	= g_arena.getAttributeIndex( 
                                    g_arena.searchSeanceClassTable( "CorpseAttr" ),
                                    context->searchObject(m_corpseAttrName) 
                                 );

}




 /*********************************
  *
  *   Taxi interfaces
  *
  *********************************/

    // IDynamicObject interface

 //============================================================
CFVector3  Taxi::getPos      ()
{
    return getPosition();
}

 //============================================================
double     Taxi::getHAngle   ()
{
    return M_PI/2; // FIXME
}

 //============================================================
CFVector3  Taxi::getUpVector ()
{
    return CFVector3(0,1,0); // FIXME
}

 //============================================================
CFVector3  Taxi::getCenter   () // Относительно 0 объекта
{
    return m_skin.Model()->Center();
}

 //============================================================
double     Taxi::getRadius   () // Относительно центра
{
    return m_skin.Model()->Radius();
}

 //============================================================
double     Taxi::getRadius0  () // Относительно 0 объекта
{
    return m_skin.Model()->Radius0();
}

 //============================================================
CFVector3  Taxi::getMoveDir  () // Направление движения
{
    return CFVector3(0,1,0);
}

 //============================================================
double     Taxi::getMoveSpeed() // Скорость
{
    return 0;
}

 //============================================================
void      Taxi::getMatrix   ( CFMatrix3x4 &m )
{
    m.LoadIdentity();
    //m.TranslateL( getPosition() );
}

    // IUnit interface
 //============================================================
double Taxi::getPower() // Сила юнита 0..10
{
    return 0.0;
}

 //============================================================
int    Taxi::isFriend( const KR_ObjectID & )
{
    return 1;
}

 //============================================================
double Taxi::getDamage() // Целостность от 0..1
{
    return m_damage;
}

 //============================================================
KR_ObjectID Taxi::getAttributeForVehicle()
{
    return m_attr->m_attrForVehicle;
}

 //============================================================
CFVector3  Taxi::taxiPos()
{
    return getPosition();
}

double Taxi::desireShoot()
{
    return 0;
}

void *Taxi::queryInterface( int interNum )
 {
    switch( interNum )
    {
    case IUnknownIID:       return (KR_Object*)this;
    case IDynamicObjectIID: return (IDynamicObject*)this;
    case IUnitIID:          return (IUnit*)this;
    case ITaxiIID:          return (ITaxi*)this;
    }

    return 0;
 }


void Taxi::setDamage  ( double d, const CFVector3 &, double ts, KR_ObjectID )
{
   m_damage -= d;
   echo( "damage=%lg", m_damage);
   if(  m_damage <= 0  )
   {
		createCorpse(	getPosition() , 
						ts,
						getObjectID(),
						m_attr->m_cacheCorpseTable, 
						m_attr->m_cacheCorpseAttr);
		
		context->removeObject(getObjectID() );
   }
}
 
KR_ObjectID Taxi::getCommander()
{
    return KR_ObjectID::NUL();
}



static
void AnimateCallBack1(CViewObjectBaseSet *,CViewObjectBase */*pBase*/,CViewObjectRef */*ref*/)
 {
    //(void)pBase;
 }

static
void AnimateCallBack2(CViewObjectBaseSet *,CViewObjectBase */*pBase*/,CViewObjectRef */*ref*/)
 {
    //(void)pBase;
 }

void Taxi::setCommander(KR_ObjectID)
{
    echo("Taxi::setCommander don't support");
}

void  Taxi::taxiSetBulletCnt(int cnt)
{
    m_bulletCnt = cnt;
}

int   Taxi::taxiGetBulletCnt()
{
    return m_bulletCnt;
}


bool	Taxi::dump(PIN_SaveFile & sf)
{
		if (!ct_Subject::dump(sf))
			return false;

		if (!sf.WriteData( (char *) & m_taxiAttrID, sizeof(TaxiData)  ))
			return false;

		return true;
}

bool	Taxi::load(PIN_SaveFile & sf)
{
		if (!ct_Subject::load(sf))
			return false;

		if (!sf.GetData( (char *) & m_taxiAttrID, sizeof(TaxiData)  ))
			return false;
		
		return true;
}

void	Taxi::loadNotify()
{
	ct_Subject::loadNotify();
	setTaxiAttr();
	SetDir(m_taxiDir);		
	CFVector3 pos = getPosition();
	setPosition(pos);
}

/* End of file C:\NW\ARENA\OBASE\Taxi\Taxi.cpp */
