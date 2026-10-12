#if	0 && !defined(__HOOKALLO_H__)
#define	__HOOKALLO_H__

#include<malloc.h>

#ifdef	__cplusplus
	extern "C" {
#endif

	void *DbgAllocHook(unsigned l);
	void DbgFreeHook(void*p);

#ifdef	__cplusplus
	}
#endif

#ifdef	__cplusplus
inline void *operator new(unsigned l) { return DbgAllocHook(l); }
inline void *operator new[](unsigned l) { return DbgAllocHook(l); }
inline void operator delete(void*p) { DbgFreeHook(p); }
inline void operator delete[](void*p) { DbgFreeHook(p); }
inline void *DbgLibAlloc(unsigned l) { return malloc(l); }
inline void DbgLibFree(void*p) { free(p); }
void DbgSeekGarbage();
#endif

#define	malloc(X) DbgAllocHook(X)
#define	free(X) DbgFreeHook(X)

#endif

