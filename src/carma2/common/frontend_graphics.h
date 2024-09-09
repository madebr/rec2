#ifndef FRONTEND_GRAPHICS_H
#define FRONTEND_GRAPHICS_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_GRAPHICS);


void C2_HOOK_FASTCALL GetGraphicsSettingsData(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Graphics_Infunc(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL SetGraphicsSettingsData(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Graphics_Outfunc(tFrontend_spec* pFrontend);

#endif /* FRONTEND_GRAPHICS_H */
