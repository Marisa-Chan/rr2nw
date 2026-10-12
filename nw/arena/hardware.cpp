#include <windows.h>
#include "hardware.h"
#include "kernel/h/session.h"
#include "kernel\h\context.h"
#include "kernel\h\echo.h"
#include "message\a_msg.h"
#include "message\hardmsg.h"
#include "menu.h"
#define LAST_H__SCENE
#include "game.h"
#include "console.h"
#include "olevel.h"

#include "storage/h/savefile.h"

/*
  All demo must be fixed - format has been changed

  */

extern SDeviceList _dL;


Action CtrlSet::m_action[] = {
    {"Forward",             MOVE_FORWARD,       MOVE_BACKWARD,  {0, }},
    {"Backward",            MOVE_BACKWARD,      MOVE_FORWARD,   {0, }},
    {"StrafeLeft",          STRAFE_LEFT,        STRAFE_RIGHT,   {0, }},
    {"StrafeRight",         STRAFE_RIGHT,       STRAFE_LEFT,    {0, }},
    {"StrafeUp",            STRAFE_UP,          STRAFE_DOWN,    {0, }},
    {"StrafeDown",          STRAFE_DOWN,        STRAFE_UP,      {0, }},
    {"LookUp",              LOOK_UP,            LOOK_DOWN,      {0, }},
    {"LookDown",            LOOK_DOWN,          LOOK_UP,        {0, }},
    {"TurnLeft",            TURN_LEFT,          TURN_RIGHT,     {0, }},
    {"TurnRight",           TURN_RIGHT,         TURN_LEFT,      {0, }},
    {"RollLeft",            ROLL_LEFT,          ROLL_RIGHT,     {0, }},
    {"RollRight",           ROLL_RIGHT,         ROLL_LEFT,      {0, }},
    {"TurretLeft",          TURRET_LEFT,        TURRET_RIGHT,   {0, }},
    {"TurretRight",         TURRET_RIGHT,       TURRET_LEFT,    {0, }},
    {"Strafe",              STRAFE,             -1,             {0, }},
    {"FirePrimary",         FIRE_PRIMARY,       -1,             {0, }},
    {"FireSecondary",       FIRE_SECONDARY,     -1,             {0, }},
    {"ChangeWeapon",        CHANGE_WEAPON,      -1,             {0, }},
    {"Weapon0",             WEAPON0,            -1,             {0, }},
    {"Weapon1",             WEAPON1,            -1,             {0, }},
    {"Weapon2",             WEAPON2,            -1,             {0, }},
    {"Weapon3",             WEAPON3,            -1,             {0, }},
    {"Weapon4",             WEAPON4,            -1,             {0, }},
    {"Weapon5",             WEAPON5,            -1,             {0, }},
    {"Weapon6",             WEAPON6,            -1,             {0, }},
    {"Weapon7",             WEAPON7,            -1,             {0, }},
    {"Weapon8",             WEAPON8,            -1,             {0, }},
    {"Weapon9",             WEAPON9,            -1,             {0, }},
    {"LockTagert",          LOCK_TAGERT,        -1,             {0, }},
    {"Jump",                JUMP,               -1,             {0, }},
    {"Crouch",              CROUCH,             -1,             {0, }},
    {"Forceage",            FORCEAGE,           -1,             {0, }},
    {"ViewCabine",          VIEW_CABINE,        -1,             {0, }},
    {"ViewExtFixed",        VIEW_EXTERN_FIXED,  -1,             {0, }},
    {"ViewExtCleaver",      VIEW_EXTERN_CLEAVER,-1,             {0, }},
    {"ViewLandFixed",       VIEW_LAND_FIXED,    -1,             {0, }},
    {"ViewLandCleaver",     VIEW_LAND_CLEAVER,  -1,             {0, }},
    {"ViewNextWingman",     VIEW_NEXT_WINGMAN,  -1,             {0, }},
    {"ViewPrevWingman",     VIEW_PREV_WINGMAN,  -1,             {0, }},
    {"ViewWingman",         VIEW_WINGMAN,       -1,             {0, }},
    {"ViewNextObject",      VIEW_NEXT_OBJECT,   -1,             {0, }},
    {"ViewPrevObject",      VIEW_PREV_OBJECT,   -1,             {0, }},
    {"ViewObject",          VIEW_OBJECT,        -1,             {0, }},
    {"ViewObserver",        VIEW_OBSERVER,      -1,             {0, }},
    {"CenterView",          CENTER_VIEW,        -1,             {0, }},
    {"ToggleFollowScout",   TOGGLE_FOLLOW_SCOUT,-1,             {0, }},
    {"LeaveVehicle",        LEAVE_VEHICLE,      -1,             {0, }},
    {"GetVehicle",          GET_VEHICLE,        -1,             {0, }},
    {"ZoomIn",              ZOOM_IN,            -1,             {0, }},
    {"ZoomOut",             ZOOM_OUT,           -1,             {0, }},
    {"BrfBegRecord",        BRF_BEGIN_RECORD,   -1,             {0, }},
    {"BrfEndRecord",        BRF_END_RECORD,     -1,             {0, }},
    {"BrfAddPoint",         BRF_ADD_CTRL_POINT, -1,             {0, }},
    {"BrfAddPos",           BRF_ADD_POS,        -1,             {0, }},
    {"BrfSaveFlight",       BRF_SAVE_FLIGHT,    -1,             {0, }},
    {"BrfSavePos",          BRF_SAVE_POS,       -1,             {0, }},
    {"Exec",                EXEC,               -1,             {0, }},
    {"Console",             CONSOLE,            -1,             {0, }, TRUE},
    {"Light",               LIGHT,              -1,             {0, }},
    {"TogLight",            TOGGLE_LIGHT,       -1,             {0, }},
    {"StopVehicle",         STOP_VEHICLE,       -1,             {0, }},
    {"Exit",                EXIT,               -1,             {0, }, TRUE},
    {"ChangeVehicle",       CHANGE_VEHICLE,     -1,             {0, }},
    {"EnableMouse",         ENABLE_MOUSE,       -1,             {0, }},
    {"DisableMouse",        DISABLE_MOUSE,      -1,             {0, }},
    {"MoveMouse",           MOUSE_MOVE,         -1,             {0, }},
    {"MoveJoystick",        JOYSTICK_MOVE,      -1,             {0, }},
    {"SysKey",              SYS_KEY,            -1,             {0, }, TRUE},
    {"Say",                 SAY,                -1,             {0, }, TRUE},
    {"RecordDemo",          RECORD_DEMO,        -1,             {0, }, TRUE},
    {"PlayDemo",            PLAY_DEMO,          -1,             {0, }, TRUE},
    {"WriteDemo",           WRITE_DEMO,         -1,             {0, }, TRUE},
    {"StopDemo",            STOP_DEMO,          -1,             {0, }, TRUE},
    {"MenuToggle",          MENU_TOGGLE,        -1,             {0, }, TRUE},
    {"MenuUp",              MENU_UP,            -1,             {0, }, TRUE},
    {"MenuDown",            MENU_DOWN,          -1,             {0, }, TRUE},
    {"MenuLeft",            MENU_LEFT,          -1,             {0, }, TRUE},
    {"MenuRight",           MENU_RIGHT,         -1,             {0, }, TRUE},
    {"MenuEnter",           MENU_ENTER,         -1,             {0, }, TRUE},
    {"MenuDel",             MENU_DEL,           -1,             {0, }, TRUE},
    {"GameMap",             TOGGLE_GAME_MAP,    -1,             {0, }},
    {"DMap",                DMAP_TOGGLE,        -1,             {0, }, TRUE},
    {"DMapObstacle",        DMAP_TOGGLE_OBST,   -1,             {0, }, TRUE},
    {"DMapRoute",           DMAP_TOGGLE_ROUTE,  -1,             {0, }, TRUE},
    {"DMapObj",             DMAP_TOGGLE_OBJ,    -1,             {0, }, TRUE},
    {"DMapObjInfo",         DMAP_TOGGLE_OBJINFO,-1,             {0, }, TRUE},
    {"DMapScrollUp",        DMAP_SCROLL_UP,     -1,             {0, }, TRUE},
    {"DMapScrollDown",      DMAP_SCROLL_DOWN,   -1,             {0, }, TRUE},
    {"DMapScrollLeft",      DMAP_SCROLL_LEFT,   -1,             {0, }, TRUE},
    {"DMapScrollRight",     DMAP_SCROLL_RIGHT,  -1,             {0, }, TRUE},
    {"DMapToggleFollowMode",DMAP_TOGGLE_FOLLOW_MODE,  -1,       {0, }},
    {"DMapNextMission",     DMAP_NEXT_MISSION,  -1,             {0, }},
    {"DMapPreviousMission", DMAP_PREVIOUS_MISSION,    -1,             {0, }},
    {"DMapTextBoxUp",       DMAP_TEXT_BOX_UP,   -1,             {0, }},
    {"DMapTextBoxDown",     DMAP_TEXT_BOX_DOWN, -1,             {0, }},
    {"DropArtefact",        DROP_ARTEFACT,      -1,             {0, }},
};
//-------------------------------------------------------------
Key KR_Hardware::m_key[] = {
	{"Esc", VK_ESCAPE},
	{"F1", VK_F1},
	{"F2", VK_F2},
	{"F3", VK_F3},
	{"F4", VK_F4},
	{"F5", VK_F5},
	{"F6", VK_F6},
	{"F7", VK_F7},
	{"F8", VK_F8},
	{"F9", VK_F9},
	{"F10", VK_F10},
	{"F11", VK_F11},
	{"F12", VK_F12},
	//{"PrnScr", VK_PRINT},  // ??????
	{"Pause", VK_PAUSE},
	{"Ins", VK_INSERT+CTRL_EXTENDED_KEY},
	{"Del", VK_DELETE+CTRL_EXTENDED_KEY},
	{"Home", VK_HOME+CTRL_EXTENDED_KEY},
	{"End", VK_END+CTRL_EXTENDED_KEY},
	{"PgDn", VK_PRIOR+CTRL_EXTENDED_KEY},
	{"PgUp", VK_NEXT+CTRL_EXTENDED_KEY},
	{"Num/", VK_DIVIDE+CTRL_EXTENDED_KEY},
	{"Num*", VK_MULTIPLY},
	{"Num-", VK_SUBTRACT},
	{"Num+", VK_ADD},
	{"NumEnter", VK_RETURN+CTRL_EXTENDED_KEY},
	{"NumIns", VK_INSERT},
	{"NumDel", VK_DELETE},
	{"Num0", VK_NUMPAD0},
	{"Num1", VK_NUMPAD1},
	{"Num2", VK_NUMPAD2},
	{"Num3", VK_NUMPAD3},
	{"Num4", VK_NUMPAD4},
	{"Num5", VK_NUMPAD5},
	{"Num6", VK_NUMPAD6},
	{"Num7", VK_NUMPAD7},
	{"Num8", VK_NUMPAD8},
	{"Num9", VK_NUMPAD9},
	{"NumLook", VK_NUMLOCK+CTRL_EXTENDED_KEY},
	{"CapsLook", VK_CAPITAL},
	{"ScrLook", VK_SCROLL},
	{"Left", VK_LEFT+CTRL_EXTENDED_KEY},
	{"Right", VK_RIGHT+CTRL_EXTENDED_KEY},
	{"Up", VK_UP+CTRL_EXTENDED_KEY},
	{"Down", VK_DOWN+CTRL_EXTENDED_KEY},
	{"Bs", VK_BACK},
	{"Tab", VK_TAB},
	{"Enter", VK_RETURN},
	{"LShift", VK_LSHIFT},
	{"RShift", VK_RSHIFT},
	{"LCtrl", VK_LCONTROL},
	{"RCtrl", VK_RCONTROL},
	{"LAlt", VK_LMENU},
	{"RAlt", VK_RMENU},
	{"Space", VK_SPACE},
	{"`", 0xC0},
	{"1", 0x31},
	{"2", 0x32},
	{"3", 0x33},
	{"4", 0x34},
	{"5", 0x35},
	{"6", 0x36},
	{"7", 0x37},
	{"8", 0x38},
	{"9", 0x39},
	{"0", 0x30},
	{"A", 0x41},
	{"B", 0x42},
	{"C", 0x43},
	{"D", 0x44},
	{"E", 0x45},
	{"F", 0x46},
	{"G", 0x47},
	{"H", 0x48},
	{"I", 0x49},
	{"J", 0x4A},
	{"K", 0x4B},
	{"L", 0x4C},
	{"M", 0x4D},
	{"N", 0x4E},
	{"O", 0x4F},
	{"P", 0x50},
	{"Q", 0x51},
	{"R", 0x52},
	{"S", 0x53},
	{"T", 0x54},
	{"U", 0x55},
	{"V", 0x56},
	{"W", 0x57},
	{"X", 0x58},
	{"Y", 0x59},
	{"Z", 0x5A},
	{"[", 0xDB},
	{"\\",0xDC},
	{"]", 0xDD},
	{";", 0xBA},
	{"'", 0xDE},
	{"<", 0xBC},
	{"=", 0xBB},
	{"-", 0xBD},
	{">", 0xBE},
	{"/", 0xBF},
    {"MouseL", VK_LBUTTON},
    {"MouseR", VK_RBUTTON},
    {"MouseM", VK_MBUTTON},
    {"Joy1", HARDWARE_JOYSTICK0},
    {"Joy2", HARDWARE_JOYSTICK1},
    {"Joy3", HARDWARE_JOYSTICK2},
    {"Joy4", HARDWARE_JOYSTICK3},
    {NULL, 0},
};


//-------------------------------------------------------------
CtrlSet::CtrlSet(){
	ClearActions();
	ClearCache();
	SetDefault();
}
//-------------------------------------------------------------
void CtrlSet::SetControl(int action, int code){
	int i;

	s_ASSERT(action >= (int)MOVE_FORWARD && action <= (int)ACTIONS_NUM, "");
	s_ASSERT(code >= 0 && code < 1000, "");

    for(i = 0; i < MAX_CODES_ON_ACTION; ++i)
		if(m_action[action].code[i] == 0){
			m_action[action].code[i] = code;
			break;
		}
	if(i == MAX_CODES_ON_ACTION) m_action[action].code[i-1] = code;
}
//-------------------------------------------------------------
void CtrlSet::ClearActions(){
	int i, j;

	for(i = 0; i < LAST_ACTION; ++i)
	for(j = 0; j < MAX_CODES_ON_ACTION; ++j)
		m_action[i].code[j] = 0;
	
}
//-------------------------------------------------------------
void CtrlSet::ClearCache(){
	int i, j;

	for(i = 0; i < MAX_KEYS_CODES; ++i){
		m_codeCacheSize[i] = 0;
		for(j = 0; j < MAX_ACTIONS_ON_KEY; ++j) m_codeCache[i][j] = -1;
	}
}
//-------------------------------------------------------------
void CtrlSet::RemoveCode(int action, int code){
    int i;
    
    for(i = 0; i < MAX_CODES_ON_ACTION; ++i) 
        if(m_action[action].code[i] == code){
            m_action[action].code[i] = 0;
            break;
        }
    for(i = 0; i < MAX_ACTIONS_ON_KEY; ++i) 
        if(m_codeCache[code][i] == action){
            m_codeCache[code][i] = -1;
            break;
        }
    m_codeCacheSize[code]--;
    m_action[action].code[MAX_CODES_ON_ACTION-1] = 0;
}
//-------------------------------------------------------------
void CtrlSet::Link(){
	int i, j, code;

	ClearCache();
	for(i = 0; i < ACTIONS_NUM; ++i)
	for(j = 0; j < MAX_CODES_ON_ACTION; ++j){
		code = m_action[i].code[j];
		s_ASSERT(code >= 0, "");
		if(code > 0){
			m_codeCache[code][m_codeCacheSize[code]] = i;
			if(m_codeCacheSize[code] < MAX_ACTIONS_ON_KEY) m_codeCacheSize[code]++;
		}
	}
}
//-------------------------------------------------------------
int CtrlSet::Translate(int msgType, int code, int buttonDown, double *downAction, int *event) const{
    int n, i, j, k, ev, cEv;
    int c, ext;
    double d;
    int jbDown; 
    JOYINFO jInfo;
    double keySens;

    n   = 0; 
    d   = 0;
    c   = 0;
    ext = 0;
    switch(msgType){
        case CTRL_CHAR:
            event[0]      = SYS_KEY;
            downAction[0] = buttonDown;
            return(1);
        case CTRL_BUTTONS_MSG:
            event[0]      = SYS_KEY;
            downAction[0] = buttonDown;
            keySens       = g_levelAttr.get_double("keySens");
            keySens       = MinMax((double)keySens, (double)0.01, (double)1);
            for(i = 0, j = 1; i < MAX_ACTIONS_ON_KEY; ++i){
                if(m_codeCache[code][i] != -1){
                    ev  = m_action[m_codeCache[code][i]].event;
                    cEv = m_action[m_codeCache[code][i]].cEvent;
                    for(k = 0; k < MAX_CODES_ON_ACTION && m_action[ev].code[k] > 0; ++k){ // direct actions
                        if(m_action[ev].code[k] >= HARDWARE_JOYSTICK0){
                            if(g_hardware.m_ctrlUse.joystick){
                                ::joyGetPos(JOYSTICKID1, &jInfo);
                                jbDown = FALSE;
                                switch(m_action[ev].code[k]){
                                    case HARDWARE_JOYSTICK0: jbDown = jInfo.wButtons & JOY_BUTTON1; break;
                                    case HARDWARE_JOYSTICK1: jbDown = jInfo.wButtons & JOY_BUTTON2; break;
                                    case HARDWARE_JOYSTICK2: jbDown = jInfo.wButtons & JOY_BUTTON3; break;
                                    case HARDWARE_JOYSTICK3: jbDown = jInfo.wButtons & JOY_BUTTON4; break;
                                }
                                d += jbDown ? 1 : 0;
                            }
                        }
                        else{
                            if(m_action[ev].code[k] >= CTRL_EXTENDED_KEY){
                                ext  = TRUE;
                                c = m_action[ev].code[k]-CTRL_EXTENDED_KEY;
                            }
                            else{
                                ext = FALSE;
                                c = m_action[ev].code[k];
                            }
                            if(c != code) d += ((::GetKeyState(c) & 0x8000) ? 1 : 0)*keySens;
                            else          d += (buttonDown ? 1 : 0)*keySens;
                        }
                    }
                    if(cEv != -1) 
                    for(k = 0; k < MAX_CODES_ON_ACTION && m_action[cEv].code[k] > 0; ++k){ // direct actions
                        if(m_action[cEv].code[k] >= HARDWARE_JOYSTICK0){
                            if(g_hardware.m_ctrlUse.joystick){
                                ::joyGetPos(JOYSTICKID1, &jInfo);
                                jbDown = FALSE;
                                switch(m_action[cEv].code[k]){
                                    case HARDWARE_JOYSTICK0: jbDown = jInfo.wButtons & JOY_BUTTON1; break;
                                    case HARDWARE_JOYSTICK1: jbDown = jInfo.wButtons & JOY_BUTTON2; break;
                                    case HARDWARE_JOYSTICK2: jbDown = jInfo.wButtons & JOY_BUTTON3; break;
                                    case HARDWARE_JOYSTICK3: jbDown = jInfo.wButtons & JOY_BUTTON4; break;
                                }
                                d -= jbDown ? 1 : 0;
                            }
                        }
                        else{
                            if(m_action[cEv].code[k] >= CTRL_EXTENDED_KEY){
                                ext  = TRUE;
                                c = m_action[cEv].code[k]-CTRL_EXTENDED_KEY;
                            }
                            else{
                                ext = FALSE;
                                c = m_action[cEv].code[k];
                            }
                            if(c != code) d -= ((::GetKeyState(c) & 0x8000) ? 1 : 0)*keySens;
                            else          d -= (buttonDown ? 1 : 0)*keySens;
                        }
                    }
                    event[n+j]      = ev;
                    downAction[n+j] = d;
                    j++;
                }
            }   
            n += j;
            break;
        case CTRL_JOYSTICK_MOVE_MSG: 
            n = 1;
            event[0] = JOYSTICK_MOVE;
            break;
        case CTRL_MOUSE_MOVE_MSG:    
            n = 1;
            event[0] = MOUSE_MOVE;
            break;
        default: break;
    }
    return(n);
}
//=============================================================


//=============================================================
KR_Hardware::KR_Hardware(){
    m_ctrlPresence.mouse = TRUE;
}
//-------------------------------------------------------------
KR_Hardware::~KR_Hardware(){
}
//-------------------------------------------------------------
void KR_Hardware::addNotify(){
	KR_Object::addNotify();
	for(int i = 0; i < MAX_RECIVERS; ++i) m_reciver[i].type = NOT_USED;
	m_reciversNum   = 0;
	m_exclusiveMode = FALSE;
	m_demo.recording = FALSE;
	m_demo.playing = FALSE;

	m_lShift = (::GetAsyncKeyState(VK_LSHIFT)   & 0x8000) ? 1 : 0;
	m_rShift = (::GetAsyncKeyState(VK_RSHIFT)   & 0x8000) ? 1 : 0;
	m_lAlt   = (::GetAsyncKeyState(VK_LMENU)    & 0x8000) ? 1 : 0;
	m_rAlt   = (::GetAsyncKeyState(VK_RMENU)    & 0x8000) ? 1 : 0;
	m_lCtrl  = (::GetAsyncKeyState(VK_LCONTROL) & 0x8000) ? 1 : 0;
	m_rCtrl  = (::GetAsyncKeyState(VK_RCONTROL) & 0x8000) ? 1 : 0;
	m_returnMouse = FALSE;
    m_mouseUsed   = FALSE;
    m_joyCalibrateRange  = FALSE;
    m_joyCalibrateCenter = FALSE;
    m_ctrlPresence.keyboard = TRUE;
	m_ctrlPresence.mouse    = TRUE;
    m_mouseCaptured         = FALSE;
    m_joystickCaptured      = FALSE;
    m_ctrlPresence.joystick = ::joyGetNumDevs() ? TRUE : FALSE;
	
	m_ctrlTranslator.ClearCache();
	m_ctrlTranslator.ClearActions();

	ChangeRes();
}
//-------------------------------------------------------------
void KR_Hardware::removeNotify(){
    ActivateJoystickCapture(FALSE);
    ActivateMouseCapture(FALSE);
    ::ClipCursor(NULL);
}
//-------------------------------------------------------------
void KR_Hardware::ChangeRes(){
	::GetWindowRect(_gr_hWnd, &m_windowRect);
	m_winLen	    = m_windowRect.right-m_windowRect.left;
	m_winH	        = m_windowRect.bottom-m_windowRect.top;
	m_centerWindowX = m_winLen/2;
	m_centerWindowY = m_winH/2;

    if(_dL.currDevice->fullScreen || m_ctrlUse.mouse) HideMouseCursor();
}
//-------------------------------------------------------------
int KR_Hardware::SearchCode(const char *name) const{
	for(int i = 0; i < MAX_KEYS_CODES; ++i)
		if(!strcmp(name, m_key[i].name)) return(m_key[i].code);

	return(-1);
}
//-------------------------------------------------------------
const int *KR_Hardware::GetCodesByAction(int action) const{
    for(int i = 0; i < ACTIONS_NUM; ++i)
        if(m_ctrlTranslator.m_action[i].event == action) return(m_ctrlTranslator.m_action[i].code);
    return(NULL);
}
//-------------------------------------------------------------
int KR_Hardware::SearchAction(const char *name) const{
	for(int i = 0; i < ACTIONS_NUM; ++i)
		if(!strcmp(name, m_ctrlTranslator.m_action[i].name))
			return(m_ctrlTranslator.m_action[i].event);

	return(-1);
}
//-------------------------------------------------------------
const char *KR_Hardware::SearchName(int code) const{
	for(int i = 0; i < MAX_KEYS_CODES; ++i)
		if(m_key[i].code == code) return(m_key[i].name);

	return(NULL);
}
//-------------------------------------------------------------
const char *KR_Hardware::SearchActioName(int act) const{
    for(int i = 0; i < LAST_ACTION; ++i) 
        if(m_ctrlTranslator.m_action[i].event == act) return(m_ctrlTranslator.m_action[i].name);
    return(NULL);
}
//-------------------------------------------------------------
void KR_Hardware::ActivateMouseCapture(int activate){
	if(!m_ctrlPresence.mouse) return;
    
    switch(activate){
    case 1:
        if(!m_mouseCaptured){
            m_mouseCaptured = TRUE;
            ::SetCursorPos(m_windowRect.left+m_centerWindowX, m_windowRect.top+m_centerWindowY);
            ::ClipCursor(&m_windowRect);
            ::SetCapture(_gr_hWnd);
	    HideMouseCursor();
            m_ms.normX = 1;
            m_ms.normY = 1;
	    }
        break;
    case 0:
        if(m_mouseCaptured){
            m_mouseCaptured = FALSE;
	    ::ReleaseCapture();
            ::ClipCursor(NULL);
	    if(_dL.currDevice && !_dL.currDevice->fullScreen) ShowMouseCursor();
	    }
        break;
    }
}
//-------------------------------------------------------------
void KR_Hardware::ActivateJoystickCapture(int activate){
    int res;                     
    JOYCAPS joystickCaps;
    int period;
    
    if(!m_ctrlPresence.joystick) return;
    if(activate){
        if(!m_joystickCaptured){
            res = ::joyGetDevCaps(JOYSTICKID1, &joystickCaps, sizeof(joystickCaps));
        
            if(res != JOYERR_NOERROR) { echo("Jostick getCaps() failed.");return;}
            period = MinMax(50, (int)joystickCaps.wPeriodMin, (int)joystickCaps.wPeriodMax);
            m_js.normX = 1/(m_js.params.xMax-m_js.params.xMin);
            m_js.normY = 1/(m_js.params.yMax-m_js.params.yMin);

            res = ::joySetCapture(_gr_hWnd, JOYSTICKID1, period, FALSE); 
            if(res != JOYERR_NOERROR) {
                echo("Jostick capture failed."); 
                return; 
            }
            else {
                m_joystickCaptured = TRUE;
                echo("Joystick captured");
            }
        }
    }
    else{
        if(m_joystickCaptured){
            res = ::joyReleaseCapture(JOYSTICKID1); 
            if(res != JOYERR_NOERROR){
                echo("Jostick release failed.");
            }
            else{
                echo("Joystick realesed");
                m_joystickCaptured = FALSE;
            }
        }
    }
}
//-------------------------------------------------------------
void KR_Hardware::BeginCalibrateJoystick(){
    if(!m_ctrlPresence.joystick) return;

    if(!m_joystickCaptured) {
        g_GameConsole.PrintUrgent("Can't capture joystick", 4, GameConsole::CENTER);
        return;
    }
    m_joyCalibrateRange  = TRUE;
    m_joyCalibrateCenter = FALSE;

    m_js.params.xMin  = 65535;
    m_js.params.yMin  = 65535;
    m_js.params.xMax  = -65535;
    m_js.params.yMax  = -65535;
    m_js.params.cxMin = 0;
    m_js.params.cyMin = 0;
    m_js.params.cxMax = 0;
    m_js.params.cyMax = 0;

    g_GameConsole.PrintUrgent("Move joystick and press button", 60, GameConsole::CENTER);
}
//-------------------------------------------------------------
void KR_Hardware::EndCalibrateJoystick(){
    double dx, dy;

    g_GameConsole.DeleteUrgentMessge();

    m_js.params.dx    = m_js.params.xMax - m_js.params.xMin;
    m_js.params.dy    = m_js.params.yMax - m_js.params.yMin;
    dx                  = fabs(m_js.params.dx/2 - m_js.pos.x) + m_js.params.dx*0.1;
    dy                  = fabs(m_js.params.dy/2 - m_js.pos.y) + m_js.params.dy*0.1;
    m_js.params.cxMin = m_js.params.dx/2-dx;
    m_js.params.cyMin = m_js.params.dy/2-dy;
    m_js.params.cxMax = m_js.params.dx/2+dx;
    m_js.params.cyMax = m_js.params.dy/2+dy;
    m_js.params.cx    = (m_js.params.cxMax+m_js.params.cxMin)/2;
    m_js.params.cy    = (m_js.params.cyMax+m_js.params.cyMin)/2;

    m_joyCalibrateRange  = FALSE;
    m_joyCalibrateCenter = FALSE;
    m_js.calibrated      = TRUE;
}
//-------------------------------------------------------------
void KR_Hardware::BeginRecordDemo(const char *fName, const char *author){

	strncpy(m_demo.fName, fName, DEMO_MAX_FNAME_LEN);
	strncpy(m_demo.demoInfo.author, author, DEMO_AUTHOR_MAX_NAME_LEN);

	strncpy(m_demo.demoInfo.signature, DEMO_SIGNATURE, DEMO_SIGNATURE_LEN);
	m_demo.demoInfo.version        = DEMO_VERSION;
	m_demo.demoInfo.eventsNum      = 0;
	m_demo.demoInfo.beginModelTime = Session::m_realTimer->GetTime();
	m_demo.demoInfo.totalDuration  = 0;

	m_demo.prevTime  = Session::m_realTimer->GetTime();
	m_demo.fh = fopen(m_demo.fName, "wb");
	if(m_demo.fh == NULL){
		echo("Can't open file '%s' for write", m_demo.fName);
		return;
	}
	m_demo.recording = TRUE;

	fwrite(&m_demo.demoInfo, sizeof(m_demo.demoInfo), 1, m_demo.fh);
	fwrite(&m_demo.hardwareInfo, sizeof(m_demo.hardwareInfo), 1, m_demo.fh);
	fwrite(&m_demo.missionInfo, sizeof(m_demo.missionInfo), 1, m_demo.fh);

	WriteDemoEvent(DEMO_START);
}
//-------------------------------------------------------------
void KR_Hardware::StopRecordDemo(){
	if(!m_demo.recording){
		echo("Not recording.");
		return;
	}
	m_demo.recording = FALSE;
	m_demo.demoInfo.totalDuration = Session::m_realTimer->GetTime()-m_demo.demoInfo.beginModelTime;
	WriteDemoEvent(DEMO_END);
	fclose(m_demo.fh);
}
//-------------------------------------------------------------
WORD KR_Hardware::PackTime(double val) const{
	s_ASSERT(val >= 0 && val*DEMO_TICKS_PER_SECOND < 65535, "");
	return(val*DEMO_TICKS_PER_SECOND);
}
//-------------------------------------------------------------
double KR_Hardware::UnpackTime(WORD val) const{
	return((double)val/DEMO_TICKS_PER_SECOND);
}
//-------------------------------------------------------------
int KR_Hardware::SearchActionsOnCode(int code, int *action) const{
    int i, j, n, c;

    for(i = 0, n = 0; i < ACTIONS_NUM; ++i) 
    for(j = 0; j < MAX_CODES_ON_ACTION; ++j){
        c = m_ctrlTranslator.m_action[i].code[j];
        if(c == -1) break;
        if(c == code) action[n++] = m_ctrlTranslator.m_action[i].event;
    }
    return(n); // fixme
}
//-------------------------------------------------------------
void KR_Hardware::WriteDemoEvent(int eventTag, const int *act, int aNum){
	int i;
	byte b;
	double d;
	int a;

	s_ASSERT(aNum == 1 || (aNum > 1 && eventTag == DEMO_BUTTONS), "");
	b = (byte)eventTag;

	for(i = 0; i < aNum; ++i){
		if(act != NULL){
			a = (act[i] >= CTRL_ACTION_RELEASE) ? act[i]-CTRL_ACTION_RELEASE: act[i];
			if(m_ctrlTranslator.m_action[a].exclusive) continue;
		}
		d = Session::m_realTimer->GetTime();
		fwrite(&b, sizeof(byte), 1, m_demo.fh);	// write event
		fwrite(&d, sizeof(double), 1, m_demo.fh);

		switch(eventTag){
			case DEMO_START:	echo("Demo start"); break;
			case DEMO_END:		echo("Demo end"); break;
			case DEMO_BUTTONS:				 // свои события от клавиатуры

				echo("Event tag %i time %.1f action %i\n", (int)b, d, act[i]);
				
				fwrite(&act[i], sizeof(int), 1, m_demo.fh);
				break;
			case DEMO_MOUSE:				 // свои события от мыши
				break;
			case DEMO_TIME_UPDATE:
				break;
			default: s_ASSERTNQ("Unknown demo event.");
		}
	}
}
//-------------------------------------------------------------
void KR_Hardware::PlayDemo(const char *fName){
	m_demo.fh = fopen(fName, "rb");
	if(m_demo.fh == NULL){
		echo("Can't open demo file '%s'.", fName);
		return;
	}
	fread(&m_demo.demoInfo, sizeof(m_demo.demoInfo), 1, m_demo.fh);
	fread(&m_demo.hardwareInfo, sizeof(m_demo.hardwareInfo), 1, m_demo.fh);
	fread(&m_demo.missionInfo, sizeof(m_demo.missionInfo), 1, m_demo.fh);
	if(
		strcmp(m_demo.demoInfo.signature, DEMO_SIGNATURE) ||
		m_demo.demoInfo.version != DEMO_VERSION ||
		m_demo.demoInfo.eventsNum < 0 ||
		m_demo.demoInfo.beginModelTime < 0 ||
		m_demo.demoInfo.totalDuration < 0){
		
		echo("Bad demo header");
		fclose(m_demo.fh);
		return;
	}
	m_demo.playing  = TRUE;
	m_demo.prevTime = Session::m_realTimer->GetTime();
}
//-------------------------------------------------------------
void KR_Hardware::StopDemo(){
	m_demo.playing = FALSE;
	fclose(m_demo.fh);
}
//-------------------------------------------------------------
void KR_Hardware::ProcessDemoEvents(){
	int msgType;
	byte eventTag;
	int bEvent;
	WORD x, y;
	KR_Event event;
	double time;
	
	time = Session::m_realTimer->GetTime();
	while(m_demo.prevTime < time && !feof(m_demo.fh) && m_demo.playing){
		fread(&eventTag, sizeof(byte), 1, m_demo.fh);
		fread(&m_demo.prevTime, sizeof(double), 1, m_demo.fh);
		msgType = 0;
		switch(eventTag){
			case DEMO_START: echo("Demo play start.");break;
			case DEMO_END:	 
				StopDemo(); 
				echo("Demo play ends.");	
				break;
			case DEMO_BUTTONS:				 
				fread(&bEvent, sizeof(int), 1, m_demo.fh);
				m_actions[0] = bEvent;
				msgType = CTRL_BUTTONS_MSG;
				echo("Event tag %i time %.1f action %i\n", (int)eventTag, m_demo.prevTime, bEvent);
				break;
			case DEMO_MOUSE:				 
				fread(&x, sizeof(WORD), 1, m_demo.fh);
				fread(&y, sizeof(WORD), 1, m_demo.fh);
				m_ms.pos = CVector2(x, y);
				msgType = CTRL_MOUSE_MOVE_MSG;
				break;
			case DEMO_TIME_UPDATE:
				fread(&time, sizeof(double), 1, m_demo.fh);
				Session::m_realTimer->SetCurTime(time);
				echo("Demo play timer update");
				break;
			default: s_ASSERTNQ("Unknown demo event.");
		}
		if(msgType == CTRL_BUTTONS_MSG || msgType == CTRL_MOUSE_MOVE_MSG);
			/*TranslateCtrl2Action(event, type, code, repeat, n, m_actions, m_downAction,
				                 m_ctrlUse.mouse, m_ms.m_invX, m_ms.m_invY, m_ms.m_sensX, m_ms.m_sensY, m_ms.m_pos,
                                 m_ctrlUse.joystick, m_js.m_invX, m_js.m_invY, m_js.m_sensX, m_js.m_sensY, m_js.m_pos);
            */
	}
}
//-------------------------------------------------------------
int KR_Hardware::receiveEvent(KR_Event &event){
	int action;
	int code;
	int n, i, type;
	KR_ObjectID id;
    int repeat, buttonDown;

	switch( event.label ){
		case KR_WAKE_UP:			
            m_cursor = ::SetCursor(NULL);
            ::SetCursor(m_cursor);
            if(m_ctrlPresence.mouse) m_ctrlUse.mouse = g_levelAttr.get_int("msUse");
            else                     m_ctrlUse.mouse = FALSE;
            m_ms.invX  = g_levelAttr.get_int("msInvX");
            m_ms.invY  = g_levelAttr.get_int("msInvY");
            m_ms.sensX = g_levelAttr.get_double("msSensX");
            m_ms.sensY = g_levelAttr.get_double("msSensY");
            ActivateMouseCapture(m_ctrlUse.mouse);
            if(m_ctrlPresence.joystick) m_ctrlUse.joystick = g_levelAttr.get_int("jsUse");
            else                        m_ctrlUse.joystick = FALSE;
            ActivateJoystickCapture(m_ctrlUse.joystick);
            m_js.invX  = g_levelAttr.get_int("jsInvX");
            m_js.invY  = g_levelAttr.get_int("jsInvY");
            m_js.sensX = g_levelAttr.get_double("jsSensX");
            m_js.sensY = g_levelAttr.get_double("jsSensY");
            break;
		case CTRL_LINK_CONTROLS:	Link();         			    break;
		case CTRL_CLEAR:			ClearCache();		            break;
		case CTRL_LOAD_DEFAULT:		m_ctrlTranslator.SetDefault();	break;
		case CTRL_SET_EXCLUSIVE:	m_exclusiveMode = TRUE;			break;
		case CTRL_SET_NORMAL:		m_exclusiveMode = FALSE;		break;
        case CTRL_CALIBRATE_JOYSTICK: 
            if(m_ctrlUse.joystick){
                if(g_menu.IsActive()) g_menu.Deactivate();
                BeginCalibrateJoystick(); 
            }
            break;
        case CTRL_CHANGE_MOUSE_PARAMS:    
            if(m_ctrlPresence.mouse) m_ctrlUse.mouse = g_levelAttr.get_int("msUse");
            else                     m_ctrlUse.mouse = FALSE;
            ActivateMouseCapture(m_ctrlUse.mouse);
            m_ms.invX  = g_levelAttr.get_int("msInvX");
            m_ms.invY  = g_levelAttr.get_int("msInvY");
            m_ms.sensX = g_levelAttr.get_double("msSensX");
            m_ms.sensY = g_levelAttr.get_double("msSensY");
            break;
        case CTRL_CHANGE_JOYSTICK_PARAMS: 
            if(m_ctrlPresence.joystick) m_ctrlUse.joystick = g_levelAttr.get_int("jsUse");
            else                        m_ctrlUse.joystick = FALSE;
            ActivateJoystickCapture(m_ctrlUse.joystick);
            m_js.invX  = g_levelAttr.get_int("jsInvX");
            m_js.invY  = g_levelAttr.get_int("jsInvY");
            m_js.sensX = g_levelAttr.get_double("jsSensX");
            m_js.sensY = g_levelAttr.get_double("jsSensY");
            break;
		case CTRL_RECORD_DEMO:      
			event.data.open(EDO_READ)
						  .getStr(m_demo.fName, DEMO_MAX_FNAME_LEN)
						  .getStr(m_demo.demoInfo.author, DEMO_AUTHOR_MAX_NAME_LEN)
				      .close();
			
			BeginRecordDemo(m_demo.fName, m_demo.demoInfo.author);	
			break;
		case CTRL_STOP_RECORD:		StopRecordDemo();		break;
		case CTRL_STOP_DEMO:		StopDemo();				break;
		case CTRL_PLAY_DEMO:
            event.data.open(EDO_READ)
	                      .getStr(m_demo.fName, DEMO_MAX_FNAME_LEN)
                      .close();
			PlayDemo(m_demo.fName);
			break;
		case CTRL_DEL_SUBSCRIBE:
			for(i = 0; i < MAX_RECIVERS; ++i) m_reciver[i].type = NOT_USED;
			m_reciversNum = 0;
			break;
		case CTRL_SET_HARDWARE:
            event.data.open(EDO_READ)
	                      .getInt(m_ctrlPresence.keyboard)
						  .getInt(m_ctrlPresence.mouse)
						  .getInt(m_ctrlPresence.joystick)
                      .close();
			break;
		case CTRL_ENABLE_HARDWARE:
            event.data.open(EDO_READ)
	                      .getInt(m_ctrlUse.keyboard)
						  .getInt(m_ctrlUse.mouse)
						  .getInt(m_ctrlUse.joystick)
                      .close();
			if(m_ctrlPresence.keyboard) m_ctrlUse.keyboard = TRUE;
			if(m_ctrlPresence.mouse)    m_ctrlUse.mouse    = TRUE;
			if(m_ctrlPresence.joystick) m_ctrlUse.joystick = TRUE;

			ActivateMouseCapture(m_ctrlUse.mouse);
            ActivateJoystickCapture(m_ctrlUse.joystick);
			break;
		case CTRL_SUBSCRIBE:
            event.data.open(EDO_READ)
	                      .getObjectID(id)
						  .getInt(type)
                      .close();
			s_ASSERT(m_reciversNum < MAX_RECIVERS, "");
			for(i = 0; i < MAX_RECIVERS; ++i)
				if(m_reciver[i].type == NOT_USED){
					m_reciver[i].id  = id;
					s_ASSERT(type == NORMAL || type == EXCLUSIVE, "");
					m_reciver[i].type = type;
					break;
				}
			m_reciversNum++;
			s_ASSERT(i < MAX_RECIVERS, "");
			break;
		case CTRL_UNSUBSCRIBE:
            event.data.open(EDO_READ)
	                      .getObjectID(id)
                      .close();
			for(i = 0; i < MAX_RECIVERS; ++i)
				if(m_reciver[i].id == id){
					m_reciver[i].type = NOT_USED;
					break;
				}
			m_reciversNum--;
			s_ASSERT(i < MAX_RECIVERS && m_reciversNum >= 0, "");
			break;
		case CTRL_SET_CONTROL:
            event.data.open(EDO_READ)
						  .getInt(action)
						  .getInt(code)
                      .close();
			SetControl(action, code);
			break;
		case CTRL_REMOTE_HARDWARE_EVENT:break;
		case CTRL_REMOTE_EVENT:			break;
		case CTRL_REMOTE_EVENT_DATA:	break;
		case CTRL_REMOTE_EVENT_DATA_PREPARE:break;
		case CTRL_NETWORK_EVENT:		break;
		case CTRL_HARDWARE_EVENT:
			event.data.open(EDO_READ)
				          .getInt(type)		  
                          .getInt(code)
                          .getInt(buttonDown)
                          .getInt(repeat)
                      .close();
            n = m_ctrlTranslator.Translate(type, code, buttonDown, m_downAction, m_actions);
			double mX, mY, jX, jY;

            if(m_ctrlUse.mouse){
                mX = (m_ms.pos.x)*(m_ms.invX ? -1 : 1)*m_ms.sensX*m_ms.normX;
                mY = (m_ms.pos.y)*(m_ms.invY ? -1 : 1)*m_ms.sensY*m_ms.normY;
            }
            if(m_ctrlUse.joystick && m_js.calibrated){
                jX = jY = 0;
                if(m_js.pos.x >= m_js.params.cxMax)
                    jX = (m_js.pos.x-m_js.params.cx)/(m_js.params.xMax-m_js.params.cx);
                if(m_js.pos.x <= m_js.params.cxMin)
                    jX = (m_js.pos.x-m_js.params.cx)/(m_js.params.cx-m_js.params.xMin);
                if(m_js.pos.y >= m_js.params.cyMax)
                    jY = (m_js.pos.y-m_js.params.cy)/(m_js.params.yMax-m_js.params.cy);
                if(m_js.pos.y <= m_js.params.cyMin)
                    jY = (m_js.pos.y-m_js.params.cy)/(m_js.params.cy-m_js.params.yMin);
                jX *= (m_js.invX ? -1 : 1)*m_js.sensX;
                jY *= (m_js.invY ? -1 : 1)*m_js.sensY;
            }
            TranslateCtrl2Action(event, type, code, repeat, n, m_actions, m_downAction,
				                 m_ctrlUse.mouse, mX, mY, m_ctrlUse.joystick, jX, jY);
			break;
		default: return(0);
    }
    return(1);
}
//-------------------------------------------------------------
void KR_Hardware::TranslateCtrl2Action(KR_Event &event, int msgType, int code, int repeat, 
                                       int n, const int *ev, const double *downAction,
									   int um, double mx, double my, int uj, double jx, double jy){
	int i, j;
    double timeStamp = event.timeStamp;

	if(m_demo.recording){
		int type;
		switch(msgType){
			case CTRL_BUTTONS_MSG:		type = DEMO_BUTTONS; break;
			case CTRL_MOUSE_MOVE_MSG:	type = DEMO_MOUSE;    break;
			case CTRL_JOYSTICK_MOVE_MSG:type = DEMO_JOYSTICK; break;
			default: s_ASSERTNQ("");
		}
		if(n > 0) WriteDemoEvent(type, ev, n); // fixme
	}
	for(i = 0; i < MAX_RECIVERS; ++i)
        for(j = 0; j < n; ++j){
            if( (m_reciver[i].type == EXCLUSIVE && (!m_exclusiveMode || m_reciver[i].id == m_exclusiveReciver)) || 
                (m_reciver[i].type != NOT_USED && !m_exclusiveMode && !CtrlSet::m_action[ev[j]].exclusive) ){
                event.timeStamp     = timeStamp;
                event.source		= getObjectID();
				event.destination	= m_reciver[i].id;
				event.label			= msgType;
				switch(msgType){
					case CTRL_BUTTONS_MSG:
						event.data.open(EDO_WRITE)
							        .putInt(ev[j])		
                                    .putDouble(downAction[j])
                                    .putInt(code)
                                    .putInt(repeat)
								  .close();
						break;	
					case CTRL_MOUSE_MOVE_MSG:
                        event.data.open(EDO_WRITE)
                                     .putInt(um)
                                     .putDouble(mx)
                                     .putDouble(my)
								  .close();
						break;
					case CTRL_JOYSTICK_MOVE_MSG:
                        event.data.open(EDO_WRITE)
                                     .putInt(uj)
                                     .putDouble(jx)
                                     .putDouble(jy)
								  .close();
						break;
                    case CTRL_CHAR:
						event.data.open(EDO_WRITE)
									.putInt(code)		
								  .close();
                        break;
					default: s_ASSERTNQ("");
				}
				getContext()->addEvent(event);
			}
		}
}
//-----------------------------------------------------------
bool	KR_Hardware::dump(PIN_SaveFile & sf)
{
		if (!sf.WriteData( (char *) & m_ms, sizeof(HardwareData)  ))
			return false;

		return true;
}

bool	KR_Hardware::load(PIN_SaveFile & sf)
{
		if (!sf.GetData( (char *) & m_ms, sizeof(HardwareData)  ))
			return false;
		
		return true;
}

void	KR_Hardware::loadNotify()
{
	ChangeRes();
}

//-----------------------------------------------------------

bool KR_Hardware::SaveStaticData(PIN_SaveFile & sf)
{
	if (!sf.WriteData( (char *) CtrlSet::m_action, sizeof(Action) * ACTIONS_NUM) ||
		!sf.WriteData( (char *) m_key, sizeof(Key) * MAX_KEYS_CODES))
		return false;

	return true;
}

bool KR_Hardware::LoadStaticData(PIN_SaveFile & sf)
{

	if (!sf.GetData( (char *) CtrlSet::m_action, sizeof(Action) * ACTIONS_NUM) ||
		!sf.GetData( (char *) m_key, sizeof(Key) * MAX_KEYS_CODES))
		return false;

	return true;
}
//-------------------------------------------------------------
void KR_Hardware::MessageLoop(){
    MSG msg;
    
    if(m_demo.playing) ProcessDemoEvents();
    while(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
    if(m_ctrlUse.mouse && m_mouseCaptured){
        int cx, cy;
        POINT mPos;
        KR_Event event;

        cx = g_hardware.m_windowRect.left + g_hardware.m_centerWindowX;
        cy = g_hardware.m_windowRect.top  + g_hardware.m_centerWindowY;
        ::GetCursorPos(&mPos);
        ::SetCursorPos(cx, cy);
		m_ms.prevPos = m_ms.pos;
		m_ms.pos     = CVector2(mPos.x-cx, mPos.y-cy);
        if(m_ms.pos.x == 0 && m_ms.pos.y == 0) return;

	    if(Session::m_realTimer == NULL) return;
        event.source	  = getObjectID();
	    event.destination = getObjectID();
        event.timeStamp   = Session::m_realTimer->ConvertSysTime(m_msgTime);
	    event.label	      = CTRL_HARDWARE_EVENT;
	    event.data.open(EDO_WRITE)
	 	            .putInt(CTRL_MOUSE_MOVE_MSG)	   
                    .putInt(0)
                    .putInt(0)
                    .putInt(0)
                  .close();
	    getContext()->sendEventNow(event);
    }
}
//-----------------------------------------------------------
LRESULT CALLBACK KR_Hardware::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
	KR_Event event;
	int buttonDown, repeat;
	int button;
	int ext;
	int code;
    int ch;
    int msgType;
	int lShift, rShift, lAlt, rAlt, lCtrl, rCtrl;
    		
    m_msgTime = ::GetMessageTime();
    button  = -1;
	msgType	= 0;
    ch      = 0;
    repeat  = 0;
    ext     = 0; 
    code    = 0;
    switch (msg){
        case WM_PAINT: {
			PAINTSTRUCT ps;
            ::BeginPaint(hWnd,&ps);
		if (_dL.currDevice != NULL)  GRDumpScreen();
            ::EndPaint(hWnd,&ps);
            return(0);
        }
        case WM_ACTIVATEAPP:
            if (wParam) {
                ActivateMouseCapture(m_ctrlUse.mouse);
                ActivateJoystickCapture(m_ctrlUse.joystick);
                _gr_bRestoreSurf = 1;
            }
            else {
                ActivateMouseCapture(FALSE);
                ActivateJoystickCapture(FALSE);
            }
            break;
        case WM_SIZE:
            if(_dL.currDevice != NULL) ChangeRes();
            if (wParam == SIZE_RESTORED) _gr_bRestoreSurf = 1;
            break;
        case WM_MOVE:
            if(_dL.currDevice != NULL) ChangeRes();
            break;
        case WM_MOUSEMOVE: 
			break;
        case WM_CHAR:
            ch         = wParam;
            ext		   = lParam & 0x01000000;
			buttonDown = 1;
			msgType	   = CTRL_BUTTONS_MSG;
            break;
        case WM_DEADCHAR:
            ch         = wParam;
            ext		   = lParam & 0x01000000;
			buttonDown = 0;
			msgType	   = CTRL_BUTTONS_MSG;
            break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            repeat     = (lParam & 0x40000000) ? 1 : 0;	 // key auto repeat
			button     = wParam;
			ext		   = lParam & 0x01000000;
			buttonDown = 1;
			msgType	   = CTRL_BUTTONS_MSG;
            if(!ext)
            switch(button){
                case VK_HOME:  button = VK_NUMPAD7; break;
                case VK_LEFT:  button = VK_NUMPAD4; break;
                case VK_END:   button = VK_NUMPAD1; break;
                case VK_DOWN:  button = VK_NUMPAD2; break;
                case VK_NEXT:  button = VK_NUMPAD3; break;
                case VK_RIGHT: button = VK_NUMPAD6; break;
                case VK_PRIOR: button = VK_NUMPAD9; break;
                case VK_UP:    button = VK_NUMPAD8; break;
                case VK_CLEAR: button = VK_NUMPAD5; break;
                default: break;
            }
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			button     = wParam;
			ext		   = lParam & 0x1000000;
			buttonDown = 0;
			msgType	   = CTRL_BUTTONS_MSG;
            if(!ext)
            switch(button){
                case VK_HOME:  button = VK_NUMPAD7; break;
                case VK_LEFT:  button = VK_NUMPAD4; break;
                case VK_END:   button = VK_NUMPAD1; break;
                case VK_DOWN:  button = VK_NUMPAD2; break;
                case VK_NEXT:  button = VK_NUMPAD3; break;
                case VK_RIGHT: button = VK_NUMPAD6; break;
                case VK_PRIOR: button = VK_NUMPAD9; break;
                case VK_UP:    button = VK_NUMPAD8; break;
                case VK_CLEAR: button = VK_NUMPAD5; break;
                default: break;
            }
			break;
		case WM_LBUTTONDOWN: 
			if(m_ctrlUse.mouse){
				buttonDown = TRUE;
                button     = VK_LBUTTON;
				msgType	   = CTRL_BUTTONS_MSG;
			}
			break;
		case WM_LBUTTONUP:
			if(m_ctrlUse.mouse){
				buttonDown = FALSE;
                button     = VK_LBUTTON;
				msgType	   = CTRL_BUTTONS_MSG;
			}
			break;
		case WM_RBUTTONDOWN: 
			if(m_ctrlUse.mouse){
			    buttonDown = TRUE;
                button     = VK_RBUTTON;
				msgType	   = CTRL_BUTTONS_MSG;
			}
			break;
		case WM_RBUTTONUP:
			if(m_ctrlUse.mouse){
                buttonDown = FALSE;
                button     = VK_RBUTTON;
				msgType	   = CTRL_BUTTONS_MSG;
			}
			break;
		case WM_MBUTTONDOWN: 
			if(m_ctrlUse.mouse){
                buttonDown = TRUE;
                button     = VK_MBUTTON;
				msgType	   = CTRL_BUTTONS_MSG;
			}
			break;
		case WM_MBUTTONUP:
			if(m_ctrlUse.mouse){
				buttonDown = FALSE;
                button     = VK_MBUTTON;
                msgType	   = CTRL_BUTTONS_MSG;
			}
			break;
        case MM_JOY1MOVE:
            if(m_ctrlUse.joystick){
                m_js.prevPos = m_js.pos;
                m_js.pos.x   = LOWORD(lParam); 
                m_js.pos.y   = HIWORD(lParam); 

                if(m_joyCalibrateRange){
                    if(m_js.pos.x < m_js.params.xMin) m_js.params.xMin = m_js.pos.x;
                    if(m_js.pos.y < m_js.params.yMin) m_js.params.yMin = m_js.pos.y;
                    if(m_js.pos.x > m_js.params.xMax) m_js.params.xMax = m_js.pos.x;
                    if(m_js.pos.y > m_js.params.yMax) m_js.params.yMax = m_js.pos.y;
                    break;
                }
                msgType = CTRL_JOYSTICK_MOVE_MSG;
             }
             break;        
        case MM_JOY1BUTTONDOWN:
            if(m_ctrlUse.joystick){
                if(m_joyCalibrateRange){
                    m_joyCalibrateRange  = FALSE;
                    m_joyCalibrateCenter = TRUE;
                    g_GameConsole.PrintUrgent("Center joystik and press button", 10000, GameConsole::CENTER);
                }
                if(m_joyCalibrateCenter){ EndCalibrateJoystick(); break;} 
                buttonDown = TRUE;
                msgType    = CTRL_BUTTONS_MSG;
                button     = -1;
                if(wParam & JOY_BUTTON1CHG) button = HARDWARE_JOYSTICK0;
                if(wParam & JOY_BUTTON2CHG) button = HARDWARE_JOYSTICK1;
                if(wParam & JOY_BUTTON3CHG) button = HARDWARE_JOYSTICK2;
                if(wParam & JOY_BUTTON4CHG) button = HARDWARE_JOYSTICK3;
            }
            break;
        case MM_JOY1BUTTONUP:
            if(m_ctrlUse.joystick){
                if(m_joyCalibrateRange || m_joyCalibrateCenter) break;

                buttonDown = FALSE;
                msgType    = CTRL_BUTTONS_MSG;
                button     = -1;
                if(wParam & JOY_BUTTON1CHG) button = HARDWARE_JOYSTICK0;
                if(wParam & JOY_BUTTON2CHG) button = HARDWARE_JOYSTICK1;
                if(wParam & JOY_BUTTON3CHG) button = HARDWARE_JOYSTICK2;
                if(wParam & JOY_BUTTON4CHG) button = HARDWARE_JOYSTICK3;
            }
            break;
		default: break;
    }
    if(msgType == 0) return(::DefWindowProc(hWnd, msg, wParam, lParam));
	
	if(msgType == CTRL_BUTTONS_MSG){
        if((button == VK_SHIFT || button == VK_MENU || button == VK_CONTROL)){
            {//if(!repeat){
                lShift = (::GetKeyState(VK_LSHIFT)   & 0x8000) ? 1 : 0;
			    rShift = (::GetKeyState(VK_RSHIFT)   & 0x8000) ? 1 : 0;
			    lAlt   = (::GetKeyState(VK_LMENU)    & 0x8000) ? 1 : 0;
			    rAlt   = (::GetKeyState(VK_RMENU)    & 0x8000) ? 1 : 0;
			    lCtrl  = (::GetKeyState(VK_LCONTROL) & 0x8000) ? 1 : 0;
			    rCtrl  = (::GetKeyState(VK_RCONTROL) & 0x8000) ? 1 : 0;
                //echo("lCtrl %i rCtrl %i lAlt %i rAlt %i lShift %i rShift %i", lCtrl, rCtrl, lAlt ,rAlt ,lShift ,rShift);
			    if((button == VK_SHIFT) && lShift && !m_lShift && buttonDown){
				    code = VK_LSHIFT; m_lShift = 1;
			    }
			    if((button == VK_SHIFT) && !lShift && m_lShift && !buttonDown){
				    code = VK_LSHIFT; m_lShift = 0;
			    }
			    if((button == VK_SHIFT) && rShift && !m_rShift && buttonDown){
				    code = VK_RSHIFT; m_rShift = 1;
			    }
			    if((button == VK_SHIFT) && !rShift && m_rShift && !buttonDown){
				    code = VK_RSHIFT; m_rShift = 0;
			    }
			    if((button == VK_MENU) && lAlt && !m_lAlt      && buttonDown){
				    code = VK_LMENU; m_lAlt = 1;
			    }
			    if((button == VK_MENU) && !lAlt && m_lAlt      && !buttonDown){
				    code = VK_LMENU; m_lAlt = 0;
			    }
			    if((button == VK_MENU) && rAlt && !m_rAlt      && buttonDown){
				    code = VK_RMENU; m_rAlt = 1;
			    }
			    if((button == VK_MENU) && !rAlt && m_rAlt      && !buttonDown){
				    code = VK_RMENU; m_rAlt = 0;
			    }
			    if((button == VK_CONTROL) && lCtrl && !m_lCtrl && buttonDown){
				    code = VK_LCONTROL; m_lCtrl = 1;
			    }
			    if((button == VK_CONTROL) && !lCtrl && m_lCtrl && !buttonDown){
				    code = VK_LCONTROL; m_lCtrl = 0;
			    }
			    if((button == VK_CONTROL) && rCtrl && !m_rCtrl && buttonDown){
				    code = VK_RCONTROL; m_rCtrl = 1;
			    }
			    if((button == VK_CONTROL) && !rCtrl && m_rCtrl && !buttonDown){
				    code = VK_RCONTROL; m_rCtrl = 0;
                }
			}
		}
        else code = button + (ext ? CTRL_EXTENDED_KEY : 0);
		if(code == 0 && button == VK_SHIFT && !repeat){
			lShift = buttonDown;
			code   = VK_LSHIFT;
		}
		if(code == 0 && button == VK_MENU && !repeat){
			lAlt = buttonDown;
			code = VK_LMENU;
		}
		if(code == 0 && button == VK_CONTROL && !repeat){
			lCtrl = buttonDown;
			code  = VK_LCONTROL;
		} 
        if(code == 0 && msgType	== CTRL_BUTTONS_MSG) return(::DefWindowProc(hWnd, msg, wParam, lParam));
	}
    if(ch != 0){ 
        code    = ch;
        msgType = CTRL_CHAR;
    }
    if(Session::m_realTimer != NULL){
        event.source	  = getObjectID();
	    event.destination = getObjectID();
        event.timeStamp   = Session::m_realTimer->ConvertSysTime(m_msgTime);
	    event.label	      = CTRL_HARDWARE_EVENT;
	    event.data.open(EDO_WRITE)
	 	            .putInt(msgType)	   
                    .putInt(code)
                    .putInt(buttonDown)
                    .putInt(repeat)
                  .close();
	    getContext()->sendEventNow(event);
    }
    
    return(::DefWindowProc(hWnd, msg, wParam, lParam));
}
//-----------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
    return(g_hardware.WndProc(hWnd, msg, wParam, lParam));
}
