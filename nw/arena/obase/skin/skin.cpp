/*
 * File   : D:\GAME\OBASE\Skin\skin.cpp
 * Author : Suavik
 * Ver   1.0
 */

#include <string.h>
#include "skin.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"

#include "message/skinmsg.h"
#include "message/peopmsg.h"

 //===========================================================================
class SkinTable : public ct_ClassTable
{
 private:
    Skin *m_table;
 public:
    SkinTable()
    {
        m_table = NULL;
        registerClass( "Skin" );
    }
    ~SkinTable()
    {
        delete [] m_table;
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static SkinTable  __classTable;
 /*********************************
  *
  *   Skin implementation
  *
  *********************************/

 //============================================================
Skin::Skin()
 {
     startInitialize();
 }

 //============================================================
Skin::~Skin()
 {
 }

 //============================================================
int Skin::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case KR_WAKE_UP:
            break;

    case pe_EV_SETANIM:
            {
            m_isAutoAnim = 1;
            int  num = -1;
            if(  m_animProgSP < m_animProgCnt )
            {
                 event.data.open(EDO_READ)
                        .getInt(num);
                 s_ASSERT(num>=0 && num < m_aniCnt0,"Skin::receiveEvent:pe_EV_SETANIM");
                 m_animProg[m_animProgSP].setAnim(event,num);
                 event.data.close();
                 ++m_animProgSP;
            }
            else echo("Error: animate prog len too big");
            }
            break;

    case sk_EV_LOAD:
            {
            s_ASSERT(!m_loaded,"Skin::receiveEvent():LOAD Dublicate load");
            char fname[100];
            event.data.open(EDO_READ)
                        .getStr(fname,sizeof(fname))
                      .close();
            load(fname);
            }
            break;

    case sk_EV_QUERY_MODEL_PTR:
            {
            s_ASSERT(m_loaded,"Skin::receiveEvent():QUERY_MODEL_PTR");
            
            event.label = sk_EV_QUERY_MODEL_PTR_OK;
            CViewObjectModel *model = &m_model;

            event.data.open(EDO_WRITE)
                        .put(&model,sizeof(void*))
                      .close();
            }
            break;

    case sk_EV_PROG:
            {
             char com[40];
             int  num,num0,progLen;

             event.data.open(EDO_READ)
                         .getStr(com,sizeof(com)); 
             if(  strcmp(com,"set0")==0  )
             {
                  event.data.getInt(num).getStr(com,sizeof(com));
                  set0(num,com);
             }
             else
             if(  strcmp(com,"set")==0  )
             {
                  event.data.getInt(num).getStr(com,sizeof(com));
                  set(num,com);
             }
             else
             if(  strcmp(com,"createAniSets")==0  )  
             {
                  event.data.getInt(num).getInt(num0);
                  createAniSets(num,num0);
             }
             else 
             if(  strcmp(com,"createAniSetsAuto")==0  )  
             {
                  event.data.getInt(num).getInt(num0).getInt(progLen);
                  createAniSets(num,num0,progLen);
             }
             else s_ASSERTNQ1("Skin::receiveEvent.sk_EV_PROG: Unknown command %s",com);
             event.data.close();
            }
            break;
    default: return 0;
    }
    return 1;
 }

 //============================================================
void Skin::addNotify()
 {
    ct_Object::addNotify();
    // insert your code here
 }

 //============================================================
void Skin::removeNotify()
 {
    ct_Object::removeNotify();
    // insert your code here
    removeAniSets();
 }

 /*************************************
  *
  *   SkinTable implementation
  *
  *************************************/

 //============================================================
void SkinTable::allocObjects( int objectQnty )
 {
    m_table = new Skin[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void SkinTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *SkinTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"SkinTable::getObjectPTR");
    return &(m_table[ index ]);
 }


void Skin::load( const char *fileName )
{
    CTaggedFile f(TRUE);
    f.Open(fileName);
      m_model.Read(f,TRUE);
    f.Close();
    m_model.Split(TRUE);
    m_loaded = true;
}

void Skin::createAniSets( int aniCnt, int aniCnt0 )
{
    s_ASSERT(m_loaded,"Skin::createAniSets");
    removeAniSets();

    int redCnt = m_model.NumBaseSets(),i;
    m_reducNum = redCnt;
    m_aniCnt  = aniCnt;
    m_aniCnt0 = aniCnt0;

    if(  m_aniCnt != 0  )
    {
         int size = aniCnt * redCnt;
         m_array  = new AniCell [size];
         for( i = 0; i < size; ++i )
         {
               m_array[i].m_use = 0;
               m_array[i].m_fs  = 0;
         }
    }
    if(  m_aniCnt0!= 0  )
    {
         int size = aniCnt0 * redCnt;
         m_array0 = new AniCell0[size];
         for( i = 0; i < size; ++i )
         {
               m_array0[i].m_use = 0;
               m_array0[i].m_fs  = 0;
         }
    }
}

void Skin::createAniSets( int aniCnt, int aniCnt0, int progLen )
{
    createAniSets( aniCnt, aniCnt0 );
    if(  progLen!= 0  )
    {
         m_animProgCnt = progLen;
         m_animProg = new AnimateInfo[progLen];

         for( int i = 0; i < progLen; ++i )
              m_animProg[i].startInitialize(this);
    }
}

void Skin::removeAniSets()
{
    delete [] m_array;    m_array  = 0;
    delete [] m_array0;   m_array0 = 0;
    delete [] m_animProg; m_animProgCnt = 0;
}

void Skin::set ( int set, const char *name )
{
 //&(m_model->BaseSet(0).Base(reduc).KFSet().Mod(aniBlock));
    s_ASSERT( set<m_aniCnt, "Skin::set. Range Check Error" );
    int base = set*m_reducNum;

    for( int i = 0; i < m_reducNum; ++i )
    {
         AniCell &cell = m_array[base+i];
         cell.m_use = 1;
         cell.m_fs = &(m_model.BaseSet(0).Base(i).KFSet().Mod(name));
    }
}

void Skin::set0( int set, const char *name )
{
    s_ASSERT( set<m_aniCnt0, "Skin::set. Range Check Error" );
    int base = set*m_reducNum;

    for( int i = 0; i < m_reducNum; ++i )
    {
         AniCell0 &cell = m_array0[base+i];
         cell.m_use = 1;
         cell.m_fs = &(m_model.BaseSet(0).Base(i).KFSet().Mod0(name));
    }
}

CViewBaseModifier  *Skin::get ( int set, int reduc )
{
    s_ASSERT( set<m_aniCnt, "Skin::set. Range Check Error" );
    AniCell &cell = m_array[ m_reducNum*set+reduc ];
    s_ASSERT1(cell.m_use,"Skin::get. cell not init %i",set);
    return cell.m_fs;
}

CViewBaseModifier0 *Skin::get0( int set, int reduc )
{
    s_ASSERT( set<m_aniCnt0, "Skin::set. Range Check Error" );
    AniCell0 &cell = m_array0[ m_reducNum*set+reduc ];
    s_ASSERT1(cell.m_use,"Skin::get. cell not init %i",set);
    return cell.m_fs;
}

void  *Skin::queryInterface(int iid )
{
    if(  iid == ISkinIID  )
         return (ISkin*)this;
    return 0;
}

void Skin::runAutoProg( CViewObjectBaseSet *bs, CViewObjectBase *ob, int )
{
    for( int i = 0; i < m_animProgSP; ++i )
         m_animProg[i].animateProg( bs, ob );
}

int Skin::isAutoAnim()
{
    return m_isAutoAnim;
}


void AnimateAutoCallBack(CViewObjectBaseSet *bs,CViewObjectBase *pBase,CViewObjectRef *ref)
{
    Skin &self  = *((Skin*)(ref->GetUserAttrib()));
    int reduc = pBase->ReductionNum();
    self.runAutoProg(bs,pBase,reduc);
}

void Skin::skinSetAnimAuto(CViewObjectRef *ref)
{
    ref->SetUserAttrib(this);
    ref->SetAnimationCallback(AnimateAutoCallBack); 
}

/* End of file D:\GAME\OBASE\Skin\skin.cpp */
