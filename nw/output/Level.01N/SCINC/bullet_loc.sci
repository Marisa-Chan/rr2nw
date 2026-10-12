func void CreateBulletAttr(int ctIDAttr)
 var  int oID, cachePos;
{
    s_NewObject( ctIDAttr, "Bullet.Led" );
    s_SearchObjectID( oID, cachePos, "Bullet.Led" );
    SetAttribute_f  ( oID, cachePos, "m_step0", 0.15 );
    SetAttribute_f  ( oID, cachePos, "m_step", 0.00 );

    SetAttribute_f  ( oID, cachePos, "m_radius0", 0.10 );
    SetAttribute_f  ( oID, cachePos, "m_radius1", 0.20 );
    SetAttribute_f  ( oID, cachePos, "m_length",  3 );

    SetAttribute_s  ( oID, cachePos, "m_explAttrName", "Expl.Attr.Small" );

    SetAttribute_s  ( oID, cachePos, "m_smokeAttrName","Smoke.Attr.LedSm");
    SetAttribute_f  ( oID, cachePos, "massa", 0.009 );
    SetAttribute_f  ( oID, cachePos, "m_startSpeed", 200.0 );
    SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(0,0,0)); 
    SetAttribute_i  ( oID, cachePos, "m_RGB0", ConvertColor(180,60,20));
    SetAttribute_i  ( oID, cachePos, "m_useLight", 1 );

    SetAttribute_i  ( oID, cachePos, "m_lightColor", LIGHT_COLOR_RED );
    SetAttribute_f  ( oID, cachePos, "m_lightRadius", 20 );
    SetAttribute_i  ( oID, cachePos, "m_lightBrightness", 80 );
    SetAttribute_i  ( oID, cachePos, "m_traceExist", 0 );
    SetAttribute_s  ( oID, cachePos, "m_shootSndName","wav.Shoot.Gun1");
}

func void main_CreateBullets()
 var int ctID;
{
    s_AddClassTable("Bullet"     ,500);
    ctID := s_AddClassTable("BulletAttr",4);
    CreateBulletAttr    (ctID);
    CreateBulletAttrPrim(ctID);
    CreateBulletAttrUnit(ctID);
    CreateBulletSecAttr (ctID);
}
