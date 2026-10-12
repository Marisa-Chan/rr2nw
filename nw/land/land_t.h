#ifndef __LAND_T_H__
	#define __LAND_T_H__

#include "land_c.h"

typedef struct{
		unsigned short 	x,y;
		char	*mapadr;
} MapH;

typedef struct{
		unsigned short 	x,y;
		int	*mapadr;
} MapD;

typedef struct{
        unsigned char x;
        unsigned char y;
} Ccoord;

typedef union{
		unsigned short	ind;
		Ccoord			crd;
} MaskCrdUnPack;

typedef struct{
		unsigned short	x,y;
		unsigned short	*mapadr;
} MapC;

typedef struct{
        long    x,y;
} MapCoord;

/*
typedef struct{
    int ;
    int
} VehicleParm;
*/
typedef struct {
        long    x,y,z;
        char    farp;

        char    axis;
        long    len;
        long    stlen;

        double  ha,va,ia;

        long    boundMaxX, boundMinX;
        long    boundMaxY, boundMinY;

        int     Direction;

		double	w_h_ang;
		double	h_h_ang;
		double	w_st_ang;
		double	h_st_ang;
        double  fCellSize;
        double  hratio;

		MapCoord	pyr_vertex[8];
        MapCoord    trap_vertex[6];

        int         trap_vertex_num;
        int         dir;
} Ship;

typedef struct{
    long            x,y;
    unsigned long   bl,bh;
    unsigned long   blb,bhb;
    long            zLen;
    double          iz;
} CoordBuf;

typedef struct{
    double          bl,bh;
    double          blb,bhb;
    double          z;
} CoordBufD;

typedef struct{
    int             x;
    int             y;
    int             z;
    int         bl,bh;
    int         blb,bhb;
} ClipBuf;

typedef struct{
    CoordBuf*   pStart,*pEnd;
    ClipBuf*    pStartc,*pEndc;

    int         startInd;
    int         endInd;
} TAlignPlane;

typedef struct{
	struct{
		int bl;
		int bh;
	} st;
	struct{
		int bl;
		int bh;
	} en;
	int			rlen;
	char		*scrPtr;
	int			xfs;
	CoordBuf    *c1;
	CoordBuf    *c2;
	int			amout;
	int			zLen;
    int         flag;
} DataSbLoop;

#endif
