/*
 * File  : C:\NW\ARENA\OBASE\Fountain\Fountain.cpp
 * Author : Suavik
 * Ver   1.0 
 */
#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "Fountain.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/fountmsg.h"
#include "h/light.h"
#include "zav.h"

#include "storage\h\savefile.h"


#define MAX_ITER_WAIT 100



bool	FountBranch::dump(PIN_SaveFile & sf)
{
	PIN_SaveItemPrefix prefix;

	prefix.m_Type = PIN_SaveItemPrefix::IP_BRANCH;
	strcpy(prefix.m_Check,PIN_check);

	if (!sf.WriteData( (char *) & prefix, sizeof(PIN_SaveItemPrefix)) ||
		!sf.WriteData( (char *) &m_phase, sizeof(FountBranchData)) )
		return false;
		


	return true;
}


bool	FountBranch::load(PIN_SaveFile & sf)
{
	PIN_SaveItemPrefix prefix;	

	if (!sf.GetData((char *) & prefix, sizeof(PIN_SaveItemPrefix)))
		return false;

	if (strcmp(prefix.m_Check,PIN_check) != 0 ||
		prefix.m_Type != PIN_SaveItemPrefix::IP_BRANCH)
		return false;

	if (!sf.GetData( (char *) &m_phase, sizeof(FountBranchData)) )
		return false;

	return true;
}



FountBranch  Fountain::m_branch[MAX_BRANCH];
FountBranch *Fountain::m_freeList = 0;
 //===========================================================================
class AttributeFountain : public ct_Attribute
{
 public:
    enum
    {
        MAX_COLOR = 4
    };
    unsigned long m_color[MAX_COLOR];
    virtual void    update(double ts);
//{{ATTRIBUTE
    ct_AttrItem  m_array[33];
    int             m_RGB0                   ;  // 
    int             m_RGB1                   ;  // 
    int             m_RGB2                   ;  // 
    int             m_RGB3                   ;  // 
    double          m_timeIncrement          ;  // 
    double          m_minTimeAdd             ;  // 
    double          m_maxTimeAdd             ;  // 
    double          m_hAngle                 ;  // 
    double          m_vAngle                 ;  // 
    double          m_angleDelta             ;  // 
    double          m_minLifeTime            ;  // 
    double          m_maxLifeTime            ;  // 
    double          m_minSpeed               ;  // 
    double          m_maxSpeed               ;  // 
    double          m_radius                 ;  // 
    double          m_minrA                  ;  // 
    double          m_maxrA                  ;  // 
    double          m_minrB                  ;  // 
    double          m_maxrB                  ;  // 
    double          m_minrC                  ;  // 
    double          m_maxrC                  ;  // 
    double          m_viewHeight             ;  // 
    int             m_onLand                 ;  // 
    int             m_useLight               ;  // 
    int             m_lightColor             ;  // 
    double          m_minLightBright         ;  // 
    double          m_maxLightBright         ;  // 
    double          m_lightRadius            ;  // 
    double          m_lightBrightStep        ;  // 
    double          m_lightOffset            ;  // 
    int             m_maxBranchCnt           ;  // 
    int             m_isBlob                 ;  // 
    double          m_radSpeed               ;  // 

    AttributeFountain()
    {
        m_RGB0               = 0;
        m_RGB1               = 0;
        m_RGB2               = 0;
        m_RGB3               = 0;
        m_timeIncrement      = 0.05;
        m_minTimeAdd         = 0.004;
        m_maxTimeAdd         = 0.08;
        m_hAngle             = 0;
        m_vAngle             = 0;
        m_angleDelta         = 5.0/180.0*3.14;
        m_minLifeTime        = 2.2;
        m_maxLifeTime        = 2.6;
        m_minSpeed           = 5.0;
        m_maxSpeed           = 9.0;
        m_radius             = 1.0;
        m_minrA              = -0.3;
        m_maxrA              = -0.15;
        m_minrB              = 0.17;
        m_maxrB              = 0.2;
        m_minrC              = 0.1;
        m_maxrC              = 0.2;
        m_viewHeight         = 0.5;
        m_onLand             = 1;
        m_useLight           = 0;
        m_lightColor         = 7;
        m_minLightBright     = 20;
        m_maxLightBright     = 255;
        m_lightRadius        = 10.0;
        m_lightBrightStep    = 3.0;
        m_lightOffset        = 2.0;
        m_maxBranchCnt       = 200;
        m_isBlob             = 0;
        m_radSpeed           = 10;

        m_array[0].set("m_RGB0",m_RGB0);
        m_array[1].set("m_RGB1",m_RGB1);
        m_array[2].set("m_RGB2",m_RGB2);
        m_array[3].set("m_RGB3",m_RGB3);
        m_array[4].set("m_timeIncrement",m_timeIncrement);
        m_array[5].set("m_minTimeAdd",m_minTimeAdd);
        m_array[6].set("m_maxTimeAdd",m_maxTimeAdd);
        m_array[7].set("m_hAngle",m_hAngle);
        m_array[8].set("m_vAngle",m_vAngle);
        m_array[9].set("m_angleDelta",m_angleDelta);
        m_array[10].set("m_minLifeTime",m_minLifeTime);
        m_array[11].set("m_maxLifeTime",m_maxLifeTime);
        m_array[12].set("m_minSpeed",m_minSpeed);
        m_array[13].set("m_maxSpeed",m_maxSpeed);
        m_array[14].set("m_radius",m_radius);
        m_array[15].set("m_minrA",m_minrA);
        m_array[16].set("m_maxrA",m_maxrA);
        m_array[17].set("m_minrB",m_minrB);
        m_array[18].set("m_maxrB",m_maxrB);
        m_array[19].set("m_minrC",m_minrC);
        m_array[20].set("m_maxrC",m_maxrC);
        m_array[21].set("m_viewHeight",m_viewHeight);
        m_array[22].set("m_onLand",m_onLand);
        m_array[23].set("m_useLight",m_useLight);
        m_array[24].set("m_lightColor",m_lightColor);
        m_array[25].set("m_minLightBright",m_minLightBright);
        m_array[26].set("m_maxLightBright",m_maxLightBright);
        m_array[27].set("m_lightRadius",m_lightRadius);
        m_array[28].set("m_lightBrightStep",m_lightBrightStep);
        m_array[29].set("m_lightOffset",m_lightOffset);
        m_array[30].set("m_maxBranchCnt",m_maxBranchCnt);
        m_array[31].set("m_isBlob",m_isBlob);
        m_array[32].set("m_radSpeed",m_radSpeed);

        linkTable(m_array,33);
    }
//}}END_OF_ATTRIBUTE
};

static AttributeFountain __defaultAttr;

 //===========================================================================
class FountainTable : public ct_SubjectTable
{
 private:
    Fountain *m_table;
 public:
    FountainTable()
    {
        m_table = NULL;
        registerClass( "Fountain" );
        Fountain::createFreeList();
    }
    ~FountainTable()
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
class AttributeTableFountain : public ct_AttributeTable
{
 protected:
    AttributeFountain *m_table;

 public:
    AttributeTableFountain()
    {
       m_table = NULL;
       registerClass( "FountainAttr" );
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static FountainTable  __classTable;
static AttributeTableFountain __attrTable;
 /*********************************
  *
  *   Fountain implementation
  *
  *********************************/

 //============================================================
Fountain::Fountain()
 {
    m_attr = 0;//&__defaultAttr;
 }

 //============================================================
Fountain::~Fountain()
 {
 }


void Fountain::setFountainAttr()
{
	ct_Attribute *attr = __attrTable.searchAttribute(m_fountainAttrID);
	if( attr==NULL )
		echo( "Fountain::receiveEvent: Unknown attribute %s",
		context->searchObject(m_fountainAttrID));
	else 
		m_attr = (AttributeFountain*)attr;
}

 //============================================================
int Fountain::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case KR_WAKE_UP:
            break;

    case KR_SET_ATTR:
            {
            //KR_ObjectID oID;
            event.data.open(EDO_READ)
                        .getObjectID(m_fountainAttrID)
                      .close();

			setFountainAttr();
            
            }
            break;

    case fou_EVCMD_START:
            {
            m_prevTimeStamp  = event.timeStamp;
             //
             // ѕо старту устанавливаютс€ атрибуты и позици€.
             // «апускаютс€ два цикла - создание веток и перемещение
             //
            //KR_ObjectID oID;
            event.data.open(EDO_READ)
                        .getObjectID(m_fountainAttrID)
                        .getDouble(m_fountainPosition.x)
                        .getDouble(m_fountainPosition.y)
                        .getDouble(m_fountainPosition.z)
                      .close();

			setFountainAttr();          

            if(  m_attr->m_onLand  )
            {
                 double height;
                 CFVector3 normal;
                 CViewScene::Current()->GetTerrain()->GetPlane(m_fountainPosition,normal,height);
                 m_fountainPosition.y = height;
            }

            m_brightness = m_attr->m_minLightBright;

            KR_Event ev;

            ev.label      = fou_EV_ADD_BRANCH;
            ev.source     = getObjectID();
            ev.destination= event.source;
            ev.timeStamp  = m_prevTimeStamp;
            receiveEvent( ev );

            ev.label      = fou_EVC_MOVING;
            ev.source     = getObjectID();
            ev.timeStamp  = m_prevTimeStamp;
            ev.destination= event.source;
            receiveEvent( ev );

            }
            break;

    case fou_EV_ADD_BRANCH:
            if(  !m_isVisible   )
                 event.timeStamp += 2.12+m_addSlipPeriod;
            else event.timeStamp += context->rnd_f(m_attr->m_minTimeAdd, m_attr->m_maxTimeAdd);

            addBranch(event.timeStamp-m_prevTimeStamp);
            
            event.label       = fou_EV_ADD_BRANCH;
            event.destination = getObjectID();
            event.source      = event.destination;
            issueEvent( event );
            break;

    case fou_EVC_MOVING:
            {
            double deltaT = event.timeStamp - m_prevTimeStamp;

            int deleteCommandCnt = 0;

            for( 
                 FountBranch *b= m_branchList.m_next; 
                 b != &m_branchList ; 
                 b = b->m_next )
            {
                 b->m_phase += deltaT;
                 moving( *b, deleteCommandCnt );
            }

            int i;
            for( i = 0; i < deleteCommandCnt; ++i )
                 delBranch(m_branch[i].deleteCommand);

            if(  m_attr->m_useLight  )
            {
                 m_brightness +=  context->rnd_f(m_attr->m_lightBrightStep) 
                                - m_attr->m_lightBrightStep/2;
                 if(  m_brightness < m_attr->m_minLightBright  )
                      m_brightness = m_attr->m_minLightBright;
                 else
                 if(  m_brightness > m_attr->m_maxLightBright  )
                      m_brightness = m_attr->m_maxLightBright;
                      
            }


            m_prevTimeStamp = event.timeStamp;
            if(  !m_isVisible  )
                 event.timeStamp += 1.1+m_addSlipPeriod;
            else event.timeStamp += m_attr->m_timeIncrement;

            event.label       = fou_EVC_MOVING;
            event.destination = getObjectID();
            issueEvent( event );
            }
            break;

    case fou_EVCMD_END:
            context->removeEvent (fou_EVC_MOVING,getObjectID());
            context->removeEvent (fou_EV_ADD_BRANCH,getObjectID());
            context->removeObject(getObjectID());
            break;
    default: return 0;
    }
    return 1;
 }


FountBranch * Fountain::addBranch()
{
    if(  m_freeList != 0 && m_branchCnt < m_attr->m_maxBranchCnt )
    {
         FountBranch &br = *m_freeList;
         m_freeList = m_freeList->m_next;

         br.m_next = m_branchList.m_next;
         br.m_prev = &m_branchList;
         
         m_branchList.m_next->m_prev = &br;
         m_branchList.m_next         = &br;
         m_branchCnt++;

		 return & br;
	}


	return NULL;
}



 //============================================================
void  Fountain::addBranch( double time0 )
{
    if(  m_freeList != 0 && m_branchCnt < m_attr->m_maxBranchCnt )
    {
         FountBranch &br = *m_freeList;
         m_freeList = m_freeList->m_next;

         br.m_next = m_branchList.m_next;
         br.m_prev = &m_branchList;
         
         m_branchList.m_next->m_prev = &br;
         m_branchList.m_next         = &br;
         m_branchCnt++;




         br.m_phase = time0;

         double hA = context->rnd_f(2*M_PI);
         double vA = context->rnd_f(m_attr->m_angleDelta);

         double sin_V = sin(vA);
         CFVector3 delta(cos(hA)*sin_V, cos(vA), sin(hA)*sin_V);
         
         CFMatrix3x4 m;
         m.LoadIdentity();
         m.RotateOzL(m_attr->m_vAngle);
         m.RotateOyL(m_attr->m_hAngle);

         delta = m*delta;
         br.xT = delta.x;
         br.yT = delta.y;
         br.zT = delta.z;

         double speed = context->rnd_f(m_attr->m_minSpeed, m_attr->m_maxSpeed);
         br.xT *= speed;
         br.yT *= speed;
         br.zT *= speed;

         br.rA = context->rnd_f( m_attr->m_minrA, m_attr->m_maxrA );
         br.rB = context->rnd_f( m_attr->m_minrB, m_attr->m_maxrB );
         br.rC = context->rnd_f( m_attr->m_minrC, m_attr->m_maxrC );

         br.m_timeOfLife = context->rnd_f(m_attr->m_minLifeTime, m_attr->m_maxLifeTime);
         br.m_color = m_attr->m_color[context->rnd_i()&3];
    }
}

 //============================================================
void  Fountain::delBranch( FountBranch *node  )
{
     if(  node != 0  )
     {
          node->m_prev->m_next = node->m_next;
          node->m_next->m_prev = node->m_prev;

          node->m_next = m_freeList;
          m_freeList   = node;
          m_branchCnt--;
     }
}

 //============================================================
void  Fountain::moving( FountBranch &br, int &dc )
{
     if(  br.m_phase >= br.m_timeOfLife  )
          delCommand( &br, dc );
}

 //============================================================
void Fountain::addNotify()
 {
    ct_Object::addNotify();
    // insert your code here
    m_branchList.m_next = &m_branchList;
    m_branchList.m_prev = &m_branchList;
    m_branchCnt  = 0;

    m_viewObj.m_master = this;
    m_addSlipPeriod = context->rnd_f(0.3);
 }

 //============================================================
void Fountain::removeNotify()
 {
    ct_Object::removeNotify();
    // insert your code here

    int deleteCommandCnt = 0;
    for( 
         FountBranch *b = m_branchList.m_next; 
         b != &m_branchList; 
         b = b->m_next 
       )
         delCommand( b, deleteCommandCnt );

    int i;
    for( i = 0; i < deleteCommandCnt; ++i )
         delBranch(m_branch[i].deleteCommand);
 }

 //============================================================
void Fountain::draw(CDC &)
 {
 }

 /*************************************
  *
  *   FountainTable implementation
  *
  *************************************/
bool FountainTable::isRendering()
{
    return 1;
}

 //============================================================
void FountainTable::allocObjects( int objectQnty )
 {
    m_table = new Fountain[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void FountainTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *FountainTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"FountainTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 /*************************************
  *
  *   AttributeTable implementation
  *
  *************************************/

 //============================================================
void AttributeTableFountain::allocObjects( int objectQnty )
 {
    m_table = new AttributeFountain[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void AttributeTableFountain::freeObjects()
 {
    delete [] m_table;
    m_table         = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *AttributeTableFountain::getObjectPTR( int index )
 {
    s_ASSERT(index>=0 && index <m_maxObjectQnty,"AttributeTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 //============================================================
#define RGB_TO_LIST(col)  ((col)>>16), ((col)>>8)&255, (col)&255

void  AttributeFountain::update(double)
{
  m_color[0] = GRCreateColor(RGB_TO_LIST(m_RGB0));
  m_color[1] = GRCreateColor(RGB_TO_LIST(m_RGB1));
  m_color[2] = GRCreateColor(RGB_TO_LIST(m_RGB2));
  m_color[3] = GRCreateColor(RGB_TO_LIST(m_RGB3));
}


void Fountain::draw()
{
    int deleteCommandCnt = 0;
    double wl = ZAV_Scene()->GetTerrain()->Waterline();

    for( 
          FountBranch *br = m_branchList.m_next; 
          br != &m_branchList; 
          br = br->m_next )
    {
         double T = br->m_phase, T2 = T*T;

         double width = br->rA*T2 + br->rB*T + br->rC;
         if(  width <= 0.001  )
              delCommand( br, deleteCommandCnt );
         else
         {
              double y;
              
              if(  m_attr->m_isBlob  )
              {
                   y = br->yT*T+width*m_attr->m_radSpeed;
                   if(  y > wl-m_fountainPosition.y  )
                   {
                        y = wl-m_fountainPosition.y;
                        delCommand( br, deleteCommandCnt );
                   }
              }
              else y = br->yT*T - 9.8/2*T2;

              if(  y >= m_attr->m_viewHeight  )
              {
                   CFVector3 np(m_fountainPosition+CFVector3(br->xT*T, y, br->zT*T ));
                   CFVector3	v = CViewObject::m_viewPointDirSMx*np;
                   double d_v = 1./v.z;
                   if( v.z < CViewObject::m_fFrontClip ) continue;

                   int screen_width  = Round(width*CViewObject::m_viewPointScale.x*d_v);
                   int screen_x = Round(v.x*d_v),
		               screen_y = Round(v.y*d_v);

                   GRDrawParticle(screen_x, screen_y, screen_width, 65536*d_v, br->m_color);   
              }
         }
    }

    int i;
    for( i = 0; i < deleteCommandCnt; ++i )
         delBranch(m_branch[i].deleteCommand);

    if(  m_attr->m_useLight  )
         g_lightChain.add( m_fountainPosition+CFVector3(0,m_attr->m_lightOffset,0), 
                      m_attr->m_lightColor, 
                      (int)(m_brightness), 
                      m_attr->m_lightRadius );

}

void s_FountainObject::prepareToRender()
{
    double y = m_master->m_fountainPosition.y + m_master->m_attr->m_radius;
    m_dynBase = m_bump.start = m_master->m_fountainPosition;
    m_dynBase.y    = m_bump.start.y = y;
    m_bump.fRadius = m_master->m_attr->m_radius;
    m_bump.vel     = CFVector3(0,0,0);
    m_bump.fTime   = 0;
    m_dynBase1     = m_dynBase;
    m_bump.nBumpFlags  = 0;
}

void s_FountainObject::Draw()
{
   
    m_master->draw();
}

void Fountain::render( CViewDynamicList &list, double )
{
    m_viewObj.prepareToRender();
    list.Load( &m_viewObj );
}

void Fountain::endRender( CViewScene *scene )
{
    scene->RemoveLandDynamic( &m_viewObj );
}

CFVector3 Fountain::realPosition()
{
    return m_fountainPosition;
}

void  Fountain::createFreeList()
{
    for( int i = 0; i < MAX_BRANCH; ++i )
    {
         m_branch[i].m_next = &m_branch[i+1];
         m_branch[i].m_prev = 0;
         m_branch[i].deleteCommand = 0;
    }
    m_branch[MAX_BRANCH-1].m_next = 0;
    m_freeList = m_branch;
}



bool	Fountain::dump(PIN_SaveFile & sf)
{
		if (!ct_Subject::dump(sf) ||		    
		    !sf.WriteData( (char *) & m_fountainAttrID, sizeof(FountainData)  ))
			return false;


		for( 
          FountBranch *br = m_branchList.m_next; 
          br != &m_branchList; 
          br = br->m_next )
		{	
			if (!br->dump(sf))
				return false;
		}
		

		return true;
}

bool	Fountain::load(PIN_SaveFile & sf)
{

    m_branchList.m_next = &m_branchList;
    m_branchList.m_prev = &m_branchList;
    m_branchCnt  = 0;
    m_viewObj.m_master = this;

	
	if (!ct_Subject::load(sf) ||  		    
		!sf.GetData( (char *) & m_fountainAttrID, sizeof(FountainData)  ))
		return false;
	
	int type;
	
	while ( (type = sf.GetNextDataItemType()) == PIN_SaveItemPrefix::IP_BRANCH)
	{
		FountBranch * theBranch = addBranch();
		
		if (!theBranch)
			return false;
		
		if (!theBranch->load(sf))
			return false;
	}
	
	return true;
}


void	Fountain::loadNotify()
{
	ct_Subject::loadNotify();	
	setFountainAttr();
}

/* End of file C:\NW\ARENA\OBASE\Fountain\Fountain.cpp */
