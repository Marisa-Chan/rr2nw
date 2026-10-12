func void main_CreateSmoker()
 var int attrID, attrCP, ctID;
{
     ctID := s_AddClassTable( "Smoker",50 );

    s_SearchObjectID(attrID,attrCP,"Smoker.Attr.Huge");
    s_AttachSmoker( ctID, "htk_gas", attrID, attrCP, 
                          "Smoke.Attach", [0,0,0]);
}
