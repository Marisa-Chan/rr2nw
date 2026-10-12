
/**************************************************************************
 *
 *                    Skins
 *
 **************************************************************************/

func void main_LoadSkin()
 var int ctIDSkin;
{
    ctIDSkin := s_AddClassTable("Skin",30);

    LoadSkin( ctIDSkin, "dcross.vbc"    , "sk.DebugCross" );


    LoadSkin( ctIDSkin, "piece.vbc"   , "Expl.Piece" );
    LoadSkin( ctIDSkin, "bird.vbc"    , "sk.Bird.0" );

    LoadSkin( ctIDSkin, "ast_csh1.vbc", "sk.corpse.default");
    LoadSkin( ctIDSkin, "corpse.vbc",   "sk.corpse.final");
    LoadSkin( ctIDSkin, "war_t06.vbc"    , "sk.Taxi.akula" );
    LoadSkin( ctIDSkin, "art1.vbc", "sk.Artefact.0");

    ctIDSkin := s_AddClassTable("SkinSpr",1);
    LoadSkin( ctIDSkin, "fusion.txr", "sk.Fusion.0"  );
}
