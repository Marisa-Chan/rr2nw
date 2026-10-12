#ifdef	__WATCOMC__
#include <mem.h>
#endif
#include "_land.h"

//extern "C" extern CRect2 _gr_clipRect;
#define CLIP_2D_MAX_VERTEX 32
CoordBuf intBuf[CLIP_2D_MAX_VERTEX];

void ClipHorizontal(CoordBuf *vNew, CoordBuf *vp, CoordBuf *v,long y)
{

  vNew->y = y;
  //vNew->x = ((((((double)v->x - (double)vp->x)*((double)y - (double)vp->y))) / ((double)v->y - (double)vp->y))) + (double)vp->x;
  vNew->x = (_MulDiv((v->x-vp->x)<<12,y-vp->y,v->y-vp->y)>>12)+vp->x;

  vNew->iz = ((v->iz - vp->iz) / ((double)v->y - (double)vp->y))*((double)y - (double)vp->y) + vp->iz;

  /*
  vNew->bl = (unsigned long)((((((double)v->bl)*v->iz - ((double)vp->bl)*vp->iz)
                  *(double)(y - vp->y)) / (double)(v->y - vp->y) + (double)(vp->bl)*vp->iz)/(double)vNew->iz);
  vNew->bh = (unsigned long)((((((double)v->bh)*v->iz - ((double)vp->bh)*vp->iz)
                  *(double)(y - vp->y)) /(double)(v->y - vp->y) + ((double)vp->bh)*vp->iz)/(double)vNew->iz);
  */

  //vNew->bl = (((long)v->bl - (long)vp->bl) / (v->y - vp->y))*(y - vp->y) + (long)vp->bl;
  //vNew->bh = (((long)v->bh - (long)vp->bh) / (v->y - vp->y))*(y - vp->y) + (long)vp->bh;

  vNew->bl = _MulDiv(v->bl - vp->bl,y - vp->y,v->y - vp->y)+vp->bl;
  vNew->bh = _MulDiv(v->bh - vp->bh,y - vp->y,v->y - vp->y)+vp->bh;

  vNew->blb = _MulDiv(v->blb - vp->blb,y - vp->y,v->y - vp->y)+vp->blb;
  vNew->bhb = _MulDiv(v->bhb - vp->bhb,y - vp->y,v->y - vp->y)+vp->bhb;
}

void ClipVertical(CoordBuf *vNew, CoordBuf *vp, CoordBuf *v,long x)
{

  vNew->x = x;
  //vNew->y = ((((((double)v->y - (double)vp->y)*((double)x - (double)vp->x)))/ ((double)v->x - (double)vp->x)))+ (double)vp->y;
  vNew->y = (_MulDiv((v->y-vp->y)<<12,x-vp->x,v->x-vp->x)>>12)+vp->y;

  vNew->iz = ((v->iz - vp->iz) / ((double)v->x - (double)vp->x))*((double)x - (double)vp->x) + vp->iz;

  /*
  vNew->bl = (unsigned long)((((((double)v->bl)*v->iz - ((double)vp->bl)*vp->iz)
                  *(double)(x - vp->x)) /(double)(v->x - vp->x) + ((double)vp->bl)*vp->iz)/(double)vNew->iz);
  vNew->bh = (unsigned long)((((((double)v->bh)*v->iz - ((double)vp->bh)*vp->iz)
                  *(double)(x - vp->x)) /(double)(v->x - vp->x) + ((double)vp->bh)*vp->iz)/(double)vNew->iz);
  */

  //vNew->bl = (((long)v->bl - (long)vp->bl) / (v->x - vp->x))*(x - vp->x) + (long)vp->bl;
  //vNew->bh = (((long)v->bh - (long)vp->bh) / (v->x - vp->x))*(x - vp->x) + (long)vp->bh;

  vNew->bl = _MulDiv(v->bl - vp->bl,x - vp->x,v->x - vp->x)+vp->bl;
  vNew->bh = _MulDiv(v->bh - vp->bh,x - vp->x,v->x - vp->x)+vp->bh;

  vNew->blb = _MulDiv(v->blb - vp->blb,x - vp->x,v->x - vp->x)+vp->blb;
  vNew->bhb = _MulDiv(v->bhb - vp->bhb,x - vp->x,v->x - vp->x)+vp->bhb;

}

CoordBuf intBuf1[CLIP_2D_MAX_VERTEX];

long Clip2DPolygonPCCW(CoordBuf *vert,CoordBuf **startBuf,CoordBuf **endBuf,CoordBuf **topVert,CoordBuf **bottomVert,long vertC)
{ int i, num = 0, curin, nextin;
  CoordBuf *pin = vert, *pout = intBuf,*in,*out, *next;
  long leftX, rightX,flag = 0;
  long topY, bottomY;
  CoordBuf *leftVert, *rightVert;


  leftX = rightX = pin->x;
  topY = bottomY = pin->y;
  *topVert = leftVert = *bottomVert = rightVert = pin;

  for(i = 1,next = pin;i < vertC;i++) {
	 next++;

     if (topY > next->y) {
        topY = next->y;
        *topVert = next;
	 }
     else if (bottomY < next->y) {
        bottomY = next->y;
        *bottomVert = next;
	 }
     if (leftX > next->x) {
        leftX = next->x;
		leftVert = next;
	 }
     else if (rightX < next->x) {
        rightX = next->x;
		rightVert = next;
	 }
  }

  //----------simple clipping------------
  if (bottomY < _gr_clipRect.top ||
      topY >= _gr_clipRect.bottom ||
      leftX >= _gr_clipRect.right ||
      rightX < _gr_clipRect.left) return (0);

  //--------------TOP----------------------
  if (topY < _gr_clipRect.top) {
	 in = pin;
	 out = pout;
     curin = (in->y >= _gr_clipRect.top);

     for (i = 0 ;i < vertC - 1 ;i++,in++,curin = nextin) {
        next = in + 1;

        // Keep the current vertex if it's inside the plane
        if (curin) *out++ = *in,num++;

        nextin = (next->y >= _gr_clipRect.top);

       // Add a clipped vertex if one end of the current edge is
       // inside the plane and the other is outside
       if (curin != nextin)
          ClipHorizontal(out++,in,next,_gr_clipRect.top), num++;
	 }

     if (curin) *out++ = *in,num++;

     if (curin != (pin->y >= _gr_clipRect.top))
        ClipHorizontal(out++,in,pin,_gr_clipRect.top), num++;

     if (num < 3) return 0;
     vertC = num;
	 next = pin;
     pin = pout;
	 pout = next;
     flag = 1;
  }

  //----------------------LEFT--------------------
  if (leftX < _gr_clipRect.left) {
     num = 0;
     in = pin;
	 out = pout;
     curin = (in->x >= _gr_clipRect.left);

     for (i = 0 ;i < vertC - 1 ;i++,in++,curin = nextin) {
        next = in + 1;

        // Keep the current vertex if it's inside the plane
        if (curin) *out++ = *in,num++;

        nextin = (next->x >= _gr_clipRect.left);

        // Add a clipped vertex if one end of the current edge is
        // inside the plane and the other is outside
        if (curin != nextin)
           ClipVertical(out++,in,next,_gr_clipRect.left), num++;
	 }

     if (curin) *out++ = *in,num++;

     if (curin != (pin->x >= _gr_clipRect.left))
        ClipVertical(out++,in,pin,_gr_clipRect.left), num++;

     if (num < 3) return 0;
     vertC = num;
	 next = pin;
     pin = pout;
	 pout = next;

	 flag = 1;
  }
  //------------------------BOTTOM---------------------
  if (bottomY >= _gr_clipRect.bottom) {
     num = 0;
     in = pin;
	 out = pout;
     curin = (in->y < _gr_clipRect.bottom);

     for (i = 0 ;i < vertC - 1 ;i++,in++,curin = nextin) {
        next = in + 1;

        // Keep the current vertex if it's inside the plane
        if (curin) *out++ = *in,num++;

        nextin = (next->y < _gr_clipRect.bottom);

        // Add a clipped vertex if one end of the current edge is
        // inside the plane and the other is outside
        if (curin != nextin)
           ClipHorizontal(out++,in,next,_gr_clipRect.bottom), num++;
	 }

     if (curin) *out++ = *in,num++;

     if (curin != (pin->y < _gr_clipRect.bottom))
        ClipHorizontal(out++,in,pin,_gr_clipRect.bottom), num++;

     if (num < 3) return 0;
     vertC = num;
	 next = pin;
     pin = pout;
	 pout = next;

	 flag = 1;
  }
  //--------------------------RIGHT--------------------
  if (rightX >= _gr_clipRect.right) {
     num = 0;
     in = pin;
	 out = pout;
     curin = (in->x < _gr_clipRect.right);

     for (i = 0 ;i < vertC - 1 ;i++,in++,curin = nextin) {
        next = in + 1;

        // Keep the current vertex if it's inside the plane
        if (curin) *out++ = *in,num++;

        nextin = (next->x < _gr_clipRect.right);

        // Add a clipped vertex if one end of the current edge is
        // inside the plane and the other is outside
        if (curin != nextin)
           ClipVertical(out++,in,next,_gr_clipRect.right-1), num++;
	 }

     if (curin) *out++ = *in,num++;

     if (curin != (pin->x < _gr_clipRect.right))
        ClipVertical(out++,in,pin,_gr_clipRect.right-1), num++;

     if (num < 3) return 0;
	 vertC = num;
	 pin = pout;

	 flag = 1;
  }
  //-----------------Calc topY and bottomY-----------------
  if (flag) {
     topY = bottomY = pin->y;
     *topVert = *bottomVert = pin;

     for(i = 1,next = pin; i < num;i++) {
	    next++;
        if (next->y < topY) {
           topY = next->y;
           *topVert = next;
		}
        else if (next->y > bottomY) {
           bottomY = next->y;
           *bottomVert = next;
		}
	 }
  }

  *startBuf = pin;
  *endBuf = next;
  return (vertC);

}

long Clip2DPolygonPCCW1(CoordBuf *vert,CoordBuf **startBuf,CoordBuf **endBuf,CoordBuf **topVert,CoordBuf **bottomVert,long vertC)
{ int i, num = 0, curin, nextin;
  CoordBuf *pin = vert, *pout = intBuf1, *next;
  long topY, bottomY;

  //--------------TOP----------------------
  curin = (pin->y >= _gr_clipRect.top);

  for (i = 0 ;i < vertC - 1 ;i++,pin++,curin = nextin) {
     next = pin + 1;

     // Keep the current vertex if it's inside the plane
     if (curin) *pout++ = *pin,num++;

     nextin = (next->y >= _gr_clipRect.top);

     // Add a clipped vertex if one end of the current edge is
     // inside the plane and the other is outside
     if (curin != nextin)
        ClipHorizontal(pout++,pin,next,_gr_clipRect.top), num++;
  }

  if (curin) *pout++ = *pin,num++;

  if (curin != (vert->y >= _gr_clipRect.top))
     ClipHorizontal(pout++,pin,vert,_gr_clipRect.top), num++;

  if (num < 3) return 0;
  vertC = num;

  //----------------------LEFT--------------------
  pin = intBuf1; pout = intBuf; num = 0;

  curin = (pin->x >= _gr_clipRect.left);

  for (i = 0 ;i < vertC - 1 ;i++,pin++,curin = nextin) {
     next = pin + 1;

     // Keep the current vertex if it's inside the plane
     if (curin) *pout++ = *pin,num++;

     nextin = (next->x >= _gr_clipRect.left);

     // Add a clipped vertex if one end of the current edge is
     // inside the plane and the other is outside
     if (curin != nextin)
        ClipVertical(pout++,pin,next,_gr_clipRect.left), num++;
  }

  if (curin) *pout++ = *pin,num++;

  if (curin != (intBuf1[0].x >= _gr_clipRect.left))
     ClipVertical(pout++,pin,intBuf1,_gr_clipRect.left), num++;

  if (num < 3) return 0;
  vertC = num;

  //------------------------BOTTOM---------------------
  pin = intBuf; pout = intBuf1; num = 0;

  curin = (pin->y < _gr_clipRect.bottom);

  for (i = 0 ;i < vertC - 1 ;i++,pin++,curin = nextin) {
     next = pin + 1;

     // Keep the current vertex if it's inside the plane
     if (curin) *pout++ = *pin,num++;

     nextin = (next->y < _gr_clipRect.bottom);

     // Add a clipped vertex if one end of the current edge is
     // inside the plane and the other is outside
     if (curin != nextin)
        ClipHorizontal(pout++,pin,next,_gr_clipRect.bottom), num++;
  }

  if (curin) *pout++ = *pin,num++;

  if (curin != (intBuf[0].y < _gr_clipRect.bottom))
     ClipHorizontal(pout++,pin,intBuf,_gr_clipRect.bottom), num++;

  if (num < 3) return 0;
  vertC = num;

  //--------------------------RIGHT--------------------
  pin = intBuf1; pout = intBuf; num = 0;

  curin = (pin->x < _gr_clipRect.right);

  for (i = 0 ;i < vertC - 1 ;i++,pin++,curin = nextin) {
     next = pin + 1;

     // Keep the current vertex if it's inside the plane
     if (curin) *pout++ = *pin,num++;

     nextin = (next->x < _gr_clipRect.right);

     // Add a clipped vertex if one end of the current edge is
     // inside the plane and the other is outside
     if (curin != nextin)
        ClipVertical(pout++,pin,next,_gr_clipRect.right-1), num++;
  }

  if (curin) *pout++ = *pin,num++;

  if (curin != (intBuf1[0].x < _gr_clipRect.right))
     ClipVertical(pout++,pin,intBuf1,_gr_clipRect.right-1), num++;

  if (num < 3) return 0;
  //-----------------Calc topY and bottomY-----------------
  topY = bottomY = intBuf[0].y;
  *topVert = *bottomVert = intBuf;

  for(i = 1,pin = intBuf; i < num;i++) {
	 pin++;
     if (pin->y < topY) {
        topY = pin->y;
        *topVert = pin;
	 }
     else if (pin->y > bottomY) {
        bottomY = pin->y;
        *bottomVert = pin;
	 }
  }
  *startBuf=intBuf;
  *endBuf=pin;
  return (num);

}
