//************************** FARTER ***************************

func void CreateFarterAttr (int ctID, str name, str soundName)
var int oID,cachePos;
{
   New(ctID,name,oID,cachePos);
   SetAttribute_s( oID, cachePos, "m_soundName", soundName );
}

func void CreateFarter( int ctID, str attr, vector pos )
var int oID,cachePos, event, attrID, attrCP;
{
    s_New(ctID,"Smoker.Auto", oID, cachePos);
    s_SearchObjectID(attrID,attrCP,attr);

    event := s_OpenEventData(EDO_WRITE);
                s_WriteObjectID(event, attrID, attrCP);
                s_WriteFloat   (event,pos.x);
                s_WriteFloat   (event,pos.y);
                s_WriteFloat   (event,pos.z);
             s_CloseEventData( event );

    s_SendEventNow( event,
                    START_FARTING,
                    oID, cachePos );
}
