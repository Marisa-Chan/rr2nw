#ifndef __FIREMSG_H__INCLUDED
#define __FIREMSG_H__INCLUDED

#ifndef __A_MSG_H__
#include "message/a_msg.h"
#endif

enum
 {
    fi_EVCMD_START = FIRE_MESSAGE,
    fi_EVCMD_END,
    fi_EVC_MOVING,
    //{{EVENT
    //}}END_OF_EVENT
 };

#endif
