#ifndef	__BUSH_CPP__
#include"_bush.h"
#endif

//#include <stdio.h>
//#include "brend.h"

typedef union
{
  dword  i;
  word   w;
  byte   b[4];
} bsh_TBytePack;

/*
 * 0-Примитивы вывода
 * Вывод в поток
 */

void bsh_PutB(bsh_TCodeStream &s,byte b) // Помещение в поток байта
 {
    RTCHECK( s.m_stream!=NULL,"bsh_PutB: m_stream==NULL" );
    RTCHECK( s.m_pos<s.m_size,"bsh_PutB: m_pos>=m_size" );
    s.m_stream[s.m_pos] = b;
    ++s.m_pos;
 }

void bsh_PutW(bsh_TCodeStream &s, word w) // Помещение в поток слова
 {
    bsh_TBytePack bp;
    int i;
    bp.w = w;

    for( i=0; i<sizeof(w); ++i)
         bsh_PutB(s,bp.b[i]);
 }

void bsh_PutD(bsh_TCodeStream &s,dword d) // Помещение в поток двойного слова
 {
    bsh_TBytePack bp;
    int i;
    bp.i = d;

    for( i=0; i<sizeof(i); ++i)
         bsh_PutB(s,bp.b[i]);
 }


/*****************************************************************************
 * 1-Примитивы вывода
 * Набор базовых инструкций
 * Формирование кодов команд I386 в protected mode
 */

void bsh_OutMovBEAX(bsh_TCodeStream &s,int dest,byte val)
 {
 /*
  * mov  byte ptr[eax+dest],val
  */

    if( dest==0 )
    {
        bsh_PutB(s,0xC6);
        bsh_PutB(s,0x00);
        bsh_PutB(s,val);
    }
    else
    if( dest<=127 && dest>= -128 )
    {
        bsh_PutB(s,0xC6);
        bsh_PutB(s,0x40);
        bsh_PutB(s,(byte)(dest));
        bsh_PutB(s,val);
    }
    else
    {
        bsh_PutB(s,0xC6);
        bsh_PutB(s,0x80);
        bsh_PutD(s,(dword)(dest));
        bsh_PutB(s,val);
    }

 }

void bsh_OutMovWEAX(bsh_TCodeStream &s,int dest,word val)
 {
   /*
    *  mov         word ptr[eax+dest],val
    */

    if( dest==0 )
    {
          bsh_PutB(s,0x66);
          bsh_PutB(s,0xC7);
          bsh_PutB(s,0x00);
          bsh_PutW(s,val);
    }
    else
    if( dest<=127 && dest>= -128 )
    {
          bsh_PutB(s,0x66);
          bsh_PutB(s,0xC7);
          bsh_PutB(s,0x40);
          bsh_PutB(s,(byte)dest);
          bsh_PutW(s,val);
    }
    else
    {
          bsh_PutB(s,0x66);
          bsh_PutB(s,0xC7);
          bsh_PutB(s,0x80);
          bsh_PutD(s,(dword)dest);
          bsh_PutW(s,val);
    }

 }


void bsh_OutMovDEAX( bsh_TCodeStream &s, int dest,dword val)
 {
   /*
    *  mov         dword ptr[eax+dest],val
    */

    if( dest==0 )
    {
          bsh_PutB(s,0xC7);
          bsh_PutB(s,0x00);
          bsh_PutD(s,val);
    }
    else
    if( dest<=127 && dest>= -128 )
    {
          bsh_PutB(s,0xC7);
          bsh_PutB(s,0x40);
          bsh_PutB(s,(byte)dest);
          bsh_PutD(s,val);
    }
    else
    {
          bsh_PutB(s,0xC7);
          bsh_PutB(s,0x80);
          bsh_PutD(s,(dword)dest);
          bsh_PutD(s,val);
    }
 }


void bsh_OutLoadParamEAX( bsh_TCodeStream &s, int posInStack)
 {
   /*
    * mov edi,[esp+posInStack]
    */
    bsh_PutB(s,0x8B);
    bsh_PutB(s,0x84);
    bsh_PutB(s,0x24);
    bsh_PutD(s,(dword)posInStack);
 }

void bsh_OutLoadParamEDX( bsh_TCodeStream &s, int posInStack)
 {
   /*
    * mov edi,[esp+posInStack]
    */
    bsh_PutB(s,0x8B);
    bsh_PutB(s,0x94);
    bsh_PutB(s,0x24);
    bsh_PutD(s,(dword)posInStack);
 }

void bsh_OutAdd_EAX_EDX( bsh_TCodeStream &s )
 {
   /*
    * add   eax , edx
    */
    bsh_PutB(s,0x03);
    bsh_PutB(s,0xC2);
 }

void bsh_OutRetn( bsh_TCodeStream &s )
 {
   /*
    * C3 retn
    */
    bsh_PutB(s,0xC3);
 }

void bsh_OutHead( bsh_TCodeStream &s )
 {
     //bsh_PutB(s,0x52); push edx
     bsh_OutLoadParamEAX( s, 4); // Позиция в экране
     bsh_OutLoadParamEDX( s, 8); // ширина экрана
 }

void bsh_OutEnd( bsh_TCodeStream &s )
 {
     //bsh_PutB(s,0x5A); pop edx
     bsh_OutRetn(s);
 }


/*****************************************************************************
 *  2-Примитивы вывода
 */

 /********************************************
  * bsh_MakePutLines()
  * компиляция вывода горизонтального отрезка
  * ptr - выходной поток
  * dest - относительное смещение относительно базового регистра (eax) в виртуальном экране
  * sour - массив точек для вывода
  * cnt  - количество выводимых точек
  */
#define _AS(x,y)  (*((x*)(y)))
void bsh_PutNoAlign( bsh_TCodeStream &s, int dest, byte *sour, int cnt )
 {
    int i;
    int dcnt=cnt/4;

    for( i=0; i<dcnt; ++i)
    {
         bsh_OutMovDEAX(s,dest, _AS(dword,sour));
         sour+=4;
         dest+=4;
         cnt -=4;
    }

    if( cnt>=2 )
    {
         bsh_OutMovWEAX(s,dest, _AS( word,sour));
         sour+=2;
         dest+=2;
         cnt -=2;
    }

    if( cnt>=1 )
    {
         bsh_OutMovBEAX(s,dest, *sour);
         sour+=1;
         dest+=1;
         cnt -=1;
    }
 }

void bsh_MakePutLines( bsh_TCodeStream &s, int dest, byte *sour, int cnt )
 {
    if( cnt < 10 )
        bsh_PutNoAlign(s,dest,sour,cnt);
    else
    {
        if( (dest&1)!=0 )
        {
             bsh_OutMovBEAX(s,dest,*sour);
             ++dest;
             ++sour;
             --cnt;
        }

        if( (dest&3)!=0 )
        {
             bsh_OutMovWEAX(s,dest, _AS(word,sour) );
             dest+=2;
             sour+=2;
             cnt -=2;
        }
        bsh_PutNoAlign(s,dest,sour,cnt);
    }
 }

 /*********************************************
  * bsh_Align()
  * выравнивание текущего адреса на значение,
  * кратное 4. Забивается операцией NOP
  */
void bsh_Align( bsh_TCodeStream &s )
 {
        while((s.m_pos&3)!=0)
                bsh_PutB(s, 0x90 );
 }


int  bsh_CalcRectLOD( bsh_TCodeStream &s,
                      b_TBushRect     &br,
                      int              sq_size )
 {
    bsh_Align(s);
    int i,j,step;
    int adr = s.m_pos;
    byte buff[1024];
    RTCHECK(sq_size<sizeof(buff),"bsh_CalcRectLOD: too big LOD");

    if( sq_size<=1 )
    {
        buff[0] = b_TBush::m_singleCol;
        bsh_OutHead(s);
        bsh_MakePutLines( s, 0, buff, 1);
        bsh_OutEnd (s);
        return adr;
    }

    step = br.m_size/sq_size;

    bsh_OutHead(s);

    int pos=0;
    for( j=0; j<sq_size; ++j)
    {
         buff[j] = br.data0[pos>>16];
         pos+=step;
    }
    bsh_MakePutLines( s, 0, buff, sq_size);
    bsh_OutAdd_EAX_EDX(s);

    pos = step;
    for( i=1; i<sq_size-1; ++i)
    {
         buff[0] = br.data3[pos>>16];
         bsh_MakePutLines( s, 0        , buff, 1);

         buff[0] = br.data1[pos>>16];
         bsh_MakePutLines( s, sq_size-1, buff, 1);
         pos+=step;
         bsh_OutAdd_EAX_EDX(s);
    }

    pos=0;
    for( j=0; j<sq_size; ++j)
    {
         buff[j] = br.data2[pos>>16];
         pos+=step;
    }
    bsh_MakePutLines( s, 0, buff, sq_size);

    bsh_OutEnd (s);

    return adr;
 }

void bsh_CompileRectLODs( bsh_TCodeStream &s )
 {
    int i;
    int adr[r_MAX_RECT_SIZE+r_RECT_MINUS];
    byte buff[15000];
    bsh_TCodeStream cs;
    cs.m_stream = buff;
    cs.m_pos    = 0;
    cs.m_size   = sizeof(buff);

    for( i = 0; i < r_MAX_RECT_SIZE+r_RECT_MINUS; ++i )
    {
         if( i<r_RECT_MINUS )
          adr[i] = bsh_CalcRectLOD( cs, b_TBush::m_bushRect,0);
         else
          adr[i] = bsh_CalcRectLOD( cs, b_TBush::m_bushRect,i-r_RECT_MINUS);
    }

    s = cs;

    for( i = 0; i < r_MAX_RECT_SIZE+r_RECT_MINUS; ++i )
         b_TBush::CompileRectAr[i] = (bsh_TOutComileRectFunc)
                                     (&(s.m_stream[ adr[i] ]));
    cs.m_stream = NULL;
 }
