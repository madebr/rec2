#ifndef FRONTEND_NETWORK_H
#define FRONTEND_NETWORK_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_NETWORK);
C2_HOOK_VARIABLE_DECLARE(tNet_game_type, gFrontend_game_type);
C2_HOOK_VARIABLE_DECLARE(tNet_game_options, gFrontend_net_options);
C2_HOOK_VARIABLE_DECLARE(tU32, gFrontend_net_current_roll);
C2_HOOK_VARIABLE_DECLARE(tU32, gFrontend_net_last_roll);
C2_HOOK_VARIABLE_DECLARE(tNet_mode, gFrontend_net_mode);
C2_HOOK_VARIABLE_DECLARE(tNet_game_details*, gGame_to_join);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_net_car_index);
C2_HOOK_VARIABLE_DECLARE(int, gNet_join_host_result);
C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gFrontend_host_join_buffer, 256);

void C2_HOOK_FASTCALL RefreshNetRacesScroller(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL UpdateNetTrackScroller(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL UpdateNetGameTypeScroller(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetGameTypeUp(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetSetRaceType(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetGameTypeDn(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetRaceUp(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetHostChooseThisRace(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetRaceDn(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetCancel(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL DoMultiplayerStartStuff(tNet_mode pNet_mode);

int C2_HOOK_FASTCALL NetworkJoinGoAhead(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetworkStartHost(tFrontend_spec* pFrontend);

#endif /* FRONTEND_NETWORK_H */
