include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"
include "scinc/units.sci"

// -------------------------- Start of Mission A13 --------------------------
func void CreateGroupsA13()
{
 // Confederates (Magician)
 CreateMagicTank("m.group.ma13.00","m.unit.ma13.00","ma13.g00","Route/A13/g00.rt","ma13.p00","Route/A13/p00.rt");

 // Enemies (Kingdom)
 CreateBallSenderFirst("Kingdom","k.group.ma13.bs00","k.unit.ma13.bs00","ma13.ebs00","Route/A13/ebs00.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma13.bs01","k.unit.ma13.bs01","ma13.ebs01","Route/A13/ebs01.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma13.bs02","k.unit.ma13.bs02","ma13.ebs02","Route/A13/ebs02.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma13.bs03","k.unit.ma13.bs03","ma13.ebs03","Route/A13/ebs03.rt");
 CreateKingYoungTank("k.group.ma13.00","k.unit.ma13.00","ma13.eg00","Route/A13/eg00.rt","ma13.ep00","Route/A13/ep00.rt");
 CreateKingYoungTank("k.group.ma13.01","k.unit.ma13.01","ma13.eg01","Route/A13/eg01.rt","ma13.ep01","Route/A13/ep01.rt");
 CreateKingYoungTank("k.group.ma13.02","k.unit.ma13.02","ma13.eg02","Route/A13/eg02.rt","ma13.ep02","Route/A13/ep02.rt");
 CreateKingYoungTank("k.group.ma13.03","k.unit.ma13.03","ma13.eg03","Route/A13/eg03.rt","ma13.ep03","Route/A13/ep03.rt");
 CreateKingKnight("k.group.ma13.mn00","k.unit.ma13.mn00","ma13.emn00","Route/A13/emn00.rt",0);
 CreateKingKnight("k.group.ma13.mn01","k.unit.ma13.mn01","ma13.emn01","Route/A13/emn01.rt",0);
 CreateKingKnight("k.group.ma13.mn02","k.unit.ma13.mn02","ma13.emn02","Route/A13/emn02.rt",0);
 CreateKingKnight("k.group.ma13.mn03","k.unit.ma13.mn03","ma13.emn03","Route/A13/emn03.rt",0);
 CreateKingKnight("k.group.ma13.mn04","k.unit.ma13.mn04","ma13.emn04","Route/A13/emn04.rt",0);
 CreateKingKnight("k.group.ma13.mn05","k.unit.ma13.mn05","ma13.emn05","Route/A13/emn05.rt",0);
}

func void SetTaxiA13()
 var int nTaxiCTID;
{nTaxiCTID:=s_SearchSeanceClassTable("Taxi");
 CreateTaxi(nTaxiCTID,[3125.899,2776.687,52.613],"Taxi.Attr.anl_01");
 CreateTaxi(nTaxiCTID,[2175.496,2114.195,67.074],"Taxi.Attr.anl_00");
 CreateTaxi(nTaxiCTID,[2180.992,2114.195,67.074],"Taxi.Attr.anl_00");
 CreateTaxi(nTaxiCTID,[2236.547,2101.837,68.137],"Taxi.Attr.anl_02");
}

func void main()
{SetTaxiA13();
 CreateGroupsA13();
}
// --------------------------- End of Mission A13 ---------------------------
