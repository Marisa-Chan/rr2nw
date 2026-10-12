/*
   File:  Suavik\d:\game\mproj\mproj.cpp
   Autor: Suavik
   Ver    1.0

   Префикс mp_

   Описание заданий на миссию.

   Одновременно писать данные в несколько узлов задания нельзя,
   поскольку они должны лежать в куче последовательно, без всяких
   списков.

   Данные в узле не структурируются, поскольку предполагается,
   что сами по себе узлы образуют структуру
 */
#include "mproj/h/mproj.h"
#include "kernel/h/echo.h"
#include "kernel/h/context.h"
#include "../output/defs.h"
#define HANDLE int
#include "storage\h\savefile.h"

 //===========================================================================
class mp_TreeNode
 {
 public:
      int              m_command;
      KR_byte         *m_data;
      int              m_left, m_right;
      int              m_size;
      int              m_start;
      int              m_pos;

      mp_TreeNode()
      {
        m_left  = -1;
        m_right = -1;
        m_data  = NULL;
        m_size  = 0;
        m_start = 0;
        m_pos   = 0;
      }
 };

mp_ProjectTable projectTable;

 //===========================================================================
mp_ProjectHeap::mp_ProjectHeap()
 {
    m_data = NULL;
    m_size = 0;
    m_pos  = 0;
 }

 //===========================================================================
mp_ProjectHeap::~mp_ProjectHeap()
 {
    delete [] m_data;
    m_data = NULL;
    m_size = 0;
    m_pos  = 0;
 }

 //===========================================================================
void mp_ProjectHeap::create(int size)
 {
    m_data = new KR_byte[size];
    m_pos  = 0;
    m_size = size;
 }

 //===========================================================================
void mp_ProjectHeap::remove()
 {
    delete [] m_data;
    m_data = NULL;
    m_size = 0;
    m_pos  = 0;
 }

 //===========================================================================
KR_byte *mp_ProjectHeap::getCurHeap()
 {
    if( m_data==NULL )
    {
         echo("mp_ProjectHeap::getCurHeap: Heap not allocated\n");
         return NULL;
    }

    if( m_pos >= m_size )
    {
         echo("mp_ProjectHeap::getCurHeap: Heap overflow\n");
         return NULL;
    }

    return &(m_data[m_pos]);
 }

 //===========================================================================
void mp_ProjectHeap::step(int s)
 {
    if( m_data==NULL )
    {
         echo("mp_ProjectHeap::step: Heap not allocated\n");
         return;
    }

    if( m_pos+s>=m_size )
    {
        m_pos = m_size-1;
        echo("mp_ProjectHeap::step: Heap overflow\n");
        return;
    }
    m_pos += s;
 }

 /**************************************
  *
  *       mp_Project
  *
  **************************************/

 //===========================================================================
mp_Project::~mp_Project()
 {
 }

 //===========================================================================
int mp_Project::receiveEvent(KR_Event &event)
 {
    switch( event.label )
    {
    case KR_WAKE_UP: break;
    default: return 0;
    }
    return 1;
 }

bool mp_Project::dump(PIN_SaveFile & sf)
{
	if (!sf.WriteData( (char *) & m_treeNode, sizeof(mp_ProjectData)  ))
		return false;
	
	return true;
}

bool mp_Project::load(PIN_SaveFile & sf)
{
	if (!sf.GetData( (char *) & m_treeNode, sizeof(mp_ProjectData)  ))
		return false;
	
	return true;
}


 /**************************************
  *
  *       mp_TreeNode
  *
  **************************************/

 //===========================================================================
mp_NodeNum mp_New( mp_ProjectTable &p, int command, int left, int right )
 {
    if( p.m_treePos < p.m_treeSize )
    {
        mp_TreeNode &node = p.m_tree[p.m_treePos];
        node.m_command = command;
        node.m_left    = mp_Code2Int( left  );
        node.m_right   = mp_Code2Int( right );
        ++p.m_treePos;
        return mp_Int2Code(p.m_treePos-1);
    }

    echo("MPROJ.CPP::mp_New: tree buffer overflow\n");
    return mp_Int2Code(-1);
 }

 /**************************************
  *
  *       mp_ProjectTable
  *
  **************************************/

 //============================================================================
void mp_ProjectTable::create(
                             int                  objectQnty,
                             SimulationContext   *context,
                             ct_Storage          &storage,
                             int                  treeSize,
                             int                  heapSize
                            )
 {
    ct_ClassTable::create( objectQnty, context, storage );
    createTreeHeap( treeSize );
    createDataHeap( heapSize );
 }

 //============================================================================
void mp_ProjectTable::allocObjects( int objectQnty )
 {
    m_table = new mp_Project[objectQnty];

    if( m_table==NULL )
         m_maxObjectQnty = 0;
 }

 //============================================================================
void mp_ProjectTable::freeObjects()
 {
    delete [] m_tree;
    m_tree = NULL;
    m_treeSize = 0;
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================================
ct_Object *mp_ProjectTable::getObjectPTR( int index )
 {
    if( index<0 || index>=m_maxObjectQnty )
    {
         echo("mp_ProjectTable::SearchObject: Bad index");
         return NULL;
    }
    return &(m_table[index]);
 }

 //============================================================================
void mp_ProjectTable::createTreeHeap( int size )
 {
    m_tree     = new mp_TreeNode[size];
    m_treeSize = size;
    m_treePos  = 0;
 }

 //============================================================================
mp_ProjectTable::mp_ProjectTable()
 {
    /*
       Класс не регистрируется в общем списке
       и обязан использоваться напрямую
     */
    m_tree     = NULL;
    m_treeSize = 0;
    m_treePos  = 0;
    m_table    = NULL;
    m_open     = EDO_NONE;

    registerClass( "Project" );
 }

 //============================================================================
mp_ProjectTable::~mp_ProjectTable()
 {
    delete [] m_table;
    m_table = NULL;
    delete [] m_tree;
    m_tree  = NULL;
 }

 //============================================================================
KR_ObjectID  mp_ProjectTable::newProject( const char *pname, int node , int permanent )
 {
    KR_ObjectID oID = newObject( pname );
    mp_Project *proj = searchProject( oID );

	proj->m_permanent = permanent;

    int nodeNum = mp_Code2Int( node );
    if( nodeNum<0 || nodeNum >= m_treePos )
    {
         echo("Create project with unknown tree node\n");
         return KR_ObjectID(-1,-1);
    }
    if( proj==NULL )
         echo("Unknown project %s\n",pname);
    else proj->set(node);

    return oID;
 }

 //============================================================================
mp_Project *mp_ProjectTable::searchProject( const KR_ObjectID &oID ) const
 {
    mp_Project *proj = (mp_Project*)m_existList;

    for(; proj != NULL ; proj = (mp_Project*)(proj->next()) )
          if( proj->getObjectID() == oID )
               return proj;

    return NULL;
 }

 //============================================================================
int mp_ProjectTable::getProjectRoot( const KR_ObjectID &oID )
 {
    mp_Project *proj = searchProject( oID );

    if( proj == NULL )
    {
         printf("Unknown project [%i]\n", oID.id );
         return mp_NodeNULL();
    }
    return proj->get();
 }

 //============================================================================
int         mp_ProjectTable::getCommand( mp_NodeNum n ) const
 {
    n = mp_Code2Int(n);

    if( n >= 0  &&  n < m_treePos )
         return m_tree[n].m_command;

    echo("mp_ProjectTable::getCommand: Bed project node\n");
    return -1;
 }

 //============================================================================
mp_NodeNum  mp_ProjectTable::getRight  ( mp_NodeNum n ) const
 {
    n = mp_Code2Int(n);

    if( n >= 0  &&  n < m_treePos )
         return mp_Int2Code(m_tree[n].m_right);

    echo("mp_ProjectTable::getRight: Bed project node\n");
    return mp_NodeNULL();
 }

 //============================================================================
mp_NodeNum  mp_ProjectTable::getLeft   ( mp_NodeNum n ) const
 {
    n = mp_Code2Int(n);

    if( n >= 0  &&  n < m_treePos )
         return mp_Int2Code(m_tree[n].m_left);

    echo("mp_ProjectTable::getLeft: Bed project node\n");
    return mp_NodeNULL();
 }

 /**************************************************
  *
  *               Project data
  *
  **************************************************/

 //============================================================================
void   mp_OpenData   ( mp_ProjectTable &p, mp_NodeNum n, s_EventDataOpen edo )
 {
    n = mp_Code2Int(n);

    if( n>=0 && n<p.m_treePos )
    {
         mp_TreeNode &node = p.m_tree[n];

         switch( p.m_open )
         {
         case EDO_NONE:  break;
         case EDO_WRITE: echo("MPROJ.CPP::mp_OpenData: Data open for write\n"); break;
         case EDO_READ:  echo("MPROJ.CPP::mp_OpenData: Data open for read\n"); break;
         }
         p.m_open = edo;
         switch( edo )
         {
         case EDO_NONE:
                 echo("MPROJ.CPP::mp_OpenData: Can't open with style EDO_NONE\n");
                 break;
         case EDO_WRITE:
                 node.m_size = 0;
                 node.m_data = p.getCurHeap();
                 node.m_start= p.m_heap.m_pos;
                 break;
         case EDO_READ:
                 node.m_pos  = 0;
                 break;
         }
         return;
    }
    echo("MPROJ::mp_OpenData: Bad index\n");
 }

 //============================================================================
void   mp_CloseData  ( mp_ProjectTable &p, mp_NodeNum n )
 {
    n = mp_Code2Int(n);

    if( n>=0 && n<p.m_treePos )
    {
         switch( p.m_open )
         {
         case EDO_NONE: echo("MPROJ.CPP::mp_CloseData: Data not open\n"); break;
         case EDO_WRITE:
         case EDO_READ: p.m_open = EDO_NONE;
         }
         return;
    }
    echo("MPROJ.CPP::mp_CloseData: Bad index\n");
 }

 //============================================================================
void   mp_WriteByte( mp_ProjectTable &p, mp_TreeNode &n, KR_byte b )
 {
    if( n.m_data==NULL )
    {
         echo("MPROJ.CPP::mp_WriteByte: node data==NULL\n");
         return;
    }

    n.m_data[n.m_size] = b;
    p.m_heap.step(1);
    n.m_size = p.m_heap.m_pos - n.m_start;
 }

 //============================================================================
void   mp_ReadByte( mp_TreeNode &n, KR_byte &b  )
 {
    if( n.m_data==NULL )
    {
         echo("MPROJ.CPP::mp_ReadByte: node data==NULL\n");
         return;
    }

    b = n.m_data[n.m_pos];
    ++n.m_pos;
    if( n.m_pos>n.m_size )
    {
         echo("MPROJ.CPP::mp_ReadByte: read data too big\n");
         n.m_pos = n.m_size;
    }
 }

 //============================================================================
void   mp_WriteInt   ( mp_ProjectTable &p, mp_NodeNum n, int    v )
 {
    n = mp_Code2Int(n);

    if( n>=0 && n<p.m_treePos )
    {
         switch( p.m_open )
         {
         case EDO_NONE:
                 echo("MPROJ.CPP::mp_WriteInt: data not open \n");
                 return;
         case EDO_READ:
                 echo("MPROJ.CPP::mp_WriteInt: data open for read\n");
                 return;
         }
         mp_TreeNode &node = p.m_tree[n];
         mp_WriteByte( p, node, (KR_byte)EDI_INT );
         int     i;
         KR_byte *ptr=(KR_byte*)(&v);

         for( i = 0; i < sizeof(int); ++i, ++ptr )
              mp_WriteByte( p, node, *ptr );

         return;
    }
    echo("MPROJ.CPP::mp_WriteInt: Bad index\n");
 }

 //============================================================================
void   mp_WriteFloat  ( mp_ProjectTable &p, mp_NodeNum n, double v )
 {
    n = mp_Code2Int(n);

    if( n>=0 && n<p.m_treePos )
    {
         switch( p.m_open )
         {
         case EDO_NONE:
                 echo("MPROJ.CPP::mp_WriteFloat: data not open \n");
                 return;
         case EDO_READ:
                 echo("MPROJ.CPP::mp_WriteFloat: data open for read\n");
                 return;
         }
         mp_TreeNode &node = p.m_tree[n];
         mp_WriteByte( p, node, (KR_byte)EDI_DOUBLE );
         int     i;
         KR_byte *ptr=(KR_byte*)(&v);

         for( i = 0; i < sizeof(double); ++i, ++ptr )
              mp_WriteByte( p, node, *ptr );

         return;
    }
    echo("MPROJ.CPP::mp_WriteFloat: Bad index\n");
 }

 //============================================================================
void   mp_WriteStr  ( mp_ProjectTable &p, mp_NodeNum n, const char *str )
 {
    n = mp_Code2Int(n);

    if( n>=0 && n<p.m_treePos )
    {
         switch( p.m_open )
         {
         case EDO_NONE:
                 echo("MPROJ.CPP::mp_WriteStr: data not open \n");
                 return;
         case EDO_READ:
                 echo("MPROJ.CPP::mp_WriteStr: data open for read\n");
                 return;
         }
         mp_TreeNode &node = p.m_tree[n];
         mp_WriteByte( p, node, (KR_byte)EDI_STR );

         for(;;++str)
         {
              mp_WriteByte( p, node, (KR_byte)(*str) );

              if( *str==0 )
                   break;
         }

         return;
    }
    echo("MPROJ.CPP::mp_WriteStr: Bad index\n");
 }


 //============================================================================
void   mp_ReadInt    ( mp_ProjectTable &p, mp_NodeNum n, int    &v )
 {
    n = mp_Code2Int(n);

    if( n>=0 && n<p.m_treePos )
    {
         switch( p.m_open )
         {
         case EDO_NONE:
                 echo("MPROJ.CPP::mp_ReadInt: data not open \n");
                 return;
         case EDO_WRITE:
                 echo("MPROJ.CPP::mp_ReadInt: data open for read\n");
                 return;
         }
         mp_TreeNode &node = p.m_tree[n];
         KR_byte      b;
         KR_byte     *dest = (KR_byte*)(&v);
         mp_ReadByte( node, b );

         if( ((s_EventDataItemStyle)b) != EDI_INT )
         {
              echo("MPROJ.CPP::mp_ReadInt: this is not INT[%s]\n", ed_Tag2Msg(((s_EventDataItemStyle)b)));
              return;
         }

         for( int i = 0; i < sizeof(int); ++i, ++dest )
         {
              mp_ReadByte( node, b );
              *dest =  b;
         }

         return;
    }
    echo("MPROJ.CPP::mp_ReadInt: Bad index\n");
 }

 //============================================================================
void         mp_ReadFloat  ( mp_ProjectTable &p, mp_NodeNum n, double &v )
 {
    n = mp_Code2Int(n);

    if( n>=0 && n<p.m_treePos )
    {
         switch( p.m_open )
         {
         case EDO_NONE:
                 echo("MPROJ.CPP::mp_ReadFloat: data not open \n");
                 return;
         case EDO_WRITE:
                 echo("MPROJ.CPP::mp_ReadFloat: data open for read\n");
                 return;
         }
         mp_TreeNode &node = p.m_tree[n];
         KR_byte      b;
         KR_byte     *dest = (KR_byte*)(&v);
         mp_ReadByte( node, b );

         if( ((s_EventDataItemStyle)b) != EDI_DOUBLE )
         {
              echo("MPROJ.CPP::mp_ReadFloat: this is not INT[%s]\n", ed_Tag2Msg(((s_EventDataItemStyle)b)));
              return;
         }

         for( int i = 0; i < sizeof(v); ++i, ++dest )
         {
              mp_ReadByte( node, b );
              *dest =  b;
         }

         return;
    }
    echo("MPROJ.CPP::mp_ReadFloat: Bad index\n");
 }

 //============================================================================
void         mp_ReadStr    ( mp_ProjectTable &p, mp_NodeNum n, char *v   )
 {
    n = mp_Code2Int(n);

    if( n>=0 && n<p.m_treePos )
    {
         switch( p.m_open )
         {
         case EDO_NONE:
                 echo("MPROJ.CPP::mp_ReadStr: data not open \n");
                 return;
         case EDO_WRITE:
                 echo("MPROJ.CPP::mp_ReadStr: data open for read\n");
                 return;
         }
         mp_TreeNode &node = p.m_tree[n];
         KR_byte      b;
         KR_byte     *dest = (KR_byte*)(&v);
         mp_ReadByte( node, b );

         if( ((s_EventDataItemStyle)b) != EDI_STR )
         {
              echo("MPROJ.CPP::mp_ReadStr: this is not INT[%s]\n", ed_Tag2Msg(((s_EventDataItemStyle)b)));
              return;
         }

         for(;;++v)
         {
              mp_ReadByte( node, b );
              *v =  b;

              if( b==0 )
                   break;
         }

         return;
    }
    echo("MPROJ.CPP::mp_ReadStr: Bad index\n");
 }

 //============================================================================
void         mp_SetLink    (
                             mp_ProjectTable &p, mp_NodeNum n,
                             mp_NodeNum  left,
                             mp_NodeNum  right
                           )
 {
    n = mp_Code2Int(n);

    if( n>=0 && n<p.m_treePos )
    {
         switch( p.m_open )
         {
         case EDO_NONE:
                 echo("MPROJ.CPP::mp_SetLink: data not open \n");
                 return;
         case EDO_READ:
                 echo("MPROJ.CPP::mp_SetLink: data open for read\n");
                 return;
         }
         mp_TreeNode &node = p.m_tree[n];
         node.m_left = mp_Code2Int(left);
         node.m_right= mp_Code2Int(right);
         return;
    }
    echo("MPROJ.CPP::mp_SetLink: Bad index\n");
 }

bool  mp_IsCommanderEqu( mp_ProjectTable &p, mp_NodeNum pn, const char *c )
{
   mp_NodeNum pNULL = mp_NodeNULL();

   for(
       ;
       pn != pNULL;
       pn = projectTable.getRight( pn )
   )
   {
       switch( projectTable.getCommand( pn ) )
       {
       case  COM_0COMMANDER:
        {
             char cName[50];
             mp_OpenData   ( p, pn, EDO_READ );
             mp_ReadStr    ( p, pn, cName );
             mp_CloseData  ( p, pn);

             return strcmp(cName, c)==0;
                 
             
        }

       }
   }
   return true;
}



/* End of file MPROJ.CPP */