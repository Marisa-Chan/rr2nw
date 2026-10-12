/*
        Coded by Green.
        Describe land based dynamic objects moving AI using local view region.
        BEGIN: 08/05/98
 */
#include "nearAI.h"
#include "mathlib.h"
#include "tank.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "kernel/h/context.h"
#include "draw.h"
#include "message/unitmsg.h"
#include "message/dcrossmsg.h"
#include "dynamic.h"
#include "scene.h"

#include "cmap.h"

//extern CMap<byte> g_oMap;
//static const double cellSize = 10;

//-------------------------------------------------------------
inline double ScalarMult(CFVector2 v1, CFVector2 v2){
        return(v1.x*v2.x + v1.y*v2.y);
}
//-------------------------------------------------------------
static double GetStaticObstacleDist(CFVector2 , CFVector2 , double viewDist, double ){ // fixme
 /*
        double x, y;

        for(double d = 0; d < viewDist/cellSize; d += 0.5){
                x = pos.x/cellSize+d*dir.x;
                y = pos.y/cellSize+d*(-dir.y);
                if( x < 0 || y < 0 || x > g_oMap.GetW() || y > g_oMap.GetH())
                   continue;
                if(g_oMap(x, y)){
					return(d);
				}
        }
        */
        return(viewDist);
}
//-------------------------------------------------------------
static double GetDynamicObstacleDist(const Tank &tank, CFVector2 dir2, double viewDist, CFVector2 &nearestBumpObjectDir){
	ct_SubjectFindData fsd;
	KR_Event event;
	IDynamicObject *iFace;
	int xMin, xMax, zMin, zMax;
	CFVector3 pos, objPos;
	CFVector3 objDir, dir;
	double selfSpeed, objSpeed;
	double objR, selfR, bumpDist, dist;
	
	pos   = tank.getPosition();
	dir   = CFVector3(dir2.x, 0, -dir2.y);
	xMin  = pos.x-viewDist;
	xMax  = pos.x+viewDist;
	zMin  = pos.z-viewDist;
	zMax  = pos.z+viewDist;
	bumpDist = viewDist;
	iFace =(IDynamicObject *)(tank.getContext()->queryInterface( tank.getObjectID(), IDynamicObjectIID));
	selfR     = iFace->getRadius();
	selfSpeed = iFace->getMoveSpeed();
	if(selfSpeed <= 0) return(bumpDist);

	ct_Arena::findFirstSubject( fsd, xMin, zMin, xMax, zMax );
	for(int i = 0; i < fsd.getCount(); ++i ){
		const KR_ObjectID  &objID = fsd[i];
		if(objID == tank.getObjectID()) continue;
		iFace =(IDynamicObject *)(tank.getContext()->queryInterface( objID, IDynamicObjectIID));
		if(iFace == NULL) continue;
		
		objR      = iFace->getRadius();
		objPos    = iFace->getPos();
		objSpeed  = iFace->getMoveSpeed();
		objDir    = iFace->getMoveDir();
		objDir.y  = 0;
	
		CFVector3 v10 = objDir - dir;
		CFVector3 s01 = pos - objPos;
		double s2 = s01*s01, sv = s01*v10;
		//if( v10.x == 0 && v10.y == 0 && v10.z == 0 ) return FALSE;
		if( s2 < 1e-3  || sv < 0) continue;
		double	r2 = selfR+objR;

		r2 = s2-r2*r2;
		if( r2 < 0 ) { // мы, бля уже столкнулись и разлетаемся
			//echo("Fuck");
		}
		double	v2 = v10*v10;
		if( v2 < 1e-6 ) continue;
		double	d = sv*sv-v2*r2;
		if( d <= 0 ) continue;
		double t = (sv-sqrt(d))/v2;

		dist = selfSpeed*t;
		if(dist <= 0) return(viewDist/2);
		if(dist < bumpDist && dist < viewDist){
			bumpDist = dist;
			nearestBumpObjectDir = CFVector2(objDir.x, -objDir.z);
			//echo("Bump in %.1f meters", bumpDist);
		}
		if(bumpDist < selfR) break;
	}
	return(bumpDist);
} 
//-------------------------------------------------------------
double GetStaticDirPref(CFVector2 pos, CFVector2 gDir, CFVector2 dir,
                               double viewDist, double obstH, double tol){
   double d;
   double val;

   d   = GetStaticObstacleDist(pos, dir, viewDist, obstH);
   val = 0;
   val += (d <= 2) ? 1.1*tol: (viewDist-d)/viewDist*tol*0.6;
   val += ((1-ScalarMult(dir, gDir))/2)*tol*0.3;

   return(val);
}
//-------------------------------------------------------------
double GetDynamicDirPref(const Tank &tank, CFVector2 gDir, CFVector2 dir, CFVector2 prevDir,
                                double viewDist, double tol){
   double d;
   double val;
   CFVector2 nearestBumpObjectDir;

   d   = GetDynamicObstacleDist(tank, dir, viewDist, nearestBumpObjectDir);
   Normal(nearestBumpObjectDir);
   val = 0;
   val += (d <= 1) ? 1.1*tol: (viewDist-d)/viewDist*tol*0.5;
   val += ((1-ScalarMult(dir, gDir))/2)*tol*0.2;
   val += (1-ScalarMult(nearestBumpObjectDir, prevDir))/2*tol*0.3;

   return(val); 
}
//-------------------------------------------------------------
void Move(Tank &tank, double time){
        double pref[MAX_DIRECTIONS+1];// +1 is accomadate
        int    allow[MAX_DIRECTIONS+1];// +1 is accomadate
        int dir, bestDir;
        double bVal;
        const double angleTresh = tank.m_attr->m_turnAngleTreshold;
        const double sTol       = tank.m_attr->m_staticTresh;
        const double dTol       = tank.m_attr->m_dynamicTresh;
        const double tol        = sTol+dTol;
        const double obstH      = tank.m_attr->m_obstH;
        const CFVector2 pos(tank.getPosition().x, -tank.getPosition().z);
        double  viewDist;

		tank.m_dir[0] = CFVector2(tank.m_destDir);
        double c = atan2(tank.m_destDir.y, tank.m_destDir.x);
        tank.m_dirAngle[0] = c;
        bestDir = -1;
        for(dir = 0, bVal = sTol; dir < tank.m_dirCnt; ++dir){
           allow[dir] = (ScalarMult(tank.m_dir[dir], tank.m_prevDir) < angleTresh) ? 0 : 1;
           viewDist   = tank.m_attr->m_maxViewDist;
           if(tank.m_dir[dir] == tank.m_prevDir) viewDist *= 2;
		   pref[dir] = 0;
           pref[dir] += GetStaticDirPref (pos, tank.m_destDir, tank.m_dir[dir],
                                          viewDist, obstH, sTol);
           pref[dir] += GetDynamicDirPref(tank, tank.m_destDir, tank.m_dir[dir], tank.m_prevDir,
                                          viewDist, dTol);
           if(pref[0] == 0 && allow[0]) { bestDir = 0; break; }
           if(allow[dir] && pref[dir] < bVal){
                  bestDir = dir;
                  bVal    = pref[dir];
           }
        }
        if(bestDir == -1){  // bestDir is not allowed
           for(dir = 0, bVal = tol*100; dir < tank.m_dirCnt; ++dir)
                  if(pref[dir] < bVal){
                         bestDir = dir;
                         bVal    = pref[dir];
                  }

/*           if(pref[bestDir] >= sTol)
                  echo("!!!!!!!!!!!!!!!!!!Crical clinch - cant move!!!!!!!!!!!");
           else
                  echo("!!!!!!!!!!!!!!!!!!Clinch - perform unallowed turn!!!!!!!!!!!");*/
 
        }
    /*echo("Pos [%.1f, %.1f] Gen dir is %.1f best dir is %.1f", pos.x/cellSize, pos.y/cellSize, 
		  c*180/M_PI, tank.m_dirAngle[bestDir]*180/M_PI);*/
    tank.m_prevDir = tank.m_dir[bestDir];
	tank.model_drive( time, tank.m_needPow, tank.m_dirAngle[bestDir]);
}
//-------------------------------------------------------------
void SetupDirections(Tank &t, int maxDir){
        int i;
        double angle;
        double aPlus;

        t.m_dirCnt = maxDir+1;
        aPlus      = -2*M_PI/maxDir;
        for(i = 1, angle = M_PI; i <= maxDir; ++i, angle += aPlus){
                t.m_dir[i] = CFVector2(cos(angle), sin(angle));
                t.m_dirAngle[i] = angle;
        }
}
//-------------------------------------------------------------
void InitNearAI(Tank &t){
    t.m_destDir     = CFVector2(0, 1);
    t.m_prevDir     = CFVector2(0, 1);
    //t.m_maxViewDist = 30;
    SetupDirections(t, 8);
}
//-------------------------------------------------------------
void ShutdownNearAI(Tank &){
}
