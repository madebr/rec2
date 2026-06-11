#ifndef GUARD_42_INPUT_H
#define GUARD_42_INPUT_H

#include "c2_hooks.h"

extern int gKey_mapping[77];

// SetJoystickArrays

// PollKeys

// CyclePollKeys

// ResetPollKeys

// CheckKeysForMouldiness

// EitherMouseButtonDown

// AnyModifiersDown

// EdgeTriggeryKey

extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);

// PDKeyDown3

// PDAnyKeyDown

extern int C2_HOOK_FASTCALL AnyKeyDown(void);

// KevKeyService

extern int C2_HOOK_FASTCALL KeyIsDown(int pKey_index);

// KeyIsDownNoMouldiness

// WaitForNoKeys

// GetMousePosition

// InitRollingLetters

// EndRollingLetters

// AddRollingLetter

// ChangeCharTo

// RevertTyping

// EdgeTriggerModeOn

// EdgeTriggerModeOff

#endif // GUARD_42_INPUT_H
