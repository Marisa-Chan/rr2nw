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
 var int ctID, oID, cachePos;
{
    ctID := s_AddClassTable( "PeopleAttr", 15 );
    CreatePeopleAttr( ctID, "Peop.Attr.cln_f01",  "Route.plane",    NO_LAND,    "sk.Taxi.cln_f01",  30, 1.4,  0 ,"wav.Engine2","Corpse.Attr.Plane");

//-----------------------------------------------------------------------------

    CreatePeopleAttr( ctID, "Peop.Attr.cln_f01_as_L", "Route.plane", NO_LAND, "sk.Taxi.cln_f01", 30, 2.5, 0 , "wav.Engine2", "Corpse.Attr.Plane");
    CreatePeopleAttr( ctID, "Peop.Attr.cln_f00_L",  "Route.plane", NO_LAND, "sk.Taxi.cln_f00", 20, 1.4, 0 , "wav.Engine2", "Corpse.Attr.Plane");
    CreatePeopleAttr( ctID, "Peop.Attr.cln_f00_pr_L",  "Route.plane", NO_LAND, "sk.Taxi.cln_f00", 10, 0.5, 0 , "wav.Engine2", "Corpse.Attr.Plane");
    CreatePeopleAttr( ctID, "Peop.Attr.cln_f04_L",  "Route.plane", NO_LAND, "sk.Taxi.cln_f04", 30, 1.4, 0 , "wav.Engine2", "Corpse.Attr.Plane");

//-----------------------------------------------------------------------------

    CreatePeopleAttr( ctID, "Peop.Attr.cln_f08",  "Route.car1",     ON_OBJECTS, "sk.Taxi.cln_f08",  15, 1.2,  0 ,"wav.Tank2","Corpse.Attr.Default");
    CreatePeopleAttr( ctID, "Peop.Attr.shiz_f0a", "Route.dirizabl", NO_LAND,    "sk.Peop.shiz_f0a", 10, 0.5,  0 ,"wav.PassBy1","Corpse.Attr.Default");
    CreatePeopleAttr( ctID, "Peop.Attr.cln_smn2", "Route.sub1",     NO_LAND,   "sk.Peop.cln_smn2", 15, 6,   -5 ,"wav.Engine7","Corpse.Attr.Default");
    CreatePeopleAttr( ctID, "Peop.Attr.lorr_f0b", "Route.car2",     ON_LAND,    "sk.Peop.lorr_f0b", 15, 6,    0 ,"wav.Tank2","Corpse.Attr.Default");
    CreatePeopleAttr( ctID, "Peop.Attr.man_wmn",  "Route.man1",     ON_LAND,    "sk.Peop.man_wmn",   5, 6,    1 ,"","");
    CreatePeopleAttr( ctID, "Peop.Attr.man_01",   "Route.man3",     ON_OBJECTS, "sk.Peop.man_01",    5, 6,    0 ,"","");
    CreatePeopleAttr( ctID, "Peop.Attr.cln_S00",  "Route.boat1",    ON_WATER,   "sk.Peop.cln_S00",   3, 3,    0 ,"wav.Kalflash","Corpse.Attr.Default");


    s_SearchObjectID( oID, cachePos, "Peop.Attr.cln_f01" );
    SetAttribute_s( oID, cachePos, "m_bulletAttrName", "Bullet.Led"  );
    SetAttribute_f( oID, cachePos, "m_cannonSpeed", 0.15  );
    //SetAttribute_i( oID, cachePos, "m_stopIfAttack", 1  );

//-----------------------------------------------------------------------------

    s_SearchObjectID( oID, cachePos, "Peop.Attr.cln_f01_as_L" );
    SetAttribute_s( oID, cachePos, "m_bulletAttrName", "Bullet.Led"  );
    SetAttribute_f( oID, cachePos, "m_cannonSpeed", 0.15  );
    //SetAttribute_i( oID, cachePos, "m_stopIfAttack", 1  );

    s_SearchObjectID( oID, cachePos, "Peop.Attr.cln_f00_L" );
    SetAttribute_s( oID, cachePos, "m_bulletAttrName", "Bullet.Led"  );
    SetAttribute_f( oID, cachePos, "m_cannonSpeed", 0.15  );
    //SetAttribute_i( oID, cachePos, "m_stopIfAttack", 1  );

    s_SearchObjectID( oID, cachePos, "Peop.Attr.cln_f00_pr_L" );
    SetAttribute_s( oID, cachePos, "m_bulletAttrName", "Bullet.Led"  );
    SetAttribute_f( oID, cachePos, "m_cannonSpeed", 0.15  );
    //SetAttribute_i( oID, cachePos, "m_stopIfAttack", 1  );

    s_SearchObjectID( oID, cachePos, "Peop.Attr.cln_f04_L" );
    SetAttribute_s( oID, cachePos, "m_bulletAttrName", "Bullet.Led"  );
    SetAttribute_f( oID, cachePos, "m_cannonSpeed", 0.15  );
    //SetAttribute_i( oID, cachePos, "m_stopIfAttack", 1  );

//-----------------------------------------------------------------------------

    s_SearchObjectID( oID, cachePos, "Peop.Attr.shiz_f0a" );
    SetAttribute_s( oID, cachePos, "m_bulletAttrName", "Bullet.Led"  );
    SetAttribute_f( oID, cachePos, "m_cannonSpeed", 0.5  );
    SetAttribute_f( oID, cachePos, "m_maxOutDist",  25  );

    s_SearchObjectID( oID, cachePos, "Peop.Attr.man_wmn" );
    SetAttribute_s( oID, cachePos, "m_bulletAttrName", "Bullet.Led"  );
    SetAttribute_f( oID, cachePos, "m_cannonSpeed", 1  );
    SetAttribute_f( oID, cachePos, "m_maxOutDist",  5  );
}

func void SetAnimatePeople()
 var int event, oID, cachePos;
 {
    CreateAnimSets (oID,cachePos, "sk.Peop.shiz_f0a", 0, 2 );
    SetAnimateBlock(oID,cachePos,0,  0, "VesloR");
    SetAnimateBlock(oID,cachePos,0,  1, "VesloL");

    CreateAnimSets (oID,cachePos, "sk.Taxi.cln_f01", 0, 1 );
    SetAnimateBlock(oID,cachePos,0,  0, "Prop");

    CreateAnimSets (oID,cachePos, "sk.Taxi.cln_f08", 1, 6 );
    SetAnimateBlock(oID,cachePos,0,  0, "KFL");
    SetAnimateBlock(oID,cachePos,0,  1, "KFR");
    SetAnimateBlock(oID,cachePos,0,  2, "KBL");
    SetAnimateBlock(oID,cachePos,0,  3, "KBR");
    SetAnimateBlock(oID,cachePos,0,  4, "Mah");
    SetAnimateBlock(oID,cachePos,0,  5, "P");
    SetAnimateBlock(oID,cachePos,1,  0, "update");
 }
