include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

// -------------------------- Start of Mission L06 --------------------------
func void CreateGroupsL06()
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
{
 /* Confederates */

 comName:="Colony";
 groupName:="C.Group.l06.00";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l06.00",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g1","Route/LEV/l00_g1.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g1","Route/LEV/l00_g1.rt",1,0);

 /*comName:="Actek";
 groupName:="A.Group.l16.00";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l16.00",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l00.route.g1","Route/LEV/l00_g1.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g1","Route/LEV/l00_g1.rt",1,0);*/

 /* Enemy */

 comName:="Actek";

 groupName:="A.Group.l06.00";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l06.00",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l05.route.g0","Route/LEV/l05_g0.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g0","Route/LEV/l05_g0.rt",1,0);

 groupName:="A.Group.l06.01";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l06.01",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l05.route.g1","Route/LEV/l05_g1.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g1","Route/LEV/l05_g1.rt",1,0);

 /*groupName:="A.Group.l06.02";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l06.02",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l05.route.g2","Route/LEV/l05_g2.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g2","Route/LEV/l05_g2.rt",1,0);*/

 /*groupName:="A.Group.l06.03";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l06.03",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l05.route.g3","Route/LEV/l05_g3.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g3","Route/LEV/l05_g3.rt",1,0);

 groupName:="A.Group.l06.04";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l06.04",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l05.route.g4","Route/LEV/l05_g4.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g4","Route/LEV/l05_g4.rt",1,0);

 groupName:="A.Group.l06.05";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l06.05",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l05.route.g5","Route/LEV/l05_g5.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g5","Route/LEV/l05_g5.rt",1,0);

 groupName:="A.Group.l06.06";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l06.06",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l05.route.g6","Route/LEV/l05_g6.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g6","Route/LEV/l05_g6.rt",1,0);*/

 groupName:="A.Group.l06.07";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l06.07",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l05.route.g7","Route/LEV/l05_g7.rt",0,1);
 AddRouteToMember(comName,groupName,"l05.route.g7","Route/LEV/l05_g7.rt",1,0);

/* groupName:="A.Group.l06.08";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l06.08",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l00.route.g4","Route/LEV/l00_g4.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g4","Route/LEV/l00_g4.rt",1,0);

 groupName:="A.Group.l06.09";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l06.09",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l00.route.g0","Route/LEV/l00_g0.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.p0","Route/LEV/l00_p0.rt",1,0);*/
}

func void SetTaxiL06a()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2760,2632,74.144],"Taxi.Attr.cln_f05");
}

func void SetTaxiL06b()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2672,3176,77],"Taxi.Attr.cln_f05");
}

func void SetTaxiL06c()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2512,3236,76.05],"Taxi.Attr.cln_f05");
}

func void main()
{
 SetTaxiL06a();
 SetTaxiL06b();
 SetTaxiL06c();
 CreateGroupsL06();
}
