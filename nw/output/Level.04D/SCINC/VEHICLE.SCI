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

func void CreateVehicleAttr_otank( int ctID )
var int oID, cachePos;
{
   New(ctID,"Vehicle.Attr.otank",oID,cachePos);
   SetAttribute_i( oID, cachePos, "m_type", 1 );
   SetAttribute_s( oID, cachePos, "m_panelName", "Panels\otank.pnl" );
   SetAttribute_s( oID, cachePos, "m_dynamic", "TankGenn1" );
   SetAttribute_s( oID, cachePos, "m_taxiName", "Taxi.Attr.cln_f05" );
   SetAttribute_s( oID, cachePos, "m_soundName", "wav.Tank2" );	
   SetAttribute_s( oID, cachePos, "m_bulletAttrName", "Bullet.Led.Prim" );
   SetAttribute_s( oID, cachePos, "m_bulletSecAttrName", "Bullet.Sec" );
   SetAttribute_f( oID, cachePos, "m_power", 20 );
   SetAttribute_f( oID, cachePos, "m_soundMinPitch",0.3);
   SetAttribute_f( oID, cachePos, "m_soundMaxPitch",1.4);

}

func void CreateVehicleAttr_f03( int ctID )
var int oID, cachePos;
{
   New(ctID,"Vehicle.Attr.f03",oID,cachePos);
   SetAttribute_i( oID, cachePos, "m_type", 1 );
   SetAttribute_s( oID, cachePos, "m_panelName", "Panels\steam.pnl" );
   SetAttribute_s( oID, cachePos, "m_dynamic",   "TankGenn2" );
   SetAttribute_s( oID, cachePos, "m_taxiName",  "Taxi.Attr.cln_f03" );
   SetAttribute_s( oID, cachePos, "m_soundName", "wav.Steam2" );	
   SetAttribute_s( oID, cachePos, "m_bulletAttrName", "Bullet.Led.Prim" );
   SetAttribute_s( oID, cachePos, "m_bulletSecAttrName", "Bullet.Sec" );
   SetAttribute_f( oID, cachePos, "m_power", 20 );
   SetAttribute_f( oID, cachePos, "m_soundMinPitch",0.3);
   SetAttribute_f( oID, cachePos, "m_soundMaxPitch",1.4);

   SetAttribute_s( oID, cachePos, "m_outFlicName", "brief/door.txt" );
}

func void CreateVehicleAttr_f08( int ctID )
var int oID, cachePos;
{
   New(ctID,"Vehicle.Attr.f08",oID,cachePos);
   SetAttribute_i( oID, cachePos, "m_type", 1 );
   //SetAttribute_s( oID, cachePos, "m_panelName", "Panels\civil1.pnl" );
   SetAttribute_s( oID, cachePos, "m_panelName", "" );
   SetAttribute_s( oID, cachePos, "m_dynamic", "TankGenn2" );
   SetAttribute_s( oID, cachePos, "m_taxiName", "Taxi.Attr.cln_f08" );
   SetAttribute_s( oID, cachePos, "m_soundName", "wav.Steam2" );	
   SetAttribute_s( oID, cachePos, "m_bulletAttrName", "Bullet.Led.Prim" );
   SetAttribute_s( oID, cachePos, "m_bulletSecAttrName", "Bullet.Sec" );
   SetAttribute_f( oID, cachePos, "m_power", 20 );
   SetAttribute_f( oID, cachePos, "m_soundMinPitch",0.3);
   SetAttribute_f( oID, cachePos, "m_soundMaxPitch",1.4);
}

func void CreateVehicleAttr_helicop( int ctID )
var int oID, cachePos;
{
   New(ctID,"Vehicle.Attr.helicop",oID,cachePos);
   SetAttribute_i( oID, cachePos, "m_type", 1 );
   SetAttribute_s( oID, cachePos, "m_panelName", "Panels\helicop.pnl" );
   SetAttribute_s( oID, cachePos, "m_dynamic", "Emveshka" );
   SetAttribute_s( oID, cachePos, "m_taxiName", "Taxi.Attr.cln_f04" );
   SetAttribute_s( oID, cachePos, "m_soundName", "wav.Helicopt" );	
   SetAttribute_s( oID, cachePos, "m_bulletAttrName", "Bullet.Led.Prim" );
   SetAttribute_s( oID, cachePos, "m_bulletSecAttrName", "Bullet.Sec" );
   SetAttribute_f( oID, cachePos, "m_power", 20 );
   SetAttribute_f( oID, cachePos, "m_soundMinPitch",0.3);
   SetAttribute_f( oID, cachePos, "m_soundMaxPitch",2.2);
}

func void CreateVehicleAttr_f00( int ctID )
var int oID, cachePos;
{
   New(ctID,"Vehicle.Attr.f00",oID,cachePos);
   SetAttribute_i( oID, cachePos, "m_type", 1 );
   SetAttribute_s( oID, cachePos, "m_panelName", "Panels\wing.pnl" );
   SetAttribute_s( oID, cachePos, "m_taxiName", "Taxi.Attr.cln_f00" );
   SetAttribute_s( oID, cachePos, "m_soundName", "wav.Engine2" );	
   SetAttribute_s( oID, cachePos, "m_bulletAttrName", "Bullet.Led.Prim" );
   SetAttribute_s( oID, cachePos, "m_bulletSecAttrName", "Bullet.Sec" );
   SetAttribute_f( oID, cachePos, "m_power", 20 );
   SetAttribute_f( oID, cachePos, "m_soundMinPitch",0.3);
   SetAttribute_f( oID, cachePos, "m_soundMaxPitch",1.4);

}
func void CreateVehicleAttr_f01( int ctID )
var int oID, cachePos;
{
   New(ctID,"Vehicle.Attr.f01",oID,cachePos);
   SetAttribute_i( oID, cachePos, "m_type", 1 );
   SetAttribute_s( oID, cachePos, "m_panelName", "Panels\plane.pnl" );
   SetAttribute_s( oID, cachePos, "m_taxiName", "Taxi.Attr.cln_f01" );
   SetAttribute_s( oID, cachePos, "m_soundName", "wav.Engine1" );	
   SetAttribute_s( oID, cachePos, "m_bulletAttrName", "Bullet.Led.Prim" );
   SetAttribute_s( oID, cachePos, "m_bulletSecAttrName", "Bullet.Sec" );
   SetAttribute_f( oID, cachePos, "m_power", 20 );
   SetAttribute_f( oID, cachePos, "m_soundMinPitch",0.3);
   SetAttribute_f( oID, cachePos, "m_soundMaxPitch",1.4);

}


func void main_CreateVehicleAttr()
 var int ctID;
{
   ctID := s_AddClassTable("VehicleAttr",8);
   CreateVehicleAttr_default( ctID );
   CreateVehicleAttr_otank  ( ctID );
   CreateVehicleAttr_f03    ( ctID );
   CreateVehicleAttr_f08    ( ctID );
   CreateVehicleAttr_helicop( ctID );
   CreateVehicleAttr_f00    ( ctID );
   CreateVehicleAttr_f01    ( ctID );
   CreateVehicleAttr_dead   ( ctID );
}

func void main_CreateVehicle()
 var int ctID, oID, cachePos;
{
   ctID := s_AddClassTable( "Vehicle", 5 );

   s_New( ctID, "Vehicle.Default", oID, cachePos );
//   ChangeObjectAttrN( "Vehicle.Attr.default", "Vehicle.Default" );
   ChangeObjectAttrN( "Vehicle.Attr.helicop", "Vehicle.Default" );
}

