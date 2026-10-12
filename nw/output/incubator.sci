func void CreateRecruitCenter(int ctID,str name,vector pos,str comander,str defBrf)
 var int oID, oCP, event;
{
    s_New( ctID, name, oID, oCP );
    event := s_OpenEventData( EDO_WRITE );
                s_WriteFloat   ( event, pos.x );
                s_WriteFloat   ( event, pos.y );
                s_WriteFloat   ( event, pos.z );
                s_WriteStr     ( event, comander );
		s_WriteStr     ( event, defBrf);
              s_CloseEventData( event );

    s_SendEventNow( event,
                    t_EV_SET_ATTR_POS,
                    oID, oCP );

    event := s_OpenEventData( EDO_WRITE );
                s_WriteFloat   ( event, 10 );
                s_WriteFloat   ( event, 6 );
                s_WriteFloat   ( event, 20 );
              s_CloseEventData( event );

    s_SendEventNow( event,
                    rc_SET_EJECT,
                    oID, oCP );
}


func void CreateIncubator(int ctID, str name, int attrID, int attrCP,vector pos)
 var int event, oID, cachePos;
 {
     New( ctID, name, oID, cachePos );
     event := s_OpenEventData( EDO_WRITE );
                s_WriteObjectID( event, attrID, attrCP );
                s_WriteFloat   ( event, pos.x );
                s_WriteFloat   ( event, pos.y );
                s_WriteFloat   ( event, pos.z );
              s_CloseEventData( event );

    s_SendEventNow( event,
                    KR_SET_ATTR,
                    oID, cachePos );
 }

