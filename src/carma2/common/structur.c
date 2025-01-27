#include "structur.h"

#include <stdlib.h>
void (C2_HOOK_FASTCALL * DoProgram_original)(void);
void C2_HOOK_FASTCALL DoProgram() {
#if defined(C2_HOOKS_ENABLED)
    DoProgram_original();
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00503c50, DoProgram, DoProgram_original);
