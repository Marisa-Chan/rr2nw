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



// -------------------------- Start of Mission A08 --------------------------
func void CreateGroupsA08()
 var str szGroupName, szComName;
{
 // Confederates (Acteks)

 // Enemies (Colony)

 szComName:="Colony";
 szGroupName:="C.Group.ma08.00";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma08.00",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma08.route.eg0","Route/A08/eg0.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma08.route.ep0","Route/A08/ep0.rt",1,0);

 szGroupName:="C.Group.ma08.01";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma08.01",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma08.route.eg1","Route/A08/eg1.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma08.route.ep1","Route/A08/ep1.rt",1,0);

 szGroupName:="C.Group.ma08.02";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma08.02",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma08.route.eg2","Route/A08/eg2.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma08.route.ep2","Route/A08/ep2.rt",1,0);

 szGroupName:="C.Group.ma08.03";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma08.03",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma08.route.eg3","Route/A08/eg3.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma08.route.ep3","Route/A08/ep3.rt",1,0);

 szGroupName:="C.Group.ma08.04";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma08.04",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma08.route.eg4","Route/A08/eg4.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma08.route.ep4","Route/A08/ep4.rt",1,0);

 szGroupName:="C.Group.ma08.05";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma08.05",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma08.route.eg5","Route/A08/eg5.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma08.route.ep5","Route/A08/ep5.rt",1,0);
}

func void SetTaxiA08()
 var int ctID;
{ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2393,2572,63.75],"Taxi.Attr.cln_f03");
 CreateTaxi(ctID,[3001.5,1846.8,97.2],"Taxi.Attr.cln_f05");
}

func void main()
{SetTaxiA08();
 CreateGroupsA08();
}
// --------------------------- End of Mission A08 ---------------------------
