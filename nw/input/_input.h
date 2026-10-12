
#include<stdlib.h>
#include<string.h>
#include<signal.h>

#ifdef MSDOS
#include<conio.h>
#include<dos.h>
#include<i86.h>
#endif

#ifdef	__sgi
#include<unistd.h>
#endif

#define	LAST_H__PATTERNS
#include"game.h"
#include"input.h"

#ifdef	__NT__
#include<windows.h>
#endif
