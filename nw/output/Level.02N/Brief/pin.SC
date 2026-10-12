include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"
include "scinc/units.sci"

// -------------------------- Start of Mission A21 --------------------------
func void CreateGroupsPIN()
var int ctID;
{
 // Enemies (Magician)
 ctID :=s_SearchSeanceClassTable("Howitzer");

 CreateHowitzerName( ctID, "Howitzer.Attr.Default", "HowitzerMagicians", 0, "hwz.mag");
 s_SetCommander("hwz.mag", "Kingdom");
 
 CreateHowitzerName( ctID, "Howitzer.Attr.Default", "HowitzerKingdom1", 0, "hwz.kng1");
 s_SetCommander("hwz.kng1", "Magician");

 CreateHowitzerName( ctID, "Howitzer.Attr.Default", "HowitzerKingdom2", 0, "hwz.kng2");
 s_SetCommander("hwz.kng2", "Magician");

 
}

func void SetTaxiPIN()
 var int nTaxiCTID;
{nTaxiCTID:=s_SearchSeanceClassTable("Taxi");
 CreateTaxi(nTaxiCTID,[2666.521,2774.499,52.001],"Taxi.Attr.anl_00");
 CreateTaxi(nTaxiCTID,[2604.679,2774.499,52.001],"Taxi.Attr.anl_02");
}

func void main()
{SetTaxiPIN();
 CreateGroupsPIN();
}
// --------------------------- End of Mission A21 ---------------------------
