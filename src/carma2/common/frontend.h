#ifndef REC2_FRONTEND_H
#define REC2_FRONTEND_H

#include "rec2_types.h"

#include "c2_hooks.h"

#include "rec2_types.h"

void C2_HOOK_FASTCALL LoadFrontendStrings(void);

void C2_HOOK_FASTCALL CreateMenuActors(void);

int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec);

void C2_HOOK_FASTCALL FRONTEND_Setup(tFrontendMenuType pType) ;

#endif //REC2_FRONTEND_H
