include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

func void CreateGroups() // own
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
{
    comName   := "Actek";

    groupName := "A.Group.m1g0";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m1g0",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m2.route.e.g0",  "Route/m2_e_g0.rt", 0,1);
    AddRouteToMember( comName, groupName, "m2.route.e.p0",  "Route/m2_e_p0.rt", 1,0);


    groupName := "A.Group.m1g1";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m1g1",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m2.route.e.g1",  "Route/m2_e_p1.rt", 0,1);
    AddRouteToMember( comName, groupName, "m2.route.e.p1",  "Route/m2_e_p1.rt", 1,0);

    groupName := "A.Group.m1g2";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m1g2",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m2.route.e.g2",  "Route/m2_e_p2.rt", 0,1);
    AddRouteToMember( comName, groupName, "m2.route.e.p2",  "Route/m2_e_p2.rt", 1,0);
}


func void SetTaxi()
 var int ctID;
{
   ctID := s_SearchSeanceClassTable("Taxi");
   CreateTaxi(ctID,[2720, 2720, 76.25 ],"Taxi.Attr.cln_f05");
}


func void main()
{
   SetTaxi();
   CreateGroups();
}
