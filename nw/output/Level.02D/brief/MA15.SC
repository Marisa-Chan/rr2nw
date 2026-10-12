include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"
include "scinc/units.sci"

// -------------------------- Start of Mission A15 --------------------------
func void CreateGroupsA15()
{
 // Enemies (Magician)
 CreateMortarFirst("Magician","m.group.ma15.mr00","m.unit.ma15.mr00","ma15.emr00","Routes/A15/emr00.rt");
 CreateMortarFirst("Magician","m.group.ma15.mr01","m.unit.ma15.mr01","ma15.emr01","Routes/A15/emr01.rt");
 CreateMortarFirst("Magician","m.group.ma15.mr02","m.unit.ma15.mr02","ma15.emr02","Routes/A15/emr02.rt");
 CreateMortarFirst("Magician","m.group.ma15.mr03","m.unit.ma15.mr03","ma15.emr03","Routes/A15/emr03.rt");
 CreateMagicTank("m.group.ma15.00","m.unit.ma15.00","ma15.eg00","Routes/A15/eg00.rt","ma15.ep00","Routes/A15/ep00.rt");
 CreateMagicTank("m.group.ma15.01","m.unit.ma15.01","ma15.eg01","Routes/A15/eg01.rt","ma15.ep01","Routes/A15/ep01.rt");
 CreateMagicKnight("m.group.ma15.mn00","m.unit.ma15.mn00","ma15.emn00","Routes/A15/emn00.rt",0);
 CreateMagicKnight("m.group.ma15.mn01","m.unit.ma15.mn01","ma15.emn01","Routes/A15/emn01.rt",0);
 CreateMagicKnight("m.group.ma15.mn02","m.unit.ma15.mn02","ma15.emn02","Routes/A15/emn02.rt",0);
 CreateMagicKnight("m.group.ma15.mn03","m.unit.ma15.mn03","ma15.emn03","Routes/A15/emn03.rt",0);
 CreateMagicKnight("m.group.ma15.mn04","m.unit.ma15.mn04","ma15.emn04","Routes/A15/emn04.rt",0);
 CreateMagicKnight("m.group.ma15.mn05","m.unit.ma15.mn05","ma15.emn05","Routes/A15/emn05.rt",0);
 CreateMagicKnight("m.group.ma15.mn06","m.unit.ma15.mn06","ma15.emn06","Routes/A15/emn06.rt",0);

 // Confederates (Kingdom)
 CreateKingTank("k.group.ma15.00","k.unit.ma15.00","ma15.g00","Routes/A15/g00.rt","ma15.p00","Routes/A15/p00.rt");
 CreateKingTank("k.group.ma15.01","k.unit.ma15.01","ma15.g01","Routes/A15/g01.rt","ma15.p01","Routes/A15/p01.rt");
 CreateKingTank("k.group.ma15.02","k.unit.ma15.02","ma15.g02","Routes/A15/g02.rt","ma15.p02","Routes/A15/p02.rt");
}

func void SetTaxiA15()
 var int nTaxiCTID;
{nTaxiCTID:=s_SearchSeanceClassTable("Taxi");
 CreateTaxi(nTaxiCTID,[2666.521,2774.499,52.001],"Taxi.Attr.anl_00");
 CreateTaxi(nTaxiCTID,[3730.664,2359.340,64.500],"Taxi.Attr.anl_01");
}

func void main()
{SetTaxiA15();
 CreateGroupsA15();
}
// --------------------------- End of Mission A15 ---------------------------
