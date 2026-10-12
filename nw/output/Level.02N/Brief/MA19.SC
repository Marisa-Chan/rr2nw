include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"
include "scinc/units.sci"

// -------------------------- Start of Mission A19 --------------------------
func void CreateGroupsA19()
{
 // Confederates (Magician)
 CreateMagicTank("m.group.ma19.00","m.unit.ma19.00","ma19.g00","Route/A19/g00.rt","ma19.p00","Route/A19/p00.rt");

 // Enemies (Kingdom)
 CreateKingTank("k.group.ma19.00","k.unit.ma19.00","ma19.eg00","Route/A19/eg00.rt","ma19.ep00","Route/A19/ep00.rt");
 CreateKingTank("k.group.ma19.01","k.unit.ma19.01","ma19.eg01","Route/A19/eg01.rt","ma19.ep01","Route/A19/ep01.rt");
 CreateKingBaggage("k.group.ma19.02","k.unit.ma19.02","ma19.eg02","Route/A19/eg02.rt","ma19.ep02","Route/A19/ep02.rt");
 CreateKingTank("k.group.ma19.03","k.unit.ma19.03","ma19.eg03","Route/A19/eg03.rt","ma19.ep03","Route/A19/ep03.rt");
 CreateKingTank("k.group.ma19.04","k.unit.ma19.04","ma19.eg04","Route/A19/eg04.rt","ma19.ep04","Route/A19/ep04.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma19.bs00","k.unit.ma19.bs00","ma19.ebs00","Route/A19/ebs00.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma19.bs01","k.unit.ma19.bs01","ma19.ebs01","Route/A19/ebs01.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma19.bs02","k.unit.ma19.bs02","ma19.ebs02","Route/A19/ebs02.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma19.bs03","k.unit.ma19.bs03","ma19.ebs03","Route/A19/ebs03.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma19.bs04","k.unit.ma19.bs04","ma19.ebs04","Route/A19/ebs04.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma19.bs05","k.unit.ma19.bs05","ma19.ebs05","Route/A19/ebs05.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma19.bs06","k.unit.ma19.bs06","ma19.ebs06","Route/A19/ebs06.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma19.bs07","k.unit.ma19.bs07","ma19.ebs07","Route/A19/ebs07.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma19.bs08","k.unit.ma19.bs08","ma19.ebs08","Route/A19/ebs08.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma19.bs09","k.unit.ma19.bs09","ma19.ebs09","Route/A19/ebs09.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma19.bs10","k.unit.ma19.bs10","ma19.ebs10","Route/A19/ebs10.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma19.bs11","k.unit.ma19.bs11","ma19.ebs11","Route/A19/ebs11.rt");
 CreateBallSenderFirst("Kingdom","k.group.ma19.bs12","k.unit.ma19.bs12","ma19.ebs12","Route/A19/ebs12.rt");
 CreateMortarFirst("Kingdom","k.group.ma19.mr00","k.unit.ma19.mr00","ma19.emr00","Route/A19/emr00.rt");
 CreateMortarFirst("Kingdom","k.group.ma19.mr01","k.unit.ma19.mr01","ma19.emr01","Route/A19/emr01.rt");
 CreateMortarFirst("Kingdom","k.group.ma19.mr02","k.unit.ma19.mr02","ma19.emr02","Route/A19/emr02.rt");
 CreateMortarFirst("Kingdom","k.group.ma19.mr03","k.unit.ma19.mr03","ma19.emr03","Route/A19/emr03.rt");
 CreateMortarFirst("Kingdom","k.group.ma19.mr04","k.unit.ma19.mr04","ma19.emr04","Route/A19/emr04.rt");
 CreateMortarFirst("Kingdom","k.group.ma19.mr05","k.unit.ma19.mr05","ma19.emr05","Route/A19/emr05.rt");
 CreateMortarFirst("Kingdom","k.group.ma19.mr06","k.unit.ma19.mr06","ma19.emr06","Route/A19/emr06.rt");
 CreateMortarFirst("Kingdom","k.group.ma19.mr07","k.unit.ma19.mr07","ma19.emr07","Route/A19/emr07.rt");
 CreateMortarFirst("Kingdom","k.group.ma19.mr08","k.unit.ma19.mr08","ma19.emr08","Route/A19/emr08.rt");
 CreateMortarFirst("Kingdom","k.group.ma19.mr09","k.unit.ma19.mr09","ma19.emr09","Route/A19/emr09.rt");
 CreateMortarFirst("Kingdom","k.group.ma19.mr10","k.unit.ma19.mr10","ma19.emr10","Route/A19/emr10.rt");
 CreateKingKnight("k.group.ma19.mn00","k.unit.ma19.mn00","ma19.emn00","Route/A19/emn00.rt",0);
 CreateKingKnight("k.group.ma19.mn01","k.unit.ma19.mn01","ma19.emn01","Route/A19/emn01.rt",0);
 CreateKingKnight("k.group.ma19.mn02","k.unit.ma19.mn02","ma19.emn02","Route/A19/emn02.rt",0);
}

func void SetTaxiA19()
 var int nTaxiCTID;
{nTaxiCTID:=s_SearchSeanceClassTable("Taxi");
 CreateTaxi(nTaxiCTID,[3125.899,2776.687,52.613],"Taxi.Attr.anl_01");
 CreateTaxi(nTaxiCTID,[2219.521,2414.066,67.001],"Taxi.Attr.anl_00");
 CreateTaxi(nTaxiCTID,[1631.081,3169.347,40.811],"Taxi.Attr.anl_00");
}

func void main()
{SetTaxiA19();
 CreateGroupsA19();
}
// --------------------------- End of Mission A19 ---------------------------
