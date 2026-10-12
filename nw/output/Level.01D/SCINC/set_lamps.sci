func void main_CreateLamps()
 var int ctID, attrID, attrCP;
{
    ctID := s_AddClassTable( "Lamp",250 );

    // Огни над городом
    CreateMovingLamp(ctID, "Lamp.Attr.Moving", [1280, 160, -2345], [1280, 160, -2215], 24, 0);
    CreateMovingLamp(ctID, "Lamp.Attr.Moving", [1280, 160, -2215], [1410, 160, -2215], 24, 0);
    CreateMovingLamp(ctID, "Lamp.Attr.Moving", [1410, 160, -2215], [1410, 160, -2345], 24, 0);
    CreateMovingLamp(ctID, "Lamp.Attr.Moving", [1410, 160, -2345], [1280, 160, -2345], 24, 0);

    // Огна вдоль летунского моста
    CreateMovingLamp(ctID, "Lamp.Attr.Moving", [4266, 110, -1090], [4266, 110, -990], 24, 0);
    CreateMovingLamp(ctID, "Lamp.Attr.Moving", [4304, 110, -1090], [4304, 110, -990], 24, 0);


    s_SearchObjectID(attrID,attrCP,"Lamp.Attr.White.Blink");
    s_AttachObject( ctID, "twn_pike", attrID, attrCP, 
                          "Lamp.Attach", [0,100.2,0], lmp_EV_START);

    s_SearchObjectID(attrID,attrCP,"Lamp.Attr.Fd2Attach");
    s_AttachObject( ctID, "ship_00a", attrID, attrCP, 
                          "Lamp.Attach", [0,5,0], lmp_EV_START);
    s_AttachObject( ctID, "twn_jbrg", attrID, attrCP, 
                          "Lamp.Attach", [-11.8,29.1,-35.1], lmp_EV_START);
    s_AttachObject( ctID, "twn_jbrg", attrID, attrCP, 
                          "Lamp.Attach", [-3.6,29.1,-35.1], lmp_EV_START);


   s_SearchObjectID(attrID,attrCP,"Lamp.Attr.Md");    
   s_AttachObject( ctID, "man", attrID, attrCP, 
                         "Lamp.Attach", [-29.85,77,9.64], lmp_EV_START);
   s_AttachObject( ctID, "ship_00a", attrID, attrCP, 
                         "Lamp.Attach", [0,32,-2.4], lmp_EV_START);

}


