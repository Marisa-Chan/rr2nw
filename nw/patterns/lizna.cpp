#ifndef	__PATTERNS_CPP__
#include"_pattern.h"
#endif

extern "C" {
	extern int _gr_nScreenWidth;
}
//===========================CLinza====================================

CLinza::CLinza(int scrW, int scrH, double fMin, double fMax, int count)
{ word *mask1, *data;
  byte *flag;
  long len, i, j, k, hp;
  double kf;

  dx = (scrW - scrH)/2;
  len = (scrW * (scrW + 2))/8 - (dx * (dx + 1))/2;
  mask1 = new word[len];
  mask = new byte[len];
  flag = new byte[512];
  data = new word[512];
  memset(flag,0,512);

  w2 = scrW/2 - 1;
  h2 = scrH/2 - 1;

  //maxhyp = hypot((double)w2, (double)h2);
  kf = 255.0 / hypot((double)w2, (double)h2);

  for(j = 0,k = 0;j < dx;j++)
	 for(i = dx;i <= w2;i++,k++) {
		hp = ((w2 - i) * (w2 - i) + (w2 - j) * (w2 - j));
		hp = ceil(sqrt((double)hp)*kf);
		mask1[k] = (word)hp;
		flag[hp] = 1;
	 }
  for(;j <= w2;j++)
	 for(i = j;i <= w2;i++,k++) {
		hp = ((w2 - i) * (w2 - i) + (w2 - j) * (w2 - j));
		hp = ceil(sqrt((double)hp)*kf);
		mask1[k] = (word)hp;
		flag[hp] = 1;
	 }

  for(i = 0,k = 0;i < 512;i++)
	 if (flag[i]) {
		for(j = 0;j < len;j++)
		   if (mask1[j] == i) mask[j] = (byte)k;
		data[k++] = (word)i;
	 }
  //if (k > MAX_COUNT) return;
  dFact = k;

  delete []flag;
  delete []mask1;
  fact = new byte[(fCount = count) * dFact];

  for(k = 0,j = 0;k < count; k++) {
	 double a = fMin + (fMax - fMin) * ((sin(M_PI * ((double)k)/
	 		    (count - 1) - M_PI/2.0) + 1.0)/2.0);
	 for(i = 0;i < dFact;i++,j++) {
		double hyp = data[i]/255.0, f = 0;
	    if( hyp != 0 ) f = (pow(hyp,a)/hyp)*255.0;
	    fact[j] = f;
	 }
  }
  delete [] data;
}

//===========================~CLinza====================================

CLinza::~CLinza()
{ delete [] fact;
  delete [] mask;
}

//==============================Draw Lizna====================================

void CLinza::LinzaApply(byte * pScr, double power)
{ byte *f;
  long u, v, n, uk, uk1, uk2, uk3, tmp;

  power = MinMax(power,0.,1.);
  f = fact + (long)(ceil(((double)(fCount - 1)) * power) * dFact);
  pScr += (tmp = h2 * _gr_nScreenWidth) + w2;

  for(u = -w2,n = 0;u < -w2 + dx;u++)
	 for(v = -h2,uk = -tmp + u,uk1 = -tmp - u;v <= 0;
	     v++,uk += _gr_nScreenWidth,uk1 += _gr_nScreenWidth) {
	 	long x,y;

	    x = (u * f[mask[n]])>>8;
	    y = ((v * f[mask[n++]])>>8) * _gr_nScreenWidth;

		pScr[uk]   = pScr[x+y];
	    pScr[-uk]  = pScr[-x-y];
	    pScr[uk1]  = pScr[y-x];
	    pScr[-uk1] = pScr[-y+x];
	 }

  for(tmp = u * _gr_nScreenWidth;u <= 0;u++,tmp += _gr_nScreenWidth)
	 for(v = u, uk2 = tmp + v,uk3 = tmp - v,uk = tmp + u,uk1 = tmp - u;v <= 0;
 	     v++,uk2++,uk3--,uk += _gr_nScreenWidth, uk1 += _gr_nScreenWidth) {
		long x,y,x1,y1;
	    y1 = (x = (u*f[mask[n]])>>8) * _gr_nScreenWidth;
	    y = (x1 = (v*f[mask[n++]])>>8) * _gr_nScreenWidth;

	    pScr[uk]   = pScr[y+x];
	    pScr[-uk]  = pScr[-y-x];
	    pScr[uk1]  = pScr[y-x];
	    pScr[-uk1] = pScr[-y+x];

		pScr[uk2]  = pScr[y1+x1];
	    pScr[-uk2] = pScr[-y1-x1];
	    pScr[uk3]  = pScr[y1-x1];
	    pScr[-uk3] = pScr[-y1+x1];

	 }
   /*
   for(l1=u*W;u<=0;u++,l1+=W)
		for(v=u,l=l1;v<=0;v++,l+=W) {
		   long x,y,x1,y1;
	       x = (u*fact[m+mask[n]])>>7;
	       x1 = (v*fact[m+mask[n++]])>>7;
		   y1 = x*w;
		   y = x1*w;

	       *(SCR+l+u) = pBuf[y+x];
	       *(SCR-l-u) = pBuf[-y-x];
	       *(SCR+l-u) = pBuf[y-x];
	       *(SCR-l+u) = pBuf[-y+x];


		   *(SCR+l1+v) = pBuf[y1+x1];
	       *(SCR-l1-v) = pBuf[-y1-x1];
	       *(SCR+l1-v) = pBuf[y1-x1];
	       *(SCR-l1+v) = pBuf[-y1+x1];

		}
	*/
}
