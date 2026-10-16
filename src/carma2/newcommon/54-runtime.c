#include "54-runtime.h"

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x006ba640
tS8 gRace_memory[1000];

// GetRuntimeVariable

// AccumulateRuntimeVariable

// ToggleRuntimeVariable

// FUNCTION: CARMA2_HW 0x004e74b0
void C2_HOOK_FASTCALL ClearAllRuntimeVariables(void) {
    memset(gRace_memory, 0, sizeof(gRace_memory));
}

