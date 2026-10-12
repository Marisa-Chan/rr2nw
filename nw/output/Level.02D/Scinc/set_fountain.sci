func void main_CreateFountain()
 var int attrID, attrCP, ctID;
{
   ctID := s_AddClassTable( "Fountain",50 );

// Siren's Fort
    CreateFountain("Fount.0","Fount.Attr.Water",[989.9,65.25,-1610.8]);
    CreateFountain("Fount.1","Fount.Attr.Water2",[946.0,65.25,-1719]);

// Enlightened Spirit
    CreateFountain("Fount.2","Fount.Attr.Water2",[2310,65.25,-1645]);

// Dead fountain
    CreateFountain("Fount.3","Fount.Attr.Water2",[3891,141.5,-1735]);

// Blood fountains
    CreateFountain("Blood.1","Fount.Attr.Blood2",[3348.0,81.25,-2748]);
    CreateFountain("Blood.2","Fount.Attr.Blood",[2874.0,172.5,-3179]);

    s_SearchObjectID(attrID,attrCP,"Fount.Attr.Water");
    s_AttachObject( ctID, "fnt_drag", attrID, attrCP, 
                          "Fountain.Attach", [0,4,1], fou_EVCMD_START);

}
