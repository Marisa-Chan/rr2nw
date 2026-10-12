/*
 * File  : C:\WinGame\OBASE\People\People.cpp
 * Autor :
 * Ver   1.0
 */
#include "People.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "kernel/h/session.h"

#include "message/skinmsg.h"
#include "message/peopmsg.h"
#include "message/bulmsg.h"
#include "message/sndmsg.h"
#include "message/unitmsg.h"

#include "enum/spaceEnum.h"
#include "i/route.i"
#include "h/phisics.h"
#include "h/vehicle.h"
#include "zav.h"
#include "Sound.h"
#include "obase/sound/wavobj.h"

static void AnimateCallBack0(CViewObjectBaseSet *,CViewObjectBase *pBase,CViewObjectRef *ref);
static void AnimateCallBack2(CViewObjectBaseSet *,CViewObjectBase *pBase,CViewObjectRef *ref);

enum
{
    NO_LAND  = 0,
    ON_LAND  = 1,
    ON_WATER = 2,
    ON_OBJ   = 3
};


void shoot( KR_ObjectID fromID,
            int bulletTable, int attrIndex,
            const CFVector3 &pos, const CFVector3 &dir,
            SimulationContext *context, double ts );


 //===========================================================================
class AttributePeople : public ct_Attribute
{
 public:
    CViewObjectModel*   m_cacheSkin              ;  //
    KR_ObjectID		m_skinID;
    virtual void        update(double ts);
    int                 m_bulletIndex;


    WAVObj            * m_wav;
    ct_ClassTableID     m_ctsndID;
    ct_ClassTableID     m_cacheExplosionTable;
    int                 m_cacheExplAttr;
    ct_ClassTableID     m_smokeTableID;
    KR_ObjectID         m_smokeAttrID;

    ct_ClassTableID     m_cacheCorpseTable;
    int                 m_cacheCorpseAttr;
    ct_ClassTableID     m_bulletTable;
    CFVector3           m_cannonOfs;


//{{ATTRIBUTE
    ct_AttrItem  m_array[24];
    ct_AttrStr      m_routeName              ;  // 
    double          m_timeInc                ;  // 
    int             m_onLand                 ;  // 
    ct_AttrStr      m_skinName               ;  // 
    double          m_speed                  ;  // 
    double          m_addRoll                ;  // 
    double          m_rollSpeed              ;  // 
    double          m_centerHeight           ;  // 
    ct_AttrStr      m_soundName              ;  // 
    ct_AttrStr      m_explAttrName           ;  // 
    ct_AttrStr      m_smokeAttrName          ;  // 
    ct_AttrStr      m_corpseAttrName         ;  // 
    double          m_initialDamage          ;  // 
    ct_AttrStr      m_bulletAttrName         ;  // 
    double          m_cannonSpeed            ;  // 
    double          m_cannonX                ;  // 
    double          m_cannonY                ;  // 
    double          m_cannonZ                ;  // 
    double          m_maxOutDist             ;  // Расстояние, на которое можем отойти
    int             m_useRndNode             ;  // 
    int             m_stopIfAttack           ;  // 
    int             m_burstCount             ;  // Количество пуль в выпускаемой очереди
    double          m_deltaZeroSpeed         ;  // 
    double          m_maxDeep                ;  // 

    AttributePeople()
    {
        strncpy(m_routeName,"", sizeof( ct_AttrStr )-1 );
        m_timeInc            = 0.04;
        m_onLand             = 1;
        strncpy(m_skinName,"", sizeof( ct_AttrStr )-1 );
        m_speed              = 10.0;
        m_addRoll            = 0;
        m_rollSpeed          = 3;
        m_centerHeight       = 0;
        strncpy(m_soundName,"", sizeof( ct_AttrStr )-1 );
        strncpy(m_explAttrName,"Unknown", sizeof( ct_AttrStr )-1 );
        strncpy(m_smokeAttrName,"Unknown", sizeof( ct_AttrStr )-1 );
        strncpy(m_corpseAttrName,"Corpse.Attr.Default", sizeof( ct_AttrStr )-1 );
        m_initialDamage      = 1.0;
        strncpy(m_bulletAttrName,"", sizeof( ct_AttrStr )-1 );
        m_cannonSpeed        = 0.8;
        m_cannonX            = 0;
        m_cannonY            = 1.4;
        m_cannonZ            = 0;
        m_maxOutDist         = 1e3;
        m_useRndNode         = 0;
        m_stopIfAttack       = 0;
        m_burstCount         = 5;
        m_deltaZeroSpeed     = 0.9;
        m_maxDeep            = 2.0;

        m_array[0].set("m_routeName",m_routeName);
        m_array[1].set("m_timeInc",m_timeInc);
        m_array[2].set("m_onLand",m_onLand);
        m_array[3].set("m_skinName",m_skinName);
        m_array[4].set("m_speed",m_speed);
        m_array[5].set("m_addRoll",m_addRoll);
        m_array[6].set("m_rollSpeed",m_rollSpeed);
        m_array[7].set("m_centerHeight",m_centerHeight);
        m_array[8].set("m_soundName",m_soundName);
        m_array[9].set("m_explAttrName",m_explAttrName);
        m_array[10].set("m_smokeAttrName",m_smokeAttrName);
        m_array[11].set("m_corpseAttrName",m_corpseAttrName);
        m_array[12].set("m_initialDamage",m_initialDamage);
        m_array[13].set("m_bulletAttrName",m_bulletAttrName);
        m_array[14].set("m_cannonSpeed",m_cannonSpeed);
        m_array[15].set("m_cannonX",m_cannonX);
        m_array[16].set("m_cannonY",m_cannonY);
        m_array[17].set("m_cannonZ",m_cannonZ);
        m_array[18].set("m_maxOutDist",m_maxOutDist);
        m_array[19].set("m_useRndNode",m_useRndNode);
        m_array[20].set("m_stopIfAttack",m_stopIfAttack);
        m_array[21].set("m_burstCount",m_burstCount);
        m_array[22].set("m_deltaZeroSpeed",m_deltaZeroSpeed);
        m_array[23].set("m_maxDeep",m_maxDeep);

        linkTable(m_array,24);
    }
//}}END_OF_ATTRIBUTE
};

//static AttributePeople __defaultAttr;

 //===========================================================================
class PeopleTable : public ct_SubjectTable
{
 private:
    People *m_table;
 public:
    PeopleTable()
    {
        m_table = NULL;
        registerClass( "People" );
        m_defaultAddMode = CT_KILLINVISIBLE;
    }
    ~PeopleTable()
    {
        delete [] m_table;
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
    virtual bool       isRendering();
    virtual bool       isAudible();

};


bool PeopleTable::isRendering()
{
   return true;
}

bool PeopleTable::isAudible()
{
   return true;
}

 //===========================================================================
class AttributeTablePeople : public ct_AttributeTable
{
 protected:
    AttributePeople *m_table;

 public:
    AttributeTablePeople()
    {
       m_table = NULL;
       registerClass( "PeopleAttr" );
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static PeopleTable  __classTable;
static AttributeTablePeople __attrTable;
 /*********************************
  *
  *   People implementation
  *
  *********************************/

inline void People::setHAngle( double hAngle )
{
    m_hAngle   = hAngle;
    m_rotateOy = m_attr->m_addRoll-M_PI/2-m_hAngle;
}

inline void People::setHAngleFixDir( double hAngle )
{
    m_hAngle   = hAngle;
    m_rotateOy = m_attr->m_addRoll-M_PI/2-m_hAngle;
    m_dir.x    = cos(m_hAngle)*m_attr->m_speed;
    m_dir.z    = sin(m_hAngle)*m_attr->m_speed;
}


 //============================================================
People::People()
    : m_viewDynObj(m_skin)
 {
    m_attr = 0;//&__defaultAttr;
 }

 //============================================================
People::~People()
 {
 }


//==============================================================
void People::attackAllways(KR_Event &event, const CFVector3 &pos)
{
   m_nextNode = pos;
   event.timeStamp += 0.1;
}


//==============================================================
void People::nexNodeDefault(KR_Event &event)
{
    CFVector3 currentNode = m_nextNode;
    double    dist;
    CFVector3 pos(getPosition());

    switch( m_attr->m_onLand )
    {
    case NO_LAND:   dist = Abs( currentNode-pos ); break;
    case ON_LAND:
    case ON_WATER:
    case ON_OBJ:    dist = hypot(currentNode.z-pos.z, currentNode.x-pos.x ); break;
    default: echo("People::Warning: unknown m_onLand");
    }

    if(  dist*(1.0+m_attr->m_rollSpeed)/(3.0+m_attr->m_speed/3.0)
       > 7.0  )
    {
         event.timeStamp += dist/m_attr->m_speed;
         issueEvent( event );
         return;
    }

    IRouteObject *ri = (IRouteObject*)(context->queryInterface(m_routeID,IRouteObjectIID));

    m_curNode = (m_curNode+1)%ri->GetNodeCnt();
    int nnn   = (m_curNode+1)%ri->GetNodeCnt();

    CFVector3 nextNode( ri->GetNode(nnn) );
    m_nextNode = nextNode;


    switch( m_attr->m_onLand )
    {
    case NO_LAND:   dist = Abs( nextNode-pos ); break;
    case ON_LAND:
    case ON_WATER:
    case ON_OBJ:    dist = hypot(nextNode.z-pos.z, nextNode.x-pos.x ); break;
    default: echo("People::Warning: unknown m_onLand");
    }

    event.timeStamp += dist/m_attr->m_speed;
    issueEvent( event );
}


//==============================================================

void RotateVertical(
                     CFVector3       &v,
                     double           angle,
                     const CFVector3 &mpos,
                     CFVector3       &nextNode
                   )
{
    double hang  = atan2( v.x, v.z  );
    double len   = Abs( v );
    double cos_a = cos(angle)*len,
           sin_a = sin(angle)*len;
    v = CFVector3( cos_a*sin(hang), sin_a, cos_a*cos(hang) );
    nextNode = mpos + v;
}


//==============================================================
void People::manoeuvre(KR_Event &event,
                    const CFVector3 &/* pos */,
                    const CFVector3 &mpos
                   )
{
     double           deltaT  = 0.8+context->rnd_f(2.8);
     double           chkT    = 0.7;
     double           clzTime = 10;
     KR_ObjectID      oID;
     CFVector3        movedir = m_nextNode-mpos;

     if(  checkCollision(
                                mpos,    // начало движени
                                Normal(movedir)*m_attr->m_speed,            // напрвление со скоростью
                                20.0,      // радиус
                                deltaT+chkT,     // время для проверки
                                getObjectID(),     // кого игнорировать
                                clzTime, // время, через которое стукнемс
                                oID      // объект, о который стукнемс
                              ) )
     {
         if(  clzTime < deltaT  )
         {
              //
              // too near
              //
              if(  clzTime < chkT  )
              {
                   if(  m_nextNode.y > 150 && (!oID.isNUL()) )
                        RotateVertical(movedir,-30*M_PI/180, mpos, m_nextNode); //m_nextNode.y -= 20;
                   else RotateVertical(movedir,+30*M_PI/180, mpos, m_nextNode); //m_nextNode.y += 20;
                   deltaT = clzTime-chkT/2;
                   if(  deltaT<0.05  ) deltaT = 0.05;
              }
              else
              {
                   deltaT = clzTime-(chkT-0.05);
                   if(  deltaT<0.05  ) deltaT = 0.05;
                   if(  m_nextNode.y > 170 && (!oID.isNUL())  )
                        RotateVertical(movedir,-20*M_PI/180, mpos, m_nextNode);//m_nextNode.y -= 10;
                   else RotateVertical(movedir,+20*M_PI/180, mpos, m_nextNode);//m_nextNode.y += 10;
              }
         }
         else RotateVertical(movedir,+20*M_PI/180, mpos, m_nextNode);//m_nextNode.y += 10;
     }

     event.timeStamp += deltaT;
}


//==============================================================
void People::nexNodeAttackFar(KR_Event &event,
                              const CFVector3 &pos,
                              const CFVector3 &mpos,
                              IDynamicObject *dobj
                             )
{
    //echo("far");
    if(  m_attr->m_stopIfAttack )
         attackAllways( event, pos );
    else
    {
         CFVector3 newDir(m_nextNode-pos);
         m_nextNode = pos + dobj->getMoveDir()*dobj->getMoveSpeed()*0.4+CFVector3(0,0.2,0);
         double len2 = Abs2(newDir);

         if(  len2 > m_attr->m_maxOutDist*m_attr->m_maxOutDist  )
              m_nextNode = (m_attr->m_maxOutDist/sqrt(len2))*newDir;

         manoeuvre(  event, pos, mpos );
    }
}


//==============================================================
void People::nexNodeAttackNear(KR_Event &event,
                              const CFVector3 &pos,
                              const CFVector3 &mpos,
                              IDynamicObject *dobj
                             )
{
    //echo("near");
    if(  m_attr->m_stopIfAttack  )
         attackAllways( event, pos );
    else
    {
         double range = 95;
         m_nextNode = pos-dobj->getMoveDir()*20
                              +CFVector3(context->rnd_f(-range,range),
                               context->rnd_f(-10,5+25*(mpos.y<180)),
                               context->rnd_f(-range,range));

         double len2 = Abs2(m_nextNode-mpos);

         if(  len2 > m_attr->m_maxOutDist*m_attr->m_maxOutDist  )
              m_nextNode = (m_attr->m_maxOutDist/(1+sqrt(len2)))*(m_nextNode-mpos);


         manoeuvre(  event, pos, mpos );
    }
}



void People::setPeopleAttr()
{
	ct_Attribute *attr = __attrTable.searchAttribute(m_peopleAttrID);
	if( attr==NULL )
		echo( "People::receiveEvent: Unknown attribute %s",
		context->searchObject(m_peopleAttrID));
	else
		m_attr = (AttributePeople*)attr;


	updateSound( getObjectID(),
		context,
		m_attr->m_ctsndID,
		m_attr->m_wav,
		m_snd );



	m_skin.Attach(m_attr->m_cacheSkin);
	m_viewDynObj.BumpDef().fRadius = m_skin.Model()->Radius();
	m_askin = (ISkin*)(context->queryInterface(m_attr->m_skinID, ISkinIID));

	m_skin.SetUserAttrib(this);
	if(  strcmp(m_attr->m_skinName,"sk.Peop.shiz_f0a")==0 )
		m_skin.SetAnimationCallback(AnimateCallBack0);
	else
//	if(  strcmp(m_attr->m_skinName,"sk.Taxi.cln_f01")==0 )
//		m_skin.SetAnimationCallback(AnimateCallBack1);
//	else
	if(  strcmp(m_attr->m_skinName,"sk.Taxi.cln_f08")==0 )
		m_skin.SetAnimationCallback(AnimateCallBack2);
}

//==========================================================================
void People::crashExpl( CFVector3 pos, double time )
{
    createExplosion(
	             pos,
	             time,
	             getObjectID(),
	             m_attr->m_cacheExplosionTable,
	             m_attr->m_cacheExplAttr
	           );
}

//==========================================================================
void People::crashCreateCorpse( CFVector3 pos, double time )
{
    // ¬л ­  §Ґ¬«Ґ, б®§¤ вм вагЇ
    // Ґб«Ё ¬л ­  §Ґ¬«Ґ   ­Ґ ­  ¤Ё­ ¬ЁзҐбЄ®¬ ®ЎкҐЄвҐ
    createCorpse( pos,
                  time,
     	          getObjectID(),
                  m_attr->m_cacheCorpseTable,
                  m_attr->m_cacheCorpseAttr
                );
}

//==========================================================================
void People::moveKilled_FALLOW_checkClz( KR_Event &event )
{
    double clzTime;
    KR_ObjectID oID;

    if(  !m_deleted  )
    if(  checkCollision(
                    getPosition(),    // ­ з «® ¤ўЁ¦Ґ­Ёп
                    m_dir,            // ­ Їаў«Ґ­ЁҐ б® бЄ®а®бвмо
                    getRadius(),      // а ¤Ёгб
                    m_calcPosInc,     // ўаҐ¬п ¤«п Їа®ўҐаЄЁ
                    getObjectID(),    // Є®Ј® ЁЈ­®аЁа®ў вм
                    clzTime,          // ўаҐ¬п, зҐаҐ§ Є®в®а®Ґ бвгЄ­Ґ¬бп
                    oID      // ®ЎкҐЄв, ® Є®в®ал© бвгЄ­Ґ¬бп
                ) )
    {
         CFVector3 crashPos(getPosition()+m_dir*clzTime);

         crashExpl( crashPos, m_prevTime+clzTime );

         if(  m_attr->m_cacheCorpseAttr != -1  &&  oID.isNUL()  )
                  // ¬л ­  §Ґ¬«Ґ, б®§¤ вм вагЇ
                  // Ґб«Ё ¬л ­  §Ґ¬«Ґ   ­Ґ ­  ¤Ё­ ¬ЁзҐбЄ®¬ ®ЎкҐЄвҐ
                  crashCreateCorpse( crashPos, m_prevTime+clzTime );

         if(  !m_deleted  )
              g_arena.delObject(getObjectID(), m_prevTime+clzTime+0.1 );
         m_deleted = 1;
    }
    else
    {
         event.timeStamp += m_calcPosInc;
         issueEvent( event );
    }
}

//==========================================================================
void People::moveKilled_FALLOW_1( KR_Event &event, double deltaT )
{
    m_dir = m_dir*(1-deltaT)+CFVector3(0,-34,0)*deltaT;
    ct_Subject::setPosition(getPosition()+m_dir*deltaT);

    m_kill_deltaVAngle += 0.3*deltaT;
    if(  m_kill_deltaVAngle > 1  )
         m_kill_deltaVAngle = 1;

    m_rotateOx = m_kill_deltaVAngle*sin(m_kill_startPhase-(m_prevTime-m_kill_time)*4);
    moveKilled_FALLOW_checkClz( event );
}

//==========================================================================
void People::moveKilled_FALLOW_2( KR_Event &event, double deltaT )
{
    m_dir += CFVector3(0,-2,0)*deltaT;
    ct_Subject::setPosition(getPosition()+m_dir*deltaT);

    m_kill_deltaVAngle += 0.3*deltaT;

    m_rotateOz = m_kill_deltaVAngle;

    createSmoke(
                 getPosition(),
                 getObjectID(),
                 m_prevTime,
                 m_attr->m_smokeTableID,
                 m_attr->m_smokeAttrID
               );

     moveKilled_FALLOW_checkClz( event );
}

//==========================================================================
void People::moveKilled_FALLOW_SINK( KR_Event &event, double deltaT )
{
    m_dir = m_dir*(1-deltaT)+CFVector3(0,-1,0)*deltaT;
    ct_Subject::setPosition(getPosition()+m_dir*deltaT);

    m_kill_deltaVAngle += 0.15*deltaT;
    if(  m_kill_deltaVAngle > 0.7  )
         m_kill_deltaVAngle = 0.7;

    m_rotateOx = m_kill_deltaVAngle*sin(m_kill_startPhase-(m_prevTime-m_kill_time)*0.5);
    double clzTime;
    KR_ObjectID oID;

    if( checkCollision(
		          getPosition(),    // ­ з «® ¤ўЁ¦Ґ­Ёп
		          m_dir,            // ­ Їаў«Ґ­ЁҐ б® бЄ®а®бвмо
		          getRadius(),      // а ¤Ёгб
		          m_calcPosInc,     // ўаҐ¬п ¤«п Їа®ўҐаЄЁ
		          getObjectID(),     // Є®Ј® ЁЈ­®аЁа®ў вм
		          clzTime, // ўаҐ¬п, зҐаҐ§ Є®в®а®Ґ бвгЄ­Ґ¬бп
		          oID      // ®ЎкҐЄв, ® Є®в®ал© бвгЄ­Ґ¬бп
		          ))
         context->removeObject( getObjectID() );
    else
    {
         event.timeStamp += m_calcPosInc;
         issueEvent( event );
    }
}

 //============================================================
void People::moveKilled( KR_Event &event, double deltaT )
{
    switch( m_killed )
    {
    case KILL_FALLOW_1: // бЎЁвл© ¤ЁаЁ¦ Ў«м
          moveKilled_FALLOW_1( event, deltaT );
          return;

    case KILL_FALLOW_2: // бЎЁвл© б ¬®«Ґв
          moveKilled_FALLOW_2( event, deltaT );
          return;

    case KILL_SINK: // в®­Ґв Є®а Ў«ЁЄ
          moveKilled_FALLOW_SINK( event, deltaT );
          return;

    case KILL_NONE: 
          context->removeObject( getObjectID() );
          return;
    }
}


 //============================================================
int People::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case pe_EVC_MOVE:
     {
        double deltaT = event.timeStamp-m_prevTime;

        m_lastMovePos       = getPosition();
        m_lastMoveTimeStamp = event.timeStamp;
        m_lastMoveDeltaT    = deltaT;

        m_prevTime = event.timeStamp;

        if(  m_killed!=KILL_NONE   )
        {
             moveKilled( event, deltaT );
             return 1;
        }

        { //============== MOVING ===================================

             CFVector3 nextNode( m_nextNode );

             switch( m_attr->m_onLand )
             {
             case ON_LAND:
              {
                  CFVector3 pos(getPosition());
                  double step = deltaT;
                  if(  m_stoped  && (getState() == pe_STATE_ATTACK)  )
                       step = 0;
                  else
                  if(  m_attr->m_stopIfAttack
                    && Normal(CFVector3(m_dir.x,0,m_dir.z))*Normal(CFVector3(nextNode.x-pos.x,
                                                      0,
                                                      nextNode.z-pos.z)) < m_attr->m_deltaZeroSpeed  )
                       step = 0;

                  setPosition(getPosition()+m_dir*step);
                  pos = getPosition();
                  double hAngle = atan2( nextNode.z-pos.z, nextNode.x-pos.x );
                  if( deltaT < 0.5 )
                  {
                      setHAngleFixDir(
                                interpolateAngle( m_hAngle, hAngle,
                                                 m_attr->m_rollSpeed,
                                                 deltaT ));

                  }
                  m_rotateOx = 0;

                  double height;
                  CFVector3 normal;
                  CViewScene::Current()->GetTerrain()->GetPlane(pos,normal,height);
                  pos.y = height;
                  setPosition(pos);
              }
              break;

             case ON_WATER:
              {
                  double step = deltaT;
                  CFVector3 pos(getPosition());

                  if(  m_stoped  && (getState() == pe_STATE_ATTACK)  )
                       step = 0;
                  else
                  if(  m_attr->m_stopIfAttack 
                    && Normal(m_dir)*Normal(nextNode-pos) < m_attr->m_deltaZeroSpeed  )
                       step = 0;

                  setPosition(getPosition()+m_dir*step);
                  pos = getPosition();
                  
                  //m_pos += m_dir*deltaT;

                  double hAngle = atan2( nextNode.z-pos.z, nextNode.x-pos.x );
                  if( deltaT < 0.5 )
                      setHAngle(
                                  interpolateAngle( m_hAngle, hAngle,
                                                 m_attr->m_rollSpeed,
                                                 deltaT ));
                  
                  m_dir.x = cos(m_hAngle)*m_attr->m_speed;
                  m_dir.z = sin(m_hAngle)*m_attr->m_speed;
                  m_rotateOx = 0;
            
                  pos.y = ZAV_Scene()->GetTerrain()->Waterline();
                  double height;
                  CFVector3 normal;
                  CViewScene::Current()->GetTerrain()->GetPlane(pos,normal,height);
                  if(  pos.y-height >m_attr->m_maxDeep  )
                       setPosition(pos);
              }
              break;

             case ON_OBJ:
              {
                  double step = deltaT;
                  CFVector3 pos(getPosition());

                  if(  m_stoped  &&  (getState() == pe_STATE_ATTACK)  )
                       step = 0;
                  else
                  if(  m_attr->m_stopIfAttack 
                    && Normal(m_dir)*Normal(nextNode-pos) < m_attr->m_deltaZeroSpeed  )
                       step = 0;

                  pos = getPosition()+m_dir*step;

                  double hAngle = atan2( nextNode.z-pos.z, nextNode.x-pos.x );
                  if( deltaT < 0.5 )
                      if(  !m_isClz  )
                      setHAngleFixDir( 
                                  interpolateAngle( m_hAngle, hAngle,
                                                 m_attr->m_rollSpeed,
                                                 deltaT ));
                  


                  {
                   m_isClz = 0;

                   double      tm, tm2;
                   KR_ObjectID oID,oID2;
                   double radius = getRadius();
                   double cos_oy = cos(m_rotateOy)*radius;
                   double sin_oy = sin(m_rotateOy)*radius;
                   
                   if( checkCollision( 
                     pos+CFVector3(sin_oy,3,cos_oy),    // начало движения
                     CFVector3(0,-1,0),    // напрвление со скоростью
                     1, // радиус
                     50,  // время для проверки
                     getObjectID(),  // кого игнорировать
                     tm, // время, через которое стукнемся
                     oID      // объект, о который стукнемся
                   ) ) 
                   {
                       double height = 3-tm-1 + pos.y;
                       if(  !oID.isNUL()  )
                       {
                            setHAngleFixDir( interpolateAngle( m_hAngle, m_hAngle - 10*M_PI/180,
                                                 m_attr->m_rollSpeed,
                                                 deltaT ) );
                            m_isClz = 1;
                       }

                       if( checkCollision( 
                         pos+CFVector3(-sin_oy,3,-cos_oy),    // начало движения
                         CFVector3(0,-1,0),    // напрвление со скоростью
                         1, // радиус
                         50,  // время для проверки
                         getObjectID(),  // кого игнорировать
                         tm2,            // время, через которое стукнемся
                         oID2            // объект, о который стукнемся
                       ) ) 
                       {
                            if(  !oID2.isNUL()  )
                            {
                                 setHAngleFixDir( interpolateAngle( m_hAngle, m_hAngle + 10*M_PI/180,
                                                 m_attr->m_rollSpeed,
                                                 deltaT ) );
                                 m_isClz = 1;
                            }
                            double height2 = 3-tm2-1+pos.y;
                            double a = atan2(height2-height,2*radius);

                            m_rotateOx = a;     
                            pos.y = (height+height2)*0.5;

                            if(  oID2.isNUL() )
                                 setPosition(pos);
                       }
                       else
                       {
                            setHAngleFixDir( interpolateAngle( m_hAngle, m_hAngle + 10*M_PI/180,
                                             m_attr->m_rollSpeed,
                                             deltaT ) );
                            m_isClz = 1;
                       }
                   }
                   else
                   {
                       setHAngleFixDir( interpolateAngle( m_hAngle, m_hAngle + 10*M_PI/180,
                                        m_attr->m_rollSpeed,
                                        deltaT ) );
                       m_isClz = 1;
                   }
                  }
            
              }
              break;

             case NO_LAND:
              {
                  double step = deltaT;
                  CFVector3 pos(getPosition());

                  if(  m_stoped && (getState() == pe_STATE_ATTACK)  )
                       step = 0;
                  else
                  if(  m_attr->m_stopIfAttack 
                    && Normal(m_dir)*Normal(nextNode-pos) < m_attr->m_deltaZeroSpeed  )
                       step = 0;
                  setPosition(getPosition()+m_dir*step);
                  pos = getPosition();

                  double dist2D = hypot( nextNode.x-pos.x, nextNode.z-pos.z );
                  double hAngle = atan2( nextNode.z-pos.z, nextNode.x-pos.x );
                  double vAngle = atan2( nextNode.y-pos.y, dist2D );

                  if( deltaT < 0.5 )
                  {
                      setHAngle( interpolateAngle( m_hAngle, hAngle,
                                                 m_attr->m_rollSpeed,
                                                 deltaT ));
                  
                      vAngle = interpolateAngle( m_rotateOx,vAngle, m_attr->m_rollSpeed,
                                                 deltaT);
                  }

                  m_rotateOx = vAngle;

                  double co = cos(m_rotateOx)*m_attr->m_speed;
                  m_dir.x = cos(m_hAngle)*co;
                  m_dir.z = sin(m_hAngle)*co;
                  m_dir.y = sin(m_rotateOx)*m_attr->m_speed;
              }
             }

             if(  getState()==pe_STATE_ATTACK  && 
                  event.timeStamp-m_prevShootTime > m_attr->m_cannonSpeed &&
                  m_attr->m_bulletIndex != -1
               )
                 onShoot(event.timeStamp);
        }
        
        if(  m_isVisible  )
        {
             double haze = CViewFigure::HazeMax();
             event.timeStamp += m_calcPosInc*(Abs(g_vehicle->getPos()-getPosition())+haze*0.2)/haze;
        }
        else event.timeStamp += m_calcPosInc*2;
        issueEvent( event );

        // Sound
         if(  !m_snd.isNUL() && m_audibleThisFrame)
         {
          CFVector3 pos(getPosition());
          event.label = snd_EV_MOVE_TO;
          event.destination = m_snd;
          event.source      = getObjectID();
          event.data.open(EDO_WRITE)
                     .putDouble(pos.x)
                     .putDouble(pos.y)
                     .putDouble(pos.z)
                   .close();
          context->sendEventNow( event );
         }

     }
         break;


    case pe_EVC_NEXTNODE:
     {
         double ts = event.timeStamp;
         for(;;)
         {
         switch( getState() )
         {
         case pe_STATE_DEFAULT: nexNodeDefault(event); break;
         case pe_STATE_ATTACK:
          {
            IDynamicObject *dobj = (IDynamicObject*)(context->queryInterface(getEnemyID(),IDynamicObjectIID));
            if(  dobj  )
            {
                 CFVector3 pos(dobj->getPos());
                 CFVector3 mpos(getPosition());

                 if(  Abs2(pos-mpos) > 35*35 )
                      nexNodeAttackFar (event, pos, mpos, dobj );
                 else nexNodeAttackNear(event, pos, mpos, dobj );

                 issueEvent( event );
            }
            else
            {
                 popState();
                 continue;
            }
          }
          break;
         }
         break;
         } // for(;;)
     }
     m_stoped = m_attr->m_stopIfAttack && (getState()==pe_STATE_ATTACK);
     break;

    case pe_EVC_FIND_ENEMY:
         {
             event.timeStamp += 2.0+(!m_isVisible)*5;
             issueEvent(event);
             
             if(  getState() == pe_STATE_DEFAULT  )
             {
                  ct_SubjectFindData fsd;
                  KR_ObjectID        eID = KR_ObjectID::NUL();
                  double             K   = -1e3;
                  double xMin,xMax, zMin, zMax;
                  CFVector3 pos(realPosition());
                  xMin = xMax = pos.x;
                  zMin = zMax = pos.z;
                  xMin -= 300; xMax += 300;
                  zMin -= 300; zMax += 300;

                  ct_Arena::findFirstSubject( fsd, xMin, zMin, xMax, zMax );

                  for( int i = 0; i < fsd.getCount(); ++i )
                  {
                       const KR_ObjectID  &enemyID = fsd[i];
                       IUnit *u =(IUnit *)(context->queryInterface( enemyID, IUnitIID ));

                       if(  u==0  )
                            continue;


                       if(  !u->isFriend(m_commanderID)  )
                       {
                             double     power = u->getPower();
                             IDynamicObject *dobj = (IDynamicObject *)
                             (context->queryInterface( enemyID, IDynamicObjectIID ));

                            if(  dobj != 0  )
                            {

                                 CFVector3 pos(dobj->getPos());
                                 double curK = power/(1+Abs(CFVector2(m_position.x,m_position.z)-CFVector2(pos.x,pos.z)));
                                 if(  curK > K  )
                                 {
                                      K   = curK;
                                      eID = enemyID;
                                 }
                           }
                       }
                  }

                  if(  !eID.isNUL()  )
                  {
                       pushState(pe_STATE_ATTACK,eID);
                  }
                  else popState();
             }


         }
         break;

    case pe_EV_STARTSHOW:
         m_isNotCreate = 0;
         break;

    case pe_EVCMD_START:
     {
         //KR_ObjectID attrID;
         char        route[50];
         double      timeStart = 0;

         m_lastMoveTimeStamp = event.timeStamp;
         m_lastMoveDeltaT    = 0;

         event.data.open(EDO_READ)
                     .getObjectID(m_peopleAttrID)
                     .getStr     (route,sizeof(route)-1)
                     .getDouble  (timeStart)
                   .close();
         double  viewTime = event.timeStamp+timeStart+context->rnd_f(0.1);

	 setPeopleAttr();

         /*ct_Attribute *attr = __attrTable.searchAttribute(attrID);
         if( attr==NULL )
              echo( "People::receiveEvent: Unknown attribute %s",
                    context->searchObject(attrID));
         else m_attr = (AttributePeople*)attr;*/


		m_damage = m_attr->m_initialDamage;


	/*updateSound( getObjectID(),
	    	     context,
		     m_attr->m_ctsndID,
		     m_attr->m_wav,
		     m_snd );



        m_skin.Attach(m_attr->m_cacheSkin);
        m_viewDynObj.BumpDef().fRadius = m_skin.Model()->Radius();
        m_askin = (ISkin*)(context->queryInterface(m_attr->m_skinID, ISkinIID));
        
        m_skin.SetUserAttrib(this);
        if(  strcmp(m_attr->m_skinName,"sk.Peop.shiz_f0a")==0 )
             m_skin.SetAnimationCallback(AnimateCallBack0); 
        else
        if(  strcmp(m_attr->m_skinName,"sk.Taxi.cln_f01")==0 )
             m_skin.SetAnimationCallback(AnimateCallBack1); 
        else
        if(  strcmp(m_attr->m_skinName,"sk.Taxi.cln_f08")==0 )
             m_skin.SetAnimationCallback(AnimateCallBack2); */

        if( m_askin->isAutoAnim()  )
            m_askin->skinSetAnimAuto(&m_skin);

        if(  route[0] == 0  )
             m_routeID = context->searchObject( m_attr->m_routeName );
        else m_routeID = context->searchObject( route );
        m_prevTime = viewTime;

        m_curNode = 0;
        m_calcPosInc = m_attr->m_timeInc;

        IRouteObject *ri = (IRouteObject*)
              (context->queryInterface(m_routeID,IRouteObjectIID));

        ri->AddRef();

        {
             setPosition( ri->GetNode(0) );
             CFVector3 nextNode(ri->GetNode(1));

             CFVector3 pos(getPosition());
             setHAngleFixDir( atan2( nextNode.z-pos.z, nextNode.x-pos.x ) );
             m_dir.y = 0;
             double dist = hypot(nextNode.z-pos.z, nextNode.x-pos.x );
             m_nextNode = nextNode;

             if(  m_attr->m_speed > 1e-3 )
             {
                  event.label       = pe_EVC_NEXTNODE;
                  event.timeStamp   = viewTime + dist/m_attr->m_speed;
                  issueEvent( event );
             }
        }
        event.label       = pe_EVC_MOVE;
        event.source      = getObjectID();
        event.destination = getObjectID();
        event.timeStamp   = viewTime;
        issueEvent( event );
        event.label       = pe_EV_STARTSHOW;
        issueEvent( event );
     }
     m_lastMovePos = getPosition();
     break;

    case pe_EVCMD_STOP:
            context->removeEvent(pe_EVC_MOVE,getObjectID());
            break;

    case KR_WAKE_UP:
            break;

    case KR_SET_ATTR:
            s_ASSERTNQ("People:receiveEvent:case KR_SET_ATTR: do't supported");
            break;

    case pe_EV_SETAUTOANIM:
            m_askin->skinSetAnimAuto(&m_skin);
            break;

    case t_EV_ONCOLLISION:
            break;

    default: return 0;
    }
    return 1;
 }

 //============================================================
void People::addNotify()
 {
    ct_Subject::addNotify();
    m_askin = NULL;
    m_damage = 1;
    m_killed = KILL_NONE;
    m_rotateOz = 0;
    m_deleted  = 0;
    m_nextNode = CFVector3(0,0,0);
    initState();
    m_prevShootTime  = 0;
    m_lastDamageTime = 0;
    m_correctScale   = 1;
    m_commanderID    = KR_ObjectID::NUL();
    m_isNotCreate    = 1;
    m_prevStartShoot = 0;
    m_stoped         = 0;
    m_isClz          = 0;
    m_rotateOx = m_rotateOy = m_rotateOz = 0;
}

 //============================================================
void People::removeNotify()
 {    
    IRouteObject *ri = (IRouteObject*)
              (context->queryInterface(m_routeID,IRouteObjectIID));

    if(  ri!=0  )
         ri->DelRef();

    if(  !m_snd.isNUL()  )
         context->removeObject( m_snd );

    ct_Subject::removeNotify();
 }

 //============================================================
void People::draw()
{
}

 //============================================================


void People::onExitAudibleZone(double ts) 
{

        if(  !m_snd.isNUL()  )
        {  
          KR_Event event;
          event.destination = m_snd;
          event.source      = getObjectID();
          event.timeStamp   = ts;
          event.label       = snd_EV_END;
          context->sendEventNow( event );
         }
}

void People::onEnterAudibleZone(double ts) 
{

        // Sound
         if(  !m_snd.isNUL() )
         {
	  KR_Event event;

          event.destination = m_snd;
          event.source      = getObjectID();
	  event.timeStamp   = ts;
          event.label       = snd_EV_START;
          event.data.open(EDO_WRITE)
                 .putInt(0)
              .close();
          context->sendEventNow( event );
         } 
} 
  
 //============================================================
void People::render   ( CViewDynamicList &list, double ts)
{
    if(  m_isNotCreate  )
         return;


    m_calcPosInc = m_attr->m_timeInc;

    CFMatrix3x4 &m = m_skin.GetDirModify();
    m.LoadIdentity();
    m.RotateOzL(m_rotateOz);
    m.RotateOxL(m_rotateOx);
    m.RotateOyL(m_rotateOy);
    CFVector3 np = getPosition()+CFVector3(0,m_attr->m_centerHeight,0);
    if(  m_lastMoveDeltaT > 1e-3  )
         np += (getPosition() - m_lastMovePos)*((ts-m_lastMoveTimeStamp)/m_lastMoveDeltaT);
    m.TranslateL( np );

    m_viewDynObj.prepareToRender();
    list.Load( &m_viewDynObj );
}


 //============================================================

void People::endRender( CViewScene *scene )
{
    if(  m_isNotCreate  )
         return;

    scene->RemoveLandDynamic( &m_viewDynObj );
}

 //============================================================
CFVector3     People::realPosition() {  return getPosition();  }


 /*************************************
  *
  *   PeopleTable implementation
  *
  *************************************/

/*
 //============================================================

bool PeopleTable::isRendering()
{
    return 1;
}
*/
 //============================================================
void PeopleTable::allocObjects( int objectQnty )
 {
    m_table = new People[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void PeopleTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *PeopleTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"PeopleTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 /*************************************
  *
  *   AttributeTable implementation
  *
  *************************************/

 //============================================================
void AttributeTablePeople::allocObjects( int objectQnty )
 {
    m_table = new AttributePeople[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void AttributeTablePeople::freeObjects()
 {
    delete [] m_table;
    m_table         = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *AttributeTablePeople::getObjectPTR( int index )
 {
    s_ASSERT(index>=0 && index <m_maxObjectQnty,"AttributeTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 /*************************************
  *
  *   AttributePeople implementation
  *
  *************************************/

 //============================================================
bool People::isShooter()
{
    return m_attr->m_bulletIndex!=-1;
}

void AttributePeople::update(double ts)
{
    m_bulletTable = -1;
    m_bulletIndex = -1;
    if(  m_bulletAttrName[0]!=0  )
    {
         m_bulletTable=g_arena.searchSeanceClassTable("Bullet");

         m_bulletIndex = g_arena.getAttributeIndex(
                                       g_arena.searchSeanceClassTable("BulletAttr"),
                                       context->searchObject(m_bulletAttrName)
                                      );
    }

    m_cannonOfs = CFVector3(m_cannonX,m_cannonY,m_cannonZ);
   /*
    * Search and get skin
    */
    KR_ObjectID skinID =  context->searchObject( m_skinName );
    s_ASSERT( !skinID.isNUL(), "AttributePeople::update" );
    
    m_skinID = skinID;

    KR_Event event;
    event.timeStamp   = ts;
    event.label       = sk_EV_QUERY_MODEL_PTR;
    event.destination = skinID;
    context->sendEventNow( event );
    s_ASSERT(event.label==sk_EV_QUERY_MODEL_PTR_OK,"AttributePeople::update");
    event.data.open(EDO_READ)
                .get(&m_cacheSkin,sizeof(void*))
              .close();

    m_cacheExplosionTable = g_arena.searchSeanceClassTable( "Explosion" );
    m_cacheExplAttr = g_arena.getAttributeIndex( 
                                    g_arena.searchSeanceClassTable( "ExplosionAttr" ),
                                    context->searchObject(m_explAttrName) 
                                 );

    m_smokeTableID = g_arena.searchSeanceClassTable( "Smoke" );
    m_smokeAttrID = context->searchObject(m_smokeAttrName);

    if (!SetSoundAttr(getObjectID(), context,m_soundName,m_ctsndID, (void *)&m_wav))
	m_wav = NULL;

    m_cacheCorpseTable = g_arena.searchSeanceClassTable( "Corpse" );
    m_cacheCorpseAttr  = g_arena.getAttributeIndex( 
                                    g_arena.searchSeanceClassTable( "CorpseAttr" ),
                                    context->searchObject(m_corpseAttrName) 
                                 );


}

void People::onKilled( const CFVector3 &/*explPos*/, double ts )
{

     if(  strcmp(m_attr->m_skinName, "sk.Peop.cln_S00" )==0  )
          m_killed = KILL_SINK;
     else
     if(  strcmp(m_attr->m_skinName, "sk.Taxi.cln_f01" )==0  )
          m_killed = KILL_FALLOW_2;
     else m_killed = KILL_FALLOW_1;
     m_kill_startPhase  = asin(m_rotateOx);
     m_kill_deltaVAngle = 0;
     m_kill_time        = ts;
}

/*
  Дирижабль с махающими крыльями
 */
static
void AnimateCallBack0(CViewObjectBaseSet *,CViewObjectBase *pBase,CViewObjectRef *ref)
 {
    //(void)pBase;
    People &self  = *((People*)(ref->GetUserAttrib()));
        int reduc = pBase->ReductionNum();
    double a  = Session::m_moment;
    double c  = cos(a*3);
    double s  = sin(a*3);

    //pBase->KFSet().Mod0("VesloR")
    if(  c > 0  )
    {
         CFVector3 ax(-3.99,7.34,-2.69);
         self.m_askin->get0(1,reduc)->
               LoadIdentity()
               .RotateOz(M_PI/4,ax)
               .RotateOy( 0.8*s,ax)
              .Update();

         ax.x = 3.99;
         self.m_askin->get0(0,reduc)->
               LoadIdentity()
               .RotateOz(-M_PI/4,ax)
               .RotateOy(-0.8*s,ax)
              .Update();
    }
    else
    {
         CFVector3 ax(-3.99,7.34,-2.69);

         double ang = c*4;
         if(  c > M_PI/2  ) 
              c = M_PI/2;
         else
         if(  c < -M_PI/2  )
              c = -M_PI/2;

         self.m_askin->get0(1,reduc)->
               LoadIdentity()
               .RotateOx(c,ax)
               .RotateOz(M_PI/4,ax)
               .RotateOy(0.8*s,ax)
              .Update();
         ax.x = 3.99;
         self.m_askin->get0(0,reduc)->
               LoadIdentity()
               .RotateOx(-c,ax)
               .RotateOz(-M_PI/4,ax)
               .RotateOy(-0.8*s,ax)
              .Update();
    }

 }

//static
void AnimateCallBack1(CViewObjectBaseSet *,CViewObjectBase *pBase,CViewObjectRef *ref)
 {
    //(void)pBase;
    People &self  = *((People*)(ref->GetUserAttrib()));
    int reduc = pBase->ReductionNum();
    double a  = Session::m_moment;

    CFVector3 ax(0, 2.5, -5.3);
    self.m_askin->get0(0,reduc)->
               LoadIdentity()
               .RotateOz(a*8,ax)
              .Update();
 }

static
void AnimateCallBack2(CViewObjectBaseSet *,CViewObjectBase *pBase,CViewObjectRef *ref)
 {
    //(void)pBase;
    People &self  = *((People*)(ref->GetUserAttrib()));
    int reduc = pBase->ReductionNum();
    double a  = Session::m_moment;
    double p;

    CFVector3 ax;

    ax = CFVector3(-2.37,0.53,-1.30);
   

    p = -a*18*15/self.m_attr->m_speed;
    self.m_askin->get0(0,reduc)->
               LoadIdentity()
               .RotateOx(p,ax);
    self.m_askin->get0(1,reduc)->
               LoadIdentity()
               .RotateOx(p,ax);

    ax = CFVector3(0.624,0.98,1.67);
    p  = -a*12*15/self.m_attr->m_speed;
    self.m_askin->get0(2,reduc)->
               LoadIdentity()
               .RotateOx(p,ax);
    self.m_askin->get0(3,reduc)->
               LoadIdentity()
               .RotateOx(p,ax);

    ax = CFVector3(-2.37,3.05,-0.37);
    self.m_askin->get0(4,reduc)->
               LoadIdentity()
               .RotateOx(p,ax);

    double ofsy = 0.5*sin(-p),
           ofsz = 0.5*cos(-p);

    self.m_askin->get0(5,reduc)->
               LoadIdentity()
               .Translate( CFVector3(0,ofsy,ofsz) );

    self.m_askin->get (0,reduc)->
               Update();
 }

void shoot( KR_ObjectID fromID, 
            int bulletTable, int attrIndex, 
            const CFVector3 &pos, const CFVector3 &dir,
            SimulationContext *context, double ts )
{
    KR_Event event;

    event.label       = b_EV_START;
    event.source      = fromID;
    event.destination = g_arena.newObject( bulletTable , "B" );

    if(  !event.destination.isNUL()  )
    {
         event.data.open(EDO_WRITE)
                        .descend( VECTOR3D_F, 0 )
                          .putDouble(pos.x)
                          .putDouble(pos.y)
                          .putDouble(pos.z)
                        .ascend()
                        .descend( VECTOR3D_F, 0 )
                          .putDouble(dir.x)
                          .putDouble(dir.y)
                          .putDouble(dir.z)
                        .ascend()
                        .putInt(attrIndex)
                        .putObjectID(fromID)
                   .close();
         event.timeStamp = ts;
         context->sendEventNow( event );
    }
}

void People::onShoot(double ts)
{
      IDynamicObject *dobj = (IDynamicObject*)(context->queryInterface(getEnemyID(),IDynamicObjectIID));
      if(  dobj  )
      {
           CFMatrix3x4 m;
           getMatrix(m);
           m.TranslateR(m_attr->m_cannonOfs)
            .TranslateL(getPosition());

           CFVector3 pos(dobj->getPos());
           CFVector3 mpos(m.Offset());

           switch( m_attr->m_onLand )
           {
           case NO_LAND:
             if(  Abs2(pos-mpos) < 200*200 )
             {
                CFVector3 dir(getMoveDir());

                if(  dir*Normal(pos-mpos) > 0.90  )
                {
                     m_correctScale *= 0.8;
                     m_correctScale += 0.001;
                     shoot( getObjectID(),
                           m_attr->m_bulletTable, m_attr->m_bulletIndex,
                           mpos+dir*8, dir,
                           context,ts );

                     if(  m_attr->m_stopIfAttack  )
                     if(  m_prevStartShoot > m_attr->m_burstCount  )
                     {
                          m_prevStartShoot = 0;
                          popState();
                     }
                     else m_prevStartShoot++;
                     m_prevShootTime = ts;
                }
                else m_correctScale = 1;
             }
             else m_correctScale = 1;
             break;

           case ON_OBJ: case ON_LAND: case ON_WATER:
             if(  Abs2(pos-mpos) < 200*200 )
             {
                CFVector3 dir(getMoveDir());
                CFVector3 to(Normal(pos-mpos));

                if(  Normal(CFVector3(dir.x,0,dir.z))
                    *Normal(CFVector3(to.x ,0,to.z)) > 0.9  )
                {
                     m_correctScale *= 0.8;
                     m_correctScale += 0.001;
                     shoot( getObjectID(),
                           m_attr->m_bulletTable, m_attr->m_bulletIndex,
                           mpos, to,
                           context,ts );

                     if(  m_attr->m_stopIfAttack  )
                     if(  m_prevStartShoot > m_attr->m_burstCount  )
                     {
                          m_prevStartShoot = 0;
                          popState();
                     }
                     else m_prevStartShoot++;
                     m_prevShootTime = ts;
                }
                else m_correctScale = 1;
             }
             else m_correctScale = 1;
             break;
           }
      }
      else m_correctScale = 1;
}

void  People::pushState( int state, KR_ObjectID enemyID )
{
   if(  m_stateSP < MAX_STATE-1  )
   {
        m_state  [m_stateSP] = state;

        if(  state==pe_STATE_ATTACK  )
        {
             m_returnPoint = getPosition();
             s_ASSERT(!enemyID.isNUL(),"People::pushState");
        }

         m_enemyID[m_stateSP] = enemyID;
         m_stateSP++;
    }
}



bool	People::dump(PIN_SaveFile & sf)
{
    if (!ct_Subject::dump(sf) ||
        !sf.WriteData( (char *) & m_peopleAttrID, sizeof(PeopleData)  ))
        return false;
		
    return true;
}

bool	People::load(PIN_SaveFile & sf)
{
    if (!ct_Subject::load(sf) ||
        !sf.GetData( (char *) & m_peopleAttrID, sizeof(PeopleData)  ))
        return false;
		
    return true;
}

void	People::loadNotify()
{
    ct_Subject::loadNotify();
    setPeopleAttr();
}


double g_distToSeg(   const CFVector3 &v,   const CFVector3 &p0,   const CFVector3 &p1    )
{
    CFVector3 vec = p1-p0;
    double    len = Abs(vec);
    double    ofs = vec*(v-p0);
    if(  ofs<0  )
         return Abs(v-p0);
    if(  ofs>len  )
         return Abs(v-p1);
    return sqrt(Abs2(v-p0)-ofs*ofs);
}

void g_toSeg( CFVector3 &v, const CFVector3 &p0, const CFVector3 &p1, double max )
{
    CFVector3 vec = p1-p0;
    double    len = Abs(vec);
    double    ofs = vec*(v-p0);
    if(  ofs<0  )
    {
         double d = Abs(v-p0);
         if(  d>max  )
              v = p0+Normal(v-p0)*max;
         return;
    }

    if(  ofs>len  )
    {
         double d = Abs(v-p1);
         if(  d>max  )
              v = p1+Normal(v-p1)*max;
         return;
    }
    double d = sqrt(Abs2(v-p0)-ofs*ofs);
    if(  d>max  )
    {
         CFVector3 nea = p0+Normal(p1-p0)*ofs;
         v = near+Normal(v-nea)*max;
    }
}

void People::setPosition(  const CFVector3 &vec  )
{
    IRouteObject *ri = (IRouteObject*)(context->queryInterface(m_routeID,IRouteObjectIID));

    int n0 = m_curNode;
    int n1 = (n0+1)%ri->GetNodeCnt();

    CFVector3 v = vec;
    g_toSeg( v, ri->GetNode(n0), ri->GetNode(n1), m_attr->m_maxOutDist );

    ct_Subject::setPosition( v );
}

/* End of file C:\WinGame\OBASE\People\People.cpp */
