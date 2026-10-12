#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "menu.h"
#include "image.h"
#include "font.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "kernel/h/context.h"
#include "kernel/h/session.h"
#include "kernel/h/echo.h"
#include "message/menumsg.h"
#include "graph.h"
#include "hardware.h"
#include "h/cachesmoke.h"
#define LAST_H__SCENE
#include "game.h"
#include "zav.h"
#include "suavik.h"
#include "../../../graph/drawd3d.h"
#include "console.h"


static unsigned int stBaseX, stBaseY, stW, stH;
static unsigned int ctBaseX, ctBaseY, ctW, ctH;
static int menuBkColorR, menuBkColorG, menuBkColorB, menuBkColorOpasity;

//================================================================
MenuItem_::MenuItem_(const char *name, KR_ObjectID objNotify, int eventLabel, int pVertSpace, int sideW,
                     int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity){
    m_objNotify  = objNotify;
    m_eventLabel = eventLabel;
    m_pColor     = pColor;
    m_pOpasity   = pOpasity;
    m_abColor    = abColor;
    m_abOpasity  = abOpasity;
    m_afColor    = afColor;
    m_afOpasity  = afOpasity;
    m_pVertSpace = pVertSpace;
    m_pSideW     = sideW;

    strncpy(m_name, name, MAX_MENUITEM_NAME_LEN-1);
    m_name[MAX_MENUITEM_NAME_LEN-1] = 0;
    m_disabled   = 0;
    m_hidden     = 0;
    m_next       = NULL;
    m_prev       = NULL;
    m_down       = NULL;
    m_up         = NULL;
}
//----------------------------------------------
void MenuItem_::SetXY(int x, int y){
    m_baseX  = x;
    m_baseY  = y;
    m_aBaseX = m_baseX+(m_pSideW-m_aSideW);
    m_aBaseY = m_baseY+(m_totalH-m_aH)/2;
}
//----------------------------------------------
void MenuItem_::Draw(int ox, int oy, int active) const{
    int x0, y0, x1, y1;
    int u0, v0, u1, v1;
    int color, opasity;
    void *texture;

    GRStartScene();
    texture = g_menu.m_textute;

    // sides
    color   = active ? m_abColor : m_pColor;
    opasity = active ? m_abOpasity : m_pOpasity;
    x0 = ox+m_baseX;
    y0 = oy+m_baseY;
    x1 = x0+m_pSideW;
    y1 = y0+m_totalH-1;
    u0 = stBaseX;
    v0 = stBaseY;
    u1 = u0+stW;
    v1 = v0+stH;
    DrawAlphaSprite(x0, y0, x1, y1, u0, v0, u1, v1, color, opasity, texture);
    x0 = ox+m_baseX+m_totalW-m_pSideW;
    y0 = oy+m_baseY;
    x1 = x0+m_pSideW;
    y1 = y0+m_totalH;
    u0 = stBaseX+stW;
    v0 = stBaseY;
    u1 = u0-stW;
    v1 = v0+stH;
    DrawAlphaSprite(x0, y0, x1, y1, u0, v0, u1, v1, color, opasity, texture);
    if(active){
        color   = m_afColor;
        opasity = m_afOpasity;
        x0 = ox+m_aBaseX;
        y0 = oy+m_aBaseY;
        x1 = x0+m_aSideW;
        y1 = y0+m_aH;
        u0 = stBaseX;
        v0 = stBaseY;
        u1 = u0+stW;
        v1 = v0+stH;
        DrawAlphaSprite(x0, y0, x1, y1, u0, v0, u1, v1, color, opasity, texture);
        x0 = ox+m_aBaseX+m_aSideW+m_centerW;
        y0 = oy+m_aBaseY;
        x1 = x0+m_aSideW;
        y1 = y0+m_aH;
        u0 = stBaseX+stW;
        v0 = stBaseY;
        u1 = u0-stW;
        v1 = v0+stH;
        DrawAlphaSprite(x0, y0, x1, y1, u0, v0, u1, v1, color, opasity, texture);
    }
    // center
    if(m_centerW > 0){
        color   = active ? m_abColor : m_pColor;
        opasity = active ? m_abOpasity : m_pOpasity;
        x0 = ox+m_baseX+m_pSideW;
        y0 = oy+m_baseY;
        x1 = x0+m_centerW;
        y1 = y0+m_totalH;
        u0 = ctBaseX;
        v0 = ctBaseY;
        u1 = u0+ctW;
        v1 = v0+ctH;
        DrawAlphaSprite(x0, y0, x1, y1, u0, v0, u1, v1, color, opasity, texture);
        if(active){
            color   = m_afColor;
            opasity = m_afOpasity;
            x0 = ox+m_aBaseX+m_aSideW;
            y0 = oy+m_aBaseY;
            x1 = x0+m_centerW;
            y1 = y0+m_aH;
            DrawAlphaSprite(x0, y0, x1, y1, u0, v0, u1, v1, color, opasity, texture);
        }
    }
    GREndScene();
}
//----------------------------------------------
void MenuItem_::DrawAlphaSprite(int x0, int y0, int x1, int y1, int u0, int v0, int u1, int v1,
                                int color, int opasity, void *texture) const{
    
    SGRAlphaSprite as;
    
    as.x0 = x0 - _gr_nScreenOriginX; 
    as.y0 = y0 - _gr_nScreenOriginY; 
    as.x1 = x1 - _gr_nScreenOriginX; 
    as.y1 = y1 - _gr_nScreenOriginY;
    as.u0 = u0; 
    as.v0 = v0; 
    as.u1 = u1; 
    as.v1 = v1;
    as.color    = color;
    as.opacity  = opasity;
    as.iz       = 65535;
    as.hTexture = texture;
    GRDrawAlphaSprite(&as);
    if (GRIsHardware()) D3D_DrawZList();
}
//================================================================
MenuItemText::MenuItemText(const char *name, KR_ObjectID on, int el, int pVertSpace, int sideW,
                 int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                 IFixedFont *pFontI, IFixedFont *aFontI, const char *l0Text, const char *l1Text)
                 :MenuItem_(name, on, el, pVertSpace, sideW, pColor, pOpasity, abColor, abOpasity, afColor, afOpasity){
   s_ASSERT((pFontI != NULL && aFontI != NULL) || (l0Text == NULL && l1Text == NULL), "");

   m_pFontI = pFontI;
   m_aFontI = aFontI;
   if(l0Text != NULL){
       m_pTextH = pFontI->Height();
       m_aTextH = aFontI->Height();
       strncpy(m_text[0], l0Text, MAX_MENUITEM_TEXT_LEN-1);
       strncpy(m_text[1], l1Text, MAX_MENUITEM_TEXT_LEN-1);
       m_text[0][MAX_MENUITEM_TEXT_LEN-1] = 0;
       m_text[1][MAX_MENUITEM_TEXT_LEN-1] = 0;
       SetLocale(0);
   }
}
//----------------------------------------------
void MenuItemText::SetLocale(int l){
     MenuItem_::SetLocale(l);
     if(m_aFontI != NULL){
        m_aTextW = m_aFontI->StringWidth(m_text[m_locale]);
        m_pTextW = m_pFontI->StringWidth(m_text[m_locale]);
     }
}
//----------------------------------------------
void MenuItemText::Draw(int ox, int oy, int active) const{
    MenuItem_::Draw(ox, oy, active);
    if(active) m_aFontI->LUPrintClipAt(ox + m_baseX+m_aTextBaseX, oy + m_baseY+m_aTextBaseY, m_text[m_locale]);
    else       m_pFontI->LUPrintClipAt(ox + m_baseX+m_pTextBaseX, oy + m_baseY+m_pTextBaseY, m_text[m_locale]);
}
//----------------------------------------------
void MenuItemText::GraphEvent(){ 
    MenuItem_::GraphEvent();
    if(m_aFontI != NULL) m_aFontI->RecreateFont();
    if(m_pFontI != NULL) m_pFontI->RecreateFont();
}
//----------------------------------------------
void MenuItemText::CalcSelfCoords(int maxItemLen){
    // here m_baseX, m_baseY, m_pSideW has been setted

    if(maxItemLen != -1) m_maxItemLen = maxItemLen;

    m_centerW = m_centerSegment ? m_maxItemLen : 0;
    m_totalH  = m_pVertSpace*2+Max(m_aTextH, m_pTextH);
    m_totalW  = m_pSideW*2+m_centerW;
    m_aSideW  = m_pSideW-5; // ????
    m_aH      = Max(m_aTextH, m_pTextH) + (m_totalH-Max(m_aTextH, m_pTextH))/3;
    
    m_aTextBaseX = (m_totalW-m_aTextW)/2;
    m_aTextBaseY = (m_totalH-m_aTextH)/2;
    m_pTextBaseX = (m_totalW-m_pTextW)/2;
    m_pTextBaseY = (m_totalH-m_pTextH)/2;
}
//----------------------------------------------
int MenuItemText::GetItemLen() const{
    return(Max(m_aTextW, m_pTextW));
}
//================================================================
void MenuItemInput::CalcSelfCoords(int maxItemLen){
    MenuItemText::CalcSelfCoords(maxItemLen);
}
//----------------------------------------------
int MenuItemInput::GetItemLen() const{
    return(80);
}
//----------------------------------------------
int MenuItemInput::PostKey(int ctrlEvent, int code, int release, int repeat){
    int strLen = strlen(m_text[m_locale]);
    const char cursor[] = "+";
    
    if(repeat) return(FALSE);
    if(ctrlEvent == MENU_ENTER && !release){
        if(!m_inputActive){
            m_inputActive = TRUE;
            strcpy(m_oldText, m_text[m_locale]);
            strcat(m_text[m_locale], cursor);
        }
        else{
            m_inputActive = FALSE;
            m_text[m_locale][strLen-1] = 0;
            m_menu->ForceChieldNotify(this);
        }
        return(TRUE);
    }
    if(!m_inputActive) return(FALSE);
    if(release) return(TRUE);
    if(ctrlEvent == MENU_TOGGLE && !repeat) { 
        m_inputActive = FALSE; 
        strcpy(m_text[m_locale], m_oldText);
        SetLocale();
        CalcSelfCoords(GetItemLen());
        return(TRUE); 
    }
    if(ctrlEvent == CTRL_CHAR){
        if(code == g_hardware.SearchCode("Bs") && strLen > 1){
            m_text[m_locale][strLen-2] = 0;
            strcat(m_text[m_locale], cursor);
        }
        if(g_GameConsole.IsLetter(code) && strLen < MAX_MENUITEM_TEXT_LEN-1){
            m_text[m_locale][strLen-1] = (char)code;
            strcat(m_text[m_locale], cursor);
        }
        SetLocale();
        CalcSelfCoords(GetItemLen());
    }
    return(TRUE);
}
//================================================================
MenuControlSetup::MenuControlSetup(const char *name, KR_ObjectID on, int el, int pVertSpace, int sideW,
                 int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                 IFixedFont *pFontI, IFixedFont *aFontI, const char *l0Text, const char *l1Text) 
                 : MenuItemText(name, on, el, pVertSpace, sideW, pColor, pOpasity, abColor, abOpasity, afColor, afOpasity, pFontI, aFontI, l0Text, l1Text){
    int i;
    
    m_grLen        = 300;
    m_grH          = 240;

    m_setupActive  = FALSE;
    m_addCtrlMode  = FALSE;
    m_linesNum     = 0;
    m_curBase      = 0;
    m_curItem      = 0;
    m_vertSpacing  = 2;
    m_maxNameLen   = 0;
    m_normalCtrlColor  = GRTransparentColor(0, 255, 0);
    m_crossedCtrlColor = GRTransparentColor(255, 0, 0);

    for(i = 0;i < MAX_CONTROLS_LINES; ++i){
        m_line[i].name[0]          = 0;
        m_line[i].buttonsBinded[0] = 0;
        m_line[i].color            = m_normalCtrlColor;
        m_line[i].action           = 0;
        m_line[i].buttonsBindedNum = 0;
    }
}
//----------------------------------------------
MenuControlSetup::~MenuControlSetup(){
    //SaveSetup("setup.txt");
}
//----------------------------------------------
int MenuControlSetup::receiveEvent(KR_Event &event){
    char name[MAX_CONTROL_LINE_NAME_LEN], ctrlName[MAX_ACT_NAME_LEN];
    switch(event.label){
        case EV_MENU_SETUP_ADD_LINE: 
            event.data.open(EDO_READ)
                         .getStr(name, MAX_CONTROL_LINE_NAME_LEN)
                         .getStr(ctrlName, MAX_ACT_NAME_LEN)
                      .close();
            AddControlLine(name, ctrlName);
            break;
        default: return(0);
    }
    return(1);
}
//----------------------------------------------
void MenuControlSetup::Draw(int ox, int oy, int active) const{
    int i, X, Y;
    
    if(!m_setupActive){ MenuItemText::Draw(ox, oy, active); return; }

    ox = (_gr_nScreenWidth  - m_grLen)/2;
    oy = (_gr_nScreenHeight - m_grH)/2;
    for(i = 0; i < Min(m_maxScreenLinesNum, m_linesNum); ++i){
        X   = ox+m_scrLinesBaseX;
        Y   = oy+m_scrLinesBaseY + i*(m_pTextH+m_vertSpacing);
        m_pFontI->LUPrintClipAt(X, Y, m_line[i+m_curBase].name);
        if(m_line[i+m_curBase].buttonsBindedNum > 0)
            m_pFontI->LUPrintClipAt(ox+m_scrBindesBaseX, Y, m_line[i+m_curBase].buttonsBinded);
    }
    DrawPtr(X, oy+m_scrLinesBaseY + (m_curItem-m_curBase)*(m_pTextH+m_vertSpacing));
}
//----------------------------------------------
void MenuControlSetup::DrawPtr(int x, int y) const{
    char ptr[] = "\x10";
    int len = m_pFontI->StringWidth(ptr);
    
    m_pFontI->LUPrintClipAt(x-len-2, y, ptr);
}
//----------------------------------------------
void MenuControlSetup::OnActivate(){
    m_keyEnter = g_hardware.SearchCode("Enter");
    FillLines();
}
//----------------------------------------------
void MenuControlSetup::OnDeactivate(){
    g_hardware.Link();
}
//----------------------------------------------
void MenuControlSetup::GraphEvent(){
    MenuItem_::GraphEvent();

    m_setupX = m_setupY = 0;
    
    m_maxScreenLinesNum = m_grH/(m_pTextH + m_vertSpacing);
    m_scrLinesBaseX     = m_setupX;
    m_scrLinesBaseY     = m_setupY;
    m_scrBindesBaseX    = m_setupX+m_maxNameLen+10;
}
//----------------------------------------------
void MenuControlSetup::FillLines(){
    int i, j, action;
    const int *code; 

    for(i = 0; i < m_linesNum; ++i){
        action = m_line[i].action;
        code   = g_hardware.GetCodesByAction(action);
        m_line[i].buttonsBinded[0] = 0;
        for(j = 0; j < MAX_CODES_ON_ACTION && code[j] > 0; ++j){
            m_line[i].code[j] = code[j];
            strcat(m_line[i].buttonsBinded, g_hardware.SearchName(code[j]));
            if(j < MAX_CODES_ON_ACTION-1 && code[j+1] > 0)
                strcat(m_line[i].buttonsBinded, "  or  ");
        }
        m_line[i].buttonsBindedNum = j;
    }
}
//----------------------------------------------
int MenuControlSetup::PostKey(int ctrlEvent, int code, int release, int repeat){
    int i;

    if(!m_setupActive && ctrlEvent == MENU_ENTER){
        m_setupActive     = TRUE;
        m_baseBackup      = m_menu->m_curBase;
        m_prevBackup      = m_prev;
        m_nextBackup      = m_next;
        m_menu->m_curBase = m_next = m_prev = this;
        m_menu->DisablePointer();

        FillLines();
        return(1);
    }
    if(!m_setupActive)return(0); 
    if(release || repeat)return(1);
    
    if(ctrlEvent == MENU_TOGGLE){
        if(m_addCtrlMode) m_addCtrlMode = FALSE;
        else{
            m_setupActive     = FALSE;
            m_menu->m_curBase = m_baseBackup;
            m_prev            = m_prevBackup;
            m_next            = m_nextBackup;
            m_menu->EnablePointer();
        }
        return(1);
    }
    if(m_addCtrlMode){
        if(code == m_keyEnter && ctrlEvent != MENU_ENTER) return(1);
        m_addCtrlMode = FALSE;
        if(m_line[m_curItem].buttonsBindedNum < MAX_CODES_ON_ACTION)
             m_line[m_curItem].code[m_line[m_curItem].buttonsBindedNum++] = code;
        else m_line[m_curItem].code[m_line[m_curItem].buttonsBindedNum-1] = code;

        g_hardware.SetControl(m_line[m_curItem].action, code);
        /*
        int crossedAction[10], n, pos;
        int color;
        
        n     = g_hardware.SearchActionsOnCode(code, crossedAction);
        color = (n > 1) ? m_crossedCtrlColor : m_normalCtrlColor;
        for(i = 0; i < n; ++i){
            pos = SearchLineByAction(crossedAction[i]);
            s_ASSERT(pos != -1, "");
            m_line[pos].color = color;
        }*/
        echo("code %i", code);
        m_line[m_curItem].buttonsBinded[0] = 0;
        for(i = 0; i < m_line[m_curItem].buttonsBindedNum; ++i){
            const char *s = g_hardware.SearchName(m_line[m_curItem].code[i]);
            if(i > 0 && i < m_line[m_curItem].buttonsBindedNum)
                strcat(m_line[m_curItem].buttonsBinded, "  or  ");
            strcat(m_line[m_curItem].buttonsBinded, s);
        } 
        return(1);
    }
    if(ctrlEvent == MENU_DEL){
        m_line[m_curItem].buttonsBinded[0] = 0;
        for(i = 0; i < m_line[m_curItem].buttonsBindedNum; ++i)
            g_hardware.RemoveCode(m_line[m_curItem].action, m_line[m_curItem].code[i]);

        m_line[m_curItem].buttonsBindedNum = 0;
        m_line[m_curItem].color            = m_normalCtrlColor;
    }
    if(ctrlEvent == MENU_UP)
        if(m_curItem > 0){
            m_curItem--;
            while(m_curBase > m_curItem) m_curBase--;
        }
        else{
            m_curItem = m_linesNum-1;
            m_curBase = Max(0, m_curItem-m_maxScreenLinesNum+1);
        }
    if(ctrlEvent == MENU_DOWN)
        if(m_curItem < m_linesNum-1){
            m_curItem++;
            while(m_curItem-m_curBase > m_maxScreenLinesNum-1) m_curBase++;
        }
        else{
            m_curItem = 0;
            m_curBase = 0;
        }
    if(ctrlEvent == MENU_ENTER){
        s_ASSERT(m_addCtrlMode == 0, "");
        m_addCtrlMode = TRUE;
    }
    return(1);
}
//----------------------------------------------
void MenuControlSetup::AddControlLine(const char *name, const char *ctrlName){
    s_ASSERT(name != NULL && ctrlName != NULL, "");
    s_ASSERT(m_linesNum < MAX_CONTROLS_LINES-1, "");
    
    strncpy(m_line[m_linesNum].name, name, MAX_CONTROL_LINE_NAME_LEN);
    m_line[m_linesNum].name[MAX_CONTROL_LINE_NAME_LEN-1] = 0;
    m_line[m_linesNum].action           = g_hardware.SearchAction(ctrlName);
    s_ASSERT(m_line[m_linesNum].action != -1, "");
    m_line[m_linesNum].color            = m_normalCtrlColor;
    m_line[m_linesNum].buttonsBindedNum = 0;
    m_line[m_linesNum].buttonsBinded[0] = 0;
    m_linesNum++;

    if(m_maxNameLen < m_pFontI->StringWidth(name)) m_maxNameLen = m_pFontI->StringWidth(name);
}
//----------------------------------------------
int MenuControlSetup::SaveSetup(const char *fName) const{
    FILE *fh = fopen(fName, "wt");
    int i, j;
    const char *key, *ctrl;
    
    if(fh == NULL) return(0);

    for(i = 0; i < m_linesNum; ++i){
        ctrl = g_hardware.SearchActioName(m_line[i].action);
        for(j = 0; j < m_line[i].buttonsBindedNum; ++j){
            key = g_hardware.SearchName(m_line[i].code[j]);
            fprintf(fh, "s_SetCTRL(\"%s\", \"%s\");\n", ctrl, key);
        }
    }
    fclose(fh);
    return(1);
}
static const char scrollStr[] = "\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB"; // 10 times
//================================================================
MenuItemScroll::MenuItemScroll(const char *name, KR_ObjectID on, int el, int pVertSpace, int sideW,
                 int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                 IFixedFont *pFontI, IFixedFont *aFontI, const char *attrName, const char *l0Text, const char *l1Text, 
                 double min, double max, double step, double def)
                 :MenuItemText(name, on, el, pVertSpace, sideW, pColor, pOpasity, abColor, abOpasity, afColor, afOpasity, pFontI, aFontI, l0Text, l1Text){
    s_ASSERT(min < max, "");
    s_ASSERT(step > 0 &&  step <= max-min, "");
    
    m_min   = min;
    m_max   = max;
    m_def   = def;
    m_step  = step;
    m_delta = max-min;
    m_lineW = pFontI->StringWidth(scrollStr);
    m_leftCH  = '\x10';
    m_rightCH = '\x11';
    m_pointer = '\xDB';
    m_text2line    = 1;
    m_line2counter = 4;
    strncpy(m_attrName, attrName, MAX_ATTR_NAMELEN-1);
    m_attrName[MAX_ATTR_NAMELEN-1] = 0;
    g_levelAttr.set_double(m_attrName, def);
}
//----------------------------------------------
void MenuItemScroll::CalcSelfCoords(int maxItemLen){
    int wp = m_pTextW, wa = m_aTextW;

    if(maxItemLen != -1) m_pTextW = m_aTextW = maxItemLen;
    MenuItemText::CalcSelfCoords(maxItemLen);
    m_pTextW = wp;
    m_aTextW = wa;

    m_sBaseX = m_baseX + Max(m_pTextBaseX, m_aTextBaseX)+Max(m_pTextW, m_aTextW);

    m_sBaseY    = m_baseY + (m_totalH-m_pFontI->Height())/2;
    m_cBaseX    = m_sBaseX+m_lineW+m_pFontI->CharWidth(m_rightCH);
    m_cBaseY    = m_sBaseY;
}
//----------------------------------------------
int MenuItemScroll::GetItemLen() const{
    int textW, selfW;

    textW = MenuItemText::GetItemLen();
    selfW = m_text2line+m_pFontI->CharWidth(m_leftCH)+m_lineW+m_pFontI->CharWidth(m_rightCH)+m_line2counter;//+m_pFontI->StringWidth("000000");

    return(textW + selfW);
}
//----------------------------------------------
void MenuItemScroll::GraphEvent(){
    MenuItem_::GraphEvent();
}
//----------------------------------------------
int MenuItemScroll::PostKey(int ctrlEvent, int , int release, int){
	double val;

    if(release) return(0);
    val = MinMax(g_levelAttr.get_double(m_attrName), m_min, m_max);

    switch(ctrlEvent){
        case MENU_LEFT:
            if(val > m_min){ val -= m_step; val = Max(m_min, val);}
            else return(0);
            break;
        case MENU_RIGHT:
            if(val < m_max){ val += m_step; val = Min(m_max, val);}
            else return(0);
            break;
        default: return(0);
    }
    g_levelAttr.set_double(m_attrName, val);
    m_menu->ForceChieldNotify(this);
    return(0);
}
//----------------------------------------------    
void MenuItemScroll::Draw(int ox, int oy, int active) const{
    char str[20];
    int px;
    double val = MinMax(g_levelAttr.get_double(m_attrName), m_min, m_max);
    
    str[1] = 0;
    MenuItemText::Draw(ox, oy, active);
    
    str[0] = m_leftCH; 
    m_pFontI->LUPrintClipAt(ox + m_sBaseX, oy + m_sBaseY, str);
    px = (val-m_min)*strlen(scrollStr)/m_delta;
    strcpy(str, scrollStr);
    str[px] = 0;
    m_pFontI->LUPrintClipAt(ox + m_sBaseX+m_pFontI->CharWidth(m_leftCH)+m_text2line, oy + m_sBaseY, str);
    str[0] = m_rightCH; 
    str[1] = 0;
    m_pFontI->LUPrintClipAt(ox + m_sBaseX+m_pFontI->CharWidth(m_leftCH)+m_lineW, oy + m_sBaseY, str);
}
//================================================================
MenuItemList::MenuItemList(const char *name, KR_ObjectID on, int el, int pVertSpace, int sideW,
                 int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                 IFixedFont *pFontI, IFixedFont *aFontI, const char *attrName, const char *l0Text, const char *l1Text, int immediate)
                 :MenuItemText(name, on, el, pVertSpace, sideW, pColor, pOpasity, abColor, abOpasity, afColor, afOpasity, pFontI, aFontI, l0Text, l1Text){
    m_itemsNum = 0;
    m_maxItemW = 0;
    m_imediate = immediate;
    strncpy(m_attrName, attrName, MAX_ATTR_NAMELEN-1);
    m_attrName[MAX_ATTR_NAMELEN-1] = 0;

    for(int i = 0; i < MAX_LIST_ITEMS; ++i) m_item[i][0] = 0;
}
//----------------------------------------------    
void MenuItemList::AddItem(const char *str){
    int w;

    s_ASSERT(str != NULL, "");

    if(m_itemsNum == MAX_LIST_ITEMS-1){
        echo("MenuItemList:: To many items");
        return;
    }
    w = Max(m_pFontI->StringWidth(str), m_aFontI->StringWidth(str));
    if(m_maxItemW < w) m_maxItemW = w;

    strncpy(m_item[m_itemsNum], str, MAX_MENUITEM_LIST_LEN-1);
    m_item[m_itemsNum][MAX_MENUITEM_LIST_LEN-1] = 0;
    m_itemsNum++;
}
//----------------------------------------------    
void MenuItemList::CalcSelfCoords(int maxItemLen){
    int wp = m_pTextW, wa = m_aTextW;

    if(maxItemLen != -1)m_pTextW = m_aTextW = maxItemLen;
    MenuItemText::CalcSelfCoords(maxItemLen);
    m_pTextW = wp;
    m_aTextW = wa;
    
    m_listBaseX = m_baseX + Max(m_pTextBaseX, m_aTextBaseX)+Max(m_pTextW, m_aTextW)+8;
    m_listBaseY = m_baseY+m_pTextBaseY;
}
//----------------------------------------------    
int MenuItemList::GetItemLen() const{
    int textW = MenuItemText::GetItemLen();

    return(textW + 8 + m_maxItemW);
}
//----------------------------------------------    
void MenuItemList::GraphEvent(){
    MenuItem_::GraphEvent();
}
//----------------------------------------------    
int MenuItemList::PostKey(int ctrlEvent, int , int release, int ){
	int curItem;

    if(release) return(0);
    curItem = MinMax((int)g_levelAttr.get_int(m_attrName), (int)0, (int)(m_itemsNum-1));

    switch(ctrlEvent){
        case MENU_LEFT:
            if(curItem > 0){ curItem--; curItem = Max(0, curItem);}
            else return(0);
            break;
        case MENU_RIGHT:
            if(curItem < m_itemsNum-1){ curItem++; curItem = Min(m_itemsNum-1, curItem);}
            else return(0);
            break;
        default: return(0);
    }
    g_levelAttr.set_int(m_attrName, (double)curItem);
    if(m_imediate) m_menu->ForceChieldNotify(this);
    return(0);
}
//----------------------------------------------    
void MenuItemList::Draw(int ox, int oy, int active) const{
    int curItem = MinMax((int)g_levelAttr.get_int(m_attrName), (int)0, (int)(m_itemsNum-1));

    MenuItemText::Draw(ox, oy, active);
    
    if(active) m_aFontI->LUPrintClipAt(ox + m_listBaseX, oy + m_listBaseY, m_item[curItem]);
    else       m_pFontI->LUPrintClipAt(ox + m_listBaseX, oy + m_listBaseY, m_item[curItem]);
}


//================================================================
void Menu::Init(const char *cfgFileName){
    CConfigFile cfg(cfgFileName);
    
    m_viewPort = NULL;
    
    stBaseX = cfg.GetInt("Sprite", "stBaseX", 128);
    stBaseY = cfg.GetInt("Sprite", "stBaseY", 1);
    stW     = cfg.GetInt("Sprite", "stW", 121);
    stH     = cfg.GetInt("Sprite", "stH", 27);
    ctBaseX = cfg.GetInt("Sprite", "ctBaseX", stBaseX+stW);
    ctBaseY = cfg.GetInt("Sprite", "ctBaseY", stBaseY);
    ctW     = cfg.GetInt("Sprite", "ctW", 6);
    ctH     = cfg.GetInt("Sprite", "ctH", stH);

    stBaseX <<= 16;
    stBaseY <<= 16;
    stW     <<= 16;
    stH     <<= 16;
    ctBaseX <<= 16;
    ctBaseY <<= 16;
    ctW     <<= 16;
    ctH     <<= 16;

    menuBkColorR = cfg.GetInt("Background", "R", 100);
    menuBkColorG = cfg.GetInt("Background", "G", 100);
    menuBkColorB = cfg.GetInt("Background", "B", 100);
    menuBkColorOpasity = cfg.GetInt("Background", "Opasity", 128);

    SetRootAsCurrent();
    m_bkColor = GRTransparentColor(menuBkColorR, menuBkColorG, menuBkColorB);
    m_textute = g_loadSmoke("corona.spr", NULL);
    OnChangeRes();
}
//----------------------------------------------
void Menu::SetRootAsCurrent(){
    m_curBase = m_curItem = m_items->m_down;
}
//----------------------------------------------
void Menu::DestroyItems(MenuItem_ *root){
   MenuItem_ *ptr, *prev;

   s_ASSERT(root != NULL, "");

   ptr = root;
   root->m_prev->m_next = NULL;  
   do{
      if(ptr->m_down) DestroyItems(ptr->m_down);
      prev = ptr;
      ptr = ptr->m_next;
      delete prev;
	  m_totalItemsNum--;
   }while(ptr != NULL);
}
//----------------------------------------------
void Menu::OnChangeRes(){
    SRect2 clipRect;
    
    clipRect.top    = 0;
    clipRect.bottom = _gr_nScreenHeight;
    clipRect.left   = 0;
    clipRect.right  = _gr_nScreenWidth;
    if(m_viewPort != NULL) GRReleaseViewport(m_viewPort);
    m_viewPort      = GRCreateViewport(_gr_nScreenWidth/2, _gr_nScreenHeight/2, clipRect);
    GraphEvent(m_items);
    CreateMenuCoords(m_items->m_down, 0);
}
//----------------------------------------------
void Menu::CreateMenuCoords(MenuItem_ *root, int level){
   MenuItem_ *ptr;
   int maxItemLen, screenW, screenH, vSpace;
   int x, y;

   s_ASSERT(root != NULL, "");
   s_ASSERT(level >= 0 && level < 20, "");

   maxItemLen = 0; ptr = root;
   do{
       if(maxItemLen < ptr->GetItemLen()) maxItemLen = ptr->GetItemLen();
       ptr->m_centerSegment = (level == 0) ? 0 : 1;
       ptr = ptr->m_next;
   }while(ptr != root);
   
   ptr     = root; 
   screenH = 0;
   vSpace  = (level == 0) ? 10 : 5; // fixme
   do{
       ptr->CalcSelfCoords(maxItemLen);
       if(!ptr->IsHidden()) screenW = ptr->GetGrW();
       screenH += ptr->IsHidden() ? 0 : ptr->GetGrH()+vSpace;
       ptr = ptr->m_next;
   }while(ptr != root);

   ptr = root; 
   x   = (_gr_nScreenWidth  - screenW)/2;
   y   = (_gr_nScreenHeight - screenH)/2;
   do{
       ptr->SetXY(x, y);
       if(!ptr->IsHidden()) y += ptr->GetGrH()+vSpace;
       ptr->CalcSelfCoords(maxItemLen);
       ptr = ptr->m_next;
   }while(ptr != root);
   
   ptr = root; 
   do{
       if(ptr->m_down != NULL) CreateMenuCoords(ptr->m_down, level+1);
       ptr = ptr->m_next;
   }while(ptr != root);
}
//----------------------------------------------
void Menu::GraphEvent(MenuItem_ *root){
   MenuItem_ *ptr;

   s_ASSERT(root != NULL, "");

   ptr = root;
   do{
      ptr->GraphEvent();
      if(ptr->m_down) GraphEvent(ptr->m_down);
      ptr = ptr->m_next;
   }while(ptr != root);
}
//----------------------------------------------
void Menu::addNotify(){
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
    m_items = new MenuItemText("", KR_ObjectID::NUL(), 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                               NULL, NULL, NULL, NULL);
    m_curItem = m_curBase = m_items;
    m_items->m_next   = m_items->m_prev = m_items;
    m_items->m_hidden = 1;
    m_totalItemsNum   = 1;
}
//----------------------------------------------
void Menu::removeNotify(){
	KR_Event event;

    event.timeStamp   = Session::m_moment;
	event.source	  = getObjectID();
	event.destination = g_hardware.getObjectID();

	if ( event.destination.isNUL() )
	 return;

	event.label		  = CTRL_UNSUBSCRIBE;
	event.data.open(EDO_WRITE)
				.putObjectID(getObjectID())
			  .close();
	getContext()->sendEventNow(event);
    DestroyItems(m_items);
	s_ASSERT(m_totalItemsNum == 0, "");
}
//----------------------------------------------
void Menu::SetLocale(MenuItem_ *root, int locale){
    MenuItem_ *ptr;

    s_ASSERT(root != NULL, "");
    s_ASSERT(locale >= 0 && locale < MAX_LOCALES_NUM, "");

    ptr = root;
    do{
        ptr->SetLocale(locale);
        if(ptr->m_down) SetLocale(ptr->m_down, locale);
        ptr = ptr->m_next;
    }while(ptr != root);
}
//----------------------------------------------
int Menu::receiveEvent(KR_Event &event){
	int code;
	int ctrlEvent = 0;
	double timeStamp, down;
	int repeat;

	switch( event.label ){
		case KR_WAKE_UP: break;
		case EV_MENU_EXIT:
			if(g_menu.IsActive()) g_menu.Deactivate();
            SUA_DeinitEverything();
			ZAV_Deinit();
			ZAV_PrintOverallInfo();
			exit(0);
            break;
        case EV_MENU_SET_LOCALE:
			int locale;
            MenuItem_ *ptr;

            ptr = SearchMenuItem("/Lang");
            if(ptr == NULL) break;
            locale = ptr->GetDouble();
            SetLocale(m_items->m_down, locale);
            break;
        case CTRL_CHAR: 
			event.data.open(EDO_READ)
						.getInt(code)
					  .close();
            m_curItem->PostKey(CTRL_CHAR, code, 0, 0); 
            break;
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
            if(m_active) if(m_curItem->PostKey(ctrlEvent, code, down ? 0 : 1, repeat)) break;
            if(down == 0 || repeat) break;
			switch(ctrlEvent){
                case EXIT:
        			SUA_DeinitEverything();
		        	ZAV_Deinit();
			        ZAV_PrintOverallInfo();
			        exit(0); 
                    break;
                case MENU_TOGGLE:
                    if(!m_active){ 
                        Activate();
                    }
					else{
                        if(m_curItem->m_up == m_items)
                            Deactivate();
						else{
							s_ASSERT(m_curItem->m_up != NULL, "");
							m_curItem->OnDeactivate();
							m_curItem = m_curItem->m_up;
							m_curBase = m_curItem;
							m_curItem->OnActivate();
						}
					}
                    break;
                case MENU_UP:
                    if(!m_active) break;
                    m_curItem->OnDeactivate();
                    do{ m_curItem = m_curItem->m_prev; }while(m_curItem->IsHidden() || m_curItem->IsDisabled());
                    m_curItem->OnActivate();
                    break;
                case MENU_DOWN:
                    if(!m_active) break;
                    m_curItem->OnDeactivate();
                    do{ m_curItem = m_curItem->m_next; }while(m_curItem->IsHidden() || m_curItem->IsDisabled());
                    m_curItem->OnActivate();
                    break;
                case MENU_ENTER:
                    if(!m_active) break;
                    m_curItem->OnChoose();
                    if(m_curItem->m_objNotify != KR_ObjectID::NUL()) ChieldNotify(m_curItem);
                    if(m_curItem->m_down){
                        m_curItem->OnDeactivate();
		                m_curItem = m_curItem->m_down;
                        m_curBase = m_curItem;
                        m_curItem->OnActivate();
                    }
				default: break;
			}
			break;
		default: return(0);
    }
 	return(1);
}
//----------------------------------------------
void Menu::OnTime(double time){
    MenuItem_ *tmp = m_curBase;

    do{
       tmp->OnTime(time);
       tmp = tmp->m_next;
    }while(tmp != m_curBase);
}
//----------------------------------------------
void Menu::Activate(){
	SetRootAsCurrent();
    g_hardware.GetExclusiveMode(getObjectID());
    m_oldViewPort = GRGetViewport();
    m_curItem->OnActivate();
	m_active = TRUE;
    m_pointerEnabled = TRUE;
}
//----------------------------------------------
void Menu::Deactivate(){
    g_hardware.ReleaseExclusiveMode();
    m_curItem->OnDeactivate();
	m_active = FALSE;
}
//----------------------------------------------
void Menu::ChieldNotify(MenuItem_ *item){
	KR_Event event;

    s_ASSERT(item != NULL, "");
    if(item->m_objNotify == KR_ObjectID::NUL()) return;

    event.source	  = getObjectID();
	event.destination = item->m_objNotify;
	event.label       = item->m_eventLabel;
	event.timeStamp	  = Session::m_moment;
    event.data.open(EDO_WRITE)
                  .putInt((int)item)
              .close();
	getContext()->sendEventNow(event);
}
//----------------------------------------------
int Menu::AddMenuItem(MenuItem_ *item, const char *path){
    MenuItem_ *curItem, *last, *first;

	curItem = SearchMenuItem(path);
    if(curItem == NULL) return(0);

    item->m_up   = curItem;
    item->m_menu = this;
    if(curItem->m_down == NULL){
        item->m_prev    = item;
        item->m_next    = item;
        curItem->m_down = item;
    }
    else{
        curItem = curItem->m_down;
   	    last  = curItem->m_prev; // Take last item (list is cycled) :)))
   	    first = curItem;
        item->m_next  = first;
   	    item->m_prev  = last;
	    last->m_next  = item;
   		first->m_prev = item;
    }
    m_totalItemsNum++;

    return(1);
}
//----------------------------------------------
MenuItem_ *Menu::SearchMenuItem(const char *path) const{
    MenuItem_ *curItem, *first;
    const char *slash;
    char name[MAX_MENUITEM_NAME_LEN];
    int len;

    curItem = m_items;
    if(path[0] != '/')return(NULL);

    while(*path != 0){
        path++;
        slash = strchr(path, '/');
        len = (slash == NULL) ? strlen(path) : (int)(slash-path);
        if(len == 0) break;
        strncpy(name, path, len);
        curItem = curItem->m_down;
        name[len] = 0;
        path += len;
        first = curItem;
        do{
            if(!strcmp(name, curItem->m_name)) break;
            if(curItem == curItem->m_next) return(NULL);
            curItem = curItem->m_next;
        }while(curItem->m_next != first);
        if(strcmp(name, curItem->m_name)) return(NULL);
    }
    return(curItem);
}
//----------------------------------------------
void Menu::DrawBkGround(){
    int x, y;
    
    GRStartScene();

    x = _gr_nScreenOriginX;
    y = _gr_nScreenOriginY;
    _gr_polygon.dwFullType = GR_POLY_TRANSPARENT;
    _gr_polygon.dwAddType  = 0;
    _gr_polygon.nVertices  = 4;
    _gr_polygon.dwOpacity  = menuBkColorOpasity;
    _gr_polygon.nLights    = 0;
    _gr_polygon.dwColor.color = m_bkColor;
    _gr_vertices[0].any.x = -x;
    _gr_vertices[0].any.y = -y;
    _gr_vertices[0].any.iz = 1;
    _gr_vertices[1].any.x = -x;
    _gr_vertices[1].any.y = y;
    _gr_vertices[1].any.iz = 1;
    _gr_vertices[2].any.x = x;
    _gr_vertices[2].any.y = y;
    _gr_vertices[2].any.iz = 1;
    _gr_vertices[3].any.x = x;
    _gr_vertices[3].any.y = -y;
    _gr_vertices[3].any.iz = 1;

    GRDrawPolygonPCCW();
}
//----------------------------------------------
void Menu::Draw(){
    MenuItem_ *tmp;

    GRZBufferEnable(FALSE);
    GRSetViewport(m_viewPort);
    DrawBkGround();
    tmp = m_curBase;

    do{
       if(!tmp->m_hidden) 
           if(m_pointerEnabled && tmp == m_curItem) tmp->Draw(m_baseX, m_baseY, TRUE);
           else                                     tmp->Draw(m_baseX, m_baseY, FALSE);
       tmp = tmp->m_next;
    }while(tmp != m_curBase);
    GRSetViewport(m_oldViewPort);
    GRZBufferEnable(TRUE);
}
//================================================================


//================================================================
int AddMenuText(SimulationContext &context, const char *path, const char *name,
                KR_ObjectID on, int el, int pVertSpace, int sideW,
                int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                const char *pFontName, const char *aFontName, const char *l0Text, const char *l1Text){
    MenuItemText *item;
    IFixedFont *pFontI, *aFontI;
    KR_ObjectID pFontID, aFontID;

    if(context.isExist(pFontName)){
        pFontID = context.searchObject(pFontName);
        pFontI  = (IFixedFont *)context.queryInterface(pFontID, IFixedFontIID);
    }
    else pFontI = NULL;
    if(context.isExist(aFontName)){
        aFontID = context.searchObject(aFontName);
        aFontI  = (IFixedFont *)context.queryInterface(aFontID, IFixedFontIID);
    }
    else aFontI = NULL;
    
    item = new MenuItemText(name, on, el, pVertSpace, sideW, pColor, pOpasity, abColor, abOpasity, afColor, afOpasity, pFontI, aFontI, l0Text, l1Text);
    
    return(g_menu.AddMenuItem(item, path));
}
//--------------------------------------------------
int AddMenuInput(SimulationContext &context, const char *path, const char *name,
                KR_ObjectID on, int el, int pVertSpace, int sideW,
                int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                const char *pFontName, const char *aFontName, const char *defText){
    MenuItemInput *item;
    IFixedFont *pFontI, *aFontI;
    KR_ObjectID pFontID, aFontID;

    if(context.isExist(pFontName)){
        pFontID = context.searchObject(pFontName);
        pFontI  = (IFixedFont *)context.queryInterface(pFontID, IFixedFontIID);
    }
    else pFontI = NULL;
    if(context.isExist(aFontName)){
        aFontID = context.searchObject(aFontName);
        aFontI  = (IFixedFont *)context.queryInterface(aFontID, IFixedFontIID);
    }
    else aFontI = NULL;
    
    item = new MenuItemInput(name, on, el, pVertSpace, sideW, pColor, pOpasity, abColor, abOpasity, afColor, afOpasity, pFontI, aFontI, defText);
    
    return(g_menu.AddMenuItem(item, path));
}
//--------------------------------------------------
int AddMenuScroll(SimulationContext &context, const char *path, const char *name,
                KR_ObjectID on, int el, int pVertSpace, int sideW,
                int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                const char *pFontName, const char *aFontName, const char *attrName, const char *l0Text, const char *l1Text,
                double min, double max, double step, double def){
    MenuItemScroll *item;
    IFixedFont *pFontI, *aFontI;
    KR_ObjectID pFontID, aFontID;

    if(context.isExist(pFontName)){
        pFontID = context.searchObject(pFontName);
        pFontI  = (IFixedFont *)context.queryInterface(pFontID, IFixedFontIID);
    }
    else pFontI = NULL;
    if(context.isExist(aFontName)){
        aFontID = context.searchObject(aFontName);
        aFontI  = (IFixedFont *)context.queryInterface(aFontID, IFixedFontIID);
    }
    else aFontI = NULL;
    
    item = new MenuItemScroll(name, on, el, pVertSpace, sideW, pColor, pOpasity, abColor, abOpasity, afColor, afOpasity, pFontI, aFontI, attrName, l0Text, l1Text, min, max, step, def);
    
    return(g_menu.AddMenuItem(item, path));
}
//--------------------------------------------------
int AddMenuList(SimulationContext &context, const char *path, const char *name,
                KR_ObjectID on, int el, int pVertSpace, int sideW,
                int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                const char *pFontName, const char *aFontName, const char *attrName, const char *l0Text, const char *l1Text, int immediate){
    MenuItemList *item;
    IFixedFont *pFontI, *aFontI;
    KR_ObjectID pFontID, aFontID;

    if(context.isExist(pFontName)){
        pFontID = context.searchObject(pFontName);
        pFontI  = (IFixedFont *)context.queryInterface(pFontID, IFixedFontIID);
    }
    else pFontI = NULL;
    if(context.isExist(aFontName)){
        aFontID = context.searchObject(aFontName);
        aFontI  = (IFixedFont *)context.queryInterface(aFontID, IFixedFontIID);
    }
    else aFontI = NULL;
    
    item = new MenuItemList(name, on, el, pVertSpace, sideW, pColor, pOpasity, abColor, abOpasity, afColor, afOpasity, pFontI, aFontI, attrName, l0Text, l1Text, immediate);
    
    return(g_menu.AddMenuItem(item, path));
}
//--------------------------------------------------
int AddMenuListItem(const char *path, const char *str){
    KR_ObjectID setupID;
    MenuItemList *item;

    item = (MenuItemList *)g_menu.SearchMenuItem(path);
    if(item == NULL) return(0);

    item->AddItem(str);

    return(1);
}
//--------------------------------------------------
int AddMenuSetup(SimulationContext &context, const char *path, const char *name,
                KR_ObjectID on, int el, int pVertSpace, int sideW,
                int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                const char *pFontName, const char *aFontName,
                const char *l0Text, const char *l1Text){
    MenuControlSetup *item;
    IFixedFont *pFontI, *aFontI;
    KR_ObjectID pFontID, aFontID;
    const char setupName[] = "Menu.Setup";

    if(context.isExist(pFontName)){
        pFontID = context.searchObject(pFontName);
        pFontI  = (IFixedFont *)context.queryInterface(pFontID, IFixedFontIID);
    }
    else pFontI = NULL;
    if(context.isExist(aFontName)){
        aFontID = context.searchObject(aFontName);
        aFontI  = (IFixedFont *)context.queryInterface(aFontID, IFixedFontIID);
    }
    else aFontI = NULL;

    if(context.isExist(setupName)) return(0);
    item = new MenuControlSetup(name, on, el, pVertSpace, sideW, pColor, pOpasity, abColor, abOpasity, afColor, afOpasity, pFontI, aFontI, l0Text, l1Text);
    context.addObject(setupName, item);
    
    return(g_menu.AddMenuItem(item, path));
}
//--------------------------------------------------
int AddMenuSetupLine(SimulationContext &context, const char *name, const char *ctrlName){
    KR_ObjectID setupID;
    KR_Event event;
    
    if(!context.isExist("Menu.Setup")) return(0);
    setupID           = context.searchObject("Menu.Setup");
    event.source      = KR_ObjectID::NUL();
    event.destination = setupID;
    event.label       = EV_MENU_SETUP_ADD_LINE;
    event.timeStamp   = Session::m_moment;
    event.data.open(EDO_WRITE)
                  .putStr(name)
                  .putStr(ctrlName)
              .close();
    context.sendEventNow(event);

    return(1);
}
