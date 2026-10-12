/*
 * File  : C:\NW\ARENA\OBASE\Explosion\Explosion.cpp
 * Autor :
 * Ver   1.0 
 */

#include "Explosion.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "kernel/h/session.h"
#include "message/explmsg.h"
#include "h/light.h"
#include "h/cachesmoke.h"
#include "message/skinmsg.h"
#include "message/sndmsg.h"
#include "sound.h"
#include "obase/sound/wavobj.h"
#include "i/dynobj.i"
#include "i/unit.i"
#include "i/player.i"
#include "h/phisics.h"
#include "h/olevel.h"

ExplBranch *Explosion::m_free = 0;
ExplBranch  Explosion::m_branch[Explosion::MAX_BRANCH];
int			Explosion::m_explsWithTraces = 0;

#define COLLINE    8
#define MAX_BRIGHT 255
#define MAX_TRACE  4

extern SDeviceList _dL;

 //===========================================================================
class AttributeExplosion : public ct_Attribute
{

    	

 public:
    int             m_brightness[MAX_BRIGHT];
    unsigned long   m_color0;
    unsigned long   m_color1;
    unsigned long   m_color2;
    unsigned long   m_color3;
    unsigned long   m_colBuf[COLLINE*3];
    GR_HTEXTURE     m_hTexture;
    unsigned long   m_colorSnTail, m_colorSnHead, m_colorSnCenter;
    CViewObjectModel* m_cacheSkin;  // 

    WAVObj            * m_wav;	
    ct_ClassTableID     m_ctsndID;
    unsigned long       m_rayColor;

    ct_ClassTableID     m_smokeTableID;
    KR_ObjectID         m_smokeAttrID;



    virtual void    update(double ts);  
//{{ATTRIBUTE
    ct_AttrItem  m_array[88];
    double          m_moveTimeInc            ;  // 
    double          m_minPartSize            ;  // 
    double          m_maxPartSize            ;  // 
    double          m_minPartSnSize          ;  // 
    double          m_maxPartSnSize          ;  // 
    int             m_minPartCnt             ;  // 
    int             m_maxPartCnt             ;  // 
    int             m_minPartSnCnt           ;  // 
    int             m_maxPartSnCnt           ;  // 
    int             m_minPieceCnt            ;  // 
    int             m_maxPieceCnt            ;  // 
    int             m_minPieceSmokeCnt       ;  // 
    int             m_maxPieceSmokeCnt       ;  // 
    int             m_minSmokeCnt            ;  // 
    int             m_maxSmokeCnt            ;  // 
    int             m_RGB0                   ;  // 
    int             m_RGB1                   ;  // 
    int             m_RGB2                   ;  // 
    int             m_RGB3                   ;  // 
    double          m_radius                 ;  // 
    double          m_createRadius           ;  // 
    double          m_createSmokeRadius      ;  // 
    double          m_minPartSpeed           ;  // 
    double          m_maxPartSpeed           ;  // 
    double          m_minPieceSpeed          ;  // 
    double          m_maxPieceSpeed          ;  // 
    double          m_minPartTimeLife        ;  // 
    double          m_maxPartTimeLife        ;  // 
    double          m_minPartSnTimeLife      ;  // 
    double          m_maxPartSnTimeLife      ;  // 
    double          m_minPieceTimeLife       ;  // 
    double          m_maxPieceTimeLife       ;  // 
    double          m_minPieceSmTimeLife     ;  // 
    double          m_maxPieceSmTimeLife     ;  // 
    double          m_minSmokeTimeLife       ;  // 
    double          m_maxSmokeTimeLife       ;  // 
    int             m_sRGB0                  ;  // 
    int             m_sRGB1                  ;  // 
    int             m_sRGB2                  ;  // 
    int             m_sRGB3                  ;  // 
    double          m_minSmokeA              ;  // 
    double          m_maxSmokeA              ;  // 
    double          m_minSmokeB              ;  // 
    double          m_maxSmokeB              ;  // 
    double          m_minSmokeC              ;  // 
    double          m_maxSmokeC              ;  // 
    double          m_minSmokeTA             ;  // 
    double          m_maxSmokeTA             ;  // 
    double          m_minSmokeTB             ;  // 
    double          m_maxSmokeTB             ;  // 
    double          m_minSmokeTC             ;  // 
    double          m_maxSmokeTC             ;  // 
    double          m_minSmokeSpeed          ;  // 
    double          m_maxSmokeSpeed          ;  // 
    double          m_minMulSpeed            ;  // 
    double          m_maxMulSpeed            ;  // 
    ct_AttrStr      m_smokeName              ;  // 
    double          m_ofsVAngle              ;  // 
    double          m_ofsHAngle              ;  // 
    double          m_ofsSpeed               ;  // 
    int             m_snRGBtail              ;  // 
    int             m_snRGBhead              ;  // 
    int             m_snRGBcenter            ;  // 
    double          m_snDeltaT               ;  // Длина хвоста снейка по времени
    int             m_snPartCnt              ;  // Число партиклей в хвосте
    ct_AttrStr      m_pieceName              ;  // 
    double          m_minPieceOySpeed        ;  // 
    double          m_maxPieceOySpeed        ;  // 
    double          m_minPieceOxSpeed        ;  // 
    double          m_maxPieceOxSpeed        ;  // 
    double          m_lightOffset            ;  // 
    double          m_lightRadius            ;  // 
    int             m_lightColor             ;  // 
    double          m_lightTimeLife          ;  // 
    double          m_radiusDamage           ;  // 
    double          m_power                  ;  // 
    ct_AttrStr      m_soundName              ;  // 
    int             m_useRay                 ;  // 
    int             m_minRayCnt              ;  // 
    int             m_maxRayCnt              ;  // 
    double          m_minRayLen              ;  // 
    double          m_maxRayLen              ;  // 
    double          m_minRayWidth            ;  // 
    double          m_maxRayWidth            ;  // 
    int             m_rayRGB                 ;  // 
    double          m_traceNewPuffTime       ;  // Время, через которое может народиться новый кусок дыма 
    double          m_ofsSpeedMul            ;  // 
    ct_AttrStr      m_traceSmokeName         ;  // 

    AttributeExplosion()
    {
        m_moveTimeInc        = 0.03;
        m_minPartSize        = 0.2;
        m_maxPartSize        = 0.8;
        m_minPartSnSize      = 0.5;
        m_maxPartSnSize      = 0.6;
        m_minPartCnt         = 10;
        m_maxPartCnt         = 20;
        m_minPartSnCnt       = 8;
        m_maxPartSnCnt       = 12;
        m_minPieceCnt        = 5;
        m_maxPieceCnt        = 8;
        m_minPieceSmokeCnt   = 3;
        m_maxPieceSmokeCnt   = 6;
        m_minSmokeCnt        = 3;
        m_maxSmokeCnt        = 6;
        m_RGB0               = 0;
        m_RGB1               = 0;
        m_RGB2               = 0;
        m_RGB3               = 0;
        m_radius             = 5;
        m_createRadius       = 2;
        m_createSmokeRadius  = 0.5;
        m_minPartSpeed       = 3.0;
        m_maxPartSpeed       = 10.0;
        m_minPieceSpeed      = 3;
        m_maxPieceSpeed      = 10;
        m_minPartTimeLife    = 0.5;
        m_maxPartTimeLife    = 1.0;
        m_minPartSnTimeLife  = 1.0;
        m_maxPartSnTimeLife  = 1;
        m_minPieceTimeLife   = 1.0;
        m_maxPieceTimeLife   = 1.0;
        m_minPieceSmTimeLife = 1;
        m_maxPieceSmTimeLife = 1;
        m_minSmokeTimeLife   = 1;
        m_maxSmokeTimeLife   = 1;
        m_sRGB0              = 0;
        m_sRGB1              = 0;
        m_sRGB2              = 0;
        m_sRGB3              = 0;
        m_minSmokeA          = -1;
        m_maxSmokeA          = -1;
        m_minSmokeB          = 10;
        m_maxSmokeB          = 12;
        m_minSmokeC          = 0.5;
        m_maxSmokeC          = 0.8;
        m_minSmokeTA         = 0;
        m_maxSmokeTA         = 0;
        m_minSmokeTB         = 10;
        m_maxSmokeTB         = 12;
        m_minSmokeTC         = 200;
        m_maxSmokeTC         = 255;
        m_minSmokeSpeed      = 0;
        m_maxSmokeSpeed      = 10;
        m_minMulSpeed        = 1;
        m_maxMulSpeed        = 1;
        strncpy(m_smokeName,"smoke.spr", sizeof( ct_AttrStr )-1 );
        m_ofsVAngle          = 0;
        m_ofsHAngle          = 0;
        m_ofsSpeed           = 4;
        m_snRGBtail          = 0;
        m_snRGBhead          = 0;
        m_snRGBcenter        = 0xFFFFFF;
        m_snDeltaT           = 0.1;
        m_snPartCnt          = 8;
        strncpy(m_pieceName,"Expl.Piece", sizeof( ct_AttrStr )-1 );
        m_minPieceOySpeed    = 2.0;
        m_maxPieceOySpeed    = 2.5;
        m_minPieceOxSpeed    = 0.5;
        m_maxPieceOxSpeed    = 1;
        m_lightOffset        = 5;
        m_lightRadius        = 15;
        m_lightColor         = 7;
        m_lightTimeLife      = 1;
        m_radiusDamage       = 5;
        m_power              = 0.6;
        strncpy(m_soundName,"", sizeof( ct_AttrStr )-1 );
        m_useRay             = 0;
        m_minRayCnt          = 3;
        m_maxRayCnt          = 10;
        m_minRayLen          = 15;
        m_maxRayLen          = 45;
        m_minRayWidth        = 0.1;
        m_maxRayWidth        = 0.3;
        m_rayRGB             = 0xFFFFFF;
        m_traceNewPuffTime   = 0.2;
        m_ofsSpeedMul        = 1;
        strncpy(m_traceSmokeName,"Smoke.Attr.Trace", sizeof( ct_AttrStr )-1 );

        m_array[0].set("m_moveTimeInc",m_moveTimeInc);
        m_array[1].set("m_minPartSize",m_minPartSize);
        m_array[2].set("m_maxPartSize",m_maxPartSize);
        m_array[3].set("m_minPartSnSize",m_minPartSnSize);
        m_array[4].set("m_maxPartSnSize",m_maxPartSnSize);
        m_array[5].set("m_minPartCnt",m_minPartCnt);
        m_array[6].set("m_maxPartCnt",m_maxPartCnt);
        m_array[7].set("m_minPartSnCnt",m_minPartSnCnt);
        m_array[8].set("m_maxPartSnCnt",m_maxPartSnCnt);
        m_array[9].set("m_minPieceCnt",m_minPieceCnt);
        m_array[10].set("m_maxPieceCnt",m_maxPieceCnt);
        m_array[11].set("m_minPieceSmokeCnt",m_minPieceSmokeCnt);
        m_array[12].set("m_maxPieceSmokeCnt",m_maxPieceSmokeCnt);
        m_array[13].set("m_minSmokeCnt",m_minSmokeCnt);
        m_array[14].set("m_maxSmokeCnt",m_maxSmokeCnt);
        m_array[15].set("m_RGB0",m_RGB0);
        m_array[16].set("m_RGB1",m_RGB1);
        m_array[17].set("m_RGB2",m_RGB2);
        m_array[18].set("m_RGB3",m_RGB3);
        m_array[19].set("m_radius",m_radius);
        m_array[20].set("m_createRadius",m_createRadius);
        m_array[21].set("m_createSmokeRadius",m_createSmokeRadius);
        m_array[22].set("m_minPartSpeed",m_minPartSpeed);
        m_array[23].set("m_maxPartSpeed",m_maxPartSpeed);
        m_array[24].set("m_minPieceSpeed",m_minPieceSpeed);
        m_array[25].set("m_maxPieceSpeed",m_maxPieceSpeed);
        m_array[26].set("m_minPartTimeLife",m_minPartTimeLife);
        m_array[27].set("m_maxPartTimeLife",m_maxPartTimeLife);
        m_array[28].set("m_minPartSnTimeLife",m_minPartSnTimeLife);
        m_array[29].set("m_maxPartSnTimeLife",m_maxPartSnTimeLife);
        m_array[30].set("m_minPieceTimeLife",m_minPieceTimeLife);
        m_array[31].set("m_maxPieceTimeLife",m_maxPieceTimeLife);
        m_array[32].set("m_minPieceSmTimeLife",m_minPieceSmTimeLife);
        m_array[33].set("m_maxPieceSmTimeLife",m_maxPieceSmTimeLife);
        m_array[34].set("m_minSmokeTimeLife",m_minSmokeTimeLife);
        m_array[35].set("m_maxSmokeTimeLife",m_maxSmokeTimeLife);
        m_array[36].set("m_sRGB0",m_sRGB0);
        m_array[37].set("m_sRGB1",m_sRGB1);
        m_array[38].set("m_sRGB2",m_sRGB2);
        m_array[39].set("m_sRGB3",m_sRGB3);
        m_array[40].set("m_minSmokeA",m_minSmokeA);
        m_array[41].set("m_maxSmokeA",m_maxSmokeA);
        m_array[42].set("m_minSmokeB",m_minSmokeB);
        m_array[43].set("m_maxSmokeB",m_maxSmokeB);
        m_array[44].set("m_minSmokeC",m_minSmokeC);
        m_array[45].set("m_maxSmokeC",m_maxSmokeC);
        m_array[46].set("m_minSmokeTA",m_minSmokeTA);
        m_array[47].set("m_maxSmokeTA",m_maxSmokeTA);
        m_array[48].set("m_minSmokeTB",m_minSmokeTB);
        m_array[49].set("m_maxSmokeTB",m_maxSmokeTB);
        m_array[50].set("m_minSmokeTC",m_minSmokeTC);
        m_array[51].set("m_maxSmokeTC",m_maxSmokeTC);
        m_array[52].set("m_minSmokeSpeed",m_minSmokeSpeed);
        m_array[53].set("m_maxSmokeSpeed",m_maxSmokeSpeed);
        m_array[54].set("m_minMulSpeed",m_minMulSpeed);
        m_array[55].set("m_maxMulSpeed",m_maxMulSpeed);
        m_array[56].set("m_smokeName",m_smokeName);
        m_array[57].set("m_ofsVAngle",m_ofsVAngle);
        m_array[58].set("m_ofsHAngle",m_ofsHAngle);
        m_array[59].set("m_ofsSpeed",m_ofsSpeed);
        m_array[60].set("m_snRGBtail",m_snRGBtail);
        m_array[61].set("m_snRGBhead",m_snRGBhead);
        m_array[62].set("m_snRGBcenter",m_snRGBcenter);
        m_array[63].set("m_snDeltaT",m_snDeltaT);
        m_array[64].set("m_snPartCnt",m_snPartCnt);
        m_array[65].set("m_pieceName",m_pieceName);
        m_array[66].set("m_minPieceOySpeed",m_minPieceOySpeed);
        m_array[67].set("m_maxPieceOySpeed",m_maxPieceOySpeed);
        m_array[68].set("m_minPieceOxSpeed",m_minPieceOxSpeed);
        m_array[69].set("m_maxPieceOxSpeed",m_maxPieceOxSpeed);
        m_array[70].set("m_lightOffset",m_lightOffset);
        m_array[71].set("m_lightRadius",m_lightRadius);
        m_array[72].set("m_lightColor",m_lightColor);
        m_array[73].set("m_lightTimeLife",m_lightTimeLife);
        m_array[74].set("m_radiusDamage",m_radiusDamage);
        m_array[75].set("m_power",m_power);
        m_array[76].set("m_soundName",m_soundName);
        m_array[77].set("m_useRay",m_useRay);
        m_array[78].set("m_minRayCnt",m_minRayCnt);
        m_array[79].set("m_maxRayCnt",m_maxRayCnt);
        m_array[80].set("m_minRayLen",m_minRayLen);
        m_array[81].set("m_maxRayLen",m_maxRayLen);
        m_array[82].set("m_minRayWidth",m_minRayWidth);
        m_array[83].set("m_maxRayWidth",m_maxRayWidth);
        m_array[84].set("m_rayRGB",m_rayRGB);
        m_array[85].set("m_traceNewPuffTime",m_traceNewPuffTime);
        m_array[86].set("m_ofsSpeedMul",m_ofsSpeedMul);
        m_array[87].set("m_traceSmokeName",m_traceSmokeName);

        linkTable(m_array,88);
    }
//}}END_OF_ATTRIBUTE
};

 //===========================================================================
class ExplosionTable : public ct_SubjectTable
{
 private:
    Explosion *m_table;
 public:
    ExplosionTable()
    {
        m_table = NULL;
        registerClass( "Explosion" );
        Explosion::createFreeList();
    }
    ~ExplosionTable()
    {
        delete [] m_table;
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
    virtual  bool      isRendering();
    virtual  bool      isAudible();
};

bool ExplosionTable::isRendering()
{
   return true;
}

bool ExplosionTable::isAudible()
{
   return true;
}


 //===========================================================================
class AttributeTableExplosion : public ct_AttributeTable
{
 protected:
    AttributeExplosion *m_table;

 public:
    AttributeTableExplosion()
    {
       m_table = NULL;
       registerClass( "ExplosionAttr" );
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static ExplosionTable  __classTable;
static AttributeTableExplosion __attrTable;
 /*********************************
  *
  *   Explosion implementation
  *
  *********************************/

 //============================================================
Explosion::Explosion()
 {
    m_attr = 0;
 }

 //============================================================
Explosion::~Explosion()
 {
 }

 //============================================================
int Explosion::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
	case EXPLOSION_NEWPUFF:
		{
			for(
             ExplBranch *b = m_list.m_next;
             b != &m_list;
             b = b->m_next
           )
             if ( b->m_type == expl_PIECE_WITH_SMOKE)	
			 {
				b->m_createPuffNow = 1;
			 }

           event.timeStamp += m_attr->m_traceNewPuffTime;
           issueEvent( event );


		}

		break;



    case EXPLOSION_MOVE:
     {
        int delCnt = 0;
        T = event.timeStamp - m_startTime;
        T2 = event.timeStamp - m_prevTime;

        if(  T>1 && !m_isVisible  )
        {
             removeAll();
             context->removeObject( getObjectID() );
             break;
        }
        else
        if(  T>15 )
        {
             removeAll();
             context->removeObject( getObjectID() );
             break;
        }
        for(
             ExplBranch *b = m_list.m_next;
             b != &m_list;
             b = b->m_next
           )
             switch( b->m_type )
             {
             case expl_PARTICLE_SIMPLE:  onMovePARTICLE_SIMPLE( *b, delCnt ); break;
             case expl_PARTICLE_SNAKE:   onMovePARTICLE_SNAKE( *b, delCnt );  break;
             case expl_PIECE_SIMPLE:     onMovePIECE_SIMPLE( *b, delCnt );    break;
             case expl_PIECE_WITH_SMOKE: onMovePIECE_WITH_SMOKE( *b, delCnt );break;
             case expl_SMOKE:            onMoveSMOKE( *b, delCnt );           break;
             case expl_RAY: break;
             default: s_ASSERTNQ("Explosion::receiveEvent: Unknown branch type");
             }
             
        updateDel(delCnt);

        if(  m_list.m_next == & m_list  )
             context->removeObject( getObjectID() );
        else
        {
             m_prevTime = event.timeStamp;
             event.timeStamp += m_attr->m_moveTimeInc;
             issueEvent( event );
        }
     }
     break;
    case KR_WAKE_UP:
            break;

    case KR_SET_ATTR:
            s_ASSERTNQ("Explosion:receiveEvent:case KR_SET_ATTR: do't supported");
            break;

 
    case EXPLOSION_START:
     {
        static int ecnt = 0;
        if( ecnt > 0  )
            echo( "%i", ecnt++ );
        m_startTime = m_prevTime = event.timeStamp;
        m_fromID = event.source;

        int index;
        event.data.open(EDO_READ)
                    .getInt(index)
                    .getDouble(m_position.x)
                    .getDouble(m_position.y)
                    .getDouble(m_position.z)
                  .close();
        
        __attrTable.setAttribute(index,(ct_Attribute *&)m_attr);

	    //updateSound();

	    updateSound( getObjectID(),
	    	         context,
		         m_attr->m_ctsndID,
		         m_attr->m_wav,
		         m_snd );


        CFVector3 normal;
        CViewScene::Current()->GetTerrain()->GetPlane(m_position,normal,m_landY);

        setDamage( event.timeStamp );

        int cnt, i;

	//if (_dL.currDevice->swHw != GR_SOFTWARE)
	{
        cnt =  context->rnd_i(m_attr->m_minRayCnt, m_attr->m_maxRayCnt);

        for( i = 0; i < cnt; ++i )
        {
             ExplBranch *b = add();
             if(   b != 0  )
             {
                   b->m_type = expl_RAY;
                   onCreateRAY(*b);
             }
             else break;
        }
        }

        cnt = context->rnd_i(m_attr->m_minPartCnt, m_attr->m_maxPartCnt);
        if(  Session::m_frameSec > 0.05 )
             cnt >>= 1;
        else
        if(  Session::m_frameSec > 0.08 )
             cnt >>= 2;


        for( i = 0; i < cnt; ++i )
        {
             ExplBranch *b = add();
             if(   b != 0  )
             {
                   b->m_type = expl_PARTICLE_SIMPLE;
                   onCreatePARTICLE_SIMPLE(*b);
             }
             else break;
        }

        cnt = context->rnd_i(m_attr->m_minPartSnCnt, m_attr->m_maxPartSnCnt);
        if(  Session::m_frameSec > 0.07 )
             cnt >>= 2;

        for( i = 0; i < cnt; ++i )
        {
             ExplBranch *b = add();
             if(   b != 0  )
             {
                   b->m_type = expl_PARTICLE_SNAKE;
                   onCreatePARTICLE_SNAKE(*b);
             }
             else break;
        }

        cnt = context->rnd_i(m_attr->m_minPieceCnt, m_attr->m_maxPieceCnt);

        if(  Session::m_frameSec > 0.1 )
             cnt = 0;
        else
        if(  Session::m_frameSec > 0.07 )
             cnt >>= 2;
        

        for( i = 0; i < cnt; ++i )
        {
             ExplBranch *b = add();
             if(   b != 0  )
             {
                   b->m_type = expl_PIECE_SIMPLE;
                   onCreatePIECE_SIMPLE(*b);
             }
             else break;
        }

		if (m_explsWithTraces < MAX_TRACE && (!m_attr->m_smokeAttrID.isNUL() ))
		{

		cnt = context->rnd_i(m_attr->m_minPieceSmokeCnt, m_attr->m_maxPieceSmokeCnt);

                if(  Session::m_frameSec > 0.1 )
                     cnt = 0;
                else
                if(  Session::m_frameSec > 0.07 )
                     cnt >>= 2;

	        for( i = 0; i < cnt; ++i )
		    {
			     ExplBranch *b = add();
				 if(   b != 0  )
				 {
					   b->m_type = expl_PIECE_WITH_SMOKE;
					   onCreatePIECE_WITH_SMOKE(*b);
				       
					   event.source 	  = getObjectID();
					   event.label  	  = EXPLOSION_NEWPUFF;
					   event.destination  = getObjectID();
					   event.timeStamp    = m_startTime + m_attr->m_traceNewPuffTime;
					   issueEvent( event );
				 }
				else break;
			}

			m_explsWithTraces++;
			m_hasTraces = 1;
		}
		 else
		m_hasTraces = 0;

        cnt = context->rnd_i(m_attr->m_minSmokeCnt, m_attr->m_maxSmokeCnt);
        if(  Session::m_frameSec > 0.1 )
             cnt >>= 2;

        for( i = 0; i < cnt; ++i )
        {
             ExplBranch *b = add();
             if(   b != 0  )
             {
                   b->m_type = expl_SMOKE;
                   onCreateSMOKE(*b);
             }
             else break;
        }


        m_started = 1;
   	// Sound
       if(  !m_snd.isNUL()  )
       {
         CFVector3 pos(m_position);
         event.label = snd_EV_MOVE_TO;
         event.destination = m_snd;
         event.source      = getObjectID();
	 event.timeStamp   = m_startTime;
         event.data.open(EDO_WRITE)
                     .putDouble(pos.x)
                     .putDouble(pos.y)
                     .putDouble(pos.z)
                   .close();
         context->sendEventNow( event );

         event.destination = m_snd;
	 event.timeStamp   = m_startTime;
         event.source      = getObjectID();

         event.label       = snd_EV_START;
         event.data.open(EDO_WRITE)
                 .putInt(1)
              .close();
         context->sendEventNow( event );
        }
                          
        event.source 	  = getObjectID();
        event.label  	  = EXPLOSION_MOVE;
	event.destination = getObjectID();
	event.timeStamp   = m_startTime;
        receiveEvent( event );
     }
     break;

    default: return 0;
    }
    return 1;
 }

 //============================================================
void Explosion::addNotify()
 {
    ct_Object::addNotify();
    // insert your code this
    m_list.m_next = &m_list;
    m_list.m_prev = &m_list;
    m_viewObj.m_master = this;
    m_attr = 0;
    m_started = 0;
    m_hasTraces = 0;
    m_fromID = KR_ObjectID::NUL();
 }

 //============================================================
void Explosion::removeNotify()
{
    if(  !m_snd.isNUL()  )
         context->removeObject( m_snd );

    ct_Object::removeNotify();
    // insert your code this
    context->removeEvent(EXPLOSION_NEWPUFF,getObjectID());
	if (m_hasTraces)
		m_explsWithTraces--;
}

 /*************************************
  *
  *   ExplosionTable implementation
  *
  *************************************/

 //============================================================
void ExplosionTable::allocObjects( int objectQnty )
 {
    m_table = new Explosion[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void ExplosionTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *ExplosionTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"ExplosionTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 /*************************************
  *
  *   AttributeTable implementation
  *
  *************************************/

 //============================================================
void AttributeTableExplosion::allocObjects( int objectQnty )
 {
    m_table = new AttributeExplosion[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void AttributeTableExplosion::freeObjects()
 {
    delete [] m_table;
    m_table         = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *AttributeTableExplosion::getObjectPTR( int index )
 {
    s_ASSERT(index>=0 && index <m_maxObjectQnty,"AttributeTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 //============================================================
void  Explosion::createFreeList()
{
    for( int i = 0; i < MAX_BRANCH-1; ++i )
    {
         m_branch[i].m_next = &(m_branch[i+1]);
         m_branch[i].m_prev = 0;
    }
    m_branch[MAX_BRANCH-1].m_next = 0;
    m_free = m_branch;
}

 //============================================================
ExplBranch *Explosion::add()
{
     if(  m_free == 0  )
          return 0;

     ExplBranch *cur = m_free;
     m_free = m_free->m_next;

     cur->m_next = m_list.m_next;
     cur->m_prev = &m_list;

     m_list.m_next->m_prev = cur;
     m_list.m_next         = cur;

     return cur;
}

 //============================================================
void Explosion::del( ExplBranch &b, int &cnt )
{
    m_branch[cnt].m_deleted = &b;
    ++cnt;
}

 //============================================================
void Explosion::updateDel( int cnt )
{
    for( int i = 0; i < cnt; ++i )
    {
         ExplBranch * cur = m_branch[i].m_deleted;

         cur->m_next->m_prev = cur->m_prev;
         cur->m_prev->m_next = cur->m_next;

         cur->m_next = m_free;
         m_free      = cur;
    }
}

 //============================================================
void Explosion::removeAll()
{
    int delc = 0;
    for(
         ExplBranch *b = m_list.m_next;
         b != &m_list;
         b = b->m_next
       )
         del( *b, delc );
    updateDel(delc);
}

  /************************
   *
   *
   *         Create
   *
   *
   ************************/


 //============================================================
void Explosion::onCreateRAY( ExplBranch &b )
{
    b.m_mulSpeed = context->rnd_f(-M_PI,M_PI );
    b.xT = context->rnd_f(m_attr->m_minRayLen, m_attr->m_maxRayLen );
    b.yT = context->rnd_f(m_attr->m_minRayWidth, m_attr->m_maxRayWidth );
}

 //============================================================
void Explosion::onCreatePARTICLE_SIMPLE( ExplBranch &b )
{
    b.rA = 0;
    b.rB = 0;
    b.rC = context->rnd_f(m_attr->m_minPartSize,m_attr->m_maxPartSize);

    switch(context->rnd_i()&3)
    {
    case 0: b.m_color = m_attr->m_color0; break;
    case 1: b.m_color = m_attr->m_color1; break;
    case 2: b.m_color = m_attr->m_color2; break;
    case 3: b.m_color = m_attr->m_color3; break;
    }

    CFVector3 dir(
             context->rnd_f(-m_attr->m_createRadius,m_attr->m_createRadius ),
             context->rnd_f(-m_attr->m_createRadius,m_attr->m_createRadius ),
             context->rnd_f(-m_attr->m_createRadius,m_attr->m_createRadius )
                 );


    b.m_startPos = m_position + dir;

    dir = Normal(dir)*context->rnd_f(m_attr->m_minPartSpeed,m_attr->m_maxPartSpeed);
    b.xT = dir.x;
    b.yT = dir.y;
    b.zT = dir.z;

    b.m_timeOfLife = context->rnd_f(m_attr->m_minPartTimeLife,m_attr->m_maxPartTimeLife);
}

 //============================================================
void Explosion::onCreatePARTICLE_SNAKE( ExplBranch &b )
{
    b.m_timeOfLife = context->rnd_f(m_attr->m_minPartSnTimeLife,m_attr->m_maxPartSnTimeLife);
    b.rA = 0;
    b.rB = 0;
    b.rC = context->rnd_f(m_attr->m_minPartSnSize,m_attr->m_maxPartSnSize);

    CFVector3 dir(
             context->rnd_f(-m_attr->m_createRadius,m_attr->m_createRadius ),
             context->rnd_f(-m_attr->m_createRadius,m_attr->m_createRadius ),
             context->rnd_f(-m_attr->m_createRadius,m_attr->m_createRadius )
                 );


    b.m_startPos = m_position + dir;

    dir = Normal(dir)*context->rnd_f(m_attr->m_minPartSpeed,m_attr->m_maxPartSpeed);
    b.xT = dir.x;
    b.yT = dir.y;
    b.zT = dir.z;
    b.m_tailCnt = m_attr->m_snPartCnt;
}

 //============================================================
void Explosion::onCreatePIECE_SIMPLE( ExplBranch &b )
{
    b.m_timeOfLife = context->rnd_f(m_attr->m_minPieceTimeLife,m_attr->m_maxPieceTimeLife);

    /*
     * Attach skin
     */
    b.m_skin.Attach(m_attr->m_cacheSkin);
    b.m_viewDynObj.BumpDef().fRadius = b.m_skin.Model()->Radius();
    //-------------

   CFVector3 dir(
             context->rnd_f(-m_attr->m_createRadius,m_attr->m_createRadius ),
             context->rnd_f(-m_attr->m_createRadius,m_attr->m_createRadius ),
             context->rnd_f(-m_attr->m_createRadius,m_attr->m_createRadius )
                 );


    b.m_startPos = m_position + dir;

    dir = Normal(dir)*context->rnd_f(m_attr->m_minPieceSpeed,m_attr->m_maxPieceSpeed);
    b.xT = dir.x;
    b.yT = dir.y;
    b.zT = dir.z;

    b.m_rotOys = context->rnd_f(m_attr->m_minPieceOySpeed,m_attr->m_maxPieceOySpeed);
    b.m_rotOxs = context->rnd_f(m_attr->m_minPieceOxSpeed,m_attr->m_maxPieceOxSpeed);
}

 //============================================================
void Explosion::onCreatePIECE_WITH_SMOKE( ExplBranch &b )
{
    b.m_timeOfLife = context->rnd_f(m_attr->m_minPieceTimeLife,m_attr->m_maxPieceTimeLife);

    /*
     * Attach skin
     */
    b.m_skin.Attach(m_attr->m_cacheSkin);
    b.m_viewDynObj.BumpDef().fRadius = b.m_skin.Model()->Radius();
    //-------------

   CFVector3 dir(
             context->rnd_f(-m_attr->m_createRadius,m_attr->m_createRadius ),
             context->rnd_f(-m_attr->m_createRadius,m_attr->m_createRadius ),
             context->rnd_f(-m_attr->m_createRadius,m_attr->m_createRadius )
                 );


    b.m_startPos = m_position + dir;

    dir = Normal(dir)*context->rnd_f(m_attr->m_minPieceSpeed * 2,m_attr->m_maxPieceSpeed * 2);
    b.xT = dir.x;
    b.yT = dir.y;
    b.zT = dir.z;

    b.m_rotOys = context->rnd_f(m_attr->m_minPieceOySpeed,m_attr->m_maxPieceOySpeed);
    b.m_rotOxs = context->rnd_f(m_attr->m_minPieceOxSpeed,m_attr->m_maxPieceOxSpeed);
}
double Sqr2(double A, double B, double C)
{
    if(  fabs(A) < 1e-5  )
    {
         if(  fabs(B) < 1e-5  )
              return 1e10;

         double res = -C/B;
         if(  res < 1.001  )
              return 1e10;
         return res;
    }

    double d = B*B-4*A*C;
    if(  d < 0  )
         return 1e10;

    double numer = sqrt(d);
    double q1 = (-B+numer) / (2*A);
    double q2 = (-B-numer) / (2*A);

    if(  q1 > q2  )
         if(  q1 < 0.001  )
              return 1e10;
         else return q1;

    if(  q2 < 0.001  )
         return 1e10;
    return q2;
}

 //============================================================
void Explosion::onCreateSMOKE( ExplBranch &b )
{
    b.m_timeOfLife = context->rnd_f(m_attr->m_minSmokeTimeLife,m_attr->m_maxSmokeTimeLife);
    b.rA = context->rnd_f(m_attr->m_minSmokeA,m_attr->m_maxSmokeA);
    b.rB = context->rnd_f(m_attr->m_minSmokeB,m_attr->m_maxSmokeB);
    b.rC = context->rnd_f(m_attr->m_minSmokeC,m_attr->m_maxSmokeC);

    b.tA = context->rnd_f(m_attr->m_minSmokeTA,m_attr->m_maxSmokeTA);
    b.tB = context->rnd_f(m_attr->m_minSmokeTB,m_attr->m_maxSmokeTB);
    b.tC = context->rnd_f(m_attr->m_minSmokeTC,m_attr->m_maxSmokeTC);

    CFVector3 dir(
             context->rnd_f(-m_attr->m_createSmokeRadius,m_attr->m_createSmokeRadius ),
             context->rnd_f(-m_attr->m_createSmokeRadius,m_attr->m_createSmokeRadius ),
             context->rnd_f(-m_attr->m_createSmokeRadius,m_attr->m_createSmokeRadius )
                 );


    b.m_startPos = m_position + dir;

    dir = Normal(dir)*context->rnd_f(m_attr->m_minSmokeSpeed,m_attr->m_maxSmokeSpeed);
    b.xT = dir.x;
    b.yT = dir.y;
    b.zT = dir.z;
    b.m_mulSpeed = context->rnd_f(m_attr->m_minMulSpeed,m_attr->m_maxMulSpeed);

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

    double t0 = Sqr2(b.tA,b.tB,b.tC);
    double t1 = Sqr2(b.rA,b.rB,b.rC);
    double minTime = t0;
    if(  t1 < minTime  ) minTime = t1;
    if(  b.m_timeOfLife < minTime  ) minTime = b.m_timeOfLife;
    b.m_d_maxTime = 1.0 / minTime;


   //
   // Задаем вектор сноса
   //
   CFMatrix3x4 m;
   m.LoadIdentity();
   m.RotateOzL(m_attr->m_ofsVAngle);
   m.RotateOyL(m_attr->m_ofsHAngle);

   b.m_ofsDir   = (m*CFVector3(0,1,0))*m_attr->m_ofsSpeed;
}


  /************************
   *
   *
   *      MOVE
   *
   *
   ************************/



 //============================================================
inline void Explosion::onMovePARTICLE_SIMPLE( ExplBranch &b, int &delCnt )
{
    if(  T > b.m_timeOfLife  )
         del( b, delCnt );
}

 //============================================================
inline void Explosion::onMovePARTICLE_SNAKE( ExplBranch &b, int &delCnt )
{
    if(  T > b.m_timeOfLife  )
    {
         if(  b.m_tailCnt > 0  ) 
              b.m_tailCnt--;
         else del( b, delCnt );
    }
}

 //============================================================
inline void Explosion::onMovePIECE_SIMPLE( ExplBranch &b, int &delCnt )
{
    if(  T > b.m_timeOfLife  )
    {
         del( b, delCnt );
         return;
    }

    double y = b.m_startPos.y + b.yT*T - 9.8/2*T*T;

    if(  y < m_landY  )
    {
         del( b, delCnt );
    }

}

 //============================================================
inline void Explosion::onMovePIECE_WITH_SMOKE( ExplBranch &b, int &delCnt )
{
    if(  T > b.m_timeOfLife  )
    {
         del( b, delCnt );
         return;
    }


	CFVector3 newPos = b.m_startPos+CFVector3(b.xT*T, b.yT*T - 9.8/2*T*T, b.zT*T);
    

    if(  newPos.y < m_landY  )
    {
         del( b, delCnt );
		 return;
    }

	if (b.m_createPuffNow)
	{
		createSmoke( newPos,
			         getObjectID(),
				     m_prevTime,
					 m_attr->m_smokeTableID,
					 m_attr->m_smokeAttrID
					);

		b.m_createPuffNow = 0;
	}
}

 //============================================================
inline void Explosion::onMoveSMOKE( ExplBranch &b, int &delCnt )
{
    if(  T > b.m_timeOfLife  )
         del( b, delCnt );
    else
    {
         b.xT *= b.m_mulSpeed*(1-T2);
         b.yT *= b.m_mulSpeed*(1-T2);
         b.zT *= b.m_mulSpeed*(1-T2);

         b.m_startPos += CFVector3(b.xT,b.yT,b.zT)*T2 + b.m_ofsDir*T2;
         b.m_ofsDir *= m_attr->m_ofsSpeedMul;
    }
}


  /************************
   *
   *
   *      DRAW
   *
   *
   ************************/


 //============================================================
void Explosion::onDrawRAY( ExplBranch &b, int &delCnt )
{
    CFVector3 np(m_position);   
    CFVector3	v = CViewObject::m_viewPointDirSMx*np;
    int screen_x = Round(v.x*d_Z),
	screen_y = Round(v.y*d_Z);

    double t = Session::m_moment-m_startTime;
    double timeOfLife = 0.6;
    double alpha = (timeOfLife-t)/timeOfLife;
    if(  alpha <=0  )
    {
         del( b, delCnt );
         return;
    }
    double len   = t*b.xT; // Скорость увеличения
    double screen_len  = len*CViewObject::m_viewPointScale.x*d_Z;
    int screen_dx = cos(b.m_mulSpeed)*screen_len;
    int screen_dy = sin(b.m_mulSpeed)*screen_len;
    double width = screen_len*b.yT;
    int transp = (int)(alpha*255);
    if(  transp < 0  ) transp = 0;
    else if( transp >255 ) transp = 255;
    
    if(  transp > 1  && width > 0.6  )
    GRDrawRay( screen_x, screen_y, 
               screen_x+screen_dx, 
               screen_y+screen_dy, m_attr->m_rayColor,
               transp, (int)(65536*d_Z), (float)width, 1.0 - alpha);
}

 //============================================================
void Explosion::onDrawPARTICLE_SIMPLE( ExplBranch &b, int & )
{
    CFVector3 np(b.m_startPos+CFVector3(b.xT*T, b.yT*T - 9.8/2*T2, b.zT*T));
    CFVector3	v = CViewObject::m_viewPointDirSMx*np;

    int screen_width  = Round(b.rC*CViewObject::m_viewPointScale.x*d_Z);
    int screen_x = Round(v.x*d_Z),
		screen_y = Round(v.y*d_Z);

    GRDrawParticle(screen_x, screen_y, screen_width, 65536*d_Z, b.m_color);   
}

 //============================================================
void Explosion::onDrawPARTICLE_SNAKE( ExplBranch &b, int & )
{
    double pT = T-m_attr->m_snDeltaT;
    if(  pT < 0  )
         pT = 0;

    double coef = (T-pT)/m_attr->m_snPartCnt;
    double sizec = b.rC/m_attr->m_snPartCnt;
    int max = b.m_tailCnt;

	int i;
    for( i = 0; i < max; ++i  )
    {
         double t = pT+i*coef,
                t2= t*t;

         CFVector3 np(b.m_startPos+CFVector3(b.xT*t, b.yT*t - 9.8/2*t2, b.zT*t));
         CFVector3	v = CViewObject::m_viewPointDirSMx*np;

         int screen_width  = Round(sizec*i*CViewObject::m_viewPointScale.x*d_Z);
         int screen_x = Round(v.x*d_Z),
		     screen_y = Round(v.y*d_Z);

         GRDrawParticle(screen_x, screen_y, screen_width, 65536*d_Z, m_attr->m_colorSnTail);
    }

    double y;
    CFVector3 np(b.m_startPos+CFVector3(b.xT*T, y = b.yT*T - 9.8/2*T2, b.zT*T));
    CFVector3	v = CViewObject::m_viewPointDirSMx*np;

    int screen_width  = Round(sizec*i*CViewObject::m_viewPointScale.x*d_Z);
    int screen_x = Round(v.x*d_Z),
		screen_y = Round(v.y*d_Z);

    GRDrawParticle(screen_x, screen_y, screen_width, 65536*d_Z, m_attr->m_colorSnHead);

    screen_width = screen_width>>1;
    if(  screen_width<=0  ) screen_width = 1;

    GRDrawParticle(screen_x, screen_y, screen_width, 65536*d_Z, m_attr->m_colorSnCenter);
}

 //============================================================
void Explosion::onDrawPIECE_SIMPLE( CViewDynamicList &list, ExplBranch &b, int & )
{
    CFMatrix3x4 &m = b.m_skin.GetDirModify();
    m.LoadIdentity();
    m.RotateOyL(b.m_rotOys*T);
    m.RotateOxL(b.m_rotOxs*T);
    m.TranslateL(b.m_startPos+CFVector3(b.xT*T, b.yT*T - 9.8/2*T*T, b.zT*T));

    b.m_viewDynObj.prepareToRender();
    list.Load( &b.m_viewDynObj );
}

 //============================================================
void Explosion::onDrawPIECE_WITH_SMOKE( CViewDynamicList &list, ExplBranch &b, int & )
{
    CFMatrix3x4 &m = b.m_skin.GetDirModify();
    m.LoadIdentity();
    m.RotateOyL(b.m_rotOys*T);
    m.RotateOxL(b.m_rotOxs*T);
    m.TranslateL(b.m_startPos+CFVector3(b.xT*T, b.yT*T - 9.8/2*T*T, b.zT*T));

    b.m_viewDynObj.prepareToRender();
    list.Load( &b.m_viewDynObj );
}

 //============================================================
void Explosion::onDrawSMOKE( ExplBranch &b, int &delCnt )
{
    double radius = b.rA*T2+b.rB*T+b.rC;
    double alpha  = b.tA*T2+b.tB*T+b.tC;

    if(  alpha < 0 || radius < 0.01 )
    {
         del( b, delCnt );
         return;
    }
    CFVector3	v( CViewObject::m_viewPointDirSMx*b.m_startPos );
    if( v.z < CViewObject::m_fFrontClip ) return;
    double d_v = 1./v.z;
    
    int screen_width  = Round(radius*CViewObject::m_viewPointScale.x*d_v);
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

    int index = T*b.m_d_maxTime*(COLLINE*3-1);
    if(  index < 0  ) index = 0; else if(  index > (COLLINE*3-1) ) index = (COLLINE*3-1);
    par.color   = m_attr->m_colBuf[index];
    par.opacity = (int)alpha;
    if(  par.opacity  < 0  ) par.opacity = 0 ;
    else if(  par.opacity  > 255  ) par.opacity = 255 ;
         
    par.iz      = (int)(65536*d_v);

    par.hTexture = m_attr->m_hTexture;

    GRDrawAlphaSprite(&par);
}


 //============================================================
void Explosion::draw()
{
    if(  !m_started  )
         return;

    int  delCnt = 0;
    bool drawSimple = false;
    CFVector3  center =  CViewObject::m_viewPointDirSMx*m_position;

    T   = m_prevTime - m_startTime;
    T2  = T*T;
    Z   = center.z;

    if(  Z > CViewObject::m_fFrontClip  )
    {
         drawSimple = true;
         d_Z = 1.0 / Z;
    }

    for(
         ExplBranch *b = m_list.m_next;
         b != &m_list;
         b = b->m_next
       )
         switch( b->m_type )
         {
         case expl_PARTICLE_SIMPLE:  if(  drawSimple  )
                                     onDrawPARTICLE_SIMPLE( *b, delCnt ); break;
         case expl_PARTICLE_SNAKE:   if(  drawSimple  )
                                     onDrawPARTICLE_SNAKE( *b, delCnt );  break;
         //case expl_PIECE_SIMPLE:
         //case expl_PIECE_WITH_SMOKE: onDrawPIECE_WITH_SMOKE( *b, delCnt );break;
         case expl_SMOKE:            onDrawSMOKE( *b, delCnt );           break;
         case expl_RAY:              if( drawSimple )
                                         onDrawRAY  ( *b, delCnt );   
                                     break;
         }
         
     if(  T <= m_attr->m_lightTimeLife  )
     {
          int ind = (int)(T*MAX_BRIGHT/m_attr->m_lightTimeLife);
          if(  ind < 0  ) ind = 0; else if(  ind > MAX_BRIGHT-1  ) ind = MAX_BRIGHT-1;
          
          g_lightChain.add( m_position+CFVector3(0,m_attr->m_lightOffset,0), 
                       m_attr->m_lightColor, 
                       m_attr->m_brightness[ind], 
                       m_attr->m_lightRadius );
     }

     updateDel(delCnt);
}

 //============================================================
void Explosion::render   ( CViewDynamicList &list, double )
{
    if(  m_attr == 0  )
         return;

    int delCnt = 0;
    T   = m_prevTime - m_startTime;
    for(
         ExplBranch *b = m_list.m_next;
         b != &m_list;
         b = b->m_next
       )
         switch( b->m_type )
         {
         case expl_PIECE_SIMPLE:     onDrawPIECE_SIMPLE( list, *b, delCnt );     break;
		 case expl_PIECE_WITH_SMOKE: onDrawPIECE_WITH_SMOKE( list, *b, delCnt ); break;
         }

    m_viewObj.prepareToRender();
    list.Load( &m_viewObj );
    updateDel(delCnt);
}

 //============================================================
void Explosion::endRender( CViewScene *scene )
{
    if(  m_attr == 0  )
         return;

    for(  
         ExplBranch *b = m_list.m_next;
         b != &m_list;
         b = b->m_next
       )
         switch( b->m_type )
         {        
			 case expl_PIECE_SIMPLE:     scene->RemoveLandDynamic( &(b->m_viewDynObj) ); break;
			 case expl_PIECE_WITH_SMOKE: scene->RemoveLandDynamic( &(b->m_viewDynObj) ); break;
         }

    scene->RemoveLandDynamic( &m_viewObj );
}

 //============================================================
CFVector3     Explosion::realPosition() {  return m_position;  }



  /************************
   *
   *
   *   Explosion Object
   *
   *
   ************************/

void s_ExplosionObject::Draw()
{   
    m_master->draw();
}

void s_ExplosionObject::prepareToRender()
{
    m_dynBase = m_dynBase1 = m_bump.start = m_master->m_position;
    m_bump.vel = CFVector3(0,0,0);
    m_bump.fTime = 0;
    m_bump.fRadius = m_master->m_attr->m_radius;
}

 //============================================================
#define RGB_TO_LIST(col)  ((col)>>16), ((col)>>8)&255, (col)&255


void AttributeExplosion::update(double ts)
{
   m_color0 = GRCreateColor(RGB_TO_LIST(m_RGB0));
   m_color1 = GRCreateColor(RGB_TO_LIST(m_RGB1));
   m_color2 = GRCreateColor(RGB_TO_LIST(m_RGB2));
   m_color3 = GRCreateColor(RGB_TO_LIST(m_RGB3));

   m_smokeTableID = g_arena.searchSeanceClassTable( "Smoke" );
   m_smokeAttrID = context->searchObject(m_traceSmokeName);


   m_colorSnTail   = GRCreateColor(RGB_TO_LIST(m_snRGBtail));
   m_colorSnHead   = GRCreateColor(RGB_TO_LIST(m_snRGBhead));
   m_colorSnCenter = GRCreateColor(RGB_TO_LIST(m_snRGBcenter));

   m_rayColor =     GRTransparentColor(RGB_TO_LIST(m_rayRGB));
   int i = 0;
   int  r0,g0,b0, r1,g1,b1, r, g, b;
   r0 = m_sRGB0>>16;
   g0 = (m_sRGB0>>8)&255;
   b0 = m_sRGB0&255;

   r1 = m_sRGB1>>16;
   g1 = (m_sRGB1>>8)&255;
   b1 = m_sRGB1&255;


   for( i = 0; i < COLLINE; ++i )
   {
        r = r0+(r1-r0)*i/(COLLINE-1);
        g = g0+(g1-g0)*i/(COLLINE-1);
        b = b0+(b1-b0)*i/(COLLINE-1);

        if(  r < 0  ) r = 0; else if(  r > 255  ) r = 255;
        if(  g < 0  ) g = 0; else if(  g > 255  ) g = 255;
        if(  b < 0  ) b = 0; else if(  b > 255  ) b = 255;
        m_colBuf[i] = GRTransparentColor(r,g,b);
   }

   r0 = m_sRGB1>>16;
   g0 = (m_sRGB1>>8)&255;
   b0 = m_sRGB1&255;

   r1 = m_sRGB2>>16;
   g1 = (m_sRGB2>>8)&255;
   b1 = m_sRGB2&255;

   for( i = 0; i < COLLINE; ++i )
   {
        r = r0+(r1-r0)*i/(COLLINE-1);
        g = g0+(g1-g0)*i/(COLLINE-1);
        b = b0+(b1-b0)*i/(COLLINE-1);

        if(  r < 0  ) r = 0; else if(  r > 255  ) r = 255;
        if(  g < 0  ) g = 0; else if(  g > 255  ) g = 255;
        if(  b < 0  ) b = 0; else if(  b > 255  ) b = 255;
        m_colBuf[i+COLLINE] = GRTransparentColor(r,g,b);
   }

   r0 = m_sRGB2>>16;
   g0 = (m_sRGB2>>8)&255;
   b0 = m_sRGB2&255;

   r1 = m_sRGB3>>16;
   g1 = (m_sRGB3>>8)&255;
   b1 = m_sRGB3&255;

   for( i = 0; i < COLLINE; ++i )
   {
        r = r0+(r1-r0)*i/(COLLINE-1);
        g = g0+(g1-g0)*i/(COLLINE-1);
        b = b0+(b1-b0)*i/(COLLINE-1);

        if(  r < 0  ) r = 0; else if(  r > 255  ) r = 255;
        if(  g < 0  ) g = 0; else if(  g > 255  ) g = 255;
        if(  b < 0  ) b = 0; else if(  b > 255  ) b = 255;
        m_colBuf[i+COLLINE*2] = GRTransparentColor(r,g,b);
   }

   m_hTexture = g_loadSmoke( m_smokeName, NULL );

   /*
    * Search and get skin
    */
    KR_ObjectID skinID =  context->searchObject( m_pieceName );

    KR_Event event;
    event.timeStamp = ts;
    s_ASSERT( !skinID.isNUL(), "AttributeExplosion::update" );
    event.label       = sk_EV_QUERY_MODEL_PTR;
    event.destination = skinID;
    context->sendEventNow( event );
    s_ASSERT(event.label==sk_EV_QUERY_MODEL_PTR_OK,"AttributeExplosion::update");
    event.data.open(EDO_READ)
                .get(&m_cacheSkin,sizeof(void*))
              .close();


    for( i = 0; i < MAX_BRIGHT; ++i )
    {
        int &br = m_brightness[i];
        br = i*20;
        if(  br > 255 ) br = 255;

        if(  i >  MAX_BRIGHT/3  )
        {
             int j = i - MAX_BRIGHT/3;
             br = (int)(255.0/j);
        }
    }

     if (!SetSoundAttr(getObjectID(), context,m_soundName,m_ctsndID, (void *) & m_wav))
	m_wav = NULL;

}


void Explosion::setDamage( double ts )
{
    ct_SubjectFindData fsd;

    g_arena.findFirstSubject( fsd, 
                                   m_position.x-m_attr->m_radius,
                                   m_position.z-m_attr->m_radius,
                                   m_position.x+m_attr->m_radius,
                                   m_position.z+m_attr->m_radius
                                   );

    CFVector3 pos;
    double    radius;
    IPlayer *player = 0;
    if(  !m_fromID.isNUL()  )
         player = (IPlayer*)(context->queryInterface(m_fromID,IPlayerIID));

    IUnit *fromu = (IUnit *)(context->queryInterface(m_fromID,IUnitIID ));


    for( int i = 0; i < fsd.getCount(); ++i )
    {
         KR_ObjectID curID(fsd[i]);

         IDynamicObject *dobj = (IDynamicObject *)
                  (context->queryInterface( curID,IDynamicObjectIID ));
         if(  dobj == NULL  )
              continue;

         pos    = dobj->getPos();
         radius = dobj->getRadius();

         double dist = Abs(pos-m_position);

         if(  dist < radius+m_attr->m_radiusDamage  )
         {
              IUnit *uobj = (IUnit *)(context->queryInterface( curID,IUnitIID ));
              if(  uobj == NULL  )
                   continue;

              double d = m_attr->m_power*(1-(dist-radius)/m_attr->m_radiusDamage);

              if(  player==0  &&  fromu != 0  )
              if(  uobj->isFriend(fromu->getCommander()) )
                   d *= g_levelAttr.m_fromFriendDamageScale;

              uobj->setDamage( d, m_position, ts, m_fromID );

              if(  player!=0  )
              {
                   KR_ObjectID unitCom = uobj->getCommander();
                   if(  !unitCom.isNUL() )
                        player->attackUnit(unitCom,d);

              }
         }
    }
    
}

/* End of file C:\NW\ARENA\OBASE\Explosion\Explosion.cpp */

