/*
   File:   Suavik\d:\game\message\bombmsg.h
   Autor:  Suavik
   Ver     1.0

   Описание типов для тэгов в данных KR_Event.
   Описание сообщений для класса Bomb.
 */
#ifndef __BOMBMSG_H__
#define __BOMBMSG_H__


#ifndef __A_MSG_H__
#include "message/a_msg.h"
#endif

enum
 {
    TAG_CREATE_BOMB_DATA,
    TAG_EXPLOSION_STYLE,
    TAG_BOMB_ORIENT
 };

enum
 {
    BOMB_MSG_CREATE = BOMB_MESSAGE,
    BOMB_MSG_DRIVE,
    BOMB_MSG_BOOM,
 };


#endif

/* End of file BOMBMSG.H */