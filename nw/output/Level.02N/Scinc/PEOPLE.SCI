/**************************************************************************
 *
 *                    People
 *
 **************************************************************************/
func void CreatePeopleAttr( int ctID, str attrName, str   route, 
                                                    int   onLand, 
                                                    str   skin,
                                                    float speed,
                                                    float rollSpeed,
                                                    float cy,
					            str	  sound,
						    str	  corpse
                          )
 var int oID, cachePos;
{
    s_New( ctID, attrName, oID, cachePos );

    SetAttribute_s( oID, cachePos, "m_routeName", route  );
    SetAttribute_f( oID, cachePos, "m_timeInc",   0.08   );
    SetAttribute_i( oID, cachePos, "m_onLand",    onLand );
    SetAttribute_s( oID, cachePos, "m_skinName",  skin   );
    SetAttribute_f( oID, cachePos, "m_speed",     speed  );
    SetAttribute_f( oID, cachePos, "m_rollSpeed", rollSpeed  );
    SetAttribute_f( oID, cachePos, "m_centerHeight", cy  );
    SetAttribute_s( oID, cachePos, "m_soundName",  sound   );
    SetAttribute_s( oID, cachePos, "m_explAttrName",  "Expl.Attr.Big");
    SetAttribute_s( oID, cachePos, "m_smokeAttrName", "Smoke.Attr.Small");
    SetAttribute_s( oID, cachePos, "m_corpseAttrName", corpse);
}                       

func void CreateMan( int ctID, str attrName, str routeName, float timeStart )
 var int oID, cachePos, attrID, attrCP, event;
{
    s_New( ctID, "Peop.Auto", oID, cachePos );
    s_SearchObjectID( attrID, attrCP, attrName );
    event := s_OpenEventData ( EDO_WRITE );
               s_WriteObjectID( event, attrID, attrCP );
               s_WriteStr     ( event, routeName );
               s_WriteFloat   ( event, timeStart );
             s_CloseEventData( event );

    s_SendEventNow( event,
                  pe_EVCMD_START,
                  oID, cachePos );
}

func void CreateManName( int ctID, str attrName, str routeName, float timeStart, str mname )
 var int oID, cachePos, attrID, attrCP, event;
{
    s_New( ctID, mname, oID, cachePos );
    s_SearchObjectID( attrID, attrCP, attrName );
    event := s_OpenEventData ( EDO_WRITE );
               s_WriteObjectID( event, attrID, attrCP );
               s_WriteStr     ( event, routeName );
               s_WriteFloat   ( event, timeStart );
             s_CloseEventData( event );

    s_SendEventNow( event,
                  pe_EVCMD_START,
                  oID, cachePos );
}

func void main_CreatePeopleAttr()
 var int nPeopleAttrCTID,nObjectID,nCachePos;
{nPeopleAttrCTID:=s_AddClassTable("PeopleAttr",11);

 CreatePeopleAttr(nPeopleAttrCTID,"Peop.Attr.anl_02","",NO_LAND,"sk.peop.anl_02",30,1.2,0,"wav.Dragon","Corpse.Attr.Default");
 CreatePeopleAttr(nPeopleAttrCTID,"Peop.Attr.vhl_hel","",NO_LAND,"sk.peop.vhl_hel",30,1.0,0,"wav.Windcopter","Corpse.Attr.Default");
 CreatePeopleAttr(nPeopleAttrCTID,"Peop.Attr.man_knht","",ON_OBJECTS,"sk.peop.man_knht",2,0.9,0,"","");
 CreatePeopleAttr(nPeopleAttrCTID,"Peop.Attr.man_mknt","",ON_OBJECTS,"sk.peop.man_mknt",2,1,0,"","");
 CreatePeopleAttr(nPeopleAttrCTID,"Peop.Attr.man_peas","",ON_LAND,"sk.peop.man_peas",2,1,0,"","");

 CreatePeopleAttr( nPeopleAttrCTID, "Peop.Attr.vhl_ship",  "", ON_WATER, "sk.ship.vhl_shp",   2.5, 0.3,    2 ,"","Corpse.Attr.Default");
 CreatePeopleAttr( nPeopleAttrCTID, "Peop.Attr.ship_stay",  "", ON_WATER, "sk.ship.stay",   0, 3,    2.5 ,"","Corpse.Attr.Default");
 CreatePeopleAttr( nPeopleAttrCTID, "Peop.Attr.rat",  "", NO_LAND, "sk.rat",   20, 3,    2 ,"","Corpse.Attr.Default");
 CreatePeopleAttr( nPeopleAttrCTID, "Peop.Attr.gal",  "", ON_WATER, "sk.ship.gal",   9.0, 0.3,    0 ,"","Corpse.Attr.Default");
 CreatePeopleAttr( nPeopleAttrCTID, "Peop.Attr.gal.king",  "", ON_WATER, "sk.ship.gal.king",   6.0, 0.2,    0 ,"","Corpse.Attr.Default");
 CreatePeopleAttr( nPeopleAttrCTID, "Peop.Attr.ship_go",  "", ON_WATER, "sk.ship.vhl_shp",   6, 0.3,    2 ,"","Corpse.Attr.Default");

 s_SearchObjectID(nObjectID,nCachePos,"Peop.Attr.anl_02");
 SetAttribute_s(nObjectID,nCachePos,"m_bulletAttrName","Bullet.Led");
 SetAttribute_f(nObjectID,nCachePos,"m_cannonSpeed",0.15);

 s_SearchObjectID(nObjectID,nCachePos,"Peop.Attr.vhl_hel");
 SetAttribute_s(nObjectID,nCachePos,"m_bulletAttrName","Bullet.Led");
 SetAttribute_f(nObjectID,nCachePos,"m_cannonSpeed",0.15);

 s_SearchObjectID(nObjectID,nCachePos,"Peop.Attr.man_knht");
 SetAttribute_s(nObjectID,nCachePos,"m_bulletAttrName","Bullet.Led.Prim");
 SetAttribute_f(nObjectID,nCachePos,"m_cannonSpeed",1.00);
 SetAttribute_f(nObjectID,nCachePos,"m_initialDamage",0.06);
 SetAttribute_i(nObjectID,nCachePos,"m_stopIfAttack",1);

 s_SearchObjectID(nObjectID,nCachePos,"Peop.Attr.man_mknt");
 SetAttribute_s(nObjectID,nCachePos,"m_bulletAttrName","Bullet.Led.Prim");
 SetAttribute_f(nObjectID,nCachePos,"m_cannonSpeed",1.00);
 SetAttribute_f(nObjectID,nCachePos,"m_initialDamage",0.06);
 SetAttribute_i(nObjectID,nCachePos,"m_stopIfAttack",1);

 s_SearchObjectID(nObjectID,nCachePos,"Peop.Attr.rat");
 SetAttribute_s(nObjectID,nCachePos,"m_bulletAttrName","Bullet.Led");
 SetAttribute_f(nObjectID,nCachePos,"m_cannonSpeed",1.5);

 s_SearchObjectID(nObjectID,nCachePos,"Peop.Attr.vhl_ship");
 SetAttribute_s(nObjectID,nCachePos,"m_bulletAttrName","Bullet.Led");
 SetAttribute_f(nObjectID,nCachePos,"m_cannonSpeed",0.5);
 SetAttribute_i (nObjectID, nCachePos, "m_stopIfAttack", 1);
 SetAttribute_i (nObjectID, nCachePos, "m_burstCount", 5);
 SetAttribute_f (nObjectID, nCachePos, "m_deltaZeroSpeed", 0.93);

 s_SearchObjectID(nObjectID,nCachePos,"Peop.Attr.ship_go");
 SetAttribute_s(nObjectID,nCachePos,"m_bulletAttrName","Bullet.Led");
 SetAttribute_f(nObjectID,nCachePos,"m_cannonSpeed", 1);
 SetAttribute_i (nObjectID, nCachePos, "m_stopIfAttack", 1);
 SetAttribute_i (nObjectID, nCachePos, "m_burstCount", 5);
 SetAttribute_f (nObjectID, nCachePos, "m_deltaZeroSpeed", 0.96);

}

func void SetAnimatePeople()
 var int event, oID, cachePos;
 {
/*
    CreateAnimSets (oID,cachePos, "sk.Peop.shiz_f0a", 0, 2 );
    SetAnimateBlock(oID,cachePos,0,  0, "VesloR");
    SetAnimateBlock(oID,cachePos,0,  1, "VesloL");
 */
 }
