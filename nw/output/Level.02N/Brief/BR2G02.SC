include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"
include "scinc/units.sci"

func void CreateGroups() // own
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
 var int ctID, mctID;
{

    groupName := "K.Group.202g00";
    comName   := "Kingdom";

    CreateGroup(groupName, comName);
    CreateUnit("K.Unit.2m02g0", groupName, "Tank.King.Attr");
    AddRouteToMember( comName, groupName, "m2g02.route.e.g0", "Route/m2g02eg0.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m2g02.route.e.p0", "Route/m2g02ep0.rt", 
                       1,0);

    groupName := "K.Group.202g01";
    CreateGroup( groupName, comName);
    CreateUnit( "K.Unit.2m02g1", groupName, "Tank.King.Attr");
    AddRouteToMember( comName, groupName, "m2g02.route.e.g1", "Route/m2g02eg1.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m2g02.route.e.p1", "Route/m2g02ep1.rt", 
                       1,0);

    groupName := "K.Group.202g02";
    CreateGroup( groupName, comName);
    CreateUnit( "K.Unit.2m02g2", groupName, "Tank.King.Attr");
    AddRouteToMember( comName, groupName, "m2g02.route.e.g2", "Route/m2g02eg2.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m2g02.route.e.p2", "Route/m2g02ep2.rt", 
                       1,0);

    groupName := "K.Group.202g03";
    CreateGroup( groupName, comName);
    CreateUnit( "K.Unit.2m02g3", groupName, "Tank.King.Attr");
    AddRouteToMember( comName, groupName, "m2g02.route.e.g3", "Route/m2g02eg3.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m2g02.route.e.p3", "Route/m2g02ep3.rt", 
                       1,0);

    groupName := "K.Group.202g04";
    CreateGroup( groupName, comName);
    CreateUnit( "K.Unit.2m02g4", groupName, "Tank.King.Attr");
    AddRouteToMember( comName, groupName, "m2g02.route.e.g4", "Route/m2g02eg4.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m2g02.route.e.p4", "Route/m2g02ep4.rt", 
                       1,0);

    groupName := "K.Group.202g05";
    CreateGroup( groupName, comName);
    CreateUnit( "K.Unit.2m02g5", groupName, "Tank.King.Attr");
    AddRouteToMember( comName, groupName, "m2g02.route.e.g5", "Route/m2g02eg5.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m2g02.route.e.p5", "Route/m2g02ep5.rt", 
                       1,0);

//------------------------------------     Magic      -----------------------//

    groupName := "M.Group.202g02";
    comName   := "Magician";

    CreateGroup(groupName, comName);
    CreateUnit("M.Unit.2m02g2", groupName, "Tank.Magic.Attr");
    AddRouteToMember( comName, groupName, "m2g02.route.g2", "Route/m2g02g2.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m2g02.route.p2", "Route/m2g02p2.rt", 
                       1,0);
    CreateMortarFirst("Magician", "M.Group.202g00", "M.Unit.2m02g0", "m2g02.route.g0", "Route/m2g02g0.rt");
    CreateMortarFirst("Magician", "M.Group.202g01", "M.Unit.2m02g1", "m2g02.route.g1", "Route/m2g02g1.rt");
}

func void SetTaxi()
 var int ctID;
{
   ctID := s_SearchSeanceClassTable("Taxi");
   CreateTaxi(ctID,[3084.251, 2800.139, 52.026],"Taxi.Attr.anl_01");
}


func void main()
{
   SetTaxi();
   CreateGroups();
}
