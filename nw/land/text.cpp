#include <stdlib.h>
#include "debugext.h"
#include "mathlib.h"
#include "graph.h"
#include "_land.h"


#if _USE_OWNGR
static double lineMaskArray[5000];
#endif
extern "C" int    textBuffer[32];
int    textBuffer[32];

int bumpCompress[522];

extern "C" char bumpCompressa[522];
char bumpCompressa[522];

#define _UC2I(c) ((int)((signed char)(c)))

inline int UC2I(char a) { return _UC2I(a); }
#ifdef	__WATCOMC__
#pragma aux UC2I=\
                "shl eax,24"\
                "sar eax,24"\
                parm [eax] modify [eax] value [eax]
#endif

extern "C" {
	extern void LineMaskL32_ia(int,int,int,int,char *,char *);
}


void LineEdgeZ(CoordBufD *out,CoordBuf *st,CoordBuf *en,int delta){

    if (delta==0){
        out->z=st->iz;
        out->bl=st->bl;
        out->bh=st->bh;
        out->blb=st->blb;
        out->bhb=st->bhb;
        return;
    }

    CoordBufD   *pEdgeBuf=out;

    double      enNZ=en->iz;
    double      stNZ=st->iz;

    double      stepZ=(enNZ-stNZ)/delta;
    double      stZ=stNZ;

    double      stepBl=(double(en->bl)-double(st->bl))/double(delta);
    double      stBl=(st->bl);

    double      stepBh=(double(en->bh)-double(st->bh))/double(delta);
    double      stBh=(st->bh);

    double      stepBlb=(double(en->blb)-double(st->blb))/double(delta);
    double      stBlb=(st->blb);

    double      stepBhb=(double(en->bhb)-double(st->bhb))/double(delta);
    double      stBhb=(st->bhb);


    for (int i=0;i<=delta;i++){
        pEdgeBuf->z=stZ;
        stZ+=stepZ;

        pEdgeBuf->bl=stBl;
        stBl+=stepBl;

        pEdgeBuf->bh=stBh;
        stBh+=stepBh;

        pEdgeBuf->blb=stBlb;
        stBlb+=stepBlb;

        pEdgeBuf->bhb=stBhb;
        stBhb+=stepBhb;

        pEdgeBuf++;
    }

}


#if _USE_OWNGR
void LineDithMaskZ(CoordBufD &st,CoordBufD &en,int X,int delta,int Y){

    double      stepZ=(en.z-st.z)/delta;
    double      stZ=st.z;

    double      stepBl=(en.bl-st.bl)/delta;
    double      stBl=st.bl;

    double      stepBh=(en.bh-st.bh)/delta;
    double      stBh=st.bh;

    double      stepBlb=(en.blb-st.blb)/delta;
    double      stBlb=st.blb;

    double      stepBhb=(en.bhb-st.bhb)/delta;
    double      stBhb=st.bhb;

    int         bl,bh,bld,bhd,blb,bhb,al;
    int         bhbd,blbd;

    for (int i=0;i<delta;i++){
        lineMaskArray[i]=stZ;
        stZ+=stepZ;
    }

    for (i=0;i<delta;i++){

        bl=(stBl)/lineMaskArray[i];
        bh=(stBh)/lineMaskArray[i];

        blb=(stBlb)/lineMaskArray[i];
        bhb=(stBhb)/lineMaskArray[i];

        bld=(stBl*256.)/lineMaskArray[i];
        bhd=(stBh*65536.)/lineMaskArray[i];

        blbd=(stBlb*256.)/lineMaskArray[i];
        bhbd=(stBhb*65536.)/lineMaskArray[i];

        bld&=0x00FF;

        bhd&=0xFF00;

        blbd&=0x00FF;

        bhbd&=0xFF00;

        bhd+=bld;
        bhd=dithMask.mapadr[bhd];

        bhbd+=blbd;
        bhbd=dithMask.mapadr[bhbd];

        bhb<<=8;
        bhb+=blb;
        bhb+=bhbd;

        al= UC2I(bumpMask.mapadr[bhb]);

        double k=1.-(65536.*1024./lineMaskArray[i]-40.*4.)/(512.*4.-40.*4.);
        if (k<0) k=0;
        else if (k>1) k=1;

        al*=k;
        bl+=al;

        if (bl>253) bl=253;
        else
        if (bl<2) bl=2;

        bh<<=8;
        bh+=bl;
        bh+=bhd;

        *(char *)(_gr_pYCache[Y]+X+i)=mapmask.mapadr[bh];

        stBl+=stepBl;
        stBh+=stepBh;

        stBlb+=stepBlb;
        stBhb+=stepBhb;
    }
}
#endif

//extern "C" LineSbScrDivNoQuest();

void LineDithMaskL32_i(char *scrPtr,int l,int ls,int c,int cs)
 {
     char *sptr = scrPtr;
     int
         light       = ROR(l,8),
         color       = c;
              int lightStep   = ROR(ls,8);
     volatile int colorStep   = cs;

#define PUT_LAST(N) sptr[N]=                 \
     mapmask.mapadr[                         \
               dithMask.mapadr[              \
                    ( light&0x00FF ) +       \
                    ((color   )&0xFF00)      \
               ] +                           \
               ((light>>8)&0x00FF) +         \
               ((color>>8) &0xFF00)          \
     ];
#define PUT_1(N) PUT_LAST(N) light += lightStep; color += colorStep;

#define PUT_2(N)     PUT_1(N) PUT_1(N+1)
#define PUT_4(N)     PUT_2(N) PUT_2(N+2)
#define PUT_8(N)     PUT_4(N) PUT_4(N+4)
#define PUT_16(N)    PUT_8(N) PUT_8(N+8)

     PUT_16(0)
     PUT_16(16)

#undef PUT_LAST
#undef PUT_1
#undef PUT_2
#undef PUT_4
#undef PUT_8
#undef PUT_16
 }


inline void LineDithBumpL_i(
                        int l,int ls,
                        int c,int cs,
                        int k,int ks,
                        MapH &bmask,int cnt)
 {
     register int *d       = textBuffer;
     register int light    = l;
     register int color    = c;
     register signed char *  bm   = (signed char *)bmask.mapadr;
     register int *  dth   = dithMask.mapadr ;



#define CALC_BUMP  *d =  ((int)(bm [     \
                   dth[                       \
                     ((light>>8)&0x00FF)+     \
                     ( color    &0xFF00)      \
                   ] +                        \
                   ( (light>>16)) +           \
                   ( (color>>8 )&0xFF00 ) ]))*k;

#define PUT_1 CALC_BUMP light += ls; color += cs; d++; k+=ks;

#define I(N)   case N: PUT_1

     switch(cnt)
     {
     I(31) I(30)
     I(29)  I(28) I(27) I(26) I(25)  I(24) I(23) I(22) I(21)  I(20)
     I(19)  I(18) I(17) I(16) I(15)  I(14) I(13) I(12) I(11)  I(10)
     I( 9)  I( 8) I( 7) I( 6) I( 5)  I( 4) I( 3) I( 2) case 1: CALC_BUMP
     case 0:;
     }

#undef I

#undef CALC_BUMP
#undef PUT_1
 }
//*****************************************************************************
inline void LineBumpL_i(
                        int l,int ls,
                        int c,int cs,
                        int k,int ks,
                        MapH &bmask,int cnt)
 {
     register int *d       = textBuffer;
     register int light    = l;
     register int color    = c;
     register signed char *  bm   = (signed char *)bmask.mapadr;



#define CALC_BUMP  *d =  ((int)(bm [     \
                   ( (light>>16)) +           \
                   ( (color>>8 )&0xFF00 ) ]))*k;

#define PUT_1 CALC_BUMP light += ls; color += cs; d++; k+=ks;

#define I(N)   case N: PUT_1

     switch(cnt)
     {
     I(31) I(30)
     I(29)  I(28) I(27) I(26) I(25)  I(24) I(23) I(22) I(21)  I(20)
     I(19)  I(18) I(17) I(16) I(15)  I(14) I(13) I(12) I(11)  I(10)
     I( 9)  I( 8) I( 7) I( 6) I( 5)  I( 4) I( 3) I( 2) case 1: CALC_BUMP
     case 0:;
     }

#undef I

#undef CALC_BUMP
#undef PUT_1
 }
//*****************************************************************************

inline void LineMaskL_i(
                            int xMask, int xStep,
                            int yMask, int yStep,
                            char *dest,int cnt)
 {
     register int *d       = textBuffer;
     register int light    = xMask;
     register int color    = yMask;
     register char *  scr  = dest;


#define CALC_LIGHT  *d += light
#define PUT_1     CALC_LIGHT; light += xStep; d++;
#define I(N)   case N: PUT_1

     switch(cnt)
     {
     I(31) I(30)
     I(29)  I(28) I(27) I(26) I(25)  I(24) I(23) I(22) I(21)  I(20)
     I(19)  I(18) I(17) I(16) I(15)  I(14) I(13) I(12) I(11)  I(10)
     I( 9)  I( 8) I( 7) I( 6) I( 5)  I( 4) I( 3) I( 2) case 1: CALC_LIGHT;
     case 0:;
     }

#undef CALC_LIGHT
#undef PUT_1
#undef I

    d=textBuffer;
    register char *mptr = mapmask.mapadr;

#define CALC_OK  light = *d;            \
    *scr = mptr[                           \
            bumpCompress[128+(light>>16)]+   \
            ((color>>8)&0xFF00)              \
    ]

#define PUT_1     CALC_OK; color += yStep; ++d;++scr;


#define I(N)   case N: PUT_1

     switch(cnt)
     {
     I(31) I(30)
     I(29)  I(28) I(27) I(26) I(25)  I(24) I(23) I(22) I(21)  I(20)
     I(19)  I(18) I(17) I(16) I(15)  I(14) I(13) I(12) I(11)  I(10)
     I( 9)  I( 8) I( 7) I( 6) I( 5)  I( 4) I( 3) I( 2) case 1: CALC_OK;
     case 0:;
     }

#undef I

#undef CALC_OK
#undef PUT_1
 }

//*****************************************************************************

inline void LineDithMaskL_i(
                            int xMask, int xStep,
                            int yMask, int yStep,
                            char *dest,int cnt)
 {
     register int *d       = textBuffer;
     register int light    = xMask;
     register int color    = yMask;
     register char *  scr  = dest;


#define CALC_LIGHT  *d += light
#define PUT_1     CALC_LIGHT; light += xStep; d++;
#define I(N)   case N: PUT_1

     switch(cnt)
     {
     I(31) I(30)
     I(29)  I(28) I(27) I(26) I(25)  I(24) I(23) I(22) I(21)  I(20)
     I(19)  I(18) I(17) I(16) I(15)  I(14) I(13) I(12) I(11)  I(10)
     I( 9)  I( 8) I( 7) I( 6) I( 5)  I( 4) I( 3) I( 2) case 1: CALC_LIGHT;
     case 0:;
     }

#undef CALC_LIGHT
#undef PUT_1
#undef I

    d=textBuffer;

    register char *mptr = mapmask.mapadr;
    register int *  dth   = dithMask.mapadr;

#define CALC_OK  light = *d;               \
    *scr = mptr[                              \
            dth[                              \
                     ((light>>8)&0x00FF)+     \
                     ( color    &0xFF00)      \
            ] +                               \
            bumpCompress[128+(light>>16)]+    \
            ((color>>8)&0xFF00)               \
    ]

#define PUT_1     CALC_OK ; color += yStep; ++d; ++scr;


#define I(N)   case N: PUT_1

     switch(cnt)
     {
     I(31) I(30)
     I(29)  I(28) I(27) I(26) I(25)  I(24) I(23) I(22) I(21)  I(20)
     I(19)  I(18) I(17) I(16) I(15)  I(14) I(13) I(12) I(11)  I(10)
     I( 9)  I( 8) I( 7) I( 6) I( 5)  I( 4) I( 3) I( 2) case 1: CALC_OK;
     case 0:;
     }

#undef I

#undef CALC_OK
#undef PUT_1
 }

 /**********************************************
  * Формируем массив яркостей BUMP-a с dither-ом
  */
inline void LineDithBumpL32_i(
                        int l,int ls,
                        int c,int cs,
                        int k,int ks,
                        MapH &bmask)
 {
     register int light    = l;
     register int color    = c;
     register signed char *  bm   = (signed char *)bmask.mapadr;
     register int *  dth   = dithMask.mapadr ;


     ls<<=1;
     cs<<=1;
     ks<<=1;

#define CALC_BUMP(N)  textBuffer[(N)*2] = textBuffer[(N)*2+1] = ((int)(bm [     \
                   dth[                       \
                     ((light>>8)&0x00FF)+     \
                     ( color    &0xFF00)      \
                   ] +                        \
                   ( (light>>16)) +           \
                   ( (color>>8 )&0xFF00 ) ]))*k;

#define PUT_1(N)     CALC_BUMP(N); light += ls; color += cs; k += ks;
#define PUT_2(N)     PUT_1(N) PUT_1(N+1)
#define PUT_4(N)     PUT_2(N) PUT_2(N+2)
#define PUT_8(N)     PUT_4(N) PUT_4(N+4)
#define PUT_16(N)    PUT_8(N) PUT_8(N+8)
     PUT_16(0)
     //PUT_16(16)
#undef PUT_1
#undef PUT_2
#undef PUT_4
#undef PUT_8
#undef PUT_16
#undef CALC_BUMP
 }

 /**********************************************
  * Формируем массив яркостей BUMP-a
  */
inline void LineBumpL32_i(
                        int l,int ls,
                        int c,int cs,
                        int k,int ks,
                        MapH &bmask)
 {
     register int light    = l;
     register int color    = c;
     register signed char *  bm   = (signed char *)bmask.mapadr;
     register int comp     = k;

#define CALC_BUMP(N)  textBuffer[N] =  ((int)(bm [     \
                   ( (light>>16)) +           \
                   ( (color>>8 )&0xFF00 ) ]))*comp

#define PUT_1(N)     CALC_BUMP(N); light += ls; color += cs; comp += ks;
#define PUT_2(N)     PUT_1(N) PUT_1(N+1)
#define PUT_4(N)     PUT_2(N) PUT_2(N+2)
#define PUT_8(N)     PUT_4(N) PUT_4(N+4)
#define PUT_16(N)    PUT_8(N) PUT_8(N+8)
     PUT_16(0)
     PUT_16(16)
#undef PUT_1
#undef PUT_2
#undef PUT_4
#undef PUT_8
#undef PUT_16
#undef CALC_BUMP
 }


inline void LineMaskL32_i(
                            int xMask, int xStep,
                            int yMask, int yStep,
                            char *dest)
 {
     register int light    = xMask;
     register int color    = yMask;
     register char *  scr  = dest;
     register int l;
     register char *mptr = mapmask.mapadr;

#define CALC_OK(N)  l = textBuffer[N]+light; l>>=16;  \
    scr[N] = mptr[                           \
            bumpCompress[128+l]+   \
            ((color>>8)&0xFF00)              \
    ]
#define PUT_1(N)     CALC_OK(N); color += yStep; light+=xStep;
#define PUT_2(N)     PUT_1(N) PUT_1(N+1)
#define PUT_4(N)     PUT_2(N) PUT_2(N+2)
#define PUT_8(N)     PUT_4(N) PUT_4(N+4)
#define PUT_16(N)    PUT_8(N) PUT_8(N+8)
     PUT_16(0)
     PUT_16(16)
#undef PUT_1
#undef PUT_2
#undef PUT_4
#undef PUT_8
#undef PUT_16
#undef CALC_OK
#undef CALC_LIGHT
 }

inline void LineDithMaskL32_i(
                            int xMask, int xStep,
                            int yMask, int yStep,
                            char *dest
                            )
 {
     register int light    = xMask;
     register int color    = yMask;
     register char *  scr  = dest;
     register int l;
     register int *  dth   = dithMask.mapadr;

#define CALC_OK(N)  l = textBuffer[N]+light;  l>>=8;  \
    scr[N] = mapmask.mapadr[                          \
            dth[ (l&0x00FF)+                          \
                 ( color&0xFF00)                      \
            ] +                                       \
            bumpCompress[128+(l>>8)]+                 \
            ((color>>8)&0xFF00)                       \
    ]
#define PUT_1(N)     CALC_OK(N); light+= xStep; color += yStep;
#define PUT_2(N)     PUT_1(N) PUT_1(N+1)
#define PUT_4(N)     PUT_2(N) PUT_2(N+2)
#define PUT_8(N)     PUT_4(N) PUT_4(N+4)
#define PUT_16(N)    PUT_8(N) PUT_8(N+8)
     PUT_16(0)
     PUT_16(16)
#undef PUT_1
#undef PUT_2
#undef PUT_4
#undef PUT_8
#undef PUT_16
#undef CALC_LIGHT
#undef CALC_OK
 }

/*************************************************************/
void LineComressBumpInit(void)
 {
    int i;
    for (i=0;i<=130;i++) bumpCompress[i] = 2;
    for (;i<(130+253);i++) bumpCompress[i] = i-129;
    for (;i<522;i++) bumpCompress[i] = 253;

    for (i=0;i<=130;i++) bumpCompressa[i] = 2;
    for (;i<(130+253);i++) bumpCompressa[i] = (char)(i-129);
    for (;i<522;i++) bumpCompressa[i] = 253;

 }

 //
 // Вычисляет конечный результат складывания bump и текстуры
 //


void LineDithMaskBumpL32_i(  char *scrPtr,
                             int u,int us,
                             int v,int vs,
                             int kl,int kr,int ks,
                             int l,int ls,
                             int c,int cs,
                             MapH &bmask)
 {

    if ( kl==0 && kr==0 ){
        LineBumpL32_i(u,us, v,vs, kl,ks,bmask);
        LineDithMaskL32_i(l,ls,c,cs,scrPtr);
    }
    else if (kl != 0 && kr != 0){
        LineDithBumpL32_i(u,us, v,vs, kl,ks,bmask);
        //LineMaskL32_i(l,ls,c,cs,scrPtr);
		#if _USE_OWNGR
	        LineMaskL32_ia(l,ls,c,cs,scrPtr,mapmask.mapadr);
		#endif
    }
    else{
        LineDithBumpL32_i(u,us, v,vs, kl,ks,bmask);
        LineDithMaskL32_i(l,ls,c,cs,scrPtr);
    }

 }



void LineDithMaskBumpL_i(  char *scrPtr,
                             int u,int us,
                             int v,int vs,
                             int kl,int kr,int ks,
                             int l,int ls,
                             int c,int cs,
                             int cnt,MapH &bmask)
 {

    if ( kl==0 && kr==0 ){
        LineBumpL_i(u,us, v,vs, kl,ks,bmask,cnt);
        LineDithMaskL_i(l,ls,c,cs,scrPtr,cnt);
    }
    else if (kl != 0 && kr != 0){
        LineDithBumpL_i(u,us, v,vs, kl,ks,bmask,cnt);
        LineMaskL_i(l,ls,c,cs,scrPtr,cnt);
    }
    else{
        LineDithBumpL_i(u,us, v,vs, kl,ks,bmask,cnt);
        LineDithMaskL_i(l,ls,c,cs,scrPtr,cnt);
    }
 }



#define _INT(x) ((int)(x))

#define BDIST (65536.*1024.)/(512.*4.)

#if _USE_OWNGR
void LineDithMaskL32(CoordBufD &st,CoordBufD &en,int X,int delta,int Y,MapH &bmask)
 {
    CoordBuf    left,right;
    delta++;

    int         dNum=delta/32;
    int         ost=delta%32;

    double      stepZ=(en.z-st.z)*32.0/delta;
    double      stZ=st.z;

    double      stepBl=(en.bl-st.bl)*32.0*65536./delta;
    double      stBl=st.bl*65536.;

    double      stepBh=(en.bh-st.bh)*32.0*65536./delta;
    double      stBh=st.bh*65536.;

    double      stepBlb=(en.blb-st.blb)*32.*65536./delta;
    double      stBlb=st.blb*65536.;

    double      stepBhb=(en.bhb-st.bhb)*32.*65536./delta;
    double      stBhb=st.bhb*65536.;


    for (int i=0;i<=dNum;i++)
    {
        lineMaskArray[i]=stZ;
        stZ+=stepZ;
    }

    char *sptr=(char *)(_gr_pYCache[Y]+X);
    int blbdl,
        bhbdl,
        blbdr,
        bhbdr;
    double kl,kr;

    blbdl = D2INT(stBlb/lineMaskArray[0]);
    bhbdl = D2INT(stBhb/lineMaskArray[0]);

    left.bl=D2INT(stBl/lineMaskArray[0]);
    left.bh=D2INT(stBh/lineMaskArray[0]);

    kl=1.-(65536.*1024./lineMaskArray[0]-40.*4.)/(512.*4.-40.*4.);
    if (kl<0) kl=0;
    kl*=65536.;


    for (i=0;i<dNum;i++,sptr+=32)
    {
        double denum = lineMaskArray[i+1];
        blbdr = D2INT((stBlb+stepBlb)/denum);
        bhbdr = D2INT((stBhb+stepBhb)/denum);

        right.bl=D2INT((stBl+stepBl)/denum);
        right.bh=D2INT((stBh+stepBh)/denum);

        kr=1.-(65536.*1024./denum-40.*4.)/(512.*4.-40.*4.);
        if (kr<0) kr=0;
        kr*=65536.;


        LineDithMaskBumpL32_i(sptr,
                                    blbdl,_INT(blbdr-blbdl)>>5,
                                    bhbdl,_INT(bhbdr-bhbdl)>>5,
                                    D2INT(kl),D2INT(kr),D2INT(kr-kl)>>5,
                                    left.bl,  _INT(right.bl-left.bl)>>5,
                                    left.bh,  _INT(right.bh-left.bh)>>5,
                                    bmask
                                    );

        stBl+=stepBl;
        stBh+=stepBh;

        stBlb+=stepBlb;
        stBhb+=stepBhb;

        blbdl = blbdr;
        bhbdl = bhbdr;

        left.bl = right.bl;
        left.bh = right.bh;

        kl = kr;
    }

    if (ost != 0)
    {
    	right.x=left.x+ost;
    	right.bl=D2INT(en.bl*65536./en.z);
    	right.bh=D2INT(en.bh*65536./en.z);

    	blbdr=D2INT(en.blb*65536./en.z);
    	bhbdr=D2INT(en.bhb*65536./en.z);

        kr=1.-(65536.*1024./en.z-40.*4.)/(512.*4.-40.*4.);
        if (kr<0) kr=0;
        kr*=65536.;

        LineDithMaskBumpL_i(sptr,
                                blbdl,_INT(blbdr-blbdl)/ost,
                                bhbdl,_INT(bhbdr-bhbdl)/ost,
                                D2INT(kl),D2INT(kr),D2INT(kr-kl)/ost,
                                left.bl,  _INT(right.bl-left.bl)/ost,
                                left.bh,  _INT(right.bh-left.bh)/ost,ost,
                                bmask
                                );

    }
 }

#endif
