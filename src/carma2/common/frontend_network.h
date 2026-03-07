#ifndef FRONTEND_NETWORK_H
#define FRONTEND_NETWORK_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern tFrontend_spec gFrontend_NETWORK;
extern tNet_game_type gFrontend_game_type;
extern tNet_game_options gFrontend_net_options;
extern tU32 gFrontend_net_current_roll;
extern tU32 gFrontend_net_last_roll;
extern tNet_mode gFrontend_net_mode;
extern tNet_game_details* gGame_to_join;
extern int gFrontend_net_car_index;
extern int gNet_join_host_result;
extern char gFrontend_host_join_buffer[256];

void C2_HOOK_FASTCALL SaveSinglePlayerState(void);

void C2_HOOK_FASTCALL NetworkJoinSetup(void);

int C2_HOOK_FASTCALL NetworkJoinMenuInfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetworkJoinMenuOutfunc(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL NetworkJoinUpdateScroller(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL FRONTEND_NetworkJoinMenuHandler(tFrontend_spec* pFrontend);

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

void C2_HOOK_FASTCALL NetworkUpdateSelectedGameInfo(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetJoinChooseThisGame(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetGameToggleTyping(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetworkStartJoin(tFrontend_spec* pFrontend);

#endif /* FRONTEND_NETWORK_H */