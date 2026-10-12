#ifndef __FLYMSG_H__INCLUDED
#define __FLYMSG_H__INCLUDED

#ifndef __A_MSG_H__
#include "message/a_msg.h"
#endif

enum
 {
    fly_EVCMD_START_PATROL = FLY_MESSAGE,
    fly_EVC_FIND_ENEMY,
    fly_EV_ENEMY_FOUNDED,
    //{{EVENT
    //}}END_OF_EVENT
 };

#endif

/* End of file bimsg.h */

