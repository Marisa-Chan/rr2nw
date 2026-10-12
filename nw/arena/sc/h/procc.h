#ifndef __PROCC_H__
#define __PROCC_H__

#ifndef __SCTYPES_H__
#include "outstrm.h"
#endif

typedef union {
    TInt   i;
    TFloat f;
} TStackCell;

typedef struct {
    TStackCell   *m_stack;
    int           m_stackSize;
    int           m_quants;
    TByte        *m_codePtr;
    TInt          SP;
    TInt          BP;
    TInt          IP;
} TProcessContext;

#endif
/* End of PROCC.H */


