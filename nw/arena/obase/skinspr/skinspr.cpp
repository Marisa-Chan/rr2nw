/*
 * File  : C:\NW\ARENA\OBASE\SkinSpr\SkinSpr.cpp
 * Autor :
 * Ver   1.0 
 */
#include "SkinSpr.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/skinmsg.h"
#include "graph.h"

 //===========================================================================
class SkinSprTable : public ct_ClassTable
{
 private:
    SkinSpr *m_table;
 public:
    SkinSprTable()
    {
        m_table = NULL;
        registerClass( "SkinSpr" );
    }
    ~SkinSprTable()
    {
        delete [] m_table;
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static SkinSprTable  __classTable;
 /*********************************
  *
  *   SkinSpr implementation
  *
  *********************************/

 //============================================================
SkinSpr::SkinSpr()
 {
    startInitialize();
 }

 //============================================================
SkinSpr::~SkinSpr()
 {
 }


static void CallBack(byte *pImage,SRGB*)
{
   (void)pImage;
   ((dword*)pImage)[-3] = TEXTURE_ALPHA|TEXTURE_SPRITE|TEXTURE_TXR_FORMAT;
}

 //============================================================
int SkinSpr::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case KR_WAKE_UP:
            break;

    case sk_EV_LOAD:
            {
            s_ASSERT(!m_loaded,"SkinSpr::receiveEvent():LOAD Dublicate load");
            char fname[100];
            event.data.open(EDO_READ)
                        .getStr(fname,sizeof(fname))
                      .close();
            //CTaggedFile f(TRUE);
            //f.Open(fname);
            //  m_texture.Read(f,TRUE,CallBack);
            //f.Close();
            m_texture.Read(fname, TRUE, CallBack);
            RTCHECK(m_texture.HImage(),"SkinSpr::receiveEvent:sk_EV_LOAD");
            m_loaded = true;
            }
            break;

    case sk_EV_QUERY_MODEL_PTR:
            {
            s_ASSERT(m_loaded,"SkinSpr::receiveEvent():QUERY_MODEL_PTR");
            
            event.label = sk_EV_QUERY_MODEL_PTR_OK;
            CViewTexture *model = &m_texture;

            event.data.open(EDO_WRITE)
                        .put(&model,sizeof(void*))
                      .close();
            }
            break;

    default: return 0;
    }
    return 1;
 }

 //============================================================
void SkinSpr::addNotify()
 {
    ct_Object::addNotify();
    // insert your code this
 }

 //============================================================
void SkinSpr::removeNotify()
 {
    ct_Object::removeNotify();
    // insert your code this
 }

 /*************************************
  *
  *   SkinSprTable implementation
  *
  *************************************/

 //============================================================
void SkinSprTable::allocObjects( int objectQnty )
 {
    m_table = new SkinSpr[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void SkinSprTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *SkinSprTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"SkinSprTable::getObjectPTR");
    return &(m_table[ index ]);
 }

/* End of file C:\NW\ARENA\OBASE\SkinSpr\SkinSpr.cpp */
