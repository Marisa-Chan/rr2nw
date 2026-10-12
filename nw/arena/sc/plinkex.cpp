#include <stdio.h>
#include "sc/h/linkex.h"
#include "storage/h/subject.h"
#include "kernel/h/context.h"
#include "kernel/h/session.h"

#include "message/groupmsg.h"
#include "message/comanmsg.h"
#include "message/routmsg.h"
#include "message/attrmsg.h"
#include "message/skinmsg.h"
#include "message/sparkmsg.h"
#include "message/fountmsg.h"
#include "message/bimsg.h"
#include "message/peopmsg.h"
#include "message/fartmsg.h"
#include "message/vehicleMsg.h"
#include "message/lampMsg.h"
#include "message/levelmsg.h"

#include "mproj/h/mproj.h"
#include "kernel/h/echo.h"
#include "hardware.h"
#include "menu.h"
#include "font.h"
#include "image.h"
#include "message/menumsg.h"

#include "sc/green_func.h"
#include "sc/pin_func.h" 

/**********************************
  *
***       Mission Project
  *
  *********************************/

 //===========================================================================
void s_CreateProjectTable( TProcessContext *pc, void *a )
 {
    ct_Arena &storage  = *((ct_Arena*)(a));
    int mpQnty   = SC_PARI(2),
        treeSize = SC_PARI(1),
        heapSize = SC_PARI(0);

    projectTable.create(
                        mpQnty,
                        storage.context,
                        storage,
                        treeSize,
                        heapSize
                       );
 }

 //===========================================================================
void s_NewPNode( TProcessContext *pc, void *a )
 {
    (void)a;
    SC_PARI(3) = mp_New( projectTable, SC_PARI(2), SC_PARI(1), SC_PARI(0) );
 }

 //===========================================================================
void s_OpenProjectData( TProcessContext *pc, void *a )
 {
    (void)a;
    mp_OpenData( projectTable, SC_PARI(0), EDO_WRITE );
 }

 //===========================================================================
void s_CloseProjectData( TProcessContext *pc, void *a )
 {
    (void)a;
    mp_CloseData( projectTable, SC_PARI(0) );
 }

 //===========================================================================
void s_ProjectWriteInt( TProcessContext *pc, void *a )
 {
    (void)a;
    mp_WriteInt( projectTable, SC_PARI(1), SC_PARI(0) );
 }

 //===========================================================================
void s_ProjectWriteFloat( TProcessContext *pc, void *a )
 {
    (void)a;
    mp_WriteFloat( projectTable, SC_PARI(1), SC_PARF(0) );
 }

 //===========================================================================
void s_ProjectWriteStr( TProcessContext *pc, void *a )
 {
    (void)a;
    const char *st = (char *)&(pc->m_codePtr[SC_PARI(0)]);
    mp_WriteStr( projectTable, SC_PARI(1), st );
 }

 //===========================================================================
void s_ProjectNodeSetLink( TProcessContext *pc, void *a )
 {
    (void)a;
    mp_SetLink(
                projectTable,
                SC_PARI(2),
                SC_PARI(1),
                SC_PARI(0)
              );
 }


 //===========================================================================
void s_NodeNULL( TProcessContext *pc, void *a )
 {
    (void)a;
    SC_PARI(0) =  mp_NodeNULL();
 }



#include "sc/suavik_func.h"
 /**********************************************
  *
***          External constant
  *
  **********************************************/

#include "sc/suavik_const.h"
#include "sc/green_const.h"
#include "sc/pin_const.h"


TLinkConstExtern constExternLinkTable[] = {
 {"START_FARTING",           s_Const_START_FARTING,   0},
#include "sc/green_consttab.h"
#include "sc/suavik_consttab.h"
#include "sc/pin_consttab.h"
 { NULL   , NULL, 0 }
};


TLinkExtern externLinkTable[] = {
#include "sc/green_functab.h"
#include "sc/suavik_functab.h"
#include "sc/pin_functab.h"
  {NULL,NULL}
};
