#include <stdio.h>
#include <string.h>

#include "route.h"
#include "kernel/h/s_debug.h"

#include "mathlib.h"

#define HANDLE int
#include "storage\h\savefile.h"

#define min Min
#define max Max
#define BYTE byte

#define IsNAN(x)  (((x)*(x)==(x)) && (x)!=0)


static RouteTable __routeTable;


CFVector3 Route::m_node[ROUTE_MAX_NODE_NUM];
CFVector3 Route::m_napr[ROUTE_MAX_NODE_NUM];
double	 Route::m_length[ROUTE_MAX_NODE_NUM];
int Route::m_totalNodePos=0;

//============================================================================
void Route::AddRef()
{
    m_refs++;
}

//============================================================================
void Route::DelRef()
{
    m_refs--;
    if(  m_refs==0  )
    {
         if(  context  )
         {
              Delete();
              context->removeObject( getObjectID() );
         }
    };
}

//============================================================================
void Route::Delete()
{
     int i;
     int cnt = m_totalNodePos-m_nodeQnty;

     for( i = m_base; i < cnt; ++i )
     {
          m_node[i]   = m_node[i+m_nodeQnty];
          m_napr[i]   = m_napr[i+m_nodeQnty];
          m_length[i] = m_length[i+m_nodeQnty];
     }

     m_totalNodePos -= m_nodeQnty;

     for(
           ct_Object *ro = __routeTable.getExist();
           ro != 0;
           ro = ro->next()

        )
     {
          Route *r = (Route*)ro;
          if(  r->m_base > m_base  )
               r->m_base -= m_nodeQnty;
     }
}

//============================================================================
static void ReadStr( FILE *f, char buff[], int maxSize ){
    int i, len;
    char c;

	for( i = 0, c = 0; (!feof(f)) && c != 13; ++i ){
         fread(&c, 1, 1, f);
         if(i >= maxSize-1)break;
         buff[i]   = c;
         buff[i+1] = 0;
    }
    if((!feof(f)) && c == 13) fread(&c,1,1,f);

    for(;;){
         len = strlen(buff);
         if( len>0 && buff[len-1]<=' ') buff[len-1] = 0;
         else                           break;
    }
}
//============================================================================
void Route::addNotify()
{
    m_nodeQnty = 0; 
    m_refs     = 0;
    ct_Object::addNotify();
}

//============================================================================
void Route::removeNotify()
{
	m_nodeQnty = 0;
	ct_Object::removeNotify();
}

//============================================================================
void Route::EvaluateLenght()
{
	int i;

	for( i= m_base, m_totalLenght = 0 ; i < m_base+m_nodeQnty-1 ; i++ )
		m_totalLenght += Abs( m_node[i] - m_node[i+1] );

	for( i = m_base ; i < m_base+m_nodeQnty-1 ; i++ ){
		m_length[i] = Abs( m_node[i] - m_node[i+1] ) / m_totalLenght;
		m_napr[i]   = ( m_node[i+1] - m_node[i] ) / m_length[i] ;
	}
}

//============================================================================
void Route::LoadRoute(const char *fName)
 {
    FILE *f = fopen(fName,"rb");
    char buff[256];

    if(f == NULL){
         echo("Route::load: File %s not found\n", fName);
         s_ASSERTNQ1("Route::load: File %s not found\n", fName);
         return;
    }
    ReadStr(f, buff, sizeof(buff));
    if( strcmp(buff, context->searchObject(getObjectID())) != 0 )
         echo("Route::load: Suacript_RouteName != file_RouteName\n");

    ReadStr(f, buff, sizeof(buff));
    sscanf(buff, "%i", &m_nodeQnty);

    m_base = m_totalNodePos;
    s_ASSERT(m_nodeQnty > 0 && m_totalNodePos + m_nodeQnty < ROUTE_MAX_NODE_NUM, "");

    for( int i = m_base; i < m_base+m_nodeQnty; ++i ){
         double x,y,z;
         ReadStr(f,buff,sizeof(buff));
         sscanf(buff,"[ %lf , %lf , %lf ]",&x,&y,&z);

         if( IsNAN(x) || IsNAN(y) || IsNAN(z) ){
              echo("Route::load: Bad coord[%i]\n",i);
              fclose(f);
              return;
         }
         m_node[i].x = x;
         m_node[i].y = y;
         m_node[i].z = z;
    }
    m_totalNodePos += m_nodeQnty;
    fclose(f);
}
//============================================================================
int Route::receiveEvent(KR_Event &event){
    char msg[250];
	CFVector3 pos;
	int n, i;

    switch( event.label ){
		case KR_WAKE_UP:
                                break;
		case ROUTE_LOAD:
				event.data.open(EDO_READ)
							.getStr( msg , sizeof(msg) )
						  .close();
				Load(msg);
				EvaluateLenght();
				break;

  		case ro_EV_GET_NODE_CNT:
				event.data.open( EDO_WRITE )
							.putInt( m_nodeQnty )
						  .close();
				break;
			 
  		case ro_EV_GET_LENGHT:
				event.data.open( EDO_WRITE )
							.putDouble( m_totalLenght )
						  .close();
				break;

		case ro_EV_GET_NODE:
				event.data.open( EDO_READ )
							.getInt( n )
						  .close();
				
				if( n<0 && n>=m_nodeQnty )
					echo( "Route::GIVE_POINT:Wrong point number." );

				event.data.open( EDO_WRITE )
								.descend( 5 , 1 )
										.putDouble( m_node[m_base+n].x )
										.putDouble( m_node[m_base+n].y )
										.putDouble( m_node[m_base+n].z )
								.ascend()
						  .close();
			break;

		case ro_EV_GET_POS:
				double part;
				int	   type;

				event.data.open( EDO_READ )
							.getDouble(part)
							.getInt( type )
						  .close();

				if(part < 0 || part > 1){
					echo("Route::GIVE_INT_POS: Wrong part." );
					break;
				}
				for(i = m_base ;  ; i++ )
					if(part <= m_length[i] || i == m_base+m_nodeQnty-2 ){
						pos = m_node[i] + m_napr[i] * part;
						event.data.open( EDO_WRITE )
									.descend( 5 , 2 )
										.putDouble( pos.x )
										.putDouble( pos.y )
										.putDouble( pos.z )
									.ascend()
							      .close();
						break;
					}
					else part -= m_length[i] ;
			break;

		default: return 0;
    }
    return 1;
}


bool	Route::dump(PIN_SaveFile & sf)
{
		if (!sf.WriteData( (char *) & m_nodeQnty, sizeof(RouteData)  ))
			return false;
		
		return true;
}

bool	Route::load(PIN_SaveFile & sf)
{
		if (!sf.GetData( (char *) & m_nodeQnty, sizeof(RouteData)  ))
			return false;
		
		return true;
}

void	Route::loadNotify()
{
}

bool Route::LoadStaticData(PIN_SaveFile & sf)
{
	if (!sf.GetData( (char *) m_node,   sizeof(CFVector3) * ROUTE_MAX_NODE_NUM) ||
		!sf.GetData( (char *) m_napr,   sizeof(CFVector3) * ROUTE_MAX_NODE_NUM) ||
		!sf.GetData( (char *) m_length, sizeof(double)    * ROUTE_MAX_NODE_NUM) ||
		!sf.GetData( (char *) & m_totalNodePos, sizeof(int)))
		return false;
	return true;
}

bool Route::SaveStaticData(PIN_SaveFile & sf)
{
	if (!sf.WriteData( (char *) m_node,		sizeof(CFVector3) * ROUTE_MAX_NODE_NUM) ||
		!sf.WriteData( (char *) m_napr,		sizeof(CFVector3) * ROUTE_MAX_NODE_NUM) ||
		!sf.WriteData( (char *) m_length,	sizeof(double)    * ROUTE_MAX_NODE_NUM) ||
		!sf.WriteData( (char *) & m_totalNodePos, sizeof(int)))
		return false;
	return true;
}


//============================================================================

//============================================================================
void RouteTable::allocObjects( int objectQnty ){
    Route::m_totalNodePos = 0; 
    m_table = new Route[objectQnty];
    if(m_table == NULL) m_maxObjectQnty = 0;
}

//============================================================================
void RouteTable::freeObjects(){
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
}

//============================================================================
ct_Object *RouteTable::getObjectPTR( int index ){
    if( index<0 || index>=m_maxObjectQnty ){
         echo("RouteTable::SearchObject: Bad index");
         return NULL;
    }
    return &(m_table[index]);
}



/* End of file ROUTE.CPP */