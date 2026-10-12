include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"
include "scinc/units.sci"

// -------------------------- Start of Mission A14 --------------------------
func void CreateGroupsA14()
{
 // Enemies (Magician)
 CreateMortarFirst("Magician","m.group.ma14.mr00","m.unit.ma14.mr00","ma14.emr00","Route/A14/emr00.rt");
 CreateMortarFirst("Magician","m.group.ma14.mr01","m.unit.ma14.mr01","ma14.emr01","Route/A14/emr01.rt");
 CreateMortarFirst("Magician","m.group.ma14.mr02","m.unit.ma14.mr02","ma14.emr02","Route/A14/emr02.rt");
 CreateMortarFirst("Magician","m.group.ma14.mr13","m.unit.ma14.mr13","ma14.emr13","Route/A14/emr13.rt");
 CreateMortarFirst("Magician","m.group.ma14.mr14","m.unit.ma14.mr14","ma14.emr14","Route/A14/emr14.rt");
 CreateMortarFirst("Magician","m.group.ma14.mr15","m.unit.ma14.mr15","ma14.emr15","Route/A14/emr15.rt");
 CreateBallSenderSecond("Magician","m.group.ma14.bs00","m.unit.ma14.bs00","ma14.ebs00","Route/A14/ebs00.rt");
 CreateMagicTank("m.group.ma14.00","m.unit.ma14.00","ma14.eg00","Route/A14/eg00.rt","ma14.ep00","Route/A14/ep00.rt");
 CreateMagicTank("m.group.ma14.01","m.unit.ma14.01","ma14.eg01","Route/A14/eg01.rt","ma14.ep01","Route/A14/ep01.rt");
 CreateMagicKnight("m.group.ma14.mn00","m.unit.ma14.mn00","ma14.emn00","Route/A14/emn00.rt",0);
 CreateMagicKnight("m.group.ma14.mn01","m.unit.ma14.mn01","ma14.emn01","Route/A14/emn01.rt",0);
 CreateMagicKnight("m.group.ma14.mn02","m.unit.ma14.mn02","ma14.emn02","Route/A14/emn02.rt",0);
 CreateMagicKnight("m.group.ma14.mn03","m.unit.ma14.mn03","ma14.emn03","Route/A14/emn03.rt",0);
 CreateMagicKnight("m.group.ma14.mn04","m.unit.ma14.mn04","ma14.emn04","Route/A14/emn04.rt",0);
 CreateMagicKnight("m.group.ma14.mn05","m.unit.ma14.mn05","ma14.emn05","Route/A14/emn05.rt",0);
 CreateMagicKnight("m.group.ma14.mn06","m.unit.ma14.mn06","ma14.emn06","Route/A14/emn06.rt",0);
 CreateMagicKnight("m.group.ma14.mn10","m.unit.ma14.mn10","ma14.emn10","Route/A14/emn10.rt",0);
 CreateMagicKnight("m.group.ma14.mn11","m.unit.ma14.mn11","ma14.emn11","Route/A14/emn11.rt",0);
 CreateMagicKnight("m.group.ma14.mn12","m.unit.ma14.mn12","ma14.emn12","Route/A14/emn12.rt",0);
 CreateMagicAirplane("m.group.ma14.ap00","m.unit.ma14.ap00","ma14.eap00","Route/A14/eap00.rt",450);

 // Confederates (Kingdom)
 CreateKingTank("k.group.ma14.00","k.unit.ma14.00","ma14.g00","Route/A14/g00.rt","ma14.p00","Route/A14/p00.rt");
 CreateKingTank("k.group.ma14.01","k.unit.ma14.01","ma14.g01","Route/A14/g01.rt","ma14.p01","Route/A14/p01.rt");
 CreateKingTank("k.group.ma14.02","k.unit.ma14.02","ma14.g02","Route/A14/g02.rt","ma14.p02","Route/A14/p02.rt");
 CreateKingAirplane("k.group.ma14.ap00","k.unit.ma14.ap00","ma14.ap00","Route/A14/ap00.rt",400);
}

func void SetTaxiA14()
 var int nTaxiCTID;
{nTaxiCTID:=s_SearchSeanceClassTable("Taxi");
 CreateTaxi(nTaxiCTID,[2666.521,2774.499,52.001],"Taxi.Attr.anl_00");
 CreateTaxi(nTaxiCTID,[4610.986,2726.000,89.527],"Taxi.Attr.vhl_hel");
 CreateTaxi(nTaxiCTID,[4661.330,2726.000,89.527],"Taxi.Attr.vhl_hel");
}

func void main()
{SetTaxiA14();
 CreateGroupsA14();
}
// --------------------------- End of Mission A14 ---------------------------
