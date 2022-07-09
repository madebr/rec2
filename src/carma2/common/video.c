#include "video.h"

void (C2_HOOK_CDECL * VideoInit_original)(void);
void C2_HOOK_CDECL VideoInit(void) {
#if defined(C2_HOOKS_ENABLED)
    VideoInit_original();
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e1700, VideoInit, VideoInit_original)
