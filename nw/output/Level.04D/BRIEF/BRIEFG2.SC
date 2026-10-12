include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

func void CreateGroups() // own
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
{
    groupName := "A.Group.2g01";
    comName   := "Actek";

    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m2g0",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m3.route.e.g0",  "Route/m3_e_g0.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m3.route.e.p0",  "Route/m3_e_p0.rt", 
                       1,0);


    groupName := "A.Group.2g02";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m2g1",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m3.route.e.g1",  "Route/m3_e_g1.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m3.route.e.p1",  "Route/m3_e_p1.rt", 
                       1,0);

    groupName := "A.Group.2g03";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m2g2",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m3.route.e.g2",  "Route/m3_e_g2.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m3.route.e.p2",  "Route/m3_e_p2.rt", 
                       1,0);

    groupName := "C.Group.2g01";
    comName   := "Colony";
    CreateGroup(groupName,comName);
    CreateUnit("C.Unit.m2g0",groupName,"TankEnglAttr");
    AddRouteToMember( comName, groupName, "m3.route.g0",  "Route/m3_g0.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m3.route.p0",  "Route/m3_p0.rt", 
                       1,0);

    groupName := "C.Group.2g02";
    CreateGroup(groupName,comName);
    CreateUnit("C.Unit.m2g1",groupName,"TankEnglAttr");
    AddRouteToMember( comName, groupName, "m3.route.g1",  "Route/m3_g1.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m3.route.p1",  "Route/m3_p1.rt", 
                       1,0);

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
