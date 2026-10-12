#ifndef __ARTFMSG_H__INCLUDED
#define __ARTFMSG_H__INCLUDED

#ifndef __A_MSG_H__
#include "message/a_msg.h"
#endif

enum
 {
    ARTEFACT_ATTACH = ARTEFACT_MESSAGE,
    ARTEFACT_MOVE,
    ARTEFACT_CHANGEDIR,
    ARTEFACT_MOVETO,
    //{{EVENT
    //}}END_OF_EVENT
 };

#endif

/* End of file bimsg.h */
