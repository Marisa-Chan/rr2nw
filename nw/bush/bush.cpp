#ifndef	__BUSH_CPP__
#include"_bush.h"
#endif


//#include <stdlib.h>
//#include "brend.h"


void bsh_SingleLineLRH_nclp(int x0,int y0, int x1, int y1)
 {
    register byte *dest = &(r_YCACHE[y0][x0]);
             int x  = x0;
             int dx = x1-x+1;
             int dy = y1-y0+1;
             int s  = 0;
    volatile int sign = r_VSCR_WIDTH;
    byte  col  = b_TBush::m_singleCol;

    if( dy < 1 )
    {
         sign = -r_VSCR_WIDTH;
         dy   = 2-dy;
    }

#define r_PUT_ONE *dest = col; ++dest;s += dy; if( s>=dx ) {dest += sign; s -= dx; }
    switch( dx )
    {
    case 7: r_PUT_ONE
    case 6: r_PUT_ONE
    case 5: r_PUT_ONE
    case 4: r_PUT_ONE
    case 3: r_PUT_ONE
    case 2: r_PUT_ONE
    case 1: *dest = col;
    case 0: return;
    default:
            for(;x<=x1; ++x )
            {
                r_PUT_ONE
            }
    }
#undef r_PUT_ONE
 }

void bsh_SingleLineRLH_nclp(int x0,int y0, int x1, int y1)
 {
    register byte *dest = &(r_YCACHE[y0][x0]);
             int x  = x0;
             int dx = x0-x1+1;
             int dy = y1-y0+1;
             int s  = 0;
    volatile int sign = r_VSCR_WIDTH;
    byte  col  = b_TBush::m_singleCol;

    if( dy < 1 )
    {
         sign = -r_VSCR_WIDTH;
         dy   = 2-dy;
    }

#define r_PUT_ONE *dest = col; --dest;s += dy; if( s>=dx ) {dest += sign; s -= dx; }
    switch( dx )
    {
    case 7: r_PUT_ONE
    case 6: r_PUT_ONE
    case 5: r_PUT_ONE
    case 4: r_PUT_ONE
    case 3: r_PUT_ONE
    case 2: r_PUT_ONE
    case 1: *dest = col;
    case 0: return;
    default:
            for(;x>=x1;--x )
            {
                r_PUT_ONE
            }
    }
#undef r_PUT_ONE
 }

void bsh_SingleLineLRH_clip(int x0,int y0, int x1, int y1)
 {
    int x=x0,y=y0,s=0;
    int dx = x1-x0+1,dy = y1-y0+1, sign=1;

    if( dy<1 )
    {
        sign = -1;
        dy = 2-dy;
    }

    for( ; x<=x1; ++x)
    {
         if( x>=r_CLIP2D.left && x<r_CLIP2D.right &&
             y>=r_CLIP2D.top  && y<r_CLIP2D.bottom )
              r_YCACHE[y][x] = b_TBush::m_singleCol;
         s += dy;
         if( s>=dx )
         {
              s-=dx;
              y+=sign;
         }
    }
 }

void bsh_SingleLineRLH_clip(int x0,int y0, int x1, int y1)
 {
    int x=x0,y=y0,s=0;
    int dx = x0-x1+1,dy = y1-y0+1, sign=1;

    if( dy<1 )
    {
        sign = -1;
        dy = 2-dy;
    }

    for( ; x>=x1; --x)
    {
         if( x>=r_CLIP2D.left && x<r_CLIP2D.right &&
             y>=r_CLIP2D.top  && y<r_CLIP2D.bottom )
              r_YCACHE[y][x] = b_TBush::m_singleCol;
         s += dy;
         if( s>=dx )
         {
              s-=dx;
              y+=sign;
         }
    }
 }

//=======================================================================

void bsh_SingleLineLRVUD_clip( int x0,int y0, int x1, int y1 )
 {
    int x=x0,y=y0,s=0;
    int dx = x1-x0+1,dy = y1-y0+1;

    for( ; y<=y1; ++y)
    {
         if( x>=r_CLIP2D.left && x<r_CLIP2D.right &&
             y>=r_CLIP2D.top  && y<r_CLIP2D.bottom )
              r_YCACHE[y][x] = b_TBush::m_singleCol;
         s += dx;
         if( s>=dy )
         {
              s-=dy;
              ++x;
         }
    }
 }

void bsh_SingleLineRLVUD_clip( int x0,int y0, int x1, int y1 )
 {
    int x=x0,y=y0,s=0;
    int dx = x0-x1+1,dy = y1-y0+1;

    for( ; y<=y1; ++y)
    {
         if( x>=r_CLIP2D.left && x<r_CLIP2D.right &&
             y>=r_CLIP2D.top  && y<r_CLIP2D.bottom )
              r_YCACHE[y][x] = b_TBush::m_singleCol;
         s += dx;
         if( s>=dy )
         {
              s-=dy;
              --x;
         }
    }
 }

void bsh_SingleLineLRVDU_clip( int x0,int y0, int x1, int y1 )
 {
    register int x=x0,
                 y=y0,
                 s=0;

    int dx = x1-x +1,
        dy = y -y1+1;

    for( ; y>=y1; --y)
    {
         if( x>=r_CLIP2D.left && x<r_CLIP2D.right &&
             y>=r_CLIP2D.top  && y<r_CLIP2D.bottom )
              r_YCACHE[y][x] = b_TBush::m_singleCol;
         s += dx;
         if( s>=dy )
         {
              s-=dy;
              ++x;
         }
    }
 }

void bsh_SingleLineRLVDU_clip( int x0,int y0, int x1, int y1 )
 {
    int x=x0,y=y0,s=0;
    int dx = x0-x1+1,dy = y0-y1+1;

    for( ; y>=y1; --y)
    {
         if( x>=r_CLIP2D.left && x<r_CLIP2D.right &&
             y>=r_CLIP2D.top  && y<r_CLIP2D.bottom )
              r_YCACHE[y][x] = b_TBush::m_singleCol;
         s += dx;
         if( s>=dy )
         {
              s-=dy;
              --x;
         }
    }
 }

//------------------------------------------------------------
//void bsh_SingleLineLRVUD_nclp( int x0,int y0, int x1, int y1 );
//void bsh_SingleLineRLVUD_nclp( int x0,int y0, int x1, int y1 );
void bsh_SingleLineLRVDU_nclp( int x0,int y0, int x1, int y1 )
 {
     int s = 0;
     byte *dest = &(r_YCACHE[y0][x0]);
     int   sign = r_VSCR_WIDTH;
     int y = y0,
         dx= x1-x0+1,
         dy= y0-y1+1;
     byte col = b_TBush::m_singleCol;
#define r_PUT_ONE *dest=col; dest-=sign; s+=dx;if(s>=dy){s-=dy;++dest;}
     switch( dy )
     {
     case 0: return;
     case 7: r_PUT_ONE
     case 6: r_PUT_ONE
     case 5: r_PUT_ONE
     case 4: r_PUT_ONE
     case 3: r_PUT_ONE
     case 2: r_PUT_ONE
     case 1: *dest = col;
             return;
     default:
             for(; y>=y1 ; --y )
             {
                  r_PUT_ONE
             }
     }
#undef r_PUT_ONE
 }

void bsh_SingleLineRLVDU_nclp( int x0,int y0, int x1, int y1 )
 {
     int s = 0;
     byte *dest = &(r_YCACHE[y0][x0]);
     int   sign = r_VSCR_WIDTH;
     int y = y0,
         dx= x0-x1+1,
         dy= y0-y1+1;
     byte col = b_TBush::m_singleCol;
#define r_PUT_ONE *dest=col; dest-=sign; s+=dx;if(s>=dy){s-=dy;--dest;}
     switch( dy )
     {
     case 0: return;
     case 7: r_PUT_ONE
     case 6: r_PUT_ONE
     case 5: r_PUT_ONE
     case 4: r_PUT_ONE
     case 3: r_PUT_ONE
     case 2: r_PUT_ONE
     case 1: *dest = col;
             return;
     default:
             for(; y>=y1 ; --y )
             {
                  r_PUT_ONE
             }
     }
#undef r_PUT_ONE
 }

//------------------------------------------------------------
/*
 *       0
 *    +-------+
 *    |       |
 *  3 |       | 1
 *    |_______|
 *        2
 */
void bsh_PutRect( int xcoord, int ycoord, int sq_size )
 {
    if( sq_size<=1 )
    {
        if( xcoord>=r_CLIP2D.left  &&
            xcoord <r_CLIP2D.right &&
            ycoord>=r_CLIP2D.top   &&
            ycoord <r_CLIP2D.bottom)
             r_YCACHE[ycoord][xcoord] = b_TBush::m_singleCol;
        return;
    }
    register int pos = sq_size>>1;
    register byte *sour;
    register byte *dest;

    sour = (byte*)(xcoord-pos);
    pos  = ycoord-pos;
#define R0 ((int)(sour))
#define R1  pos

    if(  (R0>=r_CLIP2D.right)
       ||(R1>=r_CLIP2D.bottom) )
           return;

    volatile   int x0 = R0;
    volatile   int y0 = R1;

    sour = (byte*)(R0+sq_size-1);
    pos  = R1+sq_size-1;
    if(   (R0<r_CLIP2D.left)
        ||(R1<r_CLIP2D.top) )
        return;

    volatile   int x1 = R0;
    volatile   int y1 = R1;

    volatile   int temp;
    volatile   int temp2;
               int i;
               int step;

#define r_DRAW_V  { *dest = sour[pos>>16]; \
                    dest += r_VSCR_WIDTH;  \
                    pos  += step; }

#define r_DRAW_H  { *dest = sour[pos>>16]; \
                    ++dest;                \
                    pos  += step; }

#define r_SET_ARG(p,x,y,s) pos=(p); \
                           dest=&(r_YCACHE[(y)][(x)]); \
                           sour=b_TBush::m_bushRect.data##s

    switch(
            ( x0 >= r_CLIP2D.left  )      |
            ((y0 >= r_CLIP2D.top   )  <<2)|
            ((x1 <  r_CLIP2D.right )  <<1)|
            ((y1 <  r_CLIP2D.bottom)  <<3)
            )
    {
    case 15:
            if( sq_size<r_MAX_RECT_SIZE  )
            {
                 b_TBush::CompileRectAr[r_RECT_MINUS+sq_size]
                               ( &(r_YCACHE[y0][x0]), r_VSCR_WIDTH);
                 return;
            }
            step = b_TBush::m_bushRect.m_size / sq_size;
            r_SET_ARG( 0, x0,y0, 0 );
            for( i = sq_size; i>0; --i )
                 r_DRAW_H

            r_SET_ARG( 0, x0,y1, 2 );
            for( i = sq_size; i>0; --i )
                 r_DRAW_H

            r_SET_ARG( step, x0,y0+1, 3 );
            for( i = sq_size-2; i>0; --i )
                 r_DRAW_V

            r_SET_ARG( step, x1, y0+1, 1 );
            for( i = sq_size-2; i>0; --i )
                 r_DRAW_V
            return;
    case 1 : // x0 >= r_CLIP2D.left                                     12345
             step = b_TBush::m_bushRect.m_size / sq_size;
             r_SET_ARG( step*(r_CLIP2D.top - y0),  x0,r_CLIP2D.top, 3 );
             for( i = r_CLIP2D.bottom-r_CLIP2D.top; i>0; --i )
                  r_DRAW_V
             return;

    case 2:  // x1 <  r_CLIP2D.right                                    12345
             step = b_TBush::m_bushRect.m_size / sq_size;
             r_SET_ARG( step*(r_CLIP2D.top - y0),  x1,r_CLIP2D.top, 1 );
             for( i = r_CLIP2D.bottom-r_CLIP2D.top; i>0; --i )
                  r_DRAW_V
             return;

    case 3:  // x0 >= r_CLIP2D.left       |   |                         12345
             // x1 <  r_CLIP2D.right      |   |
             step = b_TBush::m_bushRect.m_size / sq_size;
             temp = step*(r_CLIP2D.top - y0);
             r_SET_ARG( temp,  x0,r_CLIP2D.top, 3 );
             for( i = r_CLIP2D.bottom-r_CLIP2D.top; i>0; --i )
                  r_DRAW_V

             r_SET_ARG( temp,  x1,r_CLIP2D.top, 1 );
             for( i = r_CLIP2D.bottom-r_CLIP2D.top; i>0; --i )
                  r_DRAW_V
             return;

    case 4:  // y0 >= r_CLIP2D.top                                      12345
             step = b_TBush::m_bushRect.m_size / sq_size;
             r_SET_ARG( step*(r_CLIP2D.left - x0),  r_CLIP2D.left,y0, 0 );
             for( i = r_CLIP2D.right-r_CLIP2D.left; i>0; --i )
                  r_DRAW_H
             return;

    case 5:  // x0 >= r_CLIP2D.left       +---                          12345
             // y0 >= r_CLIP2D.top        |
             step = b_TBush::m_bushRect.m_size / sq_size;
             r_SET_ARG( 0, x0,y0, 0 );
             for( i = r_CLIP2D.right-x0; i>0; --i )
                  r_DRAW_H

             r_SET_ARG( step, x0,y0+1,  3 );
             for( i = r_CLIP2D.bottom-y0-1; i>0; --i )
                  r_DRAW_V
             return;

    case 6:  // x1 <  r_CLIP2D.right  ----+                             12345
             // y0 >= r_CLIP2D.top        |
             step = b_TBush::m_bushRect.m_size / sq_size;
             r_SET_ARG( step*(r_CLIP2D.left-x0), r_CLIP2D.left,y0,  0 );
             for( i = x1-r_CLIP2D.left+1; i>0; --i )
                  r_DRAW_H

             r_SET_ARG( step, x1,y0+1,  1 );
             for( i = r_CLIP2D.bottom-y0-1; i>0; --i )
                  r_DRAW_V
             return;

    case 7:  // x0 >= r_CLIP2D.left    _____
             // x1 <  r_CLIP2D.right   |   |                            12345
             // y0 >= r_CLIP2D.top     |   |
             step = b_TBush::m_bushRect.m_size / sq_size;
             r_SET_ARG( 0, x0,y0, 0 );
             for( i = sq_size; i>0; --i )
                  r_DRAW_H

             r_SET_ARG( step, x0,y0+1, 3 );
             for( i = r_CLIP2D.bottom-y0-1; i>0; --i )
                  r_DRAW_V

             r_SET_ARG( step, x1,y0+1, 1 );
             for( i = r_CLIP2D.bottom-y0-1; i>0; --i )
                  r_DRAW_V
             return;

    case 8 : // видна нижн€€ граница ------------------------------------
             // y1 <  r_CLIP2D.bottom                                   12345
             step = b_TBush::m_bushRect.m_size / sq_size;
             r_SET_ARG( step*(r_CLIP2D.left-x0), r_CLIP2D.left,y1, 2 );
             for( i = r_CLIP2D.right-r_CLIP2D.left; i>0; --i )
                  r_DRAW_H
             return;

    case 9: // x0 >= r_CLIP2D.left      |                               12345
            // y1 <  r_CLIP2D.bottom    |______
            step = b_TBush::m_bushRect.m_size / sq_size;
            r_SET_ARG( 0, x0,y1, 2 );
            for( i = r_CLIP2D.right-x0; i>0; --i )
                 r_DRAW_H

            r_SET_ARG( step*(r_CLIP2D.top-y0), x0,r_CLIP2D.top, 3 );
            for( i = y1-r_CLIP2D.top; i>0; --i )
                 r_DRAW_V
            return;

    case 10:// x1 <  r_CLIP2D.right     |                               12345
            // y1 <  r_CLIP2D.bottom  __|
            step = b_TBush::m_bushRect.m_size / sq_size;
            r_SET_ARG( step*(r_CLIP2D.left-x0),  r_CLIP2D.left,y1, 2 );
            for( i = x1-r_CLIP2D.left+1; i>0; --i )
                 r_DRAW_H

            r_SET_ARG( step*(r_CLIP2D.top-y0),  x1,r_CLIP2D.top,  1 );
            for( i = y1-r_CLIP2D.top; i>0; --i )
                 r_DRAW_V
            return;

    case 11:
            // x0 >= r_CLIP2D.left                                      12345
            // x1 <  r_CLIP2D.right    |    |
            // y1 <  r_CLIP2D.bottom   |____|
            step = b_TBush::m_bushRect.m_size / sq_size;
            r_SET_ARG( 0, x0,y1, 2 );
            for( i = sq_size; i>0; --i )
                 r_DRAW_H

            temp = step*(r_CLIP2D.top-y0);
            temp2= y1-r_CLIP2D.top;
            r_SET_ARG( temp, x0,r_CLIP2D.top, 3 );
            for( i = temp2; i>0; --i )
                 r_DRAW_V

            r_SET_ARG( temp, x1,r_CLIP2D.top, 1 );
            for( i = temp2; i>0; --i )
                 r_DRAW_V
            return;

    case 12:// y0 >= r_CLIP2D.top     ------                            12345
            // y1 <  r_CLIP2D.bottom  ------
            step = b_TBush::m_bushRect.m_size / sq_size;
            temp = step*(r_CLIP2D.left-x0);
            temp2= r_CLIP2D.right-r_CLIP2D.left;

            r_SET_ARG( temp, r_CLIP2D.left,y0, 0 );
            for( i = temp2; i>0; --i )
                 r_DRAW_H

            r_SET_ARG( temp, r_CLIP2D.left,y1, 2 );
            for( i = temp2; i>0; --i )
                 r_DRAW_H
            return;

    case 13:// x0 >= r_CLIP2D.left       ____                           12345
            // y0 >= r_CLIP2D.top       |
            // y1 <  r_CLIP2D.bottom    |____
            step = b_TBush::m_bushRect.m_size / sq_size;
            temp2 = r_CLIP2D.right-x0;
            r_SET_ARG( 0, x0,y0, 0 );
            for( i = temp2; i>0; --i )
                 r_DRAW_H

            r_SET_ARG( 0, x0,y1, 2 );
            for( i = temp2; i>0; --i )
                 r_DRAW_H

            r_SET_ARG( step, x0,y0+1, 3 );
            for( i = sq_size-2; i>0; --i )
                 r_DRAW_V
            return;

    case 14:// x1 <  r_CLIP2D.right    ______                           12345
            // y0 >= r_CLIP2D.top           |
            // y1 <  r_CLIP2D.bottom   -----+
            step = b_TBush::m_bushRect.m_size / sq_size;
            temp = step*(r_CLIP2D.left-x0);
            temp2= x1-r_CLIP2D.left+1;

            r_SET_ARG( temp, r_CLIP2D.left,y0, 0 );
            for( i = temp2; i>0; --i )
                 r_DRAW_H

            r_SET_ARG( temp, r_CLIP2D.left,y1, 2 );
            for( i = temp2; i>0; --i )
                 r_DRAW_H

            r_SET_ARG( step, x1,y0+1, 1 );
            for( i = sq_size-2; i>0; --i )
                 r_DRAW_V

            return;
    case 0 : return;
    }
 }

void bsh_PutLineRL(int x0,int y0, int x1, int y1, int w0, int w1 )
 {
    register int w;
    register int ws;
    register int m;
    register int c;
             int s;
             int wd   = w1-w0;
             int wSig = 0;
    volatile int xSub = x0-x1;
    volatile int ySub = y1-y0;

    w  = w0;
    ws = (w+w1) >> 1;
    if( wd > 0 )
    {
         wSig = 1;
         ++wd;
    }
    else
    if( wd < 0 )
    {
         wSig = -1;
         wd = 1-wd;
    }

    if( xSub > abs(ySub) )
    {
         if( w<=1 && w1<=1 )
         {
             b_TBush::SingleLineRLH( x0, y0, x1, y1 );
             return;
         }

         m = x0;
         c = y0<<16;
         ++xSub;
         s = (ySub<<16) / xSub;

         if(    b_TBush::m_visAll    //
            && (w0<r_MAX_RECT_SIZE)  //  Compile rect out
            && (w1<r_MAX_RECT_SIZE)) //
         {
              for(; m>=x1 ; --m, c += s )
              {
                   b_TBush::CompileRectAr[r_RECT_MINUS+w]
                              ( &(r_YCACHE[(c>>16)-(w>>1)][m-(w>>1)]), r_VSCR_WIDTH);
                   ws += wd;
                   while( ws >= xSub )
                   {
                       ws -= xSub;
                       w  += wSig;
                       b_TBush::CompileRectAr[r_RECT_MINUS+w]
                              ( &(r_YCACHE[(c>>16)-(w>>1)][m-(w>>1)]), r_VSCR_WIDTH);
                   }
              }
              return;
         }


         for(; m>=x1 ; --m, c += s )
         {
              bsh_PutRect( m, c>>16, w );
              ws += wd;
              while( ws >= xSub )
              {
                  ws -= xSub;
                  w  += wSig;
                  bsh_PutRect( m, c>>16, w );
              }
         }
    }
    else
    {
        m = y0;
        c = x0<<16;

        if( ySub>=0 )
        {

             if( w<=1 && w1<=1 )
             {
                  bsh_SingleLineRLVUD_clip(x0,y0,x1,y1);
                  return;
             }

             ++ySub;
             s = -(xSub<<16) / ySub;

             if(    b_TBush::m_visAll    //
                && (w0<r_MAX_RECT_SIZE)  //  Compile rect out
                && (w1<r_MAX_RECT_SIZE)) //
             {
                for(; m<=y1 ; ++m)
                {
                     b_TBush::CompileRectAr[r_RECT_MINUS+w]
                                ( &(r_YCACHE[m-(w>>1)][(c>>16)-(w>>1)]), r_VSCR_WIDTH);

                     ws += wd;
                     while( ws >= ySub )
                     {
                         ws -= ySub;
                         w  += wSig;
                         b_TBush::CompileRectAr[r_RECT_MINUS+w]
                                    ( &(r_YCACHE[m-(w>>1)][(c>>16)-(w>>1)]), r_VSCR_WIDTH);
                     }
                     c += s;
                }
                return;
             }

             for(; m<=y1 ; ++m)
             {
                  bsh_PutRect( c>>16, m, w );

                  ws += wd;
                  while( ws >= ySub )
                  {
                      ws -= ySub;
                      w  += wSig;
                      bsh_PutRect( c>>16, m, w );
                  }
                  c += s;
             }
             return;
        }
             if( w<=1 && w1<=1 )
             {
                  b_TBush::SingleLineRLVDU( x0, y0, x1, y1 );
                  return;
             }

             ySub = 1-ySub;
             s = -(xSub<<16) / ySub;

             if(    b_TBush::m_visAll    //
                && (w0<r_MAX_RECT_SIZE)  //  Compile rect out
                && (w1<r_MAX_RECT_SIZE)) //
             {
                for(; m>=y1 ; --m)
                {
                     b_TBush::CompileRectAr[r_RECT_MINUS+w]
                                ( &(r_YCACHE[m-(w>>1)][(c>>16)-(w>>1)]), r_VSCR_WIDTH);
                     ws += wd;
                     while( ws >= ySub )
                     {
                         ws -= ySub;
                         w  += wSig;
                         b_TBush::CompileRectAr[r_RECT_MINUS+w]
                                    ( &(r_YCACHE[m-(w>>1)][(c>>16)-(w>>1)]), r_VSCR_WIDTH);
                     }
                     c += s;
                }
                return;
             }

             for(; m>=y1 ; --m)
             {
                  bsh_PutRect( c>>16, m, w );
                  ws += wd;
                  while( ws >= ySub )
                  {
                      ws -= ySub;
                      w  += wSig;
                      bsh_PutRect( c>>16, m, w );
                  }
                  c += s;
             }
    }
 }

void bsh_PutLineLR(int x0,int y0, int x1, int y1, int w0, int w1 )
 {
     register int w;
     register int ws;
     register int m;
     register int c;
              int s;
              int wd   = w1-w0;
              int wSig = 0;
     volatile int xSub = x1-x0;
     volatile int ySub = y1-y0;

     w  = w0;
     ws = (w+w1) >> 1;
     if( wd > 0 )
     {
          wSig = 1;
          ++wd;
     }
     else
     if( wd < 0 )
     {
          wSig = -1;
          wd = 1-wd;
     }

     if( xSub > abs(ySub) )
     {
          if( w<=1 && w1<=1 )
          {
              b_TBush::SingleLineLRH( x0, y0, x1, y1 );
              return;
          }

          m = x0;
          c = y0<<16;
          ++xSub;
          s = (ySub<<16) / xSub;

          if(    b_TBush::m_visAll    //
             && (w0<r_MAX_RECT_SIZE)  //  Compile rect out
             && (w1<r_MAX_RECT_SIZE)) //
          {
               for(; m<=x1 ; ++m, c += s )
               {
                    int k = w>>1;
                    b_TBush::CompileRectAr[r_RECT_MINUS+w]
                               ( &(r_YCACHE[(c>>16)-k][m-k]), r_VSCR_WIDTH);
                    ws += wd;
                    while( ws >= xSub )
                    {
                        ws -= xSub;
                        w  += wSig;
                        k = w>>1;
                        b_TBush::CompileRectAr[r_RECT_MINUS+w]
                               ( &(r_YCACHE[(c>>16)-k][m-k]), r_VSCR_WIDTH);
                    }
               }
               return;
          }

          for(; m<=x1 ; ++m, c += s )
          {
               bsh_PutRect( m, c>>16, w );
               ws += wd;
               while( ws >= xSub )
               {
                   ws -= xSub;
                   w  += wSig;
                   bsh_PutRect( m, c>>16, w );
               }
          }

     }
     else
     {
         m = y0;
         c = x0<<16;

         if( ySub>0 )
         {

              if( w<=1 && w1<=1 )
              {
                  bsh_SingleLineLRVUD_clip( x0,y0, x1, y1 );
                  return;
              }
              ++ySub;
              s = (xSub<<16) / ySub;

              if(    b_TBush::m_visAll    //
                 && (w0<r_MAX_RECT_SIZE)  //  Compile rect out
                 && (w1<r_MAX_RECT_SIZE)) //
              {
                 for(; m<=y1 ; ++m)
                 {
                      b_TBush::CompileRectAr[r_RECT_MINUS+w]
                                 ( &(r_YCACHE[m-(w>>1)][(c>>16)-(w>>1)]), r_VSCR_WIDTH);

                      ws += wd;
                      while( ws >= ySub )
                      {
                          ws -= ySub;
                          w  += wSig;
                          b_TBush::CompileRectAr[r_RECT_MINUS+w]
                                     ( &(r_YCACHE[m-(w>>1)][(c>>16)-(w>>1)]), r_VSCR_WIDTH);
                      }
                      c += s;
                 }
                 return;
              }


              for(; m<=y1 ; ++m)
              {
                   bsh_PutRect( c>>16, m, w );

                   ws += wd;
                   while( ws >= ySub )
                   {
                       ws -= ySub;
                       w  += wSig;
                       bsh_PutRect( c>>16, m, w );
                   }
                   c += s;
              }
              return;
         }
              if( w<=1 && w1<=1 )
              {
                   if( ySub==0 )
                   {
                        if( x0>=r_CLIP2D.left && x0<r_CLIP2D.right &&
                            y0>=r_CLIP2D.top  && y0<r_CLIP2D.bottom )
                             r_YCACHE[y0][x0] = b_TBush::m_singleCol;
                        return;
                   }
                   b_TBush::SingleLineLRVDU( x0, y0, x1, y1 );
                   return;
              }

              ySub = 1-ySub;
              s = (xSub<<16) / ySub;

              if(    b_TBush::m_visAll    //
                 && (w0<r_MAX_RECT_SIZE)  //  Compile rect out
                 && (w1<r_MAX_RECT_SIZE)) //
              {
                 for(; m>=y1 ; --m)
                 {
                      int k = w>>1;
                      b_TBush::CompileRectAr[r_RECT_MINUS+w]
                                 ( &(r_YCACHE[m-k][(c>>16)-k]), r_VSCR_WIDTH);
                      ws += wd;
                      while( ws >= ySub )
                      {
                          ws -= ySub;
                          w  += wSig;
                          k = w>>1;
                          b_TBush::CompileRectAr[r_RECT_MINUS+w]
                                     ( &(r_YCACHE[m-k][(c>>16)-k]), r_VSCR_WIDTH);
                      }
                      c += s;
                 }
                 return;
              }

              for(; m>=y1 ; --m)
              {
                   bsh_PutRect( c>>16, m, w );
                   ws += wd;
                   while( ws >= ySub )
                   {
                       ws -= ySub;
                       w  += wSig;
                       bsh_PutRect( c>>16, m, w );
                   }
                   c += s;
              }
     }
 }

