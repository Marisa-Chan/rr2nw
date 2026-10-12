
void s_lev_LOAD( TStackCell *cell )    { cell->i = lev_LOAD; }
void s_lev_SAVE( TStackCell *cell )    { cell->i = lev_SAVE; }
void s_lev_RESTART( TStackCell *cell ) { cell->i = lev_RESTART; }

 //===========================================================================
void s_Const_lmp_EV_START( TStackCell *cell )
 {
     cell->i = lmp_EV_START;
 }

 //===========================================================================
void s_Const_lmp_EV_SETENDPOS( TStackCell *cell )
 {
     cell->i = lmp_EV_SETENDPOS;
 }


/*
    Event labels restored from nw.exe (absent in the snapshot headers).
  */
enum {
    EV_VEHICLE_PRINTMESSAGE      = 0x2ef2,
    EV_VEHICLE_SOUNDEVENT        = 0x2ef3,
    EV_VEHICLE_PANELEVENT        = 0x2ef4,
    pe_EVCMD_START_EX            = 0x659a,
    rc_SET_VIDEO                 = 0x985d,
    rc_SET_DEFTAXI               = 0x985e,
    rc_SET_DICTIONARY            = 0x985f,
    s_SndVol                     = 0x9485,
    train_EV_ATTACH              = 0xabe1,
    train_EV_ATTACH_SMOKER       = 0xabe2,
    train_EV_SETPAUSE            = 0xabe3,
    train_EV_ADDCANNON           = 0xabe5,
    taxi_SET_TO_POS              = 5018,
    DESTROYABLE_IMMORTAL_STATE   = 45000
};

 //===========================================================================
void s_Const_s_SndVol( TStackCell *cell )               { cell->i = s_SndVol; }
void s_Const_pe_EVCMD_START_EX( TStackCell *cell )      { cell->i = pe_EVCMD_START_EX; }
void s_Const_taxi_SET_TO_POS( TStackCell *cell )        { cell->i = taxi_SET_TO_POS; }
void s_Const_train_EV_ATTACH( TStackCell *cell )        { cell->i = train_EV_ATTACH; }
void s_Const_train_EV_ATTACH_SMOKER( TStackCell *cell ) { cell->i = train_EV_ATTACH_SMOKER; }
void s_Const_train_EV_SETPAUSE( TStackCell *cell )      { cell->i = train_EV_SETPAUSE; }
void s_Const_train_EV_ADDCANNON( TStackCell *cell )     { cell->i = train_EV_ADDCANNON; }
void s_Const_rc_SET_VIDEO( TStackCell *cell )           { cell->i = rc_SET_VIDEO; }
void s_Const_rc_SET_DEFTAXI( TStackCell *cell )         { cell->i = rc_SET_DEFTAXI; }
void s_Const_rc_SET_DICTIONARY( TStackCell *cell )      { cell->i = rc_SET_DICTIONARY; }
void s_Const_DESTROYABLE_IMMORTAL_STATE( TStackCell *cell ) { cell->i = DESTROYABLE_IMMORTAL_STATE; }
void s_Const_EV_VEHICLE_PRINTMESSAGE( TStackCell *cell ){ cell->i = EV_VEHICLE_PRINTMESSAGE; }
void s_Const_EV_VEHICLE_SOUNDEVENT( TStackCell *cell )  { cell->i = EV_VEHICLE_SOUNDEVENT; }
void s_Const_EV_VEHICLE_PANELEVENT( TStackCell *cell )  { cell->i = EV_VEHICLE_PANELEVENT; }
