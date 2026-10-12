/*
 * File  : C:\NW\ARENA\OBASE\Sound\WAVOBJ.cpp
 * Autor :
 * Ver   1.0
 */
#include "WAVOBJ.h"
#include "kernel/h/context.h"
#include "kernel/h/echo.h"
#include "kernel/h/s_debug.h"
#include "message/skinmsg.h"
#include "message/sndmsg.h"

 //===========================================================================
class WAVObjTable : public ct_ClassTable
{
 private:
    WAVObj *m_table;
 public:
    WAVObjTable()
    {
        m_table = NULL;
        registerClass( "WAVObj" );
    }
    ~WAVObjTable()
    {
        delete [] m_table;
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

static WAVObjTable  __classTable;
 /*********************************
  *
  *   WAVObj implementation
  *
  *********************************/

 //============================================================
WAVObj::WAVObj()
 {
    m_loaded = false;
 }

 //============================================================
WAVObj::~WAVObj()
 {
 }

 //============================================================
int WAVObj::receiveEvent( KR_Event &event )
 {
    switch( event.label )
    {
    case KR_WAKE_UP:
            break;

    case sk_EV_LOAD:
            {
            s_ASSERT(!m_loaded,"WAVObj::receiveEvent():LOAD Duplicate load");
            char fname[100];
            double fMinFront,
            fMinBack ,
            fMaxFront,
            fMaxBack ,
            fIntensity;


            event.data.open(EDO_READ)
                        .getStr(fname,sizeof(fname))
                        .getDouble( fMinFront )
                        .getDouble( fMinBack  )
                        .getDouble( fMaxFront )
                        .getDouble( fMaxBack  )
                        .getDouble( fIntensity)
                      .close();
            load( fname ,   fMinFront,
                            fMinBack ,
                            fMaxFront,
                            fMaxBack ,
                            fIntensity);


            m_loaded = true;
            }
            break;

    case sk_EV_QUERY_MODEL_PTR:
            {
            s_ASSERT(m_loaded,"Skin::receiveEvent():QUERY_MODEL_PTR");

            event.label = sk_EV_QUERY_MODEL_PTR_OK;
            void *self = this;

            event.data.open(EDO_WRITE)
                        .put(&self,sizeof(void*))
                      .close();
            }
            break;

    default: return 0;
    }
    return 1;
 }

 //============================================================
void WAVObj::addNotify()
 {
    ct_Object::addNotify();
    // insert your code this
 }

 //============================================================
void WAVObj::removeNotify()
 {
    ct_Object::removeNotify();
    // insert your code this
 }

 /*************************************
  *
  *   WAVObjTable implementation
  *
  *************************************/

 //============================================================
void WAVObjTable::allocObjects( int objectQnty )
 {
    m_table = new WAVObj[ objectQnty ];

    if(  m_table == NULL  )
         m_maxObjectQnty = 0;
 }

 //============================================================
void WAVObjTable::freeObjects()
 {
    delete [] m_table;
    m_table = NULL;
    m_maxObjectQnty = 0;
 }

 //============================================================
ct_Object *WAVObjTable::getObjectPTR( int index )
 {
    s_ASSERT( index >= 0 && index <= m_maxObjectQnty ,"WAVObjTable::getObjectPTR");
    return &(m_table[ index ]);
 }


    /*************************************
     *
     *        WAV Object implementation
     *
     *************************************/

 //============================================================
void WAVObj::load( const char *fname,   double fMinFront,
                                        double fMinBack ,
                                        double fMaxFront,
                                        double fMaxBack ,
                                        double fIntensity )
 {
    ZeroMemory(&m_rsxCE, sizeof(RSXCACHEDEMITTERDESC));
    m_rsxCE.cbSize = sizeof(RSXCACHEDEMITTERDESC);
    m_rsxCE.dwFlags = RSXEMITTERDESC_NODOPPLER | RSXEMITTERDESC_NOREVERB | RSXEMITTERDESC_PREPROCESS | RSXEMITTERDESC_INMEMORY;

    if (!snd_true3d)
    {
	m_rsxCE.dwFlags |= RSXEMITTERDESC_NOSPATIALIZE;
    }

    //strcpy(m_rsxCE.szFilename,fname);
    sprintf(m_rsxCE.szFilename,"..\\SOUND\\%s",fname);

    m_rsxEModel.fMinFront    = fMinFront  ;
    m_rsxEModel.fMinBack     = fMinBack   ;
    m_rsxEModel.fMaxFront    = fMaxFront  ;
    m_rsxEModel.fMaxBack     = fMaxBack   ;
    m_rsxEModel.fIntensity   = fIntensity ;

    m_rsxEModel.cbSize = sizeof(RSXEMITTERMODEL);

 }


bool SetSoundAttr(	const KR_ObjectID & selfID,
			SimulationContext *context,
			char * soundName,
			ct_ClassTableID & ctsndID,
			void * wav)
{
    if (*soundName && lpRSX2Unk)
    {
	
	KR_ObjectID	wavID;	

        wavID     = context->searchObject(soundName);
        ctsndID   = g_arena.searchSeanceClassTable("SoundObj");

        if (!wavID.isNUL())
	{
	   KR_Event event;
           event.label       = sk_EV_QUERY_MODEL_PTR;
           event.destination = wavID;
           event.source      = selfID;
           event.timeStamp   = 0.1; //FIXME
           context->sendEventNow( event );
	   //context->addEvent( event );

	   event.data.open(EDO_READ)
              .get( wav, sizeof(void*))
           .close();
	   
	   return true;	
	}
	
    }

    return false;
}                   	


void updateSound( const KR_ObjectID & selfID,
		  SimulationContext *context,
		  ct_ClassTableID & ctsndID,
		  WAVObj * wav,
		  KR_ObjectID & snd )
{
    snd = KR_ObjectID::NUL();

    if ( wav)
    {
	snd = g_arena.newObject(ctsndID,"snd.snd");

    if(  snd.isNUL()  ) 
         return;

	KR_Event event;

	event.data.open(EDO_WRITE)
              .put( & wav,sizeof(void *))
         .close();
      
	event.label = snd_EV_SET_WAV;
    event.destination = snd;
	event.source      = selfID;
	event.timeStamp   = 0.1; // FIXME

    context->sendEventNow( event );
    //context->addEvent( event );
   }
}

/* End of file C:\NW\ARENA\OBASE\Sound\WAVOBJ.cpp */