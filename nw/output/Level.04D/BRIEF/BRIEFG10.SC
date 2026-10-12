include "../defines.sci"
include "../sys.sci"
include "../sysf.sci"


func void CreateGroups() // own
 var int groupCTID, oID, cachePos;
 var str groupName, comName;
 var int ctID, mctID;
{
    ctID := s_SearchSeanceClassTable("Route");
    mctID:= s_SearchSeanceClassTable("People");

    s_LoadRoute( ctID, "Route\m11_e_p0.rt"   ,"Route.m11.e.p0"   );
    CreateManName( mctID, "Peop.Attr.shiz_f0a","Route.m11.e.p0"   , 0, "shiz.m10e0" );

    s_LoadRoute( ctID, "Route\m11_e_p1.rt"   ,"Route.m11.e.p1"   );
    CreateManName( mctID, "Peop.Attr.shiz_f0a","Route.m11.e.p1"   , 0, "shiz.m10e1" );


    groupName := "A.Group.10g02";
    comName   := "Actek";

    CreateGroup(groupName,comName);
    CreateUnit("A.Unit.m10g2",groupName,"TankChAttr");
    AddRouteToMember( comName, groupName, "m11.route.e.g2",  "Route/m11_e_g2.rt", 
                       0,1);
    AddRouteToMember( comName, groupName, "m11.route.e.p2",  "Route/m11_e_p2.rt", 
                       1,0);

}

func void SetTaxi()
 var int ctID;
{
   ctID := s_SearchSeanceClassTable("Taxi");
   CreateTaxi(ctID,[2762, 2602, 76.25 ],"Taxi.Attr.cln_f01");
//   CreateTaxiName(ctID,[2076,3166,67.5 ],"Taxi.Attr.cln_f04", "TaxiHell");
}


func void main()
{
   SetTaxi();
   CreateGroups();
}
