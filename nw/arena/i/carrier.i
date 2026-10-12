#ifndef __CARRIER_I__INCLUDED
#define __CARRIER_I__INCLUDED

#include "kernel/h/context.h"

#define ICarrierIID  15
#define IArtefactIID 16

class IArtefact;

class ICarrier
{
public:
    KR_ObjectID  m_artefactID;
    IArtefact   *m_artefact;

    ICarrier()
    {
        m_artefactID = KR_ObjectID::NUL();
        m_artefact   = 0;
    }

    void         carrierDropArtefact(double ts);
    void         carrierTakeArtefact(KR_ObjectID oID, IArtefact *artefact );
    void         carrierOnRemoveArtefact();
    void         carrierOnCollizion (KR_ObjectID oID);
    int          carrierReceiveEvent( KR_Event &event );
    void         carrierAddNotify   (SimulationContext*context,double ts);
    void         carrierRemoveNotify(SimulationContext*context,double ts);
    virtual void carrierLoadMatrix  (CFMatrix3x4 &m)    = 0;
    void         carrierOnMove      ();
    virtual bool	 dump(PIN_SaveFile &);
    virtual bool	 load(PIN_SaveFile &);
    virtual void	 loadNotify();

};

class IArtefact
{
public:
    KR_ObjectID   m_carrierID;
    ICarrier     *m_carrier;

    IArtefact()
    {
       m_carrierID = KR_ObjectID::NUL();
       m_carrier   = 0;
    }
    virtual  int   isAttached() const { return m_carrier!=0; }
    virtual  void  moveTo    ( CFMatrix3x4 &m )                         = 0;
    virtual  int   attachTo  ( KR_ObjectID masterID, ICarrier *master ) = 0;
    virtual  void  drop      ( CFMatrix3x4 &m, double ts )              = 0;
    virtual  void  artefactMove( const CFVector3 &dir ) = 0;

    virtual bool	 dump(PIN_SaveFile &);
    virtual bool	 load(PIN_SaveFile &);
    virtual void	 loadNotify();

};

#endif
