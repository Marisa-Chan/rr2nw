include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

func void CreateGroups() // own
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
{
    groupName := "A.Group.4g01";
    comName   := "Actek";

    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m4g0",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m5.route.e.g0",  "Route/m5_e_g0.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m5.route.e.p0",  "Route/m5_e_p0.rt", 
                       1,0);


    groupName := "A.Group.4g02";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m4g1",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m5.route.e.g1",  "Route/m5_e_g1.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m5.route.e.p1",  "Route/m5_e_p1.rt", 
                       1,0);

    groupName := "A.Group.4g03";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m4g2",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m5.route.e.g2",  "Route/m5_e_g2.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m5.route.e.p2",  "Route/m5_e_p2.rt", 
                       1,0);

    groupName := "A.Group.4g04";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m4g3",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m5.route.e.g3",  "Route/m5_e_g3.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m5.route.e.p3",  "Route/m5_e_p3.rt", 
                       1,0);


    groupName := "C.Group.4g01";
    comName   := "Colony";
    CreateGroup(groupName,comName);
    CreateUnit("C.Unit.m4g0",groupName,"TankEnglAttr");
    AddRouteToMember( comName, groupName, "m5.route.g0",  "Route/m5_g0.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m5.route.p0",  "Route/m5_p0.rt", 
                       1,0);

    groupName := "C.Group.4g02";
    CreateGroup(groupName,comName);
    CreateUnit("C.Unit.m4g1",groupName,"TankEnglAttr");
    AddRouteToMember( comName, groupName, "m5.route.g1",  "Route/m5_g1.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m5.route.p1",  "Route/m5_p1.rt", 
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
