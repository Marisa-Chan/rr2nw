#ifndef __COMMANDER_I__INCLUDED
#define __COMMANDER_I__INCLUDED

enum
{
    ICommanderIID = 18
};

class ICommander
{

public:

    virtual void       setFriendly(KR_ObjectID &) = 0;
    virtual void       setHostile (KR_ObjectID &) = 0;

    virtual int       isFriendly(KR_ObjectID &) = 0;
    virtual int       isHostile (KR_ObjectID &) = 0;
};

#endif

