include "../defines.sci"
include "../sys.sci"

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



// -------------------------- Start of Mission A07 --------------------------
func void CreateGroupsA07()
 var str szGroupName, szComName;
{
 // Confederates (Colony)

 // Enemies (Acteks)

 szComName:="Actek";
 szGroupName:="A.Group.ma07.00";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma07.00",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma07.route.eg0","Route/A07/eg0.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma07.route.ep0","Route/A07/ep0.rt",1,0);

 szGroupName:="A.Group.ma07.01";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma07.01",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma07.route.eg1","Route/A07/eg1.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma07.route.ep1","Route/A07/ep1.rt",1,0);

 szGroupName:="A.Group.ma07.02";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma07.02",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma07.route.eg2","Route/A07/eg2.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma07.route.ep2","Route/A07/ep2.rt",1,0);

 szGroupName:="A.Group.ma07.03";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma07.03",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma07.route.eg3","Route/A07/eg3.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma07.route.ep3","Route/A07/ep3.rt",1,0);

 szGroupName:="A.Group.ma07.04";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma07.04",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma07.route.eg4","Route/A07/eg4.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma07.route.ep4","Route/A07/ep4.rt",1,0);

 szGroupName:="A.Group.ma07.05";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma07.05",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma07.route.eg5","Route/A07/eg5.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma07.route.ep5","Route/A07/ep5.rt",1,0);

 szGroupName:="A.Group.ma07.06";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma07.06",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma07.route.eg6","Route/A07/eg6.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma07.route.ep6","Route/A07/ep6.rt",1,0);

 szGroupName:="A.Group.ma07.07";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma07.07",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma07.route.eg7","Route/A07/eg7.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma07.route.ep7","Route/A07/ep7.rt",1,0);
}

func void SetTaxiA07()
 var int ctID;
{ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2760,2632,74.144],"Taxi.Attr.cln_f05");
 CreateTaxi(ctID,[2822,3351,103.25],"Taxi.Attr.cln_f03");
 CreateTaxi(ctID,[3229.37,3224.64,72.3],"Taxi.Attr.cln_f05"); 
}

func void main()
{SetTaxiA07();
 CreateGroupsA07();
}
// --------------------------- End of Mission A07 ---------------------------
