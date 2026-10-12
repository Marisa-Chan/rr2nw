#ifndef __CACHESMOKE_H_INCLUDED
#define __CACHESMOKE_H_INCLUDED

class cs_CacheSmoke
{
public:
        char        fname[128];
        void       *hand;
};

extern cs_CacheSmoke g_cacheSmoke[10];
extern int           g_cacheSmokeCnt;
void *g_loadSmoke( const char *fileName, void * , void *a = NULL);

#endif
