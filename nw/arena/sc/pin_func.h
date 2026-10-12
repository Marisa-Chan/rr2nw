#include "i/commander.i"
#include "debugext.h"
#include "../arena/h/super.h"
#include "briefing.h"

 //===========================================================================
void s_NewProjectEx( TProcessContext *pc, void *a )
 {
    (void)a;
    int	 permanent = SC_PARI(0);
    int  node      = SC_PARI(1);
    const char *st = (char *)&(pc->m_codePtr[SC_PARI(2)]);
    
    projectTable.newProject( st, node , permanent);
 }



void s_SetHostileCommander( TProcessContext *pc, void *a )
 {    
    (void)a;

    KR_ObjectID commander (SC_PARI(3),SC_PARI(2));
    KR_ObjectID relativeCommander (SC_PARI(1),SC_PARI(0));

    //ct_Arena &storage = *((ct_Arena*)(a));

    ICommander *uobj=(ICommander*)(g_arena.getContext()->queryInterface(commander,ICommanderIID));
    VERIFYMSG(uobj,"Cannot find commander");
    uobj->setHostile(relativeCommander);

    uobj=(ICommander*)(g_arena.getContext()->queryInterface(relativeCommander,ICommanderIID));
    VERIFYMSG(uobj,"Cannot find commander");
    uobj->setHostile(commander);
}

void s_SetFriendlyCommander( TProcessContext *pc, void *a )
 {    
    (void)a;
                    
    KR_ObjectID commander (SC_PARI(3),SC_PARI(2));
    KR_ObjectID relativeCommander (SC_PARI(1),SC_PARI(0));

    //ct_Arena &storage = *((ct_Arena*)(a));

    ICommander *uobj=(ICommander*)(g_arena.getContext()->queryInterface(commander,ICommanderIID));
    VERIFYMSG(uobj,"Cannot find commander");
    uobj->setHostile(relativeCommander);

    uobj=(ICommander*)(g_arena.getContext()->queryInterface(relativeCommander,ICommanderIID));
    VERIFYMSG(uobj,"Cannot find commander");
    uobj->setHostile(commander);
}


void s_DeleteHowitzer( TProcessContext *pc, void *a )
{
   (void)a;
   const char *name   = SC_PARS(0);  

   g_super.m_level.AttachToHowitzerHolder(name, KR_ObjectID::NUL());
}
 //===========================================================================
void s_RemoveObject( TProcessContext *pc, void *a )
 {
    (void)a;
    ct_Arena &storage = *((ct_Arena*)(a));
    const char *name  = SC_PARS(1);
    KR_ObjectID oID( storage.getContext()->searchObject(name) );
    storage.delObject( oID, SC_PARF(0) + Session::m_moment );
 }

 //===========================================================================
void s_ForceRemoveObject( TProcessContext *pc, void *a )
 {
    (void)a;
    ct_Arena &storage = *((ct_Arena*)(a));
    const char *name  = SC_PARS(0);
    KR_ObjectID oID( storage.getContext()->searchObject(name) );
    storage.delObject( oID );
 }

 //===========================================================================
void s_SearchObjectIDNoWarning( TProcessContext *pc, void *a )
 {
    (void)a;
    ct_Arena &storage = *((ct_Arena*)(a));
    const char *name  = SC_PARS(0);

    KR_ObjectID oID = storage.getContext()->isExist(name)
                      ? storage.getContext()->searchObject(name)
                      : KR_ObjectID::NUL();
    pc->m_stack[SC_PARI(1)].i = oID.getCachePos();
    pc->m_stack[SC_PARI(2)].i = oID.id;
 }

 /*
    Globals of the level-restart machinery (restored from nw.exe).
    s_RestartLevel stores RESTART_REQUEST into g_RestartRequest,
    the main loop picks it up and reinitializes the level.
  */
enum { RESTART_REQUEST = 3 };
int g_RestartRequest = 0;
int g_StartLevel     = 1;
int g_LevelParams[11];

 //===========================================================================
void s_RestartLevel( TProcessContext *pc, void *a )
 {
    (void)a;
    int level = SC_PARI(0);

    g_RestartRequest = RESTART_REQUEST;

    if( level < 0 ) level = 0;
    else if( level > 7 ) level = 7;

    if( (g_StartLevel == 1) && (level == 2) )
    {
         g_LevelParams[0] = g_LevelParams[6];
         g_LevelParams[2] = g_LevelParams[8];
         g_LevelParams[3] = g_LevelParams[9];
         g_LevelParams[4] = g_LevelParams[10];
    }

    g_StartLevel = level;
 }

 //===========================================================================
/*  Returns the total briefing time for the script. The full dry-run
    computation lives in CBriefing; here we just fetch the prepared
    value after asking the briefing object to parse the file.        */
void s_GetBriefingTime( TProcessContext *pc, void *a )
 {
    (void)a;
    const char *name = SC_PARS(0);
    SC_PARF(0) = g_briefing.GetBriefingTime(name);
 }
