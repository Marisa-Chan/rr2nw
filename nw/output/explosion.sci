/**************************************************************************
 *
 *                            Explosion
 *
 **************************************************************************/
func float get_par_koef(int type, float y1, float t2, float t3)
var float a, b;
{
	if type = 3 then return y1;
        if type = 1 then {
	  a := -y1/(t3*t3 - 2 * t2 * t3);
	  return a;
	}
	if type = 2 then {
	  b := 2 * y1 * t2 / (t3*t3 - 2 * t3 * t2);
	  return b;
	}
}

func void CreateExplosionAttrBig( int ctID )
 var int oID, cachePos;
{
   New(ctID, "Expl.Attr.Big", oID, cachePos);
   SetAttribute_f  ( oID, cachePos, "m_radiusDamage", 7 );
   SetAttribute_f  ( oID, cachePos, "m_power", 0.7 );


// Градиет для дыма
    SetAttribute_i  ( oID, cachePos, "m_sRGB0", ConvertColor(250,250,250));
    SetAttribute_i  ( oID, cachePos, "m_sRGB1", ConvertColor(100,60,20));
    SetAttribute_i  ( oID, cachePos, "m_sRGB2", ConvertColor(0,0,0));
    SetAttribute_f  ( oID, cachePos, "m_minPartTimeLife", 0.15 );
    SetAttribute_f  ( oID, cachePos, "m_maxPartTimeLife", 0.6 );
    SetAttribute_f  ( oID, cachePos, "m_minPartSize", 0.3 );
    SetAttribute_f  ( oID, cachePos, "m_maxPartSize", 0.7 );
    SetAttribute_f  ( oID, cachePos, "m_minPartSnSize", 0.4 );
    SetAttribute_f  ( oID, cachePos, "m_maxPartSnSize", 0.7 );


// Particles
    fount_SetColors ( oID, cachePos,0,0,0,  20,2,2,  100,5,0, 80,3,3);
    SetAttribute_i  ( oID, cachePos, "m_minPartCnt", 19 );
    SetAttribute_i  ( oID, cachePos, "m_maxPartCnt", 26 );
    SetAttribute_f  ( oID, cachePos, "m_createRadius", 4.0 );

// Snakes
    SetAttribute_i  ( oID, cachePos, "m_snRGBtail",   ConvertColor(120,60,46));
    SetAttribute_i  ( oID, cachePos, "m_snRGBhead",   ConvertColor(150,150,150));
    SetAttribute_i  ( oID, cachePos, "m_snRGBcenter", ConvertColor(220,220,220));
    SetAttribute_f  ( oID, cachePos, "m_snDeltaT", 0.4);
    SetAttribute_i  ( oID, cachePos, "m_snPartCnt", 6 );
    SetAttribute_i  ( oID, cachePos, "m_minPartSnCnt", 14 );
    SetAttribute_i  ( oID, cachePos, "m_maxPartSnCnt", 18 );
    SetAttribute_f  ( oID, cachePos, "m_minPartSnTimeLife", 0.2 );
    SetAttribute_f  ( oID, cachePos, "m_maxPartSnTimeLife", 0.6 );



    SetAttribute_i  ( oID, cachePos, "m_minSmokeCnt", 11 );
    SetAttribute_i  ( oID, cachePos, "m_maxSmokeCnt", 15 );

    SetAttribute_f  ( oID, cachePos, "m_minPieceSpeed", 4 );
    SetAttribute_f  ( oID, cachePos, "m_maxPieceSpeed", 8 );


    SetAttribute_f  ( oID, cachePos, "m_minPartSpeed", 15 );
    SetAttribute_f  ( oID, cachePos, "m_maxPartSpeed", 18 );

    SetAttribute_f  ( oID, cachePos, "m_minSmokeTimeLife", 10 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeTimeLife", 10 );

    SetAttribute_f  ( oID, cachePos, "m_minPieceTimeLife", 3 );
    SetAttribute_f  ( oID, cachePos, "m_maxPieceTimeLife", 7 );


//коеф. параболы прозрачности дыма (от 0 до 255)
    SetAttribute_f  ( oID, cachePos, "m_minSmokeTA", -70 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeTA", -65 );

    SetAttribute_f  ( oID, cachePos, "m_minSmokeTB", 40 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeTB", 70 );

    SetAttribute_f  ( oID, cachePos, "m_minSmokeTC", 100 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeTC", 155 );


//коэф. параболы размеров дыма в (м)                                
    SetAttribute_f  ( oID, cachePos, "m_minSmokeA", -1.6 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeA", -1.5 );

    SetAttribute_f  ( oID, cachePos, "m_minSmokeB", 12 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeB", 14 );

    SetAttribute_f  ( oID, cachePos, "m_minSmokeC", 4 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeC", 4.5 );


//Скорость разбегания дыма
    SetAttribute_f  ( oID, cachePos, "m_minSmokeSpeed", 15 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeSpeed", 17 );
    SetAttribute_f  ( oID, cachePos, "m_minMulSpeed", 0.9 );
    SetAttribute_f  ( oID, cachePos, "m_maxMulSpeed", 0.9 );


    SetAttribute_f  ( oID, cachePos, "m_minPieceOySpeed", 2 );
    SetAttribute_f  ( oID, cachePos, "m_maxPieceOySpeed", 3 );
    SetAttribute_f  ( oID, cachePos, "m_minPieceOxSpeed", 3 );
    SetAttribute_f  ( oID, cachePos, "m_maxPieceOxSpeed", 6 );

//Скорость поднятия дыма
    SetAttribute_f  ( oID, cachePos, "m_ofsSpeed", 25.0 );
    SetAttribute_f  ( oID, cachePos, "m_ofsSpeedMul", 0.97 );

    SetAttribute_s  ( oID, cachePos, "m_smokeName", "expl.spr" );

//Свет
    SetAttribute_f  ( oID, cachePos, "m_lightRadius", 20 );
    SetAttribute_f  ( oID, cachePos, "m_lightOffset", 6.0 );
    SetAttribute_i  ( oID, cachePos, "m_lightColor", LIGHT_COLOR_YELLOW );

    SetAttribute_i  ( oID, cachePos, "m_rayRGB", ConvertColor(255,200,150));
    SetAttribute_f  ( oID, cachePos, "m_minRayLen", 15 );
    SetAttribute_f  ( oID, cachePos, "m_maxRayLen", 30 );
    SetAttribute_f  ( oID, cachePos, "m_lightTimeLife", 0.8 );
    SetAttribute_i  ( oID, cachePos, "m_minRayCnt", 6 );
    SetAttribute_i  ( oID, cachePos, "m_maxRayCnt", 8 );
    SetAttribute_f  ( oID, cachePos, "m_traceNewPuffTime",0.04);
    SetAttribute_s  ( oID, cachePos, "m_soundName","wav.Explosion");
}

func void SetExplSmall(int oID, int cachePos)
{
// Градиет для дыма
    SetAttribute_i  ( oID, cachePos, "m_sRGB0", ConvertColor(255,255,255));
    SetAttribute_i  ( oID, cachePos, "m_sRGB1", ConvertColor(190,190,190));
    SetAttribute_i  ( oID, cachePos, "m_sRGB2", ConvertColor(160,160,160));
    SetAttribute_f  ( oID, cachePos, "m_minPartTimeLife", 0.02 );
    SetAttribute_f  ( oID, cachePos, "m_maxPartTimeLife", 0.04 );
    SetAttribute_f  ( oID, cachePos, "m_minPartSize", 0.1 );
    SetAttribute_f  ( oID, cachePos, "m_maxPartSize", 0.4 );
    SetAttribute_f  ( oID, cachePos, "m_minPartSnSize", 0.2 );
    SetAttribute_f  ( oID, cachePos, "m_maxPartSnSize", 0.3 );


// Particles
    fount_SetColors ( oID, cachePos,0,0,0,  20,2,2,  100,5,0, 80,3,3);
    SetAttribute_i  ( oID, cachePos, "m_minPartCnt", 4 );
    SetAttribute_i  ( oID, cachePos, "m_maxPartCnt", 7 );
    SetAttribute_f  ( oID, cachePos, "m_createRadius", 1.0 );

// Snakes
    SetAttribute_i  ( oID, cachePos, "m_snRGBtail",   ConvertColor(120,20,20));
    SetAttribute_i  ( oID, cachePos, "m_snRGBhead",   ConvertColor(100,100,100));
    SetAttribute_i  ( oID, cachePos, "m_snRGBcenter", ConvertColor(170,170,170));
    SetAttribute_f  ( oID, cachePos, "m_snDeltaT", 0.2 );
    SetAttribute_i  ( oID, cachePos, "m_snPartCnt", 5 );
    SetAttribute_i  ( oID, cachePos, "m_minPartSnCnt", 2 );
    SetAttribute_i  ( oID, cachePos, "m_maxPartSnCnt", 4 );
    SetAttribute_f  ( oID, cachePos, "m_minPartSnTimeLife", 0.15 );
    SetAttribute_f  ( oID, cachePos, "m_maxPartSnTimeLife", 0.25 );



    SetAttribute_i  ( oID, cachePos, "m_minSmokeCnt", 6 );
    SetAttribute_i  ( oID, cachePos, "m_maxSmokeCnt", 9 );

    SetAttribute_f  ( oID, cachePos, "m_minPieceSpeed", 0 );
    SetAttribute_f  ( oID, cachePos, "m_maxPieceSpeed", 0 );


    SetAttribute_f  ( oID, cachePos, "m_minPartSpeed", 22 );
    SetAttribute_f  ( oID, cachePos, "m_maxPartSpeed", 27 );

    SetAttribute_f  ( oID, cachePos, "m_minSmokeTimeLife", 10 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeTimeLife", 10 );

    SetAttribute_f  ( oID, cachePos, "m_minPieceTimeLife", 0 );
    SetAttribute_f  ( oID, cachePos, "m_maxPieceTimeLife", 0 );


//коеф. параболы прозрачности дыма (от 0 до 255)
    SetAttribute_f  ( oID, cachePos, "m_minSmokeTA", -210 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeTA", -200 );

    SetAttribute_f  ( oID, cachePos, "m_minSmokeTB", 40 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeTB", 70 );

    SetAttribute_f  ( oID, cachePos, "m_minSmokeTC", 100 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeTC", 155 );


//коэф. параболы размеров дыма в (м)
    SetAttribute_f  ( oID, cachePos, "m_minSmokeA", get_par_koef(1, 0.4, 0.65, 1.5) );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeA", get_par_koef(1, 0.4, 0.65, 1.5) + 0.8 );

    SetAttribute_f  ( oID, cachePos, "m_minSmokeB", get_par_koef(2, 0.4, 0.65, 1.5) );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeB", get_par_koef(2, 0.4, 0.65, 1.5) + 2 );

    SetAttribute_f  ( oID, cachePos, "m_minSmokeC", get_par_koef(3, 0.4, 0.65, 1.5) );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeC", get_par_koef(3, 0.4, 0.65, 1.5) + 0.1 );



    SetAttribute_f  ( oID, cachePos, "m_minSmokeSpeed", 1 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeSpeed", 15 );
    SetAttribute_f  ( oID, cachePos, "m_minMulSpeed", 0.7 );
    SetAttribute_f  ( oID, cachePos, "m_maxMulSpeed", 0.8 );

    SetAttribute_f  ( oID, cachePos, "m_minPieceOySpeed", 2 );
    SetAttribute_f  ( oID, cachePos, "m_maxPieceOySpeed", 3 );
    SetAttribute_f  ( oID, cachePos, "m_minPieceOxSpeed", 3 );
    SetAttribute_f  ( oID, cachePos, "m_maxPieceOxSpeed", 6 );

    SetAttribute_f  ( oID, cachePos, "m_ofsSpeed", 1.0 );

    SetAttribute_s  ( oID, cachePos, "m_smokeName", "expl.spr" );

//Свет
    SetAttribute_f  ( oID, cachePos, "m_lightRadius", 2 );
    SetAttribute_f  ( oID, cachePos, "m_lightOffset", 1.0 );
    SetAttribute_i  ( oID, cachePos, "m_lightColor", LIGHT_COLOR_YELLOW );
    SetAttribute_f  ( oID, cachePos, "m_lightTimeLife", 0.1 );

    SetAttribute_i  ( oID, cachePos, "m_rayRGB", ConvertColor(255,200,150));
    SetAttribute_f  ( oID, cachePos, "m_minRayLen", 3 );
    SetAttribute_f  ( oID, cachePos, "m_maxRayLen", 5 );
    SetAttribute_f  ( oID, cachePos, "m_lightTimeLife", 0.1 );
    SetAttribute_i  ( oID, cachePos, "m_minRayCnt", 3 );
    SetAttribute_i  ( oID, cachePos, "m_maxRayCnt", 5 );
    SetAttribute_f  ( oID, cachePos, "m_traceNewPuffTime",0.04);
    SetAttribute_s  ( oID, cachePos, "m_soundName","wav.Explosion");
}

func void CreateExplosionAttrSmall( int ctID )
 var int oID, cachePos;
{
   New(ctID, "Expl.Attr.Small", oID, cachePos);

   SetAttribute_f  ( oID, cachePos, "m_radiusDamage", 2 );
   SetAttribute_f  ( oID, cachePos, "m_power", 0.1 );

   SetExplSmall( oID, cachePos );
}

func void CreateExplosionAttrSmallPrim( int ctID )
 var int oID, cachePos;
{
   New(ctID, "Expl.Attr.Small.Prim", oID, cachePos);

   SetAttribute_f  ( oID, cachePos, "m_radiusDamage", 1 );
   SetAttribute_f  ( oID, cachePos, "m_power", 0.03 );
   SetExplSmall( oID, cachePos );
}

func void CreateExplosionAttrDefault( int ctID )
 var int oID, cachePos;
{
   New(ctID, "Expl.Attr.Default", oID, cachePos);

   SetAttribute_f  ( oID, cachePos, "m_radiusDamage", 3 );
   SetAttribute_f  ( oID, cachePos, "m_power", 0.4 );

// Градиет для дыма
    SetAttribute_i  ( oID, cachePos, "m_sRGB0", ConvertColor(255,255,255));
    SetAttribute_i  ( oID, cachePos, "m_sRGB1", ConvertColor(255,230,120));
    SetAttribute_i  ( oID, cachePos, "m_sRGB2", ConvertColor(100,50,0));
    SetAttribute_f  ( oID, cachePos, "m_minPartTimeLife", 0.4 );
    SetAttribute_f  ( oID, cachePos, "m_maxPartTimeLife", 2.0 );
    fount_SetColors ( oID, cachePos,0,0,0,  20,20,20,  85,75,0, 60,60,60);
    SetAttribute_i  ( oID, cachePos, "m_minPartCnt", 40 );
    SetAttribute_i  ( oID, cachePos, "m_maxPartCnt", 70 );
    SetAttribute_f  ( oID, cachePos, "m_createRadius", 3.0 );

    // Snakes
    SetAttribute_i  ( oID, cachePos, "m_snRGBtail",   ConvertColor(100,70,40));
    SetAttribute_i  ( oID, cachePos, "m_snRGBhead",   ConvertColor(160,100,40));
    SetAttribute_i  ( oID, cachePos, "m_snRGBcenter", ConvertColor(160,160,40));
    SetAttribute_f  ( oID, cachePos, "m_snDeltaT", 0.5 );
    SetAttribute_i  ( oID, cachePos, "m_snPartCnt", 8 );
    SetAttribute_i  ( oID, cachePos, "m_minPartSnCnt", 10 );
    SetAttribute_i  ( oID, cachePos, "m_maxPartSnCnt", 20 );
    SetAttribute_f  ( oID, cachePos, "m_minPartSnTimeLife", 1 );
    SetAttribute_f  ( oID, cachePos, "m_maxPartSnTimeLife", 1.5 );



//    SetAttribute_i  ( oID, cachePos, "m_minSmokeCnt", 3 );
//    SetAttribute_i  ( oID, cachePos, "m_maxSmokeCnt", 6 );

    SetAttribute_f  ( oID, cachePos, "m_minPieceSpeed", 3 );
    SetAttribute_f  ( oID, cachePos, "m_maxPieceSpeed", 15 );


    SetAttribute_f  ( oID, cachePos, "m_minPartSpeed", 8 );
    SetAttribute_f  ( oID, cachePos, "m_maxPartSpeed", 16 );

    SetAttribute_f  ( oID, cachePos, "m_minSmokeTimeLife", 10 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeTimeLife", 10 );

    SetAttribute_f  ( oID, cachePos, "m_minPieceTimeLife", 30. );
    SetAttribute_f  ( oID, cachePos, "m_maxPieceTimeLife", 30 );

    SetAttribute_f  ( oID, cachePos, "m_minSmokeTA", -210 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeTA", -200 );

    SetAttribute_f  ( oID, cachePos, "m_minSmokeTB", 40 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeTB", 70 );

    SetAttribute_f  ( oID, cachePos, "m_minSmokeTC", 200 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeTC", 255 );


    SetAttribute_f  ( oID, cachePos, "m_minSmokeA", -25.4 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeA", -24.2 );

    SetAttribute_f  ( oID, cachePos, "m_minSmokeB", 45 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeB", 50 );

    SetAttribute_f  ( oID, cachePos, "m_minSmokeC", 0.5 );
    SetAttribute_f  ( oID, cachePos, "m_maxSmokeC", 0.8 );



    SetAttribute_f  ( oID, cachePos, "m_minMulSpeed", 0.7 );
    SetAttribute_f  ( oID, cachePos, "m_maxMulSpeed", 0.8 );

    SetAttribute_f  ( oID, cachePos, "m_minPieceOySpeed", 2 );
    SetAttribute_f  ( oID, cachePos, "m_maxPieceOySpeed", 3 );
    SetAttribute_f  ( oID, cachePos, "m_minPieceOxSpeed", 3 );
    SetAttribute_f  ( oID, cachePos, "m_maxPieceOxSpeed", 6 );

    SetAttribute_f  ( oID, cachePos, "m_ofsSpeed", 10.0 );

    SetAttribute_s  ( oID, cachePos, "m_smokeName", "expl.spr" );

    SetAttribute_f  ( oID, cachePos, "m_lightRadius", 15 );
    SetAttribute_f  ( oID, cachePos, "m_lightOffset", 5.0 );
    SetAttribute_i  ( oID, cachePos, "m_lightColor", LIGHT_COLOR_YELLOW );
    SetAttribute_f  ( oID, cachePos, "m_lightTimeLife", 0.5 );

    SetAttribute_i  ( oID, cachePos, "m_rayRGB", ConvertColor(255,200,150));
    SetAttribute_f  ( oID, cachePos, "m_minRayLen", 20 );
    SetAttribute_f  ( oID, cachePos, "m_maxRayLen", 50 );
    SetAttribute_f  ( oID, cachePos, "m_lightTimeLife", 0.5 );
    SetAttribute_i  ( oID, cachePos, "m_minRayCnt", 10 );
    SetAttribute_i  ( oID, cachePos, "m_maxRayCnt", 15 );
    SetAttribute_f  ( oID, cachePos, "m_traceNewPuffTime",0.04);
    SetAttribute_s  ( oID, cachePos, "m_soundName","wav.Explosion");
}


func void CreateExplosionAttrSplash( int ctID )
 var int oID, cachePos;
{
   New(ctID, "BulletSplash", oID, cachePos);

   SetAttribute_f  ( oID, cachePos, "m_radiusDamage", 0.1 );
   SetAttribute_f  ( oID, cachePos, "m_power", 0.01 );

    SetAttribute_i  ( oID, cachePos, "m_minSmokeCnt", 0 );
    SetAttribute_i  ( oID, cachePos, "m_maxSmokeCnt", 0 );

    SetAttribute_i  ( oID, cachePos, "m_minPieceCnt", 0 );
    SetAttribute_i  ( oID, cachePos, "m_maxPieceCnt", 0 );

    SetAttribute_i  ( oID, cachePos, "m_minPieceSmokeCnt", 0 );
    SetAttribute_i  ( oID, cachePos, "m_maxPieceSmokeCnt", 0 );

    SetAttribute_f  ( oID, cachePos, "m_minPartTimeLife", 1.4 );
    SetAttribute_f  ( oID, cachePos, "m_maxPartTimeLife", 2.0 );
    fount_SetColors ( oID, cachePos,0,140,140,  20,120,140,  0,175,185, 60,160,170);
    SetAttribute_i  ( oID, cachePos, "m_minPartCnt", 50 );
    SetAttribute_i  ( oID, cachePos, "m_maxPartCnt", 70 );
    SetAttribute_f  ( oID, cachePos, "m_createRadius", 0.5 );

    // Snakes
    SetAttribute_i  ( oID, cachePos, "m_snRGBtail",   ConvertColor(40,120,140));
    SetAttribute_i  ( oID, cachePos, "m_snRGBhead",   ConvertColor(40,140,180));
    SetAttribute_i  ( oID, cachePos, "m_snRGBcenter", ConvertColor(120,160,180));
    SetAttribute_f  ( oID, cachePos, "m_snDeltaT", 0.5 );
    SetAttribute_i  ( oID, cachePos, "m_snPartCnt", 8 );
    SetAttribute_i  ( oID, cachePos, "m_minPartSnCnt", 15 );
    SetAttribute_i  ( oID, cachePos, "m_maxPartSnCnt", 20 );
    SetAttribute_f  ( oID, cachePos, "m_minPartSnTimeLife", 1 );
    SetAttribute_f  ( oID, cachePos, "m_maxPartSnTimeLife", 1.5 );


    SetAttribute_f  ( oID, cachePos, "m_minPartSpeed", 8 );
    SetAttribute_f  ( oID, cachePos, "m_maxPartSpeed", 10 );


    SetAttribute_f  ( oID, cachePos, "m_minMulSpeed", 0.7 );
    SetAttribute_f  ( oID, cachePos, "m_maxMulSpeed", 0.8 );

    SetAttribute_f  ( oID, cachePos, "m_ofsSpeed", 10.0 );

    SetAttribute_s  ( oID, cachePos, "m_smokeName", "expl.spr" );

    SetAttribute_f  ( oID, cachePos, "m_lightRadius", 5 );
    SetAttribute_f  ( oID, cachePos, "m_lightOffset", 2.0 );
    SetAttribute_i  ( oID, cachePos, "m_lightColor", LIGHT_COLOR_BLUE );
    SetAttribute_f  ( oID, cachePos, "m_lightTimeLife", 0.3 );

    SetAttribute_i  ( oID, cachePos, "m_minRayCnt", 0 );
    SetAttribute_i  ( oID, cachePos, "m_maxRayCnt", 0 );
    SetAttribute_f  ( oID, cachePos, "m_traceNewPuffTime",0.04);
    SetAttribute_s  ( oID, cachePos, "m_soundName","wav.Explosion");
}

func void main_CreateExplosionAttr()
 var int   ctID;
{
   ctID := s_AddClassTable( "ExplosionAttr",5 );
   CreateExplosionAttrDefault  (ctID);
   CreateExplosionAttrSmall    (ctID);
   CreateExplosionAttrSmallPrim(ctID);
   CreateExplosionAttrBig      (ctID);
   CreateExplosionAttrSplash   (ctID);
}
