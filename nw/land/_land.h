//
// Объявления глобальных пеpеменных
//
#ifndef ___LAND_H__
        #define ___LAND_H__

#include "land_t.h"
#include "land_c.h"
#include "debugext.h"
#include "filesys.h"
#include "mathlib.h"
#include "graph.h"
#include "patterns.h"
#include "view.h"

#if _USE_OPENGL
	#ifdef	__NT__
		#include<windows.h>
	#endif
	#include<GL/gl.h>
	#include<gl3de.h>
#endif

#ifdef __LAND_C__
        #define GLOBAL
#else
        #define GLOBAL extern
#endif

GLOBAL MapCoord         hcoordl[1000],hcoordr[1000];
GLOBAL MapCoord         hcoordltmp[1000],hcoordrtmp[1000];
GLOBAL int              cnt;
GLOBAL char             *pal,*bumpIndex,*hazeTable;
GLOBAL Ship             madsh;
GLOBAL MapH             map,map1,map2,map3,map4,bumpMask;
GLOBAL MapH*            maps[5],bMask[32],relMap[32];
GLOBAL MapH             mapmask;
GLOBAL byte             aHaze[16*256];
GLOBAL MapC             mapc,mapc1,mapc2,mapc3,mapc4;
GLOBAL MapC*            clipmaps[5];
GLOBAL MapD             dithMask;
GLOBAL CoordBuf         mapline[MAXLEN],maplinep[MAXLEN];
GLOBAL ClipBuf          clipLine[MAXLEN],clipLinep[MAXLEN];
GLOBAL CoordBuf         scrlinefirst[MAXLEN],scrlinefirstp[MAXLEN];
GLOBAL double			paintTri,allTri;
GLOBAL CFMatrix3x4      m;
GLOBAL int              bufInt,triVert;

inline int D2INT(double w) { return (int)w; }
#ifdef	__WATCOMC__
#pragma aux D2INT =\
            "fistp bufInt"\
			"mov eax,bufInt"\
			parm [8087]\
			value [eax]
#endif

inline int _MulDiv(int a,int b,int c) { return (int)(a*(double)b/c); }
#ifdef __WATCOMC__
#pragma aux _MulDiv=\
    "imul    ebx"\
    "idiv    ecx"\
     parm [eax] [ebx] [ecx]\
     modify [ecx ebx eax edx]
#endif

extern "C" {
	GLOBAL DataSbLoop vsbloop;
	GLOBAL char *maskPtr;
	GLOBAL int *maskDithPtr;
}


#undef GLOBAL
#endif

