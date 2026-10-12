
/**************************************************************************
 *
 *                            Fountain
 *
 **************************************************************************/
func void fount_SetColors( int oID, int cachePos,
                           int r0, int g0, int b0,
                           int r1, int g1, int b1,
                           int r2, int g2, int b2,
                           int r3, int g3, int b3
                         )
{
   SetAttribute_i  ( oID, cachePos, "m_RGB0", ConvertColor(r0,g0,b0));
   SetAttribute_i  ( oID, cachePos, "m_RGB1", ConvertColor(r1,g1,b1));
   SetAttribute_i  ( oID, cachePos, "m_RGB2", ConvertColor(r2,g2,b2));
   SetAttribute_i  ( oID, cachePos, "m_RGB3", ConvertColor(r3,g3,b3));
}

func void CreateVolcanoAttr(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Fount.Attr.Volcano", oID, cachePos );

   fount_SetColors ( oID, cachePos, 0,0,0,  0,0,0,  80,0,0,  0,0,0 );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.05 );
   SetAttribute_f  ( oID, cachePos, "m_minTimeAdd", 0.01 );
   SetAttribute_f  ( oID, cachePos, "m_maxTimeAdd", 0.02 );

   SetAttribute_f  ( oID, cachePos, "m_hAngle", 0 );
   SetAttribute_f  ( oID, cachePos, "m_vAngle", 0 );
   SetAttribute_f  ( oID, cachePos, "m_angleDelta", 20~ );

   SetAttribute_f  ( oID, cachePos, "m_minLifeTime", 12.2 );
   SetAttribute_f  ( oID, cachePos, "m_maxLifeTime", 12.6 );

   SetAttribute_f  ( oID, cachePos, "m_minSpeed", 46.0 );
   SetAttribute_f  ( oID, cachePos, "m_maxSpeed", 55.0 );

   SetAttribute_f  ( oID, cachePos, "m_radius", 10.0 );

   SetAttribute_f  ( oID, cachePos, "m_minrA",  0 );
   SetAttribute_f  ( oID, cachePos, "m_maxrA",  0 );
   SetAttribute_f  ( oID, cachePos, "m_minrB",  0 );
   SetAttribute_f  ( oID, cachePos, "m_maxrB",  0 );
   SetAttribute_f  ( oID, cachePos, "m_minrC",  2.8 );
   SetAttribute_f  ( oID, cachePos, "m_maxrC",  3.5 );

   SetAttribute_f  ( oID, cachePos, "m_viewHeight",  -100.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  1 );

   SetAttribute_i  ( oID, cachePos, "m_useLight",  1 );
   SetAttribute_i  ( oID, cachePos, "m_lightColor", LIGHT_COLOR_RED  );
   SetAttribute_f  ( oID, cachePos, "m_lightRadius",  100.0 );
   SetAttribute_f  ( oID, cachePos, "m_minLightBright",  120.0 );
   SetAttribute_f  ( oID, cachePos, "m_maxLightBright",  255.0 );
   SetAttribute_f  ( oID, cachePos, "m_lightBrightStep",  80.0 );
   SetAttribute_f  ( oID, cachePos, "m_lightOffset",  50.0 );

   SetAttribute_i  ( oID, cachePos, "m_maxBranchCnt",  100 );
}

// Заурядный водный фонтан
func void CreateFountainAttr(int ctIDAttr)
 var  int oID, cachePos;
 var  float light;
{
   New( ctIDAttr, "Fount.Attr.Water", oID, cachePos );
   light := 1.6;
   fount_SetColors( oID, cachePos, 80*light,90*light,110*light,
                                   40*light,80*light,100*light,
                                   40*light,60*light,80*light,
                                   40*light,40*light,80*light);

   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.05 );
   SetAttribute_f  ( oID, cachePos, "m_minTimeAdd", 0.001 );
   SetAttribute_f  ( oID, cachePos, "m_maxTimeAdd", 0.002 );

   SetAttribute_f  ( oID, cachePos, "m_hAngle", 0 );
   SetAttribute_f  ( oID, cachePos, "m_vAngle", 0 );
   SetAttribute_f  ( oID, cachePos, "m_angleDelta", 8~ );

   SetAttribute_f  ( oID, cachePos, "m_minLifeTime", 12.2 );
   SetAttribute_f  ( oID, cachePos, "m_maxLifeTime", 12.6 );

   SetAttribute_f  ( oID, cachePos, "m_minSpeed", 8.0 );
   SetAttribute_f  ( oID, cachePos, "m_maxSpeed", 15.0 );

   SetAttribute_f  ( oID, cachePos, "m_radius", 10.0 );

   SetAttribute_f  ( oID, cachePos, "m_minrA",  -0.1 );
   SetAttribute_f  ( oID, cachePos, "m_maxrA",  -0.05 );
   SetAttribute_f  ( oID, cachePos, "m_minrB",  0.05 );
   SetAttribute_f  ( oID, cachePos, "m_maxrB",  0.1 );
   SetAttribute_f  ( oID, cachePos, "m_minrC",  0.1 );
   SetAttribute_f  ( oID, cachePos, "m_maxrC",  0.2 );

   SetAttribute_f  ( oID, cachePos, "m_viewHeight",  0.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );

   SetAttribute_i  ( oID, cachePos, "m_useLight",  0 );
   SetAttribute_i  ( oID, cachePos, "m_lightColor", LIGHT_COLOR_CYAN  );
   SetAttribute_f  ( oID, cachePos, "m_lightRadius",  3.0 );
   SetAttribute_f  ( oID, cachePos, "m_minLightBright",  40.0 );
   SetAttribute_f  ( oID, cachePos, "m_maxLightBright",  200.0 );
   SetAttribute_f  ( oID, cachePos, "m_lightBrightStep",  250.0 );
   SetAttribute_f  ( oID, cachePos, "m_lightOffset",  2.0 );

   SetAttribute_i  ( oID, cachePos, "m_maxBranchCnt",  600 );
}

// Водный разлапистый фонтан
func void CreateFountain2Attr(int ctIDAttr)
 var  int oID, cachePos;
 var  float light;
{
   New( ctIDAttr, "Fount.Attr.Water2", oID, cachePos );
   light := 1.6;
   fount_SetColors( oID, cachePos, 100*light,100*light,150*light,
                                   80*light,90*light,110*light,
                                   40*light,60*light,80*light,
                                   40*light,40*light,80*light);

   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.01 );
   SetAttribute_f  ( oID, cachePos, "m_minTimeAdd", 0.0005 );
   SetAttribute_f  ( oID, cachePos, "m_maxTimeAdd", 0.001 );

   SetAttribute_f  ( oID, cachePos, "m_hAngle", 0 );
   SetAttribute_f  ( oID, cachePos, "m_vAngle", 0 );
   SetAttribute_f  ( oID, cachePos, "m_angleDelta", 35~ );

   SetAttribute_f  ( oID, cachePos, "m_minLifeTime", 12.2 );
   SetAttribute_f  ( oID, cachePos, "m_maxLifeTime", 12.5 );

   SetAttribute_f  ( oID, cachePos, "m_minSpeed", 8.0 );
   SetAttribute_f  ( oID, cachePos, "m_maxSpeed", 8.0 );

   SetAttribute_f  ( oID, cachePos, "m_radius", 5.0 );

   SetAttribute_f  ( oID, cachePos, "m_minrA",  -0.09 );
   SetAttribute_f  ( oID, cachePos, "m_maxrA",  -0.06 );
   SetAttribute_f  ( oID, cachePos, "m_minrB",  0.05 );
   SetAttribute_f  ( oID, cachePos, "m_maxrB",  0.06 );
   SetAttribute_f  ( oID, cachePos, "m_minrC",  0.1 );
   SetAttribute_f  ( oID, cachePos, "m_maxrC",  0.2 );

   SetAttribute_f  ( oID, cachePos, "m_viewHeight",  -1.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );

   SetAttribute_i  ( oID, cachePos, "m_useLight",  0 );
   SetAttribute_i  ( oID, cachePos, "m_lightColor", LIGHT_COLOR_CYAN  );
   SetAttribute_f  ( oID, cachePos, "m_lightRadius",  4.0 );
   SetAttribute_f  ( oID, cachePos, "m_minLightBright",  40.0 );
   SetAttribute_f  ( oID, cachePos, "m_maxLightBright",  150.0 );
   SetAttribute_f  ( oID, cachePos, "m_lightBrightStep",  150.0 );
   SetAttribute_f  ( oID, cachePos, "m_lightOffset",  2.0 );

   SetAttribute_i  ( oID, cachePos, "m_maxBranchCnt",  500 );

}

// Обычный кровавый фонтан
func void CreateFountainBloodAttr(int ctIDAttr)
 var  int oID, cachePos;
 var  float light;
{
   New( ctIDAttr, "Fount.Attr.Blood", oID, cachePos );
   light := 1.6;
   fount_SetColors( oID, cachePos, 255*light,50*light,50*light,
                                   150*light,0*light,50*light,
                                   120*light,0*light,0*light,
                                   50*light,0*light,0*light);

// Время, через которое пересчитываются частицы фонтана
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.02 );

// Времена, через которые добавляются частицы
   SetAttribute_f  ( oID, cachePos, "m_minTimeAdd", 0.000 );
   SetAttribute_f  ( oID, cachePos, "m_maxTimeAdd", 0.003 );

// Наклон фонтана
   SetAttribute_f  ( oID, cachePos, "m_hAngle", 0 );
   SetAttribute_f  ( oID, cachePos, "m_vAngle", 0 );

// Разброс частиц
   SetAttribute_f  ( oID, cachePos, "m_angleDelta", 10~ );

// Время существования каждой частицы
   SetAttribute_f  ( oID, cachePos, "m_minLifeTime", 0 );
   SetAttribute_f  ( oID, cachePos, "m_maxLifeTime", 12 );

// Скорости движения частиц
   SetAttribute_f  ( oID, cachePos, "m_minSpeed", 9.0 );
   SetAttribute_f  ( oID, cachePos, "m_maxSpeed", 10.0 );

// Оценочный радиус фонтана
   SetAttribute_f  ( oID, cachePos, "m_radius", 5.0 );

// Размер частиц: r(t) = at*t + b*t + c
   SetAttribute_f  ( oID, cachePos, "m_minrA",  -0.1 );
   SetAttribute_f  ( oID, cachePos, "m_maxrA",  -0.05 );
   SetAttribute_f  ( oID, cachePos, "m_minrB",  0.05 );
   SetAttribute_f  ( oID, cachePos, "m_maxrB",  0.1 );
   SetAttribute_f  ( oID, cachePos, "m_minrC",  0.05 );
   SetAttribute_f  ( oID, cachePos, "m_maxrC",  0.1 );

// Высота плоскости, достигая которой частицы уничтожаются
   SetAttribute_f  ( oID, cachePos, "m_viewHeight",  0.5 );

// Опускать ли на землю?
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );

// Световые параметры
   SetAttribute_i  ( oID, cachePos, "m_useLight", 0 );
   SetAttribute_i  ( oID, cachePos, "m_lightColor", LIGHT_COLOR_RED  );
   SetAttribute_f  ( oID, cachePos, "m_lightRadius",  3.0 );
   SetAttribute_f  ( oID, cachePos, "m_minLightBright",  40.0 );
   SetAttribute_f  ( oID, cachePos, "m_maxLightBright",  200.0 );
   SetAttribute_f  ( oID, cachePos, "m_lightBrightStep",  250.0 );
   SetAttribute_f  ( oID, cachePos, "m_lightOffset",  2.0 );

// Количество частиц
   SetAttribute_i  ( oID, cachePos, "m_maxBranchCnt",  500 );
}

// Кровный разлапистый фонтан
func void CreateFountainB2Attr(int ctIDAttr)
 var  int oID, cachePos;
 var  float light;
{
   New( ctIDAttr, "Fount.Attr.Blood2", oID, cachePos );
   light := 1.6;

   fount_SetColors( oID, cachePos, 255*light,50*light,50*light,
                                   150*light,0*light,50*light,
                                   120*light,0*light,0*light,
                                   50*light,0*light,0*light);

   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.01 );
   SetAttribute_f  ( oID, cachePos, "m_minTimeAdd", 0.0005 );
   SetAttribute_f  ( oID, cachePos, "m_maxTimeAdd", 0.001 );

   SetAttribute_f  ( oID, cachePos, "m_hAngle", 0 );
   SetAttribute_f  ( oID, cachePos, "m_vAngle", 0 );
   SetAttribute_f  ( oID, cachePos, "m_angleDelta", 35~ );

   SetAttribute_f  ( oID, cachePos, "m_minLifeTime", 12.2 );
   SetAttribute_f  ( oID, cachePos, "m_maxLifeTime", 12.5 );

   SetAttribute_f  ( oID, cachePos, "m_minSpeed", 8.0 );
   SetAttribute_f  ( oID, cachePos, "m_maxSpeed", 8.0 );

   SetAttribute_f  ( oID, cachePos, "m_radius", 5.0 );

   SetAttribute_f  ( oID, cachePos, "m_minrA",  -0.09 );
   SetAttribute_f  ( oID, cachePos, "m_maxrA",  -0.06 );
   SetAttribute_f  ( oID, cachePos, "m_minrB",  0.05 );
   SetAttribute_f  ( oID, cachePos, "m_maxrB",  0.06 );
   SetAttribute_f  ( oID, cachePos, "m_minrC",  0.1 );
   SetAttribute_f  ( oID, cachePos, "m_maxrC",  0.2 );

   SetAttribute_f  ( oID, cachePos, "m_viewHeight",  -1.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );

   SetAttribute_i  ( oID, cachePos, "m_useLight",  0 );
   SetAttribute_i  ( oID, cachePos, "m_lightColor", LIGHT_COLOR_CYAN  );
   SetAttribute_f  ( oID, cachePos, "m_lightRadius",  4.0 );
   SetAttribute_f  ( oID, cachePos, "m_minLightBright",  40.0 );
   SetAttribute_f  ( oID, cachePos, "m_maxLightBright",  150.0 );
   SetAttribute_f  ( oID, cachePos, "m_lightBrightStep",  150.0 );
   SetAttribute_f  ( oID, cachePos, "m_lightOffset",  2.0 );

   SetAttribute_i  ( oID, cachePos, "m_maxBranchCnt",  500 );

}
 
func void CreateBlobAttr(int ctIDAttr)
 var  int oID, cachePos;
 var  float light;
{
   New( ctIDAttr, "Fount.Attr.Blob", oID, cachePos );
   light := 3.6;
   fount_SetColors( oID, cachePos, 80*light,90*light,110*light,
                                   40*light,80*light,100*light,
                                   40*light,60*light,80*light,
                                   40*light,40*light,80*light);

   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.05 );
   SetAttribute_f  ( oID, cachePos, "m_minTimeAdd", 0.1 );
   SetAttribute_f  ( oID, cachePos, "m_maxTimeAdd", 0.2 );

   SetAttribute_f  ( oID, cachePos, "m_hAngle", 0 );
   SetAttribute_f  ( oID, cachePos, "m_vAngle", 0 );
   SetAttribute_f  ( oID, cachePos, "m_angleDelta", 5~ );

   SetAttribute_f  ( oID, cachePos, "m_minLifeTime", 120.2 );
   SetAttribute_f  ( oID, cachePos, "m_maxLifeTime", 120.6 );

   SetAttribute_f  ( oID, cachePos, "m_minSpeed", 4.0 );
   SetAttribute_f  ( oID, cachePos, "m_maxSpeed", 6.0 );

   SetAttribute_f  ( oID, cachePos, "m_radius", 1.0 );

   SetAttribute_f  ( oID, cachePos, "m_minrA",  -0.01 );
   SetAttribute_f  ( oID, cachePos, "m_maxrA",  -0.005 );
   SetAttribute_f  ( oID, cachePos, "m_minrB",  0.05 );
   SetAttribute_f  ( oID, cachePos, "m_maxrB",  0.1 );
   SetAttribute_f  ( oID, cachePos, "m_minrC",  0.1 );
   SetAttribute_f  ( oID, cachePos, "m_maxrC",  0.2 );

   SetAttribute_f  ( oID, cachePos, "m_viewHeight",  0.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  1 );

   SetAttribute_i  ( oID, cachePos, "m_useLight",  0 );

   SetAttribute_i  ( oID, cachePos, "m_maxBranchCnt",  100 );
   SetAttribute_i  ( oID, cachePos, "m_isBlob",  1 );
   SetAttribute_f  ( oID, cachePos, "m_radSpeed", 20 );
}

func void CreateArabeskAttr(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Fount.Attr.Arab", oID, cachePos );
   fount_SetColors( oID, cachePos, 255,255,255,  255,255,280,  
                                   230,230,160,  255,255,0 );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.03 );
   SetAttribute_f  ( oID, cachePos, "m_minTimeAdd", 0.009 );
   SetAttribute_f  ( oID, cachePos, "m_maxTimeAdd", 0.01 );

   SetAttribute_f  ( oID, cachePos, "m_hAngle", 0 );
   SetAttribute_f  ( oID, cachePos, "m_vAngle", 0 );
   SetAttribute_f  ( oID, cachePos, "m_angleDelta", 180~ );

   SetAttribute_f  ( oID, cachePos, "m_minLifeTime", 0.3 );
   SetAttribute_f  ( oID, cachePos, "m_maxLifeTime", 0.6 );

   SetAttribute_f  ( oID, cachePos, "m_minSpeed", 4.0 );
   SetAttribute_f  ( oID, cachePos, "m_maxSpeed", 4.5 );

   SetAttribute_f  ( oID, cachePos, "m_radius", 3.0 );

   SetAttribute_f  ( oID, cachePos, "m_minrA",  -0.4 );
   SetAttribute_f  ( oID, cachePos, "m_maxrA",  -0.3 );
   SetAttribute_f  ( oID, cachePos, "m_minrB",   0.1 );
   SetAttribute_f  ( oID, cachePos, "m_maxrB",   0.15 );
   SetAttribute_f  ( oID, cachePos, "m_minrC",  0.05 );
   SetAttribute_f  ( oID, cachePos, "m_maxrC",  0.08 );

   SetAttribute_f  ( oID, cachePos, "m_viewHeight",  -10.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );

   SetAttribute_i  ( oID, cachePos, "m_useLight",  1 );
   SetAttribute_i  ( oID, cachePos, "m_lightColor", LIGHT_COLOR_VIOLET  );
   SetAttribute_f  ( oID, cachePos, "m_lightRadius",  8.0 );
   SetAttribute_f  ( oID, cachePos, "m_minLightBright",  40.0 );
   SetAttribute_f  ( oID, cachePos, "m_maxLightBright",  230.0 );
   SetAttribute_f  ( oID, cachePos, "m_lightBrightStep",  250.0 );
   SetAttribute_f  ( oID, cachePos, "m_lightOffset",  0.0 );

   SetAttribute_i  ( oID, cachePos, "m_maxBranchCnt",  200 );
}


func void CreateFountain( str name, str attr, vector pos )
var int oID,cachePos, event;
{
    s_SearchObjectID( oID, cachePos, attr );
    s_NewObjectN("Fountain",name);
    event := s_OpenEventData(EDO_WRITE);
                s_WriteObjectID(event, oID, cachePos);
                s_WriteFloat   (event,pos.x);
                s_WriteFloat   (event,pos.y);
                s_WriteFloat   (event,pos.z);
             s_CloseEventData( event );

    s_SearchObjectID( oID, cachePos, name );
    s_SendEventNow( event,
                    fou_EVCMD_START,
                    oID, cachePos );
}

func void CreateBlob( int ctID, vector pos )
var int oID,cachePos, event, attrID, attrCP;
{
    s_SearchObjectID( attrID, attrCP, "Fount.Attr.Blob" );
    s_New( ctID, "Blob.Water", oID, cachePos );
    event := s_OpenEventData(EDO_WRITE);
                s_WriteObjectID(event, oID, cachePos);
                s_WriteFloat   (event,pos.x);
                s_WriteFloat   (event,pos.y);
                s_WriteFloat   (event,pos.z);
             s_CloseEventData( event );

    s_SendEventNow( event,
                    fou_EVCMD_START,
                    oID, cachePos );
}


