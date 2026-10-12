#ifndef __TEXT_H__
    #define __TEXT_H__

//void LineZ(CoordBuf *out,ClipBuf *outc,CoordBuf &st,CoordBuf &en,ClipBuf &stc,ClipBuf &enc);
//void LineMask(CoordBuf &st,CoordBuf &en,ClipBuf &stc,ClipBuf &enc,int Y);
void LineEdgeZ(CoordBufD *out,CoordBuf *st,CoordBuf *en,int delta);
void LineMaskZ(CoordBufD &st,CoordBufD &en,int X,int delta,int Y);
void LineDithMaskZ(CoordBufD &st,CoordBufD &en,int X,int delta,int Y,int pres);
//void LineDithMaskL32(CoordBufD &st,CoordBufD &en,int X,int delta,int Y,int pres=0);
void LineDithMaskL32(CoordBufD &st,CoordBufD &en,int X,int delta,int Y,MapH &bmask);
void LineComressBumpInit(void);

#endif
