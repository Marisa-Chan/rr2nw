/**************************************************************************
 *
 *                            Smoke
 *
 **************************************************************************/




func void smoke_SetRadiusComponent( int oID, int cachePos,
                                    float minA, float maxA,
                                    float minB, float maxB,
                                    float minC, float maxC)
{
   if(  minA >  maxA or minB > maxB or minC > maxC  )
   then print( "smoke_SetRadiusComponent: Error order component", );
  
   SetAttribute_f  ( oID, cachePos, "m_minRA", minA  );
   SetAttribute_f  ( oID, cachePos, "m_maxRA", maxA  );
   SetAttribute_f  ( oID, cachePos, "m_minRB", minB  );
   SetAttribute_f  ( oID, cachePos, "m_maxRB", maxB  );
   SetAttribute_f  ( oID, cachePos, "m_minRC", minC  );
   SetAttribute_f  ( oID, cachePos, "m_maxRC", maxC  );
}

func void smoke_SetTransparetComp( int oID, int cachePos,
                                    float minA, float maxA,
                                    float minB, float maxB,
                                    float minC, float maxC)
{
   if(  minA >  maxA or minB > maxB or minC > maxC  )
   then print( "smoke_SetTransparetComp: Error order component", );

   SetAttribute_f  ( oID, cachePos, "m_minTA", minA  );
   SetAttribute_f  ( oID, cachePos, "m_maxTA", maxA  );
   SetAttribute_f  ( oID, cachePos, "m_minTB", minB  );
   SetAttribute_f  ( oID, cachePos, "m_maxTB", maxB  );
   SetAttribute_f  ( oID, cachePos, "m_minTC", minC  );
   SetAttribute_f  ( oID, cachePos, "m_maxTC", maxC  );
}


/*
 * Set wind and out direct
 */
func void smoke_SetOfsDir(int oID, int cachePos,
                          vector ofs, vector dir )
{
   SetAttribute_f  ( oID, cachePos, "m_ofsHAngle",  ofs.x );
   SetAttribute_f  ( oID, cachePos, "m_ofsVAngle",  ofs.y );
   SetAttribute_f  ( oID, cachePos, "m_ofsSpeed",   ofs.z );
   SetAttribute_f  ( oID, cachePos, "m_dirHAngle",  dir.x );
   SetAttribute_f  ( oID, cachePos, "m_dirVAngle",  dir.y );
   SetAttribute_f  ( oID, cachePos, "m_dirSpeed",   dir.z );
}

func void smoke_SetDirSpeedMul( int oID, int cachePos, float min, float max )
{
   if  min > max
   then print( "smoke_SetDirSpeedMul: Error param order" );

   SetAttribute_f  ( oID, cachePos, "m_minDirInc",  min );
   SetAttribute_f  ( oID, cachePos, "m_maxDirInc",  max );
}


func void /********************************/  CreateSmokeAttrFireArea(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Smoke.Attr.FireArea",  oID, cachePos );

   SetAttribute_f  ( oID, cachePos, "m_radius",  2.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  1 );
   SetAttribute_i  ( oID, cachePos, "m_maxBlob", 4  );
   smoke_SetDirSpeedMul( oID, cachePos, 1.05, 1.06 );
   SetAttribute_f  ( oID, cachePos, "m_rndOfs",  20.4 ); // Разброс от цента

   smoke_SetOfsDir ( oID,cachePos, [0,0,1],  [0,0,6] );

   SetAttribute_f  ( oID, cachePos, "m_maxTimeLife", 200.0 );

   smoke_SetRadiusComponent(  oID, cachePos,
                             -17,-15,
                             45,50,
                             9.0, 12.0 );

   smoke_SetTransparetComp (  oID, cachePos,
                              -180, -130,
                              40,50,
                              240,255 );

   SetAttribute_s  ( oID, cachePos, "m_imageName", "flame.spr"  );
   SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(255,120,40) );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.04  );
}



func void /*************************************/ CreateSmokeAttr(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Smoke.Attr.Small",  oID, cachePos );

   SetAttribute_f  ( oID, cachePos, "m_radius",  2.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );
   SetAttribute_i  ( oID, cachePos, "m_maxBlob", 2  );
   smoke_SetDirSpeedMul( oID, cachePos, 0.96, 0.99 );
   SetAttribute_f  ( oID, cachePos, "m_rndOfs",  0.4 ); // Разброс от цента

   smoke_SetOfsDir ( oID,cachePos, [0,60~,1],  [0,0,10] );
   SetAttribute_f  ( oID, cachePos, "m_maxTimeLife", 200.0 );

   smoke_SetRadiusComponent(  oID, cachePos,
                              -0.4, -0.3,
                               2.0, 3.0,
                               1.0, 1.7 );

   smoke_SetTransparetComp (  oID, cachePos,
                              -13.5, -10,
                               15, 20,
                               200, 255 );

   SetAttribute_s  ( oID, cachePos, "m_imageName", "smoke.spr"  );
   SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(0,0,0) );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.08  );
}

func void /*************************************/ CreateSmokeAttrTrace(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Smoke.Attr.Trace",  oID, cachePos );

   SetAttribute_f  ( oID, cachePos, "m_radius",  1.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );
   SetAttribute_i  ( oID, cachePos, "m_maxBlob", 1  );
   smoke_SetDirSpeedMul( oID, cachePos, 0.96, 0.99 );
   SetAttribute_f  ( oID, cachePos, "m_rndOfs",  0.1 ); // Разброс от цента

   smoke_SetOfsDir ( oID,cachePos, [0,60~,1],  [0,0,10] );
   SetAttribute_f  ( oID, cachePos, "m_maxTimeLife", 1.0 );

   smoke_SetRadiusComponent(  oID, cachePos,
                              -0.4, -0.3,
                               2.0, 3.0,
                               0.5, 0.7 );

   smoke_SetTransparetComp (  oID, cachePos,
                              -150.5, -150.0,
                               15, 20,
                               200, 255 );

   SetAttribute_s  ( oID, cachePos, "m_imageName", "smoke.spr"  );
   SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(0,0,0) );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.1  );   
}



func void /*************************************/ CreateSmokeAttrTower(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Smoke.Attr.Tower",  oID, cachePos );

   SetAttribute_f  ( oID, cachePos, "m_radius",  2.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );
   SetAttribute_i  ( oID, cachePos, "m_maxBlob", 1  );
   smoke_SetDirSpeedMul( oID, cachePos, 0.99, 1.0 );
   SetAttribute_f  ( oID, cachePos, "m_rndOfs",  0.2 ); // Разброс от цента

   smoke_SetOfsDir ( oID,cachePos, [0,60~,2],  [0,0,5] );
   SetAttribute_f  ( oID, cachePos, "m_maxTimeLife", 20.0 );

   smoke_SetRadiusComponent(  oID, cachePos,
                              -0.4, -0.3,
                               2.0, 3.0,
                               1.0, 1.7 );

   smoke_SetTransparetComp (  oID, cachePos,
                              -13.5, -10,
                               15, 20,
                               200, 255 );

   SetAttribute_s  ( oID, cachePos, "m_imageName", "smoke.spr"  );
   SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(0,0,0) );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.04  );
}

func void /*************************************/ CreateSmokeAttrCorpse(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Smoke.Attr.Corpse",  oID, cachePos );

   SetAttribute_f  ( oID, cachePos, "m_radius",  2.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );
   SetAttribute_i  ( oID, cachePos, "m_maxBlob", 1  );
   smoke_SetDirSpeedMul( oID, cachePos, 0.99, 1.0 );
   SetAttribute_f  ( oID, cachePos, "m_rndOfs",  0.2 ); // Разброс от цента

   smoke_SetOfsDir ( oID,cachePos, [0,60~,2],  [0,0,5] );
   SetAttribute_f  ( oID, cachePos, "m_maxTimeLife", 20.0 );

   smoke_SetRadiusComponent(  oID, cachePos,
                              -0.4, -0.3,
                               2.0, 3.0,
                               2.0, 2.7 );

   smoke_SetTransparetComp (  oID, cachePos,
                              -13.5, -10,
                               15, 20,
                               200, 255 );

   SetAttribute_s  ( oID, cachePos, "m_imageName", "smoke.spr"  );
   SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(0,0,0) );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.04  );
}


func void /*************************************/ CreateSmokeAttrWhite(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Smoke.Attr.White",  oID, cachePos );

   SetAttribute_f  ( oID, cachePos, "m_radius",  2.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );
   SetAttribute_i  ( oID, cachePos, "m_maxBlob", 1  );
   smoke_SetDirSpeedMul( oID, cachePos, 0.99, 1.0 );
   SetAttribute_f  ( oID, cachePos, "m_rndOfs",  0.2 ); // Разброс от цента

   smoke_SetOfsDir ( oID,cachePos, [0,60~,2],  [0,0,5] );
   SetAttribute_f  ( oID, cachePos, "m_maxTimeLife", 20.0 );

   smoke_SetRadiusComponent(  oID, cachePos,
                              -0.4, -0.3,
                               2.0, 3.0,
                               1.0, 1.7 );

   smoke_SetTransparetComp (  oID, cachePos,
                              -13.5, -10,
                               15, 20,
                               200, 255 );

   SetAttribute_s  ( oID, cachePos, "m_imageName", "smoke.spr"  );
   SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(250,250,250) );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.04  );
}

func void /*************************************/ CreateSmokeAttrHuge(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Smoke.Attr.Huge",  oID, cachePos );

   SetAttribute_f  ( oID, cachePos, "m_radius",  2.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );
   SetAttribute_i  ( oID, cachePos, "m_maxBlob", 2  );
   smoke_SetDirSpeedMul( oID, cachePos, 1.01, 1.02 );
   SetAttribute_f  ( oID, cachePos, "m_rndOfs",  0.5 ); // Разброс от центра

   smoke_SetOfsDir ( oID,cachePos, [0,60~,4],  [0,0,4] );
   SetAttribute_f  ( oID, cachePos, "m_maxTimeLife", 50.0 );

   smoke_SetRadiusComponent(  oID, cachePos,
                              0 , 0,
                              1.0, 1.5,
                              2, 3 );

   smoke_SetTransparetComp (  oID, cachePos,
                             -7.5, -5,                           
                              18, 24,
                              200, 255 );

   SetAttribute_s  ( oID, cachePos, "m_imageName", "smoke.spr"  );
   SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(0,0,0) );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.04  );
}


func void /*********************************/  CreateSmokeAttrLed(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Smoke.Attr.Led",  oID, cachePos );

   SetAttribute_f  ( oID, cachePos, "m_radius",  2.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );
   SetAttribute_i  ( oID, cachePos, "m_maxBlob", 3  );
   smoke_SetDirSpeedMul( oID, cachePos, 0.90, 0.91 );
   SetAttribute_f  ( oID, cachePos, "m_rndOfs",  0.4 ); // Разброс от цента

   smoke_SetOfsDir ( oID,cachePos, [0,60~,1],  [0,0,40] );

   SetAttribute_f  ( oID, cachePos, "m_maxTimeLife", 20.0 );

   smoke_SetRadiusComponent(  oID, cachePos,
                              0,0,     
                              4.0, 5.0,    
                              0.2, 0.3 );

   smoke_SetTransparetComp (  oID, cachePos,
                              0,0,
                              -80, -60,
                              200, 255 );

   SetAttribute_s  ( oID, cachePos, "m_imageName", "smoke.spr"  );
   SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(200,200,200) );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.02  );
}

func void /*********************************/  CreateSmokeAttrLedBig(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Smoke.Attr.LedBig",  oID, cachePos );

   SetAttribute_f  ( oID, cachePos, "m_radius",  2.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );
   SetAttribute_i  ( oID, cachePos, "m_maxBlob", 3  );
   smoke_SetDirSpeedMul( oID, cachePos, 0.93, 0.94 );
   SetAttribute_f  ( oID, cachePos, "m_rndOfs",  0.4 ); // Разброс от цента

   smoke_SetOfsDir ( oID,cachePos, [0,60~,1],  [0,0,60] );

   SetAttribute_f  ( oID, cachePos, "m_maxTimeLife", 20.0 );

   smoke_SetRadiusComponent(  oID, cachePos,
                              -0.2,-0.2,     
                              4.0, 5.0,    
                              0.2, 0.3 );

   smoke_SetTransparetComp (  oID, cachePos,
                              0,0,
                              -70, -50,
                              200, 255 );

   SetAttribute_s  ( oID, cachePos, "m_imageName", "smoke.spr"  );
   SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(200,200,200) );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.02  );
}


func void /*******************************/  CreateSmokeAttrLedSm(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Smoke.Attr.LedSm",  oID, cachePos );

   SetAttribute_f  ( oID, cachePos, "m_radius",  2.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );
   SetAttribute_i  ( oID, cachePos, "m_maxBlob", 3  );
   smoke_SetDirSpeedMul( oID, cachePos, 0.90, 0.91 );
   SetAttribute_f  ( oID, cachePos, "m_rndOfs",  0.4 ); // Разброс от цента

   smoke_SetOfsDir ( oID,cachePos, [0,60~,1],  [0,0,40] );

   SetAttribute_f  ( oID, cachePos, "m_maxTimeLife", 20.0 );

   smoke_SetRadiusComponent(  oID, cachePos,
                              0,0,     
                              1.0, 1.5,    
                              0.2, 0.3 );

   smoke_SetTransparetComp (  oID, cachePos,
                              0,0,
                              -90, -80,
                              150, 180 );

   SetAttribute_s  ( oID, cachePos, "m_imageName", "smoke.spr"  );
   SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(200,200,200) );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.02  );
}

func void /*********************************/  CreateSmokeAttrDef(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Smoke.Attr.Def",  oID, cachePos );

   SetAttribute_f  ( oID, cachePos, "m_radius",  2.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );
   SetAttribute_i  ( oID, cachePos, "m_maxBlob", 2  );
   smoke_SetDirSpeedMul( oID, cachePos, 1.01, 1.01 );
   SetAttribute_f  ( oID, cachePos, "m_rndOfs",  0.4 ); // Разброс от цента

   smoke_SetOfsDir ( oID,cachePos, [0,60~,4],  [0,0,30] );

   SetAttribute_f  ( oID, cachePos, "m_maxTimeLife", 200.0 );

   smoke_SetRadiusComponent(  oID, cachePos,
                              0,0,     
                              4,6,   
                              3.0, 4.7 );

   smoke_SetTransparetComp (  oID, cachePos,
                              -7,-5,
                              10,12,
                              200,255 );

   SetAttribute_s  ( oID, cachePos, "m_imageName", "smoke.spr"  );
   SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(0,0,0) );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.04  );
}

func void /*****************************/  CreateSmokeAttrVolcano(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Smoke.Attr.Volcano",  oID, cachePos );

   SetAttribute_f  ( oID, cachePos, "m_radius",  2.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );
   SetAttribute_i  ( oID, cachePos, "m_maxBlob", 2  );
   smoke_SetDirSpeedMul( oID, cachePos, 1.0, 1.0 );
   SetAttribute_f  ( oID, cachePos, "m_rndOfs",  0.4 ); // Разброс от цента

   smoke_SetOfsDir ( oID,cachePos, [0,60~,20],  [0,0,100] );

   SetAttribute_f  ( oID, cachePos, "m_maxTimeLife", 200.0 );

   smoke_SetRadiusComponent(  oID, cachePos,
                              0,0,     8,10,   20, 25.7 );

   smoke_SetTransparetComp (  oID, cachePos,
                             -2, -1,
                             10, 12,
                             200, 255 );

   SetAttribute_s  ( oID, cachePos, "m_imageName", "smoke.spr"  );
   SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(0,0,0) );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.04  );
}

func void /********************************/  CreateSmokeAttrFire(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Smoke.Attr.Fire",  oID, cachePos );

   SetAttribute_f  ( oID, cachePos, "m_radius",  2.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );
   SetAttribute_i  ( oID, cachePos, "m_maxBlob", 2  );
   smoke_SetDirSpeedMul( oID, cachePos, 1.05, 1.06 );
   SetAttribute_f  ( oID, cachePos, "m_rndOfs",  0.4 ); // Разброс от цента

   smoke_SetOfsDir ( oID,cachePos, [0,60~,1],  [0,0,40] );

   SetAttribute_f  ( oID, cachePos, "m_maxTimeLife", 200.0 );

   smoke_SetRadiusComponent(  oID, cachePos,
                             -17,-15,
                             45,50,
                             6.5, 7.0 );

   smoke_SetTransparetComp (  oID, cachePos,
                              -180, -130,
                              40,50,
                              200,255 );

   SetAttribute_s  ( oID, cachePos, "m_imageName", "flame.spr"  );
   SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(255,80,0) );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.04  );
}


func void /****************************/  CreateSmokeAttrFireGrad(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Smoke.Attr.FireGrad",  oID, cachePos );

   SetAttribute_f  ( oID, cachePos, "m_radius",  2.0 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );
   SetAttribute_i  ( oID, cachePos, "m_maxBlob", 2  );
   smoke_SetDirSpeedMul( oID, cachePos, 1.05, 1.06 );
   SetAttribute_f  ( oID, cachePos, "m_rndOfs",  0.4 ); // Разброс от цента

   smoke_SetOfsDir ( oID,cachePos, [0,60~,1],  [0,0,40] );

   SetAttribute_f  ( oID, cachePos, "m_maxTimeLife", 200.0 );

   smoke_SetRadiusComponent(  oID, cachePos,
                             -17,-15,
                             45,50,
                             6.5, 7.0 );

   smoke_SetTransparetComp (  oID, cachePos,
                              -180, -130,
                              40,50,
                              200,255 );

   SetAttribute_s  ( oID, cachePos, "m_imageName", "flame.spr"  );
   SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(255,80,0) );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.04  );


   SetAttribute_i  ( oID, cachePos, "m_isColorGradient", 1);
   SetAttribute_i  ( oID, cachePos, "RGB0", ConvertColor(255,120,40) );
   SetAttribute_i  ( oID, cachePos, "RGB1", ConvertColor(255,100, 0) );
   SetAttribute_i  ( oID, cachePos, "RGB2", ConvertColor(255, 80, 0) );
   SetAttribute_i  ( oID, cachePos, "RGB3", ConvertColor(200, 40, 0) );

   SetAttribute_f  ( oID, cachePos, "m_r0", 15.0 );
   SetAttribute_f  ( oID, cachePos, "m_r1", 20.0 );
   SetAttribute_f  ( oID, cachePos, "m_r2", 35.0 );
   SetAttribute_f  ( oID, cachePos, "m_r3", 40.0 );
}




func void /***************************/  CreateSmokeAttrFireSmall(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Smoke.Attr.FireSm",  oID, cachePos );

   SetAttribute_f  ( oID, cachePos, "m_radius",  0.2 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );
   SetAttribute_i  ( oID, cachePos, "m_maxBlob", 3  );
   smoke_SetDirSpeedMul( oID, cachePos, 1.14, 1.141);
   SetAttribute_f  ( oID, cachePos, "m_rndOfs",  0.03 ); // Разброс от цента

   smoke_SetOfsDir ( oID,cachePos, [0,60~,0.3],  [0,0,0.2] );

   SetAttribute_f  ( oID, cachePos, "m_maxTimeLife", 0.9 );

   smoke_SetRadiusComponent(  oID, cachePos,
                             -1.2,-1.1,
                             1.0,1.1,
                             0.17, 0.2 );

   smoke_SetTransparetComp (  oID, cachePos,
                              -270, -250,
                              40,45,
                              200,255 );

   SetAttribute_s  ( oID, cachePos, "m_imageName", "flame.spr"  );
   SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(255,80,0) );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.04  );

   SetAttribute_i  ( oID, cachePos, "m_isColorGradient", 1);
   SetAttribute_i  ( oID, cachePos, "RGB0", ConvertColor(255,255,170) );
   SetAttribute_i  ( oID, cachePos, "RGB1", ConvertColor(255,170,20) );
   SetAttribute_i  ( oID, cachePos, "RGB2", ConvertColor(255, 60,0) );
   SetAttribute_i  ( oID, cachePos, "RGB3", ConvertColor(200,0,-100) );

   SetAttribute_i  ( oID, cachePos, "m_useRndDir", 1);
   SetAttribute_f  ( oID, cachePos, "m_rndDir", 0.5);

   SetAttribute_f  ( oID, cachePos, "m_r0", 0.01);
   SetAttribute_f  ( oID, cachePos, "m_r1", 0.5 );
   SetAttribute_f  ( oID, cachePos, "m_r2", 0.4 );
   SetAttribute_f  ( oID, cachePos, "m_r3", 0.28 );

}

func void /***************************/  CreateSmokeAttrFireMidle(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Smoke.Attr.FireMd",  oID, cachePos );

   SetAttribute_f  ( oID, cachePos, "m_radius",  0.2 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );
   SetAttribute_i  ( oID, cachePos, "m_maxBlob", 3  );
   smoke_SetDirSpeedMul( oID, cachePos, 1.14, 1.141);
   SetAttribute_f  ( oID, cachePos, "m_rndOfs",  0.03 ); // Разброс от цента

   smoke_SetOfsDir ( oID,cachePos, [0,60~,0.3],  [0,0,0.8] );

   SetAttribute_f  ( oID, cachePos, "m_maxTimeLife", 0.9 );

   smoke_SetRadiusComponent(  oID, cachePos,
                             -1.2,-1.1,
                             1.0,1.1,
                             0.17, 0.2 );

   smoke_SetTransparetComp (  oID, cachePos,
                              -270, -250,
                              40,45,
                              200,255 );

   SetAttribute_s  ( oID, cachePos, "m_imageName", "flame.spr"  );
   SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(255,80,0) );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.04  );

   SetAttribute_i  ( oID, cachePos, "m_isColorGradient", 1);
   SetAttribute_i  ( oID, cachePos, "RGB0", ConvertColor(255,255,170) );
   SetAttribute_i  ( oID, cachePos, "RGB1", ConvertColor(255,170,20) );
   SetAttribute_i  ( oID, cachePos, "RGB2", ConvertColor(255, 60,0) );
   SetAttribute_i  ( oID, cachePos, "RGB3", ConvertColor(200,0,-100) );

   SetAttribute_i  ( oID, cachePos, "m_useRndDir", 1);
   SetAttribute_f  ( oID, cachePos, "m_rndDir", 0.5);

   SetAttribute_f  ( oID, cachePos, "m_r0", 0.04);
   SetAttribute_f  ( oID, cachePos, "m_r1", 2.0 );
   SetAttribute_f  ( oID, cachePos, "m_r2", 1.6 );
   SetAttribute_f  ( oID, cachePos, "m_r3", 1.2 );

}


func void /***************************/  CreateSmokeAttrFireCorpse(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Smoke.Attr.Fire.Corpse",  oID, cachePos );

   SetAttribute_f  ( oID, cachePos, "m_radius",  0.2 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );
   SetAttribute_i  ( oID, cachePos, "m_maxBlob", 3  );
   smoke_SetDirSpeedMul( oID, cachePos, 1.14, 1.2 );
   SetAttribute_f  ( oID, cachePos, "m_rndOfs",  0.03 ); // Разброс от цента

   smoke_SetOfsDir ( oID,cachePos, [0,60~,0.3],  [0,0,0.8] );

   SetAttribute_f  ( oID, cachePos, "m_maxTimeLife", 0.9 );

   smoke_SetRadiusComponent(  oID, cachePos,
                             -1.2,-1.1,
                             1.0,1.1,
                             0.17, 0.2 );

   smoke_SetTransparetComp (  oID, cachePos,
                              -270, -250,
                              40,45,
                              200,255 );

   SetAttribute_s  ( oID, cachePos, "m_imageName", "flame.spr"  );
   SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(255,80,0) );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.04  );

   SetAttribute_i  ( oID, cachePos, "m_isColorGradient", 1);
   SetAttribute_i  ( oID, cachePos, "RGB0", ConvertColor(255,255,170) );
   SetAttribute_i  ( oID, cachePos, "RGB1", ConvertColor(255,170,20) );
   SetAttribute_i  ( oID, cachePos, "RGB2", ConvertColor(255, 60,0) );
   SetAttribute_i  ( oID, cachePos, "RGB3", ConvertColor(200,0,-100) );

   SetAttribute_i  ( oID, cachePos, "m_useRndDir", 1);
   SetAttribute_f  ( oID, cachePos, "m_rndDir", 0.5);

   SetAttribute_f  ( oID, cachePos, "m_r0", 0.5);
   SetAttribute_f  ( oID, cachePos, "m_r1", 5.0 );
   SetAttribute_f  ( oID, cachePos, "m_r2", 4.0 );
   SetAttribute_f  ( oID, cachePos, "m_r3", 1.2 );

}

func void /***************************/  CreateSmokeAttrFireMidleBlue(int ctIDAttr)
 var  int oID, cachePos;
{
   New( ctIDAttr, "Smoke.Attr.FireMdBlue",  oID, cachePos );

   SetAttribute_f  ( oID, cachePos, "m_radius",  0.2 );
   SetAttribute_i  ( oID, cachePos, "m_onLand",  0 );
   SetAttribute_i  ( oID, cachePos, "m_maxBlob", 4  );
   smoke_SetDirSpeedMul( oID, cachePos, 1.14, 1.141);
   SetAttribute_f  ( oID, cachePos, "m_rndOfs",  0.03 ); // Разброс от цента

   smoke_SetOfsDir ( oID,cachePos, [0,60~,0.3],  [0,0,0.8] );

   SetAttribute_f  ( oID, cachePos, "m_maxTimeLife", 0.9 );

   smoke_SetRadiusComponent(  oID, cachePos,
                             -1.2,-1.1,
                             1.0,1.1,
                             0.17, 0.2 );

   smoke_SetTransparetComp (  oID, cachePos,
                              -270, -250,
                              40,45,
                              200,255 );

   SetAttribute_s  ( oID, cachePos, "m_imageName", "flame.spr"  );
   SetAttribute_i  ( oID, cachePos, "m_RGB",  ConvertColor(255,80,0) );
   SetAttribute_f  ( oID, cachePos, "m_timeIncrement", 0.04  );

   SetAttribute_i  ( oID, cachePos, "m_isColorGradient", 1);
   SetAttribute_i  ( oID, cachePos, "RGB0", ConvertColor(255,255,255) );
   SetAttribute_i  ( oID, cachePos, "RGB1", ConvertColor(20,170,255) );
   SetAttribute_i  ( oID, cachePos, "RGB2", ConvertColor(0, 60,255) );
   SetAttribute_i  ( oID, cachePos, "RGB3", ConvertColor(-100,0,200) );

   SetAttribute_i  ( oID, cachePos, "m_useRndDir", 1);
   SetAttribute_f  ( oID, cachePos, "m_rndDir", 0.5);

   SetAttribute_f  ( oID, cachePos, "m_r0", 0.04);
   SetAttribute_f  ( oID, cachePos, "m_r1", 2.0 );
   SetAttribute_f  ( oID, cachePos, "m_r2", 1.6 );
   SetAttribute_f  ( oID, cachePos, "m_r3", 1.2 );
}


func void CreateSmoke(str name,vector pos)
var int oID,cachePos, event;
{
    s_SearchObjectID( oID, cachePos, "Smoke.Attr.Small" );
    s_NewObjectN("Smoke",name);
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

func void     main_CreateSmokeAttr()
 var int ctSmokeAttr;
{
    ctSmokeAttr :=  s_AddClassTable("SmokeAttr",18);
    CreateSmokeAttr        (ctSmokeAttr);
    CreateSmokeAttrLed     (ctSmokeAttr);
    CreateSmokeAttrLedSm   (ctSmokeAttr);
    CreateSmokeAttrLedBig  (ctSmokeAttr);
    CreateSmokeAttrDef     (ctSmokeAttr);
    CreateSmokeAttrFire    (ctSmokeAttr);
    CreateSmokeAttrFireGrad(ctSmokeAttr);
    CreateSmokeAttrVolcano(ctSmokeAttr);
    CreateSmokeAttrFireArea  (ctSmokeAttr);
    CreateSmokeAttrTower  (ctSmokeAttr);
    CreateSmokeAttrHuge   (ctSmokeAttr);
    CreateSmokeAttrWhite  (ctSmokeAttr);
    CreateSmokeAttrFireSmall(ctSmokeAttr);
    CreateSmokeAttrFireMidle(ctSmokeAttr);
    CreateSmokeAttrTrace(ctSmokeAttr);
    CreateSmokeAttrFireMidleBlue(ctSmokeAttr);
    CreateSmokeAttrCorpse  	(ctSmokeAttr);
    CreateSmokeAttrFireCorpse   (ctSmokeAttr);
}


/**************************************************************************
 *
 *                            Smoker
 *
 **************************************************************************/

func void smoker_SetAttr(int oID, int cachePos,
                         float minAdd, float maxAdd, str name, float t, int ol)
{
   SetAttribute_f( oID, cachePos, "m_createIncMin", minAdd  );
   SetAttribute_f( oID, cachePos, "m_createIncMax", maxAdd  );
   SetAttribute_s( oID, cachePos, "m_smokeAttrName", name );
   SetAttribute_f( oID, cachePos, "m_maxTimeLife", t );
   SetAttribute_i ( oID, cachePos, "m_onLand",  ol );
}


func void CreateSmokerAttrFireCorpse(int ctSmokerAttrID, str name)
 var  int oID, cachePos;
{
   New(ctSmokerAttrID, name, oID, cachePos);

   smoker_SetAttr( oID, cachePos,
                   0.1, 0.2, "Smoke.Attr.Fire.Corpse", INFINITY_TIME, NO_LAND  );
   SetAttribute_i ( oID, cachePos, "m_useLight",    1 );
   SetAttribute_i ( oID, cachePos, "m_lightColor",  LIGHT_COLOR_YELLOW );
   SetAttribute_f ( oID, cachePos, "m_lightRadius",  3.0 );
   SetAttribute_f ( oID, cachePos, "m_lightOffset",  1.5 );
   SetAttribute_f ( oID, cachePos, "m_minLightBright",  100.0 );
   SetAttribute_f ( oID, cachePos, "m_maxLightBright",  140.0 );
   SetAttribute_f ( oID, cachePos, "m_lightBrightStep",  120.0 );

   SetAttribute_f ( oID, cachePos, "m_maxTimeLife",  15.0 );

   SetAttribute_i ( oID, cachePos, "m_useCorona",    0 );
   SetAttribute_i ( oID, cachePos, "m_coronaRGB",  ConvertColor(255,230,170) );
   SetAttribute_s ( oID, cachePos, "m_coronaName",   "corona.spr" );
   SetAttribute_f ( oID, cachePos, "m_maxCoronaR",    40 );
   SetAttribute_f ( oID, cachePos, "m_coronaR",    0.003 );
   SetAttribute_i ( oID, cachePos, "m_coronaAlpha",   140 );

}


func void CreateSmokerAttrFireArea(int ctSmokerAttrID, str name)
 var  int oID, cachePos;
{
   New(ctSmokerAttrID, name, oID, cachePos);

   smoker_SetAttr( oID, cachePos,
                   0.1, 0.8, "Smoke.Attr.FireArea", INFINITY_TIME, ON_LAND  );
   SetAttribute_i ( oID, cachePos, "m_useLight",    1 );
   SetAttribute_i ( oID, cachePos, "m_lightColor",  LIGHT_COLOR_RED );
   SetAttribute_f ( oID, cachePos, "m_lightRadius",  90.0 );
   SetAttribute_f ( oID, cachePos, "m_lightOffset",  15 );
   SetAttribute_f ( oID, cachePos, "m_minLightBright",  100.0 );
   SetAttribute_f ( oID, cachePos, "m_maxLightBright",  255.0 );
   SetAttribute_f ( oID, cachePos, "m_lightBrightStep",  120.0 );
}



func void CreateSmokerAttr(int ctSmokerAttrID, str name)
 var  int oID, cachePos;
{
   New(ctSmokerAttrID, name, oID, cachePos);

   smoker_SetAttr( oID, cachePos,
                   0.1, 1.8, "Smoke.Attr.Volcano", INFINITY_TIME, ON_LAND  );
}

func void CreateSmokerAttrFire(int ctSmokerAttrID, str name)
 var  int oID, cachePos;
{
   New(ctSmokerAttrID, name, oID, cachePos);

   smoker_SetAttr( oID, cachePos,
                   0.1, 0.8, "Smoke.Attr.FireGrad", INFINITY_TIME, ON_LAND  );

   SetAttribute_i ( oID, cachePos, "m_useLight",    1 );
   SetAttribute_i ( oID, cachePos, "m_lightColor",  LIGHT_COLOR_RED );
   SetAttribute_f ( oID, cachePos, "m_lightRadius",  30.0 );
   SetAttribute_f ( oID, cachePos, "m_lightOffset",  5 );
   SetAttribute_f ( oID, cachePos, "m_minLightBright",  100.0 );
   SetAttribute_f ( oID, cachePos, "m_maxLightBright",  255.0 );
   SetAttribute_f ( oID, cachePos, "m_lightBrightStep",  120.0 );
}




func void CreateSmokerAttrFireSmall(int ctSmokerAttrID, str name)
 var  int oID, cachePos;
{
   New(ctSmokerAttrID, name, oID, cachePos);

   smoker_SetAttr( oID, cachePos,
                   0.1, 0.2, "Smoke.Attr.FireSm", INFINITY_TIME, NO_LAND  );
   SetAttribute_i ( oID, cachePos, "m_useLight",    1 );
   SetAttribute_i ( oID, cachePos, "m_lightColor",  LIGHT_COLOR_YELLOW );
   SetAttribute_f ( oID, cachePos, "m_lightRadius",  3.0 );
   SetAttribute_f ( oID, cachePos, "m_lightOffset",  1.5 );
   SetAttribute_f ( oID, cachePos, "m_minLightBright",  100.0 );
   SetAttribute_f ( oID, cachePos, "m_maxLightBright",  140.0 );
   SetAttribute_f ( oID, cachePos, "m_lightBrightStep",  120.0 );


}

func void CreateSmokerAttrFireMidle(int ctSmokerAttrID, str name)
 var  int oID, cachePos;
{
   New(ctSmokerAttrID, name, oID, cachePos);

   smoker_SetAttr( oID, cachePos,
                   0.1, 0.2, "Smoke.Attr.FireMd", INFINITY_TIME, NO_LAND  );
   SetAttribute_i ( oID, cachePos, "m_useLight",    1 );
   SetAttribute_i ( oID, cachePos, "m_lightColor",  LIGHT_COLOR_YELLOW );
   SetAttribute_f ( oID, cachePos, "m_lightRadius",  3.0 );
   SetAttribute_f ( oID, cachePos, "m_lightOffset",  1.5 );
   SetAttribute_f ( oID, cachePos, "m_minLightBright",  100.0 );
   SetAttribute_f ( oID, cachePos, "m_maxLightBright",  140.0 );
   SetAttribute_f ( oID, cachePos, "m_lightBrightStep",  120.0 );

   SetAttribute_i ( oID, cachePos, "m_useCorona",    1 );
   SetAttribute_i ( oID, cachePos, "m_coronaRGB",  ConvertColor(255,230,170) );
   SetAttribute_s ( oID, cachePos, "m_coronaName",   "corona.spr" );
   SetAttribute_f ( oID, cachePos, "m_maxCoronaR",    40 );
   SetAttribute_f ( oID, cachePos, "m_coronaR",    0.003 );
   SetAttribute_i ( oID, cachePos, "m_coronaAlpha",   140 );

}

func void CreateSmokerAttrTower(int ctSmokerAttrID, str name)
 var  int oID, cachePos;
{
   New(ctSmokerAttrID, name, oID, cachePos);

   smoker_SetAttr( oID, cachePos,
                   0.1, 1.2, "Smoke.Attr.Tower", INFINITY_TIME, NO_LAND  );
}


func void CreateSmokerAttrCorpse(int ctSmokerAttrID, str name)
 var  int oID, cachePos;
{
   New(ctSmokerAttrID, name, oID, cachePos);

   smoker_SetAttr( oID, cachePos,
                   0.1, 1.2, "Smoke.Attr.Corpse", INFINITY_TIME, NO_LAND  );
}


func void CreateSmokerAttrWhite(int ctSmokerAttrID, str name)
 var  int oID, cachePos;
{
   New(ctSmokerAttrID, name, oID, cachePos);

   smoker_SetAttr( oID, cachePos,
                   0.1, 1.2, "Smoke.Attr.White", INFINITY_TIME, NO_LAND  );
}

func void CreateSmokerAttrHuge(int ctSmokerAttrID, str name)
 var  int oID, cachePos;
{
   New(ctSmokerAttrID, name, oID, cachePos);

   smoker_SetAttr( oID, cachePos,
                   0.1, 1.0, "Smoke.Attr.Huge", INFINITY_TIME, NO_LAND  );
}

func void CreateSmokerAttrFireMidleBlue(int ctSmokerAttrID, str name)
 var  int oID, cachePos;
{
   New(ctSmokerAttrID, name, oID, cachePos);

   smoker_SetAttr( oID, cachePos,
                   0.1, 0.2, "Smoke.Attr.FireMdBlue", INFINITY_TIME, NO_LAND  );
   SetAttribute_i ( oID, cachePos, "m_useLight",    1 );
   SetAttribute_i ( oID, cachePos, "m_lightColor",  LIGHT_COLOR_CYAN );
   SetAttribute_f ( oID, cachePos, "m_lightRadius",  16.0 );
   SetAttribute_f ( oID, cachePos, "m_lightOffset",  1.5 );
   SetAttribute_f ( oID, cachePos, "m_minLightBright",  100.0 );
   SetAttribute_f ( oID, cachePos, "m_maxLightBright",  140.0 );
   SetAttribute_f ( oID, cachePos, "m_lightBrightStep",  120.0 );

   SetAttribute_i ( oID, cachePos, "m_useCorona",    1 );
   SetAttribute_i ( oID, cachePos, "m_coronaRGB",  ConvertColor(130,230,255) );
   SetAttribute_s ( oID, cachePos, "m_coronaName",   "corona.spr" );
   SetAttribute_f ( oID, cachePos, "m_maxCoronaR",    20 );
   SetAttribute_f ( oID, cachePos, "m_coronaR",    0.003 );
   SetAttribute_i ( oID, cachePos, "m_coronaAlpha",   100 );
}


//m_useCorona

func void main_CreateSmokerAttr()
 var int ctSmoker;
{
    ctSmoker := s_AddClassTable("SmokerAttr",11);
    CreateSmokerAttr      (ctSmoker,"Smoker.Attr");     // default
    CreateSmokerAttrFire  (ctSmoker,"Smoker.Attr.Fire");
    CreateSmokerAttrFireSmall  (ctSmoker,"Smoker.Attr.FireSm");
    CreateSmokerAttrFireMidle  (ctSmoker,"Smoker.Attr.FireMd");
    CreateSmokerAttrTower (ctSmoker,"Smoker.Attr.Tower");
    CreateSmokerAttrHuge  (ctSmoker,"Smoker.Attr.Huge");
    CreateSmokerAttrWhite (ctSmoker,"Smoker.Attr.White");
    CreateSmokerAttrFireMidleBlue  (ctSmoker,"Smoker.Attr.FireMdBlue");
    CreateSmokerAttrFireArea  (ctSmoker,"Smoker.Attr.FireArea");
    CreateSmokerAttrFireCorpse (ctSmoker,"Smoker.Attr.Fire.Corpse");
    CreateSmokerAttrCorpse     (ctSmoker,"Smoker.Attr.Corpse");
}


func void CreateSmoker( int ctID, str attr, vector pos )
 var int oID,cachePos, event, attrID, attrCP;
{
    s_SearchObjectID( attrID, attrCP, attr );
    s_New(ctID,"Smoker.Auto", oID, cachePos);

    event := s_OpenEventData(EDO_WRITE);
                s_WriteObjectID(event, attrID, attrCP);
                s_WriteFloat   (event,pos.x);
                s_WriteFloat   (event,pos.y);
                s_WriteFloat   (event,pos.z);
             s_CloseEventData( event );

    s_SendEventNow( event,
                    fou_EVCMD_START,
                    oID, cachePos );
}
