include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

// -------------------------- Start of Mission L05 --------------------------
func void CreateGroupsL05()
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
{
 /* Confederates */

 comName:="Actek";
 groupName:="A.Group.l05.00";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l05.00",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l05.route.p0","Route/LEV/l05_p0.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.p1","Route/LEV/l05_p1.rt",1,0);

 /*comName:="Colony";
 groupName:="C.Group.l15.00";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l15.00",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g6","Route/LEV/l00_g6.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g6","Route/LEV/l00_g6.rt",1,0);*/

 /* Enemy */

 comName:="Colony";

 groupName:="C.Group.l05.00";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l05.00",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l05.route.g0","Route/LEV/l05_g0.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g0","Route/LEV/l05_g0.rt",1,0);

 groupName:="C.Group.l05.01";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l05.01",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l05.route.g1","Route/LEV/l05_g1.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g1","Route/LEV/l05_g1.rt",1,0);

 groupName:="C.Group.l05.02";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l05.02",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l05.route.g2","Route/LEV/l05_g2.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g2","Route/LEV/l05_g2.rt",1,0);

 /*groupName:="C.Group.l05.03";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l05.03",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l05.route.g3","Route/LEV/l05_g3.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g3","Route/LEV/l05_g3.rt",1,0);

 groupName:="C.Group.l05.04";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l05.04",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l05.route.g4","Route/LEV/l05_g4.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g4","Route/LEV/l05_g4.rt",1,0);

 groupName:="C.Group.l05.05";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l05.05",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l05.route.g5","Route/LEV/l05_g5.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g5","Route/LEV/l05_g5.rt",1,0);

 groupName:="C.Group.l05.06";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l05.06",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l05.route.g6","Route/LEV/l05_g6.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g6","Route/LEV/l05_g6.rt",1,0);*/

 groupName:="C.Group.l05.07";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l05.07",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l05.route.g7","Route/LEV/l05_g7.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g7","Route/LEV/l05_g7.rt",1,0);

 /*groupName:="C.Group.l05.08";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l05.08",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g2","Route/LEV/l00_g2.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.p2","Route/LEV/l00_p2.rt",1,0);*/
}

func void SetTaxiL05a()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2393,2572,63.75],"Taxi.Attr.cln_f03");
}

func void SetTaxiL05b()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2672,3176,77],"Taxi.Attr.cln_f03");
}

func void SetTaxiL05c()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2512,3236,76.05],"Taxi.Attr.cln_f03");
}

func void main()
{
 SetTaxiL05a();
 SetTaxiL05b();
 SetTaxiL05c();
 CreateGroupsL05();
}
