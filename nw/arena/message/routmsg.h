/*
   File:   Suavik\d:\game\message\routmsg.h
   Autor:  Suavik
   Ver     1.0
   
   Сообщения для класса Route
 */
#ifndef __ROUTMSG_H__
#define __ROUTMSG_H__

#ifndef __A_MSG_H__
#include "message/a_msg.h"
#endif


enum
 {
    ROUTE_LOAD   = ROUT_MESSAGE,
   //{{EVENT
    ro_EV_GET_NODE_CNT,
	ro_EV_GET_NODE,
	ro_EV_GET_LENGHT,
	ro_EV_GET_POS,
   //}}END_OF_EVENT	
 };

#endif
/* End of file ROUTMSG.H */

