#ifndef __PEOPMSG_H__INCLUDED
#define __PEOPMSG_H__INCLUDED

#include "a_msg.h"

enum
 {
    p_EV_PEOPLE_FIRST_MESSAGE = PEOPLE_MESSAGE,
    //{{EVENT
    pe_EVCMD_START,
    pe_EVCMD_STOP,
    pe_EVC_MOVE,
    //}}END_OF_EVENT
    pe_EVC_NEXTNODE,
    pe_EV_SETANIM,
    pe_EVC_FIND_ENEMY,
    pe_EVC_NEXTRNDNODE,
    pe_EV_STARTSHOW,
    pe_EV_SETAUTOANIM,
 };

#endif
