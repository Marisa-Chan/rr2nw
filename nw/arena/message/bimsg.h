#ifndef __BIMSG_H__INCLUDED
#define __BIMSG_H__INCLUDED

#ifndef __A_MSG_H__
#include "message/a_msg.h"
#endif

enum
 {
    bi_EV_START_BIRD_MESSAGES = BIRD_MESSAGE,
    //{{EVENT
	bi_EVC_NEWPOS,
	bi_EVC_MOVE,
	bi_EV_BEGIN_MOVE,
    //}}END_OF_EVENT
 };

#endif

/* End of file bimsg.h */
