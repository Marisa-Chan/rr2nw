#ifndef __CNMSG_H__INCLUDED
#define __CNMSG_H__INCLUDED

#include "message/a_msg.h"

enum // Tags
 {
     cn_ROTATE = 10,
     cn_SHOOT
 };

enum
 {
    cn_EV_START_MESSAGE = CANNON_MESSAGE,
    cn_EV_I_HAVE_SHOOT,
    //{{EVENT
    cn_EV_SHOOT,
    cn_EV_IDLEOK,
    cn_EV_END_SHOOTING,
    cn_EV_SINGLE_SHOOT,
    cn_EVC_IDLE,
    cn_EV_ENDOFSHOOT,
    cn_EVC_ROTATE,
    cn_EVCMD_ROTATE_AND_SHOOT,
    cn_EVCMD_DIRECT_SHOOT,
    //}}END_OF_EVENT
 };

#endif
