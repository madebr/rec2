#include "piping.h"

void (C2_HOOK_FASTCALL * DisposePiping_original)(void);
void C2_HOOK_FASTCALL DisposePiping(void) {

#if defined(C2_HOOKS_ENABLED)
    DisposePiping_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004024a0, DisposePiping, DisposePiping_original)
