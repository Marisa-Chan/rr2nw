func void main_LoadSkin()
 var int ctIDSkin;
{
    ctIDSkin := s_AddClassTable("Skin",30);

    LoadSkin( ctIDSkin, "dcross.vbc"    , "sk.DebugCross" );


    LoadSkin( ctIDSkin, "piece.vbc"   , "Expl.Piece" );
    LoadSkin( ctIDSkin, "bird.vbc"    , "sk.Bird.0" );

    LoadSkin( ctIDSkin, "vessel.vbc", "sk.Taxi.vessel" );

    ctIDSkin := s_AddClassTable("SkinSpr",1);
    LoadSkin( ctIDSkin, "fusion.txr", "sk.Fusion.0"  );
}
