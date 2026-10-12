#include <math.h>
#include "_land.h"
#define  __SHIP_C__
#include "ship.h"


//************************************** PutScrLine ***********************************************

int PutScrLine(CoordBuf *linePtr,CoordBuf *st,CoordBuf *en){
    int d_y = en->y - st->y,i;

	//if(d_y<1)	return;

	if (d_y == 0){
		linePtr->x=st->x;
		return 0;
	}

	long stX = (st->x<<16)+(1<<15)-1;

    long step = ((en->x<<16) - (st->x<<16))/d_y;

    for (i=0;i<d_y;i++){
        linePtr[i].x=stX>>16;
		stX+=step;
	}
	return d_y;
}

//*********************************** LineEdgeIZ ************************************************
void LineEdgeIZ(CoordBufD *out,CoordBuf *st,CoordBuf *en,int delta){

    if (delta==0){
        out->z=st->iz;
        return;
    }

    CoordBufD   *pEdgeBuf=out;

    double      enNZ=en->iz;
    double      stNZ=st->iz;

    double      stepZ=(enNZ-stNZ)/delta;
    double      stZ=stNZ;

    for (int i=0;i<=delta;i++){

        pEdgeBuf->z=stZ;
        stZ+=stepZ;

        pEdgeBuf++;
    }

}

#define ADJUSTANGLE(cur_angle, norm_angle) \
	if ( ABS(cur_angle - norm_angle) < (M_PI/180) ) \
		cur_angle = norm_angle;

//******************************************* CalcConnerPoint *****************
// Функция pасчета угловых точек тpапеции, опpеделения главной оси
// и дальней точки
//
void CalcConnerPoint(Ship *const view){

	CFMatrix3x4 		rev_m;

	long x=view->x;    //Миpовые кооpдинаты коpабля
	long y=view->y;    //
	long z=view->z;    //

	ADJUSTANGLE(view->ha,-M_PI)
	ADJUSTANGLE(view->ha,M_PI)
	ADJUSTANGLE(view->ha,0)
	ADJUSTANGLE(view->va,-M_PI)
	ADJUSTANGLE(view->va,M_PI)
	ADJUSTANGLE(view->va,0)
	ADJUSTANGLE(view->ia,-M_PI)
	ADJUSTANGLE(view->ia,M_PI)
	ADJUSTANGLE(view->ia,0)

	if (view->ha>=M_PI) view->ha-=2*M_PI;
	if (view->ha<-M_PI) view->ha+=2*M_PI;

	if (view->va>=M_PI) view->va-=2*M_PI;
	if (view->va<-M_PI) view->va+=2*M_PI;

	if (view->ia>=M_PI) view->ia-=2*M_PI;
	if (view->ia<-M_PI) view->ia+=2*M_PI;


	if ( (view->va > M_PI/2) || (view->va < -M_PI/2) )
		view->Direction = -1;
  else
		view->Direction = 1;

    rev_m.LoadIdentity()
                .RotateOxL(-view->ia)
                .RotateOyL(-view->va)
                .RotateOzL(view->ha)
                .TranslateL(CFVector3(x,y,z));

{
		CFVector3       cur_vertex;
		double y_coord,z_coord;

		y_coord = view->len*tan(view->w_h_ang);
		z_coord = view->len*sin(view->w_h_ang);

		rev_m.Mult(CFVector3(view->len, y_coord, z_coord), cur_vertex);
		view->pyr_vertex[0].x = cur_vertex.x;
		view->pyr_vertex[0].y = cur_vertex.y;

		rev_m.Mult(CFVector3(view->len, -y_coord, z_coord), cur_vertex);
		view->pyr_vertex[1].x = cur_vertex.x;
		view->pyr_vertex[1].y = cur_vertex.y;

		rev_m.Mult(CFVector3(view->len, -y_coord, -z_coord), cur_vertex);
		view->pyr_vertex[2].x = cur_vertex.x;
		view->pyr_vertex[2].y = cur_vertex.y;

		rev_m.Mult(CFVector3(view->len, y_coord, -z_coord), cur_vertex);
		view->pyr_vertex[3].x = cur_vertex.x;
		view->pyr_vertex[3].y = cur_vertex.y;


		y_coord = view->stlen*tan(view->w_st_ang);
		z_coord = view->stlen*sin(view->w_st_ang);

		rev_m.Mult(CFVector3(-view->stlen, y_coord, z_coord), cur_vertex);
		view->pyr_vertex[4].x = cur_vertex.x;
		view->pyr_vertex[4].y = cur_vertex.y;

		rev_m.Mult(CFVector3(-view->stlen, -y_coord, z_coord), cur_vertex);
		view->pyr_vertex[5].x = cur_vertex.x;
		view->pyr_vertex[5].y = cur_vertex.y;

		rev_m.Mult(CFVector3(-view->stlen, -y_coord, -z_coord), cur_vertex);
		view->pyr_vertex[6].x = cur_vertex.x;
		view->pyr_vertex[6].y = cur_vertex.y;

		rev_m.Mult(CFVector3(-view->stlen, y_coord, -z_coord), cur_vertex);
		view->pyr_vertex[7].x = cur_vertex.x;
		view->pyr_vertex[7].y = cur_vertex.y;
}


	if (((view->ha<=M_PI/4)&&(view->ha>-M_PI/4)) ||
				(view->ha>=M_PI*3/4) || (view->ha<-M_PI*3/4))
        view->axis=0;
	else
        view->axis=1;

}
