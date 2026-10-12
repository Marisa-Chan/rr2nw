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



// -------------------------- Start of Mission A02 --------------------------
func void CreateGroupsA02()
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
{
 /* Confederates */

 comName:="Actek";
 groupName:="A.Group.ma02.00";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.ma02.00",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"ma02.route.g0","Route/A02/g0.rt",0,1);
 AddRouteToMember(comName,groupName,"ma02.route.p0","Route/A02/p0.rt",1,0);

 groupName:="A.Group.ma02.01";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.ma02.01",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"ma02.route.g1","Route/A02/g1.rt",0,1);
 AddRouteToMember(comName,groupName,"ma02.route.p1","Route/A02/p1.rt",1,0);

 groupName:="A.Group.ma02.02";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.ma02.02",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"ma02.route.g2","Route/A02/g2.rt",0,1);
 AddRouteToMember(comName,groupName,"ma02.route.p2","Route/A02/p2.rt",1,0);

 /* Enemy */

 comName:="Colony";
 groupName:="C.Group.ma02.00";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.ma02.00",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"ma02.route.eg0","Route/A02/eg0.rt",0,1);
 AddRouteToMember(comName,groupName,"ma02.route.ep0","Route/A02/ep0.rt",1,0);

 groupName:="C.Group.ma02.01";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.ma02.01",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"ma02.route.eg1","Route/A02/eg1.rt",0,1);
 AddRouteToMember(comName,groupName,"ma02.route.ep1","Route/A02/ep1.rt",1,0);

 groupName:="C.Group.ma02.02";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.ma02.02",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"ma02.route.e.g2","Route/A02/eg2.rt",0,1);
 AddRouteToMember(comName,groupName,"ma02.route.e.p2","Route/A02/ep2.rt",1,0);

 groupName:="C.Group.ma02.03";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.ma02.03",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"ma02.route.e.g3","Route/A02/eg3.rt",0,1);
 AddRouteToMember(comName,groupName,"ma02.route.e.p3","Route/A02/ep3.rt",1,0);
}

func void SetTaxiA02()
 var int ctID;
{ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2393,2572,63.75],"Taxi.Attr.cln_f03");
 CreateTaxi(ctID,[3431.82,3273,72.30],"Taxi.Attr.cln_f05");
}

func void main()
{SetTaxiA02();
 CreateGroupsA02();
}
// --------------------------- End of Mission A02 ---------------------------
