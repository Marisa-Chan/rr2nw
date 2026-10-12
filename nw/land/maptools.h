#ifndef __MAPTOOLS_H__
	#define __MAPTOOLS_H__

#include "land_t.h"

void LoadMap(MapH *map,char *mname);
void LoadMapC(MapC *map,char *mname);
void LoadPal(char *name);
void SaveToSpr(const char* fname,int *fnum);
void LoadDithMask(MapD *map,char *mname);

#endif

