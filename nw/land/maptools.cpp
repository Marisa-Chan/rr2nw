#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef MSDOS
#include <i86.h>
#endif

#include "_land.h"

#define __MAPTOOLS_C__
#include "maptools.h"

//******************************* LoadMap *************************************

void LoadMap(MapH *map,char *mname){
	FILE	*fs;
	int		res;
	char	c;

    RTCHECK1((fs=fopen(mname,"rb"))!=NULL,"Map file not found %s",mname);
	fread(&map->x,2,1,fs);
	fread(&map->y,2,1,fs);
	fread(&c,1,1,fs);
	ReverseByteOrderVarBE(map->x);
	ReverseByteOrderVarBE(map->y);

	#if 1 //_USE_GLIDE
		const int extra = 12;
	#else
		const int extra = 0;
	#endif
    map->mapadr=(char *)malloc(map->x*map->y+20+extra)+extra;
    res = fread(map->mapadr,map->x*map->y,1,fs);

	fclose(fs);
	#if 1 //_USE_GLIDE
		((dword*)map->mapadr)[-1] = map->x;
		((dword*)map->mapadr)[-2] = map->y;
		((dword*)map->mapadr)[-3] = 1;
	#endif
}

//******************************* LoadMapC ************************************

void LoadMapC(MapC *map,char *mname){
	FILE	*fs;
	int		res;
	char	c;

    RTCHECK1((fs=fopen(mname,"rb"))!=NULL,"MapC file not found %s",mname);
    fread(&map->x,2,1,fs);
	fread(&map->y,2,1,fs);
	fread(&c,1,1,fs);
	ReverseByteOrderVarBE(map->x);
	ReverseByteOrderVarBE(map->y);

	map->mapadr=(unsigned short *)malloc((map->x*map->y)*2+20);

	res = fread(map->mapadr,(map->x*map->y)*2,1,fs);
	#ifdef	_BIG_ENDIAN
		int dims = map->x*map->y;
		for(int i = 0 ; i < dims ; i++ ) {
			word *w = (word*)map->mapadr+i;
			byte t = w[0];
			w[0] = w[1];
			w[1] = t;
		}
	#endif
	fclose(fs);
}

//****************************** LoadDithMask *********************************
void LoadDithMask(MapD *map,char *mname){
    FILE    *fs;
	int		res;
    unsigned short    width;

    RTCHECK((fs=fopen(mname,"rb"))!=NULL,"DithMask file not found");
    map->x=0;
    map->y=0;

	fread(&map->x,2,1,fs);
	fread(&map->y,2,1,fs);
    fread(&width,2,1,fs);
	ReverseByteOrderVarBE(map->x);
	ReverseByteOrderVarBE(map->y);
	ReverseByteOrderVarBE(width);

    map->mapadr=(int *)malloc((map->x*map->y)*4);

    res = fread(map->mapadr,(map->x*map->y),4,fs);
	fclose(fs);
}
