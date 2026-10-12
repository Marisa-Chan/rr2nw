func void main_CreateSmoker()
 var int attrID, attrCP, ctID;
{
	ctID := s_AddClassTable( "Smoker",200 );

	s_SearchObjectID(attrID,attrCP,"Smoker.Attr.White");
	s_AttachSmoker( ctID, "rec_murd", attrID, attrCP, 
                          "Smoke.Attach", [-2, 26.3, -3.9]);
	s_AttachSmoker( ctID, "war_s00", attrID, attrCP, 
                          "Smoke.Attach", [-10,27.5,-5.3]);
	s_AttachSmoker( ctID, "war_s00", attrID, attrCP, 
                          "Smoke.Attach", [-10,25,-16]);
	s_AttachSmoker( ctID, "war_s05", attrID, attrCP, 
                          "Smoke.Attach", [0,11,-36]);
	s_AttachSmoker( ctID, "war_s05", attrID, attrCP, 
                          "Smoke.Attach", [-3,0,31]);

	s_SearchObjectID(attrID,attrCP,"Smoker.Attr.Tower");
	s_AttachSmoker( ctID, "war_m04", attrID, attrCP, 
                          "Smoke.Attach", [-3.3,2.9,1.1]);

	CreateSmoker(ctID,"Smoker.Attr.Huge", [2886,65,-3035] );
	CreateSmoker(ctID,"Smoker.Attr.Huge", [3103,53,-3470] );
	CreateSmoker(ctID,"Smoker.Attr.Huge", [2515,118,-3674] );

	CreateSmoker(ctID,"Smoker.Attr.Huge", [2998,118.5,-1507] );
	CreateSmoker(ctID,"Smoker.Attr.FireMd", [2998,118.5,-1507] );
}
