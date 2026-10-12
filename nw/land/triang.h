
#ifndef __TRIANG_H__
	#define __TRIANG_H__

#if !_USE_OPENGL
	void TriangFirstPlaneY(CoordBuf *cCrd,int num,int ch,int x,int y);
#endif
void CalcSplitCoords(long left,long top,long right,long bottom,int mapsize);
int  CalcCoordByX_First(int step, CoordBuf *mapLine, ClipBuf *lClip, int stX, int endX, int qual);
int  CalcCoordByY_First(int step, CoordBuf *mapLine, ClipBuf *lClip, int stY, int endY, int qual);

#endif
