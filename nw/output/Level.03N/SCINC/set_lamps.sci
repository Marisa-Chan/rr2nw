func void main_CreateLamps()
 var int ctID, attrID, attrCP;
{
   
    ctID := s_AddClassTable( "Lamp",500 );

//	s_SearchObjectID(attrID,attrCP,"Lamp.Attr.Md");
	s_SearchObjectID(attrID,attrCP,"Lamp.Attr.White.Blink");
	s_AttachObject( ctID, "war_c06", attrID, attrCP, 
                          "Lamp.Attach", [-7,200,7], lmp_EV_START);
        s_AttachObject( ctID, "war_c06", attrID, attrCP, 
                          "Lamp.Attach", [7,200,7], lmp_EV_START);
	s_AttachObject( ctID, "war_c06", attrID, attrCP, 
                          "Lamp.Attach", [7,200,-7], lmp_EV_START);
	s_AttachObject( ctID, "war_c06", attrID, attrCP, 
                          "Lamp.Attach", [-7,200,-7], lmp_EV_START);
	s_AttachObject( ctID, "arn_pike", attrID, attrCP, 
                          "Lamp.Attach", [0,59,0], lmp_EV_START);

/*   s_SearchObjectID(attrID,attrCP,"Lamp.Attr.Md");    
   s_AttachObject( ctID, "war_c0b", attrID, attrCP, 
                         "Lamp.Attach", [-4.2,14.55,0], lmp_EV_START);*/
}
