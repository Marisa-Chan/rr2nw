/*
 * File  : D:\GAME\OBASE\DCross\DCross.cpp
 * Autor :
 * Ver   1.0 
 */

#define LAST_H__VIEW
#include "game.h"
#include "scene.h"
#include "..\DynObj\DynObj.h"
#include "storage/h/subject.h"
#include "kernel/h/active.h"
#include "message/dcrossmsg.h"
#include "mathlib.h"

#include "DCross.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/dcrossmsg.h"
#include "enum/spaceEnum.h"
#include "message/skinmsg.h"
#include "kernel/h/session.h"

#ifdef  __TRACE_NW__
#include "afxwin.h"
#else
class CDC{};
typedef unsigned long COLORREF;
#endif
extern void D3D_DrawZList();

 //===========================================================================
class DCrossTable : public ct_SubjectTable
{
 private:
    DCross *m_table;

 public:
    DCrossTable()
    {
        m_table = NULL;
        registerClass( "DebugCross" );
    }
    ~DCrossTable()
    {
        delete [] m_table;
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
    virtual bool       isRendering () { return true; }
};

static DCrossTable  __classTable;
 /*********************************
  *
  *   DCross implementation
  *
  *********************************/

 //============================================================
DCross::DCross()
    : m_viewDynObj(m_skin)
 {
     startInitialize();
 }

 //============================================================
DCross::~DCross()
 {
 }

 //============================================================
int DCross::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case KR_WAKE_UP:
            break;
    
    case dc_EV_CREATE:
            {
            {
            CViewObjectModel  *cacheSkin = 0;
            KR_ObjectID skinID =  context->searchObject( "sk.DebugCross" );
            s_ASSERT( !skinID.isNUL(), "DCross" );

            KR_Event ev;
            ev.timeStamp   = 0.1;
            ev.label       = sk_EV_QUERY_MODEL_PTR;
            ev.destination = skinID;
            context->sendEventNow( ev );
            s_ASSERT(ev.label==sk_EV_QUERY_MODEL_PTR_OK,"");
            ev.data.open(EDO_READ)
                .get(&cacheSkin,sizeof(void*))
              .close();

            m_skin.Attach(cacheSkin);
            m_viewDynObj.BumpDef().fRadius = m_skin.Model()->Radius();
            }

            COLORREF color;
            double xPos,yPos,zPos;
            //{{GET_EVENT(dc_EV_CREATE)
            event.data.open(EDO_READ)
                           .getInt(m_type)
                           .getDouble(removeTime)
                           .getULong(color)
                           .descend( RECT2D_I, 0 )
                             .getInt(x0)
                             .getInt(y0)
                             .getInt(x1)
                             .getInt(y1)
                           .ascend()
                           .descend( VECTOR3D_F, 0 )
                             .getDouble(xPos)
                             .getDouble(yPos)
                             .getDouble(zPos)
                           .ascend()
                           .getStr(text,sizeof(text))
                           .getInt(xText)
                           .getInt(yText)
                      .close();
            //}}END_OF_GET_EVENT(dc_EV_CREATE)
            m_position = CFVector3(xPos,yPos,zPos);
            m_color    = color;
            
            event.label       = dc_EV_REMOVE;
            event.source      = getObjectID();
            event.destination = getObjectID();
            event.timeStamp  += removeTime;
            issueEvent( event );
            }
            break;

    case dc_EV_REMOVE:
            context->removeObject(getObjectID());
            break;

    default: return 0;
    }
    return 1;
 }

 //============================================================
void DCross::addNotify()
 {
    ct_Object::addNotify();
    // insert your code this
 }

 //============================================================
void DCross::removeNotify()
 {
    ct_Object::removeNotify();
    // insert your code this
 }

 //============================================================
#ifdef  __TRACE_NW__
void DCross::draw( CDC &dc )
 {
     int x = (int) m_position.x,
         y = (int)-m_position.z;

     CBrush brush(m_color);
     CPen   pen(PS_SOLID,1,m_color);
     CPen *oldPen = dc.SelectObject(&pen);

     switch(  m_type )
     {
     case dc_CROSS:
              {
              dc.MoveTo(x+x0,y+y0);
              dc.LineTo(x+x1,y+y1);

              dc.MoveTo(x+x1,y+y0);
              dc.LineTo(x+x0,y+y1);
              }
              break;

     case dc_CROSS_POINT:
              dc.SetPixel(x,y,m_color);
              break;

     case dc_CROSS_CIRCLE:
              {
              dc.Ellipse(x+x0,y+y0,x+x1,y+y1);
              }
              break;

     case dc_CROSS_RECT:
              {
              RECT rect;
              rect.left   = x0+x;
              rect.top    = y0+y;
              rect.right  = x1+x;
              rect.bottom = y1+y;
              dc.FillRect(&rect,&brush);
              }
              break;

     case dc_CROSS_LINE:
              dc.MoveTo(x+x0,y+y0);
              dc.LineTo(x+x1,y+y1);
              break;
     }

     if(  text != NULL  )
          dc.TextOut(x+xText,y+yText,text,strlen(text));
     dc.SelectObject(oldPen);
 }
#else
void DCross::draw( CDC & ) {}
#endif

CFVector3 DCross::realPosition()
 {
     return m_position;
 }

 /*************************************
  *
  *   DCrossTable implementation
  *
  *************************************/

 //============================================================
void DCrossTable::allocObjects( int objectQnty )
 {
    m_table = new DCross[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void DCrossTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *DCrossTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"DCrossTable::getObjectPTR");
    return &(m_table[ index ]);
 }

void dc_CreateCross(int type,const CFVector3 &p,double removeTime,int x0,int y0, int x1,int y1,
                    unsigned long color,const char *text,int xText,int yText)
 {
     KR_Event        event;
     KR_ObjectID     newID;
     static ct_ClassTableID ctID = -1;

     double xPos = p.x,
            yPos = p.y,
            zPos = p.z;

     static bool first = true;

     if(  first  )
     {
          ctID = g_arena.searchSeanceClassTable( "DebugCross" );
          first = false;
     }

     newID = g_arena.newObject(
                          ctID,
                          "Cross"
                         );

     if(  newID.isNUL()  ) return;
     double height;
     CFVector3 normal;
     CViewScene::Current()->GetTerrain()->GetPlane(CFVector3(xPos,yPos,zPos),normal,height);
     if(  yPos < height+2  )
          yPos = height+2;


     event.label = dc_EV_CREATE;
     event.timeStamp = Session::m_realTimer->GetTime();
     event.destination = newID;
     event.data.open(EDO_WRITE)
                    .putInt(type)
                    .putDouble(removeTime)
                    .putULong(color)
                    .descend( RECT2D_I, 0 )
                      .putInt(x0)
                      .putInt(y0)
                      .putInt(x1)
                      .putInt(y1)
                    .ascend()
                    .descend( VECTOR3D_F, 0 )
                      .putDouble(xPos)
                      .putDouble(yPos)
                      .putDouble(zPos)
                    .ascend()
                    .putStr(text)
                    .putInt(xText)
                    .putInt(yText)
               .close();



     g_arena.context->sendEventNow(event);
 }

void DCross::render   ( CViewDynamicList &list, double )
{
    CFMatrix3x4 &m = m_skin.GetDirModify();
    m.LoadIdentity();
    m.TranslateL( m_position );

    m_viewDynObj.prepareToRender();
    list.Load( &m_viewDynObj );
}

extern CFixedColorFont	font5;
 //============================================================

void DCross::endRender( CViewScene *scene )
{
    CFVector3	v = CViewObject::m_viewPointDirSMx*m_position;
    if( v.z < CViewObject::m_fFrontClip ) return;
    double d_v = 1./v.z;
    int		screen_x = Round(v.x*d_v),
		    screen_y = Round(v.y*d_v);

    font5.PrintClipColorAt(screen_x, screen_y,text,GRFillColor(255,255,255));
    
    scene->RemoveLandDynamic( &m_viewDynObj );
}



void drawLine(CFVector3 start,CFVector3 end,int rad,int color, int , int )
{
/*
    CFVector3 dir = Normal(end-start)*0.4;
    unsigned color = GRCreateColor(r,g,b);
    int cnt = Abs(start-end)*(1/0.4);

    for(int i=0; i<cnt;++i)
    {
        CFVector3 v = CViewObject::m_viewPointDirSMx*(start+dir*i);
        if( v.z < CViewObject::m_fFrontClip ) continue;
        double d_v = 1./v.z;

        int screen_x = Round(v.x*d_v),
            screen_y = Round(v.y*d_v);
        GRDrawParticle(screen_x, screen_y, rad, 65536*d_v, color); 
    }

    if (GRIsHardware()) D3D_DrawZList();
 */
    CFVector3 v0 = CViewObject::m_viewPointDirSMx*start;
    if( v0.z < CViewObject::m_fFrontClip ) return;
    double d_v0 = 1./v0.z;

    CFVector3 v1 = CViewObject::m_viewPointDirSMx*end;
    if( v1.z < CViewObject::m_fFrontClip ) return;
    double d_v1 = 1./v1.z;


    int x0 = Round(v0.x*d_v0),
        y0 = Round(v0.y*d_v0);

    int x1 = Round(v1.x*d_v1),
        y1 = Round(v1.y*d_v1);

    int sx= x1-x0,sy=y1-y0;
    int dx = abs(sx);
    int dy = abs(sy);
 
    if(  sx < 0 )  sx = -1;
    else 
    if(  sx > 0 )  sx = 1;
    else sx = 0;
   
    if(  sy < 0 )  sy = -1;
    else 
    if(  sy > 0 )  sy = 1;
    else sy = 0;
   
    int i,x=x0,y=y0,s=0;

    d_v0 *=65536;
    if(  dx>=dy  )
    {
         for(i = 0; i < dx; ++i)
         {
             if( !(i&3) )
             GRDrawParticle(x, y, rad, d_v0, color); 
             s+=dy;
             if( s>=dx )
             {
                 s-=dx; 
                 y += sy;
             }
             x+=sx;
         }
    }
    else
    {
         for(i = 0; i < dy; ++i)
         {
             if( !(i&3) )
             GRDrawParticle(x, y, rad, d_v0, color); 
             s+=dx;
             if( s>=dy )
             {
                 s-=dy; 
                 x += sx;
             }
             y+=sy;
         }
    }


}

void drawPoint(CFVector3 pos, int rad, int color, int , int )
{
    CFVector3 v = CViewObject::m_viewPointDirSMx*pos;
    if( v.z < CViewObject::m_fFrontClip ) return;
    double d_v = 1./v.z;
    int		screen_x = Round(v.x*d_v),
		    screen_y = Round(v.y*d_v);
    
    GRDrawParticle(screen_x, screen_y, rad, 65536*d_v, color);   
}

/* End of file D:\GAME\OBASE\DCross\DCross.cpp */
