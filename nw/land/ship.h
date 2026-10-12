#ifndef __SHIP_H__
	#define __SHIP_H__

#include "_land.h"

void CalcConnerPoint(Ship *const view);
void LineEdgeIZ(CoordBufD *out,CoordBuf *st,CoordBuf *en,int delta);

#endif
