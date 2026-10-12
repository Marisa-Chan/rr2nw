#ifndef __CONTOUR_H__
	#define __CONTOUR_H__
#include "land_t.h"

void CoordToBuff(Ship *view);
long Bresenham_X(MapCoord* line, CFVector2& st, CFVector2& en);
long Bresenham_Y(MapCoord* line, CFVector2& st, CFVector2& en);
void CreatePolygon(Ship &view);

#endif
