include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"
include "scinc/units.sci"

// -------------------------- Start of Mission A20 --------------------------
func void CreateGroupsA20()
{
 // Confederates (Magician)
 CreateMagicAirplane("m.group.ma20.ap00","m.unit.ma20.ap00","ma20.ap00","Route/A20/ap00.rt",35);
 CreateMagicKnight("m.group.ma20.mn00","m.unit.ma20.mn00","ma20.mn00","Route/A20/mn00.rt",35);
 CreateMagicKnight("m.group.ma20.mn01","m.unit.ma20.mn01","ma20.mn01","Route/A20/mn01.rt",35);

 // Enemies (Kingdom)
 CreateKingTank("k.group.ma20.00","k.unit.ma20.00","ma20.eg00","Route/A20/eg00.rt","ma20.ep00","Route/A20/ep00.rt");
 CreateKingAirplane("k.group.ma20.ap00","k.unit.ma20.ap00","ma20.eap00","Route/A20/eap00.rt",35);
}

func void SetTaxiA20()
 var int nTaxiCTID;
{nTaxiCTID:=s_SearchSeanceClassTable("Taxi");
 CreateTaxi(nTaxiCTID,[3125.899,2776.687,52.613],"Taxi.Attr.anl_01");
 CreateTaxi(nTaxiCTID,[3090.187,2776.607,52.613],"Taxi.Attr.vhl_hel");
}

func void main()
{SetTaxiA20();
 CreateGroupsA20();
}
// --------------------------- End of Mission A20 ---------------------------
