
 //=======================================================================
func void SetAnimateTankCh( str skinName )
 var int event, oID, cachePos;
 {
    s_SearchObjectID( oID, cachePos, skinName );

    event := s_OpenEventData( EDO_WRITE );
               s_WriteStr( event, "createAniSets" );
               s_WriteInt( event, 0 );
               s_WriteInt( event, 12 );
             s_CloseEventData( event );

    s_SendEventNow( event,
                    sk_EV_PROG,
                    oID, cachePos );

    SetAnimateBlock(oID,cachePos,0,  0, "N_FR");
    SetAnimateBlock(oID,cachePos,0,  1, "N_BL");
    SetAnimateBlock(oID,cachePos,0,  2, "P_FRU");
    SetAnimateBlock(oID,cachePos,0,  3, "P_FRD");
    SetAnimateBlock(oID,cachePos,0,  4, "P_BLU");
    SetAnimateBlock(oID,cachePos,0,  5, "P_BLD");

    SetAnimateBlock(oID,cachePos,0,  6, "N_FL");
    SetAnimateBlock(oID,cachePos,0,  7, "N_BR");
    SetAnimateBlock(oID,cachePos,0,  8, "P_FLU");
    SetAnimateBlock(oID,cachePos,0,  9, "P_FLD");
    SetAnimateBlock(oID,cachePos,0, 10, "P_BRU");
    SetAnimateBlock(oID,cachePos,0, 11, "P_BRD");
	
 }

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

func void CreateTankChAttr(int ctIDTankAttr)
 var  int oID, cachePos;
{
    New( ctIDTankAttr, "TankChAttr", oID, cachePos );
    SetAttribute_s  ( oID, cachePos, "m_skin",    "sk.Tank.1" );
    SetAttribute_s  ( oID, cachePos, "m_soundName", "wav.Steam2" );
    SetAttribute_i  ( oID, cachePos, "m_type",    1 );
    SetAttribute_f  ( oID, cachePos, "m_blobCreateTime", 0.15);
    SetAttribute_i  ( oID, cachePos, "m_cannonCnt", 1 );
    SetAttribute_s  ( oID, cachePos, "m_cannon0", "Cannon.Attr.Ch" );
    SetAttribute_s  ( oID, cachePos, "m_bulletAttr","Bullet.LedUnit");

    SetAttribute_f  ( oID, cachePos, "powerDeltaTime", 0.1);
}

func void CreateTankEnglAttr(int ctIDTankAttr)
 var  int oID, cachePos; 
{
    New( ctIDTankAttr, "TankEnglAttr", oID, cachePos );
    SetAttribute_s  ( oID, cachePos, "m_skin",    "sk.Taxi.cln_f05" );
    SetAttribute_s  ( oID, cachePos, "m_soundName", "wav.Steam2" );
    SetAttribute_i  ( oID, cachePos, "m_type",    -1 );
    SetAttribute_f  ( oID, cachePos, "m_blobCreateTime", 0.15);
    SetAttribute_i  ( oID, cachePos, "m_cannonCnt", 1 );
    SetAttribute_s  ( oID, cachePos, "m_cannon0", "Cannon.Attr.Ch" );
    SetAttribute_s  ( oID, cachePos, "m_bulletAttr","Bullet.LedUnit");

    SetAttribute_f  ( oID, cachePos, "powerDeltaTime", 0.1);
}

func void CreateTankHtAttr(int ctIDTankAttr)
 var  int oID, cachePos;
{
    New( ctIDTankAttr, "TankAttr", oID, cachePos );
    SetAttribute_s  ( oID, cachePos, "m_skin",    "sk.Tank.0" );
    SetAttribute_s  ( oID, cachePos, "m_soundName", "wav.Tank2" );
    SetAttribute_i  ( oID, cachePos, "m_type",    0 );
    SetAttribute_i  ( oID, cachePos, "m_cannonCnt", 1 );
    SetAttribute_s  ( oID, cachePos, "m_cannon0", "Cannon.Attr.Ht" );
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

    ctIDTankAttr  := s_AddClassTable("TankAttr",3);
    CreateTankChAttr  (ctIDTankAttr);
    CreateTankEnglAttr(ctIDTankAttr);
    CreateTankHtAttr  (ctIDTankAttr);

    SetAnimateTankCh( "sk.Tank.1" );
}
