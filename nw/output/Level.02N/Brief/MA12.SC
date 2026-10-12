include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"
include "scinc/units.sci"

// -------------------------- Start of Mission A12 --------------------------
func void CreateGroupsA12()
 var str szGroupName, szComName;
 var int nRouteCTID,nPeopleCTID;
{
 // Confederates (Magician)
/*
 szComName:="Magician";
 szGroupName:="M.Group.ma12.00";
 CreateGroup(szGroupName,szComName);
 CreateUnit("M.Unit.ma12.00",szGroupName,"Tank.Magic.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.g0","Route/A12/g0.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.p0","Route/A12/p0.rt",1,0);

 szGroupName:="M.Group.ma12.01";
 CreateGroup(szGroupName,szComName);
 CreateUnit("M.Unit.ma12.01",szGroupName,"Tank.Magic.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.g1","Route/A12/g1.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.p1","Route/A12/p1.rt",1,0);

 szGroupName:="M.Group.ma12.02";
 CreateGroup(szGroupName,szComName);
 CreateUnit("M.Unit.ma12.02",szGroupName,"Tank.Magic.Attr");
 AddRouteToMember(szComName,szGroupName,"ma12.route.g2","Route/A12/g2.rt",0,1);
 AddRouteToMember(szComName,szGroupName,"ma12.route.p2","Route/A12/p2.rt",1,0);
*/
 // Enemies (Kingdom)
 CreateBallSenderFirst("Kingdom","k.group.ma12.00","k.unit.ma12.00","ma12.ebs00","Route/A12/ebs00.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma12.01","k.unit.ma12.01","ma12.ebs01","Route/A12/ebs01.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma12.02","k.unit.ma12.02","ma12.ebs02","Route/A12/ebs02.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma12.03","k.unit.ma12.03","ma12.ebs03","Route/A12/ebs03.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma12.04","k.unit.ma12.04","ma12.ebs04","Route/A12/ebs04.rt");
 CreateKingKnight("k.group.ma12.mn00","k.unit.ma12.mn00","ma12.emn00","Route/A12/emn00.rt",0);
 CreateKingKnight("k.group.ma12.mn01","k.unit.ma12.mn01","ma12.emn01","Route/A12/emn01.rt",0);
 CreateKingKnight("k.group.ma12.mn02","k.unit.ma12.mn02","ma12.emn02","Route/A12/emn02.rt",0);
 CreateKingKnight("k.group.ma12.mn03","k.unit.ma12.mn03","ma12.emn03","Route/A12/emn03.rt",0);
 CreateKingKnight("k.group.ma12.mn04","k.unit.ma12.mn04","ma12.emn04","Route/A12/emn04.rt",0);
 CreateKingKnight("k.group.ma12.mn05","k.unit.ma12.mn05","ma12.emn05","Route/A12/emn05.rt",0);
}

func void SetTaxiA12()
 var int nTaxiCTID;
{nTaxiCTID:=s_SearchSeanceClassTable("Taxi");
 CreateTaxi(nTaxiCTID,[3125.899,2776.687,52.613],"Taxi.Attr.anl_00");
 CreateTaxi(nTaxiCTID,[2437.039,2454.647,68.819],"Taxi.Attr.anl_02");
}

func void main()
{SetTaxiA12();
 CreateGroupsA12();
}
// --------------------------- End of Mission A12 ---------------------------
