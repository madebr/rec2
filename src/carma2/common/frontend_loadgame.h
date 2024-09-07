#ifndef FRONTEND_LOADGAME_H
#define FRONTEND_LOADGAME_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_LOAD_GAME);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_count_saved_games);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_load_game_index_top);


int C2_HOOK_FASTCALL LoadGameInFunc(tFrontend_spec* pFrontend);

#endif /* FRONTEND_LOADGAME_H */
