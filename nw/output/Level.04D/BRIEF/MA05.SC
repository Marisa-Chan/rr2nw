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



// -------------------------- Start of Mission A05 --------------------------
func void CreateGroupsA05()
 var str szGroupName, szComName;
{
 // Confederates (Acteks)

 szComName:="Actek";
 szGroupName:="A.Group.ma05.00";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma05.00",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma05.route.g0","Route/A05/g0.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma05.route.p0","Route/A05/p0.rt",1,0);

 szGroupName:="A.Group.ma05.01";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma05.01",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma05.route.g1","Route/A05/g1.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma05.route.p1","Route/A05/p1.rt",1,0);

 szGroupName:="A.Group.ma05.02";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma05.02",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma05.route.g2","Route/A05/g2.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma05.route.p2","Route/A05/p2.rt",1,0);

 szGroupName:="A.Group.ma05.03";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma05.03",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma05.route.g3","Route/A05/g3.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma05.route.p3","Route/A05/p3.rt",1,0);

 // Enemies (Colony)

 szComName:="Colony";
 szGroupName:="C.Group.ma05.00";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma05.00",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma05.route.eg0","Route/A05/eg0.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma05.route.ep0","Route/A05/ep0.rt",1,0);

 szGroupName:="C.Group.ma05.01";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma05.01",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma05.route.eg1","Route/A05/eg1.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma05.route.ep1","Route/A05/ep1.rt",1,0);

 szGroupName:="C.Group.ma05.02";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma05.02",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma05.route.eg2","Route/A05/eg2.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma05.route.ep2","Route/A05/ep2.rt",1,0);

 szGroupName:="C.Group.ma05.03";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma05.03",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma05.route.eg3","Route/A05/eg3.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma05.route.ep3","Route/A05/ep3.rt",1,0);

 szGroupName:="C.Group.ma05.04";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma05.04",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma05.route.eg4","Route/A05/eg4.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma05.route.ep4","Route/A05/ep4.rt",1,0);

 szGroupName:="C.Group.ma05.05";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma05.05",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma05.route.eg5","Route/A05/eg5.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma05.route.ep5","Route/A05/ep5.rt",1,0);
}

func void SetTaxiA05()
 var int ctID;
{ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2393,2572,63.75],"Taxi.Attr.cln_f03");
}

func void main()
{SetTaxiA05();
 CreateGroupsA05();
}
// --------------------------- End of Mission A05 ---------------------------
