#ifndef __DCROSSMSG_H__INCLUDED
#define __DCROSSMSG_H__INCLUDED

enum
 {
     dc_EV_START_MESSAGE = DCROSS_MESSAGE,
     dc_EV_CREATE,
     dc_EV_REMOVE
 };

enum 
 {
     dc_CROSS,
     dc_CROSS_POINT,
     dc_CROSS_CIRCLE,
     dc_CROSS_RECT,
     dc_CROSS_LINE
 } ;

class CFVector3;
void dc_CreateCross(int type,const CFVector3 &p,double time,
                    int x0,int y0, int x1,int y1,
                    unsigned long color=0,const char *text="",int xt=0,int yt=0);

void drawLine(CFVector3 start,CFVector3 end,int rad,int r, int g, int b);
void drawPoint(CFVector3 pos, int rad, int r, int g, int b);
#endif
