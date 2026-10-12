func void CreateTaxiAttrCorpseFinal(int ctID)
 var int oID, cachePos;
{
   New( ctID, "Taxi.Attr.CorpseFinal", oID, cachePos );
   SetAttribute_s( oID, cachePos, "m_skinName", "sk.corpse.final" );
   SetAttribute_s( oID, cachePos, "m_attrForVehicleName", "Vehicle.Attr.default" );
   SetAttribute_s( oID, cachePos, "m_corpseAttrName", "Corpse.Attr.Final" );
}

func void CreateTaxiAttr_vessel(int ctID)
 var int oID, cachePos;
{
   New( ctID, "Taxi.Attr.vessel", oID, cachePos );
   SetAttribute_s( oID, cachePos, "m_skinName", "sk.Taxi.vessel" );
   SetAttribute_s( oID, cachePos, "m_attrForVehicleName", "Vehicle.Attr.vessel" );
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
   ctID := s_AddClassTable("TaxiAttr",2);
   CreateTaxiAttr_vessel(ctID);
   CreateTaxiAttrCorpseFinal(ctID);
}

