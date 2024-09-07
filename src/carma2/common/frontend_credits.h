#ifndef FRONTEND_CREDITS_H
#define FRONTEND_CREDITS_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_CREDITS);


int C2_HOOK_FASTCALL CreditsScreenInfunc(tFrontend_spec* pFrontend);

#endif /* FRONTEND_CREDITS_H */
