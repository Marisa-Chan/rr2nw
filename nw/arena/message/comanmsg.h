/*
   File:  Suavik\d:\game\message\comanmsg.h
   Autor: Suavik
   Ver    1.0
   
   Файл описания сообщений для класса Commander
 */
#ifndef __COMANMSG_H__
#define __COMANMSG_H__


#ifndef __A_MSG_H__
#include "message/a_msg.h"
#endif

enum 
 {
    CT_VECTOR
 };

enum 
 {
    COMMANDER_ADD_MEMBER_N = COMMANDER_MESSAGE,
    com_EV_GROUP_AREA,
    com_EV_GROUP_REACHED,
    com_EV_GROUP_TARGET_DESTROYED,
    com_EV_GROUP_YOUR_MY_MEMBER,
    com_EV_SET_ROUTE,
    com_EV_PLANE0,
    com_EV_PLANE1,
    com_EV_SETHOSTILECOMMANDER,
    com_EV_SETFRIENDLYCOMMANDER,
    com_EV_I_AM_DEAD,
 };

#endif

/* End of file COMANMSG.H */




