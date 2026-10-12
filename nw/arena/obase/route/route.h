#ifndef __ROUTE__
#define __ROUTE__


#define LAST_H__SCENE
#include "string.h"
#include "stdtypes.h"
#include "mathlib.h"

#include "kernel/h/echo.h"
#include "kernel/h/context.h"
#include "storage/h/classtab.h"
#include "message/routmsg.h"
#include "i/route.i"


#define ROUTE_MAX_NODE_NUM 3000

class CChannelMap;


typedef struct {
	int         m_nodeQnty;
    double      m_totalLenght;
    int         m_base;
    int         m_refs;
} RouteData;

class Route:	public ct_Object, 
				public IRouteObject,
				public RouteData
{
public:

    static CFVector3 m_node[ROUTE_MAX_NODE_NUM];
    static CFVector3 m_napr[ROUTE_MAX_NODE_NUM];
    static double    m_length[ROUTE_MAX_NODE_NUM];
    static int       m_totalNodePos;

	static bool LoadStaticData(PIN_SaveFile & sf);
	static bool SaveStaticData(PIN_SaveFile & sf);


    Route() {m_nodeQnty = 0;}
    virtual ~Route(){}

    void         LoadRoute(const char *fName);
	void		 EvaluateLenght();

    virtual int  receiveEvent(KR_Event &event);
    virtual void addNotify();
    virtual void removeNotify();
    void         Delete();

	virtual void      Load(const char * fName);
	virtual int       GetNodeCnt() const;
	virtual int       GetLen() const;
	virtual CFVector3 GetNode(int n) const;  
	virtual CFVector3 GetPos(double part) const;
    virtual KR_ObjectID getID() { return getObjectID(); }
    virtual void      AddRef();
    virtual void      DelRef();

	virtual void *queryInterface( int interNum );

	virtual bool	shouldDump () { return true; } 
	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
	virtual void	loadNotify();  
};

class RouteTable: public ct_ClassTable{
private:
    Route *m_table;
public:
    ct_Object *getExist() const { return m_existList; }
    RouteTable() { registerClass( "Route" ); }
    virtual  void allocObjects( int objectQnty );
    virtual  void freeObjects();
    virtual  ct_Object  *getObjectPTR( int index );
};

#endif // __ROUTE__
