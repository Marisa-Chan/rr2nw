/*
 * File  : C:\WinGame\OBASE\bird\bird.cpp
 * Autor :
 * Ver   1.0 
 */

#include "bird.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/bimsg.h"
#include "message/skinmsg.h"
#include "storage/h/savefile.h"

//#include <afxwin.h>

 //===========================================================================
class AttributeBird : public ct_Attribute
{
 public:
    CViewObjectModel*   m_cacheSkin              ;  // 
    virtual void    update(double ts);  

//{{ATTRIBUTE
    ct_AttrItem  m_array[4];
    double          m_calcNewPosIncrement    ;  // 
    double          m_calcPosIncrement       ;  // 
    ct_AttrStr      m_skinName               ;  // 
    double          m_speed                  ;  // 

    AttributeBird()
    {
        m_calcNewPosIncrement = 5;
        m_calcPosIncrement   = 0.080012345678;
        strncpy(m_skinName,"Bird.0", sizeof( ct_AttrStr )-1 );
        m_speed              = 20;

        m_array[0].set("m_calcNewPosIncrement",m_calcNewPosIncrement);
        m_array[1].set("m_calcPosIncrement",m_calcPosIncrement);
        m_array[2].set("m_skinName",m_skinName);
        m_array[3].set("m_speed",m_speed);

        linkTable(m_array,4);
    }
//}}END_OF_ATTRIBUTE
};

//static AttributeBird __defaultAttr;

 //===========================================================================
strg_SUBJECT_TABLE_IMPLEMENTATION(Bird,1)
strg_ATTRIBUTE_TABLE_IMPLEMENTATION(Bird,"BirdAttr")

static BirdTable          __classTable;
 /*********************************
  *
  *   Bird implementation
  *
  *********************************/

 //============================================================
strg_CONSTRUCTOR_DYNVIEW(Bird)
 {
    m_attr = 0;//&__defaultAttr;
 }

 //============================================================
Bird::~Bird()
 {
 }

 //============================================================
void Bird::check( double &a , double &b )
 {
	if( a>b )
	{
		double t;
		t = a;
		a = b;
		b = t;
	}
 }

void Bird::onView(double time)
{
 
    if(  !m_sendedMove  )
    {
         KR_Event event;

         m_prevTime = time;
         event.timeStamp   = time; 
         event.label       = bi_EVC_MOVE;
         event.source      = getObjectID();
         event.destination = getObjectID();
         m_sendedMove = true;
         issueEvent( event );
    }

    if(  !m_sendedNewPos  )
    {
         KR_Event event;
         m_prevTime = time;
         event.timeStamp   = time; 
         event.label       = bi_EVC_NEWPOS;
         event.source      = getObjectID();
         event.destination = getObjectID();
         m_sendedNewPos = true;
         issueEvent( event );
    }
}


void Bird::setBirdAttr()
{
         ct_Attribute *attr = __attrTable.searchAttribute(m_birdAttrID);
         if( attr==NULL )
              echo( "Bird::receiveEvent: Unknown attribute %s",
                    context->searchObject(m_birdAttrID));
         else m_attr = (AttributeBird*)attr;

        m_skin.Attach(m_attr->m_cacheSkin);
        m_viewDynObj.BumpDef().fRadius = m_skin.Model()->Radius();
}

 //============================================================
int Bird::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {

    case bi_EVC_MOVE:
        m_lastMovePos       = m_pos;
        m_lastMoveTimeStamp = event.timeStamp;
        if(  m_isVisible  )
	{
             double deltaT = event.timeStamp - m_prevTime;
             m_prevTime = event.timeStamp;
             m_pos += m_dir * deltaT; 

             event.timeStamp += m_attr->m_calcPosIncrement;
             issueEvent( event );
             m_sendedMove = true;
        } 
        else m_sendedMove = false;

        break;

    case bi_EVC_NEWPOS:
        if(  !m_isVisible  )
             m_sendedNewPos = false;
        else
        {
                m_lastMovePos       = m_pos;
                m_lastMoveTimeStamp = event.timeStamp;

		if( m_pos.x<m_minPos.x ) m_pos.x = m_minPos.x;
		if( m_pos.y<m_minPos.y ) m_pos.y = m_minPos.y;
		if( m_pos.z<m_minPos.z ) m_pos.z = m_minPos.z;

		if( m_pos.x>m_maxPos.x ) m_pos.x = m_maxPos.x;
		if( m_pos.y>m_maxPos.y ) m_pos.y = m_maxPos.y;
		if( m_pos.z>m_maxPos.z ) m_pos.z = m_maxPos.z;

        
	    CFVector3 newPos;
        
        CFMatrix3x4 m;
        m.LoadIdentity()
         .RotateOxR(context->rnd_f(3.14/18, 3.14/8))
         .RotateOyR(context->rnd_f(3.14/18, 3.14/8));
        m_dir = Normal(m*m_dir)*m_attr->m_speed;

		newPos = m_pos + m_dir*m_attr->m_calcNewPosIncrement;
		if( newPos.x<m_minPos.x ) newPos.x = m_minPos.x;
		if( newPos.y<m_minPos.y ) newPos.y = m_minPos.y;
		if( newPos.z<m_minPos.z ) newPos.z = m_minPos.z;

		if( newPos.x>m_maxPos.x ) newPos.x = m_maxPos.x;
		if( newPos.y>m_maxPos.y ) newPos.y = m_maxPos.y;
		if( newPos.z>m_maxPos.z ) newPos.z = m_maxPos.z;
        /*
        newPos.x = context->rnd_f(m_minPos.x,m_maxPos.x);
        newPos.y = context->rnd_f(m_minPos.y,m_maxPos.y);
        newPos.z = context->rnd_f(m_minPos.z,m_maxPos.z);
        */
	    CFVector3 delta = newPos - m_pos;
        double len = Abs(delta);
        if(  len < 3.0  )
        {
             newPos = CFVector3( 
                                 context->rnd_f(m_minPos.x,m_maxPos.x),
                                 context->rnd_f(m_minPos.y,m_maxPos.y),
                                 context->rnd_f(m_minPos.z,m_maxPos.z)
                               );
             delta = newPos - m_pos;
             len = Abs( delta );
             if(  len < 0.2  ) 
             {
                  m_dir = CFVector3(m_attr->m_speed,0,0);
                  newPos = m_pos + CFVector3(0.2,0,0);
                  len = 1;
             }
             else m_dir = Normal(delta)*m_attr->m_speed;
        }
        else m_dir = Normal(delta)*m_attr->m_speed;
        m_rotateOx = atan2(m_dir.y,hypot(m_dir.x,m_dir.z));
        m_rotateOy = -atan2(m_dir.z,m_dir.x);

        double nextTime = len/m_attr->m_speed;

        if(  nextTime > m_attr->m_calcNewPosIncrement  )
             nextTime = m_attr->m_calcNewPosIncrement;
		
        m_prevTime = event.timeStamp;

		event.timeStamp += nextTime+context->rnd_f(0.3);
        issueEvent( event );
        m_sendedNewPos = true;

		}
        break;

    case KR_WAKE_UP:
            break;

    case KR_SET_ATTR:
            s_ASSERTNQ("Bird:receiveEvent:case KR_SET_ATTR: do't supported");
            break;

    case bi_EV_BEGIN_MOVE:
     {
        //m_sendedMove = false;
        //KR_ObjectID attrID;

		event.data.open(EDO_READ)
            .getObjectID(m_birdAttrID)

		    .descend( 1 , 0 )			
                   .getDouble( m_minPos.x )
                   .getDouble( m_minPos.y )
                   .getDouble( m_minPos.z )
			.ascend ( )
			.descend( 2 , 0 )			
                   .getDouble( m_maxPos.x )
                   .getDouble( m_maxPos.y )
                   .getDouble( m_maxPos.z )
			.ascend ( )
        .close();

	setBirdAttr();

        
         
        check( m_minPos.x , m_maxPos.x );
		check( m_minPos.y , m_maxPos.y );
		check( m_minPos.z , m_maxPos.z );

		CFVector3 scSize = m_maxPos - m_minPos;


	m_pos.x = m_minPos.x + context->rnd_f( scSize.x );
	m_pos.y = m_minPos.y + context->rnd_f( scSize.y );
        m_pos.z = m_minPos.z + context->rnd_f( scSize.z );
        m_dir = CFVector3(0,20,0);
        m_lastMovePos       = m_pos;
        m_lastMoveTimeStamp = event.timeStamp;

        m_prevTime = event.timeStamp;

        m_sendedNewPos = false;
        m_sendedMove = false;
     }
     break;

    default: return 0;
    }
    return 1;
 }

 //============================================================
void Bird::addNotify()
 {
    ct_Subject::addNotify();
    // insert your code this
    m_sendedNewPos = false;
    m_sendedMove   = false;
 }

 //============================================================
void Bird::removeNotify()
 {
    ct_Subject::removeNotify();
    // insert your code this
 }

 //============================================================
void Bird::onRender(double ts)
{
    CFMatrix3x4 &m = m_skin.GetDirModify();
    m.LoadIdentity();
    m.RotateOxL(m_rotateOx);
    m.RotateOyL(m_rotateOy+M_PI/2);
    m.TranslateL( m_pos + (m_pos-m_lastMovePos)*
                  ((ts-m_lastMoveTimeStamp)/m_attr->m_calcPosIncrement)
                );
}


bool	Bird::dump(PIN_SaveFile & sf)
{

		if (!ct_Subject::dump(sf))
			return false;

		if (!sf.WriteData( (char *) & m_birdAttrID, sizeof(BirdData)  ))
			return false;

		return true;
}

bool	Bird::load(PIN_SaveFile & sf)
{
		if (!ct_Subject::load(sf))
			return false;

		if (!sf.GetData( (char *) & m_birdAttrID, sizeof(BirdData)  ))
			return false;
		
		return true;
}

void	Bird::loadNotify()
{
	ct_Subject::loadNotify();
	setBirdAttr();
}

strg_SUBJECT_DYNVIEW_IMPLEMENTATION(Bird)

 //============================================================
CFVector3     Bird::realPosition() {  return m_pos;  }



 /*************************************
  *
  *   AttributeBird implementation
  *
  *************************************/

 //============================================================


void AttributeBird::update(double ts)
{
   strg_UPDATE_ATTRIBUTE_SKIN(m_skinName,m_cacheSkin,ts)
}

/* End of file C:\WinGame\OBASE\bird\bird.cpp */