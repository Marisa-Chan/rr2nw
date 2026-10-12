#include "windows.h"
#include "graph.h"
#include "sd1_epal.h"
//#include "debugext.h"
#include "d3d.h"


extern TExtendedPalette        _EPal;

extern IDirectDrawSurface     *_BackBuffer;
extern SDeviceList             _dL;

extern IDirect3DDevice2       *_d3dDevice;
extern int LoadTextureTo3DCard(void *tDB);

void D3DSetError(char * title, int error);
extern int BitCount(unsigned int dw);

extern "C" void ASM_GRDrawPanel(unsigned char *panel, int x0, int y0);

#define PANEL_FILE_ID "PNL"

typedef struct {
    char id[3];
    unsigned char version;
    int  resolCount;
    unsigned char palette[768];
}SPanelFileHeader;

typedef struct {
    int flags;
    int height,
        width;
    unsigned char data[1];
}SHwPanelFileTexture;

typedef struct aaaa{
    int width,
        height;

    int originX,
        originY;
    CRect2 clip;

    //---SW Panel--------------
    int swPanelSize;
    //---HW Panel--------------
    int hwPanelSize;
    int hwPanelTexturesSize;
    //-------------------------------
    //--SWPanel data (swPanelSize)
    //--HWPanel data (hwPanelSize)
    //----------HWPanelTextures------
    //--textureSize (int)
    //--SHwPanelFileTexture (textureSize)
    //---------Controls-------------
    //--Controls Count (int)
    //--Controls Data
    //--m.b. dx,dy,Sprite
    //--m.b. size,font
    //----
} SPanelFile;



void ConvertColor(unsigned long *col, int createColor)
{  int r, g, b;

   r = (*col >> 16);
   g = ((*col & 0xFF00) >> 8);
   b = *col & 0xFF;

   if (createColor)
      *col = GRCreateColor(r,g,b);
   else
      *col = GRFillColor(r,g,b);
}

GR_HTEXTURE ReloadFunc(const char *filename, GR_HTEXTURE hT, void *user)
{
   ((CGRPanel*)user)->ReloadTextures(filename, hT);
   return hT;
}


/***********************************************************************
 ********************             CGRPanel          ********************
 ***********************************************************************/

CGRPanel::CGRPanel(const char *filename)
{ FILE * fp = fopen(filename,"rb");
  SPanelFileHeader pH;
  SPanelFile pF;
  int i,j;
  int swPalReloc[256];
  CRect2 clipRect;

  resolCount = 0;
  currentPanel = NULL;

  if (fp == NULL) return;

  if (_dL.currDevice == NULL) {
     fclose(fp);
     return;
  }

  fread(&pH, sizeof(SPanelFileHeader), 1, fp);

  if (strncmp(pH.id, PANEL_FILE_ID, 3) != 0 || pH.version > 0) {
     fclose(fp);
     return;
  }

  resolCount = pH.resolCount;

  if (_dL.currDevice->swHw == GR_SOFTWARE) {
     for(i = 0;i < 256;i++) {
        RGB_i rgb(pH.palette[i*3+0],
                  pH.palette[i*3+1],
                  pH.palette[i*3+2]);
        swPalReloc[i] = epal_Match( _EPal, rgb);
     }
  }

  for(i = 0;i < resolCount;i++) {
     fread(&pF, sizeof(SPanelFile), 1, fp);

     panel[i].width = pF.width;
     panel[i].height = pF.height;
     panel[i].open = 0;
     panel[i].pSWPanel = NULL;
     panel[i].pHWPanel = NULL;
     clipRect.left = 0;
     clipRect.top = 0;
     clipRect.right = pF.width;
     clipRect.bottom = pF.height;

     if (_dL.currDevice->swHw == GR_SOFTWARE) {
        if (pF.swPanelSize > 0) {
           panel[i].pSWPanel = (SSwPanel*) new char [pF.swPanelSize];
           fread(panel[i].pSWPanel, pF.swPanelSize, 1, fp);
           panel[i].panelViewport = GRCreateViewport(pF.originX, pF.originY, pF.clip);
           //-------------Match Panel------------------------------
           unsigned char *swData = panel[i].pSWPanel->pPanel;

           while(1) {
              unsigned short flag = *((unsigned short*)(swData));

              if (flag == 0) break;
              swData += 2;
              if (flag & 0x8000) {
                 flag &= 0x7FFF;
                 while(flag-- > 0) {
                    *swData = (unsigned char) swPalReloc[*swData];
                    swData++;
                 }
              }
           }
        }
        else
           panel[i].panelViewport = GRCreateViewport(pF.width/2, pF.height/2, clipRect);
        //------------------------------------------------------
        fseek(fp, pF.hwPanelSize + pF.hwPanelTexturesSize, SEEK_CUR);
     }
     else {

        int memory = 2200000 *
                     ((_dL.currDevice->textureFormat[NORMAL_TEXTURE_INDEX].usePalette)?1:2);

        fseek(fp, pF.swPanelSize, SEEK_CUR);

        if (pF.hwPanelSize > 0) {
           panel[i].pHWPanel = (SHwPanel*) new char [pF.hwPanelSize];
           fread(panel[i].pHWPanel, pF.hwPanelSize, 1, fp);

           //-----------------Relocate Pointers & Load Textures-------------
           SHwPanel *hwPanel = panel[i].pHWPanel;
           unsigned char * pHWP = (unsigned char*) panel[i].pHWPanel;

           for(j = 0;j < hwPanel->textureCount;j++) {
              hwPanel->texture[j] = (SHwPanelTexture*) (pHWP + ((unsigned long)(hwPanel->texture[j])));

              SHwPanelTexture * hwPanelT = hwPanel->texture[j];
              //-------------Reloc Areas----------------------
              for(int k = 0;k < hwPanelT->numAreas;k++) {
                 hwPanelT->area[k] = (SHwPanelArea*) (pHWP + ((unsigned long)hwPanelT->area[k]));
                 //correct z and oow for ZBuffer
                 for(int l = 0;l < hwPanelT->area[k]->numPoints;l++) {
                    hwPanelT->area[k]->point[l].d3d.oow = 1.0 / 0.4;
                    hwPanelT->area[k]->point[l].d3d.z = 0.0;
                    hwPanelT->area[k]->point[l].d3d.d3dcolor.color = 0xFFFFFFFF;
                    hwPanelT->area[k]->point[l].d3d.specular = 0xFF000000;
                 }
              }
              //-------------Load Texture---------------------
              SHwPanelFileTexture * hwPanelFileT;
              int textSize;

              fread(&textSize, sizeof(int), 1, fp);

              hwPanelFileT = (SHwPanelFileTexture*) new char [textSize];
              fread(hwPanelFileT, textSize, 1, fp);

              if (_dL.currDevice->agp || _dL.currDevice->textureMemory > memory)
                 hwPanelFileT->flags = TEXTURE_PAL_FORMAT | TEXTURE_SPRITE | TEXTURE_RGB;
              else
                 hwPanelFileT->flags = TEXTURE_PAL_FORMAT | TEXTURE_SPRITE;

              hwPanelT->hTexture = GRLoadTextureToDB(NULL, pH.palette, 256, hwPanelFileT->data);

              GRSetTextureLoadFunc( hwPanelT->hTexture, filename, ReloadFunc, (void *) this);

              delete [] hwPanelFileT;
           }
           panel[i].panelViewport = GRCreateViewport(pF.originX, pF.originY, pF.clip);
        }
        else
           panel[i].panelViewport = GRCreateViewport(pF.width/2, pF.height/2, clipRect);
     }

     //LoadControls
     fread(&panel[i].controlCount, 4, 1, fp);

     for(j = 0;j < panel[i].controlCount;j++) {
        SControl *ct = &panel[i].control[j];

        fread(ct, sizeof(SControl), 1, fp);

        switch (ct->type) {
           case PANEL_CTRL_INDICATOR:
              ConvertColor(&ct->indicator.color0,TRUE);
              ConvertColor(&ct->indicator.color1,TRUE);
              ct->controlData.fValue = ct->indicator.alpha0;
           break;

           case PANEL_CTRL_ARROW:
              ConvertColor(&ct->arrow.color,FALSE);
              ct->controlData.fValue = ct->arrow.alpha0;
           break;

           case PANEL_CTRL_MOVE_SPRITE:
           case PANEL_CTRL_FILL_SPRITE: {
              int dx,dy;
              unsigned char *tmp;

              fread(&dx,4,1,fp);
              fread(&dy,4,1,fp);

              tmp = (unsigned char*) new char[dx*dy];
              fread(tmp, dx*dy, 1, fp);

              if (ct->type == PANEL_CTRL_MOVE_SPRITE) {
                 ct->moveSprite.sprite = new CGRImage(dx,dy);
                 ct->moveSprite.sprite->SetPalette(pH.palette);
                 ct->moveSprite.sprite->LoadPalImage(tmp, 0,0, 0,0,dx-1,dy-1,dx);
                 switch (ct->moveSprite.type)  {
                    case CTRL_MOVE_SPRITE_ARC:
                       ct->controlData.fValue = ct->arrow.alpha0;
                    break;

                    case CTRL_MOVE_SPRITE_LINE:
                       ct->controlData.fValue = 0;
                    break;

                    case CTRL_MOVE_SPRITE_BOX:
                       ct->controlData.fValue = 0;
                    break;
                 }
              }
              else {
                 ct->fillSprite.sprite = new CGRImage(dx,dy);
                 ct->fillSprite.sprite->SetPalette(pH.palette);
                 ct->fillSprite.sprite->LoadPalImage(tmp, 0,0, 0,0,dx-1,dy-1,dx);
                 ct->controlData.fValue = 0;
              }

              delete [] tmp;

           }
           break;

           case PANEL_CTRL_DIGITS: {
              int size;
              char * tmp;

              fread(&size,4,1,fp);

              tmp = new char[size];
              fread(tmp,size,1,fp);

              ct->digits.font = new CFixedColorFont();
              ct->digits.font->ReadFromMemory(tmp);

              delete [] tmp;

              strcpy(ct->controlData.sValue,"000");
           }
           break;
        }
     }
  }

  fclose(fp);

  //read Crosshair
  char crossName[256];

  strcpy(crossName, filename);
  i = strlen(crossName);
  while(crossName[i-1] != '.') i--;
  strcpy(crossName + i, "crh");

  fp = fopen(crossName,"r");
  if (fp != NULL) {
     fclose(fp);
     crosshair.LoadFromBMPFile(crossName, 0, 0);
  }

  drawCrosshair = 1;
  drawPanel     = 1;
}



void CGRPanel::ReloadTextures(const char *filename, GR_HTEXTURE hT)
{ FILE * fp = fopen(filename,"rb");
  SPanelFileHeader pH;
  SPanelFile pF;
  int i,j,cc;

  if (fp == NULL) return;

  fread(&pH, sizeof(SPanelFileHeader), 1, fp);

  if (strncmp(pH.id, PANEL_FILE_ID, 3) != 0 || pH.version > 0) {
     fclose(fp);
     return;
  }


  for(i = 0;i < pH.resolCount;i++) {
     fread(&pF, sizeof(SPanelFile), 1, fp);

     fseek(fp, pF.swPanelSize, SEEK_CUR);

     if (pF.hwPanelSize > 0) {
        fseek(fp, pF.hwPanelSize, SEEK_CUR);

        //-----------------Relocate Pointers & Load Textures-------------
        SHwPanel *hwPanel = panel[i].pHWPanel;

        for(j = 0;j < hwPanel->textureCount;j++) {
           //-------------Load Texture---------------------
           SHwPanelFileTexture * hwPanelFileT;
           int textSize;
           SHwPanelTexture * hwPanelT = hwPanel->texture[j];

           fread(&textSize, sizeof(int), 1, fp);

           if (hwPanelT->hTexture == hT) {

              hwPanelFileT = (SHwPanelFileTexture*) new char [textSize];
              fread(hwPanelFileT, textSize, 1, fp);

              GRLoadTextureToDB(hT, pH.palette, 256, hwPanelFileT->data);
              GRSetTextureLoadFunc( hT, filename, ReloadFunc, (void *) this);

              delete [] hwPanelFileT;
           }
           else
              fseek(fp,textSize,SEEK_CUR);
        }
     }

     //LoadControls
     fread(&cc, 4, 1, fp);

     for(j = 0;j < panel[i].controlCount;j++) {
        SControl *ct = &panel[i].control[j];

        fseek(fp, sizeof(SControl), SEEK_CUR);

        switch (ct->type) {
           case PANEL_CTRL_MOVE_SPRITE:
           case PANEL_CTRL_FILL_SPRITE: {
              int dx,dy;

              fread(&dx,4,1,fp);
              fread(&dy,4,1,fp);

              fseek(fp, dx*dy, SEEK_CUR);

           }
           break;

           case PANEL_CTRL_DIGITS: {
              int size;

              fread(&size,4,1,fp);

              fseek(fp,size,SEEK_CUR);

           }
           break;
        }
     }
  }

  fclose(fp);
}


//===================================================================

CGRPanel::~CGRPanel()
{
  if (resolCount == 0) return;

  if (currentPanel != NULL) {
     if (currentPanel->open) Close();
  }

  for(int i = 0;i < resolCount;i++) {
     GRReleaseViewport(panel[i].panelViewport);

     if (_dL.currDevice != NULL && _dL.currDevice->swHw == GR_HARDWARE &&
         panel[i].pHWPanel != NULL
        )
        for(int j = 0;j < panel[i].pHWPanel->textureCount;j++)
           GRDeleteTextureFromDB(panel[i].pHWPanel->texture[j]->hTexture);

     delete [] panel[i].pSWPanel;
     delete [] panel[i].pHWPanel;


     for(int j = 0;j < panel[i].controlCount;j++) {
        SControl *ct = &panel[i].control[j];

        switch (ct->type) {
           case PANEL_CTRL_MOVE_SPRITE:
              delete ct->moveSprite.sprite;
           break;

           case PANEL_CTRL_FILL_SPRITE:
              delete ct->fillSprite.sprite;
           break;

           case PANEL_CTRL_DIGITS:
              delete ct->digits.font;
           break;
        }
     }
  }

  resolCount = 0;
}

//=====================================================================
void CGRPanel::SetResolution(int width, int height)
{ int i;

  if (resolCount == 0) return;

  if (currentPanel != NULL) {
     if (currentPanel->open) Close();
     currentPanel = NULL;
  }

  for(i = 0;i < resolCount;i++) {
     if (panel[i].width == width && panel[i].height == height) {
        currentPanel = &(panel[i]);
        break;
     }
  }

  return;
}

//=====================================================================
SGRViewport * CGRPanel::Open()
{
  if (resolCount == 0 || currentPanel == NULL) return NULL;

  if (currentPanel->open) return currentPanel->panelViewport;

  if (_dL.currDevice->swHw == GR_HARDWARE && currentPanel->pHWPanel != NULL) {
     SHwPanel * hwPanel = currentPanel->pHWPanel;

     for(int i = 0;i < hwPanel->textureCount;i++) {
        //add UNLOAD flag to texture
        *((unsigned long *)(hwPanel->texture[i]->hTexture)) |= TEXTURE_UNLOAD;
        if (LoadTextureTo3DCard(hwPanel->texture[i]->hTexture) != TRUE)
           return NULL;
     }
  }

  currentPanel->open = 1;

  return currentPanel->panelViewport;
}

//=====================================================================
void CGRPanel::Close()
{
  if (resolCount == 0 || currentPanel == NULL) return;

  if (! currentPanel->open) return;

  if (_dL.currDevice->swHw == GR_HARDWARE && currentPanel->pHWPanel != NULL) {
     SHwPanel * hwPanel = currentPanel->pHWPanel;

     for(int i = 0;i < hwPanel->textureCount;i++) {
        //del flags to texture
        *((unsigned long *)(hwPanel->texture[i]->hTexture)) &= ~TEXTURE_UNLOAD;
        //set counter to 1: this texture will be Unload First
        *(((unsigned long *)(hwPanel->texture[i]->hTexture)) + 1) = 1;
     }
  }

  currentPanel->open = 0;
}

//======================================================================
void CGRPanel::Draw()
{
  if (resolCount == 0) return;
  //if (resolCount == 0 || currentPanel == NULL) return;
  //if (! currentPanel->open) return;

  if (drawPanel && currentPanel != NULL && currentPanel->open) {

  DrawControlsFirst();

  if (_dL.currDevice->swHw == GR_HARDWARE) {
     if (currentPanel->pHWPanel != NULL) {
        SHwPanel * hwPanel = currentPanel->pHWPanel;

        //_d3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
        //_d3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
        _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_NEAREST);
        _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_NEAREST);
        _d3dDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, TRUE);
        //GRZBufferEnable(0);

        for(int i = 0;i < hwPanel->textureCount;i++) {
           SHwPanelTexture * hwPanelT = hwPanel->texture[i];

           LoadTextureTo3DCard(hwPanelT->hTexture);

           for(int j = 0;j < hwPanelT->numAreas;j++) {
              SHwPanelArea * hwPanelA = hwPanelT->area[j];

              _d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                    (LPVOID) hwPanelA->point, hwPanelA->numPoints, D3DDP_DONOTCLIP);

           }
        }

        //_d3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
        //_d3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
        _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_LINEAR);
        _d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_LINEAR);
        _d3dDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
        //GRZBufferEnable(1);
     }
  }
  else {
     if (currentPanel->pSWPanel != NULL) {
        SSwPanel * swPanel = currentPanel->pSWPanel;

        ASM_GRDrawPanel(swPanel->pPanel, swPanel->xStart, swPanel->yStart);
     }
  }

  GREndScene();

  DrawControlsLast();
  }

  //draw Small Panel

  //draw Crosshair

  GREndScene();

  if (drawCrosshair)
     crosshair.DrawSprite(_gr_nScreenOriginX - crosshair.Width()/2,
                          _gr_nScreenOriginY - crosshair.Height()/2
                         );
  //GRStartScene();
}

///////////////////////////Controls/////////////////////////////////////

#define SECTOR_STEP (20.*3.1415926/180.)

void CGRPanel::DrawSector(SControl *ct, float a0, float a1, unsigned long color)
{  int i, casQ;

   casQ = (int)((a1 - a0) / SECTOR_STEP);
   if (casQ == 0) casQ = 1;
   else if (casQ < 0) casQ = -casQ;

   _gr_polygon.dwFullType = GR_POLY_FLAT;
   _gr_polygon.dwAddType = 0;
   _gr_polygon.nVertices = casQ + 2;
   _gr_polygon.dwColor.color = color;
   _gr_polygon.nLights = 0;

   int x0 = ct->indicator.x - _gr_nScreenOriginX,
       y0 = ct->indicator.y - _gr_nScreenOriginY;

   float rNew = ct->indicator.r / cos((a1-a0)/(casQ*2)),
         deltaA = (a0 - a1) / casQ;

   //int iz = (int)(65536. / _gr_fFrontClip);

   for(i = 0;i <= casQ;i++) {
      _gr_vertices[i].any.x  = (int)(rNew*cos(a1)) + x0;
      _gr_vertices[i].any.y  = y0 - (int)(rNew*sin(a1));
      _gr_vertices[i].any.iz = 10;

      a1 += deltaA;
   }

   _gr_vertices[i].any.x  = x0;
   _gr_vertices[i].any.y  = y0;
   _gr_vertices[i].any.iz = 10;

   GRDrawPolygonPCCW();

}

void CGRPanel::DrawControlsFirst()
{
   if (resolCount == 0 || currentPanel == NULL) return;

   for(int i = 0;i < currentPanel->controlCount;i++) {
      SControl * ct = &currentPanel->control[i];

      switch (ct->type) {
         case PANEL_CTRL_INDICATOR: {
            GRZBufferEnable(0);

            //patch clipRect
            _gr_clipRect.left   = -_gr_nScreenOriginX;
            _gr_clipRect.right  = _gr_nScreenWidth - _gr_nScreenOriginX;
            _gr_clipRect.top    = -_gr_nScreenOriginY;
            _gr_clipRect.bottom = _gr_nScreenHeight - _gr_nScreenOriginY;
            GRSetClipRect();
            //draw
            // color0
            DrawSector(ct, ct->indicator.alpha0, ct->indicator.alpha1, ct->indicator.color1);
            //color1
            DrawSector(ct, ct->indicator.alpha0, ct->controlData.fValue, ct->indicator.color0);

            GRZBufferEnable(1);

            //restore clipRect
            _gr_clipRect = GRGetViewport()->clipRect;
            GRSetClipRect();
         }
         break;
      }
   }

}

void CGRPanel::DrawControlsLast()
{
   if (resolCount == 0 || currentPanel == NULL) return;
   if (currentPanel->controlCount == 0) return;

   for(int i = 0;i < currentPanel->controlCount;i++) {
      SControl * ct = &currentPanel->control[i];

      switch (ct->type) {
         case PANEL_CTRL_ARROW: {
            GREnable2D();
            float s = -sin(ct->controlData.fValue),
                  c =  cos(ct->controlData.fValue);

            GRLine((int)(c*ct->arrow.rTrsp) + ct->arrow.x,
                   (int)(s*ct->arrow.rTrsp) + ct->arrow.y,
                   (int)(c*ct->arrow.r) + ct->arrow.x,
                   (int)(s*ct->arrow.r) + ct->arrow.y,
                   ct->arrow.color
                  );
            GRDisable2D();
         }
         break;

         case PANEL_CTRL_MOVE_SPRITE: {
            int x,y;

            GREndScene();

            switch (ct->moveSprite.type)  {
               case CTRL_MOVE_SPRITE_ARC: {
                  float xs,ys;

                  xs = (float)ct->moveSprite.x + ct->moveSprite.r*
                       cos(ct->controlData.fValue);
                  ys = (float)ct->moveSprite.y - ct->moveSprite.r*
                       sin(ct->controlData.fValue);

                  x = (int)xs - ct->moveSprite.sprite->Width()/2;
                  y = (int)ys - ct->moveSprite.sprite->Height()/2;

                  ct->moveSprite.sprite->DrawSprite(x, y);
               }
               break;

               case CTRL_MOVE_SPRITE_LINE: {
                  float xs,ys;

                  xs = (float)ct->moveSprite.x + ct->controlData.fValue *
                       (float)(ct->moveSprite.x1 - ct->moveSprite.x);
                  ys = (float)ct->moveSprite.y + ct->controlData.fValue *
                       (float)(ct->moveSprite.y1 - ct->moveSprite.y);

                  x = (int)xs - ct->moveSprite.sprite->Width()/2;
                  y = (int)ys - ct->moveSprite.sprite->Height()/2;

                  ct->moveSprite.sprite->DrawSprite(x, y);
               }
               break;

               case CTRL_MOVE_SPRITE_BOX: {
                  float xs,ys;

                  xs = (float)ct->moveSprite.u + ct->controlData.fValue *
                       (float)(ct->moveSprite.u1 - ct->moveSprite.u);
                  ys = (float)ct->moveSprite.v + ct->controlData.fValue *
                       (float)(ct->moveSprite.v1 - ct->moveSprite.v);

                  //x = (int)xs - ct->moveSprite.sprite->Width()/2;
                  //y = (int)ys - ct->moveSprite.sprite->Height()/2;

                  ct->moveSprite.sprite->DrawSprite(ct->moveSprite.x, ct->moveSprite.y,
                                                    (int)xs,(int)ys,(int)xs+ct->moveSprite.x1,(int)ys+ct->moveSprite.y1);
               }
               break;
            }


            GRStartScene();
         }
         break;

         case PANEL_CTRL_FILL_SPRITE: {
            int xs, ys;
            int x0, y0, x1, y1;

            GREndScene();

            switch (ct->fillSprite.type) {
               case CTRL_FILL_SPRITE_LR:
                  x0 = y0 = 0;
                  x1 = (int)((float)(ct->fillSprite.sprite->Width() - 1) *
                             ct->controlData.fValue);
                  y1 = ct->fillSprite.sprite->Height() - 1;
                  xs = ct->fillSprite.x;
                  ys = ct->fillSprite.y;
               break;

               case CTRL_FILL_SPRITE_RL:
                  x0 = (int)((float)(ct->fillSprite.sprite->Width() - 1) *
                             (1.0 - ct->controlData.fValue));
                  y0 = 0;
                  x1 = ct->fillSprite.sprite->Width() - 1;
                  y1 = ct->fillSprite.sprite->Height() - 1;
                  xs = ct->fillSprite.x + x0;
                  ys = ct->fillSprite.y;
               break;

               case CTRL_FILL_SPRITE_TB:
                  x0 = y0 = 0;
                  x1 = ct->fillSprite.sprite->Width() - 1;
                  y1 = (int)((float)(ct->fillSprite.sprite->Height() - 1) *
                             ct->controlData.fValue);
                  xs = ct->fillSprite.x;
                  ys = ct->fillSprite.y;
               break;

               case CTRL_FILL_SPRITE_BT:
                  x0 = 0;
                  y0 = (int)((float)(ct->fillSprite.sprite->Height() - 1) *
                             (1.0 - ct->controlData.fValue));
                  x1 = ct->fillSprite.sprite->Width() - 1;
                  y1 = ct->fillSprite.sprite->Height() - 1;
                  xs = ct->fillSprite.x;
                  ys = ct->fillSprite.y + y0;
               break;
            }

            ct->fillSprite.sprite->DrawSprite(xs,ys,x0,y0,x1,y1);

            GRStartScene();
         }
         break;

         case PANEL_CTRL_DIGITS: {
            char tmp[2];

            tmp[1] = 0;
            GREndScene();

            for(int j = 0;j < ct->digits.digitsCount;j++) {
               tmp[0] = ct->controlData.sValue[j];
               ct->digits.font->LUPrintAt(ct->digits.x[j],ct->digits.y[j],tmp);
            }

            GRStartScene();
         }
         break;
      }
   }
}

int  CGRPanel::IsDigitControl(const char *name)
{
   if (resolCount == 0 || currentPanel == NULL) return 0;
   if (currentPanel->controlCount == 0) return 0;

   for(int i = 0;i < currentPanel->controlCount;i++) {
      SControl * ct = &currentPanel->control[i];
      if (strcmp(name,ct->name) == 0)
         return (ct->type == PANEL_CTRL_DIGITS);
   }

   return 0;
}


void CGRPanel::SetControlValue(char *name, unsigned long value)
{  float val;

   if (resolCount == 0 || currentPanel == NULL) return;
   if (currentPanel->controlCount == 0) return;

   for(int i = 0;i < currentPanel->controlCount;i++) {
      SControl * ct = &currentPanel->control[i];
      if (strcmp(name,ct->name) == 0) {
         switch (ct->type) {
            case PANEL_CTRL_INDICATOR:
               val = *((float*)&value);
               val = (ct->indicator.alpha1 - ct->indicator.alpha0) * val +
                      ct->indicator.alpha0;
               ct->controlData.fValue = val;
            break;

            case PANEL_CTRL_ARROW:
               val = *((float*)&value);
               val = (ct->arrow.alpha1 - ct->arrow.alpha0) * val +
                      ct->arrow.alpha0;
               ct->controlData.fValue = val;
            break;

            case PANEL_CTRL_MOVE_SPRITE:
               switch (ct->moveSprite.type)  {
                  case CTRL_MOVE_SPRITE_ARC:
                     val = *((float*)&value);
                     val = (ct->arrow.alpha1 - ct->arrow.alpha0) * val +
                           ct->arrow.alpha0;
                     ct->controlData.fValue = val;
                  break;

                  case CTRL_MOVE_SPRITE_LINE:
                     ct->controlData.fValue = *((float*)&value);
                  break;
               }
            break;

            case PANEL_CTRL_FILL_SPRITE:
               ct->controlData.fValue = *((float*)&value);
            break;

            case PANEL_CTRL_DIGITS: {
               char tmp[5] = "%0_d";

               tmp[2] = (char)(ct->digits.digitsCount + '0');

               sprintf(ct->controlData.sValue, tmp, value);
            }
            break;
         }

         //break; FIXME - TMP!!!!!!!!!!!!!!!!!!!1
      }
   }
}