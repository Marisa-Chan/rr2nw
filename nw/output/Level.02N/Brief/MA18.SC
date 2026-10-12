include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"
include "scinc/units.sci"

// -------------------------- Start of Mission A18 --------------------------
func void CreateGroupsA18()
{
 // Enemies (Magician)
 CreateMortarFirst("Magician","m.group.ma18.mr00","m.unit.ma18.mr00","ma18.emr00","Route/A18/emr00.rt");
 CreateMortarFirst("Magician","m.group.ma18.mr01","m.unit.ma18.mr01","ma18.emr01","Route/A18/emr01.rt");
 CreateMortarFirst("Magician","m.group.ma18.mr02","m.unit.ma18.mr02","ma18.emr02","Route/A18/emr02.rt");
 CreateMortarFirst("Magician","m.group.ma18.mr03","m.unit.ma18.mr03","ma18.emr03","Route/A18/emr03.rt");
 CreateMortarFirst("Magician","m.group.ma18.mr04","m.unit.ma18.mr04","ma18.emr04","Route/A18/emr04.rt");
 CreateMortarFirst("Magician","m.group.ma18.mr05","m.unit.ma18.mr05","ma18.emr05","Route/A18/emr05.rt");
 CreateMortarFirst("Magician","m.group.ma18.mr06","m.unit.ma18.mr06","ma18.emr06","Route/A18/emr06.rt");
 CreateMortarFirst("Magician","m.group.ma18.mr07","m.unit.ma18.mr07","ma18.emr07","Route/A18/emr07.rt");
 CreateMagicTank("m.group.ma18.00","m.unit.ma18.00","ma18.eg00","Route/A18/eg00.rt","ma18.ep00","Route/A18/ep00.rt");
 CreateMagicTank("m.group.ma18.01","m.unit.ma18.01","ma18.eg01","Route/A18/eg01.rt","ma18.ep01","Route/A18/ep01.rt");
 CreateMagicTank("m.group.ma18.02","m.unit.ma18.02","ma18.eg02","Route/A18/eg02.rt","ma18.ep02","Route/A18/ep02.rt");
 CreateMagicAirplane("m.group.ma18.ap00","m.unit.ma18.ap00","ma18.eap00","Route/A18/eap00.rt",535);
 CreateMagicAirplane("m.group.ma18.ap01","m.unit.ma18.ap01","ma18.eap01","Route/A18/eap01.rt",655);
 CreateMagicAirplane("m.group.ma18.ap02","m.unit.ma18.ap02","ma18.eap02","Route/A18/eap02.rt",655);
 CreateMagicKnight("m.group.ma18.emn00","m.unit.ma18.emn00","ma18.emn00","Route/a18/emn00.rt",0);
 CreateMagicKnight("m.group.ma18.emn01","m.unit.ma18.emn01","ma18.emn01","Route/a18/emn01.rt",0);
 CreateMagicKnight("m.group.ma18.emn02","m.unit.ma18.emn02","ma18.emn02","Route/a18/emn02.rt",0);
 CreateMagicKnight("m.group.ma18.emn03","m.unit.ma18.emn03","ma18.emn03","Route/a18/emn03.rt",0);
 CreateMagicKnight("m.group.ma18.emn04","m.unit.ma18.emn04","ma18.emn04","Route/a18/emn04.rt",0);
 CreateMagicKnight("m.group.ma18.emn05","m.unit.ma18.emn05","ma18.emn05","Route/a18/emn05.rt",0);
 CreateMagicKnight("m.group.ma18.emn06","m.unit.ma18.emn06","ma18.emn06","Route/a18/emn06.rt",0);
 CreateMagicKnight("m.group.ma18.emn07","m.unit.ma18.emn07","ma18.emn07","Route/a18/emn07.rt",0);

 // Confederates (Kingdom)
 CreateKingTank("k.group.ma18.00","k.unit.ma18.00","ma18.g00","Route/A18/g00.rt","ma18.p00","Route/A18/p00.rt");
 CreateKingTank("k.group.ma18.01","k.unit.ma18.01","ma18.g01","Route/A18/g01.rt","ma18.p01","Route/A18/p01.rt");
 CreateKingBaggage("k.group.ma18.02","k.unit.ma18.02","ma18.g02","Route/A18/g02.rt","ma18.p02","Route/A18/p02.rt");
 CreateKingTank("k.group.ma18.03","k.unit.ma18.03","ma18.g03","Route/A18/g03.rt","ma18.p03","Route/A18/p03.rt");
 CreateKingTank("k.group.ma18.04","k.unit.ma18.04","ma18.g04","Route/A18/g04.rt","ma18.p04","Route/A18/p04.rt");
 CreateBallSenderSecond("Kingdom","k.group.ma18.bs00","k.unit.ma18.bs00","ma18.bs00","Route/A18/bs00.rt");
 CreateBallSenderSecond("Kingdom","k.group.ma18.bs01","k.unit.ma18.bs01","ma18.bs01","Route/A18/bs01.rt");
 CreateBallSenderSecond("Kingdom","k.group.ma18.bs02","k.unit.ma18.bs02","ma18.bs02","Route/A18/bs02.rt");
 CreateBallSenderSecond("Kingdom","k.group.ma18.bs03","k.unit.ma18.bs03","ma18.bs03","Route/A18/bs03.rt");
 CreateBallSenderSecond("Kingdom","k.group.ma18.bs04","k.unit.ma18.bs04","ma18.bs04","Route/A18/bs04.rt");
}

func void SetTaxiA18()
 var int nTaxiCTID;
{nTaxiCTID:=s_SearchSeanceClassTable("Taxi");
 CreateTaxi(nTaxiCTID,[2666.521,2774.499,52.001],"Taxi.Attr.anl_00");
 CreateTaxi(nTaxiCTID,[3858.688,2084.319,69.631],"Taxi.Attr.anl_01");
 CreateTaxi(nTaxiCTID,[4140.261,2522.806,52.001],"Taxi.Attr.anl_01");
 CreateTaxi(nTaxiCTID,[3860.513,2522.212,52.000],"Taxi.Attr.anl_01");
 CreateTaxi(nTaxiCTID,[3677.860,2503.101,59.027],"Taxi.Attr.vhl_hel");
 CreateTaxi(nTaxiCTID,[2464.249,2420.067,64.534],"Taxi.Attr.anl_02");
}

func void main()
{SetTaxiA18();
 CreateGroupsA18();
}
// --------------------------- End of Mission A18 ---------------------------
