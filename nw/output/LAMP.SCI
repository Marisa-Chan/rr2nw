
/**************************************************************************
 *
 *                            Lamps
 *
 **************************************************************************/

func void CreateLampAttr(int ctLampAttrID, str name)
 var  int oID, cachePos;
{
   New(ctLampAttrID, name, oID, cachePos);
   SetAttribute_i ( oID, cachePos, "m_onLand",  1);
   SetAttribute_f ( oID, cachePos, "m_maxCoronaR",    40 );
   SetAttribute_f ( oID, cachePos, "m_coronaR",    0.03 );

}


func void CreateLampAttrBlinking(int ctLampAttrID)
 var  int oID, cachePos;
{
   New(ctLampAttrID, "Lamp.Attr.Blinking", oID, cachePos);
   SetAttribute_i ( oID, cachePos, "m_lightMode",  2);
   SetAttribute_i ( oID, cachePos, "m_onLand",  1);
   SetAttribute_f ( oID, cachePos, "m_maxBrightTime",0.2);
   SetAttribute_f ( oID, cachePos, "m_maxCoronaR",    40 );
   SetAttribute_f ( oID, cachePos, "m_coronaR",    0.03 );

}


func void CreateLampAttrFading(int ctLampAttrID)
 var  int oID, cachePos;
{
   New(ctLampAttrID, "Lamp.Attr.Fading", oID, cachePos);
   SetAttribute_i ( oID, cachePos, "m_lightMode",  3);
   SetAttribute_f ( oID, cachePos, "m_createIncMin", 0.2);
   SetAttribute_f ( oID, cachePos, "m_createIncMax", 0.8);
   SetAttribute_i ( oID, cachePos, "m_coronaRGB",  ConvertColor(130,230,255) );
   SetAttribute_i ( oID, cachePos, "m_lightColor",  LIGHT_COLOR_CYAN );
   SetAttribute_f ( oID, cachePos, "m_FadeOutCoeff",600);
   SetAttribute_f ( oID, cachePos, "m_FadeInCoeff",2000);  
   SetAttribute_i ( oID, cachePos, "m_onLand",  1);
   SetAttribute_f ( oID, cachePos, "m_maxCoronaR",    40 );
   SetAttribute_f ( oID, cachePos, "m_coronaR",    0.03 );

}


func void CreateLampAttrFading2(int ctLampAttrID)
 var  int oID, cachePos;
{
   New(ctLampAttrID, "Lamp.Attr.Fading2", oID, cachePos);
   SetAttribute_i ( oID, cachePos, "m_lightMode",  0);
   SetAttribute_f ( oID, cachePos, "m_createIncMin", 0.2);
   SetAttribute_f ( oID, cachePos, "m_createIncMax", 0.2);
   SetAttribute_i ( oID, cachePos, "m_coronaRGB",  ConvertColor(255,0,0) );
   SetAttribute_i ( oID, cachePos, "m_lightColor",  LIGHT_COLOR_RED );
   SetAttribute_f ( oID, cachePos, "m_FadeOutCoeff",3200);
   SetAttribute_f ( oID, cachePos, "m_FadeInCoeff",3200);
   SetAttribute_f ( oID, cachePos, "m_maxBrightTime",0.2);
   SetAttribute_f ( oID, cachePos, "m_sleepTime",0.2);
   SetAttribute_i ( oID, cachePos, "m_onLand",  1);
   SetAttribute_f ( oID, cachePos, "m_maxCoronaR",    40 );
   SetAttribute_f ( oID, cachePos, "m_coronaR",    0.03 );
   SetAttribute_i ( oID, cachePos, "m_isMoving",    1 );
   SetAttribute_f ( oID, cachePos, "m_movementDeltaT",0.2);
   SetAttribute_i ( oID, cachePos, "m_useCorona",    0 );
   SetAttribute_i ( oID, cachePos, "m_hasParticle",    1 );

}

func void CreateLampAttrFdAttach(int ctLampAttrID)
 var  int oID, cachePos;
{
   New(ctLampAttrID, "Lamp.Attr.FdAttach", oID, cachePos);
   SetAttribute_i ( oID, cachePos, "m_lightMode",  3);
   SetAttribute_f ( oID, cachePos, "m_createIncMin", 0.2);
   SetAttribute_f ( oID, cachePos, "m_createIncMax", 0.2);

   SetAttribute_i ( oID, cachePos, "m_coronaRGB",  ConvertColor(130,230,255) );
   SetAttribute_i ( oID, cachePos, "m_lightColor",  LIGHT_COLOR_CYAN );

   SetAttribute_f ( oID, cachePos, "m_FadeOutCoeff",3200);
   SetAttribute_f ( oID, cachePos, "m_FadeInCoeff",3200);
   SetAttribute_f ( oID, cachePos, "m_maxBrightTime",0.2);
   SetAttribute_f ( oID, cachePos, "m_sleepTime",0.2);
   SetAttribute_i ( oID, cachePos, "m_onLand",  1);
   SetAttribute_f ( oID, cachePos, "m_maxCoronaR",    40 );
   SetAttribute_f ( oID, cachePos, "m_coronaR",    0.03 );
   SetAttribute_i ( oID, cachePos, "m_onLand",  0);

}

func void CreateLampAttrFd2Attach(int ctLampAttrID)
 var  int oID, cachePos;
{
   New(ctLampAttrID, "Lamp.Attr.Fd2Attach", oID, cachePos);
   SetAttribute_i ( oID, cachePos, "m_lightMode",  3);
   SetAttribute_f ( oID, cachePos, "m_createIncMin", 0.2);
   SetAttribute_f ( oID, cachePos, "m_createIncMax", 0.8);
   SetAttribute_i ( oID, cachePos, "m_coronaRGB",  ConvertColor(255,0,0) );
   SetAttribute_i ( oID, cachePos, "m_lightColor",  LIGHT_COLOR_RED );
   SetAttribute_f ( oID, cachePos, "m_FadeOutCoeff",2100);
   SetAttribute_f ( oID, cachePos, "m_FadeInCoeff",12000);  
   SetAttribute_i ( oID, cachePos, "m_onLand",  1);
   SetAttribute_f ( oID, cachePos, "m_maxCoronaR",    40 );
   SetAttribute_f ( oID, cachePos, "m_coronaR",    0.03 );
   SetAttribute_i ( oID, cachePos, "m_onLand",  0);
}




func void CreateLampAttrMiddle(int ctLampAttrID, str name)
 var  int oID, cachePos;
{
   New(ctLampAttrID, name, oID, cachePos);

   SetAttribute_i ( oID, cachePos, "m_lightColor",  LIGHT_COLOR_YELLOW );
   SetAttribute_f ( oID, cachePos, "m_lightRadius",  16.0 );
   SetAttribute_f ( oID, cachePos, "m_lightOffset",  1.5 );
   SetAttribute_f ( oID, cachePos, "m_maxLightBright",  140.0 );
   SetAttribute_i ( oID, cachePos, "m_useCorona",    1 );
   SetAttribute_i ( oID, cachePos, "m_coronaRGB",  ConvertColor(255,230,130) );
   SetAttribute_s ( oID, cachePos, "m_coronaName",   "corona.spr" );
   SetAttribute_f ( oID, cachePos, "m_maxCoronaR",    40 );
   SetAttribute_f ( oID, cachePos, "m_coronaR",    0.03 );
   SetAttribute_i ( oID, cachePos, "m_coronaAlpha",   100 );
   SetAttribute_i ( oID, cachePos, "m_onLand",  0);
} 

func void CreateLampAttrMiddleBlue(int ctLampAttrID, str name)
 var  int oID, cachePos;
{
   New(ctLampAttrID, name, oID, cachePos);

   SetAttribute_i ( oID, cachePos, "m_lightColor",  LIGHT_COLOR_CYAN );
   SetAttribute_f ( oID, cachePos, "m_lightRadius",  16.0 );
   SetAttribute_f ( oID, cachePos, "m_lightOffset",  1.5 );
   SetAttribute_f ( oID, cachePos, "m_minLightBright",  100.0 );
   SetAttribute_f ( oID, cachePos, "m_maxLightBright",  140.0 );
   SetAttribute_f ( oID, cachePos, "m_lightBrightStep",  120.0 );


   SetAttribute_i ( oID, cachePos, "m_useCorona",    1 );
   SetAttribute_i ( oID, cachePos, "m_coronaRGB",  ConvertColor(100,230,255) );
   SetAttribute_s ( oID, cachePos, "m_coronaName",   "corona.spr" );
   SetAttribute_f ( oID, cachePos, "m_maxCoronaR",    40 );
   SetAttribute_f ( oID, cachePos, "m_coronaR",    0.03 );
   SetAttribute_i ( oID, cachePos, "m_coronaAlpha",   70 );

   SetAttribute_i ( oID, cachePos, "m_onLand",  0);
}


func void CreateLampAttrMoving(int ctLampAttrID)
 var  int oID, cachePos;
{
   New(ctLampAttrID, "Lamp.Attr.Moving", oID, cachePos);

   SetAttribute_i ( oID, cachePos, "m_lightColor",  LIGHT_COLOR_YELLOW );
   SetAttribute_f ( oID, cachePos, "m_lightRadius",  16.0 );
   SetAttribute_f ( oID, cachePos, "m_lightOffset",  1.5 );
   SetAttribute_f ( oID, cachePos, "m_maxLightBright",  140.0 );
   SetAttribute_i ( oID, cachePos, "m_useCorona",    1 );
   SetAttribute_i ( oID, cachePos, "m_coronaRGB",  ConvertColor(255,230,130) );
   SetAttribute_s ( oID, cachePos, "m_coronaName",   "corona.spr" );
   SetAttribute_f ( oID, cachePos, "m_maxCoronaR",    30 );
   SetAttribute_f ( oID, cachePos, "m_coronaR",    0.03 );
   SetAttribute_i ( oID, cachePos, "m_coronaAlpha",   100 );
   SetAttribute_i ( oID, cachePos, "m_onLand",  0);
   SetAttribute_i ( oID, cachePos, "m_isMoving",  1);	
   SetAttribute_f ( oID, cachePos, "m_movementDeltaT",0.2);
} 

func void CreateLampAttrWhiteBlink(int ctLampAttrID)
 var  int oID, cachePos;
{
   New(ctLampAttrID, "Lamp.Attr.White.Blink", oID, cachePos);
   SetAttribute_i ( oID, cachePos, "m_lightMode",  2);

   SetAttribute_i ( oID, cachePos, "m_coronaRGB",  ConvertColor(255,255,255) );
   SetAttribute_i ( oID, cachePos, "m_lightColor",  LIGHT_COLOR_WHITE );

   SetAttribute_f ( oID, cachePos, "m_maxBrightTime",0.2);
   SetAttribute_f ( oID, cachePos, "m_sleepTime",2);
   SetAttribute_f ( oID, cachePos, "m_maxCoronaR",    30 );
   SetAttribute_f ( oID, cachePos, "m_coronaR",    0.03 );
   SetAttribute_i ( oID, cachePos, "m_onLand",  1);
   SetAttribute_f ( oID, cachePos, "m_particleWidth",0.5);
}





func void main_CreateLampAttr()
 var int ctLamp;
{
    ctLamp := s_AddClassTable("LampAttr",10);
    CreateLampAttr           (ctLamp,"Lamp.Attr");     // default
    CreateLampAttrBlinking   (ctLamp);    
    CreateLampAttrFading     (ctLamp);    
    CreateLampAttrFading2    (ctLamp);    
    CreateLampAttrMiddle     (ctLamp, "Lamp.Attr.Md");
    CreateLampAttrMiddleBlue (ctLamp, "Lamp.Attr.MdBlue");
    CreateLampAttrFdAttach   (ctLamp);
    CreateLampAttrFd2Attach  (ctLamp);
    CreateLampAttrMoving     (ctLamp);
    CreateLampAttrWhiteBlink (ctLamp);

}


func void CreateLamp( int ctID, str attr, vector pos )
 var int oID,cachePos, event, attrID, attrCP;
{
    s_SearchObjectID( attrID, attrCP, attr );
    s_New(ctID,"Lamp.Auto", oID, cachePos);

    event := s_OpenEventData(EDO_WRITE);
                s_WriteObjectID(event, attrID, attrCP);
                s_WriteFloat   (event,pos.x);
                s_WriteFloat   (event,pos.y);
                s_WriteFloat   (event,pos.z);
             s_CloseEventData( event );

    s_SendEventNow( event,
		    lmp_EV_START,
                    oID, cachePos );
}


func void CreateMovingLamp( int ctID, str attr, vector pos0,  vector pos1, int numberOfSteps, int step)
 var int oID,cachePos, event, attrID, attrCP;
{
    s_SearchObjectID( attrID, attrCP, attr );
    s_New(ctID,"Lamp.Auto", oID, cachePos);



    event := s_OpenEventData(EDO_WRITE);
                s_WriteObjectID(event, attrID, attrCP);
                s_WriteFloat   (event,pos1.x);
                s_WriteFloat   (event,pos1.y);
                s_WriteFloat   (event,pos1.z);
             s_CloseEventData( event );

    s_SendEventNow( event,
		    lmp_EV_START,
                    oID, cachePos );

    event := s_OpenEventData(EDO_WRITE);
                s_WriteObjectID(event, attrID, attrCP);
                s_WriteFloat   (event,pos0.x);
                s_WriteFloat   (event,pos0.y);
                s_WriteFloat   (event,pos0.z);
		s_WriteInt     (event,numberOfSteps);
		s_WriteInt     (event,step);
             s_CloseEventData( event );

    s_SendEventNow( event,
		    lmp_EV_SETENDPOS,
                    oID, cachePos );

}



