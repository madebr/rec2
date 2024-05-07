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

C2_HOOK_VARIABLE_IMPLEMENT(br_uint_32, DriverTimestamp, 0x0079feb0);

br_device* (C2_HOOK_CDECL * BrDrv1Begin_original)(char* arguments);
br_device* C2_HOOK_CDECL BrDrv1Begin(char* arguments) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrDrv1Begin_original(arguments);
#else
    br_device *device;

    if (C2V(DriverTimestamp) == 0) {
        C2V(DriverTimestamp) = TIMESTAMP_START;
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
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00540590, BrDrv1Begin, BrDrv1Begin_original)
