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

    s_LoadRoute ( ctID, "Route\m2g05p0.rt"   ,"Route.m2g05.p0"   );
    CreateManName ( mctID, "Peop.Attr.ship_go","Route.m2g05.p0"   , 15, "ship.m2g050" );
    s_SetCommander ("ship.m2g050", "Kingdom");
			
    s_LoadRoute( ctID, "Route\m2g05p1.rt"   ,"Route.m2g05.p1"   );
    CreateManName( mctID, "Peop.Attr.ship_go","Route.m2g05.p1"   , 15, "ship.m2g051" );
    s_SetCommander("ship.m2g051", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g05p2.rt"   ,"Route.m2g05.p2"   );
    CreateManName( mctID, "Peop.Attr.ship_go","Route.m2g05.p2"   , 15, "ship.m2g052" );
    s_SetCommander("ship.m2g052", "Kingdom");

/***************        Provodnik   dragon     **************/
    s_LoadRoute( ctID, "Route\m2g05p3.rt"   ,"Route.m2g05.p3"   );
    CreateManName( mctID, "Peop.Attr.anl_02","Route.m2g05.p3"   , 56, "anl.m2g053" );
    s_SetCommander("anl.m2g053", "Kingdom");
    s_SetDamage("anl.m2g053", 0.7);

    s_LoadRoute( ctID, "Route\m2g05ep5.rt"   ,"Route.m2g05.e.p5"   );
    CreateManName( mctID, "Peop.Attr.vhl_hel","Route.m2g05.e.p5"   , 65, "hel.m2g05e5" );
    s_SetCommander("hel.m2g05e5", "Magician");

    s_LoadRoute( ctID, "Route\m2g05ep6.rt"   ,"Route.m2g05.e.p6"   );
    CreateManName( mctID, "Peop.Attr.vhl_hel","Route.m2g05.e.p6"   , 65, "hel.m2g05e6" );
    s_SetCommander("hel.m2g05e6", "Magician");

    s_LoadRoute( ctID, "Route\m2g05ep2.rt"   ,"Route.m2g05.e.p2"   );
    CreateManName( mctID, "Peop.Attr.gal","Route.m2g05.e.p2"   , 0, "gal.m2g05e2" );
    s_SetCommander("gal.m2g05e2", "Magician");
    SetToRoute ( "gal.m2g05e2", 1, 5, 0.96);

    s_LoadRoute( ctID, "Route\m2g05ep3.rt"   ,"Route.m2g05.e.p3"   );
    CreateManName( mctID, "Peop.Attr.vhl_hel","Route.m2g05.e.p3"   , 0, "hel.m2g05e3" );
    s_SetCommander("hel.m2g05e3", "Magician");

    s_LoadRoute( ctID, "Route\m2g05ep4.rt"   ,"Route.m2g05.e.p4"   );
    CreateManName( mctID, "Peop.Attr.vhl_hel","Route.m2g05.e.p4"   , 0, "hel.m2g05e4" );
    s_SetCommander("hel.m2g05e4", "Magician");

    groupName := "M.Group.205g00";
    comName   := "Magician";

    CreateGroup(groupName, comName);
    CreateUnit("M.Unit.2m05g0", groupName, "Tank.Magic.Attr");
    AddRouteToMember( comName, groupName, "m2g05.route.e.g0", "Route/m2g05ep0.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m2g05.route.e.p0", "Route/m2g05ep0.rt", 
                       1,0);

    groupName := "M.Group.205g01";

    CreateGroup(groupName, comName);
    CreateUnit("M.Unit.2m05g1", groupName, "Tank.Magic.Attr");
    AddRouteToMember( comName, groupName, "m2g05.route.e.g1", "Route/m2g05ep1.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m2g05.route.e.p1", "Route/m2g05ep1.rt", 
                       1,0);

}

func void SetTaxi()
 var int ctID;
{
   ctID := s_SearchSeanceClassTable("Taxi");
   CreateTaxi(ctID,[2594.958, 2790.316, 52.000],"Taxi.Attr.anl_02");
}


func void main()
{
   SetTaxi();
   CreateGroups();
}
