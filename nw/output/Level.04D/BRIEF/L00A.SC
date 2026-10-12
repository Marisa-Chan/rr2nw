include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

// -------------------------- Start of Mission L00a --------------------------
func void CreateGroupsL00()
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
{
 /* Confederates */

 comName:="Actek";
 groupName:="A.Group.l00.00";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l00.00",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l00.route.g7","Route/LEV/l00_g7.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g7","Route/LEV/l00_g7.rt",1,0);

 /* Enemy */

 comName:="Colony";
 /*groupName:="C.Group.l00.00";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l00.00",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g0","Route/LEV/l00_g0.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.p0","Route/LEV/l00_p0.rt",1,0);

 groupName:="C.Group.l00.01";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l00.01",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.p0","Route/LEV/l00_p0.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g0","Route/LEV/l00_g0.rt",1,0);*/

 groupName:="C.Group.l00.02";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l00.02",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g1","Route/LEV/l00_g1.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g1","Route/LEV/l00_g1.rt",1,0);

 groupName:="C.Group.l00.03";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l00.03",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g2","Route/LEV/l00_g2.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.p2","Route/LEV/l00_p2.rt",1,0);

 groupName:="C.Group.l00.04";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l00.04",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g3","Route/LEV/l00_g3.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g3","Route/LEV/l00_g3.rt",1,0);

 groupName:="C.Group.l00.05";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l00.05",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g4","Route/LEV/l00_g4.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g4","Route/LEV/l00_g4.rt",1,0);

 groupName:="C.Group.l00.06";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l00.06",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g5","Route/LEV/l00_g5.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g5","Route/LEV/l00_g5.rt",1,0);
}

func void SetTaxiL00a()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2393,2580,63.75],"Taxi.Attr.cln_f03");
}

func void SetTaxiL00b()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2876,2382,59.8],"Taxi.Attr.cln_f03");
}

func void SetTaxiL00c()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2410,2590,67.15],"Taxi.Attr.cln_f03");
}

func void main()
{
 SetTaxiL00a();
 SetTaxiL00b();
 SetTaxiL00c();
 CreateGroupsL00();
}
// --------------------------- End of Mission L00a ---------------------------
