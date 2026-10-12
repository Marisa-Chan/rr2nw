#ifndef	__INPUT_H__
#define	__INPUT_H__

#include"filesys.h"

class	CUserKeyDefs	{
public:
	byte	m_left, m_right, m_up, m_down;
	byte	m_fire, m_strafe, m_rotleft, m_rotright;
	byte	m_accelerate, m_deccelerate, m_rearview;
	CUserKeyDefs();
	void Write(CTaggedFile &f);
	void Read(CTaggedFile &f);
	static	bool	IsReservedKey(int sc);
	static	TCchar	*KeyName(int sc);
};

class	CUserCtlDefs	{
public:
	CUserKeyDefs	m_keys;
	double			m_fMouseSensitivity, m_fJoystickSensitivity; // 0..1
	bool			m_bAbsoluteMouse;	// Absolute/Relative
	int				m_nResolution, m_nScreenOptimize;
	double			m_fHaze;
	CUserCtlDefs();
	void Write(CTaggedFile &f);
	void Read(CTaggedFile &f);
};

enum {
	SC_ESC = 1, SC_1 = 2, SC_2 = 3,
	SC_3 = 4, SC_4 = 5, SC_5 = 6, SC_6 = 7,
	SC_7 = 8, SC_8 = 9, SC_9 = 0xA, SC_0 = 0xB,
	SC_MINUS = 0xC, SC_PLUS = 0xD, SC_BACKSPACE = 0xE, SC_TAB = 0xF,
	SC_Q = 0x10, SC_W = 0x11, SC_E = 0x12, SC_R = 0x13,
	SC_T = 0x14, SC_Y = 0x15,  SC_U = 0x16, SC_I = 0x17,
	SC_O = 0x18, SC_P = 0x19, SC_LBRAK = 0x1A, SC_RBRAK = 0x1B,
	SC_ENTER = 0x1C, SC_CTRL = 0x1D,	SC_A = 0x1E, SC_S = 0x1F,
	SC_D = 0x20, SC_F = 0x21, SC_G = 0x22, SC_H = 0x23,
	SC_J = 0x24, SC_K = 0x25, SC_L = 0x26, SC_COLON = 0x27,
	SC_QUOTE = 0x28, SC_TILDE = 0x29, SC_LSHIFT = 0x2A, SC_BACKSLASH = 0x2B,
	SC_Z = 0x2C, SC_X = 0x2D, SC_C = 0x2E, SC_V = 0x2F,
	SC_B = 0x30, SC_N = 0x31, SC_M = 0x32, SC_COMMA = 0x33,
	SC_POINT = 0x34, SC_SLASH = 0x35, SC_RSHIFT = 0x36,
	SC_ALT = 0x38, SC_SPACE = 0x39, SC_F1 = 0x3B,
	SC_F2 = 0x3C, SC_F3 = 0x3D, SC_F4 = 0x3E, SC_F5 = 0x3F,
	SC_F6 = 0x40, SC_F7 = 0x41, SC_F8 = 0x42, SC_F9 = 0x43,
	SC_F10 = 0x44,
	SC_HOME = 0x47,
	SC_UP = 0x48,
	SC_PGUP = 0x49,
	SC_GREYMINUS = 0x4A,
	SC_LEFT = 0x4B,
	SC_RIGHT = 0x4D,
	SC_GREYPLUS = 0x4E,
	SC_END = 0x4F,
	SC_DOWN = 0x50,
	SC_PGDN = 0x51,
	SC_F11 = 0x57, SC_F12 = 0x58
};

class	CKeyBoard	{
public:
	enum MODE { M_NONE, M_RECORD, M_PLAYBACK };
private:
	#ifdef	MSDOS
		static void interrupt far (*m_Old9)();
	#endif
	static bool m_bKeyState[], m_bEventState[],
				m_bEventKey[], m_bEnableDefault, m_bCancelPB;
	static byte m_nKeyBuf[32];
	static struct SEvent { byte key; dword dwTime; } m_eventBuf[32];
	static volatile int  m_nFirstBuf, m_nLastBuf;
	static volatile int  m_nFirstEvent, m_nLastEvent;
	static MODE m_eMode, m_eEffMode;
	static FILE *m_pRecordFile;
	static dword m_dwRecTime, m_dwTrueTime;
	#ifdef	MSDOS
		static void interrupt Int9();
	#endif
public:
	static void CleanUp();
	static bool Install();
	static bool Restore();
	static bool Key(int sc);
	static byte GetKey();
	static byte WaitKey();
	static byte WaitKeyDef();
	static byte WaitKey(dword dwMaxTicks,bool (*pCallBack)(byte key)=NULL);
	static byte WaitKeyD(double fMaxSeconds,bool (*pCallBack)(byte key)=NULL);
	static void EnableDefault(bool bEnable) { m_bEnableDefault = bEnable; }
	static bool IsDefaultEnabled() { return m_bEnableDefault; }
	static void SetEventKey(byte key,bool bEvent=TRUE) {
												m_bEventKey[key] = bEvent; }
	static void ResetEventKeys();
	static byte GetEvent(dword &dwTime);
	static void UpdateEvent(byte sc) {
								m_bEventState[sc&0x7F] = !(sc&0x80); }
	static bool Event(int sc) {
		ASSERT(sc>=0 && sc<0x80); return m_bEventState[sc]; }
	static void ClearEvents();
	static void SetTimeAllEvents(dword dwTime);
	static void SetMode(MODE eMode,TCchar *szFileName=NULL);
	static MODE Mode() { return m_eMode; }
	static void EnableRecorder(bool bEnable) {
								m_eEffMode = bEnable ? m_eMode : M_NONE; }
	static bool IsRecorderEnabled() { return m_eEffMode != M_NONE; }
	static dword Time();
	static dword UpdatedTime();
	static void InitPBSynchro();
	static void MarkFrame();
	static void RecordData(void *pData,int nLen);
	static bool IsCancelPB() { return m_eMode == M_PLAYBACK && m_bCancelPB; }
	static bool ProcessKey(byte key,dword dwTime);
};


class	CJoystick	{
private:
	static	bool	m_bPresent, m_bEnabled, m_bActive;
	static	byte	m_nMask1, m_nMask2, m_nMaskX, m_nMaskY;
	static	int		m_nPosX, m_nPosY, m_nMaxCount;
	static	bool	m_bButton1, m_bButton2;
	static	bool	m_bFlipX, m_bFlipY;
	static	int		m_nMaxX, m_nMaxY, m_nMinX, m_nMinY, m_nCenterX, m_nCenterY;
	static	enum	CALSTEP { CAL_OFF, CAL_BR, CAL_UL, CAL_C, CAL_END }
															m_eCalibrate;
	enum	{ CALIBRATE_COUNT = 20 };
	static	int		Scan();
public:
	static	bool	Detect();
	static	bool	IsPresent() { return m_bPresent; }
	static	void	Enable(bool b) {
							m_bActive = m_bPresent && bool(m_bEnabled = b); }
	static	bool	IsEnabled() { return m_bEnabled; }

	static	void	BeginCalibrate();
	static	void	CalibrateC();
	static	void	CalibrateBR();
	static	void	CalibrateUL();
	static	void	EndCalibrate();

	struct	SStatus	{
		double	x, y;
		bool	b1, b2;
	};
	static	void	GetStatus(SStatus &s);
	static	void	GetButtonStatus(SStatus &s);
	static	void	WriteParms(CTaggedFile &f);
	static	void	ReadParms(CTaggedFile &f);
	static	void	SetFlip(bool x,bool y) { m_bFlipX = x; m_bFlipY = y; }
	static	void	GetFlip(bool &x,bool &y) { x = m_bFlipX; y = m_bFlipY; }
};

class	CMouse	{
private:
	static	bool	m_bPresent, m_bEnabled, m_bActive;
	static	byte	m_nCursorColor[];
	static	bool	m_bFlipX, m_bFlipY;
	static	CRect2	m_range, m_screenRange;
public:
	static	bool	Detect();
	static	bool	Reset() { return m_bEnabled ? Detect() : FALSE; }
	static	bool	IsPresent() { return m_bPresent; }
	static	void	Enable(bool b);
	static	bool	IsEnabled() { return m_bEnabled; }
	struct	SStatus	{
		double	x, y;
		bool	b1, b2;
	};
	static	void	CenterMouse();
	static	void	GetStatus(SStatus &s);
	static	void	GetOffset(int &x,int &y);
	static	void	ResetEvents();
	static	void	MatchCursor();
	static	void	DrawCursor(byte *pScreen,int nScrMaxX,int nScrMaxY);
	static	void	SetRange(TCCRect2 &range); //***
	static	void	MoveTo(double x, double y);
	struct	SDirectStatus {
		int  x,y;
		bool b1,b2;
	};
	static  void 	GetDirectStatus(SDirectStatus &scrs); //***
	static	void	SetScreenRange(TCCRect2 &screenRange) {
											m_screenRange = screenRange; }
	static	void	DirectMoveTo(int x,int y); //***
	static	void	SetFlip(bool x,bool y) { m_bFlipX = x; m_bFlipY = y; }
	static	void	GetFlip(bool &x,bool &y) { x = m_bFlipX; y = m_bFlipY; }
	static	void	WriteParms(CTaggedFile &f);
	static	void	ReadParms(CTaggedFile &f);
};


#if !defined(MSDOS) && !defined(__NT__)
#include<time.h>
#endif

extern "C" {
	#if defined(MSDOS) || defined(__NT__)
		bool	HookPIT(dword dwDivider);
		void	UnhookPIT();
		dword	GetPITCount();
		dword	GetPITGlobalLocal();
		dword	GetPITStatusAddr();
		dword	SetPITCallBack(void (*pCallBack)(),dword dwDivider);
		void	(*GetPITCallBack())();
	#endif
	//#define	PIT_FACTOR	64
	//#define	PIT_TCK	(1193180+65536/PIT_FACTOR/2)/(65536/PIT_FACTOR)
	//#define	PIT_TCKD	(1193180./(65536/PIT_FACTOR))
	#ifdef	__NT__
		dword	GetPITCountNT();
	#endif
}

class	CTimer	{
public:
	struct	STimerStatus	{
		dword	dwTime;
		dword	dwNewCommon, dwNewOwn;
	};
	#ifdef	__NT__
		static	double	m_fTicksms;
		static	dword	NT2PIT(dword dwNT) { return (dword)(dwNT*m_fTicksms); }
	#endif
private:
	static	bool	m_bInstalled;
	static	dword	m_dwTicks, m_dwGlobalLocal;
	static	double	m_fTicks;
	friend	class	CKeyBoard;
	#if defined(MSDOS)
		static	dword	TrueTime() { return m_bInstalled ? GetPITCount() : *(dword*)0x46C; }
	#elif defined(__NT__)
		static	dword	TrueTime() { return GetTickCount(); /*NT2PIT(GetPITCountNT());*/ }
	#else
		static	dword	TrueTime() { return clock(); }
	#endif
public:
	static	bool	Install(dword dwMultiply=64);
	static	void	Uninstall();
	static	dword	Time() { return CKeyBoard::Time(); }
	static	dword	UpdatedTime() { return CKeyBoard::UpdatedTime(); }
	#if defined(MSDOS) || defined(__NT__)
		static	dword	Ticks() { return 1000; /*m_bInstalled ? m_dwTicks : 18;*/ }	// Ticks per second
		static	double	TicksD() { return 1000.; /*m_bInstalled ? m_fTicks : 1193180./65536;*/ }	// Ticks per second
	#else
		static	dword	Ticks() { return CLOCKS_PER_SEC; }	// Ticks per second
		static	double	TicksD() { return CLOCKS_PER_SEC; }	// Ticks per second
	#endif
	static	dword	Diff(dword dwT1,dword dwT2) { return dwT1-dwT2; }
	static	dword	Add(dword dwT1,dword dwT2) { return dwT1+dwT2; }
	static	void	Wait(dword dwTicks);
	static	void	WaitD(double sec) { Wait((dword)(sec*TicksD()+0.5)); }
	static	dword	Sup() { return 0; }
	static	void	RdStatus() {}
	static	bool	IsInstalled() { return m_bInstalled; }
	static	void	UpdateGlobal() {
				#ifdef	MSDOS
					if(m_bInstalled) m_dwGlobalLocal = GetPITGlobalLocal();
				#endif
			}
	static	dword	Global2Local(dword dwGlobal) { return dwGlobal-m_dwGlobalLocal; }
	static	dword	Local2Global(dword dwLocal) { return dwLocal+m_dwGlobalLocal; }
	#if defined(MSDOS) || defined(__NT__)
		static	void	*GetStatusPtr() {
							RTCHECK(m_bInstalled,"Timer NeedInst");
							return (void*)GetPITStatusAddr(); }
	#endif
};

#endif	//	__INPUT_H__

