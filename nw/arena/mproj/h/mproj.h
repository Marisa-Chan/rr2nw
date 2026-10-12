/*
   File:  Suavik\d:\game\mproj\h\mproj.h
   Autor: Suavik
   Ver    1.0
 */
#ifndef __MPROJ_H__
#define __MPROJ_H__

#ifndef __CLASSTABLE_H__
#include "storage/h/subject.h"
#endif

typedef int mp_NodeNum;

class mp_TreeNode;


inline mp_NodeNum mp_Int2Code( int x )
 {
    return (x^0xFACE)+0x1234;
 }

inline int mp_Code2Int( mp_NodeNum x )
 {
    return (x-0x1234)^0xFACE;
 }

inline mp_NodeNum mp_NodeNULL()
 {
    return mp_Int2Code(-1);
 }

 //===========================================================================
class mp_ProjectHeap
 {
 public:
    KR_byte       *m_data;
    int            m_size;
    int            m_pos;

    void           create(int size);
    void           remove();
    KR_byte       *getCurHeap();
    void           step(int size);
     mp_ProjectHeap();
    ~mp_ProjectHeap();
 };


typedef struct {
    int            m_treeNode;
	int			   m_permanent;
} mp_ProjectData;

class PIN_SaveFile;

 //===========================================================================
class mp_Project: public ct_Object, public mp_ProjectData
 {
 public:

    void           set( int node ) { m_treeNode = mp_Code2Int(node); }
    int            get()  const    { return mp_Int2Code(m_treeNode); }
    virtual int    receiveEvent( KR_Event &event );
    mp_Project() { m_treeNode = -1; m_permanent = 0; }
    ~mp_Project();

	virtual bool	shouldDump () { return true; }  
	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
};


 //===========================================================================
class mp_ProjectTable: public ct_ClassTable
 {
 friend int mp_New( mp_ProjectTable &p, int command, int left, int right );
 friend void         mp_WriteByte( mp_ProjectTable &p, mp_TreeNode &n, KR_byte b );
 friend void         mp_OpenData   ( mp_ProjectTable &p, mp_NodeNum n, s_EventDataOpen edo );
 friend void         mp_CloseData  ( mp_ProjectTable &p, mp_NodeNum n );
 friend void         mp_WriteInt   ( mp_ProjectTable &p, mp_NodeNum n, int    v );
 friend void         mp_WriteFloat ( mp_ProjectTable &p, mp_NodeNum n, double v );
 friend void         mp_WriteStr   ( mp_ProjectTable &p, mp_NodeNum n, const char *v );
 friend void         mp_ReadInt    ( mp_ProjectTable &p, mp_NodeNum n, int    &v );
 friend void         mp_ReadFloat  ( mp_ProjectTable &p, mp_NodeNum n, double &v );
 friend void         mp_ReadStr    ( mp_ProjectTable &p, mp_NodeNum n, char *v   );
 friend void         mp_SetLink    (
                             mp_ProjectTable &p, mp_NodeNum node,
                             mp_NodeNum  left,
                             mp_NodeNum  right
                           );
 protected:
    mp_TreeNode     *m_tree;
    mp_ProjectHeap   m_heap;
    int              m_treeSize;
    int              m_treePos;

    mp_Project      *m_table;
    s_EventDataOpen  m_open;

    virtual  void        allocObjects( int objectQnty );
    virtual  void        freeObjects ();
    virtual  ct_Object  *getObjectPTR( int index );
    
    void                 createTreeHeap( int size );
    void                 createDataHeap( int size ) { m_heap.create(size); }
    KR_byte             *getCurHeap() { return m_heap.getCurHeap(); }
 public:
                     mp_ProjectTable();
    virtual         ~mp_ProjectTable();

	mp_Project      *searchProject( const KR_ObjectID &oID ) const;

    void                 create(
                                 int                  objectQnty,
                                 SimulationContext   *context,
                                 ct_Storage          &storage,
                                 int                  treeSize,
                                 int                  heapSize
                               );

    KR_ObjectID          newProject( const char *pname, int node , int permanent);

    /*
     *  User functions
     */
    mp_NodeNum           getProjectRoot( const KR_ObjectID &OID );
    int                  getCommand( mp_NodeNum n ) const;
    mp_NodeNum           getRight  ( mp_NodeNum n ) const;
    mp_NodeNum           getLeft   ( mp_NodeNum n ) const;

	bool	dump(PIN_SaveFile & sf);
	bool	load(PIN_SaveFile & );

 };


bool         mp_IsCommanderEqu( mp_ProjectTable &p, mp_NodeNum pn, const char *c );

mp_NodeNum   mp_New        ( mp_ProjectTable &p, int command, int left, int right );
void         mp_OpenData   ( mp_ProjectTable &p, mp_NodeNum n, s_EventDataOpen edo );
void         mp_CloseData  ( mp_ProjectTable &p, mp_NodeNum n );
void         mp_WriteInt   ( mp_ProjectTable &p, mp_NodeNum n, int         v );
void         mp_WriteFloat ( mp_ProjectTable &p, mp_NodeNum n, double      v );
void         mp_WriteStr   ( mp_ProjectTable &p, mp_NodeNum n, const char *v );
void         mp_ReadInt    ( mp_ProjectTable &p, mp_NodeNum n, int    &v );
void         mp_ReadFloat  ( mp_ProjectTable &p, mp_NodeNum n, double &v );
void         mp_ReadStr    ( mp_ProjectTable &p, mp_NodeNum n, char   *v );

void         mp_SetLink    (
                             mp_ProjectTable &p, mp_NodeNum node,
                             mp_NodeNum  left,
                             mp_NodeNum  right
                           );
extern mp_ProjectTable projectTable;
#endif

/* End of file MPROJ.H */