include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

func void CreateGroups()
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
{
    comName   := "Actek";

    groupName := "A.Group.01";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m0.01",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m0.route.g0",  "Route/m0_g0.rt", 0,1);
    AddRouteToMember( comName, groupName, "m0.route.p0",  "Route/m0_p0.rt", 1,0);

    AttachArtefact("Artefact.0", "A.Unit.m0.01");

    groupName := "A.Group.02";
    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m0.02",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m0.route.g1",  "Route/m0_g1.rt", 0,1);
    AddRouteToMember( comName, groupName, "m0.route.p1",  "Route/m0_p1.rt", 1,0);

   //============== Enemy ===================================================

    comName   := "Colony";
    groupName := "C.Group.01";
    CreateGroup(groupName,comName);
    CreateUnit("C.Unit.m0.01",groupName,"TankEnglAttr");
    AddRouteToMember( comName, groupName,"m0.route.e.g0","Route/m0_e_g0.rt",0,1);
    AddRouteToMember( comName, groupName,"m0.route.e.p0","Route/m0_e_p0.rt",1,0);


    groupName := "C.Group.02";
    CreateGroup(groupName,comName);
    CreateUnit("C.Unit.m0.02",groupName,"TankEnglAttr");
    AddRouteToMember( comName, groupName,"m0.route.e.g1","Route/m0_e_g1.rt",0,1);
    AddRouteToMember( comName, groupName,"m0.route.e.p1","Route/m0_e_p1.rt",1,0);

    groupName := "C.Group.03";
    CreateGroup(groupName,comName);
    CreateUnit("C.Unit.m0.03",groupName,"TankEnglAttr");
    AddRouteToMember( comName, groupName,"m0.route.e.g2","Route/m0_e_g2.rt",0,1);
    AddRouteToMember( comName, groupName,"m0.route.e.p2","Route/m0_e_p2.rt",1,0);

}



func void SetTaxi()
 var int ctID;
{
   ctID := s_SearchSeanceClassTable("Taxi");
   CreateTaxi(ctID,[2393,2572,63.75],"Taxi.Attr.cln_f03");
}


func void main()
{
   print("Briefing started---------------");
   SetTaxi();
   CreateGroups();
}
