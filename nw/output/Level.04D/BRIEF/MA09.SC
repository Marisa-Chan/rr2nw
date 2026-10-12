include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

// -------------------------- Start of Mission A09 --------------------------
func void CreateGroupsA09()
 var int nRouteCTID,nPeopleCTID;
 var str szGroupName, szComName;
{
 // Confederates (Acteks)

 szComName:="Actek";
 szGroupName:="A.Group.ma09.00";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma09.00",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma09.route.g0","Route/A09/g0.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma09.route.p0","Route/A09/p0.rt",1,0);

 szGroupName:="A.Group.ma09.01";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma09.01",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma09.route.g1","Route/A09/g1.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma09.route.p1","Route/A09/p1.rt",1,0);

 // Enemies (Colony)

 szComName:="Colony";
 szGroupName:="C.Group.ma09.00";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma09.00",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma09.route.eg0","Route/A09/eg0.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma09.route.ep0","Route/A09/ep0.rt",1,0);

 szGroupName:="C.Group.ma09.01";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma09.01",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma09.route.eg1","Route/A09/eg1.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma09.route.ep1","Route/A09/ep1.rt",1,0);

 szGroupName:="C.Group.ma09.02";
 CreateGroup(szGroupName,szComName);
 CreateUnit("C.Unit.ma09.02",szGroupName,"TankEnglAttr");
 AddRouteToMember(szComName,szGroupName,"ma09.route.eg2","Route/A09/eg2.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma09.route.ep2","Route/A09/ep2.rt",1,0);

 nRouteCTID:=s_SearchSeanceClassTable("Route");
 nPeopleCTID:=s_SearchSeanceClassTable("People");
 s_LoadRoute(nRouteCTID,"Route\A09\eap0.rt","ma09.route.eap0");
 CreateManName(nPeopleCTID,"Peop.Attr.cln_f01","ma09.route.eap0",140,"C.Unit.ma09.03");
}

func void SetTaxiA09()
 var int nTaxiCTID;
{nTaxiCTID:=s_SearchSeanceClassTable("Taxi");
 CreateTaxi(nTaxiCTID,[2393,2572,63.75],"Taxi.Attr.cln_f03");
 CreateTaxi(nTaxiCTID,[2629.5,3272.78,76.05],"Taxi.Attr.cln_f04");
}

func void main()
{SetTaxiA09();
 CreateGroupsA09();
}
// --------------------------- End of Mission A09 ---------------------------
