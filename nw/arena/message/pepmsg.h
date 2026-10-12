#ifndef __BIMSG_H__INCLUDED
#define __BIMSG_H__INCLUDED

#ifndef __A_MSG_H__
#include "message/a_msg.h"
#endif

enum
 {
    pep_EV_START_PEOPLE_MESSAGES = PEOPLE_MESSAGE,
    //{{EVENT
	pep_EVC_NEWPOS,
	pep_EVC_MOVE,
	pep_EV_BEGIN_MOVE,
	pep_EV_YOU_ARE_KILLED,
	pep_EVC_NEW,			/**/
    //}}END_OF_EVENT
 };

#endif

/* End of file pepmsg.h */
