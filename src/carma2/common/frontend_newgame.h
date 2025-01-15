#ifndef FRONTEND_NEWGAME_H
#define FRONTEND_NEWGAME_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_NEWGAME);


int C2_HOOK_FASTCALL NewGameInfunc(tFrontend_spec* pFrontend);

#endif /* FRONTEND_NEWGAME_H */
