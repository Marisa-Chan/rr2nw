#ifndef __SKINMSG_H__INCLUDED
#define __SKINMSG_H__INCLUDED

#ifndef __A_MSG_H__
#include "message/a_msg.h"
#endif

enum
 {
    sk_EV_FIRST = SKIN_MESSAGE,
    sk_EV_LOAD,
    sk_EV_PROG,
    sk_EV_QUERY_MODEL_PTR,
    sk_EV_QUERY_MODEL_PTR_OK,
 };

#endif
