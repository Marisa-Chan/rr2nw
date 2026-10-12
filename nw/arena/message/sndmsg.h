#ifndef __SNDMSG_H__INCLUDED
#define __SNDMSG_H__INCLUDED

#ifndef __A_MSG_H__
#include "message/a_msg.h"
#endif

enum
 {
    snd_EV_FIRST = SOUND_MESSAGE,
    snd_EV_MOVE_TO,
    snd_EV_SET_WAV,
    snd_EV_START,
    snd_EV_END,
 };

#endif
