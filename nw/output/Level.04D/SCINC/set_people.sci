func void main_CreatePeoples()
 var int ctID;
{
    ctID := s_AddClassTable("People",30);


    CreateManName( ctID, "Peop.Attr.cln_f01", "Route.plane", 0, "Peop.Auto.pl0" );
    CreateManName( ctID, "Peop.Attr.cln_f01", "Route.plane", 6, "Peop.Auto.pl1" );

    s_SetCommander("Peop.Auto.pl0","Colony");
    s_SetCommander("Peop.Auto.pl1","Colony");


    CreateMan( ctID, "Peop.Attr.cln_f08", "Route.car1", 0 );
    CreateMan( ctID, "Peop.Attr.cln_f08", "Route.car1", 8 );
    CreateMan( ctID, "Peop.Attr.cln_f08", "Route.car1", 19 );

    CreateMan( ctID, "Peop.Attr.shiz_f0a","Route.dirizabl", 0 );
    CreateMan( ctID, "Peop.Attr.shiz_f0a","Route.dirizabl", 10 );
    CreateMan( ctID, "Peop.Attr.shiz_f0a","Route.dirizabl", 30 );

    CreateMan( ctID, "Peop.Attr.cln_smn2","Route.sub1"    , 0 );
    CreateMan( ctID, "Peop.Attr.cln_smn2","Route.sub1"    , 10 );
    CreateMan( ctID, "Peop.Attr.cln_smn2","Route.sub1"    , 20 );

    CreateMan( ctID, "Peop.Attr.lorr_f0b","Route.car2"    , 0 );
    CreateMan( ctID, "Peop.Attr.lorr_f0b","Route.car2"    , 7 );


    CreateMan( ctID, "Peop.Attr.man_wmn", "Route.man1", 0 );
    CreateMan( ctID, "Peop.Attr.man_wmn", "Route.man1", 10 );

    CreateMan( ctID, "Peop.Attr.man_wmn", "Route.man2", 0 );
    CreateMan( ctID, "Peop.Attr.man_wmn", "Route.man4", 0 );

    CreateMan( ctID, "Peop.Attr.man_01", "Route.man3"    , 0 );
    CreateMan( ctID, "Peop.Attr.cln_S00","Route.boat1"   , 0 );
    CreateMan( ctID, "Peop.Attr.cln_S00","Route.boat1"   , 30 );

}
