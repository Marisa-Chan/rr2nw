include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"


func void CreateGroups() // own
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
 var int ctID, mctID;
{

    ctID := s_SearchSeanceClassTable("Route");
    mctID:= s_SearchSeanceClassTable("People");

    s_LoadRoute( ctID, "Route\m2g03ep0.rt"   ,"Route.m2g03.e.p0"   );
    CreateManName( mctID, "Peop.Attr.ship_stay","Route.m2g03.e.p0"   , 0, "ship.m2g03e0" );
    s_SetCommander("ship.m2g03e0", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g03ep1.rt"   ,"Route.m2g03.e.p1"   );
    CreateManName( mctID, "Peop.Attr.ship_stay","Route.m2g03.e.p1"   , 0, "ship.m2g03e1" );
    s_SetCommander("ship.m2g03e1", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g03ep2.rt"   ,"Route.m2g03.e.p2"   );
    CreateManName( mctID, "Peop.Attr.vhl_ship","Route.m2g03.e.p2"   , 0, "ship.m2g03e2" );
    s_SetCommander("ship.m2g03e2", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g03ep3.rt"   ,"Route.m2g03.e.p3"   );
    CreateManName( mctID, "Peop.Attr.vhl_ship","Route.m2g03.e.p3"   , 0, "ship.m2g03e3" );
    s_SetCommander("ship.m2g03e3", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g03ep4.rt"   ,"Route.m2g03.e.p4"   );
    CreateManName( mctID, "Peop.Attr.vhl_ship","Route.m2g03.e.p4"   , 0, "ship.m2g03e4" );
    s_SetCommander("ship.m2g03e4", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g03ep7.rt"   ,"Route.m2g03.e.p7"   );
    CreateManName( mctID, "Peop.Attr.ship_stay","Route.m2g03.e.p7"   , 0, "ship.m2g03e7" );
    s_SetCommander("ship.m2g03e7", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g03ep8.rt"   ,"Route.m2g03.e.p8"   );
    CreateManName( mctID, "Peop.Attr.ship_stay","Route.m2g03.e.p8"   , 0, "ship.m2g03e8" );
    s_SetCommander("ship.m2g03e8", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g03ep5.rt"   ,"Route.m2g03.e.p5"   );
    CreateManName( mctID, "Peop.Attr.gal.king","Route.m2g03.e.p5"   , 0, "gal.m2g03e5" );
    s_SetCommander("gal.m2g03e5", "Kingdom");

    groupName := "K.Group.203g00";
    comName   := "Kingdom";

    CreateGroup(groupName, comName);
    CreateUnit("K.Unit.2m03g0", groupName, "Tank.King.Attr");
    AddRouteToMember( comName, groupName, "m2g03.route.e.g6", "Route/m2g03eg6.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m2g03.route.e.p6", "Route/m2g03ep6.rt", 
                       1,0);

/***********  Provodniki rats  **************/ 

    s_LoadRoute( ctID, "Route\m2g03r0.rt"   ,"Route.m2g03.r0"   );
    CreateManName( mctID, "Peop.Attr.rat" ,"Route.m2g03.r0"   , 62, "rat.m2g03r0" );
    s_SetCommander("rat.m2g03r0", "Magician");

    s_LoadRoute( ctID, "Route\m2g03r1.rt"   ,"Route.m2g03.r1"   );
    CreateManName( mctID, "Peop.Attr.rat" ,"Route.m2g03.r1"   , 62, "rat.m2g03r1" );
    s_SetCommander("rat.m2g03r1", "Magician");

    s_LoadRoute( ctID, "Route\m2g03r2.rt"   ,"Route.m2g03.r2"   );
    CreateManName( mctID, "Peop.Attr.rat" ,"Route.m2g03.r2"   , 62, "rat.m2g03r2" );
    s_SetCommander("rat.m2g03r2", "Magician");

}

func void SetTaxi()
 var int ctID;
{
   ctID := s_SearchSeanceClassTable("Taxi");
   CreateTaxi(ctID,[3084.251, 2800.139, 52.026],"Taxi.Attr.vhl_hel");
}


func void main()
{
   SetTaxi();
   CreateGroups();
}
