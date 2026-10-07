#ifndef __BRIEFING_H__
#define __BRIEFING_H__

#include "../arena/kernel/h/object.h"
#include "../arena/kernel/h/echo.h"
#include "../arena/kernel/h/context.h"
#include "console.h"
#include "../arena/message/briefmsg.h"
#include "graph.h"

#define BRF_MAX_POINTS_NUM 1000
#define MAX_TIME_INTERVAL  1000
#define BRF_MAX_STR_LEN    255
#define BRF_MAX_OBJ_NUM    200
#define BRF_MAXPATH 255

#define MAX_PNT 6
#define MAX_OBJECTS 128
#define MIN_SCALE 0
#define MAX_SCALE 100

#define BRF_DEFAULT_FLIGHT_NAME "Flight1"
#define BRF_DEFAULT_BUG_CLASS   "CINEMA"


class TPoint2D{
public:
   TPoint2D() { x = y = 0;}
   TPoint2D(int X, int Y) { x = X; y = Y;}
   int x, y;
};
inline void Shift(TPoint2D &p, int dx, int dy){
   p.x += dx;
   p.y += dy;
}

class T2DObject{
public:
   T2DObject() {}
   T2DObject(int pNum) { m_pNum = pNum; }
   ~T2DObject() {}

   void Sync();
   void Move(TPoint2D to);
   void RedrawAt(TPoint2D at, int c);
   void Advance();
   void Rotate(TPoint2D center, int angle);

   int m_pNum;
   TPoint2D m_p[MAX_PNT];
   TPoint2D m_srcP[MAX_PNT];
};

class T2DAgregat{
public:
   T2DAgregat() {}
   T2DAgregat(int objNum) { m_objNum = objNum;}
   ~T2DAgregat() {}

   void RedrawAt(TPoint2D at, int angle);

   int m_objNum;
   T2DObject m_obj[MAX_OBJECTS];
   TPoint2D  m_center[MAX_OBJECTS];
   int       m_color[MAX_OBJECTS];
};

class CChannelMap;


class TFlicPlayer : public KR_Object{
public:
	TFlicPlayer() : KR_Object() { m_isPlay = FALSE; }
   ~TFlicPlayer() {}

   virtual int receiveEvent(KR_Event &event);
   void addNotify   ();
   void removeNotify();

   void Init();

   int Play(CConfigFile &cfg, const char *flicSectName,
            byte *buf, int scrX, int scrY, byte *pal64);
   void ChangeResEvent();
   int  IsPlay() const { return(m_isPlay);}
   virtual bool	shouldDump () { return false; } 

protected:
   void OutScreen(const byte *scrBuf, int flicLen, int flicH);

   byte *VHLine(byte color, int cnt, byte *vptr)  const;
   byte *VHLineW(word color, int cnt, byte *vptr) const;
   void SetPal64(const byte *pal);

   void Load_FLI_COLOR(byte *pal64);
   void Load_FLI_COLOR256(byte *pal64);
   void Load_FLI_LC(int scrX, int scrY);
   void Load_FLI_WORD_LC(int scrX, int scrY);
   void Load_FLI_BRUN(int scrX, int scrY, int lines, int colom);
   void Load_FLI_COPY(int scrX, int scrY, int lines, int colom);
   void Load_FLI_BLACK(int scrX, int scrY, int lines, int colom) const;
   int  LoadChunk(int scrX, int scrY, byte *pal64, int lines, int colom);
   int  LoadFrame(FILE *f, int scrX, int scrY, byte *pal64, int lines, int colom);

   int   m_key;
   int   m_flicW;
   int   m_isPlay;
   byte *m_frameBuf;
   byte *m_scrBuf;
   CGRImage image;
};

class CBriefing : public KR_Object{
   struct SCtrlPoint{
      SCtrlPoint()  { text[0] = 0; }
      ~SCtrlPoint() { }

      CFVector3 pos;
      double yaw, pitch, roll;
      double time;
      double delay;
      char   text[BRF_MAX_STR_LEN];
      double textDelay;
   };
   enum { FLT_NODEFINED, FLT_NONE, FLT_CINEMA, FLT_TV, FLT_CRYSTAL, FLT_GYPNOSE };
public:
   CBriefing(int flicBufSize);
   virtual ~CBriefing();

   virtual int receiveEvent(KR_Event &event);
   void addNotify   ();
   void removeNotify();

   void PlayBriefing(const char *fName);

   // Tools functions
   void BeginRecord(int maxPointsNum);
   void EndRecord();
   void WriteFlight(const char *defFileName) const;
   void WriteObjects(const char *defFileName) const;
   void AddObjectPos(double prevFlyTime, const char *str);
   void AddCtrlPoint(double prevFlyTime);
   void ChangeResEvent();
   double GetTotalBrfTime() const { return(m_brfTotalTime);}
   double GetRemainTime() const {return(m_brfTotalTime-m_time); }
   double GetBriefingTime(const char *fName);

   int  IsRecording() const { return(m_isRecording); }
   virtual bool	shouldDump () { return false; } 
protected:
   int  PlayFlic(CConfigFile &cfg, const char *flicSectName);
   int  PlayFlight(CConfigFile &cfg, const char *flightName,
                   const char *effClassName, int pNum);

   int  PerformFly(int pNum);
   void Effects(double frRendTime) const;
   void TranslatePalette() const;
   void CinemaBugs(double frRendTime) const;
   void TVBugs(double frRendTime)     const;
   void CrystalBugs(double frRendTime)  const;
   void GypnoseBugs(double frRendTime)  const;

   int  Random(int maxVal)   const { return(rand()*maxVal/RAND_MAX); }

   double m_brfTotalTime;
   double m_time;
   double m_recordPrevTime;
   int m_key;
   bool m_isRecording;
   bool m_isPlaying;
   SCtrlPoint m_point[BRF_MAX_POINTS_NUM];
   SCtrlPoint m_object[BRF_MAX_OBJ_NUM];
   int  m_objectsNum;
   int  m_pointsNum;
   int  m_maxPointsNum;
   BYTE m_sysPal[3*256];

   int  m_scrW;
   int  m_scrH;

   int m_effClass;

   byte           *m_flicBuffer;
   TFlicPlayer    *m_flicPlayer;
   byte            m_flicPal[256*3];
   CChannelMap    *m_camX, *m_camY,  *m_camZ;
   CChannelMap    *m_yaw,  *m_pitch, *m_roll;
};

extern CBriefing   g_briefing;

#endif //__BRIEFING_H__
