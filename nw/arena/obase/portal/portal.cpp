/*
 * File  : C:\WinGame\OBASE\portal\portal.cpp
 * Autor :
 * Ver   1.0 
 */

#include "portal.h"
#include "vehicle.h"

#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"

#include "message/bimsg.h"
#include "message/skinmsg.h"
#include "message/unitmsg.h"
#include "message/dcrossmsg.h"

#include "storage/h/savefile.h"
#include "i/carrier.i"

//#include <afxwin.h>
CFMatrix3x4 Portal::dummy;

 //===========================================================================
class AttributePortal : public ct_Attribute
{
 public:
    CViewObjectModel*   m_cacheSkin              ;  // 
    virtual void    update(double ts);  

//{{ATTRIBUTE
    ct_AttrItem  m_array[2];
    double          m_activRadius            ;  // 
    int             m_slotCnt                ;  // 

    AttributePortal()
    {
        m_activRadius        = 15;
        m_slotCnt            = 4;

        m_array[0].set("m_activRadius",m_activRadius);
        m_array[1].set("m_slotCnt",m_slotCnt);

        linkTable(m_array,2);
    }
//}}END_OF_ATTRIBUTE
};

strg_ATTRIBUTE_TABLE_IMPLEMENTATION(Portal,"PortalAttr")

class PortalTable : public ct_SubjectTable
{
 private:
  Portal *m_table;
 public:
    PortalTable()
    {
      m_table = NULL;
      registerClass("Portal");
    }
    ~PortalTable()
    {
      delete [] m_table;
      m_table = NULL;
    }
    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static AttributePortal __defaultAttr;

 //===========================================================================
static PortalTable          __classTable;
 /*********************************
  *
  *   Portal implementation
  *
  *********************************/

Portal::Portal()
 {
    m_attr = &__defaultAttr;
	m_portalAttrID = KR_ObjectID::NUL();
 }

 //============================================================
Portal::~Portal()
 {
 }


void Portal::setPortalAttr()
{
	if (!m_portalAttrID.isNUL() )
	{
		ct_Attribute *attr = __attrTable.searchAttribute(m_portalAttrID);
		if( attr==NULL )
			echo( "Portal::receiveEvent: Unknown attribute %s",
			context->searchObject(m_portalAttrID));
		else 
			m_attr = (AttributePortal*)attr;
	}
	 else
	m_attr = &__defaultAttr;
	
	m_slotCnt         = m_attr->m_slotCnt;
	m_occupiedSlotCnt = 0;
}

 //============================================================
int Portal::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case KR_WAKE_UP:
            break;

    case t_EV_ONCOLLISION:
            {
                KR_ObjectID oID;
                event.data.open(EDO_READ)
                            .getObjectID(oID)
                          .close();

                if(    g_vehicle->getObjectID() == oID  
                    && m_slotCnt==m_occupiedSlotCnt  )
                {
                    printf("Go to next level");
                }
            }
            break;

    case KR_SET_ATTR:
            {
              event.data.open(EDO_READ)
                          .getObjectID(m_portalAttrID)
                        .close();
              setPortalAttr();
            }
            break;

    default: return 0;
    }
    return 1;
 }


 //============================================================
void Portal::addNotify()
 {
    ct_Subject::addNotify();
    // insert your code here
    m_slotCnt         = m_attr->m_slotCnt;
    m_occupiedSlotCnt = 0;
 }

 //============================================================
void Portal::removeNotify()
 {
    ct_Subject::removeNotify();
    // insert your code here
 }


bool	Portal::dump(PIN_SaveFile & sf)
{

    if (!ct_Subject::dump(sf))
        return false;

    if (!sf.WriteData( (char *) & m_portalAttrID, sizeof(PortalData)  ))
        return false;

    return true;
}

bool	Portal::load(PIN_SaveFile & sf)
{
    if (!ct_Subject::load(sf))
         return false;

    if (!sf.GetData( (char *) & m_portalAttrID, sizeof(PortalData)  ))
        return false;
		
    return true;
}

void	Portal::loadNotify()
{
	ct_Subject::loadNotify();
	setPortalAttr();
}

strg_TABLE_FUNC_IMPLEMENTATION(PortalTable,Portal)

 /*************************************
  *
  *   AttributePortal implementation
  *
  *************************************/

 //============================================================


void AttributePortal::update(double)
{
}

void *Portal::queryInterface(int IID)
{
   switch(IID)
   {
   case IUnknownIID:       return (KR_Object*)this;
   case IPortalIID:        return (IPortal*)this;
   case IDynamicObjectIID: return (IDynamicObject*)this;
   }
   return 0;
}
 

CFVector3 Portal::portalGetCoord()
{
   return m_pos;
}

int Portal::portalGetSlotCnt()
{
   return m_slotCnt;
}

int Portal::portalGetOccupiedSlot()
{
   return m_occupiedSlotCnt;
}

void  Portal::portalInit(CFVector3 pos, int sc)    
{
//    KR_ObjectID m_portalAttrID;
    m_pos     = pos;
    m_slotCnt = sc;
    m_occupiedSlotCnt = 0;
}


void Portal::portalAddArtefact( KR_ObjectID artID )
{
    IArtefact *iart = (IArtefact*)(context->queryInterface(artID,IArtefactIID));
    if(  iart==0  )
    {
         echo( "Portal::portalAddArtefact() This is not artefact!" );
         return;
    }
    context->removeObject( artID );
    if(  m_occupiedSlotCnt < m_slotCnt  )
         m_occupiedSlotCnt++;

}

void Portal::portalSetPortalPoint(CFVector3 pos)
{
    setPosition(pos);
}

void PortalCallback( CViewObjectBaseSet *pBaseSet,
                      CViewObjectBase    *pBase,
                      CViewObjectRef     *pRef
                    )
{             
	(void)pBaseSet;
	(void)pBase;
	(void)pRef;
/*
        const char *name[] = 
        {
          "0", "1", "2", "3", "4", "Total"
        };
          

	IPortal *iport = (IPortal*)pRef->GetUserAttrib();
        int i,
            close = m_slotCnt-m_occupiedSlotCnt,
            open  = m_occupiedSlotCnt;

        for( i = 0;  i < open; ++i )
        {
             pBase->KFSet().Mod0( name[i] )
                  .LoadIdentity();
        }
*/
/*
&pBase.KFSet().Mod0("Body");

	pData->vbmi0->LoadIdentity()             
      .RotateOz(0.4 * sin(Session::m_moment*pData->speed * 2.), pData->axis0)
      .Update();
       	pData->vbmi1->LoadIdentity()             
      .RotateOz(Session::m_moment*pData->speed * 1.5,pData->axis1)
      .Update();
	pData->vbmi2->LoadIdentity()             
      .RotateOz(Session::m_moment*pData->speed,pData->axis2)
      .Update();
*/
}



void g_enablePortal(const char *portalName)
{
    CNameDecls &refNames = ZAV_Scene()->ObjRefNames();  // objects' names
    ct_ClassTableID  ctID = g_arena.searchSeanceClassTable("Portal");

    CNameDecl  &portArr   = refNames[portalName]; // array of objects named "mill"
    int         portCount = portArr.Count();  // number of objects named "mill"

    for( int i = 0 ; i < portCount ; i++ ) 
    {
         CViewObjectRef  *portRef = (CViewObjectRef*)portArr[i];

         CFVector3 pos( portRef->GetDir().Offset() );

         KR_ObjectID oID = g_arena.newObject(ctID,"PortalObj");
         if(  oID.isNUL()  )
              break;

         IPortal *p =(IPortal*)(g_arena.context->queryInterface(oID,IPortalIID));
         portRef->SetAnimationCallback( PortalCallback );
         portRef->SetUserAttrib       ( p );
         p->portalInit(pos, 4);
         echo("Founded portal.. animated");

         CFVector3 bp(0,2.6,6.97); 
         bp = portRef->GetDir()*bp;
         p->portalSetPortalPoint(bp);
         //dc_CreateCross(0,bp,1000,0,0,1,1,0,"Portal");
    }

}


CFVector3  Portal::getPos()
{
  return getPosition();
}

double     Portal::getHAngle()
{
  return 0;
}

CFVector3  Portal::getUpVector ()
{
  return CFVector3(0,1,0);
}

CFVector3  Portal::getCenter()
{
  return CFVector3(0,0,0);
}

double     Portal::getRadius   ()
{
  return 1;
}

double     Portal::getRadius0  ()
{
  return 1;
}

CFVector3  Portal::getMoveDir  ()
{
   return CFVector3(1,0,0);
}

double     Portal::getMoveSpeed()
{
  return 0;
}

void Portal::draw()
{
}

CFVector3 Portal::realPosition()
{
   return getPosition();
}

void Portal::getMatrix( CFMatrix3x4 &m )
{
   m.LoadIdentity().TranslateL(getPosition());
}



/* End of file C:\WinGame\OBASE\portal\portal.cpp */
