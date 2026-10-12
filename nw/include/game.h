
#define	_NOAUTOINCLUDE

// ------------------------------------------------------------------------

#define	___LAST_H__STDTYPES	0
#define	___LAST_H__DEBUGEXT	1
#define	___LAST_H__FILESYS	2
#define	___LAST_H__MATHLIB	3
#define	___LAST_H__MUSIC	4
#define	___LAST_H__GRAPH	5
#define	___LAST_H__PATTERNS	6
#define	___LAST_H__VIEW		7
#define	___LAST_H__SCENE	8
#define	___LAST_H__MAIN		9

// ------------------------------------------------------------------------

#ifdef	LAST_H__STDTYPES
	#ifdef	___LAST_H
		#error	Last include workout error
	#endif
	#define	___LAST_H	___LAST_H__STDTYPES
#endif

#ifdef	LAST_H__DEBUGEXT
	#ifdef	___LAST_H
		#error	Last include workout error
	#endif
	#define	___LAST_H	___LAST_H__DEBUGEXT
#endif

#ifdef	LAST_H__FILESYS
	#ifdef	___LAST_H
		#error	Last include workout error
	#endif
	#define	___LAST_H	___LAST_H__FILESYS
#endif

#ifdef	LAST_H__MATHLIB
	#ifdef	___LAST_H
		#error	Last include workout error
	#endif
	#define	___LAST_H	___LAST_H__MATHLIB
#endif

#ifdef	LAST_H__MUSIC
	#ifdef	___LAST_H
		#error	Last include workout error
	#endif
	#define	___LAST_H	___LAST_H__MUSIC
#endif

#ifdef	LAST_H__GRAPH
	#ifdef	___LAST_H
		#error	Last include workout error
	#endif
	#define	___LAST_H	___LAST_H__GRAPH
#endif

#ifdef	LAST_H__PATTERNS
	#ifdef	___LAST_H
		#error	Last include workout error
	#endif
	#define	___LAST_H	___LAST_H__PATTERNS
#endif

#ifdef	LAST_H__VIEW
	#ifdef	___LAST_H
		#error	Last include workout error
	#endif
	#define	___LAST_H	___LAST_H__VIEW
#endif

#ifdef	LAST_H__SCENE
	#ifdef	___LAST_H
		#error	Last include workout error
	#endif
	#define	___LAST_H	___LAST_H__SCENE
#endif

#ifdef	LAST_H__MAIN
	#ifdef	___LAST_H
		#error	Last include workout error
	#endif
	#define	___LAST_H	___LAST_H__MAIN
#endif

#ifndef	___LAST_H
	#error	Last include workout error
#endif

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------


#include"hookallo.h"
#if 0
void *MyNew(unsigned l);
void MyDelete(void*p);
inline void *operator new(unsigned l) { return MyNew(l); }
inline void *operator new[](unsigned l) { return MyNew(l); }
inline void operator delete(void*p) { MyDelete(p); }
inline void operator delete[](void*p) { MyDelete(p); }
#endif



#if	___LAST_H >= ___LAST_H__STDTYPES
	#include"stdtypes.h"
#endif

#if	___LAST_H >= ___LAST_H__DEBUGEXT
	#include<stdio.h>
	#include<string.h>
	#include"debugext.h"
#endif

#if	___LAST_H >= ___LAST_H__FILESYS
	#include"filesys.h"
#endif

#if	___LAST_H >= ___LAST_H__MATHLIB
	#include<stdlib.h>
	#include<math.h>
	#include"mathlib.h"
#endif

#if	___LAST_H >= ___LAST_H__MUSIC
//	#include"music.h"
#endif

#if	___LAST_H >= ___LAST_H__GRAPH
	#include"graph.h"
#endif

#if	___LAST_H >= ___LAST_H__PATTERNS
	#include"patterns.h"
#endif

#if	___LAST_H >= ___LAST_H__VIEW
	#include"view.h"
	#include"land.h"
#endif

#if	___LAST_H >= ___LAST_H__SCENE
	#include"scene.h"
#endif

#if	___LAST_H >= ___LAST_H__MAIN
	#include"main.h"
#endif

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------

#undef	___LAST_H__STDTYPES
#undef	___LAST_H__DEBUGEXT
#undef	___LAST_H__FILESYS
#undef	___LAST_H__MATHLIB
#undef	___LAST_H__MUSIC
#undef	___LAST_H__GRAPH
#undef	___LAST_H__PATTERNS
#undef	___LAST_H__VIEW
#undef	___LAST_H__SCENE

