#ifndef __INCUBMSG_H__
#define __INCUBMSG_H__

#ifndef __A_MSG_H__
#include "message/a_msg.h"
#endif

enum
{
    incub_CREATE_IS = INCUBATOR_MESSAGE,
    incub_CREATE_II,
    incub_SET_FRIEND,
    incub_ON_CREATE_OBJECT,
    incub_CREATE_IS_PEOPLE,
};

#endif
