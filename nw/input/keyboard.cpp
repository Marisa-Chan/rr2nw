#ifndef	__INPUT_CPP__
#include"_input.h"
#endif

#ifdef	MSDOS
	void interrupt far (*CKeyBoard::m_Old9)() = NULL;
#endif
bool CKeyBoard::m_bKeyState[0x80];
bool CKeyBoard::m_bEventState[0x80];
bool CKeyBoard::m_bEventKey[0x80];
byte CKeyBoard::m_nKeyBuf[32];
CKeyBoard::SEvent CKeyBoard::m_eventBuf[32];
volatile int  CKeyBoard::m_nFirstBuf, CKeyBoard::m_nLastBuf;
volatile int  CKeyBoard::m_nFirstEvent, CKeyBoard::m_nLastEvent;
bool CKeyBoard::m_bEnableDefault = FALSE;
bool CKeyBoard::m_bCancelPB = FALSE;
CKeyBoard::MODE	CKeyBoard::m_eMode = M_NONE;
CKeyBoard::MODE	CKeyBoard::m_eEffMode = M_NONE;
FILE *CKeyBoard::m_pRecordFile = NULL;
dword	CKeyBoard::m_dwRecTime, CKeyBoard::m_dwTrueTime;

bool playBack_error = FALSE;


bool CKeyBoard::ProcessKey(byte key,dword dwTime)
{
	if( m_bEventKey[key&0x7F] && m_bKeyState[key&0x7F] != !(key&0x80) ) {
		int nLastEvent = (m_nLastEvent+1)%32;
		if( nLastEvent != m_nFirstEvent )	{
			m_eventBuf[m_nLastEvent].key = key;
			m_eventBuf[m_nLastEvent].dwTime = dwTime;
			m_nLastEvent = nLastEvent;
		}
	}
	if( bool(m_bKeyState[key&0x7F] = !(key&0x80)) ) {
		int nLastBuf = (m_nLastBuf+1)%32;
		if( nLastBuf != m_nFirstBuf ) {
			m_nKeyBuf[m_nLastBuf] = key;
			m_nLastBuf = nLastBuf;
		}
	}
	return TRUE;
}

#if defined(MSDOS)
void interrupt CKeyBoard::Int9()
{
	unsigned key = inp(0x60);
	if( m_bEventKey[key&0x7F] && m_bKeyState[key&0x7F] != !(key&0x80) ) {
		int nLastEvent = (m_nLastEvent+1)%32;
		if( nLastEvent != m_nFirstEvent )	{
			m_eventBuf[m_nLastEvent].key = (byte)key;
			m_eventBuf[m_nLastEvent].dwTime = CTimer::TrueTime();
			m_nLastEvent = nLastEvent;
		}
	}
	if( bool(m_bKeyState[key&0x7F] = !(key&0x80)) ) {
		int nLastBuf = (m_nLastBuf+1)%32;
		if( nLastBuf != m_nFirstBuf ) {
			m_nKeyBuf[m_nLastBuf] = (byte)key;
			m_nLastBuf = nLastBuf;
		}
	}
	if( m_bEnableDefault ) {
		*(word*)0x41A = *(word*)0x41c;
		_chain_intr(m_Old9);
	}
	else {
		unsigned nOld61 = inp(0x61);
		outp(0x61,nOld61|0x80);
		outp(0x61,nOld61);
		outp(0x20,0x20);
	}
}
#endif

void	CKeyBoard::CleanUp()
{
	for( int i = 0 ; i < 0x80 ; i++ )
		m_bKeyState[i] = m_bEventState[i] = FALSE;
	m_nFirstBuf = m_nLastBuf = 0;
	m_nFirstEvent = m_nLastEvent = 0;
}

bool	CKeyBoard::Install()
{
	CleanUp();
	for( int  i = 0 ; i < 0x80 ; i++ )	m_bEventKey[i] = FALSE;
	#ifdef	MSDOS
		if( m_Old9 )	return FALSE;
		m_Old9 = _dos_getvect(9);
		_dos_setvect(9,Int9);
	#endif
	return	TRUE;
}

bool	CKeyBoard::Restore()
{
	#ifdef	MSDOS
		if( m_Old9 )	{
			_dos_setvect(9,m_Old9);
			m_Old9 = NULL;
			return TRUE;
		}
		return FALSE;
	#else
		return TRUE;
	#endif
}

void	CKeyBoard::SetTimeAllEvents(dword dwTime)
{
	for( int i = m_nFirstEvent ; i != m_nLastEvent ; i = (i+1)%32 )
		m_eventBuf[i].dwTime = dwTime;
}

void	CKeyBoard::ClearEvents()
{
	for( int  i = 0 ; i < 0x80 ; i++ )	m_bEventState[i] = FALSE;
	m_nFirstEvent = m_nLastEvent = 0;
}

void	CKeyBoard::ResetEventKeys()
{
	for( int  i = 0 ; i < 0x80 ; i++ )	m_bEventKey[i] = FALSE;
}

bool CKeyBoard::Key(int sc) {
	ASSERT(sc>=0 && sc<0x80);
	if(m_eEffMode==M_PLAYBACK) {
		bool bState=0;
		if(fread(&bState,1,1,m_pRecordFile)!=1) {
			#ifdef _DEBUG
				RTCHECK(0,"PlayBack Complete");
			#else
				playBack_error = TRUE;
			#endif
		}
		return bState;
	}
	bool bState = m_bKeyState[sc];
	if(m_eEffMode==M_RECORD) fwrite(&bState,1,1,m_pRecordFile);
	return bState;
}

byte	CKeyBoard::GetKey()
{
	byte nKey;
	if( m_eEffMode == M_PLAYBACK ) {
		if (playBack_error) {
			return SC_ESC;
		}
		while( m_nLastBuf != m_nFirstBuf ) {
			if(m_nKeyBuf[m_nFirstBuf]==SC_ESC) {
				//RTCHECK(0,"PlayBack Cancelled");
				m_bCancelPB = TRUE;
			}
			m_nFirstBuf = (m_nFirstBuf+1)%32;
		}
		if(fread(&nKey,1,1,m_pRecordFile)!=1) {
			#ifdef _DEBUG
				RTCHECK(0,"PlayBack Complete");
			#else
				playBack_error = TRUE;
				return SC_ESC;
			#endif
		}
		return nKey;
	}
	nKey = 0;
	if( m_nLastBuf != m_nFirstBuf ) {
		nKey = m_nKeyBuf[m_nFirstBuf];
		m_nFirstBuf = (m_nFirstBuf+1)%32;
	}
	if( m_eEffMode == M_RECORD )
		fwrite(&nKey,1,1,m_pRecordFile);
	return	nKey;
}

byte	CKeyBoard::WaitKey()
{
	if( m_eEffMode != M_PLAYBACK )  while( m_nFirstBuf == m_nLastBuf );
	return GetKey();
}

byte	CKeyBoard::WaitKeyDef()
{
	if( m_eEffMode == M_PLAYBACK )	return GetKey();
	byte nKey = 0;
	while( m_nFirstBuf == m_nLastBuf );
	nKey = m_nKeyBuf[m_nFirstBuf];
	m_nFirstBuf = (m_nFirstBuf+1)%32;
	dword dwTime0 = CTimer::TrueTime(), dwDeltaT = CTimer::TicksD()*0.05+0.5;
	while( CTimer::Diff(CTimer::TrueTime(),dwTime0) < dwDeltaT )
		if( m_nFirstBuf != m_nLastBuf ) {
			nKey = m_nKeyBuf[m_nFirstBuf];
			m_nFirstBuf = (m_nFirstBuf+1)%32;
		}
	if( m_eEffMode == M_RECORD )
		fwrite(&nKey,1,1,m_pRecordFile);
	return nKey;
}

byte	CKeyBoard::WaitKey(dword dwMaxTicks,bool (*pCallBack)(byte))
{
	if( m_eEffMode == M_PLAYBACK )	return GetKey();
	dword	dwTime0 = CTimer::TrueTime();
	byte nKey = 0;
	while( CTimer::Diff(CTimer::TrueTime(),dwTime0) < dwMaxTicks ) {
		byte n = 0;
		if( m_nFirstBuf != m_nLastBuf )	{
			n = m_nKeyBuf[m_nFirstBuf];
			m_nFirstBuf = (m_nFirstBuf+1)%32;
		}
		if( !pCallBack || pCallBack(n) )	{ nKey = n; break; }
	}
	if( m_eEffMode == M_RECORD ) {
		fwrite(&nKey,1,1,m_pRecordFile);
	}
	return nKey;
}

byte	CKeyBoard::GetEvent(dword &dwTime)
{
	if( m_eEffMode == M_PLAYBACK ) {
		m_nLastEvent = m_nFirstEvent;
		byte nKey;
		if(fread(&nKey,1,1,m_pRecordFile)!=1) {
			#ifdef _DEBUG
				RTCHECK(0,"PlayBack Complete");
			#else
				playBack_error = TRUE;
				nKey = SC_ESC;
			#endif
		}
		if(nKey!=0&&fread(&dwTime,4,1,m_pRecordFile)!=1) {
			#ifdef _DEBUG
				RTCHECK(0,"Err in playback");
			#else
				playBack_error = TRUE;
				nKey = SC_ESC;
			#endif
		}
		return nKey;
	}
	byte nKey = 0;
	if( m_nLastEvent != m_nFirstEvent )	{
		nKey = m_eventBuf[m_nFirstEvent].key;
		dwTime = m_eventBuf[m_nFirstEvent].dwTime;
		m_nFirstEvent = (m_nFirstEvent+1)%32;
		//m_bEventState[nKey&0x7F] = !(nKey&0x80);
	}
	if( m_eEffMode == M_RECORD ) {
		fwrite(&nKey,1,1,m_pRecordFile);
		if(nKey!=0)
			fwrite(&dwTime,4,1,m_pRecordFile);
	}
	return	nKey;
}

void	CKeyBoard::SetMode(CKeyBoard::MODE eMode,TCchar *szFileName)
{
	m_bCancelPB = FALSE;
	switch(eMode) {
		case M_NONE:
			if(m_eMode!=M_NONE)	fclose(m_pRecordFile); break;
		case M_RECORD:
			ASSERT(m_eMode==M_NONE);
			m_pRecordFile=fopen(szFileName,"wb");
			RTCHECK(m_pRecordFile,"RecFile create");
			break;
		case M_PLAYBACK:
			ASSERT(m_eMode==M_NONE);
			m_pRecordFile=CFileResource::FOpenCurrent(szFileName);
			RTCHECK(m_pRecordFile,"RecFile open");
			playBack_error = FALSE;
			break;
		default:ASSERT(0);
	}
	m_eMode = eMode;
	m_eEffMode = M_NONE;
	InitPBSynchro();
}

dword CKeyBoard::Time()
{
  dword dwTime;
	if(m_eEffMode==M_PLAYBACK) {
		if(fread(&dwTime,4,1,m_pRecordFile)!=1) {
			#ifdef _DEBUG
				RTCHECK(0,"PlayBack Complete");
			#else
				playBack_error = TRUE;
				dwTime = CTimer::TrueTime();
			#endif
		}
		return dwTime;
	}
	dwTime = CTimer::TrueTime();
	if(m_eEffMode==M_RECORD)	fwrite(&dwTime,4,1,m_pRecordFile);
	return dwTime;
}

dword CKeyBoard::UpdatedTime()
{
  dword dwTime;
	if(m_eEffMode==M_PLAYBACK) {
		if(fread(&dwTime,4,1,m_pRecordFile)!=1) {
			#ifdef _DEBUG
				RTCHECK(0,"PlayBack Complete");
			#else
				playBack_error = TRUE;
			#endif
		}
		dword dwTrueTime;
		#ifndef _DEBUG
			if (dwTime-m_dwRecTime > (dword)Round(CTimer::TicksD()*0.2)) {
				playBack_error = TRUE;
			}
		#endif
		while( (dwTrueTime=CTimer::TrueTime())-m_dwTrueTime<Min(dwTime-m_dwRecTime, (dword)Round(CTimer::TicksD()*0.2)));
//		while( (dwTrueTime=CTimer::TrueTime())-m_dwTrueTime<dwTime-m_dwRecTime);
		m_dwTrueTime = dwTrueTime;
		m_dwRecTime = dwTime;
		return dwTime;
	}
	dwTime = CTimer::TrueTime();
	if(m_eEffMode==M_RECORD)	{
		fwrite(&dwTime,4,1,m_pRecordFile);
	}
	return dwTime;
}

void	CKeyBoard::InitPBSynchro()
{
	switch( m_eMode ) {
		case M_RECORD: {
			dword dwTime = CTimer::TrueTime();
			fwrite(&dwTime,4,1,m_pRecordFile);
		}
		break;
		case M_PLAYBACK: {
			if(fread(&m_dwRecTime,4,1,m_pRecordFile)!=1)
				RTCHECK(0,"PlayBack Complete");
			m_dwTrueTime = CTimer::TrueTime();
		}
		break;
	}
}

void	CKeyBoard::MarkFrame()
{
	#if 0
	static char *szDesc = "FRAME";
	switch(m_eEffMode) {
		case M_RECORD: fwrite(szDesc,5,1,m_pRecordFile); break;
		case M_PLAYBACK: {
			char s[5];
			VERIFY(fread(s,5,1,m_pRecordFile)==1);
			VERIFY(memcmp(szDesc,s,5)==0);
			break;
		}
	}
	#endif
}

void	CKeyBoard::RecordData(void *pData,int nLen)
{
	switch(m_eMode) {
		case M_RECORD: fwrite(pData,nLen,1,m_pRecordFile); break;
		case M_PLAYBACK:
			if(fread(pData,nLen,1,m_pRecordFile)!=1) {
				#ifdef _DEBUG
					RTCHECK(0,"Playback Complete");
				#else
					playBack_error = TRUE;
				#endif
			}
			break;
	}
}

byte	CKeyBoard::WaitKeyD(double fMaxSeconds,bool (*pCallBack)(byte))
{
	return WaitKey(fMaxSeconds*CTimer::TicksD()+0.5,pCallBack);
}
// ----------------------------------------------------------------------

//             yet another joystick
#ifdef	MSDOS


#define inportb inp
#define outportb outp
#define inport inpw
#define outport outpw

#define OK	0

#define LA_BASE	0x220

typedef unsigned short int word;

word base_address;
int mil_joy = 0;

int try_base(word base)
{
  int i;

	if ((inportb(base + 3) == 0xff) ||
		(inportb(base + 2) == 0xff) ||
		((word)inport(base + 4) == 0xffff)) {
		return !OK;
	}
	outportb(base + 3, 0x98);	// control word
	outportb(base + 5, 0);	// channel 0
	outportb(base + 4, 1);	// run
/*	printf("%x = %02x %02x %02x %02x %02x %02x\n",
		base,
		inportb(base),
		inportb(base+1),
		inportb(base+2),
		inportb(base+3),
		inportb(base+4),
		inportb(base+5));
*/
	if (inportb(base + 2) & 0x80) {
		return !OK;
	}

	for( i = 10000; i > 0; i--) {
		if ((inportb(base + 2) & 0x80) == 0x80) {
			return OK;
		}
	}
	return !OK;

}

int find_base(word base)
{
  static word bases[] =
	{ 0x200, 0x210, 0x220, 0x230, 0x300, 0x310, 0x320, 0x330 };
  int i;

	if (mil_joy) {

	if (base != 0) {
		base_address = base;
		return OK;
	}
	for (i = 0; i < sizeof(bases)/sizeof(bases[0]); i++) {
		if (try_base(bases[i]) == OK) {
			base_address = bases[i];
			return OK;
		}
	}
	mil_joy = 0;
	return !OK;

	}
	return !OK;
}

int read_ADC(int channel)
{
  int i;
	outportb(base_address + 5, channel);			// channel
	outportb(base_address + 4, 1);	// run
	for( i = 1000; i > 0; i--) {
		if ((inportb(base_address + 2) & 0x80) == 0x80) {
			return inport(base_address + 4) & 0xfff;
		}
	}
	return -1;
}

word read_PPI(void)
{
	return (word)inportb(base_address + 0);
}

void write_PPI(word data)
{
	outportb(base_address + 1, data);
}
#endif	// MSDOS
/*
main()
{
  long cnt;
  int data;

	if (find_base(0) != OK) {
		printf("There is no LA-70M3 board\n");
		return 1;
	} else {
		printf("LA-70M3 found on address %x\n", base_address);
	}
base_address = 0x200;
	printf("Press any key to continue...");
	if (getch() == 27) // ESC
		return 1;
		printf("\n");
	while(!kbhit()) {
		printf("%04d %04d %04d %02x\r", read_ADC(8), read_ADC(9), read_ADC(10), read_PPI());
	}
	getch();
	return 0;
}
*/


// -------------


bool	CJoystick::m_bPresent = FALSE, CJoystick::m_bEnabled = FALSE,
					CJoystick::m_bActive = FALSE;
byte	CJoystick::m_nMask1, CJoystick::m_nMask2,
						CJoystick::m_nMaskX, CJoystick::m_nMaskY;
int		CJoystick::m_nPosX = 0, CJoystick::m_nPosY = 0,
							CJoystick::m_nMaxCount = 500;
bool	CJoystick::m_bButton1 = FALSE, CJoystick::m_bButton2 = FALSE;
bool	CJoystick::m_bFlipX = FALSE, CJoystick::m_bFlipY = FALSE;
int		CJoystick::m_nMaxX = 500, CJoystick::m_nMaxY = 500,
			CJoystick::m_nMinX = 0, CJoystick::m_nMinY = 0,
			CJoystick::m_nCenterX = 250, CJoystick::m_nCenterY = 250;
CJoystick::CALSTEP CJoystick::m_eCalibrate = CJoystick::CAL_OFF;

#ifndef	_BIG_ENDIAN
void	CJoystick::WriteParms(CTaggedFile &f)
{
	f.Descend("JOY_",TRUE,0);
		f.WriteBool(m_bEnabled);
		f.WriteInt(m_nMinX);
		f.WriteInt(m_nMinY);
		f.WriteInt(m_nCenterX);
		f.WriteInt(m_nCenterY);
		f.WriteInt(m_nMaxX);
		f.WriteInt(m_nMaxY);
		f.WriteBool(m_bFlipX);
		f.WriteBool(m_bFlipY);
	f.Ascend();
}
#endif

void	CJoystick::ReadParms(CTaggedFile &f)
{
	f.Descend("JOY_",TRUE,0);
		f.ReadBool(m_bEnabled);	m_bActive = m_bPresent && m_bEnabled;
		f.ReadInt(m_nMinX);
		f.ReadInt(m_nMinY);
		f.ReadInt(m_nCenterX);
		f.ReadInt(m_nCenterY);
		f.ReadInt(m_nMaxX);
		f.ReadInt(m_nMaxY);
		f.ReadBool(m_bFlipX);
		f.ReadBool(m_bFlipY);
	f.Ascend();
}

int	CJoystick::Scan()
{
	#ifndef MSDOS
		return 0;
	#else

	if (mil_joy) {
        byte n = (byte)read_PPI();

        m_bButton1 = !(n & 0x20);
        m_bButton2 = !(n & 0x10);

        m_nPosX= 2048 - (read_ADC(8) - 2000);
        m_nPosY = read_ADC(9) - 2000;
        return 1;

    }

	_disable(); //(void)inp(0x71);
	outp(0x201,0);
	byte n = (byte)inp(0x201);
	m_bButton1 = !(n & m_nMask1);
	m_bButton2 = !(n & m_nMask2);
	byte	nLowFlags = 0;
	for( int  i = 0 ; nLowFlags != 3 && i < m_nMaxCount ; i++ ) {
		n = (byte)inp(0x201);
		if( !(nLowFlags&1) && (n&m_nMaskX)==0 )	nLowFlags |= 1, m_nPosX = i;
		if( !(nLowFlags&2) && (n&m_nMaskY)==0 )	nLowFlags |= 2, m_nPosY = i;
	}
	_enable(); //(void)inp(0x71);
	if( !(nLowFlags&1) )	m_nPosX = i;
	if( !(nLowFlags&2) )	m_nPosY = i;
	return	i;
	#endif
}

void	CJoystick::BeginCalibrate()
{
	ASSERT( m_eCalibrate == CAL_OFF );
	m_eCalibrate = CAL_BR;
	m_nMaxCount *= 5;
}

void	CJoystick::CalibrateBR()
{
	ASSERT( m_eCalibrate == CAL_BR );
	int  nMaxCount = 0;
	m_nMaxX = m_nMaxY = 0;
	for( int  i = 0 ; i < CALIBRATE_COUNT ; i++ ) {
		nMaxCount = Max(nMaxCount,Scan());
		m_nMaxX += m_nPosX;
		m_nMaxY += m_nPosY;
	}
	m_nMaxCount = nMaxCount;
	m_nMaxX = (m_nMaxX+CALIBRATE_COUNT/2)/CALIBRATE_COUNT;
	m_nMaxY = (m_nMaxY+CALIBRATE_COUNT/2)/CALIBRATE_COUNT;
	if( m_nMaxX < 2 )	m_nMaxX = 2;
	if( m_nMaxY < 2 )	m_nMaxY = 2;
	if( m_nCenterX >= m_nMaxX )	m_nCenterX = m_nMaxX-1;
	if( m_nCenterY >= m_nMaxY )	m_nCenterY = m_nMaxY-1;
	m_eCalibrate = CAL_UL;
}

void	CJoystick::CalibrateUL()
{
	ASSERT( m_eCalibrate == CAL_UL );
	m_nMinX = m_nMinY = 0;
	for( int  i = 0 ; i < CALIBRATE_COUNT ; i++ ) {
		Scan();
		m_nMinX += m_nPosX;
		m_nMinY += m_nPosY;
	}
	m_nMinX = (m_nMinX+CALIBRATE_COUNT/2)/CALIBRATE_COUNT;
	m_nMinY = (m_nMinY+CALIBRATE_COUNT/2)/CALIBRATE_COUNT;
	if( m_nMinX >= m_nMaxX-1 )	m_nMinX = m_nMaxX-2;
	if( m_nMinY >= m_nMaxY-1 )	m_nMinY = m_nMaxY-2;
	if( m_nMinX >= m_nCenterX )	m_nCenterX = m_nMinX+1;
	if( m_nMinY >= m_nCenterY )	m_nCenterY = m_nMinY+1;
	m_eCalibrate = CAL_C;
}

void	CJoystick::CalibrateC()
{
	ASSERT( m_eCalibrate == CAL_C );
	m_nCenterX = m_nCenterY = 0;
	for( int  i = 0 ; i < CALIBRATE_COUNT ; i++ ) {
		Scan();
		m_nCenterX += m_nPosX;
		m_nCenterY += m_nPosY;
	}
	m_nCenterX = (m_nCenterX+CALIBRATE_COUNT/2)/CALIBRATE_COUNT;
	m_nCenterY = (m_nCenterY+CALIBRATE_COUNT/2)/CALIBRATE_COUNT;
	if( m_nMaxX <= m_nCenterX )	m_nMaxX = m_nCenterX+1;
	if( m_nMaxY <= m_nCenterY )	m_nMaxY = m_nCenterY+1;
	if( m_nMinX >= m_nCenterX )	m_nMinX = m_nCenterX-1;
	if( m_nMinY >= m_nCenterY )	m_nMinY = m_nCenterY-1;
	m_eCalibrate = CAL_END;
}

void	CJoystick::EndCalibrate()
{
	ASSERT( m_eCalibrate == CAL_END );
	m_eCalibrate = CAL_OFF;
}

bool	CJoystick::Detect()
{
	m_bPresent = FALSE; m_bActive = FALSE;
	#ifndef	MSDOS
		return FALSE;
	#else

    int i;
    if (find_base(LA_BASE) == OK) {

        mil_joy = 1;

    } else {
        outp(0x201,0);
        byte    nPresent1, nPresent2;
        {
            byte    n  = byte(inp(0x201) & 15);
            if( n != 15 && n != 3 && n != 12 )  return FALSE;
            nPresent1 = (n&3)==3;
            nPresent2 = (n&12)==12;
        }
        byte    nPresentFlags = 0;
        dword   dwTime0 = CTimer::Time();
        for( int i = 0 ; nPresentFlags!=3 &&
                                i < 0x70000000/20/CALIBRATE_COUNT ; i++ ) {
            dword   dwTime = CTimer::Time();
            if( dwTime >= dwTime0 + CTimer::Ticks() || dwTime < dwTime0 )   break;
            byte    n = (byte)inp(0x201);
            if( (n&3)==0 )  nPresentFlags |= nPresent1;
            if( (n&12)==0 ) nPresentFlags |= nPresent2;
        }
        if( !nPresentFlags )    return FALSE;
        m_nMaxCount = i*2;
        if( nPresentFlags & 1 ) {
            m_nMask1 = 0x10;
            m_nMask2 = 0x20;
            m_nMaskX = 1;
            m_nMaskY = 2;
        }
        else {
            m_nMask1 = 0x40;
            m_nMask2 = 0x80;
            m_nMaskX = 4;
            m_nMaskY = 8;
        }
        //dwTime0 = CTimer::Time();
        //while( CTimer::Time()-dwTime0 < CTimer::Ticks()/10 );
    }                                        // delay for joystick relaxation
	m_nCenterX = m_nCenterY = 0;
	const int CALIBRATE_COUNT1 = 5;
	for( i = 0 ; i < CALIBRATE_COUNT1 ; i++ ) {
		Scan();
		m_nCenterX += m_nPosX;
		m_nCenterY += m_nPosY;
	}
	m_nCenterX = (m_nCenterX+CALIBRATE_COUNT1/2)/CALIBRATE_COUNT1;
	m_nCenterY = (m_nCenterY+CALIBRATE_COUNT1/2)/CALIBRATE_COUNT1;
	if( m_nCenterX < 1 )	m_nCenterX = 1;
	if( m_nCenterY < 1 )	m_nCenterY = 1;
	m_nMaxX = m_nCenterX*2;
	m_nMaxY = m_nCenterY*2;
	m_nMinX = m_nMinY = 0;
	m_bPresent = TRUE; m_bActive = m_bEnabled;
	return	TRUE;
	#endif
}

void	CJoystick::GetButtonStatus(CJoystick::SStatus &s)
{
	#ifdef MSDOS
	    if (mil_joy) {
	        byte n = (byte)read_PPI();

	        s.b1 = !(n & 0x10);
	        s.b2 = !(n & 0x20);
	        return;

	    }
	#endif

	if( !m_bActive ) {
		s.b1 = s.b2 = FALSE;
		return;
	}
	#ifdef MSDOS
		outp(0x201,0);
		byte n = (byte)inp(0x201);
		s.b1 = !(n & m_nMask1);
		s.b2 = !(n & m_nMask2);
	#endif
}

void	CJoystick::GetStatus(CJoystick::SStatus &s)
{
//	ASSERT(m_eCalibrate == CAL_OFF);
	if( !m_bActive ) {
		s.b1 = s.b2 = FALSE;
		s.x = s.y = 0;
		return;
	}
	Scan();
	s.b1 = m_bButton1;
	s.b2 = m_bButton2;
	s.x = m_nPosX-m_nCenterX;
	s.y = m_nPosY-m_nCenterY;

	if( s.x >= 0 )
		s.x /= m_nMaxX-m_nCenterX;
	else
		if( (s.x /= m_nCenterX-m_nMinX) < -1 )	s.x = -1;

	if( s.y >= 0 )
		s.y /= m_nMaxY-m_nCenterY;
	else
		if( (s.y /= m_nCenterY-m_nMinY) < -1 )	s.y = -1;

	if( fabs(s.x) < 0.1 )	s.x = 0;
	if( fabs(s.y) < 0.1 )	s.y = 0;
	if( m_bFlipX )	s.x = -s.x;
	if( m_bFlipY )	s.y = -s.y;
}

// ----------------------------------------------------------------------

bool	CTimer::m_bInstalled = FALSE;
dword	CTimer::m_dwTicks, CTimer::m_dwGlobalLocal = 0;
double	CTimer::m_fTicks;
#if	defined(__NT__)
	double	CTimer::m_fTicksms;
#endif

bool	CTimer::Install(dword dwDivider)
{
	#ifdef	MSDOS
		if(m_bInstalled) return TRUE;
		if(!bool(m_bInstalled = HookPIT(dwDivider))) return FALSE;
	#endif
	m_dwGlobalLocal = 0;
	m_dwTicks = (1193180+dwDivider/2)/dwDivider;
	m_fTicks = 1193180./dwDivider;
	#if defined(__NT__)
		m_fTicksms = m_fTicks/1000;
		m_bInstalled = TRUE;
	#endif
	return TRUE;
}

void	CTimer::Uninstall()
{
	if(m_bInstalled) {
		#ifdef	MSDOS
			UnhookPIT();
		#endif
		m_bInstalled = FALSE;
	}
}

void	CTimer::Wait(dword dwTicks)
{
	dword dwTime0 = TrueTime();
	while(Diff(TrueTime(),dwTime0)<dwTicks);
}

#ifdef	__NT__
extern "C" dword GetPITCountNT()
{
	return GetTickCount();
}
#endif

