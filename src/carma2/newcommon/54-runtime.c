#include "54-runtime.h"

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x006ba640
// GLOBAL: CARMA2_SW 0x006b1320
tS8 gRace_memory[1000];

// GetRuntimeVariable

// AccumulateRuntimeVariable

// ToggleRuntimeVariable

// FUNCTION: CARMA2_HW 0x004e74b0
// FUNCTION: CARMA2_SW 0x004e32d0
void C2_HOOK_FASTCALL ClearAllRuntimeVariables(void) {
    memset(gRace_memory, 0, sizeof(gRace_memory));
}

