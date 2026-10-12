/*
 * File  : C:\NW\ARENA\OBASE\Fire\Fire.cpp
 * Autor :
 * Ver   1.0 
 */
#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "Fire.h"
#include "message/firemsg.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "kernel/h/session.h"
 //===========================================================================
class AttributeFire : public ct_Attribute
{
 public:

    enum
    {
        MAX_COLOR = 4
    };
    unsigned long m_color[MAX_COLOR];
    virtual void    update(double ts);
//{{ATTRIBUTE
    ct_AttrItem  m_array[9];
    int             m_onLand                 ;  // 
    double          m_timeOfLife             ;  // 
    double          m_timeIncrement          ;  // 
    int             m_maxBranchCnt           ;  //  
    double          m_radius                 ;  // 
    int             m_rgb0                   ;  // 
    int             m_rgb1                   ;  // 
    int             m_rgb2                   ;  // 
    int             m_rgb3                   ;  // 

    AttributeFire()
    {
        m_onLand             = 0;
        m_timeOfLife         = 2;
        m_timeIncrement      = 0.05;
        m_maxBranchCnt       = 50;
        m_radius             = 1;
        m_rgb0               = 0xFFFFFF;
        m_rgb1               = 0xFFFFFF;
        m_rgb2               = 0xFFFFFF;
        m_rgb3               = 0xFFFFFF;

        m_array[0].set("m_onLand",m_onLand);
        m_array[1].set("m_timeOfLife",m_timeOfLife);
        m_array[2].set("m_timeIncrement",m_timeIncrement);
        m_array[3].set("m_maxBranchCnt",m_maxBranchCnt);
        m_array[4].set("m_radius",m_radius);
        m_array[5].set("m_rgb0",m_rgb0);
        m_array[6].set("m_rgb1",m_rgb1);
        m_array[7].set("m_rgb2",m_rgb2);
        m_array[8].set("m_rgb3",m_rgb3);

        linkTable(m_array,9);
    }
//}}END_OF_ATTRIBUTE
};

 //===========================================================================
class FireTable : public ct_SubjectTable
{
 private:
    Fire *m_table;
 public:
    FireTable()
    {
        m_table = NULL;
        registerClass( "Fire" );
    }
    ~FireTable()
    {
        delete [] m_table;
        Fire::createFreeList();
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
    virtual  bool      isRendering();
};

 //===========================================================================
class AttributeTableFire : public ct_AttributeTable
{
 protected:
    AttributeFire *m_table;

 public:
    AttributeTableFire()
    {
       m_table = NULL;
       registerClass( "FireAttr" );
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static FireTable  __classTable;
static AttributeTableFire __attrTable;

 /*********************************
  *
  *   Fire implementation
  *
  *********************************/
 //===========================================================================
bool FireTable::isRendering()
 {
    return true;
 }

 //============================================================
Fire::Fire()
 {
    m_attr = 0;//&__defaultAttr;
 }

 //============================================================
Fire::~Fire()
 {
 }

 //============================================================
int Fire::receiveEvent( KR_Event &event )
 {
    KR_ObjectID oID;

    switch( event.label )
    {
    case fi_EVC_MOVING:
            if(  m_isVisible  )
            {
                 onMove( event.timeStamp );
                 event.timeStamp += m_attr->m_timeIncrement;
                 event.label      = fi_EVC_MOVING;
                 issueEvent( event );
                 m_moved = true;

            }
            else m_moved = false;
            break;

    case KR_WAKE_UP:
            break;

    case KR_SET_ATTR:
            s_ASSERTNQ("Fire:receiveEvent:case KR_SET_ATTR: do't supported");
            break;

    case fi_EVCMD_START:
     {
            m_prevTimeStamp  = event.timeStamp;
             //
             // По старту устанавливаются атрибуты и позиция.
             //
            event.data.open(EDO_READ)
                        .getObjectID(oID)
                        .getDouble(m_position.x)
                        .getDouble(m_position.y)
                        .getDouble(m_position.z)
                      .close();

            ct_Attribute *attr = __attrTable.searchAttribute(oID);
            if( attr==NULL )
                 echo( "Fire::receiveEvent: Unknown attribute %s",
                       context->searchObject(oID));
            else m_attr = (AttributeFire*)attr;

            if(  m_attr->m_onLand  )
            {
                 double height;
                 CFVector3 normal;
                 CViewScene::Current()->GetTerrain()->GetPlane(m_position,normal,height);
                 m_position.y = height;
            }
            m_moved = false;
            m_viewObj.m_radius = m_attr->m_radius;
     }
     break;

    case fi_EVCMD_END:
            break;

    default: return 0;
    }
    return 1;
 }

 //============================================================
void Fire::addNotify()
 {
    ct_Object::addNotify();
    // insert your code this
    m_branchList.m_next = &m_branchList;
    m_branchList.m_prev = &m_branchList;
    m_branchCnt  = 0;

    m_viewObj.m_master = this;
 }

 //============================================================
void Fire::removeNotify()
 {
    ct_Object::removeNotify();
    // insert your code this

    int deleteCommandCnt = 0;
    for( 
         FireBranch *b = m_branchList.m_next; 
         b != &m_branchList; 
         b = b->m_next 
       )
         delCommand( b, deleteCommandCnt );

    deleteBranches( deleteCommandCnt );
 }


 /*************************************
  *
  *   FireTable implementation
  *
  *************************************/

 //============================================================
void FireTable::allocObjects( int objectQnty )
 {
    m_table = new Fire[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void FireTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *FireTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"FireTable::getObjectPTR");
    return &(m_table[ index ]);
 }

 /*************************************
  *
  *   AttributeTable implementation
  *
  *************************************/

 //============================================================
void AttributeTableFire::allocObjects( int objectQnty )
 {
    m_table = new AttributeFire[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void AttributeTableFire::freeObjects()
 {
    delete [] m_table;
    m_table         = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *AttributeTableFire::getObjectPTR( int index )
 {
    s_ASSERT(index>=0 && index <m_maxObjectQnty,"AttributeTable::getObjectPTR");
    return &(m_table[ index ]);
 }

  /*****************************************
        
         Списковые команды

   *****************************************/

 //============================================================
void  Fire::createFreeList()
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

 //============================================================
void  Fire::addBranch( double time0 )
{
    if(  m_freeList != 0 && m_branchCnt < m_attr->m_maxBranchCnt )
    {
         FireBranch &br = *m_freeList;
         m_freeList = m_freeList->m_next;

         br.m_next = m_branchList.m_next;
         br.m_prev = &m_branchList;
         
         m_branchList.m_next->m_prev = &br;
         m_branchList.m_next         = &br;
         m_branchCnt++;


         br.m_phase = time0;
    }
}

 //============================================================
void  Fire::delBranch( FireBranch *node  )
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
void Fire::deleteBranches( int deleteCommandCnt )
{
    int i;
    for( i = 0; i < deleteCommandCnt; ++i )
         delBranch(m_branch[i].deleteCommand);
}


  /**************************************

             Визуализация

   **************************************/
 //============================================================
CFVector3 Fire::realPosition()
{
    return m_position;
}

 //============================================================
void s_FireObject::Draw()
{
   
    m_master->draw();
}

 //============================================================
void Fire::render( CViewDynamicList &list )
{
    m_viewObj.prepareToRender();
    list.Load( &m_viewObj );
}

 //============================================================
void Fire::endRender( CViewScene *scene )
{
    scene->RemoveLandDynamic( &m_viewObj );
}


 //============================================================
void Fire::draw()
 {
    int deleteCommandCnt = 0;
    double t = Session::m_moment;

    for( 
          FireBranch *br = m_branchList.m_next; 
          br != &m_branchList; 
          br = br->m_next )
    {
         double T = br->m_phase-t;

         if(  T > m_attr->m_timeOfLife )
              delCommand( br, deleteCommandCnt );
         else
         {
              CFVector3 np(m_position);
              CFVector3	v = CViewObject::m_viewPointDirSMx*np;
              double d_v = 1./v.z;
              if( v.z < CViewObject::m_fFrontClip ) continue;

              double width = 0.5;
              int screen_width  = Round(width*CViewObject::m_viewPointScale.x*d_v);
              int screen_x = Round(v.x*d_v),
		          screen_y = Round(v.y*d_v);

              GRDrawParticle(screen_x, screen_y, screen_width, 65536*d_v, br->m_color);   
         }
    }

    deleteBranches( deleteCommandCnt );
 }


void  Fire::onView(double time)
{
    if(  !m_moved )
    {
         KR_Event event;
         event.timeStamp  = time;
         event.label      = fi_EVC_MOVING;
         event.source     = getObjectID();
         event.destination= getObjectID();
         issueEvent( event );
         m_moved = true;
    }
}

void Fire::onMove( double  )
{
}

void  s_FireObject::prepareToRender()
{
   m_dynBase = m_dynBase1 = m_bump.start = m_pos;
   m_bump.fRadius = m_radius; // 1/sqrt(2)
   m_bump.vel = CFVector3(0,0,0);
   m_bump.fTime = 0;
}

void AttributeFire:update(double)
{
}

/* End of file C:\NW\ARENA\OBASE\Fire\Fire.cpp */