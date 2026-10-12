#ifndef __HARDWARE__
#define	__HARDWARE__

#include "message\hardmsg.h"
#include "kernel\h\s_debug.h"
#include "kernel\h\object.h"
#include "kernel\h\timer.h"
#include "mathlib.h"
#include "graph.h"

#define MAX_KEYS_CODES 2*CTRL_EXTENDED_KEY+10
#define MAX_KEY_NAME_LEN 15
#define MAX_ACT_NAME_LEN 25
#define MAX_CODES_ON_ACTION 5
#define MAX_DEVICE_BUTTONS 4
#define MAX_ACTIONS_ON_KEY 5
#define MAX_RECIVERS	   15
#define DEMO_MAX_FNAME_LEN 50
#define DEMO_SIGNATURE_LEN 10
#define DEMO_AUTHOR_MAX_NAME_LEN 20
#define DEMO_EVENT_CACHE_SIZE 0x00008FFF // 32 Kb
#define DEMO_SIGNATURE "NW-DEMO"
#define DEMO_TIME_UPDATE_INTERVAL 10000 // msec
#define DEMO_TICKS_PER_SECOND 1000
#define DEMO_MAX_EVENT_SIZE 200
#define DEMO_VERSION 0x00010000
#define HARDWARE_JOYSTICK0 500
#define HARDWARE_JOYSTICK1 501
#define HARDWARE_JOYSTICK2 502
#define HARDWARE_JOYSTICK3 503

enum CtrlActions {
	MOVE_FORWARD = 0,
	MOVE_BACKWARD,
	STRAFE_LEFT,
	STRAFE_RIGHT,
	STRAFE_UP,
	STRAFE_DOWN,
	LOOK_UP,
	LOOK_DOWN,
	TURN_LEFT,
	TURN_RIGHT,
	ROLL_LEFT,
	ROLL_RIGHT,
	TURRET_LEFT,
	TURRET_RIGHT,
	STRAFE,
	FIRE_PRIMARY,
	FIRE_SECONDARY,
	CHANGE_WEAPON,
	WEAPON0,
	WEAPON1,
	WEAPON2,
	WEAPON3,
	WEAPON4,
	WEAPON5,
	WEAPON6,
	WEAPON7,
	WEAPON8,
	WEAPON9,
	LOCK_TAGERT,
	JUMP,
	CROUCH,
	FORCEAGE,
	VIEW_CABINE,
	VIEW_EXTERN_FIXED,
	VIEW_EXTERN_CLEAVER,
	VIEW_LAND_FIXED,
	VIEW_LAND_CLEAVER,
	VIEW_NEXT_WINGMAN,
	VIEW_PREV_WINGMAN,
	VIEW_WINGMAN,
	VIEW_NEXT_OBJECT,
	VIEW_PREV_OBJECT,
	VIEW_OBJECT,
	VIEW_OBSERVER,
	CENTER_VIEW,
	TOGGLE_FOLLOW_SCOUT,
	LEAVE_VEHICLE,
	GET_VEHICLE,
	ZOOM_IN,
	ZOOM_OUT,
	BRF_BEGIN_RECORD,
	BRF_END_RECORD,
	BRF_ADD_CTRL_POINT,
	BRF_ADD_POS,
	BRF_SAVE_FLIGHT,
	BRF_SAVE_POS,
	EXEC,
    CONSOLE,
    LIGHT,
    TOGGLE_LIGHT,
	STOP_VEHICLE,
	EXIT,
	CHANGE_VEHICLE,
	ENABLE_MOUSE,
	DISABLE_MOUSE,
	MOUSE_MOVE,
	JOYSTICK_MOVE,
	SYS_KEY, 
	
	SAY,
	RECORD_DEMO,
	PLAY_DEMO,
	WRITE_DEMO,
	STOP_DEMO,
	
    MENU_TOGGLE,
    MENU_UP,
    MENU_DOWN,
    MENU_LEFT,
    MENU_RIGHT,
    MENU_ENTER,
    MENU_DEL,

    TOGGLE_GAME_MAP,

    DMAP_TOGGLE,
    DMAP_TOGGLE_OBST,
    DMAP_TOGGLE_ROUTE,
    DMAP_TOGGLE_OBJ,
    DMAP_TOGGLE_OBJINFO,
    DMAP_SCROLL_UP,
    DMAP_SCROLL_DOWN,
    DMAP_SCROLL_LEFT,
    DMAP_SCROLL_RIGHT,
    DMAP_TOGGLE_FOLLOW_MODE,
    DMAP_NEXT_MISSION,
    DMAP_PREVIOUS_MISSION,
    DMAP_TEXT_BOX_UP,
    DMAP_TEXT_BOX_DOWN,

    DROP_ARTEFACT,

	LAST_ACTION,
};

#pragma warning 004 10

#define ACTIONS_NUM ((int)LAST_ACTION)

enum {
	NOT_USED,
	NORMAL,
	EXCLUSIVE
};

typedef struct{
	char name[MAX_ACT_NAME_LEN];
	int  event;
    int  cEvent;
	int  code[MAX_CODES_ON_ACTION];
	int  exclusive;
}Action;

typedef struct{
	char name[MAX_KEY_NAME_LEN];
	int  code;
    int  links;
} Key;


class DeviceStatus{
public:
    DeviceStatus(int mb){
		s_ASSERT(mb >= 0 && mb <= MAX_DEVICE_BUTTONS, "");
		maxButtonNum = mb;
		for(int i = 0; i < MAX_DEVICE_BUTTONS; ++i) button[i] = 0;
		invX = invY = 0;
		sensX = sensY = 1;
		pos = prevPos = CVector2(0, 0);
	}
	virtual ~DeviceStatus() {}

	int GetButtonNum() const { return(bNum); }
	void SetButtonNum(int n){
		s_ASSERT(n >= 0 && n <= maxButtonNum, "");
		bNum = n;
	}
	int button[MAX_DEVICE_BUTTONS];
	int invX, invY, oldInvX, oldInvY;
	double sensX, sensY, oldSensX, oldSensY;
    double normX, normY;

	CVector2 pos;
	CVector2 prevPos;
protected:
	int bNum;
	int maxButtonNum;
};

class MouseStatus : public DeviceStatus{
public:
	MouseStatus() : DeviceStatus(3) {}
};

class JoystickStatus : public DeviceStatus{
public:
	JoystickStatus() : DeviceStatus(4) {calibrated = FALSE;}
    struct {
        double xMin, xMax, yMin, yMax;     // move rect
        double cxMin, cxMax, cyMin, cyMax; // center rect
        double cx, cy;
        double dx, dy;
    } params;
    int calibrated;
};

class CtrlSet{
public:
	CtrlSet();
	virtual ~CtrlSet() {}
	void Link();
	void ClearCache();
    void RemoveCode(int action, int code);
    void SetDefault() { Link(); } // fixme
	void SetControl(int action, int code);
	int  Translate(int msgType, int code, int buttonDown, double *downAction, int *event) const;
        void ClearActions();

	static Action m_action[ACTIONS_NUM];
protected:
	int m_codeCache	   [MAX_KEYS_CODES][MAX_ACTIONS_ON_KEY];
	int m_codeCacheSize[MAX_KEYS_CODES];
};


typedef struct Control {
	Control() { mouse = 1; keyboard = 1; joystick = 0;};
	int keyboard;
	int mouse;
	int joystick;
}    DeviceCtrl;


typedef struct {
	MouseStatus		m_ms;
	JoystickStatus	m_js;
	
	DeviceCtrl m_ctrlPresence;
	DeviceCtrl m_ctrlUse;
	
	int m_actions[MAX_ACTIONS_ON_KEY*3];
    double m_downAction[MAX_ACTIONS_ON_KEY*3];
	
	int m_returnMouse, m_mouseUsed;
	
	int m_invertMouseX, m_invertMouseY;
	int m_invertJoystickX, m_invertJoystickY;
	
	struct ReciverTag{
		KR_ObjectID id;
		int type;
	} m_reciver[MAX_RECIVERS];
	int m_reciversNum;
	
    KR_ObjectID m_exclusiveReciver;
    int m_exclusiveMode;
    int m_joyCalibrateRange, m_joyCalibrateCenter;
    LONG m_msgTime;
	
    int m_mouseCaptured;
    int m_joystickCaptured;
	
	CtrlSet m_ctrlTranslator;
	int m_lShift, m_rShift, m_lAlt, m_rAlt, m_lCtrl, m_rCtrl;
	int m_prevAbsDX, m_prevAbsDY;
} HardwareData;


class KR_Hardware:	public KR_Object, 
					public HardwareData{
public:
	enum {
		DEMO_BUTTONS,				 // ñâîè ñîáûòèÿ îò êëàâèàòóğû
			DEMO_MOUSE,					 // ñâîè ñîáûòèÿ îò ìûøè
			DEMO_JOYSTICK,				 // ñâîè ñîáûòèÿ îò äæîéñòèêà
			DEMO_SETUP_PLAYER,			 // ïåğâîå ïîÿâëåíèå èãğîêà
			DEMO_REMOVE_OBJECT,			 //	óäàëåíèå îáúåêòà ñî ñöåíû (íàïğèìåğ, èãğîê ñîæğàë áîíóñ)
			DEMO_PLAYER_SET_POS,		 //	óñòàíîâèòü èãğîêà â ïîçèöèş
			DEMO_PLAYER_SET_VEHICLE,	 // óñòàíîâèòü äâèäèòåëü èãğîêà (ñî ñêèíîì)
			DEMO_PLAYER_RESPAWN,		 //	èãğîê âîçğîäèëñÿ, ìàòü åãî òàê 
			DEMO_PLAYER_DIE,			 //	èãğîê ïîäîõ
			DEMO_PLAYER_SET_FIRE,		 // óñòàíîâèòü èëè ñáğîñèòü ñòğåëüáó
			DEMO_PLAYER_SOUND,			 // çâóê, èçäàâàåìûé èãğîêîì
			DEMO_SAY,					 // êòî-òî ÷å òî ñêàçàë
			DEMO_PAUSE,					 //	íàæàòà ïàóçà
			DEMO_START,					 //	íà÷àëî äåìêè
			DEMO_END,					 //	êîíåó äåìêè
			DEMO_CHANGE_LEVEL,			 //	ñìåíà óğîâíÿ ...
			DEMO_TIME_UPDATE,			 // ıòî ñïåóèàëüíîå ñîáâòèå ñèíõğîíèçàöèè êàæäûå n ñåêóíä 
	} DEMO_EVENT_ID;
    KR_Hardware();
    ~KR_Hardware();
	
	virtual int receiveEvent(KR_Event &event);
    void addNotify   ();
    void removeNotify();
	
	static Key m_key[MAX_KEYS_CODES];
	
	int UseKeyboard() const { return(m_ctrlUse.keyboard); }
	int UseMouse()    const{ return(m_ctrlUse.mouse); }
	int UseJoystick() const { return(m_ctrlUse.joystick); }
	int  SearchCode(const char *name) const;
	void ChangeRes();
	
    void GetExclusiveMode(KR_ObjectID objID){ m_exclusiveMode = TRUE;  m_exclusiveReciver = objID;}
    void ReleaseExclusiveMode(){ m_exclusiveMode = FALSE; }
	
    void HideMouseCursor() { ::SetCursor(NULL); ::ShowCursor(FALSE);}
    void ShowMouseCursor() { ::SetCursor(m_cursor); ::ShowCursor(TRUE);}
	
    void ClearCache() {
        for(int i = 0; m_key[i].code != 0; ++i) m_key[i].links = 0;
        m_ctrlTranslator.ClearCache();
    }
	const char *SearchName(int code) const;
	int  SearchAction(const char *name) const;
    int SearchActionsOnCode(int code, int *action) const;
    const char *SearchActioName(int act) const;
    const int *GetCodesByAction(int action) const;
    int SearchCodeCachePos(int code) const{
        for(int i = 0; m_key[i].code != 0; ++i) if(m_key[i].code == code) return(i);
        return(-1);
    }
	
    void SetControl(int action, int code){
        m_ctrlTranslator.SetControl(action, code);
        int pos = SearchCodeCachePos(code);
        m_key[pos].links++;
    }
    void RemoveCode(int action, int code) { m_ctrlTranslator.RemoveCode(action, code);}
    void Link()  {m_ctrlTranslator.Link();}
	
    void MessageLoop();
	void ActivateMouseCapture(int activate);
    void ActivateJoystickCapture(int activate);
	void PlayDemo(const char *fName);
	void BeginRecordDemo(const char *fName, const char *author);
	
	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
	virtual void	loadNotify();
	virtual bool	shouldDump () { return false; }  // Hardware is saved as dedicated object

	static bool SaveStaticData(PIN_SaveFile & sf);
	static bool LoadStaticData(PIN_SaveFile & sf);

    LRESULT CALLBACK WndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
protected:
	enum {
		NOT_USED,
			NORMAL,
			EXLUSIVE
	};
    void BeginCalibrateJoystick();
    void EndCalibrateJoystick();
	
	void StopDemo();
	void ProcessDemoEvents();
	void WriteDemoEvent(int event, const int *acts = NULL, int aNum = 1);
	void StopRecordDemo();
	WORD PackTime(double val) const;
	double UnpackTime(WORD val) const;
	void TranslateCtrl2Action(KR_Event &event, int msgType, int code, int repeat, int n, const int *ev, const double *de,
		int um, double mx, double my, int uj, double jx, double jy);
	
    HCURSOR m_cursor;

	struct DemoTag{
		struct DemoInfoTag{
			char signature[DEMO_SIGNATURE_LEN];
			char author[DEMO_AUTHOR_MAX_NAME_LEN];
			int  version;
			int  eventsNum;
			double beginModelTime;
			double totalDuration;
		}demoInfo;
		
		struct HardwareInfoTag{
			int mInvX, mInvY;
			double mSensX, mSensY;
			int jInvX, jInvY;
			double jSensX, jSensY;
		}hardwareInfo;
		
		struct MissionInfoTag{
		}missionInfo;
		
		double prevTime;
		
		char fName[DEMO_MAX_FNAME_LEN];
		FILE *fh;
		int recording;
		int playing;
	}m_demo;
	
	RECT  m_windowRect;
	int m_winLen, m_winH;
	int m_centerWindowX, m_centerWindowY;
};

extern KR_Hardware g_hardware;

#pragma warning 004 1

#endif //__HARDWARE__ 
