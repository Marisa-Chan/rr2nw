#ifndef __FOUNTMSG_H__INCLUDED
#define __FOUNTMSG_H__INCLUDED

#ifndef __A_MSG_H__
#include "message/a_msg.h"
#endif

enum
 {
    fou_EVCMD_START = FOUNTAIN_MESSAGE,
    fou_EVCMD_END,
    fou_EVC_MOVING,
    fou_EV_ADD_BRANCH,
    fou_EVCMD_START_WITHDIR,
    //{{EVENT
    //}}END_OF_EVENT
 };

#endif
