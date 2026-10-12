/*

 * File  : C:\NW\ARENA\OBASE\Lamp\Lamp.cpp
 * Autor :
 * Ver   1.0 
 */
#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "graph.h"
#include "Lamp.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/fountmsg.h"
#include "message/Lampmsg.h"
#include "h/light.h"
#include "h/phisics.h"
#include "h/cachesmoke.h"
#include "kernel/h/session.h"
#include "storage/h/savefile.h"


#define RGB_TO_LIST(col)  ((col)>>16), ((col)>>8)&255, (col)&255

 //===========================================================================
class AttributeLamp : public ct_Attribute
{
 public:
    GR_HTEXTURE     m_coronaHText;
    unsigned long   m_coronaColor;

    virtual void    update(double ts);

    double	    m_coronaFadeCoeff;

//{{ATTRIBUTE
    ct_AttrItem  m_array[26];
    int             m_lightColor             ;  // 
    double          m_minLightBright         ;  // 
    double          m_maxLightBright         ;  // 
    double          m_lightRadius            ;  // 
    double          m_lightBrightStep        ;  // 
    double          m_lightOffset            ;  // 
    int             m_useCorona              ;  // 
    int             m_coronaRGB              ;  // 
    double          m_coronaR                ;  // 
    int             m_coronaAlpha            ;  // 
    ct_AttrStr      m_coronaName             ;  // 
    double          m_maxCoronaR             ;  // 
    double          m_createIncMin           ;  // 
    double          m_createIncMax           ;  // 
    int             m_lightMode              ;  // 0 - permanent; 1 - randomly changeable; 2 - blinking; 3-fading
    double          m_maxTimeLife            ;  // 
    double          m_sleepTime              ;  // 
    double          m_maxBrightTime          ;  // 
    double          m_FadeInCoeff            ;  // 
    double          m_FadeOutCoeff           ;  // 
    int             m_onLand                 ;  // 
    int             m_isMoving               ;  // ƒвижуща€с€ байда
    int             m_movementType           ;  // 0 - в одном направлении, 1 - туда обратно
    double          m_movementDeltaT         ;  // „ерез какое врем€ пересчитывать положение
    int             m_hasParticle            ;  // 
    double          m_particleWidth          ;  // 

    AttributeLamp()
    {
        m_lightColor         = 7;
        m_minLightBright     = 20;
        m_maxLightBright     = 255;
        m_lightRadius        = 10.0;
        m_lightBrightStep    = 30.0;
        m_lightOffset        = 2.0;
        m_useCorona          = 1;
        m_coronaRGB          = 0xFFFFFF;
        m_coronaR            = 0.2;
        m_coronaAlpha        = 255;
        strncpy(m_coronaName,"corona.spr", sizeof( ct_AttrStr )-1 );
        m_maxCoronaR         = 5;
        m_createIncMin       = 0.1;
        m_createIncMax       = 0.3;
        m_lightMode          = 0;
        m_maxTimeLife        = 0;
        m_sleepTime          = 1.0;
        m_maxBrightTime      = 0.5;
        m_FadeInCoeff        = 2.0;
        m_FadeOutCoeff       = 10;
        m_onLand             = 0;
        m_isMoving           = 0;
        m_movementType       = 0;
        m_movementDeltaT     = 2.0;
        m_hasParticle        = 1;
        m_particleWidth      = 0.1;

        m_array[0].set("m_lightColor",m_lightColor);
        m_array[1].set("m_minLightBright",m_minLightBright);
        m_array[2].set("m_maxLightBright",m_maxLightBright);
        m_array[3].set("m_lightRadius",m_lightRadius);
        m_array[4].set("m_lightBrightStep",m_lightBrightStep);
        m_array[5].set("m_lightOffset",m_lightOffset);
        m_array[6].set("m_useCorona",m_useCorona);
        m_array[7].set("m_coronaRGB",m_coronaRGB);
        m_array[8].set("m_coronaR",m_coronaR);
        m_array[9].set("m_coronaAlpha",m_coronaAlpha);
        m_array[10].set("m_coronaName",m_coronaName);
        m_array[11].set("m_maxCoronaR",m_maxCoronaR);
        m_array[12].set("m_createIncMin",m_createIncMin);
        m_array[13].set("m_createIncMax",m_createIncMax);
        m_array[14].set("m_lightMode",m_lightMode);
        m_array[15].set("m_maxTimeLife",m_maxTimeLife);
        m_array[16].set("m_sleepTime",m_sleepTime);
        m_array[17].set("m_maxBrightTime",m_maxBrightTime);
        m_array[18].set("m_FadeInCoeff",m_FadeInCoeff);
        m_array[19].set("m_FadeOutCoeff",m_FadeOutCoeff);
        m_array[20].set("m_onLand  ",m_onLand  );
        m_array[21].set("m_isMoving",m_isMoving);
        m_array[22].set("m_movementType",m_movementType);
        m_array[23].set("m_movementDeltaT",m_movementDeltaT);
        m_array[24].set("m_hasParticle",m_hasParticle);
        m_array[25].set("m_particleWidth",m_particleWidth);

        linkTable(m_array,26);
    }
//}}END_OF_ATTRIBUTE
};

 //===========================================================================
class LampTable : public ct_SubjectTable
{
 private:
    Lamp *m_table;
 public:
    LampTable()
    {
        m_table = NULL;
        registerClass( "Lamp" );
    }
    ~LampTable()
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
bool LampTable::isRendering()
{
     return true;
}

 //===========================================================================
class AttributeTableLamp : public ct_AttributeTable
{
 protected:
    AttributeLamp *m_table;

 public:
    AttributeTableLamp()
    {
       m_table = NULL;
       registerClass( "LampAttr" );
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static LampTable  __classTable;
static AttributeTableLamp __attrTable;
 /*********************************
  *
  *   Lamp implementation
  *
  *********************************/

 //============================================================
Lamp::Lamp()
 {
    m_attr = 0;
 }

 //============================================================
Lamp::~Lamp()
 {
 }

 //============================================================
void Lamp::onView(double ts)
{
    if(  !m_sentMove  && m_attr->m_lightMode != 0) 
    {
          KR_Event event;
          event.label       = lmp_EV_MOVE;
          event.source      = getObjectID();
          event.destination = getObjectID();
          event.timeStamp   = ts;
          issueEvent( event );
          m_sentMove = true;
    }
}

CFVector3 Lamp::realPosition()
{
     return m_lampPosition;
}

void Lamp::RandomChange(KR_Event &event)
{
	if(  m_isVisible  )
	{
		double ts = event.timeStamp;
		
		event.timeStamp += context->rnd_f(
			m_attr->m_createIncMin,
			m_attr->m_createIncMax
			);
		issueEvent( event );
		m_sentMove = true;
		
		m_brightness +=  context->rnd_f(m_attr->m_lightBrightStep) 
			- m_attr->m_lightBrightStep/2;
		if(  m_brightness < m_attr->m_minLightBright  )
			m_brightness = m_attr->m_minLightBright;
		else
			if(  m_brightness > m_attr->m_maxLightBright  )
				m_brightness = m_attr->m_maxLightBright;
			
	}
	else 
	m_sentMove = false;
}







void Lamp::Fade()
{

	double dt = Session::m_moment - m_lastEventTime;
        m_lastEventTime = Session::m_moment;

	if(!m_isVisible)
		return;

	double db;

	switch (m_blinkState)
	{
	case 1: // fade out, check if min bright

		db = m_attr->m_FadeOutCoeff * dt * dt;
		
		m_brightness  -= db;
		m_currentTransparency -= m_attr->m_coronaFadeCoeff * db;


		if(  m_brightness < m_attr->m_minLightBright  )
		{
			m_brightness = m_attr->m_minLightBright;
			m_currentTransparency = 0;
			m_blinkState = 2;
		}
		break;	
	case 2: // fade in, check if full bright

		db = m_attr->m_FadeInCoeff  * dt * dt ;

		m_brightness += db;
		m_currentTransparency += m_attr->m_coronaFadeCoeff * db;

		if(  m_brightness > m_attr->m_maxLightBright  )
		{
			m_brightness = m_attr->m_maxLightBright;
			m_currentTransparency =	m_attr->m_coronaAlpha;
			m_blinkState = 1;
		}
		break;
	}
}

void Lamp::Blink(KR_Event &event)
{
	if (m_blinkState) // on
	{
			m_blinkState = 0;

			event.timeStamp += m_attr->m_sleepTime;
			issueEvent(event);
	}
	 else	// off
	{
			m_blinkState = 1;

			event.timeStamp += m_attr->m_maxBrightTime;
			issueEvent(event);		
	}
}

void Lamp::setLampAttr()
{
	ct_Attribute *attr = __attrTable.searchAttribute(m_lampAttrID);
	if( attr==NULL )
		echo( "Fountain::receiveEvent: Unknown attribute %s",
		context->searchObject(m_lampAttrID));
	else m_attr = (AttributeLamp*)attr;
	
	if(  m_attr->m_onLand  )
	{
		double height;
		CFVector3 normal;
		CViewScene::Current()->GetTerrain()->GetPlane(m_lampPosition,normal,height);
		m_lampPosition.y = height;
	}
	
}

 //============================================================
int Lamp::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case lmp_EV_MOVE:

			switch (m_attr->m_lightMode )
			{
				case 1: //randomly changeable
					RandomChange(event);
					break;
				case 2: // blink
					Blink(event);
					break;
			}	
            break;

	case lmp_EV_CHANGEPOS:

				if (m_movementCurrentDirection == 0) // forward
				{
					m_movementCurrentStep++;
					if (m_movementCurrentStep == m_movementNumberOfSteps)
					{
						if (m_attr->m_movementType)
						{
							m_movementCurrentDirection = 1;	// back;
						}
							else
						{
							m_movementCurrentStep = 0;
						}

					}
				}
				 else // back
				{
					m_movementCurrentStep--;
					if (m_movementCurrentStep == 0)
						m_movementCurrentDirection = 0;
				}

				m_lampPosition = m_startPosition + m_movementDelta * m_movementCurrentStep;
	
			    event.timeStamp  += m_attr->m_movementDeltaT;
				issueEvent( event );		


			break;

    case KR_WAKE_UP:
            break;

    case KR_SET_ATTR:
            s_ASSERTNQ("Lamp:receiveEvent:case KR_SET_ATTR: do't supported");
            break;

    case lmp_EV_REMOVE:
            context->removeObject(getObjectID());
            break;

	case lmp_EV_SETENDPOS:
		{
			KR_ObjectID oID;

		       event.data.open(EDO_READ)
                        .getObjectID(oID)
                        .getDouble(m_endPosition.x)
                        .getDouble(m_endPosition.y)
                        .getDouble(m_endPosition.z)
						.getInt   (m_movementNumberOfSteps)
						.getInt   (m_movementCurrentStep)
                      .close();

			m_movementDelta.x = (m_endPosition.x - m_startPosition.x) / m_movementNumberOfSteps;
			m_movementDelta.y = (m_endPosition.y - m_startPosition.y) / m_movementNumberOfSteps;
			m_movementDelta.z = (m_endPosition.z - m_startPosition.z) / m_movementNumberOfSteps;
		}
			break;

    case lmp_EV_START:
     {
            //KR_ObjectID oID;
            event.data.open(EDO_READ)
                        .getObjectID(m_lampAttrID)
                        .getDouble(m_lampPosition.x)
                        .getDouble(m_lampPosition.y)
                        .getDouble(m_lampPosition.z)
                      .close();

	
			m_startPosition = m_lampPosition;

			setLampAttr();
			
     }
     m_sentMove = false;
     m_lastEventTime = m_startTime = event.timeStamp;

     m_blinkState = 1;
     m_brightness = m_attr->m_maxLightBright;
     m_currentTransparency = m_attr->m_coronaAlpha;
	 m_movementCurrentDirection = 0;


	 m_particle.m_color = GRCreateColor(RGB_TO_LIST(m_attr->m_coronaRGB));


     
     if(   m_attr->m_maxTimeLife > 0  )
     {
           event.label       = lmp_EV_REMOVE;
           event.source      = getObjectID();
           event.destination = getObjectID();
           event.timeStamp   = m_startTime + m_attr->m_maxTimeLife;
           issueEvent( event );
     }

	 if ( m_attr->m_isMoving)
	 {
           event.label       = lmp_EV_CHANGEPOS;
           event.source      = getObjectID();
           event.destination = getObjectID();
           event.timeStamp   = m_startTime + m_attr->m_movementDeltaT;
           issueEvent( event );		
	 }


     break;


    default: return 0;
    }
    return 1;
 }

 //============================================================
void Lamp::addNotify()
 {
    ct_Subject::addNotify();
    // insert your code this
    m_attr = 0;
    m_brightness = 0;
 }

 //============================================================
void Lamp::removeNotify()
 {
    ct_Subject::removeNotify();
    // insert your code this
 }


 /*************************************
  *
  *   LampTable implementation
  *
  *************************************/

 //============================================================
void LampTable::allocObjects( int objectQnty )
 {
    m_table = new Lamp[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void LampTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *LampTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"LampTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 /*************************************
  *
  *   AttributeTable implementation
  *
  *************************************/

 //============================================================
void AttributeTableLamp::allocObjects( int objectQnty )
 {
    m_table = new AttributeLamp[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void AttributeTableLamp::freeObjects()
 {
    delete [] m_table;
    m_table         = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *AttributeTableLamp::getObjectPTR( int index )
 {
    s_ASSERT(index>=0 && index <m_maxObjectQnty,"AttributeTable::getObjectPTR");
    return &(m_table[ index ]);
 }




void  AttributeLamp::update(double)
{
    m_coronaHText = g_loadSmoke( m_coronaName, NULL );
    m_coronaColor = GRTransparentColor(m_coronaRGB>>16,(m_coronaRGB>>8)&255,m_coronaRGB&255);

    m_coronaFadeCoeff = m_coronaAlpha / (m_maxLightBright - m_minLightBright);

	
}


void Lamp::render( CViewDynamicList &list,double)
{


	if (m_attr->m_hasParticle)
	{	    
	    m_particle.prepareToRender(m_lampPosition,m_attr->m_particleWidth);
	    list.Load( & m_particle);
	}
	


	if (m_attr->m_lightMode == 3)
	 Fade();

	
	if (m_blinkState)
	{
		g_lightChain.add( m_lampPosition+CFVector3(0,m_attr->m_lightOffset,0), 
			m_attr->m_lightColor, 
			(int)(m_brightness), 
			m_attr->m_lightRadius );
		
		if(  m_attr->m_useCorona  )
		{             
			double r = Abs2(CViewObject::m_viewPointInvMx.Offset()-realPosition())*m_attr->m_coronaR;
			if(  r > m_attr->m_maxCoronaR  )
				r = m_attr->m_maxCoronaR;
			m_corona.prepareToRender(
				realPosition(),
				r,
				2<<16, 2<<16,
				126<<16, 126<<16,
				//m_attr->m_coronaAlpha,
				m_currentTransparency,
				m_attr->m_coronaColor,
				m_attr->m_coronaHText
				);
			list.Load( &m_corona );
		}

		m_renderLastFrame = 1;
	}
}

void Lamp::endRender( CViewScene *scene )
{
     if(  m_attr->m_useCorona  && m_renderLastFrame)	
          scene->RemoveLandDynamic( &m_corona );


	if (m_attr->m_hasParticle)	
	    scene->RemoveLandDynamic( & m_particle);


	 m_renderLastFrame = 0;
}


bool	Lamp::dump(PIN_SaveFile & sf)
{

		if (!ct_Subject::dump(sf))
			return false;

		if (!sf.WriteData( (char *) & m_lampAttrID, sizeof(LampData)  ))
			return false;

		return true;
}

bool	Lamp::load(PIN_SaveFile & sf)
{
		if (!ct_Subject::load(sf))
			return false;

		if (!sf.GetData( (char *) & m_lampAttrID, sizeof(LampData)  ))
			return false;
		
		return true;
}

void	Lamp::loadNotify()
{
	ct_Subject::loadNotify();
	setLampAttr();
}


/* End of file C:\NW\ARENA\OBASE\Lamp\Lamp.cpp */
