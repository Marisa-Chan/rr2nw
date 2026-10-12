/*
 * File  : C:\NW\ARENA\OBASE\Sound\WAVObj.h
 * Autor :
 * Ver   1.0
 */
#ifndef __WAVOBJ_H__INCLUDED
#define __WAVOBJ_H__INCLUDED

#include "storage/h/subject.h"
#include "kernel/h/active.h"
#include "sound.h"




class WAVObj : public ct_Object
{
 public:

    bool m_loaded;


    RSXCACHEDEMITTERDESC m_rsxCE;
    RSXEMITTERMODEL      m_rsxEModel;


             WAVObj();
    virtual ~WAVObj();
    virtual int  receiveEvent( KR_Event &event );
    virtual void addNotify   ();
    virtual void removeNotify();

    void    load( const char *fname, double, double, double, double, double );

    virtual bool shouldDump () { return false; } // cannot be allocated dynamically
};

bool SetSoundAttr(	const KR_ObjectID	& selfID,	
			SimulationContext *context,
			char * soundName,
			ct_ClassTableID & ctsndID,
			void * wav);

void updateSound( const KR_ObjectID & selfID,
		  SimulationContext *context,
		  ct_ClassTableID & ctsndID,
		  WAVObj * wav,
		  KR_ObjectID & snd );

#endif // ifndef __WAVOBJ_H__INCLUDED
/* End of file C:\NW\ARENA\OBASE\Sound\WAVObj.h */