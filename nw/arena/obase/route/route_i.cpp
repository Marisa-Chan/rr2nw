#include "route.h"
#include "kernel/h/s_debug.h"

void *Route::queryInterface( int interNum ){
    switch( interNum ){
		case IUnknownIID:       return (KR_Object*)this;
		case IRouteObjectIID:   return (IRouteObject*) this;
    }
    return NULL;
}
void Route::Load(const char * fName) 
{
   LoadRoute(fName); 
}

int Route::GetNodeCnt() const 
{
   return(m_nodeQnty);
}

int Route::GetLen() const
{
   return(m_totalLenght);
}

CFVector3 Route::GetNode(int n)  const 
{
   s_ASSERT1( n >=0 && n < m_nodeQnty, "Route::GetNode: Range check error %i", n );
   return(m_node[m_base+n]); 
}

CFVector3 Route::GetPos(double part) const{
	CFVector3 pos;
	
	if( part < 0 || part > 1) echo("Route::GIVE_INT_POS:Wrong part." );
	for(int i = m_base ;  ; i++ )
		if( part <= m_length[m_base+i] || i == m_base+m_nodeQnty-2 ){
			pos = m_node[m_base+i] + m_napr[m_base+i] * part;
			break;
		}
		else part -= m_length[m_base+i] ;

	return(pos);
}
