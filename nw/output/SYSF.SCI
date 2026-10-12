const int ARTEFACT_ATTACH = 42000;

func void AddRouteToMember(str commander,
                           str memberName,
                           str rName, 
                           str fName, 
                           int patrol,int start)
 var int event, oID, cachePos;
{
   s_SearchObjectID( oID, cachePos, commander );

    event := s_OpenEventData(EDO_WRITE);
              s_WriteInt      ( event, 1);
              s_WriteStr      ( event, memberName );
              s_WriteStr      ( event, rName );
              s_WriteStr      ( event, fName );
              s_WriteInt      ( event, patrol );
              s_WriteInt      ( event, start );
             s_CloseEventData(event);
    s_SendEventNow( event,
                  com_EV_SET_ROUTE,
                  oID, cachePos );
}

func void ClrRoute(str commander, str memberName)
 var int event, oID, cachePos;
{
   s_SearchObjectID( oID, cachePos, commander );

    event := s_OpenEventData(EDO_WRITE);
              s_WriteInt      ( event, 0);
              s_WriteStr      ( event, memberName );
             s_CloseEventData(event);
    s_SendEventNow( event,
                  com_EV_SET_ROUTE,
                  oID, cachePos );
}

func void CreateGroup(str groupName, str comName)
 var int groupCTID, oID, cachePos;
{
    groupCTID := s_SearchSeanceClassTable("TankGroup");
    s_NewObject( groupCTID, groupName );
    s_SearchObjectID   ( oID, cachePos, comName );
    AddMemberToCommander( oID, cachePos, groupName );
}

func void CreateUnit(str name,str group,str attrName)
 var int ctID, oID, cachePos;
{
    ctID := s_SearchSeanceClassTable("Tank");
    s_NewObject( ctID, name );
    s_SearchObjectID( oID, cachePos, group );
    AddMemberToGroup( oID, cachePos, name ); 
    ChangeObjectAttrN( attrName,   name );
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

//****************************************

func void CreateHowitzerName( int ctID, str attrName, str holderName, float timeStart, str name )
 var int oID, cachePos, attrID, attrCP, event;
 var float time;
{
    s_New( ctID, name, oID, cachePos );
    s_SearchObjectID( attrID, attrCP, attrName );
    event := s_OpenEventData ( EDO_WRITE );
               s_WriteObjectID( event, attrID, attrCP );
               s_WriteStr     ( event, holderName );
             s_CloseEventData( event );
    

     time := s_GetTime();

     s_IssueEvent( event,
                   pe_EVCMD_START,
                   time + timeStart,
                   oID, cachePos );
}


func void CreateHowitzer( int ctID, str attrName, str holderName, float timeStart )
{
 CreateHowitzerName(ctID, attrName, holderName, timeStart, "Howitzer.Auto");
}


//****************************************

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

func void CreateManName( int ctID, str attrName, str routeName, float timeStart, str name )
 var int oID, cachePos, attrID, attrCP, event;
{
    s_New( ctID, name, oID, cachePos );
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

func void CreateTaxiName( int ctID, vector v, str attr, str name )
 var int oID, cachePos, attrID, attrCP, event;
{
   New(ctID, name, oID, cachePos);
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

func void AttachArtefact(
                           str artefact, 
                           str destName )
 var int event, destID, destCP, oID, cachePos;
{
    s_SearchObjectID( oID   , cachePos, artefact );
    s_SearchObjectID( destID, destCP,   destName );
    event := s_OpenEventData( EDO_WRITE );
             s_WriteObjectID      ( event, destID, destCP );
             s_CloseEventData( event );

    s_SendEventNow( event, ARTEFACT_ATTACH, oID, cachePos );
}
