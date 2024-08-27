#ifndef FRONTEND_NETWORK_H
#define FRONTEND_NETWORK_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_NETWORK);

void C2_HOOK_FASTCALL RefreshNetRacesScroller(tFrontend_spec* pFrontend);

#endif /* FRONTEND_NETWORK_H */
