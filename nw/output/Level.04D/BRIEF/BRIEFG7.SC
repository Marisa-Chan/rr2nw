include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

func void CreateGroups() // own
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
{
    groupName := "A.Group.7g00";
    comName   := "Actek";

    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m7g0",groupName,"TankEnglAttr");
    AddRouteToMember( comName, groupName, "m8.route.e.g0",  "Route/m8_e_g0.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m8.route.e.p0",  "Route/m8_e_p0.rt", 
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
