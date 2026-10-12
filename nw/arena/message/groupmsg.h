/*
    File:  Suavik\D:\GAME\H\GROUPMSG.H
    Autor: Suavik
    Ver    1.0

    Форматы сообщений:

    Добавить члена группы
    GROUP_ADD_MEMBER
                Write(KR_ObjectID*,sizeof(KR_ObjectID))

    Добавить члена группы с указанным именем.
    Объект к моменту вызова должен быть создан
    GROUP_ADD_MEMBER
                WriteStr(objectName)

 */
#ifndef __GROUP_MSG__
#define __GROUP_MSG__

#ifndef __A_MSG_H__
#include "message/a_msg.h"
#endif

enum
 {
    GROUP_BEGIN_MESSAGE  = GROUP_MESSAGE,
    GROUP_ADD_MEMBER,
    GROUP_ADD_MEMBER_N,
    GROUP_DEL_MEMBER,
    GROUP_CHANGE_SKIN,
    GROUP_I_AM_DEAD,
    tg_EVCMD_SET_POSITION,
//{{EVENT
    tg_EVCMD_GO_TO,
    tg_EVC_FIND_ENEMY,
    tg_EVC_MOVING,
    tg_EV_TARGET_LOCKED,
    tg_EV_TARGET_LOST,
    tg_EV_REACHED,
    tg_EV_YOU_ATTACKED,
    tg_EV_TARGET_DESTROED,
    tg_EV_REBUILD,
//}}END_OF_EVENT
    tg_EV_LAST_EVENT
 };

#endif

/* End of file GROUPMSG.H */