#include "brbegin.h"

#include "brbhook.h"
#include "devlist.h"
#include "fwsetup.h"

#include "core/host/himage.h"
#include "core/host/hstsetup.h"
#include "core/pixelmap/pmsetup.h"


// GLOBAL: CARMA2_HW 0x006b09b4
br_boolean active;

// FUNCTION: CARMA2_HW 0x0052cf20
br_error C2_HOOK_CDECL BrBegin(void) {
    if (active) {
        return 4103;
    }
    BrFwBegin();
    HostBegin();
    BrPixelmapBegin();
    active = 1;

    _BrBeginHook();

    return 0;
}

// FUNCTION: CARMA2_HW 0x0052cf50
br_error C2_HOOK_CDECL BrEnd(void) {
    br_device* dev;

    if (!active) {
        return 4102;
    }
    _BrEndHook();
    active = 0;
    while (BrDevFind(&dev, NULL) == 0) {
        if (dev != NULL) {
            BrDevRemove(dev);
        }
    }
    BrPixelmapEnd();
    HostEnd();
    BrFwEnd();
    return 0;
}
