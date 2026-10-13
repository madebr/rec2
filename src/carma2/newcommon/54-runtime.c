#include "54-runtime.h"

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x006ba640
tS8 gRace_memory[1000];

// FUNCTION: CARMA2_HW 0x004e7440
int C2_HOOK_FASTCALL GetRuntimeVariable(int pAddress) {
    if (pAddress >= 0 && pAddress < 1000) {
        return gRace_memory[pAddress];
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e7460
int C2_HOOK_FASTCALL AccumulateRuntimeVariable(int pAddress, int pAmount) {
    int value;

    if (pAddress < 0 || pAddress >= 1000) {
        return 0;
    }

    /* Counters wrap within the signed byte range. */
    value = gRace_memory[pAddress] + pAmount % 256;
    if (value > 127) {
        value -= 256;
    } else if (value < -128) {
        value += 256;
    }
    gRace_memory[pAddress] = (tS8)value;
    return value;
}

// FUNCTION: CARMA2_HW 0x004e7480
int C2_HOOK_FASTCALL ToggleRuntimeVariable(int pAddress) {
    int value;

    if (pAddress < 0 || pAddress >= 1000) {
        return 0;
    }

    value = !gRace_memory[pAddress];
    gRace_memory[pAddress] = (tS8)value;
    return value;
}

// FUNCTION: CARMA2_HW 0x004e74b0
void C2_HOOK_FASTCALL ClearAllRuntimeVariables(void) {
    memset(gRace_memory, 0, sizeof(gRace_memory));
}

