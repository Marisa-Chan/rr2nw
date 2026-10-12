#ifndef	__BUSH_CPP__
#include"_bush.h"
#endif


CViewTexture b_Leaves;
int          g_MidleR = 0,
             g_MidleG = 0,
             g_MidleB = 0;

static void CallBack(byte *pImage, SRGB*pal)
{
   (void)pImage;
   ASSERT(((dword*)pImage)[-3]&TEXTURE_TXR_FORMAT);
   ((dword*)pImage)[-3] = TEXTURE_ALPHA|TEXTURE_SPRITE|TEXTURE_TXR_FORMAT;

   g_MidleR = 0;
   g_MidleG = 0;
   g_MidleB = 0;

   word *data = (word*)pImage;
   int cnt = 0;

   for( int i = 0; i < 8; ++i )
        for( int j = 3; j < 128-3; ++j )
        {
             SRGB &c=pal[data[i*256+j]];
             g_MidleR += c.r;
             g_MidleG += c.g;
             g_MidleB += c.b;
             ++cnt;
        }

   //cnt *= 3;
   //cnt /= 4;
   g_MidleR /= cnt;
   g_MidleG /= cnt;
   g_MidleB /= cnt;
}

int bush_Init()
{
    b_CreateCacheBush();
    /*CTaggedFile f(TRUE);

    f.Open("LEAVES.TXR");
       b_Leaves.Read(f,TRUE,CallBack);
    f.Close();*/
	b_Leaves.Read("LEAVES.TXR",TRUE,CallBack);

    if( !b_LoadBushTrunk() )
    {
         ASSERT(0);
         return 0;
    }

    return 1;
}

void bush_Done()
{
    b_DeleteCacheBush();

}
