#ifndef REC2_INPUT_H
#define REC2_INPUT_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, GEdge_trigger_mode);

int C2_HOOK_FASTCALL LoadJoystickPreferences(void);

int C2_HOOK_FASTCALL AnyKeyDown(void);

void C2_HOOK_FASTCALL WaitForNoKeys(void);

void C2_HOOK_FASTCALL EdgeTriggerModeOn(void);

void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);

int C2_HOOK_FASTCALL PDKeyDown2(int pKey_index);

int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);

void C2_HOOK_FASTCALL PollKeys(void);

tU32* C2_HOOK_FASTCALL KevKeyService(void);

#endif // REC2_INPUT_H
