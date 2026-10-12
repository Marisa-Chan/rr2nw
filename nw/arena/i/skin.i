#ifndef __SKIN_I__INCLUDED
#define __SKIN_I__INCLUDED

enum
{
   ISkinIID = 6
};

class CViewBaseModifier;
class CViewBaseModifier0;
class CViewObjectRef;

class ISkin
{
 public:
    virtual void                load         ( const char *fileName )    = 0;
    virtual void                createAniSets( int aniCnt, int aniCnt0 ) = 0;
    virtual void                removeAniSets()                          = 0;
    virtual void                set ( int set, const char *name )        = 0;
    virtual void                set0( int set, const char *name )        = 0;
    virtual CViewBaseModifier  *get ( int set, int reduc )               = 0; 
    virtual CViewBaseModifier0 *get0( int set, int reduc )               = 0; 
    virtual void                skinSetAnimAuto(CViewObjectRef *ref)     = 0;
    virtual int                 isAutoAnim()                             = 0;
};

#endif
