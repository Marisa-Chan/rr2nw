const int ARTEFACT_ATTACH = 42000;

func void CreateArtefact( int ctID, str attrName, str name, 
                          var int oID, var int cachePos )
 var int event, attrID, attrCP;
{
    oID      := -1;
    cachePos := -1;

    s_SearchObjectID( attrID, attrCP, attrName );
    if  attrCP = -1
    then
    {
        print("Unknown artefact attribute " attrName ,);
        return;
    }

    s_New( ctID, name, oID, cachePos );

    event := s_OpenEventData( EDO_WRITE );
             s_WriteObjectID      ( event, attrID, attrCP );
             s_CloseEventData( event );

    s_SendEventNow( event,KR_SET_ATTR, oID, cachePos );
}

func void AttachArtefact(
                           int oID, int cachePos, 
                           str destName, int label, vector pos)
 var int event, destID, destCP;
{
    s_SearchObjectID( destID, destCP, destName );
    event := s_OpenEventData( EDO_WRITE );
             s_WriteObjectID      ( event, destID, destCP );
             s_CloseEventData( event );

    s_SendEventNow( event, ARTEFACT_ATTACH, oID, cachePos );
}


func void SetArtefactAttr()
 var int ctID, oID, cachePos;
{
    ctID := s_AddClassTable("ArtefactAttr", 2 );

    New( ctID, "Artefact.Attr.0", oID, cachePos );
    SetAttribute_s (oID, cachePos, "m_skinName","sk.Artefact.0");
    SetAttribute_f (oID, cachePos, "m_maxCoronaR", 20);
    SetAttribute_f (oID, cachePos, "m_coronaR",    0.4);
    SetAttribute_i (oID, cachePos, "m_coronaRGB", ConvertColor(255,0,255));
    SetAttribute_i (oID, cachePos, "m_coronaAlpha", 150);
}

func void main_SetArtefact()
 var int ctID, oID, cachePos;
{
   ctID := s_AddClassTable("Artefact",5);
   CreateArtefact(ctID,"Artefact.Attr.0","Artefact.0",oID,cachePos);
   //AttachArtefact(oID, cachePos, "Peop.Auto",26003,[0,8,0]);	
}

