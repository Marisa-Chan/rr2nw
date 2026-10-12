//
// Объявления глобальных пеpеменных
//
#ifndef __LAND_H__
        #define __LAND_H__

#include "land_t.h"
#include "land_c.h"
#include "filesys.h"
#include "mathlib.h"
#include "graph.h"
#include "patterns.h"
#include "view.h"

#if _USE_OPENGL
	#include<windows.h>
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
GLOBAL char             *pal;
GLOBAL Ship             madsh;
GLOBAL MapH             map,map1,map2,map3,map4;
GLOBAL MapH*            maps[5];
GLOBAL MapH             mapmask;
GLOBAL MapC             mapc,mapc1,mapc2,mapc3,mapc4;
GLOBAL MapC*            clipmaps[5];
GLOBAL MapD             dithMask;
GLOBAL CoordBuf         mapline[MAXLEN],maplinep[MAXLEN];
GLOBAL ClipBuf          clipLine[MAXLEN],clipLinep[MAXLEN];
GLOBAL CoordBuf         scrlinefirst[MAXLEN],scrlinefirstp[MAXLEN];
GLOBAL double			paintTri,allTri;
GLOBAL CFMatrix3x4      m;
GLOBAL int				bufInt;

int D2INT(double w);
#pragma aux D2INT =\
            "fistp bufInt"\
			"mov eax,bufInt"\
			parm [8087]\
			value [eax]

extern "C" {
	GLOBAL DataSbLoop vsbloop;
	GLOBAL char *maskPtr;
	GLOBAL int *maskDithPtr;
}


#undef GLOBAL
#endif
