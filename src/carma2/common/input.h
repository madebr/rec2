#ifndef REC2_INPUT_H
#define REC2_INPUT_H

#include "c2_hooks.h"

#include "rec2_types.h"

int C2_HOOK_FASTCALL LoadJoystickPreferences(void);

int C2_HOOK_FASTCALL AnyKeyDown(void);

void C2_HOOK_FASTCALL WaitForNoKeys(void);

#endif // REC2_INPUT_H
