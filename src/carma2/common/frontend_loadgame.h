#ifndef FRONTEND_LOADGAME_H
#define FRONTEND_LOADGAME_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_LOAD_GAME);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_count_saved_games);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_load_game_index_top);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_mouse_down);
C2_HOOK_VARIABLE_DECLARE(int, gPrev_frontend_mouse_down);


int C2_HOOK_FASTCALL LoadGameInFunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL LoadGameOutFunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL LoadGameUpdateFunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL TryToLoadGame(int pN);

int C2_HOOK_FASTCALL LoadSlot1(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL LoadSlot2(tFrontend_spec* pFrontend);

#endif /* FRONTEND_LOADGAME_H */
