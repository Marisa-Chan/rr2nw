include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"
include "scinc/units.sci"

// -------------------------- Start of Mission A16 --------------------------
func void CreateGroupsA16()
{
 // Enemies (Magician)
 CreateMortarFirst("Magician","m.group.ma16.mr00","m.unit.ma16.mr00","ma16.emr00","Routes/A16/emr00.rt");
 CreateMortarFirst("Magician","m.group.ma16.mr01","m.unit.ma16.mr01","ma16.emr01","Routes/A16/emr01.rt");
 CreateMortarFirst("Magician","m.group.ma16.mr02","m.unit.ma16.mr02","ma16.emr02","Routes/A16/emr02.rt");
 CreateMortarFirst("Magician","m.group.ma16.mr03","m.unit.ma16.mr03","ma16.emr03","Routes/A16/emr03.rt");
 CreateBallSenderSecond("Magician","m.group.ma16.bs00","m.unit.ma16.bs00","ma16.ebs00","Routes/A16/ebs00.rt");
 CreateBallSenderSecond("Magician","m.group.ma16.bs01","m.unit.ma16.bs01","ma16.ebs01","Routes/A16/ebs01.rt");
 CreateBallSenderSecond("Magician","m.group.ma16.bs02","m.unit.ma16.bs02","ma16.ebs02","Routes/A16/ebs02.rt");
 CreateMagicKnight("m.group.ma16.mn00","m.unit.ma16.mn00","ma16.emn00","Routes/A16/emn00.rt",0);
 CreateMagicKnight("m.group.ma16.mn01","m.unit.ma16.mn01","ma16.emn01","Routes/A16/emn01.rt",0);
 CreateMagicKnight("m.group.ma16.mn02","m.unit.ma16.mn02","ma16.emn02","Routes/A16/emn02.rt",0);
 CreateMagicKnight("m.group.ma16.mn03","m.unit.ma16.mn03","ma16.emn03","Routes/A16/emn03.rt",0);
 CreateMagicKnight("m.group.ma16.mn04","m.unit.ma16.mn04","ma16.emn04","Routes/A16/emn04.rt",0);
 CreateMagicKnight("m.group.ma16.mn05","m.unit.ma16.mn05","ma16.emn05","Routes/A16/emn05.rt",0);
 CreateMagicKnight("m.group.ma16.mn06","m.unit.ma16.mn06","ma16.emn06","Routes/A16/emn06.rt",0);
 CreateMagicAirplane("m.group.ma16.ap00","m.unit.ma16.ap00","ma16.eap00","Routes/A16/eap00.rt",320);

 // Confederates (Kingdom)
 CreateKingTank("k.group.ma16.00","k.unit.ma16.00","ma16.g00","Routes/A16/g00.rt","ma16.p00","Routes/A16/p00.rt");
 CreateKingTank("k.group.ma16.01","k.unit.ma16.01","ma16.g01","Routes/A16/g01.rt","ma16.p01","Routes/A16/p01.rt");
 CreateKingTank("k.group.ma16.02","k.unit.ma16.02","ma16.g02","Routes/A16/g02.rt","ma16.p02","Routes/A16/p02.rt");
 CreateKingAirplane("k.group.ma16.ap00","k.unit.ma16.ap00","ma16.ap00","Routes/A16/ap00.rt",270);
}

func void SetTaxiA16()
 var int nTaxiCTID;
{nTaxiCTID:=s_SearchSeanceClassTable("Taxi");
 CreateTaxi(nTaxiCTID,[2666.521,2774.499,52.001],"Taxi.Attr.anl_00");
 CreateTaxi(nTaxiCTID,[3885.641,2347.941,70.134],"Taxi.Attr.vhl_hel");
 CreateTaxi(nTaxiCTID,[3937.020,2347.941,70.134],"Taxi.Attr.vhl_hel");
}

func void main()
{SetTaxiA16();
 CreateGroupsA16();
}
// --------------------------- End of Mission A16 ---------------------------
