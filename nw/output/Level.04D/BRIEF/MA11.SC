include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"

// -------------------------- Start of Mission A11 --------------------------
func void CreateGroupsA11()
 var int nRouteCTID,nPeopleCTID;
 var str szGroupName, szComName;
{
 // Confederates (Colony)

 // Enemies (Acteks)

 szComName:="Actek";
 szGroupName:="A.Group.ma11.00";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma11.00",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma11.route.eg0","Route/A11/eg0.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma11.route.ep0","Route/A11/ep0.rt",1,0);

 szGroupName:="A.Group.ma11.01";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma11.01",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma11.route.eg1","Route/A11/eg1.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma11.route.ep1","Route/A11/ep1.rt",1,0);

 szGroupName:="A.Group.ma11.02";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma11.02",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma11.route.eg2","Route/A11/eg2.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma11.route.ep2","Route/A11/ep2.rt",1,0);

 szGroupName:="A.Group.ma11.03";
 CreateGroup(szGroupName,szComName);
 CreateUnit("A.Unit.ma11.03",szGroupName,"TankChAttr");
 AddRouteToMember(szComName,szGroupName,"ma11.route.eg3","Route/A11/eg3.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma11.route.ep3","Route/A11/ep3.rt",1,0);

 nRouteCTID:=s_SearchSeanceClassTable("Route");
 nPeopleCTID:=s_SearchSeanceClassTable("People");
 s_LoadRoute(nRouteCTID,"Route\A11\eap0.rt","ma11.route.eap0");
 CreateManName(nPeopleCTID,"Peop.Attr.shiz_f0a","ma11.route.eap0",48,"A.Unit.ma11.04");

 s_LoadRoute(nRouteCTID,"Route\A11\eap1.rt","ma11.route.eap1");
 CreateManName(nPeopleCTID,"Peop.Attr.cln_f04","ma11.route.eap1",5.5,"A.Unit.ma11.05");

 s_LoadRoute(nRouteCTID,"Route\A11\eap2.rt","ma11.route.eap2");
 CreateManName(nPeopleCTID,"Peop.Attr.cln_f04","ma11.route.eap2",6,"A.Unit.ma11.06");
}

func void SetTaxiA11()
 var int nTaxiCTID;
{nTaxiCTID:=s_SearchSeanceClassTable("Taxi");
 CreateTaxi(nTaxiCTID,[2760,2632,74.144],"Taxi.Attr.cln_f05");
 CreateTaxi(nTaxiCTID,[1606.87,2071.13,107.48],"Taxi.Attr.cln_f04");
 CreateTaxi(nTaxiCTID,[749.74,2590.76,107.30],"Taxi.Attr.cln_f04");
}

func void main()
{SetTaxiA11();
 CreateGroupsA11();
}
// --------------------------- End of Mission A11 ---------------------------
