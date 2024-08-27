#ifndef FRONTEND_MAIN_H
#define FRONTEND_MAIN_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_MAIN);
C2_HOOK_VARIABLE_DECLARE(tU32, gLast_host_query);

int C2_HOOK_FASTCALL MainMenuInfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL MainMenuOutfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NewGameToggleTyping(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL FRONTEND_MainMenuHandler(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL OnePlayerSetup(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL MultiplayerSetup(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL testUp(tFrontend_spec *pFrontend);

#endif /* FRONTEND_MAIN_H */
