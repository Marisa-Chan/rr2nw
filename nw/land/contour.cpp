#include <stdio.h>
//#include <process.h>
#ifdef MSDOS
	#include <conio.h>
#endif
#include "_land.h"
#include "mathlib.h"
#include "triang.h"
#include "poly.h"

#define __CONTOUR_C__
#include "contour.h"


inline void FindMaxMin_X(MapCoord* p, int *max, int *min)
{
	int i_max = 0;
	int i_min = 0;

	if ((p[1].x > p[i_max].x)||
			(p[1].x == p[i_max].x)&&(p[1].y > p[i_max].y)) i_max = 1;
	else if ((p[1].x < p[i_min].x)||
					(p[1].x == p[i_min].x)&&(p[1].y < p[i_min].y)) i_min = 1;

	if ((p[2].x > p[i_max].x)||
			(p[2].x == p[i_max].x)&&(p[2].y > p[i_max].y)) i_max = 2;
	else if ((p[2].x < p[i_min].x)||
					(p[2].x == p[i_min].x)&&(p[2].y < p[i_min].y)) i_min = 2;

	if ((p[3].x > p[i_max].x)||
			(p[3].x == p[i_max].x)&&(p[3].y > p[i_max].y)) i_max = 3;
	else if ((p[3].x < p[i_min].x)||
					(p[3].x == p[i_min].x)&&(p[3].y < p[i_min].y)) i_min = 3;

	*max = i_max;
	*min = i_min;
}

inline void FindMaxMin_Y(MapCoord* p, int *max, int *min)
{
	int i_max = 0;
	int i_min = 0;

	if ((p[1].y > p[i_max].y)||
			(p[1].y == p[i_max].y)&&(p[1].x > p[i_max].x)) i_max = 1;
	else if ((p[1].y < p[i_min].y)||
					(p[1].y == p[i_min].y)&&(p[1].x < p[i_min].x)) i_min = 1;

	if ((p[2].y > p[i_max].y)||
			(p[2].y == p[i_max].y)&&(p[2].x > p[i_max].x)) i_max = 2;
	else if ((p[2].y < p[i_min].y)||
					(p[2].y == p[i_min].y)&&(p[2].x < p[i_min].x)) i_min = 2;

	if ((p[3].y > p[i_max].y)||
			(p[3].y == p[i_max].y)&&(p[3].x > p[i_max].x)) i_max = 3;
	else if ((p[3].y < p[i_min].y)||
					(p[3].y == p[i_min].y)&&(p[3].x < p[i_min].x)) i_min = 3;

	*max = i_max;
	*min = i_min;
}


long Bresenham_X(MapCoord* line, CFVector2& st, CFVector2& en)
{
	MapCoord f,l;
	f.x = (long)st.x>>8;
	f.y = (long)st.y>>8;
	l.x = (long)en.x>>8;
	l.y = (long)en.y>>8;

	long d_x = l.x - f.x;
	long dx = ABS(d_x);
	long sx = SIG(d_x);

	long i, curX;

//	if (d_x == 0){
//		line->x = f.x;
//		line->y = l.y;
//		return 1;
//	}

	if (d_x == 0) return 0;

	MapCoord *mapbuf=line;
	long stY = (f.y<<16)+(1<<15)-1;
	long step = ((l.y - f.y)<<16)/dx;

	for(i=dx, curX=f.x; i>0; curX+=sx, i--){
		mapbuf->y=stY>>16;
		mapbuf->x=curX;
		stY+=step;
		mapbuf++;
	}
	return dx;

}

long Bresenham_Y(MapCoord* line, CFVector2& st, CFVector2& en)
{
	MapCoord f,l;
	f.x = (long)st.x>>8;
	f.y = (long)st.y>>8;
	l.x = (long)en.x>>8;
	l.y = (long)en.y>>8;

	long d_y = l.y - f.y;
	long dy = ABS(d_y);
	long sy = SIG(d_y);

	long i, curY;

//	if (d_y == 0){
//		line->x = l.x;
//		line->y = f.y;
//		return 1;
//	}

	if (d_y == 0) return 0;

	MapCoord *mapbuf=line;
	long stX = (f.x<<16)+(1<<15)-1;
	long step = ((l.x - f.x)<<16)/dy;

	for(i=dy, curY=f.y; i>0; curY+=sy, i--){
		mapbuf->x=stX>>16;
		mapbuf->y=curY;
		stX+=step;
		mapbuf++;
	}
	return dy;

}

#define MAKE_LEG(x) (x>1? 1:(x<-1 ? -1 : x))
#define CHECK_BRANCH(br) \
	if ( Abs2(br) < 5 ) { br.x = br.y = 0; }


void CreatePolygon(Ship &view){
    CPolygon  gndPol;
    SVertex   oneVert;
    CFVector2 polVert[10],*polMaxX,*polMinX,*polMaxY,*polMinY;
    CFVector2 *polStart,*polEnd,*polPtr,*polPtrPrev;
    int       cnt,i;

    if ((view.pyr_vertex[0].x == view.pyr_vertex[1].x) &&
        (view.pyr_vertex[0].x == view.pyr_vertex[2].x) &&
        (view.pyr_vertex[0].x == view.pyr_vertex[3].x) ){

        int indMax=0,indMin=0;
        int yMax=view.pyr_vertex[0].y,yMin=view.pyr_vertex[0].y;

        for(i=1;i<4;i++){
            if (view.pyr_vertex[i].y>yMax){
                yMax=view.pyr_vertex[i].y;
                indMax=i;
            }
            else if (view.pyr_vertex[i].y<yMin){
                yMin=view.pyr_vertex[i].y;
                indMin=i;
            }
        }

        oneVert.pnt.x=view.pyr_vertex[indMin].x;
        oneVert.pnt.y=view.pyr_vertex[indMin].y;
        gndPol.AddVertex(oneVert);

        oneVert.pnt.x=view.pyr_vertex[indMax].x;
        oneVert.pnt.y=view.pyr_vertex[indMax].y;
        gndPol.AddVertex(oneVert);

        indMax=4,indMin=4;
        yMax=view.pyr_vertex[4].y,yMin=view.pyr_vertex[4].y;

        for(;i<8;i++){
            if (view.pyr_vertex[i].y>yMax){
                yMax=view.pyr_vertex[i].y;
                indMax=i;
            }
            else if (view.pyr_vertex[i].y<yMin){
                yMin=view.pyr_vertex[i].y;
                indMin=i;
            }
        }

        oneVert.pnt.x=view.pyr_vertex[indMin].x;
        oneVert.pnt.y=view.pyr_vertex[indMin].y;
        gndPol.AddVertex(oneVert);

        oneVert.pnt.x=view.pyr_vertex[indMax].x;
        oneVert.pnt.y=view.pyr_vertex[indMax].y;
        gndPol.AddVertex(oneVert);

    }
    else if ((view.pyr_vertex[0].y == view.pyr_vertex[1].y) &&
             (view.pyr_vertex[0].y == view.pyr_vertex[2].y) &&
             (view.pyr_vertex[0].y == view.pyr_vertex[3].y) ){

        int indMax=0,indMin=0;
        int xMax=view.pyr_vertex[0].x,xMin=view.pyr_vertex[0].x;

        for(i=1;i<4;i++){
            if (view.pyr_vertex[i].x>xMax){
                xMax=view.pyr_vertex[i].x;
                indMax=i;
            }
            else if (view.pyr_vertex[i].x<xMin){
                xMin=view.pyr_vertex[i].x;
                indMin=i;
            }
        }

        oneVert.pnt.x=view.pyr_vertex[indMin].x;
        oneVert.pnt.y=view.pyr_vertex[indMin].y;
        gndPol.AddVertex(oneVert);

        oneVert.pnt.x=view.pyr_vertex[indMax].x;
        oneVert.pnt.y=view.pyr_vertex[indMax].y;
        gndPol.AddVertex(oneVert);

        indMax=4,indMin=4;
        xMax=view.pyr_vertex[4].x,xMin=view.pyr_vertex[4].x;

        for(i=5;i<8;i++){
            if (view.pyr_vertex[i].x>xMax){
                xMax=view.pyr_vertex[i].x;
                indMax=i;
            }
            else if (view.pyr_vertex[i].x<xMin){
                xMin=view.pyr_vertex[i].x;
                indMin=i;
            }
        }

        oneVert.pnt.x=view.pyr_vertex[indMin].x;
        oneVert.pnt.y=view.pyr_vertex[indMin].y;
        gndPol.AddVertex(oneVert);

        oneVert.pnt.x=view.pyr_vertex[indMax].x;
        oneVert.pnt.y=view.pyr_vertex[indMax].y;
        gndPol.AddVertex(oneVert);

    }
    else{

        for (int i=0;i<8;i++){
            oneVert.pnt.x=view.pyr_vertex[i].x;
            oneVert.pnt.y=view.pyr_vertex[i].y;
            gndPol.AddVertex(oneVert);
        }

    }

    int numVert=gndPol.GetNVertices();

    for (i=0;i<numVert;i++){
        oneVert=gndPol.GetVertex(i);
        polVert[i].x=oneVert.pnt.x;
        polVert[i].y=oneVert.pnt.y;
    }

    polMaxY=&polVert[0];
    polMinY=&polVert[0];

    for (i=1;i<numVert;i++){
        if (polMaxY->y<polVert[i].y) polMaxY=&polVert[i];
        else if (polMinY->y>polVert[i].y) polMinY=&polVert[i];
    }
    view.boundMaxY=(int)polMaxY->y>>8;
    view.boundMinY=(int)polMinY->y>>8;

    polMaxX=&polVert[0];
    polMinX=&polVert[0];

    for (i=1;i<numVert;i++){
        if (polMaxX->x<polVert[i].x) polMaxX=&polVert[i];
        else if (polMinX->x>polVert[i].x) polMinX=&polVert[i];
    }
    view.boundMaxX=(int)polMaxX->x>>8;
    view.boundMinX=(int)polMinX->x>>8;

    if (view.axis == 0){
        if (view.ha<=M_PI/4 && view.ha>-M_PI/4) view.dir=0;
        else view.dir=2;
    }
    else{
        if (view.ha>M_PI/4 && view.ha<3*M_PI/4) view.dir=1;
        else view.dir=3;
    }

    switch (view.dir){
        case 0:{
            polStart=&polVert[0];
            polEnd=&polVert[numVert-1];

            polPtr=polMaxX;
            polPtrPrev=polPtr+1;
            cnt=0;

            while (1){
                if(polPtr>polEnd) polPtr=polStart;
                if(polPtrPrev>polEnd) polPtrPrev=polStart;
                if(polPtr == polMinX) break;

                cnt+=Bresenham_X(&hcoordr[cnt],*polPtr,*polPtrPrev);

                polPtr++;
                polPtrPrev++;

            }

            polPtr=polMaxX;
            polPtrPrev=polPtr-1;
            cnt=0;

            while (1){
                if(polPtr<polStart) polPtr=polEnd;
                if(polPtrPrev<polStart) polPtrPrev=polEnd;
                if(polPtr == polMinX) break;

                cnt+=Bresenham_X(&hcoordl[cnt],*polPtr,*polPtrPrev);

                polPtr--;
                polPtrPrev--;

            }

            break;
        }
        case 1:{
            polStart=&polVert[0];
            polEnd=&polVert[numVert-1];

            polPtr=polMaxY;
            polPtrPrev=polPtr+1;
            cnt=0;

            while (1){
                if(polPtr>polEnd) polPtr=polStart;
                if(polPtrPrev>polEnd) polPtrPrev=polStart;
                if(polPtr == polMinY) break;

                cnt+=Bresenham_Y(&hcoordr[cnt],*polPtr,*polPtrPrev);

                polPtr++;
                polPtrPrev++;
            }

            polPtr=polMaxY;
            polPtrPrev=polPtr-1;
            cnt=0;

            while (1){
                if(polPtr<polStart) polPtr=polEnd;
                if(polPtrPrev<polStart) polPtrPrev=polEnd;
                if(polPtr == polMinY) break;

                cnt+=Bresenham_Y(&hcoordl[cnt],*polPtr,*polPtrPrev);

                polPtr--;
                polPtrPrev--;
            }

            break;
        }
        case 2:{
            polStart=&polVert[0];
            polEnd=&polVert[numVert-1];

            polPtr=polMinX;
            polPtrPrev=polPtr+1;
            cnt=0;

            while (1){
                if(polPtr>polEnd) polPtr=polStart;
                if(polPtrPrev>polEnd) polPtrPrev=polStart;
                if(polPtr == polMaxX) break;

                cnt+=Bresenham_X(&hcoordr[cnt],*polPtr,*polPtrPrev);

                polPtr++;
                polPtrPrev++;
            }

            polPtr=polMinX;
            polPtrPrev=polPtr-1;
            cnt=0;

            while (1){
                if(polPtr<polStart) polPtr=polEnd;
                if(polPtrPrev<polStart) polPtrPrev=polEnd;
                if(polPtr == polMaxX) break;

                cnt+=Bresenham_X(&hcoordl[cnt],*polPtr,*polPtrPrev);

                polPtr--;
                polPtrPrev--;
            }

            break;
        }
        case 3:{
            polStart=&polVert[0];
            polEnd=&polVert[numVert-1];

            polPtr=polMinY;
            polPtrPrev=polPtr+1;
            cnt=0;

            while (1){
                if(polPtr>polEnd) polPtr=polStart;
                if(polPtrPrev>polEnd) polPtrPrev=polStart;
                if(polPtr == polMaxY) break;

                cnt+=Bresenham_Y(&hcoordr[cnt],*polPtr,*polPtrPrev);

                polPtr++;
                polPtrPrev++;
            }

            polPtr=polMinY;
            polPtrPrev=polPtr-1;
            cnt=0;

            while (1){
                if(polPtr<polStart) polPtr=polEnd;
                if(polPtrPrev<polStart) polPtrPrev=polEnd;
                if(polPtr == polMaxY) break;

                cnt+=Bresenham_Y(&hcoordl[cnt],*polPtr,*polPtrPrev);

                polPtr--;
                polPtrPrev--;
            }

            break;
        }
    }
}

void CoordToBuff(Ship *view){

	int trap_cnt = 0;

	MapCoord* pyr = view->pyr_vertex;
	MapCoord* trap = view->trap_vertex;

	if (view->axis==0){       // ось X

		int start_num, last_num;
		int cur_num, next_num;
		CFVector2 start_vertex, last_vertex;
		CFVector2 cur_vertex, next_vertex;
		int br_num1, br_num2, br_num3;
		CFVector2 branch1, branch2, branch3;
		int left_num, right_num;

		{
			int h_max_num, h_min_num;
			int st_max_num, st_min_num;
			FindMaxMin_X(pyr, &h_max_num, &h_min_num);
			FindMaxMin_X(pyr+4, &st_max_num, &st_min_num);
			st_max_num += 4;
			st_min_num += 4;
			if ( pyr[st_max_num].x > pyr[h_max_num].x )
			{
				last_num = st_max_num;
				last_vertex.x = pyr[st_max_num].x;
				last_vertex.y = pyr[st_max_num].y;
				start_num = h_min_num;
				start_vertex.x = pyr[h_min_num].x;
				start_vertex.y = pyr[h_min_num].y;
				view->boundMaxX = pyr[st_max_num].x>>8;
				view->boundMinX = pyr[h_min_num].x>>8;
				trap[trap_cnt++] = pyr[h_min_num];
			}	else if ( pyr[st_min_num].x < pyr[h_min_num].x )
			{
				last_num = st_min_num;
				last_vertex.x = pyr[st_min_num].x;
				last_vertex.y = pyr[st_min_num].y;
				start_num = h_max_num;
				start_vertex.x = pyr[h_max_num].x;
				start_vertex.y = pyr[h_max_num].y;
				view->boundMaxX = pyr[h_max_num].x>>8;
				view->boundMinX = pyr[st_min_num].x>>8;
				trap[trap_cnt++] = pyr[h_max_num];
			} else if ( (pyr[4].x+pyr[6].x)/2 > (pyr[0].x+pyr[2].x)/2 )
			{
				last_num = h_max_num;
				last_vertex.x = pyr[h_max_num].x;
				last_vertex.y = pyr[h_max_num].y;
				start_num = h_min_num;
				start_vertex.x = pyr[h_min_num].x;
				start_vertex.y = pyr[h_min_num].y;
				view->boundMaxX = pyr[h_max_num].x>>8;
				view->boundMinX = pyr[h_min_num].x>>8;
				trap[trap_cnt++] = pyr[h_min_num];
			} else
			{
				last_num = h_min_num;
				last_vertex.x = pyr[h_min_num].x;
				last_vertex.y = pyr[h_min_num].y;
				start_num = h_max_num;
				start_vertex.x = pyr[h_max_num].x;
				start_vertex.y = pyr[h_max_num].y;
				view->boundMaxX = pyr[h_max_num].x>>8;
				view->boundMinX = pyr[h_min_num].x>>8;
				trap[trap_cnt++] = pyr[h_max_num];
			}
		}

		br_num1 = (start_num + 1)&3;
		br_num2 = (start_num + 3)&3;
		br_num3 = start_num + 4;

		branch1.x = pyr[br_num1].x - start_vertex.x;
		branch1.y = pyr[br_num1].y - start_vertex.y;
		branch2.x = pyr[br_num2].x - start_vertex.x;
		branch2.y = pyr[br_num2].y - start_vertex.y;
		branch3.x = pyr[br_num3].x - start_vertex.x;
		branch3.y = pyr[br_num3].y - start_vertex.y;
		CHECK_BRANCH(branch1)
		CHECK_BRANCH(branch2)
		CHECK_BRANCH(branch3)
		{
			double sign12 = branch1%branch2;
			double sign13 = branch1%branch3;
			double sign23 = branch2%branch3;

			if ( sign12 > 0 )
				if ( sign13 > 0 )
				{
					right_num = br_num1;
					if ( sign23 < 0 )
						left_num = br_num2;
					else                    // Если вектоpы 2 и 3 совпадают, то
						left_num = br_num3;   // пеpеходим на дpугой уpовень
				}
				else
				{
					left_num = br_num2;
					if (sign13 == 0 && sign23 == 0)
						right_num = br_num1;
					else
						right_num = br_num3;
				}
			else if ( sign12 < 0 )
			{
				if ( sign23 > 0 )
				{
					right_num = br_num2;
					if ( sign13 < 0 )
						left_num = br_num1;
					else
						left_num = br_num3;
				}
				else
				{
					left_num = br_num1;
					if ( sign13 == 0 && sign23 == 0 )
						right_num = br_num3;
					else
						right_num = br_num3;
				}
			}
			else    // sign12 == 0
			{
				if ( sign13 > 0 )
				{
					left_num = br_num3;
					right_num = br_num1;
				}
				else if ( sign13 < 0 )
				{
					right_num = br_num3;
					left_num = br_num1;
				}
				else    // sign13 == 0 && sign12 == 0
				{
					if ( sign23 < 0 )
					{
						left_num = br_num2;
						right_num = br_num3;
					}
					else
					{
						left_num = br_num3;
						right_num = br_num2;
					}
				}
			}
		}

		cur_num = start_num;
		cur_vertex = start_vertex;
		next_num = right_num;
		next_vertex.x = pyr[next_num].x;
		next_vertex.y = pyr[next_num].y;

		view->boundMinY = view->boundMaxY = pyr[start_num].y;

		// Left round
		cnt = 0;
		do
		{
			trap[trap_cnt++] = pyr[next_num];

            //RTCHECK( trap_cnt <= 8, "Land trap8" );

			if ( pyr[next_num].y > view->boundMaxY )
				view->boundMaxY = pyr[next_num].y;
			else if ( pyr[next_num].y < view->boundMinY )
				view->boundMinY = pyr[next_num].y;

			cnt+=Bresenham_X(&hcoordl[cnt], cur_vertex, next_vertex);
			int add_on = next_num < 4 ? 0 : 4;
			br_num1 = ((next_num + 1 - add_on)&3) + add_on;
			br_num2 = ((next_num + 3 - add_on)&3) + add_on;
			br_num3 = (next_num + 4)&7;

			{
				if ( br_num1 == cur_num )
				{
					br_num1 = br_num2;
					br_num2 = br_num3;
				}
				else if ( br_num2 == cur_num )
						br_num2 = br_num3;

				cur_num = next_num;
				branch1.x = pyr[br_num1].x - next_vertex.x;
				branch1.y = pyr[br_num1].y - next_vertex.y;
				branch2.x = pyr[br_num2].x - next_vertex.x;
				branch2.y = pyr[br_num2].y - next_vertex.y;
				CHECK_BRANCH(branch1)
				CHECK_BRANCH(branch2)

				if ((br_num1 == last_num)&&(Abs2(branch1) == 0 ))
				{
						next_num = br_num1;
						next_vertex = last_vertex;
				}
				else if ((br_num2 == last_num)&&(Abs2(branch2) == 0 ))
				{
						next_num = br_num2;
						next_vertex = last_vertex;
				}
				else
				{
					if ( Abs2(branch1) == 0 )
						next_num = br_num2;
					else if ( Abs2(branch2) == 0 )
						next_num = br_num1;
					else
					{
						CFVector2 cur_vector = next_vertex - cur_vertex;

						double x1 = (cur_vector*branch1)/(Abs(cur_vector)*Abs(branch1));
						double x2 = (cur_vector*branch2)/(Abs(cur_vector)*Abs(branch2));
						double a1 = acos( MAKE_LEG(x1) );
						double a2 = acos( MAKE_LEG(x2) );
						if ( a1 == a2 )
							next_num = br_num1;
						else if ( a1 < a2 )
							next_num = br_num1;
						else
							next_num = br_num2;
					}
				}
			}
			cur_vertex = next_vertex;
			next_vertex.x = pyr[next_num].x;
			next_vertex.y = pyr[next_num].y;

		} while ( cur_vertex != last_vertex);


		cur_num = start_num;
		cur_vertex = start_vertex;
		next_num = left_num;
		next_vertex.x = pyr[next_num].x;
		next_vertex.y = pyr[next_num].y;
		// Right round
		cnt = 0;
		MapCoord rightVertex[6];
		int last_vert_num = trap_cnt;
		do
		{
			rightVertex[trap_cnt - last_vert_num] = pyr[next_num];
//			trap[koeff - trap_cnt] = pyr[next_num];
			trap_cnt++;

            //RTCHECK( trap_cnt <= 8, "Land trap8" );

			if ( pyr[next_num].y > view->boundMaxY )
				view->boundMaxY = pyr[next_num].y;
			else if ( pyr[next_num].y < view->boundMinY )
				view->boundMinY = pyr[next_num].y;

			cnt+=Bresenham_X(&hcoordr[cnt], cur_vertex, next_vertex);
			int add_on = next_num < 4 ? 0 : 4;
			br_num1 = ((next_num + 1 - add_on)&3) + add_on;
			br_num2 = ((next_num + 3 - add_on)&3) + add_on;
			br_num3 = (next_num + 4)&7;

			{
				if ( br_num1 == cur_num )
				{
					br_num1 = br_num2;
					br_num2 = br_num3;
				}
				else if ( br_num2 == cur_num )
						br_num2 = br_num3;

				cur_num = next_num;
				branch1.x = pyr[br_num1].x - next_vertex.x;
				branch1.y = pyr[br_num1].y - next_vertex.y;
				branch2.x = pyr[br_num2].x - next_vertex.x;
				branch2.y = pyr[br_num2].y - next_vertex.y;
				CHECK_BRANCH(branch1)
				CHECK_BRANCH(branch2)

				if ((br_num1 == last_num)&&(Abs2(branch1) == 0 ))
				{
						next_num = br_num1;
						next_vertex = last_vertex;
				}
				else if ((br_num2 == last_num)&&(Abs2(branch2) == 0 ))
				{
						next_num = br_num2;
						next_vertex = last_vertex;
				}
				else
				{
					if ( Abs2(branch1) == 0 )
						next_num = br_num2;
					else if ( Abs2(branch2) == 0 )
						next_num = br_num1;
					else
					{
						CFVector2 cur_vector = next_vertex - cur_vertex;

						double x1 = (cur_vector*branch1)/(Abs(cur_vector)*Abs(branch1));
						double x2 = (cur_vector*branch2)/(Abs(cur_vector)*Abs(branch2));
						double a1 = acos( MAKE_LEG(x1) );
						double a2 = acos( MAKE_LEG(x2) );
						if ( a1 == a2 )
							next_num = br_num1;
						else if ( a1 < a2 )
							next_num = br_num1;
						else
							next_num = br_num2;
					}
				}
			}
			cur_vertex = next_vertex;
			next_vertex.x = pyr[next_num].x;
			next_vertex.y = pyr[next_num].y;

		} while ( cur_vertex != last_vertex);

		view->boundMinY >>= 8;
		view->boundMaxY >>= 8;
		view->trap_vertex_num = trap_cnt-1;
		int num_right_vert = trap_cnt - last_vert_num - 1;
		for ( int j = 0; j < num_right_vert; j++ )
			trap[trap_cnt-2-j] = rightVertex[j];
	}
	else   // ось Y
	{
		int start_num, last_num;
		int cur_num, next_num;
		CFVector2 start_vertex, last_vertex;
		CFVector2 cur_vertex, next_vertex;
		int br_num1, br_num2, br_num3;
		CFVector2 branch1, branch2, branch3;
		int left_num, right_num;

		{
			int h_max_num, h_min_num;
			int st_max_num, st_min_num;
			FindMaxMin_Y(pyr, &h_max_num, &h_min_num);
			FindMaxMin_Y(pyr+4, &st_max_num, &st_min_num);
			st_max_num += 4;
			st_min_num += 4;
			if ( pyr[st_max_num].y > pyr[h_max_num].y )
			{
				last_num = st_max_num;
				last_vertex.x = pyr[st_max_num].x;
				last_vertex.y = pyr[st_max_num].y;
				start_num = h_min_num;
				start_vertex.x = pyr[h_min_num].x;
				start_vertex.y = pyr[h_min_num].y;
				view->boundMaxY = pyr[st_max_num].y>>8;
				view->boundMinY = pyr[h_min_num].y>>8;
				trap[trap_cnt++] = pyr[h_min_num];
			}	else if ( pyr[st_min_num].y < pyr[h_min_num].y )
			{
				last_num = st_min_num;
				last_vertex.x = pyr[st_min_num].x;
				last_vertex.y = pyr[st_min_num].y;
				start_num = h_max_num;
				start_vertex.x = pyr[h_max_num].x;
				start_vertex.y = pyr[h_max_num].y;
				view->boundMaxY = pyr[h_max_num].y>>8;
				view->boundMinY = pyr[st_min_num].y>>8;
				trap[trap_cnt++] = pyr[h_max_num];
			} else if ( (pyr[4].y+pyr[6].y)/2 > (pyr[0].y+pyr[2].y)/2 )
			{
				last_num = h_max_num;
				last_vertex.x = pyr[h_max_num].x;
				last_vertex.y = pyr[h_max_num].y;
				start_num = h_min_num;
				start_vertex.x = pyr[h_min_num].x;
				start_vertex.y = pyr[h_min_num].y;
				view->boundMaxY = pyr[h_max_num].y>>8;
				view->boundMinY = pyr[h_min_num].y>>8;
				trap[trap_cnt++] = pyr[h_min_num];
			} else
			{
				last_num = h_min_num;
				last_vertex.x = pyr[h_min_num].x;
				last_vertex.y = pyr[h_min_num].y;
				start_num = h_max_num;
				start_vertex.x = pyr[h_max_num].x;
				start_vertex.y = pyr[h_max_num].y;
				view->boundMaxY = pyr[h_max_num].y>>8;
				view->boundMinY = pyr[h_min_num].y>>8;
				trap[trap_cnt++] = pyr[h_max_num];
			}
		}

		br_num1 = (start_num + 1)&3;
		br_num2 = (start_num + 3)&3;
		br_num3 = start_num + 4;

		branch1.x = pyr[br_num1].x - start_vertex.x;
		branch1.y = pyr[br_num1].y - start_vertex.y;
		branch2.x = pyr[br_num2].x - start_vertex.x;
		branch2.y = pyr[br_num2].y - start_vertex.y;
		branch3.x = pyr[br_num3].x - start_vertex.x;
		branch3.y = pyr[br_num3].y - start_vertex.y;
		CHECK_BRANCH(branch1)
		CHECK_BRANCH(branch2)
		CHECK_BRANCH(branch3)

		{
			double sign12 = branch1%branch2;
			double sign13 = branch1%branch3;
			double sign23 = branch2%branch3;

			if ( sign12 > 0 )
				if ( sign13 > 0 )
				{
					right_num = br_num1;
					if ( sign23 < 0 )
						left_num = br_num2;
					else                    // Если вектоpы 2 и 3 совпадают, то
						left_num = br_num3;   // пеpеходим на дpугой уpовень
				}
				else
				{
					left_num = br_num2;
					if (sign13 == 0 && sign23 == 0)
						right_num = br_num1;
					else
						right_num = br_num3;
				}
			else if ( sign12 < 0 )
			{
				if ( sign23 > 0 )
				{
					right_num = br_num2;
					if ( sign13 < 0 )
						left_num = br_num1;
					else
						left_num = br_num3;
				}
				else
				{
					left_num = br_num1;
					if ( sign13 == 0 && sign23 == 0 )
						right_num = br_num3;
					else
						right_num = br_num3;
				}
			}
			else    // sign12 == 0
			{
				if ( sign13 > 0 )
				{
					left_num = br_num3;
					right_num = br_num1;
				}
				else if ( sign13 < 0 )
				{
					right_num = br_num3;
					left_num = br_num1;
				}
				else    // sign13 == 0 && sign12 == 0
				{
					if ( sign23 < 0 )
					{
						left_num = br_num2;
						right_num = br_num3;
					}
					else
					{
						left_num = br_num3;
						right_num = br_num2;
					}
				}
			}
		}

		cur_num = start_num;
		cur_vertex = start_vertex;
		next_num = right_num;
		next_vertex.x = pyr[next_num].x;
		next_vertex.y = pyr[next_num].y;

		view->boundMinX = view->boundMaxX = pyr[start_num].x;

		// Left round
		cnt = 0;
		do
		{
			trap[trap_cnt++] = pyr[next_num];

            //RTCHECK( trap_cnt <= 8, "Land trap8" );

			if ( pyr[next_num].x> view->boundMaxX )
				view->boundMaxX = pyr[next_num].x;
			else if ( pyr[next_num].x < view->boundMinX )
				view->boundMinX = pyr[next_num].x;

			cnt+=Bresenham_Y(&hcoordl[cnt], cur_vertex, next_vertex);
			int add_on = next_num < 4 ? 0 : 4;
			br_num1 = ((next_num + 1 - add_on)&3) + add_on;
			br_num2 = ((next_num + 3 - add_on)&3) + add_on;
			br_num3 = (next_num + 4)&7;

			{
				if ( br_num1 == cur_num )
				{
					br_num1 = br_num2;
					br_num2 = br_num3;
				}
				else if ( br_num2 == cur_num )
						br_num2 = br_num3;

				cur_num = next_num;
				branch1.x = pyr[br_num1].x - next_vertex.x;
				branch1.y = pyr[br_num1].y - next_vertex.y;
				branch2.x = pyr[br_num2].x - next_vertex.x;
				branch2.y = pyr[br_num2].y - next_vertex.y;
				CHECK_BRANCH(branch1)
				CHECK_BRANCH(branch2)

				if ((br_num1 == last_num)&&(Abs2(branch1) == 0 ))
				{
						next_num = br_num1;
						next_vertex = last_vertex;
				}
				else if ((br_num2 == last_num)&&(Abs2(branch2) == 0 ))
				{
						next_num = br_num2;
						next_vertex = last_vertex;
				}
				else
				{
					if ( Abs2(branch1) == 0 )
						next_num = br_num2;
					else if ( Abs2(branch2) == 0 )
						next_num = br_num1;
					else
					{
						CFVector2 cur_vector = next_vertex - cur_vertex;

						double x1 = (cur_vector*branch1)/(Abs(cur_vector)*Abs(branch1));
						double x2 = (cur_vector*branch2)/(Abs(cur_vector)*Abs(branch2));
						double a1 = acos( MAKE_LEG(x1) );
						double a2 = acos( MAKE_LEG(x2) );
						if ( a1 == a2 )
							next_num = br_num1;
						else if ( a1 < a2 )
							next_num = br_num1;
						else
							next_num = br_num2;
					}
				}
			}
			cur_vertex = next_vertex;
			next_vertex.x = pyr[next_num].x;
			next_vertex.y = pyr[next_num].y;

		} while ( cur_vertex != last_vertex);


		cur_num = start_num;
		cur_vertex = start_vertex;
		next_num = left_num;
		next_vertex.x = pyr[next_num].x;
		next_vertex.y = pyr[next_num].y;
		// Right round
		cnt = 0;
		MapCoord rightVertex[6];
		int last_vert_num = trap_cnt;
		do
		{
			rightVertex[trap_cnt - last_vert_num] = pyr[next_num];
//			trap[koeff - trap_cnt] = pyr[next_num];
			trap_cnt++;

            //RTCHECK( trap_cnt <= 8, "Land trap8" );

			if ( pyr[next_num].x > view->boundMaxX )
				view->boundMaxX = pyr[next_num].x;
			else if ( pyr[next_num].x < view->boundMinX )
				view->boundMinX = pyr[next_num].x;

			cnt+=Bresenham_Y(&hcoordr[cnt], cur_vertex, next_vertex);
			int add_on = next_num < 4 ? 0 : 4;
			br_num1 = ((next_num + 1 - add_on)&3) + add_on;
			br_num2 = ((next_num + 3 - add_on)&3) + add_on;
			br_num3 = (next_num + 4)&7;

			{
				if ( br_num1 == cur_num )
				{
					br_num1 = br_num2;
					br_num2 = br_num3;
				}
				else if ( br_num2 == cur_num )
						br_num2 = br_num3;

				cur_num = next_num;
				branch1.x = pyr[br_num1].x - next_vertex.x;
				branch1.y = pyr[br_num1].y - next_vertex.y;
				branch2.x = pyr[br_num2].x - next_vertex.x;
				branch2.y = pyr[br_num2].y - next_vertex.y;
				CHECK_BRANCH(branch1)
				CHECK_BRANCH(branch2)

				if ((br_num1 == last_num)&&(Abs2(branch1) == 0 ))
				{
						next_num = br_num1;
						next_vertex = last_vertex;
				}
				else if ((br_num2 == last_num)&&(Abs2(branch2) == 0 ))
				{
						next_num = br_num2;
						next_vertex = last_vertex;
				}
				else
				{
					if ( Abs2(branch1) == 0 )
						next_num = br_num2;
					else if ( Abs2(branch2) == 0 )
						next_num = br_num1;
					else
					{
						CFVector2 cur_vector = next_vertex - cur_vertex;

						double x1 = (cur_vector*branch1)/(Abs(cur_vector)*Abs(branch1));
						double x2 = (cur_vector*branch2)/(Abs(cur_vector)*Abs(branch2));
						double a1 = acos( MAKE_LEG(x1) );
						double a2 = acos( MAKE_LEG(x2) );
						if ( a1 == a2 )
							next_num = br_num1;
						else if ( a1 < a2 )
							next_num = br_num1;
						else
							next_num = br_num2;
					}
				}
			}
			cur_vertex = next_vertex;
			next_vertex.x = pyr[next_num].x;
			next_vertex.y = pyr[next_num].y;

		} while ( cur_vertex != last_vertex);

		view->boundMinX >>= 8;
		view->boundMaxX >>= 8;
		view->trap_vertex_num = trap_cnt-1;
		int num_right_vert = trap_cnt - last_vert_num - 1;
		for ( int j = 0; j < num_right_vert; j++ )
			trap[trap_cnt-2-j] = rightVertex[j];
	}

}

/*

// ********************************************** CoordToBuff ******************
// Занесение кооpдинат пеpиметpовых точек в глобальные буфеpы
// hcoordl и hcoordr (кооpдинаты в каpте). Две пpотивоположные
// пеpиметpовые точки опpеделяют секущую плоскость. Секущие плоскости
// могут быть паpаллельны либо оси X либо оси Y, в зависимости от
// того, к какой из осей вектоp напpавления движения pасположен ближе.
void CoordToBuff(Ship *view){
	long	sig_x,sig_y;
	long	x_map,y_map;
	Ship	mapcrd;
	long	dx,dy;
	long	s;
	int		i;


	mapcrd=*view;
	//Пpеобpазование миpовых кооpдинат в кооpдинаты каpты
	for (i=0;i<4;i++){
		mapcrd.xc[i]>>=PRESIGN;
		//mapcrd.xc[i]>>=1;
		//mapcrd.xc[i]<<=1;

		mapcrd.yc[i]>>=PRESIGN;
		//mapcrd.yc[i]>>=1;
		//mapcrd.yc[i]<<=1;
	}

#if defined(XML) || defined(XMR) || defined(YML) || defined(YMR)
	printf("ERROR !!! macro redefined \n");
	exit(1);
#endif

#	define 	INITS 0

// Макpосы заполнения буфеpов, когда основной осью является ось X
#	define  XML  for (i=0;i<dx;i++){ \
				s+=dy;\
				while(s>=dx){ \
					s-=dx;\
					y_map+=sig_y;\
				} \
				hcoordl[cnt].x=x_map; \
				hcoordl[cnt].y=y_map; \
				cnt++;\
				x_map+=sig_x;\
			  } \

#	define  XMR  for (i=0;i<dx;i++){ \
				s+=dy;\
				while(s>=dx){ \
					s-=dx;\
					y_map+=sig_y;\
				} \
				hcoordr[cnt].x=x_map; \
				hcoordr[cnt].y=y_map; \
				cnt++;\
				x_map+=sig_x;\
			  } \

// Макpосы заполнения буфеpов, когда основной осью является ось Y
#	define  YML for (i=0;i<dy;i++){ \
				s+=dx; \
				while(s>=dy){ \
					s-=dy; \
					x_map+=sig_x; \
				} \
				hcoordl[cnt].x=x_map; \
				hcoordl[cnt].y=y_map; \
				cnt++; \
				y_map+=sig_y; \
			  } \

#	define  YMR for (i=0;i<dy;i++){ \
				s+=dx; \
				while (s>=dy){ \
					s-=dy; \
					x_map+=sig_x; \
				} \
				hcoordr[cnt].x=x_map; \
				hcoordr[cnt].y=y_map; \
				cnt++; \
				y_map+=sig_y; \
			  } \

	if (view->axis==0){       // ось X
		if (view->farp==0){     // дальняя точка - 0
			  cnt=0;
			  //-------------------------------------- от точки 0 в 3
			  sig_x=SIG(mapcrd.xc[3]-mapcrd.xc[0]);
			  sig_y=SIG(mapcrd.yc[3]-mapcrd.yc[0]);
			  if((dx=ABS(mapcrd.xc[0]-mapcrd.xc[3]))==0) dx++;
			  dy=ABS(mapcrd.yc[0]-mapcrd.yc[3]);
			  x_map=mapcrd.xc[0];
			  y_map=mapcrd.yc[0];
			  s=INITS;
			  XML
			  //-------------------------------------- от точки 3 в 2
			  sig_x=SIG(mapcrd.xc[2]-mapcrd.xc[3]);
			  sig_y=SIG(mapcrd.yc[2]-mapcrd.yc[3]);
			  if((dx=ABS(mapcrd.xc[3]-mapcrd.xc[2]))==0) dx++;
			  dy=ABS(mapcrd.yc[3]-mapcrd.yc[2]);
			  x_map=mapcrd.xc[3];
			  y_map=mapcrd.yc[3];
			  s=INITS;
			  XML
			  //getch();
			  cnt=0;
			  //-------------------------------------- от точки 0 в 1
			  sig_x=SIG(mapcrd.xc[1]-mapcrd.xc[0]);
			  sig_y=SIG(mapcrd.yc[1]-mapcrd.yc[0]);
			  if((dx=ABS(mapcrd.xc[0]-mapcrd.xc[1]))==0) dx++;
			  dy=ABS(mapcrd.yc[0]-mapcrd.yc[1]);
			  x_map=mapcrd.xc[0];
			  y_map=mapcrd.yc[0];
			  s=INITS;
			  XMR
			  //-------------------------------------- от точки 1 в 2
			  sig_x=SIG(mapcrd.xc[2]-mapcrd.xc[1]);
			  sig_y=SIG(mapcrd.yc[2]-mapcrd.yc[1]);
			  if((dx=ABS(mapcrd.xc[1]-mapcrd.xc[2]))==0) dx++;
			  dy=ABS(mapcrd.yc[1]-mapcrd.yc[2]);
			  x_map=mapcrd.xc[1];
			  y_map=mapcrd.yc[1];
			  s=INITS;
			  XMR
		}
		else{                  // дальняя точка - 1
			  //-------------------------------------- от точки 1 в 0
			  cnt=0;
			  sig_x=SIG(mapcrd.xc[0]-mapcrd.xc[1]);
			  sig_y=SIG(mapcrd.yc[0]-mapcrd.yc[1]);
			  if((dx=ABS(mapcrd.xc[0]-mapcrd.xc[1]))==0) dx++;
			  dy=ABS(mapcrd.yc[1]-mapcrd.yc[0]);
			  x_map=mapcrd.xc[1];
			  y_map=mapcrd.yc[1];
			  s=INITS;
			  XML
			  //-------------------------------------- от точки 0 в 3
			  sig_x=SIG(mapcrd.xc[3]-mapcrd.xc[0]);
			  sig_y=SIG(mapcrd.yc[3]-mapcrd.yc[0]);
			  if((dx=ABS(mapcrd.xc[0]-mapcrd.xc[3]))==0) dx++;
			  dy=ABS(mapcrd.yc[0]-mapcrd.yc[3]);
			  x_map=mapcrd.xc[0];
			  y_map=mapcrd.yc[0];
			  s=INITS;
			  XML
			  cnt=0;
			  //-------------------------------------- от точки 1 в 2
			  sig_x=SIG(mapcrd.xc[2]-mapcrd.xc[1]);
			  sig_y=SIG(mapcrd.yc[2]-mapcrd.yc[1]);
			  if((dx=ABS(mapcrd.xc[1]-mapcrd.xc[2]))==0) dx++;
			  dy=ABS(mapcrd.yc[1]-mapcrd.yc[2]);
			  x_map=mapcrd.xc[1];
			  y_map=mapcrd.yc[1];
			  s=INITS;
			  XMR
			  //-------------------------------------- от точки 2 в 3
			  sig_x=SIG(mapcrd.xc[3]-mapcrd.xc[2]);
			  sig_y=SIG(mapcrd.yc[3]-mapcrd.yc[2]);
			  if((dx=ABS(mapcrd.xc[2]-mapcrd.xc[3]))==0) dx++;
			  dy=ABS(mapcrd.yc[2]-mapcrd.yc[3]);
			  x_map=mapcrd.xc[2];
			  y_map=mapcrd.yc[2];
			  s=INITS;
			  XMR
		}
	}
	else{                         // ось Y
		if (view->farp==0){         // дальняя точка - 0
			  //-------------------------------------- от точки 0 в 3
			  cnt=0;
			  sig_x=SIG(mapcrd.xc[3]-mapcrd.xc[0]);
			  sig_y=SIG(mapcrd.yc[3]-mapcrd.yc[0]);
			  dx=ABS(mapcrd.xc[0]-mapcrd.xc[3]);
			  if((dy=ABS(mapcrd.yc[0]-mapcrd.yc[3]))==0) dy++;
			  x_map=mapcrd.xc[0];
			  y_map=mapcrd.yc[0];
			  s=INITS;
			  YML
			  //-------------------------------------- от точки 3 в 2
			  sig_x=SIG(mapcrd.xc[2]-mapcrd.xc[3]);
			  sig_y=SIG(mapcrd.yc[2]-mapcrd.yc[3]);
			  if((dy=ABS(mapcrd.yc[2]-mapcrd.yc[3]))==0) dy++;
			  dx=ABS(mapcrd.xc[2]-mapcrd.xc[3]);
			  x_map=mapcrd.xc[3];
			  y_map=mapcrd.yc[3];
			  s=INITS;
			  YML
			  cnt=0;
			  //-------------------------------------- от точки 0 в 1
			  sig_x=SIG(mapcrd.xc[1]-mapcrd.xc[0]);
			  sig_y=SIG(mapcrd.yc[1]-mapcrd.yc[0]);
			  if((dy=ABS(mapcrd.yc[0]-mapcrd.yc[1]))==0) dy++;
			  dx=ABS(mapcrd.xc[0]-mapcrd.xc[1]);
			  x_map=mapcrd.xc[0];
			  y_map=mapcrd.yc[0];
			  s=INITS;
			  YMR
			  //-------------------------------------- от точки 1 в 2
			  sig_x=SIG(mapcrd.xc[2]-mapcrd.xc[1]);
			  sig_y=SIG(mapcrd.yc[2]-mapcrd.yc[1]);
			  if((dy=ABS(mapcrd.yc[1]-mapcrd.yc[2]))==0) dy++;
			  dx=ABS(mapcrd.xc[2]-mapcrd.xc[1]);
			  x_map=mapcrd.xc[1];
			  y_map=mapcrd.yc[1];
			  s=INITS;
			  YMR
		}
		else{                     // дальняя точка - 1
			  //-------------------------------------- от точки 1 в 0
			  cnt=0;
			  sig_x=SIG(mapcrd.xc[0]-mapcrd.xc[1]);
			  sig_y=SIG(mapcrd.yc[0]-mapcrd.yc[1]);
			  if((dy=ABS(mapcrd.yc[0]-mapcrd.yc[1]))==0) dy++;
			  dx=ABS(mapcrd.xc[1]-mapcrd.xc[0]);
			  x_map=mapcrd.xc[1];
			  y_map=mapcrd.yc[1];
			  s=INITS;
			  YML
			  //-------------------------------------- от точки 0 в 3
			  sig_x=SIG(mapcrd.xc[3]-mapcrd.xc[0]);
			  sig_y=SIG(mapcrd.yc[3]-mapcrd.yc[0]);
			  if((dy=ABS(mapcrd.yc[0]-mapcrd.yc[3]))==0) dy++;
			  dx=ABS(mapcrd.xc[0]-mapcrd.xc[3]);
			  x_map=mapcrd.xc[0];
			  y_map=mapcrd.yc[0];
			  s=INITS;
			  YML
			  cnt=0;
			  //-------------------------------------- от точки 1 в 2
			  sig_x=SIG(mapcrd.xc[2]-mapcrd.xc[1]);
			  sig_y=SIG(mapcrd.yc[2]-mapcrd.yc[1]);
			  if((dy=ABS(mapcrd.yc[1]-mapcrd.yc[2]))==0) dy++;
			  dx=ABS(mapcrd.xc[2]-mapcrd.xc[1]);
			  x_map=mapcrd.xc[1];
			  y_map=mapcrd.yc[1];
			  s=INITS;
			  YMR
			  //-------------------------------------- от точки 2 в 3
			  sig_x=SIG(mapcrd.xc[3]-mapcrd.xc[2]);
			  sig_y=SIG(mapcrd.yc[3]-mapcrd.yc[2]);
			  if((dy=ABS(mapcrd.yc[2]-mapcrd.yc[3]))==0) dy++;
			  dx=ABS(mapcrd.xc[2]-mapcrd.xc[3]);
			  x_map=mapcrd.xc[2];
			  y_map=mapcrd.yc[2];
			  s=INITS;
			  YMR
		}
	}
#undef XML
#undef XMR
#undef YML
#undef YMR
#undef INITS

}
*/
