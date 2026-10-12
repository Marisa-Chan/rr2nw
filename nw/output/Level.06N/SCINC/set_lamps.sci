
func void main_CreateLamps()
 var int ctID, attrID, attrCP;
{
    ctID := s_AddClassTable( "Lamp",250 );

    CreateLamp(ctID,"Lamp.Attr.White.Blink", [1760, 280, -2860]);
    CreateLamp(ctID,"Lamp.Attr.White.Blink", [3970, 297.5, -1380] );
    CreateLamp(ctID,"Lamp.Attr.Fading", [1010, 282.5, -1740] );
    CreateLamp(ctID,"Lamp.Attr.Fading", [4320, 277.5, -2260] );




//    CreateMovingLamp(ctID, "Lamp.Attr.Moving", [1460,159.5,-2350.45],  [1460,159.5,-2450], 12, 0);
//    CreateMovingLamp(ctID, "Lamp.Attr.Moving", [1460,159.5,-2350.45],  [1460,159.5,-2450], 12, 6);

CreateMovingLamp(ctID, "Lamp.Attr.Moving", [2610, 170.3, -3850], [2240, 170.3, -3850], 24, 0);
CreateMovingLamp(ctID, "Lamp.Attr.Moving", [2610, 170.3, -3850], [2240, 170.3, -3850], 24, 12);

CreateMovingLamp(ctID, "Lamp.Attr.Moving", [2610, 170.3, -3840], [2240, 170.3, -3840], 24, 0);
CreateMovingLamp(ctID, "Lamp.Attr.Moving", [2610, 170.3, -3840], [2240, 170.3, -3840], 24, 12);

CreateMovingLamp(ctID, "Lamp.Attr.Moving", [2460, 159, -1440], [2310, 159, -1440], 24, 0);
CreateMovingLamp(ctID, "Lamp.Attr.Moving", [2460, 159, -1440], [2310, 159, -1440], 24, 12);

CreateMovingLamp(ctID, "Lamp.Attr.Moving", [2460, 159, -1430], [2310, 159, -1430], 24, 0);
CreateMovingLamp(ctID, "Lamp.Attr.Moving", [2460, 159, -1430], [2310, 159, -1430], 24, 12);


CreateMovingLamp(ctID, "Lamp.Attr.Moving", [1313, 156.5, -2465.3], [1447,156.5, -2465.3], 12, 0);
CreateMovingLamp(ctID, "Lamp.Attr.Moving", [1455.5, 156.5, -2447.5], [1455.5, 156.5, -2352.5], 12, 0);
CreateMovingLamp(ctID, "Lamp.Attr.Moving", [1447, 156.5, -2334.5] , [1313, 156.5, -2334.5], 12, 0);
CreateMovingLamp(ctID, "Lamp.Attr.Moving", [1304.5, 156.5, -2352.5], [1304.5, 156.5, -2447.5], 12, 0);



    s_SearchObjectID(attrID,attrCP,"Lamp.Attr.FdAttach");
    s_AttachObject( ctID, "prt_main", attrID, attrCP, 
                          "Lamp.Attach", [-7,143.3,-0.3], lmp_EV_START);

    s_SearchObjectID(attrID,attrCP,"Lamp.Attr.Fd2Attach");

    s_AttachObject( ctID, "ship_00a", attrID, attrCP, 
                          "Lamp.Attach", [0,5,0], lmp_EV_START);
 
   s_AttachObject( ctID, "str_mtwr", attrID, attrCP, 
                          "Lamp.Attach", [0,15,0], lmp_EV_START);


   s_SearchObjectID(attrID,attrCP,"Lamp.Attr.Md");    

   s_AttachObject( ctID, "str_mtwr", attrID, attrCP, 
                         "Lamp.Attach", [-15.5,57.5,0], lmp_EV_START);
   s_AttachObject( ctID, "ship_00a", attrID, attrCP, 
                         "Lamp.Attach", [0,32,-2.4], lmp_EV_START);

   s_SearchObjectID(attrID,attrCP,"Lamp.Attr.MdBlue");
   s_AttachObject( ctID, "blg_twr", attrID, attrCP, 
                         "Lamp.Attach", [0,50.5,0], lmp_EV_START);
}


