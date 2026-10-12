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

func void CreateVehicleAttr_Akula( int ctID )
var int oID, cachePos;
{            
   New(ctID,"Vehicle.Attr.Akula",oID,cachePos);
   SetAttribute_i( oID, cachePos, "m_type", 1 );
   SetAttribute_s( oID, cachePos, "m_panelName", "" );
   SetAttribute_s( oID, cachePos, "m_dynamic", "Emveshka" );
   SetAttribute_s( oID, cachePos, "m_taxiName", "Taxi.Attr.Akula" );
   SetAttribute_s( oID, cachePos, "m_soundName", "wav.Ambient" );
   SetAttribute_s( oID, cachePos, "m_bulletAttrName", "Bullet.Led.Prim" );
   SetAttribute_s( oID, cachePos, "m_bulletSecAttrName", "Bullet.Sec" );
   SetAttribute_f( oID, cachePos, "m_power", 10 );
   SetAttribute_f( oID, cachePos, "m_soundMaxPitch",1);
   SetAttribute_f( oID, cachePos, "m_soundMinPitch",1);
}

func void main_CreateVehicleAttr()
 var int ctID;
{
   ctID := s_AddClassTable("VehicleAttr",8);
   CreateVehicleAttr_default( ctID );
   CreateVehicleAttr_dead   ( ctID );
   CreateVehicleAttr_Akula   ( ctID );
}

func void main_CreateVehicle()
 var int ctID, oID, cachePos;
{
   ctID := s_AddClassTable( "Vehicle", 5 );

   s_New( ctID, "Vehicle.Default", oID, cachePos );
//   ChangeObjectAttrN( "Vehicle.Attr.default", "Vehicle.Default" );
   ChangeObjectAttrN( "Vehicle.Attr.Akula", "Vehicle.Default" );
}

