#include "network.h"

#include "loading.h"

#include "brender/brender.h"

C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gDigits_pix, 0x0075b8f8);

void (C2_HOOK_FASTCALL * NetPlayerStatusChanged_original)(tPlayer_status pNew_status);
void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status) {
#if defined(C2_HOOKS_ENABLED)
NetPlayerStatusChanged_original(pNew_status);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004a57e0, NetPlayerStatusChanged, NetPlayerStatusChanged_original)

void C2_HOOK_FASTCALL InitNetHeadups(void) {

    C2V(gDigits_pix) = DRLoadPixelmap("HDIGITS.PIX");
    if (C2V(gDigits_pix) != NULL) {
        BrMapAdd(C2V(gDigits_pix));
    }
}
C2_HOOK_FUNCTION(0x0049a710, InitNetHeadups)

void C2_HOOK_FASTCALL DisposeNetHeadups(void) {

    if (C2V(gDigits_pix) != NULL) {
        BrMapRemove(C2V(gDigits_pix));
        BrPixelmapFree(C2V(gDigits_pix));
    }
}
C2_HOOK_FUNCTION(0x0049a730, DisposeNetHeadups)
