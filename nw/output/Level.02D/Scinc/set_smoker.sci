func void main_CreateSmoker()
 var int attrID, attrCP, ctID;
{
   ctID := s_AddClassTable( "Smoker",150 );

    s_SearchObjectID(attrID,attrCP,"Smoker.Attr.FireMd");
    s_AttachSmoker( ctID, "bld_18", attrID, attrCP, 
                          "Smoke.Attach", [0,8.4,0]);

    s_SearchObjectID(attrID,attrCP,"Smoker.Attr.FireMdBlue");
    s_AttachSmoker( ctID, "evl_trch", attrID, attrCP, 
                          "Smoke.Attach", [0,10.4,0]);
    s_AttachSmoker( ctID, "brg_magb", attrID, attrCP, 
                          "Smoke.Attach", [6,10.4,7.5]);
    s_AttachSmoker( ctID, "rec_mag", attrID, attrCP, 
                          "Smoke.Attach", [6.5,11.8,-13.5]);
    s_AttachSmoker( ctID, "rec_mag", attrID, attrCP, 
                          "Smoke.Attach", [-6.7,11.8,-13.5]);

    s_SearchObjectID(attrID,attrCP,"Smoker.Attr.Huge");
    s_AttachSmoker( ctID, "bld_v02", attrID, attrCP, 
                          "Smoke.Attach", [-5.8,14,-1.3]);

    s_SearchObjectID(attrID,attrCP,"Smoker.Attr.White");
    s_AttachSmoker( ctID, "brg_drag", attrID, attrCP, 
                          "Smoke.Attach", [0,3.6,0.8]);
    s_AttachSmoker( ctID, "evl_sctl", attrID, attrCP, 
                          "Smoke.Attach", [0,37,0]);
    s_AttachSmoker( ctID, "bld_v0a", attrID, attrCP, 
                          "Smoke.Attach", [9.5,17,1.6]);

//Sun City
    CreateSmoker(ctID,"Smoker.Attr.Tower",[1248.0,80.5,-4442.4] );
    CreateSmoker(ctID,"Smoker.Attr.Huge", [1285.5,82.7,4569] );
    CreateSmoker(ctID,"Smoker.Attr.White",[1369.5,82.7,-4502.9] );

//Emperia of Dragon
    CreateSmoker(ctID,"Smoker.Attr.White",[1763.4,85.7,-2595.2] );
    CreateSmoker(ctID,"Smoker.Attr.Huge", [1812.6,87.5,-2757.3] );
    CreateSmoker(ctID,"Smoker.Attr.Tower",[1898.6,85.7,-2810.1] );
    CreateSmoker(ctID,"Smoker.Attr.White",[1904.5,87.5,-2660.5] );

//Enlightened of Spirit
    CreateSmoker(ctID,"Smoker.Attr.Huge", [2298.4,82.8,-1597.4] );
    CreateSmoker(ctID,"Smoker.Attr.White",[2230.4,82.8,-1675.4] );
    CreateSmoker(ctID,"Smoker.Attr.Huge",[2367.1,80.5,-1618.6] );

//Iff Fortress
    CreateSmoker(ctID,"Smoker.Attr.Huge", [1500.5,115,-967.5] );
    CreateSmoker(ctID,"Smoker.Attr.White",[1563.8,113.2,-964.1] );
    CreateSmoker(ctID,"Smoker.Attr.White",[1666.4,113.2,-1036] );

//City of Quist
    CreateSmoker(ctID,"Smoker.Attr.White",[537.8,112.8,-2046] );
    CreateSmoker(ctID,"Smoker.Attr.Huge", [589.8,112.8,-2151.6] );

//Siren's Fort
    CreateSmoker(ctID,"Smoker.Attr.White",[940.8,80.7,-1604.9] );

//City of Tears
    CreateSmoker(ctID,"Smoker.Attr.Huge", [4604.6,107.5,-2677.4] );
    CreateSmoker(ctID,"Smoker.Attr.Huge",[4692.5,107.5,-2738.4] );

//Elong Evil Bridge
    CreateSmoker(ctID,"Smoker.Attr.White",[4184.8,37.8,-1505] );
    CreateSmoker(ctID,"Smoker.Attr.White",[4193.3,38, -1496] );
    CreateSmoker(ctID,"Smoker.Attr.White",[4223.8,37.8,-1503.3] );

}
