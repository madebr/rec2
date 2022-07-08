#include "c2_stdlib.h"

void (C2_HOOK_CDECL * srand_original)(int seed);
void C2_HOOK_CDECL c2_srand(int seed) {
    srand_original(seed);
}
C2_HOOK_FUNCTION_ORIGINAL(0x00576960, c2_srand, srand_original)
