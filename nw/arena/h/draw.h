#ifndef __DRAW_H__INCLUDED
#define __DRAW_H__INCLUDED

class CDC;
class CWnd;
class CBitmap;

extern CWnd *g_drawWindow; 
extern CBitmap *g_cacheViewBmp;

CDC *g_getPainter();
void g_removePainter( CDC * );

#endif
