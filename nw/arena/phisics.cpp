#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "zav.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "storage/h/subject.h"
#include "i/dynobj.i"
#include "message/explmsg.h"
#include "message/fountmsg.h"
#include "message/smokermsg.h"
#include "message/unitmsg.h"
#include "message/corpsemsg.h"
#include "message/messagemsg.h"

double interpolateAngle( 
                         double sour, 
                         double dest, 
                         double speed, 
                         double time 
                       )
 {
    double d = dest-sour;

    if(  d >= M_PI  )
         d-= M_PI*2;

    if(  d <= -M_PI  )
         d += M_PI*2;

    double angle = speed*time;
    double a     = fabs(d);

    if(  angle > a  )
         angle = a;

    if(  d > 0  )
         sour += angle;
    else sour -= angle;


    while( sour >= M_PI )
         sour -= 2*M_PI;

    while( sour <=-M_PI )
         sour += 2*M_PI;

    return sour;
 }


double interpolateAngleInert( 
                         double sour, 
                         double dest, 
                         double speed,
                         double maxAngle,
                         double time 
                       )
 {
    double d = dest-sour;

    if(  d >= M_PI  )
         d-= M_PI*2;

    if(  d <= -M_PI  )
         d += M_PI*2;

    double angle = speed*time;
    
    if(  angle > maxAngle  )
         angle = maxAngle;

    if(  d > 0  )
         sour += angle;
    else sour -= angle;


    while( sour >= M_PI )
         sour -= 2*M_PI;

    while( sour <=-M_PI )
         sour += 2*M_PI;

    return sour;
 }

double calcLocalAngle( const CFMatrix3x4 &m, double hA )
{
    CFVector3 hvec(cos(hA),0,sin(hA));
    hvec = hvec - m.Column(1)*(m.Column(1)*hvec);
    return atan2(-(hvec*m.Column(0)), -(hvec*m.Column(2)));
}

double checkCollision(
                    const  CFVector3 &v,
                    const  CFVector3 &d,
                    double            R
                    )
{
    double R0     = Abs2(v);
    double R2     = R*R;

    if(  R0 <= R2 ) // Мы внутри сферы
         return 0;

    double speed2 = Abs2(d);

    if(  speed2 > 1e-4 ) // скорость ненулевая
    {
         double sm = d*v;
         if(  sm  < 0  ) // мы движемся в сторону от сферы
              return -1;

         double t,n,s,s2;

         t = 1/(2*(speed2));
         n = 2*sm;
         s2 = n*n-4*(Abs2(v)-R2)*speed2;

         if(  s2 < 0.0  )
              return -1;

         s = sqrt( s2 );

         return t*(n-s);
    }

    return -1;
}


void calcCoef(double d, double y0, double y1, double y2, double y3,
              double &a0, double &a1, double &a2, double &a3)
{
   a1 = (-11*y0 + 18*y1 - 9*y2 + 2*y3)/(6*d);
   a2 = -(-2*y0  +  5*y1 - 4*y2 + y3  )/(2*d*d);
   a3 = (-y0 + 3*y1 - 3*y2 + y3)/(6*d*d*d);
   a0 = y0;
}

int checkDynamicCollision(
			SBumpDef &def, // мы
                     KR_ObjectID      master,  // кого игнорировать
                     KR_ObjectID     &oID,     // объект, о который стукнемся
                     CFVector3       &oPos,    // позиция объекта
                     double          &oRadius,
                     CFVector3       &oSpeed,
                     double           ts
                         )
{
    def.fMass = 0;
    ct_SubjectFindData fsd;
    CFVector3          to(def.start+def.vel*def.fTime);

    oID = KR_ObjectID::NUL();

    g_arena.findFirstSubject( fsd, def.start.x, def.start.z,
                                   to.x,   to.z);

    //CFVector3 pos,dir;
    //double    radius;
    //double    nearT = 1e10;
    KR_ObjectID nearID = KR_ObjectID::NUL();

    for( int i = 0; i < fsd.getCount(); ++i )
    {
         KR_ObjectID curID(fsd[i]);

         if(  curID != master  )
         {
              IDynamicObject *dobj = (IDynamicObject *)
                       (g_arena.context->queryInterface( curID,IDynamicObjectIID ));

              if(  dobj == NULL  )
                   continue;

		SBumpDef def1;
              def1.start    = dobj->getPos();
              def1.vel    = dobj->getMoveDir()*dobj->getMoveSpeed();
              def1.fRadius = dobj->getRadius();
		def1.fTime = def.fTime;
		def1.nBumpFlags = 0;
		def1.fMass = 1;

              /*
               * Соударяем отрезок со сферой
               */
              /*double t = ::checkCollision(
                        pos-from,
                        mdir-dir,
                        radius+mradius
                        );*/

              if(  Bump(def,def1)  )
              {
                        //nearT = t;
                        nearID = curID;
                        oPos   = def1.start;
                        oSpeed = def1.vel;
                        oRadius= def1.fRadius;

                        KR_Event event;
                        event.timeStamp   = ts + def.fTime;
                        event.label       = t_EV_ONCOLLISION;
                        event.destination = curID;
                        event.source      = master;
                        event.data.open(EDO_WRITE)
                                     .putObjectID(master)
                                  .close();
                        g_arena.getContext()->addEvent( event );
              }
         }
    }

    if(  nearID.isNUL() )
         return 0;

    oID     = nearID;
    return 1;
}

 //============================================================
 /*
  * Возвращает флаг - былоли соударение.
  */
int  checkStaticCollision( 
                     const CFVector3 &from,    // начало движения
                     const CFVector3 &mdir,    // напрвление со скоростью
                     double           mradius, // радиус
                     double           deltaT,  // время для проверки
                     double          &clzTime  // время, через которое стукнемся
                   )
{
    CViewScene *scene = ZAV_Scene();
    SBumpDef bd;
	
    bd.start   = from, 
    bd.vel     = mdir;
    bd.fTime   = deltaT;
    bd.fRadius = mradius;

    bd.nBumpFlags = 0;
    bd.fMass      = 0.001;

    BOOL res = scene->Order()->Bump(bd);

    if(  res  )
    {
         clzTime  = bd.fTime;
         return 1;
    }
    return 0;
}
 //============================================================
 /*
  * Возвращает флаг - былоли соударение.
  */
int  checkCollision( 
                     const CFVector3 &from,    // начало движения
                     const CFVector3 &mdir,    // напрвление со скоростью
                     double           mradius, // радиус
                     double           deltaT,  // время для проверки
                     KR_ObjectID      master,  // кого игнорировать
                     double          &clzTime, // время, через которое стукнемся
                     KR_ObjectID     &oID      // объект, о который стукнемся
                   )
 {
    ct_SubjectFindData fsd;
    CFVector3          to(from+mdir*deltaT);

    oID = KR_ObjectID::NUL();

    g_arena.findFirstSubject( fsd, from.x, from.z,
                                   to.x,   to.z);

    CFVector3 pos,dir;
    double    radius;
    double    nearT = 1e10;
    KR_ObjectID nearID = KR_ObjectID::NUL();

    for( int i = 0; i < fsd.getCount(); ++i )
    {
         KR_ObjectID curID(fsd[i]);

         if(  curID != master  )
         {
              IDynamicObject *dobj = (IDynamicObject *)
                       (g_arena.context->queryInterface( curID,IDynamicObjectIID ));

              if(  dobj == NULL  )
                   continue;

              pos    = dobj->getPos();
              dir    = dobj->getMoveDir()*dobj->getMoveSpeed();
              radius = dobj->getRadius();
              /*
               * Соударяем отрезок со сферой
               */
              double t = ::checkCollision(
                        pos-from,
                        mdir-dir,
                        radius+mradius
                        );

              if(  t >= 0  && t < deltaT  )
              {
                   if(  t < nearT  )
                   {
                        nearT = t;
                        nearID = curID;
                   }
              }
         }
    }

    CViewScene *scene = ZAV_Scene();
    SBumpDef bd;
	
    bd.start   = from, 
    bd.vel     = mdir;
    bd.fTime   = deltaT;
    bd.fRadius = mradius;

    bd.nBumpFlags = 0;
    bd.fMass      = 0.001;


    BOOL res = scene->Order()->Bump(bd);


    if(  (!nearID.isNUL()) && (!res || nearT < bd.fTime)  )
    {
         clzTime  = nearT;
         oID      = nearID;
         return 1;
    }
    else 
    if(  res  )
    {
         clzTime  = bd.fTime;
         return 1;
    }
    return 0;
 }

void  createExplosion( const CFVector3 &p, double t, 
                       const KR_ObjectID &fromID,
                       ct_ClassTableID ctID,int explAttrIndex )
{
    KR_Event event;

    event.label       = EXPLOSION_START;
    event.destination = g_arena.newObject(  ctID, "Expl." );
    event.source      = fromID; 
    event.timeStamp   = t;
      
    if(  !event.destination.isNUL()  )
    {
         event.data.open(EDO_WRITE)
                      .putInt   ( explAttrIndex )
                      .putDouble(p.x)
                      .putDouble(p.y)
                      .putDouble(p.z)
                   .close();
         g_arena.issueEvent( event );
    }
}


void createSmoke( 
                  const CFVector3   &p, 
                  const KR_ObjectID &master,
                  double            ts, 
                  ct_ClassTableID   ctID,
                  const KR_ObjectID attrID
                 )
{
    KR_Event event;

    event.label       = fou_EVCMD_START;
    event.source      = master;       
    event.timeStamp   = ts;
    event.data.open(EDO_WRITE)
                 .putObjectID( attrID )
                 .putDouble(p.x)
                 .putDouble(p.y)
                 .putDouble(p.z)
              .close();

    g_arena.newObject(ctID,"Smok.Static",event); 
}

void  createCorpse( const CFVector3 &p, double t, 
	const KR_ObjectID &fromID, 
	ct_ClassTableID ctID,
	int explAttrIndex )
{

    if (explAttrIndex == -1)
	return;

    KR_Event event;

    event.label       = CORPSE_START_ROTTING;
    event.destination = g_arena.newObject(  ctID, "Corpse.Corpse" );
    event.source      = fromID; 
    event.timeStamp   = t;
      
    if(  !event.destination.isNUL()  )
    {
         event.data.open(EDO_WRITE)
		      .putObjectID(fromID)
                      .putInt   ( explAttrIndex )
                      .putDouble(p.x)
                      .putDouble(p.y)
                      .putDouble(p.z)
                   .close();
         g_arena.issueEvent( event );
    }
}


/*void  createMessage(const KR_ObjectID   &fromID, 			  
			  char * message, 
			  double time ,
			  double ts,
			  char * font)
{
	
    ct_ClassTableID  ctID = g_arena.searchSeanceClassTable("Message" );
	
    KR_Event event;
	
	static char buffer[MESSAGE_TEXT_MAX_LEN];
	
	strcpy(buffer, message);
	
	int len = strlen(buffer);
	char * p = buffer;
	
	for (int i = 0; i < len; i++)
	{
		
		if (buffer[i] == '\n')
		{
			buffer[i] = 0;
			event.label       = MESSAGE_SET_MSG;
			event.destination = g_arena.newObject(  ctID, "Message.Message" );
			event.source      = fromID; 
			event.timeStamp   = ts;
			
			if(  !event.destination.isNUL()  )
			{
				event.data.open(EDO_WRITE)
					.putStr    (font)
					.putStr    (p)
					.putDouble (time)
					.close();
				g_arena.issueEvent( event );
			}
			
			p = & buffer[i+1];
		}
	}
	
	if (*p)
	{
		
		event.label       = MESSAGE_SET_MSG;
		event.destination = g_arena.newObject(  ctID, "Message.Message" );
		event.source      = fromID; 
		event.timeStamp   = ts;
		
		if(  !event.destination.isNUL()  )
		{
			event.data.open(EDO_WRITE)
				.putStr    (font)
				.putStr    (p)
				.putDouble (time)
				.close();
			g_arena.issueEvent( event );
		}
	}
}*/




void  GetMatrixByAngles( CFMatrix3x4 &m, CFVector3 & normal, double angle)
{
    m.LoadIdentity();

    double cos_A = cos(angle),
           sin_A = sin(angle);
    CFVector3 dir(cos_A,0,sin_A);
    CFVector3 dirPlane(-sin_A,0,cos_A);
    double roll = asin(normal*dirPlane);  // >0 to the right
    CFVector3 normalOnPlane = Normal(normal-dirPlane*(normal*dirPlane));
    double pitch = -atan2(dir*normalOnPlane,normalOnPlane.y);

    m.RotateOzL(-roll);
    m.RotateOxL(pitch);
    m.RotateOyL(-angle-M_PI/2);
}


double CalcCannonParam(const double v,const CFVector3 &enemyPos,const CFVector3 &myPos,
                       double &hAngle, double &vAngle,
                       const CFVector3 &eDir)
{
    //double v = tank.m_attr->m_bulletSpeed;
    double l = Abs(enemyPos-myPos);
    
    hAngle = atan2(enemyPos.z-myPos.z,enemyPos.x-myPos.x);
    vAngle = 0;

    if(  v >0.01  )
    {
         double x = hypot(enemyPos.x-myPos.x,enemyPos.z-myPos.z);
         double y = enemyPos.y - myPos.y-3.5; // ЌҐ¬­®Ј® ­Ё¦Ґ
         double g = 9.8;
         double a = g*x*x/(2*v*v);
         double b = -x;
         double c = y+g*x*x/(2*v*v);
         double D = b*b-4*a*c;
         if(  D>=0 && a>0.001  )
         {
              vAngle = atan2((-b-sqrt(D)), (2*a));
         }
         double flyT = x/cos(vAngle)*v;
         
         CFVector3 ePos = enemyPos+flyT*eDir;
         hAngle = atan2(ePos.z-myPos.z,ePos.x-myPos.x);
         
    }

//    vAngle = atan2(enemyPos.y-myPos.y,l)
//                    +asin(l*9.8/(v*v))/2;
    return l;
}
