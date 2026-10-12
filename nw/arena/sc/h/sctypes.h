#ifndef __SCTYPES_H__
#define __SCTYPES_H__

typedef long           TInt;
typedef double         TFloat;
typedef char          *TStr;
typedef unsigned char  TByte;
typedef TByte         *TBytePtr;

#define _TINT(x)          ((TInt)(x))
#define _TBYTE_TO_TINT(x) _TINT((unsigned long)(x))

#endif
/* End of SCTYPES.H */

