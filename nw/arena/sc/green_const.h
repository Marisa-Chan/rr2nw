void s_Menu_EXIT( TStackCell *cell )    { cell->i = EV_MENU_EXIT; }
void s_Menu_SURRENDER( TStackCell *cell )    { cell->i = EV_VEHICLE_SURRENDER; }
void s_Menu_MISSIONOK( TStackCell *cell )    { cell->i = EV_VEHICLE_MISSIONOK; }

void s_Menu_Point0( TStackCell *cell ) { cell->i = EV_VEHICLE_POINT0; }
void s_Menu_Point1( TStackCell *cell ) { cell->i = EV_VEHICLE_POINT1; }
void s_Menu_Point2( TStackCell *cell ) { cell->i = EV_VEHICLE_POINT2; }
void s_Menu_Point3( TStackCell *cell ) { cell->i = EV_VEHICLE_POINT3; }
void s_Menu_Point4( TStackCell *cell ) { cell->i = EV_VEHICLE_POINT4; }
void s_Menu_Point5( TStackCell *cell ) { cell->i = EV_VEHICLE_POINT5; }
void s_Menu_Point6( TStackCell *cell ) { cell->i = EV_VEHICLE_POINT6; }
void s_Menu_Point7( TStackCell *cell ) { cell->i = EV_VEHICLE_POINT7; }
void s_Menu_Point8( TStackCell *cell ) { cell->i = EV_VEHICLE_POINT8; }
void s_MenuSetLocale( TStackCell *cell ) { cell->i = EV_MENU_SET_LOCALE; }

void s_ChangeMouseParams( TStackCell *cell ) { cell->i = CTRL_CHANGE_MOUSE_PARAMS; }
void s_ChangeJoystickParams( TStackCell *cell ) { cell->i = CTRL_CHANGE_JOYSTICK_PARAMS; }
void s_CalibrateJoystick( TStackCell *cell ) { cell->i = CTRL_CALIBRATE_JOYSTICK; }

void s_lev_LOAD_SLOT0( TStackCell *cell ) { cell->i = lev_LOAD_SLOT0; }
void s_lev_LOAD_SLOT1( TStackCell *cell ) { cell->i = lev_LOAD_SLOT1; }
void s_lev_LOAD_SLOT2( TStackCell *cell ) { cell->i = lev_LOAD_SLOT2; }
void s_lev_LOAD_SLOT3( TStackCell *cell ) { cell->i = lev_LOAD_SLOT3; }
void s_lev_LOAD_SLOT4( TStackCell *cell ) { cell->i = lev_LOAD_SLOT4; }
void s_lev_LOAD_SLOT5( TStackCell *cell ) { cell->i = lev_LOAD_SLOT5; }
void s_lev_LOAD_SLOT6( TStackCell *cell ) { cell->i = lev_LOAD_SLOT6; }
void s_lev_LOAD_SLOT7( TStackCell *cell ) { cell->i = lev_LOAD_SLOT7; }

void s_lev_SAVE_SLOT0( TStackCell *cell ) { cell->i = lev_SAVE_SLOT0; }
void s_lev_SAVE_SLOT1( TStackCell *cell ) { cell->i = lev_SAVE_SLOT1; }
void s_lev_SAVE_SLOT2( TStackCell *cell ) { cell->i = lev_SAVE_SLOT2; }
void s_lev_SAVE_SLOT3( TStackCell *cell ) { cell->i = lev_SAVE_SLOT3; }
void s_lev_SAVE_SLOT4( TStackCell *cell ) { cell->i = lev_SAVE_SLOT4; }
void s_lev_SAVE_SLOT5( TStackCell *cell ) { cell->i = lev_SAVE_SLOT5; }
void s_lev_SAVE_SLOT6( TStackCell *cell ) { cell->i = lev_SAVE_SLOT6; }
void s_lev_SAVE_SLOT7( TStackCell *cell ) { cell->i = lev_SAVE_SLOT7; }
