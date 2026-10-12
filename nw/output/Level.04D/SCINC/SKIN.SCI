func void CreateAnimation_Cln_f04()
 var float  rsp;
 var int oID, cachePos;
{
    CreateAnimSetsAuto(oID,cachePos, "sk.Taxi.cln_f04", 0, 3, 5);
    SetAnimateBlock(oID,cachePos,0,  0, "Prop1");
    SetAnimateBlock(oID,cachePos,0,  1, "Prop2");
    SetAnimateBlock(oID,cachePos,0,  2, "Total");

    rsp := 10;

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 0);
    skin_SetAnimProg_ROTATEOY     (oID, cachePos, 0, [-3.65,0,0], rsp, 0);
    
    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 1);
    skin_SetAnimProg_ROTATEOY     (oID, cachePos, 1, [3.65,0,0], -rsp, 0);

    skin_SetAnimProg_UPDATE       (oID, cachePos, 2);
}

func void CreateAnimation_Cln_f00()
 var float  rsp;
 var int oID, cachePos;
{
    CreateAnimSetsAuto(oID,cachePos, "sk.Peop.cln_f00", 0, 6, 13);
    SetAnimateBlock(oID,cachePos,0,  0, "LWing1");
    SetAnimateBlock(oID,cachePos,0,  1, "LWing2");
    SetAnimateBlock(oID,cachePos,0,  2, "RWing1");
    SetAnimateBlock(oID,cachePos,0,  3, "RWing2");
    SetAnimateBlock(oID,cachePos,0,  4, "Body");
    SetAnimateBlock(oID,cachePos,0,  5, "Total");

    rsp := 5;

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 0);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 0, [-3.63,3.8,0], 25~, rsp, 270~);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 0, [-0.45,3.6,0], 20~, rsp, 0);
    
    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 1);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 1, [-0.45,3.6,0], 20~, rsp, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 2);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 2, [3.63,3.8,0], 25~, rsp, 450~);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 2, [0.45,3.6,0], 20~, rsp, 180~);
    
    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 3);
   skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 3, [0.45,3.6,0], 20~, rsp, 180~);


    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 4);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 4, [0,1,6], 3~, rsp, 0);

    skin_SetAnimProg_UPDATE       (oID, cachePos, 5);
}

func void kg_AnimateLeg(int oID, int cachePos, int num, float F)
 var float rsp;
{
    rsp := 2;

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, num, [0,1,9.4], 30~, rsp, F);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+1);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, num+1, [0,3.7,8.65], 30~, rsp, F);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+2);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, num+2, [0,5.2,11.5], 50~, rsp, F + 180~);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, num+2, [0,3.7,8.65], 30~, rsp, F);
}

func void CreateAnimation_Cln_f09()
 var float  rsp;
 var int oID, cachePos;
{
    CreateAnimSetsAuto(oID,cachePos, "sk.Peop.cln_f09", 0, 9, 19);
    SetAnimateBlock(oID,cachePos,0,  0, "FWheel");
    SetAnimateBlock(oID,cachePos,0,  1, "Wheels");
    SetAnimateBlock(oID,cachePos,0,  2, "RLeg1");
    SetAnimateBlock(oID,cachePos,0,  3, "RLeg2");
    SetAnimateBlock(oID,cachePos,0,  4, "RLeg3");
    SetAnimateBlock(oID,cachePos,0,  5, "LLeg1");
    SetAnimateBlock(oID,cachePos,0,  6, "LLeg2");
    SetAnimateBlock(oID,cachePos,0,  7, "LLeg3");
    SetAnimateBlock(oID,cachePos,0,  8, "Total");

    rsp := -2;

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 0);
    skin_SetAnimProg_ROTATEOX     (oID, cachePos, 0, [0,1.1,-0.9], rsp, 0);
    
    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 1);
    skin_SetAnimProg_ROTATEOX     (oID, cachePos, 1, [0,1.2,5], rsp, 0);

    kg_AnimateLeg( oID, cachePos, 2, 0);
    kg_AnimateLeg( oID, cachePos, 5, 180~);

    skin_SetAnimProg_UPDATE       (oID, cachePos, 8);
}

func void main_LoadSkin()
 var int ctIDSkin;
{
    ctIDSkin := s_AddClassTable("Skin",30);

    LoadSkin( ctIDSkin, "dcross.vbc"    , "sk.DebugCross" );


    LoadSkin( ctIDSkin, "car_tank.vbc", "sk.Tank.0" );
    LoadSkin( ctIDSkin, "walk_t.vbc"  , "sk.Tank.1" );
    LoadSkin( ctIDSkin, "piece.vbc"   , "Expl.Piece" );
    LoadSkin( ctIDSkin, "bird.vbc"    , "sk.Bird.0" );

    LoadSkin( ctIDSkin, "cln_f01.vbc", "sk.Taxi.cln_f01" );
    LoadSkin( ctIDSkin, "cln_f00.vbc", "sk.Taxi.cln_f00" );
    LoadSkin( ctIDSkin, "cln_f03.vbc", "sk.Taxi.cln_f03" );
    LoadSkin( ctIDSkin, "cln_f04.vbc", "sk.Taxi.cln_f04" );
    LoadSkin( ctIDSkin, "cln_f05.vbc", "sk.Taxi.cln_f05" );
    LoadSkin( ctIDSkin, "cln_f08.vbc", "sk.Taxi.cln_f08" );

    LoadSkin( ctIDSkin, "cln_f00.vbc", "sk.Peop.cln_f00");
    LoadSkin( ctIDSkin, "cln_f09.vbc", "sk.Peop.cln_f09");
    LoadSkin( ctIDSkin, "shiz_f0a.vbc", "sk.Peop.shiz_f0a" );
    LoadSkin( ctIDSkin, "cln_smn2.vbc", "sk.Peop.cln_smn2" );
    LoadSkin( ctIDSkin, "lorr_f0b.vbc", "sk.Peop.lorr_f0b" );
    LoadSkin( ctIDSkin, "man_01.vbc",   "sk.Peop.man_01" );
    LoadSkin( ctIDSkin, "cln_S00.vbc",  "sk.Peop.cln_S00" );
    LoadSkin( ctIDSkin, "man_wmn.vbc",  "sk.Peop.man_wmn" );
    LoadSkin( ctIDSkin, "cln_s03.vbc", "sk.corpse.plane");	
    LoadSkin( ctIDSkin, "ast_csh1.vbc", "sk.corpse.default");	

    LoadSkin( ctIDSkin, "art1.vbc", "sk.Artefact.0");
    LoadSkin( ctIDSkin, "corpse.vbc",   "sk.corpse.final");

    // -------- temp -------------------
    LoadSkin( ctIDSkin, "corpse.vbc",   "sk.mortar.bld_a03");

    CreateAnimation_Cln_f04();
    CreateAnimation_Cln_f00();
    CreateAnimation_Cln_f09();

    ctIDSkin := s_AddClassTable("SkinSpr",1);
    LoadSkin( ctIDSkin, "fusion.txr", "sk.Fusion.0"  );


    
}