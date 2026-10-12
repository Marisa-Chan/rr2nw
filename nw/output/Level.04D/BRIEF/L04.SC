include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

// -------------------------- Start of Mission L04 --------------------------
func void CreateGroupsL04()
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
{
 /* Confederates */

 comName:="Actek";

 groupName:="A.Group.l04.00";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l04.00",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l00.route.g9","Route/LEV/l00_g9.rt",0,1);
 AddRouteToMember(comName,groupName,"l04.route.g5","Route/LEV/l04_g5.rt",1,0);

 /*comName:="Colony";
 groupName:="C.Group.l15.00";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l15.00",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g6","Route/LEV/l00_g6.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g6","Route/LEV/l00_g6.rt",1,0);*/

 /* Enemy */

 comName:="Colony";

 groupName:="C.Group.l04.03";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l04.03",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l04.route.g1","Route/LEV/l04_g1.rt",0,1);
 AddRouteToMember(comName,groupName,"l04.route.g1","Route/LEV/l04_g1.rt",1,0);

 groupName:="C.Group.l04.04";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l04.04",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l04.route.g2","Route/LEV/l04_g2.rt",0,1);
 AddRouteToMember(comName,groupName,"l04.route.g2","Route/LEV/l04_g2.rt",1,0);

 groupName:="C.Group.l04.05";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l04.05",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l04.route.g5","Route/LEV/l04_g5.rt",0,1);
 AddRouteToMember(comName,groupName,"l04.route.g5","Route/LEV/l04_g5.rt",1,0);
}

func void SetTaxiL04a()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2393,2572,63.75],"Taxi.Attr.cln_f03");
}

func void SetTaxiL04b()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2371,2576,67.15],"Taxi.Attr.cln_f03");
}

func void main()
{
 SetTaxiL04a();
 SetTaxiL04b();
 CreateGroupsL04();
}
// --------------------------- End of Mission L04 ---------------------------
