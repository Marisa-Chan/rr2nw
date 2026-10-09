/*
   Класс TFilcPlayer.

   Обеспечивает проигрывание flic-файлов.

   Интерфейсные функции:
      int Play(COUTConsole *con, CConfigFile &cfg, const char *flicSectName,
               byte *buf, int scrX, int scrY, byte *pal64);

   Используется классом CBriefing.

***************************************************************************

   Классы TPoint2D, T2DObject, T2DAgregat.

   Обеспечивают работу 2D графики брифингов (используется в спецэффектах).

   Используется классом CBriefing.

***************************************************************************

   Класс CBriefing - экспортируется.

   Интерфейсные функции:
      void PlayBriefing(const char *fName);

      // Tools functions
      void BeginRecord(int maxPointsNum);
      void EndRecord();
      void WriteFlight(const char *defFileName) const;
      int  AddCtrlPoint(double prevFlyTime);
      int  AddObjectPos(double prevFlyTime);
      void WriteObjects(const char *defFileName) const;

   Использование класса для проигрывания брифинга: Вызвать PlayBriefing().
      Ограничения: нельзя динамически менять разрешение экрана
      (на это не расчитаны спецэффекты)
   Использование класса для записи полета и/или расстановки объектов:
      1) BeginRecord()
      2) { AddCtrlPoint() | AddObjectPos() |
           WriteFlight() | WriteObjects()  | EndRectod() }

      BeginRecord()  начинает запись чего-либо. Должна быть вызвана
                     перед началом записи новой траектории и/или объектов.

      EndRecord()    сбрасывает текущие записи (и траектории и объекты).

      AddCtrlpoint() добавляет очередную контрольную точку к
                     текущей траектории.

      AddObjectPos() добавляет очередной объект. С консоли запрашивается
                     коментарий к объекту (строка).

      WriteFlight()  записывает текущюю траекторию на диск.
                     Траектория не уничтожается. Имя файла запрашивается.

      WriteObjects() записывает объекты на диск. Объекты не уничтожаются.
                     Имя файла запрашивается.

      Пример использования - в MAINPROC.CPP.

============ СОПРОВОЖДЕНИЕ МОДУЛЯ briefing.cpp =============================

    void VRectangle() ......................... U
    void VPoly() .............................. U
    void VCircle() ............................ U

    void TFlicPlayer::SetPal64() .............. U
    void TFlicPlayer::Load_FLI_COLOR() ........
    void TFlicPlayer::Load_FLI_COLOR256() ..... U
    byte *TFlicPlayer::VHLine() ............... U
    byte *TFlicPlayer::VHLineW() .............. U
    void TFlicPlayer::Load_FLI_LC() ........... U
    void TFlicPlayer::Load_FLI_WORD_LC() ...... U
    void TFlicPlayer::Load_FLI_BRUN() ......... U
    void TFlicPlayer::Load_FLI_COPY() ......... U
    void TFlicPlayer::Load_FLI_BLACK() ........
    int  TFlicPlayer::LoadChunk() ............. U
    int  TFlicPlayer::LoadFrame() ............. U
    int  TFlicPlayer::Play() .................. U
    void TFlicPlayer::OutScreen() ............. U
    void TFlicPlayer::ChangeResEvent() ........ U
    int  TFlicPlayer::IsPlay() ................ INLINE

         CBriefing::CBriefing() ............... U
         CBriefing::~CBriefing() .............. U
    void CBriefing::PlayBriefing() ............ U
    void CBriefing::BeginRecord() ............. U
    void CBriefing::EndRecord() ............... U
    int  CBriefing::AddObjectPos() ............ U
    void CBriefing::WriteObjects() ............ U
    void CBriefing::WriteFlight() ............. U
    int  CBriefing::AddCtrlPoint() ............ U
    void CBriefing::TranslatePalette() ........
    int  CBriefing::PlayFlic() ................ U
    int  CBriefing::PlayFlight() .............. U
    int  CBriefing::PerformFly() .............. U
    void CBriefing::CinemaBugs() .............. U
    void CBriefing::TVBugs() .................. U
    void CBriefing::CrystalBugs() ............. U
    void CBriefing::GypnoseBugs() ............. !
    void CBriefing::Effects() ................. U
    void CBriefing::ChangeResEvent() .......... U
    int  CBriefing::Random() .................. INLINE

    void T2DObject::Move() .................... U
    void T2DObject::RedrawAt() ................ U
    void T2DObject::Sync() .................... U
    void T2DObject::Advance() ................. Убита за неиспользование
    void T2DObject::Rotate() .................. Убита за неиспользование
    void T2DObject::Scale() ................... Убита за неиспользование
    void T2DAgregat::RedrawAt() ............... U


*/

#define LAST_H__SCENE
#include "game.h"
#include "segment1.h"
#include "cnlmaps.h"
#include "briefing.h"
#include "stypes.h"
#include "flc.h"
#include<windows.h>
#include "suavik.h"
#include "zav.h"
#include "hardware.h"
#include "../arena/kernel/h/session.h"
#include "../arena/h/vehicle.h"
#include "../graph/drawd3d.h"

#pragma INLINE_DEPTH 0

#define IsNAN(x)  (((x)*(x)==(x)) && (x)!=0)

#define VIEW_POINT CViewObject::m_viewPointDirMx
#define MemCopy(c,s,d) memcpy(d,s,c)
#define GET_BYTE        ((m_frameBuf += 1)[-1])
#define GET_WORD        (_TO(WORD*, (m_frameBuf += 2))[-1])
#define STEP(size)      m_frameBuf += (size)
#define GET(dest)       memcpy(dest, (m_frameBuf += sizeof(dest))-sizeof(dest), sizeof(dest))
#define GET_C(dest,cnt) memcpy(dest, &((m_frameBuf += cnt)[-cnt]), cnt)


static int paletteChange = 0;
static int bufferSize = 0;
static const int maxProb = 1000;
static SGRViewport *oldViewPort = NULL, *viewPort = NULL;

//---------------------------------------------------
void VRectangle(int x, int y, int len, int h, int c, bool trans, int opasity = 128, int transCoords = 1){
   if(transCoords){
      x -= _gr_nScreenWidth/2;
      y -= _gr_nScreenHeight/2;
   }
   _gr_polygon.dwFullType = trans ? GR_POLY_TRANSPARENT : GR_POLY_FLAT;
   _gr_polygon.dwAddType  = 0;
   _gr_polygon.nVertices  = 4;
   _gr_polygon.dwOpacity  = opasity;
   _gr_polygon.dwColor.color = c;
   _gr_polygon.nLights    = 0;
   _gr_vertices[0].any.x = x;
   _gr_vertices[0].any.y = y;
   _gr_vertices[0].any.iz = 65534;
   _gr_vertices[1].any.x = x;
   _gr_vertices[1].any.y = y+h;
   _gr_vertices[1].any.iz = 65534;
   _gr_vertices[2].any.x = x+len;
   _gr_vertices[2].any.y = y+h;
   _gr_vertices[2].any.iz = 65534;
   _gr_vertices[3].any.x = x+len;
   _gr_vertices[3].any.y = y;
   _gr_vertices[3].any.iz = 65534;

   GRDrawPolygonPCCW();
}
//---------------------------------------------------
static void ClearScreen(){
   VRectangle(0, 0, _gr_nScreenWidth, _gr_nScreenHeight, 0, FALSE);
}
//---------------------------------------------------
static void VPoly(const int *x, const int *y, int n, int c, bool trans, int opasity = 128){
   _gr_polygon.dwFullType = trans ? GR_POLY_TRANSPARENT : GR_POLY_FLAT;
   _gr_polygon.dwOpacity  = opasity;
   _gr_polygon.dwAddType  = 0;
   _gr_polygon.nVertices  = n;
   _gr_polygon.dwColor.color = c;

   for(int i = 0; i < n; ++i){
      _gr_vertices[i].any.x = x[i]-_gr_nScreenWidth/2;
      _gr_vertices[i].any.y = y[i]-_gr_nScreenHeight/2;
      _gr_vertices[i].any.iz = 65534;
   }
   GRDrawPolygonPCCW();
}
//---------------------------------------------------
static void VCircle(int x, int y, int r, int c, bool trans, int opasity = 128){
   int i, n;
   double angle;

   if(r <= 4)n = 8;
   if(r > 4 && r <= 8) n = 12;
   if(r > 8) n = 16;

   x -= _gr_nScreenWidth/2;
   y -= _gr_nScreenHeight/2;
   _gr_polygon.dwFullType = trans ? GR_POLY_TRANSPARENT : GR_POLY_FLAT;
   _gr_polygon.dwAddType  = 0;
   _gr_polygon.dwOpacity  = opasity;
   _gr_polygon.nVertices  = n;
   _gr_polygon.nLights    = 0;
   _gr_polygon.dwColor.color = c;
   for(i = 0; i < n; ++i){
      angle = 2*M_PI*(double)i/(double)n;
      _gr_vertices[i].any.x = x + r*sin(angle);
      _gr_vertices[i].any.y = y + r*cos(angle);
      _gr_vertices[i].any.iz = 65534;
   }
   GRDrawPolygonPCCW();
}

//===============================================================
void TFlicPlayer::addNotify(){
	KR_Event event;

	event.source	  = getObjectID();
	event.destination = g_hardware.getObjectID();
	event.label	  = CTRL_SUBSCRIBE;
	event.timeStamp	  = Session::m_moment;
	if (event.destination.isNUL())	return;
	event.data.open(EDO_WRITE)
				.putObjectID(getObjectID())
	            .putInt(EXCLUSIVE)
			  .close();
	if (context) context->sendEventNow(event);
}
//-------------------------------------------------------------
void TFlicPlayer::removeNotify(){
	KR_Event event;

        event.timeStamp   = Session::m_moment;
	event.source	  = getObjectID();
	event.destination = g_hardware.getObjectID();
        event.label	  = CTRL_UNSUBSCRIBE;
	if (event.destination.isNUL())	return;
	event.data.open(EDO_WRITE)
				.putObjectID(getObjectID())
			  .close();
	if (context) context->sendEventNow(event);	
}
//---------------------------------------------------
int TFlicPlayer::receiveEvent(KR_Event &event){
	int code;
	int ctrlEvent;
	double timeStamp, down;
	int repeat;

	switch( event.label ){
		case KR_WAKE_UP:             break;
		case CTRL_MOUSE_MOVE_MSG:
        case CTRL_CHAR:
		case CTRL_JOYSTICK_MOVE_MSG: break;
		case CTRL_BUTTONS_MSG:
			timeStamp = event.timeStamp;
            event.data.open(EDO_READ)
						.getInt(ctrlEvent)			
                        .getDouble(down)
                        .getInt(code)
                        .getInt(repeat)
					  .close();
			if(code > 0 && down != 0) m_key = code;
			break;
		default: return(0);
    }
    return(1);
}
//-------------------------------------------------------
void TFlicPlayer::SetPal64(const byte *pal){
   ASSERT(pal != NULL);

   #ifdef __DOS__
   for(int i = 0; i < 256; ++i){
      OutPort(0x3C8, i);
      OutPort(0x3C9, *pal); ++pal;
      OutPort(0x3C9, *pal); ++pal;
      OutPort(0x3C9, *pal); ++pal;
   }
   #endif // __DOS__
   GRSetPalette(pal);
   g_GameConsole.RecreateFonts();
   image.SetPalette((unsigned char*)pal);
}
//-------------------------------------------------------
void TFlicPlayer::Load_FLI_COLOR(byte *pal64){
   int  i,j;
   int  scip = 0,scip3,count3;
   int  count;
   int  alignPal = 0;

   for(i = GET_WORD; i > 0 ; --i){
      scip += GET_BYTE;
      count = GET_BYTE;
      if(count == 0) count = 256;

      scip3  = scip+scip+scip;
      count3 = count+count+count;
      GET_C(&(pal64[scip3]), count3);
      for(j = scip3;j < scip3 + count3;j++)
         pal64[j] = (byte)(pal64[j]<<2);

      scip     += count;
      alignPal += count;
   }
   if( (alignPal&1) != 0 ) STEP(1);
   SetPal64(pal64);
   paletteChange = 1;
}
//-------------------------------------------------------
void TFlicPlayer::Load_FLI_COLOR256(byte *pal64){
   int  i;//, j;
   int  scip = 0,count,scip3,count3;
   //int  ccnt;
   int  alignPal = 0;

   for(i = GET_WORD; i > 0; --i){
      scip += GET_BYTE;
      count = GET_BYTE;
      if(count == 0) count = 256;

      scip3  = scip+scip+scip;
      count3 = count+count+count;

      GET_C(&(pal64[scip3]), count3);

      //ccnt  = scip3+count3;
      //for(j = scip3; j < ccnt; ++j) pal64[j] >>= 2;

      scip     += count;
      alignPal += count;
   }
   if(alignPal & 1) STEP(1);
   SetPal64(pal64);
   paletteChange = 1;
}
//-------------------------------------------------------
byte *TFlicPlayer::VHLine(byte color, int cnt, byte *vptr) const{
   for(; cnt > 0; --cnt, ++vptr) *vptr = color;
   return(vptr);
}
//-------------------------------------------------------
byte *TFlicPlayer::VHLineW(word color, int cnt, byte *vptr) const{
   for(; cnt > 0; --cnt, vptr += 2) *_TO(word*, vptr) = color;
   return vptr;
}
//-------------------------------------------------------
void TFlicPlayer::Load_FLI_LC(int scrX, int scrY){
   int i,j,alignScr;
   int changePix;
   int packets;//,changeLines;
   int scipLines;
   byte color;
   byte *vptr;

   //
   // Читаем число пропускаемых линий
   //
   scipLines = GET_WORD;

   //
   // Число изменяемых линий
   //
   alignScr = 0;
   scrY    += scipLines;
   for(i = GET_WORD; i > 0; --i){
      vptr = m_scrBuf + scrY*m_flicW+scrX;
      ++scrY;
      //
      //  Читаем число пакетов
      //
      packets = GET_BYTE;
      alignScr++;

      for(j = 0; j < packets; ++j){
         vptr     += _DWORD(GET_BYTE);
         changePix = GET_BYTE;

         if(changePix == 0) continue;

         if((changePix & 0x80) == 0){
            MemCopy(changePix, m_frameBuf, vptr);
            vptr       += changePix;
            m_frameBuf += changePix;
            alignScr   += changePix;
         }
         else{
            int num;

            color = GET_BYTE;
            num   = _CHAR2DWORD(-_CHAR(changePix));
            memset(vptr, color, num);
            vptr  += num;
            ++alignScr;
         }
      }
   }
   if(alignScr & 1) STEP(1);
}
//-------------------------------------------------------
void TFlicPlayer::Load_FLI_WORD_LC(int scrX, int scrY){
   int   j;
   int   changePix;
   int   packets;
   int   changeLines;
   int   scipLines;
   word  color;
   byte *vptr;

   //
   // Число изменяемых линий
   //
   changeLines = GET_WORD;
   for(; changeLines > 0; --changeLines){
      //
      // Читаем число пропускаемых линий
      //
      scipLines = GET_WORD;

   RestartContent:

      switch(_WORD(scipLines) >> 14){
         case 3:scrY     += _INT(-_SHORT(scipLines));
                scipLines = GET_WORD;
                goto RestartContent;
         case 0:packets = scipLines;break;
         case 2:
         case 1: return;
      }
      vptr = m_scrBuf + scrY*m_flicW+scrX;
      ++scrY;
      //
      // разбор пакета
      //
      for(j = 0; j < packets; ++j){
         //
         // пропускаем неизмененные точки
         //
         vptr     += _DWORD(GET_BYTE);
         changePix = GET_BYTE;

         if((changePix & 0x80) == 0){
            changePix <<= 1;
            //GET_C(vptr,changePix);
            MemCopy(changePix, m_frameBuf, vptr);
            m_frameBuf += changePix;
            vptr       += changePix;
         }
         else{
            color     = GET_WORD;
            changePix = (changePix^255)+1;
            vptr      = VHLineW(color, changePix, vptr);
         }
      }
   }
}
//-------------------------------------------------------
void TFlicPlayer::Load_FLI_BRUN(int scrX, int scrY, int lines, int colom){
   byte b_color,b_code;
   int packetCnt;
   int x,cnt,scip;
   byte *lptr,*bptr;

   scip = 0;
   lptr = m_scrBuf + scrY*m_flicW+scrX;

   for(; lines > 0; --lines, lptr += m_flicW){
      x         = 0;
      bptr      = lptr;
      packetCnt = GET_BYTE;
      scip     += 1;
      //
      // Распаковываем линию
      //
      for(; packetCnt > 0; --packetCnt){
         b_code = GET_BYTE;
         if(b_code == 0 ) break;
         if((b_code&0x80) == 0){
            b_color = GET_BYTE;
            bptr    = VHLine(b_color, b_code, bptr);
            x      += b_code;
         }
         else{
            cnt = _CHAR(-b_code);
            GET_C(bptr, cnt);
            scip += cnt+1;
            bptr += cnt;
            x    += cnt;
         }
         if(x >= colom)break;
      }
   }
   if((scip & 1) != 0) STEP(1);
}
//-------------------------------------------------------
void TFlicPlayer::Load_FLI_COPY(int scrX, int scrY, int lines, int colom){
   int i;
   byte *lptr;

   lptr = m_scrBuf + scrY*m_flicW+scrX;
   for(i = lines; i > 0; --i, lptr += m_flicW) GET_C(lptr,colom);
}
//-------------------------------------------------------
void TFlicPlayer::Load_FLI_BLACK(int scrX, int scrY, int lines, int colom) const{
   int i;
   byte *vptr = m_scrBuf + scrY*m_flicW+scrX;

   colom >>= 1;
   for(i = 0; i < lines; ++i){
      VHLineW(0, colom, vptr);
      vptr += m_flicW;
   }
}
//-------------------------------------------------------
int TFlicPlayer::LoadChunk(int scrX, int scrY, byte *pal64, int lines, int colom){
   ChunkHeader &chk = *_TO(ChunkHeader*, m_frameBuf);
   STEP(sizeof(chk));

   switch(chk.Type){
      case FLI_COLOR256:Load_FLI_COLOR256(pal64);              break;
      case FLI_WORD_LC: Load_FLI_WORD_LC(scrX,scrY);           break;
      case FLI_COLOR:   Load_FLI_COLOR(pal64);                 break;
      case FLI_LC:      Load_FLI_LC(scrX,scrY);                break;
      case FLI_BLACK:   Load_FLI_BLACK(scrX,scrY,lines,colom); break;
      case FLI_BRUN:    Load_FLI_BRUN(scrX,scrY,lines,colom);  break;
      case FLI_COPY:    Load_FLI_COPY(scrX,scrY,lines,colom);  break;
      case FLI_PREVIEW: STEP(chk.Size-sizeof(chk));            break;
      default: return 0;
   }
   return 1;
}
//-------------------------------------------------------
int TFlicPlayer::LoadFrame(FILE *f, int scrX, int scrY, byte *pal64, int lines, int colom){
   int i;
   FrameHeader fh;

   fread(&fh, sizeof(fh), 1, f);
   RTCHECK(fh.Size-sizeof(fh) < bufferSize, "LoadFLC : Bffer too low");

   fread(m_frameBuf, fh.Size-sizeof(fh), 1, f);

   if(fh.Magic == 0x00A1 || fh.Magic == 0xF100) return 1;

   RTCHECK1(fh.Magic == 0xF1FA, "Error FLC Frame %x\n", _INT(fh.Magic));
   for(i = 0; i < fh.Chunks; ++i)
      if(!LoadChunk(scrX, scrY, pal64, lines, colom)) return 0;

   return 1;
}
//-------------------------------------------------------
int TFlicPlayer::Play(CConfigFile &cfg, const char *flicSectName,
                      byte *scrBuf, int scrX, int scrY, byte *pal64){
   int i;
   double time;
   double delayC;
   int key = 0;
   char fBuf[16*1024];
   char fName[200];
   char text[500];
   double textDelay;
   const char *str;
   FLC_Header flch;
   byte *frame;
   FILE *fh;
   int esc = g_hardware.SearchCode("Esc"), space = g_hardware.SearchCode("Space");

   ASSERT(fName  != NULL);
   ASSERT(scrBuf != NULL);

   m_isPlay = TRUE;
   m_key    = 0;

   str = cfg(flicSectName, "Name ");
   RTCHECK1(str != NULL, "No flic name in section '%s'.", flicSectName);
   sprintf(fName, "%s%s", g_HDDir, str);
   str = cfg(flicSectName, "Text ");
   RTCHECK1(str != NULL, "No text in flic section '%s'.", flicSectName);
   strcpy(text, str);
   str = cfg(flicSectName, "Delay");
   RTCHECK1(str != NULL, "No delay in flic section '%s'.", flicSectName);
   textDelay = cfg.GetDouble(flicSectName, "Delay");
   RTCHECK1(textDelay > 0 && textDelay < MAX_TIME_INTERVAL,
            "Invalid text delay in flic section %s.", flicSectName);

   fh  = fopen(fName, "rb");
   RTCHECK1(fh != NULL, "Can't open flic file '%s'.", fName);
   setvbuf(fh, fBuf, _IOFBF, 16*1024);

   fread(&flch, sizeof(flch), 1, fh);

   frame    = new byte[bufferSize = flch.Width*flch.Height*2];
   m_flicW  = flch.Width;
   m_scrBuf = scrBuf;

   RTCHECK(flch.Magic == 0xAF12, "Error FLC file : Magic != 0xAF12.");
   RTCHECK(flch.Zero  == 0,      "Error FLC file : Zero  != 0.");
   RTCHECK(flch.Bits  == 8,      "Error FLC file : need 8 bits.");

   image.Delete();
   image.SetWidthHeight(flch.Width,flch.Height);

   GRClearScreen(TRUE,GRFillColor(0,0,0));

   g_GameConsole.PrintUrgent(text, textDelay);
   g_hardware.GetExclusiveMode(getObjectID());

   delayC = 1./ 15.;//flch.Speed;  // fixme
   for(i = 0; i < flch.CadrCnt; ++i){

	  g_hardware.MessageLoop();
      SUA_ProcessEvents();
      if(m_key == esc || m_key == space) break;

      time       = Session::m_realTimer->GetTime();
      m_frameBuf = frame;
      RTCHECK2(LoadFrame(fh, scrX, scrY, pal64, flch.Height, flch.Width),
               "Frame %i from %i.", i, _INT(flch.CadrCnt));

      #ifdef __DOS__    // Wait for retrace
      while((inp(0x3DA) & 8) != 0);
      while((inp(0x3DA) & 8) == 0);
      #endif // __DOS__

      if(paletteChange){
         GRClearScreen(TRUE,GRFillColor(0,0,0));
         GRDumpScreen();
         SetPal64(pal64);
         paletteChange = 0;
      }
      OutScreen(scrBuf, flch.Width ,flch.Height);
      g_GameConsole.Draw();

      while(Session::m_realTimer->GetTime() - time < delayC);
   }
   g_hardware.ReleaseExclusiveMode();
   GRStartScene();

   fclose(fh);
   delete frame;
   m_isPlay = FALSE;

   return(key);
}
//-------------------------------------------------------
void TFlicPlayer::OutScreen(const byte *scrBuf, int flicLen, int flicH) {
   image.LoadPalImage((unsigned char*)scrBuf, 0, 0, 0, 0, flicLen - 1, flicH - 1, flicLen);
   if (_gr_nScreenWidth >= 640 && _gr_nScreenHeight >= 480)
      image.DrawX2(_gr_nScreenWidth/2  - flicLen, _gr_nScreenHeight/2 - flicH);
   else
      image.Draw((_gr_nScreenWidth  - flicLen)/2, (_gr_nScreenHeight - flicH)/2);
}
//---------------------------------------------------
void TFlicPlayer::ChangeResEvent(){
   ClearScreen();
}


//---------------------------------------------------
// Set default values.
CBriefing::CBriefing(int flicBufSize) : KR_Object(){
   m_isRecording = FALSE;
   m_isPlaying   = FALSE;
   m_flicBuffer  = new byte[flicBufSize];
   m_flicPlayer  = new TFlicPlayer;
   m_camX        = new CChannelMap(BRF_MAX_POINTS_NUM);
   m_camY        = new CChannelMap(BRF_MAX_POINTS_NUM);
   m_camZ        = new CChannelMap(BRF_MAX_POINTS_NUM);
   m_yaw         = new CChannelMap(BRF_MAX_POINTS_NUM);
   m_pitch       = new CChannelMap(BRF_MAX_POINTS_NUM);
   m_roll        = new CChannelMap(BRF_MAX_POINTS_NUM);
   m_brfTotalTime = 0;
}
//---------------------------------------------------
CBriefing::~CBriefing(){
   delete [] m_flicBuffer;
   delete m_flicPlayer;
   delete m_camX;
   delete m_camY;
   delete m_camZ;
   delete m_yaw;
   delete m_pitch;
   delete m_roll;
}
//-------------------------------------------------------------
void CBriefing::addNotify(){
	KR_Event event;

	event.source	  = getObjectID();
	event.destination = g_hardware.getObjectID();
	event.label       = CTRL_SUBSCRIBE;
	event.timeStamp	  = Session::m_moment;
	event.data.open(EDO_WRITE)
				.putObjectID(getObjectID())
	            .putInt(EXCLUSIVE)
			  .close();
	getContext()->addEvent(event);

    getContext()->addObject("FlicPlayer",m_flicPlayer);
}
//-------------------------------------------------------------
void CBriefing::removeNotify(){
	KR_Event event;

    event.timeStamp   = Session::m_moment;
	event.source	  = getObjectID();
	event.destination = g_hardware.getObjectID();

	if (event.destination.isNUL())return;

	event.label = CTRL_UNSUBSCRIBE;
	event.data.open(EDO_WRITE)
				.putObjectID(getObjectID())
			  .close();
	if (context)context->sendEventNow(event);
}
//---------------------------------------------------
int CBriefing::receiveEvent(KR_Event &event){
	int code;
	int ctrlEvent;
	double timeStamp, down;
	int repeat;
	char buf[100];

	switch( event.label ){
		case KR_WAKE_UP:		break;
		case BRIEF_PLAY:
			event.data.open(EDO_READ)
						.getStr(buf, 100)
					  .close();
			PlayBriefing(buf);
			break;
		case BRIEF_ADD_CTRL_POINT:			break;
		case BRIEF_ADD_OBJECT_POS:			break;
		case BRIEF_WRITE_POSITIONS:			break;
		case BRIEF_WRITE_FLIGHT:			break;

		case CTRL_CHAR: break;
        case CTRL_MOUSE_MOVE_MSG:
		case CTRL_JOYSTICK_MOVE_MSG: break;
		case CTRL_BUTTONS_MSG:
			timeStamp = event.timeStamp;
            event.data.open(EDO_READ)
						.getInt(ctrlEvent)			
                        .getDouble(down)
                        .getInt(code)
                        .getInt(repeat)
					  .close();
			if(code == 0 || down == 0) break;
			m_key = code;
			switch(ctrlEvent){
				case BRF_BEGIN_RECORD:{
					int n;

					event.data.open(EDO_READ)
								.getInt(n)
							  .close();
					BeginRecord(n);
					break;
				}
				case BRF_END_RECORD:		EndRecord();        break;
				case BRF_ADD_CTRL_POINT:
					if(!m_isRecording)BeginRecord(500);
					AddCtrlPoint(Session::m_realTimer->GetTime() - m_recordPrevTime);
					m_recordPrevTime = Session::m_realTimer->GetTime();
					break;
				case BRF_ADD_POS:
					break;
				case BRF_SAVE_FLIGHT:
					/*g_GameConsole.SetPrompt("FLIGHT FNAME-- ");
					g_GameConsole.SetDefaultStr("flight00.txt");

					event.source	  = getObjectID();
					event.destination = g_GameConsole.getObjectID();
					event.label		  = CON_ACTIVATE;
					//event.timeStamp	  = 0;
					event.data.open(EDO_WRITE)
								.putInt(BRIEF_WRITE_FLIGHT)
							  .close();
					getContext()->sendEventNow(event);*/
					WriteFlight("BRIEFING.TXT"); // fixme
					EndRecord();
					break;
				case BRF_SAVE_POS:
					break;
				default: break;
			}
			break;
		default: return(0);
    }
    return(1);
}
//---------------------------------------------------
void CBriefing::ChangeResEvent(){
   SRect2 clipRect;

   clipRect.top    = 0;
   clipRect.bottom = _gr_nScreenHeight;
   clipRect.left   = 0;
   clipRect.right  = _gr_nScreenWidth;

   m_scrW        = _gr_nScreenWidth;
   m_scrH        = _gr_nScreenHeight;
   paletteChange = 1;  // for  flic player

   if(m_isPlaying){
      if(m_flicPlayer->IsPlay()) m_flicPlayer->ChangeResEvent();
      else TranslatePalette();
   }
   if(viewPort != NULL) GRReleaseViewport(viewPort);
   viewPort    = GRCreateViewport(_gr_nScreenWidth/2, _gr_nScreenHeight/2, clipRect);
}
//---------------------------------------------------
// Plays all briefing using description in fName.
void CBriefing::PlayBriefing(const char *fName){
   CConfigFile cfg(fName);
   int i, j, repeatNum, r;
   int actionsNum;
   char action[100], name[255];
   char actName[100];
   char sndSectName[100], soundName[255];
   char fltType[100];
   const char *str;
   double sDelay;
   int sCycle;
   int pointsNum;
   int key;

   ASSERT(fName != NULL);
   m_effClass = FLT_NONE;

   for(i = 0; i < 256; ++i){
      m_sysPal[i*3  ] = _gr_logPal.palPalEntry[i].peRed;
      m_sysPal[i*3+1] = _gr_logPal.palPalEntry[i].peGreen;
      m_sysPal[i*3+2] = _gr_logPal.palPalEntry[i].peBlue;
   }
   actionsNum = cfg.GetInt("Root", "ActionsNum", 0);
   repeatNum  = cfg.GetInt("Root", "RepeatsNum", 1);
   RTCHECK(actionsNum > 0, "BRIEFING : Invalid actions number.");
   RTCHECK(repeatNum > 0 && repeatNum < 10000, "BRIEFING : Invalid repeats number.");

   for(r = 0;r < repeatNum; ++r)
   for(i = 1; i <= actionsNum; ++i){
      sprintf(actName, "Action%i", i);
      sprintf(sndSectName, "Sound%i", i);
      str = cfg("Root", actName);

      RTCHECK(*str && *str != '\n', "No ACTION name.");
      for(j = 0; *str && *str != ',' && *str != '\n'; ++j) action[j] = *str++;
      action[j] = 0; str++;
      RTCHECK(*str && *str != '\n', "No NAME in action.");
      for(j = 0; *str && *str != ',' && *str != '\n'; ++j) name[j] = *str++;
      name[j] = 0; str++;
      if(!strcmp(action, "PlayFlight")){
         RTCHECK1(*str && *str != '\n', "No Flight TYPE in action %i.", i);
         for(j = 0; *str && *str != ',' && *str != '\n'; ++j) fltType[j] = *str++;
         fltType[j] = 0; str++;
         RTCHECK1(*str && *str != '\n', "No points num in action %i.", i);
         sscanf(str, "%i", &pointsNum);
      }
      m_isPlaying = TRUE;
      m_key = 0;
      str = cfg("Root", sndSectName);
      if(str == NULL){
          soundName[0] = 0;
          sDelay = 0;
          sCycle = 0;
      }
      else{
          for(j = 0; *str && *str != ',' && *str != '\n'; ++j) soundName[j] = *str++;
          soundName[j] = 0; str++;
          if(str && *str) sscanf(str, "%i,%f", &sCycle, &sDelay);
          if(sDelay < 0 || sDelay > 1000 || IsNAN(sDelay) ) 
		sDelay = 0;
          if(sCycle < 0 || sCycle > 1) 
		sCycle = 0;
      }
      g_vehicle->setBriefingSound(soundName, sCycle, Session::m_moment+sDelay);

      GRSetPalette(m_sysPal);
      if     (!strcmp(action, "PlayFlight"))
         key = PlayFlight(cfg, name, fltType, pointsNum);
      else if(!strcmp(action, "PlayFlic"))
         key = PlayFlic  (cfg, name);
      else RTCHECK2(0, "Invalid action %i name is '%s'.", i, action);
      g_GameConsole.DeleteUrgentMessge();

      m_isPlaying = FALSE;

      if(key == g_hardware.SearchCode("Esc")) break;
   }
   g_hardware.ReleaseExclusiveMode();
   GRClearScreen(TRUE,GRFillColor(0,0,0));
   GRDumpScreen();
   GRSetPalette(m_sysPal);
   g_GameConsole.RecreateFonts();
}
//---------------------------------------------------
// Enable recording flight process.
void CBriefing::BeginRecord(int maxPointsNum){
   ASSERT(maxPointsNum > 0 && maxPointsNum < BRF_MAX_POINTS_NUM);

   if(m_isRecording) WRITELOG("Rewrite flight and objects.\n");
   m_isRecording  = TRUE;
   m_maxPointsNum = maxPointsNum;
   m_pointsNum    = 0;
   m_objectsNum   = 0;
   m_recordPrevTime = Session::m_realTimer->GetTime();
}
//---------------------------------------------------
// Stop recording process.
void CBriefing::EndRecord(){
   m_isRecording = FALSE;
   m_objectsNum  = 0;
   m_pointsNum   = 0;
}
//---------------------------------------------------
// Add current point as new control point.
void CBriefing::AddCtrlPoint(double prevFlyTime){
   CFMatrix3x4 viewInv;

   ASSERT(prevFlyTime >= 0 && prevFlyTime <= MAX_TIME_INTERVAL);
   ASSERT(m_pointsNum < m_maxPointsNum);

   RTCHECK(m_isRecording, "AddCtrlPoint - Not recording.");

   viewInv.LoadTransposed(VIEW_POINT);
   viewInv.RestoreEuler(CFMatrix3x4::AXIS_OY, m_point[m_pointsNum].yaw,
                        CFMatrix3x4::AXIS_OX, m_point[m_pointsNum].pitch,
                        CFMatrix3x4::AXIS_OZ, m_point[m_pointsNum].roll);
   m_point[m_pointsNum].pos = viewInv.Offset();

   if(m_pointsNum > 0) m_point[m_pointsNum-1].time = prevFlyTime;
   m_pointsNum++;
   echo("Ctrl point #%i added", m_pointsNum);
}
//---------------------------------------------------
void CBriefing::AddObjectPos(double, const char *str){
   CFMatrix3x4 viewInv;

   RTCHECK(m_isRecording, "AddObjectPos - Not recording.");

   viewInv.LoadTransposed(VIEW_POINT);
   viewInv.RestoreEuler(CFMatrix3x4::AXIS_OY, m_object[m_objectsNum].yaw,
                        CFMatrix3x4::AXIS_OX, m_object[m_objectsNum].pitch,
                        CFMatrix3x4::AXIS_OZ, m_object[m_objectsNum].roll);
   m_object[m_objectsNum].pos = viewInv.Offset();

   if(str != NULL)
      strcpy(m_object[m_objectsNum++].text, str);
}
//---------------------------------------------------
void CBriefing::WriteObjects(const char *fName) const{
   FILE *fh;
   int i;

   if(fName == NULL) return;

   fh = fopen(fName, "wt");
   RTCHECK1(fh != NULL, "Error opening file %s for write.", fName);

   fprintf(fh, "DEFAULT_NAME\n");
   fprintf(fh, "%i\n", m_objectsNum);
   for(i = 0; i < m_objectsNum; ++i)
      fprintf(fh, "%-3i: [%.3f, %.3f, %.3f] [%.3f, %.3f, %.3f],%s\n",
                  i, m_object[i].pos.x, m_object[i].pos.y, m_object[i].pos.z,
                  m_object[i].yaw*180/M_PI, m_object[i].pitch*180/M_PI,
                  m_object[i].roll*180/M_PI, m_object[i].text);
   fclose(fh);
}
//---------------------------------------------------
// Wirte recorded flight to disk.
void CBriefing::WriteFlight(const char *fName) const{
   FILE *fh;
   int i;
   const char flightName[] = BRF_DEFAULT_FLIGHT_NAME;
   const char defBugs   [] = BRF_DEFAULT_BUG_CLASS;

   if(fName == NULL) return;

   fh = fopen(fName, "wt");
   RTCHECK1(fh != NULL, "Error opening file %s for write.", fName);

   fprintf(fh, "[Root]\n");
   fprintf(fh, "ActionsNum=1\n");
   fprintf(fh, "RepeatsNum=1\n");
   fprintf(fh, "Action1=PlayFlight,%s,%s,%i\n", flightName, defBugs, m_pointsNum);

   fprintf(fh, "\n");
   for(i = 0; i < m_pointsNum; ++i){
      fprintf(fh, "[%s.%i]\n", flightName, i);
      fprintf(fh, "Pos  =%.3f,%.3f, %.3f\n", m_point[i].pos.x,
                                             m_point[i].pos.y,
                                             m_point[i].pos.z);
      fprintf(fh, "Angle=%.3f,%.3f, %.3f\n", m_point[i].yaw*180/M_PI,
                                             m_point[i].pitch*180/M_PI,
                                             m_point[i].roll*180/M_PI);
      fprintf(fh, "Time =%.3f\n", m_point[i].time);
      fprintf(fh, "Delay=%.3f\n", 0.);
      fprintf(fh, "Text =\n\n");
   }
   fclose(fh);

   echo("Flight writen %i points", m_pointsNum);
}
//---------------------------------------------------
// Plays flic from file named fName.
int CBriefing::PlayFlic(CConfigFile &cfg, const char *flicSectName){
   ASSERT(flicSectName != NULL);
   ASSERT(!m_isRecording);

   return(m_flicPlayer->Play(cfg, flicSectName, m_flicBuffer, 0, 0, m_flicPal));
}
//---------------------------------------------------
void CBriefing::TranslatePalette() const {
   int r, g, b, l, i;
   byte flightPal[256*3];
   const int maxColorVal = 255;

   for(i = 0; i < 256; ++i){
      r = (unsigned)(_gr_logPal.palPalEntry[i].peRed);
      g = (unsigned)(_gr_logPal.palPalEntry[i].peGreen);
      b = (unsigned)(_gr_logPal.palPalEntry[i].peBlue);

      l = r+g+b;

      switch( m_effClass ){
         case FLT_CINEMA:
            r = l/3;
            g = l/3;
            b = (int)(l*0.27);
            break;
         case FLT_TV:
            r = 20+(int)(l/2.7);
            g = 20+(int)(l/2.5);
            b = 20+l/2;
            break;
         case FLT_CRYSTAL:{
            double n=(l/(3*maxColorVal/2)) * (l/(3*maxColorVal/2));
            r = l/2.3;
            g = (maxColorVal-l/3)*0.25+n*5;
            b = l/7+n*3;
            break;
         }
         case FLT_GYPNOSE:
            break;
		 case FLT_NONE: return;
		 default: return;
      }
      if( r > maxColorVal ) r = maxColorVal;
      if( g > maxColorVal ) g = maxColorVal;
      if( b > maxColorVal ) b = maxColorVal;
      flightPal[i*3  ] = (byte)r;
      flightPal[i*3+1] = (byte)g;
      flightPal[i*3+2] = (byte)b;
   }
   GRSetPalette(flightPal);
   g_GameConsole.RecreateFonts();
}
//---------------------------------------------------
// Play fight from briefing cfg named flightName.
int CBriefing::PlayFlight(CConfigFile &cfg, const char *flightName,
                           const char *fltType, int pNum){
   const double splineFactor = 0;
   int i, len;
   char pointSect[100];
   int itemsParsed;
   const char *txt;
   double deltaTime, time;

   ASSERT(flightName != NULL);
   ASSERT(fltType    != NULL);
   ASSERT(!m_isRecording);

   if     (!strcmp(fltType, "CINEMA"))   m_effClass = FLT_CINEMA;
   else if(!strcmp(fltType, "TV"))       m_effClass = FLT_TV;
   else if(!strcmp(fltType, "CRYSTAL"))  m_effClass = FLT_CRYSTAL;
   else if(!strcmp(fltType, "GYPNOSE"))  m_effClass = FLT_GYPNOSE;
   else if(!strcmp(fltType, "NONE"))     m_effClass = FLT_NONE;
   else                                  m_effClass = FLT_NODEFINED;
   RTCHECK(m_effClass != FLT_NODEFINED, "Invalid effects class.");

   oldViewPort = GRGetViewport();
   GRSetViewport(viewPort);
   TranslatePalette();
   g_hardware.GetExclusiveMode(getObjectID());

   m_camX ->CreateOpen(pNum, -1, splineFactor);
   m_camY ->CreateOpen(pNum, -1, splineFactor);
   m_camZ ->CreateOpen(pNum, -1, splineFactor);
   m_yaw  ->CreateOpen(pNum, -1, splineFactor);
   m_pitch->CreateOpen(pNum, -1, splineFactor);
   m_roll ->CreateOpen(pNum, -1, splineFactor);

   m_brfTotalTime = 0;
   for(i = 0; i < pNum; ++i){            // Info fteching from file
      sprintf(pointSect, "%s.%i", flightName, i);
      itemsParsed = cfg(pointSect, "Pos  ", "%lg,%lg,%lg", &m_point[i].pos.x,
                                                           &m_point[i].pos.y,
                                                           &m_point[i].pos.z);
      RTCHECK3(itemsParsed == 3, "Invalid number of coordinates"
               "(point#%i, pNum == %i, fltType =='%s').", i, pNum, fltType);
      itemsParsed = cfg(pointSect, "Angle", "%lg,%lg,%lg", &m_point[i].yaw,
                                                           &m_point[i].pitch,
                                                           &m_point[i].roll);
      RTCHECK3(itemsParsed == 3, "Invalid number of angles."
               "(point#%i, pNum == %i, fltType =='%s').", i, pNum, fltType);
      if(i > 0){
          while(m_point[i].yaw-m_point[i-1].yaw >  180) m_point[i].yaw -= 360;
          while(m_point[i].yaw-m_point[i-1].yaw <- 180) m_point[i].yaw += 360;
          while(m_point[i].pitch-m_point[i-1].pitch >  180) m_point[i].pitch -= 360;
          while(m_point[i].pitch-m_point[i-1].pitch <- 180) m_point[i].pitch += 360;
          while(m_point[i].roll-m_point[i-1].roll >  180) m_point[i].roll -= 360;
          while(m_point[i].roll-m_point[i-1].roll <- 180) m_point[i].roll += 360;
      }
      m_point[i].time = cfg.GetDouble(pointSect, "Time ", -1);
      RTCHECK3(m_point[i].time != -1, "Error in time field"
               "(point#%i, pNum == %i, fltType =='%s').", i, pNum, fltType);

      m_point[i].delay = cfg.GetDouble(pointSect, "Delay", -1);
      RTCHECK3(m_point[i].delay != -1, "Error in delay field"
               "(point#%i, pNum == %i, fltType =='%s').", i, pNum, fltType);

      m_brfTotalTime += m_point[i].time+m_point[i].delay;

      txt = cfg(pointSect, "Text ");
      m_point[i].text[0] = 0;
      if(txt != NULL && strlen(txt) > 0){
         len = strlen(txt);
         RTCHECK3(len < BRF_MAX_STR_LEN, "Str 2 long"
                  "(point#%i, pNum == %i, fltType =='%s').", i, pNum, fltType);
         strcpy(m_point[i].text, txt);
      }
      else len = 0;
      m_point[i].textDelay = cfg.GetDouble(pointSect, "TextDelay", -1);
      if(m_point[i].textDelay == -1) m_point[i].textDelay = 3 + 0.1*(double)len;
   }
   for(i = 0, time = 0; i < pNum; ++i){      // Splines adjusting
      m_camX ->Node(i).f = m_point[i].pos.x;
      m_camY ->Node(i).f = m_point[i].pos.y;
      m_camZ ->Node(i).f = m_point[i].pos.z;
      m_yaw  ->Node(i).f = m_point[i].yaw * M_PI/180;
      m_pitch->Node(i).f = m_point[i].pitch * M_PI/180;
      m_roll ->Node(i).f = m_point[i].roll * M_PI/180;
      deltaTime          = m_point[i].time;
      if(m_point[i].delay == 0){
         m_camX ->Node(i).fSpline = 1;
         m_camY ->Node(i).fSpline = 1;
         m_camZ ->Node(i).fSpline = 1;
         m_yaw  ->Node(i).fSpline = 1;
         m_pitch->Node(i).fSpline = 1;
         m_roll ->Node(i).fSpline = 1;
      }
      m_camX ->Node(i).t = time;
      m_camY ->Node(i).t = time;
      m_camZ ->Node(i).t = time;
      m_yaw  ->Node(i).t = time;
      m_pitch->Node(i).t = time;
      m_roll ->Node(i).t = time;
      time += deltaTime;
   }
   m_key = PerformFly(pNum);

   GRSetViewport(oldViewPort);
   // restore pal & viewport
   return(m_key);
}
//extern void  RenderFrame(TCSFMatrix3x4 &dir);
//---------------------------------------------------
// Flyes along to given trace (determined by cam(X, Y, Z), yaw, pitch, roll).
int CBriefing::PerformFly(int pNum){
   double frBegRendTime;
   double frEndRendTime;
   double frRendTime;
   double nextPointTime;
   double delayTime;
   int i;
   CFVector3 viewPoint;
   CFMatrix3x4 dir;
   bool isDelayed;
   enum { FLY, STOP } state;
   int bkColor = GRTransparentColor(0, 0, 0);
   int esc   = g_hardware.SearchCode("Esc"),
	   space = g_hardware.SearchCode("Space");

   m_time = 0;
   for(i = 0; i < pNum; ++i){
       if(m_point[i].text != NULL && strlen(m_point[i].text) > 0)
            g_GameConsole.PrintUrgent(m_point[i].text, m_point[i].textDelay, GameConsole::TOP, bkColor);

      if(i == pNum-1) break;   // last point
      if(m_key == esc || m_key == space) break;

      isDelayed     = m_point[i].delay != 0;
      frRendTime    = 0;
      nextPointTime = m_camX->CNode(i+1).t; // for breaking loop
      state         = isDelayed ? STOP : FLY;
      delayTime     = m_point[i].delay;
      frBegRendTime = Session::m_realTimer->GetTime();
      if(isDelayed) m_time = m_camX->CNode(i).t;   // syncronization

      while(nextPointTime - m_time > 0){
         if(state == FLY) m_time += frRendTime;
         if(state == STOP && (delayTime -= frRendTime) < 0 ) state = FLY;
         if(m_time >= m_camX->CNode(i+1).t) break;

		 dir.LoadIdentity();
         viewPoint = CFVector3(m_camX->Phase(m_time),
                               m_camY->Phase(m_time),
                               m_camZ->Phase(m_time));
         dir.EulerL(CFMatrix3x4::AXIS_OZ, -m_roll ->Phase(m_time),
                    CFMatrix3x4::AXIS_OX, -m_pitch->Phase(m_time),
                    CFMatrix3x4::AXIS_OY, -m_yaw  ->Phase(m_time));
         dir.TranslateR(-viewPoint);

         if(g_vehicle != NULL ) g_vehicle->setPosition(viewPoint);

	     CViewDynamicList list;
	     //ZAV_BeginRenderFrame(list);
	     SUA_BeginRender(ZAV_Scene(),list);
	     ZAV_RenderFrame(&dir,list);
         if(GRIsHardware()) D3D_DrawZList();
	     Effects(frRendTime);
	     ZAV_PrintFrameInfo();
 	     SUA_EndRender(ZAV_Scene());
	     ZAV_EndRenderFrame();

	     g_hardware.MessageLoop();
         SUA_ProcessEvents();
         g_GameConsole.Draw();

		 if(m_key == esc || m_key == space) break;

         frEndRendTime = Session::m_realTimer->GetTime();
         frRendTime    = frEndRendTime-frBegRendTime;
         frBegRendTime = frEndRendTime;
         //echo("Remain time %f of %f", GetRemainTime(), GetTotalBrfTime());
      }
   }
   return((m_key == esc || m_key == space) ? m_key : 0);  // no keys pressed
}
//---------------------------------------------------
void CBriefing::CinemaBugs(double) const {
   const int maxBugsLines = 80;
   const int maxAddLines  = 4;
   const int maxCircles   = 4;
   const int maxBugSecLen = m_scrW/60;
   const int maxBugsNum   = 20;
   int x, y, circlesNum, r, maxCircleR;
   int bugsNum;
   int nodeX[4], nodeY[4];
   int lX[4], lY[4];
   int j;
   static int lineX[maxBugsLines], use[maxBugsLines];
   static int lineC[maxBugsLines];
   static int linesNum = 0;
   int i;
   int addLines;
   int removeProb;
   static int ncBlack = GRCreateColor(0, 0, 0),
			  ncWhite = GRCreateColor(255, 255, 255),
			  tcFlat  = GRTransparentColor(150, 128, 40);

   int isHrd = GRIsHardware();
   if(isHrd) VRectangle(0, 0 , m_scrW, m_scrH, tcFlat, TRUE, 64);

   if(linesNum == 0) for(i = 0; i < maxBugsLines; ++i) use[i] = 0;

   for(i = 0; i < maxBugsLines; ++i){   // remove lines
      if(!use[i]) continue;
      removeProb = maxProb/5;
      if(removeProb >= Random(maxProb)) use[i] = 0;
   }
   addLines = Random(maxAddLines);     // Add new lines
   addLines = Min(addLines, maxBugsLines-linesNum);
   for(i = 0; i < addLines; ++i){
      for(int j = 0; use[j]; ++j);     // search first empty slot
      lineX[j] = Random(m_scrW);
      lineC[i] = ncBlack;
      use  [j] = 1;
   }
   for(i = 0; i < maxBugsLines; ++i)
      if(use[i]) VRectangle(lineX[i], 0, isHrd ? 1 : 0, m_scrH-1, lineC[i], FALSE);

   bugsNum = 1+Random(maxBugsNum);
   for(i = 0; i < bugsNum; ++i){
      nodeX[0] = Random(m_scrW);
      nodeY[0] = Random(m_scrH);
      for(j = 1; j < 4; ++j){
         nodeX[j] = nodeX[j-1] + 1+Random(2*maxBugSecLen)-maxBugSecLen;
         nodeY[j] = nodeY[j-1] + 1+Random(2*maxBugSecLen)-maxBugSecLen;
      }
      for(j = 0; j < 3; ++j){
         lX[0] = nodeX[j] < nodeX[j+1] ? nodeX[j] : nodeX[j+1];
         lY[0] = nodeX[j] < nodeX[j+1] ? nodeY[j] : nodeY[j+1];
         lX[1] = nodeX[j] > nodeX[j+1] ? nodeX[j] : nodeX[j+1];
         lY[1] = nodeX[j] > nodeX[j+1] ? nodeY[j] : nodeY[j+1];
         lX[2] = lX[1];
         lY[2] = lY[1]-1;
         lX[3] = lX[0];
         lY[3] = lY[0]-1;
         VPoly(lX, lY, 4, ncBlack, FALSE);
      }
   }
   maxCircleR = m_scrW/90;
   circlesNum = Random(maxCircles);
   for(i = 0; i < circlesNum; ++i){
      r = Random(maxCircleR);
      x = Random(m_scrW);
      y = Random(m_scrH);
      VCircle(x, y, r, Random(9) == 1 ? ncWhite : ncBlack, FALSE);
   }
}
//---------------------------------------------------
void CBriefing::TVBugs(double frRendTime) const {
   const    int itlFact      = 3;  // interliveFactor
   const double bandBugDelay = 0.2;
   const double bandBugTime  = 1;
   static double time;
   static enum {BAND_ON, BAND_OFF} state;
   int y;
   int bandY, bandH = m_scrH/7;
   static int tc = GRTransparentColor(0, 0, 0),
	          nc = GRCreateColor(0, 0, 0),
		  tcFlat  = GRTransparentColor(128, 128, 200);

   int isHrd = GRIsHardware();
   if(isHrd) VRectangle(0, 0 , m_scrW, m_scrH, tcFlat, TRUE, 64);

   time += frRendTime;
   if(state == BAND_ON && time > bandBugTime){
      state = BAND_OFF; time  = 0;
   }
   if(state == BAND_OFF && time > bandBugDelay){
      state = BAND_ON;  time  = 0;
   }
   if(state == BAND_ON){
      bandY = m_scrH-(bandBugTime-time)*(m_scrH+bandH)/bandBugTime;
      VRectangle(0, bandY, m_scrW, bandH, tc, TRUE);
   }
   for(y = 0; y < m_scrH; y += itlFact)  // interlive
      VRectangle(0, y, m_scrW, isHrd ? 1 : 0 , nc, FALSE);
}
//---------------------------------------------------
void CBriefing::CrystalBugs(double frRendTime) const {
   const int maxCircles = 60;
   const double circleExpTime = 2.0;
   static int x0[maxCircles], y0[maxCircles];
   static double time[maxCircles];
   static int cNum = 0;
   static int lastJ;
   static enum {CIRCLE_ON, CIRCLE_OFF} state[maxCircles];
   T2DAgregat black, white;
   T2DObject  sec(4);
   double a, a0, a1, aAdd;
   int i, j, n;
   int r1, r21, r22;
   int maxD;
   static int tcWhite = GRTransparentColor(150, 150, 150),
			  tcBlack = GRTransparentColor(50, 50, 50),
			  tcFlat  = GRTransparentColor(200, 100, 60);

   int isHrd = GRIsHardware();
   if(isHrd) VRectangle(0, 0 , m_scrW, m_scrH, tcFlat, TRUE);

   if(cNum == 0) for(j = 0; j < maxCircles; ++j) state[j] = CIRCLE_OFF;

   for(j = 0; j < maxCircles; ++j) {
      time[j] += frRendTime;
      if(state[j] == CIRCLE_ON && time[j] > circleExpTime){
         state[j] = CIRCLE_OFF; time[j]  = 0; cNum--;
      }
      if(state[j] == CIRCLE_ON){
         r1             = m_scrW - (circleExpTime-time[j])*m_scrW/circleExpTime+5;
         maxD           = 3+5*r1/m_scrW;
         n              = 20 + (r1/15)*2;
         a              = M_PI/3;
         aAdd           = -M_PI/4;
         black.m_objNum = n;
         white.m_objNum = n;

         for(i = 0; i < n; ++i){
            a0 = a*2*i/n-a;
            a1 = a*2*(i+1)/n-a;

            r21 = r1 + ( a0 <= 0 ? (a+a0)*maxD : (a-a0)*maxD);
            r22 = r1 + ( a1 <= 0 ? (a+a1)*maxD : (a-a1)*maxD);

            sec.m_srcP[0] = TPoint2D(r1 *cos(a0+aAdd), -r1 *sin(a0+aAdd));
            sec.m_srcP[1] = TPoint2D(r21*cos(a0+aAdd), -r21*sin(a0+aAdd));
            sec.m_srcP[2] = TPoint2D(r22*cos(a1+aAdd), -r22*sin(a1+aAdd));
            sec.m_srcP[3] = TPoint2D(r1 *cos(a1+aAdd), -r1 *sin(a1+aAdd));
            black.m_obj[i]   = sec;
            black.m_color[i] = tcBlack;

            sec.m_srcP[0] = TPoint2D(-sec.m_srcP[0].x, -sec.m_srcP[0].y);
            sec.m_srcP[1] = TPoint2D(-sec.m_srcP[1].x, -sec.m_srcP[1].y);
            sec.m_srcP[2] = TPoint2D(-sec.m_srcP[2].x, -sec.m_srcP[2].y);
            sec.m_srcP[3] = TPoint2D(-sec.m_srcP[3].x, -sec.m_srcP[3].y);
            white.m_obj[i]   = sec;
            white.m_color[i] = tcWhite;
         }
         black.RedrawAt(TPoint2D(x0[j], y0[j]), 0);
         white.RedrawAt(TPoint2D(x0[j], y0[j]), 0);

         r1 -= maxD*1.2;
         r1 = Max(5, r1);
         for(i = 0; i < n; ++i){
            a0 = a*2*i/n-a;
            a1 = a*2*(i+1)/n-a;

            r21 = r1 + ( a0 <= 0 ? (a+a0)*maxD : (a-a0)*maxD);
            r22 = r1 + ( a1 <= 0 ? (a+a1)*maxD : (a-a1)*maxD);

            sec.m_srcP[0] = TPoint2D(r1 *cos(a0+aAdd), -r1 *sin(a0+aAdd));
            sec.m_srcP[1] = TPoint2D(r21*cos(a0+aAdd), -r21*sin(a0+aAdd));
            sec.m_srcP[2] = TPoint2D(r22*cos(a1+aAdd), -r22*sin(a1+aAdd));
            sec.m_srcP[3] = TPoint2D(r1 *cos(a1+aAdd), -r1 *sin(a1+aAdd));
            white.m_obj[i]   = sec;
            white.m_color[i] = tcWhite;

            sec.m_srcP[0] = TPoint2D(-sec.m_srcP[0].x, -sec.m_srcP[0].y);
            sec.m_srcP[1] = TPoint2D(-sec.m_srcP[1].x, -sec.m_srcP[1].y);
            sec.m_srcP[2] = TPoint2D(-sec.m_srcP[2].x, -sec.m_srcP[2].y);
            sec.m_srcP[3] = TPoint2D(-sec.m_srcP[3].x, -sec.m_srcP[3].y);
            black.m_obj[i]   = sec;
            black.m_color[i] = tcBlack;
         }
         black.RedrawAt(TPoint2D(x0[j], y0[j]), 0);
         white.RedrawAt(TPoint2D(x0[j], y0[j]), 0);
      }
   }
   if(Random(maxProb) < maxProb/4 && cNum < maxCircles) {
      for(j = 0; state[j] == CIRCLE_ON; ++j);
      ASSERT(j < maxCircles);
      state[j] = CIRCLE_ON;
      time[j]  = 0;
      if(Random(maxProb) < maxProb*2/3){
         x0[j] = x0[lastJ];
         y0[j] = y0[lastJ];
      }
      else{
         x0[j] = m_scrW/10+Random(m_scrW*9/10);
         y0[j] = m_scrH/10+Random(m_scrH*9/10);
      }
      lastJ = j;
      cNum++;
   }
}
//---------------------------------------------------
void CBriefing::GypnoseBugs(double ) const{
}
//---------------------------------------------------
void CBriefing::Effects(double frRendTime) const {
   switch(m_effClass){
      case FLT_CINEMA:  CinemaBugs(frRendTime);  break;
      case FLT_TV:      TVBugs(frRendTime);      break;
      case FLT_CRYSTAL: CrystalBugs(frRendTime); break;
      case FLT_GYPNOSE: GypnoseBugs(frRendTime); break;
      case FLT_NONE:                             break;
      default: RTCHECK(0, "");
   }
}


//===================================================
void T2DObject::Move(TPoint2D to){
   for(int i = 0; i < m_pNum; ++i) Shift(m_p[i], to.x, to.y);
}
//-----------------------------------
void T2DObject::RedrawAt(TPoint2D at, int c){
   int i;
   int x[MAX_PNT], y[MAX_PNT];

   Sync();
   Move(at);
   for(i = 0; i < m_pNum; ++i){
      x[i] = m_p[i].x;
      y[i] = m_p[i].y;
   }
   VPoly(x, y, m_pNum, c, TRUE);
}
//-----------------------------------
void T2DObject::Sync(){
   for(int i = 0; i < m_pNum; ++i) m_p[i] = m_srcP[i];
}

//====================================
void T2DAgregat::RedrawAt(TPoint2D at, int angle){
   int i;
   TPoint2D tmp;

   ASSERT(m_objNum >= 0 && m_objNum < MAX_OBJECTS);
   ASSERT(angle == 0);  // Rotate() removed

   for(i = 0; i < m_objNum; ++i){
      m_obj[i].Move(m_center[i]);
      tmp.x = at.x+m_center[i].x;
      tmp.y = at.y+m_center[i].y;
      m_obj[i].RedrawAt(tmp, m_color[i]);
   }
}//---------------------------------------------------
// Compute total briefing time without playing it.
// Used by the game scripts (s_GetBriefingTime).
double CBriefing::GetBriefingTime(const char *fName){
   ASSERT(fName != NULL);
   FILE *f = fopen(fName, "r");
   if(f == NULL){
      WRITELOG("Briefing :: %s not found\n", fName);
      return 0.0;
   }
   fclose(f);

   // FIXME(nw.exe sync): the shipped binary accumulates the real
   // flight/flic durations here; the snapshot code only knows the
   // last computed total.
   return m_brfTotalTime;
}
