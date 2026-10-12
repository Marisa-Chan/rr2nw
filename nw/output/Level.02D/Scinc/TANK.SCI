
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
    SetAttribute_f( oID, cachePos, "m_idleTime", 2.4);
}

func void CreateCannonAttrHt( int ctID  )
 var  int oID, cachePos;
{
    New( ctID, "Cannon.Attr.Ht", oID, cachePos );
    SetAttribute_i( oID, cachePos, "m_fixed",    0 );
}



//--------------------------Functions added by Anton ------------------------

func void CreateTankKingAttr(int nTankAttrCTID)
 var int nObjectID,nCachePos;
{New(nTankAttrCTID,"Tank.King.Attr",nObjectID,nCachePos);
 SetAttribute_s(nObjectID,nCachePos,"m_skin","sk.tank.anl_00");
 SetAttribute_s(nObjectID,nCachePos,"m_soundName","");
 SetAttribute_i(nObjectID,nCachePos,"m_type",-1);
 SetAttribute_f(nObjectID,nCachePos,"maxSpeed",18.0);
 SetAttribute_f(nObjectID,nCachePos,"rotSpeed",1.6);
 SetAttribute_f(nObjectID,nCachePos,"m_blobCreateTime",0.15);
 SetAttribute_i(nObjectID,nCachePos,"m_cannonCnt",1);
 SetAttribute_s(nObjectID,nCachePos,"m_cannon0","Cannon.Attr.Ch");
 SetAttribute_s(nObjectID,nCachePos,"m_bulletAttr","Bullet.LedUnit");
 SetAttribute_s(nObjectID,nCachePos,"m_corpseAttrName","Corpse.Attr.First");
 SetAttribute_f(nObjectID,nCachePos,"powerDeltaTime",0.1);
}

func void CreateTankKingYoungAttr(int nTankAttrCTID)
 var int nObjectID,nCachePos;
{New(nTankAttrCTID,"Tank.KingYoung.Attr",nObjectID,nCachePos);
 SetAttribute_s(nObjectID,nCachePos,"m_skin","sk.tank.anl_00");
 SetAttribute_f(nObjectID,nCachePos,"m_armor",0.5);
 SetAttribute_f(nObjectID,nCachePos,"m_viewDist",100);
 SetAttribute_s(nObjectID,nCachePos,"m_soundName","");
 SetAttribute_i(nObjectID,nCachePos,"m_type",-1);
 SetAttribute_f(nObjectID,nCachePos,"maxSpeed",18.0);
 SetAttribute_f(nObjectID,nCachePos,"rotSpeed",1.6);
 SetAttribute_f(nObjectID,nCachePos,"m_blobCreateTime",60);
 SetAttribute_i(nObjectID,nCachePos,"m_cannonCnt",1);
 SetAttribute_s(nObjectID,nCachePos,"m_cannon0","Cannon.Attr.Ch");
 SetAttribute_s(nObjectID,nCachePos,"m_bulletAttr","Bullet.Led");
 SetAttribute_s(nObjectID,nCachePos,"m_corpseAttrName","Corpse.Attr.First");
 SetAttribute_f(nObjectID,nCachePos,"powerDeltaTime",0.1);
}

func void CreateTankMagicAttr(int nTankAttrCTID)
 var int nObjectID,nCachePos;
{New(nTankAttrCTID,"Tank.Magic.Attr",nObjectID,nCachePos);
 SetAttribute_s(nObjectID,nCachePos,"m_skin","sk.tank.anl_01");
 SetAttribute_s(nObjectID,nCachePos,"m_soundName","");
 SetAttribute_i(nObjectID,nCachePos,"m_type",-1);
 SetAttribute_f(nObjectID,nCachePos,"maxSpeed",18.0);
 SetAttribute_f(nObjectID,nCachePos,"rotSpeed",1.6);
 SetAttribute_f(nObjectID,nCachePos,"m_blobCreateTime",0.15);
 SetAttribute_i(nObjectID,nCachePos,"m_cannonCnt",1);
 SetAttribute_s(nObjectID,nCachePos,"m_cannon0","Cannon.Attr.Ch");
 SetAttribute_s(nObjectID,nCachePos,"m_bulletAttr","Bullet.LedUnit");
 SetAttribute_s(nObjectID,nCachePos,"m_corpseAttrName","Corpse.Attr.Second");
 SetAttribute_f(nObjectID,nCachePos,"powerDeltaTime",0.1);
}

func void CreateTankMagicYoungAttr(int nTankAttrCTID)
 var int nObjectID,nCachePos;
{New(nTankAttrCTID,"Tank.MagicYoung.Attr",nObjectID,nCachePos);
 SetAttribute_s(nObjectID,nCachePos,"m_skin","sk.tank.anl_01");
 SetAttribute_f(nObjectID,nCachePos,"m_armor",0.5);
 SetAttribute_f(nObjectID,nCachePos,"m_viewDist",100);
 SetAttribute_s(nObjectID,nCachePos,"m_soundName","");
 SetAttribute_i(nObjectID,nCachePos,"m_type",-1);
 SetAttribute_f(nObjectID,nCachePos,"maxSpeed",18.0);
 SetAttribute_f(nObjectID,nCachePos,"rotSpeed",1.6);
 SetAttribute_f(nObjectID,nCachePos,"m_blobCreateTime",60);
 SetAttribute_i(nObjectID,nCachePos,"m_cannonCnt",1);
 SetAttribute_s(nObjectID,nCachePos,"m_cannon0","Cannon.Attr.Ch");
 SetAttribute_s(nObjectID,nCachePos,"m_bulletAttr","Bullet.Led");
 SetAttribute_s(nObjectID,nCachePos,"m_corpseAttrName","Corpse.Attr.Second");
 SetAttribute_f(nObjectID,nCachePos,"powerDeltaTime",0.1);
}

func void CreateMortarFirstAttr(int nTankAttrCTID)
 var int nObjectID, nCachePos;
{New(nTankAttrCTID,"Tank.MortarFirst.Attr",nObjectID,nCachePos);
 SetAttribute_s(nObjectID,nCachePos,"m_skin","sk.mortar.bld_a00");
 SetAttribute_s(nObjectID,nCachePos,"m_soundName","");
 SetAttribute_i(nObjectID,nCachePos,"m_type",-1);
 SetAttribute_f(nObjectID,nCachePos,"maxSpeed",0.0);
 SetAttribute_f(nObjectID,nCachePos,"rotSpeed",0.6);
 SetAttribute_f(nObjectID,nCachePos,"m_blobCreateTime",0.15);
 SetAttribute_i(nObjectID,nCachePos,"m_cannonCnt",1);
 SetAttribute_s(nObjectID,nCachePos,"m_cannon0","Cannon.Attr.Ch");
 SetAttribute_s(nObjectID,nCachePos,"m_bulletAttr","Bullet.LedUnit");
 SetAttribute_f(nObjectID,nCachePos,"powerDeltaTime",0.1);
 SetAttribute_s(nObjectID,nCachePos,"m_corpseAttrName","Corpse.Attr.Second");
 SetAttribute_i(nObjectID,nCachePos,"m_isCannon",1);
}

func void CreateMortarSecondAttr(int nTankAttrCTID)
 var int nObjectID, nCachePos;
{New(nTankAttrCTID,"Tank.MortarSecond.Attr",nObjectID,nCachePos);
 SetAttribute_s(nObjectID,nCachePos,"m_skin","sk.mortar.bld_a03");
 SetAttribute_s(nObjectID,nCachePos,"m_soundName","");
 SetAttribute_i(nObjectID,nCachePos,"m_type",-1);
 SetAttribute_f(nObjectID,nCachePos,"maxSpeed",0.0);
 SetAttribute_f(nObjectID,nCachePos,"rotSpeed",0.6);
 SetAttribute_f(nObjectID,nCachePos,"m_blobCreateTime",0.15);
 SetAttribute_i(nObjectID,nCachePos,"m_cannonCnt",1);
 SetAttribute_s(nObjectID,nCachePos,"m_cannon0","Cannon.Attr.Ch");
 SetAttribute_s(nObjectID,nCachePos,"m_bulletAttr","Bullet.LedUnit");
 SetAttribute_f(nObjectID,nCachePos,"powerDeltaTime",0.1);
 SetAttribute_s(nObjectID,nCachePos,"m_corpseAttrName","Corpse.Attr.Second");
 SetAttribute_i(nObjectID,nCachePos,"m_isCannon",1);
}

func void CreateBallSenderFirstAttr(int nTankAttrCTID)
 var int nObjectID, nCachePos;
{New(nTankAttrCTID,"Tank.BallSenderFirst.Attr",nObjectID,nCachePos);
 SetAttribute_s(nObjectID,nCachePos,"m_skin","sk.ballsender.bld_a01");
 SetAttribute_s(nObjectID,nCachePos,"m_soundName","");
 SetAttribute_i(nObjectID,nCachePos,"m_type",-1);
 SetAttribute_f(nObjectID,nCachePos,"maxSpeed",0.0);
 SetAttribute_f(nObjectID,nCachePos,"rotSpeed",0.6);
 SetAttribute_f(nObjectID,nCachePos,"m_blobCreateTime",0.15);
 SetAttribute_i(nObjectID,nCachePos,"m_cannonCnt",1);
 SetAttribute_s(nObjectID,nCachePos,"m_cannon0","Cannon.Attr.Ch");
 SetAttribute_s(nObjectID,nCachePos,"m_bulletAttr","Bullet.LedUnit");
 SetAttribute_f(nObjectID,nCachePos,"powerDeltaTime",0.1);
 SetAttribute_s(nObjectID,nCachePos,"m_corpseAttrName","Corpse.Attr.First");
 SetAttribute_i(nObjectID,nCachePos,"m_isCannon",1);
}

func void CreateBallSenderSecondAttr(int nTankAttrCTID)
 var int nObjectID, nCachePos;
{New(nTankAttrCTID,"Tank.BallSenderSecond.Attr",nObjectID,nCachePos);
 SetAttribute_s(nObjectID,nCachePos,"m_skin","sk.ballsender.bld_a02");
 SetAttribute_s(nObjectID,nCachePos,"m_soundName","");
 SetAttribute_i(nObjectID,nCachePos,"m_type",-1);
 SetAttribute_f(nObjectID,nCachePos,"maxSpeed",0.0);
 SetAttribute_f(nObjectID,nCachePos,"rotSpeed",0.6);
 SetAttribute_f(nObjectID,nCachePos,"m_blobCreateTime",0.15);
 SetAttribute_i(nObjectID,nCachePos,"m_cannonCnt",1);
 SetAttribute_s(nObjectID,nCachePos,"m_cannon0","Cannon.Attr.Ch");
 SetAttribute_s(nObjectID,nCachePos,"m_bulletAttr","Bullet.LedUnit");
 SetAttribute_f(nObjectID,nCachePos,"powerDeltaTime",0.1);
 SetAttribute_s(nObjectID,nCachePos,"m_corpseAttrName","Corpse.Attr.First");
 SetAttribute_i(nObjectID,nCachePos,"m_isCannon",1);
}

func void main_CreateCannonsAndTankAttr()
 var int nCannonAttrCTID,nTankAttrCTID;
{nCannonAttrCTID:=s_AddClassTable("CannonAttr",2);
 CreateCannonAttrCh(nCannonAttrCTID);
 CreateCannonAttrHt(nCannonAttrCTID);
 nTankAttrCTID:=s_AddClassTable("TankAttr",8);
 CreateTankKingAttr(nTankAttrCTID);
 CreateTankKingYoungAttr(nTankAttrCTID);
 CreateTankMagicAttr(nTankAttrCTID);
 CreateTankMagicYoungAttr(nTankAttrCTID);
 CreateMortarFirstAttr(nTankAttrCTID);
 CreateMortarSecondAttr(nTankAttrCTID);
 CreateBallSenderFirstAttr(nTankAttrCTID);
 CreateBallSenderSecondAttr(nTankAttrCTID);
}
//----------------------End of functions added by Anton ---------------------
