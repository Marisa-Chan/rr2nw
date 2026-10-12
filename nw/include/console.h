#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#define CON_MAX_PROMPT_LEN 10
#define CON_MAX_MESSAGES   10
#define CON_MAX_MESSAGE_LEN 120
#define CON_MAX_MEMORY      20
#define CON_MAX_INPUT_LEN  200
#define CON_MAX_DISPLAY_MSG 10

#include "input.h"
#include "graph.h"
#include "../arena/kernel/h/object.h"
#include "../arena/kernel/h/echo.h"
#include "../arena/message/conmsg.h"
#include "../arena/kernel/h/context.h"
#include "../arena/storage/h/classtab.h"
#include "font.h"

extern DWORD  WINAPI HookThread(LPVOID);
extern HANDLE hookThreadHandle;
extern DWORD  hookThreadID;


class GameConsole : public KR_Object{


   friend DWORD WINAPI HookThread(LPVOID vConsole);

   enum { KEY_NONE = 256,
          KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN,
          KEY_CLEFT, KEY_CRIGHT,
          KEY_BS, KEY_DEL, KEY_END, KEY_HOME,
          KEY_ESC, KEY_ENTER };


   enum Modes { LEVEL_NONE, LEVEL_TABLE, LEVEL_ATTRIBUTE, LEVEL_ITEM } currentLevel;
   
   ct_ClassTablePTR             m_cacheClassTable;              
   ct_Attribute *               m_cacheAttribute;               

   
   BOOL m_logMode;
   char m_logFileName[255];

   //void logEcho(char *format, va_list msg);
   void logEcho(char * string);
   void PrintAttributeValues();

   int	GetDataSize();
   void FillAttributeData(PVOID data, int size);
   void UpdateData(LPVOID data);


public:
   enum {NORMAL, LEFT, RIGHT, TOP, BOTTOM, CENTER, TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT};
   GameConsole();
   virtual ~GameConsole() { /*TerminateThread(hookThreadHandle, 0);*/ }

   virtual int receiveEvent(KR_Event &event);
   void addNotify   ();
   void removeNotify();

   void ReadVessels(const char * szCfgName);
   
   void Init(const char *conFontName, const char *msgFontName, int bColor, int opasity, int msgLines, const char *logFileName = NULL);
   void Draw();
   void ChangeResEvent();
   int IsActive() const { return(m_active); }
   void RecreateFonts();
   
   virtual bool    shouldDump () { return false; } 

   void Print(const char *str, double timeToDisplay = 4, const char *fontName = NULL);
   void PrintUrgent(const char *str, double timeToDisplay = 4, int aligin = NORMAL, int bkColor = -1, const char *fontName = NULL);
   void PrintFrame(const char *str, int aligin = NORMAL);
   void DeleteUrgentMessge() { m_urgentMsg.text[0] = 0; }

   void SetPrompt(const char *str) {
      ASSERT(strlen(str) < CON_MAX_PROMPT_LEN);
      strcpy(m_prompt, str);
   }
   int IsLetter(int code) const;
protected:
    void Update();
    void DrawBkGround() const;
    int  CalcAligin(const char *str, int alignment, int &maxStrLen, int &x, int &y) const;
    void PreemptDisplayMessage(int i);
    void PrintEditStr() const;
    void DrawCursor(int x, int y, int len, int h) const;
    void Activate();
    void Deactivate();
    
    void InsertCharAt(char *str, int pos, char ch) const;
    void DeleteCharAt(char *str, int pos) const;
    int  IsSeparator(char ch) const;
    void RemoveEmptyMemory();
    void PreemptMemory(int n);
    void WordLeft(const char *str);
    void WordRight(const char *str);
    void AddMemStr(const char *str);
    void ClearMem();
    void ClearEditStr() { m_editStr[0] = 0; m_pos = 0; }

    void ParseCommand(const char *str);
   
   SGRViewport *m_oldViewPort, *m_viewPort;
   IFixedFont *m_conFontI, *m_msgFontI;
   int m_bColor, m_curColor, m_opasity;

    int m_lCtrl, m_rCtrl;
	int m_del, m_numDel;
	int m_home, m_end;
	int m_left, m_right, m_up, m_down;
	int m_esc, m_enter, m_numEnter, m_bs;
    int m_ctrlPressed;

   int m_firstChar;
   int m_active;
   
   FILE *m_logFH;
   
   int m_len, m_h, m_spacing, m_conFontH, m_msgFontH;
   int m_conLines, m_msgLines;
   
   char m_prompt[CON_MAX_PROMPT_LEN];
   struct string{
       double timeToDisplay;
       double postTime;
       char text[CON_MAX_MESSAGE_LEN];
       IFixedFont *font;
       int x, y;
       int linesNum;
       int maxStrLen;
       int color;
   }m_message[CON_MAX_MESSAGES], m_urgentMsg, m_frameMsg, m_displayMsg[CON_MAX_DISPLAY_MSG];
   
   int m_msgNum, m_dispMsgNum;
   int m_curMsg;
   int m_pos, m_editStrLen, m_curMemStr;
   int m_memItemsNum;

   char m_mem[CON_MAX_MEMORY][CON_MAX_INPUT_LEN];
   char m_editStr[CON_MAX_INPUT_LEN];
};

extern GameConsole g_GameConsole;

#endif
