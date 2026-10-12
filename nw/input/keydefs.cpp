#ifndef	__INPUT_CPP__
#include"_input.h"
#endif


CUserKeyDefs::CUserKeyDefs()
{
	m_left = SC_LEFT;
	m_right = SC_RIGHT;
	m_up = SC_UP;
	m_down = SC_DOWN;
	m_fire = SC_CTRL;
	m_strafe = SC_ALT;
	m_rotleft = SC_PGUP;
	m_rotright = SC_PGDN;
	m_accelerate = SC_A;
	m_deccelerate = SC_Z;
	m_rearview = SC_R;
}

#ifndef	_BIG_ENDIAN
void CUserKeyDefs::Write(CTaggedFile &f)
{
	f.Descend("KEYS",TRUE,0);
		f.WriteByte(m_left);
		f.WriteByte(m_right);
		f.WriteByte(m_up);
		f.WriteByte(m_down);
		f.WriteByte(m_fire);
		f.WriteByte(m_strafe);
		f.WriteByte(m_rotleft);
		f.WriteByte(m_rotright);
		f.WriteByte(m_accelerate);
		f.WriteByte(m_deccelerate);
		f.WriteByte(m_rearview);
	f.Ascend();
}
#endif

void CUserKeyDefs::Read(CTaggedFile &f)
{
	f.Descend("KEYS",TRUE,0);
		f.ReadByte(m_left);
		f.ReadByte(m_right);
		f.ReadByte(m_up);
		f.ReadByte(m_down);
		f.ReadByte(m_fire);
		f.ReadByte(m_strafe);
		f.ReadByte(m_rotleft);
		f.ReadByte(m_rotright);
		f.ReadByte(m_accelerate);
		f.ReadByte(m_deccelerate);
		f.ReadByte(m_rearview);
	f.Ascend();
}

bool	CUserKeyDefs::IsReservedKey(int sc)
{
	return sc <= SC_0 || sc >= SC_F1 && sc <= SC_F10 ||
			sc >= SC_F11 && sc <= SC_F12 || !KeyName(sc);
}

TCchar	*CUserKeyDefs::KeyName(int sc)
{
	char	*szUserKeyNames[0x59] = {
		NULL, "Esc", "1", "2", "3", "4", "5", "6",	// 0-7
		"7", "8", "9", "0", "-", "+", "Bs", "Tab",	// 8-0xF
		"Q", "W", "E", "R", "T", "Y", "U", "I",		// 0x10-0x17
		"O", "P", "[", "]", "Enter", "Ctrl", "A", "S",	// 0x18-0x1F
		"D", "F", "G", "H", "J", "K", "L", ":",	// 0x20-0x27
		"'", "~", "LShift", "\\", "Z", "X", "C", "V",	// 0x28-0x2F
		"B", "N", "M", "<", ">", "/", "RShift", NULL,	// 0x30-0x37
		"Alt", "Space", NULL, "F1", "F2", "F3", "F4", "F5",	// 0x38-0x37
		"F6", "F7", "F8", "F9", "F10", NULL, NULL, "Home",	// 0x40-0x47
		"\x18", "PgUp", NULL, "\x1b", NULL, "\x1a", NULL, "End",	// 0x48-0x4F
		"\x19", "PgDn", NULL, NULL, NULL, NULL, NULL, "F11",	// 0x50-0x57
		"F12"	// 0x58
	};
	#if 0
	char	*szUserKeyNames[0x59] = {
		NULL, "ESC", "1", "2", "3", "4", "5", "6",	// 0-7
		"7", "8", "9", "0", "-", "+", "BS", "TAB",	// 8-0xF
		"Q", "W", "E", "R", "T", "Y", "U", "I",		// 0x10-0x17
		"O", "P", "[", "]", "ENTER", "CTRL", "A", "S",	// 0x18-0x1F
		"D", "F", "G", "H", "J", "K", "L", ":",	// 0x20-0x27
		"'", "~", "LSHIFT", "\\", "Z", "X", "C", "V",	// 0x28-0x2F
		"B", "N", "M", "<", ">", "/", "RSHIFT", NULL,	// 0x30-0x37
		"ALT", "SPACE", NULL, "F1", "F2", "F3", "F4", "F5",	// 0x38-0x37
		"F6", "F7", "F8", "F9", "F10", NULL, NULL, "HOME",	// 0x40-0x47
		"UP", "PGUP", NULL, "LEFT", NULL, "RIGHT", NULL, "END",	// 0x48-0x4F
		"DOWN", "PGDN", NULL, NULL, NULL, NULL, NULL, "F11",	// 0x50-0x57
		"F12"	// 0x58
	};
	#endif
	return	(uint)sc < 0x59 ? szUserKeyNames[sc] : NULL;
}

CUserCtlDefs::CUserCtlDefs()
{
	m_fMouseSensitivity = 1;
	m_fJoystickSensitivity = 1;
	m_bAbsoluteMouse = FALSE;
	m_nResolution = 0;
	m_nScreenOptimize = 5;
	m_fHaze = 0.5;
}

#ifndef	_BIG_ENDIAN
void CUserCtlDefs::Write(CTaggedFile &f)
{
	f.Descend("CTLS",FALSE,0);
		f.Descend("CTLH",TRUE,0);
			f.WriteDouble(m_fMouseSensitivity);
			f.WriteDouble(m_fJoystickSensitivity);
			f.WriteBool(m_bAbsoluteMouse);
			f.WriteInt(m_nResolution);
			f.WriteInt(m_nScreenOptimize);
			f.WriteDouble(m_fHaze);
		f.Ascend();
		m_keys.Write(f);
	f.Ascend();
}
#endif

void CUserCtlDefs::Read(CTaggedFile &f)
{
	f.Descend("CTLS",FALSE,0);
		f.Descend("CTLH",TRUE,0);
			f.ReadDouble(m_fMouseSensitivity);
			f.ReadDouble(m_fJoystickSensitivity);
			f.ReadBool(m_bAbsoluteMouse);
			f.ReadInt(m_nResolution);
			f.ReadInt(m_nScreenOptimize);
			f.ReadDouble(m_fHaze);
		f.Ascend();
		m_keys.Read(f);
	f.Ascend();
}

