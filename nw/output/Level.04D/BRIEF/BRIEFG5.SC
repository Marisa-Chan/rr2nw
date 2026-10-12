include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

func void CreateGroups() // own
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
{
    groupName := "A.Group.5g00";
    comName   := "Actek";

    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m5g0",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m6.route.e.g0",  "Route/m6_e_g0.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m6.route.e.p0",  "Route/m6_e_p0.rt", 
                       1,0);


    groupName := "A.Group.5g01";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m5g1",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m6.route.e.g1",  "Route/m6_e_g1.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m6.route.e.p1",  "Route/m6_e_p1.rt", 
                       1,0);

    groupName := "A.Group.5g02";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m5g2",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m6.route.e.g2",  "Route/m6_e_g2.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m6.route.e.p2",  "Route/m6_e_p2.rt", 
                       1,0);

    groupName := "A.Group.5g03";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m5g3",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m6.route.e.g3",  "Route/m6_e_g3.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m6.route.e.p3",  "Route/m6_e_p3.rt", 
                       1,0);

    groupName := "A.Group.5g04";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m5g4",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m6.route.e.g4",  "Route/m6_e_g4.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m6.route.e.p4",  "Route/m6_e_p4.rt", 
                       1,0);

    groupName := "A.Group.5g05";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m5g5",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m6.route.e.g5",  "Route/m6_e_g5.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m6.route.e.p5",  "Route/m6_e_p5.rt", 
                       1,0);



    groupName := "C.Group.5g00";
    comName   := "Colony";
    CreateGroup(groupName,comName);
    CreateUnit("C.Unit.m5g0",groupName,"TankEnglAttr");
    AddRouteToMember( comName, groupName, "m6.route.g0",  "Route/m6_g0.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m6.route.p0",  "Route/m6_p0.rt", 
                       1,0);

    groupName := "C.Group.5g01";
    CreateGroup(groupName,comName);
    CreateUnit("C.Unit.m5g1",groupName,"TankEnglAttr");
    AddRouteToMember( comName, groupName, "m6.route.g1",  "Route/m6_g1.rt", 
                       0,1);                
    AddRouteToMember( comName, groupName, "m6.route.p1",  "Route/m6_p1.rt", 
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
