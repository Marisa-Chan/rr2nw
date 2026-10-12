#ifndef __LAND_C_H__
	#define __LAND_C_H__

#define M_PI        3.14159265358979323846

#define STANG		80*M_PI/180
#define STLEN       512

#define PYRH_W_ANG      78*M_PI/180
#define PYRH_H_ANG      62*M_PI/180
#define PYRST_W_ANG		80*M_PI/180
#define PYRST_H_ANG		80*M_PI/180

#define sqr(x)		((x)*(x))
#define SIG(x) 		(((x)==0)?0:(((x)<0)?-1:1))
#define SIGINV(x)	(((x)<-1)?-1:(((x)>1)?1:0))
#define ABS(x) 		(((x)>=0)?(x):-(x))

#define MAXLEN  10000

#define PRES    8
#define SQR(x)  (x)*(x)
#define ZLEN    10

#define USHORT(x)   ((unsigned short)(x))

#define ROR(x,n) ( (((unsigned)(x))>>n)+((x) << (32-n)) )

#define ROR16(x,n) ( (((unsigned)(x))>>n)+((x) << (16-n)) )
#define ROL16(x,n) ( ((x)<<n)+((unsigned short)(x) >> (16-n)) )

#define Z2M(m) ((m)*1024./madsh.fCellSize)

#ifndef _VIEW_ORTHO
    #define _VIEW_ORTHO 0
#else
	#undef	_VIEW_ORTHO
	#define	_VIEW_ORTHO 1
#endif


#endif
