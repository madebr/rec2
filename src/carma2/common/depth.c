#include "depth.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gSky_on, 0x00591188, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gDepth_cueing_on, 0x0059118c, 1);

int C2_HOOK_FASTCALL GetSkyTextureOn(void) {

    return C2V(gSky_on);
}
C2_HOOK_FUNCTION(0x00446e00, GetSkyTextureOn)

void (C2_HOOK_FASTCALL * SetSkyTextureOn_original)(int skyTextureOn);
void C2_HOOK_FASTCALL SetSkyTextureOn(int skyTextureOn) {
#if defined(C2_HOOKS_ENABLED)
    SetSkyTextureOn_original(skyTextureOn);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00446e10, SetSkyTextureOn, SetSkyTextureOn_original)

int C2_HOOK_FASTCALL GetDepthCueingOn(void) {

    return C2V(gDepth_cueing_on);
}
C2_HOOK_FUNCTION(0x00446f90, GetDepthCueingOn)

void (C2_HOOK_FASTCALL * SetDepthCueingOn_original)(int pOn);
void C2_HOOK_FASTCALL SetDepthCueingOn(int pOn) {

#if defined(C2_HOOKS_ENABLED)
    SetDepthCueingOn_original(pOn);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00446fa0, SetDepthCueingOn, SetDepthCueingOn_original)

void (C2_HOOK_FASTCALL * ToggleDepthCueingQuietly_original)(void);
void C2_HOOK_FASTCALL ToggleDepthCueingQuietly(void) {

#if defined(C2_HOOKS_ENABLED)
    ToggleDepthCueingQuietly_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00447070, ToggleDepthCueingQuietly, ToggleDepthCueingQuietly_original)

void (C2_HOOK_FASTCALL * ToggleDepthCueing_original)(void);
void C2_HOOK_FASTCALL ToggleDepthCueing(void) {

#if defined(C2_HOOKS_ENABLED)
    ToggleDepthCueing_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00447110, ToggleDepthCueing, ToggleDepthCueing_original)
