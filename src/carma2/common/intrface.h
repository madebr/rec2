#ifndef REC2_INTRFACE_H
#define REC2_INTRFACE_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, gAlways_typing);

int C2_HOOK_FASTCALL DoInterfaceScreen(const tInterface_spec* pSpec, int pOptions, int pCurrent_choice);

void C2_HOOK_FASTCALL LoadMenuSettings(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL ClearAlwaysTyping(void);

#endif //REC2_INTRFACE_H
