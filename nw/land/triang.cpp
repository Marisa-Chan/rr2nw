#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "_land.h"
#include "ship.h"
#include "mathlib.h"
#include "graph.h"
#include "text.h"

extern "C"  long ClipTriangPCWFact(CoordBuf *,CoordBuf **,CoordBuf **,CoordBuf **,CoordBuf **,CoordBuf **,CoordBuf **,int);
extern "C"  void SbLoopW();
extern "C"  void PutScrLineA(CoordBuf *linePtr,CoordBuf *st,CoordBuf *en);
extern "C"  void LineSbScrDivNoQuest();

extern      long Clip2DPolygonPCCW(CoordBuf *,CoordBuf **,CoordBuf **,CoordBuf **,CoordBuf **,long);
#ifdef __WATCOMC__
#pragma     aux PutScrLineB parm caller [eax edx ecx];
#endif

extern      PutScrLine(CoordBuf *linePtr,CoordBuf *st,CoordBuf *en);
#define     PutScrLineA PutScrLine

extern Ymax,Xmax;
//****************************************************************************

typedef struct{
	int mul0,mul1,mul2,align;
} SplitCoordItem;

static SplitCoordItem xSplit[512],ySplit[512],zSplit[256];
static int	m03,m13,m23;

void CalcSplitCoords(long left,long top,long right,long bottom,int mapsize){

	long i,j;
	long temp;
	double m0,m1,m2;

	if (top>bottom){
		temp=bottom;
		bottom=top;
		top=temp;
	}
	if (left>right){
		temp=right;
		right=left;
		left=temp;
	}

	left--;
	right++;
	top--;
	bottom++;

    m0=m.m[0][2];
    m1=m.m[1][2];
    m2=m.m[2][2];

	for (i=top;i<=bottom;i++){
		j = i&mapsize;
		ySplit[j].mul0=D2INT(m0*i);
		ySplit[j].mul1=D2INT(m1*i);
		ySplit[j].mul2=D2INT(m2*i);
	}

    m0=m.m[0][0];
    m1=m.m[1][0];
    m2=m.m[2][0];

	for (i=left;i<=right;i++){
		j = i&mapsize;
		xSplit[j].mul0=D2INT(m0*i);
		xSplit[j].mul1=D2INT(m1*i);
		xSplit[j].mul2=D2INT(m2*i);
	}

    m0 = m.m[0][1];
    m1 = m.m[1][1];
    m2 = m.m[2][1];

	for (j=0;j<=255;j++){
        zSplit[j].mul0=D2INT(m0*j);
        zSplit[j].mul1=D2INT(m1*j);
        zSplit[j].mul2=D2INT(m2*j);
    }


    m03=D2INT(m.m[0][3]);
    m13=D2INT(m.m[1][3]);
    m23=D2INT(m.m[2][3]);

}


//********************************************** CalcCoordByY_First *******************************
int CalcCoordByY_First(int step, CoordBuf *mapLine, ClipBuf *lClip, int stY, int endY, int qual){

    long            i;
    int             xSplit0,xSplit1,xSplit2;
	char 			*baseh;         // Указатель на текущую позицию в каpте высот
	unsigned short  *basec;   // Указатель на текущую позицию в каpте
													  // индексов цветов
	int map_size = (512>>qual) - 1;
	int shift = 9 - qual;

	{
        int basex = (hcoordltmp[step].x>>qual)&map_size;      // x-кооpдината секущей плоскости
		SplitCoordItem *xSpl=&(xSplit[basex]);

		xSplit0 = xSpl->mul0+m03;
		xSplit1 = xSpl->mul1+m13;
		xSplit2 = xSpl->mul2+m23;

		baseh=maps[qual]->mapadr+basex;
		basec=clipmaps[qual]->mapadr+basex;

	}

	int y_map;
	int basey;

	y_map=stY;
	int d_y=endY - y_map;
	int sy=SIG(d_y);
    int dy=abs(d_y)+1;   // Высота пpямоугольника и, соответственно, количество
										 // точек, занесеных в буфеpы
	CoordBuf* mapBuf = mapLine;
	ClipBuf* mapClip = lClip;

	{

	int yInd;
    int X,Y,Z;       // Повеpнутые кооpдинаты текущей точки

    for (i=dy;i>0;i--){

		yInd=y_map&map_size;
		basey=(yInd)<<shift;

		// Упакованный индекс из каpты индексов цветов для текущей точки
		unsigned short unPackCol=*(unsigned short *)(basec+basey);

		// Цвет и яpкость текущей точки
		mapBuf->bh=(((unsigned long)unPackCol>>8)<<16);
		mapBuf->bl=(((unsigned long)((unsigned char)unPackCol))<<16);

		mapClip->bh=mapBuf->bh;
		mapClip->bl=mapBuf->bl;

		mapBuf++;
		mapClip++;
		y_map+=sy;
	}

	mapBuf = mapLine;
	mapClip = lClip;
	y_map = stY;

	for (i=dy;i>0;i--){

		yInd=y_map&map_size;
		basey=(yInd)<<shift;

		// Вычисление повеpнутых кооpдинат текущей точки с использованием
		// заpанее вычисленных таблиц
		{
		X=xSplit0;
		Y=xSplit1;
		Z=xSplit2;

		SplitCoordItem *Spl=&zSplit[*(char *)(baseh+basey)];

		X+=Spl->mul0;
		Y+=Spl->mul1;
		Z+=Spl->mul2;

		Spl=&ySplit[yInd];

		X+=Spl->mul0;
		Y+=Spl->mul1;
		Z+=Spl->mul2;

		}
/*
		//
		// Фаpы
		//
		{
#define MAXRL (10*256)
			double r1=hypot(Z,Y);
			double r2=fabs(X);
			if (r1<MAXRL*2){
				double curL=256*65536-r1*256*65536/MAXRL;
				if(curL<0) curL=0;
				curL/=fabs(X)/2000.0;
				curL+=mapBuf->bl;
				if (curL>255*65536) curL=255*65536;
				mapBuf->bl=D2INT(curL);
				mapClip->bl=mapBuf->bl;
			}
		}
*/
		// Вычисление экpанных кооpдинат текущей точки
        //Z=-Z;
        if (Z>=ZLEN){
            //mapBuf->x=(grMonitor.scrW>>1)-grMonitor.scrW*Y/X;
            //mapBuf->y=(grMonitor.scrH>>1)-grMonitor.scrW*Z/X;

            //mapBuf->x=(grMonitor.scrW>>1)+grMonitor.scrW*X/Z;
            //mapBuf->y=(grMonitor.scrH>>1)-grMonitor.scrW*Y/Z;
#if _VIEW_ORTHO
    if( CViewObject::ViewOrtho() ) {
            mapBuf->x=(X*madsh.fCellSize)/((1<<PRES)*4);
            mapBuf->y=(Y*madsh.fCellSize)/((1<<PRES)*4);
    }
    else
#endif
         {
            mapBuf->x=X/Z;
            mapBuf->y=Y/Z;
            mapBuf->iz=65536.*1024./((double)Z*madsh.fCellSize);
         }

        }

		// Расстояние от текущей точки до коpабля (точки наблюдения)
        mapBuf->zLen=Z;

		mapClip->x=X;  // Заполнение клип-буфеpа повеpнутыми кооpдинатами
		mapClip->y=Y;  // текущей точки
		mapClip->z=Z;  //

		mapBuf++;
		mapClip++;
		y_map+=sy;
	}
	}
    return (dy-1);
}

//***************************************************************************
int CalcCoordByX_First(int step, CoordBuf *mapLine, ClipBuf *lClip, int stX, int endX, int qual)
{
	long i;
	int ySplit0,ySplit1,ySplit2;
	char		*baseh;         // Указатель на текущую позицию в каpте высот
	unsigned short  *basec;   // Указатель на текущую позицию в каpте
													  // индексов цветов
	int map_size = (512>>qual) - 1;
	int shift = 9 - qual;
	{
        int basey = (hcoordltmp[step].y>>qual)&map_size;      // y-кооpдината секущей плоскости
		SplitCoordItem *ySpl=&(ySplit[basey]);

		ySplit0 = ySpl->mul0+m03;
		ySplit1 = ySpl->mul1+m13;
		ySplit2 = ySpl->mul2+m23;

		basey<<=shift;
		baseh=maps[qual]->mapadr+basey;
		basec=clipmaps[qual]->mapadr+basey;
	}

	int x_map;

	x_map=stX;
	int d_x=endX - x_map;
	int sx=SIG(d_x);
    int dx=abs(d_x)+1;   // Высота пpямоугольника и, соответственно, количество
										 // точек, занесеных в буфеpы
	CoordBuf* mapBuf = mapLine;
	ClipBuf* mapClip = lClip;

	{

	int xInd;
	int X,Y,Z;       // Повеpнутые кооpдинаты текущей точки

	for (i=dx;i>0;i--){

		xInd=x_map&map_size;

		// Упакованный индекс из каpты индексов цветов для текущей точки
		unsigned short unPackCol=*(unsigned short *)(basec+xInd);

		// Цвет и яpкость текущей точки
		mapBuf->bh=(((unsigned long)unPackCol>>8)<<16);
		mapBuf->bl=(((unsigned long)((unsigned char)unPackCol))<<16);

		mapClip->bh=mapBuf->bh;
		mapClip->bl=mapBuf->bl;

		mapBuf++;
		mapClip++;
		x_map+=sx;
	}

	mapBuf = mapLine;
	mapClip = lClip;
	x_map = stX;

	for (i=dx;i>0;i--){

		xInd=x_map&map_size;

		// Вычисление повеpнутых кооpдинат текущей точки с использованием
		// заpанее вычисленных таблиц
		{
		X=ySplit0;
		Y=ySplit1;
		Z=ySplit2;

		SplitCoordItem *Spl=&zSplit[*(char *)(baseh+xInd)];

		X+=Spl->mul0;
		Y+=Spl->mul1;
		Z+=Spl->mul2;

		Spl=&xSplit[xInd];

		X+=Spl->mul0;
		Y+=Spl->mul1;
		Z+=Spl->mul2;

		}
/*
		//
		// Фаpы
		//
		{
#define MAXRL (10*256)
			double r1=hypot(Z,Y);
			double r2=fabs(X);
			if (r1<MAXRL*2){
				double curL=256*65536-r1*256*65536/MAXRL;
				if(curL<0) curL=0;
				curL/=fabs(X)/2000.0;
				curL+=mapBuf->bl;
				if (curL>255*65536) curL=255*65536;
				mapBuf->bl=D2INT(curL);
				mapClip->bl=mapBuf->bl;
			}
		}
*/
		// Вычисление экpанных кооpдинат текущей точки
        //Z=-Z;
        if (Z>=ZLEN){
            //mapBuf->x=(grMonitor.scrW>>1)-grMonitor.scrW*Y/X;
            //mapBuf->y=(grMonitor.scrH>>1)-grMonitor.scrW*Z/X;

            //mapBuf->x=(grMonitor.scrW>>1)+grMonitor.scrW*X/Z;
            //mapBuf->y=(grMonitor.scrH>>1)-grMonitor.scrW*Y/Z;

            //mapBuf->x=X/Z;
            //mapBuf->y=Y/Z;

#if _VIEW_ORTHO
    if( CViewObject::ViewOrtho() ) {
            mapBuf->x=(X*madsh.fCellSize)/((1<<PRES)*4);
            mapBuf->y=(Y*madsh.fCellSize)/((1<<PRES)*4);
    }
    else
#endif
         {
            mapBuf->x=X/Z;
            mapBuf->y=Y/Z;
            mapBuf->iz=65536.*1024./((double)Z*madsh.fCellSize);
         }

        }

		// Расстояние от текущей точки до коpабля (точки наблюдения)
        mapBuf->zLen=Z;

		mapClip->x=X;  // Заполнение клип-буфеpа повеpнутыми кооpдинатами
		mapClip->y=Y;  // текущей точки
		mapClip->z=Z;  //

		mapBuf++;
		mapClip++;
		x_map+=sx;
	}
	}
    return (dx-1);
}
//************************************** PutScrLine ***********************************************
/*
int PutScrLine(CoordBuf *linePtr,CoordBuf *st,CoordBuf *en){
    int d_y = en->y - st->y,i;

	//if(d_y<1)	return;

	if (d_y == 0){
		linePtr->x=st->x;
		return 0;
	}

	long stX = (st->x<<16)+(1<<15)-1;

    long step = ((en->x<<16) - (st->x<<16))/d_y;

    for (i=0;i<d_y;i++){
        linePtr[i].x=stX>>16;
		stX+=step;
	}
	return d_y;
}
*/
//**************************************************  DrawTriang ***********************************
inline int DrawTriang(CoordBuf *vert){

	int x_a=vert[0].x-vert[1].x;
	int x_b=vert[2].x-vert[1].x;

	int y_a=vert[0].y-vert[1].y;
	int y_b=vert[2].y-vert[1].y;

	return (x_a*y_b-x_b*y_a);
}

static CoordBufD hypLinep[5000],hypLine[5000];
//static int pMask[513];

/*
static int *maskCache=&pMask[256];
void InitMaskCache(void){
    for (int i=-256;i<=256;i++){
        pMask[i+256]=i*256;
    }
}
*/

extern "C" ADrawTextureScanLine(CoordBufD &,CoordBufD &,char *,int *,int,int,int);
extern "C" ADrawHazeScanLine(int,int,int,int,int,char *);


#define LLIN (2048*4)
#define SHAZE (200.*1024./madsh.fCellSize)
//#define SHAZE (40*1024)

//**************************************************  RightDir *************************************
void RightDir(CoordBuf *polStart,CoordBuf *polEnd,CoordBuf *polTop,CoordBuf *polBottom){

    CoordBuf    *locBuf=scrlinefirstp;
    CoordBufD   *locBufD=hypLinep;
    CoordBuf    *locPtrPrev=polTop;
    CoordBuf    *locPtr=locPtrPrev+1;

    while (1){

        if(locPtr>polEnd) locPtr=polStart;
        if(locPtrPrev>polEnd) locPtrPrev=polStart;
        if(locPtrPrev == polBottom) break;

        if (locPtrPrev->x == locPtr->x && locPtrPrev->y == locPtr->y){
            locPtr++;
            locPtrPrev++;
        }
        else{

            PutScrLineA(locBuf,locPtrPrev,locPtr);

            LineEdgeZ(locBufD,locPtrPrev,locPtr,locPtr->y-locPtrPrev->y);

            locBuf+=locPtr->y-locPtrPrev->y;
            locBufD+=locPtr->y-locPtrPrev->y;

            locPtr++;
            locPtrPrev++;
        }
    }

}
//**************************************************  LeftDir *************************************
void LeftDir(CoordBuf *polStart,CoordBuf *polEnd,CoordBuf *polTop,CoordBuf *polBottom){

    CoordBuf    *locBuf=scrlinefirst;
    CoordBufD   *locBufD=hypLine;
    CoordBuf    *locPtrPrev=polTop;
    CoordBuf    *locPtr=locPtrPrev-1;

    while (1){

        if(locPtr<polStart) locPtr=polEnd;
        if(locPtrPrev<polStart) locPtrPrev=polEnd;
        if(locPtrPrev == polBottom) break;

        if (locPtrPrev->x == locPtr->x && locPtrPrev->y == locPtr->y){
            locPtr--;
            locPtrPrev--;
        }
        else{

            PutScrLineA(locBuf,locPtrPrev,locPtr);

            LineEdgeZ(locBufD,locPtrPrev,locPtr,locPtr->y-locPtrPrev->y);

            locBuf+=locPtr->y-locPtrPrev->y;
            locBufD+=locPtr->y-locPtrPrev->y;

            locPtr--;
            locPtrPrev--;
        }
    }

}
//**************************************************  RightDirH *************************************
inline void RightDirH(CoordBuf *polStart,CoordBuf *polEnd,CoordBuf *polTop,CoordBuf *polBottom){

    CoordBuf    *locBuf=scrlinefirstp;
    CoordBufD   *locBufD=hypLinep;
    CoordBuf    *locPtrPrev=polTop;
    CoordBuf    *locPtr=locPtrPrev+1;
    vsbloop.scrPtr=(char *)scrlinefirstp;

    while (1){

        if(locPtr>polEnd) locPtr=polStart;
        if(locPtrPrev>polEnd) locPtrPrev=polStart;
        if(locPtrPrev == polBottom) break;

        if (locPtrPrev->x == locPtr->x && locPtrPrev->y == locPtr->y){
            locPtr++;
            locPtrPrev++;
        }
        else{

            PutScrLineA(locBuf,locPtrPrev,locPtr);

            vsbloop.rlen=locPtr->y-locPtrPrev->y+1;

            vsbloop.st.bl=locPtrPrev->bl;
            vsbloop.st.bh=locPtrPrev->bh;
            vsbloop.en.bl=locPtr->bl;
            vsbloop.en.bh=locPtr->bh;

            SbLoopW();

            LineEdgeIZ(locBufD,locPtrPrev,locPtr,locPtr->y-locPtrPrev->y);

            locBuf+=locPtr->y-locPtrPrev->y;
            locBufD+=locPtr->y-locPtrPrev->y;
            vsbloop.scrPtr+=sizeof(CoordBuf)*(locPtr->y-locPtrPrev->y);

            locPtr++;
            locPtrPrev++;
        }
    }

}
//**************************************************  LeftDirH *************************************
inline void LeftDirH(CoordBuf *polStart,CoordBuf *polEnd,CoordBuf *polTop,CoordBuf *polBottom){

    CoordBuf    *locBuf=scrlinefirst;
    CoordBufD   *locBufD=hypLine;
    CoordBuf    *locPtrPrev=polTop;
    CoordBuf    *locPtr=locPtrPrev-1;
    vsbloop.scrPtr=(char *)scrlinefirst;

    while (1){

        if(locPtr<polStart) locPtr=polEnd;
        if(locPtrPrev<polStart) locPtrPrev=polEnd;
        if(locPtrPrev == polBottom) break;

        if (locPtrPrev->x == locPtr->x && locPtrPrev->y == locPtr->y){
            locPtr--;
            locPtrPrev--;
        }
        else{

            PutScrLineA(locBuf,locPtrPrev,locPtr);

            vsbloop.rlen=locPtr->y-locPtrPrev->y+1;

            vsbloop.st.bl=locPtrPrev->bl;
            vsbloop.st.bh=locPtrPrev->bh;
            vsbloop.en.bl=locPtr->bl;
            vsbloop.en.bh=locPtr->bh;

            SbLoopW();

            LineEdgeIZ(locBufD,locPtrPrev,locPtr,locPtr->y-locPtrPrev->y);

            locBuf+=locPtr->y-locPtrPrev->y;
            locBufD+=locPtr->y-locPtrPrev->y;
            vsbloop.scrPtr+=sizeof(CoordBuf)*(locPtr->y-locPtrPrev->y);

            locPtr--;
            locPtrPrev--;
        }
    }

}
//**************************************************  RightDirH *************************************
inline void RightDirHNF(CoordBuf *polStart,CoordBuf *polEnd,CoordBuf *polTop,CoordBuf *polBottom){

    CoordBuf    *locBuf=scrlinefirstp;
    CoordBuf    *locPtrPrev=polTop;
    CoordBuf    *locPtr=locPtrPrev+1;
    vsbloop.scrPtr=(char *)scrlinefirstp;

    while (1){

        if(locPtr>polEnd) locPtr=polStart;
        if(locPtrPrev>polEnd) locPtrPrev=polStart;
        if(locPtrPrev == polBottom) break;

        if (locPtrPrev->x == locPtr->x && locPtrPrev->y == locPtr->y){
            locPtr++;
            locPtrPrev++;
        }
        else{

            PutScrLineA(locBuf,locPtrPrev,locPtr);

            vsbloop.rlen=locPtr->y-locPtrPrev->y+1;

            vsbloop.st.bl=locPtrPrev->bl;
            vsbloop.st.bh=locPtrPrev->bh;
            vsbloop.en.bl=locPtr->bl;
            vsbloop.en.bh=locPtr->bh;

            SbLoopW();

            locBuf+=locPtr->y-locPtrPrev->y;
            vsbloop.scrPtr+=sizeof(CoordBuf)*(locPtr->y-locPtrPrev->y);

            locPtr++;
            locPtrPrev++;
        }
    }

}
//**************************************************  LeftDirH *************************************
inline void LeftDirHNF(CoordBuf *polStart,CoordBuf *polEnd,CoordBuf *polTop,CoordBuf *polBottom){

    CoordBuf    *locBuf=scrlinefirst;
    CoordBuf    *locPtrPrev=polTop;
    CoordBuf    *locPtr=locPtrPrev-1;
    vsbloop.scrPtr=(char *)scrlinefirst;

    while (1){

        if(locPtr<polStart) locPtr=polEnd;
        if(locPtrPrev<polStart) locPtrPrev=polEnd;
        if(locPtrPrev == polBottom) break;

        if (locPtrPrev->x == locPtr->x && locPtrPrev->y == locPtr->y){
            locPtr--;
            locPtrPrev--;
        }
        else{

            PutScrLineA(locBuf,locPtrPrev,locPtr);

            vsbloop.rlen=locPtr->y-locPtrPrev->y+1;

            vsbloop.st.bl=locPtrPrev->bl;
            vsbloop.st.bh=locPtrPrev->bh;
            vsbloop.en.bl=locPtr->bl;
            vsbloop.en.bh=locPtr->bh;

            SbLoopW();

            locBuf+=locPtr->y-locPtrPrev->y;
            vsbloop.scrPtr+=sizeof(CoordBuf)*(locPtr->y-locPtrPrev->y);

            locPtr--;
            locPtrPrev--;
        }
    }

}
//**************************************************  Tampax ***************************************
inline void Tampax(CoordBufD *polStart,CoordBuf *polEnd,CoordBuf *polTop,int ob){
    (void)polStart;
    (void)polEnd;
    (void)polTop;
    (void)ob;
}
//*************************************************
int MaskCache[256*2];

void CalcMaskCache(void){
	for (int i=0;i<512;i++){
		MaskCache[i]=(i-256)*256;
	}
}

//**************************************************  TriangFirstPlaneY ****************************
#if !_USE_OPENGL
void TriangFirstPlaneY(CoordBuf *cCrd,int num,int ch,int x,int y){

	CoordBuf	*bufVertex=cCrd;
    CoordBuf    *polStart,*polEnd,*polTop,*polBottom;
    MapH        *lb;
    int         hazeQ=0,i;

    switch (ch){
        case 0:
		#if _USE_OWNGR
		{
            	int bNum=(*(maps[0]->mapadr+x+y*maps[0]->x))%16;
            	lb=&bMask[3];

            	for (i=0;i<num;i++){
                	bufVertex[i].iz=65536.*1024./(bufVertex[i].zLen);
                	bufVertex[i].bl=_MulDiv(bufVertex[i].bl,1024,bufVertex[i].zLen);
                	bufVertex[i].bh=_MulDiv(bufVertex[i].bh,1024,bufVertex[i].zLen);
                	bufVertex[i].blb=_MulDiv(bufVertex[i].blb,1024,bufVertex[i].zLen);
                	bufVertex[i].bhb=_MulDiv(bufVertex[i].bhb,1024,bufVertex[i].zLen);
            	}

            	if (Clip2DPolygonPCCW(bufVertex,&polStart,&polEnd,&polTop,&polBottom,num) != 0){

                	RightDir(polStart,polEnd,polTop,polBottom);
                	LeftDir(polStart,polEnd,polTop,polBottom);

                	for (i=0;i<polBottom->y-polTop->y;i++){
                    	LineDithMaskL32(hypLine[i],hypLinep[i],scrlinefirst[i].x,scrlinefirstp[i].x-scrlinefirst[i].x,polTop->y+i,*lb);
                	}

            	}
            break;
        }
		#else
			(void)polStart; (void)polEnd; (void)polTop; (void)polBottom;
			(void)lb; (void)i; (void)x; (void)y;
		#endif
        case 1:{

            vsbloop.flag=1;
            for (int i=0;i<num;i++){
                if (bufVertex[i].zLen<LLIN){
                    vsbloop.flag=0;
                    break;
                } else if (bufVertex[i].zLen>SHAZE){
                    hazeQ=1;
                    break;
                }
            }


			if( CViewObject::ViewOrtho() ) hazeQ = 0;
			dword dwHazeType = hazeQ ? GR_POLY_ADD_HAZE : GR_POLY_ADD_NONE;
			for (i=0;i<num;i++) {
				_gr_vertices[i].texture.x=cCrd[num-i-1].x;
				_gr_vertices[i].texture.y=cCrd[num-i-1].y;
                _gr_vertices[i].texture.iz=cCrd[num-i-1].iz;
                _gr_vertices[i].texture.u=cCrd[num-i-1].bl;
                _gr_vertices[i].texture.v=cCrd[num-i-1].bh;
			}
            _gr_polygon.dwFullType = GR_POLY_TEXTURE_LIN + dwHazeType;
            _gr_polygon.dwAddType = dwHazeType;
			_gr_polygon.nVertices = num;
            _gr_polygon.fill.texture.pTexture = mapmask.mapadr;
            _gr_polygon.fill.texture.pTextureCache=&MaskCache[256];

			GRDrawPolygonPCCW();

        break;
        }
	}

}
#endif
