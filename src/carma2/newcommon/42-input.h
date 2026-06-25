#ifndef GUARD_42_INPUT_H
#define GUARD_42_INPUT_H

#include "c2_hooks.h"

extern int gKey_mapping[77];

// SetJoystickArrays

extern void C2_HOOK_FASTCALL PollKeys(void);

// CyclePollKeys

// ResetPollKeys

// CheckKeysForMouldiness

extern int C2_HOOK_FASTCALL EitherMouseButtonDown(void);

// AnyModifiersDown

// EdgeTriggeryKey

extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);

// PDKeyDown3

extern int C2_HOOK_FASTCALL PDAnyKeyDown(void);

extern int C2_HOOK_FASTCALL AnyKeyDown(void);

// KevKeyService

extern int C2_HOOK_FASTCALL KeyIsDown(int pKey_index);

// KeyIsDownNoMouldiness

extern void C2_HOOK_FASTCALL WaitForNoKeys(void);

extern void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY);

extern void C2_HOOK_FASTCALL InitRollingLetters(void);

extern void C2_HOOK_FASTCALL EndRollingLetters(void);

// AddRollingLetter

// ChangeCharTo

extern void C2_HOOK_FASTCALL RevertTyping(int pSlot_index, char* pRevert_str);

extern void C2_HOOK_FASTCALL EdgeTriggerModeOn(void);

extern void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);

#endif // GUARD_42_INPUT_H
