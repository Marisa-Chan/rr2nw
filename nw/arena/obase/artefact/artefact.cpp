/*
 * File  : C:\WinGame\OBASE\bird\bird.cpp
 * Autor :
 * Ver   1.0 
 */
#include "Artefact.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "kernel/h/session.h"

#include "message/artfmsg.h"
#include "message/skinmsg.h"
#include "message/pubmsg.h"
#include "message/unitmsg.h"

#include "h/light.h"
#include "h/phisics.h"
#include "h/cachesmoke.h"

#include "storage\h\savefile.h"
#include "i/portal.i"

#define DELTAT 0.08



bool ICarrier::dump(PIN_SaveFile &sf)
{
	if (! sf.WriteData( (char * ) &m_artefactID, sizeof(KR_ObjectID)))
		return false;

	return true;
}

bool ICarrier::load(PIN_SaveFile &sf)
{
	if (! sf.GetData( (char * ) &m_artefactID, sizeof(KR_ObjectID)))
		return false;

	return true;
}

void ICarrier::loadNotify()
{
	if (!m_artefactID.isNUL())
		m_artefact = (IArtefact *) (g_arena.getContext()->queryInterface(m_artefactID,IArtefactIID));
	else
		m_artefact = NULL;
}

 //==========================================================================
void ICarrier::carrierDropArtefact(double ts)
{
  if(  m_artefact!=0  )
  {
       CFMatrix3x4 m;
       carrierLoadMatrix(m);
       m_artefact->drop( m, ts );

       m_artefact   = 0;
       m_artefactID = KR_ObjectID::NUL();
  }
}

 //==========================================================================
void ICarrier::carrierOnRemoveArtefact()
{
  if(  m_artefact!=0  )
  {
       m_artefact   = 0;
       m_artefactID = KR_ObjectID::NUL();
  }
}

 //==========================================================================
void ICarrier::carrierTakeArtefact(KR_ObjectID oID, IArtefact *artefact )
{
  if(  m_artefact==0  )
  {
       m_artefactID = oID;
       m_artefact   = artefact;
  }
}

 //==========================================================================
void ICarrier::carrierOnCollizion (KR_ObjectID oID)
{
  if(  m_artefact==0 && !oID.isNUL() )
  {
       IArtefact *artefact = (IArtefact *)
              (g_arena.getContext()->queryInterface(oID,IArtefactIID));

       if(  artefact!=0  )
            carrierTakeArtefact( oID, artefact );
  }
}

 //==========================================================================
int  ICarrier::carrierReceiveEvent( KR_Event &event )
{
   switch( event.label )
   {
   case t_EV_ONCOLLISION:
     return 0;

   default: return 0;
   }

}

 //==========================================================================
void ICarrier::carrierAddNotify   (SimulationContext*,double)
{
  m_artefactID = KR_ObjectID::NUL();
  m_artefact   = 0;
}

 //==========================================================================
void ICarrier::carrierRemoveNotify(SimulationContext*,double ts)
{
  carrierDropArtefact(ts);
}

void ICarrier::carrierOnMove      ()
{
  if(  m_artefact!=0  )
  {
       CFMatrix3x4 m;
       carrierLoadMatrix(m);
       m_artefact->moveTo(m);
  }
}




bool IArtefact::dump(PIN_SaveFile &sf)
{
	if (! sf.WriteData( (char * ) &m_carrierID, sizeof(KR_ObjectID)))
		return false;

	return true;
}

bool IArtefact::load(PIN_SaveFile &sf)
{
	if (! sf.GetData( (char * ) &m_carrierID, sizeof(KR_ObjectID)))
		return false;

	return true;
}

void IArtefact::loadNotify()
{
	if (!m_carrierID.isNUL())
		m_carrier = (ICarrier *) (g_arena.getContext()->queryInterface(m_carrierID,ICarrierIID));
	else
		m_carrier = NULL;
}

 //===========================================================================
class AttributeArtefact : public ct_Attribute
{
 public:
    CViewObjectModel*   m_cacheSkin              ;  // 
    virtual void        update(double ts);  
    unsigned long       m_rayColor;

    GR_HTEXTURE         m_coronaHText;
    unsigned long       m_coronaColor;
    int                 m_portalTable;
    virtual int         receiveEvent( KR_Event &e );
//{{ATTRIBUTE
    ct_AttrItem  m_array[15];
    double          m_radius                 ;  // 
    int             m_riceCnt                ;  // 
    ct_AttrStr      m_skinName               ;  // 
    int             m_lightColor             ;  // 
    int             m_brightness             ;  // 
    double          m_lightRadius            ;  // 
    int             m_rayRGB                 ;  // 
    int             m_useLight               ;  // 
    int             m_useRay                 ;  // 
    int             m_useCorona              ;  // 
    double          m_maxCoronaR             ;  // 
    int             m_coronaAlpha            ;  // 
    int             m_coronaRGB              ;  // 
    ct_AttrStr      m_coronaName             ;  // 
    double          m_coronaR                ;  // 

    AttributeArtefact()
    {
        m_radius             = 5;
        m_riceCnt            = 10;
        strncpy(m_skinName,"", sizeof( ct_AttrStr )-1 );
        m_lightColor         = LIGHT_COLOR_VIOLET;
        m_brightness         = 127;
        m_lightRadius        = 15;
        m_rayRGB             = 0xFFFFFF;
        m_useLight           = 1;
        m_useRay             = 1;
        m_useCorona          = 1;
        m_maxCoronaR         = 8;
        m_coronaAlpha        = 100;
        m_coronaRGB          = 0xFFFFFF;
        strncpy(m_coronaName,"corona.spr", sizeof( ct_AttrStr )-1 );
        m_coronaR            = 0.2;

        m_array[0].set("m_radius",m_radius);
        m_array[1].set("m_riceCnt",m_riceCnt);
        m_array[2].set("m_skinName",m_skinName);
        m_array[3].set("m_lightColor",m_lightColor);
        m_array[4].set("m_brightness",m_brightness);
        m_array[5].set("m_lightRadius",m_lightRadius);
        m_array[6].set("m_rayRGB",m_rayRGB);
        m_array[7].set("m_useLight",m_useLight);
        m_array[8].set("m_useRay",m_useRay);
        m_array[9].set("m_useCorona",m_useCorona);
        m_array[10].set("m_maxCoronaR",m_maxCoronaR);
        m_array[11].set("m_coronaAlpha",m_coronaAlpha);
        m_array[12].set("m_coronaRGB",m_coronaRGB);
        m_array[13].set("m_coronaName",m_coronaName);
        m_array[14].set("m_coronaR",m_coronaR);

        linkTable(m_array,15);
    }
//}}END_OF_ATTRIBUTE
};

 //==========================================================================
void ArtefactObj::Draw()
{
    if(  m_attr->m_useCorona  )
    {
         double r = Abs2(CViewObject::m_viewPointInvMx.Offset()-m_position)*m_attr->m_coronaR;

         if(  r > m_attr->m_maxCoronaR  )
              r = m_attr->m_maxCoronaR;

         m_corona.prepareToRender(
                                m_position,
                                r,
                                2<<16, 2<<16,
                                126<<16, 126<<16,
                                m_attr->m_coronaAlpha,
                                m_attr->m_coronaColor,
                                m_attr->m_coronaHText
                               );
         m_corona.Draw();
   }

   if(  m_useRay  )
   {
    CFVector3 np(m_position);
    CFVector3	v = CViewObject::m_viewPointDirSMx*np;

    if(  v.z < CViewObject::m_fFrontClip )
         return;

    double d_z = 1.0/v.z;

    int screen_x = Round(v.x*d_z),
	screen_y = Round(v.y*d_z);

    double t = Session::m_moment-m_startTime;

    double dt = t*(1.5);
           dt = dt - ((int)dt);
           dt *= (1.0/1.5);

    double len         = 15; 
    double screen_len  = len*CViewObject::m_viewPointScale.x*d_z;
    double width       = screen_len*0.05;

    int rayCnt = 20;
    t *= 0.5;
    for( int i = 0; i < rayCnt; ++i )
    {
         double dir = i * M_PI*2 / rayCnt;

         int screen_dx = cos(dir+t)*screen_len;
         int screen_dy = sin(dir+t)*screen_len;
         double nt = dt-((double)i)/rayCnt;
         if(  nt < 0  ) nt += 1;
         int transp = (int)(100*(1-nt));
         if(  transp < 0   ) transp = 0;
         else
         if(  transp > 255  ) transp = 255;

    
         GRDrawRay( screen_x, screen_y, 
                screen_x+screen_dx, 
                screen_y+screen_dy, m_rayColor,
                transp, (int)(65536*d_z), (float)width, nt);
    }
   }

   s_ViewDynamicObject::Draw();
}


int  AttributeArtefact::receiveEvent( KR_Event &e )
{
    return ct_Attribute::receiveEvent(e);
}


void  Artefact::moveTo  ( CFMatrix3x4 &m )
{
    m_orient = m;
    m_viewDynObj.m_position = m.Offset();
    setPosition( m_viewDynObj.m_position );
}

int  Artefact::attachTo( KR_ObjectID masterID, ICarrier *master )
{
    if(  master!=0  )
    {
         context->removeEvent(ARTEFACT_MOVE,getObjectID());
         m_carrierID = masterID;
         m_carrier   = master;
         return 1;
    }
    return 0;
}

void  Artefact::drop    ( CFMatrix3x4 &m, double  ts)
{
    if(  context==0  )
    {
         if(  m_carrier   )
              m_carrier->carrierOnRemoveArtefact();
         return;
    }

    moveTo( m );
    context->removeEvent(ARTEFACT_MOVE,getObjectID());
    m_dir             = CFVector3(0,-0.2,0);
    KR_Event event;
    event.label       = ARTEFACT_MOVE;
    event.source      = getObjectID();
    event.destination = getObjectID();
    event.timeStamp   = ts+DELTAT;
    issueEvent( event );
}


 //===========================================================================
strg_SUBJECT_TABLE_IMPLEMENTATION(Artefact,1)
strg_ATTRIBUTE_TABLE_IMPLEMENTATION(Artefact,"ArtefactAttr")

static ArtefactTable          __classTable;

 //============================================================
strg_CONSTRUCTOR_DYNVIEW(Artefact)
 {
    m_attr = 0;//&__defaultAttr;
 }

 //============================================================
Artefact::~Artefact()
 {
 }


 //============================================================
void Artefact::onView(double)
{
}


void Artefact::setArtefactAttr()
{
	
	ct_Attribute *attr = __attrTable.searchAttribute(m_artefactAttrID);
	if( attr==NULL )
		echo( "Artefact::receiveEvent: Unknown attribute %s",
		context->searchObject(m_artefactAttrID));
	else m_attr = (AttributeArtefact*)attr;
	
	m_skin.Attach(m_attr->m_cacheSkin);
	
	m_viewDynObj.BumpDef().fRadius = m_skin.Model()->Radius();
	m_viewDynObj.m_rayColor = m_attr->m_rayColor;
	m_viewDynObj.m_useRay = m_attr->m_useRay;
	
	m_viewDynObj.m_attr = m_attr;
}

//typedef bool (*ct_CallBack)(KR_ObjectID oID,void *userParam);

bool s_findPortal(KR_ObjectID oID, void *userParam )
{
   TPortalFind &p = *((TPortalFind*)(userParam));
   IPortal *portal = (IPortal*)(g_arena.context->queryInterface(oID,IPortalIID));
   if(  portal!=0  )
   {
        CFVector3 pos = portal->portalGetCoord();
        if(  p.id.isNUL())
        {
             p.id  = oID;
             p.pos = pos;
             p.dist2 = Abs2(p.myPos-pos);
        }
        else
        {
             double d2 = Abs2(p.myPos-pos);

             if(  d2 < p.dist2 )
             {
                  p.id  = oID;
                  p.pos = pos;
                  p.dist2 = d2;
             }
        }
   }
   return 1;
}

bool Artefact::findPortal( TPortalFind &pf )
{
   pf.myPos = getPos();
   pf.id    = KR_ObjectID::NUL();
   g_arena.userFind(m_attr->m_portalTable,s_findPortal,&pf);
   return (!pf.id.isNUL()) && pf.dist2 < 25*25;
}

 //============================================================
int Artefact::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case ARTEFACT_MOVE:
            {
            double curTime = event.timeStamp;
            event.timeStamp += DELTAT;
            issueEvent( event );

            TPortalFind pf;
            if( findPortal(pf) )
            {
                CFVector3 mpos = getPosition();
                CFVector3 v    = pf.pos-mpos;
                if(  Abs2(v) < 0.25  )
                {
                     ((IPortal*)(context->queryInterface(pf.id,IPortalIID)))
                     ->portalAddArtefact( getObjectID() );
                     break;
                }
                CFVector3 ofs  = Normal(v)*2.0*DELTAT;

                //setPosition( mpos +  );

                m_orient.TranslateL(ofs);
                m_viewDynObj.m_position = m_orient.Offset();
                setPosition(m_orient.Offset());
                break;
            }


            m_orient.TranslateL(m_dir*DELTAT);
            m_viewDynObj.m_position = m_orient.Offset();
            setPosition(m_orient.Offset());

            CFVector3 pos(m_orient.Offset());
            m_dir += CFVector3(0,-0.1,0);
            m_dir.x *= 0.98;
            m_dir.z *= 0.98;
            m_dir.y *= 0.995;


            double clzTime = 0;
            KR_ObjectID oID;

            if( checkCollision( 
                     pos,    // начало движения
                     m_dir,    // напрвление со скоростью
                     0.5, // радиус
                     DELTAT,  // время для проверки
                     getObjectID(),  // кого игнорировать
                     clzTime, // время, через которое стукнемся
                     oID      // объект, о который стукнемся
                   ) )
            {
                  event.label = ARTEFACT_CHANGEDIR;
                  event.timeStamp = curTime + clzTime;
                  issueEvent( event );
            }
            }
            break;

    case ARTEFACT_CHANGEDIR:
            m_dir = -m_dir;
            break;

    case KR_WAKE_UP:
            break;

    case KR_SET_ATTR:
         {
         m_viewDynObj.m_startTime = event.timeStamp;
	 	 event.data.open(EDO_READ)
		  .getObjectID(m_artefactAttrID)
		.close();


         //KR_ObjectID attrID;

		 setArtefactAttr();
         }
         break;

    case ARTEFACT_MOVETO:
         {
                CFVector3 ofs;

                event.data.open(EDO_READ)
                            .getDouble(ofs.x)
                            .getDouble(ofs.y)
                            .getDouble(ofs.z)
                          .close();

                m_orient.LoadIdentity()
                        .TranslateL(ofs);
                m_viewDynObj.m_position = m_orient.Offset();
                setPosition(m_orient.Offset());
         }
         break;

    case ARTEFACT_ATTACH:
         {
           KR_ObjectID masterID;

           event.data.open(EDO_READ)
                    .getObjectID(masterID)
                 .close();
           ICarrier *master = (ICarrier*)(context->queryInterface(masterID,ICarrierIID));
           if(  attachTo( masterID, master )  )
                master->carrierTakeArtefact( getObjectID(), (IArtefact*)this );
         }
         break;

    default: return 0;
    }
    return 1;
 }

 //============================================================
void Artefact::addNotify()
 {
    ct_Subject::addNotify();
    // insert your code this
    m_carrierID = KR_ObjectID::NUL();
    m_carrier   = 0;
 }

 //============================================================
void Artefact::removeNotify()
 {
    drop(m_orient,Session::m_moment);
    ct_Subject::removeNotify();
    // insert your code this
 }

 //============================================================
void Artefact::onRender(double)
{
    if(  m_attr->m_useLight  )
    {
         g_lightChain.add(  getPosition(), 
                            m_attr->m_lightColor, 
                            m_attr->m_brightness, 
                            m_attr->m_lightRadius );
    }
    CFMatrix3x4 &m = m_skin.GetDirModify();
    m = m_orient;
}                                           

strg_SUBJECT_DYNVIEW_IMPLEMENTATION(Artefact)


 //============================================================
CFVector3     Artefact::realPosition() {  return getPosition();  }



 /*************************************
  *
  *   AttributeArtefact implementation
  *
  *************************************/

 //============================================================

#define RGB_TO_LIST(col)  ((col)>>16), ((col)>>8)&255, (col)&255

void AttributeArtefact::update(double ts)
{
   strg_UPDATE_ATTRIBUTE_SKIN(m_skinName,m_cacheSkin,ts)
   if(  m_useRay  )
   m_rayColor =     GRTransparentColor(RGB_TO_LIST(m_rayRGB));

   if(  m_useCorona  )
   {
        m_coronaHText = g_loadSmoke( m_coronaName, NULL );
        m_coronaColor = GRTransparentColor(m_coronaRGB>>16,(m_coronaRGB>>8)&255,m_coronaRGB&255);
   }
   m_portalTable = g_arena.searchSeanceClassTable("Portal");
}


double Artefact::getPower   ()
{
   return 0;
}

int    Artefact::isFriend   ( const KR_ObjectID & )
{
   return 1;
}

double Artefact::getDamage  ()
{
   return 1;
}

void   Artefact::setDamage  ( double d, const CFVector3 &pos, double,
                                    KR_ObjectID )
{
    m_dir += Normal(getPosition()-pos)*(15+d*15);
}

double Artefact::desireShoot()
{
    return -1;
}

KR_ObjectID Artefact::getCommander()
{
    return m_commander;
}

void Artefact::setCommander(KR_ObjectID oID)
{
    m_commander = oID;
}


CFVector3  Artefact::getPos()
{
    return getPosition();
}

double     Artefact::getHAngle   ()
{
    return 0;
}

CFVector3  Artefact::getUpVector ()
{
    return CFVector3(0,1,0);
}

CFVector3  Artefact::getCenter   () // ╬ЄэюёшЄхы№эю 0 юс·хъЄр
{
    return CFVector3(0,0,0);
}

double     Artefact::getRadius   () // ╬ЄэюёшЄхы№эю ЎхэЄЁр
{
    return m_skin.Model()->Radius();
}

double     Artefact::getRadius0  () // ╬ЄэюёшЄхы№эю 0 юс·хъЄр
{
    return m_skin.Model()->Radius();
}

CFVector3  Artefact::getMoveDir  () // ═ряЁртыхэшх фтшцхэш 
{
    return Normal(m_dir);
}

double     Artefact::getMoveSpeed() // ╤ъюЁюёЄ№
{
   return Abs(m_dir);
}

void       Artefact::getMatrix   ( CFMatrix3x4 &m)
{
    m = m_orient;
}

double     Artefact::getMass	   ()	// ╠рёёр
{
    return 1;
}

TCCFMatrix3x4 &Artefact::GetDir	   ()
{
    return m_orient;
}

void Artefact::SetDir(TCSFMatrix3x4 &dir)
{
    m_orient = dir;
}


void *Artefact::queryInterface( int IID )
{
     switch(IID)
     {
     case IUnitIID:      return (IUnit*)this;
     case IArtefactIID: return (IArtefact*)this;
     case IDynamicObjectIID: return (IDynamicObject*)this;
     }
     return 0;
}



bool	Artefact::dump(PIN_SaveFile & sf)
{
		if (!ct_Subject::dump(sf) ||		    
			!IArtefact::dump(sf) ||		    
		    !sf.WriteData( (char *) & m_artefactAttrID, sizeof(ArtefactData)  ))
			return false;
	

		return true;
}

bool	Artefact::load(PIN_SaveFile & sf)
{
	
	if (!ct_Subject::load(sf) ||  		    
		!IArtefact::load(sf) ||  		    
		!sf.GetData( (char *) & m_artefactAttrID, sizeof(ArtefactData)  ))
		return false;
	

	return true;
}


void	Artefact::loadNotify()
{
	ct_Subject::loadNotify();	
	IArtefact::loadNotify();
	setArtefactAttr();
}

void  Artefact::artefactMove( const CFVector3 &dir )
{
     m_dir = dir;
}


/* End of file \OBASE\Artefact\Artefact.cpp */
