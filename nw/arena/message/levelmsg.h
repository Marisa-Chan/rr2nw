#ifndef __LEVELMSG_H__INCLUDED
#define __LEVELMSG_H__INCLUDED

#ifndef __A_MSG_H__
#include "message/a_msg.h"
#endif

#define lev_CHECK_COMPLITE_TIME 5

enum
 {
    lev_RELISE_PROJECT = LEVEL_MESSAGE,
    lev_CHECK_COMPLITE,
    lev_LOAD,
    lev_SAVE,
    lev_RESTART,
    lev_LOAD_SLOT0,
    lev_LOAD_SLOT1,
    lev_LOAD_SLOT2,
    lev_LOAD_SLOT3,
    lev_LOAD_SLOT4,
    lev_LOAD_SLOT5,
    lev_LOAD_SLOT6,
    lev_LOAD_SLOT7,
    lev_SAVE_SLOT0,
    lev_SAVE_SLOT1,
    lev_SAVE_SLOT2,
    lev_SAVE_SLOT3,
    lev_SAVE_SLOT4,
    lev_SAVE_SLOT5,
    lev_SAVE_SLOT6,
    lev_SAVE_SLOT7,
 };

#endif

/* End of file bimsg.h */

