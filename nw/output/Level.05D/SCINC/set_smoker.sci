func void main_CreateSmoker()
 var int attrID, attrCP, ctID;
{
    ctID := s_AddClassTable( "Smoker",150 );

    s_SearchObjectID(attrID,attrCP,"Smoker.Attr.White");
    s_AttachSmoker( ctID, "wtr_b00", attrID, attrCP, 
                          "Smoke.Attach", [3.6,22.7,-4.4]);

    s_SearchObjectID(attrID,attrCP,"Smoker.Attr.Huge");
    s_AttachSmoker( ctID, "wtr_f00", attrID, attrCP, 
                          "Smoke.Attach", [-12,50,-4.3]);

}
