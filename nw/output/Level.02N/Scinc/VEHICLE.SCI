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

func void CreateVehicleAttr_anl_00(int ctID)
var int oID, cachePos;
{New(ctID,"Vehicle.Attr.anl_00",oID,cachePos);
 SetAttribute_i(oID,cachePos,"m_type",1);
 //SetAttribute_s(oID,cachePos,"m_panelName","");
 SetAttribute_s( oID, cachePos, "m_panelName", "Panels\wood.pnl" );
 SetAttribute_s(oID,cachePos,"m_dynamic","TankGenn2");
 SetAttribute_s(oID,cachePos,"m_taxiName","Taxi.Attr.anl_00");
 SetAttribute_s(oID,cachePos,"m_soundName","wav.ASteps");	
 SetAttribute_s(oID,cachePos,"m_bulletAttrName","Bullet.Led.Prim");
 SetAttribute_s(oID,cachePos,"m_bulletSecAttrName","Bullet.Sec");
 SetAttribute_f(oID,cachePos,"m_power",20);
 SetAttribute_f(oID,cachePos,"m_soundMinPitch",0.2);
 SetAttribute_f(oID,cachePos,"m_soundMaxPitch",2.0);
}

func void CreateVehicleAttr_anl_01(int ctID)
var int oID, cachePos;
{New(ctID,"Vehicle.Attr.anl_01",oID,cachePos);
 SetAttribute_i(oID,cachePos,"m_type",1);
 //SetAttribute_s(oID,cachePos,"m_panelName","");
 SetAttribute_s( oID, cachePos, "m_panelName", "Panels\wood.pnl" );
 SetAttribute_s(oID,cachePos,"m_dynamic","TankGenn1");
 SetAttribute_s(oID,cachePos,"m_taxiName","Taxi.Attr.anl_01");
 //SetAttribute_s(oID,cachePos,"m_soundName","");
 SetAttribute_s(oID,cachePos,"m_soundName","wav.ASteps");	
 SetAttribute_s(oID,cachePos,"m_bulletAttrName","Bullet.Led.Prim");
 SetAttribute_s(oID,cachePos,"m_bulletSecAttrName","Bullet.Sec");
 SetAttribute_f(oID,cachePos,"m_power",20);
 SetAttribute_f(oID,cachePos,"m_soundMinPitch",0.3);
 SetAttribute_f(oID,cachePos,"m_soundMaxPitch",1.4);
}

func void CreateVehicleAttr_anl_02(int ctID)
var int oID, cachePos;
{New(ctID,"Vehicle.Attr.anl_02",oID,cachePos);
 SetAttribute_i(oID,cachePos,"m_type",1);
 //SetAttribute_s(oID,cachePos,"m_panelName","");
 SetAttribute_s( oID, cachePos, "m_panelName", "Panels\dragon.pnl" ); 
 SetAttribute_s(oID,cachePos,"m_dynamic","Dragon");
 SetAttribute_s(oID,cachePos,"m_taxiName","Taxi.Attr.anl_02");
 SetAttribute_s(oID,cachePos,"m_soundName","wav.Dragon");
 SetAttribute_s(oID,cachePos,"m_bulletAttrName","Bullet.Led.Prim");
 SetAttribute_s(oID,cachePos,"m_bulletSecAttrName","Bullet.Sec");
 SetAttribute_f(oID,cachePos,"m_power",20);
 SetAttribute_f(oID,cachePos,"m_soundMinPitch",0.3);
 SetAttribute_f(oID,cachePos,"m_soundMaxPitch",1.2);
}

func void CreateVehicleAttr_vhl_hel(int ctID)
var int oID, cachePos;
{New(ctID,"Vehicle.Attr.vhl_hel",oID,cachePos);
 SetAttribute_i(oID,cachePos,"m_type",1);
 //SetAttribute_s(oID,cachePos,"m_panelName","");
 SetAttribute_s( oID, cachePos, "m_panelName", "Panels\maghelic.pnl" );
 SetAttribute_s(oID,cachePos,"m_taxiName","Taxi.Attr.vhl_hel");
 SetAttribute_s(oID,cachePos,"m_soundName","");
 SetAttribute_s(oID,cachePos,"m_bulletAttrName","Bullet.Led.Prim");
 SetAttribute_s(oID,cachePos,"m_bulletSecAttrName","Bullet.Sec");
 SetAttribute_f(oID,cachePos,"m_power",20);
 SetAttribute_s( oID, cachePos, "m_soundName","wav.Windcopter");
 SetAttribute_f(oID,cachePos,"m_soundMinPitch",0.3);
 SetAttribute_f(oID,cachePos,"m_soundMaxPitch",1.4);
}

func void main_CreateVehicleAttr()
 var int ctID;
{
  ctID := s_AddClassTable("VehicleAttr",10);
  CreateVehicleAttr_default( ctID );
  CreateVehicleAttr_dead   ( ctID );

  CreateVehicleAttr_anl_00(ctID);
  CreateVehicleAttr_anl_01(ctID);
  CreateVehicleAttr_anl_02(ctID);
  CreateVehicleAttr_vhl_hel(ctID);
}

func void main_CreateVehicle()
 var int ctID, oID, cachePos;
{
   ctID := s_AddClassTable( "Vehicle", 5 );

   s_New( ctID, "Vehicle.Default", oID, cachePos );
  ChangeObjectAttrN( "Vehicle.Attr.default", "Vehicle.Default" );
//  ChangeObjectAttrN( "Vehicle.Attr.anl_02", "Vehicle.Default" );
}

