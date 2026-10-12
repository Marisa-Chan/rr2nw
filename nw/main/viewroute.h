#include "i/route.i"
#include "message/dcrossmsg.h"

int      ctroutID = -1;
unsigned routeCol = -1;
unsigned pointCol = -1;
//void drawLine(CFVector3 start,CFVector3 end,int rad,int r, int g, int b);
//void drawPoint(CFVector3 pos, int rad, int r, int g, int b);

CFVector3 getLh(CFVector3 p)
{

    double height;
    CFVector3 normal;
    CViewScene::Current()->GetTerrain()->GetPlane(p,normal,height);
    height+=1.5;
    if( p.y < height  )
        p.y = height;
    return p;
}

bool drawRoute(KR_ObjectID oID,void *)
{
   IRouteObject *r = (IRouteObject*)(g_arena.getContext()->queryInterface(oID,IRouteObjectIID));
   if(  r != 0  )
   {
        double dist = CViewFigure::HazeMax();
        CFVector3 pl = CViewObject::m_viewPointInvMx.Offset();
        dist *= dist;
        int i;

        CFVector3 prev = getLh(r->GetNode(0));

        for(i=1; i<r->GetNodeCnt(); ++i)
        {
              CFVector3 cur = getLh(r->GetNode(i));

              if(  Abs2(pl-prev)<dist || Abs2(pl-cur)<dist )
              {	
                   drawLine (prev,cur,1,routeCol,0,255);
                   drawPoint(prev, 4, pointCol, 0, 0);
                   prev = cur;
              }
        }
   }
   return 1;
}

void viewRoute()
{
  if(  ctroutID==-1  )
  {
       ctroutID = g_arena.searchSeanceClassTable( "Route" );
       pointCol = GRCreateColor(255,0,0);
       routeCol = GRCreateColor(0,0,255);
  }
 g_arena.userFind(ctroutID,drawRoute,0);
 //drawLine (CFVector3(2514, 80, -2307),CFVector3(2514, 180, -2307),10,0,0,255);
}
