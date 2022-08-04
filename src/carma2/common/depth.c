#include "depth.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gSky_on, 0x00591188, 1);

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
