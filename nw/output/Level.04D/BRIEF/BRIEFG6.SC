include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

func void CreateGroups() // own
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
{
    groupName := "A.Group.6g00";
    comName   := "Actek";

    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m6g0",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m7.route.e.g0",  "Route/m7_e_g0.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m7.route.e.p0",  "Route/m7_e_p0.rt", 
                       1,0);


    groupName := "A.Group.6g01";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m6g1",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m7.route.e.g1",  "Route/m7_e_g1.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m7.route.e.p1",  "Route/m7_e_p1.rt", 
                       1,0);

    groupName := "A.Group.6g02";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m6g2",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m7.route.e.g2",  "Route/m7_e_g2.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m7.route.e.p2",  "Route/m7_e_p2.rt", 
                       1,0);
}

func void SetTaxi()
 var int ctID;
{
   ctID := s_SearchSeanceClassTable("Taxi");
   CreateTaxi(ctID,[2935, 2460, 76.25 ],"Taxi.Attr.cln_f05");
}


func void main()
{
   SetTaxi();
   CreateGroups();
}
