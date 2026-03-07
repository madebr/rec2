#include <brender/brender.h>

#include "core/fw/object.h"
#include "device.h"
#include "rendfcty.h"
#include "timestmp.h"

#include "c2_hooks.h"

#if BASED_FIXED
#define SHORT_SUFFIX "RNDX"
#define LONG_SUFFIX "-Fixed"
#endif
#if BASED_FLOAT
#define SHORT_SUFFIX "RNDF"
#define LONG_SUFFIX "-Float"
#endif


// GLOBAL: CARMA2_HW 0x0079feb0
br_uint_32 DriverTimestamp;

// FUNCTION: CARMA2_HW 0x00540590
br_device* C2_HOOK_CDECL BrDrv1Begin(const char* arguments) {
    br_device *device;

    if (DriverTimestamp == 0) {
        DriverTimestamp = TIMESTAMP_START;
    }

    device = DeviceSoftAllocate("SOFT" SHORT_SUFFIX);

    if (device == NULL) {
        return NULL;
    }

	if (RendererFacilitySoftAllocate(device, "Default-Renderer" LONG_SUFFIX) == NULL) {
        device->dispatch->_free((br_object*)device);
        return NULL;
    }

    return device;
}
