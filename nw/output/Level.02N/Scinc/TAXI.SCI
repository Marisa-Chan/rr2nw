func void CreateTaxiAttrCorpseFinal(int ctID)
 var int oID, cachePos;
{
   New( ctID, "Taxi.Attr.CorpseFinal", oID, cachePos );
   SetAttribute_s( oID, cachePos, "m_skinName", "sk.corpse.final" );
   SetAttribute_s( oID, cachePos, "m_attrForVehicleName", "Vehicle.Attr.default" );
   SetAttribute_s( oID, cachePos, "m_corpseAttrName", "Corpse.Attr.Final" );
}

func void CreateTaxiAttr_anl_00(int nTaxiAttrCTID)
 var int nObjectID,nCachePos;
{New(nTaxiAttrCTID,"Taxi.Attr.anl_00",nObjectID,nCachePos);
 SetAttribute_s(nObjectID,nCachePos,"m_skinName","sk.tank.anl_00");
 SetAttribute_s(nObjectID,nCachePos,"m_attrForVehicleName","Vehicle.Attr.anl_00");
}

func void CreateTaxiAttr_anl_01(int nTaxiAttrCTID)
 var int nObjectID,nCachePos;
{New(nTaxiAttrCTID,"Taxi.Attr.anl_01",nObjectID,nCachePos);
 SetAttribute_s(nObjectID,nCachePos,"m_skinName","sk.taxi.anl_01");
 SetAttribute_s(nObjectID,nCachePos,"m_attrForVehicleName","Vehicle.Attr.anl_01");
}

func void CreateTaxiAttr_anl_02(int nTaxiAttrCTID)
 var int nObjectID,nCachePos;
{New(nTaxiAttrCTID,"Taxi.Attr.anl_02",nObjectID,nCachePos);
 SetAttribute_s(nObjectID,nCachePos,"m_skinName","sk.taxi.anl_02");
 SetAttribute_s(nObjectID,nCachePos,"m_attrForVehicleName","Vehicle.Attr.anl_02");
}

func void CreateTaxiAttr_vhl_hel(int nTaxiAttrCTID)
 var int nObjectID,nCachePos;
{New(nTaxiAttrCTID,"Taxi.Attr.vhl_hel",nObjectID,nCachePos);
 SetAttribute_s(nObjectID,nCachePos,"m_skinName","sk.peop.vhl_hel");
 SetAttribute_s(nObjectID,nCachePos,"m_attrForVehicleName","Vehicle.Attr.vhl_hel");
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
{ctID := s_AddClassTable("TaxiAttr",8);
 CreateTaxiAttrCorpseFinal(ctID);
 CreateTaxiAttr_anl_00(ctID);
 CreateTaxiAttr_anl_01(ctID);
 CreateTaxiAttr_anl_02(ctID);
 CreateTaxiAttr_vhl_hel(ctID);
}
