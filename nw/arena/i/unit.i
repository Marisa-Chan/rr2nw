#ifndef __UNIT_I__INCLUDED
#define __UNIT_I__INCLUDED

enum
{
  IUnitIID = 2
};

class KR_ObjectID;
class CFVector3;

class IUnit
{
public:
    virtual double getPower   ()  = 0; // Сила юнита 0..10
    virtual int    isFriend   ( const KR_ObjectID &commanderID ) = 0;
    virtual double getDamage  ()  =0; // Целостность от 0..1
    virtual void   setDamage  ( double d, const CFVector3 &pos, double ts,
                                    KR_ObjectID fromID ) = 0;
    virtual double desireShoot() = 0;
    virtual KR_ObjectID getCommander() = 0;
    virtual void        setCommander(KR_ObjectID oID) = 0;
};

#endif
