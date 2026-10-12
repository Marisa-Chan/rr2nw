include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"


func void CreateGroups() // own
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
 var int ctID, mctID;
{

    ctID  := s_SearchSeanceClassTable("Route");
    mctID := s_SearchSeanceClassTable("People");

    s_LoadRoute( ctID, "Route\m2g06ep0.rt"   ,"Route.m2g06.e.p0"   );
    CreateManName( mctID, "Peop.Attr.ship_go","Route.m2g06.e.p0"   , 10, "ship.m2g06e0" );
    s_SetCommander( "ship.m2g06e0", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g06ep1.rt"   ,"Route.m2g06.e.p1"   );
    CreateManName( mctID, "Peop.Attr.ship_go","Route.m2g06.e.p1"   , 10, "ship.m2g06e1" );
    s_SetCommander( "ship.m2g06e1", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g06ep2.rt"   ,"Route.m2g06.e.p2"   );
    CreateManName( mctID, "Peop.Attr.ship_go","Route.m2g06.e.p2"   , 10, "ship.m2g06e2" );
    s_SetCommander( "ship.m2g06e2", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g06ep5.rt"   ,"Route.m2g06.e.p5"   );
    CreateManName( mctID, "Peop.Attr.gal.king","Route.m2g06.e.p5"   , 0, "gal.m2g06e5" );
    s_SetCommander("gal.m2g06e5", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g06ep6.rt"   ,"Route.m2g06.e.p6"   );
    CreateManName( mctID, "Peop.Attr.anl_02","Route.m2g06.e.p6"   , 0, "anl.m2g06e6" );
    s_SetCommander( "anl.m2g06e6", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g06ep8.rt"   ,"Route.m2g06.e.p8"   );
    CreateManName( mctID, "Peop.Attr.anl_02","Route.m2g06.e.p8"   , 0, "anl.m2g06e8" );
    s_SetCommander( "anl.m2g06e8", "Kingdom");

/*    s_LoadRoute( ctID, "Route\m2g06ep9.rt"   ,"Route.m2g06.e.p9"   );
    CreateManName( mctID, "Peop.Attr.anl_02","Route.m2g06.e.p9"   , 0, "anl.m2g06e9" );
    s_SetCommander("anl.m2g06e9", "Kingdom");
*/
    groupName := "K.Group.206g03";
    comName   := "Kingdom";

    CreateGroup(groupName, comName);
    CreateUnit("K.Unit.2m06g3", groupName, "Tank.King.Attr");
    AddRouteToMember( comName, groupName, "m2g06.route.e.g3", "Route/m2g06ep3.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m2g05.route.e.p3", "Route/m2g06ep3.rt", 
                       1,0);

    groupName := "K.Group.206g04";

    CreateGroup(groupName, comName);
    CreateUnit("K.Unit.2m06g4", groupName, "Tank.King.Attr");
    AddRouteToMember( comName, groupName, "m2g06.route.e.g4", "Route/m2g06ep4.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m2g06.route.e.p4", "Route/m2g06ep4.rt", 
                       1,0);

/*********************    Provodniki rats   ******************/
    s_LoadRoute( ctID, "Route\m2g06r0.rt"   ,"Route.m2g06.r0"   );
    CreateManName( mctID, "Peop.Attr.rat","Route.m2g06.r0"   , 53, "rat.m2g060" );
    s_SetCommander( "rat.m2g060", "Magician");

    s_LoadRoute( ctID, "Route\m2g06r1.rt"   ,"Route.m2g06.r1"   );
    CreateManName( mctID, "Peop.Attr.rat","Route.m2g06.r1"   , 53, "rat.m2g061" );
    s_SetCommander( "rat.m2g061", "Magician");

    s_LoadRoute( ctID, "Route\m2g06r2.rt"   ,"Route.m2g06.r2"   );
    CreateManName( mctID, "Peop.Attr.rat","Route.m2g06.r2"   , 53, "rat.m2g062" );
    s_SetCommander( "rat.m2g062", "Magician");

}

func void SetTaxi()
 var int ctID;
{
   ctID := s_SearchSeanceClassTable("Taxi");
   CreateTaxi(ctID, [3084.251, 2800.139, 52.026], "Taxi.Attr.vhl_hel");
}


func void main()
{
   SetTaxi ();
   CreateGroups ();
}
