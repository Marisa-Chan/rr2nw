#ifndef __KRTYPES_H__
#define __KRTYPES_H__

typedef double        KR_TimeDelta;
typedef int           KR_EventLabel;
typedef int           KR_EventID;
typedef unsigned char KR_byte;
typedef int           KR_EventTagID;
typedef double        timeCoff;

class KR_ObjectID
 {
 friend class SimulationContext;
 friend class Publisher;
 protected:
  int  cachePos;
 public:
  long id;
  KR_ObjectID()                      {}
  KR_ObjectID( long _id, int _cp )   {    id = _id;  cachePos = _cp; }
  KR_ObjectID( const KR_ObjectID &o ){    *this = o; }
  int  getCachePos() const { return cachePos; }
  void Init( long _id, int _cp )     {    id = _id;  cachePos = _cp;  }
  static KR_ObjectID NUL()          { return KR_ObjectID(-1,-1); }
  int                isNUL()        { return cachePos == -1; }

  int operator ==(const KR_ObjectID &o ) const{ return id==o.id; }
  int operator !=(const KR_ObjectID &o ) const{ return id!=o.id; }
 };

#define ID_IS_NULL(x) ((x).id==-1)

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

#endif
