#include "racemem.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tU8, gRace_memory, 1000, 0x006ba640);

void C2_HOOK_FASTCALL ClearAllRuntimeVariables(void) {
    C2_HOOK_BUG_ON(sizeof(C2V(gRace_memory)) != 1000);
    c2_memset(C2V(gRace_memory), 0, sizeof(C2V(gRace_memory)));
}
C2_HOOK_FUNCTION(0x004e74b0, ClearAllRuntimeVariables)
