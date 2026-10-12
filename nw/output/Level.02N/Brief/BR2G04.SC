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

    s_LoadRoute( ctID, "Route\m2g04p0.rt"   ,"Route.m2g04.p0"   );
    CreateManName( mctID, "Peop.Attr.ship_stay","Route.m2g04.p0"   , 0, "ship.m2g040" );
    s_SetCommander("ship.m2g040", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g04p1.rt"   ,"Route.m2g04.p1"   );
    CreateManName( mctID, "Peop.Attr.ship_stay","Route.m2g04.p1"   , 0, "ship.m2g041" );
    s_SetCommander("ship.m2g041", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g04p2.rt"   ,"Route.m2g04.p2"   );
    CreateManName( mctID, "Peop.Attr.vhl_ship","Route.m2g04.p2"   , 0, "ship.m2g042" );
    s_SetCommander("ship.m2g042", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g04p3.rt"   ,"Route.m2g04.p3"   );
    CreateManName( mctID, "Peop.Attr.vhl_ship","Route.m2g04.p3"   , 0, "ship.m2g043" );
    s_SetCommander("ship.m2g043", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g04p4.rt"   ,"Route.m2g04.p4"   );
    CreateManName( mctID, "Peop.Attr.vhl_ship","Route.m2g04.p4"   , 0, "ship.m2g044" );
    s_SetCommander("ship.m2g044", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g04p7.rt"   ,"Route.m2g04.p7"   );
    CreateManName( mctID, "Peop.Attr.ship_stay","Route.m2g04.p7"   , 0, "ship.m2g047" );
    s_SetCommander("ship.m2g047", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g04p8.rt"   ,"Route.m2g04.p8"   );
    CreateManName( mctID, "Peop.Attr.ship_stay","Route.m2g04.p8"   , 0, "ship.m2g048" );
    s_SetCommander("ship.m2g048", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g04p5.rt"   ,"Route.m2g04.p5"   );
    CreateManName( mctID, "Peop.Attr.gal.king","Route.m2g04.p5"   , 0, "gal.m2g045" );
    s_SetCommander("gal.m2g045", "Kingdom");

    s_LoadRoute( ctID, "Route\m2g04p9.rt"   ,"Route.m2g04.p9"   );
    CreateManName( mctID, "Peop.Attr.anl_02","Route.m2g04.p9"   , 80, "ship.m2g049" );
    s_SetCommander("ship.m2g049", "Kingdom");


    s_LoadRoute( ctID, "Route\m2g04ep0.rt"   ,"Route.m2g04.e.p0"   );
    CreateManName( mctID, "Peop.Attr.vhl_hel","Route.m2g04.e.p0"   , 85, "hel.m2g04e0" );
    s_SetCommander("hel.m2g04e0", "Magician");

    s_LoadRoute( ctID, "Route\m2g04ep1.rt"   ,"Route.m2g04.e.p1"   );
    CreateManName( mctID, "Peop.Attr.vhl_hel","Route.m2g04.e.p1"   , 85, "hel.m2g04e1" );
    s_SetCommander("hel.m2g04e1", "Magician");

    s_LoadRoute( ctID, "Route\m2g04ep2.rt"   ,"Route.m2g04.e.p2"   );
    CreateManName( mctID, "Peop.Attr.vhl_hel","Route.m2g04.e.p2"   , 95, "hel.m2g04e2" );
    s_SetCommander("hel.m2g04e2", "Magician");

    s_LoadRoute( ctID, "Route\m2g04ep3.rt"   ,"Route.m2g04.e.p3"   );
    CreateManName( mctID, "Peop.Attr.vhl_hel","Route.m2g04.e.p3"   , 95, "hel.m2g04e3" );
    s_SetCommander("hel.m2g04e3", "Magician");

    s_LoadRoute( ctID, "Route\m2g04ep4.rt"   ,"Route.m2g04.e.p4"   );
    CreateManName( mctID, "Peop.Attr.vhl_hel","Route.m2g04.e.p4"   , 95, "hel.m2g04e4" );
    s_SetCommander("hel.m2g04e4", "Magician");

    s_LoadRoute( ctID, "Route\m2g04ep5.rt"   ,"Route.m2g04.e.p5"   );
    CreateManName( mctID, "Peop.Attr.vhl_hel","Route.m2g04.e.p5"   , 95, "hel.m2g04e5" );
    s_SetCommander("hel.m2g04e5", "Magician");

}

func void SetTaxi()
 var int ctID;
{
   ctID := s_SearchSeanceClassTable("Taxi");
   CreateTaxi(ctID,[2594.958, 2790.316, 52.000],"Taxi.Attr.anl_02");
}


func void main()
{
   SetTaxi();
   CreateGroups();
}
