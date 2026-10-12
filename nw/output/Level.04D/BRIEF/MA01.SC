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



// -------------------------- Start of Mission A01 --------------------------
func void CreateGroupsA01()
 var str groupName, comName;
{
 /* Confederates */

 comName:="Actek";
 groupName:="A.Group.ma01.00";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.ma01.00",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"ma01.route.g0","Route/A01/g0.rt",0,1);
 AddRouteToMember(comName,groupName,"ma01.route.p0","Route/A01/p0.rt",1,0);

 groupName:="A.Group.ma01.01";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.ma01.01",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"ma01.route.g1","Route/A01/g1.rt",0,1);
 AddRouteToMember(comName,groupName,"ma01.route.p1","Route/A01/p1.rt",1,0);

 groupName:="A.Group.ma01.02";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.ma01.02",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"ma01.route.g2","Route/A01/g2.rt",0,1);
 AddRouteToMember(comName,groupName,"ma01.route.p2","Route/A01/p2.rt",1,0);

 /* Enemy */

 comName:="Colony";
 groupName:="C.Group.ma01.00";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.ma01.00",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"ma01.route.eg0","Route/A01/eg0.rt",0,1);
 AddRouteToMember(comName,groupName,"ma01.route.ep0","Route/A01/ep0.rt",1,0);

 groupName:="C.Group.ma01.01";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.ma01.01",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"ma01.route.eg1","Route/A01/eg1.rt",0,1);
 AddRouteToMember(comName,groupName,"ma01.route.ep1","Route/A01/ep1.rt",1,0);

 groupName:="C.Group.ma01.02";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.ma01.02",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"ma01.route.e.g2","Route/A01/eg2.rt",0,1);
 AddRouteToMember(comName,groupName,"ma01.route.e.p2","Route/A01/ep2.rt",1,0);

 groupName:="C.Group.ma01.03";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.ma01.03",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"ma01.route.e.g3","Route/A01/eg3.rt",0,1);
 AddRouteToMember(comName,groupName,"ma01.route.e.p3","Route/A01/ep3.rt",1,0);

 groupName:="C.Group.ma01.04";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.ma01.04",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"ma01.route.e.g4","Route/A01/eg4.rt",0,1);
 AddRouteToMember(comName,groupName,"ma01.route.e.p4","Route/A01/ep4.rt",1,0);

 groupName:="C.Group.ma01.05";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.ma01.05",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"ma01.route.e.g5","Route/A01/eg5.rt",0,1);
 AddRouteToMember(comName,groupName,"ma01.route.e.p5","Route/A01/ep5.rt",1,0);
}

func void SetTaxiA01()
 var int ctID;
{ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2393,2572,63.75],"Taxi.Attr.cln_f03");
 CreateTaxi(ctID,[1891,3624,63.55],"Taxi.Attr.cln_f03");
}

func void main()
{SetTaxiA01();
 CreateGroupsA01();
}
// --------------------------- End of Mission A01 ---------------------------
