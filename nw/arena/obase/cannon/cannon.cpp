/*
 * File  : D:\GAME\OBASE\Cannon\Cannon.cpp
 * Autor :
 * Ver   1.0 
 */
#define LAST_H__VIEW
#include "game.h"
#include "Cannon.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/cnmsg.h"
#include "message/groupmsg.h"
#include "message/bulmsg.h"
#include "message/funitmsg.h"
#include "message/skinmsg.h"
#include "enum/spaceEnum.h"
#include "phisics.h"
#include "i/dynobj.i"

#include "message/dcrossmsg.h"
#include "sound.h"


#ifdef  __TRACE_NW__
#include "afxwin.h"
#else
class CDC{};
#endif


//{{EVENT_LABEL_NAMES
static s_ELN elnTable[]=
{
   s_ELN(cn_EV_SHOOT,"cn_EV_SHOOT"),
   s_ELN(cn_EV_IDLEOK,"cn_EV_IDLEOK"),
   s_ELN(cn_EV_END_SHOOTING,"cn_EV_END_SHOOTING"),
   s_ELN(cn_EV_SINGLE_SHOOT,"cn_EV_SINGLE_SHOOT"),
   s_ELN(cn_EVC_IDLE,"cn_EVC_IDLE"),
   s_ELN(cn_EV_ENDOFSHOOT,"cn_EV_ENDOFSHOOT"),
   s_ELN(cn_EVC_ROTATE,"cn_EVC_ROTATE"),
   s_ELN(cn_EVCMD_ROTATE_AND_SHOOT,"cn_EVCMD_ROTATE_AND_SHOOT"),
   s_ELN(cn_EVCMD_DIRECT_SHOOT,"cn_EVCMD_DIRECT_SHOOT"),
   s_ELN(),
};

static s_ELNTable selnTable("Default",elnTable);

//}}END_OF_EVENT_LABEL_NAMES

#define IsNAN(x) ((x)*2==(x) && (x)!=0.0)
 //===========================================================================
class AttributeCannon : public ct_Attribute
{
 public:

    virtual void    update(double ts);  
    virtual int     receiveEvent( KR_Event &event );
//{{ATTRIBUTE
    ct_AttrItem  m_array[12];
    double          m_rotateHSpeed           ;  // Горизонтальная скорость поворота
    double          m_rotateVSpeed           ;  // Вертикальная скорость поворота
    double          m_viewBulletDist         ;  // Расстояние, с которого начинают быть видимыми пули
    CFVector3       m_offset                 ;  // Смещение относительно центра объекта
    CFVector3       m_hAxis                  ;  // Ось горизонтального вращения
    CFVector3       m_vAxis                  ;  // Ось вращения по вертикали
    double          m_cannonLength           ;  // Длина ствола пушки
    double          m_idleTime               ;  // Время ожидания перезарядки
    ct_AttrStr      m_bulletTable            ;  // 
    double          m_minStopAngle           ;  // Минимальный угол, при котором пушка останавливается
    double          m_rotateTimeDelta        ;  // Время пересчета для поворота
    int             m_fixed                  ;  // 

    AttributeCannon()
    {
        m_rotateHSpeed       = (3.14/2.0);
        m_rotateVSpeed       = (3.14/2.0);
        m_viewBulletDist     = 5.0;
        m_offset             = CFVector3(0.71, 4.35, -4.25-1.0);
        m_hAxis              = CFVector3(0,0,0);
        m_vAxis              = CFVector3(0,0,0);
        m_cannonLength       = 2.0;
        m_idleTime           = 1.8;
        strncpy(m_bulletTable,"Bullet", sizeof( ct_AttrStr )-1 );
        m_minStopAngle       = 0.001;
        m_rotateTimeDelta    = 0.04;
        m_fixed              = 1;

        m_array[0].set("m_rotateHSpeed",m_rotateHSpeed);
        m_array[1].set("m_rotateVSpeed",m_rotateVSpeed);
        m_array[2].set("m_viewBulletDist",m_viewBulletDist);
        m_array[3].set("m_offset",EDI_NONE,&m_offset);
        m_array[4].set("m_hAxis",EDI_NONE,&m_hAxis);
        m_array[5].set("m_vAxis",EDI_NONE,&m_vAxis);
        m_array[6].set("m_cannonLength",m_cannonLength);
        m_array[7].set("m_idleTime",m_idleTime);
        m_array[8].set("m_bulletTable",m_bulletTable);
        m_array[9].set("m_minStopAngle",m_minStopAngle);
        m_array[10].set("m_rotateTimeDelta",m_rotateTimeDelta);
        m_array[11].set("m_fixed",m_fixed);

        linkTable(m_array,12);
    }
//}}END_OF_ATTRIBUTE
};

static AttributeCannon __defaultAttr;

 //===========================================================================
class CannonTable : public ct_SubjectTable
{
 private:
    Cannon *m_table;
 public:
    CannonTable()
    {
        m_table = NULL;
        registerClass( "Cannon" );
    }
    ~CannonTable()
    {
        delete [] m_table;
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

 //===========================================================================
class AttributeTableCannon : public ct_AttributeTable
{
 protected:
    AttributeCannon *m_table;

 public:
    AttributeTableCannon()
    {
       m_table = NULL;
       registerClass( "CannonAttr" );
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static CannonTable  __classTable;
static AttributeTableCannon __attrTable;

 /*********************************
  *
  *   Cannon implementation
  *
  *********************************/

 //============================================================
Cannon::Cannon()
 {
    m_attr = &__defaultAttr;
    m_bulletTable = -1;
 }

 //============================================================
Cannon::~Cannon()
 {
 }

void*Cannon::queryInterface( int interf )
{  
    switch( interf )
    {
    case ICannonIID:
            return (ICannon*)this;

    default: s_ASSERTNQ("Cannon::queryInterface");
    }
    return 0;
}
 //============================================================
int Cannon::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case cn_EVC_ROTATE:
            {
            double prevTimeStamp;
            double hAngle,vAngle;

            //{{GET_EVENT(cn_EVC_ROTATE)
            event.data.open(EDO_READ)
                           .getDouble(prevTimeStamp)
                           .getDouble(hAngle)
                           .getDouble(vAngle)
                      .close();
            //}}END_OF_GET_EVENT(cn_EVC_ROTATE)
             s_ASSERT(!IsNAN(hAngle),"Cannon::receiveEvent Bad hangle");
             s_ASSERT(!IsNAN(vAngle),"Cannon::receiveEvent Bad vangle");

             s_ASSERT(hAngle>=-M_PI && hAngle <= M_PI,"Cannon::receiveEvent Bad hangle");
             s_ASSERT(vAngle>=-M_PI && vAngle <= M_PI,"Cannon::receiveEvent Bad vangle");

            if(  fabs(hAngle-m_hAngle)+fabs(vAngle-m_vAngle) < m_attr->m_minStopAngle  )
            {
                 m_hAngle = hAngle;
                 m_vAngle = vAngle;
                 setDirection();
            }
            else
            {
                 double deltaT = event.timeStamp - prevTimeStamp;

                 m_hAngle = interpolateAngle( 
                                  m_hAngle,
                                  hAngle, 
                                  m_attr->m_rotateHSpeed, 
                                  deltaT );
                 m_vAngle = interpolateAngle( 
                                  m_vAngle,
                                  vAngle, 
                                  m_attr->m_rotateVSpeed, 
                                  deltaT );
                 setDirection();

                 prevTimeStamp    = event.timeStamp;
                 event.timeStamp += m_attr->m_rotateTimeDelta;
                 event.source     = getObjectID();
                 //{{PUT_EVENT(cn_EVC_ROTATE)
                 event.data.open(EDO_WRITE)
                                .putDouble(prevTimeStamp)
                                .putDouble(hAngle)
                                .putDouble(vAngle)
                           .close();
                 //}}END_OF_PUT_EVENT(cn_EVC_ROTATE)
                 issueEvent( event );
            }

            }
            break;

    case cn_EVCMD_ROTATE_AND_SHOOT:
            {
            double prevTimeStamp;
            double hAngle,vAngle;
            int    count,bulletAttrIndex;

            if(  m_attr->m_fixed  )
            {
                 event.destination = m_cannonMaster;
                 event.source      = getObjectID();
                 issueEvent( event );
                 break;
            }

            //{{GET_EVENT(cn_EVCMD_ROTATE_AND_SHOOT)
            event.data.open(EDO_READ)
                           .descend(cn_ROTATE , 0 )
                             .getDouble(prevTimeStamp)
                             .getDouble(hAngle)
                             .getDouble(vAngle)
                           .ascend()
                           .descend( cn_SHOOT, 0 )
                             .getInt(count)
                             .getInt(bulletAttrIndex)
                           .ascend()
                      .close();
            //}}END_OF_GET_EVENT(cn_EVCMD_ROTATE_AND_SHOOT)
             s_ASSERT(!IsNAN(hAngle),"Cannon::receiveEvent Bad hangle");
             s_ASSERT(!IsNAN(vAngle),"Cannon::receiveEvent Bad vangle");

             s_ASSERT(hAngle>=-M_PI && hAngle <= M_PI,"Cannon::receiveEvent Bad hangle");
             s_ASSERT(vAngle>=-M_PI && vAngle <= M_PI,"Cannon::receiveEvent Bad vangle");

            if(  fabs(hAngle-m_hAngle)+fabs(vAngle-m_vAngle) < m_attr->m_minStopAngle  )
            {
                 m_hAngle = hAngle;
                 m_vAngle = vAngle;
                 setDirection();

                 context->removeEvent( cn_EV_SHOOT, getObjectID() );

                 event.label       = cn_EV_SHOOT;
                 event.source      = getObjectID();
                 event.destination = getObjectID();

                 //{{PUT_EVENT(cn_EV_SHOOT)
                 event.data.open(EDO_WRITE)
                                .putInt(count)
                                .putInt(bulletAttrIndex)
                           .close();
                 //}}END_OF_PUT_EVENT(cn_EV_SHOOT)

                 issueEvent( event );
            }
            else
            {
                 double deltaT = event.timeStamp - prevTimeStamp;

                 m_hAngle = interpolateAngle( 
                                  m_hAngle,
                                  hAngle, 
                                  m_attr->m_rotateHSpeed, 
                                  deltaT );
                 m_vAngle = interpolateAngle( 
                                  m_vAngle,
                                  vAngle, 
                                  m_attr->m_rotateVSpeed, 
                                  deltaT );
                 setDirection();

                 prevTimeStamp    = event.timeStamp;
                 event.timeStamp += m_attr->m_rotateTimeDelta;
                 event.source     = getObjectID();

                 //{{PUT_EVENT(cn_EVCMD_ROTATE_AND_SHOOT)
                 event.data.open(EDO_WRITE)
                                .descend(cn_ROTATE , 0 )
                                  .putDouble(prevTimeStamp)
                                  .putDouble(hAngle)
                                  .putDouble(vAngle)
                                .ascend()
                                .descend( cn_SHOOT, 0 )
                                  .putInt(count)
                                  .putInt(bulletAttrIndex)
                                .ascend()
                           .close();
                 //}}END_OF_PUT_EVENT(cn_EVCMD_ROTATE_AND_SHOOT)
                 issueEvent( event );
            }

            }
            break;

    case cn_EVCMD_DIRECT_SHOOT:
     {
            double hAngle,vAngle;
            int    bulletAttr, count;
            //{{GET_EVENT(cn_EVCMD_DIRECT_SHOOT)
            event.data.open(EDO_READ)
                           .getDouble(hAngle)
                           .getDouble(vAngle)
                           .getInt(bulletAttr)
                           .getInt(count)
                      .close();
            //}}END_OF_GET_EVENT(cn_EVCMD_DIRECT_SHOOT)
            m_hAngle = hAngle;
            m_vAngle = vAngle;
            setDirection();
            shoot( bulletAttr, event.timeStamp );
     }
            break;

    case fu_EV_QUERY_ANGLE:
            event.data.open(EDO_WRITE)
                        .putDouble(m_vAngle)
                        .putDouble(m_hAngle)
                      .close();
            event.label = fu_EV_QUERY_ANGLE_OK;
            break;

    case KR_WAKE_UP:
            break;

    case KR_SET_ATTR:
            {
                //int attrIndex = 0;
                event.data.open(EDO_READ)
                            .getInt(m_attrIndex)
                          .close();
                 __attrTable.setAttribute(m_attrIndex,(ct_Attribute *&)m_attr);
            }
            break;

    default: return EVENTHANDLER( event );
    }
    return 1;
 }

 //============================================================
void Cannon::addNotify()
 {
    loadStateTransitionTable();
    ct_Subject::addNotify();
    // insert your code this
    resetState();
    startInitialize();
    setDirection();
    m_bulletTable = g_arena.searchSeanceClassTable( m_attr->m_bulletTable );

 }

 //============================================================
void Cannon::removeNotify()
 {
    ct_Subject::removeNotify();
    // insert your code this
 }

 //============================================================
#ifdef  __TRACE_NW__
void Cannon::draw( CDC &gc )
 {
    CFVector3 pos(queryMasterPos());
    pos += m_attr->m_offset;
    int x =(int)( pos.x),
        y =(int)(-pos.z);
 
    int dx =(int)( pos.x+m_vector.x*10.0),
        dy =(int)(-pos.z-m_vector.z*10.0);
    gc.MoveTo(x,y);
    gc.LineTo(dx,dy);
 }
#else
void Cannon::draw( CDC & ) {}
 
#endif

 //============================================================
CFVector3 Cannon::queryMasterPos() //FIXME передавать время?
 {
     IDynamicObject *dobj=(IDynamicObject *)
                          (context->queryInterface(m_cannonMaster,IDynamicObjectIID));
    return dobj->getPos();
 }

 //=======================================================================
void Cannon::shoot( int bulletAttrIndex, double timeStamp )
 {
    IDynamicObject *dobj=(IDynamicObject *)
                         (context->queryInterface(m_cannonMaster,IDynamicObjectIID));
    CFMatrix3x4 m;
    dobj->getMatrix(m);
    double localHAngle = calcLocalAngle( m, m_hAngle );

    CFVector3 axofs(m_attr->m_offset - m_attr->m_hAxis);
    double cos_A = cos(localHAngle), sin_A = sin(localHAngle);
    CFVector3 locPos( m_attr->m_hAxis.x + axofs.x*cos_A + axofs.z*sin_A,
                      m_attr->m_offset.y,
                      m_attr->m_hAxis.z + axofs.z*cos_A - axofs.x*sin_A);
//----------------------------
 
    KR_Event event;

    event.label       = b_EV_START;
    event.source      = getObjectID();
    event.destination = g_arena.newObject( m_bulletTable , "B" );

    if(  !event.destination.isNUL()  )
    {
         CFVector3 m_position( m*locPos );

         int attrIndex = bulletAttrIndex;
         double xVec = m_vector.x;
         double yVec = m_vector.y;
         double zVec = m_vector.z;


         //{{PUT_EVENT(b_EV_START)
         event.data.open(EDO_WRITE)
                        .descend( VECTOR3D_F, 0 )
                          .putDouble(m_position.x)
                          .putDouble(m_position.y)
                          .putDouble(m_position.z)
                        .ascend()
                        .descend( VECTOR3D_F, 0 )
                          .putDouble(xVec)
                          .putDouble(yVec)
                          .putDouble(zVec)
                        .ascend()
                        .putInt(attrIndex)
                        .putObjectID(m_cannonMaster)
                   .close();
         //}}END_OF_PUT_EVENT(b_EV_START)
         event.timeStamp = timeStamp;
         context->sendEventNow( event );
    }

    event.label       = cn_EV_I_HAVE_SHOOT;
    event.destination = m_cannonMaster;
    event.source      = getObjectID();
    context->sendEventNow( event );

/*
    dc_CreateCross(dc_CROSS_LINE,queryMasterPos(),event.timeStamp+10,
                    0,0, (int)(10*m_vector.x),(int)(-10*m_vector.z),
                    RGB(0,0,100),"",0,0);
  */
 }

 /*******************************
  *
  * Команда начинать стрельбу
  *
  *******************************/
void Cannon::from_STAY__to__SHOOTING__F( KR_Event &event )
{
    int bulletAttrIndex, count;
    //{{GET_EVENT(cn_EV_SHOOT)
    event.data.open(EDO_READ)
                   .getInt(count)
                   .getInt(bulletAttrIndex)
              .close();
    //}}END_OF_GET_EVENT(cn_EV_SHOOT)

    shoot(bulletAttrIndex,event.timeStamp);

    event.label  = cn_EV_SHOOT;
    event.source = getObjectID();
    //{{PUT_EVENT(cn_EV_SHOOT)
    event.data.open(EDO_WRITE)
                   .putInt(count)
                   .putInt(bulletAttrIndex)
              .close();
    //}}END_OF_PUT_EVENT(cn_EV_SHOOT)
    issueEvent( event );
}

void Cannon::from_SHOOTING__to__IDLE__F( KR_Event &event )
{
    int bulletAttrIndex, count;

    //{{GET_EVENT(cn_EV_SHOOT)
    event.data.open(EDO_READ)
                   .getInt(count)
                   .getInt(bulletAttrIndex)
              .close();
    //}}END_OF_GET_EVENT(cn_EV_SHOOT)

    count--;

    if(  count > 0  )
    {
         event.label      = cn_EV_IDLEOK;
         event.timeStamp += m_attr->m_idleTime;
         event.source     = getObjectID();
         //{{PUT_EVENT(cn_EV_IDLEOK)
         event.data.open(EDO_WRITE)
                        .putInt(bulletAttrIndex)
                        .putInt(count)
                   .close();
         //}}END_OF_PUT_EVENT(cn_EV_IDLEOK)
         issueEvent( event );
    }
    else
    {
         event.label  = cn_EV_ENDOFSHOOT;
         event.source = getObjectID();
         //{{PUT_EVENT(cn_EV_ENDOFSHOOT)
         //}}END_OF_PUT_EVENT(cn_EV_ENDOFSHOOT)
         issueEvent( event );
    }
}


void Cannon::from_STAY__to__SINGLEIDLE__F( KR_Event &event )
{
    int bulletAttrIndex;

    //{{GET_EVENT(cn_EV_SINGLE_SHOOT)
    event.data.open(EDO_READ)
                   .getInt(bulletAttrIndex)
              .close();
    //}}END_OF_GET_EVENT(cn_EV_SINGLE_SHOOT)
    shoot(bulletAttrIndex,event.timeStamp);

    event.label      = cn_EV_END_SHOOTING;
    event.timeStamp += m_attr->m_idleTime;
    event.source     = getObjectID();
    //{{PUT_EVENT(cn_EV_END_SHOOTING)
    //}}END_OF_PUT_EVENT(cn_EV_END_SHOOTING)
    issueEvent( event );
}



 /*******************************
  *
  * Приказ о создании танка
  *
  *******************************/
void Cannon::rebuldF( KR_Event &event )
{
    //{{GET_EVENT(tg_EV_REBUILD)
    //}}END_OF_GET_EVENT(tg_EV_REBUILD)
    m_cannonMaster = event.source;
}

 /*******************************
  *
  * Прекращаем стрельбу
  *
  *******************************/
void Cannon::from_AUTOIDLE__to__STAY__F( KR_Event & )
{
    //{{GET_EVENT(cn_EV_ENDOFSHOOT)
    //}}END_OF_GET_EVENT(cn_EV_ENDOFSHOOT)
}



 /*******************************
  *
  * Процесс перезаряжания кончился
  *
  *******************************/
void Cannon::repeatShootF( KR_Event &event )
{
    int bulletAttrIndex,count;

    //{{GET_EVENT(cn_EV_IDLEOK)
    event.data.open(EDO_READ)
                   .getInt(bulletAttrIndex)
                   .getInt(count)
              .close();
    //}}END_OF_GET_EVENT(cn_EV_IDLEOK)

    shoot(bulletAttrIndex,event.timeStamp);

    event.label  = cn_EV_SHOOT;
    event.source = getObjectID();
    //{{PUT_EVENT(cn_EV_SHOOT)
    event.data.open(EDO_WRITE)
                   .putInt(count)
                   .putInt(bulletAttrIndex)
              .close();
    //}}END_OF_PUT_EVENT(cn_EV_SHOOT)
    issueEvent( event );
}
/*
void Cannon::directShoot( KR_Event &event )
{
    double hAngle;
    int bulletAttr, count;
    //{{GET_EVENT(cn_EVCMD_DIRECT_SHOOT)
    event.data.open(EDO_READ)
                   .getDouble(hAngle)
                   .getDouble(vAngle)
                   .getInt(bulletAttr)
                   .getInt(count)
              .close();
    //}}END_OF_GET_EVENT(cn_EVCMD_DIRECT_SHOOT)
    m_hAngle = hAngle;
    shoot( bulletAttr, event.timeStamp );
}
*/
//{{ACTIONF_IMPLEMENTATION

void Cannon::loadStateTransitionTable()
 {
    //{{TRANSLATION_TABLE
    static KR_ActiveObject::StateTransitionTableElem row0[3] =
    {
        {tg_EV_REBUILD     , ST_NEW_STATE,  0, (ACTION)rebuldF                      },
        {cn_EV_SINGLE_SHOOT, ST_NEW_STATE,  3, (ACTION)from_STAY__to__SINGLEIDLE__F },
        {cn_EV_SHOOT       , ST_NEW_STATE,  1, (ACTION)from_STAY__to__SHOOTING__F   }
    };
    static KR_ActiveObject::StateTransitionTableElem row1[1] =
    {
        {cn_EV_SHOOT       , ST_NEW_STATE,  2, (ACTION)from_SHOOTING__to__IDLE__F   }
    };
    static KR_ActiveObject::StateTransitionTableElem row2[2] =
    {
        {cn_EV_ENDOFSHOOT  , ST_NEW_STATE,  0, (ACTION)from_AUTOIDLE__to__STAY__F   },
        {cn_EV_IDLEOK      , ST_NEW_STATE,  1, (ACTION)repeatShootF                 }
    };
    static KR_ActiveObject::StateTransitionTableElem row3[1] =
    {
        {cn_EV_END_SHOOTING, ST_NEW_STATE,  0, (ACTION)NULL                         }
    };
    //}}END_OF_TRANSLATION_TABLE{{
    static StateElem STT[4]={
      StateElem(3,row0),
      StateElem(1,row1),
      StateElem(2,row2),
      StateElem(1,row3)
    };
    m_stateTable = STT;
    m_stateQnty  = 4;
    //}}END_OF_STATE_TABLE
 }

CFVector3  Cannon::realPosition()
 {
    return CFVector3(1e10,1e10,1e10);
 }

 /*************************************
  *
  *   CannonTable implementation
  *
  *************************************/

 //============================================================
void CannonTable::allocObjects( int objectQnty )
 {
    m_table = new Cannon[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void CannonTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *CannonTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"CannonTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 /*************************************
  *
  *   AttributeTable implementation
  *
  *************************************/

 //============================================================
void AttributeTableCannon::allocObjects( int objectQnty )
 {
    m_table = new AttributeCannon[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void AttributeTableCannon::freeObjects()
 {
    delete [] m_table;
    m_table         = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *AttributeTableCannon::getObjectPTR( int index )
 {
    s_ASSERT(index>=0 && index <m_maxObjectQnty,"AttributeTable::getObjectPTR");
    return &(m_table[ index ]);
 }


   // ========== ICannon interface ============================


 //============================================================
void  Cannon::setLocalHAngle(double hAngle)
{
    m_localHAngle = hAngle;
}

 //============================================================
double Cannon::getLocalHAngle()
{
    return m_localHAngle;
}
 //============================================================
void Cannon::setLocalVAngle(double vAngle)
{
    m_localVAngle = vAngle;
}

 //============================================================
double Cannon::getLocalVAngle()
{
    return m_localVAngle;
}

 //============================================================
double Cannon::getHAngle()
{
    return m_hAngle;
}

 //============================================================
double Cannon::getVAngle()
{
    return m_vAngle;
}
  
 //============================================================
void Cannon::rotate( 
	                 double prevTimeStamp, 
					 double hAngle,
					 double vAngle
				   )
{
     context->removeEvent(cn_EVC_ROTATE,getObjectID());

     s_ASSERT(!IsNAN(hAngle),"Cannon::rotate Bad hangle");
     s_ASSERT(!IsNAN(vAngle),"Cannon::rotate Bad vangle");

     s_ASSERT(hAngle>=-M_PI && hAngle <= M_PI,"Cannon::rotate Bad hangle");
     s_ASSERT(vAngle>=-M_PI && vAngle <= M_PI,"Cannon::rotate Bad vangle");

    if(  fabs(hAngle-m_hAngle)+fabs(vAngle-m_vAngle) < m_attr->m_minStopAngle  )
    {
         m_hAngle = hAngle;
         m_vAngle = vAngle;
         setDirection();
    }
    else
    {
         KR_Event event;

         event.label      = cn_EVC_ROTATE;
         event.timeStamp  = prevTimeStamp;
         event.source     = getObjectID();
         event.destination= getObjectID();
         //{{PUT_EVENT(cn_EVC_ROTATE)
         event.data.open(EDO_WRITE)
                        .putDouble(prevTimeStamp)
                        .putDouble(hAngle)
                        .putDouble(vAngle)
                   .close();
         //}}END_OF_PUT_EVENT(cn_EVC_ROTATE)
         context->sendEventNow( event );
    }
}

 //============================================================
void Cannon::rotateAndShoot(
	                         double prevTimeStamp, 
						     double hAngle,
						     double vAngle,
						     int count,
						     int bulletAttrIndex
	                       )
{
   context->removeEvent(cn_EVCMD_ROTATE_AND_SHOOT,getObjectID());

   s_ASSERT(!IsNAN(hAngle),"Cannon::rotateAndShoot Bad hangle");
   s_ASSERT(!IsNAN(vAngle),"Cannon::rotateAndShoot Bad vangle");

   s_ASSERT(hAngle>=-M_PI && hAngle <= M_PI,"Cannon::rotateAndShoot Bad hangle");
   s_ASSERT(vAngle>=-M_PI && vAngle <= M_PI,"Cannon::rotateAndShoot Bad vangle");

  if(  fabs(hAngle-m_hAngle)+fabs(vAngle-m_vAngle) < m_attr->m_minStopAngle  )
  {
       m_hAngle = hAngle;
       m_vAngle = vAngle;
       setDirection();

       context->removeEvent( cn_EV_SHOOT, getObjectID() );

       KR_Event event;

       event.label       = cn_EV_SHOOT;
       event.timeStamp   = prevTimeStamp;
       event.source      = getObjectID();
       event.destination = getObjectID();

       //{{PUT_EVENT(cn_EV_SHOOT)
       event.data.open(EDO_WRITE)
                      .putInt(count)
                      .putInt(bulletAttrIndex)
                 .close();
       //}}END_OF_PUT_EVENT(cn_EV_SHOOT)

       issueEvent( event );
  }
  else
  {
       KR_Event event;

       event.label      = cn_EVCMD_ROTATE_AND_SHOOT;
       event.timeStamp  = prevTimeStamp;
       event.source     = getObjectID();
       event.destination= getObjectID();

       //{{PUT_EVENT(cn_EVCMD_ROTATE_AND_SHOOT)
       event.data.open(EDO_WRITE)
                      .descend(cn_ROTATE , 0 )
                        .putDouble(prevTimeStamp)
                        .putDouble(hAngle)
                        .putDouble(vAngle)
                      .ascend()
                      .descend( cn_SHOOT, 0 )
                        .putInt(count)
                        .putInt(bulletAttrIndex)
                      .ascend()
                 .close();
       //}}END_OF_PUT_EVENT(cn_EVCMD_ROTATE_AND_SHOOT)
       issueEvent( event );
  }
}

int Cannon::isFixed()
{
   return m_attr->m_fixed;
}

int  AttributeCannon::receiveEvent( KR_Event &event )
{
    if(  !ct_Attribute::receiveEvent(event)  )
    switch( event.label )
    {
    case sk_EV_PROG:
     {
        char par[100];
        double val;

        event.data.open(EDO_READ)
                    .getStr(par,sizeof(par))
                    .getDouble(val)
                  .close();
        if(  strcmp(par,"ofsx")==0  )
             m_offset.x = val;
        else
        if(  strcmp(par,"ofsy")==0  )
             m_offset.y = val;
        else
        if(  strcmp(par,"ofsz")==0  )
             m_offset.z = val;
        else s_ASSERTNQ1("AttributeCannon::receiveEvent: Unknown par%s",par);
     }
     break;

     default: 
         return 0;
    }
    return 1;
}


void AttributeCannon::update(double )
{
}


bool	Cannon::dump(PIN_SaveFile & sf)
{
		if (!ct_Subject::dump(sf) ||
		    !KR_ActiveObject::dump(sf) ||                	
		    !sf.WriteData( (char *) & m_attrIndex, sizeof(CannonData)  ))
			return false;

		return true;
}

bool	Cannon::load(PIN_SaveFile & sf)
{
		if (!ct_Subject::load(sf) ||
  		    !KR_ActiveObject::load(sf) ||
		    !sf.GetData( (char *) & m_attrIndex, sizeof(CannonData)  ))
			return false;
		
		return true;
}

void	Cannon::loadNotify()
{
	ct_Subject::loadNotify(); 
	loadStateTransitionTable();
	__attrTable.setAttribute(m_attrIndex,(ct_Attribute *&)m_attr);
}

/* End of file D:\GAME\OBASE\Cannon\Cannon.cpp */