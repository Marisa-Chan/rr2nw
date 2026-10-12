func void main_LoadRoute()
 var int classTableID;
{
    classTableID := s_AddClassTable("Route",100);

    s_LoadRoute( classTableID, "Route\plane.rt"   ,"Route.plane"   );
    s_LoadRoute( classTableID, "Route\car1.rt"    ,"Route.car1"    );
    s_LoadRoute( classTableID, "Route\dirizabl.rt","Route.dirizabl");
    s_LoadRoute( classTableID, "Route\sub1.rt"    ,"Route.sub1"    );
    s_LoadRoute( classTableID, "Route\car2.rt"    ,"Route.car2"    );

    s_LoadRoute( classTableID, "Route\man1.rt","Route.man1"        );

    s_LoadRoute( classTableID, "Route\man2.rt","Route.man2"        );
    s_LoadRoute( classTableID, "Route\man3.rt","Route.man3"        );
    s_LoadRoute( classTableID, "Route\man4.rt","Route.man4"        );

    s_LoadRoute( classTableID, "Route\boat1.rt","Route.boat1"      );
    s_LoadRoute( classTableID, "Route\boat2.rt","Route.boat2"      );

}


