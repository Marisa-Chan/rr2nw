/*
   File:  Suavik\d:\game\message\strgmsg.h
   Autor: Suavik
   Ver    1.0
   
   Сообщения для класса ct_Storage
 */
#ifndef __STRGMSG_H__
#define __STRGMSG_H__

#ifndef __A_MSG_H__
#include "message/a_msg.h"
#endif

enum
 {
    STORAGE_NEW_OBJECT   = STORAGE_MESSAGE,
    STORAGE_NEW_OBJECT_N,
    STORAGE_DEL_OBJECT,
    STORAGE_DEL_OBJECT_AFTER_TIME
 };

#endif
/* End of file STRGMSG.H */

