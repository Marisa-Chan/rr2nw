#ifndef __VESASCR_H__
#define __VESASCR_H__

#ifndef __PMTOOLS_H__
#include "pmtools.h"
#endif

#define COPYTOSCR_MAX 7


typedef struct{
	int       m_kalLine;
	char*     m_linePTR;
	char*     m_tailPTR;
	int       m_bankNum;
	int       m_nextNum;
	int       m_okLen;
} VESA_lineInfoType;

typedef VESA_lineInfoType *VESA_lineInfoTypePTR;

#ifdef __VESA_C__
	#define GLOBAL
#else
	#define GLOBAL extern
#endif
extern "C" {
GLOBAL VESA_lineInfoTypePTR VESA_lineInfo;
GLOBAL int                  VESA_bank;
}

typedef void (*CopyToScrFunc)(int x1,int y,int x2,char *memory);
void CopyToScr0(int x1,int y,int x2,char *memory);
void CopyToScr1(int x1,int y,int x2,char *memory);
void CopyToScr2(int x1,int y,int x2,char *memory);
void CopyToScr3(int x1,int y,int x2,char *memory);
void CopyToScr4(int x1,int y,int x2,char *memory);
void CopyToScr5(int x1,int y,int x2,char *memory);
void CopyToScr6(int x1,int y,int x2,char *memory);
void CopyToScrVGA(int x1,int y,int x2,char *memory);

typedef void (*ClearScrFunc)(int x1,int y,int x2,char s);
void ClearScrVESA(int x1,int y,int x2,char s);
void ClearScrVGA(int x1,int y,int x2,char s);


#ifndef __VESASCR_C__
extern "C" ClearScrFunc  ClearScr;
extern "C" CopyToScrFunc CopyToScr;
extern "C" CopyToScrFunc CopyToScrChoise[COPYTOSCR_MAX];
#else
extern "C" ClearScrFunc  ClearScr=ClearScrVESA;
extern "C"
CopyToScrFunc CopyToScr = CopyToScr5;
extern "C" 
CopyToScrFunc CopyToScrChoise[COPYTOSCR_MAX] = 
{
	CopyToScr0,
	CopyToScr1,
	CopyToScr2,
	CopyToScr3,
	CopyToScr4,
	CopyToScr5,
	CopyToScr6
};
#endif

void Pset(int x,int y,int col);


extern "C" void MemCopy0(int cnt,void *sour,void *dest);
#pragma aux MemCopy0 parm [edi esi ecx] modify [edi esi ecx eax];

extern "C" void MemCopy1(int cnt,void *sour,void *dest);
#pragma aux MemCopy1 parm [edi esi ecx] modify [edi esi ecx eax];

extern "C" void MemCopy2(int cnt,void *sour,void *dest);
#pragma aux MemCopy2 parm [edi esi ecx] modify [edi esi ecx eax];

extern "C" void MemCopy3(int cnt,void *sour,void *dest);
#pragma aux MemCopy3 parm [edi esi ecx] modify [edi esi ecx eax];

extern "C" void MemCopy4(int cnt,void *sour,void *dest);
#pragma aux MemCopy4 parm [edi esi ecx] modify [edi esi ecx eax];

extern "C" void MemCopy5(int cnt,void *sour,void *dest);
#pragma aux MemCopy5 parm [edi esi ecx] modify [edi esi ecx eax];




extern "C" char * MemStoreB(unsigned char col,int cnt,char *dest);
#pragma aux MemStoreB parm [eax ecx edi] modify [edi ecx edx] value [eax];


inline void SwitchBank(int bank)
 {

	if(bank!=VESA_bank)
	{
		DPMI::RMREGS reg;
		
		reg.w.ax = 0x4F05;
		reg.w.bx = 0;
		reg.w.dx = bank;
		DPMI::RMIntr(0x10,reg);
		bank = VESA_bank;
	}
 }
#undef GLOBAL
#endif

