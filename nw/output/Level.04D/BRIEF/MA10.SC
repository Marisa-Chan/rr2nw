include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

// -------------------------- Start of Mission A10 --------------------------
func void CreateGroupsA10()
 var int nRouteCTID,nPeopleCTID;
 var str szGroupName, szComName;
{
 // Confederates (Acteks)

 szComName:="Actek";
 szGroupName:="A.Group.ma10.00";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma10.00",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma10.route.g0","Route/A10/g0.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma10.route.p0","Route/A10/p0.rt",1,0);

 // Enemies (Colony)

 szComName:="Colony";
 szGroupName:="C.Group.ma10.00";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma10.00",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma10.route.eg0","Route/A10/eg0.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma10.route.ep0","Route/A10/ep0.rt",1,0);

 szGroupName:="C.Group.ma10.01";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma10.01",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma10.route.eg1","Route/A10/eg1.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma10.route.ep1","Route/A10/ep1.rt",1,0);

 szGroupName:="C.Group.ma10.02";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma10.02",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma10.route.eg2","Route/A10/eg2.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma10.route.ep2","Route/A10/ep2.rt",1,0);

 szGroupName:="C.Group.ma10.03";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma10.03",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma10.route.eg3","Route/A10/eg3.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma10.route.ep3","Route/A10/ep3.rt",1,0);

 szGroupName:="C.Group.ma10.04";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma10.04",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma10.route.eg4","Route/A10/eg4.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma10.route.ep4","Route/A10/ep4.rt",1,0);

 szGroupName:="C.Group.ma10.05";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma10.05",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma10.route.eg5","Route/A10/eg5.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma10.route.ep5","Route/A10/ep5.rt",1,0);

 nRouteCTID:=s_SearchSeanceClassTable("Route");
 nPeopleCTID:=s_SearchSeanceClassTable("People");
 s_LoadRoute(nRouteCTID,"Route\A10\eap0.rt","ma10.route.eap0");
 CreateManName(nPeopleCTID,"Peop.Attr.cln_f01","ma10.route.eap0",480,"C.Unit.ma10.06");
}

func void SetTaxiA10()
 var int nTaxiCTID;
{nTaxiCTID:=s_SearchSeanceClassTable("Taxi");
 CreateTaxi(nTaxiCTID,[2393,2572,63.75],"Taxi.Attr.cln_f03");
 CreateTaxi(nTaxiCTID,[3705.14,3221.85,166.52],"Taxi.Attr.cln_f01");
}

func void main()
{SetTaxiA10();
 CreateGroupsA10();
}
// --------------------------- End of Mission A10 ---------------------------
