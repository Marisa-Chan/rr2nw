#ifndef __CINTER_H__
#define __CINTER_H__

#ifndef __LINKEX_H__
#include "linkex.h"
#endif

#ifndef __LANGER_H__
#include "langer.h"
#endif

 /*
  * Линковка .SC программы.
  * Проставляются в коде адреса внутренних и внешних функций,
  * в constExternLinkTable заносятся смещения констант в стеке.
  */
void ci_LinkProgramm( os_TOutStream	  *linkInfo,
					  os_TOutStream   *cs,
					  TInt             linkList,
					  TError          *error,
					  TLinkExtern      externLinkTable[],
					  TLinkConstExtern constExternLinkTable[] );

 /*
  * Инициализация процесса перед запуском.
  * Дополнительно вызываются функции из constExternLinkTable для
  * инициализации констант.
  */
void ci_InitTProcessContext( TProcessContext *pc,
							 TStackCell      *stack,
							 int              stackSize,
							 int              quants,
							 os_TOutStream   *cs,
							 TInt             entry,
                             TLinkConstExtern constExternLinkTable[] );

TInt ci_GetFuncPtr( os_TOutStream *cs, const char *fname, int *useStack );
int  ci_RunProcess( TProcessContext *pc, int clockCnt, void *arena );


#endif
