include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

// -------------------------- Start of Mission L02a --------------------------
func void CreateGroupsL02()
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
{
 /* Confederates */

 /* Enemy */

 comName:="Actek";

 groupName:="A.Group.l02.00";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l02.00",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l00.route.g1","Route/LEV/l00_g1.rt",0,1);
 AddRouteToMember(comName,groupName,"l00.route.g1","Route/LEV/l00_g1.rt",1,0);

 groupName:="A.Group.l02.01";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l02.01",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l02.route.g0","Route/LEV/l02_g0.rt",0,1);
 AddRouteToMember(comName,groupName,"l02.route.g0","Route/LEV/l02_g0.rt",1,0);

 groupName:="A.Group.l02.02";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l02.02",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l02.route.g1","Route/LEV/l02_g1.rt",0,1);
 AddRouteToMember(comName,groupName,"l02.route.g1","Route/LEV/l02_g1.rt",1,0);

 groupName:="A.Group.l02.03";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l02.03",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l02.route.g2","Route/LEV/l02_g2.rt",0,1);
 AddRouteToMember(comName,groupName,"l02.route.g2","Route/LEV/l02_g2.rt",1,0);

 groupName:="A.Group.l02.04";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l02.04",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l02.route.g3","Route/LEV/l02_g3.rt",0,1);
 AddRouteToMember(comName,groupName,"l02.route.g3","Route/LEV/l02_g3.rt",1,0);

 groupName:="A.Group.l02.05";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l02.05",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l02.route.g4","Route/LEV/l02_g4.rt",0,1);
 AddRouteToMember(comName,groupName,"l02.route.g4","Route/LEV/l02_g4.rt",1,0);

 groupName:="A.Group.l02.06";
 CreateGroup(groupName,comName);
 CreateUnit("A.Unit.l02.06",groupName,"TankChAttr");
 AddRouteToMember(comName,groupName,"l02.route.g5","Route/LEV/l02_g5.rt",0,1);
 AddRouteToMember(comName,groupName,"l02.route.g5","Route/LEV/l02_g5.rt",1,0);
}

func void SetTaxiL02()
 var int ctID;
{ctID := s_SearchSeanceClassTable("Taxi");
 CreateTaxi(ctID,[2760,2632,74.144],"Taxi.Attr.cln_f05");
}

func void main()
{
 SetTaxiL02();
 CreateGroupsL02();
}
// --------------------------- End of Mission A02 ---------------------------
