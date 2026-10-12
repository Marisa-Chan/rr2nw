include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"


func void CreateGroups() // own
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
{
    comName   := "Actek";

    groupName := "A.Group.m0g0";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m0g0",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m1.route.g0",  "Route/m1_g0.rt", 0,1);
    AddRouteToMember( comName, groupName, "m1.route.p0",  "Route/m1_p0.rt", 1,0);


    comName   := "Colony"; 

    groupName := "C.Group.m0g1";
    CreateGroup(groupName,comName);
    CreateUnit("C.Unit.m0g1",groupName,"TankEnglAttr");
    AddRouteToMember( comName, groupName, "m1.route.e.g0",  "Route/m1_e_p0.rt", 0,1);
    AddRouteToMember( comName, groupName, "m1.route.e.p0",  "Route/m1_e_p0.rt", 1,0);

    groupName := "C.Group.m0g2";
    CreateGroup(groupName,comName);
    CreateUnit("C.Unit.m0g2",groupName,"TankEnglAttr");
    AddRouteToMember( comName, groupName, "m1.route.e.g1",  "Route/m1_e_p1.rt", 0,1);
    AddRouteToMember( comName, groupName, "m1.route.e.p1",  "Route/m1_e_p1.rt", 1,0);

    groupName := "C.Group.m0g3";
    CreateGroup(groupName,comName);
    CreateUnit("C.Unit.m0g3",groupName,"TankEnglAttr");
    AddRouteToMember( comName, groupName, "m1.route.e.g2",  "Route/m1_e_g2.rt", 0,1);
    AddRouteToMember( comName, groupName, "m1.route.e.p2",  "Route/m1_e_p2.rt", 1,0);

    groupName := "C.Group.m0g4";
    CreateGroup(groupName,comName);
    CreateUnit("C.Unit.m0g4",groupName,"TankEnglAttr");
    AddRouteToMember( comName, groupName, "m1.route.e.g3",  "Route/m1_e_g3.rt", 0,1);
    AddRouteToMember( comName, groupName, "m1.route.e.p3",  "Route/m1_e_p3.rt", 1,0);
}


func void SetTaxi()
 var int ctID;
{
   ctID := s_SearchSeanceClassTable("Taxi");
   CreateTaxi(ctID,[2343,2572,63.75],"Taxi.Attr.cln_f03");
   CreateTaxi(ctID,[1921.226, 3033.241,107.012],"Taxi.Attr.cln_f03");
}


func void main()
{
   SetTaxi();
   CreateGroups();
}
