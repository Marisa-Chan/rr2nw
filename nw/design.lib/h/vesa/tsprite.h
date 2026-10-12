#ifndef __TSPRITE_H__
#define __TSPRITE_H__
#include <stdio.h>
#include <process.h>

//#define DEBUG_SPRITES 1

typedef char *LinePTRType;

typedef struct TSVirtualScrType{
	int          m_width,m_height;
	int          m_x1, m_y1, m_x2, m_y2;
	char        *m_screen;
	LinePTRType *m_linePTR;
	
	TSVirtualScrType() { m_screen = 0; m_linePTR = 0; 
	      m_width = m_height = m_x1 = m_y1 = m_x2 = m_y2 = 0; }
} TVirtualScrType;

#define SPRITERTCHECK(n,msg) {if(!(n)) printf(msg); }


#ifdef DEBUG_SPRITES
	class TSprDataType {
          private:
                char *m_d;
                int   m_size;
          public:
                int  beenSet;
                TSprDataType() { beenSet = 0; m_size=0; }
                ~TSprDataType() { SPRITERTCHECK(beenSet==0,"~Sprite:m_data is set!\n"); }
                char &operator [] (int num)
                 {  
                  if( !beenSet )
                  {
                  	printf("Sprite:[]\n"); 
                  	exit(1);
                  }
                  if( num<0 || num>=m_size)
                  {
                  	printf("Sprite:[] boundCheck!\n");
                  	exit(2);
                  }
                  return m_d[num];
                 }
                operator char *()
                 {
                  if(!beenSet)
                  {
                  	printf("Sprite:(char *)\n");
                  	exit(3);
                  } 
                  return m_d;
                 }
               
                void New(int size) 
                 { 
                 m_size = size; 
                 SPRITERTCHECK(beenSet==0,"Sprite:New()  m_data is set!\n"); 
                 m_d = new char[size]; 
                 beenSet=1; 
                 }
                void Clear()
                 {
                   SPRITERTCHECK(beenSet==0,"Sprite:'Clear'  m_data is set!\n"); 
                   m_d = NULL; 
                   beenSet=0; 
                 }
                /*
                char * operator = (char *nw)
                 {
                   SPRITERTCHECK(beenSet==0,"Sprite:'='  m_data is set!\n"); 
                   SPRITERTCHECK(nw==NULL,"Sprite:'=NULL'  m_data is set!\n"); 
                   m_d = nw; 
                   beenSet=0; 
                   return m_d;
                 }
                 */
                void Delete() {delete m_d; m_d = 0; beenSet = 0; }
	};


	class TSpriteType {
          public:
	  int m_width,m_height;
	  int m_x,m_y;
	  int m_alpha;
          
          TSprDataType m_data ;
	};

#define DELETESPR(data)  data.Delete()
#define NEWSPR(data,size) data.New(size)
#define CLEARSPR(data)  data.Clear()
#else

	typedef struct {
	  int m_width,m_height;
	  int m_x,m_y;
	  int m_alpha;
	  char *m_data;
	} TSpriteType;

#define DELETESPR(data)  delete (data)
#define NEWSPR(data,size) data = new char[size]
#define CLEARSPR(data) data = NULL
#endif




#ifdef __TSPRITE_C__
	#define GLOBAL
#else
	#define GLOBAL extern
#endif
extern "C" {
GLOBAL TVirtualScrType VSCR_data;
}
int  InitVirtualScreen(int width,int height);
void DisposeVirtualScreen();

void PutSpriteORD(int x,int y,TSpriteType &spr,int ord);
void VScr2Scr(int x1,int y1,int x2,int y2);
void VScr2ScrWin(int x1,int y1,int x2,int y2);
void VScrLine2Scr( int x1,int y,int x2 );
void VDumpScreen0();


void FLoadSprite(FILE *f,TSpriteType &spr);
void PutSprite(int x,int y,TSpriteType &spr);
void MoveSprite( int x,int y,TSpriteType &spr);
void ClearSprite(TSpriteType &spr);

#undef GLOBAL
#endif

