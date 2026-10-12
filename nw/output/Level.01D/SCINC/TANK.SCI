/**************************************************************************
 *
 *                    Cannons & Tanks
 *
 **************************************************************************/

 //=======================================================================


func void SetCannonOffset( int oID, int cachePos, str par, float val )
 var int event;
 {
    event := s_OpenEventData( EDO_WRITE );
             s_WriteStr      ( event, par );
             s_WriteFloat    ( event, val  );
             s_CloseEventData( event );

    s_SendEventNow( event,
                    sk_EV_PROG,
                    oID, cachePos );
 }


func void CreateCannonAttrCh( int ctID  )
 var  int oID, cachePos;
{
    New( ctID, "Cannon.Attr.Ch", oID, cachePos );
    SetAttribute_i( oID, cachePos, "m_fixed",    1 );
    SetCannonOffset( oID, cachePos, "ofsx", 0.013 );
    SetCannonOffset( oID, cachePos, "ofsy", 5.429 );
    SetCannonOffset( oID, cachePos, "ofsz", -7.095 );
    SetAttribute_s( oID, cachePos, "m_soundName",    "wav.Shoot.Gun1" );
}

func void CreateCannonAttrHt( int ctID  )
 var  int oID, cachePos;
{
    New( ctID, "Cannon.Attr.Ht", oID, cachePos );
    SetAttribute_i( oID, cachePos, "m_fixed",    0 );
}



func void main_CreateCannonsAndTankAttr()
 var int ctIDCannonAttr,
         ctIDTankAttr;
{
    ctIDCannonAttr := s_AddClassTable("CannonAttr",2);
    CreateCannonAttrCh( ctIDCannonAttr );
    CreateCannonAttrHt( ctIDCannonAttr );

    ctIDTankAttr  := s_AddClassTable("TankAttr",2);

}
