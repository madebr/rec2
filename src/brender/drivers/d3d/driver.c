#include "backend.h"
#include "device.h"
#include "outfcty.h"
#include "plib.h"
#include "timestmp.h"

#include <brender/brender.h>

#include "c2_hooks.h"


br_device* C2_HOOK_CDECL BrDrv1Begin(const char* arguments) {
    int type_count;
    br_device_d3d* device = &C2V(DriverDeviceD3D);
    br_error r;

    C2V(PrimD3DDriverTimestamp) = TIMESTAMP_START;

    DetectD3DDevices();

    if (DeviceD3DInitialise(device) != 0) {
        return NULL;
    }

    r = PrimitiveLibraryD3DInitialise(&C2V(PrimitiveLibraryD3D), device);
    if (r != 0) {
        return NULL;
    }

    type_count = OutputFacilityD3DInitialise(device, &C2V(PrimitiveLibraryD3D));

    if (type_count == 0) {
        return NULL;
    }
    return (br_device*)C2V(device);
}
C2_HOOK_FUNCTION(0x10001320, BrDrv1Begin)
