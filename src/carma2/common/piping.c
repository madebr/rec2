#include "piping.h"

#include <brender/brender.h>

C2_HOOK_VARIABLE_IMPLEMENT(undefined4*, gCrush_space, 0x00694104);

void (C2_HOOK_FASTCALL * DisposePiping_original)(void);
void C2_HOOK_FASTCALL DisposePiping(void) {

#if defined(C2_HOOKS_ENABLED)
    DisposePiping_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004024a0, DisposePiping, DisposePiping_original)

void C2_HOOK_FASTCALL DisposeReplay(void) {

    DisposePiping();
    if (C2V(gCrush_space) != NULL) {
        BrMemFree(C2V(gCrush_space));
        C2V(gCrush_space) = NULL;
    }
}
C2_HOOK_FUNCTION(0x004c6c60, DisposeReplay)
