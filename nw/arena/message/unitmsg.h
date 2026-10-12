/*
   File:   Suavik\d:\game\message\unitmsg.h
   Autor:  Suavik
   Ver     1.0
 */

#ifndef __UNITMSG_H__
#define __UNITMSG_H__

#ifndef __A_MSG_H__
#include "message/a_msg.h"
#endif


enum
 {
    /*
     * Команды, передаваемые объекту от командира группы
     */
    UNIT_GO_TO =    // двигаться в точку {CT_VECTOR,0,TAG_SIMPLE}(x,y,z)
                 UNIT_MESSAGE,

    UNIT_YOR_IN_GROUP,
    UNIT_GO,        // двигаться со скоростью X(0..1), в направлении A
    UNIT_ATTENTION, // обороняться от непосредственной угрозы
    UNIT_STY,       // остановиться и топтаться на месте
    UNIT_FREEZE,    // замереть
    UNIT_ATTACK,    // атаковать объект
    UNIT_LOOK_TO,   // повернуться в указанном напрвлении

    UNIT_I_AM_ATTACKED,
    /*
     * Внутренние сообщения юнита самому себе
     */
    UNIT_I_DIST_OBJECT,
    UNIT_I_ASC_DOMAGE,
    UNIT_I_DIST_GROUP,
    UNIT_I_IS_OBJECT_EXIST,
    t_EVC_MOVING,
    t_EV_CREATE_SMOKE,

    t_EVCMD_SET_POSITION, // double, double, double
    t_EV_ONCOLLISION,
    t_EV_SET_ATTR_POS,

//{{EVENT
    t_EV_MOVE,
    t_EV_STOP,
    t_EV_ATTACK,
    t_EV_TARGET_LOCKED,
    t_EV_YOU_ATTACKED,
    t_EV_TARGET_LOST,
    t_EV_RETREATING,
    t_EV_RETURN,
    t_EV_REACHED,
    t_EVC_STAY_AND_SHOOTING,
    t_EVC_MOVING_AND_SHOOTING,
    t_EVC_RETREATING,
    t_EVC_RETURN,
    UNIT_I_DRIVE,
    t_EVC_CHECK_ROTATE,
//}}END_OF_EVENT
 };


#endif

/* End of file UNITMSG.H */
