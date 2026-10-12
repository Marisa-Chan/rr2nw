/**************************************************************************
 *
 *                    Vehicle
 *
 **************************************************************************/

func void CreateVehicleAttr_default( int ctID )
var int oID, cachePos;
{
   New(ctID,"Vehicle.Attr.default",oID,cachePos);
   SetAttribute_i( oID, cachePos, "m_type", 0 );
   SetAttribute_s( oID, cachePos, "m_panelName", "" );
   SetAttribute_s( oID, cachePos, "m_dynamic", "TankGenn0" );
   SetAttribute_s( oID, cachePos, "m_taxiName", "Taxi.Attr.CorpseFinal" );
   SetAttribute_s( oID, cachePos, "m_soundName", "wav.Ambient" );
   SetAttribute_s( oID, cachePos, "m_bulletAttrName", "Bullet.Led.Prim" );
   SetAttribute_s( oID, cachePos, "m_bulletSecAttrName", "Bullet.Sec" );
   SetAttribute_f( oID, cachePos, "m_power", 10 );
   SetAttribute_f( oID, cachePos, "m_soundMaxPitch",1);
   SetAttribute_f( oID, cachePos, "m_soundMinPitch",1);
}


func void CreateVehicleAttr_dead( int ctID )
var int oID, cachePos;
{
   New(ctID,"Vehicle.Attr.dead",oID,cachePos);
   SetAttribute_i( oID, cachePos, "m_type", 0 );
   SetAttribute_s( oID, cachePos, "m_panelName", "" );
   SetAttribute_s( oID, cachePos, "m_dynamic", "Dead" );
   SetAttribute_s( oID, cachePos, "m_taxiName", "" );
   SetAttribute_s( oID, cachePos, "m_soundName", "wav.Ambient" );
   SetAttribute_s( oID, cachePos, "m_bulletAttrName", "" );
   SetAttribute_s( oID, cachePos, "m_bulletSecAttrName", "" );
   SetAttribute_f( oID, cachePos, "m_power", 0 );
   SetAttribute_f( oID, cachePos, "m_soundMaxPitch",1);
   SetAttribute_f( oID, cachePos, "m_soundMinPitch",1);
}

func void CreateVehicleAttr_vessel( int ctID )
var int oID, cachePos;
{
   New(ctID,"Vehicle.Attr.vessel",oID,cachePos);
   SetAttribute_i( oID, cachePos, "m_type", 1 );
   SetAttribute_s( oID, cachePos, "m_panelName", "" );
   SetAttribute_s( oID, cachePos, "m_dynamic", "Emveshka" );
//   SetAttribute_s( oID, cachePos, "m_taxiName", "Taxi.Attr.cln_f04" );
   SetAttribute_s( oID, cachePos, "m_taxiName", "Taxi.Attr.CorpseFinal" );
//   SetAttribute_s( oID, cachePos, "m_soundName", "wav.Helicopt" );	
   SetAttribute_s( oID, cachePos, "m_bulletAttrName", "Bullet.Led.Prim" );
   SetAttribute_s( oID, cachePos, "m_bulletSecAttrName", "Bullet.Sec" );
   SetAttribute_f( oID, cachePos, "m_power", 20 );
   SetAttribute_f( oID, cachePos, "m_soundMinPitch",0.3);
   SetAttribute_f( oID, cachePos, "m_soundMaxPitch",2.2);
}

func void main_CreateVehicleAttr()
 var int ctID;
{
   ctID := s_AddClassTable("VehicleAttr",3);
   CreateVehicleAttr_default( ctID );
   CreateVehicleAttr_vessel ( ctID );
   CreateVehicleAttr_dead   ( ctID );
}

func void main_CreateVehicle()
 var int ctID, oID, cachePos;
{
   ctID := s_AddClassTable( "Vehicle", 5 );

   s_New( ctID, "Vehicle.Default", oID, cachePos );
   ChangeObjectAttrN( "Vehicle.Attr.vessel", "Vehicle.Default" );
}

