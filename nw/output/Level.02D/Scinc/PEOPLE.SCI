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
{nPeopleAttrCTID:=s_AddClassTable("PeopleAttr",10);

 CreatePeopleAttr(nPeopleAttrCTID,"Peop.Attr.anl_02","",NO_LAND,"sk.peop.anl_02",30,1.2,0,"","Corpse.Attr.Default");
 CreatePeopleAttr(nPeopleAttrCTID,"Peop.Attr.vhl_hel","",NO_LAND,"sk.peop.vhl_hel",30,1.0,0,"","Corpse.Attr.Default");
 CreatePeopleAttr(nPeopleAttrCTID,"Peop.Attr.man_knht","",ON_OBJECTS,"sk.peop.man_knht",2,0.9,0,"","");
 CreatePeopleAttr(nPeopleAttrCTID,"Peop.Attr.man_mknt","",ON_OBJECTS,"sk.peop.man_mknt",2,1,0,"","");
 CreatePeopleAttr(nPeopleAttrCTID,"Peop.Attr.man_peas","",ON_LAND,"sk.peop.man_peas",2,1,0,"","");
 CreatePeopleAttr(nPeopleAttrCTID,"Peop.Attr.vhl_wag","",ON_LAND,"sk.peop.vhl_wag",4,0.7,0,"","Corpse.Attr.Default");
 //CreatePeopleAttr(nPeopleAttrCTID,"Peop.Attr.Bowman","",ON_LAND,"sk.peop.Bowman", 3.5,0.7,0,"","Corpse.Attr.Default");
 //CreatePeopleAttr(nPeopleAttrCTID,"Peop.Attr.Bowman","",ON_LAND,"sk.peop.vhl_hel", 3.5,0.7,0,"","Corpse.Attr.Default");
 //CreatePeopleAttr(nPeopleAttrCTID,"Peop.Attr.Bowman","",ON_LAND,"sk.peop.vhl_ml", 5,0.7,0,"","Corpse.Attr.Default");
 CreatePeopleAttr(nPeopleAttrCTID,"Peop.Attr.Bowman","",ON_LAND,"sk.peop.vhl_wag", 5,0.7,0,"","Corpse.Attr.Default");

// CreatePeopleAttr(nPeopleAttrCTID,"Peop.Attr.anl_00","",ON_OBJECTS,"sk.tank.anl_00",18.0,1.6,0,"","Corpse.Attr.First");
// CreatePeopleAttr(nPeopleAttrCTID,"Peop.Attr.anl_01","",ON_OBJECTS,"sk.tank.anl_01",18.0,1.6,0,"","Corpse.Attr.Second");

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

 s_SearchObjectID(nObjectID,nCachePos,"Peop.Attr.man_mknt");
 SetAttribute_s(nObjectID,nCachePos,"m_bulletAttrName","Bullet.Led.Prim");
 SetAttribute_f(nObjectID,nCachePos,"m_cannonSpeed",1.00);
 SetAttribute_f(nObjectID,nCachePos,"m_initialDamage",0.06);
/*
 s_SearchObjectID(nObjectID,nCachePos,"Peop.Attr.anl_00");
 SetAttribute_s(nObjectID,nCachePos,"m_bulletAttrName","Bullet.Led.Prim");
 SetAttribute_f(nObjectID,nCachePos,"m_cannonSpeed",1200);

 s_SearchObjectID(nObjectID,nCachePos,"Peop.Attr.anl_01");
 SetAttribute_s(nObjectID,nCachePos,"m_bulletAttrName","Bullet.Led.Prim");
 SetAttribute_f(nObjectID,nCachePos,"m_cannonSpeed",1200);
*/
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
