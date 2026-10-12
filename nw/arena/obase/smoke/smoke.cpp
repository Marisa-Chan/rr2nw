/*
 * File  : C:\NW\ARENA\OBASE\Smoke\Smoke.cpp
 * Author :
 * Ver   1.0 
 */

#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "Smoke.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/fountmsg.h"
#include "h/cachesmoke.h"
#include "h/phisics.h"

 //===========================================================================
class AttributeSmoke : public ct_Attribute
{
 public:
    enum
    {
         MAX_COLOR = 32
    };
    GR_HTEXTURE    m_cacheImage;
    unsigned long  m_cacheColor;
    unsigned long  colors[MAX_COLOR];
    
  
    virtual void    update(double);
  //{{ATTRIBUTE
    ct_AttrItem  m_array[39];
    double          m_radius                 ;  // 
    int             m_onLand                 ;  // 
    int             m_maxBlob                ;  // 
    double          m_minDirInc              ;  // 
    double          m_maxDirInc              ;  // 
    double          m_rndOfs                 ;  // RND Radius for start centers
    double          m_ofsHAngle              ;  // 
    double          m_ofsVAngle              ;  // 
    double          m_ofsSpeed               ;  // 
    double          m_dirHAngle              ;  // 
    double          m_dirVAngle              ;  // 
    double          m_dirSpeed               ;  // 
    double          m_maxTimeLife            ;  // 
    double          m_minRA                  ;  // 
    double          m_maxRA                  ;  // 
    double          m_minRB                  ;  // 
    double          m_maxRB                  ;  // 
    double          m_minRC                  ;  // 
    double          m_maxRC                  ;  // 
    double          m_minTA                  ;  // 
    double          m_maxTA                  ;  // 
    double          m_minTB                  ;  // 
    double          m_maxTB                  ;  // 
    double          m_minTC                  ;  // 
    double          m_maxTC                  ;  // 
    ct_AttrStr      m_imageName              ;  // 
    int             m_RGB                    ;  // 
    double          m_timeIncrement          ;  // 
    int             RGB0                     ;  // 
    int             RGB1                     ;  // 
    int             RGB2                     ;  // 
    int             RGB3                     ;  // 
    int             m_isColorGradient        ;  // 
    int             m_useRndDir              ;  // 
    double          m_rndDir                 ;  // 
    double          m_r0                     ;  // 
    double          m_r1                     ;  // 
    double          m_r2                     ;  // 
    double          m_r3                     ;  // 

    AttributeSmoke()
    {
        m_radius             = 3;
        m_onLand             = 1;
        m_maxBlob            = 4;
        m_minDirInc          = 0.9;
        m_maxDirInc          = 0.91;
        m_rndOfs             = 1;
        m_ofsHAngle          = 0;
        m_ofsVAngle          = 0;
        m_ofsSpeed           = 0.5;
        m_dirHAngle          = 0.0;
        m_dirVAngle          = 0.0;
        m_dirSpeed           = 1.0;
        m_maxTimeLife        = 20;
        m_minRA              = 0;
        m_maxRA              = 0;
        m_minRB              = 0;
        m_maxRB              = 0;
        m_minRC              = 1;
        m_maxRC              = 2;
        m_minTA              = 0;
        m_maxTA              = 0;
        m_minTB              = 0;
        m_maxTB              = 0;
        m_minTC              = 200;
        m_maxTC              = 255;
        strncpy(m_imageName,"Smoke.spr", sizeof( ct_AttrStr )-1 );
        m_RGB                = 0;
        m_timeIncrement      = 0.02;
        RGB0                 = 0;
        RGB1                 = 0;
        RGB2                 = 0;
        RGB3                 = 0;
        m_isColorGradient    = 0;
        m_useRndDir          = 0;
        m_rndDir             = 0.2;
        m_r0                 = 0.1;
        m_r1                 = 1;
        m_r2                 = 1;
        m_r3                 = 0.5;

        m_array[0].set("m_radius",m_radius);
        m_array[1].set("m_onLand",m_onLand);
        m_array[2].set("m_maxBlob",m_maxBlob);
        m_array[3].set("m_minDirInc",m_minDirInc);
        m_array[4].set("m_maxDirInc",m_maxDirInc);
        m_array[5].set("m_rndOfs",m_rndOfs);
        m_array[6].set("m_ofsHAngle",m_ofsHAngle);
        m_array[7].set("m_ofsVAngle",m_ofsVAngle);
        m_array[8].set("m_ofsSpeed",m_ofsSpeed);
        m_array[9].set("m_dirHAngle",m_dirHAngle);
        m_array[10].set("m_dirVAngle",m_dirVAngle);
        m_array[11].set("m_dirSpeed",m_dirSpeed);
        m_array[12].set("m_maxTimeLife",m_maxTimeLife);
        m_array[13].set("m_minRA",m_minRA);
        m_array[14].set("m_maxRA",m_maxRA);
        m_array[15].set("m_minRB",m_minRB);
        m_array[16].set("m_maxRB",m_maxRB);
        m_array[17].set("m_minRC",m_minRC);
        m_array[18].set("m_maxRC",m_maxRC);
        m_array[19].set("m_minTA",m_minTA);
        m_array[20].set("m_maxTA",m_maxTA);
        m_array[21].set("m_minTB",m_minTB);
        m_array[22].set("m_maxTB",m_maxTB);
        m_array[23].set("m_minTC",m_minTC);
        m_array[24].set("m_maxTC",m_maxTC);
        m_array[25].set("m_imageName",m_imageName);
        m_array[26].set("m_RGB",m_RGB);
        m_array[27].set("m_timeIncrement",m_timeIncrement);
        m_array[28].set("RGB0",RGB0);
        m_array[29].set("RGB1",RGB1);
        m_array[30].set("RGB2",RGB2);
        m_array[31].set("RGB3",RGB3);
        m_array[32].set("m_isColorGradient",m_isColorGradient);
        m_array[33].set("m_useRndDir",m_useRndDir);
        m_array[34].set("m_rndDir",m_rndDir);
        m_array[35].set("m_r0",m_r0);
        m_array[36].set("m_r1",m_r1);
        m_array[37].set("m_r2",m_r2);
        m_array[38].set("m_r3",m_r3);

        linkTable(m_array,39);
    }
//}}END_OF_ATTRIBUTE
};


 //===========================================================================
class SmokeTable : public ct_SubjectTable
{
 private:
    Smoke *m_table;
 public:
    SmokeTable()
    {
        m_table = NULL;
        registerClass( "Smoke" );
    }
    ~SmokeTable()
    {
        delete [] m_table;
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
    virtual  bool      isRendering();
};

 //===========================================================================
class AttributeTableSmoke : public ct_AttributeTable
{
 protected:
    AttributeSmoke *m_table;

 public:
    AttributeTableSmoke()
    {
       m_table = NULL;
       registerClass( "SmokeAttr" );
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static SmokeTable  __classTable;
static AttributeTableSmoke __attrTable;
 /*********************************
  *
  *   Smoke implementation
  *
  *********************************/

 //============================================================
Smoke::Smoke()
 {
    m_attr = 0;
    m_cnt  = 0;
 }

 //============================================================
Smoke::~Smoke()
 {
 }

void Smoke::onHide(double)
{
    m_setRemove = 1;
//    context->removeObject(getObjectID());
}


void Smoke::setSmokeAttr()
{
	ct_Attribute *attr = __attrTable.searchAttribute(m_smokeAttrID);
    if( attr==NULL )
		echo( "Smoke::receiveEvent: Unknown attribute %s",
		context->searchObject(m_smokeAttrID));
	else 
		m_attr = (AttributeSmoke*)attr;
}

 //============================================================
int Smoke::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case fou_EVC_MOVING:
            if(  m_setRemove  )
            {
                 context->removeObject(getObjectID());
                 break;
            }

            if(  onMove(event.timeStamp-m_prevTimeStamp)  )
            {
                 m_prevTimeStamp = event.timeStamp;

                 s_ASSERT(m_attr->m_timeIncrement >0.002,"Smoke bed");
                 event.timeStamp += m_attr->m_timeIncrement;
                 m_viewIter--;
            
                 issueEvent( event );
            }
            break;

    case KR_WAKE_UP:
            break;

    case KR_SET_ATTR:
            s_ASSERTNQ("Smoke:receiveEvent:case KR_SET_ATTR: do't supported");
            break;

    case fou_EVCMD_START:
            {
            //KR_ObjectID oID;

            event.data.open(EDO_READ)
                        .getObjectID(m_smokeAttrID)
                        .getDouble(m_pos.x)
                        .getDouble(m_pos.y)
                        .getDouble(m_pos.z)
                      .close();

			setSmokeAttr();

            

            m_viewIter = MAX_ITER_WAIT;
            if(  m_attr->m_onLand  )
            {
                 double height;
                 CFVector3 normal;
                 CViewScene::Current()->GetTerrain()->GetPlane(m_pos,normal,height);
                 m_pos.y = height;
            }
            m_prevTimeStamp  = event.timeStamp;
            onCreate();

            event.label      = fou_EVC_MOVING;
            event.source     = getObjectID();
            context->sendEventNow( event );

            }
            break;

    case fou_EVCMD_START_WITHDIR:
     {
            KR_ObjectID oID;
            CFVector3   dir;

            event.data.open(EDO_READ)
                        .getObjectID(oID)
                        .getDouble(m_pos.x)
                        .getDouble(m_pos.y)
                        .getDouble(m_pos.z)
                        .getDouble(dir.x)
                        .getDouble(dir.y)
                        .getDouble(dir.z)
                      .close();

            ct_Attribute *attr = __attrTable.searchAttribute(oID);
            if( attr==NULL )
                 echo( "Smoke::receiveEvent: Unknown attribute %s",
                       context->searchObject(oID));
            else m_attr = (AttributeSmoke*)attr;

            m_viewIter = MAX_ITER_WAIT;
            if(  m_attr->m_onLand  )
            {
                 double height;
                 CFVector3 normal;
                 CViewScene::Current()->GetTerrain()->GetPlane(m_pos,normal,height);
                 m_pos.y = height;
            }
            m_prevTimeStamp  = event.timeStamp;
            onCreate( dir );

            event.label      = fou_EVC_MOVING;
            event.source     = getObjectID();
            context->sendEventNow( event );
     }
            break;


    default: return 0;
    }
    return 1;
 }

 //============================================================
void s_SmokeObject::prepareToRender()
{
    int cnt = m_master->m_cnt;
    m_visible = false;

    if(  cnt > 0  )
    {
         CFVector3 p(0,0,0);

         for( int i = 0; i < cnt; ++i )
              p += m_master->m_blob[i].m_position;
         p *= 1./cnt;
         m_dynBase = m_dynBase1 = m_bump.start = p;
         CFVector3 v = CViewObject::m_viewPointDirSMx*p;
         if(  v.z > 1  )
         {
              m_visible = true;
              m_z       = (int)(65536./v.z);

              m_bump.vel = CFVector3(0,0,0);
              m_bump.fTime = 0;
              m_bump.fRadius = m_master->m_attr->m_radius;
         }
    }

}

 //============================================================
void Smoke::addNotify()
 {
    ct_Subject::addNotify();
    // insert your code this
    m_viewObj.m_master = this;
    m_attr = 0;
    m_cnt  = 0;
    m_setRemove = 0;
 }

 //============================================================
void Smoke::removeNotify()
 {
    ct_Subject::removeNotify();
    // insert your code this
 }

 //============================================================
void Smoke::draw()
 {
     if(  m_cnt <= 0  )
          return;
     int i;
    

     for( i = 0; i < m_cnt; ++i )
     {
          SmokeBlob &b = m_blob[i];
          CFVector3	v = CViewObject::m_viewPointDirSMx*b.m_position;
          if( v.z < CViewObject::m_fFrontClip ) continue;
          double d_v = 1./v.z;

          
          int screen_width  = Round(b.m_radius*CViewObject::m_viewPointScale.x*d_v);
          int screen_x = Round(v.x*d_v),
		      screen_y = Round(v.y*d_v);

          SGRAlphaSprite par;

          par.x0 = screen_x-screen_width/2;
          par.y0 = screen_y-screen_width/2;
          par.x1 = par.x0+screen_width;
          par.y1 = par.y0+screen_width;

          par.u0 = b.u0;
          par.v0 = b.v0;
          par.u1 = b.u1;
          par.v1 = b.v1;

          if(  m_attr->m_isColorGradient  )
          {
               int index = (int)(b.m_phase*(AttributeSmoke::MAX_COLOR-1.0)/b.m_maxTimeLife);
               if(  index < 0  ) index = 0;
               else
               if(  index > AttributeSmoke::MAX_COLOR-1  ) 
                    index = AttributeSmoke::MAX_COLOR-1;

               par.color   = m_attr->colors[index];
          }
          else par.color   = m_attr->m_cacheColor;
          par.opacity = b.m_alpha;
          par.iz      = (int)(d_v*65536);

          par.hTexture = m_attr->m_cacheImage;

          GRDrawAlphaSprite(&par);
     }

 }

 /*************************************
  *
  *   SmokeTable implementation
  *
  *************************************/
 //============================================================
bool SmokeTable::isRendering()
{
    return 1;
}

 //============================================================
void SmokeTable::allocObjects( int objectQnty )
 {
    m_table = new Smoke[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void SmokeTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *SmokeTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"SmokeTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 /*************************************
  *
  *   AttributeTable implementation
  *
  *************************************/

 //============================================================
void AttributeTableSmoke::allocObjects( int objectQnty )
 {
    m_table = new AttributeSmoke[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void AttributeTableSmoke::freeObjects()
 {
    delete [] m_table;
    m_table         = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *AttributeTableSmoke::getObjectPTR( int index )
 {
    s_ASSERT(index>=0 && index <m_maxObjectQnty,"AttributeTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 //============================================================
int  Smoke::addBlob()
{
   if(  m_cnt < MAXSMOKEBLOB  )
   {
        m_cnt++;
        return  m_cnt-1;
   }

   return -1;
}

 //============================================================
void Smoke::delBlob(int index)
{
    if(  index <  m_cnt && index >=0  )
    {
         for( int i = index+1; i < m_cnt; ++i )
              m_blob[i-1] = m_blob[i];
         m_cnt--;
    }
}

 //============================================================
void s_SmokeObject::Draw()
{
       m_master->draw();
}

 //============================================================
void Smoke::render( CViewDynamicList &list, double )
{
     m_viewIter = MAX_ITER_WAIT;
     m_viewObj.prepareToRender();

     if(  m_viewObj.m_visible  )
          list.Load( &m_viewObj );
}

 //============================================================
void Smoke::endRender( CViewScene *scene )
{
     if(  m_viewObj.m_visible  )
          scene->RemoveLandDynamic( &m_viewObj );
}

 //============================================================
CFVector3 Smoke::realPosition()
{
    return m_pos;
}


 //============================================================
void Smoke::preCreate( SmokeBlob &b )
{
   int u0 = 2,
       v0 = 2,
       u1 = 126,
       v1 = 126,t;

   int morph = context->rnd_i();
   if(  morph&1  )
   {
        t = u0; u0 = u1; u1 = t;
   }
   if(  morph&2  )
   {
        t = v0; v0 = v1; v1 = t;
   }

   //
   // Координаты в текстуре
   //
   int pos = context->rnd_i()&3;
   int x = (pos& 1)*128,
       y = (pos>>1)*128;
   b.u0 = (x+u0)<<16;
   b.v0 = (y+v0)<<16;
   b.u1 = (x+u1)<<16;
   b.v1 = (y+v1)<<16;

   b.m_ref   = m_attr->m_cacheImage;
   b.m_phase = 0;
   b.m_color = m_attr->m_cacheColor;

   //
   // Позиция внутри сферы
   //
   CFVector3 p(m_pos);
   if(  morph&4  )
        p.x += context->rnd_f(m_attr->m_rndOfs);
   else p.x -= context->rnd_f(m_attr->m_rndOfs);
   if(  morph&8  )
        p.y += context->rnd_f(m_attr->m_rndOfs);
   else p.y -= context->rnd_f(m_attr->m_rndOfs);
   if(  morph&16  )
        p.z += context->rnd_f(m_attr->m_rndOfs);
   else p.z -= context->rnd_f(m_attr->m_rndOfs);

   b.m_position = b.m_startPos = p;
   b.m_dirIncrement = context->rnd_f(m_attr->m_minDirInc,m_attr->m_maxDirInc);

   //
   // Задаем вектор сноса
   //
   CFMatrix3x4 m;
   m.LoadIdentity();
   m.RotateOzL(m_attr->m_ofsVAngle);
   m.RotateOyL(m_attr->m_ofsHAngle);

   b.m_ofsDir   = (m*CFVector3(0,1,0))*m_attr->m_ofsSpeed;


   b.m_maxTimeLife = m_attr->m_maxTimeLife;


   b.tA         = context->rnd_f( m_attr->m_minTA, m_attr->m_maxTA );
   b.tB         = context->rnd_f( m_attr->m_minTB, m_attr->m_maxTB );
   b.tC         = context->rnd_f( m_attr->m_minTC, m_attr->m_maxTC );

   if(  m_attr->m_isColorGradient  )
   {
        double d = b.tB*b.tB-4*b.tA*b.tC;
        if(  d >= 0  )
        {
             double sqd = sqrt(d);
             double t0 = (-b.tB-sqd)/(2*b.tA);
             double t1 = (-b.tB+sqd)/(2*b.tA);
             if(  t1 > 0  && t1 < t0  )
                  t0 = t1;
             if(  t0 > 0  )
                  b.m_maxTimeLife = t0;
        }
   }


   if(  m_attr->m_isColorGradient  )
   {
        calcCoef( b.m_maxTimeLife/3, m_attr->m_r0, m_attr->m_r1, m_attr->m_r2, m_attr->m_r3,
                  b.a0, b.a1, b.a2, b.a3 );
   }
   else
   {
        b.rA         = context->rnd_f( m_attr->m_minRA, m_attr->m_maxRA );
        b.rB         = context->rnd_f( m_attr->m_minRB, m_attr->m_maxRB );
        b.rC         = context->rnd_f( m_attr->m_minRC, m_attr->m_maxRC  );
   }

}

 //============================================================
void Smoke::onCreate()
{
    for( int i = 0; i < m_attr->m_maxBlob; ++i )
    {
         int num = addBlob();
         SmokeBlob &b = m_blob[num];


         preCreate(b);

         if(  !m_attr->m_useRndDir  )
         {
              CFMatrix3x4 m;
              m.LoadIdentity();
              m.RotateOzL(m_attr->m_dirVAngle);
              m.RotateOyL(m_attr->m_dirHAngle);

              b.m_dir      = (m*CFVector3(0,1,0))*m_attr->m_dirSpeed;
         }
         else
         {
              CFMatrix3x4 m;
              m.LoadIdentity();
              m.RotateOzL(context->rnd_f(-m_attr->m_rndDir,m_attr->m_rndDir));
              m.RotateOyL(context->rnd_f(-M_PI,M_PI));

              b.m_dir      = (m*CFVector3(0,1,0))*m_attr->m_dirSpeed;
         }
  }
}

 //============================================================
void Smoke::onCreate( const CFVector3 &dir )
{
    for( int i = 0; i < m_attr->m_maxBlob; ++i )
    {
         int num = addBlob();
         SmokeBlob &b = m_blob[num];

         preCreate(b);

         b.m_dir      = Normal(dir)*m_attr->m_dirSpeed;
  }
}

 //============================================================
int Smoke::onMove( double t )
{

    if(  m_cnt <= 0  )
    {
         context->removeObject(getObjectID());
         return 0;
    }

    for( int i = 0; i < m_cnt; ++i )
    {
         SmokeBlob &b = m_blob[i];
         double T = b.m_phase += t;

         if(  m_attr->m_isColorGradient  )
              b.m_radius = b.a0 + b.a1*T + b.a2*T*T + b.a3*T*T*T;    
         else b.m_radius = b.rA*T*T+b.rB*T+b.rC;

         if(  b.m_radius < 0.001 || b.m_phase > m_attr->m_maxTimeLife  )
         {
              delBlob(i);--i;
              continue;
         }
         b.m_alpha = (int)(b.tA*T*T+b.tB*T+b.tC);
         if(  b.m_alpha <= 0  )
         {
              delBlob(i);--i;
              continue;
         }
         if(  b.m_alpha > 255  )
              b.m_alpha = 255;

         b.m_position += (b.m_ofsDir+b.m_dir)*t;
         b.m_dir *= b.m_dirIncrement*(1.0-t);
    }

    if(  m_cnt <= 0  )
    {
         context->removeObject(getObjectID());
         return 0;
    }

    return 1;
}


cs_CacheSmoke g_cacheSmoke[10];
int           g_cacheSmokeCnt = 0;

GR_HTEXTURE   g_loadSmoke( const char *fileName , GR_HTEXTURE hT, void *)
{
   int i;
   GR_HTEXTURE reth = 0;

   if (hT == NULL)
   {
        for( i = 0; i < g_cacheSmokeCnt; ++i )
            if(  strcmpi(g_cacheSmoke[i].fname,fileName) == 0  )
            {
                reth = g_cacheSmoke[i].hand;
                break;
            }


        if(  reth != 0  )
            return reth;

   }

   int x = 0,y = 0, c;

   FILE *f = CFileResource::FOpenCurrent(fileName);

   if(  f != NULL  )
   {
        fread(&x,2,1,f);
        fread(&y,2,1,f);
        fread(&c,1,1,f);

        char *data = new char[x*y+12];
        fread(data+12,x,y,f);

        ((dword*)data)[0] = TEXTURE_ALPHA|TEXTURE_PAL_FORMAT;
        ((dword*)data)[1] = y;
        ((dword*)data)[2] = x;

        reth = GRLoadTextureToDB(hT,NULL,0,(byte*)(data+12));
        RTCHECK(reth!=NULL,"AttributeSmoke::update Can't Load texture");
        GRSetTextureLoadFunc( reth, fileName, g_loadSmoke);
        delete [] data;
        fclose(f);

        if(  g_cacheSmokeCnt < 10 && hT == NULL )
        {
             strncpy(g_cacheSmoke[g_cacheSmokeCnt].fname,fileName,100);
                     g_cacheSmoke[g_cacheSmokeCnt].hand = reth;
             ++g_cacheSmokeCnt;
        }
   }
   else s_ASSERTNQ1("AttributeSmoke::update Can't open file %s",fileName);
   return reth;
}

inline double getR(int x) { return x>>16; }
inline double getG(int x) { return (x>>8)&255; }
inline double getB(int x) { return x&255; }

unsigned long calcRGB( double x, double r[4], double g[4], double b[4] )
{
     int R,G,B;
     double x2 = x*x, x3 = x2*x;

     R = (int)(r[0] + r[1]*x + r[2]*x2 + r[3]*x3);
     G = (int)(g[0] + g[1]*x + g[2]*x2 + g[3]*x3);
     B = (int)(b[0] + b[1]*x + b[2]*x2 + b[3]*x3);

     if(  R < 0  ) R = 0;
     else if(  R > 255  ) R = 255;

     if(  G < 0  ) G = 0;
     else if(  G > 255  ) G = 255;

     if(  B < 0  ) B = 0;
     else if(  B > 255  ) B = 255;

     return GRTransparentColor(R,G,B);
}

void AttributeSmoke::update(double)
{
   m_cacheColor = GRTransparentColor(m_RGB>>16,(m_RGB>>8)&255,m_RGB&255);
   m_cacheImage = g_loadSmoke( m_imageName, NULL );

   if(  m_isColorGradient  )
   {
        double r[4];
        double g[4];
        double b[4];

        calcCoef(MAX_COLOR/3.0, getR(RGB0), getR(RGB1), getR(RGB2), getR(RGB3),
              r[0], r[1], r[2], r[3]);
        calcCoef(MAX_COLOR/3.0, getG(RGB0), getG(RGB1), getG(RGB2), getG(RGB3),
              g[0], g[1], g[2], g[3]);
        calcCoef(MAX_COLOR/3.0, getB(RGB0), getB(RGB1), getB(RGB2), getB(RGB3),
              b[0], b[1], b[2], b[3]);

        for( int i = 0; i < 32; ++i )
            colors[i] = calcRGB(i*32./31., r,g,b);
   }
}


bool	Smoke::dump(PIN_SaveFile & sf)
{
		if (!ct_Subject::dump(sf))
			return false;

		if (!sf.WriteData( (char *) & m_smokeAttrID, sizeof(SmokeData)  ))
			return false;

		return true;
}

bool	Smoke::load(PIN_SaveFile & sf)
{
		if (!ct_Subject::load(sf))
			return false;

		if (!sf.GetData( (char *) & m_smokeAttrID, sizeof(SmokeData)  ))
			return false;
		
		return true;
}

void	Smoke::loadNotify()
{
	ct_Subject::loadNotify();
	setSmokeAttr();
	m_viewObj.m_master = this;

    for( int i = 0; i < m_cnt; ++i )
    {
         SmokeBlob &b = m_blob[i];
		 b.m_ref = m_attr->m_cacheImage;
	}

}

void Smoke::draw( CDC & ){}
/* End of file C:\NW\ARENA\OBASE\Smoke\Smoke.cpp */
