include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

func void CreateGroups() // own
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
{
    groupName := "A.Group.8g00";
    comName   := "Actek";

    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m8g0",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m9.route.e.g0",  "Route/m9_e_g0.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m9.route.e.p0",  "Route/m9_e_p0.rt", 
                       1,0);

    groupName := "A.Group.8g01";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m8g1",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m9.route.e.g1",  "Route/m9_e_g1.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m9.route.e.p1",  "Route/m9_e_p1.rt", 
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
