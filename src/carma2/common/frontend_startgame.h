#ifndef FRONTEND_STARTGAME_H
#define FRONTEND_STARTGAME_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_START_GAME);


int C2_HOOK_FASTCALL StartGameInfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL FRONTEND_StartGameMenuHandler(tFrontend_spec *pFrontend);

#endif /* FRONTEND_STARTGAME_H */
