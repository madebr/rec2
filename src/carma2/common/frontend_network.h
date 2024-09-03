#ifndef FRONTEND_NETWORK_H
#define FRONTEND_NETWORK_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_NETWORK);
C2_HOOK_VARIABLE_DECLARE(tNet_game_type, gFrontend_game_type);
C2_HOOK_VARIABLE_DECLARE(tNet_game_options, gFrontend_net_options);

void C2_HOOK_FASTCALL RefreshNetRacesScroller(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL UpdateNetTrackScroller(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL UpdateNetGameTypeScroller(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetGameTypeUp(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetSetRaceType(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetGameTypeDn(tFrontend_spec* pFrontend);

#endif /* FRONTEND_NETWORK_H */
