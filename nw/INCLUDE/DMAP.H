#ifndef __DMAP_H__
#define __DMAP_H__

#include "kernel\h\object.h"
#include "hardware.h"
#include "i/route.i"
#include "i/dynobj.i"
#include "i/player.i"
#include "i/font.i"


// These defines live now in player.i

//#define MAX_ROUTES       8
//#define MAX_ROUTE_POINTS 128
//#define MAX_TEXT_LEN     2048
//#define MAX_MISSIONS     8

class CGRImage;

typedef struct {
   int           pointsNum;
   int           point[MAX_ROUTE_POINTS*2];
   unsigned long color;
   float         widthS,
                 widthE;
} SMapRoute;

typedef struct {
   char            name    [40];
   //CFixedColorFont *font;   
   int             textCurrLine;
   int             textLineQnty;
   char            str[MAX_TEXT_LEN];

   int             x, y;
   int             dy;

   int             boxDx, boxDy;
   int             sliderUpDownOffX;
   int             sliderUpOffY;
   int             sliderDownOffY;
   IFixedFont	   * font;
} SMapText;




typedef struct {
   int             use;
   int             routesNum;
   SMapRoute       route[MAX_ROUTES];
   SMapText        text;
} SMapMission;

// This structure now lives in player.i
//typedef int TMissionId;



class DebugMap : public KR_Object{

public:
   DebugMap();
   ~DebugMap();

   void addNotify();
   void removeNotify();
   int  receiveEvent(KR_Event &event);

   int  IsActive() const {return(m_active); }

   void Init(const char * mapName);
   void DeInit();

   TMissionId CreateMission(const char *name);
   void DeleteMission(TMissionId mId);

   void ClearMission(TMissionId mId);

   void AddMissionRoute(TMissionId mId, IRouteObject * route, 
			unsigned long rgb, 
			float widthS, 
			float widthE);

   void AddMissionText(TMissionId mId, const char * str, const char * fntObjName);
   void Draw();
   void ClearMissions();

   virtual bool	shouldDump () { return false; }


   SGRViewport *m_vPort;
   int m_winDx,
       m_winDy;

protected:
   void EnableRender3D();
   void DisableRender3D();

   void DrawSelf(IDynamicObject * obj, unsigned long col);
   void DrawDynamicObject(IDynamicObject * obj, unsigned long col, int checkR);
   void DrawArtefact(IDynamicObject * obj, unsigned long col, int checkR);
   void DrawMissionText(SMapText * mT);
   void DrawPanel(IPlayer *player);

   void Pixel(int x, int y, int color) const;
   void Line(int x0, int y0, int x1, int y1, int color) const;
   void Rectangle(int x0, int y0, int x1, int y1, int color) const;
   void Circle(int x, int y, int r, int color, int fill);

   int m_mapW;
   int m_mapH;

   IDynamicObject * m_self;

   float m_mapScaleX;
   float m_mapScaleY;
   float m_mapiScaleX;
   float m_mapiScaleY;

   int m_mapScrollL,
       m_mapScrollR,
       m_mapScrollU,
       m_mapScrollD;


   unsigned long m_selfColor;
   unsigned long m_selfRColor;
   unsigned long m_friendColor;
   unsigned long m_enemyColor;
   unsigned long m_objectColor;
   unsigned long m_unkColor;   //RE don't know color
   unsigned long m_portalColor;
   unsigned long m_artefactColor;

   float m_artefactR;

   int m_textBoxX;
   int m_textBoxY;
   int m_textBoxOffX;
   int m_textBoxOffY;
   int m_textBoxStrQnty;
   int m_textBoxSliderDx;
   int m_textBoxSliderDy;

   unsigned long m_textBoxSlOnColor;
   unsigned long m_textBoxSlOffColor;
   unsigned long m_textBoxColor;

   int m_panelX,
       m_panelY,
       m_panelW,
       m_panelH;
   int m_panelDamageX,
       m_panelDamageY;

   unsigned long m_panelColor;

   int m_active;
   int m_drawRoutes;
   int m_drawFriendObjects;
   int m_drawEnemyObjects;
   int m_drawPortal;
   int m_drawArtefact;

   int m_enableDraw;

   int m_followMode;

   int m_winBaseX;
   int m_winBaseY;
   int m_step;

   int m_winW;
   int m_winH;
   int m_winX;
   int m_winY;

   CGRImage   *m_levelMap;  // bmp image
   CGRImage   *m_artefonImg;  // bmp image
   int         m_curMission;
   SMapMission m_mission[MAX_MISSIONS];
   
   static char m_levelName[256];
};

extern DebugMap g_debugMap;

#endif // __DMAP_H__