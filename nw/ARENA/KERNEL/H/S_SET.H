/*
    File : Suavik\E:\COM\S_set.h
    Autor: Suavik
    Ver    1.0
 */
#ifndef __S_SET_H__
#define __S_SET_H__

#define SUPPORT_TYPEID  0
#define s_DEBUG_EVENT   0
#define SUPPORT_bool    0
#define s_ENABLE_ASSERT 0

#if !SUPPORT_bool
typedef int bool;
#define true  1
#define false 0
#endif



#if  SUPPORT_TYPEID
#define s_DECLARE_INTROSPECTION     virtual type_info getType();
#else
#define s_DECLARE_INTROSPECTION     /* */
#endif


#endif
/*  End of file S_SET.H */
