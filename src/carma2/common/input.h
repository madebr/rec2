#ifndef REC2_INPUT_H
#define REC2_INPUT_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gKey_mapping, 77);
C2_HOOK_VARIABLE_DECLARE(int, GEdge_trigger_mode);
C2_HOOK_VARIABLE_DECLARE(tMouse_coord, gCurrent_mouse_position);

int C2_HOOK_FASTCALL LoadJoystickPreferences(void);

int C2_HOOK_FASTCALL AnyKeyDown(void);

void C2_HOOK_FASTCALL WaitForNoKeys(void);

void C2_HOOK_FASTCALL EdgeTriggerModeOn(void);

void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);

int C2_HOOK_FASTCALL PDKeyDown2(int pKey_index);

int C2_HOOK_FASTCALL EitherMouseButtonDown(void);

int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);

void C2_HOOK_FASTCALL PollKeys(void);

tU32* C2_HOOK_FASTCALL KevKeyService(void);

void C2_HOOK_FASTCALL CyclePollKeys(void);

int C2_HOOK_FASTCALL KeyIsDown(int pKey_index);

int C2_HOOK_FASTCALL KeyIsDown2(int pKey_index);

void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY);

#endif // REC2_INPUT_H
