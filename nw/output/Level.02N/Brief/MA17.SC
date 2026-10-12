include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"
include "scinc/units.sci"

// -------------------------- Start of Mission A17 --------------------------
func void CreateGroupsA17()
{
 // Confederates (Magician)
 CreateBallSenderSecond("Magician","m.group.ma17.bs00","m.unit.ma17.bs00","ma17.bs00","Route/A17/bs00.rt");
 CreateBallSenderSecond("Magician","m.group.ma17.bs01","m.unit.ma17.bs01","ma17.bs01","Route/A17/bs01.rt");
 CreateBallSenderSecond("Magician","m.group.ma17.bs02","m.unit.ma17.bs02","ma17.bs02","Route/A17/bs02.rt");
 CreateMagicKnight("m.group.ma17.mn00","m.unit.ma17.mn00","ma17.mn00","Route/A17/mn00.rt",0);
 CreateMagicKnight("m.group.ma17.mn01","m.unit.ma17.mn01","ma17.mn01","Route/A17/mn01.rt",0);
 CreateMagicTank("m.group.ma17.00","m.unit.ma17.00","ma17.g00","Route/A17/g00.rt","ma17.p00","Route/A17/p00.rt");
 CreateMagicAirplane("m.group.ma17.ap00","m.unit.ma17.ap00","ma17.ap00","Route/A17/ap00.rt",0);

 // Enemies (Kingdom)
 CreateKingTank("k.group.ma17.00","k.unit.ma17.00","ma17.eg00","Route/A17/eg00.rt","ma17.ep00","Route/A17/ep00.rt");
 CreateKingTank("k.group.ma17.01","k.unit.ma17.01","ma17.eg01","Route/A17/eg01.rt","ma17.ep01","Route/A17/ep01.rt");
 CreateKingTank("k.group.ma17.02","k.unit.ma17.02","ma17.eg02","Route/A17/eg02.rt","ma17.ep02","Route/A17/ep02.rt");
 CreateKingAirplane("k.group.ma17.ap00","k.unit.ma17.ap00","ma17.eap00","Route/A17/eap00.rt",140);
 CreateKingAirplane("k.group.ma17.ap01","k.unit.ma17.ap01","ma17.eap01","Route/A17/eap01.rt",140);
 CreateKingAirplane("k.group.ma17.ap02","k.unit.ma17.ap02","ma17.eap02","Route/A17/eap02.rt",140);
 CreateKingKnight("k.group.ma17.mn00","k.unit.ma17.mn00","ma17.emn00","Route/A17/emn00.rt",0);
 CreateKingKnight("k.group.ma17.mn01","k.unit.ma17.mn01","ma17.emn01","Route/A17/emn01.rt",0);
 CreateKingKnight("k.group.ma17.mn02","k.unit.ma17.mn02","ma17.emn02","Route/A17/emn02.rt",0);
}

func void SetTaxiA17()
 var int nTaxiCTID;
{nTaxiCTID:=s_SearchSeanceClassTable("Taxi");
 CreateTaxi(nTaxiCTID,[3125.899,2776.687,52.613],"Taxi.Attr.anl_01");
 CreateTaxi(nTaxiCTID,[3885.641,2347.941,70.134],"Taxi.Attr.vhl_hel");
 CreateTaxi(nTaxiCTID,[3937.020,2347.941,70.134],"Taxi.Attr.vhl_hel");
}

func void main()
{SetTaxiA17();
 CreateGroupsA17();
}
// --------------------------- End of Mission A17 ---------------------------
