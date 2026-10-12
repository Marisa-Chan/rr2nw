#ifndef __SPARKMSG_H__INCLUDED
#define __SPARKMSG_H__INCLUDED

#ifndef __A_MSG_H__
#include "message/a_msg.h"
#endif

enum
 {
    sp_EV_START_MESSAGE = SPARK_MESSAGE,
    //{{EVENT
    sp_EVC_LIFE,
    sp_EV_CREATE,
    sp_EV_SET_PHASE_COUNT,
    sp_EV_SET_PHASE,
    //}}END_OF_EVENT
 };

#endif
/* End of file Sparkmsg.h */
