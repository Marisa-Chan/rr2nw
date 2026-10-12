/*
 * File  : C:\WinGame\OBASE\Howitzer\Howitzer.cpp
 * Autor :
 * Ver   1.0 
 */


//#include "storage/h/subject.h"
//#include "storage/h/attr.h"
//#include "kernel/h/active.h"




#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "zav.h"


#include "i/dynobj.i"
#include "i/unit.i"
#include "storage/h/strgdefs.h"
#include "..\bullet\bullet.h"
#include "..\DynObj\DynObj.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "message/howitzermsg.h"
#include "message/skinmsg.h"
#include "message/peopmsg.h"
#include "message/bulmsg.h"
#include "enum/spaceEnum.h"
#include "super.h"
#include "phisics.h"


#include "howitzer.h"
#include "storage/h/savefile.h"

#include "message/dcrossmsg.h"
#include "..\dcross\dcross.h"


 //===========================================================================
class AttributeHowitzer : public ct_Attribute
{
	
 public:
    CViewObjectModel*   m_cacheSkin              ;  // 
    virtual void		update(double ts);  

	ct_ClassTableID     m_cacheCorpseTable;
    int					m_cacheCorpseAttr;


    int                 m_bulletIndex;
    ct_ClassTableID     m_bulletTable;
	double				m_bulletSpeed;

//{{ATTRIBUTE
    ct_AttrItem  m_array[19];
    ct_AttrStr      m_skinName               ;  // 
    ct_AttrStr      m_corpseAttrName         ;  // 
    double          m_initialDamage          ;  // 
    double          m_fireSpeed              ;  // Время между выстрелами
    double          m_turnSpeed              ;  // 
    double          m_addRoll                ;  // Начальный поворот пушки. Бить тех, кто скины ориентирует неправильно
    double          m_shootAngle             ;  // 
    ct_AttrStr      m_bulletAttrName         ;  // 
    double          m_dx                     ;  // Координата точки ортносительно центра, из которой вылетают пульки
    double          m_dy                     ;  // 
    double          m_dz                     ;  // 
    int             m_dumbness               ;  // "Тупость" пушки. 0 - совсем тупая, 1 - стреляет с упреждением
    double          m_deflectionXMax         ;  // Случайное отклонение при стрельбе. Пушка можеит мазать
    double          m_deflectionXMin         ;  // 
    double          m_deflectionYMax         ;  // 
    double          m_deflectionYMin         ;  // 
    double          m_deflectionZMax         ;  // 
    double          m_deflectionZMin         ;  // 
    double          m_maxBulletFlyTime       ;  // Тщательно прицеливаетмся и стреляем точно чтобы попасть. Если пуля в течении m_maxBulletFlyTime секунд не попадает  в цель или бампится, то ищем новую цель

    AttributeHowitzer()
    {
        strncpy(m_skinName,"", sizeof( ct_AttrStr )-1 );
        strncpy(m_corpseAttrName,"Corpse.Attr.Default", sizeof( ct_AttrStr )-1 );
        m_initialDamage      = 1.0;
        m_fireSpeed          = 5.0;
        m_turnSpeed          = 0.2;
        m_addRoll            = 0;
        m_shootAngle         = 0.93;
        strncpy(m_bulletAttrName,"", sizeof( ct_AttrStr )-1 );
        m_dx                 = 0;
        m_dy                 = 0;
        m_dz                 = 0;
        m_dumbness           = 0;
        m_deflectionXMax     = 1.1;
        m_deflectionXMin     = 0.9;
        m_deflectionYMax     = 1.1;
        m_deflectionYMin     = 0.9;
        m_deflectionZMax     = 1.1;
        m_deflectionZMin     = 0.9;
        m_maxBulletFlyTime   = 5;

        m_array[0].set("m_skinName",m_skinName);
        m_array[1].set("m_corpseAttrName",m_corpseAttrName);
        m_array[2].set("m_initialDamage",m_initialDamage);
        m_array[3].set("m_fireSpeed",m_fireSpeed);
        m_array[4].set("m_turnSpeed",m_turnSpeed);
        m_array[5].set("m_addRoll",m_addRoll);
        m_array[6].set("m_shootAngle",m_shootAngle);
        m_array[7].set("m_bulletAttrName",m_bulletAttrName);
        m_array[8].set("m_dx",m_dx);
        m_array[9].set("m_dy",m_dy);
        m_array[10].set("m_dz",m_dz);
        m_array[11].set("m_dumbness",m_dumbness);
        m_array[12].set("m_deflectionXMax",m_deflectionXMax);
        m_array[13].set("m_deflectionXMin",m_deflectionXMin);
        m_array[14].set("m_deflectionYMax",m_deflectionYMax);
        m_array[15].set("m_deflectionYMin",m_deflectionYMin);
        m_array[16].set("m_deflectionZMax",m_deflectionZMax);
        m_array[17].set("m_deflectionZMin",m_deflectionZMin);
        m_array[18].set("m_maxBulletFlyTime",m_maxBulletFlyTime);

        linkTable(m_array,19);
    }
//}}END_OF_ATTRIBUTE
};

//static AttributeHowitzer __defaultAttr;

 //===========================================================================
//strg_SUBJECT_TABLE_IMPLEMENTATION(Howitzer,1)

class HowitzerTable : public ct_SubjectTable{
 private:                                                   
  Howitzer *m_table;                                           
 public:                                                    
    HowitzerTable()                                           
    {                                                       
      m_table = NULL;                                       
      registerClass("Howitzer");                                 
    }                                                       
    ~HowitzerTable()                                          
    {                                                       
      delete [] m_table;                                    
      m_table = NULL;                                       
    }                                                       
    virtual void       allocObjects( int objectQnty );      
    virtual void       freeObjects ();                      
    virtual ct_Object *getObjectPTR( int index );           
    virtual bool       isRendering();                       
};                                                          


void HowitzerTable::allocObjects( int objectQnty )
 {                                                                            
    m_table = new Howitzer[ objectQnty ];                                          
    if(  m_table == NULL  )                                                   
         m_maxObjectQnty = 0;                                                 
 }                                                                            
void HowitzerTable::freeObjects()                                                      
 {                                                                            
    delete [] m_table;                                                        
    m_table         = NULL;                                                   
    m_maxObjectQnty = 0;                                                      
 }  
                                                                          
ct_Object *HowitzerTable::getObjectPTR( int index )                                    
 {                                                                            
    s_ASSERT(index>=0 && index <m_maxObjectQnty,"Howitzer::getObjectPTR()"); 
    return &(m_table[ index ]);                                               
 } 

bool HowitzerTable::isRendering() 
{ return true; }             



strg_ATTRIBUTE_TABLE_IMPLEMENTATION(Howitzer,"HowitzerAttr")

static HowitzerTable          __classTable;
 /*********************************
  *
  *   Howitzer implementation
  *
  *********************************/

 //============================================================
strg_CONSTRUCTOR_DYNVIEW(Howitzer)
 {
    m_attr = 0;
	m_commanderID = KR_ObjectID::NUL();
	m_enemyID	  = KR_ObjectID::NUL();
	m_rotateOy	  = 0;
	m_hAngle      = 0;
 }

 //============================================================
Howitzer::~Howitzer()
 {
 }


void Howitzer::setHowitzerAttr()
{
	ct_Attribute *attr = __attrTable.searchAttribute(m_HowitzerAttrID);
    if( attr==NULL )
		echo( "Howitzer::receiveEvent: Unknown attribute %s",
		context->searchObject(m_HowitzerAttrID));
	else m_attr = (AttributeHowitzer*)attr;
	
	m_skin.Attach(m_attr->m_cacheSkin);
	m_viewDynObj.BumpDef().fRadius = m_skin.Model()->Radius();
	
	m_damage = m_attr->m_initialDamage;
	
	
	CFVector3 pos = g_super.m_level.m_howitzerPool[m_HolderIndex].pos;
	// set on the surface
	
	SBumpDef def;
	def.start		= pos+CFVector3(0,500,0);	// 500 meters are quite enough I suppose
	def.vel			= CFVector3(0,-9.8,0);
	def.fRadius		= 1.;// FIXME
	def.nBumpFlags	= 0;
	def.fMass		= 1;// FIXME
	def.fTime		= 10000.;
	
	VERIFYMSG(ZAV_Scene()->Order()->Bump(def),"cannot bump the howitzer");
	
	pos.y = pos.y + 500 - def.fTime * 9.8 - 1;
	
	setPosition(pos);
	
	//dc_CreateCross(0,getPosition(),60,0,0,1,1, 0,"HOWITZER");
}


void Howitzer::CheckBlockUp()
{
	
	if (!m_enemyID.isNUL())
	{
		IDynamicObject * dObj = (IDynamicObject *) 
			(context->queryInterface( m_enemyID, IDynamicObjectIID ));
		
		if (!dObj)
		{
			m_enemyID = KR_ObjectID::NUL();
			return;
		}
		
		CFVector3 bulletPos;					
		CFVector3 ourDir = CFVector3(cos(m_hAngle),0,sin(m_hAngle));		
				
		bulletPos.x = m_position.x + m_attr->m_dx * ourDir.x; // cos(m_hAngle)
		bulletPos.y = m_position.y + m_attr->m_dy; 
		bulletPos.z = m_position.z + m_attr->m_dz * ourDir.z; // sin(m_hAngle)
		
		
		
		CFVector3 enemyPos = dObj->getPos();
		CFVector3 dP = Normal(enemyPos - bulletPos);


		KR_ObjectID oID;
		double clzTime;
		
		int shouldFire  = 
			checkCollision( 
			m_position,				// начало движения
			dP * m_attr->m_bulletSpeed,    // напрвление со скоростью
			1,                         //      // радиус
			m_attr->m_maxBulletFlyTime,// время для проверки
			getObjectID(),			    // кого игнорировать
			clzTime,                   // время, через которое стукнемся
			oID                        // объект, о который стукнемся
			);
		
		if (shouldFire)
		{
			if (oID != m_enemyID)
				shouldFire = 0;
		}
		
		
		if ( !shouldFire )	// target blocked up
							// or too far
		{
			//echo ("Target Blocked Up");			
			m_enemyID = KR_ObjectID::NUL();
		}
	}
}

 //============================================================
int Howitzer::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
		
		
	case HOWITZER_ACTION:
		{
			
			event.timeStamp += 0.2 + (!m_isVisible) * 1.5;
			issueEvent(event);
			
			double deltaT = Session::m_moment - m_lastActionTime;
			m_lastActionTime = Session::m_moment;
			
			
			if (!m_enemyID.isNUL())
			{
				IDynamicObject * dObj = (IDynamicObject *) 
					(context->queryInterface( m_enemyID, IDynamicObjectIID ));
				
				if (!dObj)
				{
					m_enemyID = KR_ObjectID::NUL();
					break;
				}
				
				CFVector3 enemyPos = dObj->getPos();
				CFVector3 ourPos   = getPos();
				CFVector3 dP	 = enemyPos - ourPos;
				
				double hAngle = atan2( dP.z, dP.x );
				
				m_hAngle = interpolateAngle( m_hAngle, hAngle,
					m_attr->m_turnSpeed,
					deltaT );
				
				m_rotateOy = m_attr->m_addRoll - M_PI/2-m_hAngle;		
				
				dP = Normal(dP);				
				dP.y = 0;
				
				
				CFVector3 ourDir = CFVector3(cos(m_hAngle),0,sin(m_hAngle));
				
				double coef = ourDir * dP;
				
				if (coef > m_attr->m_shootAngle)
				{				
					
					if (!m_shootThisBastard || 
						(m_shootThisBastard && (Session::m_moment - m_lastShootTime > m_attr->m_fireSpeed )))
					{	
						
						CFVector3 bulletPos;			
						
						event.timeStamp   = Session::m_moment;						
						event.destination = getObjectID();
						event.source      = getObjectID();
						
						bulletPos.x = m_position.x + m_attr->m_dx * ourDir.x; // cos(m_hAngle)
						bulletPos.y = m_position.y + m_attr->m_dy; 
						bulletPos.z = m_position.z + m_attr->m_dz * ourDir.z; // sin(m_hAngle)
						
						dP = Normal(enemyPos - bulletPos);																		
						
						m_shootThisBastard = 1;						
						
						event.label = HOWITZER_SHOOT;												
						event.data.open(EDO_WRITE)
							.putDouble(bulletPos.x)
							.putDouble(bulletPos.y)
							.putDouble(bulletPos.z)
							
							.putDouble(ourDir.x * context->rnd_f(m_attr->m_deflectionXMin,m_attr->m_deflectionXMax))
							.putDouble(dP.y * context->rnd_f(m_attr->m_deflectionXMin,m_attr->m_deflectionXMax))
							.putDouble(ourDir.z * context->rnd_f(m_attr->m_deflectionXMin,m_attr->m_deflectionXMax))
							
							.close();
						
						context->sendEventNow(event);
					}
				}
				else 
					m_shootThisBastard = 0;
				
			}					
			
		}
		
		break;
		
	case HOWITZER_SHOOT:
		{
			
			if (!m_shootThisBastard)
				break;
			
			m_lastShootTime = Session::m_moment;
			
			CFVector3 pos;
			CFVector3 dir;
			
			event.data.open(EDO_READ)
				.getDouble(pos.x)
				.getDouble(pos.y)
				.getDouble(pos.z)
				
				.getDouble(dir.x)
				.getDouble(dir.y)
				.getDouble(dir.z)					
				.close();
			
			
			KR_Event event;
			
			event.label       = b_EV_START;
			event.source      = getObjectID();
			event.destination = g_arena.newObject( m_attr->m_bulletTable , "B" );
			
			if(  !event.destination.isNUL()  )
			{
				
				//{{PUT_EVENT(b_EV_START)
				event.data.open(EDO_WRITE)
					.descend( VECTOR3D_F, 0 )
					.putDouble(pos.x)
					.putDouble(pos.y)
					.putDouble(pos.z)
					.ascend()
					.descend( VECTOR3D_F, 0 )
					.putDouble(dir.x)
					.putDouble(dir.y)
					.putDouble(dir.z)
					.ascend()
					.putInt(m_attr->m_bulletIndex)
					.putObjectID(getObjectID() )
					.close();
				//}}END_OF_PUT_EVENT(b_EV_START)
				event.timeStamp = Session::m_moment;
				context->sendEventNow( event );
			}
		}
		break;
		
	case HOWITZER_FIND_ENEMY:
		{
			
			event.timeStamp += 2.0 + (!m_isVisible) * 5;
			issueEvent(event);						
			
			ct_SubjectFindData fsd;		
			m_enemyID = KR_ObjectID::NUL();
			double             K   = -1e3;
			double xMin,xMax, zMin, zMax;
			
			CFVector3 pos(realPosition());
			
			xMin = xMax = pos.x;
			zMin = zMax = pos.z;
			xMin -= 300; xMax += 300;
			zMin -= 300; zMax += 300;
			
			ct_Arena::findFirstSubject( fsd, xMin, zMin, xMax, zMax );
			
			for( int i = 0; i < fsd.getCount(); ++i )
			{
				const KR_ObjectID  &enemyID = fsd[i];
				IUnit *u =(IUnit *)(context->queryInterface( enemyID, IUnitIID ));
				
				if(  u==0  )
					continue;
				
				if(  !u->isFriend(m_commanderID)  )
				{
					double     power = u->getPower();
					IDynamicObject *dobj = (IDynamicObject *)
						(context->queryInterface( enemyID, IDynamicObjectIID ));
					
					if(  dobj != 0  )
					{
						
						CFVector3 pos(dobj->getPos());
						double curK = power/(1+Abs(CFVector2(m_position.x,m_position.z)-CFVector2(pos.x,pos.z)));
						if(  curK > K  )
						{
							K		  = curK;
							m_enemyID = enemyID;
						}
					}
				}
			}
			
			if (!m_enemyID.isNUL())
				CheckBlockUp();						
			
			m_lastEnemyScanTime = Session::m_moment;
			
		}
		break;
		
		
	case pe_EVCMD_START:
		{
			
			char holder[HOWITZER_MAX_NAME + 1];								
			
			event.data.open(EDO_READ)
				.getObjectID(m_HowitzerAttrID)
				.getStr     (holder,HOWITZER_MAX_NAME)                     
				.close();
			
			m_HolderIndex = g_super.m_level.AttachToHowitzerHolder(holder, getObjectID() );
			
			setHowitzerAttr();
			
			
			m_lastActionTime = m_lastEnemyScanTime = Session::m_moment;
			
			
			// Starting cycles
			
			event.label = HOWITZER_FIND_ENEMY;
			context->sendEventNow(event);
			event.label = HOWITZER_ACTION;
			context->sendEventNow(event);
			
			m_shootThisBastard = 0;
			
		}
		
		break;
		
    case KR_SET_ATTR:
		s_ASSERTNQ("Howitzer");
		break;
		
	case KR_WAKE_UP:
		break;
		
    default: return 0;
    }
    return 1;
 }

 //============================================================
void Howitzer::addNotify()
 {
    ct_Subject::addNotify();    
 }

 //============================================================
void Howitzer::removeNotify()
{
	g_super.m_level.ReleaseHolder(m_HolderIndex, getObjectID() );
    ct_Subject::removeNotify();
    // insert your code this
}


void      *Howitzer::queryInterface( int interNum )
{
    switch( interNum )
    {
    case IUnknownIID:       return (KR_Object*)this;
    case IDynamicObjectIID: return (IDynamicObject*)this;
    case IUnitIID:          return (IUnit*)this;
    }

    return 0;
}





bool	Howitzer::dump(PIN_SaveFile & sf)
{
	
	if (!ct_Subject::dump(sf))
		return false;
	
	if (!sf.WriteData( (char *) & m_HowitzerAttrID, sizeof(HowitzerData)  ))
		return false;
	
	return true;
}

bool	Howitzer::load(PIN_SaveFile & sf)
{
	if (!ct_Subject::load(sf))
		return false;
	
	if (!sf.GetData( (char *) & m_HowitzerAttrID, sizeof(HowitzerData)  ))
		return false;
	
	return true;
}

void	Howitzer::loadNotify()
{
	ct_Subject::loadNotify();

	g_super.m_level.AttachToHowitzerHolder(m_HolderIndex, getObjectID() );

	setHowitzerAttr();
}


void   Howitzer::render( CViewDynamicList &list, double )  
{                                                 
	CFMatrix3x4 &m = m_skin.GetDirModify();
    m.LoadIdentity();
        
    m.RotateOyL(m_rotateOy);
    
	m.LoadOffset(getPosition());                          

    m_viewDynObj.prepareToRender();               
    list.Load( &m_viewDynObj );                   
}                                                 

void Howitzer::endRender( CViewScene *scene )         
{                                                 
    scene->RemoveLandDynamic( &m_viewDynObj );    
}                                               

 //============================================================
CFVector3     Howitzer::realPosition() {  return getPosition();  }



 /*************************************
  *
  *   AttributeHowitzer implementation
  *
  *************************************/

 //============================================================


void AttributeHowitzer::update(double ts)
{
   strg_UPDATE_ATTRIBUTE_SKIN(m_skinName,m_cacheSkin,ts)

   m_cacheCorpseTable	= g_arena.searchSeanceClassTable( "Corpse" );
   m_cacheCorpseAttr	= g_arena.getAttributeIndex( 
                          g_arena.searchSeanceClassTable( "CorpseAttr" ),
                          context->searchObject(m_corpseAttrName) 
                        );

	if(  m_bulletAttrName[0]!=0  )
    {
         m_bulletTable=g_arena.searchSeanceClassTable("Bullet");

         m_bulletIndex = g_arena.getAttributeIndex(
                                       g_arena.searchSeanceClassTable("BulletAttr"),
                                       context->searchObject(m_bulletAttrName)
                                      );

		 AttributeBullet bulletAttr;
		 __bulletAttrTable.setAttribute(m_bulletIndex,(ct_Attribute *&) bulletAttr);
		 m_bulletSpeed = bulletAttr.m_startSpeed;
    }

}

// IDynamicObject *****************************

CFVector3  Howitzer::getPos      ()
{
    CFMatrix3x4 m;
    getMatrix(m);
    return getPosition()+m*getCenter();
}

double     Howitzer::getHAngle   ()
{
     return m_rotateOy;
}

CFVector3  Howitzer::getUpVector ()
{
     return CFVector3(0,1,0); 
}

CFVector3  Howitzer::getCenter   () // Относительно 0 объекта
{
    return m_skin.Model()->Center();
}

double     Howitzer::getRadius   () // Относительно центра
{
    return m_skin.Model()->Radius();
}

double     Howitzer::getRadius0  () // Относительно 0 объекта
{
    return m_skin.Model()->Radius0();
}

CFVector3  Howitzer::getMoveDir  () // Направление движения
{
    return CFVector3(0,0,0);
}

double     Howitzer::getMoveSpeed() // Скорость
{
    return 0;	// we don't move!
}

void       Howitzer::getMatrix   ( CFMatrix3x4 &m )
{
    m.LoadIdentity();     
}


double Howitzer::getPower   () // Сила юнита 0..10
{
   return 0.1;
}

int    Howitzer::isFriend   ( const KR_ObjectID & commanderID)
{
   return m_commanderID==commanderID;;
}

double Howitzer::getDamage  () // Целостность от 0..1
{
   return m_damage;
}

void   Howitzer::setDamage  ( double d, const CFVector3 &, double ts, KR_ObjectID )
{

   m_damage -= d;   
   if(  m_damage <= 0  )
   {
	if (m_isVisible)
	{
		createCorpse(	getPosition() , 
						ts,
						getObjectID(),
						m_attr->m_cacheCorpseTable, 
						m_attr->m_cacheCorpseAttr);
	}
		
		context->removeObject(getObjectID() );
   }
}



KR_ObjectID Howitzer::getCommander()
{
   return m_commanderID;
}

double Howitzer::desireShoot()
{
   return 0;
}

void   Howitzer::setCommander(KR_ObjectID oID)
{
    m_commanderID	= oID;

    KR_Event        event;

    event.label			= HOWITZER_FIND_ENEMY;
    event.source		= getObjectID();
    event.destination	= getObjectID();
    event.timeStamp		= Session::m_moment+5.0;
    issueEvent( event );
}



/* End of file C:\WinGame\OBASE\Howitzer\Howitzer.cpp */