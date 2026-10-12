#include "hardware.h"
#include "menu.h"

#define MAX_OLE_EVENT 8
#define MAXPATH 256

//===========================================================================
void s_SetCTRL( TProcessContext *pc, void *a ){
    ct_Arena &storage  = *((ct_Arena*)(a));
    const char *act = (char *)&(pc->m_codePtr[SC_PARI(1)]);
    const char *key = (char *)&(pc->m_codePtr[SC_PARI(0)]);
    int actNum  = g_hardware.SearchAction(act);
    int keyCode = g_hardware.SearchCode(key);
    KR_Event event;

    event.timeStamp   = 0.1;
    event.label       = CTRL_SET_CONTROL;
	event.source	  = storage.getObjectID();
	event.destination = g_hardware.getObjectID();
	event.data.open(EDO_WRITE)
  			     .putInt(actNum)
				 .putInt(keyCode)
			  .close();
	storage.getContext()->sendEventNow(event);
}
//===========================================================================
void s_LinkCTRL( TProcessContext *, void *a){
	ct_Arena &storage  = *((ct_Arena*)(a));
    KR_Event event;

    event.timeStamp   = 0.1;
    event.label       = CTRL_LINK_CONTROLS;
	event.source	  = storage.getObjectID();
	event.destination = g_hardware.getObjectID();
	storage.getContext()->sendEventNow(event);
}
//===========================================================================
void s_SetHardware(TProcessContext *pc, void *a){
    ct_Arena &storage  = *((ct_Arena*)(a));
    int j = SC_PARI(0);
	int m = SC_PARI(1);
	int k = SC_PARI(2);
	KR_Event event;

    event.timeStamp   = 0.1;
    event.label       = CTRL_SET_HARDWARE;
	event.source	  = storage.getObjectID();
	event.destination = g_hardware.getObjectID();
	event.data.open(EDO_WRITE)
				 .putInt(k)
				 .putInt(m)
				 .putInt(j)
			  .close();
	storage.getContext()->sendEventNow(event);
}
//===========================================================================
void s_EnableHardware(TProcessContext *pc, void *a){
    ct_Arena &storage  = *((ct_Arena*)(a));
    int j = SC_PARI(0);
	int m = SC_PARI(1);
	int k = SC_PARI(2);
	KR_Event event;

    event.timeStamp   = 0.1;
    event.label       = CTRL_ENABLE_HARDWARE;
	event.source	  = storage.getObjectID();
	event.destination = g_hardware.getObjectID();
	event.data.open(EDO_WRITE)
				 .putInt(k)
				 .putInt(m)
				 .putInt(j)
			  .close();
	storage.getContext()->sendEventNow(event);
}
//===========================================================================
void s_LoadRoute( TProcessContext *pc, void *a )
 {
    ct_Arena &storage  = *((ct_Arena*)(a));

    const char *fname = (char *)&(pc->m_codePtr[SC_PARI(1)]);
    const char *rname = (char *)&(pc->m_codePtr[SC_PARI(0)]);

    KR_ObjectID oID(storage.newObject( SC_PARI(2), rname ));
    KR_Event event;
    event.label       = ROUTE_LOAD;
    event.source      = storage.getObjectID();
    event.destination = oID;
    event.timeStamp   = 0.1;
    event.data.open(EDO_WRITE)
                .putStr(fname)
              .close();
    storage.getContext()->sendEventNow(event);
 }
//-------------------------------------------------------
void s_LoadFont(TProcessContext *pc, void *a){
    FixedFontOBJ *fontOBJ;
    ct_Arena &storage  = *((ct_Arena*)(a));

    //char name[MAXPATH+10];

    fontOBJ = new FixedFontOBJ((char *)&(pc->m_codePtr[SC_PARI(1)]));
    //strcpy(name, "Font.");
    //strcat(name, (char *)&(pc->m_codePtr[SC_PARI(0)]));
    const char *name = (char *)&(pc->m_codePtr[SC_PARI(0)]);
    storage.getContext()->addObject(name, fontOBJ);
}
//-------------------------------------------------------
void s_CreateTransparetColor(TProcessContext *pc, void *){
	int b = SC_PARI(0);
	int g = SC_PARI(1);
	int r = SC_PARI(2);
	
	SC_PARI(3) = GRTransparentColor(r, g, b);
}
//-------------------------------------------------------
void s_LoadImage(TProcessContext *pc, void *a){
    ImageOBJ *imageOBJ;
    ct_Arena &storage  = *((ct_Arena*)(a));
    char name[MAXPATH+10];

    imageOBJ = new ImageOBJ((char *)&(pc->m_codePtr[SC_PARI(0)]));
    strcpy(name, "Image.");
    strcat(name, (char *)&(pc->m_codePtr[SC_PARI(0)]));
    storage.getContext()->addObject(name, imageOBJ);
}

//===========================================================================
void s_AddMenuText(TProcessContext *pc, void *a){
    ct_Arena &storage  = *((ct_Arena*)(a));
	
	const char *l1Text	  = (char *)&(pc->m_codePtr[SC_PARI(0)]);
	const char *l0Text	  = (char *)&(pc->m_codePtr[SC_PARI(1)]);
	const char *aFontName = (char *)&(pc->m_codePtr[SC_PARI(2)]);
	const char *pFontName = (char *)&(pc->m_codePtr[SC_PARI(3)]);
	int afOpasity		  = SC_PARI(4);
	int afColor			  = SC_PARI(5);
	int abOpasity		  = SC_PARI(6);
	int abColor			  = SC_PARI(7);
	int pOpasity		  = SC_PARI(8);
	int pColor			  = SC_PARI(9);
	int sideW			  = SC_PARI(10);
	int pVertSpace		  = SC_PARI(11);
	int el				  = SC_PARI(12);
	int cp				  = SC_PARI(13);
	int id				  = SC_PARI(14);
	const char *name 	  = (char *)&(pc->m_codePtr[SC_PARI(15)]);
	const char *path	  = (char *)&(pc->m_codePtr[SC_PARI(16)]);

	AddMenuText(*storage.getContext(), path, name, KR_ObjectID(id, cp), el, pVertSpace, sideW, pColor, pOpasity, abColor, abOpasity, afColor, afOpasity, pFontName, aFontName, l0Text, l1Text);
}
//-------------------------------------------------------
void s_AddMenuInput(TProcessContext *pc, void *a){
    ct_Arena &storage  = *((ct_Arena*)(a));
	
	const char *defText	  = (char *)&(pc->m_codePtr[SC_PARI(0)]);
	const char *aFontName = (char *)&(pc->m_codePtr[SC_PARI(1)]);
	const char *pFontName = (char *)&(pc->m_codePtr[SC_PARI(2)]);
	int afOpasity		  = SC_PARI(3);
	int afColor			  = SC_PARI(4);
	int abOpasity		  = SC_PARI(5);
	int abColor			  = SC_PARI(6);
	int pOpasity		  = SC_PARI(7);
	int pColor			  = SC_PARI(8);
	int sideW			  = SC_PARI(9);
	int pVertSpace		  = SC_PARI(10);
	int el				  = SC_PARI(11);
	int cp				  = SC_PARI(12);
	int id				  = SC_PARI(13);
	const char *name 	  = (char *)&(pc->m_codePtr[SC_PARI(14)]);
	const char *path	  = (char *)&(pc->m_codePtr[SC_PARI(15)]);

	AddMenuInput(*storage.getContext(), path, name, KR_ObjectID(id, cp), el, pVertSpace, sideW, pColor, pOpasity, abColor, abOpasity, afColor, afOpasity, pFontName, aFontName, defText);
}
//-------------------------------------------------------
void s_AddMenuScroll(TProcessContext *pc, void *a){
    ct_Arena &storage  = *((ct_Arena*)(a));
	
	double def			  = SC_PARF(0);
	double step			  = SC_PARF(1);
	double max			  = SC_PARF(2);
	double min			  = SC_PARF(3);
	const char *l1Text	  = (char *)&(pc->m_codePtr[SC_PARI(4)]);
	const char *l0Text	  = (char *)&(pc->m_codePtr[SC_PARI(5)]);
    const char *attrName  = (char *)&(pc->m_codePtr[SC_PARI(6)]);
	const char *aFontName = (char *)&(pc->m_codePtr[SC_PARI(7)]);
	const char *pFontName = (char *)&(pc->m_codePtr[SC_PARI(8)]);
	int afOpasity		  = SC_PARI(9);
	int afColor			  = SC_PARI(10);
	int abOpasity		  = SC_PARI(11);
	int abColor			  = SC_PARI(12);
	int pOpasity		  = SC_PARI(13);
	int pColor			  = SC_PARI(14);
	int sideW			  = SC_PARI(15);
	int pVertSpace		  = SC_PARI(16);
	int el				  = SC_PARI(17);
	int cp				  = SC_PARI(18);
	int id				  = SC_PARI(19);
	const char *name 	  = (char *)&(pc->m_codePtr[SC_PARI(20)]);
	const char *path	  = (char *)&(pc->m_codePtr[SC_PARI(21)]);

	AddMenuScroll(*storage.getContext(), path, name, KR_ObjectID(id, cp), el, pVertSpace, sideW, pColor, pOpasity, abColor, abOpasity, afColor, afOpasity, pFontName, aFontName, attrName, l0Text, l1Text, min, max, step, def);
}
//-------------------------------------------------------
void s_AddMenuList(TProcessContext *pc, void *a){
    ct_Arena &storage  = *((ct_Arena*)(a));
	
	int immediate         = SC_PARI(0);
    const char *l1Text	  = (char *)&(pc->m_codePtr[SC_PARI(1)]);
	const char *l0Text	  = (char *)&(pc->m_codePtr[SC_PARI(2)]);
    const char *attrName  = (char *)&(pc->m_codePtr[SC_PARI(3)]);
	const char *aFontName = (char *)&(pc->m_codePtr[SC_PARI(4)]);
	const char *pFontName = (char *)&(pc->m_codePtr[SC_PARI(5)]);
	int afOpasity		  = SC_PARI(6);
	int afColor			  = SC_PARI(7);
	int abOpasity		  = SC_PARI(8);
	int abColor			  = SC_PARI(9);
	int pOpasity		  = SC_PARI(10);
	int pColor			  = SC_PARI(11);
	int sideW			  = SC_PARI(12);
	int pVertSpace		  = SC_PARI(13);
	int el				  = SC_PARI(14);
	int cp				  = SC_PARI(15);
	int id				  = SC_PARI(16);
	const char *name 	  = (char *)&(pc->m_codePtr[SC_PARI(17)]);
	const char *path	  = (char *)&(pc->m_codePtr[SC_PARI(18)]);
     
	AddMenuList(*storage.getContext(), path, name, KR_ObjectID(id, cp), el, pVertSpace, sideW, pColor, pOpasity, abColor, abOpasity, afColor, afOpasity, pFontName, aFontName, attrName, l0Text, l1Text, immediate);
}
//-------------------------------------------------------
void s_AddMenuListItem(TProcessContext *pc, void *a){
    ct_Arena &storage  = *((ct_Arena*)(a));
	
	const char *str	 = (char *)&(pc->m_codePtr[SC_PARI(0)]);
	const char *path = (char *)&(pc->m_codePtr[SC_PARI(1)]);

	AddMenuListItem(path, str);
}
//-------------------------------------------------------
void s_AddMenuSetup(TProcessContext *pc, void *a){
    ct_Arena &storage  = *((ct_Arena*)(a));
	
	const char *l1Text	  = (char *)&(pc->m_codePtr[SC_PARI(0)]);
	const char *l0Text	  = (char *)&(pc->m_codePtr[SC_PARI(1)]);
	const char *aFontName = (char *)&(pc->m_codePtr[SC_PARI(2)]);
	const char *pFontName = (char *)&(pc->m_codePtr[SC_PARI(3)]);
	int afOpasity		  = SC_PARI(4);
	int afColor			  = SC_PARI(5);
	int abOpasity		  = SC_PARI(6);
	int abColor			  = SC_PARI(7);
	int pOpasity		  = SC_PARI(8);
	int pColor			  = SC_PARI(9);
	int sideW			  = SC_PARI(10);
	int pVertSpace		  = SC_PARI(11);
	int el				  = SC_PARI(12);
	int cp				  = SC_PARI(13);
	int id				  = SC_PARI(14);
	const char *name 	  = (char *)&(pc->m_codePtr[SC_PARI(15)]);
	const char *path	  = (char *)&(pc->m_codePtr[SC_PARI(16)]);

	AddMenuSetup(*storage.getContext(), path, name, KR_ObjectID(id, cp), el, pVertSpace, sideW, pColor, pOpasity, abColor, abOpasity, afColor, afOpasity, pFontName, aFontName, l0Text, l1Text);
}
//-------------------------------------------------------
void s_AddMenuSetupLine(TProcessContext *pc, void *a){
    ct_Arena &storage  = *((ct_Arena*)(a));

    const char *ctrlName  = (char *)&(pc->m_codePtr[SC_PARI(0)]);
    const char *name      = (char *)&(pc->m_codePtr[SC_PARI(1)]);

	AddMenuSetupLine(*storage.getContext(), name, ctrlName);
}
