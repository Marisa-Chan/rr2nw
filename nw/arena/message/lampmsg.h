#ifndef __SMOKERMSG_H__INCLUDED
#define __SMOKERMSG_H__INCLUDED

#include "message\a_msg.h"

enum
 {
     lmp_EV_START = LAMP_MESSAGE,
     lmp_EV_MOVE,
     lmp_EV_REMOVE,
     lmp_EV_SETENDPOS,
     lmp_EV_CHANGEPOS,
//{{EVENT
//}}END_OF_EVENT
 };

#endif
