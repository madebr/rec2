#ifndef FRONTEND_MAIN_H
#define FRONTEND_MAIN_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_MAIN);

int C2_HOOK_FASTCALL MainMenuInfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL MainMenuOutfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NewGameToggleTyping(tFrontend_spec* pFrontend);

#endif /* FRONTEND_MAIN_H */
