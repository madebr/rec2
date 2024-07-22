#include "video.h"

void (C2_HOOK_CDECL * InitQuickTimeStuff_original)(void);
void C2_HOOK_CDECL InitQuickTimeStuff(void) {
#if defined(C2_HOOKS_ENABLED)
    InitQuickTimeStuff_original();
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e1700, InitQuickTimeStuff, InitQuickTimeStuff_original)
