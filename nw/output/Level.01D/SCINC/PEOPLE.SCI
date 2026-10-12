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
					            str	  sound
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
    SetAttribute_s( oID, cachePos, "m_explAttrName",  "Expl.Attr.Default");
    SetAttribute_s( oID, cachePos, "m_smokeAttrName", "Smoke.Attr.Small");
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

func void main_CreatePeopleAttr()
 var int ctID;
{
/*
    ctID := s_AddClassTable( "PeopleAttr",10 );
    CreatePeopleAttr( ctID, "Peop.Attr.cln_f01",  "Route.plane",    NO_LAND,    "sk.Taxi.cln_f01",  30, 1.5,  0 ,"wav.Engine2");
*/
}


func void SetAnimatePeople()
 var int event, oID, cachePos;
 {
 }
