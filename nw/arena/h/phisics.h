double CalcCannonParam(const double v,const CFVector3 &enemyPos,const CFVector3 &myPos,
                       double &hAngle, double &vAngle,
                       const CFVector3 &eDir);


double interpolateAngle( 
                         double sour, 
                         double dest, 
                         double speed, 
                         double time 
                       );

double interpolateAngleInert( 
                         double sour, 
                         double dest, 
                         double speed, 
                         double maxAngle,
                         double time 
                       );
double calcLocalAngle( const CFMatrix3x4 &m, double hA );

//
// Возвращает параметр t
// v - центр сферы
// d - направление движения
// R - радиус сферы
double checkCollision(
                    const  CFVector3 &v,
                    const  CFVector3 &d,
                    double            R
                    );
void calcCoef(double d, double y0, double y1, double y2, double y3,
              double &a0, double &a1, double &a2, double &a3);

struct SBumpDef;
int checkDynamicCollision(
			SBumpDef &def, // мы
                     KR_ObjectID      master,  // кого игнорировать
                     KR_ObjectID     &oID,     // объект, о который стукнемся
                     CFVector3       &oPos,    // позиция объекта
                     double          &oRadius,
                     CFVector3       &oSpeed,
                     double           ds
                         );

int  checkCollision( 
                     const CFVector3 &from,    // начало движения
                     const CFVector3 &mdir,    // напрвление со скоростью
                     double           mradius, // радиус
                     double           deltaT,  // время для проверки
                     KR_ObjectID      master,  // кого игнорировать
                     double          &clzTime, // время, через которое стукнемся
                     KR_ObjectID     &oID      // объект, о который стукнемся
                   );

void  createExplosion( 
                       const CFVector3 &p, 
                       double           t, 
                       const KR_ObjectID &fromID,
                       ct_ClassTableID  ctID,
                       int              explAttrIndex 
                     );
void createSmoke( 
                  const CFVector3   &p, 
                  const KR_ObjectID &master,
                  double            ts, 
                  ct_ClassTableID   ctID,
                  const KR_ObjectID attrID
                 );

void createCorpse( 	const CFVector3 &p, double t, 
			const KR_ObjectID &fromID, 
			ct_ClassTableID ctID, 
			int explAttrIndex);


/*void  createMessage(const KR_ObjectID   &fromID, 			  
			  char * message, 
			  double time ,
			  double ts,
			  char * font = "Font.p_menu.fnt");*/


void  GetMatrixByAngles( CFMatrix3x4 &m, CFVector3 & normal, double angle);