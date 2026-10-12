/*
  Friend units messages
 */

#ifndef __FUNITMSG_H__INCLUDED
#define __FUNITMSG_H__INCLUDED

#include "message\a_msg.h"

/*
     fu_EV_QUERY_DAMAGE,   // double
     fu_EV_QUERY_COORD,    // (VECTOR_LAND,0){ double x, double z }
     fu_EV_QUERY_COMMANDER,// KR_ObjectID
     fu_EV_QUERY_POWER,    // double
     fu_EV_QUERY_ISFRIEND, // [in] KR_ObjectID [out] bool
     fu_EV_QUERY_POSITION, // (VECTOR3D_f,0){double x,double y,double z}
 */
enum
 {
     fu_EV_FIRST_MESSAGE = FRIEND_UNIT_MESSAGE,
     fu_EV_QUERY_ANGLE,
     fu_EV_QUERY_ANGLE_OK,
//{{EVENT
    fu_EV_QUERY_DAMAGE,
    fu_EV_QUERY_COORD,
    fu_EV_QUERY_COMMANDER,
    fu_EV_QUERY_POWER,
    fu_EV_QUERY_ISFRIEND,
    fu_EV_QUERY_POSITION,
    fu_EV_QUERY_SPEED,
    fu_EV_QUERY_DAMAGE_OK,
    fu_EV_QUERY_COORD_OK,
    fu_EV_QUERY_COMMANDER_OK,
    fu_EV_QUERY_POWER_OK,
    fu_EV_QUERY_ISFRIEND_OK,
    fu_EV_QUERY_POSITION_OK,
    fu_EV_QUERY_SPEED_OK,
    fu_EV_QUERY_POSDIRSPEED,
    fu_EV_QUERY_POSDIRSPEED_OK,
//}}END_OF_EVENT
 };

#endif
