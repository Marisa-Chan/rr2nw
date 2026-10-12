#include "message/unitmsg.h"
#include "message/recrcenmsg.h"
#include "message/artfmsg.h"

 //===========================================================================
void s_Const_GROUP_ADD_MEMBER( TStackCell *cell )
 {
     cell->i = GROUP_ADD_MEMBER;
 }

 //===========================================================================
void s_Const_GROUP_ADD_MEMBER_N( TStackCell *cell )
 {
     cell->i = GROUP_ADD_MEMBER_N;
 }

 //===========================================================================
void s_Const_COMMANDER_ADD_MEMBER_N( TStackCell *cell )
 {
     cell->i = COMMANDER_ADD_MEMBER_N;
 }

 //===========================================================================
void s_Const_ATTR_MSG_SET_CHAR( TStackCell *cell )
 {
    cell->i = ATTR_MSG_SET_CHAR;
 }
void s_Const_ATTR_MSG_SET_BYTE( TStackCell *cell )
 {
    cell->i = ATTR_MSG_SET_BYTE;
 }

void s_Const_ATTR_MSG_SET_SHORT( TStackCell *cell )
 {
    cell->i = ATTR_MSG_SET_SHORT;
 }
void s_Const_ATTR_MSG_SET_USHORT( TStackCell *cell )
 {
    cell->i = ATTR_MSG_SET_USHORT;
 }

void s_Const_ATTR_MSG_SET_INT( TStackCell *cell )
 {
    cell->i = ATTR_MSG_SET_INT;
 }
void s_Const_ATTR_MSG_SET_UINT( TStackCell *cell )
 {
    cell->i = ATTR_MSG_SET_UINT;
 }

void s_Const_ATTR_MSG_SET_LONG( TStackCell *cell )
 {
    cell->i = ATTR_MSG_SET_LONG;
 }
void s_Const_ATTR_MSG_SET_ULONG( TStackCell *cell )
 {
    cell->i = ATTR_MSG_SET_ULONG;
 }

void s_Const_ATTR_MSG_SET_FLOAT( TStackCell *cell )
 {
    cell->i = ATTR_MSG_SET_FLOAT;
 }
void s_Const_ATTR_MSG_SET_DOUBLE( TStackCell *cell )
 {
    cell->i = ATTR_MSG_SET_DOUBLE;
 }

void s_Const_ATTR_MSG_SET_STR( TStackCell *cell )
 {
    cell->i = ATTR_MSG_SET_STR;
 }

void s_Const_ATTR_MSG_SET_UNKNOWN( TStackCell *cell )
 {
    cell->i = ATTR_MSG_SET_UNKNOWN;
 }

void s_Const_sk_EV_LOAD( TStackCell *cell )
 {
    cell->i = sk_EV_LOAD;
 }

void s_Const_SP_EV_SET_PHASE_COUNT( TStackCell *cell )
 {
    cell->i = sp_EV_SET_PHASE_COUNT;
 }
void s_Const_SP_EV_SET_PHASE( TStackCell *cell )
 {
    cell->i = sp_EV_SET_PHASE;
 }

void s_Const_fou_EVCMD_START( TStackCell *cell )
 {
    cell->i = fou_EVCMD_START;
 }

void s_Const_sk_EV_PROG( TStackCell *cell )
 {
    cell->i = sk_EV_PROG;
 }

void s_Const_bi_EV_BEGIN_MOVE( TStackCell *cell )
 {
    cell->i = bi_EV_BEGIN_MOVE;
 }

void s_Const_START_FARTING( TStackCell *cell )
 {
    cell->i = START_FARTING;
 }

void s_Const_pe_EVCMD_START( TStackCell *cell )
 {
    cell->i = pe_EVCMD_START;
 }

void s_Const_t_EV_SET_ATTR_POS( TStackCell *cell )
 {
   cell->i = t_EV_SET_ATTR_POS;
 }

void s_Const_rc_SET_EJECT( TStackCell *cell )
 {
   cell->i = rc_SET_EJECT;
 }

void s_Const_com_EV_SET_ROUTE( TStackCell *cell )
 {
   cell->i = com_EV_SET_ROUTE;
 }

void s_Const_pe_EV_SETANIM( TStackCell *cell )
 {
   cell->i = pe_EV_SETANIM;
 }

void s_Const_ARTEFACT_MOVETO( TStackCell *cell )
 {
   cell->i = ARTEFACT_MOVETO;
 }
