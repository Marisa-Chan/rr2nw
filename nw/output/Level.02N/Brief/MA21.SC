include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"
include "scinc/units.sci"

// -------------------------- Start of Mission A21 --------------------------
func void CreateGroupsA21()
{
 // Enemies (Magician)
 CreateMagicTank("m.group.ma21.00","m.unit.ma21.00","ma21.eg00","Route/A21/eg00.rt","ma21.ep00","Route/A21/ep00.rt");
 CreateMagicTank("m.group.ma21.01","m.unit.ma21.01","ma21.eg01","Route/A21/eg01.rt","ma21.ep01","Route/A21/ep01.rt");
 CreateMagicAirplane("m.group.ma21.ap00","m.unit.ma21.ap00","ma21.eap00","Route/A21/eap00.rt",50.8);

 // Confederates (Kingdom)
 CreateMortarFirst("Kingdom","k.group.ma21.bs00","k.unit.ma21.bs00","ma21.bs00","Route/A21/bs00.rt");
 CreateMortarFirst("Kingdom","k.group.ma21.bs01","k.unit.ma21.bs01","ma21.bs01","Route/A21/bs01.rt");
 CreateKingAirplane("k.group.ma21.ap00","k.unit.ma21.ap00","ma21.ap00","Route/A21/ap00.rt",50.8);
 CreateKingKnight("k.group.ma21.mn00","k.unit.ma21.mn00","ma21.mn00","Route/A21/mn00.rt",0);
 CreateKingKnight("k.group.ma21.mn01","k.unit.ma21.mn01","ma21.mn01","Route/A21/mn01.rt",0);
}

func void SetTaxiA21()
 var int nTaxiCTID;
{nTaxiCTID:=s_SearchSeanceClassTable("Taxi");
 CreateTaxi(nTaxiCTID,[2666.521,2774.499,52.001],"Taxi.Attr.anl_00");
 CreateTaxi(nTaxiCTID,[2604.679,2774.499,52.001],"Taxi.Attr.anl_02");
}

func void main()
{SetTaxiA21();
 CreateGroupsA21();
}
// --------------------------- End of Mission A21 ---------------------------
