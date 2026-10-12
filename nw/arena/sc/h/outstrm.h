#ifndef __OUTSTRM_H__
#define __OUTSTRM_H__


#ifndef __SCTYPES_H__
#include "sctypes.h"
#endif

#define MAX_STREAM_NAME 100

typedef void (*os_TFunc)(void);

typedef struct {
    char    m_streamName[MAX_STREAM_NAME];
    int     m_size,
            m_base,
            m_pos;
    TByte  *m_buffer;
} os_TOutStream;

void     os_InitTOutStream  ( os_TOutStream *os, TByte *buf, int size, const char *name );
TByte  **os_ProvideForDeletingTOutStream( os_TOutStream *os );
void     os_ClearTOutStream ( os_TOutStream *os );
void     os_DropTOutStream  ( os_TOutStream *os );
void     os_ResetTOutStream ( os_TOutStream *os );
int      os_MarkOutStream   ( os_TOutStream *os );
void     os_ReleaseOutStream( os_TOutStream *os, int pos );

TInt     os_CurPos    ( os_TOutStream *os );
char    *os_CurStr    ( os_TOutStream *os );

void     os_PutByte   ( os_TOutStream *os, TInt val );
void     os_PutInt    ( os_TOutStream *os, TInt val );
void     os_PutFloat  ( os_TOutStream *os, TFloat val );
void     os_PutStr    ( os_TOutStream *os, const char * str );
void     os_PutPtr    ( os_TOutStream *os, TBytePtr val );

TByte    os_GetByte   ( os_TOutStream *os, int pos );
TInt     os_GetInt    ( os_TOutStream *os, int pos );
TInt     os_GetIntStep( os_TOutStream *os, TInt *pos );
TFloat   os_GetFloat  ( os_TOutStream *os, int pos );
TBytePtr os_GetPtr    ( os_TOutStream *os, int pos );
TStr     os_GetStr    ( os_TOutStream *os, TInt pos );

void     os_SetByte   ( os_TOutStream *os, int pos, TInt val );
void     os_SetInt    ( os_TOutStream *os, int pos, TInt val );
void     os_SetFloat  ( os_TOutStream *os, int pos, TFloat val );
void     os_SetPtr    ( os_TOutStream *os, int pos, TBytePtr val );
void     os_SetFunc   ( os_TOutStream *os, int pos, os_TFunc val );
#endif
/* End of OUTSTRM.H */

