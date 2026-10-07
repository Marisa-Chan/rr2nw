#include <dos.h>
#include <stdio.h>
#include "sobject.h"
#include "ctrlbrk.h"
#include <process.h>

#ifdef __BORLANDC__


#  ifdef __cplusplus
	#define __CPPARGS ...
#  else
	#define __CPPARGS
#  endif


  void interrupt ( *oldhandler)(__CPPARGS);


  void interrupt CtrlBreakHandler(__CPPARGS)
    {
	//CObject_::m_cancel = 1;
	*((unsigned char *)(0x471ul)) = 0;
	exit(0);
    }

  void SetControlBreak()
   {
	setvect(0x1b,CtrlBreakHandler);
   }

#else
#  ifdef __WATCOMC__
void NewHRestoreTextMode();
#pragma aux NewHRestoreTextMode = \
	"mov ax,3"	\
	"int 10h" modify [eax]

  extern void Exit1B(int *ptr,int pos);
  void Exit1B(int *ptr,int pos)
   {
   	NewHRestoreTextMode();
   	ptr[pos] = pos|5;
   }

  void __interrupt __far CtrlBreakHandler()
    {
	//CObject_::m_cancel = 1;
	*((unsigned char *)(0x471ul)) = 0;
	//Exit1B((int*)(-1),0);
    }

  void SetControlBreak()
   {
    //_dos_setvect(0x1b,CtrlBreakHandler);
   }
#  else
  void SetControlBreak()
   {
   }

#  endif
#endif

