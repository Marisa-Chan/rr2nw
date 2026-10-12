#include "kernel/h/echo.h"
#include "kernel/h/session.h"
#include "h/olevel.h"
#include "sc/h/linkex.h"
#include  "hardware.h"
#include "suavik.h"
#include <windows.h>
#include "h/cnststr.h"
#include "mproj/h/mproj.h"
#include "../output/defs.h"
#include "message/levelmsg.h"
#include "menu.h"

#define LAST_H__SCENE
#include "game.h"
#include "zav.h"


int	ol_Level::m_levelNumber=0;
int	ol_Level::m_levelToLoad=0;
int 	ol_Level::m_saveGameStatus=0;
char	ol_Level::m_saveGameName[200];


#define MAX_LEVEL_QNTY 1

struct  {
    const char *fileName;
    const char *includePath;
 } ol_levelConfigure[MAX_LEVEL_QNTY] =
 {
    {"..\\LEVEL0.SC",""}
 };

AttributeLevel g_levelAttr;

static const char saveCfgFileName[] = "../saves.cfg", emptySlotText[] = "Empty";
//--------------------------------------------------------
void ol_Level::PrepareSaveLoadMenu(const char *flp) {
    CConfigFile cfg(saveCfgFileName);
    MenuIterface_ *itemI;
    MenuItem_ *item;
    char path[200], fileSlot[100];
    const char *fileName;
    int i;

    for(i = 0; i < SAVELOAD_SLOTS_NUM; ++i){
        sprintf(path, "/Game/%s/Slot%i", flp, i);
        sprintf(fileSlot, "Save%i", i);
        itemI = (MenuIterface_ *)g_menu.SearchMenuItem(path);
        item  = (MenuItem_ *)g_menu.SearchMenuItem(path);
        if(itemI == NULL){
            echo("ol_Level::PrepareSaveMenu() : No menu save item %i", i);
            continue;
        }
        m_saveSlotUsed [i] = cfg.GetInt(fileSlot, "Used",  0);
        m_saveSlotLevel[i] = cfg.GetInt(fileSlot, "Level", 0);
        fileName           = m_saveSlotUsed[i] ? cfg(fileSlot, "FileName") : emptySlotText;
        if(fileName != NULL) {
            itemI->SetStr(fileName);
            item->SetLocale();
            item->CalcSelfCoords();
        }
        else echo("ol_Level::PrepareSaveMenu() : fileName == NULL");
    }
}
//--------------------------------------------------------
void ol_Level::DumpSaveLoadCfgFile(const char *flp) const{
    FILE *fh = fopen(saveCfgFileName, "wt");
    MenuIterface_ *itemI;
    char path[200];
    int i;
    
    if(fh == NULL){
        echo("ol_Level::DumpSaveLoadCfgFile() : Error writing %s", saveCfgFileName);
        return;
    }
    for(i = 0; i < SAVELOAD_SLOTS_NUM; ++i){
        sprintf(path, "/Game/%s/Slot%i", flp, i);
        itemI = (MenuIterface_ *)g_menu.SearchMenuItem(path);
        if(itemI == NULL){
            echo("ol_Level::DumpSaveLoadCfgFile() : No menu save item %i", i);
            continue;
        }
        fprintf(fh, "[Save%i]\n", i);
        fprintf(fh, "Used=%i\n", m_saveSlotUsed [i]);
        fprintf(fh, "Level=%i\n", m_saveSlotLevel[i]);
        fprintf(fh, "FileName=%s\n\n", m_saveSlotUsed[i] ? itemI->GetStr() : emptySlotText);	
    }
    fclose(fh);
}
//--------------------------------------------------------
int ol_Level::receiveEvent(KR_Event &event)
{
    int slotNumber;
    char path[200];
    MenuIterface_ *item;
    const char *fileName;

    switch( event.label )
    {
    	case lev_LOAD: 
		PrepareSaveLoadMenu("LoadGame");	
		break;

	case lev_SAVE: 
		PrepareSaveLoadMenu("SaveGame");	
		break;

        case lev_LOAD_SLOT0:
        case lev_LOAD_SLOT1:
        case lev_LOAD_SLOT2:
        case lev_LOAD_SLOT3:
        case lev_LOAD_SLOT4:
        case lev_LOAD_SLOT5:
        case lev_LOAD_SLOT6:
        case lev_LOAD_SLOT7:
			g_menu.Deactivate();
			m_saveGameStatus = MST_LOAD;
            slotNumber       = event.label-(int)lev_LOAD_SLOT0;
            s_ASSERT(slotNumber >= 0 && slotNumber < SAVELOAD_SLOTS_NUM, "");
            sprintf(path, "/Game/LoadGame/Slot%i", slotNumber);
            item = (MenuIterface_ *)g_menu.SearchMenuItem(path);
            s_ASSERT(item != NULL, "");
            
            fileName = item->GetStr(); // this is filename
	    sprintf(m_saveGameName,"..\\SAVES\\%s",fileName);
            //sprintf(m_saveGameName,"%s\\%s",GLOBAL_Config()("Init","SavePath"),fileName);
	    m_levelToLoad = m_saveSlotLevel[slotNumber];

            break;
        case lev_SAVE_SLOT0:
        case lev_SAVE_SLOT1:
        case lev_SAVE_SLOT2:
        case lev_SAVE_SLOT3:
        case lev_SAVE_SLOT4:
        case lev_SAVE_SLOT5:
        case lev_SAVE_SLOT6:
        case lev_SAVE_SLOT7:
	    g_menu.Deactivate();
	    m_saveGameStatus = MST_SAVE;
            slotNumber       = event.label-(int)lev_SAVE_SLOT0;
            s_ASSERT(slotNumber >= 0 && slotNumber < SAVELOAD_SLOTS_NUM, "");
            sprintf(path, "/Game/SaveGame/Slot%i", slotNumber);
            item = (MenuIterface_ *)g_menu.SearchMenuItem(path);
            s_ASSERT(item != NULL, "");
            
            fileName = item->GetStr(); // this is filename

	    sprintf(m_saveGameName,"..\\SAVES\\%s",fileName);
            /*sprintf(    m_saveGameName,"%s\\%s",
			GLOBAL_Config()("Init","SavePath"),
			fileName);*/

	    m_saveSlotUsed [slotNumber] = 1;
            m_saveSlotLevel[slotNumber] = m_levelNumber;
            
            DumpSaveLoadCfgFile("SaveGame");
            break;

	case lev_RESTART:
			{
			g_menu.Deactivate();
			m_saveGameStatus = MST_RESTART;
			m_levelNumber = g_levelAttr.m_reloadLevel;
			}
			break;


    case KR_WAKE_UP:	
        break;

    case lev_RELISE_PROJECT:
        {
        KR_ObjectID prjID;

        event.data.open(EDO_READ)
                     .getObjectID(prjID)
                  .close();
        reliseProject(projectTable.getProjectRoot(prjID));

        }
        break;

    default: 
        return 0;
    }

    return 1;
}

void ol_Level::ReleaseHolder(int index, KR_ObjectID occupant)
{	
	if (m_howitzerPool[index].occupant == occupant)
		m_howitzerPool[index].occupant = KR_ObjectID::NUL();
}

int	ol_Level::AttachToHowitzerHolder(int index, KR_ObjectID occupant)
{
	ASSERT(m_howitzerPool[index].occupant.isNUL());
	m_howitzerPool[index].occupant = occupant;
	return index;
}

//--------------------------------------------------------
int ol_Level::AttachToHowitzerHolder(const char * holderName, KR_ObjectID occupant)
{
	bool found = FALSE;

	for (int i = 0; i < m_howitzersLoaded; i++)
	{
		if (strcmp(holderName, m_howitzerPool[i].symbolic) == 0)
		{
			found = TRUE;

			if (!m_howitzerPool[i].occupant.isNUL())
				context->removeObject(m_howitzerPool[i].occupant);
			
			m_howitzerPool[i].occupant = occupant;

			return i;
		}

	} // for

	s_ASSERT(found,"Howitzer Holder Not Found");

	return -1;
}

//--------------------------------------------------------
bool ol_Level::ReadHowitzers()
{
    FILE *f = fopen( "Howitzers.hwz", "rb");
    
	char name[HOWITZER_MAX_NAME];
	
	CFVector3 pos;

    
	s_ASSERT(f != NULL, "Cannot open Howitzers.hwz");
    
	m_howitzersLoaded = 0;

	while (1) {
		
		int result = fscanf(f,"%s [%lf,%lf,%lf]",name, &pos.x,&pos.y,&pos.z);


		if ( result == 4)
		{			
			strcpy(m_howitzerPool[m_howitzersLoaded].symbolic,name);
			m_howitzerPool[m_howitzersLoaded].pos = pos;
			m_howitzerPool[m_howitzersLoaded].occupant = KR_ObjectID::NUL();

			m_howitzersLoaded++;
			if (m_howitzersLoaded == HOWITZER_MAX_LEVEL)
			{
				echo("Too many howitzerholders");
				break;
			}
		}
		 else
		if (result == EOF)
			break;

	} 
    fclose(f);

	return true;
}


//--------------------------------------------------------
void ol_Level::addNotify(){
	KR_Object::addNotify();
}
//--------------------------------------------------------
void ol_Level::removeNotify(){
    KR_Object::removeNotify();
}
//--------------------------------------------------------

int ol_Level::openLevel(
                         TSuaCript            &suacript,
                         ct_Arena             &storage,
                         int                   levelNum
                       )
 {
    TSetOfProcess sop;
    m_curLevel = -1;

	ReadHowitzers();

    if( levelNum >= MAX_LEVEL_QNTY  )
    {
         echo("ol_Level::openLevel: Level number too big\n");
         return 0;
    }

    str_ConstStr str;
    const char  *cfgStr;
    long         e = 0;
    int          scStackSize     = 200,
                 scWordBuffSize  = 1024*8,
                 scStringBuffSize= 1024*8,
                 scNameCnt       = 1024*2,
                 scTreeBuffSize  = 1024*64,
                 scCodeStreamSize= 1024*128,
                 scLinkInfoSize  = 1024*16,
                 scProgNameBuffSize=256,
                 scMaxProgrammCnt  = 1;

#define GETCFG(n,v) cfgStr = ZAV_Config()("Script",n); \
    if(  cfgStr!=NULL  )                               \
    {    str.assign(cfgStr);                           \
         if(  str.eval(e,0)  )                         \
              v = e;                                   \
    }

    GETCFG("stack",            scStackSize)
    GETCFG("wordBuffSize",     scWordBuffSize)
    GETCFG("stringBuffSize",   scStringBuffSize)
    GETCFG("nameCnt",          scNameCnt)
    GETCFG("treeBuffSize",     scTreeBuffSize)
    GETCFG("codeStreamSize",   scCodeStreamSize)
    GETCFG("linkInfoSize",     scLinkInfoSize)
    GETCFG("progNameBuffSize", scProgNameBuffSize)
    GETCFG("programmCnt",      scMaxProgrammCnt)



    if( !sc_InitTSetOfProcess( &sop,
                                scStackSize,   // stack
                                1 ) )  // process size
    {
         echo("ol_Level::openLevel: Can't initialize process\n");
         return 0;
    }


    if( !sc_InitTSuaCript( &suacript,
    /* word buff size */    scWordBuffSize,
    /*string buff size */   scStringBuffSize,
    /* name cnt        */   scNameCnt,
    /* tree buff size  */   scTreeBuffSize,
    /* code stream size */  scCodeStreamSize,
    /* link info size   */  scLinkInfoSize,
    /* prog name buff size*/scProgNameBuffSize,
    /* max programm cnt */  scMaxProgrammCnt ) )
    {
         echo("ol_Level::openLevel: Can't initialize suacript\n");
         return 0;
    }


    /*****************************
     *
     *    Load ads compile
     *
     *****************************/

    FILE *f = fopen( ol_levelConfigure[levelNum].fileName,"rb" );
    if( f==NULL )
    {
         echo("ol_Level::openLevel: Can't open file %s\n",
               ol_levelConfigure[levelNum].fileName);
         return 0;
    }
    sc_InitScannerFromFile( &suacript, f );
    lex_SetIncludePath    ( &suacript.m_scanner, 
                             ol_levelConfigure[levelNum].includePath
                          );
    sc_Compile(
               &suacript,
               ol_levelConfigure[levelNum].fileName,
               externLinkTable,
               constExternLinkTable
              );

    fclose(f);

    if( !sc_CreateProcess( &sop,
                           &suacript,
                           sc_ProgrammId(
                                         &suacript,
                                         ol_levelConfigure[levelNum].fileName
                                        ),
                           200,
                           32000,
                           constExternLinkTable ) )
    {
         echo("ol_Level::openLevel: Can't create process\n");
         return 0;
    }

    storage.context->start(0);
    while( !sc_RunProcess( &sop, 1, &storage ) );

    sc_DeleteTSuaCript( &suacript );


    m_curLevel = levelNum;
    return 1;
 }

void ol_Level::closeLevel()
 {
    m_curLevel = -1;
 }

void ol_Level::reliseProject(int prj)
{
  mp_NodeNum prjNUL = mp_NodeNULL(),
             cur;

  for( cur=prj; cur!=prjNUL; cur = projectTable.getLeft(cur))
  {
       switch(cur)
       {
       case COM_CREATE_UNITS:
                break;

       default: s_ASSERTNQ1("Unknown command %i",cur);
                break;
       }
  }

}
