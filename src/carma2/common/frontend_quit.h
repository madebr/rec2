#ifndef FRONTEND_QUIT_H
#define FRONTEND_QUIT_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_QUIT);

int C2_HOOK_FASTCALL FRONTEND_Quit_Create(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL FRONTEND_Quit_Yes(tFrontend_spec* pFrontend);

#endif /* FRONTEND_QUIT_H */
