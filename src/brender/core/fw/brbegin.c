#include "brbegin.h"

#define HOOK_BRBEGIN 1

#if !HOOK_BRBEGIN
#include "brbhook.h"
#include "devlist.h"
#include "fwsetup.h"

#include "core/host/himage.h"
#include "core/host/hstsetup.h"
#include "core/pixelmap/pmsetup.h"
#endif

C2_HOOK_VARIABLE_IMPLEMENT(br_boolean, active, 0x006b09b4);

br_error (C2_HOOK_CDECL * BrBegin_original)(void);
br_error C2_HOOK_CDECL BrBegin(void) {
#if HOOK_BRBEGIN
    return BrBegin_original();
#else
    C2_HOOK_START();
    if (C2V(active)) {
        return 4103;
    }
    BrFwBegin();
    HostBegin();
    BrPixelmapBegin();
    C2V(active) = 1;

    _BrBeginHook();

    C2_HOOK_FINISH();
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052cf20, BrBegin, BrBegin_original)

br_error (C2_HOOK_CDECL * BrEnd_original)(void);
br_error C2_HOOK_CDECL BrEnd(void) {
#if HOOK_BRBEGIN
    return BrEnd_original();
#else
    br_device* dev;

    if (!C2V(active)) {
        return 4102;
    }
    _BrEndHook();
    C2V(active) = 0;
    while (BrDevFind(&dev, NULL) == 0) {
        if (dev != NULL) {
            BrDevRemove(dev);
        }
    }
    BrPixelmapEnd();
    HostEnd();
    BrFwEnd();
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052cf50, BrEnd, BrEnd_original)
