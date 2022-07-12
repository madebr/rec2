#include "prepmap.h"

void (C2_HOOK_CDECL * BrMapUpdate_original)(br_pixelmap* map, br_uint_16 flags);
void C2_HOOK_CDECL BrMapUpdate(br_pixelmap* map, br_uint_16 flags) {
#if defined(C2_HOOKS_ENABLED)
    BrMapUpdate_original(map, flags);
#else
#error "Not implemented
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00523160, BrMapUpdate, BrMapUpdate_original)

void (C2_HOOK_STDCALL * BrBufferUpdate_original)(br_pixelmap* pm, br_token use, br_uint_16 flags);
void C2_HOOK_STDCALL BrBufferUpdate(br_pixelmap* pm, br_token use, br_uint_16 flags) {
#if defined(C2_HOOKS_ENABLED)
    BrBufferUpdate_original(pm, use, flags);
#else
#error "Not implemented
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005231f0, BrBufferUpdate, BrBufferUpdate_original)

void (C2_HOOK_STDCALL * BrBufferClear_original)(br_pixelmap* pm);
void C2_HOOK_STDCALL BrBufferClear(br_pixelmap* pm) {
#if defined(C2_HOOKS_ENABLED)
    BrBufferClear_original(pm);
#else
#error "Not implemented
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00523280, BrBufferClear, BrBufferClear_original)
