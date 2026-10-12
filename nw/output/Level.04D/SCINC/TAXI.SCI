
func void CreateTaxiAttr_cln_f01(int ctID)
 var int oID, cachePos;
{
   New( ctID, "Taxi.Attr.cln_f01", oID, cachePos );
   SetAttribute_s( oID, cachePos, "m_skinName", "sk.Taxi.cln_f01" );
   SetAttribute_s( oID, cachePos, "m_attrForVehicleName", "Vehicle.Attr.f01" );	
   SetAttribute_i( oID, cachePos, "m_buzzing", 1);
}
func void CreateTaxiAttr_cln_f00(int ctID)
 var int oID, cachePos;
{
   New( ctID, "Taxi.Attr.cln_f00", oID, cachePos );
   SetAttribute_s( oID, cachePos, "m_skinName", "sk.Taxi.cln_f00" );
   SetAttribute_s( oID, cachePos, "m_attrForVehicleName", "Vehicle.Attr.f00" );
   SetAttribute_i( oID, cachePos, "m_buzzing", 1);
}
func void CreateTaxiAttr_cln_f03(int ctID)
 var int oID, cachePos;
{
   New( ctID, "Taxi.Attr.cln_f03", oID, cachePos );
   SetAttribute_s( oID, cachePos, "m_skinName", "sk.Taxi.cln_f03" );
   SetAttribute_s( oID, cachePos, "m_attrForVehicleName", "Vehicle.Attr.f03" );
   SetAttribute_i( oID, cachePos, "m_buzzing", 1);
}
func void CreateTaxiAttr_cln_f04(int ctID)
 var int oID, cachePos;
{
   New( ctID, "Taxi.Attr.cln_f04", oID, cachePos );
   SetAttribute_s( oID, cachePos, "m_skinName", "sk.Taxi.cln_f04" );
   SetAttribute_s( oID, cachePos, "m_attrForVehicleName", "Vehicle.Attr.helicop" );
   SetAttribute_i( oID, cachePos, "m_buzzing", 1);
}
func void CreateTaxiAttr_cln_f05(int ctID)
 var int oID, cachePos;
{
   New( ctID, "Taxi.Attr.cln_f05", oID, cachePos );
   SetAttribute_s( oID, cachePos, "m_skinName", "sk.Taxi.cln_f05" );
   SetAttribute_s( oID, cachePos, "m_attrForVehicleName", "Vehicle.Attr.otank" );
   SetAttribute_i( oID, cachePos, "m_buzzing", 1);
}
func void CreateTaxiAttr_cln_f08(int ctID)
 var int oID, cachePos;
{
   New( ctID, "Taxi.Attr.cln_f08", oID, cachePos );
   SetAttribute_s( oID, cachePos, "m_skinName", "sk.Taxi.cln_f08" );
   SetAttribute_s( oID, cachePos, "m_attrForVehicleName", "Vehicle.Attr.f08" );
   SetAttribute_i( oID, cachePos, "m_buzzing", 1);
}

func void CreateTaxiAttrCorpseFinal(int ctID)
 var int oID, cachePos;
{
   New( ctID, "Taxi.Attr.CorpseFinal", oID, cachePos );
   SetAttribute_s( oID, cachePos, "m_skinName", "sk.corpse.final" );
   SetAttribute_s( oID, cachePos, "m_attrForVehicleName", "Vehicle.Attr.default" );
   SetAttribute_s( oID, cachePos, "m_corpseAttrName", "Corpse.Attr.Final" );
   SetAttribute_i( oID, cachePos, "m_buzzing", 1);
}


func void CreateTaxi( int ctID, vector v, str attr )
 var int oID, cachePos, attrID, attrCP, event;
{
   s_New(ctID,"Taxi.Obj",oID, cachePos);
   s_SearchObjectID( attrID, attrCP, attr );

    event := s_OpenEventData(EDO_WRITE);
             s_WriteObjectID ( event, attrID, attrCP );
             s_WriteFloat    ( event, v.x );
             s_WriteFloat    ( event,-v.y );
             s_CloseEventData( event );

    s_SendEventNow( event,
                  KR_SET_ATTR,
                  oID, cachePos );
}

func void main_CreateTaxiAttr()
 var int ctID;
{
   ctID := s_AddClassTable("TaxiAttr",7);
   CreateTaxiAttr_cln_f01(ctID);
   CreateTaxiAttr_cln_f00(ctID);
   CreateTaxiAttr_cln_f03(ctID);
   CreateTaxiAttr_cln_f04(ctID);
   CreateTaxiAttr_cln_f05(ctID);
   CreateTaxiAttr_cln_f08(ctID);
   CreateTaxiAttrCorpseFinal(ctID);
}

