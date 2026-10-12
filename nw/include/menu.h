#ifndef __MENU_H__
#define __MENU_H__

#define MAX_MENUITEM_NAME_LEN 30
#define MAX_MENUITEM_TEXT_LEN 30
#define MAX_MENUITEM_LIST_LEN 30
#define MAX_LIST_ITEMS 10
#define MAX_MENU_INPUTFIELD_LEN 50
#define MAX_CONTROL_LINE_NAME_LEN 20
#define MAX_CONTROLS_LINES 50
#define MAX_CONTROL_BIND_NAMES_LEN 40
#define MAX_LOCALES_NUM 2
#define MAX_ATTR_NAMELEN 30

#include <stdlib.h>
#include <stdio.h>
#include "kernel\h\object.h"
#include "kernel\h\s_debug.h"
#include "hardware.h"
#include "h/olevel.h"

class IImage;
class IFixedFont;

//================================================================
class MenuIterface_{
public:
    virtual double GetDouble()   const = 0;
    virtual const char *GetStr() const = 0;
    virtual void SetDouble(double d)   = 0;
    virtual void SetStr(const char *s) = 0;
};


//================================================================
class Menu;
class MenuItem_ : public MenuIterface_{
friend class Menu;
public:
    MenuItem_(const char *name, KR_ObjectID objNotify, int eventLabel, int pVertSpace, int sideW,
              int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity);
    virtual ~MenuItem_(){}

    virtual void CalcSelfCoords(int maxItemLen = -1) = 0;
    virtual int  GetItemLen() const = 0;
    virtual void GraphEvent(){}
    
    virtual void Draw(int ox, int oy, int active) const;
    
    virtual void OnActivate() {}
    virtual void OnDeactivate() {}
    virtual void OnChoose() {}
    virtual void OnRelease() {}
    virtual void OnTime(double ) {}
    virtual int  PostKey(int, int, int, int) { return(0); }
    virtual double GetDouble() const {return(0);}
    virtual const char *GetStr() const {return(NULL);}
    virtual void SetDouble(double)    {}
    virtual void SetStr(const char *) {}

    virtual void SetLocale(int l = -1) { 
        s_ASSERT(l < MAX_LOCALES_NUM, ""); 
        if(l != -1) m_locale = l; 
    }

    const char *GetName() const { return(m_name); }
    int IsHidden() const {return(m_hidden); }
    int IsDisabled() const {return(m_disabled); }
    void Enable(int enable) { m_disabled = !enable; }
    void Show(int show)     { m_hidden = !show; }

    int GetGrW() const {return(m_totalW); }
    int GetGrH() const {return(m_totalH); }
    void SetXY(int x, int y);
protected:
    void DrawAlphaSprite(int x0, int y0, int x1, int y1, 
                         int u0, int v0, int u1, int v1,
                         int color, int opasity, void *texture) const;
    Menu *m_menu;

    int m_baseX, m_baseY, m_totalW, m_totalH;
    int m_centerW;
    int m_aH;
    int m_pVertSpace, m_pSideW, m_aSideW;
    int m_aBaseX, m_aBaseY;
    int m_locale;

    int m_centerSegment;
    int m_active;
    int m_disabled;
    int m_hidden;
    KR_ObjectID m_objNotify;
	int m_eventLabel;
    int m_pColor, m_pOpasity;
    int m_abColor, m_afColor, m_abOpasity, m_afOpasity;
    void *m_textute;

    char m_name[MAX_MENUITEM_NAME_LEN];
    MenuItem_ *m_next, *m_prev;
    MenuItem_ *m_down, *m_up;
};
//=====================================================
class MenuItemText : public MenuItem_{
public:
    MenuItemText(const char *name, KR_ObjectID on, int el, int pVertSpace, int sideW,
                 int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                 IFixedFont *pFontI, IFixedFont *aFontI, const char *l0Text, const char *l1Text);
    
    virtual void CalcSelfCoords(int maxItemLen);
    virtual int  GetItemLen() const;
    virtual void SetLocale(int l = -1);
    virtual void GraphEvent();
    virtual const char *GetStr() const {return(m_text[m_locale]);}
    virtual void SetStr(const char *s) { 
        strncpy(m_text[m_locale], s, MAX_MENUITEM_TEXT_LEN-1); 
        m_text[m_locale][MAX_MENUITEM_TEXT_LEN-1] = 0;
    }
    virtual void Draw(int ox, int oy, int active) const;
protected:
    char m_text[MAX_LOCALES_NUM][MAX_MENUITEM_TEXT_LEN];
    IFixedFont *m_pFontI, *m_aFontI;

    int m_pTextBaseX, m_pTextBaseY;
    int m_aTextBaseX, m_aTextBaseY;
    int m_pTextW, m_pTextH;
    int m_aTextW, m_aTextH;
    int m_maxItemLen;
};
//================================================================
class MenuItemInput : public MenuItemText{
public:
    MenuItemInput(const char *name, KR_ObjectID on, int el, int pVertSpace, int sideW,
                 int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                 IFixedFont *pFontI, IFixedFont *aFontI, const char *defText)
                 :MenuItemText(name, on, el, pVertSpace, sideW, pColor, pOpasity, abColor, abOpasity, afColor, afOpasity, pFontI, aFontI, defText, defText)
    { m_inputActive = FALSE;}
    
    virtual int GetItemLen() const;
    void CalcSelfCoords(int maxItemLen);
    virtual int PostKey(int ctrlEvent, int code, int release, int repeat);
protected:
    int m_inputActive;
    char m_oldText[MAX_MENUITEM_TEXT_LEN];
};
//================================================================
class MenuItemScroll : public MenuItemText{
public:
    MenuItemScroll(const char *name, KR_ObjectID on, int el, int pVertSpace, int sideW,
                 int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                 IFixedFont *pFontI, IFixedFont *aFontI, const char *attrName, const char *l0Text, const char *l1Text, 
                 double min, double max, double step, double def);

    virtual void CalcSelfCoords(int maxItemLen);
    virtual int  GetItemLen() const;
    virtual void GraphEvent();
    virtual int PostKey(int ctrlEvent, int code, int release, int repeat);
    virtual double GetDouble() const {return(MinMax(g_levelAttr.get_double(m_attrName), m_min, m_max));}
    virtual void SetDouble(double val)  { g_levelAttr.set_double(m_attrName, val); }
    
    virtual void Draw(int ox, int oy, int active) const;
protected:
    double m_min, m_max, m_step, m_def, m_delta;
    int m_lineW;
    int m_sBaseX, m_sBaseY;
    int m_cBaseX, m_cBaseY;
    int m_text2line, m_line2counter;
    char m_leftCH, m_rightCH, m_pointer;
    char m_attrName[MAX_ATTR_NAMELEN];
};
//================================================================
class MenuItemList : public MenuItemText{
public:
    MenuItemList(const char *name, KR_ObjectID on, int el, int pVertSpace, int sideW,
                 int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                 IFixedFont *pFontI, IFixedFont *aFontI, const char *attrName, const char *l0Text, const char *l1Text, int immediate);

    virtual void CalcSelfCoords(int maxItemLen);
    virtual int  GetItemLen() const;
    virtual void GraphEvent();
    virtual int PostKey(int ctrlEvent, int code, int release, int repeat);
    virtual double GetDouble() const {return(MinMax(g_levelAttr.get_double(m_attrName), (double)0, (double)(m_itemsNum-1)));}
    virtual void SetDouble(double val)  { g_levelAttr.set_double(m_attrName, val); }

    void AddItem(const char *str);
    
    virtual void Draw(int ox, int oy, int active) const;
protected:
    int m_itemsNum;
    int m_listBaseX, m_listBaseY;
    int m_maxItemW;
    int m_imediate;
    char m_item[MAX_LIST_ITEMS][MAX_MENUITEM_LIST_LEN];
    char m_attrName[MAX_ATTR_NAMELEN];
};
//================================================================

class MenuControlSetup : public MenuItemText, public KR_Object{
public:
    MenuControlSetup(const char *name, KR_ObjectID on, int el, int pVertSpace, int sideW,
                 int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                 IFixedFont *pFontI, IFixedFont *aFontI,
                 const char *l0Text, const char *l1Text);
    ~MenuControlSetup();

    void addNotify(){}
    void removeNotify(){}
    int  receiveEvent(KR_Event &event);

    void Draw(int ox, int oy, int active) const;
    void OnActivate();
    void OnDeactivate();

    void GraphEvent();
    virtual int PostKey(int ctrlEvent, int code, int release, int repeat);

    void AddControlLine(const char *name, const char *ctrlName);
    virtual bool	shouldDump () { return false; } 
protected:
    int  SearchLineByAction(int act) const {
        for(int i = 0; i < MAX_CONTROLS_LINES; ++i) if(m_line[i].action == act) return(i);
        return(-1);
    }
    void DrawPtr(int x, int y) const;
    void FillLines();
    int  SaveSetup(const char *fName) const;

    int m_setupActive;
    int m_addCtrlMode;

    int m_linesNum;
    int m_scrLinesBaseX, m_scrLinesBaseY;
    int m_scrBindesBaseX;
    int m_maxNameLen;
    int m_setupX, m_setupY;

    int m_normalCtrlColor, m_crossedCtrlColor;
    int m_maxScreenLinesNum;
    int m_vertSpacing;
    int m_horSpacing;
    int m_fontH;
    int m_yScrPos;
    int m_keyEnter;
    int m_grLen, m_grH;

    MenuItem_ *m_baseBackup, *m_prevBackup, *m_nextBackup;

    int m_curBase, m_curItem;
    
    struct {
        char name[MAX_CONTROL_LINE_NAME_LEN];
        int  action;
        char buttonsBinded[MAX_CONTROL_BIND_NAMES_LEN];
        int  color;
        int  buttonsBindedNum;
        int  code[MAX_CODES_ON_ACTION];
    } m_line[MAX_CONTROLS_LINES];
};

//=====================================================
class Menu : public KR_Object{
friend class MenuControlSetup;
friend class MenuItem_;
public:
    Menu(){}
    ~Menu(){}

    void addNotify();
    void removeNotify();
    int  receiveEvent(KR_Event &event);

    void Activate();
    void Deactivate();
    int  IsActive() const { return(m_active); }
    void ForceChieldNotify(MenuItem_ *item) { ChieldNotify(item); }
    void OnTime(double time);
    void OnChangeRes();
    void EnablePointer()  { m_pointerEnabled = TRUE; }
    void DisablePointer() { m_pointerEnabled = FALSE; }

    void SetRootAsCurrent();

    void DrawBkGround();

    const MenuItem_ *GetMenuItem(const char *path) const { return(SearchMenuItem(path)); }
    int AddMenuItem(MenuItem_ *item, const char *path);

    void Draw();
    void Init(const char *cfgFileName);
    void LinkControls() const { g_hardware.Link(); }

    void SetLocale(MenuItem_ *root, int locale);
    virtual bool	shouldDump () { return false; } 

    MenuItem_ *SearchMenuItem(const char *path) const;
protected:
    void CreateMenuCoords(MenuItem_ *root, int level);
    void DestroyItems(MenuItem_ *root);
    void ChieldNotify(MenuItem_ *item);
    void GraphEvent(MenuItem_ *root);

    SGRViewport *m_oldViewPort, *m_viewPort;

    int m_bkColor;
    int m_pointerEnabled;
    int m_pVertSpace;
    int m_baseX, m_baseY;
    MenuItem_ *m_items;
    MenuItem_ *m_curItem;
    MenuItem_ *m_curBase;
    int m_totalItemsNum;
	int m_active;
    void *m_textute;
};
//=====================================================


int AddMenuText(SimulationContext &context, const char *path, const char *name,
                KR_ObjectID on, int el, int pVertSpace, int sideW,
                int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                const char *pFontName, const char *aFontName,
                const char *l0Text, const char *l1Text);

int AddMenuInput(SimulationContext &context, const char *path, const char *name,
                KR_ObjectID on, int el, int pVertSpace, int sideW,
                int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                const char *pFontName, const char *aFontName, const char *defText);

int AddMenuScroll(SimulationContext &context, const char *path, const char *name,
                KR_ObjectID on, int el, int pVertSpace, int sideW,
                int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                const char *pFontName, const char *aFontName, const char *attrName, const char *l0Text, const char *l1Text,
                double min, double max, double step, double def);

int AddMenuList(SimulationContext &context, const char *path, const char *name,
                KR_ObjectID on, int el, int pVertSpace, int sideW,
                int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                const char *pFontName, const char *aFontName, const char *attrName, const char *l0Text, const char *l1Text, int immediate);

int AddMenuListItem(const char *path, const char *str);


int AddMenuSetup(SimulationContext &context, const char *path, const char *name,
                KR_ObjectID on, int el, int pVertSpace, int sideW,
                int pColor, int pOpasity, int abColor, int abOpasity, int afColor, int afOpasity,
                const char *pFontName, const char *aFontName,
                const char *l0Text, const char *l1Text);

int AddMenuSetupLine(SimulationContext &context, const char *name, const char *ctrlName);

extern Menu g_menu;

#endif //__MENU_H__
