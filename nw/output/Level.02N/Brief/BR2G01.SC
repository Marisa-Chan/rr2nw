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

    s_LoadRoute( ctID, "Route\1"   ,"Route.m2g01.rat1"   );
    CreateManName( mctID, "Peop.Attr.rat","Route.m2g01.rat1"   , 0, "rat.m2g01.1" );
    s_SetCommander("rat.m2g01.1", "Magician");

    s_LoadRoute( ctID, "Route\1"   ,"Route.m2g01.rat2"   );
    CreateManName( mctID, "Peop.Attr.rat","Route.m2g01.rat2"   , 0, "rat.m2g01.2" );
    s_SetCommander("rat.m2g01.2", "Magician");

    s_LoadRoute( ctID, "Route\1"   ,"Route.m2g01.rat3"   );
    CreateManName( mctID, "Peop.Attr.rat","Route.m2g01.rat3"   , 0, "rat.m2g01.3" );
    s_SetCommander("rat.m2g01.3", "Magician");

    groupName := "K.Group.201g00";
    comName   := "Kingdom";

    CreateGroup(groupName, comName);
    CreateUnit("K.Unit.2m01g0", groupName, "Tank.Magic.Attr");
    AddRouteToMember( comName, groupName, "m2g01.route.e.g0", "Route/m2g01eg0.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m2g01.route.e.p0", "Route/m2g01ep0.rt", 
                       1,0);

    groupName := "K.Group.201g01";
    CreateGroup( groupName, comName);
    CreateUnit( "K.Unit.2m01g1", groupName, "Tank.King.Attr");
    AddRouteToMember( comName, groupName, "m2g01.route.e.g1", "Route/m2g01eg1.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m2g01.route.e.p1", "Route/m2g01ep1.rt", 
                       1,0);

    groupName := "K.Group.201g02";
    CreateGroup( groupName, comName);
    CreateUnit( "K.Unit.2m01g2", groupName, "Tank.King.Attr");
    AddRouteToMember( comName, groupName, "m2g01.route.e.g2", "Route/m2g01eg2.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m2g01.route.e.p2", "Route/m2g01ep2.rt", 
                       1,0);

}

func void SetTaxi()
 var int ctID;
{
   ctID := s_SearchSeanceClassTable("Taxi");
   CreateTaxi(ctID,[2762, 2602, 76.25 ],"Taxi.Attr.anl_02");

}


func void main()
{
   SetTaxi();
   CreateGroups();
}
