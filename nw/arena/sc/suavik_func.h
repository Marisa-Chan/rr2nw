#include "h/vehicle.h"
#include "i/unit.i"

void g_StaticAnim(int level);
void g_AttachObject(
                      ct_ClassTableID ctID,
                      const char     *name,
                      KR_ObjectID     attrID,
                      const char     *nameRef,
                      CFVector3       v,
		      int	      event
                   );

class  ole_Event: public KR_Event
 {
 public:
       int m_use;
       ole_Event()
       {
           m_use = 0;
       }
 };

ole_Event ole_event[MAX_OLE_EVENT];
 //===========================================================================
void s_OpenEventData( TProcessContext *pc, void *a )
 {
    (void)a;
    int i;

    for( i = 0; i<MAX_OLE_EVENT; ++i )
    {
         if( !ole_event[i].m_use )
         {
              ole_event[i].m_use = 1;
              SC_PARI(1)         = i;
              ole_event[i].data.open((s_EventDataOpen)SC_PARI(0));
              return;
         }
    }
    SC_PARI(1) = -1;
 }

 //===========================================================================
void s_CloseEventData( TProcessContext *pc, void *a )
 {
    (void)a;
    int num = SC_PARI(0);

    if( num >= 0  && num <MAX_OLE_EVENT )
    if( ole_event[num].m_use )
         ole_event[num].data.close();
 }

 //===========================================================================
void s_Descend( TProcessContext *pc, void *a )
 {
    (void)a;
    int num = SC_PARI(2);

    if( num >= 0  && num <MAX_OLE_EVENT )
    if( ole_event[num].m_use )
         ole_event[num].data.descend( SC_PARI(1),
                                      SC_PARI(0));
 }

 //===========================================================================
void s_Ascend( TProcessContext *pc, void *a )
 {
    (void)a;
    int num = SC_PARI(0);

    if( num >= 0  && num <MAX_OLE_EVENT )
    if( ole_event[num].m_use )
         ole_event[num].data.ascend();
 }

 //===========================================================================
void s_WriteInt( TProcessContext *pc, void *a )
 {
    (void)a;
    int num = SC_PARI(1);

    if( num >= 0  && num <MAX_OLE_EVENT )
    if( ole_event[num].m_use )
        ole_event[num].data.putInt(SC_PARI(0));
 }

 //===========================================================================
void s_WriteFloat( TProcessContext *pc, void *a )
 {
    (void)a;
    int num = SC_PARI(1);

    if( num >= 0  && num <MAX_OLE_EVENT )
    if( ole_event[num].m_use )
        ole_event[num].data.putDouble(SC_PARF(0));
 }

 //===========================================================================
void s_WriteVector( TProcessContext *pc, void *a )
 {
    (void)a;
    int num = SC_PARI(3);

    if( num >= 0  && num <MAX_OLE_EVENT )
    {
         ole_Event &ev=ole_event[num];
         if( ev.m_use )
         {
              ev.data.putDouble(SC_PARF(2));
              ev.data.putDouble(SC_PARF(1));
              ev.data.putDouble(SC_PARF(0));
         }
    }
 }

 //===========================================================================
void s_WriteStr( TProcessContext *pc, void *a )
 {
    (void)a;
    int num = SC_PARI(1);

    if( num >= 0  && num <MAX_OLE_EVENT )
    if( ole_event[num].m_use )
        ole_event[num].data.putStr( (char *)
                              &(pc->m_codePtr[SC_PARI(0)])
                               );
 }

 //===========================================================================
void s_WriteObjectID( TProcessContext *pc, void *a )
 {
    (void)a;
    int num = SC_PARI(2);

    if( num >= 0  && num <MAX_OLE_EVENT )
    if( ole_event[num].m_use )
        ole_event[num].data.putObjectID(
                     KR_ObjectID(SC_PARI(1),SC_PARI(0)));
 }

 //===========================================================================
void s_IssueEvent( TProcessContext *pc, void *a )
 {
    ct_Arena &storage = *((ct_Arena*)(a));
    int num = SC_PARI(4);

    if( num >= 0  && num <MAX_OLE_EVENT )
    {
         ole_Event &ev=ole_event[num];
         if( ev.m_use )
         {
              ev.label       = SC_PARI(3);

              ev.timeStamp   = SC_PARF(2);
              ev.destination.Init(SC_PARI(1),SC_PARI(0));
              ev.source      = storage.getObjectID();
              ev.m_use       = 0;
              storage.issueEvent(ev);
         }
    }
 }

 //===========================================================================
void s_SendEventNow( TProcessContext *pc, void *a )
 {
    ct_Arena &storage = *((ct_Arena*)(a));
    int num = SC_PARI(3);

    if( num >= 0  && num <MAX_OLE_EVENT )
    {
         ole_Event &ev=ole_event[num];
         if( ev.m_use )
         {
              ev.label       = SC_PARI(2);

              ev.timeStamp   = Session::m_moment;
              ev.destination.Init(SC_PARI(1),SC_PARI(0));
              ev.source      = storage.getObjectID();
              ev.m_use       = 0;
              storage.context->sendEventNow(ev);
         }
    }
 }

 //===========================================================================
void s_SearchObjectID( TProcessContext *pc, void *a )
 {
    ct_Arena &storage = *((ct_Arena*)(a));
    KR_ObjectID oID( storage.getContext()->searchObject(
                    (char *)&(pc->m_codePtr[SC_PARI(0)])
                   ));
    pc->m_stack[SC_PARI(1)].i = oID.getCachePos();
    pc->m_stack[SC_PARI(2)].i = oID.id;
 }

 //===========================================================================
void s_AddClassTable( TProcessContext *pc, void *a )
 {
    ct_Arena &storage = *((ct_Arena*)(a));
    const char *name  = (char *)&(pc->m_codePtr[SC_PARI(1)]);
    int         size  = SC_PARI(0);

    SC_PARI(2) = storage.addClassTable( name, size );
 }

 //===========================================================================
void s_NewObject( TProcessContext *pc, void *a )
 {
    ct_Arena &storage = *((ct_Arena*)(a));
    const char *name  = (char *)&(pc->m_codePtr[SC_PARI(0)]);
    int         ct    = SC_PARI(1);
    if(  storage.getContext()->isExist(name) )
         echo("s_NewObject:Warning! Object [%s]  exist",name); 
    storage.newObject( ct, name );
 }

 //===========================================================================
void s_NewObjectN( TProcessContext *pc, void *a )
 {
    ct_Arena &storage  = *((ct_Arena*)(a));
    const char *name   = (char *)&(pc->m_codePtr[SC_PARI(0)]);
    const char *ctname = (char *)&(pc->m_codePtr[SC_PARI(1)]);

    if(  storage.getContext()->isExist(name) )
         echo("s_NewObjectN:Warning! Object [%s]  exist",name); 
    storage.newObject( ctname, name );
 }

 //===========================================================================
void s_New( TProcessContext *pc, void *a )
 {
    ct_Arena &storage  = *((ct_Arena*)(a));
    int        ctID    = SC_PARI(3);
    const char *name   = (char *)&(pc->m_codePtr[SC_PARI(2)]);

    KR_ObjectID oID = storage.newObject( ctID, name );
    pc->m_stack[SC_PARI(1)].i = oID.id;
    pc->m_stack[SC_PARI(0)].i = oID.getCachePos();
 }

 //===========================================================================
void s_UpdateAttributes( TProcessContext *, void * )
{
    g_arena.updateAttributes(Session::m_moment);
}


void s_SetLevel( TProcessContext *pc, void * )
{
    g_StaticAnim(SC_PARI(0));
}

void s_AttachObject( TProcessContext *pc, void *a )
{
    ct_Arena &storage  = *((ct_Arena*)(a));
    int event = SC_PARI(0);
    CFVector3 v(SC_PARF(3),SC_PARF(2),SC_PARF(1));
    const char *name      = (char *)&(pc->m_codePtr[SC_PARI(4)]);
    KR_ObjectID attrID(SC_PARI(6),SC_PARI(5));
    const char *nameRef   = (char *)&(pc->m_codePtr[SC_PARI(7)]);
    ct_ClassTableID ctID = SC_PARI(8);

    g_AttachObject(
                      ctID,
                      name,
                      attrID,
                      nameRef,
                      v,
		      event
                   );

}

void s_SearchSeanceClassTable( TProcessContext *pc, void *a )
{
    ct_Arena &storage  = *((ct_Arena*)(a));
    const char *name   = SC_PARS(0);
    SC_PARI(1) = storage.searchSeanceClassTable(name);
}

TViewPoint g_vp[40];

void s_SetViewPoint( TProcessContext *pc, void * )
{
   CFVector3 v(SC_PARF(2),SC_PARF(1),SC_PARF(0));
   int index = SC_PARI(3);

   if(  index >=0 && index < 40  )
        g_vp[index].vp = v;
}

void s_GetTime( TProcessContext *pc, void * )
{
   SC_PARF(0) = Session::m_moment;
}

void s_SetDamage( TProcessContext *pc, void * )
{
    const char *name  = (char *)&(pc->m_codePtr[SC_PARI(1)]);
    double      d     = SC_PARF(0);
    KR_ObjectID oID = g_arena.getContext()->searchObject(name);
    if(  !oID.isNUL()  )
    {
          IUnit *uobj=(IUnit*)(g_arena.getContext()->queryInterface(oID,IUnitIID));

          if(  uobj!=0  )
               uobj->setDamage(d,CFVector3(0,0,0),Session::m_moment,g_arena.getObjectID());
          else echo("Object <%s> not unit",name);
    }
    else echo("Object <%s> not exist",name);
}

void s_SetCommander( TProcessContext *pc, void * )
{
    const char *name  = (char *)&(pc->m_codePtr[SC_PARI(1)]);
    const char *com   = (char *)&(pc->m_codePtr[SC_PARI(0)]);

    KR_ObjectID oID = g_arena.getContext()->searchObject(name);
    if(  !oID.isNUL()  )
    {
          IUnit *uobj=(IUnit*)(g_arena.getContext()->queryInterface(oID,IUnitIID));

	  KR_ObjectID cID = g_arena.getContext()->searchObject(com);

          if (cID.isNUL())
	  {
           echo("SetCommander: Cannot find commander <%s>", com);  
	   return;
          }

	  
          if(  uobj!=0  )
               uobj->setCommander(cID);
          else 
	       echo("SetCommander:Object <%s> is not unit",name);
    }
    else echo("SetCommander:Object <%s> does not exist",name);
}
