include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

// -------------------------- Start of Mission L01 --------------------------
func void CreateGroupsL01()
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
{
 /* Confederates */

 /*comName:="Colony";
 groupName:="C.Group.l01.00";
 CreateGroup(groupName,comName);
 CreateUnit("C.Unit.l01.00",groupName,"TankEnglAttr");
 AddRouteToMember(comName,groupName,"l00.route.g1","Route/LEV/l00_g1.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g1","Route/LEV/l00_g1.rt",1,0);*/

 /* Enemy */

 comName:="Actek";

 /*groupName:="A.Group.l01.00";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l01.00",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l00.route.g6","Route/LEV/l00_g6.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g6","Route/LEV/l00_g6.rt",1,0);*/

 groupName:="A.Group.l01.01";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l01.01",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l00.route.g0","Route/LEV/l00_g0.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g0","Route/LEV/l00_g0.rt",1,0);

 /*groupName:="A.Group.l01.02";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l01.02",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l00.route.p0","Route/LEV/l00_p0.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g0","Route/LEV/l00_g0.rt",1,0);*/

 groupName:="A.Group.l01.03";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l01.03",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l00.route.p2","Route/LEV/l00_p2.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.p2","Route/LEV/l00_p2.rt",1,0);

 groupName:="A.Group.l01.04";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l01.04",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l00.route.p3","Route/LEV/l00_p3.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.p3","Route/LEV/l00_p3.rt",1,0);

 /*groupName:="A.Group.l01.05";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l01.05",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l00.route.g4","Route/LEV/l00_g4.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g4","Route/LEV/l00_g4.rt",1,0);*/

 groupName:="A.Group.l01.06";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l01.06",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l00.route.g5","Route/LEV/l00_g5.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g5","Route/LEV/l00_g5.rt",1,0);
}

func void SetTaxiL01a()
 var int ctID;
{ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2760,2632,74.144],"Taxi.Attr.cln_f05");
}

func void SetTaxiL01b()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2876,2382,59.8],"Taxi.Attr.cln_f05");
}

func void SetTaxiL01c()
 var int ctID;
{
 ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2393,2580,63.75],"Taxi.Attr.cln_f03");
}

func void main()
{
 SetTaxiL01a();
 SetTaxiL01b();
 SetTaxiL01c();
 CreateGroupsL01();
}
// --------------------------- End of Mission L01 ---------------------------
