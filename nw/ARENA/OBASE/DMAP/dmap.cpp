#include "dmap.h"
#include "kernel/h/context.h"
#include "kernel/h/session.h"
#include "kernel/h/s_debug.h"
#include "kernel/h/echo.h"
#include "Storage/h/subject.h"
#include "vehicle.h"
#include "filesys.h"
#include "i/dynobj.i"
#include "d3d.h"

extern SDeviceList _dL;
extern IDirect3DDevice2       *_d3dDevice;
extern float __HazeLen;

char DebugMap::m_levelName[256];

//=======================================================
DebugMap::DebugMap(){
   m_levelMap = new CGRImage;
}
//---------------------------------------------
DebugMap::~DebugMap(){
   delete m_levelMap;
}
//---------------------------------------------
void DebugMap::Init(const char * mapName)
{
   m_levelMap->LoadFromBMPFile(mapName, 0, 0);
   strcpy(m_levelName, mapName);

   m_mapW = m_levelMap->Width();
   m_mapH = m_levelMap->Height();
   m_mapScaleX  = (float)m_mapW / (512.*10.);
   m_mapScaleY  = (float)m_mapH / (512.*10.);
   m_mapiScaleX = (512.*10.) / (float)m_mapW;
   m_mapiScaleY = (512.*10.) / (float)m_mapH;

   m_mapScrollL = g_hardware.SearchCode("Left");
   m_mapScrollR = g_hardware.SearchCode("Right");
   m_mapScrollU = g_hardware.SearchCode("Up");
   m_mapScrollD = g_hardware.SearchCode("Down");

   m_selfColor     = GRTransparentColor(255, 255, 255);
   m_selfRColor    = GRFillColor(255, 255, 255);
   m_friendColor   = GRTransparentColor(0, 255, 0);
   m_enemyColor    = GRTransparentColor(255, 0, 0);
   m_objectColor   = GRTransparentColor(128, 128, 128);
   m_portalColor   = GRCreateColor(255, 0, 255);
   m_artefactColor = GRFillColor(0, 0, 255);

   m_artefactR = 10.0;

   m_textBoxX          = 10;
   m_textBoxY          = 10;
   m_textBoxOffX       = 4;
   m_textBoxOffY       = 4;
   m_textBoxStrQnty    = 5;
   m_textBoxSliderDx   = 14;
   m_textBoxSliderDy   = 14;
   m_textBoxSlOnColor  = GRTransparentColor(0,128,0);
   m_textBoxSlOffColor = GRTransparentColor(0,0,0);
   m_textBoxColor      = GRTransparentColor(0,0,0);


   m_active            = FALSE;
   m_drawRoutes        = TRUE;
   m_drawFriendObjects = TRUE;
   m_drawEnemyObjects  = TRUE;
   m_drawPortal        = FALSE;
   m_drawArtefact      = TRUE;

   m_enableDraw        = TRUE;

   m_followMode        = TRUE;

   m_winBaseX = 0;
   m_winBaseY = 0;
   m_step     = 6;

   m_winW    = Min(_gr_nScreenWidth,  m_mapW);
   m_winH    = Min(_gr_nScreenHeight, m_mapH);
   m_winX    = Max(0, (_gr_nScreenWidth  - m_mapW) / 2);
   m_winY    = Max(0, (_gr_nScreenHeight - m_mapH) / 2);

   m_panelW       = 320 - m_textBoxX*2;
   m_panelH       = 32;
   m_panelX       = (m_winW - m_panelW)/2;
   m_panelY       = m_winH - m_panelH - m_textBoxY;
   m_panelDamageX = m_panelX+4;
   m_panelDamageY = m_panelY+8;

   m_panelColor = GRTransparentColor(0,0,0);

   m_curMission = 0;
   for(int i = 0;i < MAX_MISSIONS;i++)
      DeleteMission(i);


   m_winDx = m_winW/4;
   m_winDy = m_winH/4;

   CRect2      viewRect(m_winX + (m_winW*3)/4 - m_textBoxX,
                        m_winY + m_textBoxY,
                        m_winX + m_winW - m_textBoxX,
                        m_winY + m_winH/4 + m_textBoxY
                       );

   m_vPort = GRCreateViewport(m_winX + (m_winW*7)/8 - m_textBoxX,
                              m_winY + m_winH/8 + m_textBoxY,
                              viewRect
                             );
}

//---------------------------------------------
void DebugMap::DeInit()
{
   m_levelMap->Delete();

   m_enableDraw = FALSE;
}

//---------------------------------------------
TMissionId DebugMap::CreateMission(const char *name)
{
   for(int i = 0;i < MAX_MISSIONS;i++)
      if (m_mission[i].use == FALSE) {
          m_mission[i].use = TRUE;
          strcpy(m_mission[i].text.name, name);
          return i;
      }

   return -1;
}

//---------------------------------------------
void DebugMap::DeleteMission(TMissionId mId)
{
   if (mId < 0 || mId >= MAX_MISSIONS) return;

   m_mission[mId].use = 0;
   ClearMission(mId);
}

//---------------------------------------------
void DebugMap::ClearMission(TMissionId mId)
{
   if (mId < 0 || mId >= MAX_MISSIONS) return;

   m_mission[mId].text.str[0] = 0;
   m_mission[mId].text.name[0] = 0;
   m_mission[mId].text.textLineQnty = 0;
   m_mission[mId].text.font = NULL;
   m_mission[mId].routesNum = 0;
}

//---------------------------------------------
void DebugMap::AddMissionRoute(TMissionId mId, IRouteObject * route,
                        unsigned long rgb, float widthS, float widthE)
{
   if (mId < 0 || mId >= MAX_MISSIONS || m_mission[mId].use == 0) return;

   SMapRoute *r = &(m_mission[mId].route[m_mission[mId].routesNum++]);

   r->pointsNum = route->GetNodeCnt();
   for(int i = 0; i < r->pointsNum; i++) {
      CFVector3 node = route->GetNode(i);

      r->point[i*2+0] = m_winX + (int)(node.x*m_mapScaleX);
      r->point[i*2+1] = m_winY + m_mapH + (int)(node.z*m_mapScaleY);
   }

   r->widthS = widthS;
   r->widthE = widthE;
   r->color = GRCreateColor(rgb>>16, (rgb>>8) & 0xff, rgb & 0xff);
}

//---------------------------------------------
void DebugMap::AddMissionText(TMissionId mId, const char * str, 
	const char * fntObjName)
	//CFixedColorFont *fnt)
{  SMapText *txt;
   int i;
   char *tmp;

   if (mId < 0 || mId >= MAX_MISSIONS || m_mission[mId].use == 0) return;

   txt = &m_mission[mId].text;
   
   //txt->font = fnt;
   KR_ObjectID  fontID = g_arena.getContext()->searchObject(fntObjName);
   txt->font = (IFixedFont*)(g_arena.getContext()->queryInterface(fontID,IFixedFontIID));
   txt->textCurrLine = 0;
   strcpy(txt->str, str);

   txt->textLineQnty = 1;
   for(i = 0; txt->str[i] != 0;i++)
      if (txt->str[i] == '$') {
         txt->str[i] = 0;
         txt->textLineQnty++;
      }

   txt->dy = txt->font->Height();
   txt->x  = m_textBoxX + m_textBoxOffX;
   txt->y  = m_textBoxY + m_textBoxOffY + txt->dy;

   txt->boxDy = m_textBoxOffY*2 + txt->dy*(m_textBoxStrQnty+1); //+1 for name

   txt->boxDx = 0;
   tmp = txt->str;

   int len;

   for(i = 0;i < txt->textLineQnty;i++) {
      len = txt->font->StringWidth(tmp);
      if (len > txt->boxDx)
         txt->boxDx = len;

      while(*tmp++ != 0);
   }

   len = txt->font->StringWidth(txt->name);
   if (len > txt->boxDx)
      txt->boxDx = len;

   txt->boxDx += m_textBoxOffX*2 + m_textBoxSliderDx;

   txt->sliderUpDownOffX = txt->boxDx - m_textBoxOffX - m_textBoxSliderDx;
   txt->sliderUpOffY     = txt->dy + m_textBoxOffY;
   txt->sliderDownOffY   = txt->boxDy - m_textBoxOffY - m_textBoxSliderDy;
}


//---------------------------------------------
void DebugMap::Draw() {
    int i,j;
    SMapMission *mM = &m_mission[m_curMission];
    IPlayer        *player;

    if (!m_enableDraw) return;

    //patch clipRect
    _gr_clipRect.left   = m_winX -_gr_nScreenOriginX;
    _gr_clipRect.right  = m_winX + m_winW - _gr_nScreenOriginX;
    _gr_clipRect.top    = m_winY -_gr_nScreenOriginY;
    _gr_clipRect.bottom = m_winY + m_winH - _gr_nScreenOriginY;
    GRSetClipRect();

    m_self = (IDynamicObject *)(g_vehicle->queryInterface(IDynamicObjectIID));
    player = (IPlayer *)(g_vehicle->queryInterface(IPlayerIID));

    //draw Map
    GREndScene();

    if (m_followMode) {
       CFVector3 pos = m_self->getPos();
       int x = (int)(pos.x*m_mapScaleX);
       int y = m_mapH + (int)(pos.z*m_mapScaleY);

       if (x + m_winW/2 > m_mapW)
          m_winBaseX = m_mapW - m_winW;
       else {
          if (x - m_winW/2 < 0)
             m_winBaseX = 0;
          else
             m_winBaseX = x - m_winW/2;
       }

       if (y + m_winH/2 > m_mapH)
          m_winBaseY = m_mapH - m_winH;
       else {
          if (y - m_winH/2 < 0)
             m_winBaseY = 0;
          else
             m_winBaseY = y - m_winH/2;
       }
    }

    m_levelMap->Draw(m_winX, m_winY, m_winBaseX, m_winBaseY, m_winBaseX+m_winW, m_winBaseY+m_winH);

    GRStartScene();

    GRZBufferEnable(0);
    if (_dL.currDevice->swHw == GR_HARDWARE)
       _d3dDevice->SetRenderState(D3DRENDERSTATE_EDGEANTIALIAS, TRUE);

    //draw Routes
    if (m_drawRoutes) {
       int points[MAX_ROUTE_POINTS*2];
       SMapRoute *rt = mM->route;

       for(i = 0;i < mM->routesNum;i++,rt++) {
          for(j = 0;j < rt->pointsNum*2;j += 2) {
             points[j+0] = rt->point[j+0] - m_winBaseX;
             points[j+1] = rt->point[j+1] - m_winBaseY;
          }

          GRLUDrawArrow(points, rt->pointsNum, rt->widthS, rt->widthE, rt->color);
       }
    }

    //draw Portal
    if (m_drawPortal) {
    }


    //draw Friend ,Enemy  and Artefact Objects
    if (m_drawFriendObjects || m_drawEnemyObjects || m_drawArtefact) {
       float xMin, yMin, xMax, yMax;
       ct_SubjectFindData fsd;
       IDynamicObject *dyn;
       IUnit          *unit;
       IArtefact      *art;


       xMin =  m_winBaseX * m_mapiScaleX;
       yMin =  (m_winBaseY - m_mapH) * m_mapiScaleY;
       xMax = xMin + m_winW*m_mapiScaleX;
       yMax = yMin + m_winH*m_mapiScaleY;
       ct_Arena::findFirstSubject( fsd, xMin, yMin, xMax, yMax);

       for(i = 0; i < fsd.getCount(); ++i ) {
          KR_ObjectID id = fsd[i];

          if (id == KR_ObjectID::NUL()) continue;

          dyn = (IDynamicObject *)(getContext()->queryInterface(id, IDynamicObjectIID));
          if (dyn == NULL) continue;

          art = (IArtefact *)(getContext()->queryInterface(id, IArtefactIID));

          if (art != NULL) {
             if (m_drawArtefact)
                if (!art->isAttached() || art->m_carrierID != g_vehicle->getObjectID())
                   DrawArtefact(dyn, m_artefactColor, TRUE);
          }
          else {
             unit = (IUnit*) (getContext()->queryInterface(id, IUnitIID));

             if (unit != NULL) {
                if (dyn != m_self) {
                   KR_ObjectID com = unit->getCommander();

                   if (com.isNUL())
                      DrawDynamicObject(dyn, m_objectColor, FALSE);
                   else {
                      if (player->isRenegat(com)) {
                         if (m_drawEnemyObjects)
                            DrawDynamicObject(dyn, m_enemyColor, TRUE);
                      }
                      else {
                         if (m_drawFriendObjects)
                            DrawDynamicObject(dyn, m_friendColor, FALSE);
                      }
                   }
                }
             }
          }
       }

    }

    //draw Self
    DrawSelf(m_self, m_selfColor);

    //draw Mission Text
    if (mM->text.font != NULL)
       DrawMissionText(&mM->text);

    //draw Panel
    GRStartScene();

    if (mM->text.font != NULL)
       DrawPanel(player);

    if (_dL.currDevice->swHw == GR_HARDWARE)
       _d3dDevice->SetRenderState(D3DRENDERSTATE_EDGEANTIALIAS, FALSE);
    GRZBufferEnable(1);
    //restore clipRect
    _gr_clipRect = GRGetViewport()->clipRect;
    GRSetClipRect();

    GRStartScene();

}
//---------------------------------------------
void DebugMap::DrawSelf(IDynamicObject * obj, unsigned long col)
{  CFVector3 objPos = obj->getPos();
   float     x, y, r;

   DrawDynamicObject(obj, col, FALSE);

   //GREndScene();
   GREnable2D();

   x  = m_winX - m_winBaseX + (int)(objPos.x*m_mapScaleX);
   y  = m_winY + m_mapH - m_winBaseY + (int)(objPos.z*m_mapScaleY);
   r  = __HazeLen*m_mapScaleX;

   Circle((int)x, (int)y, (int)r, m_selfRColor, FALSE);

   GRDisable2D();
   //GRStartScene();
}
//---------------------------------------------

#define PROC 0.7

void DebugMap::DrawDynamicObject(IDynamicObject * obj, unsigned long col, int checkR)
{  CFVector3 objPos = obj->getPos();
   float     objR   = obj->getRadius();
   float     hAng   = obj->getHAngle();
   float     x, y;
   float     r,r1;
   float     dx,dy;

   if (checkR) {
      CFVector3 sPos = m_self->getPos();
      if (Abs2(objPos - sPos) >= __HazeLen*__HazeLen)
         return;
   }

   x  = m_winX - m_winBaseX + (int)(objPos.x*m_mapScaleX) - _gr_nScreenOriginX;
   y  = m_winY + m_mapH - m_winBaseY + (int)(objPos.z*m_mapScaleY) - _gr_nScreenOriginY;
   r  = Max(objR*m_mapScaleX, 8.);
   r1 = r*PROC;

   _gr_polygon.dwFullType = GR_POLY_TRANSPARENT;
   _gr_polygon.dwAddType = 0;
   _gr_polygon.nVertices = 3;
   _gr_polygon.dwColor.color = col;
   _gr_polygon.dwOpacity = 200;
   _gr_polygon.nLights = 0;

   dx = cos(hAng);
   dy = sin(hAng);

   _gr_vertices[0].any.x = (int)(x + dx*r);
   _gr_vertices[0].any.y = (int)(y + dy*r);
   _gr_vertices[0].any.iz = 6550;

   x -= (dx*r);
   y -= (dy*r);

   _gr_vertices[1].any.x = (int)(x + dy*r1);
   _gr_vertices[1].any.y = (int)(y - dx*r1);
   _gr_vertices[1].any.iz = 6550;

   _gr_vertices[2].any.x = (int)(x - dy*r1);
   _gr_vertices[2].any.y = (int)(y + dx*r1);
   _gr_vertices[2].any.iz = 6550;

   GRDrawPolygonPCCW();

}
//---------------------------------------------
void DebugMap::DrawArtefact(IDynamicObject * obj, unsigned long col, int checkR)
{  CFVector3 objPos = obj->getPos();
   float     x, y, time;

   if (checkR) {
      CFVector3 sPos = m_self->getPos();
      if (Abs2(objPos - sPos) >= __HazeLen*__HazeLen)
         return;
   }

   //GREndScene();
   GREnable2D();

   x  = m_winX - m_winBaseX + (int)(objPos.x*m_mapScaleX);
   y  = m_winY + m_mapH - m_winBaseY + (int)(objPos.z*m_mapScaleY);

   Circle((int)x, (int)y, 3, col, TRUE);

   time = Session::m_realTimer->GetTime();

   Circle((int)x, (int)y, (int)(m_artefactR*(time - ((int)time))), col, FALSE);

   GRDisable2D();
   //GRStartScene();
}
//---------------------------------------------
void DebugMap::DrawPanel(IPlayer *player)
{  char str[40];

   (void)player;

   _gr_polygon.dwFullType = GR_POLY_TRANSPARENT;
   _gr_polygon.dwAddType = 0;
   _gr_polygon.nVertices = 4;
   _gr_polygon.dwOpacity = 128;
   _gr_polygon.dwColor.color = m_panelColor;
   _gr_polygon.nLights = 0;

   _gr_vertices[0].any.x = m_panelX - _gr_nScreenOriginX;
   _gr_vertices[0].any.y = m_panelY - _gr_nScreenOriginY;
   _gr_vertices[0].any.iz = 6550;

   _gr_vertices[1].any.x = m_panelX - _gr_nScreenOriginX;
   _gr_vertices[1].any.y = m_panelY - _gr_nScreenOriginY + m_panelH;
   _gr_vertices[1].any.iz = 6550;

   _gr_vertices[2].any.x = m_panelX - _gr_nScreenOriginX + m_panelW;
   _gr_vertices[2].any.y = m_panelY - _gr_nScreenOriginY + m_panelH;
   _gr_vertices[2].any.iz = 6550;

   _gr_vertices[3].any.x = m_panelX - _gr_nScreenOriginX + m_panelW;
   _gr_vertices[3].any.y = m_panelY - _gr_nScreenOriginY;
   _gr_vertices[3].any.iz = 6550;

   GRDrawPolygonPCCW();

   GREndScene();

   sprintf(str,"Health  %d%%", (int)(g_vehicle->getDamage()*100.));

   m_mission[m_curMission].text.font->LUPrintAt(m_panelDamageX, m_panelDamageY, str);
}

//---------------------------------------------
void DebugMap::DrawMissionText(SMapText * mT)
{  char *tmp;
   int i, drL, y;

   //GREnableZBuffer(0);

   _gr_polygon.dwFullType = GR_POLY_TRANSPARENT;
   _gr_polygon.dwAddType = 0;
   _gr_polygon.nVertices = 4;
   _gr_polygon.dwOpacity = 128;
   _gr_polygon.dwColor.color = m_textBoxColor;
   _gr_polygon.nLights = 0;

   _gr_vertices[0].any.x = m_textBoxX - _gr_nScreenOriginX;
   _gr_vertices[0].any.y = m_textBoxY - _gr_nScreenOriginY;
   _gr_vertices[0].any.iz = 6550;

   _gr_vertices[1].any.x = m_textBoxX - _gr_nScreenOriginX;
   _gr_vertices[1].any.y = m_textBoxY - _gr_nScreenOriginY + mT->boxDy;
   _gr_vertices[1].any.iz = 6550;

   _gr_vertices[2].any.x = m_textBoxX - _gr_nScreenOriginX + mT->boxDx;
   _gr_vertices[2].any.y = m_textBoxY - _gr_nScreenOriginY + mT->boxDy;
   _gr_vertices[2].any.iz = 6550;

   _gr_vertices[3].any.x = m_textBoxX - _gr_nScreenOriginX + mT->boxDx;
   _gr_vertices[3].any.y = m_textBoxY - _gr_nScreenOriginY;
   _gr_vertices[3].any.iz = 6550;

   GRDrawPolygonPCCW();

   //draw Sliders
   //up
   _gr_polygon.dwFullType = GR_POLY_TRANSPARENT;
   _gr_polygon.dwAddType = 0;
   _gr_polygon.nVertices = 3;
   _gr_polygon.dwOpacity = 200;
   if (mT->textCurrLine > 0)
      _gr_polygon.dwColor.color = m_textBoxSlOnColor;
   else
      _gr_polygon.dwColor.color = m_textBoxSlOffColor;
   _gr_polygon.nLights = 0;

   _gr_vertices[0].any.x = m_textBoxX - _gr_nScreenOriginX +
                           mT->sliderUpDownOffX + m_textBoxSliderDx/2;
   _gr_vertices[0].any.y = m_textBoxY - _gr_nScreenOriginY +
                           mT->sliderUpOffY;
   _gr_vertices[0].any.iz = 6550;

   _gr_vertices[1].any.x = m_textBoxX - _gr_nScreenOriginX +
                           mT->sliderUpDownOffX;
   _gr_vertices[1].any.y = m_textBoxY - _gr_nScreenOriginY +
                           mT->sliderUpOffY + m_textBoxSliderDy;
   _gr_vertices[1].any.iz = 6550;

   _gr_vertices[2].any.x = m_textBoxX - _gr_nScreenOriginX +
                           mT->sliderUpDownOffX + m_textBoxSliderDx;
   _gr_vertices[2].any.y = m_textBoxY - _gr_nScreenOriginY +
                           mT->sliderUpOffY + m_textBoxSliderDy;
   _gr_vertices[2].any.iz = 6550;

   GRDrawPolygonPCCW();

   //down
   _gr_polygon.dwFullType = GR_POLY_TRANSPARENT;
   _gr_polygon.dwAddType = 0;
   _gr_polygon.nVertices = 3;
   _gr_polygon.dwOpacity = 200;
   if (mT->textLineQnty - mT->textCurrLine > m_textBoxStrQnty)
      _gr_polygon.dwColor.color = m_textBoxSlOnColor;
   else
      _gr_polygon.dwColor.color = m_textBoxSlOffColor;
   _gr_polygon.nLights = 0;

   _gr_vertices[0].any.x = m_textBoxX - _gr_nScreenOriginX +
                           mT->sliderUpDownOffX + m_textBoxSliderDx/2;
   _gr_vertices[0].any.y = m_textBoxY - _gr_nScreenOriginY +
                           mT->sliderDownOffY + m_textBoxSliderDy;
   _gr_vertices[0].any.iz = 6550;

   _gr_vertices[1].any.x = m_textBoxX - _gr_nScreenOriginX +
                           mT->sliderUpDownOffX + m_textBoxSliderDx;
   _gr_vertices[1].any.y = m_textBoxY - _gr_nScreenOriginY +
                           mT->sliderDownOffY;
   _gr_vertices[1].any.iz = 6550;

   _gr_vertices[2].any.x = m_textBoxX - _gr_nScreenOriginX +
                           mT->sliderUpDownOffX;
   _gr_vertices[2].any.y = m_textBoxY - _gr_nScreenOriginY +
                           mT->sliderDownOffY;
   _gr_vertices[2].any.iz = 6550;

   GRDrawPolygonPCCW();

   //draw Mission Sliders
   y = (mT->boxDx - m_textBoxOffX*2 - m_textBoxSliderDx*2 - mT->font->StringWidth(mT->name))/2;
   int nameX0 = mT->x + m_textBoxSliderDx + y;
   int nameX1 = mT->x + m_textBoxSliderDx + y + mT->font->StringWidth(mT->name);
   //draw left Slider
   _gr_polygon.dwFullType = GR_POLY_TRANSPARENT;
   _gr_polygon.dwAddType = 0;
   _gr_polygon.nVertices = 3;
   _gr_polygon.dwOpacity = 200;

   drL = 0;
   for(i = m_curMission - 1;i >= 0;i--)
      if (m_mission[i].use) {
         drL = 1;
         break;
      }
   if (drL)
      _gr_polygon.dwColor.color = m_textBoxSlOnColor;
   else
      _gr_polygon.dwColor.color = m_textBoxSlOffColor;

   _gr_polygon.nLights = 0;

   _gr_vertices[0].any.x = -2 - _gr_nScreenOriginX +
                           nameX0 - m_textBoxSliderDx;
   _gr_vertices[0].any.y = m_textBoxY - _gr_nScreenOriginY +
                           m_textBoxOffY + mT->dy/2;
   _gr_vertices[0].any.iz = 6550;

   _gr_vertices[1].any.x = -2 - _gr_nScreenOriginX +
                           nameX0;
   _gr_vertices[1].any.y = m_textBoxY - _gr_nScreenOriginY +
                           m_textBoxOffY + mT->dy;
   _gr_vertices[1].any.iz = 6550;

   _gr_vertices[2].any.x = -2 - _gr_nScreenOriginX +
                           nameX0;
   _gr_vertices[2].any.y = m_textBoxY - _gr_nScreenOriginY +
                           m_textBoxOffY;
   _gr_vertices[2].any.iz = 6550;

   GRDrawPolygonPCCW();

   //draw right Slider
   _gr_polygon.dwFullType = GR_POLY_TRANSPARENT;
   _gr_polygon.dwAddType = 0;
   _gr_polygon.nVertices = 3;
   _gr_polygon.dwOpacity = 200;

   drL = 0;
   for(i = m_curMission + 1;i < MAX_MISSIONS;i++)
      if (m_mission[i].use) {
         drL = 1;
         break;
      }
   if (drL)
      _gr_polygon.dwColor.color = m_textBoxSlOnColor;
   else
      _gr_polygon.dwColor.color = m_textBoxSlOffColor;

   _gr_polygon.nLights = 0;

   _gr_vertices[0].any.x =  - _gr_nScreenOriginX +
                           nameX1 + m_textBoxSliderDx;
   _gr_vertices[0].any.y = m_textBoxY - _gr_nScreenOriginY +
                           m_textBoxOffY + mT->dy/2;
   _gr_vertices[0].any.iz = 6550;

   _gr_vertices[1].any.x =  - _gr_nScreenOriginX +
                           nameX1;
   _gr_vertices[1].any.y = m_textBoxY - _gr_nScreenOriginY +
                           m_textBoxOffY;
   _gr_vertices[1].any.iz = 6550;

   _gr_vertices[2].any.x =  - _gr_nScreenOriginX +
                           nameX1;
   _gr_vertices[2].any.y = m_textBoxY - _gr_nScreenOriginY +
                           m_textBoxOffY + mT->dy;
   _gr_vertices[2].any.iz = 6550;

   GRDrawPolygonPCCW();

   //draw Text
   GREndScene();

   //mission Name

   mT->font->LUPrintAt(nameX0, m_textBoxY + m_textBoxOffY, mT->name);

   tmp = mT->str;
   for(i = 0;i < mT->textCurrLine;i++) {
      while(*tmp++ != 0);
   }

   drL = Min(m_textBoxStrQnty, mT->textLineQnty - mT->textCurrLine);

   y = mT->y;
   for(i = 0;i < drL;i++) {
      mT->font->LUPrintAt(mT->x, y, tmp);
      y += mT->dy;
      while(*tmp++ != 0);
   }

   //GREnableZBuffer(1);
}
//---------------------------------------------
void DebugMap::Pixel(int x, int y, int color) const{
    GRPset(x, y, color);
}
//---------------------------------------------
void DebugMap::Line(int x0, int y0, int x1, int y1, int color) const{
    GRLine(x0, y0, x1, y1, color);
}
//---------------------------------------------
void DebugMap::Rectangle(int x0, int y0, int x1, int y1, int color) const{
    GRRect(x0, y0, x1, y1, color);
}
//---------------------------------------------
void DebugMap::Circle(int x, int y, int r, int color, int fill)
{
    GRCircle(x, y, r, color, fill);
}

//---------------------------------------------
//---------------------------------------------
void DebugMap::addNotify(){
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

	KR_Object::addNotify();
}
//---------------------------------------------
void DebugMap::removeNotify(){
        KR_Object::removeNotify();

        KR_Event event;

        event.timeStamp   = Session::m_moment;
        event.source      = getObjectID();
        event.destination = g_hardware.getObjectID();

        if (event.destination.isNUL())
                return;

        event.label               = CTRL_UNSUBSCRIBE;
        event.data.open(EDO_WRITE)
                                .putObjectID(getObjectID())
                          .close();
//      getContext()->addEvent(event);
        if (context)
           context->sendEventNow(event);


}
//---------------------------------------------
int DebugMap::receiveEvent(KR_Event &event){
	int code;
	int ctrlEvent;
	double timeStamp, down;
	int repeat;

	switch( event.label ){
      case KR_WAKE_UP:  break;
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

         if (down == 0) break;

         if (ctrlEvent == DMAP_TOGGLE) {
            if (m_active) {
               m_active = FALSE;
               EnableRender3D();
               if (!m_followMode)
                  g_hardware.ReleaseExclusiveMode();
            }
            else{
               m_active = TRUE;
               DisableRender3D();
               if (!m_followMode)
                  g_hardware.GetExclusiveMode(getObjectID());
            }
         }

         if (!m_active) break;

         if (!m_followMode) {
            if (code ==  m_mapScrollL) {
               m_winBaseX -= m_step;
               m_winBaseX = Max(0, m_winBaseX);
            }
            else
            if (code == m_mapScrollR) {
               m_winBaseX += m_step;
               m_winBaseX = Min(m_mapW-m_winW, m_winBaseX);
            }
            else
            if (code == m_mapScrollU) {
                  m_winBaseY -= m_step;
                  m_winBaseY = Max(0, m_winBaseY);
            }
            else
            if (code == m_mapScrollD) {
                  m_winBaseY += m_step;
                  m_winBaseY = Min(m_mapH-m_winH, m_winBaseY);
            }
         }

         switch(ctrlEvent){
            case DMAP_TOGGLE_FOLLOW_MODE:
               m_followMode = !m_followMode;
               if (m_followMode)
                  g_hardware.ReleaseExclusiveMode();
               else
                  g_hardware.GetExclusiveMode(getObjectID());
            break;

            case DMAP_NEXT_MISSION: {
               for(int i = m_curMission + 1;i < MAX_MISSIONS;i++)
                  if (m_mission[i].use) {
                     m_curMission = i;
                     break;
                  }
            }
            break;

            case DMAP_PREVIOUS_MISSION: {
               for(int i = m_curMission - 1;i >= 0;i--)
                  if (m_mission[i].use) {
                     m_curMission = i;
                     break;
                  }
            }
            break;

            case DMAP_TEXT_BOX_UP:
               if (m_mission[m_curMission].text.textCurrLine > 0)
                  m_mission[m_curMission].text.textCurrLine--;
            break;

            case DMAP_TEXT_BOX_DOWN: {
               SMapText *mT = &m_mission[m_curMission].text;

               if (mT->textCurrLine < mT->textLineQnty - m_textBoxStrQnty)
                  mT->textCurrLine++;
            }
            break;

            default: break;
			}

      break;

      default:
      return(0);
   }
 	return(1);
}
//---------------------------------------------
void DebugMap::EnableRender3D()
{
}
//---------------------------------------------
void DebugMap::DisableRender3D()
{
}

void DebugMap::ClearMissions()
{
   m_curMission = 0;
   for(int i = 0;i < MAX_MISSIONS;i++)
      DeleteMission(i);
}