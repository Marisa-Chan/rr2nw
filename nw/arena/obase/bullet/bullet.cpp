/*
 * File  : D:\GAME\OBASE\Bullet\bullet.cpp
 * Autor :
 * Ver   1.0 
 */

#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "zav.h"

#include "bullet.h"

#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "kernel/h/session.h"

#include "enum/SpaceEnum.h"

#include "message/bulmsg.h"
#include "message/sparkmsg.h"
#include "message/funitmsg.h"
#include "message/fountmsg.h"
#include "message/explmsg.h"
#include "message/sndmsg.h"
#include "message/skinmsg.h"

#include "h/light.h"
#include "h/phisics.h"
#include "h/cachesmoke.h"

#include "i/dynobj.i"

#include "storage/h/savefile.h"

#ifdef  __TRACE_NW__
#include "afxwin.h"
#else
class CDC{};
#endif



//{{EVENT_LABEL_NAMES
static s_ELN elnTable[]=
{
   s_ELN(b_EVC_MOVING,"b_EVC_MOVING"),
   s_ELN(b_EVC_CHECK_COLLISION,"b_EVC_CHECK_COLLISION"),
   s_ELN(b_EV_START,"b_EV_START"),
   s_ELN(),
};

static s_ELNTable selnTable("Default",elnTable);

//}}END_OF_EVENT_LABEL_NAMES


static AttributeBullet __defaultAttr;
static BulletTable  __classTable;
AttributeTableBullet __bulletAttrTable;

 /*********************************
  *
  *   Bullet implementation
  *
  *********************************/

 //============================================================
strg_CONSTRUCTOR_DYNVIEW(Bullet)
 {
    m_attr = &__defaultAttr;
 }

 //============================================================
Bullet::~Bullet()
 {
 }

void Bullet::initTrace()
{
	if (m_attr->m_traceExist)
	{		
		m_traceLength = strlen(m_attr->m_traceWidthString);
		
		for (int i = 0; i < m_traceLength; i++)
		{
			m_viewTrace[i].m_minDist = m_attr->m_traceMinDist;
			switch (i % 4) {
			case 0:
				m_viewTrace[i].m_frontTextureX = 1;
				m_viewTrace[i].m_frontTextureY = 1;
				break;
			case 1:			
				m_viewTrace[i].m_frontTextureX = 127;
				m_viewTrace[i].m_frontTextureY = 1;
				break;
			case 2:			
				m_viewTrace[i].m_frontTextureX = 1;
				m_viewTrace[i].m_frontTextureY = 127;
				break;
			case 3:			
				m_viewTrace[i].m_frontTextureX = 127;
				m_viewTrace[i].m_frontTextureY = 127;
				break;			
			}
			m_viewTrace[i].m_flatDist = m_attr->m_traceAllFlatDist;
		}	// for
		
		
		
		
	}
}


 //============================================================
int Bullet::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case b_EVC_MOVING:
            {
            m_lastMoveTimeStamp = event.timeStamp;
            double prevTimeStamp;

            //{{GET_EVENT(b_EVC_MOVING)
            event.data.open(EDO_READ)
                           .getDouble(prevTimeStamp)
                      .close();
            //}}END_OF_GET_EVENT(b_EVC_MOVING)
            double deltaT = event.timeStamp - prevTimeStamp;

            m_position += (m_dir+CFVector3(0,-9.8,0)*0.5*deltaT)*deltaT; 
            m_dir += CFVector3(0,-9.8,0)*deltaT;


            if(  m_position.y > 0  )
            {
                 prevTimeStamp = event.timeStamp;
                 event.timeStamp += m_attr->m_moveTimeIncrement;
                 //{{PUT_EVENT(b_EVC_MOVING)
                 event.data.open(EDO_WRITE)
                                .putDouble(prevTimeStamp)
                           .close();
                 //}}END_OF_PUT_EVENT(b_EVC_MOVING)
                 issueEvent( event );
                 CFVector3 seg(m_position-m_prevPos);
                 double    segLen = Abs(seg);
                 if(  segLen > m_attr->m_length  )
                 {
                       m_prevPos = m_position - seg*(m_attr->m_length/segLen);
                 }


				 if (m_attr->m_traceExist)
					traceStep(deltaT);
            }
            else
            {
                 createSpark( m_attr->m_cacheSparkAttr, event.timeStamp );
                 context->removeObject( getObjectID() );
            }

            }
            break;


    case KR_WAKE_UP:
            break;

    case KR_SET_ATTR:
            s_ASSERTNQ("Bullet::receiveEvent:case KR_SET_ATTR");
            break;

    case b_EV_START:
		{
			//static bcnt = 0;
			//if(  (bcnt+=1)-1 == 63  )
			//     echo("bullet %i",bcnt++);
            m_lastMoveTimeStamp = event.timeStamp;
            double xVec,yVec,zVec;
            int    attrIndex;
            m_wline = ZAV_Scene()->GetTerrain()->Waterline();
			
			m_traceCurrentTextureOffset  = TEXTURE_CHECK_LENGTH;
			
            //{{GET_EVENT(b_EV_START)
            event.data.open(EDO_READ)
				.descend( VECTOR3D_F, 0 )
				.getDouble(m_position.x)
				.getDouble(m_position.y)
				.getDouble(m_position.z)
				.ascend()
				.descend( VECTOR3D_F, 0 )
				.getDouble(xVec)
				.getDouble(yVec)
				.getDouble(zVec)
				.ascend()
				.getInt(attrIndex)
				.getObjectID(m_bulletMaster)
				.close();
            //}}END_OF_GET_EVENT(b_EV_START)
            __bulletAttrTable.setAttribute(attrIndex,(ct_Attribute *&)m_attr);
            
			m_initialPos	= m_position;
			m_initialDir	= m_dir = Normal(CFVector3(xVec,yVec,zVec))*m_attr->m_startSpeed;
			
			m_traceCurrentSegmentLength = 0;
			
            double prevTimeStamp = event.timeStamp;
			
            //createSpark( m_attr->m_cacheOutSparkAttr, event.timeStamp );
            if (m_attr->m_useBarellSmoke)
			 createSmoke( m_position, CFVector3(xVec,yVec,zVec), event.timeStamp );
			
			
			m_traceCurrentLength = 0;
			
			initTrace();

			KR_Event soundEvent;

			updateSound( getObjectID(),
				context,
				m_attr->m_ctsndID,
				m_attr->m_wav,
				m_snd );
			
			if(  !m_snd.isNUL())
			{
				soundEvent.label = snd_EV_MOVE_TO;
				soundEvent.destination = m_snd;
				soundEvent.source      = getObjectID();
				soundEvent.timeStamp   = prevTimeStamp;
				soundEvent.data.open(EDO_WRITE)
					.putDouble(m_position.x)
					.putDouble(m_position.y)
					.putDouble(m_position.z)
					.close();
				context->sendEventNow( soundEvent);
				
				soundEvent.destination = m_snd;
				soundEvent.source      = getObjectID();
				soundEvent.label       = snd_EV_START;
				soundEvent.data.open(EDO_WRITE)
					.putInt(1)
					.close();
				context->sendEventNow( soundEvent );
				
			}

			
			
            event.timeStamp   = prevTimeStamp;
            m_prevPos = m_position;
			
            event.label  = b_EVC_MOVING;
            event.source = getObjectID();
            //{{PUT_EVENT(b_EVC_MOVING)
            event.data.open(EDO_WRITE)
				.putDouble(prevTimeStamp)
				.close();
            //}}END_OF_PUT_EVENT(b_EVC_MOVING)
            receiveEvent( event );
			
            if(  context == NULL  ) break;
            event.label       = b_EVC_CHECK_COLLISION;
            event.source      = getObjectID();
            event.destination = getObjectID();
            receiveEvent( event );
						
            if(  m_attr->m_useSkin  )
            {
                 m_skin.Attach(m_attr->m_cacheSkin);
                 m_viewDynObj.BumpDef().fRadius = m_skin.Model()->Radius();

            }
			
            }
            break;


    case b_EVC_CHECK_COLLISION:
     {
            double clzTime;
            //{{GET_EVENT(b_EVC_CHECK_COLLISION)
            //}}END_OF_GET_EVENT(b_EVC_CHECK_COLLISION)
            double wtime = -1;
            if(  m_dir.y < 0.01  && m_position.y > m_wline  )
            {
                 wtime = (m_wline - m_position.y)/m_dir.y;
                 if(  wtime > m_attr->m_chkClzTimeIncrement  )
                      wtime = -1;
            }

            if(  checkCollision( clzTime )  )
            {
                 if(  wtime >= 0  && wtime <  clzTime  && m_attr->m_cacheSplashAttr != -1 )
                 createExplosion(  m_position+m_dir*wtime, event.timeStamp+wtime ,
                       m_bulletMaster,
                       m_attr->m_cacheExplosionTable,
                       m_attr->m_cacheSplashAttr
                     );

                 //createSpark( m_attr->m_cacheSparkAttr, event.timeStamp+clzTime );
                 createExplosion(  m_position+m_dir*clzTime, event.timeStamp+clzTime ,
                       m_bulletMaster,
                       m_attr->m_cacheExplosionTable,
                       m_attr->m_cacheExplAttr
                     );

                 //createExplosion( m_position+m_dir*clzTime, event.timeStamp+clzTime );
                 context->removeEvent( b_EVC_MOVING, getObjectID() );
                 context->removeObject( getObjectID() );
            }
            else 
            {
                 if(  wtime >= 0  && m_attr->m_cacheSplashAttr != -1 )
                 createExplosion(  m_position+m_dir*wtime, event.timeStamp+wtime ,
                       m_bulletMaster,
                       m_attr->m_cacheExplosionTable,
                       m_attr->m_cacheSplashAttr
                     );

                 event.timeStamp += m_attr->m_chkClzTimeIncrement;
                 issueEvent( event );
            }
     }
     break;

    case fu_EV_QUERY_SPEED:
            event.label = fu_EV_QUERY_SPEED_OK;
            event.data.open(EDO_WRITE) 
                         .putDouble(m_attr->m_startSpeed) 
                      .close();
            break;

    default: return 0;
    }

    return 1;
 }


 void Bullet::traceStep(double deltaT)
 {
	 if (m_traceCurrentLength > 0)
	 { 
		 m_viewTrace[0].m_dir     = m_dir;
		 m_viewTrace[0].m_pos	  = m_position; 
		 
		 
		 for (int i = 1; i < m_traceCurrentLength; i++)
		 {
			 m_viewTrace[i].m_dir += CFVector3(0,-9.8,0)*deltaT;
			 m_viewTrace[i].m_pos += m_viewTrace[i].m_dir* deltaT; 
		 }
	 }

			 
	 //if (m_traceCurrentLength < MAX_TRACE_LENGTH)
	 if (m_traceCurrentLength < m_traceLength)
	 {
		 
		 CFVector3 dfr = m_viewTrace[m_traceCurrentLength - 1].m_pos - m_initialPos;
		 
		 m_traceCurrentSegmentLength += Abs2(dfr);

		 //if (m_traceCurrentSegmentLength >= TRACE_SEGMENT_LENGTH)

		 if (m_traceCurrentSegmentLength >= m_attr->m_traceSegmentLength)
		 {
			 m_viewTrace[m_traceCurrentLength].m_pos = m_initialPos;
			 m_viewTrace[m_traceCurrentLength].m_dir = m_initialDir;
			 m_viewTrace[m_traceCurrentLength]
				.setTexture(m_attr->m_cacheImage, m_attr->m_cacheImageFront);
			 m_traceCurrentLength++;
			 m_traceCurrentSegmentLength = 0;

			 // Assigning new width 
			 int index;

			 for (int i = 0; i < m_traceCurrentLength; i++)
			 {
				
				index = i / 2;
				
				if (i % 2)
				{
					m_viewTrace[index].m_width0 = (m_attr->m_traceWidthString[index] - 'A') * 0.1;
					ASSERT(m_viewTrace[index].m_width0 > 0);
				}
				 else
				{
					m_viewTrace[m_traceCurrentLength - index - 1].m_width0 = (m_attr->m_traceWidthString[m_traceLength - index - 1] - 'A') * 0.1;
					ASSERT(m_viewTrace[m_traceCurrentLength - index - 1].m_width0 > 0);
				}

			 }

		 }
	 }

	m_traceCurrentTextureOffset -= 20; //m_attr->m_traceTextureD; // TEXTURE_STEP;
	
	if (m_traceCurrentTextureOffset < TEXTURE_FIRST_COORD)
		m_traceCurrentTextureOffset += (TEXTURE_CHECK_LENGTH);


 }

 //============================================================
void Bullet::addNotify()
 {
    ct_Object::addNotify();
    // insert your code this
    m_wline = 0;
 }

 //============================================================
void Bullet::removeNotify()
 {
    if(  !m_snd.isNUL()  )
         context->removeObject( m_snd );

    ct_Object::removeNotify();
    // insert your code this
 }

 //============================================================
#ifdef  __TRACE_NW__
void Bullet::draw( CDC &gc )
 {
    int x = (int)( m_position.x),
        y = (int)(-m_position.z);
    gc.SetPixel(x-1,y,m_attr->m_RGB);
    gc.SetPixel(x  ,y,m_attr->m_RGB);
    gc.SetPixel(x+1,y,m_attr->m_RGB);
    gc.SetPixel(x,y-1,m_attr->m_RGB);
    gc.SetPixel(x,y+1,m_attr->m_RGB);
 }
#else
void Bullet::draw( CDC & ) {}
#endif
 



 //============================================================
void Bullet::createSpark( int attrIndex, double timeStamp )
 {
    KR_Event event;

    event.label        = sp_EV_CREATE;
    event.source       = getObjectID();
    event.destination  = g_arena.newObject(  m_attr->m_cacheSparkTable,"S" );
    if(  event.destination.isNUL()  )
         return;
    event.timeStamp    = timeStamp;

    //{{PUT_EVENT(sp_EV_CREATE)
    event.data.open(EDO_WRITE)
                   .descend(VECTOR3D_F , 0 )
                     .putDouble(m_position.x)
                     .putDouble(m_position.y)
                     .putDouble(m_position.z)
                   .ascend()
                   .putInt(attrIndex)
              .close();
    //}}END_OF_PUT_EVENT(sp_EV_CREATE)
    issueEvent( event );
    //__classTable.getContext()->issueEvent( event );
 }


 /*************************************
  *
  *   BulletTable implementation
  *
  *************************************/

 //============================================================
void BulletTable::allocObjects( int objectQnty )
 {
    m_table = new Bullet[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void BulletTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *BulletTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"BulletTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 //============================================================
bool BulletTable::isRendering()
 {
    return true;
 }

bool BulletTable::isAudible()
{
	return true;
}

 /*************************************
  *
  *   AttributeTable implementation
  *
  *************************************/

 //============================================================
void AttributeTableBullet::allocObjects( int objectQnty )
 {
    m_table = new AttributeBullet[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void AttributeTableBullet::freeObjects()
 {
    delete [] m_table;
    m_table         = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *AttributeTableBullet::getObjectPTR( int index )
 {
    s_ASSERT(index>=0 && index <m_maxObjectQnty,"AttributeTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 //============================================================
void AttributeBullet::update(double ts)
 {

	 if (m_traceExist)
	 {
		m_cacheImage  	  = g_loadSmoke( m_traceTexture, NULL );
		m_cacheImageFront = g_loadSmoke( "smoke.spr", NULL );
	 }

     m_cacheSparkTable     = g_arena.searchSeanceClassTable( m_sparkTable );
     m_cacheSparkAttrTable = g_arena.searchSeanceClassTable( m_sparkAttrTable );
     m_cacheExplosionTable = g_arena.searchSeanceClassTable( "Explosion" );

     KR_ObjectID  sparkAttrID(context->searchObject(m_sparkAttr));
     s_ASSERT1(sparkAttrID!=KR_ObjectID::NUL(),
           "AttributeBullet::addNotify: Unknown attr object<%s>",m_sparkAttr);
      
     m_cacheSparkAttr    = g_arena.getAttributeIndex( 
                                     m_cacheSparkAttrTable,
                                     sparkAttrID 
                                   );

     m_cacheOutSparkAttr = g_arena.getAttributeIndex( 
                                     m_cacheSparkAttrTable,
                                     context->searchObject(m_outSparkAttr) 
                                  );

     int explTableID = g_arena.searchSeanceClassTable( "ExplosionAttr" );
     m_cacheExplAttr = g_arena.getAttributeIndex( 
                                     explTableID,context->searchObject(m_explAttrName) 
                                  );

     m_cacheSplashAttr = g_arena.getAttributeIndex( 
                                     explTableID,context->searchObject(m_splashAttr) 
                                  );

     m_cacheColor = GRCreateColor(m_RGB>>16,(m_RGB>>8)&0xFF,m_RGB&0xFF);

     int r0 = m_RGB0>>16,
         g0 = (m_RGB0>>8)&0xFF,
         b0 = m_RGB0&0xFF;
     int r1 = m_RGB>>16,
         g1 = (m_RGB>>8)&0xFF,
         b1 = m_RGB&0xFF;

     for( int i = 0; i < COLOR_GRAD; ++i )
     {
          int r = r0+(r1-r0)*i/(COLOR_GRAD-1);
          int g = g0+(g1-g0)*i/(COLOR_GRAD-1);
          int b = b0+(b1-b0)*i/(COLOR_GRAD-1);
          if(  r < 0  ) r = 0; else if(  r > 255  ) r = 255;
          if(  g < 0  ) g = 0; else if(  g > 255  ) g = 255;
          if(  b < 0  ) b = 0; else if(  b > 255  ) b = 255;
          m_colorGrad[i]= GRCreateColor(r,g,b);
     }

     m_smokeTableID = g_arena.searchSeanceClassTable( m_smokeTableName );
     m_smokeAttrID  = context->searchObject(m_smokeAttrName);


    if (!SetSoundAttr(getObjectID(), context,m_shootSndName,m_ctsndID, (void *)&m_wav))
		m_wav = NULL;

    if(  m_useSkin  )
         strg_UPDATE_ATTRIBUTE_SKIN(m_skinName,m_cacheSkin,ts)

}

 //============================================================
int  AttributeBullet::receiveEvent( KR_Event &event )
 {
     switch( event.label )
     {
     case fu_EV_QUERY_SPEED:
            event.data.open(EDO_WRITE)
                         .putDouble(m_startSpeed)
                      .close();
            event.label = fu_EV_QUERY_SPEED_OK;
            break;
     default: return ct_Attribute::receiveEvent( event );
     }
     return 1;
 }

 //============================================================
int  Bullet::checkCollision( double &clzTime )
 {
    ct_SubjectFindData fsd;
    CFVector3          to(m_position+m_dir*m_attr->m_chkClzTimeIncrement);

    g_arena.findFirstSubject( fsd, m_position.x,m_position.z,
                                   to.x,        to.z);

    CFVector3 pos,dir;
    double    radius;
    double    nearT = 1e10;
    KR_ObjectID nearID;

    for( int i = 0; i < fsd.getCount(); ++i )
    if(  fsd[i] != m_bulletMaster  )
    {
         IDynamicObject *dobj = (IDynamicObject *)
                  (context->queryInterface( fsd[i],IDynamicObjectIID ));
         if(  dobj == NULL  )
              continue;

         pos    = dobj->getPos();
         dir    = dobj->getMoveDir()*dobj->getMoveSpeed();
         radius = dobj->getRadius();
         /*
          * Соударяем отрезок со сферой
          */
         double t = ::checkCollision(
                   pos-m_position,
                   m_dir-dir,
                   radius
                   );

         if(  t >= 0  )
         {
              if(  t < nearT  )
              {
                   //echo("-------------Bum-------------");
                   nearT = t;
                   nearID = fsd[i];
              }
         }
    }

    CViewScene *scene = ZAV_Scene();
    SBumpDef bd;
	
    bd.start = realPosition(), 
    bd.vel   = m_dir;
	bd.fTime = m_attr->m_chkClzTimeIncrement;
	bd.fRadius = 0.01;

    bd.nBumpFlags = 0;

    BOOL res = scene->Order()->Bump(bd);

    if(  nearT >= 0 && nearT < bd.fTime  )
    {
         clzTime  = nearT;
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

CFVector3 Bullet::realPosition()
 {
    return m_position;
 }


void Bullet::render( CViewDynamicList &list, double ts )
{
    if(  m_attr->m_useSkin  )
    {
         onRender(ts);
         m_viewDynObj.prepareToRender();
         list.Load( &m_viewDynObj );
         return;
    }


    m_viewPart.prepareToRender(
                                   m_prevPos,
                                   m_position+(ts-m_lastMoveTimeStamp)*m_dir,
                                   m_attr->m_radius0, m_attr->m_radius1,
                                   m_attr->m_step0,m_attr->m_step,
                                   m_attr->m_colorGrad,
                                   0
                              );
    list.Load( &m_viewPart );


	for (int i = 0; i < m_traceCurrentLength - 1; i++)
	{
		int texturePos = (m_traceCurrentTextureOffset + TEXTURE_SEGMENT_LENGTH * i);

		while (texturePos > TEXTURE_CHECK_LENGTH)
			texturePos -= (TEXTURE_CHECK_LENGTH);




		m_viewTrace[i].prepareToRender(m_viewTrace[i+1].m_pos, m_viewTrace[i+1].m_width0, texturePos);
		list.Load( &m_viewTrace[i] );
	}

    if(  m_attr->m_useLight  )
    {
         s_ASSERT(m_attr->m_lightColor>=0 && m_attr->m_lightColor < 8,"Bullet::render");
         g_lightChain.add( m_position, 
                           m_attr->m_lightColor, 
                           m_attr->m_lightBrightness, 
                           m_attr->m_lightRadius );
    }
}

void Bullet::endRender( CViewScene *scene )
{
    if(  m_attr->m_useSkin  )
         scene->RemoveLandDynamic( &m_viewDynObj );
    else scene->RemoveLandDynamic( &m_viewPart );
	
	for (int i = 0; i < m_traceCurrentLength; i++)
		scene->RemoveLandDynamic( &m_viewTrace[i]);
}


void  Bullet::createSmoke( const CFVector3 &p, const CFVector3 &d, double t )
{
    if(  Session::m_frameSec>0.09  )
         return;

    KR_Event event;

    event.label       = fou_EVCMD_START_WITHDIR;
    event.destination = g_arena.newObject(  m_attr->m_smokeTableID,"Smok." );
    event.source      = getObjectID(); 
    event.timeStamp   = t;      
      
    if(  !event.destination.isNUL()  )
    {
         event.source      = getObjectID();
         event.data.open(EDO_WRITE)
                      .putObjectID( m_attr->m_smokeAttrID )
                      .putDouble(p.x)
                      .putDouble(p.y)
                      .putDouble(p.z)

                      .putDouble(d.x)
                      .putDouble(d.y)
                      .putDouble(d.z)
                   .close();
         context->sendEventNow( event );
    }
}


bool	Bullet::dump(PIN_SaveFile & sf)
{
		if (!ct_Subject::dump(sf))
			return false;


		if (!sf.WriteData( (char *) & m_attrIndex, sizeof(BulletData)  ))
			return false;

		return true;
}

bool	Bullet::load(PIN_SaveFile & sf)
{
		if (!ct_Subject::load(sf))
			return false;

		if (!sf.GetData( (char *) & m_attrIndex, sizeof(BulletData)  ))
			return false;
		
		return true;
}

void	Bullet::loadNotify()
{
	ct_Subject::loadNotify(); 
	__bulletAttrTable.setAttribute(m_attrIndex,(ct_Attribute *&)m_attr);
	initTrace();

	updateSound( getObjectID(),
		context,
		m_attr->m_ctsndID,
		m_attr->m_wav,
		m_snd );	

        if(  m_attr->m_useSkin  )
        {
             m_skin.Attach(m_attr->m_cacheSkin);
             m_viewDynObj.BumpDef().fRadius = m_skin.Model()->Radius();
        }
}

void Bullet::onRender(double ts)
{
    CFMatrix3x4 &m = m_skin.GetDirModify();
    m.LoadIdentity();

    double ha = atan2(-m_dir.x,-m_dir.z);
    double va = atan2(m_dir.y,hypot(m_dir.x,m_dir.z));
    m.RotateOzL(   m_attr->m_rotSpeedOz*ts);
    m.RotateOxL(va+m_attr->m_rotSpeedOx*ts);
    m.RotateOyL(ha+m_attr->m_rotSpeedOy*ts);
    m.TranslateL( realPosition() + m_dir*(ts-m_lastMoveTimeStamp) );
}

/* End of file D:\GAME\OBASE\Bullet\bullet.cpp */