#ifndef GUARD_54_RUNTIME_H
#define GUARD_54_RUNTIME_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tS8 gRace_memory[1000];

int C2_HOOK_FASTCALL GetRuntimeVariable(int pAddress);

int C2_HOOK_FASTCALL AccumulateRuntimeVariable(int pAddress, int pAmount);

int C2_HOOK_FASTCALL ToggleRuntimeVariable(int pAddress);

extern void C2_HOOK_FASTCALL ClearAllRuntimeVariables(void);

#endif // GUARD_54_RUNTIME_H
