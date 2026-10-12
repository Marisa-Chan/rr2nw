/*
 * File  : C:\NW\ARENA\OBASE\Sound\SoundObj.cpp
 * Autor :
 * Ver   1.0
 */
#include "SoundObj.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/unitmsg.h"
#include "message/sndmsg.h"



 //===========================================================================
class SoundObjTable : public ct_ClassTable
{
 private:
    SoundObj *m_table;
 public:
    SoundObjTable()
    {
        m_table = NULL;
        registerClass( "SoundObj" );
    }
    ~SoundObjTable()
    {
        delete [] m_table;
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static SoundObjTable  __classTable;
 /*********************************
  *
  *   SoundObj implementation
  *
  *********************************/

 //============================================================
SoundObj::SoundObj()
 {
    m_position      = CFVector3(0,0,0);
    m_emitterValid  = 0;
    m_lpCE	    = 0;
    /*m_positionValid = 0;*/
 }

 //============================================================
SoundObj::~SoundObj()
 {
/*    if (m_emitterValid)
	m_lpCE->Release();
    m_emitterValid = 0;	*/
 }

 //============================================================
int SoundObj::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case KR_WAKE_UP:
            break;

    case snd_EV_SET_WAV:
            {
	    if (!lpRSX2Unk)	
		return 1;
            event.data.open(EDO_READ)
                         .get(&m_wav,sizeof(void*))
                      .close();

            HRESULT hr = CoCreateInstance(
                CLSID_RSXCACHEDEMITTER,     // GUID for cachedemitter object
                NULL,
                CLSCTX_INPROC_SERVER,
                IID_IRSXCachedEmitter,
                (void ** )&m_lpCE);

            if ( FAILED(hr) )
                return 1;

            hr = m_lpCE->Initialize(&getWAV()->m_rsxCE, lpRSX2Unk);

            if ( FAILED(hr))
                return 1;

            hr = m_lpCE->SetModel(&getWAV()->m_rsxEModel);

            if ( FAILED(hr))
                return 1;


            m_emitterValid = 1;
            }
            break;

    case snd_EV_MOVE_TO:
            event.data.open(EDO_READ)
                   .getDouble(m_position.x)
                   .getDouble(m_position.y)
                   .getDouble(m_position.z)
                 .close();
            onChangePos();	    
            break;

    case snd_EV_START:	    	
            {	
            int count;
            event.data.open(EDO_READ)
                   .getInt(count)
                 .close();
            startPlay(count);
            }
            break;

    case snd_EV_END:
            endPlay();
            break;

    default: return 0;
    }
    return 1;
 }

 //============================================================
void SoundObj::addNotify()
 {
    ct_Object::addNotify();
    // insert your code this
    m_wav = 0;
 }

 //============================================================
void SoundObj::removeNotify()
 {
    ct_Object::removeNotify();

    if (m_emitterValid)
	m_lpCE->Release();

    // m_emitterValid = 0;

    // insert your code this
 }

 /*************************************
  *
  *   SoundObjTable implementation
  *
  *************************************/

 //============================================================
void SoundObjTable::allocObjects( int objectQnty )
 {
    m_table = new SoundObj[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void SoundObjTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *SoundObjTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"SoundObjTable::getObjectPTR");
    return &(m_table[ index ]);
 }



WAVObj *SoundObj::getWAV()
 {
    s_ASSERT(m_wav,"SoundObj::getWAV(): Unknown wave");
    return m_wav;
 }

CFVector3  SoundObj::getPosition()
 {
    return m_position;
 }

void SoundObj::onChangePos()
 {
    if (!m_emitterValid)
        return;

//    m_positionValid = 1;	

    RSXVECTOR3D v3d;
    CFVector3 pos = getPosition();

    v3d.x = pos.x;
    v3d.y = pos.y;
    v3d.z = pos.z;

    m_lpCE->SetPosition(&v3d);
 }

void SoundObj::startPlay( int count )
 {
    if (m_emitterValid)		
    	m_lpCE->ControlMedia(RSX_PLAY, count, 0);
 }

void SoundObj::endPlay()
 {  
    if (m_emitterValid)	
    	m_lpCE->ControlMedia(RSX_STOP, 0, 0);
 }


/* End of file C:\NW\ARENA\OBASE\Sound\SoundObj.cpp */