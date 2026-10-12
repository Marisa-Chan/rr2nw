#ifndef __BULMSG_H__INCLUDED
#define __BULMSG_H__INCLUDED

#ifndef __A_MSG_H__
#include "message/a_msg.h"
#endif

enum
 {
    b_EV_START_BULLET_MESSAGES = BULLET_MESSAGE,
    //{{EVENT
    b_EVC_MOVING,
    b_EVC_CHECK_COLLISION,
    b_EV_START,
    //}}END_OF_EVENT
 };

#endif

/* End of file Bulmsg.h */
