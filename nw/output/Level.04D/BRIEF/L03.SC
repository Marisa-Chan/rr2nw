include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

// -------------------------- Start of Mission L03 --------------------------
func void CreateGroupsL03()
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
{
 /* Confederates */

 comName:="Actek";

 groupName:="A.Group.l03.00";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l03.00",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l02.route.g7","Route/LEV/l02_g7.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g8","Route/LEV/l00_g8.rt",0,1);
 AddRouteToMember(comName,groupName,"l02.route.p0","Route/LEV/l02_p0.rt",1,0);

 /* Enemy */

 comName:="Colony";

/* groupName:="C.Group.l03.00";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l03.00",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g0","Route/LEV/l00_g0.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.p0","Route/LEV/l00_p0.rt",1,0);

 groupName:="C.Group.l03.01";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l03.01",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.p0","Route/LEV/l00_p0.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g0","Route/LEV/l00_g0.rt",1,0);*/

 groupName:="C.Group.l03.02";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l03.02",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l02.route.g6","Route/LEV/l02_g6.rt",0,1);
 AddRouteToMember(comName,groupName,"l02.route.g6","Route/LEV/l02_g6.rt",1,0);

 groupName:="C.Group.l03.03";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l03.03",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g5","Route/LEV/l00_g5.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g5","Route/LEV/l00_g5.rt",1,0);

 groupName:="C.Group.l03.04";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l03.04",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g1","Route/LEV/l00_g1.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g1","Route/LEV/l00_g1.rt",1,0);

 groupName:="C.Group.l03.05";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l03.05",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l02.route.g0","Route/LEV/l02_g0.rt",0,1);
 AddRouteToMember(comName,groupName,"l02.route.g0","Route/LEV/l02_g0.rt",1,0);

 /*groupName:="C.Group.l03.06";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l03.06",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l02.route.g1","Route/LEV/l02_g1.rt",0,1);
 AddRouteToMember(comName,groupName,"l02.route.g1","Route/LEV/l02_g1.rt",1,0);*/

 groupName:="C.Group.l03.07";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l03.07",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l02.route.g2","Route/LEV/l02_g2.rt",0,1);
 AddRouteToMember(comName,groupName,"l02.route.g2","Route/LEV/l02_g2.rt",1,0);

 /*groupName:="C.Group.l03.08";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l03.08",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l02.route.g3","Route/LEV/l02_g3.rt",0,1);
 AddRouteToMember(comName,groupName,"l02.route.g3","Route/LEV/l02_g3.rt",1,0);*/

 groupName:="C.Group.l03.09";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l03.09",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l02.route.g4","Route/LEV/l02_g4.rt",0,1);
 AddRouteToMember(comName,groupName,"l02.route.g4","Route/LEV/l02_g4.rt",1,0);

 /*groupName:="C.Group.l03.10";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l03.10",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l02.route.g5","Route/LEV/l02_g5.rt",0,1);
 AddRouteToMember(comName,groupName,"l02.route.g5","Route/LEV/l02_g5.rt",1,0);*/
}

func void SetTaxiL03a()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2393,2572,63.75],"Taxi.Attr.cln_f03");
}

func void SetTaxiL03b()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[3360,3078,72.3],"Taxi.Attr.cln_f03");
}

func void SetTaxiL03c()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2876,2382,59.8],"Taxi.Attr.cln_f03");
}

func void main()
{
 SetTaxiL03a();
 SetTaxiL03b();
 SetTaxiL03c();
 CreateGroupsL03();
}
// --------------------------- End of Mission L03 ---------------------------
