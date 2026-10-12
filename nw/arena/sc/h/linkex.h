/*
 *  Типы информации линкеру и
 *  объявление массивов с линкерной информацией о
 *  внешних функциях.
 */
#ifndef __LINKEX_H__
#define __LINKEX_H__

#ifndef __PROCC_H__
#include "procc.h"
#endif

#define SC_PARI(num) (pc->m_stack[(int)(pc->SP-num-1)].i)
#define SC_PARF(num) (pc->m_stack[(int)(pc->SP-num-1)].f)
#define SC_PARS(num) (char *)&(pc->m_codePtr[SC_PARI(num)])

typedef void (*TFuncExtern)(TProcessContext *pc, void *arena);
typedef void (*TCalcConstExtern)( TStackCell *cell );

typedef struct {
    const char       *m_name;
    TFuncExtern       m_func;
    const char       *m_parm;
} TLinkExtern;

typedef struct {
    const char       *m_name;
    TCalcConstExtern  m_func;
    int               m_offset;
} TLinkConstExtern;

extern TLinkExtern      externLinkTable[];
extern TLinkConstExtern constExternLinkTable[];

#endif
/*  End of LINKEX.H */


