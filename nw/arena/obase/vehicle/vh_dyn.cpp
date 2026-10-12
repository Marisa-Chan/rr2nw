/*
 * File  : C:\NW\ARENA\OBASE\Vehicle\Vehicle.cpp
 * Autor :
 * Ver   1.0 
 */
#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "Vehicle.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/vehiclemsg.h"
#include "message/bulmsg.h"
#include "phisics.h"
#include "vh_vessel.h"
#include "vs_zav.h"
#include "hardware.h"
#include "kernel/h/session.h"
#include "graph.h"
#include "i/taxi.i"
#include "enum/spaceEnum.h"
#include "sound.h"
#include "filesys.h"
#include "briefing.h"

#include "obase/sound/wavobj.h"
#include "message/skinmsg.h"
#include "../taxi/taxi.h"
#include "h/olevel.h"

//CVesselEmv   SEmvAttrs;
//CVesselWheels SWheelsAttrs;




Vehicle *g_vehicle = 0;

SEmvAttrs      g_emvAttr0;
SEmvAttrs      g_emvAttrDragon;
SWheelsAttrs   g_walkAttr0;
SWheelsAttrs   g_tankAttr1;
SWheelsAttrs   g_tankAttr2;
SWheelsAttrs   g_tankAttr3;
SWheelsAttrs   g_dead;

CVesselEmv     g_emv;
CVesselWheels  g_tank;
CVesselWheels  g_walk;



//==========================================================================
void Vehicle::addNotify()
{
	ct_Subject::addNotify();

        carrierAddNotify(context,Session::m_moment);
        m_secBulletCnt = 30;
        m_attr   = 0;
        m_vessel = 0;
        m_firePrim = false;
        m_fireSec  = false;
        m_primEnable = true;
        m_secEnable  = true;
        m_firePrimPress = false;
        m_fireSecPress  = false;
        m_playedBrief   = false;
        m_lastLeaveTime = -10;
        m_skipTime = -1;
        m_lastTime = Session::m_moment;
	
	m_dead = 0;
	m_isTakingTaxiNow = 0;                                                  
        
    m_vehicleAttrDefaultID = context->searchObject("Vehicle.Attr.default");
	m_vehicleAttrDeadID    = context->searchObject("Vehicle.Attr.dead");

    RTCHECK(!m_vehicleAttrDefaultID.isNUL(),"Unknown object 'Vehicle.Attr.default'");
	RTCHECK(!m_vehicleAttrDeadID.isNUL(),"Unknown object 'Vehicle.Attr.dead'");

	InitSound();
}

//-------------------------------------------------------------
void Vehicle::removeNotify()
{
	ct_Subject::removeNotify();
        carrierRemoveNotify(context,Session::m_moment);

	if (m_lpCE)
	{
		m_lpCE->Release();
		m_lpCE = NULL;
	}

	/*if (m_lpCannonCE)
	{
		m_lpCannonCE->Release();
		m_lpCannonCE = NULL;
	}*/

	if (m_lpDL)
	{
        	m_lpDL->Release();
        	m_lpDL = NULL;        
	}
}
	

VehicleTable  __classTable;
AttributeTableVehicle __attrVehicleTable;

 /*************************************
  *
  *   VehicleTable implementation
  *
  *************************************/

void VehicleTable::ReadConfig()
{
    CConfigFile cf("vessels.cfg");
    g_walkAttr0.Read(cf,"Walk0");
    g_tankAttr1.Read(cf,"Tank1");
    g_tankAttr2.Read(cf,"Tank2");
    g_tankAttr3.Read(cf,"Tank3");
    g_dead.Read(cf,"Dead");
    g_emvAttr0.Read(cf,"Emv0");
    g_emvAttrDragon.Read(cf,"Dragon");
}



 //============================================================
void VehicleTable::allocObjects( int objectQnty )
 {
    ReadConfig();
    m_table = new Vehicle[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void VehicleTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *VehicleTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"VehicleTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 /*************************************
  *
  *   AttributeTable implementation
  *
  *************************************/

 //============================================================
void AttributeTableVehicle::allocObjects( int objectQnty )
 {
    m_table = new AttributeVehicle[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void AttributeTableVehicle::freeObjects()
 {
    delete [] m_table;
    m_table         = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *AttributeTableVehicle::getObjectPTR( int index )
 {
    s_ASSERT(index>=0 && index <m_maxObjectQnty,"AttributeTable::getObjectPTR");
    return &(m_table[ index ]);
 }


 /*********************************
  *
  *   Vehicle implementation
  *
  *********************************/

    // IDynamicObject interface

 //============================================================
CFVector3  Vehicle::getPos      ()
{
    return getPosition();
}

 //============================================================
double     Vehicle::getHAngle   ()
{
    const CFMatrix3x4 &m = m_vessel->GetDir();
    CFVector3 dir = -m.Row(2);
    if(  dir.x==0 && dir.z==0  )
         return 0;
    return atan2(dir.z,dir.x);
}

 //============================================================
CFVector3  Vehicle::getUpVector ()
{
    return CFVector3(0,1,0); // FIXME
}

 //============================================================
CFVector3  Vehicle::getCenter   () // Относительно 0 объекта
{
    return CFVector3(0,m_attr->m_centerOffsetY,0);
}

 //============================================================
double     Vehicle::getRadius   () // Относительно центра
{
    return m_attr->m_radius;
}

 //============================================================
double     Vehicle::getRadius0  () // Относительно 0 объекта
{
    return m_attr->m_radius0;
}

 //============================================================
CFVector3  Vehicle::getMoveDir  () // Направление движения
{
    return Normal(m_vessel->Speed());
}

 //============================================================
double     Vehicle::getMoveSpeed() // Скорость
{
    return Abs(m_vessel->Speed());
}

 //============================================================
void       Vehicle::getMatrix   ( CFMatrix3x4 &m )
{
    m.LoadTransposed(m_vessel->GetDir());
    //m.TranslateL( getPosition() );
}

    // IUnit interface
 //============================================================
double Vehicle::getPower() // Сила юнита 0..10
{
	return m_attr->m_power;
}

 //============================================================
int    Vehicle::isFriend( const KR_ObjectID &commanderID )
{
    return !(player().isRenegat(commanderID));
}

KR_ObjectID Vehicle::getCommander()
{
    return KR_ObjectID::NUL(); //FIXME
}

 //============================================================
double Vehicle::getDamage() // Целостность от 0..1
{
    return m_damage;
}

void *Vehicle::queryInterface( int interNum )
 {
    switch( interNum )
    {
    case IUnknownIID:       return (KR_Object*)this;
    case IDynamicObjectIID: return (IDynamicObject*)this;

    case IUnitIID:          if ((!m_dead) && (!m_playedBrief))
				return (IUnit*)this;
			    else
				return NULL;

    case IVehicleIID:       return this;
    case IPlayerIID:        return (IPlayer*)(&m_player);
    }

    return 0;
 }

CFVector3  Vehicle::realPosition()
{
    return CFVector3(1e10,1e10,1e10);
}

void AttributeVehicle::update(double)
{
    if(  m_panelName[0] != 0  )
    {
         m_panel = new CGRPanel(m_panelName);
         m_panel->SetResolution(_gr_nScreenWidth,_gr_nScreenHeight);
    }
    else m_panel = 0;

    m_taxiID           = context->searchObject(m_taxiName);
    if( m_type )
        RTCHECK1(!m_taxiID.isNUL(),"Unknown object '%s'",m_taxiName);

    m_bulletTable     = g_arena.searchSeanceClassTable("Bullet");
    m_bulletAttrIndex = g_arena.getAttributeIndex(
                                       g_arena.searchSeanceClassTable("BulletAttr"),
                                       context->searchObject(m_bulletAttrName)
                                      );

    m_bulletSecAttrIndex = g_arena.getAttributeIndex(
                                       g_arena.searchSeanceClassTable("BulletAttr"),
                                       context->searchObject(m_bulletSecAttrName)
                                      );
}

void AttributeVehicle::removeNotify()
{
   ct_Attribute::removeNotify();
   delete m_panel;
   m_panel = 0;
}

void Vehicle::setBriefingSound(char * name, int cycle, double ts)
{
	if (!lpRSX2Unk)
		return;

	if (name)
	 if (strcmp(name,"same")==0)
	 {
          return;
	 }
	
	if (m_lpCE)
	{
		m_lpCE->Release();
		m_lpCE = 0;
	}

	m_playingBriefingSound = true;

	if (! name)
		return;	
	if (! * name)
		return;
	
		
	KR_ObjectID  	wavID( context->searchObject(name) );
	ct_ClassTableID ctsndID = g_arena.searchSeanceClassTable("SoundObj");
	
	if( wavID.isNUL() )
		return;
	
	KR_Event event;
	
	event.label       = sk_EV_QUERY_MODEL_PTR;
	event.destination = wavID;
	event.source      = getObjectID();
	event.timeStamp   = ts;
	context->sendEventNow( event ); // Возвращает адрес WAV-объекта
	
	WAVObj    *wav;	
	
	event.data.open(EDO_READ)
		.get(&wav,sizeof(void*))
		.close();
	
	HRESULT hr = CoCreateInstance(
		CLSID_RSXCACHEDEMITTER,     // GUID for cachedemitter object
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IRSXCachedEmitter,
		(void ** )&m_lpCE);
	
	if ( FAILED(hr) )
	{
		m_lpCE = 0;
		return;
	}
	
	
	RSXCACHEDEMITTERDESC ceDesc;
	
	ZeroMemory(& ceDesc, sizeof(RSXCACHEDEMITTERDESC));
	ceDesc.cbSize = sizeof(RSXCACHEDEMITTERDESC);
	ceDesc.dwFlags = RSXEMITTERDESC_NOSPATIALIZE | RSXEMITTERDESC_NOATTENUATE | RSXEMITTERDESC_NODOPPLER | RSXEMITTERDESC_NOREVERB | RSXEMITTERDESC_PREPROCESS | RSXEMITTERDESC_INMEMORY;
	
	strcpy(ceDesc.szFilename,wav->m_rsxCE.szFilename);
	
	hr = m_lpCE->Initialize(& ceDesc, lpRSX2Unk);
	if ( FAILED(hr) )
	{
		m_lpCE->Release();
		m_lpCE = 0;
		return;
	}
	
	RSXEMITTERMODEL theModel;
	
	memcpy(&theModel,&wav->m_rsxEModel,sizeof(RSXEMITTERMODEL));
	
	theModel.fIntensity = snd_engineIntensity;
	
	hr = m_lpCE->SetModel(& theModel);
	
	if ( FAILED(hr) )
	{
		m_lpCE->Release();
		m_lpCE = 0;
		return;
	}
	
	m_lpCE->ControlMedia(RSX_PLAY, cycle, 0);	
}


void Vehicle::updateSound(double ts)
{
	
	m_playingBriefingSound = false;
	
	if (!lpRSX2Unk)
		return;
	
	
	if (m_lpCE)
	{
		m_lpCE->Release();
		m_lpCE = 0;
	}

	if (!snd_engine)
		return;

	
	if (! * m_attr->m_soundName)
		return;
	
	KR_ObjectID  	wavID( context->searchObject(m_attr->m_soundName) );
	ct_ClassTableID ctsndID = g_arena.searchSeanceClassTable("SoundObj");
	
	if( wavID.isNUL() )
		return;
	
	KR_Event event;
	
	event.label       = sk_EV_QUERY_MODEL_PTR;
	event.destination = wavID;
	event.source      = getObjectID();
	event.timeStamp   = ts;
	context->sendEventNow( event ); // Возвращает адрес WAV-объекта
	
	WAVObj    *wav;	
	
	event.data.open(EDO_READ)
		.get(&wav,sizeof(void*))
		.close();
	
	HRESULT hr = CoCreateInstance(
		CLSID_RSXCACHEDEMITTER,     // GUID for cachedemitter object
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IRSXCachedEmitter,
		(void ** )&m_lpCE);
	
	if ( FAILED(hr) )
	{
		m_lpCE = 0;
		return;
	}
	
	
	RSXCACHEDEMITTERDESC ceDesc;
	
	ZeroMemory(& ceDesc, sizeof(RSXCACHEDEMITTERDESC));
	ceDesc.cbSize = sizeof(RSXCACHEDEMITTERDESC);
	ceDesc.dwFlags = RSXEMITTERDESC_NOSPATIALIZE | RSXEMITTERDESC_NOATTENUATE | RSXEMITTERDESC_NODOPPLER | RSXEMITTERDESC_NOREVERB | RSXEMITTERDESC_PREPROCESS | RSXEMITTERDESC_INMEMORY;
	
	strcpy(ceDesc.szFilename,wav->m_rsxCE.szFilename);
	
	hr = m_lpCE->Initialize(& ceDesc, lpRSX2Unk);
	if ( FAILED(hr) )
	{
		m_lpCE->Release();
		m_lpCE = 0;
		return;
	}
	
	RSXEMITTERMODEL theModel;
	
	memcpy(&theModel,&wav->m_rsxEModel,sizeof(RSXEMITTERMODEL));
	
	theModel.fIntensity = snd_engineIntensity;
	
	hr = m_lpCE->SetModel(& theModel);
	
	if ( FAILED(hr) )
	{
		m_lpCE->Release();
		m_lpCE = 0;
		return;
	}
	
	m_lpCE->ControlMedia(RSX_PLAY, 0, 0);
	m_currentPitch = m_attr->m_soundMaxPitch;
		
}


double Vehicle::s_curTime = 0;

bool Vehicle::MasterBumpCallBack( ct_Subject  * master, SBumpDef &def )
{
	Vehicle	*veh = (Vehicle*)master;

    //SBumpDef &def = veh->m_vessel->GetBumpDef();
    def.fMass = 1;
    double oRadius;
    CFVector3 oPos, oSpeed;
    KR_ObjectID oID;
    checkDynamicCollision(
			def, // мы
                     veh->getObjectID(),  // кого игнорировать
                     oID,     // объект, о который стукнемся
                     oPos,    // позиция объекта
                     oRadius,
                     oSpeed,
                     s_curTime );

     veh->carrierOnCollizion(oID);
//	nBump = 0;
    /*if(nBump) {
        def.fTime = clzTime;
        def.vel1 = -def.vel;
        def.nBumpFlags = BF_BUMPDYNAMIC;
    }*/

    return def.nBumpFlags != 0;
}


void Vehicle::setAttr(KR_Event &event)
{
   
   event.data.open(EDO_READ)
               .getObjectID(m_vehicleAttrID )
             .close();

   setVehicleAttr();
}


void Vehicle::setVehicleAttr()
{
   ct_Attribute *attr =__attrVehicleTable.searchAttribute(m_vehicleAttrID);
   if( attr==NULL )
        echo( "Vehicle::receiveEvent: Unknown attribute %s",
              context->searchObject(m_vehicleAttrID));
   else 
   {
        m_attr = (AttributeVehicle*)attr;
        m_panel = m_attr->m_panel;

        if(  strcmp(m_attr->m_dynamic,"Dragon")==0  )
        {
             g_emv.Attach(&g_emvAttrDragon);
             m_vessel = &g_emv;
        }
        else
        if(  strcmp(m_attr->m_dynamic,"Emveshka")==0  )
        {
             g_emv.Attach(&g_emvAttr0);
             m_vessel = &g_emv;
        }
        else
        if(  strcmp(m_attr->m_dynamic,"TankGenn0")==0  )
        {
             g_walk.Attach(&g_walkAttr0);
             m_vessel = &g_walk;
        }
        else
        if(  strcmp(m_attr->m_dynamic,"Dead")==0  )
        {
             g_walk.Attach(&g_dead);
             m_vessel = &g_walk;
        }
        else
        if(  strcmp(m_attr->m_dynamic,"TankGenn1")==0  )
        {
             g_tank.Attach(&g_tankAttr1);
             m_vessel = &g_tank;
        }
        else
        if(  strcmp(m_attr->m_dynamic,"TankGenn2")==0  )
        {
             g_tank.Attach(&g_tankAttr2);
             m_vessel = &g_tank;
        }
        else
        if(  strcmp(m_attr->m_dynamic,"TankGenn3")==0  )
        {
             g_tank.Attach(&g_tankAttr3);
             m_vessel = &g_tank;
        }
        else
        {
             s_ASSERTNQ1("Vehicle::Unknown dynamic %s",m_attr->m_dynamic);
             m_vessel = 0;//&g_vesselEmveshka;
        }
        m_vessel->Restart();
        m_vessel->SetMaster( this, MasterBumpCallBack );

        // engine start sound is available
        updateSound(Session::m_moment);        

        static double bounds[6] = 
        {
            0,5120, 0, 245*1.25, -5120, 0
        };
        m_vessel->SetBounds(bounds);
   }
}

extern SGRViewport *ZAV_Viewport();

void Vehicle::openPanel(double ts)
{
    if(  m_panel != 0  )
    {
         SGRViewport *vp = m_panel->Open();
         if(  vp!=0  )
              GRSetViewport( vp );
         else
         {
              GRSetViewport( ZAV_Viewport() );
         }
    }
    else GRSetViewport( ZAV_Viewport() );
	KR_Event event;

	event.source	  = getObjectID();
	event.destination = g_hardware.getObjectID();
	event.label		  = CTRL_SUBSCRIBE;
	event.timeStamp	  = ts;
	event.data.open(EDO_WRITE)
				.putObjectID(getObjectID())
	            .putInt(NORMAL)
			  .close();
	getContext()->sendEventNow(event);
}


void Vehicle::drawPanel()
{

    if(  m_panel != 0  )
         m_panel->Draw();
}

void Vehicle::closePanel(double ts)
{
    if(  m_panel != 0  )
         m_panel->Close();
	KR_Event event;

    event.timeStamp   = ts;
	event.source	  = getObjectID();
	event.destination = g_hardware.getObjectID();
	event.label		  = CTRL_UNSUBSCRIBE;
	event.data.open(EDO_WRITE)
				.putObjectID(getObjectID())
			  .close();
	getContext()->sendEventNow(event);
}

void Vehicle::onChangeVehicle(double ts)
{

	if (m_dead)
		return;
	
	if (m_isTakingTaxiNow)
	{
		m_isTakingTaxiNow = 0;                                                  
		return;
    }
	
    switch( m_attr->m_type )
    {
    case 0: /*  Default */ 
		{
			ct_SubjectFindData fd;
			CFVector3 p(getPosition());
			CFVector3 taxiPos;
			
			g_arena.findFirstSubject(fd,p.x-40,p.z-40,p.x+40,p.z+40);
			double      minDist = 1e10,dist;
			
			KR_ObjectID nearest(KR_ObjectID::NUL());
			
			for( int i = 0; i < fd.getCount(); ++i )
			{
				ITaxi *ti = (ITaxi*)(context->queryInterface( fd[i], ITaxiIID ));
				if(  ti != 0  )
				{
					dist = Abs( getPosition() - ti->taxiPos() );
					if(  dist < 20  &&  dist < minDist )
					{
						minDist   = dist;
						nearest = fd[i];
						taxiPos   = ti->taxiPos();
					}
				}
			}
			
			/*
			* Такси поймали, Теперь нужно сесть
			*/
			if(  !nearest.isNUL()  )
			{
				
				m_vessel->Stop();
				
				ITaxi *ti = (ITaxi*)(context->queryInterface( nearest, ITaxiIID ));
				CFVector3 vect = ti->taxiPos() - g_vehicle->Pos();
				m_currentTaxiOurPos = - g_vehicle->Pos();
				double len = Abs(vect) - 1.0;
				
				if (len < 0)
				{
					KR_Event event;
					event.label       = EV_VEHICLE_SETTAXI;
					event.destination = getObjectID();
					event.source      = getObjectID();
					event.timeStamp   = ts;
					event.data.open(EDO_WRITE)
						.putObjectID(nearest)
						.close();			
					issueEvent(event);
					
					return;
				}
				
				
				double dt  = len / m_attr->m_taxiMoveSpeed;
				
				CFMatrix3x4	tdir = g_vehicle->GetDir();					
				
				CFVector3 dv = - tdir.Row(2);
				
				double angle1 = atan2(dv.x, dv.z);
				
				m_currentTaxiPos = ti->taxiPos();
				
				double angle2 = atan2(vect.x, vect.z);				
				
				m_takingTaxiFinalAngle   = angle1 - angle2;
				m_takingTaxiCurrentAngle = 0.0;
				
				m_isTakingTaxiNow = 1;
				m_lastEventTime   = Session::m_moment;
				m_taxiRotateSpeed = m_attr->m_taxiRotateSpeed;
				
				m_spX =   m_attr->m_taxiMoveSpeed * sin (angle2);
				m_spZ =   m_attr->m_taxiMoveSpeed * cos (angle2);
				m_spY =   - vect.y / dt; 
				
				
				KR_Event event;
				event.label       = EV_VEHICLE_SETTAXI;
				event.destination = getObjectID();
				event.source      = getObjectID();
				event.timeStamp   = ts + dt;
				event.data.open(EDO_WRITE)
					.putObjectID(nearest)
					.close();			
				issueEvent(event);
			}
		}
		break;
		
    case 1: 
		{
			
			double dropTime;
			KR_ObjectID oID;				


			if(!checkCollision( 
				getPosition(),			  // начало движения
				CFVector3(0,-9.8,0),		  // напрвление со скоростью
				1,                        //g_walkAttr0.fRadius,      // радиус
				5000,                     // время для проверки
				g_vehicle->getObjectID(), // кого игнорировать
				dropTime,                 // время, через которое стукнемся
				oID))
			{
				ASSERT(0);
			}
		
			BOOL MakeOrphan = ! oID.isNUL();

			MakeOrphan |= (dropTime > 1);

			LeaveVehicle(ts, MakeOrphan );
		}       
		break;
		
    }
}


void Vehicle::LeaveVehicle(double ts, BOOL makeOrphan)
{
	
	if (m_dead)
		return;
	
	if(  m_attr->m_outFlicName[0]!= 0  )
	{
		bool oldStat = m_playedBrief;
		m_playedBrief = true;
		g_briefing.PlayBriefing(m_attr->m_outFlicName);
		m_playedBrief = oldStat;
	}
	
	KR_Event event;
	CFMatrix3x4    tdir = g_vehicle->GetDir();     
	CFVector3      toDir(m_attr->m_outOfsX,0,m_attr->m_outOfsZ),				
	
        from = getPosition()+CFVector3(0,4,0);
	
	KR_ObjectID oID;	
	
	double clzTime;
	
	if(!checkCollision( 
		from,     // начало движения
		toDir,    // напрвление со скоростью
		1,                        //g_walkAttr0.fRadius,      // радиус
		1,                        // время для проверки
		g_vehicle->getObjectID(),            // кого игнорировать
		clzTime,                  // время, через которое стукнемся
		oID))                     // объект, о который стукнемся
		clzTime = 1;
	
	
	
	BOOL mustDie = (m_attr->m_type == 0);

	
	if (!mustDie)
	{
		
		if (makeOrphan)	// создаем сироту
		{
			event.destination = g_arena.newObject("Orphan","Orphan.Object");	
			if(  event.destination.isNUL()  )
				return;
		}
		else	// Создаем такси, для того, чтобы его бросить
		{								
			event.destination = g_arena.newObject("Taxi","Taxi.Object");
			if(  event.destination.isNUL()  )
				return;
		}
		
		event.label       = EV_VEHICLE_DROP_TAXI;
		
		event.source      = getObjectID();
		event.timeStamp   = ts;
		event.data.open(EDO_WRITE)
			.putObjectID(m_attr->m_taxiID)
			.putDouble(getPosition().x)
			.putDouble(getPosition().y)
			.putDouble(getPosition().z)
			.putDouble(m_damage)
                        .putInt(m_secBulletCnt)
			.close();
		context->sendEventNow( event );
		SetPos( from+toDir*clzTime );
	}
		else	// труп
	{
		

		
		m_dead  = true;
		m_isTakingTaxiNow = 1;
		m_currentTaxiOurPos = - g_vehicle->Pos();
		
		m_lastEventTime   = Session::m_moment;
                g_GameConsole.PrintUrgent("You're dead, loser!", 40, GameConsole::CENTER);

		// труп

		AttributeTaxi * taxiAttr = (AttributeTaxi *) __attrTaxiTable.searchAttribute(m_attr->m_taxiID);

		if (taxiAttr)
		 createCorpse(	g_vehicle->Pos(),
						ts,
						getObjectID(),
						taxiAttr->m_cacheCorpseTable, 
						taxiAttr->m_cacheCorpseAttr);

		SetPos( CFVector3(0,0,0));	// чтобы не стреляли по дохлякам
		m_currentTaxiOurPos.y -= 1.5;		// Начинаем взлетать вверх с 1.5 метров
	}
	
       /*
	* Садимся на умолчальную
	*/
	
	
	
	event.label       = KR_SET_ATTR;
	event.destination = getObjectID();
	event.source      = getObjectID();
	event.timeStamp   = ts;

	event.data.open(EDO_WRITE)
		.putObjectID(m_vehicleAttrDefaultID)
		.close();

		
	closePanel(ts);
	setAttr( event );
	openPanel(ts);
	
    SetDir(tdir);		
}

void  Vehicle::onSetTaxi( KR_Event & event)
{
	KR_ObjectID nearest;
	
	event.data.open(EDO_READ)
		.getObjectID(nearest)
	.close();
	

	ITaxi *ti = (ITaxi*)(context->queryInterface( nearest, ITaxiIID ));
	IDynamicObject *ido = (IDynamicObject*)(context->queryInterface( nearest, IDynamicObjectIID ));
	IUnit *unit = (IUnit *)(context->queryInterface( nearest, IUnitIID ));

	ASSERT(ti);
	ASSERT(ido);
	ASSERT(unit);

        m_secBulletCnt = ti->taxiGetBulletCnt( );
	m_damage = unit->getDamage();
	
	event.label       = KR_SET_ATTR;
	event.destination = getObjectID();
	event.source      = getObjectID();
	event.data.open(EDO_WRITE)
		.putObjectID(ti->getAttributeForVehicle())
		.close();
	closePanel(event.timeStamp);
	setAttr( event );
	openPanel(event.timeStamp);
	
	CFMatrix3x4 m;
	m.LoadTransposed(ido->GetDir());
	SetDir(m);
	
	//setPosition( ti->taxiPos()+CFVector3(0,m_attr->m_bornY,0) );
	SetPos( ti->taxiPos()+CFVector3(0,m_attr->m_bornY,0) );
	
	context->removeObject( nearest );

	m_isTakingTaxiNow = 0;
}

static int Shoot(
                   const CFVector3    &position,
                   const CFVector3    &dir,
                   ct_ClassTableID     bulletTable,
                   int                 attrIndex,
                   KR_ObjectID         fromID,
                   SimulationContext  *context,
                   double              timeStamp
                 )
{
    KR_Event event;

    event.label       = b_EV_START;
    event.source      = fromID;
    event.destination = g_arena.newObject( bulletTable , "B" );

    if(  event.destination.isNUL() )
         return 0;

    event.data.open(EDO_WRITE)
                        .descend( VECTOR3D_F, 0 )
                          .putDouble(position.x)
                          .putDouble(position.y)
                          .putDouble(position.z)
                        .ascend()
                        .descend( VECTOR3D_F, 0 )
                          .putDouble(dir.x)
                          .putDouble(dir.y)
                          .putDouble(dir.z)
                        .ascend()
                        .putInt(attrIndex)
                        .putObjectID(fromID)
                   .close();
    event.timeStamp = timeStamp;
    context->sendEventNow( event );
    return 1;
}

void  Vehicle::onFireSec( double timeStamp)
{
    if(  m_attr->m_type==0  ) return;

    IDynamicObject *dobj=this;

    CFMatrix3x4 m;
    dobj->getMatrix(m);
//----------------------------
 
    if( Shoot(
                   getPosition()+m*CFVector3(0,-0.7,0),//+m*m_attr->m_cannonOffset,
                   -m.Column(2),
                   m_attr->m_bulletTable,
                   m_attr->m_bulletSecAttrIndex,
                   getObjectID(),
                   context,
                   timeStamp
                 )
      )
     {
         if(  m_secBulletCnt>0  )
              m_secBulletCnt--;

         /*if (m_lpCannonCE)
             m_lpCannonCE->ControlMedia(RSX_PLAY, 1, 0);*/
     }
}

void  Vehicle::onFire( double timeStamp )
{
    if(  m_attr->m_type==0  ) return;

    IDynamicObject *dobj=this;

    CFMatrix3x4 m;
    dobj->getMatrix(m);
//----------------------------
 
    if( Shoot(
                   getPosition()+m*CFVector3(0,-0.7,0),//+m*m_attr->m_cannonOffset,
                   -m.Column(2),
                   m_attr->m_bulletTable,
                   m_attr->m_bulletAttrIndex,
                   getObjectID(),
                   context,
                   timeStamp
                 )
      )
    {
         /*if (m_lpCannonCE)
             m_lpCannonCE->ControlMedia(RSX_PLAY, 1, 0);*/
    }
}

double Vehicle::desireShoot()
{
    return 1.0;
}

void  Vehicle::Restart()
{ 
  m_vessel->Restart(); 
  updateSound(Session::m_moment); 
}

extern int g_godMode;

void   Vehicle::setDamage( double d, const CFVector3 &/*pos*/, double ts, KR_ObjectID )
{
   if (g_godMode || m_dead)
	return;

   if(  ts-m_lastLeaveTime < g_levelAttr.m_666Time  )
        return;


   m_damage -= d;
   echo( "damage=%lg", m_damage);
   if(  m_damage <= 0  )
   {
        m_lastLeaveTime = ts;
        LeaveVehicle(ts, TRUE);	// always make the orphan
        m_lastLeaveTime = ts;
   }

}

void Vehicle::setCommander(KR_ObjectID)
{
   echo("Vehicle::setCommander don't support");
}

void Vehicle::carrierLoadMatrix  (CFMatrix3x4 &m)
{
    getMatrix( m );
    m.TranslateR(CFVector3(0,10,0));
}



bool	Vehicle::dump(PIN_SaveFile & sf)
{
		if (!ct_Subject::dump(sf) ||
			!ICarrier::dump(sf)   ||
			!sf.WriteData( (char *) & m_vehicleAttrID, sizeof(VehicleData)  ) ||
			!m_player.dump(sf))
			return false;

		void const * zavShit = m_vessel->SaveGame();
		DWORD size = *(DWORD *) zavShit;

		if (	!sf.WriteData( (char *) zavShit, size))	
			return false;

		return true;
}

bool	Vehicle::load(PIN_SaveFile & sf)
{
		echo ("Vehicle - load");

		if (!ct_Subject::load(sf) ||
			!ICarrier::load(sf)   ||
			!sf.GetData( (char *) & m_vehicleAttrID, sizeof(VehicleData)  ) ||
			!m_player.load(sf))
			return false;

		m_zavSavePoint = (DWORD *) sf.GetCurrentData() + 1;
		
		sf.Shift( (*(DWORD *) m_zavSavePoint) + sizeof(int));

		return true;
}


void Vehicle::loadNotify()
{
	ct_Subject::loadNotify();
	ICarrier::loadNotify();
	m_player.loadNotify();
	
	m_attr   = 0;
    m_vessel = 0;
        
    RTCHECK(!m_vehicleAttrDefaultID.isNUL(),"Unknown object 'Vehicle.Attr.default'");
	RTCHECK(!m_vehicleAttrDeadID.isNUL(),"Unknown object 'Vehicle.Attr.dead'");

	InitSound();
	setVehicleAttr();
        m_vessel->LoadGame(m_zavSavePoint);
}


bool	Vehicle::LoadStaticData(PIN_SaveFile & sf)
{
	if (!sf.GetData( (char *) & m_dead, sizeof (bool)) ||
		!sf.GetData( (char *) & s_curTime,sizeof(double)) ||
		!sf.GetData( (char *) & m_isTakingTaxiNow, sizeof(int)) ||
		!sf.GetData( (char *) & m_takingTaxiCurrentAngle, sizeof(double)) ||
		!sf.GetData( (char *) & m_takingTaxiFinalAngle, sizeof(double)) ||
		!sf.GetData( (char *) & m_lastEventTime, sizeof(double)) ||
		!sf.GetData( (char *) & m_taxiRotateSpeed, sizeof(double)) ||
		!sf.GetData( (char *) & m_spX, sizeof(double)) ||
		!sf.GetData( (char *) & m_spZ, sizeof(double)) ||
		!sf.GetData( (char *) & m_currentTaxiPos, sizeof(CFVector3)) ||
		!sf.GetData( (char *) & m_currentTaxiOurPos, sizeof(CFVector3)))
		return false;
	return true;
}

bool	Vehicle::SaveStaticData(PIN_SaveFile & sf)
{
	if (!sf.WriteData( (char *) & m_dead, sizeof (bool)) ||
		!sf.WriteData( (char *) & s_curTime,sizeof(double)) ||
		!sf.WriteData( (char *) & m_isTakingTaxiNow, sizeof(int)) ||
		!sf.WriteData( (char *) & m_takingTaxiCurrentAngle, sizeof(double)) ||
		!sf.WriteData( (char *) & m_takingTaxiFinalAngle, sizeof(double)) ||
		!sf.WriteData( (char *) & m_lastEventTime, sizeof(double)) ||
		!sf.WriteData( (char *) & m_taxiRotateSpeed, sizeof(double)) ||
		!sf.WriteData( (char *) & m_spX, sizeof(double)) ||
		!sf.WriteData( (char *) & m_spZ, sizeof(double)) ||
		!sf.WriteData( (char *) & m_currentTaxiPos, sizeof(CFVector3)) ||
		!sf.WriteData( (char *) & m_currentTaxiOurPos, sizeof(CFVector3)))
		return false;
	return true;
}


void Vehicle::InitSound()
{
	// Direct Listener Initialization
	
	if (!lpRSX2)
		return;        // RSX is not initialized
	
	
	HRESULT hr = CoCreateInstance(
		CLSID_RSXDIRECTLISTENER,                // GUID for direct listener object
		NULL,                                                   // the rest of the params are the same
		CLSCTX_INPROC_SERVER,           // as before
		IID_IRSXDirectListener,
		(void ** ) &m_lpDL);
	
	
	if ( FAILED(hr) || !m_lpDL)
	{
		m_lpDL = NULL;
		return;
	}                       
	
	RSXDIRECTLISTENERDESC rsxDL;            // listener description
	ZeroMemory(&rsxDL, sizeof(RSXDIRECTLISTENERDESC));
	
	// fill up the directlistener settings for the initialize call
	rsxDL.cbSize = sizeof(RSXDIRECTLISTENERDESC);
	if (snd_useDS)
		rsxDL.hMainWnd = _gr_hWnd;
	else
		rsxDL.hMainWnd = 0;
	
	
	rsxDL.dwUser = 0;
	rsxDL.lpwf = NULL;
	
	hr = m_lpDL->Initialize(&rsxDL, lpRSX2Unk);
	
	if ( FAILED(hr))
	{
		m_lpDL->Release();
		m_lpDL = NULL;
		return;
	}
	
}



/* End of file C:\NW\ARENA\OBASE\Vehicle\Vehicle.cpp */
