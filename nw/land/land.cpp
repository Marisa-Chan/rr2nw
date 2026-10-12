#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef MSDOS
	#include <conio.h>
#endif
#include <string.h>
//#include <time.h>

#define __LAND_C__
#include "_land.h"
#include "maptools.h"
#include "contour.h"
#include "ship.h"
#include "triang.h"
#include "clip.h"
#include "text.h"



extern "C" void InitSbLoop(MapH *mask);
extern "C" void Init_pYCache(void);

#define MAXVIEW (madsh.len*4)
#define DDIST  (512*4)
//#define DDIST  (2048*4)



int quality=0;

#if _USE_OPENGL
	//static	CFMatrix3x4 landGLMatrix;
	static	CFVector3	_glLandBumpAxisZ;
	static	GLuint		_glLandBumpBindIdx[16];
#endif

#if _USE_GLIDE
	extern "C" {
		byte *_gr_pYCache = NULL;
	}
#endif

//******************************** GetLandHeightMap *********************************************
byte *GetLandHeightMap(int nReduction){
        ASSERT((uint)nReduction<5);
    return (byte *)maps[nReduction]->mapadr;
}
//******************************** GetLandMaskMap *********************************************
char *GetLandMaskMap(void){
    return mapmask.mapadr;
}
//******************************** dbgs *********************************************************
#if 0 //_USE_OWNGR && defined(MSDOS)
void dbgs(void){
    	memcpy((char *)0xA0000,(char *)_gr_pScreen,320*200);
    	getch();
}
#endif

//************************************** MatchLandColors ***************************************
void MatchLandColors(SRGB *pPal8){
    paletteTranslator.Define(pPal8,256);
    for (int i=0;i<256*256;i++){
        mapmask.mapadr[i]=(char)paletteTranslator((byte)mapmask.mapadr[i]);
    }
}
//************************************** MatchLandColorsFile ***********************************
bool MatchLandColorsFile(char *name){
    byte pal6[768];
    FILE *fs;
    if ((fs=CFileResource::FOpenCurrent(name)) == NULL) { RTCHECK(0,name); return FALSE; }
    fread(pal6,768,1,fs);
    fclose(fs);

    for (int i=0;i<768;i++) pal6[i]=(byte)Round(pal6[i]*255./63.);

    MatchLandColors((SRGB *)pal6);
    return TRUE;
}
//************************************* IntersectQuest ******************************************
int IntersectQuest(Ship &view,TCCRect2 &rect){
    if (view.boundMaxX<=rect.left || view.boundMinX>=rect.right
                                  || view.boundMaxY<=rect.top
                                  || view.boundMinY>=rect.bottom) return 0;
    else return 1;
}
//************************************* CreateLandfragment **************************************

void CreateLandFragment(Ship &view,TCCRect2 &rect,MapCoord *lDir,MapCoord *rDir){

    CRect2 newRect=rect;
    int startInd,i,k=0;

    if (view.boundMaxX<newRect.right) newRect.right=view.boundMaxX;
    if (view.boundMinX>newRect.left) newRect.left=view.boundMinX;

    if (view.boundMaxY<newRect.bottom) newRect.bottom=view.boundMaxY;
    if (view.boundMinY>newRect.top) newRect.top=view.boundMinY;

    switch (view.dir){
        case 0:{
            startInd=view.boundMaxX-newRect.right;

            for (i=0;i<=(newRect.right-newRect.left);i++){

                if ((newRect.bottom<hcoordr[startInd+i].y) || (newRect.top>hcoordl[startInd+i].y)) continue;

                if (newRect.bottom<=hcoordl[startInd+i].y){
                    lDir[k].y=newRect.bottom;
                    lDir[k].x=newRect.right-i;
                }
                else{
                    lDir[k].y=hcoordl[startInd+i].y;
                    lDir[k].x=newRect.right-i;
                }

                if (newRect.top>=hcoordr[startInd+i].y){
                    rDir[k].y=newRect.top;
                    rDir[k].x=newRect.right-i;
                }
                else{
                    rDir[k].y=hcoordr[startInd+i].y;
                    rDir[k].x=newRect.right-i;
                }
                if (rDir[k].y == lDir[k].y){
                    continue;
                }
                k++;
            }

            cnt=k;
            break;
        }

        case 1:{
            startInd=view.boundMaxY-newRect.bottom;

            for (i=0;i<=(newRect.bottom-newRect.top);i++){

                if ((newRect.left>hcoordr[startInd+i].x) || (newRect.right<hcoordl[startInd+i].x)) continue;

                if (newRect.left>=hcoordl[startInd+i].x){
                    lDir[k].x=newRect.left;
                    lDir[k].y=newRect.bottom-i;
                }
                else{
                    lDir[k].x=hcoordl[startInd+i].x;
                    lDir[k].y=newRect.bottom-i;
                }

                if (newRect.right<=hcoordr[startInd+i].x){
                    rDir[k].x=newRect.right;
                    rDir[k].y=newRect.bottom-i;
                }
                else{
                    rDir[k].x=hcoordr[startInd+i].x;
                    rDir[k].y=newRect.bottom-i;
                }
                if (rDir[k].x == lDir[k].x){
                    continue;
                }
                k++;
            }

            cnt=k;
            break;
        }

        case 2:{
            startInd=newRect.left-view.boundMinX;

            for (i=0;i<=(newRect.right-newRect.left);i++){

                if ((newRect.bottom<hcoordl[startInd+i].y) || (newRect.top>hcoordr[startInd+i].y)) continue;

                if (newRect.bottom<=hcoordr[startInd+i].y){
                    rDir[k].y=newRect.bottom;
                    rDir[k].x=newRect.left+i;
                }
                else{
                    rDir[k].y=hcoordr[startInd+i].y;
                    rDir[k].x=newRect.left+i;
                }

                if (newRect.top>=hcoordl[startInd+i].y){
                    lDir[k].y=newRect.top;
                    lDir[k].x=newRect.left+i;
                }
                else{
                    lDir[k].y=hcoordl[startInd+i].y;
                    lDir[k].x=newRect.left+i;
                }
                if (rDir[k].y == lDir[k].y){
                    continue;
                }
                k++;
            }

            cnt=k;
            break;
        }

        case 3:{
            startInd=newRect.top-view.boundMinY;

            for (i=0;i<=(newRect.bottom-newRect.top);i++){

                if ((newRect.left>hcoordl[startInd+i].x) || (newRect.right<hcoordr[startInd+i].x)) continue;

                if (newRect.left>=hcoordr[startInd+i].x){
                    rDir[k].x=newRect.left;
                    rDir[k].y=newRect.top+i;
                }
                else{
                    rDir[k].x=hcoordr[startInd+i].x;
                    rDir[k].y=newRect.top+i;
                }

                if (newRect.right<=hcoordl[startInd+i].x){
                    lDir[k].x=newRect.right;
                    lDir[k].y=newRect.top+i;
                }
                else{
                    lDir[k].x=hcoordl[startInd+i].x;
                    lDir[k].y=newRect.top+i;
                }
                if (rDir[k].x == lDir[k].x){
                    continue;
                }
                k++;
            }

            cnt=k;
            break;
        }

    }

}
/*
void CreateLandFragment(Ship &view,TCCRect2 &rect,MapCoord *lDir,MapCoord *rDir){

    CRect2 newRect=rect;
    int startInd,i=0,k=0;

    if (view.boundMaxX<newRect.right) newRect.right=view.boundMaxX;
    if (view.boundMinX>newRect.left) newRect.left=view.boundMinX;

    if (view.boundMaxY<newRect.bottom) newRect.bottom=view.boundMaxY;
    if (view.boundMinY>newRect.top) newRect.top=view.boundMinY;


    switch (view.dir){
        case 0:{

            for (i=0;i<=(newRect.right-newRect.left);i++){

                lDir[i].y=newRect.bottom;
                lDir[i].x=newRect.right-i;

                rDir[i].y=newRect.top;
                rDir[i].x=newRect.right-i;

            }

            cnt=i;
            break;
        }

        case 1:{

            for (i=0;i<=(newRect.bottom-newRect.top);i++){

                lDir[i].x=newRect.left;
                lDir[i].y=newRect.bottom-i;

                rDir[i].x=newRect.right;
                rDir[i].y=newRect.bottom-i;
            }

            cnt=i;
            break;
        }

        case 2:{

            for (i=0;i<=(newRect.right-newRect.left);i++){

                rDir[i].y=newRect.bottom;
                rDir[i].x=newRect.left+i;

                lDir[i].y=newRect.top;
                lDir[i].x=newRect.left+i;

            }

            cnt=i;
            break;
        }

        case 3:{

            for (i=0;i<(newRect.bottom-newRect.top);i++){

                rDir[i].x=newRect.left;
                rDir[i].y=newRect.top+i;

                lDir[i].x=newRect.right;
                lDir[i].y=newRect.top+i;

            }

            cnt=i;
            break;
        }

    }

}
*/
#define LASTU (254<<16)
//************************************* DrawLayerRtoLByY ****************************************
#if _USE_OPENGL
const double fLandMaxBump = 50;
struct S_glLandCellVert {
	int x, y;
	bool bump;
	double wz, alpha;
	byte h, col;
	bool Define(int _x,int _y);
	void glVertex();
	void glBumpVertex();
};
inline bool S_glLandCellVert::Define(int _x,int _y)
{
	x = _x; y = _y;
	int  offs = ((_y&511)<<9)+(_x&511);
	col = mapmask.mapadr[((word*)clipmaps[0]->mapadr)[offs]],
	h = *((byte*)maps[0]->mapadr+offs);
	wz = _glLandBumpAxisZ*CFVector3(_x,h,-y)+CViewObject::m_viewPointDirMx.m[2][3];
	wz = -wz;
	alpha = wz/fLandMaxBump;
	return bool(bump = wz >= 0 && wz < fLandMaxBump);
}
inline void S_glLandCellVert::glVertex()
{
	glColor3ub(_gr_glPal[col].r,_gr_glPal[col].g,_gr_glPal[col].b);
	glVertex3i(x,h,-y);
}
inline void S_glLandCellVert::glBumpVertex()
{
	glColor4d(_gr_glPal[col].r/127.5,_gr_glPal[col].g/127.5,_gr_glPal[col].b/127.5,alpha);
	glVertex3i(x,h,-y);
}
inline	void glLandCell(int x,int y,int d=1)
{
	ASSERT(x>=0&&x<512&&y>=0&&y<512);
	S_glLandCellVert v[4];
	bool bBump = v[0].Define(x,y) |
				 v[1].Define(x+d,y) |
				 v[2].Define(x+d,y+d) |
				 v[3].Define(x,y+d);
	glBegin(GL_TRIANGLES);
	v[0].glVertex();
	v[1].glVertex();
	v[2].glVertex();
	v[0].glVertex();
	v[2].glVertex();
	v[3].glVertex();
	glEnd();
	if( bBump ) {
		glPushAttrib(GL_TEXTURE_BIT|GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT);
		glBlendFunc(GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
		#ifdef __sgi
			glBindTextureEXT(GL_TEXTURE_2D,_glLandBumpBindIdx[v[0].h&15]);
		#else
			int nMask = v[0].h&15
			glTexImage2D(GL_TEXTURE_2D,0,1,bMask[nMask].x,bMask[nMask].y,
					0,GL_LUMINANCE,GL_UNSIGNED_BYTE,bMask[nMask].mapadr);
		#endif
		glBegin(GL_TRIANGLES);
		glTexCoord2d(0,0); v[0].glBumpVertex();
		glTexCoord2d(1,0); v[1].glBumpVertex();
		glTexCoord2d(1,1); v[2].glBumpVertex();
		glTexCoord2d(0,0); v[0].glBumpVertex();
		glTexCoord2d(1,1); v[2].glBumpVertex();
		glTexCoord2d(0,1); v[3].glBumpVertex();
		glEnd();

		glPopAttrib();
	}
}
#endif

void DrawLayerRtoLByY(TAlignPlane &pPlane,TAlignPlane &nPlane,int numBlocks,CLandObjectMap1 *p,int X,int st,int en){

    int             Y,s_y=SIG(en-st);

    if (madsh.dir == 0){
        Y=st;
    }
    else{
        Y=st-1;
    }
	#if _USE_OPENGL
		(void)pPlane; (void)nPlane;
    	for (int j=1;j<=numBlocks;j++,Y+=s_y){
			/*glBegin(GL_TRIANGLES);
			glLandVertex(X,Y);
			glLandVertex(X+1,Y);
			glLandVertex(X+1,Y+1);
			glLandVertex(X,Y);
			glLandVertex(X+1,Y+1);
			glLandVertex(X,Y+1);
			glEnd();*/
			glLandCell(X,Y);
			p->Draw(X,Y);
		}
	#else

    CoordBuf        triVert[10];
	int ch;

    for (int j=1;j<=numBlocks;j++){

        if ( pPlane.pStart[j].zLen < MAXVIEW &&
                    pPlane.pStart[j-1].zLen < MAXVIEW &&
                    nPlane.pStart[j-1].zLen < MAXVIEW )
        {
            if ( pPlane.pStart[j].zLen >= ZLEN &&
                    pPlane.pStart[j-1].zLen >= ZLEN &&
                    nPlane.pStart[j-1].zLen >= ZLEN )
            {
                int x_a=(pPlane.pStart[j].x-pPlane.pStart[j-1].x);
                int x_b=(nPlane.pStart[j-1].x-pPlane.pStart[j-1].x);

                int y_a=(pPlane.pStart[j].y-pPlane.pStart[j-1].y);
                int y_b=(nPlane.pStart[j-1].y-pPlane.pStart[j-1].y);

                if ((x_a*y_b-x_b*y_a)<=0){

                    triVert[0]=pPlane.pStart[j];
                    triVert[1]=pPlane.pStart[j-1];
                    triVert[2]=nPlane.pStart[j-1];

                    if ( triVert[0].zLen < DDIST ||
                         triVert[1].zLen < DDIST ||
                         triVert[2].zLen < DDIST
                       ) ch=0;
                    else ch=1;

                    if (madsh.dir==0){
                        triVert[0].blb=LASTU;
                        triVert[0].bhb=2<<16;

                        triVert[1].blb=LASTU;
                        triVert[1].bhb=LASTU;

                        triVert[2].blb=2<<16;
                        triVert[2].bhb=LASTU;
                    }
                    else{
                        triVert[0].blb=2<<16;
                        triVert[0].bhb=LASTU;

                        triVert[1].blb=2<<16;
                        triVert[1].bhb=2<<16;

                        triVert[2].blb=LASTU;
                        triVert[2].bhb=2<<16;
                    }
                    TriangFirstPlaneY(triVert,3,ch,X,Y);

                }
            }

            else if (pPlane.pStart[j].zLen < ZLEN &&
                                pPlane.pStart[j-1].zLen < ZLEN &&
                                nPlane.pStart[j-1].zLen < ZLEN){
            }
            else{
                ClipBuf mapVert[3];

                mapVert[0]=pPlane.pStartc[j];
                mapVert[1]=pPlane.pStartc[j-1];
                mapVert[2]=nPlane.pStartc[j-1];

                if ( mapVert[0].z < DDIST ||
                     mapVert[1].z < DDIST ||
                     mapVert[2].z < DDIST
                   ) ch=0;
                else ch=1;

                if (madsh.dir==0){
                    mapVert[0].blb=LASTU;
                    mapVert[0].bhb=2<<16;

                    mapVert[1].blb=LASTU;
                    mapVert[1].bhb=LASTU;

                    mapVert[2].blb=2<<16;
                    mapVert[2].bhb=LASTU;
                }
                else{
                    mapVert[0].blb=2<<16;
                    mapVert[0].bhb=LASTU;

                    mapVert[1].blb=2<<16;
                    mapVert[1].bhb=2<<16;

                    mapVert[2].blb=LASTU;
                    mapVert[2].bhb=2<<16;
                }

                int numVert=FrontClip(mapVert,triVert);

                int x_a=(triVert[0].x-triVert[1].x);
                int x_b=(triVert[2].x-triVert[1].x);

                int y_a=(triVert[0].y-triVert[1].y);
                int y_b=(triVert[2].y-triVert[1].y);

                if ((x_a*y_b-x_b*y_a)<=0){
                    TriangFirstPlaneY(triVert,numVert,ch,X,Y);
                }
            }
        }


        if ( pPlane.pStart[j].zLen < MAXVIEW &&
                    nPlane.pStart[j-1].zLen < MAXVIEW &&
                    nPlane.pStart[j].zLen < MAXVIEW )
        {
            if ( pPlane.pStart[j].zLen >= ZLEN &&
                    nPlane.pStart[j-1].zLen >= ZLEN &&
                    nPlane.pStart[j].zLen >= ZLEN )
            {
                int x_a=(pPlane.pStart[j].x-nPlane.pStart[j-1].x);
                int x_b=(nPlane.pStart[j].x-nPlane.pStart[j-1].x);

                int y_a=(pPlane.pStart[j].y-nPlane.pStart[j-1].y);
                int y_b=(nPlane.pStart[j].y-nPlane.pStart[j-1].y);

                if ((x_a*y_b-x_b*y_a)<=0){

                    triVert[0]=pPlane.pStart[j];
                    triVert[1]=nPlane.pStart[j-1];
                    triVert[2]=nPlane.pStart[j];

                    if ( triVert[0].zLen < DDIST ||
                         triVert[1].zLen < DDIST ||
                         triVert[2].zLen < DDIST
                       ) ch=0;
                    else ch=1;

                    if (madsh.dir==0){
                        triVert[0].blb=LASTU;
                        triVert[0].bhb=2<<16;

                        triVert[1].blb=2<<16;
                        triVert[1].bhb=LASTU;

                        triVert[2].blb=2<<16;
                        triVert[2].bhb=2<<16;
                    }
                    else{
                        triVert[0].blb=2<<16;
                        triVert[0].bhb=LASTU;

                        triVert[1].blb=LASTU;
                        triVert[1].bhb=2<<16;

                        triVert[2].blb=LASTU;
                        triVert[2].bhb=LASTU;
                    }
                    TriangFirstPlaneY(triVert,3,ch,X,Y);

                }
            }

            else if (pPlane.pStart[j].zLen < ZLEN &&
                                nPlane.pStart[j-1].zLen < ZLEN &&
                                nPlane.pStart[j].zLen < ZLEN){
            }
            else{
                ClipBuf mapVert[3];

                mapVert[0]=pPlane.pStartc[j];
                mapVert[1]=nPlane.pStartc[j-1];
                mapVert[2]=nPlane.pStartc[j];

                if ( mapVert[0].z < DDIST ||
                     mapVert[1].z < DDIST ||
                     mapVert[2].z < DDIST
                   ) ch=0;
                else ch=1;

                if (madsh.dir == 0){
                    mapVert[0].blb=LASTU;
                    mapVert[0].bhb=2<<16;

                    mapVert[1].blb=2<<16;
                    mapVert[1].bhb=LASTU;

                    mapVert[2].blb=2<<16;
                    mapVert[2].bhb=2<<16;
                }
                else{
                    mapVert[0].blb=2<<16;
                    mapVert[0].bhb=LASTU;

                    mapVert[1].blb=LASTU;
                    mapVert[1].bhb=2<<16;

                    mapVert[2].blb=LASTU;
                    mapVert[2].bhb=LASTU;
                }

                int numVert=FrontClip(mapVert,triVert);

                int x_a=(triVert[0].x-triVert[1].x);
                int x_b=(triVert[2].x-triVert[1].x);

                int y_a=(triVert[0].y-triVert[1].y);
                int y_b=(triVert[2].y-triVert[1].y);

                if ((x_a*y_b-x_b*y_a)<=0){
                    TriangFirstPlaneY(triVert,numVert,ch,X,Y);
                }
            }
        }
        p->Draw(X,Y);
        Y+=s_y;
    }
	#endif
}
//************************************* DrawLayerLtoRByY ****************************************
void DrawLayerLtoRByY(TAlignPlane &pPlane,TAlignPlane &nPlane,int cnt,int numBlocks,CLandObjectMap1 *p,int X,int st,int en){

    int             Y,s_y=SIG(en-st);

    if (madsh.dir == 0){
        Y=st-1;
    }
    else{
        Y=st;
    }

	#if _USE_OPENGL
		(void)pPlane; (void)nPlane; (void)p; (void)cnt;
    	for (int j=0;j<numBlocks;j++,Y+=s_y){
			/*glBegin(GL_TRIANGLES);
			glLandVertex(X,Y);
			glLandVertex(X+1,Y);
			glLandVertex(X+1,Y+1);
			glLandVertex(X,Y);
			glLandVertex(X+1,Y+1);
			glLandVertex(X,Y+1);
			glEnd();*/
			glLandCell(X,Y);
			p->Draw(X,Y);
		}
	#else
    CoordBuf        triVert[10];
	int ch;
    for (int j=0;j<numBlocks;j++){

        if ( pPlane.pStart[cnt-j].zLen < MAXVIEW &&
                    pPlane.pStart[cnt-j-1].zLen < MAXVIEW &&
                    nPlane.pStart[cnt-j-1].zLen < MAXVIEW )
        {
            if ( pPlane.pStart[cnt-j].zLen >= ZLEN &&
                    pPlane.pStart[cnt-j-1].zLen >= ZLEN &&
                    nPlane.pStart[cnt-j-1].zLen >= ZLEN )
            {
                int x_a=(pPlane.pStart[cnt-j].x-pPlane.pStart[cnt-j-1].x);
                int x_b=(nPlane.pStart[cnt-j-1].x-pPlane.pStart[cnt-j-1].x);

                int y_a=(pPlane.pStart[cnt-j].y-pPlane.pStart[cnt-j-1].y);
                int y_b=(nPlane.pStart[cnt-j-1].y-pPlane.pStart[cnt-j-1].y);

                if ((x_a*y_b-x_b*y_a)<=0){

                    triVert[0]=pPlane.pStart[cnt-j];
                    triVert[1]=pPlane.pStart[cnt-j-1];
                    triVert[2]=nPlane.pStart[cnt-j-1];

                    if ( triVert[0].zLen < DDIST ||
                         triVert[1].zLen < DDIST ||
                         triVert[2].zLen < DDIST
                       ) ch=0;
                    else ch=1;

                    if (madsh.dir==0){
                        triVert[0].blb=LASTU;
                        triVert[0].bhb=2<<16;

                        triVert[1].blb=LASTU;
                        triVert[1].bhb=LASTU;

                        triVert[2].blb=2<<16;
                        triVert[2].bhb=LASTU;
                    }
                    else{
                        triVert[0].blb=2<<16;
                        triVert[0].bhb=LASTU;

                        triVert[1].blb=2<<16;
                        triVert[1].bhb=2<<16;

                        triVert[2].blb=LASTU;
                        triVert[2].bhb=2<<16;
                    }

                    TriangFirstPlaneY(triVert,3,ch,X,Y);

                }
            }

            else if (pPlane.pStart[cnt-j].zLen < ZLEN &&
                                pPlane.pStart[cnt-j-1].zLen < ZLEN &&
                                nPlane.pStart[cnt-j-1].zLen < ZLEN){
            }
            else{
                ClipBuf mapVert[3];

                mapVert[0]=pPlane.pStartc[cnt-j];
                mapVert[1]=pPlane.pStartc[cnt-j-1];
                mapVert[2]=nPlane.pStartc[cnt-j-1];

                if ( mapVert[0].z < DDIST ||
                     mapVert[1].z < DDIST ||
                     mapVert[2].z < DDIST
                   ) ch=0;
                else ch=1;

                if (madsh.dir==0){
                    mapVert[0].blb=LASTU;
                    mapVert[0].bhb=2<<16;

                    mapVert[1].blb=LASTU;
                    mapVert[1].bhb=LASTU;

                    mapVert[2].blb=2<<16;
                    mapVert[2].bhb=LASTU;
                }
                else{
                    mapVert[0].blb=2<<16;
                    mapVert[0].bhb=LASTU;

                    mapVert[1].blb=2<<16;
                    mapVert[1].bhb=2<<16;

                    mapVert[2].blb=LASTU;
                    mapVert[2].bhb=2<<16;
                }



                int numVert=FrontClip(mapVert,triVert);

                int x_a=(triVert[0].x-triVert[1].x);
                int x_b=(triVert[2].x-triVert[1].x);

                int y_a=(triVert[0].y-triVert[1].y);
                int y_b=(triVert[2].y-triVert[1].y);

                if ((x_a*y_b-x_b*y_a)<=0){
                    TriangFirstPlaneY(triVert,numVert,ch,X,Y);
                }
            }
        }


        if ( pPlane.pStart[cnt-j].zLen < MAXVIEW &&
                    nPlane.pStart[cnt-j-1].zLen < MAXVIEW &&
                    nPlane.pStart[cnt-j].zLen < MAXVIEW )
        {
            if ( pPlane.pStart[cnt-j].zLen >= ZLEN &&
                    nPlane.pStart[cnt-j-1].zLen >= ZLEN &&
                    nPlane.pStart[cnt-j].zLen >= ZLEN )
            {
                int x_a=(pPlane.pStart[cnt-j].x-nPlane.pStart[cnt-j-1].x);
                int x_b=(nPlane.pStart[cnt-j].x-nPlane.pStart[cnt-j-1].x);

                int y_a=(pPlane.pStart[cnt-j].y-nPlane.pStart[cnt-j-1].y);
                int y_b=(nPlane.pStart[cnt-j].y-nPlane.pStart[cnt-j-1].y);

                if ((x_a*y_b-x_b*y_a)<=0){

                    triVert[0]=pPlane.pStart[cnt-j];
                    triVert[1]=nPlane.pStart[cnt-j-1];
                    triVert[2]=nPlane.pStart[cnt-j];

                    if ( triVert[0].zLen < DDIST ||
                         triVert[1].zLen < DDIST ||
                         triVert[2].zLen < DDIST
                       ) ch=0;
                    else ch=1;

                    if (madsh.dir==0){
                        triVert[0].blb=LASTU;
                        triVert[0].bhb=2<<16;

                        triVert[1].blb=2<<16;
                        triVert[1].bhb=LASTU;

                        triVert[2].blb=2<<16;
                        triVert[2].bhb=2<<16;
                    }
                    else{
                        triVert[0].blb=2<<16;
                        triVert[0].bhb=LASTU;

                        triVert[1].blb=LASTU;
                        triVert[1].bhb=2<<16;

                        triVert[2].blb=LASTU;
                        triVert[2].bhb=LASTU;
                    }

                    TriangFirstPlaneY(triVert,3,ch,X,Y);

                }
            }

            else if (pPlane.pStart[cnt-j].zLen < ZLEN &&
                                nPlane.pStart[cnt-j-1].zLen < ZLEN &&
                                nPlane.pStart[cnt-j].zLen < ZLEN){
            }
            else{
                ClipBuf mapVert[3];

                mapVert[0]=pPlane.pStartc[cnt-j];
                mapVert[1]=nPlane.pStartc[cnt-j-1];
                mapVert[2]=nPlane.pStartc[cnt-j];

                if ( mapVert[0].z < DDIST ||
                     mapVert[1].z < DDIST ||
                     mapVert[2].z < DDIST
                   ) ch=0;
                else ch=1;

                if (madsh.dir==0){
                    mapVert[0].blb=LASTU;
                    mapVert[0].bhb=2<<16;

                    mapVert[1].blb=2<<16;
                    mapVert[1].bhb=LASTU;

                    mapVert[2].blb=2<<16;
                    mapVert[2].bhb=2<<16;
                }
                else{
                    mapVert[0].blb=2<<16;
                    mapVert[0].bhb=LASTU;

                    mapVert[1].blb=LASTU;
                    mapVert[1].bhb=2<<16;

                    mapVert[2].blb=LASTU;
                    mapVert[2].bhb=LASTU;
                }

                int numVert=FrontClip(mapVert,triVert);

                int x_a=(triVert[0].x-triVert[1].x);
                int x_b=(triVert[2].x-triVert[1].x);

                int y_a=(triVert[0].y-triVert[1].y);
                int y_b=(triVert[2].y-triVert[1].y);

                if ((x_a*y_b-x_b*y_a)<=0){
                    TriangFirstPlaneY(triVert,numVert,ch,X,Y);
                }
            }
        }
        p->Draw(X,Y);
        Y+=s_y;
    }
	#endif
}
//************************************* DrawLayerRtoLByX ****************************************
void DrawLayerRtoLByX(TAlignPlane &pPlane,TAlignPlane &nPlane,int numBlocks,CLandObjectMap1 *p,int Y,int st,int en){

    int         X,s_x=SIG(en-st);

    if (madsh.dir == 1){
        X=st-1;
    }
    else{
        X=st;
    }
	#if _USE_OPENGL
		(void)pPlane; (void)nPlane; (void)p;
    	for (int j=1;j<=numBlocks;j++,X+=s_x){
			/*glBegin(GL_TRIANGLES);
			glLandVertex(X,Y);
			glLandVertex(X+1,Y);
			glLandVertex(X+1,Y+1);
			glLandVertex(X,Y);
			glLandVertex(X+1,Y+1);
			glLandVertex(X,Y+1);
			glEnd();*/
			glLandCell(X,Y);
			p->Draw(Y,X);
		}
	#else

    CoordBuf        triVert[10];
	int ch;
    for (int j=1;j<=numBlocks;j++){

        if ( pPlane.pStart[j].zLen < MAXVIEW &&
                    pPlane.pStart[j-1].zLen < MAXVIEW &&
                    nPlane.pStart[j].zLen < MAXVIEW )
        {
            if ( pPlane.pStart[j].zLen >= ZLEN &&
                    pPlane.pStart[j-1].zLen >= ZLEN &&
                    nPlane.pStart[j].zLen >= ZLEN )
            {
                int x_a=(pPlane.pStart[j].x-pPlane.pStart[j-1].x);
                int x_b=(nPlane.pStart[j].x-pPlane.pStart[j-1].x);

                int y_a=(pPlane.pStart[j].y-pPlane.pStart[j-1].y);
                int y_b=(nPlane.pStart[j].y-pPlane.pStart[j-1].y);

                if ((x_a*y_b-x_b*y_a)<=0){

                    triVert[0]=pPlane.pStart[j];
                    triVert[1]=pPlane.pStart[j-1];
                    triVert[2]=nPlane.pStart[j];

                    if ( triVert[0].zLen < DDIST ||
                         triVert[1].zLen < DDIST ||
                         triVert[2].zLen < DDIST
                       ) ch=0;
                    else ch=1;

                    if (madsh.dir==1){
                        triVert[0].blb=2<<16;
                        triVert[0].bhb=2<<16;

                        triVert[1].blb=LASTU;
                        triVert[1].bhb=2<<16;

                        triVert[2].blb=2<<16;
                        triVert[2].bhb=LASTU;
                    }
                    else{
                        triVert[0].blb=LASTU;
                        triVert[0].bhb=LASTU;

                        triVert[1].blb=2<<16;
                        triVert[1].bhb=LASTU;

                        triVert[2].blb=LASTU;
                        triVert[2].bhb=2<<16;
                    }
                    TriangFirstPlaneY(triVert,3,ch,X,Y);

                }
            }

            else if (pPlane.pStart[j].zLen < ZLEN &&
                                pPlane.pStart[j-1].zLen < ZLEN &&
                                nPlane.pStart[j].zLen < ZLEN){
            }
            else{
                ClipBuf mapVert[3];

                mapVert[0]=pPlane.pStartc[j];
                mapVert[1]=pPlane.pStartc[j-1];
                mapVert[2]=nPlane.pStartc[j];

                if ( mapVert[0].z < DDIST ||
                     mapVert[1].z < DDIST ||
                     mapVert[2].z < DDIST
                   ) ch=0;
                else ch=1;

                if (madsh.dir==1){
                    mapVert[0].blb=2<<16;
                    mapVert[0].bhb=2<<16;

                    mapVert[1].blb=LASTU;
                    mapVert[1].bhb=2<<16;

                    mapVert[2].blb=2<<16;
                    mapVert[2].bhb=LASTU;
                }
                else{
                    mapVert[0].blb=LASTU;
                    mapVert[0].bhb=LASTU;

                    mapVert[1].blb=2<<16;
                    mapVert[1].bhb=LASTU;

                    mapVert[2].blb=LASTU;
                    mapVert[2].bhb=2<<16;
                }
                int numVert=FrontClip(mapVert,triVert);

                int x_a=(triVert[0].x-triVert[1].x);
                int x_b=(triVert[2].x-triVert[1].x);

                int y_a=(triVert[0].y-triVert[1].y);
                int y_b=(triVert[2].y-triVert[1].y);

                if ((x_a*y_b-x_b*y_a)<=0){
                    TriangFirstPlaneY(triVert,numVert,ch,X,Y);
                }
            }
        }


        if ( pPlane.pStart[j-1].zLen < MAXVIEW &&
                    nPlane.pStart[j-1].zLen < MAXVIEW &&
                    nPlane.pStart[j].zLen < MAXVIEW )
        {
            if ( pPlane.pStart[j-1].zLen >= ZLEN &&
                    nPlane.pStart[j-1].zLen >= ZLEN &&
                    nPlane.pStart[j].zLen >= ZLEN )
            {
                int x_a=(pPlane.pStart[j-1].x-nPlane.pStart[j-1].x);
                int x_b=(nPlane.pStart[j].x-nPlane.pStart[j-1].x);

                int y_a=(pPlane.pStart[j-1].y-nPlane.pStart[j-1].y);
                int y_b=(nPlane.pStart[j].y-nPlane.pStart[j-1].y);

                if ((x_a*y_b-x_b*y_a)<=0){

                    triVert[0]=pPlane.pStart[j-1];
                    triVert[1]=nPlane.pStart[j-1];
                    triVert[2]=nPlane.pStart[j];

                    if ( triVert[0].zLen < DDIST ||
                         triVert[1].zLen < DDIST ||
                         triVert[2].zLen < DDIST
                       ) ch=0;
                    else ch=1;

                    if (madsh.dir==1){
                        triVert[0].blb=LASTU;
                        triVert[0].bhb=2<<16;

                        triVert[1].blb=LASTU;
                        triVert[1].bhb=LASTU;

                        triVert[2].blb=2<<16;
                        triVert[2].bhb=LASTU;
                    }
                    else{
                        triVert[0].blb=2<<16;
                        triVert[0].bhb=LASTU;

                        triVert[1].blb=2<<16;
                        triVert[1].bhb=2<<16;

                        triVert[2].blb=LASTU;
                        triVert[2].bhb=2<<16;
                    }
                    TriangFirstPlaneY(triVert,3,ch,X,Y);

                }
            }

            else if (pPlane.pStart[j-1].zLen < ZLEN &&
                                nPlane.pStart[j-1].zLen < ZLEN &&
                                nPlane.pStart[j].zLen < ZLEN){
            }
            else{
                ClipBuf mapVert[3];

                mapVert[0]=pPlane.pStartc[j-1];
                mapVert[1]=nPlane.pStartc[j-1];
                mapVert[2]=nPlane.pStartc[j];

                if ( mapVert[0].z < DDIST ||
                     mapVert[1].z < DDIST ||
                     mapVert[2].z < DDIST
                   ) ch=0;
                else ch=1;

                if (madsh.dir==1){
                    mapVert[0].blb=LASTU;
                    mapVert[0].bhb=2<<16;

                    mapVert[1].blb=LASTU;
                    mapVert[1].bhb=LASTU;

                    mapVert[2].blb=2<<16;
                    mapVert[2].bhb=LASTU;
                }
                else{
                    mapVert[0].blb=2<<16;
                    mapVert[0].bhb=LASTU;

                    mapVert[1].blb=2<<16;
                    mapVert[1].bhb=2<<16;

                    mapVert[2].blb=LASTU;
                    mapVert[2].bhb=2<<16;
                }

                int numVert=FrontClip(mapVert,triVert);

                int x_a=(triVert[0].x-triVert[1].x);
                int x_b=(triVert[2].x-triVert[1].x);

                int y_a=(triVert[0].y-triVert[1].y);
                int y_b=(triVert[2].y-triVert[1].y);

                if ((x_a*y_b-x_b*y_a)<=0){
                    TriangFirstPlaneY(triVert,numVert,ch,X,Y);
                }
            }
        }
        p->Draw(Y,X);
        X+=s_x;
    }

	#endif
}
//************************************* DrawLayerRtoLByX ****************************************
void DrawLayerLtoRByX(TAlignPlane &pPlane,TAlignPlane &nPlane,int cnt,int numBlocks,CLandObjectMap1 *p,int Y,int st,int en){

    int             X,s_x=SIG(en-st);

    if (madsh.dir == 1){
        X=st;
    }
    else{
        X=st-1;
    }
	#if _USE_OPENGL
		(void)pPlane; (void)nPlane; (void)p; (void)cnt;
    	for (int j=0;j<numBlocks;j++,X+=s_x){
			/*glBegin(GL_TRIANGLES);
			glLandVertex(X,Y);
			glLandVertex(X+1,Y);
			glLandVertex(X+1,Y+1);
			glLandVertex(X,Y);
			glLandVertex(X+1,Y+1);
			glLandVertex(X,Y+1);
			glEnd();*/
			glLandCell(X,Y);
			p->Draw(Y,X);
		}
	#else

    CoordBuf        triVert[10];
	int ch;

    for (int j=0;j<numBlocks;j++){

        if ( pPlane.pStart[cnt-j].zLen < MAXVIEW &&
                    pPlane.pStart[cnt-j-1].zLen < MAXVIEW &&
                    nPlane.pStart[cnt-j].zLen < MAXVIEW )
        {
            if ( pPlane.pStart[cnt-j].zLen >= ZLEN &&
                    pPlane.pStart[cnt-j-1].zLen >= ZLEN &&
                    nPlane.pStart[cnt-j].zLen >= ZLEN )
            {
                int x_a=(pPlane.pStart[cnt-j].x-pPlane.pStart[cnt-j-1].x);
                int x_b=(nPlane.pStart[cnt-j].x-pPlane.pStart[cnt-j-1].x);

                int y_a=(pPlane.pStart[cnt-j].y-pPlane.pStart[cnt-j-1].y);
                int y_b=(nPlane.pStart[cnt-j].y-pPlane.pStart[cnt-j-1].y);

                if ((x_a*y_b-x_b*y_a)<=0){

                    triVert[0]=pPlane.pStart[cnt-j];
                    triVert[1]=pPlane.pStart[cnt-j-1];
                    triVert[2]=nPlane.pStart[cnt-j];

                    if ( triVert[0].zLen < DDIST ||
                         triVert[1].zLen < DDIST ||
                         triVert[2].zLen < DDIST
                       ) ch=0;
                    else ch=1;

                    if (madsh.dir==1){
                        triVert[0].blb=2<<16;
                        triVert[0].bhb=2<<16;

                        triVert[1].blb=LASTU;
                        triVert[1].bhb=2<<16;

                        triVert[2].blb=2<<16;
                        triVert[2].bhb=LASTU;
                    }
                    else{
                        triVert[0].blb=LASTU;
                        triVert[0].bhb=LASTU;

                        triVert[1].blb=2<<16;
                        triVert[1].bhb=LASTU;

                        triVert[2].blb=LASTU;
                        triVert[2].bhb=2<<16;
                    }
                    TriangFirstPlaneY(triVert,3,ch,X,Y);

                }
            }

            else if (pPlane.pStart[cnt-j].zLen < ZLEN &&
                                pPlane.pStart[cnt-j-1].zLen < ZLEN &&
                                nPlane.pStart[cnt-j].zLen < ZLEN){
            }
            else{
                ClipBuf mapVert[3];

                mapVert[0]=pPlane.pStartc[cnt-j];
                mapVert[1]=pPlane.pStartc[cnt-j-1];
                mapVert[2]=nPlane.pStartc[cnt-j];

                if ( mapVert[0].z < DDIST ||
                     mapVert[1].z < DDIST ||
                     mapVert[2].z < DDIST
                   ) ch=0;
                else ch=1;

                if (madsh.dir==1){
                    mapVert[0].blb=2<<16;
                    mapVert[0].bhb=2<<16;

                    mapVert[1].blb=LASTU;
                    mapVert[1].bhb=2<<16;

                    mapVert[2].blb=2<<16;
                    mapVert[2].bhb=LASTU;
                }
                else{
                    mapVert[0].blb=LASTU;
                    mapVert[0].bhb=LASTU;

                    mapVert[1].blb=2<<16;
                    mapVert[1].bhb=LASTU;

                    mapVert[2].blb=LASTU;
                    mapVert[2].bhb=2<<16;
                }

                int numVert=FrontClip(mapVert,triVert);

                int x_a=(triVert[0].x-triVert[1].x);
                int x_b=(triVert[2].x-triVert[1].x);

                int y_a=(triVert[0].y-triVert[1].y);
                int y_b=(triVert[2].y-triVert[1].y);

                if ((x_a*y_b-x_b*y_a)<=0){
                    TriangFirstPlaneY(triVert,numVert,ch,X,Y);
                }
            }
        }


        if ( pPlane.pStart[cnt-j-1].zLen < MAXVIEW &&
                    nPlane.pStart[cnt-j-1].zLen < MAXVIEW &&
                    nPlane.pStart[cnt-j].zLen < MAXVIEW )
        {
            if ( pPlane.pStart[cnt-j-1].zLen >= ZLEN &&
                    nPlane.pStart[cnt-j-1].zLen >= ZLEN &&
                    nPlane.pStart[cnt-j].zLen >= ZLEN )
            {
                int x_a=(pPlane.pStart[cnt-j-1].x-nPlane.pStart[cnt-j-1].x);
                int x_b=(nPlane.pStart[cnt-j].x-nPlane.pStart[cnt-j-1].x);

                int y_a=(pPlane.pStart[cnt-j-1].y-nPlane.pStart[cnt-j-1].y);
                int y_b=(nPlane.pStart[cnt-j].y-nPlane.pStart[cnt-j-1].y);

                if ((x_a*y_b-x_b*y_a)<=0){

                    triVert[0]=pPlane.pStart[cnt-j-1];
                    triVert[1]=nPlane.pStart[cnt-j-1];
                    triVert[2]=nPlane.pStart[cnt-j];

                    if ( triVert[0].zLen < DDIST ||
                         triVert[1].zLen < DDIST ||
                         triVert[2].zLen < DDIST
                       ) ch=0;
                    else ch=1;

                    if (madsh.dir==1){
                        triVert[0].blb=LASTU;
                        triVert[0].bhb=2<<16;

                        triVert[1].blb=LASTU;
                        triVert[1].bhb=LASTU;

                        triVert[2].blb=2<<16;
                        triVert[2].bhb=LASTU;
                    }
                    else{
                        triVert[0].blb=2<<16;
                        triVert[0].bhb=LASTU;

                        triVert[1].blb=2<<16;
                        triVert[1].bhb=2<<16;

                        triVert[2].blb=LASTU;
                        triVert[2].bhb=2<<16;
                    }
                    TriangFirstPlaneY(triVert,3,ch,X,Y);

                }
            }

            else if (pPlane.pStart[cnt-j-1].zLen < ZLEN &&
                                nPlane.pStart[cnt-j-1].zLen < ZLEN &&
                                nPlane.pStart[cnt-j].zLen < ZLEN){
            }
            else{
                ClipBuf mapVert[3];

                mapVert[0]=pPlane.pStartc[cnt-j-1];
                mapVert[1]=nPlane.pStartc[cnt-j-1];
                mapVert[2]=nPlane.pStartc[cnt-j];

                if ( mapVert[0].z < DDIST ||
                     mapVert[1].z < DDIST ||
                     mapVert[2].z < DDIST
                   ) ch=0;
                else ch=1;

                if (madsh.dir==1){
                    mapVert[0].blb=LASTU;
                    mapVert[0].bhb=2<<16;

                    mapVert[1].blb=LASTU;
                    mapVert[1].bhb=LASTU;

                    mapVert[2].blb=2<<16;
                    mapVert[2].bhb=LASTU;
                }
                else{
                    mapVert[0].blb=2<<16;
                    mapVert[0].bhb=LASTU;

                    mapVert[1].blb=2<<16;
                    mapVert[1].bhb=2<<16;

                    mapVert[2].blb=LASTU;
                    mapVert[2].bhb=2<<16;
                }
                int numVert=FrontClip(mapVert,triVert);

                int x_a=(triVert[0].x-triVert[1].x);
                int x_b=(triVert[2].x-triVert[1].x);

                int y_a=(triVert[0].y-triVert[1].y);
                int y_b=(triVert[2].y-triVert[1].y);

                if ((x_a*y_b-x_b*y_a)<=0){
                    TriangFirstPlaneY(triVert,numVert,ch,X,Y);
                }
            }
        }
        p->Draw(Y,X);
        X+=s_x;
    }
	#endif
}
//************************************* AlignPlane **********************************************
int AlignPlane(int step,TAlignPlane &prev,TAlignPlane &cur,Ship &view){

    switch (view.dir){
        case 0:{
            if (prev.startInd>cur.startInd){
                prev.pStart+=cur.startInd-prev.startInd;
                prev.pStartc+=cur.startInd-prev.startInd;
                CalcCoordByY_First(step,prev.pStart,prev.pStartc,cur.startInd,prev.startInd-1,quality);
                prev.startInd=cur.startInd;
            }
            else if (prev.startInd<cur.startInd){
                prev.pStart+=cur.startInd-prev.startInd;
                prev.pStartc+=cur.startInd-prev.startInd;
                prev.startInd=cur.startInd;
            }

            if (prev.endInd<cur.endInd){
                CalcCoordByY_First(step,prev.pEnd+1,prev.pEndc+1,prev.endInd+1,cur.endInd,quality);
                prev.pEnd+=cur.endInd-prev.endInd;
                prev.pEndc+=cur.endInd-prev.endInd;
                prev.endInd=cur.endInd;
            }
            else if (cur.endInd<prev.endInd){
                prev.pEnd+=cur.endInd-prev.endInd;
                prev.pEndc+=cur.endInd-prev.endInd;
                prev.endInd=cur.endInd;
            }
            break;
        }
        case 1:{
            if (cur.startInd>prev.startInd){
                prev.pStart+=prev.startInd-cur.startInd;
                prev.pStartc+=prev.startInd-cur.startInd;
                CalcCoordByX_First(step,prev.pStart,prev.pStartc,cur.startInd,prev.startInd+1,quality);
                prev.startInd=cur.startInd;
            }
            else if (cur.startInd<prev.startInd){
                prev.pStart+=prev.startInd-cur.startInd;
                prev.pStartc+=prev.startInd-cur.startInd;
                prev.startInd=cur.startInd;
            }

            if (prev.endInd>cur.endInd){
                CalcCoordByX_First(step,prev.pEnd+1,prev.pEndc+1,prev.endInd-1,cur.endInd,quality);
                prev.pEnd+=prev.endInd-cur.endInd;
                prev.pEndc+=prev.endInd-cur.endInd;
                prev.endInd=cur.endInd;
            }
            else if (cur.endInd>prev.endInd){
                prev.pEnd+=prev.endInd-cur.endInd;
                prev.pEndc+=prev.endInd-cur.endInd;
                prev.endInd=cur.endInd;
            }
            break;
        }
        case 2:{
            if (cur.startInd>prev.startInd){
                prev.pStart+=prev.startInd-cur.startInd;
                prev.pStartc+=prev.startInd-cur.startInd;
                CalcCoordByY_First(step,prev.pStart,prev.pStartc,cur.startInd,prev.startInd+1,quality);
                prev.startInd=cur.startInd;
            }
            else if (cur.startInd<prev.startInd){
                prev.pStart+=prev.startInd-cur.startInd;
                prev.pStartc+=prev.startInd-cur.startInd;
                prev.startInd=cur.startInd;
            }

            if (prev.endInd>cur.endInd){
                CalcCoordByY_First(step,prev.pEnd+1,prev.pEndc+1,prev.endInd-1,cur.endInd,quality);
                prev.pEnd+=prev.endInd-cur.endInd;
                prev.pEndc+=prev.endInd-cur.endInd;
                prev.endInd=cur.endInd;
            }
            else if (cur.endInd>prev.endInd){
                prev.pEnd+=prev.endInd-cur.endInd;
                prev.pEndc+=prev.endInd-cur.endInd;
                prev.endInd=cur.endInd;
            }
            break;
        }
        case 3:{
            if (prev.startInd>cur.startInd){
                prev.pStart+=cur.startInd-prev.startInd;
                prev.pStartc+=cur.startInd-prev.startInd;
                CalcCoordByX_First(step,prev.pStart,prev.pStartc,cur.startInd,prev.startInd-1,quality);
                prev.startInd=cur.startInd;
            }
            else if (prev.startInd<cur.startInd){
                prev.pStart+=cur.startInd-prev.startInd;
                prev.pStartc+=cur.startInd-prev.startInd;
                prev.startInd=cur.startInd;
            }

            if (prev.endInd<cur.endInd){
                CalcCoordByX_First(step,prev.pEnd+1,prev.pEndc+1,prev.endInd+1,cur.endInd,quality);
                prev.pEnd+=cur.endInd-prev.endInd;
                prev.pEndc+=cur.endInd-prev.endInd;
                prev.endInd=cur.endInd;
            }
            else if (cur.endInd<prev.endInd){
                prev.pEnd+=cur.endInd-prev.endInd;
                prev.pEndc+=cur.endInd-prev.endInd;
                prev.endInd=cur.endInd;
            }
            break;
        }
    }
    return abs(prev.endInd-prev.startInd);
}
//************************************* DrawRectByYRtoL *********************************************
void DrawRectByYRtoL(TAlignPlane &curPlane,TAlignPlane &nextPlane,CLandObjectMap1 *p,int st,int en){

    TAlignPlane buf;
    int         i=st,sum;
    int         start,end;

    sum=CalcCoordByY_First(i,curPlane.pStart,curPlane.pStartc,curPlane.startInd,curPlane.endInd,quality);
    curPlane.pEnd=curPlane.pStart+sum;
    curPlane.pEndc=curPlane.pStartc+sum;

    i += (1<<quality);
    if (madsh.dir == 0){
        while (i<=en){

            nextPlane.startInd=hcoordrtmp[i].y;
            nextPlane.endInd=hcoordltmp[i].y;

            sum=CalcCoordByY_First(i,nextPlane.pStart,nextPlane.pStartc,nextPlane.startInd,nextPlane.endInd,quality);
            nextPlane.pEnd=nextPlane.pStart+sum;
            nextPlane.pEndc=nextPlane.pStartc+sum;

            sum=AlignPlane(i-1,curPlane,nextPlane,madsh);

            start=curPlane.startInd;
            end=curPlane.endInd;

            if (curPlane.startInd>=(madsh.y>>8)){
                DrawLayerLtoRByY(curPlane,nextPlane,sum,sum,p,hcoordltmp[i].x,end,start);
            }
            else if (curPlane.endInd<=(madsh.y>>8)){
                DrawLayerRtoLByY(curPlane,nextPlane,sum,p,hcoordltmp[i].x,start,end);
            }
            else{
                DrawLayerRtoLByY(curPlane,nextPlane,abs(curPlane.startInd-(madsh.y>>8)),p,hcoordltmp[i].x,start,madsh.y>>8);
                DrawLayerLtoRByY(curPlane,nextPlane,sum,abs(curPlane.endInd-(madsh.y>>8)),p,hcoordltmp[i].x,end,madsh.y>>8);
            }

            buf=nextPlane;
            nextPlane=curPlane;
            curPlane=buf;

            i += (1<<quality);
        }
    }
    else{
        while (i<=en){

            nextPlane.startInd=hcoordrtmp[i].y;
            nextPlane.endInd=hcoordltmp[i].y;

            sum=CalcCoordByY_First(i,nextPlane.pStart,nextPlane.pStartc,nextPlane.startInd,nextPlane.endInd,quality);
            nextPlane.pEnd=nextPlane.pStart+sum;
            nextPlane.pEndc=nextPlane.pStartc+sum;

            sum=AlignPlane(i-1,curPlane,nextPlane,madsh);

            start=curPlane.startInd;
            end=curPlane.endInd;

            if (curPlane.startInd<=(madsh.y>>8)){
                DrawLayerLtoRByY(curPlane,nextPlane,sum,sum,p,hcoordltmp[i-1].x,end,start);
            }
            else if (curPlane.endInd>=(madsh.y>>8)){
                DrawLayerRtoLByY(curPlane,nextPlane,sum,p,hcoordltmp[i-1].x,start,end);
            }
            else{
                DrawLayerLtoRByY(curPlane,nextPlane,sum,abs(curPlane.endInd-(madsh.y>>8)),p,hcoordltmp[i-1].x,end,madsh.y>>8);
                DrawLayerRtoLByY(curPlane,nextPlane,abs(curPlane.startInd-(madsh.y>>8)),p,hcoordltmp[i-1].x,start,madsh.y>>8);
            }

            buf=nextPlane;
            nextPlane=curPlane;
            curPlane=buf;

            i += (1<<quality);
        }
    }
}
//************************************* DrawRectByYLtoR *********************************************
void DrawRectByYLtoR(TAlignPlane &curPlane,TAlignPlane &nextPlane,CLandObjectMap1 *p,int st,int en){

    TAlignPlane buf;
    int         i=st,sum;
    int         start,end;

    sum=CalcCoordByY_First(i,curPlane.pStart,curPlane.pStartc,curPlane.startInd,curPlane.endInd,quality);
    curPlane.pEnd=curPlane.pStart+sum;
    curPlane.pEndc=curPlane.pStartc+sum;

    i -= (1<<quality);
    if (madsh.dir == 0){
        while (i>=en){

            nextPlane.startInd=hcoordrtmp[i].y;
            nextPlane.endInd=hcoordltmp[i].y;

            sum=CalcCoordByY_First(i,nextPlane.pStart,nextPlane.pStartc,nextPlane.startInd,nextPlane.endInd,quality);
            nextPlane.pEnd=nextPlane.pStart+sum;
            nextPlane.pEndc=nextPlane.pStartc+sum;

            sum=AlignPlane(i+1,curPlane,nextPlane,madsh);

            start=curPlane.startInd;
            end=curPlane.endInd;

            if (curPlane.startInd>=(madsh.y>>8)){
                DrawLayerLtoRByY(nextPlane,curPlane,sum,sum,p,hcoordltmp[i+1].x,end,start);
            }
            else if (curPlane.endInd<=(madsh.y>>8)){
                DrawLayerRtoLByY(nextPlane,curPlane,sum,p,hcoordltmp[i+1].x,start,end);
            }
            else{
                DrawLayerRtoLByY(nextPlane,curPlane,abs(curPlane.startInd-(madsh.y>>8)),p,hcoordltmp[i+1].x,start,madsh.y>>8);
                DrawLayerLtoRByY(nextPlane,curPlane,sum,abs(curPlane.endInd-(madsh.y>>8)),p,hcoordltmp[i+1].x,end,madsh.y>>8);
            }

            buf=nextPlane;
            nextPlane=curPlane;
            curPlane=buf;

            i -= (1<<quality);
        }
    }
    else{
        while (i>=en){

            nextPlane.startInd=hcoordrtmp[i].y;
            nextPlane.endInd=hcoordltmp[i].y;

            sum=CalcCoordByY_First(i,nextPlane.pStart,nextPlane.pStartc,nextPlane.startInd,nextPlane.endInd,quality);
            nextPlane.pEnd=nextPlane.pStart+sum;
            nextPlane.pEndc=nextPlane.pStartc+sum;

            sum=AlignPlane(i+1,curPlane,nextPlane,madsh);

            start=curPlane.startInd;
            end=curPlane.endInd;

            if (curPlane.startInd<=(madsh.y>>8)){
                DrawLayerLtoRByY(nextPlane,curPlane,sum,sum,p,hcoordltmp[i].x,end,start);
            }
            else if (curPlane.endInd>=(madsh.y>>8)){
                DrawLayerRtoLByY(nextPlane,curPlane,sum,p,hcoordltmp[i].x,start,end);
            }
            else{
                DrawLayerLtoRByY(nextPlane,curPlane,sum,abs(curPlane.endInd-(madsh.y>>8)),p,hcoordltmp[i].x,end,madsh.y>>8);
                DrawLayerRtoLByY(nextPlane,curPlane,abs(curPlane.startInd-(madsh.y>>8)),p,hcoordltmp[i].x,start,madsh.y>>8);
            }

            buf=nextPlane;
            nextPlane=curPlane;
            curPlane=buf;

            i -= (1<<quality);
        }
    }
}
//************************************* DrawRectByXRtoL *********************************************
void DrawRectByXRtoL(TAlignPlane &curPlane,TAlignPlane &nextPlane,CLandObjectMap1 *p,int st,int en){

    TAlignPlane buf;
    int         i=st,sum;
    int         start,end;

    sum=CalcCoordByX_First(i,curPlane.pStart,curPlane.pStartc,curPlane.startInd,curPlane.endInd,quality);
    curPlane.pEnd=curPlane.pStart+sum;
    curPlane.pEndc=curPlane.pStartc+sum;

    i += (1<<quality);
    if (madsh.dir == 3){
        while (i<=en){

            nextPlane.startInd=hcoordrtmp[i].x;
            nextPlane.endInd=hcoordltmp[i].x;

            sum=CalcCoordByX_First(i,nextPlane.pStart,nextPlane.pStartc,nextPlane.startInd,nextPlane.endInd,quality);
            nextPlane.pEnd=nextPlane.pStart+sum;
            nextPlane.pEndc=nextPlane.pStartc+sum;

            sum=AlignPlane(i-1,curPlane,nextPlane,madsh);

            start=curPlane.startInd;
            end=curPlane.endInd;

            if (curPlane.startInd>=(madsh.x>>8)){
                DrawLayerLtoRByX(curPlane,nextPlane,sum,sum,p,hcoordltmp[i-1].y,end,start);
            }
            else if (curPlane.endInd<=(madsh.x>>8)){
                DrawLayerRtoLByX(curPlane,nextPlane,sum,p,hcoordltmp[i-1].y,start,end);
            }
            else{
                DrawLayerRtoLByX(curPlane,nextPlane,abs(curPlane.startInd-(madsh.x>>8)),p,hcoordltmp[i-1].y,start,madsh.x>>8);
                DrawLayerLtoRByX(curPlane,nextPlane,sum,abs(curPlane.endInd-(madsh.x>>8)),p,hcoordltmp[i-1].y,end,madsh.x>>8);
            }

            buf=nextPlane;
            nextPlane=curPlane;
            curPlane=buf;

            i += (1<<quality);
        }
    }
    else{
        while (i<=en){

            nextPlane.startInd=hcoordrtmp[i].x;
            nextPlane.endInd=hcoordltmp[i].x;

            sum=CalcCoordByX_First(i,nextPlane.pStart,nextPlane.pStartc,nextPlane.startInd,nextPlane.endInd,quality);
            nextPlane.pEnd=nextPlane.pStart+sum;
            nextPlane.pEndc=nextPlane.pStartc+sum;

            sum=AlignPlane(i-1,curPlane,nextPlane,madsh);

            start=curPlane.startInd;
            end=curPlane.endInd;

            if (curPlane.startInd<=(madsh.x>>8)){
                DrawLayerLtoRByX(curPlane,nextPlane,sum,sum,p,hcoordltmp[i].y,end,start);
            }
            else if (curPlane.endInd>=(madsh.x>>8)){
                DrawLayerRtoLByX(curPlane,nextPlane,sum,p,hcoordltmp[i].y,start,end);
            }
            else{
                DrawLayerLtoRByX(curPlane,nextPlane,sum,abs(curPlane.endInd-(madsh.x>>8)),p,hcoordltmp[i].y,end,madsh.x>>8);
                DrawLayerRtoLByX(curPlane,nextPlane,abs(curPlane.startInd-(madsh.x>>8)),p,hcoordltmp[i].y,start,madsh.x>>8);
            }

            buf=nextPlane;
            nextPlane=curPlane;
            curPlane=buf;

            i += (1<<quality);
        }
    }
}
//************************************* DrawRectByXLtoR *********************************************
void DrawRectByXLtoR(TAlignPlane &curPlane,TAlignPlane &nextPlane,CLandObjectMap1 *p,int st,int en){

    TAlignPlane buf;
    int         i=st,sum;
    int         start,end;

    sum=CalcCoordByX_First(i,curPlane.pStart,curPlane.pStartc,curPlane.startInd,curPlane.endInd,quality);
    curPlane.pEnd=curPlane.pStart+sum;
    curPlane.pEndc=curPlane.pStartc+sum;

    i -= (1<<quality);
    if (madsh.dir == 3){
        while (i>=en){

            nextPlane.startInd=hcoordrtmp[i].x;
            nextPlane.endInd=hcoordltmp[i].x;

            sum=CalcCoordByX_First(i,nextPlane.pStart,nextPlane.pStartc,nextPlane.startInd,nextPlane.endInd,quality);
            nextPlane.pEnd=nextPlane.pStart+sum;
            nextPlane.pEndc=nextPlane.pStartc+sum;

            sum=AlignPlane(i+1,curPlane,nextPlane,madsh);

            start=curPlane.startInd;
            end=curPlane.endInd;

            if (curPlane.startInd>=(madsh.x>>8)){
                DrawLayerLtoRByX(nextPlane,curPlane,sum,sum,p,hcoordltmp[i].y,end,start);
            }
            else if (curPlane.endInd<=(madsh.x>>8)){
                DrawLayerRtoLByX(nextPlane,curPlane,sum,p,hcoordltmp[i].y,start,end);
            }
            else{
                DrawLayerRtoLByX(nextPlane,curPlane,abs(curPlane.startInd-(madsh.x>>8)),p,hcoordltmp[i].y,start,madsh.x>>8);
                DrawLayerLtoRByX(nextPlane,curPlane,sum,abs(curPlane.endInd-(madsh.x>>8)),p,hcoordltmp[i].y,end,madsh.x>>8);
            }

            buf=nextPlane;
            nextPlane=curPlane;
            curPlane=buf;

            i -= (1<<quality);
        }
    }
    else{
        while (i>=en){

            nextPlane.startInd=hcoordrtmp[i].x;
            nextPlane.endInd=hcoordltmp[i].x;

            sum=CalcCoordByX_First(i,nextPlane.pStart,nextPlane.pStartc,nextPlane.startInd,nextPlane.endInd,quality);
            nextPlane.pEnd=nextPlane.pStart+sum;
            nextPlane.pEndc=nextPlane.pStartc+sum;

            sum=AlignPlane(i+1,curPlane,nextPlane,madsh);

            start=curPlane.startInd;
            end=curPlane.endInd;

            if (curPlane.startInd<=(madsh.x>>8)){
                DrawLayerLtoRByX(nextPlane,curPlane,sum,sum,p,hcoordltmp[i+1].y,end,start);
            }
            else if (curPlane.endInd>=(madsh.x>>8)){
                DrawLayerRtoLByX(nextPlane,curPlane,sum,p,hcoordltmp[i+1].y,start,end);
            }
            else{
                DrawLayerLtoRByX(nextPlane,curPlane,sum,abs(curPlane.endInd-(madsh.x>>8)),p,hcoordltmp[i+1].y,end,madsh.x>>8);
                DrawLayerRtoLByX(nextPlane,curPlane,abs(curPlane.startInd-(madsh.x>>8)),p,hcoordltmp[i+1].y,start,madsh.x>>8);
            }

            buf=nextPlane;
            nextPlane=curPlane;
            curPlane=buf;

            i -= (1<<quality);
        }
    }
}
//************************************* ShowLand ************************************************
void ShowLandFragment(TCCRect2 &rect,CLandObjectMap2 &map2){

    if (!IntersectQuest(madsh,rect)) return;
    CreateLandFragment(madsh,rect,hcoordltmp,hcoordrtmp);


	#if _USE_OWNGR
    	ASSERT(_gr_pYCache);
    	Init_pYCache();
	#endif

    int         i=0;
    int         cntEnd=cnt;

    if (cnt<=1) return;
    cntEnd--;

	GRSetZPrecision(0);

	#if _USE_OPENGL
		glPushAttrib(GL_POLYGON_BIT|GL_FOG_BIT|GL_LIGHTING_BIT);
			// GL_ENABLE_BIT: GL_CULL_FACE, GL_FOG
			// GL_FOG_BIT: GL_FOG
			// GL_LIGHTING_BIT: GL_SHADE_MODEL
			// GL_POLYGON_BIT: GL_CULL_FACE, GL_CULL_FACE_MODE, GL_FRONT_FACE
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_FOG);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glScaled(madsh.fCellSize,
					madsh.fCellSize*(double)madsh.hratio/(1<<PRES),
					madsh.fCellSize);
	#endif
        if (madsh.axis==0){ // главная ось - X
            CLandObjectMap1 *p=&map2.MapX();
                        p->SetViewPoint(madsh.x>>PRES,madsh.y>>PRES);
                        p->SetReduction(0,0);

            TAlignPlane curPlane,nextPlane;

            if (madsh.dir == 0){
                if (hcoordltmp[cntEnd-1].x>=(madsh.x>>8)){
                    i=0;

                    curPlane.pStart=&maplinep[3000];
                    curPlane.pStartc=&clipLinep[3000];
                    curPlane.startInd=hcoordrtmp[i].y;
                    curPlane.endInd=hcoordltmp[i].y;

                    nextPlane.pStart=&mapline[3000];
                    nextPlane.pStartc=&clipLine[3000];

                    DrawRectByYRtoL(curPlane,nextPlane,p,0,cntEnd);

                }
                else if (hcoordltmp[0].x<=(madsh.x>>8)){
                        i=cntEnd-1;

                        curPlane.pStart=&maplinep[3000];
                        curPlane.pStartc=&clipLinep[3000];
                        curPlane.startInd=hcoordrtmp[i].y;
                        curPlane.endInd=hcoordltmp[i].y;

                        nextPlane.pStart=&mapline[3000];
                        nextPlane.pStartc=&clipLine[3000];

                        DrawRectByYLtoR(curPlane,nextPlane,p,cntEnd,0);
                }
                else{
                    i=cntEnd-1;

                    curPlane.pStart=&maplinep[3000];
                    curPlane.pStartc=&clipLinep[3000];
                    curPlane.startInd=hcoordrtmp[i].y;
                    curPlane.endInd=hcoordltmp[i].y;

                    nextPlane.pStart=&mapline[3000];
                    nextPlane.pStartc=&clipLine[3000];

                    DrawRectByYLtoR(curPlane,nextPlane,p,cntEnd,hcoordltmp[0].x-(madsh.x>>8));

                    i=0;

                    curPlane.pStart=&maplinep[3000];
                    curPlane.pStartc=&clipLinep[3000];
                    curPlane.startInd=hcoordrtmp[i].y;
                    curPlane.endInd=hcoordltmp[i].y;

                    nextPlane.pStart=&mapline[3000];
                    nextPlane.pStartc=&clipLine[3000];

                    DrawRectByYRtoL(curPlane,nextPlane,p,0,hcoordltmp[0].x-(madsh.x>>8));

                }

            }
            else{
                if (hcoordltmp[cntEnd-1].x<=(madsh.x>>8)){
                    i=0;

                    curPlane.pStart=&maplinep[3000];
                    curPlane.pStartc=&clipLinep[3000];
                    curPlane.startInd=hcoordrtmp[i].y;
                    curPlane.endInd=hcoordltmp[i].y;

                    nextPlane.pStart=&mapline[3000];
                    nextPlane.pStartc=&clipLine[3000];

                    DrawRectByYRtoL(curPlane,nextPlane,p,0,cntEnd);
                }
                else if (hcoordltmp[0].x>=(madsh.x>>8)){
                        i=cntEnd-1;

                        curPlane.pStart=&maplinep[3000];
                        curPlane.pStartc=&clipLinep[3000];
                        curPlane.startInd=hcoordrtmp[i].y;
                        curPlane.endInd=hcoordltmp[i].y;

                        nextPlane.pStart=&mapline[3000];
                        nextPlane.pStartc=&clipLine[3000];

                        DrawRectByYLtoR(curPlane,nextPlane,p,cntEnd,0);
                }
                else{

                    i=0;

                    curPlane.pStart=&maplinep[3000];
                    curPlane.pStartc=&clipLinep[3000];
                    curPlane.startInd=hcoordrtmp[i].y;
                    curPlane.endInd=hcoordltmp[i].y;

                    nextPlane.pStart=&mapline[3000];
                    nextPlane.pStartc=&clipLine[3000];

                    DrawRectByYRtoL(curPlane,nextPlane,p,0,(madsh.x>>8)-hcoordltmp[0].x);

                    i=cntEnd-1;

                    curPlane.pStart=&maplinep[3000];
                    curPlane.pStartc=&clipLinep[3000];
                    curPlane.startInd=hcoordrtmp[i].y;
                    curPlane.endInd=hcoordltmp[i].y;

                    nextPlane.pStart=&mapline[3000];
                    nextPlane.pStartc=&clipLine[3000];

                    DrawRectByYLtoR(curPlane,nextPlane,p,cntEnd,(madsh.x>>8)-hcoordltmp[0].x);

                }
            }
        }
        else   // главная ось - Y
        {

            CLandObjectMap1 *p=&map2.MapY();
                        p->SetViewPoint(madsh.x>>PRES,madsh.y>>PRES);
                        p->SetReduction(0,0);

            TAlignPlane curPlane,nextPlane;

            if (madsh.dir == 1){
                if (hcoordltmp[cntEnd-1].y>=(madsh.y>>8)){
                    i=0;

                    curPlane.pStart=&maplinep[3000];
                    curPlane.pStartc=&clipLinep[3000];
                    curPlane.startInd=hcoordrtmp[i].x;
                    curPlane.endInd=hcoordltmp[i].x;

                    nextPlane.pStart=&mapline[3000];
                    nextPlane.pStartc=&clipLine[3000];

                    DrawRectByXRtoL(curPlane,nextPlane,p,0,cntEnd);

                }
                else if (hcoordltmp[0].y<=(madsh.y>>8)){
                        i=cntEnd-1;

                        curPlane.pStart=&maplinep[3000];
                        curPlane.pStartc=&clipLinep[3000];
                        curPlane.startInd=hcoordrtmp[i].x;
                        curPlane.endInd=hcoordltmp[i].x;

                        nextPlane.pStart=&mapline[3000];
                        nextPlane.pStartc=&clipLine[3000];

                        DrawRectByXLtoR(curPlane,nextPlane,p,cntEnd,0);
                }
                else{

                    i=cntEnd-1;

                    curPlane.pStart=&maplinep[3000];
                    curPlane.pStartc=&clipLinep[3000];
                    curPlane.startInd=hcoordrtmp[i].x;
                    curPlane.endInd=hcoordltmp[i].x;

                    nextPlane.pStart=&mapline[3000];
                    nextPlane.pStartc=&clipLine[3000];

                    DrawRectByXLtoR(curPlane,nextPlane,p,cntEnd,hcoordltmp[0].y-(madsh.y>>8));

                    i=0;

                    curPlane.pStart=&maplinep[3000];
                    curPlane.pStartc=&clipLinep[3000];
                    curPlane.startInd=hcoordrtmp[i].x;
                    curPlane.endInd=hcoordltmp[i].x;

                    nextPlane.pStart=&mapline[3000];
                    nextPlane.pStartc=&clipLine[3000];

                    DrawRectByXRtoL(curPlane,nextPlane,p,0,hcoordltmp[0].y-(madsh.y>>8));


                }

            }
            else{
                if (hcoordltmp[cntEnd-1].y<=(madsh.y>>8)){
                    i=0;

                    curPlane.pStart=&maplinep[3000];
                    curPlane.pStartc=&clipLinep[3000];
                    curPlane.startInd=hcoordrtmp[i].x;
                    curPlane.endInd=hcoordltmp[i].x;

                    nextPlane.pStart=&mapline[3000];
                    nextPlane.pStartc=&clipLine[3000];

                    DrawRectByXRtoL(curPlane,nextPlane,p,0,cntEnd);
                }
                else if (hcoordltmp[0].y>=(madsh.y>>8)){
                        i=cntEnd-1;

                        curPlane.pStart=&maplinep[3000];
                        curPlane.pStartc=&clipLinep[3000];
                        curPlane.startInd=hcoordrtmp[i].x;
                        curPlane.endInd=hcoordltmp[i].x;

                        nextPlane.pStart=&mapline[3000];
                        nextPlane.pStartc=&clipLine[3000];

                        DrawRectByXLtoR(curPlane,nextPlane,p,cntEnd,0);
                }
                else{

                    i=0;

                    curPlane.pStart=&maplinep[3000];
                    curPlane.pStartc=&clipLinep[3000];
                    curPlane.startInd=hcoordrtmp[i].x;
                    curPlane.endInd=hcoordltmp[i].x;

                    nextPlane.pStart=&mapline[3000];
                    nextPlane.pStartc=&clipLine[3000];

                    DrawRectByXRtoL(curPlane,nextPlane,p,0,(madsh.y>>8)-hcoordltmp[0].y);

                    i=cntEnd-1;

                    curPlane.pStart=&maplinep[3000];
                    curPlane.pStartc=&clipLinep[3000];
                    curPlane.startInd=hcoordrtmp[i].x;
                    curPlane.endInd=hcoordltmp[i].x;

                    nextPlane.pStart=&mapline[3000];
                    nextPlane.pStartc=&clipLine[3000];

                    DrawRectByXLtoR(curPlane,nextPlane,p,cntEnd,(madsh.y>>8)-hcoordltmp[0].y);
                }
            }
    }

	#if _USE_OPENGL
		glPopAttrib();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	#endif
}
//*************************************** SetLandParams ******************************************
void SetLandParams(double fCellSize,double fHRatio,int planeNum=0){
    madsh.hratio=double(1<<PRES)*fHRatio;
    madsh.fCellSize=fCellSize;
    (void)planeNum;
}
//*************************************** SetLandViewPoint ***************************************
void SetLandViewPoint(void){
    double pres=1<<PRES;
    double shx,shy,shz;

    madsh.x = shx = CViewObject::m_viewPointInvMx.m[0][3]/madsh.fCellSize*pres;
    madsh.y = shy = -CViewObject::m_viewPointInvMx.m[2][3]/madsh.fCellSize*pres;
    madsh.z = shz = CViewObject::m_viewPointInvMx.m[1][3]/madsh.fCellSize*pres;

    double  fAngleY = atan2(-CViewObject::m_viewPointDirMx.m[2][0],CViewObject::m_viewPointDirMx.m[2][2]);
    double  fAngleX = atan2(CViewObject::m_viewPointDirMx.m[2][1],hypot(CViewObject::m_viewPointDirMx.m[2][0],CViewObject::m_viewPointDirMx.m[2][2]));
    CFVector3   normalY(CViewObject::m_viewPointDirMx.m[1][0],CViewObject::m_viewPointDirMx.m[1][1],CViewObject::m_viewPointDirMx.m[1][2]);
        double  fAngleZ = atan2(CFVector3(cos(fAngleY),0,sin(fAngleY))*normalY,
                CFVector3(sin(fAngleX)*sin(fAngleY),cos(fAngleX),-sin(fAngleX)*cos(fAngleY))*normalY);

    madsh.va = -fAngleX;
    madsh.ha = M_PI/2-fAngleY;
    madsh.ia = -fAngleZ;

    CFRect2 angle;
    angle.left = atan( CViewObject::m_clipRect0.left*-CViewObject::m_viewPointScale.z );
    angle.top = atan( CViewObject::m_clipRect0.bottom*-CViewObject::m_viewPointScale.z );
    angle.right = atan( CViewObject::m_clipRect0.right*-CViewObject::m_viewPointScale.z );
    angle.bottom = atan( CViewObject::m_clipRect0.top*-CViewObject::m_viewPointScale.z );

    madsh.len=CViewFigure::HazeMax()*pres/(madsh.fCellSize)+1;

#if _VIEW_ORTHO
    if( CViewObject::ViewOrtho() ) {
        madsh.w_h_ang = ((100*M_PI/180))/2;
        madsh.h_h_ang = ((100*M_PI/180))/2;
        madsh.w_st_ang = madsh.w_h_ang;
        madsh.h_st_ang = madsh.h_h_ang;
        madsh.stlen=madsh.len;
    }
    else
#endif
         {
            madsh.w_h_ang = (angle.right-angle.left+(10*M_PI/180))/2;
            madsh.h_h_ang = (angle.bottom-angle.top+(10*M_PI/180))/2;
            madsh.w_st_ang = madsh.w_h_ang;
            madsh.h_st_ang = madsh.h_h_ang;
            madsh.stlen=STLEN;
         }

    CalcConnerPoint(&madsh);
    //CoordToBuff(&madsh);
    CreatePolygon(madsh);

	double	pres4 = pres*4;
    m = CViewObject::m_viewPointDirSMx;
    m.ScaleR(CFVector3(madsh.fCellSize/pres,madsh.fCellSize/pres,madsh.fCellSize/pres));
    m.ScaleR(CFVector3(pres,madsh.hratio,-pres));
    m.ScaleL(CFVector3(pres4/madsh.fCellSize,pres4/madsh.fCellSize,pres4/madsh.fCellSize));

	#if _USE_OPENGL
		_glLandBumpAxisZ = CFVector3(
			CViewObject::m_viewPointDirMx.m[2][0]*madsh.fCellSize,
			CViewObject::m_viewPointDirMx.m[2][1]*madsh.fCellSize*madsh.hratio/pres,
			CViewObject::m_viewPointDirMx.m[2][2]*madsh.fCellSize);
	#endif

    CalcSplitCoords(madsh.boundMinX>>quality, madsh.boundMinY>>quality,
                madsh.boundMaxX>>quality,madsh.boundMaxY>>quality,
                (512>>quality)-1);

}
//************************************* InvertHaze ***********************************************

void InvertHaze(void)
{
        for( int i = 0 ; i < 16 ; i++ )
                memcpy(aHaze+i*256,CPaletteTranslator::Haze()+(15-i)*256,256);
}


extern void CalcMaskCache();
//*************************************** LoadLand ************************************************
void LoadLand(void){

    LoadMap(&map,"covh7.spr");       //Загpузка каpты высот и
        LoadMapC(&mapc,"mapc7.spr");     //соответствующей каpты индексов цветов

        LoadMap(&map1,"covh7$1.spr");
        LoadMapC(&mapc1,"mapc7$1.spr");

        LoadMap(&map2,"covh7$2.spr");
        LoadMapC(&mapc2,"mapc7$2.spr");

        LoadMap(&map3,"covh7$3.spr");
        LoadMapC(&mapc3,"mapc7$3.spr");

        LoadMap(&map4,"covh7$4.spr");
        LoadMapC(&mapc4,"mapc7$4.spr");

        LoadDithMask(&dithMask,"dith.dth");
        maskDithPtr=dithMask.mapadr;

    for (int i=1;i<=16;i++) {
        char lname[64];
        //char buf[32];
		sprintf(lname,"bump/a%d.spr",i);
        //strcpy(lname,"bump\\a");
        //strcat(lname,itoa(i,buf,10));
        //strcat(lname,".spr");
        LoadMap(&bMask[i-1],lname);
    }
	#if _USE_OPENGL
		#ifdef __sgi
			glGenTexturesEXT(16,_glLandBumpBindIdx);
		#endif
		/*int maxbump = -128;
		for( i= 0 ; i < 16 ;i++) {
			MapH &mask = bMask[i];
			int i, dims = (int)mask.x*mask.y;
			for( i = 0 ; i < dims ; i++ )
				maxbump = Max(maxbump,(signed char)mask.mapadr[i]);
		}
		maxbump = 255-maxbump;*/
		for( i= 0 ; i < 16 ;i++) {
			{
				MapH &mask = bMask[i];
				int i, dims = (int)mask.x*mask.y;
				for( i = 0 ; i < dims ; i++ )
					((byte *)mask.mapadr)[i] =
							(byte)(128+((signed char *)mask.mapadr)[i]);
			}
			#ifdef __sgi
				glPushAttrib(GL_TEXTURE_BIT);
				glBindTextureEXT(GL_TEXTURE_2D,_glLandBumpBindIdx[i]);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexImage2D(GL_TEXTURE_2D,0,1,bMask[i].x,bMask[i].y,
						0,GL_LUMINANCE,GL_UNSIGNED_BYTE,bMask[i].mapadr);
				glPopAttrib();
			#endif
		}
	#endif

    LoadMap(&mapmask,"mask2.spr");      //Загpузка маски цветов
    InvertHaze();

        maps[0] = &map;
        maps[1] = &map1;
        maps[2] = &map2;
        maps[3] = &map3;
        maps[4] = &map4;

        clipmaps[0] = &mapc;
        clipmaps[1] = &mapc1;
        clipmaps[2] = &mapc2;
        clipmaps[3] = &mapc3;
        clipmaps[4] = &mapc4;

		#if !_USE_OPENGL
        	InitSbLoop(&mapmask);
	    	LineComressBumpInit();
			CalcMaskCache();
		#endif

		MatchLandColorsFile("mask2.pal");
}
//**************************************** UnloadLand *********************************************
void UnloadLand(void){

	#if 1 //_USE_GLIDE
    	#define UNLOADMAP(m) if (m##.mapadr != NULL) free(m##.mapadr-12);
	#else
    	#define UNLOADMAP(m) if (m##.mapadr != NULL) free(m##.mapadr);
	#endif
	#define UNLOADMAPC(m) if (m##.mapadr != NULL) free(m##.mapadr);
	#define UNLOADMAPDITH(m) if (m##.mapadr != NULL) free(m##.mapadr);

        UNLOADMAP(map);
        UNLOADMAPC(mapc);
        UNLOADMAP(map1);
        UNLOADMAPC(mapc1);
        UNLOADMAP(map2);
        UNLOADMAPC(mapc2);
        UNLOADMAP(map3);
        UNLOADMAPC(mapc3);
        UNLOADMAP(map4);
        UNLOADMAPC(mapc4);
        UNLOADMAPDITH(dithMask);
        UNLOADMAP(mapmask);
        for (int i=0;i<16;i++) UNLOADMAP(bMask[i]);
    #undef  UNLOADMAP
	#undef	UNLOADMAPC
	#undef	UNLOADMAPDITH
}

