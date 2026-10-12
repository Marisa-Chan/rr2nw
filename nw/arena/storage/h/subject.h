/*
    File:  Suavik\d:\game\storage\h\subject.h
    Autor: Suavik
    Ver    1.0
 */
#ifndef __SUBJECT_H__
#define __SUBJECT_H__

#ifndef __CLASSTAB_H__
#include "storage/h/classtab.h"
#endif
#include "kernel/h/s_debug.h"


#include "mathlib.h"

#define SCENE_DELTA 0.1

class   ct_Subject;
class   CViewScene;
class   CViewDynamicList;

typedef ct_Subject *ct_SubjectPTR;


//===========================================================================
class ct_SubjectFindData
 {
 friend class ct_Arena;

 protected:
     enum{
        MAX_VIEW_OBJECT = 255
     };
     int         m_qnty,
                 m_pos;
     KR_ObjectID m_list[MAX_VIEW_OBJECT];

 public:
     ct_SubjectFindData()
     {
         m_qnty = 0;
         m_pos  = 0;
     }
     int   getCount() const { return m_qnty; }
     const KR_ObjectID &operator [] (int index)
     {
          s_ASSERT2( index>=0 && index < m_qnty,
                     "ct_SubjectFindData:operator[%i] of %i",
                     index, m_qnty);
          return m_list[index];
     }
 };



typedef struct {
    int	        m_audibleThisFrame,
                m_isVisible;
    double      m_lastMoveTimeStamp;
    CFVector3   m_position;
} SubjectData;

//===========================================================================

class PIN_SaveFile ;

class ct_Subject:	public ct_Object,
					public SubjectData
 {
 friend class ct_SubjectTable;
 friend class ct_Arena;
 private:
    void             dropInCachePos( ct_SubjectPTR &list );
    void             insertToCachePos( ct_SubjectPTR &list );

 protected:


    ct_Subject      *m_prevSubjectInCell,
                    *m_nextSubjectInCell;
    int              m_inCellPos;
    ct_Subject      *m_nextRender;

 public:
    virtual CFVector3    realPosition() = 0;
    void                 setPosition ( const CFVector3 &vec );
    const CFVector3      &getPosition () const;

    virtual int           receiveEvent(KR_Event &event) = 0;
    virtual void          addNotify   ();
    virtual void          removeNotify();
    virtual OBJECT_STYLE  style       () const;
    virtual void          render      ( CViewDynamicList &list, double ts );
    virtual void          endRender   ( CViewScene *scene );
    virtual void          onView(double time);
    virtual void          onHide(double time);
    ct_Subject           *getNextRender() const { return m_nextRender; }
    ct_Subject();
    virtual ~ct_Subject() {}

    virtual void          onExitAudibleZone (double ts);
    virtual void          onEnterAudibleZone(double ts);

	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & sf);
	virtual void	loadNotify();
 };


//===========================================================================
class ct_SubjectTable: public ct_ClassTable
 {
 friend class ct_Arena;
 friend class ct_Subject;
 protected:
    ct_SubjectTable     *m_nextSubjectTable;
    ct_SubjectTable     *m_nextViewSubjectTable;
    ct_SubjectTable     *m_nextAudibleSubjectTable;

    virtual  void        allocObjects      ( int objectQnty ) = 0;
    virtual  void        freeObjects       ()                 = 0;
    virtual  ct_Object  *getObjectPTR      ( int index )      = 0;

    virtual  void        create(
                                 int                  objectQnty,
                                 SimulationContext   *context,
                                 ct_Storage          &storage
                               );
 public:
    virtual  int         abstractLevel();
    virtual OBJECT_STYLE objectsType() const;

    virtual  bool        isRendering();
    virtual  bool        isAudible();

    ct_Subject          *findFirstSubject() const;
    ct_Subject          *findNextSubject( const ct_Subject *s ) const;
             ct_SubjectTable() 
             {
                  m_nextSubjectTable     = NULL; 
                  m_nextViewSubjectTable = NULL;
		  m_nextAudibleSubjectTable = NULL;
             }
    virtual ~ct_SubjectTable() {}
 };

//===========================================================================

class ct_Arena: public ct_Storage
 {
 friend class ct_SubjectTable;
 friend class ct_Subject;
 protected:
    enum{
        CACHE_POS_POW  = 8,
        CACHE_POS_SIZE = 1<<CACHE_POS_POW
    };

    static ct_SubjectTable   *m_seanceSubjectTableList;
    static ct_SubjectTable   *m_seanceViewSubjectTableList;
    static ct_SubjectTable   *m_seanceAudibleSubjectTableList;

    static ct_Subject       **m_cacheSubjectPos;

    static double           m_sceneWidth;
    static double           m_sceneDepth;
    static double           m_sceneWidth_D;
    static double           m_sceneDepth_D;
    static double           m_sceneWidth_S;
    static double           m_sceneDepth_S;
    static ct_Subject      *m_renderList;

 public:

    void                 openSeance(
                                    SimulationContext *context,
                                    double             sceneWidth,
                                    double             sceneDepth
                                   );
    ct_ClassTableID  addClassTable( const char *name, int poolQnty );
    void                 closeSeance();

    ct_ClassTableID      findFirstSubjectTableID();
    ct_ClassTableID      findNextSubjectTableID( ct_ClassTableID ctID );

    ct_SubjectTable     *findFirstViewSubjectTable(); // inline
    ct_SubjectTable     *findNextViewSubjectTable( ct_SubjectTable *st ); // inline

    ct_SubjectTable     *findFirstAudibleSubjectTable(); // inline
    ct_SubjectTable     *findNextAudibleSubjectTable( ct_SubjectTable *st ); // inline


    ct_SubjectTable     *findFirstSubjectTable();
    ct_SubjectTable     *findNextSubjectTable( ct_SubjectTable *st );

	static void			 clearCache();

    static KR_ObjectID   findFirstSubject(
                                          ct_SubjectFindData &fsd,
                                          double              x0,
                                          double              z0,
                                          double              x1,
                                          double              z1
                                         );
    static KR_ObjectID   findNextSubject( ct_SubjectFindData &fsd );

    void                 render( const CFVector3 &plPos, double radius, CViewDynamicList &list );
    void                 endRender( CViewScene *scene );
    ct_Subject          *getRenderList() const { return m_renderList; }

    ct_Arena()
    {
        m_seanceSubjectTableList = NULL;
        m_cacheSubjectPos        = NULL;
        m_sceneWidth = 0;
        m_sceneDepth = 0;
    }

    ~ct_Arena()
    {
        delete [] m_cacheSubjectPos;
    }


	virtual bool shouldDump () { return false; } // dedicated object
 };

inline 
ct_SubjectTable *ct_Arena::findFirstViewSubjectTable()
 {
    return m_seanceViewSubjectTableList;
 }

inline
ct_SubjectTable *ct_Arena::findNextViewSubjectTable( ct_SubjectTable *st )
 {
    return st->m_nextViewSubjectTable;
 }


inline 
ct_SubjectTable *ct_Arena::findFirstAudibleSubjectTable()
 {
    return m_seanceAudibleSubjectTableList;
 }

inline
ct_SubjectTable *ct_Arena::findNextAudibleSubjectTable( ct_SubjectTable *st )
 {
    return st->m_nextAudibleSubjectTable;
 }



extern ct_Arena g_arena;
#endif

/* End of file SUBJECT.H */