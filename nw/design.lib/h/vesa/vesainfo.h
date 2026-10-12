#ifndef __VESAINFO_H__
#define __VESAINFO_H__

#include "vesa\vesa_t.h"

#ifdef __VESA_C__
	#define GLOBAL
#else
	#define GLOBAL extern
#endif
extern "C" {
GLOBAL VgaInfoBlock    VESA_info;
GLOBAL ModeInfoBlock   MODE_info;
}

#undef GLOBAL

int SetGraphMode(int width,int height);

#endif /* __VESAINFO_H__ */
