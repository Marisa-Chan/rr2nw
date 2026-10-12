/*
   File:  Suavik\d:\game\storage\subject.cpp
   Autor: Suavik
   Ver    1.0

   Создание таблиц объектов, которые можно визуализировать.

   Префикс ct_

   ct_Subject:                      28.10.97
      ct_Subject()                  *
      addNotify()                   *
      removeNotify()                *
      insertToCachePos()            *
      dropInCachePos()              *
      setPosition()                 *
      getPosition()                 *

   ct_SubjectTable:
      create()                      *
      abstractLevel()               *
      findFirstSubject()            *
      findNextSubject()             *

   ct_Arena:
      openSeance()                  *
      closeSeance()                 *
      findFirstSubjectTableID()     *
      findNextSubjectTableID()      *
      findFirstSubjectTable()       *
      findNextSubjectTable()        *
      findFirstSubject()            *
      findNextSubject()             *

   * тест на вызов

      03.12.97 Метка первого сообщения переставлена на KR_USER_EVENT_LABEL.
               Координата z переставлена на отрицательные значения.
               setPosition() работал не со значениями, загнанными в диапозон,
               а с передоваемыми значениями.

      04.12.97 в addNotify() добавлен вызов ct_Object::addNotify()

      12.12.97 Change "delete mmm" to "delete [] mmm".
               closeSeance - вызывал pодительский после уничтожения таблицы
               положения объектов. Пpи последующем удалении объектов они
               пытались исключить себя из таблицы.

      25.12.97 Таблица указателей на ct_Subject увеличена на 1 и стаpтовое
               значение устанавливается на эту последнюю невидимую для
               пpосмотpа ячейку.
 */

#define LAST_H__VIEW
#include "game.h"
#include "kernel/h/echo.h"
#include "kernel/h/context.h"
#include "kernel/h/session.h"
#include "storage/h/subject.h"
#include "sound.h"
#include "storage/h/savefile.h"

#define ct_MAX_OBJECT_WIDTH 30.0


ct_SubjectTable *ct_Arena::m_seanceSubjectTableList = NULL;
ct_Subject     **ct_Arena::m_cacheSubjectPos = NULL;

ct_SubjectTable *ct_Arena::m_seanceViewSubjectTableList    = NULL;
ct_SubjectTable *ct_Arena::m_seanceAudibleSubjectTableList = NULL;

ct_Subject      *ct_Arena::m_renderList = NULL;

double           ct_Arena::m_sceneWidth;
double           ct_Arena::m_sceneDepth;
double           ct_Arena::m_sceneWidth_D;
double           ct_Arena::m_sceneDepth_D;
double           ct_Arena::m_sceneWidth_S;
double           ct_Arena::m_sceneDepth_S;

  /******************************
   *
   *        ct_Subject
   *
   ******************************/

//===========================================================================
OBJECT_STYLE ct_Subject::style() const
 {
    return OBJECT_SUBJECT;
 }

//===========================================================================
ct_Subject::ct_Subject()
 {
    m_prevSubjectInCell = NULL;
    m_nextSubjectInCell = NULL;
    m_inCellPos         = -1;
    m_position          = CFVector3(0,0,0);
 }

//===========================================================================
void ct_Subject::onExitAudibleZone(double)
{
}
//===========================================================================
void ct_Subject::onEnterAudibleZone(double)
{
}


bool	ct_Subject::dump(PIN_SaveFile & sf)
{
		if (!sf.WriteData( (char *) & m_audibleThisFrame, sizeof(SubjectData)  ))
			return false;

		return true;
}

bool	ct_Subject::load(PIN_SaveFile & sf)
{
		if (!sf.GetData( (char *) & m_audibleThisFrame, sizeof(SubjectData)  ))
			return false;
		
		return true;
}


void	ct_Subject::loadNotify()
{
	setPosition( m_position );
}

//===========================================================================

void ct_Subject::addNotify()
 {
    ct_Object::addNotify();
    m_position  = CFVector3(0,0,0);
    m_inCellPos = ct_Arena::CACHE_POS_SIZE * ct_Arena::CACHE_POS_SIZE;
    insertToCachePos( ct_Arena::m_cacheSubjectPos[m_inCellPos] );
    m_nextRender       = NULL;
    m_audibleThisFrame = 0;
    m_isVisible        = 0;
 }

//===========================================================================
void ct_Subject::removeNotify()
 {
    dropInCachePos( ct_Arena::m_cacheSubjectPos[m_inCellPos] );
    m_inCellPos = -1;

    m_prevSubjectInCell = NULL;
    m_nextSubjectInCell = NULL;
    ct_Object::removeNotify();
 }

//===========================================================================
inline void ct_Subject::insertToCachePos( ct_SubjectPTR &list )
 {
    m_nextSubjectInCell    = list;

    if(  list != NULL  )
         list->m_prevSubjectInCell = this;

    m_prevSubjectInCell   = NULL;
    list                  = this;
 }

//===========================================================================
inline void ct_Subject::dropInCachePos( ct_SubjectPTR &list )
 {
    if(  m_prevSubjectInCell != NULL  )
    {
         if(  m_nextSubjectInCell != NULL  )
              m_nextSubjectInCell
                      ->m_prevSubjectInCell = m_prevSubjectInCell;

         m_prevSubjectInCell
                 ->m_nextSubjectInCell = m_nextSubjectInCell;
    }
    else
    {
         list = m_nextSubjectInCell;
         if(  list != NULL  )
              list->m_prevSubjectInCell = NULL;
    }
 }

//===========================================================================
void ct_Subject::setPosition( const CFVector3 &vec )
 {
    double x = vec.x,
           y = vec.y,
           z = vec.z;
    int    cx,cz, pos;

    if(  x < SCENE_DELTA  )
         x = SCENE_DELTA;
    else
    if(  x >= ct_Arena::m_sceneWidth_S  )
         x = ct_Arena::m_sceneWidth_S;

    if(  z > -SCENE_DELTA  ) z = -SCENE_DELTA;
    else
    if(  z < ct_Arena::m_sceneDepth_S  )
         z = ct_Arena::m_sceneDepth_S;

    m_position.x = x;
    m_position.y = y;
    m_position.z = z;

    cx = (int)( x * ct_Arena::m_sceneWidth_D );
    cz = (int)( z * ct_Arena::m_sceneDepth_D );
    pos = cx+(cz<<ct_Arena::CACHE_POS_POW);
    if(  pos!=m_inCellPos  )
    {
         s_ASSERT(pos>=0 && pos < 256*256,"ct_Subject::setPosition");//FIXME
         dropInCachePos  ( ct_Arena::m_cacheSubjectPos[m_inCellPos] );
         insertToCachePos( ct_Arena::m_cacheSubjectPos[ pos       ] );
         m_inCellPos = pos;
    }
 }

//===========================================================================
const CFVector3 &ct_Subject::getPosition() const
 {
    return m_position;
 }

 //==========================================================================
void ct_Subject::render( CViewDynamicList &, double )
 {
 }

void ct_Subject::endRender( CViewScene * )
 {
 }

  /******************************
   *
   *        ct_SubjectTable
   *
   ******************************/




//===========================================================================
OBJECT_STYLE ct_SubjectTable::objectsType() const
 {
    return OBJECT_SUBJECT;
 }


//===========================================================================
void ct_SubjectTable::create(
                              int                  objectQnty,
                              SimulationContext   *context,
                              ct_Storage          &storage
                            )
 {
    ct_ClassTable::create( objectQnty, context, storage );
    m_nextSubjectTable = ct_Arena::m_seanceSubjectTableList;
    ct_Arena::m_seanceSubjectTableList = this;
 }

//===========================================================================
int  ct_SubjectTable::abstractLevel()
 {
    return 1;
 }

//===========================================================================
ct_Subject *ct_SubjectTable::findFirstSubject() const
 {
    return (ct_Subject*)m_existList;
 }

//===========================================================================
ct_Subject *ct_SubjectTable::findNextSubject( const ct_Subject *s ) const
 {
    if(  s==NULL  )
         return NULL;
    return (ct_Subject*)(s->m_next);
 }

//===========================================================================
bool ct_SubjectTable::isRendering()
{
    return false;
}

bool ct_SubjectTable::isAudible()
{
    return false;
}

  /******************************
   *
   *        ct_Arena
   *
   ******************************/

void  ct_Arena::clearCache()
{
	for(int i = 0; i < CACHE_POS_SIZE*CACHE_POS_SIZE+1; ++i )
         m_cacheSubjectPos[ i ] = NULL;
}


//===========================================================================
void  ct_Arena::openSeance(
                           SimulationContext  *context,
                           double              sceneWidth,
                           double              sceneDepth
                          )
 {

    ct_Storage::openSeance( context );
    m_seanceSubjectTableList = NULL;


    m_cacheSubjectPos = new ct_SubjectPTR[ CACHE_POS_SIZE*CACHE_POS_SIZE+1 ];

	clearCache();

    m_sceneWidth   = sceneWidth;
    m_sceneDepth   = sceneDepth;

    m_sceneWidth_D =  CACHE_POS_SIZE / sceneWidth;
    m_sceneDepth_D =- CACHE_POS_SIZE / sceneDepth;

    m_sceneWidth_S =   sceneWidth - SCENE_DELTA;
    m_sceneDepth_S = -(sceneDepth - SCENE_DELTA);    
 }

//===========================================================================
void ct_Arena::closeSeance()
 {
    for(
         ct_SubjectTable  *sTab = findFirstSubjectTable();
         sTab != NULL;
         sTab =  findNextSubjectTable( sTab )
       )
       {
         sTab->m_nextViewSubjectTable = NULL;
	 sTab->m_nextAudibleSubjectTable = NULL;
       }

    ct_Storage::closeSeance();

    delete [] m_cacheSubjectPos;
    m_cacheSubjectPos = NULL;

    m_seanceSubjectTableList     = NULL;
    m_seanceViewSubjectTableList = NULL;
    m_seanceAudibleSubjectTableList = NULL;

    m_sceneWidth   = 0;
    m_sceneDepth   = 0;
 }


//===========================================================================
ct_ClassTableID  ct_Arena::findFirstSubjectTableID()
 {
    if(  m_seanceSubjectTableList == NULL  )
         return ct_NULLID;

    return m_seanceSubjectTableList->getClassTableID();
 }

//===========================================================================
ct_ClassTableID  ct_Arena::findNextSubjectTableID( ct_ClassTableID ctID )
 {
    if(  ctID>=0  &&  ctID<m_seanceClassTableQnty  )
    {
         ct_SubjectTable *ct = (ct_SubjectTable*)
                               (m_seanceClassTablePool[ ctID ]);

         if(  ct->abstractLevel() == 1  ) // Control
         {
              if(  ct->m_nextSubjectTable==NULL  )
                   return ct_NULLID;

              return ct->m_nextSubjectTable->getClassTableID();
         }
         else s_ASSERTNQ("ct_Arena::findNextSubjectTableID: "
                         "Detect not SubjectTable");
    }

    return ct_NULLID;
 }

//===========================================================================
ct_SubjectTable *ct_Arena::findFirstSubjectTable()
 {
    return m_seanceSubjectTableList;
 }


//===========================================================================
ct_SubjectTable *ct_Arena::findNextSubjectTable( ct_SubjectTable *st )
 {
    if(  st==NULL  )
         return NULL;

    return st->m_nextSubjectTable;
 }

//===========================================================================
#define ct_SWAP(x,y)  { t = x; x = y; y = t;}
#define ct_TODIAP(x)  if(x<0) x=0; else if(x>=CACHE_POS_SIZE) x =CACHE_POS_SIZE-1;
//===========================================================================
KR_ObjectID ct_Arena::findFirstSubject(
                                  ct_SubjectFindData &fsd,
                                  double              x0,
                                  double              z0,
                                  double              x1,
                                  double              z1
                                 )
 {
    int    i, j;
    int    cx0, cz0, cx1, cz1;
    int    t;
    int    deltax = (int)(ct_MAX_OBJECT_WIDTH * m_sceneWidth_D) + 1;
    int    deltaz = (int)(ct_MAX_OBJECT_WIDTH * m_sceneDepth_D) + 1;


    cx0 = (int) (x0 * m_sceneWidth_D);
    cx1 = (int) (x1 * m_sceneWidth_D);

    cz0 = (int) (z0 * m_sceneDepth_D);
    cz1 = (int) (z1 * m_sceneDepth_D);

    if(  cx0 > cx1  )
         ct_SWAP(cx0,cx1)

    if(  cz0 > cz1  )
         ct_SWAP(cz0,cz1)


    cx0 -= deltax;
    cx1 += deltax;
    cz0 -= deltaz;
    cz1 += deltaz;

    ct_TODIAP(cx0)
    ct_TODIAP(cx1)
    ct_TODIAP(cz0)
    ct_TODIAP(cz1)

    fsd.m_qnty = 0;
    fsd.m_pos  = 0;

    for( i = cx0; i<=cx1; ++i )
         for( j = cz0; j<=cz1; ++j )
         {
              ct_Subject *slist = m_cacheSubjectPos[i+(j<<CACHE_POS_POW)];

              for(; slist!=NULL; slist = slist->m_nextSubjectInCell )
              {
                   fsd.m_list[ fsd.m_qnty ] = slist->getObjectID();
                   ++fsd.m_qnty;

                   if(  fsd.m_qnty >= ct_SubjectFindData::MAX_VIEW_OBJECT  )
                        goto breakOfSearch;
              }
         }

    breakOfSearch:

    if(  fsd.m_qnty == 0  )
         return KR_ObjectID(-1,-1);

    fsd.m_pos = 1;
    return fsd.m_list[0];
 }

//===========================================================================
KR_ObjectID ct_Arena::findNextSubject( ct_SubjectFindData &fsd )
 {
    if(  fsd.m_pos < fsd.m_qnty  )
    {
         ++fsd.m_pos;
         return fsd.m_list[fsd.m_pos-1];
    }

    return KR_ObjectID(-1,-1);
 }


//===========================================================================
ct_ClassTableID  ct_Arena::addClassTable( const char *name, int poolQnty )
 {
    ct_ClassTableID  ctID = ct_Storage::addClassTable(name,poolQnty);

    if(  ctID >= 0  )
    {
         ct_ClassTable *ct = m_seanceClassTablePool[ctID];

         if(  ct->objectsType() == OBJECT_SUBJECT  )
         {
              ct_SubjectTable *st = (ct_SubjectTable*)(ct);

              if(  st->isRendering()  )
              {
                   if(  m_seanceViewSubjectTableList == NULL  )
                        m_seanceViewSubjectTableList = st;
                   else
                   {
                        st->m_nextViewSubjectTable = m_seanceViewSubjectTableList;
                        m_seanceViewSubjectTableList = st;
                   }
              }

              if(  st->isAudible()  )
              {
                   if(  m_seanceAudibleSubjectTableList == NULL  )
                        m_seanceAudibleSubjectTableList = st;
                   else
                   {
                        st->m_nextAudibleSubjectTable = m_seanceAudibleSubjectTableList;
                        m_seanceAudibleSubjectTableList = st;
                   }
              }

         }
    }
    return ctID;
 }

void ct_Subject::onView(double)
{
}

void ct_Subject::onHide(double)
{
}

//===========================================================================
void ct_Arena::render( const CFVector3 &plPos, double radius, CViewDynamicList &list )
 {
    double radius2 = radius*radius;
    m_renderList = NULL;
    double startViewTime = Session::m_realTimer->GetTime();
    ct_Subject *next = NULL;

    for(
         ct_SubjectTable *st = findFirstViewSubjectTable();
         st != NULL;
         st = findNextViewSubjectTable( st )
       )
         for( 
              ct_Subject *s = st->findFirstSubject();
              s != NULL;
              s = next
            )
         {
              next = st->findNextSubject( s );

              double dist = Abs2(s->realPosition()-plPos);		      

              if(  dist < radius2 )
              {  
                   if(  !s->m_isVisible   )
                   {
                        s->m_isVisible = 1;
                        s->onView(Session::m_moment);
                   }
                   s->render( list, startViewTime );
                   s->m_nextRender = m_renderList;
                   m_renderList = s;
              }
              else
              if(  s->m_isVisible  )
              {
                   s->m_isVisible = 0;
                   s->onHide(Session::m_moment);
              }
         }

    // Now audible objects

    for( ct_SubjectTable *
         st = findFirstAudibleSubjectTable();
         st != NULL;
         st = findNextAudibleSubjectTable( st )
       )

         for( 
              ct_Subject *s = st->findFirstSubject();
              s != NULL;
              s = next
            )
	  {
              next = st->findNextSubject( s );

	      double dist = Abs2(s->realPosition()-plPos);		      

	      if(  dist < snd_distMax2)	
	      {
		   if (!s->m_audibleThisFrame)
		   {
			s->m_audibleThisFrame = 1;
			s->onEnterAudibleZone(Session::m_moment);
 		   }
	      }
		else
	      if (s->m_audibleThisFrame)
	      {
		     s->m_audibleThisFrame = 0;
		     s->onExitAudibleZone(Session::m_moment);
	      }	
	}
 }

//===========================================================================
void ct_Arena::endRender( CViewScene *scene )
 {
    for(
         ct_Subject *s = m_renderList;
         s != NULL;
         s = s->m_nextRender
       )
         s->endRender( scene );
 }

ct_Arena g_arena;
/* End of file SUBJECT.CPP */
