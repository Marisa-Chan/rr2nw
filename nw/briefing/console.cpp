/*
   Класс GameConsole - экспортируется.
   Обеспечивает работу со строкой ввода в графических режимах.
   Строка обладает памятью на введенные строки.
   Позволяет редактировать текущую строку.
   Позволяет менять строку приглашения.
   Использует горизонтальный скролинг.
   Различает цвета: фона, текста, приглашения, курсора.
   Максимальная длина строки               - CONIN_MAX_STR_LEN    (255).
   Максимальная количество строк в памяти  - CONIN_MAX_STR        (30).
   Максимальная длина приглашения          - CONIN_MAX_PROMPT_LEN (80).
   Позволяет редактировать строку, используя клавиши:
      LEFT, RIGHT, DEL, BS, HOME, END, CTRL+LEFT, CTRL+RIGHT.
   Редактирование производится в режиме вставки.
   Для работы с памятью слюжат клавиши UP, DOWN.

   НЕ ИСПОЛЬЗУЕТ ДИНАМИЧЕСКУЮ ПАМЯТЬ.

   Инерфейсные методы:

      void Init(int x, int y, int len, int h, int spacing);
      const char *Activate();
      void ClearMem();
      void SetPrompt(const char *str);
      void SetDefualtStr(const char *str);
      void SetColor(int fc, int pc, int, cc, int bc);

   Использование:

   1) Создать объект.
   2) Установить графическое расположение функцией Init().
   3) НЕОБЯЗАТЕЛЬНО - установить цвета SetColor().
   4) НЕОБЯЗАТЕЛЬНО - установить приглашение SetPrompt().
   5) НЕОБЯЗАТЕЛЬНО - установить текущею строку  SetDefualtStr().
   6) Вызвать Activate().

   Над объектом можно выполнять произвольное количество операций
   Init() и Activate().

   В любое время память может быть очищена вызовом ClearMem().

***************************************************************************

   Класс ScrollPrint - экспортируется.
   Обеспечивет пользователю скролирующююся по времяни и вытеснению консоль.
   Максимальная длина строки               - CONOUT_MAX_STR_LEN    (255).
   Максимальная количество строк в памяти  - CONOUT_MAX_STR        (30).

   НЕ ИСПОЛЬЗУЕТ ДИНАМИЧЕСКУЮ ПАМЯТЬ.

   Инерфейсные методы:

      void Init(int x, int y, int len, int h, int spacing, int mxStr, bool trans);
      void Print(const char *txt, double delay);
      void Print(const char *txt, int fColor, int bColor, double delay);
      void Clear();
      void SetColor(int fc, int bc);
      void TimeUpdate();
      void Draw();

   Использование:

   1) Создать объект.
   2) Установить графическое расположение функцией Init().
   3) НЕОБЯЗАТЕЛЬНО - установить цвета SetColor().
   4) Далее следует любое количество опреаций:
      { [SetColor()] Print() } | { [...], Clear(), [...]}
      { TimeUpdate() }
      { Draw() }

   В любое время консоль может быть очищена вызовом Clear().(Не на экране !!)


   Пример использования классов в BRIEFING.CPP.

 */
#define LAST_H__SCENE
#include "game.h"
#include <windows.h>
#include <ctype.h>
#include "patterns.h"
#include "console.h"
#include "../arena/message/hardmsg.h"
#include "hardware.h"
#include "kernel/h/session.h"
#include "..\ARENA\STORAGE\H\attr.h"
#include "..\ARENA\STORAGE\H\subject.h"


extern void VRectangle(int x, int y, int len, int h, int c, bool trans, int opasity, int transCoords);

typedef enum {
	DATA_FILLATTR = 0,
	DATA_SETATTR,
	DATA_GOD,
	DATA_UPDATE,
} DataTransfer;


HANDLE hookThreadHandle;
DWORD  hookThreadID;

DWORD WINAPI HookThread(LPVOID vConsole)
{

	GameConsole * console = (GameConsole * ) vConsole;


	HANDLE s_hFileMap = NULL;
		

	int size = console->GetDataSize() + sizeof(int);


	s_hFileMap = ::CreateFileMapping((HANDLE) 0xFFFFFFFF, NULL, PAGE_READWRITE, 0, 
		size , __TEXT("RR2HookSharedData") );

	if (s_hFileMap == NULL)
	{
		console->logEcho("File Mapping Creation Failed");
		return 0;
	}

	LPVOID data = MapViewOfFile (s_hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0 ,0 ,0);

	if (! data)
	{
		console->logEcho("File Mapping Failed");
		CloseHandle(s_hFileMap);
		return 0;
	}

	console->FillAttributeData(data, size);

	HANDLE h_mutex = CreateMutex(NULL,FALSE,__TEXT("RR2HookMutex"));

	

	while (1)
	{
		WaitForSingleObject(h_mutex,INFINITE);
		console->logEcho("Mutex Caught");

		console->UpdateData(data);

		ReleaseMutex(h_mutex);
		Sleep(1000);		
	}

}

extern int g_godMode;

void GameConsole::UpdateData(LPVOID data)
{
	char * p = (char *) data;

	int action = *(int *) p;
	p += sizeof(int);

	char * table;
	char * attr;
	char * item;
	void * value;

	switch ( action )		
	{
	case DATA_GOD:
		if (g_godMode)
 			g_godMode = 0;
		else
			g_godMode = 1;

		
		logEcho("Toggle God Mode");			

		break;

	case DATA_UPDATE:
		
		if (currentLevel != LEVEL_ATTRIBUTE)
		{
			logEcho("Must be at ATTRIBUTE level\n");
			return;
		}

		logEcho("Update\n");
		m_cacheAttribute->update(Session::m_moment);
		break;

	case DATA_SETATTR:
		{
			table = p;
			attr  = p + 128;
			item  = p + 256;
			value  = p + 384;
			
			logEcho(table);
			logEcho(attr);
			logEcho(item);

			m_cacheClassTable = g_arena.searchClassTable( table );

			for( const ct_Object *ob = m_cacheClassTable->m_existList; ob!=NULL;ob = ob->m_next )
			{
				const char * name = g_arena.context->searchObject(ob->getObjectID());
				if (strcmp(attr,name) == 0)
				{
					m_cacheAttribute = (ct_Attribute	*) ob;
					break;
				}
			}

			ct_AttrItem  *theItem = m_cacheAttribute->searchItem( item );

			if (theItem->m_type != EDI_STR)
				memcpy(theItem->m_variable,value, sizeof(double));
			else
			{
				int len = strlen((char *)value);
				
				char * b = new char [len + 1];
				strcpy(b, (char *)value);
				theItem->m_variable = b;
			}

			currentLevel = LEVEL_ATTRIBUTE;
		}
		break;
	}
}

//---------------------------------------------------------
// Init var to default values.
GameConsole::GameConsole() : 
KR_Object(){
}

void GameConsole::FillAttributeData(PVOID _data, int size)
{

	char * data = (char * ) _data;

	char * p = data;

	* (int *) p = DATA_FILLATTR;
	p += sizeof(int);

	ct_ClassTable *list = ct_Storage::m_classTableList;
	
	* (int *) p = size;

	char * numberOfTables = p + sizeof(int);
	p+= 2 * sizeof(int);
		
	
	int t = 0;
	
	for(; list!=NULL ; list = list->m_nextClassTable )
	{

		if (list->objectsType() == OBJECT_ATTRIBUTE)
		{
			t++;
			strcpy(p, list->m_name);
			p += 128;
			
			char * numberOfAttributes = p;

			p += sizeof(int);

			int j = 0;
						
			for( const ct_Object *ob = list->m_existList; 
					ob!=NULL;ob = ob->m_next)
			{		
						
				j++;
				
				strcpy(p,g_arena.context->searchObject(ob->getObjectID()));						
				p += 128;

				char * numberOfItems = p;

				p += sizeof(int);

				ct_Attribute * attr =  (ct_Attribute *) ob;

				int i;

				for (i = 0; i < attr->m_attrQnty; i++)
				{
					strcpy(p, attr->m_list[i].m_name);
					p += 128;
					
					* (int *) p = attr->m_list[i].m_type;
					
					p += sizeof(int);	// attribte item type
					
					switch (attr->m_list[i].m_type) {
					case EDI_BYTE:
						* (unsigned char *) p = * ((unsigned char *) attr->m_list[i].m_variable);
						p += sizeof(double);	
						break;
					case EDI_CHAR:
						* (char *) p = * ((char *)attr->m_list[i].m_variable) ;
						p += sizeof(double);	
						break;
					case EDI_INT:
						* (int *) p = * ((int *)attr->m_list[i].m_variable) ;
						p += sizeof(double);	
						break;
					case EDI_UINT:
						* (unsigned *) p = * ((unsigned *)attr->m_list[i].m_variable);
						p += sizeof(double);	
						break;
					case EDI_LONG:
						* (long *) p = * ((long *)attr->m_list[i].m_variable);
						p += sizeof(double);	
						break;
					case EDI_ULONG:
						* (unsigned long *) p = * ((unsigned long *)attr->m_list[i].m_variable);
						p += sizeof(double);	
						break;
					case EDI_SHORT:
						* (short *) p = * ((short *)attr->m_list[i].m_variable);
						p += sizeof(double);	
						break;
					case EDI_USHORT:
						* (unsigned short *) p = * ((unsigned short *)attr->m_list[i].m_variable);
						p += sizeof(double);	
						break;
					case EDI_DOUBLE:
						* (double *) p = * ((double *)attr->m_list[i].m_variable);
						p += sizeof(double);	
						break;
					case EDI_FLOAT:
						* (float *) p = * ((float *) attr->m_list[i].m_variable);
						p += sizeof(double);	
						break;
					case EDI_STR:
						strcpy(p,(char *) attr->m_list[i].m_variable);
						p += 128;
						break;
					};			
				
				} // for attribute items

				* (int *) numberOfItems = i;

			} // for attribute names

			* (int *) numberOfAttributes = j;
		} // if attribute table
	} // tables

	* (int *) numberOfTables = t;
}

int GameConsole::GetDataSize()
{

	
	ct_ClassTable *list = ct_Storage::m_classTableList;
	int size = 2 * sizeof(int); // size of structure + number of attr tables
		
	for(; list!=NULL ; list = list->m_nextClassTable )
	{
		if (list->objectsType() == OBJECT_ATTRIBUTE)
		{
			size += 128;		 // length of table name;	
			size += sizeof(int); // number of attributes
			
			for( const ct_Object *ob = list->m_existList; 
					ob!=NULL;ob = ob->m_next )
			{								
						
				ct_Attribute * attr =  (ct_Attribute *) ob;

				size += 128; // length of attribte name;
				size += sizeof(int); // number of attribute items

				for (int i = 0; i < attr->m_attrQnty; i++)
				{
					size += 128;			// length of attribte item name
					size += sizeof(int);	// attribte item type

					if (attr->m_list[i].m_type == EDI_STR)
						size += 128; //string length
					else
						size += sizeof(double); // attribute item value				
				} // for attribute items
			} // for attribute names
		} // if attribute table
	} // tables

	return size;
}

//---------------------------------------------------------
// Inittialize console params.
void GameConsole::Init(const char *conFontName, const char *msgFontName, int bColor, int opasity, int msgLines, const char *logFileName){
    KR_ObjectID conFontID, msgFontID;

    if(getContext()->isExist(conFontName)){
        conFontID  = getContext()->searchObject(conFontName);
        m_conFontI = (IFixedFont *)getContext()->queryInterface(conFontID, IFixedFontIID);
    }
    else s_ASSERTNQ("");
    
    if(getContext()->isExist(msgFontName)){
        msgFontID  = getContext()->searchObject(msgFontName);
        m_msgFontI = (IFixedFont *)getContext()->queryInterface(msgFontID, IFixedFontIID);
    }
    else s_ASSERTNQ("");

	m_lCtrl	      = g_hardware.SearchCode("LCtrl");
	m_rCtrl	      = g_hardware.SearchCode("RCtrl");
	m_del	      = g_hardware.SearchCode("Del");
	m_numDel      = g_hardware.SearchCode("NumDel");
	m_home	      = g_hardware.SearchCode("Home");
	m_end	      = g_hardware.SearchCode("End");
	m_left	      = g_hardware.SearchCode("Left");
	m_right	      = g_hardware.SearchCode("Right");
	m_up	      = g_hardware.SearchCode("Up");
	m_down	      = g_hardware.SearchCode("Down");
	m_esc	      = g_hardware.SearchCode("Esc");
	m_enter	      = g_hardware.SearchCode("Enter");
	m_numEnter    = g_hardware.SearchCode("NumEnter");
	m_bs 	      = g_hardware.SearchCode("Bs");
    m_firstChar   = FALSE;
    m_bColor      = bColor;
    m_opasity     = opasity;
    m_logFH       = NULL;
    m_msgLines    = MinMax(msgLines, 1, CON_MAX_DISPLAY_MSG-2);
    m_curColor    = GRTransparentColor(255, 255, 255);
    m_memItemsNum = 0;
    m_curMemStr   = 0;
    if(logFileName != NULL){
        m_logFH = fopen(logFileName, "wt");
        if(m_logFH == NULL) echo("Error opening log file '%s'", logFileName);
    }
    ChangeResEvent();
}
//---------------------------------------------------
void GameConsole::addNotify(){

    int i;
    
    m_active     = FALSE;
    m_msgNum     = 0;
    m_dispMsgNum = 0;
    m_curMsg     = 0;
    for(i = 0; i < CON_MAX_MESSAGES; ++i){
        m_message[i].timeToDisplay = 0;
        m_message[i].postTime      = 0;
        m_message[i].text[0]       = 0;
    }
    for(i = 0; i < CON_MAX_DISPLAY_MSG; ++i){
        m_displayMsg[i].timeToDisplay = 0;
        m_displayMsg[i].postTime      = 0;
        m_displayMsg[i].text[0]       = 0;
    }
    m_urgentMsg.timeToDisplay = 0;
    m_urgentMsg.postTime      = 0;
    m_urgentMsg.text[0]       = 0;

    for(i = 0; i < CON_MAX_MEMORY; ++i) m_mem[i][0] = 0;
    m_editStr[0] = 0;

    SetPrompt(">");
    m_logMode = FALSE;
    currentLevel = LEVEL_NONE;     


	KR_Event event;

	event.source	  = getObjectID();
	event.destination = g_hardware.getObjectID();
	event.label	  = CTRL_SUBSCRIBE;
	event.timeStamp	  = Session::m_moment;
	event.data.open(EDO_WRITE)
				.putObjectID(getObjectID())
	            .putInt(EXCLUSIVE)
			  .close();
	getContext()->addEvent(event);
}
//-------------------------------------------------------------
void GameConsole::removeNotify(){
	KR_Event event;
	
	event.timeStamp   = Session::m_moment;
	event.source	  = getObjectID();
	event.destination = g_hardware.getObjectID();
	event.label	  = CTRL_UNSUBSCRIBE;
        if (event.destination.isNUL())return;
        if (context) context->sendEventNow(event);
}
//---------------------------------------------------
void GameConsole::RecreateFonts(){
    if(m_conFontI != NULL) m_conFontI->RecreateFont();
    if(m_msgFontI != NULL) m_msgFontI->RecreateFont();
}
//---------------------------------------------------
int GameConsole::receiveEvent(KR_Event &event){
	int code;
	int ctrlEvent;
	double timeStamp, down;
	int repeat;
	char ch;
	
	switch( event.label ){
		case KR_WAKE_UP: break;
		case CTRL_MOUSE_MOVE_MSG:    break;
		case CTRL_JOYSTICK_MOVE_MSG: break;
        case CTRL_CHAR: 
			event.data.open(EDO_READ)
						.getInt(code)
					  .close();
            if(!m_active) break;
            if(m_firstChar){
                m_firstChar = FALSE;
                break;
            }
			if(code == m_enter || code == m_numEnter){ 
				if(m_editStrLen > 0)
                                {
				    ParseCommand(m_editStr);
                                    AddMemStr(m_editStr);	
                                }
                Print(m_editStr);
                //PrintUrgent(m_editStr);
                ClearEditStr();
			}
			if(code == m_bs && m_pos > 0){
				DeleteCharAt(m_editStr, m_pos-1);
				m_pos--; 
				m_editStrLen--;
			}
            if(IsLetter(code) && m_editStrLen < CON_MAX_INPUT_LEN-1){
                ch = (char)code ;
				InsertCharAt(m_editStr, m_pos, ch);
				m_pos++;
				m_editStrLen++;
			}
            break;
		case CTRL_BUTTONS_MSG:
			timeStamp = event.timeStamp;
            event.data.open(EDO_READ)
						.getInt(ctrlEvent)			
                        .getDouble(down)
                        .getInt(code)
                        .getInt(repeat)
					  .close();
            
            if((code == m_lCtrl || code == m_rCtrl) && !repeat && ctrlEvent == SYS_KEY){   
				m_ctrlPressed = down;
				break; 
			}
			if(down == 0 || code == 0) break;
			if(ctrlEvent == CONSOLE) 
				if(!m_active){ 
					Activate(); 
                    m_firstChar = TRUE;
					break; 
				}
                else{
                    Deactivate();
                    break;
                }
            if(ctrlEvent != SYS_KEY) break;
			if(code == m_left  && m_pos > 0  ) m_pos--;
			if(code == m_right && m_pos < m_editStrLen) m_pos++;
			if(code == m_left  && m_ctrlPressed) WordLeft(m_editStr);
			if(code == m_right && m_ctrlPressed) WordRight(m_editStr);
			if(code == m_home) m_pos = 0;
			if(code == m_end ) m_pos = m_editStrLen;
			if(code == m_up && m_curMemStr > 0){
				m_curMemStr--;	
				strcpy(m_editStr, m_mem[m_curMemStr]);
				m_editStrLen = strlen(m_editStr); 
				m_pos        = m_editStrLen;
			}
			if(code == m_down && m_curMemStr < m_memItemsNum){
				m_curMemStr++;
				strcpy(m_editStr, m_mem[m_curMemStr]);
				m_editStrLen = strlen(m_editStr);
				m_pos    = m_editStrLen;
			}
			if((code == m_del || code == m_numDel) && m_pos < m_editStrLen){
				DeleteCharAt(m_editStr, m_pos);
				m_editStrLen--;
			}
			break;
		default: return(0);
    }
    return(1);
}
//---------------------------------------------------------
void GameConsole::Activate(){ 
	m_active = TRUE;
	g_hardware.GetExclusiveMode(getObjectID());
	m_editStr[0] = 0;
	m_editStrLen = 0;
	m_pos        = m_editStrLen;
	m_curMemStr  = Max(0, m_memItemsNum-1);
    AddMemStr("");
}
//---------------------------------------------------------
void GameConsole::Deactivate(){ 
	KR_Event event;
	
	RemoveEmptyMemory();
	m_active = FALSE; 
	g_hardware.ReleaseExclusiveMode();
}
//---------------------------------------------------
void GameConsole::ChangeResEvent(){
    SRect2 clipRect;

    clipRect.top    = 0;
    clipRect.bottom = _gr_nScreenHeight;
    clipRect.left   = 0;
    clipRect.right  = _gr_nScreenWidth;
    
    m_viewPort = GRCreateViewport(_gr_nScreenWidth/2, _gr_nScreenHeight/2, clipRect);
    m_spacing  = 1;
    m_conFontH = m_conFontI->Height();
    m_conLines = (_gr_nScreenHeight/2)/(m_conFontH+m_spacing)-1;
    m_h        = (m_conLines+1)*(m_conFontH+m_spacing);
    m_len      = _gr_nScreenWidth;
}
//---------------------------------------------------
void GameConsole::DrawBkGround() const{
    _gr_polygon.dwFullType = GR_POLY_TRANSPARENT;
    _gr_polygon.dwAddType  = 0;
    _gr_polygon.nVertices  = 4;
    _gr_polygon.dwOpacity  = m_opasity;
    _gr_polygon.nLights    = 0;
    _gr_polygon.dwColor.color = m_bColor;
    _gr_vertices[0].any.x = -_gr_nScreenOriginX;
    _gr_vertices[0].any.y = -_gr_nScreenOriginY;
    _gr_vertices[0].any.iz = 65535;
    _gr_vertices[1].any.x = -_gr_nScreenOriginX;
    _gr_vertices[1].any.y = -_gr_nScreenOriginY+m_h+4;
    _gr_vertices[1].any.iz = 65535;
    _gr_vertices[2].any.x = -_gr_nScreenOriginX+_gr_nScreenWidth;
    _gr_vertices[2].any.y = -_gr_nScreenOriginY+m_h+4;
    _gr_vertices[2].any.iz = 65535;
    _gr_vertices[3].any.x = -_gr_nScreenOriginX+_gr_nScreenWidth;
    _gr_vertices[3].any.y = -_gr_nScreenOriginY;
    _gr_vertices[3].any.iz = 65535;
    GRDrawPolygonPCCW();
}
//---------------------------------------------------
void GameConsole::Print(const char *str, double timeToDisplay, const char *fontName){
    KR_ObjectID fontID;

    if(str == NULL || timeToDisplay <= 0){
        echo("GameConsole::Print - illegal data.");
        return;
    }
    if(m_logFH != NULL) fprintf(m_logFH, "%s\n", str);

    while(m_dispMsgNum >= m_msgLines-1) 
        PreemptDisplayMessage(0);
    
    m_displayMsg[m_dispMsgNum].timeToDisplay = timeToDisplay;
    m_displayMsg[m_dispMsgNum].postTime      = Session::m_moment;
    m_displayMsg[m_dispMsgNum].font          = m_msgFontI;
    strncpy(m_displayMsg[m_dispMsgNum].text, str, CON_MAX_MESSAGE_LEN-1);
    m_displayMsg[m_dispMsgNum].text[CON_MAX_MESSAGE_LEN-1] = 0;
    m_dispMsgNum++;

    m_message[m_curMsg].timeToDisplay = timeToDisplay;
    m_message[m_curMsg].postTime      = Session::m_moment;
    m_message[m_curMsg].font          = m_conFontI;
    strncpy(m_message[m_curMsg].text, str, CON_MAX_MESSAGE_LEN-1);
    m_message[m_curMsg].text[CON_MAX_MESSAGE_LEN-1] = 0;
    if(m_msgNum < CON_MAX_MESSAGES-1) m_msgNum++;
    m_curMsg = (m_curMsg+1) % CON_MAX_MESSAGES;

    if(fontName != NULL && getContext()->isExist(fontName)){
        fontID  = getContext()->searchObject(fontName);
        m_message[m_curMsg].font = (IFixedFont *)getContext()->queryInterface(fontID, IFixedFontIID);
    }
}
//---------------------------------------------------
void GameConsole::PrintUrgent(const char *str, double timeToDisplay, int aligin, int bkColor, const char *fontName){
    KR_ObjectID fontID;

    if(str == NULL) return;
    if(timeToDisplay < 0){
        echo("GameConsole::PrintUrgent - illegal data.");
        return;
    }
    m_urgentMsg.linesNum      = CalcAligin(str, aligin, m_urgentMsg.maxStrLen, m_urgentMsg.x, m_urgentMsg.y);
    m_urgentMsg.timeToDisplay = timeToDisplay;
    m_urgentMsg.postTime      = Session::m_moment;
    m_urgentMsg.font          = m_msgFontI;
    m_urgentMsg.color         = bkColor;
    strncpy(m_urgentMsg.text, str, CON_MAX_MESSAGE_LEN-1);
    m_urgentMsg.text[CON_MAX_MESSAGE_LEN-1] = 0;
    if(fontName != NULL && getContext()->isExist(fontName)){
        fontID  = getContext()->searchObject(fontName);
        m_urgentMsg.font = (IFixedFont *)getContext()->queryInterface(fontID, IFixedFontIID);
    }
}
//---------------------------------------------------
void GameConsole::PrintFrame(const char *str, int aligin){
    if(str == NULL){
        echo("GameConsole::PrintFrame - illegal data.");
        return;
    }
    m_frameMsg.linesNum      = CalcAligin(str, aligin, m_frameMsg.maxStrLen, m_frameMsg.x, m_frameMsg.y);
    m_frameMsg.timeToDisplay = 0;
    m_frameMsg.postTime      = Session::m_moment;
    m_frameMsg.font          = m_msgFontI;
    strncpy(m_urgentMsg.text, str, CON_MAX_MESSAGE_LEN-1);
    m_frameMsg.text[CON_MAX_MESSAGE_LEN-1] = 0;
}
//---------------------------------------------------
int GameConsole::CalcAligin(const char *str, int alignment, int &maxStrLen, int &x, int &y) const{
    int strLen, n, i, l;
    const int indent = 5;
    
    maxStrLen = strLen = 0;
    l = strlen(str);
    for(n = 1, i = 0; i < l; ++i){
        if(i < l-1 && str[i] == '\\' && str[i+1] == 'n'){
            ++n;
            ++i;
            if(strLen > maxStrLen) maxStrLen = strLen;
            strLen = 0;
            continue;
        }
        strLen += m_msgFontI->CharWidth(str[i]);
    }
    if(strLen > maxStrLen) maxStrLen = strLen;
    switch(alignment){
        case CENTER:       
            x = (_gr_nScreenWidth - maxStrLen)/2; 
            y = (_gr_nScreenHeight - (m_msgFontH+m_spacing)*n)/2; 
            break;
        case LEFT:
            x = indent;
            y = (_gr_nScreenHeight - (m_msgFontH+m_spacing)*n)/2; 
            break;
        case RIGHT:
            x = (_gr_nScreenWidth-maxStrLen);
            y = (_gr_nScreenHeight - (m_msgFontH+m_spacing)*n)/2; 
            break;
        case TOP:
            x = (_gr_nScreenWidth - maxStrLen)/2; 
            y = indent;
            break;
        case BOTTOM:
            x = (_gr_nScreenWidth - maxStrLen)/2; 
            y = (_gr_nScreenHeight - (m_msgFontH+m_spacing)*n); 
            break;
        case NORMAL:
        case TOP_LEFT:     
            x = indent; 
            y = indent; 
            break;
        case TOP_RIGHT:    
            x = (_gr_nScreenWidth-maxStrLen); 
            y = indent; 
            break;
        case BOTTOM_LEFT:  
            x = indent; 
            y = (_gr_nScreenHeight - (m_msgFontH+m_spacing)*n); 
            break;
        case BOTTOM_RIGHT: 
            x = (_gr_nScreenWidth - maxStrLen); 
            y = (_gr_nScreenHeight - (m_msgFontH+m_spacing)*n); 
            break;
        default: s_ASSERTNQ("");
    }
    return(n);
}
//---------------------------------------------------
void GameConsole::Update(){
    if(m_dispMsgNum != 0)
    for(int i = m_dispMsgNum-1; i >= 0; --i)
        if(m_displayMsg[i].postTime+m_displayMsg[i].timeToDisplay < Session::m_moment)
            PreemptDisplayMessage(i);
    
    if(m_urgentMsg.text[0] != 0)
    if(m_urgentMsg.postTime+m_urgentMsg.timeToDisplay < Session::m_moment)
        m_urgentMsg.text[0] = 0;
}
//---------------------------------------------------
void GameConsole::PreemptDisplayMessage(int n){
    for(int i = n; i < m_dispMsgNum-1; ++i) m_displayMsg[i] = m_displayMsg[i+1];
    m_dispMsgNum--;
}
//---------------------------------------------------
void GameConsole::PrintEditStr() const{
   int scrX, scrY;
   int pLen, cLen, eLen;
   int cX;
   int scrLen, beg;
   int i;
   int len;
   char tmp[CON_MAX_INPUT_LEN];

   len  = strlen(m_editStr);
   scrX = 2 + m_spacing;
   scrY = m_h-m_conFontH+m_spacing;
   pLen = m_conFontI->StringWidth(m_prompt);
   cLen = (m_editStr[m_pos] != 0) ? m_conFontI->CharWidth(m_editStr[m_pos]) : m_conFontI->CharWidth(' ');
   eLen = m_len-pLen;

   for(i = 0, scrLen = 0; i <= m_pos; ++i) scrLen += m_conFontI->CharWidth(m_editStr[i]);
   for(beg = 0; scrLen > eLen; ++beg)      scrLen -= m_conFontI->CharWidth(m_editStr[beg]);
   if(beg == 0){
      for(i = m_pos; i < len && scrLen <= eLen; ++i) scrLen += m_conFontI->CharWidth(m_editStr[i]);
      strncpy(tmp, m_editStr, i);
      tmp[i] = 0;
   }
   else{
      if(m_pos == len){
         strncpy(tmp, m_editStr+beg, m_pos-beg);
         tmp[m_pos-beg] = 0;
      }
      else{
         strncpy(tmp, m_editStr+beg, m_pos-beg+1);
         tmp[m_pos-beg+1] = 0;
      }
   }
   for(i = 0, cX = scrX+pLen; i < Min((int)strlen(tmp), m_pos); ++i)
      cX += m_conFontI->CharWidth(tmp[i]);
   if(beg > 0 && m_pos != strlen(m_editStr)) cX -= cLen;

   m_conFontI->LUPrintClipAt(scrX, scrY, m_prompt);
   m_conFontI->LUPrintClipAt(scrX+pLen, scrY, tmp);
   
   GRStartScene();
   DrawCursor(cX, scrY+(m_conFontI->Height()-1), cLen, 1);
   GREndScene();
}
//---------------------------------------------------
void GameConsole::DrawCursor(int x, int y, int len, int h) const{
    _gr_polygon.dwFullType = GR_POLY_TRANSPARENT;
    _gr_polygon.dwAddType  = 0;
    _gr_polygon.nVertices  = 4;
    _gr_polygon.dwOpacity  = 128;
    _gr_polygon.dwColor.color = m_curColor;
    _gr_polygon.nLights    = 0;
    _gr_vertices[0].any.x = x-_gr_nScreenOriginX;
    _gr_vertices[0].any.y = y-_gr_nScreenOriginY;
    _gr_vertices[0].any.iz = 65535;
    _gr_vertices[1].any.x = x-_gr_nScreenOriginX;
    _gr_vertices[1].any.y = y-_gr_nScreenOriginY+h;
    _gr_vertices[1].any.iz = 65535;
    _gr_vertices[2].any.x = x-_gr_nScreenOriginX+len;
    _gr_vertices[2].any.y = y-_gr_nScreenOriginY+h;
    _gr_vertices[2].any.iz = 65535;
    _gr_vertices[3].any.x = x-_gr_nScreenOriginX+len;
    _gr_vertices[3].any.y = y-_gr_nScreenOriginY;
    _gr_vertices[3].any.iz = 65535;
    GRDrawPolygonPCCW();
}
//---------------------------------------------------
void GameConsole::Draw() {
    int i, index, msgToPrint, n;
    char tmp[CON_MAX_MESSAGE_LEN], *str, *prev;
    
    Update();
    GRZBufferEnable(FALSE);
    if(m_active){
        m_oldViewPort = GRGetViewport();
        GRSetViewport(m_viewPort);
        GRStartScene();
        DrawBkGround();
        GREndScene();

        msgToPrint = Min(m_conLines, m_msgNum);
        for(i = m_conLines-1; i >= m_conLines-msgToPrint; --i){
            index = (m_curMsg-1) - (m_conLines-i-1);
            if(index < 0)
                if(m_msgNum > m_curMsg-1) index += m_msgNum+1;
                else break;

            m_message[index].font->LUPrintClipAt(2, i*(m_message[index].font->Height()+m_spacing), m_message[index].text);
        }
        PrintEditStr();
        GRSetViewport(m_oldViewPort);
    }
    else{
        GREndScene();
        if(m_dispMsgNum > 0)
            for(i = m_dispMsgNum-1; i >= 0 ; --i){
                strcpy(tmp, m_displayMsg[i].text);
                prev = tmp;
                str  = strstr(tmp, "\\n");
                for(n = 0; str != NULL; ++n){
                    *str = 0;
                    str += 2;
                    m_displayMsg[i].font->LUPrintClipAt(m_displayMsg[i].x, m_displayMsg[i].y + n*(m_displayMsg[i].font->Height()+m_spacing), prev);
                    prev = str;
                    str  = strstr(str, "\\n");
                }
                m_displayMsg[i].font->LUPrintClipAt(m_displayMsg[i].x, m_displayMsg[i].y + n*(m_displayMsg[i].font->Height()+m_spacing), prev);
            }
        if(m_urgentMsg.text[0] != 0){
            if(m_urgentMsg.color != -1){
                GRStartScene();
                VRectangle(m_urgentMsg.x, 
                           m_urgentMsg.y, 
                           m_urgentMsg.maxStrLen, 
                           m_urgentMsg.linesNum*(m_urgentMsg.font->Height()+m_spacing), 
                           m_urgentMsg.color, TRUE, 128, 1);
                GREndScene();
            }
            strcpy(tmp, m_urgentMsg.text);
            prev = tmp;
            str  = strstr(tmp, "\\n");
            for(n = 0; str != NULL; ++n){
                *str = 0;
                str += 2;
                m_urgentMsg.font->LUPrintClipAt(m_urgentMsg.x, m_urgentMsg.y + n*(m_urgentMsg.font->Height()+m_spacing), prev);
                prev = str;
                str  = strstr(str, "\\n");
            }
            m_urgentMsg.font->LUPrintClipAt(m_urgentMsg.x, m_urgentMsg.y + n*(m_urgentMsg.font->Height()+m_spacing), prev);
        }
        if(m_frameMsg.text[0] != 0){
            strcpy(tmp, m_frameMsg.text);
            prev = tmp;
            str  = strstr(tmp, "\\n");
            for(n = 0; str != NULL; ++n){
                *str = 0;
                str += 2;
                m_frameMsg.font->LUPrintClipAt(m_frameMsg.x, m_frameMsg.y + n*(m_frameMsg.font->Height()+m_spacing), prev);
                prev = str;
                str  = strstr(str, "\\n");
            }
            m_frameMsg.font->LUPrintClipAt(m_frameMsg.x, m_frameMsg.y + n*(m_frameMsg.font->Height()+m_spacing), prev);
            m_frameMsg.text[0] = 0;
        }
    }
    GRDumpScreen();
    GRZBufferEnable(TRUE);
}
//---------------------------------------------------------
// Function scan all console memory and remove EMPTY strings.
void GameConsole::RemoveEmptyMemory(){
   for(int i = 0; i < m_memItemsNum; ++i)
      if(strlen(m_mem[i]) == 0) PreemptMemory(i);
}
//---------------------------------------------------------
int GameConsole::IsSeparator(char ch) const{
   const char sep[] = " ,.;-+";

   for(int i = 0; i < strlen(sep); ++i) if(ch == sep[i]) return(1);

   return(0);
}
//---------------------------------------------------------
void GameConsole::WordLeft(const char *str){
   if(m_pos == 0) return;
   do { --m_pos; } while(m_pos > 0 &&  IsSeparator(str[m_pos]));
   do { --m_pos; } while(m_pos > 0 && !IsSeparator(str[m_pos]));
   while(IsSeparator(str[m_pos])) ++m_pos;
}
//---------------------------------------------------------
void GameConsole::WordRight(const char *str){
   int len = strlen(str);

   if(m_pos == len) return;
   while(m_pos < len && !IsSeparator(str[m_pos])) ++m_pos;
   while(m_pos < len &&  IsSeparator(str[m_pos])) ++m_pos;
}
//---------------------------------------------------------
// Insert given ch into str at pos.
void GameConsole::InsertCharAt(char *str, int pos, char ch) const{
   ASSERT(str != NULL);
   ASSERT(pos >= 0);
   ASSERT(pos < CON_MAX_INPUT_LEN);

   int len = strlen(str);
   if(pos == len){           // Add to end of string.
      str[len]   = ch;
      str[len+1] = 0;
   }
   else{
      for(int i = len; i >= pos; --i) str[i+1] = str[i];
      str[pos] = ch;
   }
}
//---------------------------------------------------------
// Delete char from str at pos.
void GameConsole::DeleteCharAt(char *str, int pos) const{
   ASSERT(str != NULL);
   ASSERT(pos >= 0);
   ASSERT(pos < CON_MAX_INPUT_LEN);

   int len = strlen(str);
   if(pos == len) str[len] = 0;    // Delete to end of string.
   else for(int i = pos; i <= len; ++i) str[i] = str[i+1];
}
//---------------------------------------------------------
int GameConsole::IsLetter(int code) const{
    return((isprint(code) || ispunct(code) || code == ' ') ? TRUE : FALSE);
}
//---------------------------------------------------------
// Emptyes console memory.
void GameConsole::ClearMem(){
   for(int i = 0; i < CON_MAX_MEMORY; ++i) m_mem[i][0] = 0;
   m_memItemsNum = 0;
}
//---------------------------------------------------------
// Preempts string number n.
void GameConsole::PreemptMemory(int n){
   for(int i = n; i < m_memItemsNum; ++i) strcpy(m_mem[i], m_mem[i+1]);
   m_memItemsNum--;
}
//---------------------------------------------------------
// Add new string to console memory.
void GameConsole::AddMemStr(const char *str){
   ASSERT(strlen(str) < CON_MAX_INPUT_LEN);

   if(m_memItemsNum == CON_MAX_MEMORY) PreemptMemory(0);
   strcpy(m_mem[m_memItemsNum], str);
   m_memItemsNum++;
   m_curMemStr = m_memItemsNum;
}

