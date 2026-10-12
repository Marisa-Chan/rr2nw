#include "kernel/h/echo.h"
#include "kernel/h/context.h"
#include "kernel/h/setofid.h"
#include "storage/h/subject.h"

#include "message/comanmsg.h"
#include "message/groupmsg.h"
#include "enum/spaceenum.h"

#include "i/route.i"
#include "i/commander.i"

#define HANDLE int
#include "storage\h\savefile.h"



#define MAX_MEMBER 250
#define GROUP_MAX_ROUTE_NUM 4

s_ELN commanderMsgNames[] =
 {
   s_ELN(COMMANDER_ADD_MEMBER_N,"COMMANDER_ADD_MEMBER_N"),
   s_ELN(com_EV_GROUP_REACHED,"com_EV_GROUP_REACHED"),
   s_ELN(com_EV_GROUP_TARGET_DESTROYED,"com_EV_GROUP_TARGET_DESTROYED"),
   s_ELN(com_EV_GROUP_YOUR_MY_MEMBER,"com_EV_GROUP_YOUR_MY_MEMBER"),
   s_ELN(com_EV_PLANE0,"com_EV_PLANE0"),
   s_ELN()
 };

s_ELNTable commanderMsgTable("Commander messages",commanderMsgNames);

class GroupMember{
public:
    GroupMember() {  m_routeCnt = m_curRoute = 0; m_patrol = 0; }

	KR_ObjectID m_id;
	int m_patrol;
	int m_routeCnt;
	int m_curRoute;
	struct Route
        {
	    KR_ObjectID id;
	    int repCnt;
	    int repeated;
	    int patrol;
	    int curNode;
	    int nodeCnt;
	    int dir;
	    double waitNext;

            void reset()
            {
                repCnt    = 
                repeated = 
                waitNext = 
                nodeCnt  = 
                curNode  = 0; 
                dir = 1; 
                patrol = FALSE;
            }

            Route() 
            { 
                reset();
            }
	} 
          m_route[GROUP_MAX_ROUTE_NUM];

        void reset()
        {
            m_id = KR_ObjectID::NUL();
            m_patrol   = 0;
            m_routeCnt = 0;
            m_curRoute = 0;

            for( int i = 0; i < GROUP_MAX_ROUTE_NUM; ++i )
                 m_route[i].reset();
        }
};


typedef struct {
	GroupMember  m_member[MAX_MEMBER];
    int          m_memberQnty;

    double       m_xMoving,m_zMoving;
    int          m_wMoving,m_dMoving;
	
	KR_SetOfID	 m_hostileCommanders;
	KR_SetOfID	 m_friendlyCommanders;

} CommanderData;


class com_Commander:	public ICommander,
						public ct_Object,
						public CommanderData
 {
 public:    

	void *queryInterface( int interNum );

    virtual void addNotify();
    virtual int  receiveEvent(KR_Event &event);

    void LoadRoute(int rNum, const char *fName, const char *rName, int partol);
    int  loadRoute(const char *rName, const char *fName);
    void addRoute(int mNum,const char *rName, int patrol);
    void clrRoute(int mNum);
    void setToStart(int mNum,double ts);

	virtual bool	shouldDump () { return true; } 	
	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & sf);
	virtual void	loadNotify();     


    virtual void       setFriendly(KR_ObjectID &);
    virtual void       setHostile (KR_ObjectID &);

    virtual int       isFriendly(KR_ObjectID &);
    virtual int       isHostile (KR_ObjectID &);


	//virtual void	  removeMember(KR_ObjectID &);

 };

class com_CommanderTable: public ct_ClassTable
 {
 private:
    com_Commander *m_table;
 public:
    com_CommanderTable()
    {
       registerClass("Commander");
    }
    virtual  void allocObjects( int objectQnty );
    virtual  void freeObjects();
    virtual  ct_Object  *getObjectPTR( int index );
 };

com_CommanderTable CommanderTable;

 /******************************
  *
  *       Commander
  *
  ******************************/

IRouteObject *newRoute( SimulationContext *c, const char *rname )
{
    KR_ObjectID rID = g_arena.newObject("Route",rname);
    if(  rID.isNUL()  )
         return 0;
    return (IRouteObject *)(c->queryInterface(rID,IRouteObjectIID));
}


void *com_Commander::queryInterface( int interNum )
{
    switch( interNum )
    {
    case IUnknownIID:   return (KR_Object  *) this;
    case ICommanderIID: return (ICommander *) this;
    }

    return 0;
}


//============================================================================
void com_Commander::addNotify()
 {
    m_memberQnty = 0;
    m_xMoving =  2060;
    m_zMoving = -2390;
    m_wMoving = 100;
    m_dMoving = 100;
 }
//============================================================================

/*void com_Commander::removeMember(KR_ObjectID &mID)
{
	for (int i = 0; i < m_memberQnty; i++)
		if (mID == m_member[i].m_id)
		{
			for (int j = i; j < m_memberQnty - 1; j++)
			{
				m_member[j] = m_member[j+1];
			}
			m_memberQnty--;
			break;
		}
}*/

//============================================================================
void com_Commander::LoadRoute(int rNum, const char *fName, const char *rName, int patrol){
	IRouteObject *route;

	route = newRoute(context, rName);
        if(  route==0  )
             return;

	route->Load(fName);
	m_member[rNum].m_route[m_member[rNum].m_routeCnt].id      = route->getID();
	m_member[rNum].m_route[m_member[rNum].m_routeCnt].nodeCnt = route->GetNodeCnt();
	m_member[rNum].m_route[m_member[rNum].m_routeCnt].patrol  = patrol;
	m_member[rNum].m_routeCnt++;
}

void SetMemberPos( com_Commander *com,double ts,KR_ObjectID memID, const CFVector3 &node)
{
    KR_Event event;
    event.timeStamp   = ts;
    event.source      = com->getObjectID();
    event.destination = memID;
    event.label       = tg_EVCMD_SET_POSITION;
    
    event.data.open(EDO_WRITE)
                 .putDouble(node.x)
				 .putDouble(node.y)
                 .putDouble(node.z)
              .close();
    
    com->context->sendEventNow( event );
}

void GotoMemberPos( com_Commander *com,double ts,KR_ObjectID memID, const CFVector3 &node)
{
    KR_Event event;
    event.timeStamp   = ts;
	event.source      = com->getObjectID();
    event.destination = memID;
    event.label       = tg_EVCMD_GO_TO;
    //{{PUT_EVENT(tg_EVCMD_GO_TO)

    event.data.open(EDO_WRITE)
                   .descend( VECTOR_LAND, 0 )
                     .putDouble(node.x)
                     .putDouble(node.z)
                   .ascend()
              .close();
    //}}END_OF_PUT_EVENT(tg_EVCMD_GO_TO)
    com->context->sendEventNow( event );
}

//============================================================================
void com_Commander::addRoute(int mNum,const char *rName, int patrol)
 {
        if(  mNum < 0 || mNum >= m_memberQnty )
        {
             echo("com_Commander::addRoute: Range check error!");
             return;
        }

        KR_ObjectID rID(context->searchObject(rName));
        if(  rID.isNUL()  )
        {
             echo("com_Commander::addRoute: Unknown route %s", rName);
             return;
        }
	IRouteObject *route = (IRouteObject *)(context->queryInterface(rID,IRouteObjectIID));
        if(  route==NULL  )
        {
             echo("com_Commander::addRoute: This [%s] not route",rName);
             return;
        }

        int &rCnt = m_member[mNum].m_routeCnt;
        if(  rCnt >= GROUP_MAX_ROUTE_NUM )
        {
             echo("com_Commander::addRoute:Route Table overflow!");
             return;
        }
        route->AddRef();
	m_member[mNum].m_route[rCnt].id      = route->getID();
	m_member[mNum].m_route[rCnt].nodeCnt = route->GetNodeCnt();
	m_member[mNum].m_route[rCnt].patrol  = patrol;
	rCnt++;
 }

//============================================================================
int  com_Commander::loadRoute(const char *rName, const char *fName)
 {
    IRouteObject *route;

    route = newRoute(context, rName);
    if(  route != 0 )
    {
         route->Load(fName);
         return 1;
    }
    return 0;
 }

//============================================================================
void com_Commander::clrRoute(int mNum)
 {
    if(  mNum < 0 || mNum >= m_memberQnty )
    {
         echo("com_Commander::addRoute: Range check error!");
         return;
    }
    m_member[mNum].m_routeCnt = 0;
 }


//============================================================================
void com_Commander::setToStart(int mNum,double ts)
 {
   IRouteObject *
   riFace =(IRouteObject *)(getContext()->queryInterface( m_member[mNum].m_route[0].id, IRouteObjectIID));

   SetMemberPos (this,ts,m_member[mNum].m_id,       riFace->GetNode(0));
   GotoMemberPos(this,ts,m_member[mNum].m_id,       riFace->GetNode(1));
//echo("Group #0[%s] send to [%.2f, %.2f, %.2f]", context->searchObject(m_member[i].m_id),node.x, node.y, node.z); 

 }

void com_Commander::setFriendly(KR_ObjectID & oID)
{
	m_friendlyCommanders.add ( oID );
	m_hostileCommanders .del ( oID );
	
}

void com_Commander::setHostile (KR_ObjectID & oID)
{
	m_friendlyCommanders.del ( oID );
	m_hostileCommanders .add ( oID );
}

int com_Commander::isFriendly(KR_ObjectID & oID)
{
	if (oID == getObjectID())
		return 1;

	return (m_friendlyCommanders.find(oID) != -1);
}

int com_Commander::isHostile (KR_ObjectID & oID)
{
	if (oID == getObjectID())
		return 0;

	return (m_hostileCommanders.find(oID) != -1);
}


//============================================================================
int com_Commander::receiveEvent(KR_Event &event)
 {
    char msg[MAX_SYMBOLIC_LENGHT+1];
	char fName[255], rName[255];
	int i;
	IRouteObject *riFace;
	CFVector3    node;

    switch( event.label )
    {
    case com_EV_SETHOSTILECOMMANDER:
		{
			KR_ObjectID oID;
			
			event.data.open(EDO_READ)
				.getObjectID(oID)     
			.close();

			setHostile(oID);
		}
		
		break;
		
    case com_EV_SETFRIENDLYCOMMANDER:
		{
			KR_ObjectID oID;
			
			event.data.open(EDO_READ)
				.getObjectID(oID)
			.close();
			setFriendly(oID);
		}
		
		break;
		
    case KR_WAKE_UP:
		m_memberQnty = 0;
		break;
		
    case GROUP_I_AM_DEAD:
		for(i = 0; i < m_memberQnty; ++i )
		{
			if(  m_member[i].m_id== event.source  )
			{
				for( int j = 0; j < m_member[i].m_routeCnt; ++j )
				{
					IRouteObject *route = (IRouteObject *)
						(context->queryInterface(
						m_member[i].m_route[j].id,
						IRouteObjectIID)
						);
					if(  route!=0  )
						route->DelRef();
					
				}
				
				for(; i < m_memberQnty-1; ++i )
					m_member[i] = m_member[i+1];
				
				m_memberQnty--;
				echo("Commander: group dead");
				break;
			}
		}
		break;
		
		
    case com_EV_GROUP_AREA:
		event.data.open(EDO_READ)
			.getDouble(m_xMoving)
			.getDouble(m_zMoving)
			.getInt(m_wMoving)
			.getInt(m_dMoving)
			.close();
		break;
		
    case COMMANDER_ADD_MEMBER_N:
		{
            event.data.open(EDO_READ)
				.getStr(msg,sizeof(msg))
				.close();
            KR_ObjectID oid( context->searchObject( msg ));
            if( oid.id==-1 )
            {
				echo("com_Commander: Object %s not found",msg);
				break;
            }
			
            if( m_memberQnty<MAX_MEMBER )
            {
				m_member[m_memberQnty].reset();
				m_member[m_memberQnty].m_id = oid;
				++m_memberQnty;
				event.destination = oid;
				event.source      = getObjectID();
				event.label       = com_EV_GROUP_YOUR_MY_MEMBER;				
				event.data.open(EDO_WRITE)
						 .putObjectID(event.source)                          
                      .close();

				context->sendEventNow( event );
            }
            else echo("com_Commander: Members overflow");
			
		}
		break;
		
    case com_EV_SET_ROUTE:
		{
			char memName[40];
			int  patrol;
			int  add = 0,
				isSetToStart = 0;
			
			event.data.open(EDO_READ)
				.getInt(add);
			if(  add  )
			{
				event.data
					.getStr(memName,sizeof(memName))
					.getStr(rName,sizeof(rName))
					.getStr(fName,sizeof(fName))
					.getInt(patrol)
					.getInt(isSetToStart)
					.close();
				
				for( i = 0; i < m_memberQnty; ++i )
					if(  strcmp(context->searchObject(m_member[i].m_id),
						memName)==0  )
					{
						if(  !context->isExist(rName)  )
							if(  !loadRoute( rName, fName )  )
								break;
							
							addRoute( i, rName, patrol);
							
							if(  isSetToStart  )
                                setToStart(i,event.timeStamp);
							
							break;
					}
			}
			else 
			{
				event.data.getInt(i).close();
				clrRoute(i);
			}
			
		}
		break;
		
		
	case com_EV_GROUP_REACHED:
		for(i = 0; i < m_memberQnty; ++i )
		{
			GroupMember &mem = m_member[i];
			if(  mem.m_id != event.source ) 
				continue;
			
			int &curRoute = mem.m_curRoute;
			int  routeCnt = mem.m_routeCnt;
			int &curNode  = mem.m_route[curRoute].curNode;
			int  nodeCnt  = mem.m_route[curRoute].nodeCnt;
			int &dir      = mem.m_route[curRoute].dir;
			mem.m_patrol = mem.m_route[curRoute].patrol;
			
			if(  mem.m_patrol  )
			{
				switch( dir )
				{
				case 1:
					curNode++;
					if(  curNode >=  nodeCnt )
					{
						dir = 0;
						curNode--;
					}
					break;
					
				case 0:
					curNode--;
					if(  curNode < 0  )
					{
						dir = 1;
						curNode = 0;
					}
					break;
				}
			}
			else
			{
				if(  curNode < nodeCnt-1  )
					curNode++;
				else 
				{
					if(  curRoute < routeCnt-1  )
					{
						curRoute++;
						mem.m_route[curRoute].curNode = 0;
						curNode = 0;
						//echo( "End of route" );
					}
				}
			}
			
			riFace =(IRouteObject *)(getContext()->queryInterface( mem.m_route[curRoute].id, IRouteObjectIID));
			GotoMemberPos(this,event.timeStamp,mem.m_id,node=riFace->GetNode(curNode));
			
			//echo("Group send to [%.2f, %.2f, %.2f]", node.x, node.y, node.z); 
		} 
		break;
		
    default: return 0;
    }
    return 1;
 }


bool	com_Commander::dump(PIN_SaveFile & sf)
{
		if (!sf.WriteData( (char *) m_member, sizeof(CommanderData)  ))
			return false;
		
		return true;
}

bool	com_Commander::load(PIN_SaveFile & sf)
{
		if (!sf.GetData( (char *) m_member, sizeof(CommanderData)  ))
			return false;
		
		return true;
}

void	com_Commander::loadNotify()
{
}


 /******************************
  *
  *      Commander table
  *
  ******************************/

//============================================================================
void com_CommanderTable::allocObjects( int objectQnty )
 {
    m_table = new com_Commander[objectQnty];
    if( m_table==NULL )
         m_maxObjectQnty = 0;
 }

//============================================================================
void com_CommanderTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

//============================================================================
ct_Object *com_CommanderTable::getObjectPTR( int index )
 {
    if( index<0 || index>=m_maxObjectQnty )
    {
         echo("com_CommanderTable::SearchObject: Bad index");
         return NULL;
    }
    return &(m_table[index]);
 }

