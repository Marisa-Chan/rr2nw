func void main_CreatePeoples()
 var int ctID;
 var int mctID;
{
    ctID := s_AddClassTable("People",30);
    mctID:=s_SearchSeanceClassTable("Route");

    //==============================================
    s_LoadRoute( mctID, "Route/bowman.rt"   ,"Route.bowman"   );
    CreateManName( ctID, "Peop.Attr.Bowman","Route.bowman", 10, "ship.m2g070.e" );
    s_SetCommander( "ship.m2g070.e", "Kingdom");
    //==============================================

    //CreateManName( ctID, "Peop.Attr.cln_f01", "Route.plane", 0, "Peop.Auto.pl0" );
    //s_SetComander("Peop.Auto.pl0","Colony");


    //CreateMan(ctID,"Peop.Attr.cln_f08", "Route.car1", 0 );
}
